#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct RES_RECORD answers[20];

void logicaAnswer(int i, int finalizar, struct DNS_HEADER *dns,
    struct RES_RECORD answers[20], unsigned char buf[65536], unsigned char *reader);
void mostrarAnswers(int i, int finalizar, struct DNS_HEADER *dns, struct RES_RECORD answers[20], unsigned char buf[65536], unsigned char *reader);