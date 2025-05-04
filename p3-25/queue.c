#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"

struct queue {
    struct element *first; // Puntero al primer elemento
    struct element *block_start; // Puntero al bloque original (añadir esta línea)
    int size;             // Tamaño máximo de la cola
    int count;            // Número actual de elementos
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
};

//To create a queue
struct queue* queue_init(int size, int belt) {
    struct queue *q = malloc(sizeof(struct queue));
    if (!q) return NULL;

    // Inicializar los elementos de la cola
    struct element *elements = malloc(size * sizeof(struct element));
    if (!elements) {
        free(q);
        return NULL;
    }

    // Crear la lista circular
    for (int i = 0; i < size; i++) {
        elements[i].num_edition = 0;
        elements[i].last = 0;
        elements[i].id_belt = belt;
        elements[i].next = &elements[(i + 1) % size];
    }

    q->block_start = elements;
    q->first = &elements[0];
    q->size = size;
    q->count = 0;

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);

    return q;
}


// To Enqueue an element
int queue_put(struct queue *q, struct element *ele) {
    pthread_mutex_lock(&q->mutex);

    while (q->count >= q->size) {
        pthread_cond_wait(&q->not_full, &q->mutex); // Esperar hasta que haya espacio y desbloquear
    }

    // Encontrar la posición para insertar
    struct element *current = q->first;
    for (int i = 0; i < q->count; i++) {
        current = current->next;
    }

    // Copiar los datos del elemento
    current->num_edition = ele->num_edition;
    current->id_belt = ele->id_belt;
    if (q->count == q->size - 1) {
        current->last = 1; // El último elemento
    } else {
        current->last = 0;
    }

    q->count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

// To extract elements from the queue
struct element* queue_get(struct queue *q) {
    pthread_mutex_lock(&q->mutex);

    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    // Obtener el elemento del frente
    struct element *item = malloc(sizeof(struct element));
    *item = *(q->first);

    // Mover el puntero first al siguiente
    q->first = q->first->next;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return item;
}


//To check queue state
int queue_empty(struct queue *q) {
    pthread_mutex_lock(&q->mutex);
    int empty = (q->count == 0);
    pthread_mutex_unlock(&q->mutex);
    return empty;
}

int queue_full(struct queue *q) {
    pthread_mutex_lock(&q->mutex);
    int full = (q->count >= q->size);
    pthread_mutex_unlock(&q->mutex);
    return full;
}

//To destroy the queue and free the resources
int queue_destroy(struct queue *q) {
    if (!q) return -1;

    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);

    // Liberar solo una vez el bloque original completo
    free(q->block_start);

    // Liberar la estructura queue
    free(q);
    return 0;
}