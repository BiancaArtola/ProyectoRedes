#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <netdb.h>

//Servidor DNS
char servidorDNS[10];

//Tipos de resolucion de consulta
#define C_RECURSIVA 1
#define C_ITERATIVO 0

#define PUERTO_DEFECTO 53

#pragma pack(push, 1)
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

struct informacionConsultaDNS {
	char* servidor;
	char* puerto;
	int nroConsulta;
	int nroResolucionConsulta;
    unsigned char* consulta;
}infoConsulta;

struct resultadoSOA{
    unsigned int serial;
    unsigned int refresh;
    unsigned int retry;
    unsigned int expire;
    unsigned int minimum;
}resSOA;