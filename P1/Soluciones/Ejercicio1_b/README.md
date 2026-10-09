# Ejercicio 1b — `ejec.c` (árbol ejec → A → B → X, Y, Z)

## Enunciado

Programa `ejec.c` que recibe un único argumento (segundos) y genera el
siguiente árbol de procesos:

```
       ejec
         A
         B
      X  Y  Z
```

- El proceso **Z** no puede usar `sleep()`: debe programarse una alarma con
  `alarm(segundos)`.
- Transcurridos esos segundos, **Z** envía una señal a **A**, que ejecuta el
  comando `pstree` para mostrar el árbol.
- Después debe destruirse el árbol en orden: los padres no pueden morir
  antes que sus hijos.

Salida esperada (abreviada):

```
$ ./ejec 15
Soy el proceso ejec: mi pid es 751
Soy el proceso A: mi pid es 752. Mi padre es 751
Soy el proceso B: mi pid es 753. Mi padre es 752. Mi abuelo es 751
Soy el proceso X: mi pid es 754. Mi padre es 753. ...
Soy el proceso Y: mi pid es 755. ...
Soy el proceso Z: mi pid es 756. ...
<pstree tras 15 segundos>
Soy Z (756) y muero
Soy Y (755) y muero
Soy X (754) y muero
Soy B (753) y muero
Soy A (752) y muero
Soy ejec (751) y muero
```

## Ficheros

| Fichero | Proceso | Contenido |
|---|---|---|
| `ejec.c` | `ejec` (superpadre) | Validación de argumentos, creación de A y terminación ordenada del árbol |
| `A.c` | `A` | Crea B, espera la señal de Z, lanza `pstree`, avisa a `ejec` y propaga la orden a B |
| `B.c` | `B` | Crea X, Y y Z en un único bucle; espera la orden de A y la propaga a los tres |
| `X.c` | `X` | Proceso hoja: se identifica, espera y muere |
| `Y.c` | `Y` | Proceso hoja: se identifica, espera y muere |
| `Z.c` | `Z` | Programa la alarma, avisa a A al vencer y espera la orden de terminación |

Los PID de A y del superpadre se comparten entre ficheros mediante variables
globales con `extern` (`pid_A`, `pidSuperPadre`), igual que el manejador
`despertar()`, que se define en `ejec.c`.

## Compilación y ejecución

```sh
gcc -o ejec ejec.c A.c B.c X.c Y.c Z.c
./ejec 15            # Z esperará 15 segundos
```

Se recomienda compilar con `-Wall`. El argumento debe ser un entero
positivo.

## Secuencia de señales

1. `ejec` hace `fork()` de **A** y queda en `pause()` esperando `SIGUSR1`.
2. **A** crea **B**, que crea **X**, **Y** y **Z**; todos quedan en `pause()`.
3. **Z** programa `alarm(segundos)`; al vencer, despierta de su `pause()` y
   envía `SIGUSR1` a **A**.
4. **A** ejecuta `pstree -c <pid_superpadre>` (en un proceso hijo), espera a
   que termine y envía `SIGUSR1` a `ejec`.
5. `ejec` despierta, envía `SIGUSR1` a **A** y espera con `wait()` a que muera.
6. **A** envía `SIGUSR1` a **B** y espera por él. **B** lo propaga a **X**,
   **Y** y **Z** y espera a los tres. Los hijos despiertan y mueren.
7. La muerte se recoge hacia arriba con `wait()` en cada nivel, de forma que
   ningún padre muere antes que sus hijos.

## Llamadas al sistema empleadas

`fork`, `signal`, `alarm`, `pause`, `kill`, `wait`, `execlp` (para
`pstree -c`), `getpid`, `getppid`.
