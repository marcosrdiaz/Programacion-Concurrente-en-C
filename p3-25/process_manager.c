/*
 *
 * process_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"
#include "manager.h"
#include <semaphore.h>
#include <stdbool.h>

// Estructura para pasar información a los hilos
typedef struct {
    struct queue *belt;
    int id_belt;
    int num_products;
    bool *finished;
} thread_data_t;

// Función para el hilo productor
void *producer_function(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    struct queue *belt = data->belt;
    int id_belt = data->id_belt;
    int num_products = data->num_products;
    
    for (int i = 0; i < num_products; i++) {
        // Crear un nuevo elemento
        struct element elem;
        elem.num_edition = i + 1;
        elem.id_belt = id_belt;
        elem.last = (i == num_products - 1) ? 1 : 0;
        
        // Insertar en la cola
        if (queue_put(belt, &elem) < 0) {
            fprintf(stderr, "[ERROR][queue] There was an error while using queue with id: %d.\n", id_belt);
            pthread_exit(NULL);
        }
        
        printf("[OK][queue] Introduced element with id %d in belt %d.\n", elem.num_edition, elem.id_belt);
        
        // Pequeña espera para simular producción
        usleep(50000);
    }
    
    pthread_exit(NULL);
}

// Función para el hilo consumidor
void *consumer_function(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    struct queue *belt = data->belt;
    int id_belt = data->id_belt;
    int num_products = data->num_products;
    bool *finished = data->finished;
    
    for (int i = 0; i < num_products; i++) {
        // Obtener un elemento de la cola
        struct element *elem = queue_get(belt);
        if (!elem) {
            fprintf(stderr, "[ERROR][queue] There was an error while using queue with id: %d.\n", id_belt);
            pthread_exit(NULL);
        }
        
        printf("[OK][queue] Obtained element with id %d in belt %d.\n", elem->num_edition, elem->id_belt);
        
        // Si es el último elemento, marcar como finalizado
        if (elem->last == 1) {
            *finished = true;
        }
        
        free(elem);
        
        // Pequeña espera para simular consumo
        usleep(70000);
    }
    
    pthread_exit(NULL);
}

int process_manager(cinta_t params) {
    int id_belt = params.id;
    int belt_size = params.tam_cinta;
    int num_products = params.productos;
    
    // Verificar argumentos
    if (id_belt <= 0 || belt_size <= 0 || num_products <= 0) {
        fprintf(stderr, "[ERROR][process_manager] Arguments not valid.\n");
        return -1;
    }
    
    printf("[OK][process_manager] Process_manager with id %d waiting to produce %d elements.\n", 
           id_belt, num_products);
    
    // Crear la cinta (cola circular)
    struct queue *belt = queue_init(belt_size, id_belt);
    if (!belt) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_belt);
        return -1;
    }
    
    printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d elements.\n", 
           id_belt, belt_size);
    
    // Variable para controlar la finalización
    bool finished = false;
    
    // Datos para pasar a los hilos
    thread_data_t thread_data = {
        .belt = belt,
        .id_belt = id_belt,
        .num_products = num_products,
        .finished = &finished
    };
    
    // Crear los hilos productor y consumidor
    pthread_t producer, consumer;
    int rc;
    
    rc = pthread_create(&producer, NULL, producer_function, (void *)&thread_data);
    if (rc) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_belt);
        queue_destroy(belt);
        return -1;
    }
    
    rc = pthread_create(&consumer, NULL, consumer_function, (void *)&thread_data);
    if (rc) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_belt);
        pthread_cancel(producer);
        queue_destroy(belt);
        return -1;
    }
    
    // Esperar a que finalicen los hilos
    if (pthread_join(producer, NULL) != 0) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_belt);
        return -1;
    }
    
    if (pthread_join(consumer, NULL) != 0) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_belt);
        return -1;
    }
    
    // Liberar recursos
    if (queue_destroy(belt) < 0) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_belt);
        return -1;
    }
    
    printf("[OK][process_manager] Process_manager with id %d has produced %d elements.\n", 
           id_belt, num_products);
    
    return 0;
}
