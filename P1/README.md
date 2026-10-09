# Práctica 1 — Gestión de procesos y archivos

Práctica de la asignatura **Sistemas Operativos**: creación y terminación
de procesos, cambio de imagen, señales, gestión de archivos, tuberías y
memoria compartida.

- **Enunciado completo:** (Doc/Enunciado_P1.pdf)
- **Entrega:** semana del 5 al 9 de octubre (memoria + código fuente).

## Estructura del repositorio

```
P1/
├── Doc/
│   └── Enunciado_P1.pdf        Enunciado de la práctica
├── Soluciones/
│   ├── Ejercicio1_a/           Ejercicio 1a: malla de procesos (pstree)
│   │   ├── README.md
│   │   ├── malla.c  horizontal.c  vertical.c  pfinal.c
│   │   └── malla               (binario)
│   ├── Ejercicio1_b/           Ejercicio 1b: árbol ejec → A → B → X,Y,Z
│   │   ├── README.md
│   │   ├── ejec.c  A.c  B.c  X.c  Y.c  Z.c
│   │   └── ejec                (binario)
│   ├── Ejercicio2/             Ejercicio 2: hacha (división con tuberías)
│   │   ├── README.md
│   │   ├── hacha.c
│   │   └── hacha               (binario)
│   └── Ejercicio3/             Ejercicio 3: hijos (memoria compartida)
│       ├── README.md
│       ├── hijos.c
│       └── hijos               (binario)
└── Test/                       Programas de ejemplo del enunciado
```

Cada ejercicio tiene su propio `README.md` con enunciado, compilación,
ejemplo de ejecución y explicación del funcionamiento.

## Compilación rápida

```sh
# Ejercicio 1a
gcc -Wall -o malla Soluciones/Ejercicio1_a/malla.c \
    Soluciones/Ejercicio1_a/horizontal.c \
    Soluciones/Ejercicio1_a/vertical.c \
    Soluciones/Ejercicio1_a/pfinal.c

# Ejercicio 1b
gcc -Wall -o ejec Soluciones/Ejercicio1_b/*.c

# Ejercicio 2
gcc -Wall -o hacha Soluciones/Ejercicio2/hacha.c

# Ejercicio 3
gcc -Wall -o hijos Soluciones/Ejercicio3/hijos.c
```

## Test/ — ejemplos del enunciado

Programas de demostración de las llamadas al sistema vistas en la
práctica (todos documentados en cabecera):

| Fichero | Demuestra |
|---|---|
| `creaproc.c` | `fork()`: creación de un proceso hijo e identificación con `getpid()`/`getppid()`, observables con `ps` |
| `creaproc1.c` | Los procesos tienen zonas de datos **privadas** tras el `fork()` (cada uno modifica su propia copia de `i`) |
| `creaproc3.c` | Recogida del estado de finalización con `wait(&estado)` y decodificación (señal o valor de `exit()`) |
| `prog1.c` / `prog2.c` | Cambio de imagen con `execvp()`: `prog1` sustituye su imagen por `prog2` conservando PID y argumentos |

Compilación de los programas que van en pareja (exec):

```sh
gcc -o prog2 prog2.c && gcc -o prog1 prog1.c
./prog1 p1 p2 param3 &
```

> `prog2` debe estar en el mismo directorio porque `prog1` lo invoca como
> `./prog2`.

## Notas

- Requieren un sistema Unix-like (probado en maquina virtual de Ubuntu con `pstree`
  para los ejercicios 1a y 1b).
- Los README.md de cada ejercicio de la práctica permiten su comprensión práctica
  de funcionamiento, para entender a nivel conceptual y teórico los ejercicios
  que componen esta práctica, esnecesaria la lectura de la memoria de la práctica 
  Doc/Memoria.pdf o Doc/Memoria.docx
