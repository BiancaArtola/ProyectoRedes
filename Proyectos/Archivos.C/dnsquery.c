#include <stdio.h>
#include "consultas.h"
#include "ayuda.h"

void evaluarIngreso(char* argv[], int argc){
	if (strcmp(argv[1], "query")==0){
		evaluarParametros(argv, argc);
	}
	else
		mensajeAyuda();
	
}

int main (int argc, char* argv[]){ 
	if (argc > 6)
		mensajeAyuda();
	else{
		evaluarIngreso(argv, argc);
	}
	return 0;
}
