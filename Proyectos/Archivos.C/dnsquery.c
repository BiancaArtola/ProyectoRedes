#include <stdio.h>
#include "consultas.h"
#include "ayuda.h"

void *evaluarIngreso(char cadena[]){	
	char delimitador[]=" ";
	char *puntero= strtok(cadena, delimitador);

	if (strcmp(puntero, "query")==0){
		puntero= strtok(NULL, delimitador);
		
		if (strcmp(puntero, "consulta")==0)
			evaluarConsulta(puntero);	
			
		else if (strcmp(puntero, "-h")==0)	
			mensajeAyuda();		
		else
			printf("comando no encontrado. \n");
	}
	else 
		printf("comando no encontrado. \n");
	
}

int main (int argc, char* argv[])
{ 
	char cadena[MAX_STRING]="";
	printf ("Ingrese su consulta: \n");
	scanf("%[^\n]s", &cadena);

	evaluarIngreso(cadena);
	
return 0;
}
