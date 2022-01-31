// Nome: Cássio Fehlberg Lemos          Status do programa: Funcionando

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define BUFFERSIZE   2

typedef struct buffer{
    int vetor[BUFFERSIZE];
    int quantprod;
    int quantcons;
    int maxprodutos;
    pthread_mutex_t mutexproduzindo, mutexconsumindo;
    sem_t scheio, svazio;
}BUFFER;

int BufferCheio(int vetor[]){
    int flag=0;
    for(int i=0; i<BUFFERSIZE; i++){
        if(vetor[i]==0)
            return 0;
    }
    return 1;
}

int BufferVazio(int vetor[]){
    int flag=0;
    for(int i=0; i<BUFFERSIZE; i++){
        if(vetor[i]!=0)
            return 0;
    }
    return 1;
}

void * Produtor(void *args){
    BUFFER *buff;
    buff = (BUFFER*) args;
    int flag=0;
    if(BufferCheio(buff->vetor)==1 && buff->quantprod<buff->maxprodutos)
        sem_wait(&buff->scheio);
    for(int i=0; i<BUFFERSIZE; i++){
        pthread_mutex_lock(&buff->mutexproduzindo);
        if(buff->vetor[i]==0 && flag==0){
            buff->quantprod=buff->quantprod+1;
            buff->vetor[i]=buff->quantprod;
            printf("Produziu %d\n", buff->vetor[i]);
            flag=1;
        }
        pthread_mutex_unlock(&buff->mutexproduzindo);
    }
    sem_post(&buff->svazio);
}

void * Consumidor(void *args){
    BUFFER *buff;
    int flag=0;
    buff = (BUFFER*) args;
    if(BufferVazio(buff->vetor)==1 && buff->quantprod<buff->maxprodutos)
        sem_wait(&buff->svazio);
    for(int i=0; i<BUFFERSIZE; i++){
        pthread_mutex_lock(&buff->mutexconsumindo);
        if(buff->vetor[i]!=0 && flag==0){
            printf("Consumiu %d\n", buff->vetor[i]);
            buff->vetor[i]=0;
            buff->quantcons=buff->quantcons+1;
            flag=1;
        }
        pthread_mutex_unlock(&buff->mutexconsumindo);
    }
    sem_post(&buff->scheio);
}

int main(int argc, char *argv[])
{

    int quantp = atoi(argv[1]);
    int quantc = atoi(argv[2]);
    int quantmax = atoi(argv[3]);
    int i=0;
    int j=0;
    BUFFER *buff;
    buff=malloc(sizeof(BUFFER));
    pthread_t threadp[quantp];
    pthread_t threadc[quantc];
    buff->maxprodutos=quantmax;
    buff->quantprod=0;
    buff->quantcons=0;
    pthread_mutex_init(&buff->mutexproduzindo, NULL);
    pthread_mutex_init(&buff->mutexconsumindo, NULL);
    sem_init(&buff->scheio, 1, 1);
    sem_init(&buff->svazio, 1, 1);
    for(int i=0; i<BUFFERSIZE; i++){
        buff->vetor[i]=0;
    }
    while(buff->quantprod<buff->maxprodutos || buff->quantprod<buff->maxprodutos){
        if(i==quantp){
            for(i=0;i<quantp;i++)
                pthread_join(threadp[i],NULL);
            i=0;
        }
        if(j==quantc){
            for(j=0;j<quantc;j++)
                pthread_join(threadc[j],NULL);
            j=0;
        }
         pthread_create(&threadp[i], NULL, Produtor,buff);
         pthread_create(&threadc[j], NULL, Consumidor, buff);
         sleep(1);
    }
    return 0;
}
