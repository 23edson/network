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
void insere_fix(msg *buf, int save, int saveID, time_t back);
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

msg *message_fix(msg *buf, int origem, int destino){
	
	buf->origem = origem;
	buf->destino = destino;
	return buf;
	
}

void enviarMsg(void){
	
	
  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);
  char buf[GETFILE];
  char message[GETFILE];
  int destino;
  char ipf[15];
  msg mensg;
  int id = 1;
  
  router *destRouter = NULL;
  usleep(300000);//espera 300ms
  while(1){
		printf("\n");
		printf("ROTEADOR NUMERO %d\n\n", myRouter->id);
		usleep(1000000);
		printf("Enviar msg para roteador destino:");
			scanf("%d", &destino);
	
		fgetc(stdin);
		//pthread_mutex_lock(&count_mutex);
		if(destino == myRouter->id){
			printf("\nEnviar para mim mesmo?\n");
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
					//mensg.entregue = 0;
					mensg.nextH = myConnect[myRouter->id-1].idImediato[i];
					//mensg.tentativas = 0;
					//mensg.timestamp = 0;
					mensg.ack = 0;
					mensg.pSize = 0;
					strcpy(mensg.ip, destRouter->ip);
					//mensg.parent = (int *)malloc(sizeof(int)*vertices);
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
		
			mensg.idMsg = id++;
			//if(!mensg.parent)puts("como");
			mensg.parent[mensg.pSize] = myRouter->id;
			mensg.pSize++;
			//printf("parent :%d  size : %d",mensg.parent[mensg.pSize-1],mensg.pSize);
			insereFila(&mensg);
		//puts("osdf");
		
			//printf("ID>::%d e %d\n", mensg.origem, mensg.destino);
			pthread_mutex_unlock(&count_mutex);
		
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
	
	//printf("tamanho : %d   end\n", buf->pSize);  
	if(buf->ack == 1){
		//puts("entrei nela");
		for(i = 0; i < vertices; i++){//puts("esrou");
			if(myConnect[myRouter->id-1].idVizinho[i] == buf->parent[buf->pSize]){
				buf->nextH = myConnect[myRouter->id-1].idImediato[i];
			
				if(!(destRouter = leInfos(rout_u, buf->nextH))){
					printf("Destino nao e valido\n");return 0;
				}
				break;
			}	
			
			
		}
		//printf("aq");
		//printf("porta : %d\n", destRouter->port);
		
	}
	else{//puts("foi por causa dela");
		for( i = 0; i < vertices; i++){//puts("evvrwibi");printf("xx%d", myRouter->id);
			//printf("i:%d\n", i);
			
			//if(!myConnect)puts("ati");else puts("bem legal");
			//printf("xxH %d\n", myConnect[0].idVizinho[0]);
			//printf("xxH %d\n", myConnect[0].idVizinho[1]);
			//printf("xxH %d\n", myConnect[0].idVizinho[2]);
			//printf("xxH %d\n", myConnect[0].idImediato[2]);
			//printf("xxH %d\n", myConnect[0].idVizinho[3]);
			
			 
			if(myConnect[myRouter->id-1].idVizinho[i] == buf->destino){//puts("eibimmmm");
				buf->nextH = myConnect[myRouter->id-1].idImediato[i];
			
				if(!(destRouter = leInfos(rout_u, buf->nextH))){
					printf("Destino nao e valido\n");return 0;
				}
				break;
			}
				//printf("next %d p %d \n", buf->nextH, destRouter->port); 
				
		}
	} 
	//if(!destRouter)puts("bug");else{printf("vteste :/ %d %d %d;",buf->destino, buf->nextH, destRouter->port);}
	//puts("outfff");
	
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
	if(!buf->ack)
		printf("\nRoteador : %d encaminhando msg #%d de %d bytes para roteador : %d\n", myRouter->id,buf->idMsg, strlen(buf->text)-1,destRouter->id); 
	else
		printf("\nRoteador : %d enviando confirmacao de pacote #%d para rot. %d\n", myRouter->id, buf->idMsg, destRouter->id);
	free(destRouter);
	return 1;
	
}

void server(void){ //Para receber as mensagens
	
	int s, recv_len,i,flag = 0;
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
		//printf("MSGGGGG AAAA: %d %d\n", mensg.pSize,mensg.destino);
		if((mensg.destino != myRouter->id) && (mensg.ack == 0)){ //caso este nao for o destino
			//printf("caso");
			//msg *r = malloc(sizeof(msg));
			
			for(i = 0; i < tamanho; i++){ //ignorar pacotes ja recebidos
				
				if(mensg.idMsg == filas[i].mesg->idMsg && mensg.nextH == myRouter->id){
					flag = 1;
					break;
				}
				
			}
			if(!flag){//se a mensagem nao é repetida
				msg *confr = malloc(sizeof(msg));
			
			
				mensg.parent[mensg.pSize++] = myRouter->id;
				confr = copyData(confr,&mensg);
				//printf("detr %d %d %d %d %d\n", confr->pSize, confr->destino, confr->idMsg, confr->nextH, confr->origem);
				insereFila(confr);
				//r = copyData(r,&mensg);
				//confr->ack = 1;
				//confr = message_fix(confr, mensg.nextH, mensg.origem);
				//int back = mensg.origem;
				//confr->origem = mensg.nextH;
				//confr->destino = back;
			
				//insereFila(r);
			
			}
			flag=0;
		}
		else{ //caso este for o destino
		//puts("aqui");
		
			
			
			msg *conf = malloc(sizeof(msg));
			if(!mensg.ack){ //se nao for msg de confirmacao
				//insereFila(conf);
					
				/*int back = mensg.origem;
				conf->origem = mensg.nextH;
				conf->destino = back;*/
				mensg.ack = 1;
				mensg.pSize--; 
				conf = copyData(conf, &mensg);
				//conf = message_fix(conf, mensg.nextH, mensg.origem);
				printf("\nRoteador : %d recebeu a msg de %d\n", myRouter->id, mensg.origem);
				printf("Msg: %s\n", mensg.text);
					
				insereFila(conf);
					
					
				//conf->entregue = 1;
				//conf = &mensg;
				//insereFila(conf);
				//printf("um: %d dios :%d", mensg.nextH, mensg.origem);
				//printf("ok");
			}
			else{//caso recebe a confirmacao
				//printf("mens %d %d\n", mensg.pSize, mensg.origem);
				for(i = 0; i < tamanho; i++){
					if(mensg.parent[mensg.pSize] == myRouter->id && filas[i].mesg->origem == mensg.origem){
						remove_fix(i);
						//puts("entrei nessa buraca\n");
						break;
					}
				}
				
				
				if(mensg.origem == myRouter->id){
					printf("\nMensagem confirmada!!\n");
					//printf("\nv : %d %d\n", mensg.idMsg, mensg.destino);
					//printf("msg %d\n", tamanho);
				
				}
				else{
					mensg.pSize--;
					conf = copyData(conf, &mensg);
					insereFila(conf);
				}
			}
		}
		pthread_mutex_unlock(&count_mutex);
	}
}
	


