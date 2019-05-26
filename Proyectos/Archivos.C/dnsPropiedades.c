#include "../Archivos.H/dnsPropiedades.h"
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
