#include "../Archivos.H/dns.h"

#include "../Archivos.H/DNS-service.h"
#include "../Archivos.H/consultaLOC.h"
#include "../Archivos.H/socket.h"


struct sockaddr_in dest;
struct DNS_HEADER *dns ;
struct sockaddr_in a;
struct RES_RECORD answers[20],auth[20],addit[20]; 
unsigned char buf[65536];
unsigned char *reader;

void asignarInformacion(struct informacionConsultaDNS parametros);
void asignarServidorDNS(char* servidor);
void buscarIPporNombre(unsigned char *host);
void consultaIterativa(unsigned char *host, int qtype);

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
    //asignarStruct(parametros, infoConsulta);

    asignarInformacion(parametros);
    dest.sin_port = (int) infoConsulta.puerto;  
    asignarServidorDNS(infoConsulta.servidor);
     
   // buscarIPporNombre(infoConsulta.consulta);
   if (infoConsulta.nroResolucionConsulta == 0){
       //Seteo el primer servidor para que sea el raiz
       //infoConsulta.servidor="202.12.27.33";
        consultaIterativa(infoConsulta.consulta, infoConsulta.nroConsulta );
       // descomponer();
     }
    else{
      buscarIPporNombre(infoConsulta.consulta);
     }
    return 0;
}

void mostrarAnswers(int i, int finalizar){
    if(ntohs(answers[i].resource->type)==ns_t_a) {
            int j;
            for(j=0;j<ntohs(answers[i].resource->data_len);j++)
                answers[i].rdata[j]=reader[j];            
            
            answers[i].rdata[ntohs(answers[i].resource->data_len)]='\0';
                reader+=ntohs(answers[i].resource->data_len);

            long *p;
            p=(long*)answers[i].rdata;
            a.sin_addr.s_addr=(*p);      
            printf("  IN     A      %s \n", inet_ntoa(a.sin_addr));
        }
        else if (ntohs(answers[i].resource->type)==ns_t_mx ) {
            *answers[i].rdata = *(reader+1);      
     
            printf("         IN MX        %d ", *(answers[i].rdata));

            reader+=sizeof(short);           
            answers[i].rdata+= sizeof(short);

            answers[i].rdata=leerFormatoDNS(reader, buf, &finalizar);

            answers[i].rdata-= sizeof(short);
            
            printf("%s \n", answers[i].rdata+sizeof(short));

            reader+=finalizar;
        }else if (ntohs(answers[i].resource->type)==ns_t_ns ){           
             answers[i].rdata = leerFormatoDNS(reader, buf, &finalizar);   
             reader+=finalizar;     
             printf("     IN    NS    %s \n", answers[i].rdata);    
        }
        else if (ntohs(answers[i].resource->type)==ns_t_loc )
        {
            consulta_LOC(reader);
        }
}

mostrarAuthoritive(int i, int finalizar){
     if (ntohs(auth[i].resource->type)==ns_t_soa )
                {
                    printf("       IN     SOA             ");

                    auth[i].rdata=leerFormatoDNS(reader,buf,&finalizar);
                    reader+=finalizar;
                    printf("%s  ",  auth[i].rdata);

                    auth[i].rdata=leerFormatoDNS(reader,buf,&finalizar);
                    reader+=finalizar;
                    printf("%s  ",  auth[i].rdata);

                    struct resultadoSOA *resSOA=(struct resultadoSOA*) reader;

                    printf("%d  %d  %d  %d  %d \n", ntohl(resSOA->serial), ntohl(resSOA->refresh), ntohl(resSOA->retry),
                    ntohl(resSOA->expire), ntohl(resSOA->minimum));

                    auth[i].rdata+=sizeof(struct resultadoSOA);
                } 
                else if (ntohs(auth[i].resource->type)==ns_t_ns )
                    {
                        answers[i].rdata = leerFormatoDNS(reader, buf, &finalizar);   
                        reader+=finalizar;     
                        printf("     IN    NS    %s \n", answers[i].rdata);  
                    }
}

