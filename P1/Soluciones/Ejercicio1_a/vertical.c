#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


void manejador_alarma(int sig);

void crearVertical(int col, int x, int y) {
    int fila = 0;

    // Creación de la cadena vertical
    for (int f = 1; f < x; f++) {
        pid_t pid_cadena = fork();
        if (pid_cadena > 0) {
            break; 
        }
        fila = f; 
    }

    if (col == (y - 1) && fila == (x - 1)) {
        signal(SIGALRM, manejador_alarma);
        alarm(1);
        pause(); // Despierta con la alarma y notifica al superpadre
        pause(); // Espera la señal SIGUSR1 del superpadre tras el pstree
    } else {
        pause(); 
    }

    wait(NULL); // Limpieza de hijos en la cadena
}