#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>

sem_t agentSem, tobacco, match, paper, tobaccoSem, matchSem, paperSem;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
int isTobacco, isPaper, isMatch;

void smoke(){
    sleep(1);
}

//Agent A suplies tobacco and paper
void *agent_A(void *arg){
    while(1){
        sem_wait(&agentSem);
        printf("Tobacco Paper ");
        sem_post(&tobacco);
        sem_post(&paper);
    }
}

//Agent B suplies match and paper
void *agent_B(void *arg){
    while(1){
        sem_wait(&agentSem);
        printf("Paper Match ");
        sem_post(&paper);
        sem_post(&match);
    }
}

//Agent C suplies tobacco and match
void *agent_C(void *arg){
    while(1){
        sem_wait(&agentSem);
        printf("Tobacco Match ");
        sem_post(&tobacco);
        sem_post(&match);
    }
}

//pusher A, wakes when tobacco is available
void *pusher_A(void *arg){
    while(1){
        sem_wait(&tobacco);
        pthread_mutex_lock(&mtx);
        if(isMatch){
            isMatch = 0;
            sem_post(&paperSem);
        }
        else if(isPaper){
            isPaper = 0;
            sem_post(&matchSem);
        }
        else{
            isTobacco = 1;
        }
        pthread_mutex_unlock(&mtx);
    }
}

//pusher B, wakes when paper is available
void *pusher_B(void *arg){
    while(1){
        sem_wait(&paper);
        pthread_mutex_lock(&mtx);
        if(isMatch){
            isMatch = 0;
            sem_post(&tobaccoSem);
        }
        else if(isTobacco){
            isTobacco = 0;
            sem_post(&matchSem);
        }
        else{
            isPaper = 1;
        }
        pthread_mutex_unlock(&mtx);
    }
}

//pusher C, wakes when match is available
void *pusher_C(void *arg){
    while(1){
        sem_wait(&match);
        pthread_mutex_lock(&mtx);
        if(isTobacco){
            isTobacco = 0;
            sem_post(&paperSem);
        }
        else if(isPaper){
            isPaper = 0;
            sem_post(&tobaccoSem);
        }
        else{
            isMatch = 1;
        }
        pthread_mutex_unlock(&mtx);
    }
}

//smoker with tobacco
void *smoker_T(void *arg){
    while(1){
        sem_wait(&tobaccoSem);
        printf("Tobacco\n");
        smoke();
        sem_post(&agentSem);
    }
}

//smoker with paper
void *smoker_P(void *arg){
    while(1){
        sem_wait(&paperSem);
        printf("Paper\n");
        smoke();
        sem_post(&agentSem);
    }
}

//smoker with match
void *smoker_M(void *arg){
    while(1){
        sem_wait(&matchSem);
        printf("Match\n");
        smoke();
        sem_post(&agentSem);
    }
}

int main(){
    //initialize all 7 semaphores
    sem_init(&agentSem, 0, 1);
    sem_init(&tobacco, 0, 0);
    sem_init(&match, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&tobaccoSem, 0, 0);
    sem_init(&matchSem, 0, 0);
    sem_init(&paperSem, 0, 0);

    //There will be 9 different threads;
    //agent consist of 3 threads, 3 helper pusher threads, 3 smoker threads;

    pthread_t threads[9];
    int s, i;

    s = pthread_create(&threads[0], NULL, agent_A, NULL);
    if(s != 0)
        exit(-1);

    s = pthread_create(&threads[1], NULL, agent_B, NULL);
    if(s != 0)
        exit(-1);

    s = pthread_create(&threads[2], NULL, agent_C, NULL);
    if(s != 0)
        exit(-1);

    s = pthread_create(&threads[3], NULL, pusher_A, NULL);
    if(s != 0)
        exit(-1);

    s = pthread_create(&threads[4], NULL, pusher_B, NULL);
    if(s != 0)
        exit(-1);

    s = pthread_create(&threads[5], NULL, pusher_C, NULL);
    if(s != 0)
        exit(-1);

    s = pthread_create(&threads[6], NULL, smoker_T, NULL);
    if(s != 0)
        exit(-1);
    
    s = pthread_create(&threads[7], NULL, smoker_P, NULL);
    if(s != 0)
        exit(-1);

    s = pthread_create(&threads[8], NULL, smoker_M, NULL);
    if(s != 0)
        exit(-1);

    for(i=0; i<9; i++)
        pthread_join(threads[i], NULL);

    return 0;
}