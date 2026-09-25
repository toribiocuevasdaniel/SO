#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

pid_t pidPadreOriginal;

void manejador_padre(int sig) {
    
}


void manejador_alarma(int sig) {

    kill(pidPadreOriginal, SIGUSR1);
}

void crearMalla(int x, int y) {
    for (int col = 0; col < y; col++) {
        pid_t pid = fork();

        if (pid == 0) { //hijo
        
            int fila = 0; 

            
            for (int f = 1; f < x; f++) {
                pid_t pid_cadena = fork();
                if (pid_cadena > 0) {
                    break; 
                }
                fila = f; 
            }

            
            if (col == (y - 1) && fila == (x - 1)) {
                
                signal(SIGALRM, manejador_alarma);
                alarm(1); 
                pause();        
                pause();
            } else {
                
                pause();
            }

            
            wait(NULL);
            exit(0);
        }
    }
}
void parse(){
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <filas (x)> <columnas (y)>", argv[0]);
        return 1;
    }
}
int main(int argc, char *argv[]) {
    parse(argc, argv);
    int filas = atoi(argv[1]);
    int columnas = atoi(argv[2]);
    pidPadreOriginal = getpid();

    
    signal(SIGUSR1, manejador_padre);

    
    crearMalla(filas, columnas);

    
    pause();

    
    system("pstree -c $(pgrep -o malla)");

    
    kill(0, SIGUSR1);

    for (int i = 0; i < columnas; i++) {
        wait(NULL);
    }

    return 0;
}