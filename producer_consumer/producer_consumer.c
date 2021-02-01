#include<stdio.h>
#include<signal.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

#define BUF_SIZE 5

static int buffer[BUF_SIZE];
static int b_index = -1;

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c1 = PTHREAD_COND_INITIALIZER;


void *producer(void *arg){
    int s;
    s = pthread_mutex_lock(&m1);
    if( s != 0)
        exit(-1);
    
    while(b_index + 1 >= BUF_SIZE){
        s = pthread_cond_wait(&c1, &m1);
        if(s != 0)
            pthread_exit(NULL);
    }
    b_index++;
    buffer[b_index] = rand() % 100;
    printf("Produced %d\n", buffer[b_index]);
    sleep(1);

    s = pthread_mutex_unlock(&m1);
    if(s != 0)
        exit(-1);

    s = pthread_cond_signal(&c1);
    if(s != 0)
        exit(-1);

    pthread_exit(NULL);
}


void *consumer(void *arg){
    int s, val;
    while(1){
        s = pthread_mutex_lock(&m1);
        if(s != 0)
            exit(-1);

        while(b_index < 0){
            s = pthread_cond_wait(&c1, &m1);
            if(s != 0)
                exit(-1);
        }

        val = buffer[b_index];
        b_index--;
        printf("Consumer prossed the value: %d\n", val);

        s = pthread_mutex_unlock(&m1);
        if(s != 0)
            exit(-1);

        s = pthread_cond_signal(&c1);
        if(s != 0)
            exit(-1);
    
    }
}


int main(){
    pthread_t con1, con2, p[10];
    int s, i;

    s = pthread_create(&con1, NULL, consumer, NULL);
    if(s != 0)
        exit(-1);

    s = pthread_create(&con2, NULL, consumer, NULL);
    if(s != 0)
        exit(-1);

    for(i=0; i<10; i++){
        s = pthread_create(&p[i], NULL, producer, NULL);
        if(s != 0)
            exit(-1);
    }

    pthread_join(con1, NULL);
    pthread_join(con2, NULL);
    for(i=0; i<10; i++)
        pthread_join(p[i], NULL);

    return 0;
}