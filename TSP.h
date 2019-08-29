#ifndef __TSP_H__
#define __TSP_H__

/*
  Mapa contiene la informacion del
  grafo de ciudades. Guarda las ciudades
  en un array y a partir de esto,
  el programa maneja las ciudades como indices
  para reducir costo de procesamiento

  La matriz pesos es la matriz de adyacencias
  ponderada, y es triangular superior para
  no almacenar el doble de memoria. 0 implica
  que las ciudades no estan conectadas

  (ejemplo de pesos para 5 ciudades)
  pesos = [ 4 5 0 1 ]
              2 6 0
                3 5
                  2
*/
typedef struct {
  char ** ciudades;
  unsigned int cantidad_ciudades;
  unsigned int ** pesos;
} Mapa;

/*
  Camino lleva la cuenta del peso total
  y un array de indices (ciudades) que se
  va sobreescribiendo de forma regresiva
  segun se van recorriendo caminos.
*/
typedef struct {
  unsigned int * recorrido;
  unsigned int peso_total;
} Camino;

/*
  Visitadas lleva la cuenta de las ciudades
  ya visitadas (tanto cantidad como indices).
  Los indices se almacenan en su posicion en
  la bitmask.
  Por ejemplo ciudades 2 y 5 = 0010 0100
*/
typedef struct {
  long long int bitmask;
  unsigned int cantidad_visitada;
}  Visitadas;


/*
  Headers de funciones 
*/

Mapa * cargar_archivo(char * archivo);
void guardar_archivo(char * archivo, Mapa * mapa, Camino camino);
unsigned int obtener_indice_ciudad(Mapa * mapa, char * nombre_ciudad);
unsigned int * obtener_pesos_adyacentes(Mapa * mapa, int ciudad);
void limpiar_mapa(Mapa * mapa);

Camino menor_camino(Mapa * mapa);
Camino tsp(Mapa * mapa, Camino optimo, Camino temporal, Visitadas visitadas, unsigned int actual);

Visitadas visitar(Visitadas visitadas, unsigned int ciudad);
unsigned int fue_visitada(Visitadas visitadas, unsigned int ciudad);


#endif