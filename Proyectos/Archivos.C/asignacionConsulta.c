#include "../Archivos.H/asignacionConsulta.h"
#include <stdio.h>

/**
 * Si el usuario no asigno ningun servidor, se asigna el servidor por defecto, el cual
 * es obtenido en el archivo "resolv.conf".
 * Si el usuario ya asigno un servidor, se setea ese servidor
 * */
void asignarServidorDNS(char* servidor, struct informacionConsultaDNS infoConsulta){
	if (strcmp(servidor, "") == 0){
		FILE *fp;
		char line[200] , *p;
		if((fp = fopen("/etc/resolv.conf" , "r")) == NULL)
			printf("Failed opening /etc/resolv.conf file \n");

		while(fgets(line , 200 , fp)){
			if(line[0] == '#')
				continue;
			if(strncmp(line , "nameserver" , 10) == 0){
				p = strtok(line , " ");
				p = strtok(NULL , " ");
				//agarra el ip que esta en ese archivo, queda en p
			}
		}
		strcpy(servidorDNS, p);
	}
	else
		strcpy(servidorDNS, servidor);
     infoConsulta.servidor = servidorDNS;
}

/**
 * Se asignan los parametros ingresados por el usuario
 * */
void asignarInformacion(struct informacionConsultaDNS parametros, struct informacionConsultaDNS infoConsulta){
    infoConsulta.servidor = parametros.servidor;
    infoConsulta.puerto = parametros.puerto;
    infoConsulta.nroConsulta = parametros.nroConsulta;
    infoConsulta.nroResolucionConsulta = parametros.nroResolucionConsulta;
    infoConsulta.consulta = parametros.consulta;
}
 void asignarStruct(struct informacionConsultaDNS parametros, struct informacionConsultaDNS infoConsulta){
    asignarInformacion(parametros, infoConsulta);
    
    asignarServidorDNS(infoConsulta.servidor, infoConsulta);

}
