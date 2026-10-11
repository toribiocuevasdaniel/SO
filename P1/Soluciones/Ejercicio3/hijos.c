#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

pid_t pidSuperPadre;

// Estructura de datos de la memoria compartida
typedef struct {
    int x;
    int y;
    pid_t padres[100];   
    pid_t subhijos[100]; 
} MemoriaCompartida;

int shmid;
MemoriaCompartida *mem = NULL;

// Comprobación de argumentos
void parse(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <longitud cadena vertical> <numero subhijos finales>\n", argv[0]);
        exit(1);
    }
    else if (atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0) {
        fprintf(stderr, "Introduzca solo números positivos \n");
        exit(1);
    }
    
}

// Función vacía usada para despertar a los procesos de pause()
void despertar(int sig){}

// Función que configura todas las señales necesarias para los procesos
void configurarSenyales(){
    signal(SIGALRM, despertar);
    signal(SIGUSR1, despertar);
}


// El subhijo guarda su PID en la memoria compartida
void registrarSubhijo(int indice) {
    mem->subhijos[indice] = getpid();
}

// El subhijo imprime la lista de sus ascendientes (leída de la memoria compartida)
void imprimirPadresSubhijo() {
    printf("Soy el subhijo %d, mi padres son:", getpid());
    for (int k = 0; k < mem->x; k++) {
        if (k == mem->x - 1) {
            printf(" %d", mem->padres[k]);
        } else {
            printf(" %d,", mem->padres[k]);
        }
    }
    printf("\n");
}

// El subhijo espera su señal de terminación; el último avisa al superpadre
void terminarSubhijo(int indice, int y) {
    if (indice == y - 1) {          // Último hijo horizontal
        kill(pidSuperPadre, SIGUSR1); // Despierta al SuperPadre
    }
    pause(); // Espera señal para terminar
    exit(0);
}

// Lógica de creación de los subhijos
void crearHorizontal(int y, pid_t pids_Y[]){
    for(int i = 0; i < y; i++){
        pid_t pid = fork();
        if(pid == 0){ // Código del hijo
            configurarSenyales();
            registrarSubhijo(i);
            imprimirPadresSubhijo();
            terminarSubhijo(i, y);
        } else { // Código del padre
            pids_Y[i] = pid; 
        }
    }
}



// Indica si el nodo actual es el último de la vertical
int esUltimoNodo(int nivel_actual, int x) {
    return nivel_actual == x - 1;
}

// Manda la señal de terminación a los subhijos horizontales y los recoge
void terminarHijosHorizontal(pid_t pids_Y[], int y) {
    for(int j = 0; j < y; j++){
        kill(pids_Y[j], SIGUSR1);
    }
    for(int j = 0; j < y; j++){
        wait(NULL);
    }
}

// Comportamiento del último nodo vertical: crea la fila horizontal y la limpia
void ejecutarHojaVertical(int nivel_actual, int y) {
    pid_t pids_Y[y];

    crearHorizontal(y, pids_Y); // Crea los 'y' procesos horizontales
    pause();                    // Espera la señal para arrancar la limpieza

    terminarHijosHorizontal(pids_Y, y);

    // Si x == 1, el SuperPadre es la hoja vertical y debe volver al main
    if (nivel_actual == 0) return;

    exit(0);
}

// Un nodo intermedio propaga la señal hacia abajo, recoge a su hijo y muere
void propagarTerminacionYMorir(pid_t pidHijo) {
    kill(pidHijo, SIGUSR1); // Manda señal a su hijo directo
    wait(NULL);
    exit(0);
}

// Lógica de creación de la cadena de hijos verticales
void crearVertical(int nivel_actual, int x, int y) {
    // Guardamos el PID del nodo actual en su índice exacto
    mem->padres[nivel_actual] = getpid();

    // Si hemos alcanzado el último nodo de la vertical (índice x - 1)
    if (esUltimoNodo(nivel_actual, x)) {
        ejecutarHojaVertical(nivel_actual, y);
        return;
    }

    // Creamos el siguiente nodo de la cadena vertical
    pid_t pid = fork();

    if (pid == 0) { // Código del hijo
        crearVertical(nivel_actual + 1, x, y);
        exit(0);
    } else if (pid > 0) { // Código del padre
        configurarSenyales();
        pause(); // Espera la orden para propagar/continuar

        // Si NO es el SuperPadre, propaga la señal hacia abajo y muere
        if (nivel_actual > 0) {
            propagarTerminacionYMorir(pid);
        }
        // Si ES el SuperPadre (nivel_actual == 0), retorna al main() para ejecutar pstree
        return;
    } else {
        perror("Error en fork\n");
        exit(1);
    }
}


void imprimirSubhijosFinales(int y) {
    printf("Soy el superpadre (%d) : mis hijos finales son:", getpid());
    for (int j = 0; j < y; j++) {
        if (j == y - 1) {
            printf(" %d", mem->subhijos[j]);
        } else {
            printf(" %d,", mem->subhijos[j]);
        }
    }
    printf("\n");
}

void llamadaPstree(pid_t pidpadreOriginal) {
    if (fork() == 0) {
        char pid_str[16];
        sprintf(pid_str, "%d", pidpadreOriginal);
        execlp("pstree", "pstree", "-c", pid_str, NULL);
        perror("Error en execlp");
        exit(1);
    }
}

// Crea el segmento de memoria compartida y lo vincula
MemoriaCompartida *crearMemoriaCompartida(int x, int y) {
    shmid = shmget(IPC_PRIVATE, sizeof(MemoriaCompartida), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        exit(1);
    }
    MemoriaCompartida *m = (MemoriaCompartida *)shmat(shmid, NULL, 0);

    // Asignación de argumentos a sus equivalentes en la memoria compartida
    m->x = x;
    m->y = y;
    return m;
}

void liberarMemoriaCompartida() {
    shmdt(mem);
    shmctl(shmid, IPC_RMID, NULL);
}

// Función de flujo principal del programa
int main(int argc, char *argv[]){
    parse(argc, argv);
    configurarSenyales();
    pidSuperPadre = getpid();
    
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    // Declaramos la memoria compartida
    mem = crearMemoriaCompartida(x, y);

    // Inicia la cadena vertical
    crearVertical(0, x, y);

    // Al despertar el SuperPadre y retornar al main, imprime el mensaje final
    imprimirSubhijosFinales(y);

    // Ejecutamos el pstree para mostrar la jerarquía
    llamadaPstree(getpid());
    wait(NULL); // Espera a que termine pstree

    // Si x > 1, despierta a la cadena vertical para iniciar la muerte en cascada
    if (x > 1) {
        // Obtenemos el PID del primer hijo vertical almacenado en mem->padres[1]
        kill(mem->padres[1], SIGUSR1);
        wait(NULL);
    }

    // Liberamos recursos del segmento de memoria compartida
    liberarMemoriaCompartida();
    exit(0);
}
