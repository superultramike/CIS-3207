#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MSG "SIGUSR1 received\n"
#define MSG2 "SIGUSR2 received\n"

typedef struct {
    int type;
    int time;
} signals;
signals s;

// Global variables
int max = 1000;

signals * distributor;
int fillIndex = 0;
int useIndex = 0;
int amount = 0;

// Global products and pthread variables
signals * buffer1, * buffer2;
pthread_cond_t empty1, full1, empty2, full2;
pthread_mutex_t mutex;

// Global products and pthread variables
signals *buffer1;
pthread_cond_t empty1, full1;
pthread_mutex_t mutex;

// Global buffer variables
int buffer1amount = 0;
int buffer2amount = 0;
int buffer1fillIndex = 0;
int buffer2fillIndex = 0;
int buffer1useIndex = 0;
int buffer2useIndex = 0;

// Function to put produced signals into the distributor buffer
void put(signals value) {
    // Declare signal value in buffer location
    distributor[fillIndex] = value;
    fillIndex = (fillIndex + 1) % max;
    // Increase distributor buffer count
    amount++;
}

// Function to get produced signals out of the distributor buffer
signals buffer1get() {
    // Declare signal value in buffer location
    signals s = buffer1[buffer1useIndex];
    buffer1useIndex = (buffer1useIndex + 1) % max;
    // Decrease distributor buffer count
    buffer1amount--;
    return s;
}

void *reporter(void *arg) {
    FILE *fp1;
    fp1 = fopen("received_signal.txt", "a");

    pthread_mutex_lock(&mutex);
    while(buffer1amount == 0) {
        pthread_cond_wait(&full1, &mutex);
    }
    signals s = buffer1get();

    if(s.type == 1) {
        fprintf(fp1, "Type: SIGUSR1, Time: what\n");
        printf("Type: SIGUSR1, Time: what\n");
    }
    else {
        fprintf(fp1, "Type: SIGUSR2, Time: what\n");
        printf("Type: SIGUSR2, Time: what\n");
    }

    pthread_cond_signal(&empty1);
    pthread_mutex_unlock(&mutex);

    fclose(fp1);

    while(1) {
        pause();
    }
}

// consumer 1
void handle_SIGUSR1(int sig) {
    FILE *fp1;
    fp1 = fopen("received_signal.txt", "a");

    pthread_mutex_lock(&mutex);
    while(buffer1amount == 0) {
        pthread_cond_wait(&full1, &mutex);
    }
    if(s.type == 1) {
        fprintf(fp1, "Type: SIGUSR1, Time: what\n");
        printf("Type: SIGUSR1, Time: what\n");
    }
    else {
        fprintf(fp1, "Type: SIGUSR2, Time: what\n");
        printf("Type: SIGUSR2, Time: what\n");
    }

    pthread_cond_signal(&empty1);
    pthread_mutex_unlock(&mutex);

    fclose(fp1);
}

// consumer 2
void handle_SIGUSR2(int sig) {
    FILE *fp1;
    fp1 = fopen("received_signal.txt", "a");

    pthread_mutex_lock(&mutex);
    while(buffer2amount == 0) {
        pthread_cond_wait(&full1, &mutex);
    }
    signals s = buffer1get();

    if(s.type == 1) {
        fprintf(fp1, "Type: SIGUSR1, Time: what\n");
        printf("Type: SIGUSR1, Time: what\n");
    }
    else {
        fprintf(fp1, "Type: SIGUSR2, Time: what\n");
        printf("Type: SIGUSR2, Time: what\n");
    }

    pthread_cond_signal(&empty1);
    pthread_mutex_unlock(&mutex);

    fclose(fp1);
}

// producer 1 buffer1
void *t1(void *arg) {
    sigset_t sigmask;
    struct sigaction sa = {0};
    sa.sa_handler = handle_SIGUSR1;
    sa.sa_flags = 0;

    if(sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("sigaction");
        return NULL;
    }

    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGUSR1);

    int error;
    if((error = pthread_sigmask(SIG_UNBLOCK, &sigmask, NULL)) != 0) {
        fprintf(stderr, "pthread_sigmask: %s\n", strerror(error));
        return NULL;
    }

    pthread_mutex_lock(&mutex);
    while(buffer1amount == max) {
        pthread_cond_wait(&empty1, &mutex);
    }
    buffer1[buffer1fillIndex] = s;
    buffer1fillIndex = (buffer1fillIndex + 1) % max;
    buffer1amount++;
    pthread_cond_signal(&full2);
    pthread_mutex_unlock(&mutex);

    while(1) {
        pause();
    }
}

// producer 2 buffer2
void *t2(void *arg) {
    sigset_t sigmask;
    struct sigaction sa = {0};
    sa.sa_handler = handle_SIGUSR2;
    sa.sa_flags = 0;
    if(sigaction(SIGUSR2, &sa, NULL) < 0) {
        perror("sigaction");
        return NULL;
    }
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGUSR2);
    int error;
    if((error = pthread_sigmask(SIG_UNBLOCK, &sigmask, NULL)) != 0) {
        fprintf(stderr, "pthread_sigmask: %s\n", strerror(error));
        return NULL;
    }

    pthread_mutex_lock(&mutex);
    while(buffer2amount == max) {
        pthread_cond_wait(&empty2, &mutex);
    }
    buffer2[buffer2fillIndex] = s;
    buffer2fillIndex = (buffer2fillIndex + 1) % max;
    buffer2amount++;

    pthread_cond_signal(&full2);
    pthread_mutex_unlock(&mutex);

    while(1) {
        pause();
    }
}

int main(int argc, char **argv) {
    pthread_t thread1, thread2, reporter;

    buffer1 = malloc(sizeof(signal) * max);
    buffer2 = malloc(sizeof(signal) * max);

    int error;
    if((error = pthread_create(&thread1, NULL, t1, NULL)) != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(error));
        return 1;
    }

    if((error = pthread_create(&thread2, NULL, t2, NULL)) != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(error));
        return 1;
    }

    if((error = pthread_create(&reporter, NULL, reporter, NULL)) != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(error));
        return 1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(reporter, NULL);
}
