```markdown
# Productor-Consumidor con buffer limitado (Jbaci)

Ejercicio clásico de productor-consumidor con un único productor, un único
consumidor y un buffer circular de tamaño `N = 4`, escrito para **Jbaci**
(la versión Java del entorno BACI de simulación de concurrencia).

## Qué hace

- El **productor** genera `ITEMS = 8` ítems y los va depositando en el buffer
  siempre que haya huecos libres.
- El **consumidor** los va retirando en el mismo orden en que se producen.
- La sincronización se resuelve con tres semáforos:
  - `vacios`  → huecos disponibles en el buffer (arranca en `N`).
  - `llenos`  → ítems disponibles en el buffer (arranca en `0`).
  - `mutex`   → exclusión mutua para la sección crítica.
- Además de la lógica, el programa **dibuja la escena**: cuatro huecos
  centrales (rectángulos), una bolita azul arriba (productor) y una bolita
  amarilla abajo (consumidor). Cuando el productor coloca un ítem, su bolita
  viaja hasta el hueco y este se pinta de **verde**; cuando el consumidor lo
  retira, su bolita viaja hasta el hueco y este vuelve a **rojo**.

## Requisitos

- **Jbaci** (BACI para Java).
- **Probado en un ordenador con Windows.**
- El archivo **`gdefs.cm`** debe estar **en la misma carpeta** que este código.
  Sin él no compila: es donde viven las definiciones de las primitivas
  gráficas (`create`, `moveto`, `changecolor`) 

## Cómo ejecutarlo

1. Coloca en la misma carpeta:
   - el archivo `.cm` de este ejercicio,
   - `gdefs.cm`.
2. Abre Jbaci y carga el `.cm` del ejercicio.
3. Compila y ejecuta desde Jbaci.
4. Verás en consola la traza de producción/consumo y en la ventana gráfica
   cómo se mueven las bolitas y cambian de color los huecos.

## Salida esperada (resumen)

```
[Main] Inicializando indices y semaforos...
[Main] Dibujando escena...
[Main] Lanzando productor y consumidor...
[Productor] Preparando item 1 (esperando hueco)...
Productor produce 100  (en buffer: 1)
[Consumidor] Preparando consumo 1 (esperando item)...
    Consumidor consume 100  (en buffer: 0)
...
Fin: todos los items producidos y consumidos.
```

El orden exacto entre líneas del productor y del consumidor puede variar
entre ejecuciones (eso es justamente lo que ilustra el ejercicio), pero
siempre se producen y consumen los 8 ítems y se termina con el mensaje
`Fin: todos los items producidos y consumidos.`
```