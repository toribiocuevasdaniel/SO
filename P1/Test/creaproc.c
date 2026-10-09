// Primer código de ejemplo de la práctica: creación de un proceso con fork().
// El padre y el hijo imprimen sus PIDs y duermen (30 s y 20 s) para que
// ambos puedan observarse con 'ps' mientras están vivos.
//
// Compilación y ejecución:  gcc -o creaproc creaproc.c && ./creaproc &
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
pid_t pid;
pid = fork();
switch (pid)
{
case -1: // Error: no se pudo crear el proceso hijo
printf ("No he podido crear el proceso hijo \n");
break;
case 0: // HIJO: getpid() es su PID y getppid() el del padre
// Tras el fork, getppid() puede devolver 1 si el padre ya terminó

printf ("Soy el hijo, mi PID es %d y mi PPID es %d \n",
getpid(), getppid());
sleep (20);
break;
default: // PADRE: 'pid' contiene el PID recién creado
printf ("Soy el padre, mi PID es %d y el PID de mi hijo es %d \n",
getpid(), pid);
sleep (30);
}
printf ("Final de ejecución de %d \n", getpid());
exit (0);
}