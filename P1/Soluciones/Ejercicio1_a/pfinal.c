#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

extern pid_t pidPadreOriginal;

// Función vacía para despertar los procesos tras pause() sin matarlos
void despertar(int sig) {
}

void manejador_alarma(int sig) {
    kill(pidPadreOriginal, SIGUSR1);
}

void configurar_senyales(void) {
    signal(SIGUSR1, despertar);
}