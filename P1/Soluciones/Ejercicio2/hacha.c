#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

// Validar argumentos de línea de comandos 
void parse(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: hacha <archivo> <tamaño>\n");
        exit(1);
    }
}

// Obtener el tamaño del archivo con stat() 
int obtener_tamanyo_archivo(const char *nombre_archivo) {
    struct stat info_archivo;
    if (stat(nombre_archivo, &info_archivo) < 0) {
        perror("Error en stat");
        exit(1);
    }
    return info_archivo.st_size;
}

// Abrir el archivo original 
int abrir_archivo_origen(const char *nombre_archivo) {
    int fd_in = open(nombre_archivo, O_RDONLY);
    if (fd_in < 0) {
        perror("Error al abrir archivo original");
        exit(1);
    }
    return fd_in;
}

//Calcular número de fragmentos necesarios 
int calcular_num_trozos(int tamanyo_total, int tamanyo_trozo) {
    int num_trozos = tamanyo_total / tamanyo_trozo;
    if (tamanyo_total % tamanyo_trozo != 0) {
        num_trozos++;
    }
    return num_trozos;
}

//  Lógica del proceso hijo
void ejecutar_hijo(int fd_pipe_read, const char *nombre_archivo, int num_trozo) {
    // Formatear el nombre del fragmento (.h00, .h01...)
    char nombre_destino[256];
    sprintf(nombre_destino, "%s.h%02d", nombre_archivo, num_trozo);

    // Crear archivo de salida
    int fd_out = creat(nombre_destino, 0666);
    if (fd_out < 0) {
        perror("Error en creat");
        exit(1);
    }

    // Leer de la tubería y escribir en el archivo destino
    char buffer[1024];
    int leidos;
    while ((leidos = read(fd_pipe_read, buffer, sizeof(buffer))) > 0) {
        write(fd_out, buffer, leidos);
    }

    // Cerrar descriptores y finalizar proceso
    close(fd_out);
    close(fd_pipe_read);
    exit(0);
}

// Lógica del proceso padre
void ejecutar_padre(int fd_pipe_write, int fd_in, int num_trozo, int num_trozos, int tamanyo_trozo, int tamanyo_total) {
    // Calcular cuántos bytes enviar en este trozo en concreto
    int bytes_a_enviar = tamanyo_trozo;
    if (num_trozo == num_trozos - 1 && (tamanyo_total % tamanyo_trozo) != 0) {
        bytes_a_enviar = tamanyo_total % tamanyo_trozo;
    }

    // Leer del archivo origen y volcar en la tubería
    char buffer[1024]; //Tamaño de bytes que envian como máximo por la tubería
    int bytes_enviados = 0;

    // Escritura en el pipe
    while (bytes_enviados < bytes_a_enviar) {
        int a_leer = sizeof(buffer);
        if (bytes_a_enviar - bytes_enviados < sizeof(buffer)) {
            a_leer = bytes_a_enviar - bytes_enviados;
        }

        int leidos = read(fd_in, buffer, a_leer);
        if (leidos > 0) {
            write(fd_pipe_write, buffer, leidos);
            bytes_enviados += leidos;
        } else {
            break;
        }
    }

    // Cerrar tubería para enviar EOF al hijo
    close(fd_pipe_write);
}

// Bucle concurrente de creación de procesos y tuberías 
void crear_fragmentos_concurrentes(int fd_in, const char *nombre_archivo, int num_trozos, int tamanyo_trozo, int tamanyo_total) {
    for (int i = 0; i < num_trozos; i++) {
        int fd_pipe[2];

        if (pipe(fd_pipe) < 0) {
            perror("Error en pipe");
            exit(1);
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Error en fork");
            exit(1);
        } 
        else if (pid == 0) {
            close(fd_pipe[1]); // El hijo no escribe en la tubería
            ejecutar_hijo(fd_pipe[0], nombre_archivo, i);
        } 
        else {
            close(fd_pipe[0]); // El padre no lee de la tubería
            ejecutar_padre(fd_pipe[1], fd_in, i, num_trozos, tamanyo_trozo, tamanyo_total);
        }
    }
}

// Recoger todos los procesos hijos ---
void esperar_hijos(int num_hijos) {
    for (int i = 0; i < num_hijos; i++) {
        wait(NULL);
    }
}


int main(int argc, char *argv[]) {
    // Validar argumentos
    parse(argc, argv);

    // Obtener parámetros de entrada
    char *nombre_archivo = argv[1];
    int tamanyo_trozo = atoi(argv[2]);

    // Preparar recursos del archivo
    int tamanyo_total = obtener_tamanyo_archivo(nombre_archivo);
    int fd_in = abrir_archivo_origen(nombre_archivo);
    int num_trozos = calcular_num_trozos(tamanyo_total, tamanyo_trozo);

    // Crear los fragmentos concurrentemente con procesos e IPC
    crear_fragmentos_concurrentes(fd_in, nombre_archivo, num_trozos, tamanyo_trozo, tamanyo_total);

    // Limpieza y sincronización final
    close(fd_in);
    esperar_hijos(num_trozos);

    return 0;
}