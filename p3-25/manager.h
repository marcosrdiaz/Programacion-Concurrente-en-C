
#ifndef MANAGER_H
#define MANAGER_H

typedef struct {
	int id;
	int tam_cinta;
	int productos;
}cinta_t;

int buscar_cinta(cinta_t array[], int tamano, int elemento);

void *PrintHello(void *threadid);
int process_manager(cinta_t array);
#endif //MANAGER_H
