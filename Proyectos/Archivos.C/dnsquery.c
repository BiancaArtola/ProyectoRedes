#include <stdio.h>
#include "../Archivos.H/consultas.h"
#include "../Archivos.H/ayuda.h"

int main (int argc, char* argv[]){ 
	int mostrarayuda = evaluarIngreso(argv, argc);	
	
	if (mostrarayuda == -1)
		mensajeAyuda();
	return 0;
}
