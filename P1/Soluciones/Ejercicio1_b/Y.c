
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Definidos en A.c y ejec.c
extern pid_t pid_A;
extern pid_t pidSuperPadre;
extern void despertar(int sig);

// Rutina del proceso Y
void codigo_Y(){
    // Muestra su PID y el de su padre, abuelo y bisabuelo
    printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), getppid(), pid_A, pidSuperPadre);
    signal(SIGUSR1, despertar);
    pause(); // Espera la señal de terminación enviada por B
    printf("Soy Y (%d) y muero\n", getpid());
    exit(0);
}