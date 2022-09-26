#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int dado(int max) {
	printf("\nTirando dado...\n");
    return (rand() % max) + 1;
}

int bart_cansado() {
    return (rand() % 10) == 5;
}

int main(int argc, char const *argv[]) {

    srand(time(NULL) ^ (getpid() << 16));
    printf("Homero: Hola soy Homero, voy a jugar a los dados con Bart.\n");

    int pipe_bart[2];
    int pipe_homero[2];
    if (pipe(pipe_bart) == -1) {
        perror("Creacion de pipes");
        exit(1);
    }

    if (pipe(pipe_homero) == -1) {
        perror("Creacion de pipes");
        exit(1);
    }

    int pid = fork();
    if (pid == 0) {
        // Soy bart
        close(pipe_homero[1]);
        close(pipe_bart[0]);
        printf("Bart: Hola soy Bart, voy a tirar dados hasta cansarme y Homero los va a ir contando.\n");
        int tirada;
        int resultado;
        while (!bart_cansado()) {
            tirada = dado(6);
            printf("Bart: Mi dado fue %d y se lo voy a mandar a homero.\n", tirada);
            if (write(pipe_bart[1], &tirada, sizeof(int)) != sizeof(int)) {
                perror("Escritura");
                exit(1);
            }
            struct timespec rem, req = {0, 500000000};
    		nanosleep(&req, &rem);
        }

        printf("\nBart: Me cansé de jugar...\n");
        close(pipe_bart[1]);


        if (read(pipe_homero[0], &resultado, sizeof(int)) > 0)
            printf("Bart: Homero dice que mis dados sumaron %d.\n", resultado);

        exit(0);
    }

    // soy homero
    close(pipe_bart[1]);
    close(pipe_homero[0]);

    int data;
    int sum = 0;

    // leo hasta que bart se canse
    int i = 0;
    while (read(pipe_bart[0], &data, sizeof(int)) > 0) {
        i++;
        sum += data;
        printf("Homero: Bart me manda el dado %d, con este son %d y suman %d.\n", data, i, sum);
    }

    close(pipe_bart[0]);

    printf("Homero: Bart no quiere jugar más. Tiró %d dados en y estos sumaron %d en total.\n", i, sum);

    // le escribo a bart el resultado
    if (write(pipe_homero[1], &sum, sizeof(int)) != sizeof(int)) {
        perror("Escritura");
        exit(1);
    }

    wait(NULL);
    printf("Homero: ADIOS.\n");

    return 0;
}