void serverControl(){
	//puts("zxsa");
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
			pthread_mutex_lock(&count_mutex);
		 
		 msg *conf = (msg *)malloc(sizeof(msg));
		 
	    // puts("prddmj");
	     //if(!filas[i].mesg)//puts("uma!!!!!!");
		 //usleep(400000);
		if(filas[i].mesg->ack){ //caso recebeu o pacote
			//printf("problema aq\n");
			//printf("t:%d %d\n", filas[i].mesg->origem, filas[i].mesg->destino);
			 encaminhaMsg(s,&controle,filas[i].mesg);
			 remove_f();
			 /*for(j = 0; j < tamanho; j++){
				 
			 }
			 int aux = filas[i].mesg->origem;
			 
			 //modifica para enviar a confirmacao
			 filas[i].mesg->origem = filas[i].mesg->destino;
			 filas[i].mesg->destino = aux;
			 */
			// puts("ok1");
			 
		 }
		 else if(filas[i].tentativas == 0){ //se a mensagem nao foi enviada, ou seja, veio do usuario
		// puts("orimero");
		 //printf("detr %d %d %d %d %d\n", filas[i].mesg->pSize, filas[i].mesg->destino, filas[i].mesg->idMsg, filas[i].mesg->nextH, filas[i].mesg->origem);
		 
			 if(encaminhaMsg(s,&controle,filas[i].mesg)==0){
			//	 puts("ffffffffff");
				 remove_f();//puts("ffffffffffddergw");
			 }
			 else{//puts("ffffffffffzzzzzzz");
			 
				filas[i].tentativas++;
				filas[i].timestamp = time(0);
				 //printf("T:%d\n", filas[i].tentativas);
				 save = filas[i].tentativas;
				 saveId = filas[i].id;
				 back = filas[i].timestamp;
				 conf = copyData(conf, filas[i].mesg);//puts("lipar");
				 remove_f();
				 insere_fix(conf, save, saveId, back);//puts("limpar");
				 //puts("fkowçççç");
			 }
		//puts("ok2");
		}
		else if(filas[i].tentativas > 0 && filas[i].tentativas < 3){
			// puts("pro2");
			// puts("tentativa mais");
			 double tempo = difftime(time(0), filas[i].timestamp);
			 
			 if(filas[i].tentativas < 3 && tempo > 2){ //max 3 tentativas
				printf("\nRetransmissao\n");
				if(encaminhaMsg(s,&controle, filas[i].mesg)==0){
					remove_f();
				}
				else{
					
					//filas[i].timestamp = time(0);
					filas[i].tentativas++;
					back = time(0);
					save = filas[i].tentativas;
					saveId = filas[i].id;
					conf = copyData(conf,filas[i].mesg);
					remove_f();
					
					insere_fix(conf,save,saveId,back); //coloca no final da fila
				 }
			 }
			 else if(filas[i].tentativas < 3 && tempo <= 2){ //Aprox 2s
				// encaminhaMsg(s,&controle, filas[i].mesg);
				 conf = copyData(conf,filas[i].mesg);
				 save = filas[i].tentativas;
				 back = filas[i].timestamp;
				 saveId = filas[i].id;
				 remove_f();
				
				 insere_fix(conf,save,saveId,back); //coloca no final da fila
				 //total aproximadamente de 6s para tentar enviar
			 }//puts("ok3");
		}
		else{//puts("pro34");
			printf("\nNao foi possivel encaminhar a mensagem\n");
			remove_f(); //Desiste de enviar
			//puts("ok45");
		}
		 pthread_mutex_unlock(&count_mutex);
		 
	 }
    
  }
  close(s);
  pthread_exit(NULL);
}

