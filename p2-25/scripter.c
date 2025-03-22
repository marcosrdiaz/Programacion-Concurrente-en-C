#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

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
    char *token = strtok(linea, delim);
    while (token != NULL && i < max_tokens - 1) {
        tokens[i++] = token;
        token = strtok(NULL, delim);
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
    //initialization for every command
    filev[0] = NULL;
    filev[1] = NULL;
    filev[2] = NULL;
    //Store the pointer to the filename if needed.
    //args[i] set to NULL once redirection is processed
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            filev[0] = args[i+1];
            args[i] = NULL;
            args[i + 1] = NULL;
        } else if (strcmp(args[i], ">") == 0) {
            filev[1] = args[i+1];
            args[i] = NULL;
            args[i + 1] = NULL;
        } else if (strcmp(args[i], "!>") == 0) {
            filev[2] = args[i+1];
            args[i] = NULL;
            args[i + 1] = NULL;
        }
    }
}

/**
 * This function processes the input command line and returns in global variables:
 * argvv -- command an args as argv
 * filev -- files for redirections. NULL value means no redirection.
 * background -- 0 means foreground; 1 background.
 */
int procesar_linea(char *linea) {
    char *comandos[max_commands];
    int num_comandos = tokenizar_linea(linea, "|", comandos, max_commands);
    printf("Num comandos = %d\n", num_comandos);
    //Check if background is indicated
    if (strchr(comandos[num_comandos - 1], '&')) {
        background = 1;
        char *pos = strchr(comandos[num_comandos - 1], '&');
        //remove character
        *pos = '\0';
    }

    //Finish processing
    for (int i = 0; i < num_comandos; i++) {
        int args_count = tokenizar_linea(comandos[i], " \t\n", argvv, max_args);
        procesar_redirecciones(argvv);

        /********* This piece of code prints the command, args, redirections and background. **********/
        /*********************** REMOVE BEFORE THE SUBMISSION *****************************************/
        /*********************** IMPLEMENT YOUR CODE FOR PROCESSES MANAGEMENT HERE ********************/
        printf("Comando = %s\n", argvv[0]);
        for(int arg = 1; arg < max_args; arg++)
            if(argvv[arg] != NULL)
                printf("Args = %s\n", argvv[arg]);

        printf("Background = %d\n", background);
        if(filev[0] != NULL)
            printf("Redir [IN] = %s\n", filev[0]);
        if(filev[1] != NULL)
            printf("Redir [OUT] = %s\n", filev[1]);
        if(filev[2] != NULL)
            printf("Redir [ERR] = %s\n", filev[2]);
        /**********************************************************************************************/
    }

    return num_comandos;
}

int main(int argc, char *argv[]) {
if (argc != 2) {
    write(STDERR_FILENO, "Error: Número de argumentos inválido\n", 37);
    return -1;
}

int fd = open(argv[1], O_RDONLY);
if (fd < 0) {
    perror("Error al abrir el fichero");
    return -2;
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
                perror("Error: Línea vacía detectada");
                close(fd);
                return -1;
            }

            if (linea_act == 0 && strcmp(linea, "## Script de SSOO") != 0) {
                perror("La primera línea no es '## Script de SSOO'");
                close(fd);
                return -1;
            }
            printf("\n------------%s------------\n\n", linea);

            if (linea_act >= 1) {
                char *comandos[max_commands];
                int num_comandos = tokenizar_linea(linea, "|", comandos, max_commands);
                int pipes[max_commands - 1][2];
                pid_t pids[max_commands];

                for (int j = 0; j < num_comandos; j++) {
                    tokenizar_linea(comandos[j], " \t\n", argvv, max_args);
                    procesar_redirecciones(argvv);

                    if (j < num_comandos - 1) {
                        if (pipe(pipes[j]) < 0) {
                            perror("Error al crear pipe");
                            exit(EXIT_FAILURE);
                        }
                    }

                    pids[j] = fork();
                    if (pids[j] == -1) {
                        perror("Error al crear el proceso hijo");
                        exit(EXIT_FAILURE);
                    }

                    if (pids[j] == 0) { // Proceso hijo
                        if (j > 0) {
                            dup2(pipes[j - 1][0], STDIN_FILENO);
                            close(pipes[j - 1][0]);
                            close(pipes[j - 1][1]);
                        }
                        if (j < num_comandos - 1) {
                            close(pipes[j][0]);
                            dup2(pipes[j][1], STDOUT_FILENO);
                            close(pipes[j][1]);
                        }
                        if (filev[0]) {
                            int fd_in = open(filev[0], O_RDONLY);
                            if (fd_in < 0) {
                                perror("Error al abrir archivo de entrada");
                                exit(1);
                            }
                            dup2(fd_in, STDIN_FILENO);
                            close(fd_in);
                        }
                        if (filev[1]) {
                            int fd_out = open(filev[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (fd_out < 0) {
                                perror("Error al abrir archivo de salida");
                                exit(1);
                            }
                            dup2(fd_out, STDOUT_FILENO);
                            close(fd_out);
                        }
                        if (filev[2]) {
                            int fd_err = open(filev[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (fd_err < 0) {
                                perror("Error al abrir archivo de error");
                                exit(1);
                            }
                            dup2(fd_err, STDERR_FILENO);
                            close(fd_err);
                        }
                        execvp(argvv[0], argvv);
                        perror("Error en execvp");
                        exit(EXIT_FAILURE);
                    }

                    if (j > 0) {
                        close(pipes[j - 1][0]);
                        close(pipes[j - 1][1]);
                    }
                }

                for (int j = 0; j < num_comandos; j++) {
                    if (!background) {
                        waitpid(pids[j], NULL, 0);
                    }
                }
            }
            linea_act++;
            start = i + 1;
        }
    }

    offset = bytes_read + offset - start;
    memmove(buffer, buffer + start, offset);
}

if (bytes_read < 0) {
    perror("Error al leer el fichero");
    close(fd);
    return -3;
}

printf("Salí bien?\n");
close(fd);
return 0;
}