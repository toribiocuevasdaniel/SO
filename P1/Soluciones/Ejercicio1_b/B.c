// B.c - Ejercicio 1b: código del proceso B (hijo de A, padre de X, Y y Z).
// Crea sus tres hijos en un único bucle (reutilizado con un switch para
// llamar a la rutina correspondiente de cada uno) y espera bloqueado hasta
// que A le ordene la terminación; entonces señala a los tres y recoge a
// cada uno antes de morir.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Definidos en ejec.c, A.c, X.c, Y.c y Z.c
extern pid_t pidSuperPadre;
extern void despertar(int sig);
void codigo_X(void);
void codigo_Y(void);
void codigo_Z(int segundos);

// Rutina del proceso B
void codigo_B(int segundos){
    pid_t pids[3]; // PIDs de X (0), Y (1) y Z (2)
    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n", getpid(), getppid(), pidSuperPadre);

    // Crea los tres hijos: en cada iteración el fork devuelve un índice
    // distinto de 'i', de forma que el hijo ejecuta su propia rutina
    for(int i = 0; i < 3; i++){
        pid_t pid = fork();

        if(pid == 0){
            // Rama hijo: según el valor de i se ejecuta X, Y o Z
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
            pids[i] = pid; // Rama padre: guarda el PID del hijo creado
        }
    }

    // Espera bloqueado la orden de terminación que envía A
    signal(SIGUSR1, despertar);
    pause();

    // Propaga la orden a sus tres hijos y espera a que los tres mueran
    for(int i = 0; i < 3; i++){
        kill(pids[i],SIGUSR1);
    }
    for(int i = 0; i < 3; i++){
        wait(NULL);
    }
    printf("Soy B (%d), y muero\n", getpid());
    exit(0);
}