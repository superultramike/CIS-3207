// Library imports
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

// Struct definition
typedef struct {
    int id;
    int type;
} product;
product p;

// Global variables
int max;
product * distributor;
int fillIndex = 0;
int useIndex = 0;
int amount = 0;

// Global products and pthread variables
product * buffer1, * buffer2;
pthread_cond_t empty1, full1, empty2, full2;
pthread_mutex_t mutex;
pthread_t cons[2];

// Global buffer variables
int buffer1amount = 0;
int buffer2amount = 0;
int buffer1fillIndex = 0;
int buffer2fillIndex = 0;
int buffer1useIndex = 0;
int buffer2useIndex = 0;

// Function to put produced products into the distributor buffer
void put(product value) {
    // Declare product value in buffer location
    distributor[fillIndex] = value;
    fillIndex = (fillIndex + 1) % max;
    // Increase distributor buffer count
    amount++;
}

// Function to get produced products out of the distributor buffer
product get() {
    // Declare product value in buffer location
    product tmp = distributor[useIndex];
    useIndex = (useIndex + 1) % max;
    // Decrease distributor buffer count
    amount--;
    return tmp;
}

// Function to get produced products out of the type 1 buffer
product buffer1get() {
    // Declare the product at THAT location
    product p = buffer1[buffer1useIndex];
    buffer1useIndex = (buffer1useIndex + 1) % max;
    // Decrease buffer 1 count
    buffer1amount--;
    return p;
}

// Function to get produced products out of the type 2 buffer
product buffer2get() {
    // Declare the product at THAT location
    product p = buffer2[buffer2useIndex];
    buffer2useIndex = (buffer2useIndex + 1) % max;
    // Decrease buffer 2 count
    buffer2amount--;
    return p;
}

// Function that creates a type 1 consumer, consumes type 1 product and prints data
void * type1consumer(void * arg) {
    while(1) {
        // Declare file pointer
        FILE *fp1;
        fp1 = fopen("foo.txt", "a");

        // Acquire lock and enter critical section
        pthread_mutex_lock(&mutex);
        // If buffer 1 is full then...
        while(buffer1amount == 0) {
            //...Then wait for the signal from product 2 insertion
            pthread_cond_wait(&full1, &mutex);
        }
        // Get an element from buffer 1
        product p = buffer1get();

        // Print contents to the file
        fprintf(fp1, "Type: %d, Thread ID: %ld, Production Sequence # for type 1: %d, Consumption Sequence #: %d\n", p.type, pthread_self(), buffer1useIndex, p.id);
        // Print contents to the terminal
        printf("Type: %d, Thread ID: %ld, Production Sequence # for type 1: %d, Consumption Sequence #: %d\n", p.type, pthread_self(), buffer1useIndex, p.id);

        // Signal and unlock mutex
        pthread_cond_signal(&empty1);
        pthread_mutex_unlock(&mutex);

        // Close file pointer
        fclose(fp1);
    }
}

// Function that creates a type 2 consumer, consumes type 2 product and prints data
void * type2consumer(void * arg) {
    while(1) {
        // Declare file pointer
        FILE *fp2;
        fp2 = fopen("foo.txt", "a");

        // Acquire lock and enter critical section
        pthread_mutex_lock(&mutex);
        // If buffer 1 is full then...
        while(buffer2amount == 0) {
            //...wait for the signal from product 2 insertion
            pthread_cond_wait(&full2, &mutex);
        }
        // Get an element from buffer 2
        product p = buffer2get();

        // Print contents to the file
        fprintf(fp2, "Type: %d, Thread ID: %ld, Production Sequence # for type 2: %d, Consumption Sequence #: %d\n", p.type, pthread_self(), buffer2useIndex, p.id);
        // Print contents to the terminal
        printf("Type: %d, Thread ID: %ld, Production Sequence # for type 2: %d, Consumption Sequence #: %d\n", p.type, pthread_self(), buffer2useIndex, p.id);


        // Signal and unlock mutex
        pthread_cond_signal(&empty2);
        pthread_mutex_unlock(&mutex);

        // Close file pointer
        fclose(fp2);
    }
}

