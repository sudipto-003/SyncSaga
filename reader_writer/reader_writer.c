#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

#define B_Size 50

int database[B_Size];
sem_t mutex, exclusive, print_sync, trunstile;
int readers = 0;

void *reader(void *arg){
    int index = *((int *) arg);
    int num_reader, val;

    sem_wait(&trunstile);
    sem_post(&trunstile);

    sem_wait(&mutex);
    readers += 1;
    num_reader = readers;
    if( readers == 1)
        sem_wait(&exclusive);
    sem_post(&mutex);

    val = database[index];
    int sleep_time = rand() % 5;
    sleep(sleep_time);

    sem_wait(&print_sync);
    printf("Read: index=%d value=%d #reader=%d\n", index, val, num_reader);
    sem_post(&print_sync);

    sem_wait(&mutex);
    readers -= 1;
    if(readers == 0)
        sem_post(&exclusive);
    sem_post(&mutex);

    pthread_exit(NULL);
}


void *writer(void *arg){
    int index = *((int *) arg);
    int old;

    sem_wait(&trunstile);
    sem_wait(&exclusive);
    old = database[index];
    int new_val = rand() % 5001;
    database[index] = new_val;
    printf("Write index=%d old_value=%d new_value=%d\n", index, old, new_val);
    sleep(2);
    sem_post(&trunstile);
    sem_post(&exclusive);

    pthread_exit(NULL);
}



int main(){
    int i, s, val[50];

    for(int i=0; i<B_Size; i++){
        database[i] = rand() % 5001;
    }

    if(sem_init(&mutex, 0, 1) == -1)
        exit(-1);

    if(sem_init(&exclusive, 0, 1) == -1)
        exit(-1);

    if(sem_init(&print_sync, 0, 1) == -1)
        exit(-1);

    if(sem_init(&trunstile, 0, 1) == -1)
        exit(-1);

    pthread_t p[50];

    for(i=0; i<50; i++){
        int selector = rand() % 5;
        val[i] = rand() % 50;
        if(selector <= 2){
            s = pthread_create(&p[i], NULL, reader, &val[i]);
            if(s != 0)
                exit(-1);
        }
        else{
            s = pthread_create(&p[i], NULL, writer, &val[i]);
            if(s != 0)
                exit(-1);
        }
    }

    for(i=0; i<50; i++){
        pthread_join(p[i], NULL);
    }

    return 0;
}