#include "../Archivos.H/dns.h"
//#include "../Archivos.H/asignacionConsulta.h"

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
u_char* ReadName(unsigned char* reader,unsigned char* buffer, int* contador);
void cambiarAFormatoDNS(unsigned char* dns,unsigned char* host);
static const char *precsize_ntoa(u_int8_t  prec);


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
    infoConsulta.servidor = servidorDNS;
    
}

void asignarInformacion(struct informacionConsultaDNS parametros){
    infoConsulta.servidor = parametros.servidor;
    infoConsulta.puerto = parametros.puerto;
    infoConsulta.nroConsulta = parametros.nroConsulta;
    infoConsulta.nroResolucionConsulta = parametros.nroResolucionConsulta;
    infoConsulta.consulta = parametros.consulta;
}

int iniciarDNS(struct informacionConsultaDNS parametros){
  //  asignarStruct(parametros, infoConsulta);
  
    asignarInformacion(parametros);
       dest.sin_port = infoConsulta.puerto;  
    asignarServidorDNS(infoConsulta.servidor);
     
  
    buscarIPporNombre(infoConsulta.consulta);
   /*if (infoConsulta.nroResolucionConsulta == 1){
        infoConsulta.nroConsulta = T_NS;
       // descomponer();
     //  char* mierda = "google.com";
  // infoConsulta.consulta = mierda;
        buscarIPporNombre(infoConsulta.consulta);
     }*/
    return 0;
}



void leerRegistros(unsigned char buf[65536], unsigned char *reader){
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
            reader+=sizeof(short);
            answers[i].rdata+= sizeof(short);

            finalizar = 0 ;
            
            answers[i].rdata = ReadName(reader, buf, &finalizar);
 
            answers[i].rdata-= sizeof(short);
          
            reader+=finalizar;
        }else if (ntohs(answers[i].resource->type)==T_NS){           
             int j;
             answers[i].rdata = ReadName(reader, buf, &finalizar);   
             reader+=finalizar;         
        }
        else if (ntohs(answers[i].resource->type)==T_LOC)
        {
            consulta_LOC(reader);
        }
    }
        //read authorities PROBE ESTO CON MX Y TAMBIEN TIRO ADITIONAL 
        for(i=0;i<ntohs(dns->auth_count);i++){
                auth[i].name=ReadName(reader,buf,&finalizar);

                reader+=finalizar;
                auth[i].resource=(struct R_DATA*)(reader);
                reader=reader+sizeof(struct R_DATA);       
                auth[i].rdata= (unsigned char*)malloc(ntohs(auth[i].resource->data_len));
                            
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
                    // *addit[i].rdata = *(reader+1);
                    finalizar = 0 ;
                    addit[i].rdata = ReadName(reader, buf, &finalizar);
                    reader+=finalizar;
                }    
        }
        
    
}
imprimirRegistrosNS(struct QUESTION *qinfo,  unsigned char buf[65536],unsigned char *reader){
   printf("hola");
    qinfo->qtype = htons(infoConsulta.nroConsulta);
    leerRegistros(buf, reader);
}

