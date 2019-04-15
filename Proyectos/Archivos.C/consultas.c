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

int *evaluarCantidadParametros(char *puntero){
	int cantidad = -1 ;
	char* delimitador = " ";
	char* auxPuntero = &puntero;
	while (*auxPuntero != NULL ){
		*auxPuntero= strtok(NULL, delimitador);
		cantidad++;
	}
	if (cantidad > 3){
		printf("ERROR en el ingreso de la consulta.");
		return 0; //ERROR
	}
	else return cantidad; //Cantidad de parametros correcta
}

void *evaluarConsulta(char *puntero){
	printf("Puntero2: %s \n",puntero);
	int cantidadParametros = evaluarCantidadParametros(puntero);
	if (cantidadParametros > 0){
		printf("Cantidad parametros: %i \n",cantidadParametros);		
		printf("Puntero: %s \n",(puntero));
		char* delimitador = " ";
		while (puntero != NULL ){		
			puntero= strtok(NULL, delimitador);
			printf("Puntero: %s \n",(puntero));
			char* auxiliarParametro= puntero;
			printf("aux: %s \n",auxiliarParametro);
			//evaluarParametro(auxiliarParametro);
		}
	}
}
