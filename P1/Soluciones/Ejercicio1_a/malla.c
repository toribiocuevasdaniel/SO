#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t pidPadreOriginal;

void configurar_senyales();
void manejador_padre(int sig);
void crearHorizontal(int x, int y);

void parse(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <filas (x)> <columnas (y)>\n", argv[0]);
        exit(1);
    }
    else if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0) {
        fprintf(stderr, "Introduzca números positivos para filas y columnas\n");
        exit(1);
    }
}

void llamadaPstree(pid_t pidpadreOriginal) {
    char pid_str[16];
    sprintf(pid_str, "%d", pidpadreOriginal);
    execlp("pstree", "pstree", "-c", pid_str, NULL);
    perror("Error en execlp");
    exit(1);
}

int main(int argc, char *argv[]) {
    parse(argc, argv);
    int filas = atoi(argv[1]);
    int columnas = atoi(argv[2]);
    pidPadreOriginal = getpid();

    configurar_senyales();

    
    crearHorizontal(filas, columnas);

    
    pause();

    
    pid_t pid_pstree = fork();
    if (pid_pstree == 0) {
        llamadaPstree(pidPadreOriginal);
    } 
    wait(NULL); //Espera a que termine su único hijo directo, pstree

    // Liberar a todos los hijos
    kill(0, SIGUSR1);

    for (int i = 0; i < columnas; i++) {
        wait(NULL);
    }

    return 0;
}