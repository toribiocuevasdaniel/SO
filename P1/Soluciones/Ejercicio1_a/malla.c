// malla.c - Ejercicio 1a: crea una malla de procesos de x filas por y columnas
// y muestra el árbol resultante con pstree -c.
// Composición del programa: malla.c + horizontal.c + vertical.c + pfinal.c
//
// Estructura del árbol generado:
//   malla (superpadre)
//     -> p11, p12, ... p1y   (primera fila, creados por crearHorizontal)
//        -> p21, ... p2y     (cadena vertical de cada columna)
//           -> ...
//              -> px1, ... pxy (última fila)
// El proceso en la esquina inferior derecha (pxy) avisa al superpadre con
// SIGALRM/SIGUSR1, que entonces muestra pstree y desencadena la destrucción
// ordenada del árbol propagando SIGUSR1 hacia abajo.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// PID del proceso principal; lo comparten el resto de ficheros con 'extern'
pid_t pidPadreOriginal;

// Prototipos de funciones definidas en otros ficheros de la práctica
void configurar_senyales(void);
void despertar(int sig);
void crearHorizontal(int x, int y, pid_t pids[]);

// Valida los argumentos de línea de comandos: necesita exactamente
// dos argumentos positivos (filas x y columnas y)
void parse(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <filas (x)> <columnas (y)>\n", argv[0]);
        exit(1);
    }
    else if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0) {
        fprintf(stderr, "Introduzca números positivos para filas y columnas\n");
        exit(1);
    }
}

// Lanza 'pstree -c <pid>' en un proceso hijo para mostrar el árbol de procesos.
// El padre no espera aquí: la espera se hace en main() con wait(NULL)
void llamadaPstree(pid_t pidpadreOriginal) {
    if (fork() == 0) {
        char pid_str[16];
        sprintf(pid_str, "%d", pidpadreOriginal);
        execlp("pstree", "pstree", "-c", pid_str, NULL);
        perror("Error en execlp");
        exit(1);
    }
}

// Flujo principal del programa
int main(int argc, char *argv[]) {
    // Validar argumentos y obtener parámetros de entrada
    parse(argc, argv);
    int filas = atoi(argv[1]);
    int columnas = atoi(argv[2]);
    pidPadreOriginal = getpid();


    // Programar el manejador que despierta a pause()
    configurar_senyales();

    // Crear la primera fila de procesos (cada uno inicia su columna vertical)
    pid_t pids[columnas];
    crearHorizontal(filas, columnas, pids);

    // Espera la llegada de la señal del último hijo (esquina inferior derecha)
    pause();

    // Muestra el árbol de procesos
    llamadaPstree(pidPadreOriginal);
    wait(NULL); 

    // Señal de terminación a cada hijo de la primera fila
    for (int i = 0; i < columnas; i++) {
        kill(pids[i], SIGUSR1);
    }

    // Recoger a los hijos de la primera fila antes de terminar
    for (int i = 0; i < columnas; i++) {
        wait(NULL);
    }

    return 0;
}