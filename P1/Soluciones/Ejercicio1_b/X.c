#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
extern pid_t pid_A;
extern pid_t pidSuperPadre;
extern void despertar(int sig);
void codigo_X(){
   printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n", getpid(), getppid(), pid_A, pidSuperPadre);
   signal(SIGUSR1, despertar);
   pause();
   printf("Soy X (%d), y muero\n", getpid());
   exit(0);
}