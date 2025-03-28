#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr,"Usage: %s <ruta_fichero> <cadena_busqueda>\n", argv[0]);
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

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == argv[2][match_index]) {
                match_index++;
                if (match_index == l_cadena) {
                    // Encontró la cadena completa, imprimir la línea
                    ssize_t j = line_start;
                    while (j < bytes_read && buffer[j] != '\n') {
                        j++;
                    }
                    char *palabra = malloc(j - line_start + 1);
                    memcpy(palabra, buffer + line_start, j - line_start);
                    palabra[j - line_start] = '\0'; 
                    printf("Found: %s\n", palabra);
                    found = 1;
                    match_index = 0;
                    free(palabra);
                }
            } else {
                match_index = 0;
            }

            if (buffer[i] == '\n') {
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
