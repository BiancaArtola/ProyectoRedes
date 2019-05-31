#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct DNS_HEADER{
    unsigned short id;

    unsigned char rd :1;
    unsigned char tc :1; 
    unsigned char aa :1; 
    unsigned char opcode :4; 
    unsigned char qr :1;

    unsigned char rcode :4; 
    unsigned char cd :1; 
    unsigned char ad :1; 
    unsigned char z :1; 
    unsigned char ra :1; 

    unsigned short q_count; 
    unsigned short ans_count; 
    unsigned short auth_count; 
    unsigned short add_count; 
};
void cambiarAFormatoDNS(unsigned char* dns,unsigned char* host);
void asignarPropiedadesDNS(struct DNS_HEADER *dns, int bitRecursion);
void mostrarContenidoRespuesta(struct DNS_HEADER *dns, unsigned char* host);
u_char* leerFormatoDNS(unsigned char* reader,unsigned char* buffer, int* contador);
