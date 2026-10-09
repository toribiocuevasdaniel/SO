// Z.c - Ejercicio 1b: código del proceso Z (hijo de B).
// Es el desencadenante del final del programa: tras los segundos indicados
// por el argumento de ejec (mediante alarm(), sin usar sleep) envía SIGUSR1
// a A para que muestre pstree. Después espera la orden de terminación de B.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Definidos en A.c y ejec.c
extern pid_t pid_A;
extern pid_t pidSuperPadre;
extern void despertar(int sig);

// Rutina del proceso Z
void codigo_Z(int segundos){
    printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), getppid(), pid_A, pidSuperPadre);

    // Programa la alarma con los segundos recibidos como argumento.
    // Al dispararse, pause() se interrumpe y el manejador (despertar) vuelve
    signal(SIGALRM, despertar);
    alarm(segundos);
    pause(); 

    //Avisa a A de que toca mostrar pstree y propagar la terminación
    kill(pid_A, SIGUSR1);

    //Espera la orden de terminación que B envía de vuelta
    signal(SIGUSR1, despertar);
    pause();
    printf("Soy Z (%d) y muero\n", getpid());
    exit(0);

}