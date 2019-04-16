#include "consultas.h"
#include <stdio.h>
#define SERVIDOR_DEFECTO 12
#define PUERTO_DEFECTO 80
#define TIPOCONSULTA_DEFECTO "-a"
#define TIPORESOLUCION_DEFECTO "-r"

struct parametrosConsulta {
	char* servidor;
	char* tipoConsulta;
	char* tipoResolucionConsulta;
};

int contar(char cadena[]){
	int cantidadTotal = strlen(cadena);
	int cantPalabras = 1;
	int i;
	for (i=0 ; i<cantidadTotal;i++)
		if (cadena[i]==' ') cantPalabras++;
	return cantPalabras;
}

char* agarrar(char cadena[], int comienzo){
	char palabra[20]="";
	int boolean = 0;
	int i =0;
	int total = strlen(cadena);
	int contador=0;
	for (i=comienzo;i< total && boolean==0;i++){
		if (cadena[i]!=' '){
			palabra[contador]=cadena[i];			
			contador++;
		}
		else boolean=1;		
	}
	return palabra;
}