void buscarIPporNombre(unsigned char *host){	
    unsigned char buf[65536],*qname,*reader;
    int j;   
	int tamanioMensajeSocket=0;       
    struct QUESTION *qinfo = NULL;

    printf("Evaluando la consulta: %s \n\n" , host);
	

	//Se importa con la libreria netinet
    dest.sin_family = AF_INET; //Familia de la direccion
    dest.sin_addr.s_addr = inet_addr(infoConsulta.servidor);

	dns = NULL;	
    //Asigna la estructura DNS para queries estandar
    dns = (struct DNS_HEADER *)&buf;
	asignarPropiedadesDNS();    

    //Apunta a la parte del query
    tamanioMensajeSocket = sizeof(struct DNS_HEADER);
    qname =(unsigned char*)&buf[tamanioMensajeSocket];
    if (infoConsulta.nroConsulta==T_LOC)
        cambiarAFormatoDominio("SW1A2AA.find.me.uk", qname);
    else
         cambiarAFormatoDNS(qname, host);

    
    //Informacion de consulta
    tamanioMensajeSocket+=(strlen((const char*)qname) + 1);
    qinfo =(struct QUESTION*)&buf[tamanioMensajeSocket];	
    qinfo->qtype = htons(infoConsulta.nroConsulta); //Tipo de consulta
    qinfo->qclass = htons(VALOR_CLASS_IN); 


    int socketDNS;
    socketDNS = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	tamanioMensajeSocket+=sizeof(struct QUESTION);
	int tamanioDest = sizeof(dest);
    if (sendto(socketDNS, (char*)buf, tamanioMensajeSocket, 0, (struct sockaddr*)&dest, tamanioDest) < 0){
        perror("Error en el servidor");
        return 0;
    }	

    if (recvfrom(socketDNS, (char*)buf, 65536, 0, (struct sockaddr*)&dest, (socklen_t*)&tamanioDest ) < 0) {
        perror("recvfrom failed");
        return 0;
    }

    dns = (struct DNS_HEADER*) buf;
    
    mostrarContenidoRespuesta(dns);
    
    tamanioDest = sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION);
    reader = &buf[tamanioDest];  //mueve el puntero	

	leerRegistros(buf, reader);
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
        printf("\n\n;; ANSWERS SECTION\n");
    for(i=0 ; i < ntohs(dns->ans_count) ; i++){
        if ( ntohs(answers[i].resource->type) == T_A) {
            long *p;
            p=(long*)answers[i].rdata;
            a.sin_addr.s_addr=(*p);
            printf(" %s              IN     A              ",answers[i].name);        
            printf("%s \n\n", inet_ntoa(a.sin_addr));
        }
        else if ( ntohs(answers[i].resource->type) == T_MX) {   
            printf(" %s             IN     MX              ",answers[i].name);        
            printf(" %s \n", answers[i].rdata+sizeof(short));
        }

          else if ( ntohs(answers[i].resource->type) == T_NS) {   
            printf(" %s              IN     NS              ",answers[i].name);        
            printf(" %s \n", answers[i].rdata);
        }
         else if ( ntohs(answers[i].resource->type) == T_LOC)
         {
            printf(" %s              IN     LOC              ",answers[i].name); 
             printf("%d %.2d %.2d.%.3d %c %d %.2d %.2d.%.3d %c %d.%.2dm %sm %sm %sm \n", 
             resLOC.latdeg, resLOC.latmin, resLOC.latsec, resLOC.latsecfrac,
            resLOC.northsouth, resLOC.longdeg, resLOC.longmin, resLOC.longsec, resLOC.longsecfrac, 
            resLOC.eastwest,resLOC.altmeters, resLOC.altfrac, resLOC.sizestr, resLOC.hpstr, resLOC.vpstr);
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
            printf("-Nombre servidor: %s \n\n",auth[i].rdata);
            printf(" %s          5    IN     LOC             ",auth[i].name);        
           // printf(" %s \n",auth[i].rdata);
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
    dns->ans_count= 0;
    dns->auth_count = 0;
    dns->add_count = 0;
}


u_char* ReadName(unsigned char* reader,unsigned char* buffer, int* contador){ 
    *contador = 1;
    
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
            jumped = 1; //we have jumped to another location so contadoring wont go up!
        }else        
            name[p++]=*reader;        
        reader = reader+1;
        if (jumped == 0)
            *contador = *contador + 1; //if we havent jumped to another location then we can contador up
  
    }
    name[p]='\0'; //FInalizo string
    if(jumped==1)
        *contador = *contador + 1; //number of steps we actually moved forward in the packet

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

