#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	Los indices se almacenan 
*/
typedef struct {
	long long int bitmask;
	unsigned int cantidad_visitada;
}  Visitadas;

Camino menor_camino(Mapa * mapa);
Camino procesar(Mapa * mapa, Camino optimo, Camino temporal,
									Visitadas visitadas,	unsigned int actual);
Mapa * cargar_archivo(char * archivo);
void guardar_archivo(char * archivo, Mapa * mapa, Camino camino);
unsigned int obtener_indice_ciudad(Mapa * mapa, char * nombre_ciudad);
Visitadas visitar(Visitadas visitadas, unsigned int ciudad);
unsigned int fue_visitada(Visitadas visitadas, unsigned int ciudad);
unsigned int * obtener_pesos_adyacentes(Mapa * mapa, int ciudad);
void limpiar_mapa(Mapa * mapa);

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

Camino menor_camino(Mapa * mapa) {
	Camino optimo;
	optimo.recorrido = NULL;
	optimo.peso_total = -1;		// Maximo entero sin signo

	Camino temporal;
	temporal.recorrido = malloc(sizeof(unsigned int) * mapa->cantidad_ciudades);
	temporal.recorrido[0] = 0;
	temporal.peso_total = 0;

	Visitadas visitadas;
	visitadas.bitmask = 0;
	visitadas.cantidad_visitada = 0;

	optimo = procesar(mapa, optimo, temporal, visitadas, 0);

	free(temporal.recorrido);

	return optimo;
}

Camino procesar(Mapa * mapa, Camino optimo, Camino temporal, Visitadas visitadas, unsigned int actual) {
	
	Camino nuevo = temporal;

	unsigned int * pesos_adyacentes;
	unsigned int a_calcular;
	unsigned int i;
	unsigned int peso_guardado = nuevo.peso_total;	// Para frenar incrementado por iteracion

	visitadas = visitar(visitadas, actual);
	pesos_adyacentes = obtener_pesos_adyacentes(mapa, actual);

	if (visitadas.cantidad_visitada == mapa->cantidad_ciudades) {
		// Si todas fueron visitadas y la primera esta conectada, dar
		// por finalizado y devolver el menor entre el optimo y el nuevo
		if (pesos_adyacentes[0] != 0) {
			nuevo.peso_total += pesos_adyacentes[0];

			free(pesos_adyacentes);
			
			/* Debug
			printf("\nCamino terminal : ");
			for (i = 0 ; i < visitadas.cantidad_visitada; ++i)
				printf("-> %u", nuevo.recorrido[i]);
			printf("\nPeso nuevo: %u | Peso optimo: %u\n", nuevo.peso_total, optimo.peso_total);
			*/

			// Si es el primer camino o es de menor peso al optimo, guardarlo
			if (!(optimo.recorrido) || nuevo.peso_total < optimo.peso_total) {
				/* Debug
				printf("Encontrado un mejor\n");
				*/

				if (!(optimo.recorrido))	// En caso que haya sido el primero
					optimo.recorrido = malloc(sizeof(unsigned int) * mapa->cantidad_ciudades);

				// Crear una copia del recorrido optimo
				for (i = 0; i < visitadas.cantidad_visitada; ++i)
					optimo.recorrido[i] = nuevo.recorrido[i];
				optimo.peso_total = nuevo.peso_total;
			}

			// En caso que el nuevo no sea menor al optimo
			return optimo;
		} else {
			free(pesos_adyacentes);
			return optimo;
		}
	} else {
		for (i = 1; i < mapa->cantidad_ciudades; ++i) {
			a_calcular = i - (i >= actual ? 1 : 0);

			if (fue_visitada(visitadas, i) || pesos_adyacentes[a_calcular] == 0)
				continue;

			if (nuevo.peso_total + pesos_adyacentes[a_calcular] >= optimo.peso_total) {
				/* Debug
				printf("\nLa siguiente iteracion no tiene efecto\n");
				*/
				continue;
			}

			nuevo.peso_total = peso_guardado;

			nuevo.peso_total += pesos_adyacentes[a_calcular];
			nuevo.recorrido[visitadas.cantidad_visitada] = i;
			optimo = procesar(mapa, optimo, nuevo, visitadas, i);
		}
	}

	free(pesos_adyacentes);
	return optimo;
}

