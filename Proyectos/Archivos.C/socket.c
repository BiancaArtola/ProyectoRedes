#include "../Archivos.H/socket.h"
int crearSocket( unsigned char buf[65536], int tamanioMensajeSocket, struct sockaddr_in dest){
    int socketDNS;
    socketDNS = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	tamanioMensajeSocket+=sizeof(struct QUESTION);
	int tamanioDest = sizeof(dest);
    if (sendto(socketDNS, (char*)buf, tamanioMensajeSocket, 0, (struct sockaddr*)&dest, tamanioDest) < 0){
        perror("Error en el servidor");
    }	

    if (recvfrom(socketDNS, (char*)buf, 65536, 0, (struct sockaddr*)&dest, (socklen_t*)&tamanioDest ) < 0) {
        perror("recvfrom failed");
    }
    return tamanioDest;
}