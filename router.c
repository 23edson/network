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
#include <time.h>

#define GETFILE 105
#define MAXFILA 50
//gcc funcs.h readFiles.c router.c -D_REENTRANT -lpthread -o m

void remove_f();
int iniciaFila();
void insereFila(msg *buf);
void remove_fix(int i);
msg *copyData(msg *new, msg *buf);

//int myId;
int vertices;
//int maxFila = 50
int tamanho = 0; //fila
tabela *myConnect = NULL;
router *myRouter = NULL;
fila_t *filas = NULL;

//struct timeval tv;
//tv.tv_sec = 0;
//tv.tv_usec = 500000;

char rout_u[20] = "roteador.config";
char link_u[20] = "enlaces.config";

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;



void enviarMsg(void){
	
	
  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);
  char buf[GETFILE];
  char message[GETFILE];
  int destino;
  char ipf[15];
  msg mensg;
  
  router *destRouter = NULL;
  usleep(300000);//espera 300ms
  while(1){
	
	printf("Enviar msg para roteador destino:");
		scanf("%d", &destino);
	printf("\n");
	fgetc(stdin);
	//pthread_mutex_lock(&count_mutex);
	if(destino == myRouter->id){
		printf("Enviar para mim mesmo?\n");
		continue;
	}
	//fflush(stdin);
	//pthread_mutex_unlock(&count_mutex);
	if(!(destRouter = leInfos(rout_u, destino))){ printf("Destino invalido\n");}
	
	else{
		
		for( i = 0; i < vertices; i++){
			if(myConnect[myRouter->id-1].idVizinho[i] == destino){
				mensg.origem = myRouter->id;
				mensg.destino = destino;
				mensg.entregue = 0;
				mensg.nextH = myConnect[myRouter->id-1].idImediato[i];
				mensg.tentativas = 0;
				mensg.timestamp = 0;
				mensg.ack = 0;
				strcpy(mensg.ip, destRouter->ip);
				break;
			}
			
		}
		// pthread_mutex_lock(&count_mutex);
		printf("Msg: ");
			//gets(message);
		fgets(message, 100,stdin);
			
		//mensg.id = myRouter->id;
		//mensg.destino = destino;
			//strcpy(mensg.ip,ipf);
		strcpy(mensg.text, message);
		 
		pthread_mutex_lock(&count_mutex);
		if(!filas)
			if(!iniciaFila())return;
		
		insereFila(&mensg);
	    printf("ID>::%d e %d\n", mensg.origem, mensg.destino);
		pthread_mutex_unlock(&count_mutex);usleep(100000);
		}
	}
	 
		close(s);
		//pthread_exit(NULL);
	
}
int encaminhaMsg(int s, struct sockaddr_in *etc, msg *buf){ //
	//struct sockaddr_in si_other;
	//int s;
	int i;
	
	router *destRouter = NULL;
	int t = sizeof(*etc);
	//printf("dest %d\n", buf->destino);
	//if(!(destRouter = leInfos(rout_u, destino))){ printf("Destino invalido\n");}
	for( i = 0; i < vertices; i++){
			if(myConnect[myRouter->id-1].idVizinho[i] == buf->destino){
				buf->nextH = myConnect[myRouter->id-1].idImediato[i];
				
				if(!(destRouter = leInfos(rout_u, buf->nextH))){
					printf("Destino nao e valido\n");return 0;
				}
				printf("next %d p %d \n", buf->nextH, destRouter->port); 
				break;
			}
	} 
	
	etc->sin_port = htons(destRouter->port);
	
	/*if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("ERRO: encaminha_msg ... SOCKET .\n");
		return;
	}
	memset((char *) &si_other, 0, sizeof(si_other));
	*/
	//si_other.sin_family = AF_INET;
	
	if (inet_aton(destRouter->ip , &etc->sin_addr) == 0) {
		fprintf(stderr, "inet_aton() error\n");
		return 0;
	}
	
	if (sendto(s, buf, sizeof(msg), 0, (struct sockaddr *)etc, t) == -1) {
		printf("Nao foi possivel encaminhar a mensagem()...\n");
		return 0;
	}
	printf("Roteador : %d encaminhando msg de %d bytes para roteador : %d\n", myRouter->id, strlen(buf->text),destRouter->id); 
	free(destRouter);
	return 1;
	
}

