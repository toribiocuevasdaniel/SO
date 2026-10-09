// pfinal.c - Ejercicio 1a: manejadores de señales compartidos por todo el
// programa de la malla (se enlazan junto a malla.c, horizontal.c y vertical.c).

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// PID del proceso principal; definido en malla.c
extern pid_t pidPadreOriginal;

// Función vacía para despertar los procesos tras pause() sin matarlos
void despertar(int sig) {
}

// Manejador de SIGALRM: solo avisa al superpadre de que la malla está
// completa enviándole SIGUSR1 (programado en codigo_ultimo_hijo)
void manejador_alarma(int sig) {
    kill(pidPadreOriginal, SIGUSR1);
}

// Programa el manejador de la señal de usuario empleada como "despertador"
// para los pause() de terminación. Debe llamarse tras cada fork(), ya que
// los manejadores instalados con signal() se restablecen a SIG_DFL en el hijo
void configurar_senyales(void) {
    signal(SIGUSR1, despertar);
}