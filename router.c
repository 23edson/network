#include "funcs.h"
#define GETFILE 100


int main(int argc, char *arq[]){
  //int i,j;
  int  routerId = atoi(arq[1]);
  char *rout = arq[2];
  char *link = arq[3];
  tabela *myConnect = NULL;
  //Leitura dos argumentos via terminal

  router *myRouter = NULL;
  //printf("%d %s %s", routerId, rout, link);
  
  myConnect = leEnlaces(link, vertices =countIn(rout));
  myRouter = leInfos(rout, routerId);

  printf("id : %d \n",myRouter->id);
  printf("port: %d \n",myRouter->port);
  printf("ip : %s \n", myRouter->ip);
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
    }printf("\n");                                                              
                                                                                
    } */                        
  return 0;
}
