/*
 *
 * factory_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stddef.h>
#include <semaphore.h>
#include <string.h>
#include <sys/stat.h>
#include "manager.h"


#define MAX_BUFFER 1024

sem_t semaforo;

int buscar_index(cinta_t array[], int tamano, int elemento){
	for (int i = 0; i < tamano; i++){
		if (array[i].id == elemento){
			return -1;
		}
	}
	return 0;
}

void *control_thread(void *arg){
	cinta_t *cinta = (cinta_t *)arg;

	sem_wait(&semaforo);
	process_manager(*cinta);
	printf("[OK][factory_manager] Process_manager with id %d has finished.\n", cinta->id);
	pthread_exit(NULL);
}

int main(int argc, const char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Error: Número de argumentos inválido\n");
		return -1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		return -1;
	}

	char buffer[MAX_BUFFER];
	ssize_t bytes_read = read(fd, buffer, MAX_BUFFER - 1);
	if (bytes_read < 0) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		close(fd);
		return -1;
	}
	buffer[bytes_read] = '\0'; // Asegurar terminación en NULL

	// Reemplazar el primer '\n' por '\0' para limitar la lectura a la primera línea
	char *newline_pos = strchr(buffer, '\n');
	if (newline_pos != NULL) {
		*newline_pos = '\0';
	}

	char *token = strtok(buffer, " ");
	int max_producers = atoi(token); // Primer número: max_producers
	if (max_producers <= 0) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		close(fd);
		return -1;
	}

	int palabra = 0, index = 0, subindex = 1;
	cinta_t array[max_producers];
	while (token != NULL) {
		 if (palabra > 0) {
			if (index > max_producers - 1) {
				fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
				close(fd);
				return -1;
			}
			if (subindex == 1){
				if (buscar_index(array, index, atoi(token)) < 0){
					fprintf(stderr,"[ERROR][factory_manager] Invalid file.\n");
					close(fd);
					return -1;
				}
				if (atoi(token) <= 0){
					fprintf(stderr,"[ERROR][factory_manager] Invalid file.\n");
					close(fd);
					return -1;
				}
				array[index].id = atoi(token); // ID de la cinta
			}
			if (subindex == 2){
				if (atoi(token) <= 0){
					fprintf(stderr,"[ERROR][factory_manager] Invalid file.\n");
					close(fd);
					return -1;
				}
				array[index].tam_cinta = atoi(token);
			} // Tamaño de la cinta
			if (subindex == 3){
				if (atoi(token) <= 0){
					fprintf(stderr,"[ERROR][factory_manager] Invalid file.\n");
					close(fd);
					return -1;
				}
				array[index].productos = atoi(token); // Productos en la cinta
				subindex = 0; // Reiniciar subíndice para el siguiente grupo
				index++;
			}
		 	subindex++;
		}
		palabra++;
		token = strtok(NULL, " ");
	}

	if (subindex != 1){
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		close(fd);
		return -1;
	}
	if (close(fd) < 0) {
		fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
		return -1;
	}
	pthread_t threads[index];

	sem_init(&semaforo, 0, 0);
	// Imprimir resultados para verificar
	for (int i = 0; i < index; i++) {
		if (pthread_create(&threads[i], NULL, control_thread, &array[i]) != 0){
			fprintf(stderr, "[ERROR][factory_manager] creación del hilo para cinta %d\n", array[i].id);
			return -1;
		}
		printf("[OK][factory_manager] Process_manager with id %d has been created.\n", array[i].id);
	}

	for (int i = 0; i < index; i++){
		sem_post(&semaforo);
	}

	for (int i = 0; i < index; i++){
		pthread_join(threads[i], NULL);
	}

	sem_destroy(&semaforo);
	printf("[OK][factory_manager] Finishing.\n");
	return 0;
}
