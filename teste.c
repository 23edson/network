/* You can compile this program with:
 * gcc -Wall -D_REENTRANT -o thread thread.c<\n>
 * -lpthread */
/* We always need to include this header file for<\n>
 * the threads */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>


#include <string.h> //memset
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data
/* This is the prototype for our thread function */
void *mythread(void *data);
void *funcMassa();

void die(char *s)
{
    perror(s);
    exit(1);
}
/* We must initialize our mutex */
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
/* This will be a shared variable between all of<\n>
 * the threads */
int x = 0;
int main(void) {
   /* This will be an array holding the thread ids<\n>
    * for each thread */
   /* We keep track of all the tids so that we can<\n>
    * call pthread_join()later to retrieve
    * the return value from the thread */
   pthread_t serv,clie;
   int i;
   /* We will now create the 10 threads. Each<\n>
    * thread will increment x until x is 4000.
    * On our last argument ot pthread_create
    * we could have passed an argument to the
    * thread function */
  
      pthread_create(&serv, NULL, funcMassa, NULL);
      pthread_create(&clie, NULL, mythread, NULL);
   
   /* We will now wait for each thread to<\n>
    * terminate */
  
   /* This will block until the specified
    * thread finishes execution. Our second
    * argument to pthread_join can be a pointer
    * that will have the return value of the
    * thread stored in it */
	
      pthread_join(serv, NULL);
      printf("\nThread id %ld returned\n", serv);
      pthread_join(clie, NULL);
      printf("\nThread id %ld returned\n", clie);
   
   return(1);
}
   /* This is our actual thread function */
   
   
void *funcMassa(){
	
	struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);
         
        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }
    }
 
    close(s);
	pthread_exit(NULL);
}
void *mythread(void *data) {
struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 
    while(1)
    {
        printf("Enter message : ");
        //gets(message);
        scanf("%s",message);
         
        //send the message
        if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
         
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        {
            die("recvfrom()");
        }
         
        puts(buf);
    }
 
    close(s);
   /* We can return a pointer. Whatever pointer
    * we return can later be retrieved using the
    * pthread_join function */
   pthread_exit(NULL);
}
