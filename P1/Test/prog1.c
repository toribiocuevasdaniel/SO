// prog1.c - Ejemplo de la práctica: cambio de imagen de un proceso con exec.
// prog1 muestra sus argumentos, espera 10 segundos (para poder observarlo
// con ps) y después sustituye su imagen por la de prog2 mediante execvp().
// Nota: strcpy(argv[0], "prog2") solo altera el nombre visible en ps;
// la imagen real la sustituye la llamada execvp().
//
// Compilar AMBOS programas antes de ejecutar (prog2 debe estar en el
// mismo directorio porque se invoca como "./prog2"):
//   gcc -o prog2 prog2.c && gcc -o prog1 prog1.c
//   ./prog1 p1 p2 param3 &
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
int main (int argc, char *argv[])
{
int i;
// Muestra los argumentos con los que se lanzó el programa
printf ("\nEjecutando el programa invocador (prog1). Sus argumentos son: \n");
for ( i = 0; i < argc; i ++ )
printf (" argv[%d] : %s \n", i, argv[i]);
sleep( 10 );
// Sustituye la imagen del proceso por la de prog2 conservando los argumentos
strcpy (argv[0],"prog2");
if (execvp ("./prog2", argv) < 0) {
printf ("Error en la invocacion a prog2 \n");
exit (1);
};
exit (0);
}