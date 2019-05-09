#include "dns.h"

struct sockaddr_in dest;

int primerParametro=0;
int segundoParametro=0;
struct DNS_HEADER *dns ;
struct sockaddr_in a;
unsigned char* hostOriginal= NULL;

int iniciarDNS(unsigned char consulta[]){
    unsigned char* consultaUsuario;
    consultaUsuario = consulta;
    buscarIPporNombre(consultaUsuario);
    return 0;
}

/*
 * Asigna el puerto correspondiente.
 * Si numeroPuerto=0 no se asigno ningun puerto, por lo tanto se asigna el puerto por defecto.
 * Sino, se asigna el puerto ingresado por el usuario
 * */
void asignarPuerto(int numeroPuerto){
	if (numeroPuerto == 0)
		dest.sin_port = htons(PUERTO_DEFECTO); 
	else
		dest.sin_port = htons(numeroPuerto); 
}

void asignarTipoConsultaDNS(char* consulta){
	if (strcmp(consulta, "-a") == 0)
		primerParametro = T_A;
	else if (strcmp(consulta, "-mx") == 0)
		primerParametro = T_MX;
	else
		primerParametro = T_LOC;
}

void asignarTipoResolucionConsultaDNS(char* consulta){
	if (strcmp(consulta, "-r") == 0)
		segundoParametro = C_RECURSIVA;
	else if (strcmp(consulta, "-t") == 0)
		segundoParametro = C_ITERATIVO;
}

void asignarPropiedadesDNS(){
	dns->id = (unsigned short) htons(getpid());
    //0 es falso.
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;
}

void mostrarContenidoRespuesta(){
	printf("La respuesta contiene: \n");
    printf("%d Questions. \n", ntohs(dns->q_count));
    printf("%d Answer. \n", ntohs(dns->ans_count));
    printf("%d Authoritative Servers. \n", ntohs(dns->auth_count));
    printf("%d Additional records.\n\n", ntohs(dns->add_count));
}

void mostrarAnswerRecords(struct RES_RECORD answers[20]){
	int i;
    printf("Answer Records: %d \n" , ntohs(dns->ans_count) );
    for(i=0 ; i < ntohs(dns->ans_count) ; i++){
        printf("-Nombre: %s \n",answers[i].name);

        if ( ntohs(answers[i].resource->type) == T_A) {
            long *p;
            p=(long*)answers[i].rdata;
            a.sin_addr.s_addr=(*p); //working without ntohl
            printf("-Direccion IP (IPv4): %s \n\n",inet_ntoa(a.sin_addr));
        }
    }
}

void mostrarAutoritiveRecords(struct RES_RECORD auth[20]){
	int i;
    printf("Authoritive Records: %d \n" , ntohs(dns->auth_count) );
    for( i=0 ; i < ntohs(dns->auth_count) ; i++) {
        printf("-Nombre : %s \n",auth[i].name);
        if(ntohs(auth[i].resource->type)==2){
            printf("-Nombre servidor: %s \n\n",auth[i].rdata);
        }
        printf("\n");
    }
}

void mostrarAdditionalRecords(struct RES_RECORD addit[20]){
	int i;
    printf("Additional Records: %d \n" , ntohs(dns->add_count) );
    for(i=0; i < ntohs(dns->add_count) ; i++) {
        printf("-Nombre: %s \n ",addit[i].name);
        if(ntohs(addit[i].resource->type)==1) {
            long *p;
            p=(long*)addit[i].rdata;
            a.sin_addr.s_addr=(*p);
            printf("-Direccion IP (IPv4): %s \n\n",inet_ntoa(a.sin_addr));
        }
    }
}

void mostrarRespuestas(struct RES_RECORD answers[20],struct RES_RECORD auth[20], struct RES_RECORD addit[20]){
	mostrarAnswerRecords(answers);
    mostrarAutoritiveRecords(auth);
    mostrarAdditionalRecords(addit);     
}

