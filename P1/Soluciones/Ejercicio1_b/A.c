#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
pid_t pid_A;
extern pid_t pidSuperPadre;
extern void despertar(int sig);
void codigo_B(int segundos);
void codigo_A(int segundos);
void llamadaPstree(pid_t pidSuperPadre) {

    if(fork() == 0){
        char pid_str[16];
        sprintf(pid_str, "%d", pidSuperPadre);
        execlp("pstree", "pstree", "-c", pid_str, NULL);
        perror("Error en execlp");
        exit(1);
    }
}
void codigo_A(int segundos){
    pid_A = getpid();
    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", getpid(),getppid());
    pid_t pid_B = fork();
    if(pid_B == 0){
        codigo_B(segundos);
    }
    else{
        signal(SIGUSR1, despertar);
        pause();
        llamadaPstree(pidSuperPadre);
        wait(NULL);
        kill(pidSuperPadre, SIGUSR1);
        pause();
        kill(pid_B, SIGUSR1);
        wait(NULL);
        printf("Soy A (%d) y muero\n", getpid());
        exit(0);
    }
}