Mapa * cargar_archivo(char * archivo){
	FILE * fp;
	fp = fopen(archivo, "r");

	if (!fp) {
		printf("Error abriendo el archivo %s.", archivo);
		return NULL;
	}

	unsigned int tope = 15;		// Para empujar el maximo de ciudades dinamicamente

	Mapa * mapa = malloc(sizeof(Mapa));
	mapa->ciudades = malloc(sizeof(char *) * tope);
	mapa->cantidad_ciudades = 0;

	// Tragar el texto "ciudades"
	char a;
	while ((a = fgetc(fp)) != '\n')
		if (a == EOF) return NULL;

	// Leer la lista de ciudades
	char buff[256];
	char terminador = ',';
	unsigned int i;
	while (terminador != '\n') {
		for (i = 0; i < 255; ++i) {
			buff[i] = fgetc(fp);

			if (buff[i] == ',' || buff[i] == '\n') {
				terminador = buff[i];

				if (mapa->cantidad_ciudades >= tope) {
					tope += 5;
					mapa->ciudades = realloc(mapa->ciudades, tope);
				}

				buff[i] = '\0';
				mapa->ciudades[mapa->cantidad_ciudades] = malloc(sizeof(char) * (i+1));
				strcpy(mapa->ciudades[mapa->cantidad_ciudades], buff);
				++(mapa->cantidad_ciudades);

				fgetc(fp);	// Tragar el espacio en blanco

				break;
			}
		}
	}

	/* Debug
	printf("Ciudades : [");
	for (i = 0; i < mapa->cantidad_ciudades; ++i) {
		printf("%s, ", mapa->ciudades[i]);
	}
	printf("]\n");
	*/

	// Inicializar la matriz triangular superior en 0
	unsigned int iteraciones = 1;		// Para crear la matriz triangular superior
	unsigned int j;		
	mapa->pesos = malloc(sizeof(unsigned int *) * (mapa->cantidad_ciudades - 1));
	for (i = 0; i < mapa->cantidad_ciudades - 1; ++i) {
		mapa->pesos[i] = malloc(sizeof(unsigned int) * iteraciones);

		for (j = 0; j < iteraciones; ++j){
			mapa->pesos[i][j] = 0;
		}

		++iteraciones;
	}
	// Debugprintf("\n");
	// Tragar el texto "Costos"
	while ((a = fgetc(fp)) != '\n')
		if (a == EOF) return NULL;

	// Leer la lista de costos
	char ciudad1[256];
	unsigned int indice1 = 0;
	char ciudad2[256];
	unsigned int indice2 = 0;
	unsigned int peso = 0;

	// Leer hasta el final del archivo
	while (terminador != EOF) {
		// Leer ciudad 1
		for (i = 0; i < 255; ++i) {
			ciudad1[i] = fgetc(fp);

			if (ciudad1[i] == ',') {
				ciudad1[i] = '\0';
				break;
			}
		}

		indice1 = obtener_indice_ciudad(mapa, ciudad1);
		if (indice1 == mapa->cantidad_ciudades) {
			printf("La ciudad '%s' no existe.\n", ciudad1);
			fclose(fp);
			limpiar_mapa(mapa);
			return NULL;
		}

		// Leer ciudad 2
		for (i = 0; i < 255; ++i) {
			ciudad2[i] = fgetc(fp);

			if (ciudad2[i] == ',') {
				ciudad2[i] = '\0';
				break;
			}
		}

		indice2 = obtener_indice_ciudad(mapa, ciudad2);
		if (indice2 == mapa->cantidad_ciudades || indice2 == indice1) {
			if (indice1 == indice2)
				printf("Una ciudad no se conecta con sigo misma.\n");
			else
				printf("La ciudad '%s' no existe.\n", ciudad2);
			
			fclose(fp);
			limpiar_mapa(mapa);
			return NULL;
		}

		// Leer el costo
		fscanf(fp, "%u", &peso);
		terminador = fgetc(fp);

		if (indice1 < indice2)
			mapa->pesos[indice2 - 1][indice1] = peso;
		else
			mapa->pesos[indice1 - 1][indice2] = peso;
	}
	
	fclose(fp);

	return mapa;
}

