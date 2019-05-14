#include "../Archivos.H/consultas.h"
#include "../Archivos.H/dns.h"
#include "../Archivos.H/ayuda.h"
#include <stdio.h>

struct parametrosConsulta {
	char* servidor;
	char* puerto;
	int nroConsulta;
	int nroResolucionConsulta;
}parametros;

char* tipoConsulta;
char* tipoResolucionConsulta;

int buscarServidorYPuerto(char* parametroParaEvaluar[], int cantParametros){
	//Retorna 0 si el servidor fue asignado. 1 en caso contrario.
	char* parametroServidor=(char*)malloc(100);
	if (cantParametros > 3){
		parametroServidor = parametroParaEvaluar[3];
		if (parametroServidor[0] == '@'){
			evaluarPuerto(parametroServidor);
			return 0;
		}
	}
	/*Si el usuario no ingreso servidor (ni puerto) se le asigna los valores por defecto */
	parametros.servidor = "";
	parametros.puerto = "";
	return 1;
}

int buscarPuerto(char* parametroServidor){
	int posicionComienzoPuerto=0;
	int cantCaracteres = strlen(parametroServidor);
	int encontreCorchete = 0;
	int i = 0;
	for (i = 0 ; i < cantCaracteres && encontreCorchete ==0; i++){
		if (parametroServidor[i] == '[' && parametroServidor[i+1]==':') {
			encontreCorchete = 1;
			posicionComienzoPuerto = i;
		}
	}
	return posicionComienzoPuerto;
}

void asignarServidorConPuerto(char parametroServidor[], int posicionComienzoPuerto){
	parametros.servidor=(char*) malloc(100);
	parametros.puerto = (char*) malloc(50);

	//El puerto comienza donde termina el servidor --> se le quitan dos por '[:'
	int longitudServidor = posicionComienzoPuerto;
	parametros.puerto = parametroServidor+posicionComienzoPuerto+2;

	//Asigno en parametros.servidor el valor correspondiente ingresado por el usuario
	//Como el primer caracter ingresado es @, hacemos el +1
	strncpy(parametros.servidor, parametroServidor, posicionComienzoPuerto);
	parametros.servidor = parametros.servidor+1;
}

void asignarServidorSinPuerto(char parametroServidor[]){
	parametros.puerto="";
	parametros.servidor = parametroServidor+1;
}

void evaluarPuerto(char parametroServidor[]){
	int posicionComienzoPuerto = buscarPuerto(parametroServidor);

	if (posicionComienzoPuerto>0){ //Esto indica que hay puerto
		asignarServidorConPuerto(parametroServidor, posicionComienzoPuerto);
	}else
		asignarServidorSinPuerto(parametroServidor);
}

void asignarTipoConsultaPorDefecto(){
	if (tipoConsulta == NULL)
		tipoConsulta = TIPOCONSULTA_DEFECTO;
	if (tipoResolucionConsulta == NULL)
		tipoResolucionConsulta = TIPORESOLUCION_DEFECTO;
}

int evaluarParametrosFinales(char* parametrosFinales[], int cantParametros, int comienzoParametros){
	int i;
	tipoConsulta = NULL;
	tipoResolucionConsulta = NULL;
	for (i = comienzoParametros; i<cantParametros ; i++){
		if ((strcmp(parametrosFinales[i], "-a") == 0) ||
			(strcmp(parametrosFinales[i], "-mx") == 0) ||
			(strcmp(parametrosFinales[i], "-loc") == 0)){
				if (tipoConsulta == NULL)
					tipoConsulta = parametrosFinales[i];
				else
					return 0; //Hubo un error
		}
		else if ((strcmp(parametrosFinales[i], "-r") == 0) ||
				(strcmp(parametrosFinales[i], "-t") == 0)) {
					if (tipoResolucionConsulta == NULL)
						tipoResolucionConsulta = parametrosFinales[i];
					else
						return 0; //Hubo un error
				}
				else
					return 0; //No ingreso ningun tipo de parametro valido.
	}
	asignarTipoConsultaPorDefecto();
	return 1; //Salida exitosa
}

int evaluarOpcionesIngreso(int servidorAsignado, char* parametrosIngresados[], int cantParametros){
	/*Si el usuario ingreso un servidor (sirve para ver q hay un parametro mas)
	e ingreso algun otro parametro (osea argc>4) evaluo los q siguen.
	* En el caso de abajo, el usuario no ingreso servidor entonces hay un parametro menos. */
	if (servidorAsignado == 0 && cantParametros>4)
		return evaluarParametrosFinales(parametrosIngresados, cantParametros, 4);
	else if (servidorAsignado==1 && cantParametros>3)
		return evaluarParametrosFinales(parametrosIngresados, cantParametros, 3);
	else if ((servidorAsignado==1 && cantParametros==3) || (servidorAsignado==0 && cantParametros==4)){
		//No hay parametros de tipo consulta o tipo resolucion consulta
		asignarTipoConsultaPorDefecto();
		return 1;
	}
	else
		return 0;
}

void setPuerto(){
	int puertoAuxiliar = atoi(parametros.puerto);
	if (puertoAuxiliar == 0)
		parametros.puerto = htons(PUERTO_DEFECTO); 
	else
		parametros.puerto = htons(puertoAuxiliar); 
}

void setTipoConsulta(){
	if (strcmp(tipoConsulta, "-a") == 0)
		parametros.nroConsulta = T_A;
	else if (strcmp(tipoConsulta, "-mx") == 0)
		parametros.nroConsulta = T_MX;
	else
		parametros.nroConsulta = T_LOC;	
}

void setTipoResolucionConsulta(){
	if (strcmp(tipoResolucionConsulta, "-r") == 0)
		parametros.nroResolucionConsulta = C_RECURSIVA;
	else if (strcmp(tipoResolucionConsulta, "-t") == 0)
		parametros.nroResolucionConsulta = C_ITERATIVO;
}

void evaluarIngreso(char* argv[], int argc){
	if (argc > 6 || argc < 3)
		mensajeAyuda();
	else{
		if (strcmp(argv[1], "query")==0){
			int servidorAsignado = buscarServidorYPuerto(argv, argc);
			int hayParametrosAsignados=evaluarOpcionesIngreso(servidorAsignado, argv, argc);

			if (hayParametrosAsignados == 0)
				printf("Los parametros ingresados son INCORRECTOS. Por favor chequee su entrada. \n");
			else {
				setPuerto();				
				setTipoConsulta();
				setTipoResolucionConsulta();				

				asignarInformacion(parametros);
				iniciarDNS(argv[2]);
			}
		}
		else
			mensajeAyuda();
	}
}