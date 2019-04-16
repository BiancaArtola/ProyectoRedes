#include <stdio.h>
#include "consultas.h"
#include "ayuda.h"

void *evaluarIngreso(char cadena[], int cantPalabras){	
	char* primeraPalabra = "";
	primeraPalabra = agarrar(cadena, 0);
	
	if (strcmp(primeraPalabra, "query")==0){
		char* segundaPalabra = "";
		segundaPalabra = agarrar(cadena, 6);
		if (strcmp(segundaPalabra, "consulta")==0){
			printf("yes");
		}else
			mensajeAyuda();
	}
	else 
		mensajeAyuda();
}

int main (int argc, char* argv[]){ 
	char cadena[MAX_STRING]="";
	printf ("Ingrese su consulta: \n");
	scanf("%[^\n]s", &cadena); 
	int cantPalabras = contar(cadena);
	if (cantPalabras > 5) 
		mensajeAyuda();
	else
		evaluarIngreso(cadena, cantPalabras);	
return 0;
}