void guardar_archivo(char * archivo, Mapa * mapa, Camino camino) {
	FILE * fp;
	fp = fopen(archivo, "w+");

	if (!fp) {
		printf("Error abriendo el archivo %s.", archivo);
		return;
	}

	unsigned int peso;
	unsigned int i;
	unsigned int indice1, indice2;

	for (i = 0; i < mapa->cantidad_ciudades; ++i) {
		indice1 = camino.recorrido[i];
		indice2 = camino.recorrido[(i + 1) % (mapa->cantidad_ciudades)];

		peso = (indice1 < indice2
			? mapa->pesos[indice2 - 1][indice1]
			: mapa->pesos[indice1 - 1][indice2]);

		if (fprintf(fp, "%s,%s,%u\n", mapa->ciudades[indice1],
																	mapa->ciudades[indice2],
																	peso) < 0) {
			printf("Ocurrio un error al escribir el archivo... Abortando.\n");
			break;
		}

	}

	fprintf(fp, "\nPeso total: %u\n", camino.peso_total);

	fclose(fp);
}

unsigned int obtener_indice_ciudad(Mapa * mapa, char * nombre_ciudad) {
	unsigned int indice;

	for (indice = 0; indice < mapa->cantidad_ciudades; ++indice)
		if (strcmp(mapa->ciudades[indice], nombre_ciudad) == 0)
			return indice;

	// En este caso estaria fuera de rango (no existe la ciudad)
	return mapa->cantidad_ciudades;
}

Visitadas visitar(Visitadas visitadas, unsigned int ciudad) {
	// Encender el bit 'ciudad' para agregarlo a la bitmask 'visitadas'
	long long int bit_ciudad = 1 << ciudad;

	Visitadas nuevas = visitadas;
	nuevas.bitmask = nuevas.bitmask | bit_ciudad;
	nuevas.cantidad_visitada += 1;

	return nuevas;
}

unsigned int fue_visitada(Visitadas visitadas, unsigned int ciudad) {
	return visitadas.bitmask & (1 << ciudad);
}

unsigned int * obtener_pesos_adyacentes(Mapa * mapa, int ciudad) {
	unsigned int * pesos = malloc(sizeof(int) * (mapa->cantidad_ciudades - 1));
	if (!pesos) return NULL;

	unsigned int indice = 0;

	unsigned int i;
	for (i = 0; i < ciudad; ++i) {
		pesos[indice] = mapa->pesos[ciudad - 1][i];
		++indice;
	}

	for (i = ciudad; i < mapa->cantidad_ciudades - 1; ++i) {
		pesos[indice] = mapa->pesos[i][ciudad];
		++indice;
	}

	return pesos;
}

void limpiar_mapa(Mapa * mapa) {
	unsigned int i;
	
	if (mapa->cantidad_ciudades > 0) {
		for (i = 0; i < mapa->cantidad_ciudades; ++i) {
			free(mapa->ciudades[i]);
		}
		free(mapa->ciudades);
	}

	if (mapa->pesos) {
		for (i = 0; i < mapa->cantidad_ciudades - 1; ++i) {
			free(mapa->pesos[i]);
		}
		free(mapa->pesos);
	}
	free(mapa);
}