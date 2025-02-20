#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>



int main(int argc, char *argv[]) {
    if (argc != 3){
        perror("Error: el formato es: ./crear <fichero> <modo>\n"); //Argumentos incorrectos
        return -1;
    }

    char *endptr;
    errno = 0;
    long tmp1=strtol(argv[2], &endptr, 8); //Conversión de modo a octal
    if (errno!=0 || *endptr != '\0'){ //Comprobación de errores
        perror("Error: permisos erróneos\n");
        return -1;
    }

    umask(0); //Se establece la máscara de creación de archivos
    int fd;
    if ((fd = open(argv[1], O_CREAT | O_WRONLY, tmp1)) < 0) { //Se intenta crear el archivo
        perror("Error creando el archivo\n");
        return -1;
    }
    printf("Archivo %s creado con exito\n", argv[1]);
    close(fd);
    return 0; //Ejecución correcta
}

