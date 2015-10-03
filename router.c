#include "funcs.h"
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define GETFILE 105

//gcc funcs.h readFiles.c router.c -D_REENTRANT -lpthread -o m

//int myId;
int vertices;
int tamanho = 0; //fila
tabela *myConnect = NULL;
router *myRouter = NULL;
fila_t *filas = NULL;

struct timeval tv;
tv.tv_sec = 0;
tv.tv_usec = 500000;

char rout[20] = "roteador.config";
char link_u[20] = "enlaces.config";

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;



void *enviarMsg(void *data){
	
	
  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);
  char buf[GETFILE];
  char message[GETFILE];
  int destino;
  char ipf[15];
  msg mensg;
  
  router *destRouter = NULL;
  
  while(1){
	printf("Roteador destino:");
		scanf("%d", &destino);
	
	if(!(destRouter = leInfos(rout_u, destino))) return 0;
  
	// pthread_mutex_lock(&count_mutex);
	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
			die("socket");
    }
    
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(destRouter->port);
     
    if (inet_aton(destRouter->ip , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
	//pthread_mutex_unlock(&count_mutex);
    
        
		
		printf("\nmsg: ");
        //gets(message);
        scanf(" %s",message);
        
        mensg.id = myRouter->id;
        mensg.destino = destino;
        //strcpy(mensg.ip,ipf);
        strcpy(mensg.text, message);
     //   pthread_mutex_unlock(&count_mutex);
      
         
        //send the message
        if (sendto(s, &mensg, sizeof(mensg) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
         
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
       // memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
       // if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
       // {
         //   die1("recvfrom()");
        //}
         
        //puts(buf);
    }
 
    close(s);
    pthread_exit(NULL);
	
}

void encaminhaMsg(int s, struct *sockaddr_in etc, msg *buf){ //
	//struct sockaddr_in si_other;
	int s;
	
	int t = sizeof(*etc);
	etc->sin_port = htons(buf->port);
	
	/*if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("ERRO: encaminha_msg ... SOCKET .\n");
		return;
	}
	memset((char *) &si_other, 0, sizeof(si_other));
	*/
	//si_other.sin_family = AF_INET;
	
	if (inet_aton(buf->ip , &etc.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() error\n");
		exit(1);
	}
	
	if (sendto(s, buf, sizeof(msg), 0, (struct sockaddr *)etc, t) == -1) {
		printf("Nao foi possivel encaminhar a mensagem()...\n");
	}
	
	
}

void server(void){ //Para receber as mensagens
	
	int s, recv_len;
	struct sockaddr_in si_me, si_other;
	int slen = sizeof(si_other);
	msg mensg;
	
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("Socket do servidor .\n");
		exit(1);
	}

	memset((char *) &si_me, 0, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(myRouter.port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s , (struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
		printf("bind(). Porta ocupada?.\n");
		exit(1);
	}
	
	while(1){
		
		fflush(stdout);
		memset((char *)mensg, 0, sizeof(msg));
		
		if ((recv_len = recvfrom(s, &mensg, sizeof(msg), 0, (struct sockaddr *)&si_other, &slen)) == -1) {
			printf("recvfrom()");
			exit(1);
		}
		
		if(buf.destino != myRouter.id){ //caso este nao for o destino
			msg *r = malloc(sizeof(msg));
			r->id = mensg.id;
			r->destino = mensg.destino;
			strcpy(r->ip,mensg.ip);
			strcpy(r->text,mensg.text); 
			insereFila(r);
			
		}
		else{ //caso este for o destino
			
			printf("Roteador : %d recebeu a msg de %d", myRouter->id, mensg.id);
			printf("msg: %s", mensg.text);
			mensg.entregue = 1;
			msg *conf = malloc(sizeof(msg));
			conf->id = mensg.id;
			conf->destino = mensg.destino;
			strcpy(conf->ip,mensg.ip);
			strcpy(conf->text,mensg.text); 
			//conf = &mensg;
			insereFila(conf);
			
			
		}
		
	}
	
}

void *serverControl(){

  struct sockaddr_in controle;
  int s;
  //int a;
  msg buf;
 // char buffer[500];
   //300000 ms
   
  if(!iniciaFila(vertices))return;
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("Socket de controle.\n");
		return;
   }

	controle.sin_family = AF_INET; //tambem faz algo
	memset((char *) &controle, 0, sizeof(controle));
	//si_other.sin_family = AF_INET;
  

  //memset((char *)&entrada, 0, sizeof(entrada));
  //memset((char *)&cliente, 0, sizeof(cliente));
  //pthread_mutex_lock(&count_mutex);
  
  /*entrada.sin_port = htons(myRouter->port); //socket do roteador
  entrada.sin_addr.s_addr = htonl(INADDR_ANY); //faz algo

  if( bind ( s, (struct sockaddr*)&entrada,sizeof(entrada))==-1){
		printf("Erro bind()\n");exit(1);
  } //'linkagem' do socket com a porta especificada*/

  while(1){
	  
	  
	 if(tamanho == 0)continue; //fila vazia
	 
	 for(i = 0; i < tamanho; i++){
		 if(filas[i]->mesg.entregue == 1){
			 encaminhaMsg(s,controle,filas[i]->mesg);
			 remove();
			 
		 }
		 else if(tentativas == 0){ //se a mensagem nao foi enviada, ou seja, veio do usuario
			 encaminhaMsg(s,controle,filas[i]->mesg);
			 filas[i]->mesg = time(0);
			 filas[i]->mesg.tentativas++;
			 
		 }
		 else if(tentativas > 0){
			 
			 double tempo = difftime(time(0), filas[i]->mesg.time);
			 
			 if(tentativas < 4){ //max 4 tentativas
				encaminhaMsg(s,controle, filas[i]->mesg);
				filas[i]->mesg = time(0);
				filas[i]->mesg.tentativas++;
			 }
			 else if( tempo > 2){//TimeOut 2s
				 printf("Retransmissao::\n");	
				 encaminhaMsg(s,controle, filas[i]->mesg);
				 filas[i]->mesg = time(0);
				filas[i]->mesg.tentativas++
			 }
		 }
		 else{
			printf("Nao foi enviar a mensagem");
		 }
		 
	 }
    if((recv_len = recvfrom(s, &buf, sizeof(msg), 0, (struct sockaddr *)&saida, &tam))==-1){
		printf("Erro recvfrom ()\n");
		exit(1); 
		
	}

    if(buf.destino != myId){ //se a mensagem nao eh para este roteador
		
		printf("Roteador :%d encaminhando msg : %d para destino: %d\n", myId, strlen(buf,text),buf.destino);
		
		(buf, recv_len);
		//cliente.sin_family = AF_INET;
		//cliente.sin_port = htons(buf.destino);
		//cliente.sin_addr.s_addr = htonl(INADDR_ANY);
		//pthread_mutex_unlock(&count_mutex);
		//if((recv_len = recvfrom(a, &buf, sizeof(msg), 0, (struct sockaddr *)&saida, &tam))==-1)die("recvfrom()");
		 //if ( (a=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		//{
			//die("socket");
		//}
		//tam = sizeof(cliente);
		 
		/*if(sendto(a, buffer, 500, 0, (struct sockaddr *)&cliente, tam)==-1)
			die("sento() next router");
			
		memset(buffer,'\0', 50);
		strcpy("ack", buffer);
		
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buffer, 50, 0, (struct sockaddr *) &cliente, &tam) == -1)
        {
            die("recvfrom() from another gateway");
        }*/

	}
    else{ //se for, apenas confirma o recebimento e responde
      memset(buffer, '\0', 50);
      strcpy("ack", buffer);
      
      printf("Roteador :%d recebeu a msg enviada por: %d\n", myId, buf.destino);
      printf("msg : %s\n\n", buf.text);

      if(sendto(s, buffer, recv_len, 0, (struct sockaddr *)&saida, tam)==-1)
      die("sento()");
      
     

    }

  }
  close(s);
  pthread_exit(NULL);
}
int iniciaFila(int tam){
	
	fila_t *filas = malloc(sizeof(fila_t)*tam);
	if(!filas){printf("Queue problem\n");return 0}
	filas->mesg = NULL;
	filas->next = NULL;
	return 1;
}

void insereFila(msg *buf){
	
	
	filas[tamanho]->mesg = buf;
	if(tamanho == 0)
		filas[tamanho].id = 1;
	else
		filas[tamanho].id = filas[tamanho-1].id+1;
	tamanho++;
}

void remove(){
	
	int i;
	msg copy;
	
	for(i = 1; i < tamanho; i++){
		if(i == 0){
			filas[i-1].mesg = filas[i].mesg;
		}
	}
	tamanho--;
}


int main(int argc, char *arq[]){
  //int i,j;
  pthread_t tids[2];
  
  //pthread_create(&tids[0], NULL, server, NULL);
  //pthread_create(&tids[1], NULL, enviarMsg, NULL);
   
  
  int  routerId = atoi(arq[1]);
 
  
  
  //Leitura dos argumentos via terminal
  myId = routerId; //id globalmente visivel
  
  //printf("%d %s %s", routerId, rout, link);
 
  if(!(myConnect = leEnlaces(link, vertices =countIn(rout))))return 0;
    
     
  if(!(myRouter = leInfos(rout_u, routerId))) return 0;
  
  
  printf("%d\n", myRouter->id);
  printf("%d\n", myRouter->port);
  
 pthread_create(&tids[1], NULL, enviarMsg, NULL);
  pthread_create(&tids[0], NULL, server, NULL);
  
  //pthread_join(tids[1], NULL);
  //pthread_join(tids[0], NULL);
  

  /*  printf("id : %d \n",myRouter->id);
    printf("port: %d \n",myRouter->port);
    printf("ip : %s \n\n", myRouter->ip);
  int count = vertices;
  int i,j;
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
	 
   // server(myRouter,myConnect);
     
  return 0;
}



