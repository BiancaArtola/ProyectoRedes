#include "../Archivos.H/consultas.h"
#include "../Archivos.H/dns.h"
#include "../Archivos.H/ayuda.h"
#include <stdio.h>

struct parametrosConsulta {
	char* servidor;
	char* puerto;
	int nroConsulta;
	int nroResolucionConsulta;
	unsigned char* consulta;
}parametros;

char* tipoConsulta;
char* tipoResolucionConsulta;

int buscarServidorYPuerto(char* parametroParaEvaluar[], int cantParametros);
int buscarPuerto(char* parametroServidor);
void asignarServidorConPuerto(char parametroServidor[], int posicionComienzoPuerto);
void asignarServidorSinPuerto(char parametroServidor[]);
void evaluarPuerto(char parametroServidor[]);
void asignarTipoConsultaPorDefecto();
int evaluarParametrosFinales(char* parametrosFinales[], int cantParametros, int comienzoParametros);
int evaluarOpcionesIngreso(int servidorAsignado, char* parametrosIngresados[], int cantParametros);
void setPuerto();
void setTipoConsulta();
void setTipoResolucionConsulta();
void setConsulta(char* consulta);
int evaluarIngreso(char* argv[], int argc);

/**
 * 	Metodo encargado de evaluar si se ingreso servidor.
 *  Retorna 0 si el servidor fue asignado por el usuario en su entrada. Retorna 1 en caso contrario.
 * 	Parametros:
 * * parametroParaEvaluar: array con parametros a evaluar
 * * cantParametros: cantidad de parametros ingresados por el usuario
 **/
int buscarServidorYPuerto(char* parametroParaEvaluar[], int cantParametros){
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

/**
 * 	Metodo encargado de evaluar si se ingreso puerto.
 *  Retorna 0 si el usuario no ingreso un puerto. Retorna la posicion de inicio del mismo en caso contrario.
 * 	Parametros:
 * * parametroServidor: string que contiene el servidor (y en caso de que se haya ingresado, contiene tambien el puerto)
 **/
int buscarPuerto(char* parametroServidor){
	int posicionComienzoPuerto=0;
	int cantCaracteres = strlen(parametroServidor);
	int encontrePuerto = 0;
	int i = 0;
	for (i = 0 ; i < cantCaracteres && encontrePuerto ==0; i++){
		if (parametroServidor[i] == ':' ) {
			encontrePuerto = 1;
			posicionComienzoPuerto = i+1;
		}
	}
	return posicionComienzoPuerto;
}


/**
 * 	Metodo encargado de asignar SERVIDOR y PUERTO en caso de que se hayan ingresado.
 * 	Parametros:
 * * parametroServidor: string que contiene el servidor y el puerto.
 * * posicionComienzoPuerto: indica la posicion de comienzo del puerto
 **/
void asignarServidorConPuerto(char parametroServidor[], int posicionComienzoPuerto){
	parametros.servidor=(char*) malloc(100);
	parametros.puerto = (char*) malloc(50);

	//El puerto comienza donde termina el servidor
	int longitudServidor = posicionComienzoPuerto;
	parametros.puerto = parametroServidor+posicionComienzoPuerto;

	//Asigno en parametros.servidor el valor correspondiente ingresado por el usuario
	//Como el primer caracter ingresado es @, hacemos el +1
	strncpy(parametros.servidor, parametroServidor, posicionComienzoPuerto-1);
	parametros.servidor = parametros.servidor+1;
}

/**
 * 	Metodo encargado de asignar SERVIDOR en caso de que se haya ingresado.
 * 	Parametros:
 * * parametroServidor: string que contiene el servidor
 **/
void asignarServidorSinPuerto(char parametroServidor[]){
	parametros.puerto="";
	parametros.servidor = parametroServidor+1;
}

/**
 * 	Metodo encargado de verificar si el usuario ingreso o no puerto, y derivar a otros metodos para la correspondiente asignacion.
 * 	Parametros:
 * * parametroServidor: string que contiene el servidor (y en caso de que se haya ingresado, contiene tambien el puerto)
 **/
void evaluarPuerto(char parametroServidor[]){
	int posicionComienzoPuerto = buscarPuerto(parametroServidor);

	if (posicionComienzoPuerto>0){ //Esto indica que hay puerto
		asignarServidorConPuerto(parametroServidor, posicionComienzoPuerto);
	}else //Usuario no ingreso puerto
		asignarServidorSinPuerto(parametroServidor);
}

/**
 * 	Metodo encargado de asignar el tipo de consulta por defecto (TIPO A) y el tipo de resolucion de consulta por defecto (TIPO r)
 **/
void asignarTipoConsultaPorDefecto(){
	if (tipoConsulta == NULL)
		tipoConsulta = TIPOCONSULTA_DEFECTO;
	if (tipoResolucionConsulta == NULL)
		tipoResolucionConsulta = TIPORESOLUCION_DEFECTO;
}


/**
 * 	Metodo encargado de evaluar el correcto ingreso del tipo de consulta por defecto y del tipo de resolucion de consulta
 * 	Retorna 0 en caso de que haya algun error en el ingreso. Retorna 1 caso contrario.
 * Parametros:
 * * parametroFinales: array con parametros a evaluar
 * * cantParametros: indica la cantidad de parametros ingresados por el usuario
 * * comienzoParametros: indica la posicion de comienzo de los parametros de tipo consulta o tipo resolucion consulta
 **/
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
				else{				
					return 0; //Hubo un error en el ingreso de parametros
				}
		}
		else if ((strcmp(parametrosFinales[i], "-r") == 0) ||
				(strcmp(parametrosFinales[i], "-t") == 0)) {
					if (tipoResolucionConsulta == NULL)
						tipoResolucionConsulta = parametrosFinales[i];
					else{
						
						return 0; //Hubo un error en el ingreso de parametros
					}
		}
		else{			
			return 0; //Hubo un error en el ingreso de parametros
		}
	}
	asignarTipoConsultaPorDefecto();
	return 1; //Salida exitosa
}

