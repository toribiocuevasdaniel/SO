# Ejercicio 1a — `malla.c` (malla de procesos x × y)

## Enunciado

Programa `malla.c` que recibe dos argumentos `x` (filas) e `y` (columnas) y
genera un árbol de procesos con forma de malla:

```
              malla
 p11   p12   p13  ...  p1y
 p21   p22   p23  ...  p2y
  ...    ...    ...   ...
 px1   px2   px3  ...  pxy
```

La estructura real se comprueba con `pstree -c`, que el programa lanza
automáticamente una vez que la malla está completa.

## Ficheros

| Fichero | Contenido |
|---|---|
| `malla.c` | Programa principal: validación de argumentos, espera de la señal de la esquina inferior derecha, lanzamiento de `pstree` y ordenación de la terminación |
| `horizontal.c` | `crearHorizontal()`: crea la primera fila (una cabeza de columna por cada columna) |
| `vertical.c` | `crearVertical()`: crea la cadena vertical de cada columna con `x-1` fork encadenados |
| `pfinal.c` | Manejadores compartidos: `despertar()`, `manejador_alarma()` y `configurar_senyales()` |

## Compilación y ejecución

```sh
gcc -o malla malla.c horizontal.c vertical.c pfinal.c
./malla 3 3          # malla de 3 filas y 3 columnas
```

Los argumentos deben ser enteros positivos; si no, el programa muestra el
uso correcto y termina con error.

## Cómo funciona

1. `main()` programa el manejador de `SIGUSR1` y llama a `crearHorizontal()`,
   que hace un `fork()` por columna. Cada hijo es la cabeza de su columna.
2. Cada cabeza de columna llama a `crearVertical()`, que encadena `x-1`
   fork: cada proceso solo conoce a su hijo directo, formando la columna.
3. El proceso de la esquina inferior derecha (`pxy`) programa una alarma de
   1 s con `alarm()`; su manejador envía `SIGUSR1` al superpadre (evita
   bloquearse si el superpadre todavía no está en `pause()`).
4. El superpadre despierta, lanza `pstree -c <pid>` en un proceso hijo y
   espera a que termine.
5. Después propaga la terminación: envía `SIGUSR1` a los hijos de la primera
   fila, que a su vez lo reenvían a su hijo directo, y así hasta abajo.
   Cada proceso espera (`wait()`) a su hijo antes de morir, de modo que
   ningún padre muere antes que sus hijos.

## Llamadas al sistema empleadas

`fork`, `pause`, `kill`, `signal`, `alarm`, `wait`, `execlp` (para
`pstree -c`), `getpid`.