void server(void){ //Para receber as mensagens
	
	int s, recv_len,i;
	struct sockaddr_in si_me, si_other;
	int slen = sizeof(si_other);
	msg mensg;
	
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("Problema com Socket do servidor .\n");
		exit(1);
	}

	memset((char *) &si_me, 0, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(myRouter->port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	//printf("T:%d %s %d\n\n", myRouter->id, myRouter->ip, myRouter->port);
	if (bind(s , (struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
		printf("bind(). Porta ocupada?.\n");
		exit(1);
	}
	pthread_mutex_lock(&count_mutex);
	if(!iniciaFila()){
		printf("Impossivel alocar a fila\n");
		exit(1);
	}
	pthread_mutex_unlock(&count_mutex);
	while(1){
		
		fflush(stdout);
		memset((char *)&mensg, 0, sizeof(msg));
		
		if ((recv_len = recvfrom(s, &mensg, sizeof(msg), 0, (struct sockaddr *)&si_other, &slen)) == -1) {
			printf("recvfrom() ");
			exit(1);
		}
		pthread_mutex_lock(&count_mutex);
		if(mensg.destino != myRouter->id){ //caso este nao for o destino
			msg *r = malloc(sizeof(msg));
			msg *confr = malloc(sizeof(msg));
			
			confr = copyData(confr,&mensg);
			confr->ack = 1;
			int back = mensg.origem;
			confr->origem = mensg.nextH;
			confr->destino = back;
			r = copyData(r,&mensg);
			
			insereFila(confr);
			insereFila(r);
			
		}
		else{ //caso este for o destino
		//puts("aqui");
			 
			if(!mensg.ack){ //se nao for msg de confirmacao
					//insereFila(conf);
				msg *conf = malloc(sizeof(msg));
				int back = mensg.origem;
				conf->origem = mensg.nextH;
				conf->destino = back;
				conf = copyData(conf, &mensg);
				insereFila(conf);
				printf("Roteador : %d recebeu a msg de %d\n", myRouter->id, mensg.origem);
				printf("Msg: %s\n", mensg.text);
				//conf->entregue = 1;
				//conf = &mensg;
				//insereFila(conf);
			}
			else{//caso recebe a confirmacao
				printf("Msg foi encaminhada parao o proximo roteador no caminho!!\n");
				/*printf("\n%d %d %d\n", filas[i].mesg->idMsg, mensg.idMsg, filas[i].mesg->destino);*/
 				for(i = 0; i < tamanho; i++){
					if(filas[i].mesg->idMsg == mensg.idMsg && filas[i].mesg->destino == myRouter->id)
						remove_fix(i);
						break;
					
				}
				
			}
				
		}
		pthread_mutex_unlock(&count_mutex);
	}
	
}

void serverControl(){

  struct sockaddr_in controle;
  time_t back;
  int save,saveId;
  int s,i,j;
  //int a;
  msg buf;
 // char buffer[500];
   //300000 ms
   pthread_mutex_lock(&count_mutex);
  if(!iniciaFila())return;
  
  pthread_mutex_unlock(&count_mutex);
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
	  usleep(500000);//espera 500ms
	  
	 if(tamanho == 0)continue; //fila vazia
	 
	 for(i = 0; i < tamanho; i++){
		 //printf("tammmm %d %d\n", tamanho, filas[0].tentativas);
		 //usleep(100000);
		 pthread_mutex_lock(&count_mutex);
		 msg *conf = (msg *)malloc(sizeof(msg));
	     
		 
		/* if(filas[i].mesg->ack){ //caso recebeu o pacote
			 for(j = 0; j < tamanho; j++){
				 
			 }
			 int aux = filas[i].mesg->origem;
			 
			 //modifica para enviar a confirmacao
			 filas[i].mesg->origem = filas[i].mesg->destino;
			 filas[i].mesg->destino = aux;
			 encaminhaMsg(s,&controle,filas[i].mesg);
			 remove_f();
			 
		 }*/
		 if(filas[i].tentativas == 0){ //se a mensagem nao foi enviada, ou seja, veio do usuario
		 puts("orimero");
		 
			 encaminhaMsg(s,&controle,filas[i].mesg);
			 filas[i].tentativas++;
			 filas[i].timestamp = time(0);
			 save = filas[i].tentativas;
			 saveId = filas[i].id;
			 back = filas[i].timestamp;
			 conf = copyData(conf, filas[i].mesg);
			 remove_f();
			 insere_fix(conf, save, saveId, back);
		 }
			
		 else if(filas[i].tentativas > 0 && filas[i].tentativas < 3 && filas[i].mesg->ack == 0){
			 
			 
			 double tempo = difftime(time(0), filas[i].timestamp);
			 
			 if(filas[i].tentativas < 3 && tempo > 2){ //max 3 tentativas
				printf("Retransmissao\n");
				encaminhaMsg(s,&controle, filas[i].mesg);
				//filas[i].timestamp = time(0);
				filas[i].tentativas++;
				conf = copyData(conf,filas[i].mesg);
				save = filas[i].tentativas;
				back = time(0);
				saveId = filas[i].id;
				remove_f();
				
				insere_fix(conf,save,saveid,back); //coloca no final da fila
			 }
			 else if(filas[i].tentativas < 3 && tempo <= 2){ //2s
				// encaminhaMsg(s,&controle, filas[i].mesg);
				 conf = copyData(conf,filas[i].mesg);
				 save = filas[i].tentativas;
				 back = filas[i].timestamp;
				 saveID = filas[i].id;
				 remove_f();
				
				 insere_fix(conf,save,saveId,back); //coloca no final da fila
				 
			 }
		}
		else{
			printf("Nao foi enviar a mensagem\n");
			remove_f(); //Desiste de enviar
		}
		 pthread_mutex_unlock(&count_mutex);
		 
	 }
    
  }
  close(s);
  pthread_exit(NULL);
}

msg *copyData(msg *new, msg *buf){
	//Permite copiar uma mensagem
	new->origem = buf->origem;
	new->destino = buf->destino;
	new->entregue = buf->entregue;
	new->nextH = buf->nextH;
	strcpy(new->ip, buf->ip);
	strcpy(new->text, buf->text);
	new->timestamp = buf->timestamp;
	new->tentativas = buf->tentativas;
	new->ack = buf->ack;
	new->idMsg = buf->idMsg;
	return new;
	
	
}
int iniciaFila(){
	int i;
	filas = malloc(sizeof(fila_t)*MAXFILA);
	if(!filas){
		printf("Falha na alocacao\n");
		return 0;
	}
	for(i = 0; i < vertices; i++){
		filas[i].mesg = NULL;
		filas[i].id = 0;
		filas[i].timestamp = 0;
		filas[i].tentativas = 0;
	}
	//filas->next = NULL;
	return 1;
}

void insereFila(msg *buf){
	
	msg *nova = (msg *)malloc(sizeof(msg));
	
	if(!filas){
		printf("Impossivel inserir. Problema com a fila\n");
		return;
	}
	else{
		nova = copyData(nova,buf);
		
		filas[tamanho].mesg = nova;
		if(tamanho == 0){
			filas[tamanho].id = filas[tamanho].mesg->idMsg = 1;
			//filas[tamanho].timestamp = filas[tamanho].tentativas =0;
			//filas[tamanho].mesg->idMsg = filas[tamanho].id = 1;
		}
		else{
			filas[tamanho].id = filas[tamanho-1].id+1;
			filas[tamanho].mesg->idMsg = filas[tamanho].id;
			//filas[tamanho].timestamp = filas[tamanho].tentativas =0;
		}
		tamanho++;
	}
}

void insere_fix(msg *buf, int save, int saveID, time_t back){
//Para reinsercao	
	msg *nova = (msg *)malloc(sizeof(msg));
	
	if(!filas){
		printf("Impossivel inserir. Problema com a fila\n");
		return;
	}
	
	else{
		nova = copyData(nova,buf);
		
		filas[tamanho].mesg = nova;
		filas[tamanho].id = saveID;
		filas[tamanho].tentativas = save;
		filas[tamanho].timestamp = back;
		
		tamanho++;
	}
	
	
}

void remove_f(){
	
	int i;
	//msg copy;
	free(filas[0].mesg);
	filas[0].mesg = NULL; 
	for(i = 1; i < tamanho; i++){
		filas[i-1].mesg = filas[i].mesg;
	}
	tamanho--;
}

void remove_fix(int i){
	
	int j;
	
	if(i == 0){ //caso for o primeiro, remove normalmente
		remove_f();
		
	}		
	else if( i == tamanho - 1){ //se for o ultimo
		free(filas[i].mesg);
		filas[i].mesg = NULL;
		tamanho--;
		
	}
	else{ //caso for algum intermediario
		for(j = i; j < tamanho-1; j++){
			free(filas[i].mesg);
			filas[i].mesg = filas[i+1].mesg;
			
		}
	}
	return;


}


int main(int argc, char *arq[]){
  //int i,j;
  pthread_t tids[3];
  
  //pthread_create(&tids[0], NULL, server, NULL);
  //pthread_create(&tids[1], NULL, enviarMsg, NULL);
   
  if(!arq[1]){printf("Problema com o id do roteador\n");return 0;}
  int  routerId = atoi(arq[1]);
  //if(!routerId){}
  
  
  //Leitura dos argumentos via terminal
  //myId = routerId; //id globalmente visivel
  
  //printf("%d %s %s", routerId, rout, link);
 
  if(!(myConnect = leEnlaces(link_u, vertices =countIn(rout_u))))return 0;
    
     
  if(!(myRouter = leInfos(rout_u, routerId))) return 0;
  
  int count = vertices;
  int i,j;
  for(i = routerId-1; i < routerId; i++){                                                 
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
    }printf("\n\n"); }
  
  //printf("%d\n", myRouter->id);
  //printf("%d\n", myRouter->port);
  
  
  pthread_create(&tids[0], NULL, (void *)server, NULL);
  pthread_create(&tids[1], NULL, (void *)enviarMsg, NULL);
  pthread_create(&tids[2], NULL, (void *)serverControl, NULL);
  
  pthread_join(tids[1], NULL);
  pthread_cancel(tids[0]);
  pthread_cancel(tids[2]);
  pthread_join(tids[0], NULL);
  pthread_join(tids[2], NULL);
  
 //pthread_create(&tids[1], NULL, enviarMsg, NULL);
  //pthread_create(&tids[0], NULL, server, NULL);
  
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



