#include "../Archivos.H/consultaLOC.h"

static char *precsize_ntoa(u_int8_t  prec);
void consulta_LOC(unsigned char*);

/**
 * 	Metodo encargado de realizar una consulta de tipo LOC
 * Parametros:
 * * reader: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
**/
void consulta_LOC(unsigned char *reader){
	unsigned char *rcp;
	int latdeg, latmin, latsec, latsecfrac;
	 int longdeg, longmin, longsec, longsecfrac;
	 char nortesur, esteoeste;
	 int altmeters, altfrac, altsign;
	 const int referencealt = 100000 * 100;
	 int32_t latitud, longitud, altitud;
	 u_int32_t templ;
	 u_int8_t size, horizontalpre, verticalpre, version;
	 char *sizestr, *hpstr, *vpstr;
	 
     rcp = reader;
	 version = *rcp++;
	 if (version) {
		 printf("Ocurrio un error.");
	 }

	 size = *rcp++;
	 horizontalpre = *rcp++;
	 verticalpre = *rcp++;

	 GETLONG(templ,rcp);
	 latitud = (templ - ((unsigned)1<<31));

	 GETLONG(templ,rcp);
	 longitud = (templ - ((unsigned)1<<31));

	 GETLONG(templ,rcp);
	 if (templ < referencealt) {
		 altitud = referencealt - templ;
		 altsign = -1;
	 } else {
        altitud = templ - referencealt;
        altsign = 1;
    }
    if (latitud < 0) {
        nortesur = 'S';
        latitud = -latitud;
    }
    else
        nortesur = 'N';

    latsecfrac = latitud % 1000;
    latitud = latitud / 1000;
    latsec = latitud % 60;
    latitud = latitud / 60;
    latmin = latitud % 60;
    latitud = latitud / 60;
    latdeg = latitud;

    if (longitud < 0) {
        esteoeste = 'W';
        longitud = -longitud;
    }
    else
        esteoeste = 'E';

    longsecfrac = longitud % 1000;
    longitud = longitud / 1000;
    longsec = longitud % 60;
    longitud = longitud / 60;
    longmin = longitud % 60;
    longitud = longitud / 60;
    longdeg = longitud;
    altfrac = altitud % 100;
    altmeters = (altitud / 100) * altsign;

    sizestr = precsize_ntoa(size);
    hpstr = precsize_ntoa(horizontalpre);
    vpstr = precsize_ntoa(verticalpre);

    printf("           IN     LOC     %d %.2d %.2d.%.3d %c %d %.2d %.2d.%.3d %c %d.%.2dm %sm %sm %sm \n", 
            latdeg, latmin, latsec, latsecfrac, nortesur, longdeg, longmin, longsec, longsecfrac, 
            esteoeste,altmeters, altfrac, sizestr, hpstr, vpstr);
}

/**
 * 	Metodo provisto por el RFC 1876 ????
**/
static char *precsize_ntoa(u_int8_t  prec){
    static unsigned int poweroften[10] = {1, 10, 100, 1000, 10000, 100000,
    1000000,10000000,100000000,1000000000};
    static char retbuf[sizeof("90000000.00")];
    unsigned long val;
    int mantissa, exponent;
    mantissa = (int)((prec >> 4) & 0x0f) % 10;
    exponent = (int)((prec >> 0) & 0x0f) % 10;
    val = mantissa * poweroften[exponent];
    (void) sprintf(retbuf,"%d.%.2d", val/100, val%100);
    return (retbuf);
}
