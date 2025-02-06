
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2){
        perror("Error: Invalid number of arguments\n");
        return -1;
    }
    int v[10];
    for (int i = 0; i < 10; i++) v[i]=atoi(argv[1]);

    int global=0;
    for (int i = 0; i < 10; i++) global += v[i];

    printf("Suma: %d\n", global);

    global=0;
    int i =0;
    
    while (i < 10){
        global += v[i];
        i++;
    }
    printf("Suma: %d\n", global);
    return 0;
}
