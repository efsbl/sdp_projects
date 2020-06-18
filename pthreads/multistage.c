#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

pthread_barrier_t barrier;

void *function(void *arg){
    //Thread id
    int tid = *(int*)arg;
    int timeInSeconds = tid+2;
    printf("Thread %d started.\n", tid);
    sleep(timeInSeconds);
    printf("Thread %d finished.\n", tid);
    pthread_barrier_wait(&barrier);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    int T;
    T = atoi(argv[1]);
    pthread_t misThreads[T];
    int threads_ids[T];

    pthread_barrier_init(&barrier, NULL, T);
    for(int id=0; id<T; id++){
        threads_ids[id] = id;
        pthread_create(&misThreads[id], NULL, &function, (void*)&threads_ids[id]);
    }

    for(int id=0; id<T; id++)
        pthread_join(misThreads[id], NULL);

    printf("All threads finalized.\n");
    pthread_barrier_destroy(&barrier);
    return 0;
}
