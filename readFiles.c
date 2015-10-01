//Dijktra code
//http://www.vivaolinux.com.br/script/Algoritmo-de-Dijkstra

#include <string.h>
#include <math.h>
#include "funcs.h"

#define CONST 40
#define MAX 200


//vetor de custos e adjacencias
int *cost;


int countIn(char rot[CONST]){
  //Conta a qtd de vértice 
  FILE *arq;
  int i,adj;
  int count = 0;
  char ip[CONST];
  
  if(!(arq = fopen(rot,"r"))){
    printf("Nao consegui abrir a bagaça\n");
    return 0;
  }
  while(fscanf(arq,"%d %d %s", &i, &adj, ip)!=EOF)count++;
	
  fclose(arq);
  return count;

}

tabela *dijkstra(int vertex, int font, int recpt, tabela *myConnect){

  int i,v,flag=0;
  
  int *ant, *tmp, *new;
  
  double menor;
  double *dist;
  
  ant = calloc(vertex, sizeof(int *));
  tmp = calloc(vertex, sizeof(int *));
  new = calloc(vertex, sizeof(int *));
  dist = (double *)malloc(sizeof(double)*vertex);
  if(!(ant || tmp || new))return NULL;

  for(i = 0; i < vertex; i++){
    if(cost[(font-1)*vertex + i] != -1){
      ant[i] = font-1;
      dist[i] = cost[(font-1)*vertex+i];
    }
    else{
      ant[i] = -1;
      dist[i] = HUGE_VAL; //infinito
    }
    new[i] = 0;
  }
  new[font-1] = 1;
  dist[font-1] = 0; //dist dele mesmo é zero
  
  do{

    //procura o vertices de menor custo para entra no vetor new
    menor = HUGE_VAL;
    for(i=0;i<vertex;i++){
      if(!new[i])
	if(dist[i]>=0 && dist[i] < menor){
	  menor=dist[i];
	  v=i;
	}
    }
    //calcula as distancias dos vizinhos a partir dos vertices em new
    if(menor != HUGE_VAL && v!= recpt-1){
      new[v] = 1;
      for(i=0;i<vertex;i++)
	if(!new[i]){
	  if(cost[v*vertex+i]!= -1 && dist[v] + cost[v*vertex+i] < dist[i]){
	    dist[i] = dist[v] + cost[v*vertex+i];
	    ant[i] = v;
	  }
	}
    }
  }while(v!= recpt-1 && menor!=HUGE_VAL);
  
  // puts("oo");
  //  printf("i>%d recpi  %d > %d ",i, font,recpt);
  if(menor != HUGE_VAL){
    //printf("i>%d recpi > %d ", i,recpt); 
    i = recpt;
    i = ant[i-1];
    while(i != -1){
      tmp[flag] = i+1;
      flag++;
      i = ant[i];
    }
    i = flag;
    // printf("i :===  %d ", i);
    
    //printf("\nCusto:  %d,f:%d, r:%d\n",(int) dist[recpt-1],font-1,recpt-1);

    //  printf(" i>%d recpi  %d > %d > %d > %d ",i, font,recpt, (int) dist[recpt-1], tmp[flag-2]);
    
    myConnect[font-1].idVizinho[recpt-1] = recpt;
    myConnect[font-1].custo[recpt-1] = (int) dist[recpt-1];
    if( flag > 1)
      myConnect[font-1].idImediato[recpt-1] = tmp[flag-2];
    else
      myConnect[font-1].idImediato[recpt-1] = recpt;

    //puts("here");
  }

  free(dist);free(ant);free(tmp);free(new);

  return myConnect;
}


