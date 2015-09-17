#include<stdio.h>
#include<stdlib.h>

#define CONST 40 //constante para tamanho de arquivo
#define IP 15

int vertices;

typedef struct tab{
  
  int *idVizinho;
  int *custo;
  int *idImediato;
}tabela;

typedef struct rt{
  
  int id;
  int port;
  char ip[IP];
}router;

typedef struct mensagem{

  int id;
  int destino;
  char ip[IP];
  char text[105];
}msg;

tabela *leEnlaces( char enl[CONST], int count);
router *leInfos(char rout[CONST], int id);
int countIn(char rot[CONST]);
