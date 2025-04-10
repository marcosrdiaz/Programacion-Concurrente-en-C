/*
 *
 * factory_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <string.h>
#include <sys/stat.h>


#define MAX_BUFFER 1024

int main(int argc, const char *argv[]) {
	if (argc != 2) {
		perror("Error: Número de argumentos inválido\n");
		return -1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("[ERROR][factory_manager] Invalid file.\n");
		return -1;
	}

	char buffer[MAX_BUFFER];
	ssize_t bytes_read = read(fd, buffer, MAX_BUFFER - 1);
	if (bytes_read < 0) {
		perror("[ERROR][factory_manager] Invalid file.\n");
		close(fd);
		return -1;
	}
	buffer[bytes_read] = '\0'; // Asegurar terminación en NULL

	// Procesar la primera línea
	int max_producers = 0;
	int array[100][3]; // Array para almacenar los grupos de 3 números
	int palabra = 0, index = 0, subindex = 0;

	// Reemplazar el primer '\n' por '\0' para limitar la lectura a la primera línea
	char *newline_pos = strchr(buffer, '\n');
	if (newline_pos != NULL) {
		*newline_pos = '\0';
	}

	char *token = strtok(buffer, " ");
	while (token != NULL) {
		printf("%s\n", token); // Imprimir cada token para depuración
		if (palabra == 0) {
			max_producers = atoi(token); // Primer número: max_producers
		} else {
			array[index][subindex] = atoi(token); // Guardar el número en el array
			subindex++;
			if (subindex == 3) { // Pasar al siguiente grupo
				subindex = 0;
				index++;
			}
			if (index > max_producers) {
				fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
				close(fd);
				return -1;
			}
		}
		palabra++;
		token = strtok(NULL, " ");
	}

	if (subindex != 0){
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		close(fd);
		return -1;
	}
	if (close(fd) < 0) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		return -1;
	}

	// Imprimir resultados para verificar
	printf("Max Producers: %d\n", max_producers);
	for (int i = 0; i < index; i++) {
		printf("Grupo %d: %d %d %d\n", i + 1, array[i][0], array[i][1], array[i][2]);
	}

	return 0;
}
