# Ejercicio 2 — `hacha.c` (división de archivos con tuberías)

## Enunciado

Programa `hacha.c` que divide un archivo en varios trozos del tamaño
indicado, nombrados con la extensión `h00`, `h01`, `h02`, …:

```sh
$ ./hacha <archivo> <tamaño>
$ ./hacha at_madrid.mp3 50000
$ ls
at_madrid.mp3  at_madrid.mp3.h00  at_madrid.mp3.h01  at_madrid.mp3.h02
```

Consideraciones del enunciado:

- El proceso padre (`hacha`) crea **tantos hijos como fragmentos** y les
  envía la información **mediante tuberías**; los hijos son los que crean
  los archivos de destino y escriben en ellos.
- Las entradas y salidas a archivos se hacen con llamadas al sistema
  (`open`, `read`, `write`, `creat`, `stat`…), sin `printf`/`scanf` para
  los datos.
- Los hijos pueden lanzarse secuencial o concurrentemente (a elección del
  alumno): aquí se lanzan **concurrentemente**, un hijo por fragmento.

## Compilación y ejecución

```sh
gcc -Wall -o hacha hacha.c
./hacha datos.bin 4096
ls datos.bin.h*
# datos.bin.h00  datos.bin.h01  datos.bin.h02  datos.bin.h03
```

Uso: `hacha <archivo> <tamaño>`; si los argumentos no son correctos,
muestra el uso y termina con `exit(1)`.

## Cómo funciona

1. `main()` valida los argumentos (`parse`), obtiene el tamaño del archivo
   con `stat()` y lo abre en solo lectura con `open()`.
2. `calcular_num_trozos()` calcula cuántos fragmentos hacen falta
   (redondeo hacia arriba si el tamaño no es múltiplo).
3. `crear_fragmentos_concurrentes()` recorre los fragmentos y, para cada
   uno:
   - crea una tubería con `pipe()`;
   - hace `fork()`:
     - **hijo** (`ejecutar_hijo`): cierra el extremo de escritura, crea su
       archivo `origen.hNN` con `creat()` y vuelca en él todo lo que llegue
       por la tubería hasta el EOF (cuando el padre cierra su extremo);
     - **padre** (`ejecutar_padre`): cierra el extremo de lectura, calcula
       cuántos bytes corresponden a ese fragmento (el último puede ser
       menor que el tamaño indicado), lee del archivo origen y escribe en
       la tubería; al terminar cierra su extremo de escritura, lo que
       genera el EOF que hace salir al `read()` del hijo.
4. El descriptor del archivo origen es compartido por todos los procesos
   (se hereda en el `fork()`), de modo que cada padre lee justo el trozo
   que le corresponde de forma secuencial.
5. `esperar_hijos()` recoge a los `num_trozos` hijos con `wait(NULL)` antes
   de terminar.

## Estructura del código

| Función | Rol |
|---|---|
| `parse` | Valida `argc == 3` |
| `obtener_tamanyo_archivo` | `stat()` → tamaño total |
| `abrir_archivo_origen` | `open()` en solo lectura |
| `calcular_num_trozos` | Número de fragmentos |
| `crear_fragmentos_concurrentes` | Bucle `pipe()` + `fork()` |
| `ejecutar_padre` | Lee origen y escribe en la tubería |
| `ejecutar_hijo` | Lee la tubería y escribe el archivo `hNN` |
| `esperar_hijos` | Sincronización final con `wait()` |

## Llamadas al sistema empleadas

`stat`, `open`, `read`, `write`, `creat`, `close`, `pipe`, `fork`, `wait`.
