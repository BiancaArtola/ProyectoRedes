#include "../Archivos.H/Registros/answers-service.h"
#include "../Archivos.H/Registros/registros.h"


void mostrarAnswers(int i, int finalizar, struct DNS_HEADER *dns,struct RES_RECORD answers[20],
 unsigned char buf[65536], unsigned char *reader){
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


void logicaAnswer(int i, int finalizar, struct DNS_HEADER *dns, struct RES_RECORD answers[20],
unsigned char buf[65536], unsigned char *reader){
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


