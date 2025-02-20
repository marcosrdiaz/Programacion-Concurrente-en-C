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
	printf("%d", argc);
	for (int i = 0; i < argc; i++){
		printf("%s\n", argv[i]);
	}
	return 0;
}
