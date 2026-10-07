#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void crearVertical(int col, int x, int y);

void crearHorizontal(int x, int y, pid_t pids[]) {
    for (int col = 0; col < y; col++) {
        pid_t pid = fork();

        if (pid == 0) { // Proceso Hijo (Cabeza de Columna)
            crearVertical(col, x, y);
            exit(0); 
        } else if (pid > 0) {
            pids[col] = pid; // Guarda el PID del hijo en el array del padre
        }
    }
}