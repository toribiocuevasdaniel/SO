#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t pidSuperPadre;
void codigo_A(int segundos);
void despertar(int sig){}
void parse(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <duración en s alarma>\n", argv[0]);
        exit(1);
    }
    else if (atoi(argv[1]) <= 0) {
        fprintf(stderr, "Introduzca solo números enteros y positivos \n");
        exit(1);
    }
}


int main(int argc, char *argv[]){
    printf("Soy el proceso ejec: mi pid es %d\n", getpid());
    pidSuperPadre = getpid();
    pid_t pid_A = fork();
    if(pid_A == 0){
        codigo_A(atoi(argv[1]));
    }
    else{
        signal(SIGUSR1, despertar);
        pause();
        kill(pid_A, SIGUSR1);
        wait(NULL);
        printf("Soy ejec (%d) y muero\n", getpid());
        exit(0);
    }
}