
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO  9999
#define TAM_BUF 1024

int main(int argc, char *argv[])
{
    int conector;
    struct sockaddr_in dir_servidor;
    char buffer[TAM_BUF];
    ssize_t leidos;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s IP_Servidor\n", argv[0]);
        exit(1);
    }


    conector = socket(AF_INET, SOCK_STREAM, 0);
    if (conector < 0) {
        perror("socket");
        exit(1);
    }


    memset(&dir_servidor, 0, sizeof(dir_servidor));
    dir_servidor.sin_family      = AF_INET;
    dir_servidor.sin_addr.s_addr = inet_addr(argv[1]);
    dir_servidor.sin_port        = htons(PUERTO);

    if (dir_servidor.sin_addr.s_addr == INADDR_NONE) {
        fprintf(stderr, "IP no valida: %s\n", argv[1]);
        close(conector);
        exit(1);
    }


    if (connect(conector, (struct sockaddr *)&dir_servidor, sizeof(dir_servidor)) < 0) {
        perror("connect");
        close(conector);
        exit(1);
    }


    while ((leidos = read(conector, buffer, TAM_BUF)) > 0) {
        if (write(STDOUT_FILENO, buffer, leidos) < 0) {   
            perror("write");
            break;
        }
    }
    if (leidos < 0)
        perror("read");

    close(conector);
    return 0;
}