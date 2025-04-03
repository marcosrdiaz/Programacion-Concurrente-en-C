#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

/* CONST VARS */
const int max_line = 1024;
const int max_commands = 10;
#define max_redirections 3 //stdin, stdout, stderr
#define max_args 15

/* VARS TO BE USED FOR THE STUDENTS */
char *argvv[max_args];
char *filev[max_redirections];
int background = 0;

/**
 * This function splits a char* line into different tokens based on a given character
 * @return Number of tokens
 */
int tokenizar_linea(char *linea, char *delim, char *tokens[], int max_tokens) {
    int i = 0;
    char *ptr = linea;

    while (*ptr != '\0' && i < max_tokens - 1) {
        // Saltar caracteres delimitadores iniciales
        while (strchr(delim, *ptr) && *ptr != '\0') ptr++;

        if (*ptr == '\0') break;

        // Manejo de comillas dobles
        if (*ptr == '"') {
            ptr++; // Saltar comilla inicial
            tokens[i++] = ptr;
            while (*ptr && *ptr != '"') ptr++; // Buscar comilla de cierre
            if (*ptr == '"') *ptr++ = '\0'; // Reemplazar comilla de cierre con NULL
        }
        // Manejo de comillas simples
        else if (*ptr == '\'') {
            ptr++; // Saltar comilla inicial
            tokens[i++] = ptr;
            while (*ptr && *ptr != '\'') ptr++;
            if (*ptr == '\'') *ptr++ = '\0'; // Reemplazar comilla de cierre con NULL
        }
        // Manejo de tokens normales
        else {
            tokens[i++] = ptr;
            while (*ptr && !strchr(delim, *ptr)) ptr++; // Buscar siguiente delimitador
            if (*ptr) *ptr++ = '\0'; // Reemplazar delimitador con NULL
        }
    }
    tokens[i] = NULL;
    return i;
}


/**
 * This function processes the command line to evaluate if there are redirections.
 * If any redirection is detected, the destination file is indicated in filev[i] array.
 * filev[0] for STDIN
 * filev[1] for STDOUT
 * filev[2] for STDERR
 */
void procesar_redirecciones(char *args[]) {
    // Inicializar valores a NULL
    filev[0] = NULL;
    filev[1] = NULL;
    filev[2] = NULL;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0 && args[i + 1] != NULL) {
            filev[0] = args[i + 1];
        } else if (strcmp(args[i], ">") == 0 && args[i + 1] != NULL) {
            filev[1] = args[i + 1];
        } else if (strcmp(args[i], "!>") == 0 && args[i + 1] != NULL) {
            filev[2] = args[i + 1];
        }
    }

    // Segunda pasada para limpiar los argumentos después de procesar redirecciones
    int j = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if ((strcmp(args[i], "<") == 0 ||
            strcmp(args[i], ">") == 0 ||
            strcmp(args[i], "!>") == 0) && args[i + 1] != NULL) {
            i++; // Saltar el nombre del archivo
        } else {
            args[j++] = args[i];
        }
    }
    args[j] = NULL;
}


/**
 * This function processes the input command line and returns in global variables:
 * argvv -- command an args as argv
 * filev -- files for redirections. NULL value means no redirection.
 * background -- 0 means foreground; 1 background.
 */
int procesar_linea(char *linea, char *comandos[], char *argvv[][max_args]) {
    int num_comandos = tokenizar_linea(linea, "|", comandos, max_commands);
    //Check if background is indicated
    if (strchr(comandos[num_comandos - 1], '&')) {
        background = 1;
        char *pos = strchr(comandos[num_comandos - 1], '&');
        //remove character
        *pos = '\0';
    }

    //Finish processing
    for (int i = 0; i < num_comandos; i++) {
        int args_count = tokenizar_linea(comandos[i], " \t\n", argvv[i], max_args);
        //fprintf(stderr, "args_count = %d\n", args_count);
        //fprintf(stderr, "num_comandos: %d\n", num_comandos);
        //fprintf(stderr, "comandos[i] = %s\n", comandos[i]);
        //fprintf(stderr, "comando 0 = %s\n", comandos[0]);
        //fprintf(stderr, "argvv[%d] = %s\n", i, *argvv[i]);
        procesar_redirecciones(argvv[i]);
    }

    return num_comandos;

}

