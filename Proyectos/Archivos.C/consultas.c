#include "consultas.h"
#include <stdio.h>
struct parametrosConsulta {
	char* parametro1;
	char* parametro2;
	char* parametro3;
};

void *evaluarConsulta(char *puntero){
	int cantidad = -1 ;
	char* delimitador = " ";
	while (puntero != NULL ){
		puntero= strtok(NULL, delimitador);
		cantidad++;
	}
	if (cantidad > 3)
		printf("mensaje de error");
}
