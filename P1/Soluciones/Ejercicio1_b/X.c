// X.c - Ejercicio 1b: código del proceso X (hijo de B).
// Proceso hoja: solo se identifica, espera la orden de terminación de B
// y muere.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Definidos en A.c y ejec.c
extern pid_t pid_A;
extern pid_t pidSuperPadre;
extern void despertar(int sig);

// Rutina del proceso X
void codigo_X(){
   // Muestra su PID y el de su padre, abuelo y bisabuelo
   printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), getppid(), pid_A, pidSuperPadre);
   signal(SIGUSR1, despertar);
   pause(); // Espera la señal de terminación enviada por B
   printf("Soy X (%d), y muero\n", getpid());
   exit(0);
}