#include <stdio.h>
#include <stdlib.h>

#define CONST 40 //constante para tamanho de arquivo
#define IP 15

int vertices;

//tabela de roteamento
typedef struct tab{
  
  int *idVizinho; //vertice
  int *custo;   //custo minimo
  int *idImediato; //proximo vertice no caminho até idVizinho
}tabela;

typedef struct mensagem{
  int idMsg; //identificador da msg
  int origem; //quem enviou
  int destino; //para onde vai
  int entregue; //flag diz que chegou ao destino
  int nextH; //próximo roteador no caminho
  char ip[IP]; //ip de quem enviou
  char text[105]; //mensagem
  time_t timestamp;
  int tentativas; //tentativas de envio
  int ack; //flag para confirmacao
}msg;

typedef struct fila{
	time_t timestamp;
	int tentativas;
	int id;
	msg *mesg;
}fila_t;

typedef struct rt{
  
  int id;  //id do roteador
  int port; //porta associada ao socket
  char ip[IP]; //ip
}router;



tabela *leEnlaces( char enl[CONST], int count);
router *leInfos(char rout[CONST], int id);
int countIn(char rot[CONST]);
void enviarMsg(void);
void server(void);
void serverControl(void);
