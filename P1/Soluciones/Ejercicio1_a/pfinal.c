#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

extern pid_t pidPadreOriginal;

void manejador_padre(int sig) {// Función vacía para despertar al super padre sin matarlo
    
}


void manejador_alarma(int sig) {
    kill(pidPadreOriginal, SIGUSR1);
}
void configurar_senyales(){
    signal(SIGUSR1, manejador_padre);
}