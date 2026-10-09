// vertical.c - Ejercicio 1a: creación de las columnas de la malla.
// Cada "cabeza de columna" (creada en horizontal.c) genera una cadena
// vertical de x-1 fork() encadenados: cada proceso solo conoce a su hijo
// directo, con lo que la estructura resultante es una malla x * y.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Definidos en malla.c y pfinal.c
extern pid_t pidPadreOriginal;
extern void despertar(int sig);
void manejador_alarma(int sig);

// Rutina exclusiva del proceso de la esquina inferior derecha (pxy):
void codigo_ultimo_hijo(void) {
    signal(SIGALRM, manejador_alarma);
    alarm(1);
    pause(); // Despierta con la alarma y notifica al superpadre enviando SIGUSR1
}

// Crea la cadena vertical de una columna a partir de la cabeza de columna.
// 'col' es el índice de columna; 'fila' se actualiza en cada iteración para
// que cada hijo sepa su profundidad dentro de la cadena.
void crearVertical(int col, int x, int y) {
    int fila = 0;
    pid_t pid_hijo_abajo = -1; // PID del hijo directo; -1 si no tiene (última fila)

    // Creación de la cadena vertical
    // Tras el primer fork, solo el hijo continúa el bucle: el padre sale
    // conservando el PID de su hijo directo en 'pid_hijo_abajo'
    for (int f = 1; f < x; f++) {
        pid_t pid = fork();

        if (pid > 0) {
            pid_hijo_abajo = pid; // Guarda el PID del hijo inferior
            break; 
        }
        else if(pid < 0){
            perror("Error en fork");
            exit(1);
        }
        fila = f; 
    }
    // Se ejecuta el proceso del último hijo creado
    if (col == (y - 1) && fila == (x - 1)) {
        codigo_ultimo_hijo();
    }

    // Todos los procesos de la cadena quedan bloqueados aquí hasta que
    // reciban la señal de terminación
    signal(SIGUSR1, despertar);
    pause(); 

    // Propagación de la terminación hacia abajo por la columna
    if (pid_hijo_abajo > 0) {
        kill(pid_hijo_abajo, SIGUSR1);
        wait(NULL); 
    }
    exit(0);
}