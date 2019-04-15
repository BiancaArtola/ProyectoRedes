#include <stdio.h>
#include "ayuda.h"

void mensajeAyuda()
{
	FILE *archivo;
	char caracter;
	
	archivo= fopen("ayuda.txt","rt");
	
	while ((caracter=fgetc(archivo))!= EOF)
	
		printf ("%c", caracter);
	
	printf ("\n");
	fclose(archivo);
	
}
