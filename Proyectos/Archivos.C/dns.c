#include "../Archivos.H/dns.h"

struct sockaddr_in dest;
struct DNS_HEADER *dns ;
struct sockaddr_in a;
//Respuestas del servidor DNS --> tienen el mismo formato (resource records)
struct RES_RECORD answers[20],auth[20],addit[20]; 

void asignarInformacion(struct informacionConsultaDNS parametros);
void asignarServidorDNS(char* servidor);
void buscarIPporNombre(unsigned char *host);
void mostrarContenidoRespuesta();
void mostrarAnswerRecords();
void mostrarAutoritiveRecords();
void mostrarAdditionalRecords();
void mostrarRespuestas();
void asignarPropiedadesDNS();
void readGeneral(int i, struct RES_RECORD record[20], unsigned char *reader, int finalizar);
void readTipoRecurso(struct RES_RECORD record[20], int i,unsigned char *reader,  unsigned char buf[65536], int finalizar);
u_char* ReadName(unsigned char* reader,unsigned char* buffer, int* count);
void cambiarAFormatoDNS(unsigned char* dns,unsigned char* host);

void asignarInformacion(struct informacionConsultaDNS parametros){
    infoConsulta.servidor = parametros.servidor;
    infoConsulta.puerto = parametros.puerto;
    infoConsulta.nroConsulta = parametros.nroConsulta;
    infoConsulta.nroResolucionConsulta = parametros.nroResolucionConsulta;
    infoConsulta.consulta = parametros.consulta;
}

int iniciarDNS(struct informacionConsultaDNS parametros){
    asignarInformacion(parametros);

    asignarServidorDNS(infoConsulta.servidor);
    dest.sin_port = infoConsulta.puerto;
    
    buscarIPporNombre(infoConsulta.consulta);
    return 0;
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

void buscarIPporNombre(unsigned char *host){	
    unsigned char buf[65536],*qname,*reader;
    int j;   
	int tamanioMensajeSocket=0;
       
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
    cambiarAFormatoDNS(qname, host);
    
    //Informacion de consulta
    tamanioMensajeSocket+=(strlen((const char*)qname) + 1);
    qinfo =(struct QUESTION*)&buf[tamanioMensajeSocket];	
    qinfo->qtype = htons(2); //Tipo de consulta
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
    mostrarContenidoRespuesta(dns);
    
    tamanioDest = sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION);
    reader = &buf[tamanioDest];  //mueve el puntero	

	int finalizar=0;
	int i = 0;
    for (i=0; i < ntohs(dns->ans_count); i++){        
        answers[i].name=ReadName(reader, buf, &finalizar);
        reader+=finalizar;

        answers[i].resource=(struct R_DATA*)(reader);
        reader=reader+sizeof(struct R_DATA);    
        answers[i].rdata= (unsigned char*)malloc(ntohs(answers[i].resource->data_len));

        if(ntohs(answers[i].resource->type)==T_A) {
            int j;
            for(j=0;j<ntohs(answers[i].resource->data_len);j++)
                answers[i].rdata[j]=reader[j];            
            
            answers[i].rdata[ntohs(answers[i].resource->data_len)]='\0';
                reader+=ntohs(answers[i].resource->data_len);
        }
        else if (ntohs(answers[i].resource->type)==T_MX) {
           // *answers[i].rdata = *(reader+1);      

            answers[i].rdata=ReadName(reader, buf, &finalizar);
            printf("numero del mail: %s",answers[i].rdata);
            reader+=sizeof(short);
            answers[i].rdata+= sizeof(short);

            finalizar = 0 ;
            
            answers[i].rdata = ReadName(reader, buf, &finalizar);
 
            answers[i].rdata-= sizeof(short);
          
            reader+=finalizar;
        }else if (ntohs(answers[i].resource->type)==T_NS){           
             int j;
             answers[i].rdata = ReadName(reader, buf, &finalizar);            
            printf("rdata NS %s", answers[i].rdata);
        }
    }

    //read authorities PROBE ESTO CON MX Y TAMBIEN TIRO ADITIONAL 
    for(i=0;i<ntohs(dns->auth_count);i++){
        auth[i].name=ReadName(reader,buf,&finalizar);

        reader+=finalizar;
        auth[i].resource=(struct R_DATA*)(reader);
        reader=reader+sizeof(struct R_DATA);       
        auth[i].rdata= (unsigned char*)malloc(ntohs(auth[i].resource->data_len));

        printf("Reader1: %i - Finalizar %i \n", *reader, finalizar);
        auth[i].rdata=ReadName(reader,buf,&finalizar);
        printf("-Nombre servidor: %s \n\n",auth[i].rdata);
        reader+=finalizar;

        printf("Reader2: %i - Finalizar %i \n", *reader, finalizar);
        auth[i].rdata = ReadName(reader, buf, &finalizar);
        printf("-Nombre servidor: %s \n\n",auth[i].rdata);
        reader+=finalizar;


        printf("Reader3: %i - Finalizar %i \n", *reader, finalizar);       
        auth[i].rdata =       ReadName(reader, buf, &finalizar);
        printf("-Nombre servidor: %s \n\n", auth[i].rdata );        
        reader+=finalizar;

         
        /*    reader+=4;
            printf("reader %i", *reader);
            auth[i].rdata+=4;

            finalizar = 0 ;
            
            auth[i].rdata = ReadName(reader, buf, &finalizar);
 
            auth[i].rdata-=4;

        printf("-Nombre servidor: %s \n\n",auth[i].rdata+4);*/

    }

    //read additional
    for(i=0;i<ntohs(dns->add_count);i++)  {
        addit[i].name=ReadName(reader,buf,&finalizar);        
        
        reader+=finalizar;
        addit[i].resource=(struct R_DATA*)(reader);
        reader=reader+sizeof(struct R_DATA);

         addit[i].rdata= (unsigned char*)malloc(ntohs(addit[i].resource->data_len));

        if(ntohs(addit[i].resource->type)==T_A) {
            int j;
            for(j=0;j<ntohs(addit[i].resource->data_len);j++){
                addit[i].rdata[j]=reader[j];
            }
            addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
            reader+=ntohs(addit[i].resource->data_len);
            }
        else if (ntohs(addit[i].resource->type)==T_MX) {
        *addit[i].rdata = *(reader+1);
            finalizar = 0 ;
            addit[i].rdata = ReadName(reader, buf, &finalizar);
            reader+=finalizar;
        }    
    }


	mostrarRespuestas(answers, auth, addit);  
	return;
}


