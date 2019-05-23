#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/nameser.h>
#include <sys/types.h> 
#include <limits.h>
#include <errno.h>
#include <stdint.h>
//#include "loc_ntoa.h"

//Servidor DNS
char servidorDNS[10];


//Tipos de registros
#define T_A 1 
#define T_MX 15 
#define T_LOC 29
#define T_NS 2


//Valor de clase --> solo usamos internet (IN)
#define VALOR_CLASS_IN 1


//Tipos de resolucion de consulta
#define C_RECURSIVA 1
#define C_ITERATIVO 0

#define PUERTO_DEFECTO 53

//Estructura del header de DNS
struct DNS_HEADER{
    unsigned short id; // identification number
    
	//LENGTH EN BITS
    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritive answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag

    unsigned char rcode :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // its z! reserved
    unsigned char ra :1; // recursion available

    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};

//Constant sized fields of query structure
struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};

//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

//Pointers to resource record contents
struct RES_RECORD{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

/*//Estructura de una query
typedef struct{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;
*/
struct informacionConsultaDNS {
	char* servidor;
	char* puerto;
	int nroConsulta;
	int nroResolucionConsulta;
    unsigned char* consulta;
}infoConsulta;

