#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

// Función para imprimir la línea que contiene la cadena buscada
void imprimir_línea(char *buffer, ssize_t start, ssize_t bytes_read) {
    ssize_t j = start;
    // Buscar el final de la línea '\n
    while (j < bytes_read && buffer[j] != '\n') {
        j++;
    }
    char *palabra = malloc(j - start + 1);
    memcpy(palabra, buffer + start, j - start);
    palabra[j - start] = '\0'; 
    printf("%s\n", palabra);
    free(palabra);
}


int main(int argc, char **argv) {
    if (argc != 3) {
        perror("Usage: ./mygrep <ruta_fichero> <cadena_busqueda>\n");  
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int l_cadena = strlen(argv[2]);
    int found = 0;
    int line_number = 1;
    int match_index = 0;
    ssize_t line_start = 0;

    // Variable lógica para marcar si una linea ha sido impresa (1) o no (0)
    int line_printed = 0;

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {  // Leer el fichero de 1024 en 1024 y alacenarlo en buffer
        for (ssize_t i = 0; i < bytes_read; i++) {  // recorrer el buffer de carácter en carácter buscando la cadena
            if (buffer[i] == argv[2][match_index]) {
                // Si coincide, incrementar el índice de coincidencia (buscar la siguiente letra)
                match_index++;
                if (match_index == l_cadena) {
                    // Encontró la cadena completa, imprimir la línea
                    if (line_printed == 0) { // Salvo que ya se haya impreso
                        imprimir_línea(buffer, line_start, bytes_read);
                    }
                    line_printed = 1; // Marcar que se ha impreso una línea
                    match_index = 0; // Reiniciar el índice de coincidencia
                    found = 1; // Marcar que se encontró la cadena al menos una vez
                }
            } else {
                // Si no coincide, reiniciar el índice de coincidencia
                match_index = 0;
                // y si coincide con el primer carácter de la cadena, volver a atrás
                if (buffer[i] == argv[2][0]) {
                    i--;
                }
            }

            if (buffer[i] == '\n') {
                // Marcar que esta nueva línea ha sido impresa
                line_printed = 0;
                // Cambiar a la siguiente línea
                line_start = i + 1;
                line_number++;
            }
        }
    }

    if (bytes_read == -1) {
        perror("Error reading file");
        close(fd);
        return -1;
    }

    if (!found) {
        printf("%s not found.\n", argv[2]);
    }


    close(fd);
    return 0;
}