void buscarIPporNombre(unsigned char *host){	
    unsigned char buf[65536],*qname,*reader;
    int j;   
	int tamanioMensajeSocket=0;
	//Respuestas del servidor DNS --> tienen el mismo formato (resource records)
    struct RES_RECORD answers[20],auth[20],addit[20]; 
       
    struct QUESTION *qinfo = NULL;

    printf("Evaluando la consulta: %s \n\n" , host);

	int socketDNS;
    socketDNS = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//Se importa con la libreria netinet
    dest.sin_family = AF_INET; //Familia de la direccion
    dest.sin_addr.s_addr = inet_addr(servidorDNS);

	dns = NULL;	
    //Asigna la estructura DNS para queries estandar
    dns = (struct DNS_HEADER *)&buf;
	asignarPropiedadesDNS();    

    //Apunta a la parte del query
    tamanioMensajeSocket = sizeof(struct DNS_HEADER);
    qname =(unsigned char*)&buf[tamanioMensajeSocket];
    hostOriginal = host;
    cambiarAFormatoDNS(qname, host);
    
    //Informacion de consulta
    tamanioMensajeSocket+=(strlen((const char*)qname) + 1);
    qinfo =(struct QUESTION*)&buf[tamanioMensajeSocket];	
    qinfo->qtype = htons(primerParametro); //Tipo de consulta
    qinfo->qclass = htons(VALOR_CLASS_IN); 

	tamanioMensajeSocket+=sizeof(struct QUESTION);
	int tamanioDest = sizeof(dest);
    if (sendto(socketDNS, (char*)buf, tamanioMensajeSocket, 0, (struct sockaddr*)&dest, tamanioDest) < 0){
        perror("Error en el servidor");
        return 0;
    }
	
    //socket, buffer donde se guarda el msj, tamano en bytes del buffer apuntado por el puntero buffer,
    //flags(tipo de mensaje)
    //modifica el dns
    if (recvfrom(socketDNS, (char*)buf, 65536, 0, (struct sockaddr*)&dest, (socklen_t*)&tamanioDest ) < 0) {
        perror("recvfrom failed");
        return 0;
    }

    dns = (struct DNS_HEADER*) buf;
    mostrarContenidoRespuesta();
    
    tamanioDest = sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION);
    reader = &buf[tamanioDest];  //mueve el puntero	
   
	int finalizar=0;
	int i = 0;
    for (i=0; i < ntohs(dns->ans_count); i++){
        ReadName(reader, buf, &finalizar);
        answers[i].name = hostOriginal;
        reader = reader + finalizar;

        answers[i].resource = (struct R_DATA*)(reader);
        reader = reader + sizeof(struct R_DATA);

		 //if its an ipv4 address
        if(ntohs(answers[i].resource->type) == 1) {
            answers[i].rdata = (unsigned char*)malloc(ntohs(answers[i].resource->data_len));
            for(j=0 ; j<ntohs(answers[i].resource->data_len) ; j++)
                answers[i].rdata[j]=reader[j];   

            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';
            reader = reader + ntohs(answers[i].resource->data_len);
        }
        else{
            answers[i].rdata = ReadName(reader,buf,&finalizar);
            reader = reader + finalizar;
        }
    }

    //read authorities
    for(i=0;i<ntohs(dns->auth_count);i++){
        ReadName(reader,buf,&finalizar);
        auth[i].name = hostOriginal;
         printf("autorizado %s \n",auth[i].name);
        reader+=finalizar;

        auth[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);

        auth[i].rdata=ReadName(reader,buf,&finalizar);
        reader+=finalizar;
    }

    //read additional
    for(i=0;i<ntohs(dns->add_count);i++)  {
        ReadName(reader,buf,&finalizar);
        addit[i].name = hostOriginal;
        printf("adicional %s \n",addit[i].name);
        reader+=finalizar;

        addit[i].resource=(struct R_DATA*)(reader);
        reader+=sizeof(struct R_DATA);

        if(ntohs(addit[i].resource->type)==1) {
            addit[i].rdata = (unsigned char*)malloc(ntohs(addit[i].resource->data_len));
            for(j=0;j<ntohs(addit[i].resource->data_len);j++)
				addit[i].rdata[j]=reader[j];

            addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
            reader+=ntohs(addit[i].resource->data_len);
        }
        else{
            addit[i].rdata=ReadName(reader,buf,&finalizar);
            reader+=finalizar;
        }
    }
	mostrarRespuestas(answers, auth, addit);  
	return;
}

// answers[i].name = ReadName(reader, buf, &finalizar);
u_char* ReadName(unsigned char* reader,unsigned char* buffer, int* count){ 
    *count = 1;
    
    unsigned char *name;
    name = (unsigned char*)malloc(256);
    name[0]='\0'; //caracter nulo

	unsigned int offset;
	unsigned int p=0,jumped=0;
	
    //Lee los nombres en formato DNS(3www6google3com)
    while (*reader!=0) {
        if(*reader >= 192){
            offset = (*reader)*256 + *(reader+1) - 49152; //convierte a binario
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        }else        
            name[p++]=*reader;        
        reader = reader+1;
        if (jumped == 0)
            *count = *count + 1; //if we havent jumped to another location then we can count up
    }
    name[p]='\0'; //FInalizo string
    if(jumped==1)
        *count = *count + 1; //number of steps we actually moved forward in the packet

/*	 int i, j;
    //now convert 3www6google3com0 to www.google.com
    for (i=0; i < (int)strlen((const char*)name); i++){
        p=name[i];
        for(j=0;j<(int)p;j++) {
            name[i]=name[i+1];
            i=i+1;
        }
        name[i]='.';
    }
    name[i-1]='\0'; //remove the last dot
    return name;*/
    
}


/*Si el usuario no asigno ningun servidor, se asigna el servidor por defecto.
 * Si el usuario ya asigno un servidor, se setea ese servidor
 * */
void asignarServidorDNS(char* servidor){
	if (strcmp(servidor, "") == 0){
		FILE *fp;
		char line[200] , *p;
		if((fp = fopen("/etc/resolv.conf" , "r")) == NULL)
			printf("Failed opening /etc/resolv.conf file \n");

		while(fgets(line , 200 , fp)){
			if(line[0] == '#')
				continue;
			if(strncmp(line , "nameserver" , 10) == 0){
				p = strtok(line , " ");
				p = strtok(NULL , " ");
				//agarra el ip que esta en ese archivo, queda en p
			}
		}
		strcpy(servidorDNS, p);
	}
	else
		strcpy(servidorDNS, servidor);
}

/*
 * Convierte la consulta ingresada por el usuario a una consulta DNS
 * */
void cambiarAFormatoDNS(unsigned char* dns,unsigned char* host){
    int lock = 0 ;
    strcat((char*)host,".");
	
	int i;
    for(i = 0 ; i < strlen((char*)host) ; i++)
        if(host[i]=='.') {
            *dns++ = i-lock;
            for(;lock<i;lock++)            
                *dns++=host[lock];
            lock++;
        }    
    *dns++='\0';
}