/**
 * 	Metodo encargado de evaluar las opciones que ingreso el usuario.
 * 	Retorna 0 en caso de que haya algun error en el ingreso. Retorna 1 caso contrario.
 * Parametros:
 * * servidorAsignado: indica si el usuario asigno un servidor (0 positivo, 1 negativo)
 * * parametrosIngresados: array con parametros a evaluar
 * * cantParametros: indica la cantidad de parametros ingresados por el usuario
**/
int evaluarOpcionesIngreso(int servidorAsignado, char* parametrosIngresados[], int cantParametros){
	/*Si el usuario ingreso un servidor (--> un parametro mas)
	* e ingreso algun otro parametro (por lo que cantParametros>4) evaluo los que siguen. */
	if (servidorAsignado == 0 && cantParametros>4)
		return evaluarParametrosFinales(parametrosIngresados, cantParametros, 4);
	else if (servidorAsignado==1 && cantParametros>3){		
	 	//El usuario no ingreso servidor entonces hay un parametro menos
		return evaluarParametrosFinales(parametrosIngresados, cantParametros, 3);
	}
	else if ((servidorAsignado==1 && cantParametros==3) || (servidorAsignado==0 && cantParametros==4)){
		//No hay parametros de tipo consulta o tipo resolucion consulta
		asignarTipoConsultaPorDefecto();
		return 1;
	}
	else{		
		mensajeAyuda();
	}
}

/**
 * 	Metodo encargado de setear el puerto
**/
void setPuerto(){
	int puertoAuxiliar = atoi(parametros.puerto);
	if (puertoAuxiliar == 0)
		parametros.puerto = (char*)(intptr_t) htons(PUERTO_DEFECTO); 
	else
		parametros.puerto = (char*)(intptr_t) htons(puertoAuxiliar); 
}

/**
 * 	Metodo encargado de setear el tipo de consulta
**/
void setTipoConsulta(){
	if (strcmp(tipoConsulta, "-a") == 0)
		parametros.nroConsulta = ns_t_a;
	else if (strcmp(tipoConsulta, "-mx") == 0)
		parametros.nroConsulta = ns_t_mx;
	else
		parametros.nroConsulta = ns_t_loc;	
}

/**
 * 	Metodo encargado de setear el tipo de resolucion de consulta
**/
void setTipoResolucionConsulta(){
	if (strcmp(tipoResolucionConsulta, "-r") == 0)
		parametros.nroResolucionConsulta = C_RECURSIVA;
	else if (strcmp(tipoResolucionConsulta, "-t") == 0)
		parametros.nroResolucionConsulta = C_ITERATIVO;
}

/**
 * 	Metodo encargado de setear la consulta
**/
void setConsulta(char* consulta){
	parametros.consulta = consulta;
}

/**
 * 	Metodo encargado de evaluar la query ingresada por el usuario. Evalua diferentes escenarios de error.
 * Retorna -1 en caso de que la consulta ingresada por el usuario sea erronea. Retorna 1 si la consulta es correcta.
 * Parametros:
 * * argc: contiene el número de argumentos que se han introducido
 * * argv: array de punteros a caracteres
**/
int evaluarIngreso(char* argv[], int argc){
	if (argc < 7 && argc>2){
		if (strcmp(argv[1], "query")==0){
			if (strcmp(argv[2], "-h")!=0){
				int servidorAsignado = buscarServidorYPuerto(argv, argc);
				int hayParametrosAsignados=evaluarOpcionesIngreso(servidorAsignado, argv, argc);

				if (hayParametrosAsignados != 0){			
					setPuerto();				
					setTipoConsulta();
					setTipoResolucionConsulta();		
					setConsulta(argv[2]);
					iniciarDNS(parametros);
					return 1;
				}
			}
		}
	}
	return -1;
}
	