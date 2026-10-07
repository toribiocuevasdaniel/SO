#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

extern pid_t pidPadreOriginal;
extern void despertar(int sig);
void manejador_alarma(int sig);

void codigo_ultimo_hijo(void) {
    signal(SIGALRM, manejador_alarma);
    alarm(1);
    pause(); // Despierta con la alarma y notifica al superpadre enviando SIGUSR1
}

void crearVertical(int col, int x, int y) {
    int fila = 0;
    pid_t pid_hijo_abajo = -1;

    // Creación de la cadena vertical
    for (int f = 1; f < x; f++) {
        pid_t pid = fork();

        if (pid > 0) {
            pid_hijo_abajo = pid; // Guarda el PID del hijo inferior
            break; 
        }
        fila = f; 
    }
    // Si es la esquina inferior derecha, activa la alarma para avisar al SuperPadre
    if (col == (y - 1) && fila == (x - 1)) {
        codigo_ultimo_hijo();
    }

    
    signal(SIGUSR1, despertar);
    pause(); // Esperan en pause() a que su padre superior les envíe SIGUSR1

    if (pid_hijo_abajo > 0) {
        kill(pid_hijo_abajo, SIGUSR1);
        wait(NULL); // Recogen al hijo inferior cuando muera
    }
    exit(0);
}