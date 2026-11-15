#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO  9999
#define COLA    5
#define TAM_BUF 1024
#define ARCHIVO "Google.html"



// crea el socket o muere
int crear_socket_o_salir()
{
    fprintf(stderr, "[Servidor] Creando socket...\n");
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(1);
    }
    fprintf(stderr, "[Servidor] Socket creado (fd=%d).\n", fd);
    return fd;
}

// rellena la sockaddr_in del servidor
void construir_direccion(struct sockaddr_in *dir)
{
    fprintf(stderr, "[Servidor] Construyendo direccion (0.0.0.0:%d)...\n", PUERTO);
    memset(dir, 0, sizeof(*dir));
    dir->sin_family      = AF_INET;
    dir->sin_addr.s_addr = htonl(INADDR_ANY);
    dir->sin_port        = htons(PUERTO);
}

// bind o fuera
void bind_o_salir(int conector, const struct sockaddr_in *dir)
{
    fprintf(stderr, "[Servidor] Haciendo bind...\n");
    if (bind(conector, (const struct sockaddr *)dir, sizeof(*dir)) < 0) {
        perror("bind");
        close(conector);
        exit(1);
    }
    fprintf(stderr, "[Servidor] Bind correcto.\n");
}

// listen o fuera
void listen_o_salir(int conector)
{
    fprintf(stderr, "[Servidor] Poniendo a escuchar (cola=%d)...\n", COLA);
    if (listen(conector, COLA) < 0) {
        perror("listen");
        close(conector);
        exit(1);
    }
    fprintf(stderr, "[Servidor] Escuchando.\n");
}

// deja el socket listo para aceptar
void preparar_servidor(int conector)
{
    struct sockaddr_in dir;
    construir_direccion(&dir);
    bind_o_salir(conector, &dir);
    listen_o_salir(conector);
}

// acepta uno; si falla devuelve -1
int aceptar_conexion(int conector, struct sockaddr_in *dir_cliente)
{
    fprintf(stderr, "[Servidor] Esperando cliente...\n");
    socklen_t long_cliente = sizeof(*dir_cliente);
    int conexion = accept(conector, (struct sockaddr *)dir_cliente, &long_cliente);
    if (conexion < 0) perror("accept");
    else fprintf(stderr, "[Servidor] Cliente aceptado (fd=%d).\n", conexion);
    return conexion;
}

// fork con log
pid_t fork_o_error()
{
    fprintf(stderr, "[Servidor] Creando proceso hijo...\n");
    pid_t pid = fork();
    if (pid < 0) perror("fork");
    return pid;
}

// abre el archivo a mandar
int abrir_archivo_o_salir(int conexion)
{
    fprintf(stderr, "[Hijo] Abriendo archivo %s...\n", ARCHIVO);
    int fd = open(ARCHIVO, O_RDONLY);
    if (fd < 0) {
        perror("open " ARCHIVO);
        close(conexion);
        exit(1);
    }
    fprintf(stderr, "[Hijo] Archivo abierto (fd=%d).\n", fd);
    return fd;
}

// limpia y sale si falla un write
void error_write(int conexion, int fd)
{
    perror("write");
    close(fd);
    close(conexion);
    exit(1);
}

// escribe n bytes enteros al socket
void escribir_todo_o_salir(int conexion, int fd, const char *buf, ssize_t n)
{
    ssize_t total = 0;
    while (total < n) {
        ssize_t escritos = write(conexion, buf + total, n - total);
        if (escritos < 0) error_write(conexion, fd);
        total += escritos;
    }
}

// lee el archivo y lo manda por el socket
void enviar_archivo(int conexion, int fd)
{
    char buffer[TAM_BUF];
    ssize_t leidos;
    fprintf(stderr, "[Hijo] Enviando archivo al cliente...\n");
    while ((leidos = read(fd, buffer, TAM_BUF)) > 0)
        escribir_todo_o_salir(conexion, fd, buffer, leidos);
    fprintf(stderr, "[Hijo] Archivo enviado.\n");
}

// atiende a un cliente desde el hijo
void atender_cliente(int conector, int conexion)
{
    fprintf(stderr, "[Hijo] Cerrando socket del servidor en el hijo...\n");
    close(conector);
    int fd = abrir_archivo_o_salir(conexion);
    enviar_archivo(conexion, fd);
    close(fd);
    close(conexion);
    fprintf(stderr, "[Hijo] Terminando.\n");
    exit(0);
}

// hace fork; el hijo atiende y el padre cierra
void atender_si_es_hijo(int conector, int conexion)
{
    pid_t pid = fork_o_error();
    if (pid == 0)
        atender_cliente(conector, conexion);
    fprintf(stderr, "[Servidor] Cerrando conexion en el padre.\n");
    close(conexion);
}

// bucle infinito aceptando clientes
void bucle_principal(int conector)
{
    while (1) {
        struct sockaddr_in dir_cliente;
        int conexion = aceptar_conexion(conector, &dir_cliente);
        if (conexion < 0) continue;
        printf("Conexion de %s\n", inet_ntoa(dir_cliente.sin_addr));
        fflush(stdout);
        atender_si_es_hijo(conector, conexion);
    }
}

// punto de entrada del servidor
int main()
{
    fprintf(stderr, "[Servidor] Iniciando servidor...\n");
    signal(SIGCHLD,SIG_IGN);
    int conector = crear_socket_o_salir();
    preparar_servidor(conector);
    printf("Servidor escuchando en el puerto %d...\n", PUERTO);
    fflush(stdout);
    bucle_principal(conector);
    close(conector);
    return 0;
}