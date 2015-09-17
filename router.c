OB#include"funcs.h"
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>

#define GETFILE 100

int myId;

void die(char *s){
  
  perror(s);exit(1);
}

void client(msg buf, router *myRouter, tabela *myConnect){

  struct sockaddr_in entrada, saida;
  int s, i, slen=sizeof(saida);
  int dest,porta;
  char ip[15];
  if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) die("socket");

  memset((char *)&saida, 0,sizeof(saida));
  
  for(i = 0; i < vertices; i++){
    if(i == myId-1){
      dest = myConnect[myId-1].idImediato[i];
      porta = myRouter[dest-1].port;
      strcpy(ip, myRouter[dest-1].ip);
      break;
    }
  }


  saida.sin_family = AF_INET;
  saida.sin_port = htons(porta);
  
  if(inet_aton(ip, &saida.sin_addr)==0){
    fprintf(stderr,"inet_aton() failed\n");
      exit(1);

  }

  /*for(i = 0; i < vertices; i++){
    if(i == myRounter->id){
      dest = myConnect[myRounter->id]->idImediato[i];
      break;
    }
    }*/
  //envia
  if(sendto(s, &buf, sizeof(msg), 0, (struct sockaddr *)&saida, slen)==-1) die("sendto()");

  printf("Roteador :%d encaminhando mensagem de 100 caracter para destino: %d", myId, dest);
  //espera resposta
  memset(buf.text, '\0', 100);
  if(recvfrom(s, &buf, sizeof(msg), 0, (struct sockaddr *)&saida, &slen)==-1)die("recvfrom()");
  if(strcmp(buf.text, "ack")==0)
  printf("Msg do roteador : %d para %d foi confirmada", myId, dest);

  close(s);
}

void server(router *myRouter, tabela *myConnect){

  struct sockaddr_in entrada, saida;
  int s, i, tam = sizeof(saida), recv_len;
  msg buf;

  if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) die("socket");

  memset((char *)&entrada, 0, sizeof(entrada));

  entrada.sin_family = AF_INET;
  entrada.sin_port = htons(myRouter->port);
  entrada.sin_addr.s_addr = htonl(INADDR_ANY);

  if( bind ( s, (struct sockaddr*)&entrada,sizeof(entrada))==-1)die("bind");

  while(1){
    memset(buf.text, '\0', 100);

    if((recv_len = recvfrom(s, buf.text, 100, 0, (struct sockaddr *)&destino, tam))==-1)die("recvfrom()");

    if(buf.id == myId)
      client(buf, myRouter, myConnect);

    else if(buf.destino == myId){
      memset(buf.text, '\0', 100);
      strcpy("ack", buf.text);

      if(sendto(s, buf.text, recv_len, 0, (struct sockaddr *)&destino, tam)==-1)
      die("sento()");

    }

  }
  close(s);
}



int main(int argc, char *arq[]){
  //int i,j;
  int  routerId = atoi(arq[1]);
  char *rout = "roteador.config";
  char *link = "enlaces.config";
  tabela *myConnect = NULL;
  //Leitura dos argumentos via terminal
  myId = routerId; //id globalmente visivel
  router *myRouter = NULL;
  //printf("%d %s %s", routerId, rout, link);
  
  if(!(myConnect = leEnlaces(link, vertices =countIn(rout))))return 0;
     if(!(myRouter = leInfos(rout, routerId))) return 0;

     //printf("id : %d \n",myRouter->id);
     //printf("port: %d \n",myRouter->port);
     //printf("ip : %s \n", myRouter->ip);
  /*int count = vertices;
  for(i = 0; i < count; i++){                                                 
    for(j = 0; j  <count; j++){                                                 
      if(i!=j)                                                                  
        printf("%d ", myConnect[i].idVizinho[j]);                               
    }printf("\n");                                                              
    for(j = 0; j  <count; j++){                                                 
      if(i!=j)                                                                  
        printf("%d ", myConnect[i].custo[j]);                                   
    }printf("\n");                                                              
    for(j = 0; j  <count; j++){                                                 
      if(i!=j)                                                                  
        printf("%d ", myConnect[i].idImediato[j]);                              
    }printf("\n"); } */

     server(myRouter,myConnect);
     
  return 0;
}
