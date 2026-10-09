// prog2.c - Ejemplo de la práctica: programa invocado por prog1 tras la
// llamada execvp(). Comparte el mismo PID que prog1, ya que exec solo
// sustituye la imagen del proceso, no crea uno nuevo.
// Los argv recibidos son los mismos que se pasaron originalmente a prog1.
//
// Compilación:  gcc -o prog2 prog2.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main (int argc, char *argv[])
{
int i;
// Muestra los argumentos heredados del programa que lo invocó
printf ("Ejecutando el programa invocado (prog2). Sus argumentos son: \n");
for ( i = 0; i < argc; i ++ )
printf (" argv[%d] : %s \n", i, argv[i]);
sleep(10); // Tiempo para observar el proceso con ps
exit (0);
}