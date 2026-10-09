// A.c - Ejercicio 1b: código del proceso A (hijo de ejec, padre de B).
// Cuando Z envía SIGALRM (tras los segundos indicados), A muestra el árbol
// con pstree, avisa al superpadre ejec y, a la señal de este, ordena la
// terminación de B y espera a que el subárbol muera antes de morir él.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// PID de A; lo comparten X.c, Y.c y Z.c con 'extern'
pid_t pid_A;

// Definidos en ejec.c y B.c
extern pid_t pidSuperPadre;
extern void despertar(int sig);
void codigo_B(int segundos);
void codigo_A(int segundos);

// Lanza 'pstree -c <pid>' en un proceso hijo para mostrar el árbol completo
void llamadaPstree(pid_t pidSuperPadre) {

    if(fork() == 0){
        char pid_str[16];
        sprintf(pid_str, "%d", pidSuperPadre);
        execlp("pstree", "pstree", "-c", pid_str, NULL);
        perror("Error en execlp");
        exit(1);
    }
}

// Rutina del proceso A
void codigo_A(int segundos){
    pid_A = getpid();
    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", getpid(),getppid());

    // Crea el proceso B
    pid_t pid_B = fork();
    if(pid_B == 0){
        // Rama hijo: el árbol continúa en B.c
        codigo_B(segundos);
    }
    else{
        // Rama padre:
        // Espera la señal que le envía Z (indirectamente, tras la alarma)
        signal(SIGUSR1, despertar);
        pause();
        // Muestra el árbol de procesos y espera a que termine pstree
        llamadaPstree(pidSuperPadre);
        wait(NULL);
        // Avisa al superpadre de que pstree ya se ha ejecutado
        kill(pidSuperPadre, SIGUSR1);
        // Espera a que ejec le ordene la terminación
        pause();
        // Propaga la orden a B y espera por él antes de morir
        kill(pid_B, SIGUSR1);
        wait(NULL);
        printf("Soy A (%d) y muero\n", getpid());
        exit(0);
    }
}