#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/nameser.h>
#include <sys/types.h> 
#include <limits.h>
#include <errno.h>
#include <stdint.h>

void consulta_LOC(unsigned char *reader);