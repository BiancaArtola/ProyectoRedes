#include "consultas.h"
#include "dns.h"
#include <stdio.h>
#define TIPOCONSULTA_DEFECTO "-a"
#define TIPORESOLUCION_DEFECTO "-r"
#define PUERTO_DEFECTO 80

struct parametrosConsulta {
	char* servidor;
	char* puerto;
	char* tipoConsulta;
	char* tipoResolucionConsulta;
}parametros;

int leerServidor(char* argv[], int cantParametros){
	//Retorna 0 si el servidor fue asignado. 1 en caso contrario.
	char* parametroServidor;
	if (cantParametros > 3){
		parametroServidor = argv[3];
		if (parametroServidor[0] == '@'){
			parametros.servidor = parametroServidor+1;
			asignarPuerto(parametroServidor);
			return 0;
		}
	}
	/*Si el usuario no ingreso servidor (ni puerto) se le asigna los valores por defecto */
	parametros.servidor = "";
	parametros.puerto = "";
	return 1;
}

void asignarPuerto(char parametroServidor[]){
	//este es el metodo mas pedorro del planeta tierra. parezco de IPOO
	int cantCaracteres = strlen(parametroServidor);
	int encontreCorchete = 0;
	int i = 0;
	int posicionComienzoPuerto=0;
	for (i = 0 ; i < cantCaracteres && encontreCorchete ==0; i++){
		if (parametroServidor[i] == '[') {
			encontreCorchete == 1;
			posicionComienzoPuerto = i;			
		}
	}
	printf("encontre %i", encontreCorchete);
	if (encontreCorchete == 1){
		if (parametroServidor[posicionComienzoPuerto+1] == ':'){
			i = 0;
			posicionComienzoPuerto+=2;;				
			while (parametroServidor[posicionComienzoPuerto]!=']' && 
					posicionComienzoPuerto<cantCaracteres-1){
				parametros.puerto[i]=parametroServidor[posicionComienzoPuerto];		
				posicionComienzoPuerto++;
				i++;
			}
			if (posicionComienzoPuerto == cantCaracteres-1)	{
				printf("errrrorrrrrr");
				parametros.puerto = PUERTO_DEFECTO;
			}
			
		}
		printf("Hay un error en el ingreso del servidor y/o puerto. Revise su entrada");
	}
}

int evaluarParametrosFinales(char* parametrosFinales[], int cantParametros, int comienzoParametros){
	int i;
	parametros.tipoConsulta = "";
	parametros.tipoResolucionConsulta = "";
	for (i = comienzoParametros; i<cantParametros ; i++){
		if ((strcmp(parametrosFinales[i], "-a") == 0) || 
			(strcmp(parametrosFinales[i], "-mx") == 0) ||
			(strcmp(parametrosFinales[i], "-loc") == 0)){
				if (parametros.tipoConsulta == "")
					parametros.tipoConsulta = parametrosFinales[i];
				else
					return 0;
			}
		else if ((strcmp(parametrosFinales[i], "-r") == 0) ||	
				(strcmp(parametrosFinales[i], "-t") == 0)) {
					if (parametros.tipoResolucionConsulta == "")
						parametros.tipoResolucionConsulta = parametrosFinales[i];
					else
						return 0;
			}
		else
			return 0;
	}
	return 1;
}

void evaluarParametros(char* argv[], int argc){
	int servidorAsignado = leerServidor(argv, argc);
	printf("servidor: %s cant parametros: %i puerto: %c servAsignado: %i", 
	parametros.servidor, argc, parametros.puerto, servidorAsignado);
	int parametrosAsignados=0;
	/*Si el usuario ingreso un servidor (sirve para ver q hay un parametro mas)
	e ingreso algun otro parametro (osea argc>4) evaluo los q siguen.
	* En el caso de abajo, el usuario no ingreso servidor entonces hay un parametro menos. */
	if (servidorAsignado == 0 && argc>4)
		parametrosAsignados = evaluarParametrosFinales(argv, argc, 4);
	else if (servidorAsignado==1 && argc>3)
		parametrosAsignados = evaluarParametrosFinales(argv, argc, 3);
	else if ((servidorAsignado==1 && argc==3) || (servidorAsignado==0 && argc==4))
		parametrosAsignados = 1;
		
	if (parametrosAsignados == 0)
		printf("Los parametros ingresados son INCORRECTOS. Por favor chequee su entrada.");
	else {
		asignarServidorDNS(parametros.servidor);
		iniciarDNS(argv[2]);
	}
}