/*void dijkstra(int vertices,int origem,int destino)
{
  int i,v, cont = 0;
  int *ant, *tmp;
  int *z;     //vertices para os quais se conhece o caminho minimo 
  double min;
double dist[vertices]; // vetor com os custos dos caminhos 


  // aloca as linhas da matriz                                                                                                         
  ant = calloc (vertices, sizeof(int *));                                                                                                 
  tmp = calloc (vertices, sizeof(int *));                                                                                                 
  if (ant == NULL) {                                                                                                                      
    printf ("** Erro: Memoria Insuficiente **");                                                                                    
    exit(-1);                                                                                                                       
  }                                                                                                                                       
                                                                                                                                                
  z = calloc (vertices, sizeof(int *));                                                                                                   
  if (z == NULL) {                                                                                                                        
    printf ("** Erro: Memoria Insuficiente **");                                                                                    
    exit(-1);                                                                                                                       
  }                                                                                                                                       
                                                                                                                                                
  for (i = 0; i < vertices; i++) {                                                                                                        
    if (cost[(origem - 1) * vertices + i] !=- 1) {                                                                                
      ant[i] = origem - 1;                                                                                                    
      dist[i] = cost[(origem-1)*vertices+i];                                                                                
    }                                                                                                                               
    else {                                                                                                                          
      ant[i]= -1;                                                                                                             
      dist[i] = HUGE_VAL;                                                                                                     
    }                                                                                                                               
    z[i]=0;                                                                                                                         
  }
  z[origem-1] = 1;
  dist[origem-1] = 0;

  // Laco principal 
  do {

    // Encontrando o vertice que deve entrar em z                                                                                 
    min = HUGE_VAL;                                                                                                                 
    for (i=0;i<vertices;i++)                                                                                                        
      if (!z[i])                                                                                                              
	if (dist[i]>=0 && dist[i]<min) {                                                                                
	  min=dist[i];v=i;                                                                                        
	}                                                                                                               
                                                                                                                                                
	// Calculando as distancias dos novos vizinhos de z                                                                           
    if (min != HUGE_VAL && v != destino - 1) {                                                                                      
      z[v] = 1;                                                                                                               
      for (i = 0; i < vertices; i++)                                                                                          
	if (!z[i]) {                                                                                                    
	  if (cost[v*vertices+i] != -1 && dist[v] + cost[v*vertices+i] < dist[i]) {                           
	    dist[i] = dist[v] + cost[v*vertices+i];                                                       
	    ant[i] =v;                                                                                      
	  }                                                                                                       
	}                                                                                                                       
    }                                                                                                                               
  } while (v != destino - 1 && min != HUGE_VAL);

  printf("\tDe %d para %d: \t", origem, destino);
  if (min == HUGE_VAL) {
    printf("Nao Existe\n");
    printf("\tCusto: \t- \n");
  }
  else {
    i = destino;
    i = ant[i-1];
    while (i != -1) {
      //      printf("<-%d",i+1);                                                                                                     
      tmp[cont] = i+1;
      cont++;
      i = ant[i];
    }

    for (i = cont; i > 0 ; i--) {
      printf("%d -> ", tmp[i-1]);                                                                                             
    }                                                                                                                               
    printf("%d", destino);                                                                                                          
                                                                                                                                                
    printf("\n\tCusto:  %d\n",(int) dist[destino-1]);                                                                               
  }
}

*/
tabela *leEnlaces( char enl[CONST], int count){

  tabela *myConnect;
  FILE *arq;
  int i,adj,custo,j;

  if(!(myConnect = (tabela *)malloc(sizeof(tabela)*count)))
    return NULL;

  for(i = 0; i < count ; i++){
    if(!(myConnect[i].idVizinho = (int *)malloc(sizeof(int)*(count))) ||
       (!(myConnect[i].custo = (int *)malloc(sizeof(int)*(count)))) ||
       (!(myConnect[i].idImediato = (int *)malloc(sizeof(int)*(count)))))
      return NULL;
      
  }

  
  
  if(!(cost = (int *)malloc(sizeof(int)*(count*count))))
    return NULL;

  if(!(arq = fopen(enl,"r"))) return NULL;

  //inicializa vetor de custo
  for(i=0; i<= count*count;i++)cost[i] = -1;
  //lê arestas com respectivos custos
  while(fscanf(arq,"%d %d %d", &i, &adj, &custo) != EOF){
    cost[(i-1)*count + adj - 1] = custo;
    cost[(adj-1)*count + i - 1] = custo;
  }
  //puts("j");
  for(i = 1; i <= count; i++)
    for(j = 1; j <= count;j++)
      if(i!=j){
		myConnect = dijkstra(count,i,j,myConnect);
      }

  //puts("je");
  /*for(i = 0; i < count; i++){
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
  
    }
  */

  return myConnect;
}

router *leInfos(char rout[CONST], int id){

  FILE *arq = fopen(rout, "r");
  router *myRouter = (router *)malloc(sizeof(router)*vertices);
  int i = 0;
  if(!myRouter)return NULL;
  //int i,j;
  if(!arq)return NULL;

	
  
  while(fscanf(arq,"%d %d %s", &(myRouter[i].id), &(myRouter[i].port), myRouter[i].ip)!=EOF)
    i++;

  //if(id != myRouter->id){return NULL;}

  fclose(arq);
  return myRouter;
}

/*int main(){

  char enl[CONST] = "enlaces.config";
  char rot[CONST] = "roteador.config";

  leEnlaces(enl,countIn(rot));


  return 0;
  }*/
