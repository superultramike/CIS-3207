#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX 10

int buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;
int loops = 0;

// Buffer OPS
void put(int value) {
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % MAX;
    count++;
    printf("Produced: %d\n", count);
}

int get() {
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % MAX;
    count--;
    return tmp;
}

pthread_cond_t empty, full;
pthread_mutex_t mutex;

void *producer(void *arg) {
    for(int i=0; i<loops; i++) {
        pthread_mutex_lock(&mutex);
        while(count == MAX) {
            pthread_cond_wait(&empty, &mutex);
        }
        put(i);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
    }
}

void *consumer(void *arg) {
    for(int i=0; i<loops; i++) {
        pthread_mutex_lock(&mutex);
        while(count == 0) {
            pthread_cond_wait(&full, &mutex);
        }
        int tmp = get();
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
        printf("Consumed: %d\n", tmp);
    }
}


int main(int argc, char *argv[]) {
    // process arguments
    // how many times are you going to consume
    loops = 20;
    // how many consumers
    int numConsumers = 1;

    // producer thread and consumer thread
    pthread_t prods, cons[numConsumers];

    // create the producer
    pthread_create(&prods, NULL, producer, NULL);

    int i;
    // Create the consumers
    for(i=0; i<numConsumers; i++) {
        pthread_create(&cons[i], NULL, consumer, NULL);
    }

    // Wait for all threads to finish
    pthread_join(prods, NULL);
    for(i=0; i<numConsumers; i++) {
        pthread_join(cons[i], NULL);
    }

    return 0;
}
