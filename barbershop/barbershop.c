#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

#define N 6

int customers;
sem_t barber, customer, barberDone, customerDone;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;


void *customer_thread(void *arg){
    int id = *((int *)arg);
    sleep(5);
    printf("Customer #%d arrived\n", id);
    pthread_mutex_lock(&mtx);
    if(customers == N){
        printf("Customer #%d leaves as no space\n", id);
        pthread_mutex_unlock(&mtx);
        pthread_exit(NULL);
    }
    customers++;
    pthread_mutex_unlock(&mtx);

    sem_post(&customer);
    printf("Customer #%d waiting...\n", id);
    sem_wait(&barber);

    printf("Customer #%d gets haircut\n", id);
    sleep(1);

    printf("Customer #%d haircut done\n", id);
    sem_post(&customerDone);
    sem_wait(&barberDone);

    pthread_mutex_lock(&mtx);
    customers--;
    pthread_mutex_unlock(&mtx);

}

void *barber_thread(void *arg){
    while(1){
        printf("Barber sleeps\n");
        sem_wait(&customer);
        printf("Barber awakes\n");
        sem_post(&barber);

        sleep(1);

        sem_wait(&customerDone);
        printf("Barber Done\n");
        sem_post(&barberDone);
    }
}

int main(){
    int s, i, ids[10];
    sem_init(&customer, 0, 0);
    sem_init(&barber, 0, 0);
    sem_init(&customerDone, 0, 0);
    sem_init(&barberDone, 0, 0);

    pthread_t thrds[11];

    s = pthread_create(&thrds[0], NULL, barber_thread, NULL);
    if(s != 0)
        exit(-1);

    for(i=1; i<11; i++){
        ids[i-1] = i;
        s = pthread_create(&thrds[i], NULL, customer_thread, &ids[i-1]);
        if(s != 0)
            exit(-1);
    }

    for(i=0; i<11; i++){
        pthread_join(thrds[i], NULL);
    }

    return 0;
}