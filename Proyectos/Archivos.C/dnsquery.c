#include <stdio.h>
#include "consultas.h"
#include "ayuda.h"

void *evaluarIngreso(char cadena[]){	
	char delimitador[]=" ";
	//Lee el primer elemento del ingreso del usuario
	char *puntero= strtok(cadena, delimitador);

	if (strcmp(puntero, "query")==0){
		//Si la primer palabra ingresada por el usuario es 'query',
		//lee la segunda palabra 
		puntero= strtok(NULL, delimitador);
		
		if (strcmp(puntero, "consulta")==0)
			evaluarConsulta(puntero);				
		else if (strcmp(puntero, "-h")==0)	
			mensajeAyuda();		
		else
			printf("ERROR! Comando no encontrado. \n");
	}
	else 
	//Si la primer palabra ingresada por el usuario no fue query,
	//muestra mensaje de error
		printf("ERROR! Comando no encontrado. \n");
	
}

int main (int argc, char* argv[])
{ 
	char cadena[MAX_STRING]="";
	printf ("Ingrese su consulta: \n");
	scanf("%[^\n]s", &cadena); 

	evaluarIngreso(cadena);
	
return 0;
}
