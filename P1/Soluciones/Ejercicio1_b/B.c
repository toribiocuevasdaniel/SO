#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
extern pid_t pidSuperPadre;
extern void despertar(int sig);
void codigo_X(void);
void codigo_Y(void);
void codigo_Z(int segundos);
void codigo_B(int segundos){
    pid_t pids[3];
    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n", getpid(), getppid(), pidSuperPadre);
    for(int i = 0; i < 3; i++){
        pid_t pid = fork();

        if(pid == 0){
            switch(i){
                case 0:
                    codigo_X();
                    break;
                case 1:
                    codigo_Y();
                    break;
                case 2:
                    codigo_Z(segundos);
                    break;
            }
        }
        else if(pid > 0){
            pids[i] = pid;
        }
    }
    signal(SIGUSR1, despertar);
    pause();
    for(int i = 0; i < 3; i++){
        kill(pids[i],SIGUSR1);
    }
    for(int i = 0; i < 3; i++){
        wait(NULL);
    }
    printf("Soy B (%d), y muero\n", getpid());
    exit(0);
}