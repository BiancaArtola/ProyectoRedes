#include "../Archivos.H/socket.h"

int crearSocket( unsigned char buf[65536], int tamanioMensajeSocket, struct sockaddr_in dest){
    int socketDNS;
    socketDNS = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	tamanioMensajeSocket+=sizeof(struct QUESTION);
	int tamanioDest = sizeof(dest);

    /*TIMEOUT SETTINGS*/
    
    timeout.tv_sec = 30;           
    timeout.tv_usec = 0;
        
    if (setsockopt(socketDNS, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
         perror("Ocurrio un error");
    }

    if (setsockopt(socketDNS, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
         perror("Ocurrio un error");
    }
               
    /*FIN TIMEOUT SETTINGS*/ 
 
    if (sendto(socketDNS, (char*)buf, tamanioMensajeSocket, 0, (struct sockaddr*)&dest, tamanioDest) < 0){
            perror("Ocurrio un error");
            return -1;
    }	
    if (recvfrom(socketDNS, (char*)buf, 65536, 0, (struct sockaddr*)&dest, (socklen_t*)&tamanioDest ) < 0) {
        perror("Ocurrio un error");
        return -1;
    }
     
    
    return tamanioDest;
}