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
} datos_t;

// Función para el hilo productor
void *productor(void *arg) {
    datos_t *data = arg;
    struct queue *belt = data->belt;
    int id_belt = data->id_belt;
    int num_products = data->num_products;
    
    for (int i = 0; i < num_products; i++) {
        // Se crea un elemento por cada producto
        struct element elem;
        elem.num_edition = i;
        elem.id_belt = id_belt;
        elem.last = (i == num_products - 1) ? 1 : 0;

        // Se inserta en la cola
        if (queue_put(belt, &elem) < 0) {
            fprintf(stderr, "[ERROR][queue] There was an error while using queue with id: %d.\n", id_belt);
            pthread_exit(NULL);
        }

        printf("[OK][queue] Introduced element with id %d in belt %d.\n", elem.num_edition, elem.id_belt);

    }

    pthread_exit(NULL);
}

// Función para el hilo consumidor
void *consumidor(void *arg) {
    datos_t *data = arg;
    struct queue *belt = data->belt;
    int id_belt = data->id_belt;
    int num_products = data->num_products;
    bool *finished = data->finished;

    for (int i = 0; i < num_products; i++) {
        // Se recoge el elemento de la cola
        struct element *elem = queue_get(belt);
        if (!elem) {
            fprintf(stderr, "[ERROR][queue] There was an error while using queue with id: %d.\n", id_belt);
            pthread_exit(NULL);
        }

        printf("[OK][queue] Obtained element with id %d in belt %d.\n", elem->num_edition, elem->id_belt);

        // Se marca la flag last cuando se ha consumido el último producto
        if (elem->last == 1) {
            *finished = true;
        }

        free(elem);

    }

    pthread_exit(NULL);
}

int process_manager(cinta_t params) {
    // cinta_t params tiene como datos el id de la cinta, tam_cinta (tamaño) y los productos

    printf("[OK][process_manager] Process_manager with id %d waiting to produce %d elements.\n",
           params.id, params.productos);

    // Se crea la cinta (cola circular)
    struct queue *belt = queue_init(params.tam_cinta, params.id);
    if (!belt) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", params.id);
        return -1;
    }

    printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d elements.\n",
           params.id, params.tam_cinta);

    // Se usa una variable para controlar la finalización
    bool finished = false;

    // Datos para pasar a los hilos
    datos_t thread_data = {
        .belt = belt,
        .id_belt = params.id,
        .num_products = params.productos,
        .finished = &finished
    };

    // Se crean los hilos productor y consumidor
    pthread_t producer, consumer;
    int rc;

    // Productor
    rc = pthread_create(&producer, NULL, productor, &thread_data);
    if (rc) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", params.id);
        queue_destroy(belt);
        return -1;
    }

    // Consumidor
    rc = pthread_create(&consumer, NULL, consumidor, &thread_data);
    if (rc) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", params.id);
        pthread_cancel(producer);
        queue_destroy(belt);
        return -1;
    }

    // Se espera hasta que finalicen los hilos
    if (pthread_join(producer, NULL) != 0) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", params.id);
        return -1;
    }

    if (pthread_join(consumer, NULL) != 0) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", params.id);
        return -1;
    }

    // Se liberan los recursos
    if (queue_destroy(belt) < 0) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", params.id);
        return -1;
    }
    
    printf("[OK][process_manager] Process_manager with id %d has produced %d elements.\n",
           params.id, params.productos);

    return 0;
}
