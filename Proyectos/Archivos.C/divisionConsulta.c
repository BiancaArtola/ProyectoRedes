
char* descomponerIP(char cadena[1000], int inicio, int fin){
	char sub[1000];
   int position, length, c = 0;
 
  position= inicio+2;
  length=fin;
 
   while (c < length) {
      sub[c] = cadena[position+c-1];
      c++;
   }
   sub[c] = '.';
  
   return sub;     
}

void descomponer(){
	unsigned char* cadena = infoConsulta.consulta;  
	int largo = strlen(cadena);
    cadena[largo-1]='\0';
    int i=0;
    for (i = largo-1; i >= 0; i--){
        if (cadena[i]=='.'){
			char* substring = 	descomponerIP(cadena, i, largo);
			 printf("Required substring is \"%s\"\n", substring);
             buscarIPporNombre(substring);
		}   
		
		if ((i == largo-1) && (cadena[largo-1]!='.')){
			char* substring = ".";
			 printf("Required substring is \"%s\"\n", substring);
              buscarIPporNombre(substring);
		}     
    }
}