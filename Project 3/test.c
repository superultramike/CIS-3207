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
product * buffer_1, * buffer_2;
pthread_cond_t empty_1, full_1, empty_2, full_2;
pthread_mutex_t mutex;
pthread_t cons[2];


// Global buffer variables
int buffer_1_amount = 0;
int buffer_2_amount = 0;
int buffer_1_fillIndex = 0;
int buffer_2_fillIndex = 0;
int buffer_1_useIndex = 0;
int buffer_2_useIndex = 0;

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
product buffer_1_get() {
    // Declare the product at THAT location
    product p = buffer_1[buffer_1_useIndex];
    buffer_1_useIndex = (buffer_1_useIndex + 1) % max;
    // Decrease buffer 1 count
    buffer_1_amount--;
    return p;
}

// Function to get produced products out of the type 2 buffer
product buffer_2_get() {
    // Declare the product at THAT location
    product p = buffer_2[buffer_2_useIndex];
    buffer_2_useIndex = (buffer_2_useIndex + 1) % max;
    // Decrease buffer 2 count
    buffer_2_amount--;
    return p;
}

// Function that creates a type 1 consumer, consumes type 1 product and prints data
void * type_1_consumer(void * arg) {
    while(1) {
        // Declare file pointer
        FILE *fp1;
        fp1 = fopen("foo.txt", "a");

        // Acquire lock and enter critical section
        pthread_mutex_lock(&mutex);
        // If buffer 1 is full then...
        while(buffer_1_amount == 0) {
            //...Then wait for the signal from product 2 insertion
            pthread_cond_wait(&full_1, &mutex);
        }
        // Get an element from buffer 1
        product p = buffer_1_get();

        // Print contents to the file
        fprintf(fp1, "Type: %d, Thread ID: %ld, Production Sequence # for type 1: %d, Consumption Sequence #: %d\n", p.type, pthread_self(), buffer_1_useIndex, p.id);
        // Print contents to the terminal
        printf("Type: %d, Thread ID: %ld, Production Sequence # for type 1: %d, Consumption Sequence #: %d\n", p.type, pthread_self(), buffer_1_useIndex, p.id);

        // Signal and unlock mutex
        pthread_cond_signal(&empty_1);
        pthread_mutex_unlock(&mutex);

        // Close file pointer
        fclose(fp1);
    }
}

// Function that creates a type 2 consumer, consumes type 2 product and prints data
void * type_2_consumer(void * arg) {
    while(1) {
        // Declare file pointer
        FILE *fp2;
        fp2 = fopen("foo.txt", "a");

        // Acquire lock and enter critical section
        pthread_mutex_lock(&mutex);
        // If buffer 1 is full then...
        while(buffer_2_amount == 0) {
            //...wait for the signal from product 2 insertion
            pthread_cond_wait(&full_2, &mutex);
        }
        // Get an element from buffer 2
        product p = buffer_2_get();

        // Print contents to the file
        fprintf(fp2, "Type: %d, Thread ID: %ld, Production Sequence # for type 2: %d, Consumption Sequence #: %d\n", p.type, pthread_self(), buffer_2_useIndex, p.id);
        // Print contents to the terminal
        printf("Type: %d, Thread ID: %ld, Production Sequence # for type 2: %d, Consumption Sequence #: %d\n", p.type, pthread_self(), buffer_2_useIndex, p.id);


        // Signal and unlock mutex
        pthread_cond_signal(&empty_2);
        pthread_mutex_unlock(&mutex);

        // Close file pointer
        fclose(fp2);
    }
}

// Main execution function
int main(int argc, char * argv[]) {
    srand((unsigned)time(NULL));

    // Prematurely determine size of max elements in buffer
    max = atoi(argv[1]);

    // Base all buffer sizes off given max value
    distributor = malloc(sizeof(product) * max);
    buffer_1 = malloc(sizeof(product) * max);
    buffer_2 = malloc(sizeof(product) * max);

    // Consumer thread creation
    for(int i=0; i<2; i++) {
        // Create consumer thread to consumes type 1 products
        pthread_create(&cons[i], NULL, type_1_consumer, NULL);
        // Create consumer thread to consumes type 2 products
        pthread_create(&cons[i], NULL, type_2_consumer, NULL);
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
                        while(buffer_1_amount == max) {
                            //...Then wait for the signal from product 2 insertion
                            pthread_cond_wait(&empty_1, &mutex);
                        }
                        // Declare current product at buffer 1 position
                        buffer_1[buffer_1_fillIndex] = p;
                        buffer_1_fillIndex = (buffer_1_fillIndex + 1) % max;
                        // Increment the amount in buffer 1
                        buffer_1_amount++;
                        // Signal and unlock mutex
                        pthread_cond_signal(&full_1);
                        pthread_mutex_unlock(&mutex);
                    }
                    // If that product type is 2 then...
                    else if(p.type == 2) {
                        //...Acquire lock and enter critical section
                        pthread_mutex_lock(&mutex);
                        // If buffer 2 is full then...
                        while(buffer_2_amount == max) {
                            //...Then wait for the signal from product 1 insertion
                            pthread_cond_wait(&empty_2, &mutex);
                        }
                        // Declare current product at buffer 2 position
                        buffer_2[buffer_2_fillIndex] = p;
                        buffer_2_fillIndex = (buffer_2_fillIndex + 1) % max;
                        // Increment the amount in buffer 2
                        buffer_2_amount++;
                        // Signal and unlock mutex
                        pthread_cond_signal(&full_2);
                        pthread_mutex_unlock(&mutex);
                    }
                }
            }
        }
    }

    // Free all buffers
    free(distributor);
    free(buffer_1);
    free(buffer_2);
    return 0;
}
