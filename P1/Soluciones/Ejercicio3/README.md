# Ejercicio 3 — `hijos.c` (árbol de procesos con memoria compartida)

## Enunciado

Programa `hijos.c` que crea un árbol de procesos a partir de dos
parámetros, `x` (longitud de la cadena vertical) e `y` (número de subhijos
horizontales):

```sh
$ ./hijos x y
```

Salida requerida por el enunciado:

```
Soy el superpadre (pid): mis hijos finales son: pidx1, pidx2, ..., pidxy
Soy el subhijo pidx1, mis padres son: pid1, pid2, ..., pidx
```

Es decir: el superpadre lanza una **cadena vertical** de `x` procesos y el
último de ellos crea una **fila horizontal** de `y` procesos (los
"subhijos" finales). Los subhijos deben listar todos sus ascendientes, y
el superpadre todos los subhijos finales: para eso se usa **memoria compartida**
 (`shmget`/`shmat`), ya que los PIDs son visibles para todos
los procesos a través del segmento.

## Compilación y ejecución

```sh
gcc -Wall -o hijos hijos.c
./hijos 4 3
```

Ejemplo de salida:

```
Soy el subhijo 24536, mi padres son: 24531, 24533, 24534, 24535
Soy el subhijo 24537, mi padres son: 24531, 24533, 24534, 24535
Soy el subhijo 24538, mi padres son: 24531, 24533, 24534, 24535
Soy el superpadre (24531): mis hijos finales son: 24536, 24537, 24538
```

Ambos argumentos deben ser enteros positivos.

## Estructura de datos compartida

```c
typedef struct {
    int x;                 // longitud de la cadena vertical
    int y;                 // número de subhijos horizontales
    pid_t padres[100];     // PID de cada nodo de la cadena vertical
    pid_t subhijos[100];   // PID de los subhijos horizontales
} MemoriaCompartida;
```

Se crea con `shmget(IPC_PRIVATE, ...)` y se vincula con `shmat()`; el
superpadre la libera al final con `shmdt()` y `shmctl(IPC_RMID)`.

## Cómo funciona

1. `main()` valida los argumentos, programa el manejador de señales,
   crea el segmento de memoria compartida y guarda `x`, `y` y su propio PID.
2. Hace un `fork()`: el hijo ejecuta `crearVertical()`.
3. `crearVertical()` encadena `x-1` fork. Cada proceso guarda su PID en
   `mem->padres[]` y solo conoce a su hijo directo:
   - los **nodos intermedios** quedan en `pause()`; al recibir `SIGUSR1`
     reenvían la señal a su hijo, esperan con `wait()` y mueren
     (terminación en cascada de arriba abajo);
   - el **último nodo** llama a `crearHorizontal()`, espera en `pause()` y,
     al despertar, señala a sus `y` hijos horizontales, los recoge y muere.
4. `crearHorizontal()` crea los `y` subhijos. Cada uno guarda su PID en
   `mem->subhijos[]`, imprime su lista de padres (leída de la memoria
   compartida) y espera en `pause()`. El **último** subhijo, al despertar,
   avisa al superpadre con `SIGUSR1`.
5. El superpadre despierta, imprime la lista de subhijos finales, envía
   `SIGUSR1` al primer hijo de la cadena para desencadenar la cascada de
   terminación, espera con `wait()` y libera la memoria compartida.

## Llamadas al sistema empleadas

`fork`, `signal`, `pause`, `kill`, `wait`, `shmget`, `shmat`, `shmdt`,
`shmctl`, `getpid`.
