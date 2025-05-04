#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "queue.h"

struct queue *q;
struct element e1 = {1, 1, 0, NULL};
struct element e2 = {2, 1, 0, NULL};
struct element e3 = {3, 1, 0, NULL};
struct element e4 = {4, 1, 0, NULL};
struct element e5 = {5, 1, 0, NULL};
struct element e6 = {6, 1, 0, NULL};
struct element e7 = {7, 1, 0, NULL};
struct element e8 = {8, 1, 0, NULL};

void f1(struct queue *q) {
    printf("Function f1 executed\n");
    if (queue_put(q, &e1) == -1) {
        printf("Error adding element to queue\n");
    }
    if (queue_put(q, &e2) == -1) {
        printf("Error adding element to queue\n");
    }

    if (queue_put(q, &e3) == -1) {
        printf("Error adding element to queue\n");
    }
    if (queue_put(q, &e4) == -1) {
        printf("Error adding element to queue\n");
    }
}

void f2(struct queue *q) {
    printf("Function f2 executed\n");
    if (queue_put(q, &e5) == -1) {
        printf("Error adding element to queue\n");
    }
    if (queue_put(q, &e6) == -1) {
        printf("Error adding element to queue\n");
    }
    if (queue_put(q, &e7) == -1) {
        printf("Error adding element to queue\n");
    }
    if (queue_put(q, &e8) == -1) {
        printf("Error adding element to queue\n");
    }
}

int main() {
    q = queue_init(8, 1);
    if (!q) {
        printf("Error initializing queue\n");
        return -1;
    }
    printf("Queue initialized successfully\n");

    // Execute 2 threads
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, (void *)f1, (void *)q);
    pthread_create(&thread2, NULL, (void *)f2, (void *)q);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    printf("Threads executed successfully\n");

    if (!queue_empty(q)) {
        printf("Queue is not empty\n");
    } else {
        printf("Queue is empty\n");
    }

    // Extraer y mostrar todos los elementos
    struct element *deq_elem;
    while (!queue_empty(q)) {
        deq_elem = queue_get(q);
        printf("Dequeued element: %d\n", deq_elem->num_edition);
        free(deq_elem);  // Liberar memoria después de usar
    }

    queue_destroy(q);
    printf("Queue destroyed successfully\n");

    return 0;
}