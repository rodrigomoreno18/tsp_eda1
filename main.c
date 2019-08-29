/*
  Rodrigo Moreno
  LCC - Estructuras de Datos y Algoritmos | 2019
  rmoreno.unr@gmail.com
  github.com/rodrigomoreno18/tsp_eda1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TSP.h"


/* MAIN */
int main(int argc, char ** argv){
  if (argc != 3) {
    printf("Pasar como argumentos obligatorios el nombre del archivo de entrada y el de salida.\n");
    return 1;
  }

  // Armar el mapa a partir de archivo
  printf("Cargando archivo %s...\n", argv[1]);
  Mapa * mapa = cargar_archivo(argv[1]);
  if (!mapa) {
    printf("Error al generar el mapa.\n");
    return 0;
  }

  // Obtener el menor camino
  Camino camino = menor_camino(mapa); 

  // Guardar menor camino en archivo
  printf("Guardando recorrido optimo en '%s'...\n", argv[2]);
  guardar_archivo(argv[2], mapa, camino);

  // Mostrar el camino por pantalla para rapido interpretado (en indices)
  int i;
  printf("\nMenor camino (Final)\n");
  for (i = 0; i < mapa->cantidad_ciudades; ++i)
    printf("-> %d ", camino.recorrido[i] + 1);
  printf("-> 1\nPeso: %d\n", camino.peso_total);

  // Limpiar el mapa
  limpiar_mapa(mapa);

  // Limpiar el menor camino
  free(camino.recorrido);

  return 0;
}