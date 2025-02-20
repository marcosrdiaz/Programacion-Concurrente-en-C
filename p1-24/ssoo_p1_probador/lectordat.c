#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

struct alumno{
	char nombre[50];
	int nota;
	int convocatoria;
};

int main(int argc, char *argv[]){
	
	char *dir1 = argv[1];

	// printf("Los ficheros son: %s, %s, %s\n", dir1, dir2, dir3);
	int fdin1 = open(dir1, O_RDONLY, 0755);

	if (fdin1 < 0 )  {
		perror("Error al abrir los ficheros, revisa los parámetros pasados\n");
		return -1;
	}
	// Si se han abierto bien los ficheros, empezamos a clonar:
	struct alumno buf;						// Buffer para lectura de alumnos
	int i = 0;  							// Cuenta de alumnos (verificando que no supere 100)
	int size = sizeof(struct alumno); // 50 + 4 + 4

	// Lectura del primer fichero
	while( read(fdin1, &buf, size) > 0){
		printf("Alumno[%d]:\n", i);
		printf("%s\n", buf.nombre);
        printf("%d\n", buf.nota);
        printf("%d\n", buf.convocatoria); 
		printf("----------------------------------\n");
        i++;
	}

	close(fdin1);
	return 0;
}

/*
	LLegar a la carpeta de los .dat:

	../../ssoo_p1_probador/f1.dat


*/
