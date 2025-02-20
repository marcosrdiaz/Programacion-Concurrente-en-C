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

// Para poder usar qsort hace falta hacerle saber el criterio de ordenación
int compararAlumnos(const void *a, const void *b) {
    return ((struct alumno *)a)->nota - ((struct alumno *)b)->nota;
}

int main(int argc, char *argv[]){
	// Comprobación de los argumentos
	if (argc != 4) {
		perror("Error, revisa los parámetros\n");
		return -1;
	}
	char *dir1 = argv[1], *dir2 = argv[2], *dir3 = argv[3];

	// printf("Los ficheros son: %s, %s, %s\n", dir1, dir2, dir3);
	int fdin1 = open(dir1, O_RDONLY, 0755);
	int fdin2 = open(dir2, O_RDONLY, 0755);
	int fdout = open(dir3, O_WRONLY | O_CREAT | O_TRUNC, 0777);

	if (fdin1 < 0 || fdin2 < 0 || fdout < 0)  {
		perror("Error al abrir los ficheros, revisa los parámetros pasados\n");
		return -1;
	}
	// Si se han abierto bien los ficheros, empezamos a clonar:
	struct alumno buf;			// Buffer para lectura de alumnos
	struct alumno alumnos[100]; // Array con todos los alumnos
	int i = 0;  				// Cuenta de alumnos (verificando que no supere 100)
	int size = sizeof(struct alumno); // 50 + 4 + 4

	// Lectura del primer fichero
	while( read(fdin1, &buf, size) > 0 && (i < 100) ){
		/* printf("Alumno[%d]:\n", i);
		printf("%s\n", buf.nombre);
        printf("%d\n", buf.nota);
        printf("%d\n", buf.convocatoria); 
		printf("----------------------------------\n"); */
		alumnos[i] = buf;
		i++;
	}

	// Lectura del seegundo archivo
	while( read(fdin2, &buf, size) > 0 && (i < 100) ){
		alumnos[i] = buf;
		i++;
	}
	const int n_alumn = i; // Se guarda el numero de alumnos en una constante

	if (n_alumn > 100) {
		perror("Error, hay más de 100 alumnos\n");
		return -1;
	}

	// Ordenar los alumnos por nota de menor a mayor
	qsort(alumnos, n_alumn, size, compararAlumnos);

    // Escribir los alumnos ordenados en el archivo de salida
    for (int i = 0; i < n_alumn; i++) {
        write(fdout, &alumnos[i], size);
    }

	// Abrimos el fichero donde guardar las estadísticas
	int fdcsv = open("estadisticas.csv", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fdcsv < 0) {
		perror("Error al abrir estadisticas.csv\n");
		return -1;
	}

	// Cálculo las estadísticas de las notas:
	int M = 0, S = 0, N = 0, A = 0, F = 0;
		// Recuento de las notas
		for (int i = 0; i < n_alumn; i++) {
			if (alumnos[i].nota == 10) M++;
			else if (alumnos[i].nota == 9) S++;
			else if (alumnos[i].nota >= 7) N++;
			else if (alumnos[i].nota >= 5) A++;
			else F++;
		}

		// Calculo de los porcentajes
		float 	m_per = (M * 100.0) / n_alumn, 
				s_per = (S * 100.0) / n_alumn, 
				n_per = (N * 100.0) / n_alumn, 
				a_per = (A * 100.0) / n_alumn, 
				f_per = (F * 100.0) / n_alumn;

	// write(fdcsv, "M;%d;%.2f%%\n", M, m_per); no funcionaria, para evitar el uso de varios writes, montar la cadena a parte:
	char buffer[100];
	int len;

	len = sprintf(buffer, "M;%d;%.2f%%\n", M, m_per);	// creación de la cadena
	write(fdcsv, buffer, len);							// Escritura en el fichero csv
	printf("%s", buffer);								// Impresión por pantalla

	len = sprintf(buffer, "S;%d;%.2f%%\n", S, s_per);
	write(fdcsv, buffer, len);
	printf("%s", buffer);

	len = sprintf(buffer, "N;%d;%.2f%%\n", N, n_per);
	write(fdcsv, buffer, len);
	printf("%s", buffer);

	len = sprintf(buffer, "A;%d;%.2f%%\n", A, a_per);
	write(fdcsv, buffer, len);
	printf("%s", buffer);

	len = sprintf(buffer, "F;%d;%.2f%%\n", F, f_per);
	write(fdcsv, buffer, len);
	printf("%s", buffer);

	// Cierre de los descriptores
	close(fdin1);
	close(fdin2);
	close(fdout);
	close(fdcsv);
	return 0;
}

/*
	LLegar a la carpeta de los .dat:

	../../ssoo_p1_probador/f1.dat


*/
