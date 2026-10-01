#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void crearVertical(int col, int x, int y);

void crearHorizontal(int x, int y) {
    for (int col = 0; col < y; col++) {
        pid_t pid = fork();

        if (pid == 0) { // Hijo de la columna
            crearVertical(col, x, y);
            exit(0); // El proceso columna finaliza tras terminar su parte de la cadena
        }
    }
}