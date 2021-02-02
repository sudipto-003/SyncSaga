#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
//N number of philosophers or forks

#define N 5

sem_t forks[N];
//states: Thinking=0, Eating=1, Hungry=2
int states[N];
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int p_left(int i){
    return (i - 1 + N) % N;
}

int p_right(int i){
    return (i + 1) % N;
}

void get_forks(int i);
void eat(int i);
void put_forks(int i);
void test(int i);

void *philosopher(void *arg){
    int i;
    i = *((int *) arg);
    while(1){
        get_forks(i);
        eat(i);
        put_forks(i);
    }
}

void *print_thread(void *arg){
    int i, j;
    while(1){
        for(i=0; i<N; i++){
            printf("%d\t", states[i]);
        }
        printf("\r");
        fflush(stdout);
        sleep(1);
    }
}

int main(){
    //initialize the semaphores
    int i, s, p_i[N];
    pthread_t phil[N];
    pthread_t p_t;
    
    //monitoring thread
    s = pthread_create(&p_t, NULL, print_thread, NULL);
    if(s != 0)
        exit(-1);

    //To guarantee that the monitoring thread start before any philosopher thread
    sleep(1);
    for(i=0; i<N; i++){
        s = sem_init(&forks[i], 0, 0);
    }

    for(i=0; i<N; i++){
        p_i[i] = i;
        s = pthread_create(&phil[i], NULL, philosopher, &p_i[i]);
        if(s != 0)
            exit(-1);
    }

    for(i=0; i<N; i++){
        pthread_join(phil[i], NULL);
    }

    pthread_join(p_t, NULL);

    return 0;
}

void get_forks(int i){
    pthread_mutex_lock(&mtx);
    states[i] = 2;
    test(i);
    pthread_mutex_unlock(&mtx);
    sem_wait(&forks[i]);
}

void test(int i){
    if(states[i] == 2 && (states[p_left(i)]) != 1 && (states[p_right(i)]) != 1){
        states[i] = 1;
        sem_post(&forks[i]);
    }
}

void put_forks(int i){
    pthread_mutex_lock(&mtx);
    states[i] = 0;
    test(p_right(i));
    test(p_left(i));
    pthread_mutex_unlock(&mtx);
}

void eat(int i){
    sleep(5);

}