// Main execution function
int main(int argc, char * argv[]) {
    // Declare time
    srand((unsigned)time(NULL));

    // Prematurely determine size of max elements in buffer
    max = atoi(argv[1]);

    // Base all buffer sizes off given max value
    distributor = malloc(sizeof(product) * max);
    buffer1 = malloc(sizeof(product) * max);
    buffer2 = malloc(sizeof(product) * max);

    // Consumer thread creation
    for(int i=0; i<2; i++) {
        // Create consumer thread to consumes type 1 products
        pthread_create(&cons[i], NULL, type1consumer, NULL);
        // Create consumer thread to consumes type 2 products
        pthread_create(&cons[i], NULL, type2consumer, NULL);
    }

    // Declare pipe file descriptors
    int pipefd[2];

    // Pipe file descriptor error checking
    if(pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork to perform child processes
    int pid = fork();

    // Child process
    // Producer of Type 1 writes to the pipe
    if(pid == 0) {
        // Close the reading side of the pipe
        close(pipefd[0]);

        // Declare product counter for type 1
        int count = 0;

        while(1) {
            p.id = count;
            // Declare that we are making this product type 1
            p.type = 1;
            // Write product type 1 into writing side of the pipe
            write(pipefd[1], &p, sizeof(p));
            // Increment the count since we wrote something
            count += 1;

            // Sleep for a random amount of time
            int num = ((rand() % (20-1+1)) + 1);
            int microsecs = num * 10000;
            usleep(microsecs);
        }
    }

    // Parent Process
    else {
        // Fork to perform child processes
        int second_pid = fork();

        // Child Process
        // Producer of Type 2 writes to the pipe
        if(second_pid == 0) {
            // Close the reading side of the pipe
            close(pipefd[0]);

            // Declare product counter for type 2
            int count = 0;

            while(1) {
                p.id = count;
                // Declare that we are making this product type 1
                p.type = 2;
                // Write product type 1 into writing side of the pipe
                write(pipefd[1], &p, sizeof(p));
                // Increment the count since we wrote something
                count += 1;

                // Sleep for a random amount of time
                int num = ((rand() % (20-1+1)) + 1);
                int microsecs = num * 10000;
                usleep(microsecs);
            }
        }
        // Distributor that reads the pipe and sends data to specific queue
        while(1) {
            // Read the pipe if the contents of the pipe are greater then 0
            if((read(pipefd[0], &p, sizeof(p)) > 0)) {
                // Debug statement
                //printf("Product ID: %d, Type: %d\n", p.id, p.type);

                // Put pipe contents into the distributor buffer
                put(p);

                // If there are contents in the distributor buffer then...
                if(amount != 0) {
                    //...Get the specific product out of distributor buffer
                    product p = get();

                    // If that product type is 1 then...
                    if(p.type == 1) {
                        //...Acquire lock and enter critical section
                        pthread_mutex_lock(&mutex);
                        // If buffer 1 is full then...
                        while(buffer1amount == max) {
                            //...Then wait for the signal from product 2 insertion
                            pthread_cond_wait(&empty1, &mutex);
                        }
                        // Declare current product at buffer 1 position
                        buffer1[buffer1fillIndex] = p;
                        buffer1fillIndex = (buffer1fillIndex + 1) % max;
                        // Increment the amount in buffer 1
                        buffer1amount++;
                        // Signal and unlock mutex
                        pthread_cond_signal(&full1);
                        pthread_mutex_unlock(&mutex);
                    }
                    // If that product type is 2 then...
                    else if(p.type == 2) {
                        //...Acquire lock and enter critical section
                        pthread_mutex_lock(&mutex);
                        // If buffer 2 is full then...
                        while(buffer2amount == max) {
                            //...Then wait for the signal from product 1 insertion
                            pthread_cond_wait(&empty2, &mutex);
                        }
                        // Declare current product at buffer 2 position
                        buffer2[buffer2fillIndex] = p;
                        buffer2fillIndex = (buffer2fillIndex + 1) % max;
                        // Increment the amount in buffer 2
                        buffer2amount++;
                        // Signal and unlock mutex
                        pthread_cond_signal(&full2);
                        pthread_mutex_unlock(&mutex);
                    }
                }
            }
        }
    }

    // Free all buffers
    free(distributor);
    free(buffer1);
    free(buffer2);
    return 0;
}
