#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_STRING 100
#define TIPOCONSULTA_DEFECTO "-a"
#define TIPORESOLUCION_DEFECTO "-r"

struct parametrosConsulta;
void evaluarParametros(char* argv[], int argc);
