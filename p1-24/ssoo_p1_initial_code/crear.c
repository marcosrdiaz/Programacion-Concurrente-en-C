#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>



int main(int argc, char *argv[]) {
    if (argc != 3){
        perror("Error: el formato es: ./crear <fichero> <modo>\n"); //Argumentos incorrectos
        return -1;
    }

    int fd;
    if ((fd = open(argv[1], O_CREAT | O_WRONLY, strtol(argv[2], NULL, 8)) < 0)){ //Se intenta crear el archivo
        perror("Error creando el archivo\n");
        close(fd); //Se cierra el archivo en caso de error
        return -1;
    }
    printf("Archivo %s creado con exito\n", argv[1]);
    close(fd);
    return 0; //Ejecución correcta
}


