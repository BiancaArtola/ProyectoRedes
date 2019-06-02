#include "../Archivos.H/dns.h"
#include "../Archivos.H/DNS-service.h"
#include "../Archivos.H/consultaLOC.h"
#include "../Archivos.H/socket.h"

struct sockaddr_in dest;
struct DNS_HEADER *dns ;
struct sockaddr_in a;
struct RES_RECORD answers[20],auth[20],addit[20]; 
struct hostent *servidorAuxiliar;
struct in_addr ip_addr;
unsigned char buf[512];
unsigned char *reader;
int corte =0;
unsigned char* servidorDeAnswer;
unsigned char* servidorDeAuthoritive;
unsigned char* servidorAdditional;

void asignarServidorDNS(char*);
void asignarInformacion(struct informacionConsultaDNS);
int iniciarDNS(struct informacionConsultaDNS);
void mostrarAnswers(int, int);
int mostrarAuthoritive(int, int);
void mostrarAdditional(int, int);
void logicaAnswer(int, int);
void logicaAdditional(int, int);
void logicaAuthoritive(int, int);
void logicaRegistros();
int realizarConsulta(unsigned char *);
void asignarServidorAuxiliar(unsigned char*);
void consultaIterativa(unsigned char *, int);

/** Metodo encargado de asignar un servidor: 
 * --> Si el usuario no asigno ningun servidor, se asigna el servidor por defecto
 * --> Si el usuario ya asigno un servidor, se setea ese servidor
 * Parametros:
 * servidor: string que contiene el servidor ingresado por el usuario
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
		asignarServidorAuxiliar(servidor);
    infoConsulta.servidor = servidorDNS;
}

/** Metodo encargado de asignar la informacion ingresada por el usuario en una estructura global utilizada por la clase.
 * Parametros:
 * -parametros: estructura con la informacion de la query del usuario.
 **/
void asignarInformacion(struct informacionConsultaDNS parametros){
    infoConsulta.servidor = parametros.servidor;
    infoConsulta.puerto = parametros.puerto;
    infoConsulta.nroConsulta = parametros.nroConsulta;
    infoConsulta.nroResolucionConsulta = parametros.nroResolucionConsulta;
    infoConsulta.consulta = parametros.consulta;
}

/** Metodo que inicia la consulta DNS. Setea los parametros ingresados por el usuario y comienza la consulta al servidor
 * Parametros:
 * -parametros: estructura con la informacion de la query del usuario.
 **/
int iniciarDNS(struct informacionConsultaDNS parametros){
    asignarInformacion(parametros);
    dest.sin_port = (int) infoConsulta.puerto;  
    asignarServidorDNS(infoConsulta.servidor);
   if (infoConsulta.nroResolucionConsulta == C_ITERATIVO){
        consultaIterativa(infoConsulta.consulta, infoConsulta.nroConsulta );
    }
    else{
      return realizarConsulta(infoConsulta.consulta);
    }
}

/** Metodo que muestra las respuestas de tipo ANSWER.
 * Parametros:
 * -i: contador del for
 * -finalizar: indica donde finalizo la respuesta anterior
 **/
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
            corte=-1;
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
             servidorDeAnswer= answers[i].rdata;
             printf("     IN    NS    %s \n", answers[i].rdata);    
        }
        else if (ntohs(answers[i].resource->type)==ns_t_loc )
        {
            consulta_LOC(reader);
        }
}

/** Metodo que muestra las respuestas de tipo AUTHORITIVE.
 * Parametros:
 * -i: contador del for
 * -finalizar: indica donde finalizo la respuesta anterior
 **/
int mostrarAuthoritive(int i, int finalizar){
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
        corte=-1;
    } 
    else if (ntohs(auth[i].resource->type)==ns_t_ns )
        {
            auth[i].rdata = leerFormatoDNS(reader, buf, &finalizar);   
            reader+=finalizar;     
            servidorDeAuthoritive=auth[i].rdata;
            printf("     IN    NS    %s \n", auth[i].rdata);  
        }
}

/** Metodo que muestra las respuestas de tipo ADDITIONAL.
 * Parametros:
 * -i: contador del for
 * -finalizar: indica donde finalizo la respuesta anterior
 **/
void mostrarAdditional(int i, int finalizar){
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

		if (infoConsulta.nroResolucionConsulta != 0){   
			printf("%s  ",addit[i].name);  
			printf("  IN     A            %s \n",  inet_ntoa(a.sin_addr));
		}else
			asignarServidorDNS(inet_ntoa(a.sin_addr));
	}
	//Si es IPv6 lo salteo
	else if (ntohs(addit[i].resource->type)==ns_t_aaaa ) {
		reader+=ntohs(addit[i].resource->data_len);
	}  
}