msg *copyData(msg *new, msg *buf){
	//Permite copiar uma mensagem
	//puts("te");
	int i;
	new->origem = buf->origem;
	new->destino = buf->destino;
	//new->entregue = buf->entregue;
	new->nextH = buf->nextH;
	strcpy(new->ip, buf->ip);
	strcpy(new->text, buf->text);
	//new->timestamp = buf->timestamp;
	//new->tentativas = buf->tentativas;
	new->ack = buf->ack;
	new->idMsg = buf->idMsg;
	new->pSize = buf->pSize;
	//puts("ola");
	for(i = 0 ; i < MAX_PARENT; i++)
		new->parent[i] = buf->parent[i];
	
	
	//puts("ok67");
	return new;
	
	
}
int iniciaFila(){
	//puts("kkkkkk");
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
	//puts("okk8");
	return 1;
}

void insereFila(msg *buf){
	//puts("pol");
	msg *nova = (msg *)malloc(sizeof(msg));
	
	if(!filas){
		printf("Impossivel inserir. Problema com a fila\n");
		return;
	}
	else{
		nova = copyData(nova,buf);
	//	puts("estamos");
		filas[tamanho].mesg = nova;
		
		if(tamanho == 0){
			filas[tamanho].id = 1;
			filas[tamanho].timestamp = filas[tamanho].tentativas =0;
			//filas[tamanho].mesg->idMsg = filas[tamanho].id = 1;
		}
		else{
			filas[tamanho].id = filas[tamanho-1].id+1;
			//filas[tamanho].mesg->idMsg = filas[tamanho].id;
			filas[tamanho].timestamp = filas[tamanho].tentativas =0;
		}
		tamanho++;
	}//puts("ok6557");
}

void insere_fix(msg *buf, int save, int saveID, time_t back){
//Para reinsercao
		//puts("ptty");
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
	//puts("kkkklj");
	
}

void remove_f(){
	//puts("lppp");
	int i;
	//msg copy;
	//free(filas[0].mesg);
	free(filas[0].mesg);
	filas[0].mesg = NULL; 
	for(i = 1; i < tamanho; i++){
		filas[i-1].mesg = filas[i].mesg;
	}
	tamanho--;
	//puts("cm");
}

void remove_fix(int i){
	//puts("llpo");
	int j;
	
	if(i == 0){ //caso for o primeiro, remove normalmente
		remove_f();
		
	}		
	else if( i == tamanho - 1){ //se for o ultimo
		//free(filas[0].mesg->parent);
		free(filas[0].mesg);
		filas[i].mesg = NULL;
		tamanho--;
		
	}
	else{ //caso for algum intermediario
		for(j = i; j < tamanho-1; j++){
			//free(filas[j].mesg);
			free(filas[j].mesg);
			filas[j].mesg = filas[j+1].mesg;
			
		}
		tamanho--;
	}
//puts("oook567");
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
  
  
  //pthread_join(tids[1], NULL);
  //pthread_join(tids[0], NULL);
  //pthread_join(tids[2], NULL);
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
