#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

struct timeval timeout;

int crearSocket( unsigned char buf[65536], int tamanioMensajeSocket, struct sockaddr_in dest);

struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};