#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

#define M 10
#define N 5

sem_t isPotEmpty, isPotFull;
int pot = M;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void *savage(void *arg){
    int savage_id = *((int *)arg);
    while(1){
        pthread_mutex_lock(&mutex);
        if(pot == 0){
            sem_post(&isPotEmpty);
            sem_wait(&isPotFull);
            pot = M;
        }

        pot--;
        //sleep to pretend as eat
        printf("Savage #%d eating, Remaining=%d\n", savage_id, pot);
        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
}

void *cook(void *arg){
    while(1){
        sem_wait(&isPotEmpty);
        printf("Cook is cooking now\n");
        sleep(3);
        sem_post(&isPotFull);
    }

}

int main(){
    int s, i, thread_ids[N];
    sem_init(&isPotEmpty, 0, 0);
    sem_init(&isPotFull, 0, 0);
    pthread_t thrds[N+1];
    
    s = pthread_create(&thrds[0], NULL, cook, NULL);
    if(s != 0)
        exit(-1);

    for(i=1; i<=N; i++){
        thread_ids[i-1] = i;
        s = pthread_create(&thrds[i], NULL, savage, &thread_ids[i-1]);
        if(s != 0)
            exit(-1);
    }

    for(i=0; i<=N; i++){
        pthread_join(thrds[i], NULL);
    }

    return 0;
}