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
//Servidor DNS
char servidorDNS[10];

//Valor de clase --> solo usamos internet (IN)
#define VALOR_CLASS_IN 1


//Tipos de resolucion de consulta
#define C_RECURSIVA 1
#define C_ITERATIVO 0

#define PUERTO_DEFECTO 53


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

/*struct resultadoLOC{
 int latdeg;
 int latmin;
 int latsec;
 int latsecfrac;
 int longdeg;
 int longmin;
 int longsec;
 int longsecfrac;
 char nortesur;
 char esteoeste;
  int altmeters;
  int altfrac;
  int altsign;
  char *sizestr;
  char* *hpstr;
  char *vpstr;
}resLOC;*/

struct resultadoSOA{
    unsigned int serial;
    unsigned int refresh;
    unsigned int retry;
    unsigned int expire;
    unsigned int minimum;
}resSOA;