/** Metodo que realiza la logica de las consultas de tipo ANSWER.
 * Parametros:
 * -i: contador del for
 * -finalizar: indica donde finalizo la respuesta anterior
 **/
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

/** Metodo que realiza la logica de las consultas de tipo ADDITIONAL.
 * Parametros:
 * -i: contador del for
 * -finalizar: indica donde finalizo la respuesta anterior
 **/
void logicaAdditional(int i, int finalizar){
    if (dns->add_count>0 && infoConsulta.nroResolucionConsulta!=0)
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

/** Metodo que realiza la logica de las consultas de tipo AUTHORITIVE.
 * Parametros:
 * -i: contador del for
 * -finalizar: indica donde finalizo la respuesta anterior
 **/
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

/**
* Metodo que se encarga de derivar la realizacion de consultas
 **/
void logicaRegistros(){
    int finalizar=0;
	int i = 0;
    logicaAnswer(i, finalizar);
    logicaAuthoritive(i, finalizar);    
    logicaAdditional(i, finalizar);
}

/** Metodo principal de la clase, la cual se encarga de llamar los diferentes metodos para concretar la consulta
 *  Retorna 0 en caso de que ocurra algun error con el socket. Retorna 1 en caso de una consulta exitosa.
 * Parametros:
 * - host: consulta ingresada por el usuario
 **/
int realizarConsulta(unsigned char* host){	
    unsigned char *qname;
    int j;   
	int tamanioMensajeSocket=0;       
    struct QUESTION *qinfo = NULL;

    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(infoConsulta.servidor);

	dns = NULL;	

    dns = (struct DNS_HEADER *)&buf;
	asignarPropiedadesDNS(dns, infoConsulta.nroResolucionConsulta);    

    tamanioMensajeSocket = sizeof(struct DNS_HEADER);
    qname =(unsigned char*)&buf[tamanioMensajeSocket];
  
    cambiarAFormatoDNS(qname, host);

    tamanioMensajeSocket+=(strlen((const char*)qname) + 1);
    qinfo =(struct QUESTION*)&buf[tamanioMensajeSocket];	
    qinfo->qtype = htons(infoConsulta.nroConsulta);
    qinfo->qclass = htons(ns_c_in); 

    int tamanioDest = crearSocket(buf, tamanioMensajeSocket, dest);
   
    if (tamanioDest == -1)
        return 0;
    dns = (struct DNS_HEADER*) buf;
    
    if (infoConsulta.nroResolucionConsulta != C_ITERATIVO)
        mostrarContenidoRespuesta(dns, host);
    
    tamanioDest = sizeof(struct DNS_HEADER) + (strlen((const char*)qname)+1) + sizeof(struct QUESTION);
    reader = &buf[tamanioDest];  

	logicaRegistros();
    printf("\n");
    return 1;
}

/** Metodo que se encarga de realizar la primera llamada con un PUNTO al servidor en caso de que se haya requerido una consulta iterativa.
 **/
void primeraLlamada(){
  unsigned char primeraLlamada[100];
    strcpy(primeraLlamada, ".");
    infoConsulta.nroConsulta=ns_t_ns;
    int errorSocket = realizarConsulta(primeraLlamada); 
    if (errorSocket == 0)
       exit(0);
}

/** Metodo encargado de mapear un name server a su correspondiente IP
 * Parametros: 
 * - servidor: servidor al cual se le realiza la consulta
 * */
void asignarServidorAuxiliar(unsigned char* servidor)
{
     servidorAuxiliar = gethostbyname(servidor);
        if (!servidorAuxiliar) {
            printf("Ocurrio un error - Servidor incorrecto \n");
            exit(EXIT_FAILURE);
        }
        ip_addr = *(struct in_addr *)(servidorAuxiliar->h_addr);
        strcpy(servidorDNS, inet_ntoa(ip_addr));
}

/** Metodo encargado de realizar la consulta iterativa.
 * Parametros: 
 * - host: consulta ingresada por el usuario
 * - qtype: indica el tipo de consulta que indico el usuario
 * */
void consultaIterativa(unsigned char *host, int qtype){
     primeraLlamada();
     if ((ntohs(dns->add_count)==0) && (ntohs(dns->ans_count)==0) && (ntohs(dns->auth_count)==0))
     {
		 perror("No es posible realizar la consulta, por favor intente nuevamente");
		 exit (1);
	 }
     if (ntohs(dns->add_count)==0)
     {
       asignarServidorAuxiliar(servidorDeAnswer);
     }
    dns->ans_count = 0; 
    while (ntohs(dns->ans_count) == 0 && corte==0){
        infoConsulta.nroConsulta=qtype;
        realizarConsulta(host);
        if (ntohs(dns->add_count)==0 && (ntohs(dns->ans_count)==0)&& corte==0)
            {
                asignarServidorAuxiliar(servidorDeAuthoritive);
            }
    }
}
