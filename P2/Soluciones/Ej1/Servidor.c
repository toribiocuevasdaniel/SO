
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO  9999
#define COLA    5
#define TAM_BUF 1024
#define ARCHIVO "Google.html"

int main()
{
    int conector, conexion;
    struct sockaddr_in dir_servidor, dir_cliente;
    socklen_t long_cliente;
    pid_t pid;

    signal(SIGCHLD, SIG_IGN);

    conector = socket(AF_INET, SOCK_STREAM, 0);
    if (conector < 0) {
        perror("socket");
        exit(1);
    }

    memset(&dir_servidor, 0, sizeof(dir_servidor));
    dir_servidor.sin_family      = AF_INET;
    dir_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
    dir_servidor.sin_port        = htons(PUERTO);

    if (bind(conector, (struct sockaddr *)&dir_servidor, sizeof(dir_servidor)) < 0) {
        perror("bind");
        close(conector);
        exit(1);
    }

    if (listen(conector, COLA) < 0) {
        perror("listen");
        close(conector);
        exit(1);
    }

    printf("Servidor escuchando en el puerto %d...\n", PUERTO);
    fflush(stdout);

    while (1) {
        long_cliente = sizeof(dir_cliente);

        conexion = accept(conector, (struct sockaddr *)&dir_cliente, &long_cliente);
        if (conexion < 0) {
            perror("accept");
            continue;
        }

        printf("Conexion de %s\n", inet_ntoa(dir_cliente.sin_addr));
        fflush(stdout);     /* evita que el hijo duplique el buffer tras fork */

        pid = fork();
        if (pid < 0) {
            perror("fork");
            close(conexion);
            continue;
        }

        if (pid == 0) {

            char buffer[TAM_BUF];
            ssize_t leidos, escritos, total;
            int fd;

            close(conector);            

            fd = open(ARCHIVO, O_RDONLY);
            if (fd < 0) {
                perror("open " ARCHIVO);
                close(conexion);
                exit(1);
            }

            while ((leidos = read(fd, buffer, TAM_BUF)) > 0) {
                total = 0;

                while (total < leidos) {
                    escritos = write(conexion, buffer + total, leidos - total);
                    if (escritos < 0) {
                        perror("write");
                        close(fd);
                        close(conexion);
                        exit(1);
                    }
                    total += escritos;
                }
            }

            close(fd);
            close(conexion);            
            exit(0);
        }


        close(conexion);
    }

    close(conector);
    return 0;
}