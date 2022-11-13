#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    int id;
    int type;
} product;

void * type_1_consumer(void * arg);
void * type_2_consumer(void * arg);
void put(product value);
product get();
product buffer_1_get();
product buffer_2_get();

int max;
product * distributor;
int fillIndex = 0;
int useIndex = 0;
int amount = 0;

product * buffer_1;
product * buffer_2;

pthread_cond_t empty_1;
pthread_cond_t full_1;
pthread_cond_t empty_2;
pthread_cond_t full_2;

pthread_mutex_t mutex;

pthread_t cons[2];

int buffer_1_amount = 0;
int buffer_2_amount = 0;
int buffer_1_fillIndex = 0;
int buffer_2_fillIndex = 0;
int buffer_1_useIndex = 0;
int buffer_2_useIndex = 0;

int main(int argc, char * argv[]) {
    max = atoi(argv[1]);
    distributor = malloc(sizeof(product) * max);
    buffer_1 = malloc(sizeof(product) * max);
    buffer_2 = malloc(sizeof(product) * max);

    product p;
    for(int i=0; i<2; i++) {
        pthread_create(&cons[i], NULL, type_1_consumer, NULL);
        pthread_create(&cons[i], NULL, type_2_consumer, NULL);
    }

    int pipefd[2];
    if(pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if(pid == 0) {
        // producer 1 write to the pipe
        close(pipefd[0]);
        int count = 0;
        while(1) {
            p.id = count;
            p.type = 1;
            write(pipefd[1], &p, sizeof(p));
            count += 1;
            sleep(1);
        }
    }
    else {
        int second_pid = fork();
        if(second_pid == 0) {
            // producer 2 write to the pipe
            close(pipefd[0]);
            int count = 0;
            while(1) {
                p.id = count;
                p.type = 2;
                write(pipefd[1], &p, sizeof(p));
                count += 1;
                sleep(1);
            }
        }
        // Distributor that reads the pipe and sends data to specific queue
        while(1) {
            // read the pipe
            if((read(pipefd[0], &p, sizeof(p)) > 0)) {
                //printf("Product ID: %d, Type: %d\n", p.id, p.type);
                put(p);
                if(amount != 0) {
                    product p = get();
                    if(p.type == 1) {
                        pthread_mutex_lock(&mutex);
                        while(buffer_1_amount == max) {
                            pthread_cond_wait(&empty_1, &mutex);
                        }
                        buffer_1[buffer_1_fillIndex] = p;
                        buffer_1_fillIndex = (buffer_1_fillIndex + 1) % max;
                        buffer_1_amount++;
                        pthread_cond_signal(&full_1);
                        pthread_mutex_unlock(&mutex);
                    }
                    else if(p.type == 2) {
                        pthread_mutex_lock(&mutex);
                        while(buffer_1_amount == max) {
                            pthread_cond_wait(&empty_2, &mutex);
                        }
                        buffer_2[buffer_2_fillIndex] = p;
                        buffer_2_fillIndex = (buffer_2_fillIndex + 1) % max;
                        buffer_2_amount++;
                        pthread_cond_signal(&full_2);
                        pthread_mutex_unlock(&mutex);
                    }
                }
            }
        }
    }

    free(distributor);
    free(buffer_1);
    free(buffer_2);
    return 0;
}

void put(product value) {
    distributor[fillIndex] = value;
    fillIndex = (fillIndex + 1) % max;
    amount++;
}

product get() {
    product tmp = distributor[useIndex];
    useIndex = (useIndex + 1) % max;
    amount--;
    return tmp;
}

product buffer_1_get() {
    product p = buffer_1[buffer_1_useIndex];
    buffer_1_useIndex = (buffer_1_useIndex + 1) % max;
    buffer_1_amount--;
    return p;
}

product buffer_2_get() {
    product p = buffer_2[buffer_2_useIndex];
    buffer_2_useIndex = (buffer_2_useIndex + 1) % max;
    buffer_2_amount--;
    return p;
}

void * type_1_consumer(void * arg) {
    while(1) {
        pid_t tid = gettid();
        FILE *fp1;
        fp1 = fopen("foo.txt", "a");

        pthread_mutex_lock(&mutex);
        while(buffer_1_amount == 0) {
            pthread_cond_wait(&full_1, &mutex);
        }
        product p = buffer_1_get();

        fprintf(fp1, "Type: %d, Thread ID: %d, Production Sequence #: %d, Consumption Sequence #: %d\n", p.type, tid, buffer_1_useIndex, p.id);
        printf("Type: %d, Thread ID: %d, Production Sequence #: %d, Consumption Sequence #: %d\n", p.type, tid, buffer_1_useIndex, p.id);

        pthread_cond_signal(&empty_1);
        pthread_mutex_unlock(&mutex);
        fclose(fp1);
    }
}

void * type_2_consumer(void * arg) {
    while(1) {
        pid_t tid = gettid();
        FILE *fp2;
        fp2 = fopen("foo.txt", "a");

        pthread_mutex_lock(&mutex);
        while(buffer_2_amount == 0) {
            pthread_cond_wait(&full_2, &mutex);
        }
        product p = buffer_2_get();

        fprintf(fp2, "Type: %d, Thread ID: %d, Production Sequence #: %d, Consumption Sequence #: %d\n", p.type, tid, buffer_2_useIndex, p.id);
        printf("Type: %d, Thread ID: %d, Production Sequence #: %d, Consumption Sequence #: %d\n", p.type, tid, buffer_2_useIndex, p.id);

        pthread_cond_signal(&empty_2);
        pthread_mutex_unlock(&mutex);

        fclose(fp2);
    }
}
