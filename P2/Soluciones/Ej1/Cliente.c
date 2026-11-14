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

// comprueba que nos pasen la IP
int validar_argumentos(int argc, char *argv[])
{
    fprintf(stderr, "[Cliente] Validando argumentos...\n");
    if (argc == 2) {
        fprintf(stderr, "[Cliente] Argumentos correctos.\n");
        return 0;
    }
    fprintf(stderr, "Uso: %s IP_Servidor\n", argv[0]);
    return -1;
}

// crea el socket o muere
int crear_socket_o_salir()
{
    fprintf(stderr, "[Cliente] Creando socket...\n");
    int conector = socket(AF_INET, SOCK_STREAM, 0);
    if (conector < 0) {
        perror("socket");
        exit(1);
    }
    fprintf(stderr, "[Cliente] Socket creado (fd=%d).\n", conector);
    return conector;
}

// rellena la sockaddr_in del servidor
void construir_direccion(struct sockaddr_in *dir, const char *ip)
{
    fprintf(stderr, "[Cliente] Construyendo direccion del servidor (%s:%d)...\n", ip, PUERTO);
    memset(dir, 0, sizeof(*dir));
    dir->sin_family      = AF_INET;
    dir->sin_addr.s_addr = inet_addr(ip);
    dir->sin_port        = htons(PUERTO);
}

// mira si la IP parece ser valida
int es_ip_valida(const struct sockaddr_in *dir)
{
    fprintf(stderr, "[Cliente] Verificando IP...\n");
    if (dir->sin_addr.s_addr == INADDR_NONE) {
        fprintf(stderr, "[Cliente] IP invalida.\n");
        return 0;
    }
    fprintf(stderr, "[Cliente] IP valida.\n");
    return 1;
}

// aborta si la IP no vale
void salir_ip_invalida(int conector, const char *ip)
{
    fprintf(stderr, "IP no valida: %s\n", ip);
    close(conector);
    exit(1);
}

// conecta con el servidor
void conectar_o_salir(int conector, const struct sockaddr_in *dir)
{
    fprintf(stderr, "[Cliente] Conectando al servidor...\n");
    if (connect(conector, (const struct sockaddr *)dir, sizeof(*dir)) < 0) {
        perror("connect");
        close(conector);
        exit(1);
    }
    fprintf(stderr, "[Cliente] Conexion establecida.\n");
}

// escribe la salida
int escribir_salida(const char *buf, ssize_t n) {
    
    if (printf("%.*s", (int)n, buf) < 0) {
        perror("printf");
        return -1;
    }
    
    fflush(stdout); 
    return 0;
}


// bucle de lectura -> stdout
void reenviar_datos(int conector)
{
    char buffer[TAM_BUF];
    ssize_t leidos;
    fprintf(stderr, "[Cliente] Recibiendo datos del servidor...\n");
    while ((leidos = read(conector, buffer, TAM_BUF)) > 0)
        if (escribir_salida(buffer, leidos) < 0) break;
    if (leidos < 0) perror("read");
    fprintf(stderr, "[Cliente] Recepcion finalizada.\n");
}

// punto de entrada del cliente
int main(int argc, char *argv[])
{
    fprintf(stderr, "[Cliente] Iniciando cliente...\n");
    if (validar_argumentos(argc, argv) < 0) exit(1);
    int conector = crear_socket_o_salir();
    struct sockaddr_in dir;
    construir_direccion(&dir, argv[1]);
    if (!es_ip_valida(&dir)) salir_ip_invalida(conector, argv[1]);
    conectar_o_salir(conector, &dir);
    reenviar_datos(conector);
    fprintf(stderr, "[Cliente] Cerrando socket y saliendo.\n");
    close(conector);
    return 0;
}