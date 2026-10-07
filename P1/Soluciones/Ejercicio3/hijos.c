#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

pid_t pidSuperPadre;

typedef struct {
    int x;
    int y;
    pid_t padres[100];   
    pid_t subhijos[100]; 
} MemoriaCompartida;

int shmid;
MemoriaCompartida *mem = NULL;

void parse(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <longitud cadena vertical> <numero subhijos finales>\n", argv[0]);
        exit(1);
    }
    else if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0) {
        fprintf(stderr, "Introduzca solo números enteros y positivos \n");
        exit(1);
    }
}

void despertar(int sig){}

void configurarSenyales(){
    signal(SIGALRM, despertar);
    signal(SIGUSR1, despertar);
}



void crearHorizontal(int y, pid_t pids_Y[]){
    int col = -1; // -1 indica que es el PADRE que crea la fila

    for(int i = 0; i < y; i++){
        pid_t pid = fork();
        if(pid == 0){ // Código del hijo
            configurarSenyales();
            col = i;

            mem->subhijos[i] = getpid();
            printf("Soy el subhijo %d, mi padres son:", getpid());
            for (int k = 0; k < mem->x; k++) {
                if (k == mem->x - 1) {
                    printf(" %d", mem->padres[k]);
                } else {
                    printf(" %d,", mem->padres[k]);
                }
            }
            printf("\n");
           

            break; 
        } else { // Codigo del padre
            pids_Y[i] = pid; 
        }
    }

    // Solo los HIJOS horizontales entran aquí
    if(col != -1){
        if(col == y - 1){ // Último hijo horizontal
            wait(NULL); 
            kill(pidSuperPadre, SIGUSR1); // Despierta al SuperPadre
            pause(); // Espera señal para terminar
            exit(0);
        } else { // Resto de hijos horizontales
            pause(); // Espera señal para terminar
            exit(0);
        }
    }
    // El PADRE no entra al 'if', simplemente termina la función 
    // y mantiene 'pids_Y' lleno con los PIDs de sus hijos.
}

void crearVertical(int x, int y) {
    pid_t pids_Y[y];
    pid_t ultimo_hijo = 0;

    for (int i = 1; i < x; i++) {
        pid_t pid = fork();

        if (pid > 0) { // Código del padre
            configurarSenyales();
            ultimo_hijo = pid; // Guarda el PID de su hijo directo
            break; 
        }

        // Código del hijo
    
        mem->padres[i] = getpid();
        

        if (i == x - 1) { // Si es el último nodo de la cadena vertical
            crearHorizontal(y, pids_Y); // Crea los 'y' procesos horizontales
            pause(); // Espera la señal para arrancar la limpieza

            // Manda señal a sus 'y' hijos horizontales
            for(int j = 0; j < y; j++){
                kill(pids_Y[j], SIGUSR1);
            }
            for(int j = 0; j < y; j++){
                wait(NULL);
            }

            exit(0);
        }
    }

    // Código común para los nodos intermedios del padre/cadena
    if (ultimo_hijo > 0) {
        pause(); // Espera la orden para propagar
        kill(ultimo_hijo, SIGUSR1); // Manda señal a SU HIJO directo (no usa 0)
        wait(NULL);
        exit(0);
    }
}

int main(int argc, char *argv[]){
    parse(argc, argv);
    configurarSenyales();
    pidSuperPadre = getpid();
    
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);


    shmid = shmget(IPC_PRIVATE, sizeof(MemoriaCompartida), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        exit(1);
    }
    mem = (MemoriaCompartida *)shmat(shmid, NULL, 0);
    mem->x = x;
    mem->y = y;
    mem->padres[0] = pidSuperPadre; 
    

    pid_t pid = fork();
    if(pid == 0){
        crearVertical(x, y);
        exit(0);
    }
    else if(pid == -1){
        perror("Error en fork\n");
        exit(1);
    }

    pause(); 


    printf("Soy el superpadre (%d): mis hijos finales son:", getpid());
    for (int j = 0; j < y; j++) {
        if (j == y - 1) {
            printf(" %d", mem->subhijos[j]);
        } else {
            printf(" %d,", mem->subhijos[j]);
        }
    }
    printf("\n");


    kill(pid, SIGUSR1); // Despierta a la cadena en cascada
    wait(NULL);


    shmdt(mem);
    shmctl(shmid, IPC_RMID, NULL);

    exit(0);
}