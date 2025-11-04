***Breve descripción***

Conjunto de prácticas del curso de Sistemas Operativos donde se desarrollan programas en C para comprender llamadas al sistema, programación multi-hilo, control de procesos y gestión de colas.

***Qué hace el proyecto***

Práctica 1: Programa para crear archivos con permisos personalizados y otro para combinar y ordenar listas de alumnos desde varios archivos.

Práctica 2: (No incluida explícitamente, se combina con otras)

Práctica 3: Simulación concurrente de gestión de procesos en una fábrica con múltiples cintas transportadoras utilizando hilos pthread y semáforos, implementando productores-consumidores con sincronización segura.

Tecnologías usadas
- Lenguaje C
- Librerías POSIX pthread (hilos)
- Semáforos (semt)
- Funciones del sistema Linux como open, close, chmod, qsort
- Manejo de archivos y estructuras en C

***Cómo se ejecuta***

- Cada práctica contiene archivos C específicos que se compilan con gcc.
- Para práctica 1 y 3, se ejecutan con argumentos indicados (nombres de archivos o fichero de configuración).
- Ejemplo para práctica 1 (crear archivo):
    ./crear archivo.txt 777
- Ejemplo para práctica 3:
    ./programa fabrica.txt
donde fabrica.txt contiene la configuración de cintas y productos.

***Ejemplo de salida***

- Mensajes en consola que indican éxito o error en operaciones de creación de archivos, combinación y ordenación de alumnos.
- En multi-hilo, mensajes de inicio y finalización de procesos productores y consumidores, confirmando que los productos han sido procesados y la aplicación ha terminado correctamente.

