#include <stdio.h>
#include "../Archivos.H/ayuda.h"

/**
 * Lee el mensaje de ayuda de un archivo y lo muestra por pantalla
 * */
void mensajeAyuda(){
	FILE *archivo;
	char caracter;
	
	archivo= fopen("../Archivos.TXT/ayuda.txt","rt");
	
	while ((caracter=fgetc(archivo))!= EOF)		
		printf ("%c", caracter);
	
	printf ("\n");
	fclose(archivo);	
}
