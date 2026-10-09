// creaproc1.c - Ejemplo de la práctica: demuestra que los procesos creados
// con fork() tienen zonas de datos PRIVADAS (no compartidas).
// Cada proceso modifica su propia copia de la variable 'i': el padre
// imprime valores impares (parte desde 1) y el hijo pares (parte desde 0).
//
// Compilación y ejecución:  gcc -o creaproc1 creaproc1.c && ./creaproc1 &

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main ( ) {
int i;
int j;
pid_t pid;
pid = fork( );
switch (pid) {
case -1: // Error al crear el proceso hijo
printf ("\nNo he podido crear el proceso hijo");
break;
case 0: // HIJO: su copia de 'i' empieza en 0 y suma de 2 en 2
i = 0;
printf ("\nSoy el hijo, mi PID es %d y mi variable i (inicialmente a %d) es par", getpid(), i);
for ( j = 0; j < 5; j ++ ) {
i ++;   
i ++;
printf ("\nSoy el hijo, mi variable i es %d", i);
};
break;
default: // PADRE: su copia de 'i' empieza en 1 y suma de 2 en 2
i = 1;
printf ("\nSoy el padre, mi PID es %d y mi variable i (inicialmente a %d) es impar", getpid(), i);
for ( j = 0; j < 5; j ++ ) {
i ++;
i ++;
printf ("\nSoy el padre, mi variable i es %d", i);
};
};
printf ("\nFinal de ejecucion de %d \n", getpid());
exit (0);
}