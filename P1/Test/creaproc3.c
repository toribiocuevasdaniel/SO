// creaproc3.c - Ejemplo de la práctica: recogida del estado de finalización
// de un hijo con wait(&estado).
// El hijo muere con exit(13); el padre decodifica el estado:
//   - (estado & 0x7F) != 0  -> el hijo murió por una señal
//   - (estado >> 8) & 0xFF  -> valor devuelto por exit()
// (También puede probarse matando al hijo con: kill -9 <pid_hijo>)
//
// Compilación y ejecución:  gcc -o creaproc3 creaproc3.c && ./creaproc3

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main ( ) {
int estado, numero;
switch(fork()) {
case -1 : /* ERROR */
perror("Error en fork");
exit(1);
case 0 : /* HIJO */
numero = 13;
printf("Soy el hijo y muero con %d...\n", numero);
sleep(20);
exit(numero);
default : /* PADRE */
wait(&estado); // Espera a que el hijo termine y recoge su estado
printf("Soy el padre. ");
if ((estado & 0x7F) != 0) {
printf("Mi hijo ha muerto con una señal.\n");
}
else {
printf("Mi hijo ha muerto con exit(%d).\n",
(estado>>8) & 0xFF);
}
exit(0);
}
}