// horizontal.c - Ejercicio 1a: creación de la primera fila de la malla.
// Cada hijo creado aquí actúa como "cabeza de columna" y a su vez crea
// la cadena vertical correspondiente (ver vertical.c).

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Definida en vertical.c
void crearVertical(int col, int x, int y);

// Crea 'y' procesos hijos (uno por columna). Cada hijo invoca crearVertical()
// para construir su columna y termina con exit(0) al finalizar esa tarea.
// El padre guarda los PIDs de los hijos en el array 'pids' (indexado por
// columna) para poder señalarlos después desde main().
void crearHorizontal(int x, int y, pid_t pids[]) {
    for (int col = 0; col < y; col++) {
        pid_t pid = fork();

        if (pid == 0) { // Proceso Hijo (Cabeza de Columna)
            // Construye la columna vertical y finaliza el proceso
            crearVertical(col, x, y);
            exit(0); 
        } else if (pid > 0) {
            pids[col] = pid; 
        }
        else{
            perror("Error en fork");
            exit(1);
        }
    }
}