# Traveling Salesman Problem

Solucion al TSP con conexiones simetricas entre ciudades

Utilizando una forma de backtracking con una matriz de adyacencias triangular superior.

#### __*Compilado:*__
```
gcc -c --std=c99 TSP.c -o TSP.o
gcc --std=c99 main.c TSP.o -o main
```

##### __*Opcionalmente, usando el Makefile incluido*__
```
make
make clean (para limpieza de objetos)
```

#### __*Ejecutado:*__
```
./main <input_file> <output_file>
```

**Notas:**
 - El programa espera que el archivo de entrada tenga terminaciones de lineas UNIX (\n) en lugar de DOS (\r\n).
 - La última línea del archivo de entrada debe ser el último camino. En caso de haber una línea vacía, no funciona correctamente