int ejecutar_comandos(char *linea) {
    char *comandos[max_commands];
    char *argvv[max_commands][max_args];
    //char temp_linea[max_line];
    //strcpy(temp_linea, linea);
    int num_comandos = procesar_linea(linea, comandos, argvv);

    int pipes[max_commands - 1][2];     // Array para almacenar descriptores de archivo de pipes
    pid_t pids[max_commands];           // Array para almacenar IDs de procesos

    for (int j = 0; j < num_comandos; j++) {
        if (j < num_comandos - 1) {
            // Se crea una pipe para cada comando excepto el último
            if (pipe(pipes[j]) < 0) {
                perror("Error al crear pipe");
                return -1;
            }
        }

        pids[j] = fork(); // Se crea un nuevo proceso para cada comando
        switch (pids[j]){
            case -1:
                perror("Error al crear el proceso hijo");
                return -1;
            case 0:
                // Proceso hijo
                if (j > 0) {
                    // Redirigir la entrada estándar al extremo de lectura de la pipe anterior
                    if (dup2(pipes[j - 1][0], STDIN_FILENO) < 0) {
                        perror("Error al redirigir STDIN");
                        close(pipes[j - 1][0]);
                        close(pipes[j - 1][1]);
                        return -1;
                    }
                    close(pipes[j - 1][0]);
                    close(pipes[j - 1][1]);
                }
                if (j < num_comandos - 1) {
                    // Redirigir la salida estándar al extremo de escritura de la pipe actual
                    if (dup2(pipes[j][1], STDOUT_FILENO) < 0) {
                        perror("Error al redirigir STDOUT");
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                        return -1;
                    }
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                if (filev[0] && j == 0) {
                    int fd_in = open(filev[0], O_RDONLY);
                    if (fd_in < 0) {
                        perror("Error al abrir archivo de entrada");
                        exit(0);
                    }
                    if (dup2(fd_in, STDIN_FILENO) < 0) {
                        perror("Error al redirigir STDIN");
                        close(fd_in);
                        return -1;
                    }
                    close(fd_in);
                }
                if (filev[1] && j == num_comandos - 1) {
                    int fd_out = open(filev[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd_out < 0) {
                        perror("Error al abrir archivo de salida");
                        exit(0);
                    }
                    if (dup2(fd_out, STDOUT_FILENO) < 0) {
                        perror("Error al redirigir STDOUT");
                        close(fd_out);
                        return -1;
                    }
                    close(fd_out);
                }
                if (filev[2] && j == num_comandos - 1) {
                    int fd_err = open(filev[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd_err < 0) {
                        perror("Error al abrir archivo de error");
                        exit(0);
                    }
                    if (dup2(fd_err, STDERR_FILENO) < 0) {
                        perror("Error al redirigir STDERR");
                        close(fd_err);
                        return -1;
                    }
                    close(fd_err);
                }

                execvp(argvv[j][0], argvv[j]); // Ejecutar el comando
                perror("Error en execvp");
                exit(1);
            default:
                if (j > 0) {
                    close(pipes[j - 1][0]);
                    close(pipes[j - 1][1]);
                }
                if (background == 0) {
                    waitpid(pids[j], NULL, 0);
                } else {
                    if (j == num_comandos - 1){
                        printf("[%d]\n", pids[j]);
                        fflush(stdout);
                    }
                }
        }
    }
    // Gestionar la finalización de procesos hijos en background
    if (background == 1) {
        for (int j = 0; j < num_comandos; j++) {
            waitpid(pids[j], NULL, WNOHANG);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("Error: Número de argumentos inválido\n");
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error al abrir el fichero");
        return -1;
    }

    int linea_act = 0;
    ssize_t bytes_read;
    char buffer[max_line];
    char linea[max_line];
    int offset = 0;

    while ((bytes_read = read(fd, buffer + offset, max_line - offset - 1)) > 0) {
        int start = 0;
        buffer[bytes_read + offset] = '\0'; // Asegurar terminación en NULL

        for (int i = 0; i < bytes_read + offset; i++) {
            if (buffer[i] == '\n' || buffer[i] == '\0') {
                int len = i - start;
                if (len >= max_line) len = max_line - 1;
                strncpy(linea, buffer + start, len);
                linea[len] = '\0';

                if (linea[0] == '\0' || strspn(linea, " \t") == strlen(linea)) {
                    perror("ERROR: Línea vacía detectada");
                    close(fd);
                    return -1;
                }

                if (linea_act == 0 && strcmp(linea, "## Script de SSOO") != 0) {
                    perror("ERROR: La primera línea no es '## Script de SSOO'");
                    close(fd);
                    return -1;
                }
                //printf("\n------------%s------------\n\n", linea);

                if (linea_act >= 1) {
                    if (ejecutar_comandos(linea) < 0){
                        close (fd);
                        return -1;
                    }
                }
                linea_act++;
                start = i + 1;
                background = 0;
            }
        }

        offset = bytes_read + offset - start;
        memmove(buffer, buffer + start, offset);
    }

    // Procesar la última línea si no termina con un salto de línea
    if (offset > 0) {
        buffer[offset] = '\0';
        strncpy(linea, buffer, offset);
        linea[offset] = '\0';
        if (linea[0] != '\0' && strspn(linea, " \t") != strlen(linea)) {
            if ( ejecutar_comandos(linea) < 0){
                close(fd);
                return -1;
            }
        }
    }
    if (bytes_read < 0) {
        perror("Error al leer el fichero");
        close(fd);
        return -1;
    }

   // printf("Salí bien?\n");
    close(fd);
    return 0;
}