mostrarAdditional(int i, int finalizar){
    if(ntohs(addit[i].resource->type)==ns_t_a) {
                    int j;
                    for(j=0;j<ntohs(addit[i].resource->data_len);j++){
                        addit[i].rdata[j]=reader[j];
                    }
                    addit[i].rdata[ntohs(addit[i].resource->data_len)]='\0';
                    reader+=ntohs(addit[i].resource->data_len);

                    long *p;
                    p=(long*)addit[i].rdata;
                    a.sin_addr.s_addr=(*p);
   
                    printf("%s  ",addit[i].name);  
                    printf("  IN     A            %s \n",  inet_ntoa(a.sin_addr));

                    asignarServidorDNS(inet_ntoa(a.sin_addr));
                }
                //Si es IPv6 lo salteo
                else if (ntohs(addit[i].resource->type)==ns_t_aaaa ) {
                    reader+=ntohs(addit[i].resource->data_len);
                }  
}

void logicaAnswer(int i, int finalizar){
    if (dns->ans_count>0)
        printf("\n\n;; ANSWERS SECTION\n");
    for (i=0; i < ntohs(dns->ans_count); i++){        
        answers[i].name=leerFormatoDNS(reader, buf, &finalizar);
        reader+=finalizar;
        printf("%s",answers[i].name);   

        answers[i].resource=(struct R_DATA*)(reader);
        reader=reader+sizeof(struct R_DATA);    
        answers[i].rdata= (unsigned char*)malloc(ntohs(answers[i].resource->data_len));
        mostrarAnswers(i, finalizar);      
    }
}

void logicaAdditional(int i, int finalizar){
 if (dns->add_count>0)
        printf("\n\n;; ADDITIONAL SECTION\n");
    for(i=0;i<ntohs(dns->add_count);i++){
        addit[i].name=leerFormatoDNS(reader,buf,&finalizar);        
                
        reader+=finalizar;

        addit[i].resource=(struct R_DATA*)(reader);
        reader=reader+sizeof(struct R_DATA);

        addit[i].rdata= (unsigned char*)malloc(ntohs(addit[i].resource->data_len));

        mostrarAdditional(i, finalizar);     
    }
}

void logicaAuthoritive(int i, int finalizar){
    if (dns->auth_count>0)
        printf("\n\n;; AUTHORITIVE SECTION:\n");
    for(i=0;i<ntohs(dns->auth_count);i++) {
                auth[i].name=leerFormatoDNS(reader,buf,&finalizar);

                reader+=finalizar;

                printf("%s ", auth[i].name);

                auth[i].resource=(struct R_DATA*)(reader);
                reader=reader+sizeof(struct R_DATA);      
                mostrarAuthoritive(i, finalizar);
   
    }                            
}

void leerRegistros(){
    int finalizar=0;
	int i = 0;
    logicaAnswer(i, finalizar);
    logicaAuthoritive(i, finalizar);    
    logicaAdditional(i, finalizar);
}

void buscarIPporNombre(unsigned char* host){	
    unsigned char *qname;
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
	asignarPropiedadesDNS(dns);    

    //Apunta a la parte del query
    tamanioMensajeSocket = sizeof(struct DNS_HEADER);
    qname =(unsigned char*)&buf[tamanioMensajeSocket];
  
    cambiarAFormatoDNS(qname, host);

    //Informacion de consulta
    tamanioMensajeSocket+=(strlen((const char*)qname) + 1);
    qinfo =(struct QUESTION*)&buf[tamanioMensajeSocket];	
    qinfo->qtype = htons(infoConsulta.nroConsulta); //Tipo de consulta
    qinfo->qclass = htons(VALOR_CLASS_IN); 

    int tamanioDest = crearSocket(buf, tamanioMensajeSocket, dest);
 
    dns = (struct DNS_HEADER*) buf;
    
    mostrarContenidoRespuesta(dns);
    
    tamanioDest = sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION);
    reader = &buf[tamanioDest];  

	leerRegistros();
}

void consultaIterativa(unsigned char *host, int qtype){
    unsigned char primeraLlamada[100];
    strcpy(primeraLlamada, ".");
    infoConsulta.nroConsulta=ns_t_ns;
    buscarIPporNombre(primeraLlamada);
    dns->ans_count = 0;
    while (ntohs(dns->ans_count) == 0)
    {
        infoConsulta.nroConsulta=qtype;
        buscarIPporNombre(host);
    }
}