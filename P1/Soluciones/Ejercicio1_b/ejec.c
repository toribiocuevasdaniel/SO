// ejec.c - Ejercicio 1b: proceso raíz (superpadre) del árbol
//   ejec -> A -> B -> (X, Y, Z)
// Recibe un único argumento: los segundos que Z debe esperar con alarm()
// antes de desencadenar la cadena de señales que muestra pstree y destruye
// el árbol en orden (los padres no mueren antes que sus hijos).
// Composición del programa: ejec.c + A.c + B.c + X.c + Y.c + Z.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// PID del superpadre; lo comparten A.c, B.c, X.c, Y.c y Z.c con 'extern'
pid_t pidSuperPadre;

// Definida en A.c
void codigo_A(int segundos);

// Manejador vacío: solo interrumpe pause() al recibir SIGUSR1
void despertar(int sig){}

// Valida los argumentos de línea de comandos: exactamente un entero
// positivo (duración en segundos de la alarma de Z)
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


// Flujo principal del superpadre 'ejec'
int main(int argc, char *argv[]){
    printf("Soy el proceso ejec: mi pid es %d\n", getpid());
    pidSuperPadre = getpid();

    // Crea el proceso A
    pid_t pid_A = fork();
    if(pid_A == 0){
        // Rama hijo: el árbol continúa en A.c
        codigo_A(atoi(argv[1]));
    }
    else if(pid_A == -1){
        perror("Error en fork");
    }
    else{
        // Rama padre: espera el aviso de A (ya mostrado pstree) y después
        // ordena la terminación de todo el árbol
        signal(SIGUSR1, despertar);
        pause();                 // Espera el aviso de A
        kill(pid_A, SIGUSR1);    // Ordena la terminación de A
        wait(NULL);              // No muere hasta que A haya muerto
        printf("Soy ejec (%d) y muero\n", getpid());
        exit(0);
    }
}