void mostrarContenidoRespuesta(){
	printf("La respuesta contiene: \n");
    printf("%d Questions. \n", ntohs(dns->q_count));
    printf("%d Answer. \n", ntohs(dns->ans_count));
    printf("%d Authoritative Servers. \n", ntohs(dns->auth_count));
    printf("%d Additional records.\n\n", ntohs(dns->add_count));
}

void mostrarAnswerRecords(){
	int i;
    if (ntohs(dns->ans_count)>0)
        printf("\n\n;; ANSWERS SECTION:\n");
    for(i=0 ; i < ntohs(dns->ans_count) ; i++){
        

        if ( ntohs(answers[i].resource->type) == T_A) {
            long *p;
            p=(long*)answers[i].rdata;
            a.sin_addr.s_addr=(*p);
            printf(" %s          5    IN     A              ",answers[i].name);        
            printf("%s \n\n", inet_ntoa(a.sin_addr));
        }

        else if ( ntohs(answers[i].resource->type) == T_MX) {   
            printf(" %s          5    IN     MX              ",answers[i].name);        
            printf(" %s \n", answers[i].rdata+sizeof(short));
        }
    }
}

void mostrarAutoritiveRecords(){
	int i;
    if (ntohs(dns->auth_count)>0)
        printf("\n\n;; AUTHORITIVE SECTION:\n");
    for( i=0 ; i < ntohs(dns->auth_count) ; i++) {
    /*    printf("-Nombre : %s \n",auth[i].name);
        printf("-Nombre servidor: %s \n\n",auth[i].rdata);
        printf("-Nombre servidor: %s \n\n",auth[i].rdata);
        */
        if(ntohs(auth[i].resource->type)==T_LOC){
            printf("-Nombre servidor: %s \n\n",auth[i].rdata);
            printf(" %s          5    IN     SOA              ",auth[i].name);        
            printf(" %s \n",auth[i].rdata);
        }
        printf("\n");
    }
}

void mostrarAdditionalRecords(){
	int i;
    if (ntohs(dns->add_count) > 0)
        printf("\n\n;; ADDITIONAL SECTION\n");
    for(i=0; i < ntohs(dns->add_count) ; i++) {
            long *p;
            p=(long*)addit[i].rdata;
            a.sin_addr.s_addr=(*p);
            //printf("-Direccion IP (IPv4): %s \n\n",inet_ntoa(a.sin_addr));
             printf(" %s       5    IN     A            ",addit[i].name);        
            printf("%s \n",  inet_ntoa(a.sin_addr));
    }
            printf("\n");
}

void mostrarRespuestas(){
	mostrarAnswerRecords();
    mostrarAutoritiveRecords();
    mostrarAdditionalRecords();     
}

void asignarPropiedadesDNS(){
	dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = infoConsulta.nroResolucionConsulta; //Indicara si la consulta es recursiva o iterativa
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
            offset = (*reader)*256 + *(reader+1) - 49152; 
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

	 int i, j;
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
    
    return name;    
}

void readGeneral(int i, struct RES_RECORD record[20], unsigned char *reader, int finalizar){
    reader+=finalizar;
    record[i].resource=(struct R_DATA*)(reader);
    reader=reader+sizeof(struct R_DATA)-2;
}

void readTipoRecurso(struct RES_RECORD record[20], int i, 
    unsigned char *reader,  unsigned char buf[65536], int finalizar){

    record[i].rdata= (unsigned char*)malloc(ntohs(record[i].resource->data_len));

    if(ntohs(record[i].resource->type)==T_A) {
      int j;
      for(j=0;j<ntohs(record[i].resource->data_len);j++){
		record[i].rdata[j]=reader[j];
      }
      record[i].rdata[ntohs(record[i].resource->data_len)]='\0';
      reader+=ntohs(record[i].resource->data_len);
     }
     else if (ntohs(record[i].resource->type)==T_MX) {
       *record[i].rdata = *(reader+1);
        finalizar = 0 ;
        record[i].rdata = ReadName(reader, buf, &finalizar);
        reader+=finalizar;
    }
    else if (ntohs(record[i].resource->type)==T_MX){

    }
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
