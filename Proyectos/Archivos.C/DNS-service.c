#include "../Archivos.H/DNS-service.h"


void asignarPropiedadesDNS(struct DNS_HEADER *dns ){
	dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    //dns->rd = infoConsulta.nroResolucionConsulta; //Indicara si la consulta es recursiva o iterativa
    dns->rd=1;
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count= 0;
    dns->auth_count = 0;
    dns->add_count = 0;
}


/*
 * Convierte la consulta ingresada por el usuario a una consulta DNS
 * */
 void cambiarAFormatoDNS(unsigned char* dns,unsigned char* host){ 
    int lock = 0 ;     
    strcat((char*)host,"."); 	 	
    int i;     
    for(i = 0 ; i < strlen((char*)host) ; i++)         
        if(host[i]=='.') 
        {             
            *dns++ = i-lock;             
            for(;lock<i;lock++)                             
            *dns++=host[lock];             
            lock++;         
        }         
    *dns++='\0'; 
}

void mostrarContenidoRespuesta(struct DNS_HEADER *dns){
	printf("\n\nLa respuesta contiene: \n");
    printf("%d Questions. \n", ntohs(dns->q_count));
    printf("%d Answer. \n", ntohs(dns->ans_count));
    printf("%d Authoritative Servers. \n", ntohs(dns->auth_count));
    printf("%d Additional records.", ntohs(dns->add_count));
}


u_char* leerFormatoDNS(unsigned char* reader,unsigned char* buffer, int* contador){ 
    *contador = 1;
    
    unsigned char *nombre;
    nombre = (unsigned char*)malloc(256);
    nombre[0]='\0'; //caracter nulo

	unsigned int offset;
	unsigned int p=0,salto=0;
    
    while (*reader!=0) {
        if(*reader >= 192){
            offset = (*reader)*256 + *(reader+1) - 49152; 
            reader = buffer + offset - 1;
            salto = 1;
        }else        
            nombre[p++]=*reader;        
        reader = reader+1;
        if (salto == 0)
            *contador = *contador + 1;   
    }
    nombre[p]='\0'; 

    if(salto==1)
        *contador = *contador + 1; 

	int i, j;    
    for (i=0; i < (int)strlen((const char*)nombre); i++){
        p=nombre[i];
        for(j=0;j<(int)p;j++) {
            nombre[i]=nombre[i+1];
            i=i+1;
        }
        nombre[i]='.';
    }

    if (strlen(nombre)==0)
        nombre[0]='.';  
    
    return nombre;    
}