/*
 * Convierte la consulta ingresada por el usuario a una consulta DNS
 * */
 cambiarAFormatoDNS(unsigned char* dns,unsigned char* host){ 
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
void cambiarAFormatoDominio(char * dominio, unsigned char * dominioDns){
	int largoDelDominio = strlen(dominio);
	char * part = dominio;
	int contador = 0;
	int i;
	for(i = 0;i <= largoDelDominio ; i++){
		if(*dominio == '.' || i == largoDelDominio){
			int j;
			*dominioDns++ = contador + 0;
			for(j = 0;j<contador;j++){
				*dominioDns++ = *part++;
			}
			part++;
			contador = 0;
		}
		else {
			contador++;	
		}
		dominio++;
	}
	*dominioDns++ = 0;
	*dominioDns = '\0';
}

void consulta_LOC(unsigned char *reader)
{
	 static char tmpbuf[255*3];
	 register char *cp;
	 register const u_char *rcp;
	 int latdeg, latmin, latsec, latsecfrac;
	 int longdeg, longmin, longsec, longsecfrac;
	 char northsouth, eastwest;
	 int altmeters, altfrac, altsign;
	 const int referencealt = 100000 * 100;
	 int32_t latval, longval, altval;
	 u_int32_t templ;
	 u_int8_t sizeval, hpval, vpval, versionval;
	 char *sizestr, *hpstr, *vpstr;
	 
     rcp = reader;

	 versionval = *rcp++;
	 if (versionval) {
		 sprintf(cp,"; error: unknown LOC RR version");
		 return (cp);
	 }

	 sizeval = *rcp++;
	 hpval = *rcp++;
	 vpval = *rcp++;

	 GETLONG(templ,rcp);
	 latval = (templ - ((unsigned)1<<31));

	 GETLONG(templ,rcp);
	 longval = (templ - ((unsigned)1<<31));

	 GETLONG(templ,rcp);
	 if (templ < referencealt) { /* below WGS 84 spheroid */
		 altval = referencealt - templ;
		 altsign = -1;
	 } else {
        altval = templ - referencealt;
        altsign = 1;
    }
    if (latval < 0) {
        northsouth = 'S';
        latval = -latval;
    }
    else
        northsouth = 'N';

    latsecfrac = latval % 1000;
    latval = latval / 1000;
    latsec = latval % 60;
    latval = latval / 60;
    latmin = latval % 60;
    latval = latval / 60;
    latdeg = latval;
    if (longval < 0) {
        eastwest = 'W';
        longval = -longval;
    }
    else
        eastwest = 'E';
    longsecfrac = longval % 1000;
    longval = longval / 1000;
    longsec = longval % 60;
    longval = longval / 60;
    longmin = longval % 60;
    longval = longval / 60;
    longdeg = longval;
    altfrac = altval % 100;
    altmeters = (altval / 100) * altsign;

    sizestr = precsize_ntoa(sizeval);
    hpstr = precsize_ntoa(hpval);
    vpstr = precsize_ntoa(vpval);

    resLOC.latdeg = latdeg;
    resLOC.latsec = latsec;
    resLOC.latmin = latmin;
    resLOC.latsecfrac = latsecfrac;
    resLOC.northsouth = northsouth;
    resLOC.longdeg = longdeg;
    resLOC.longmin = longmin;
    resLOC.longsec = longsec;
    resLOC.longsecfrac = longsecfrac;
    resLOC.eastwest = eastwest;
    resLOC.altmeters = altmeters;
    resLOC.altfrac = altfrac;
    resLOC.sizestr = sizestr;
    resLOC.hpstr = hpstr;
    resLOC.vpstr = vpstr;


}

static unsigned int poweroften[10] = {1, 10, 100, 1000, 10000, 100000,
 1000000,10000000,100000000,1000000000};

static const char *precsize_ntoa(u_int8_t  prec)
{
    static char retbuf[sizeof("90000000.00")];
    unsigned long val;
    int mantissa, exponent;
    mantissa = (int)((prec >> 4) & 0x0f) % 10;
    exponent = (int)((prec >> 0) & 0x0f) % 10;
    val = mantissa * poweroften[exponent];
    (void) sprintf(retbuf,"%d.%.2d", val/100, val%100);
    return (retbuf);
}

