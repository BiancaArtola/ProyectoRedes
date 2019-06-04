#include <stdio.h>
#include "../Archivos.H/consultas.h"
#include "../Archivos.H/ayuda.h"

/**
 * Metodo encargado de iniciar ejecucion del programa. Muestra mensaje de ayuda en caso de que se registre un error.
 * Parametros:
 * * argc: contiene el número de argumentos que se han introducido
 * * argv: array de punteros a caracteres
 **/
int main (int argc, char* argv[]){ 
	int mostrarayuda = evaluarIngreso(argv, argc);	
	
	if (mostrarayuda == -1)
		mensajeAyuda();
		
	return 0;
}
