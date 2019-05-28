#include<stdio.h>
#include<string.h>
#include<stdlib.h>

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
void cambiarAFormatoDNS(unsigned char* dns,unsigned char* host);
void asignarPropiedadesDNS(struct DNS_HEADER *dns);
void mostrarContenidoRespuesta(struct DNS_HEADER *dns, unsigned char* host);
u_char* leerFormatoDNS(unsigned char* reader,unsigned char* buffer, int* contador);
