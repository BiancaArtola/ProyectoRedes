#include<stdio.h>
#include<string.h>

struct informacionConsultaDNS {
	char* servidor;
	char* puerto;
	int nroConsulta;
	int nroResolucionConsulta;
    unsigned char* consulta;
}infoConsulta;


char servidorDNS[10];
void asignarStruct(struct informacionConsultaDNS parametros, struct informacionConsultaDNS infoConsulta);
