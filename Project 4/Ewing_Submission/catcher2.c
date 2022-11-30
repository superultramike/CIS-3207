// Program imports
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

// Struct for signals received
typedef struct {
    double time;
    int type;
    pthread_t tid;
} signals;

// Function prototypes
void * sigusr1Catcher(void * args);
void * sigusr2Catcher(void * args);
void handleSIGUSR1(int sig);
void handleSIGUSR2(int sig);
void put(signals value);
signals get();
void reporter();

// Global variables
int max = 1000;
signals * buffer;
int fillIndex = 0;
int useIndex = 0;
int amount = 0;
int file_descriptor;
struct timespec start;
struct timespec stop;

// Global declaration of CV and Locks
pthread_mutex_t mutex;
pthread_cond_t empty;
pthread_cond_t full;

// Main function to create catcher functions/handler
int main(int argc, char * argv[]) {
    // Start clock timer for later reporterer
    if(clock_gettime(CLOCK_REALTIME, &start) == -1) {
        perror("Error with gettime");
        exit(EXIT_FAILURE);
    }

    // Allocate size of buffer to place signal content into
    buffer = malloc(sizeof(signals) * max);

    // Create file to write results too for later use
    file_descriptor = open("received_signal.txt", O_CREAT | O_RDWR | O_TRUNC, S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR);

    // Setup signal mask
    sigset_t set, old_set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &set, &old_set);

    // Begin catching execution
    // declare each pthread and reporter thread
    printf("Catcher is running\n");
    pthread_t catch_1;
    pthread_create(&catch_1, NULL, sigusr1Catcher, NULL);
    pthread_t catch_2;
    pthread_create(&catch_2, NULL, sigusr2Catcher, NULL);
    reporter();
}

// Function to catch SIGUSR1 type signals
// and establish sigmask for said signal
void * sigusr1Catcher(void * args) {
    sigset_t otherSet, otherOldSet;
    struct sigaction sig;
    sig.sa_flags = 0;
    sig.sa_handler = &handleSIGUSR1;
    sigaction(SIGUSR1, &sig, NULL);
    sigemptyset(&otherSet);
    sigaddset(&otherSet, SIGUSR1);
    sigprocmask(SIG_UNBLOCK, &otherSet, &otherOldSet);

    // Not sure why this is needed but it removes an error
    while(1) {

    }
}

// Function to catch SIGUSR2 type signals
// and establish sigmask for said signal
void * sigusr2Catcher(void * args) {
    sigset_t otherSet2, otherOldSet2;
    struct sigaction sig;
    sig.sa_flags = 0;
    sig.sa_handler = &handleSIGUSR2;
    sigaction(SIGUSR2, &sig, NULL);
    sigemptyset(&otherSet2);
    sigaddset(&otherSet2, SIGUSR2);
    sigprocmask(SIG_UNBLOCK, &otherSet2, &otherOldSet2);

    // Not sure why this is needed but it removes an error
    while(1) {

    }
}

// Function to handle all signals with type SIGUSR1
void handleSIGUSR1(int sig) {
    // gain lock control
    pthread_mutex_lock(&mutex);
        // if buffer is full, then wait
        while(amount == max) {
            pthread_cond_wait(&empty, &mutex);
        }
        // Declare signal type
        signals s;
        s.type = 1;

        // Get stop time for this specific signal instance
        if(clock_gettime(CLOCK_REALTIME, &stop) == -1) {
            perror("Error with gettime");
            exit(EXIT_FAILURE);
        }
        s.time = stop.tv_sec - start.tv_sec + (double)(stop.tv_nsec - start.tv_nsec) / 1000000000;

        // Put signal instance in buffer and signal unlock
        put(s);
        pthread_cond_signal(&full);
    // give up lock control
    pthread_mutex_unlock(&mutex);
}

// Function to handle all signals with type SIGUSR2
void handleSIGUSR2(int sig) {
    // gain lock control
    pthread_mutex_lock(&mutex);
        // if buffer is full, then wait
        while(amount == max) {
            pthread_cond_wait(&empty, &mutex);
        }

        // Declare signal type
        signals s;
        s.type = 2;

        // Get stop time for this specific signal instance
        if(clock_gettime(CLOCK_REALTIME, &stop) == -1) {
            perror("Error with gettime");
            exit(EXIT_FAILURE);
        }
        s.time = stop.tv_sec - start.tv_sec + (double)(stop.tv_nsec - start.tv_nsec) / 1000000000;

        // Put signal instance in buffer and signal unlock
        put(s);
        pthread_cond_signal(&full);
    // give up lock control
    pthread_mutex_unlock(&mutex);
}

// Function to put signal instance in buffer
void put(signals value) {
    buffer[fillIndex] = value;
    fillIndex = (fillIndex + 1) % max;
    amount++;
}

// Function to get signal instance out of buffer
signals get() {
    signals tmp = buffer[useIndex];
    useIndex = (useIndex + 1) % max;
    amount--;
    return tmp;
}

// Function to reporter details of signal contents
void reporter() {
    // Declare general string to print to a file
    char * contents = malloc(sizeof(char) * 300);

    while(1) {
        // Gain lock
        pthread_mutex_lock(&mutex);
            // While there is nothing in the buffer, wait
            while(amount == 0) {
                pthread_cond_wait(&full, &mutex);
            }
            // Get a signal and its contents from the buffer
            signals s = get();
            // Condense signal contents into contents string
            sprintf(contents, "Signal type: %d, Time received: %lf\n", s.type, s.time);
            // Print contents to terminal
            printf("%s", contents);
            // Write contents to the FD which holds the file
            write(file_descriptor, contents, strlen(contents));
            // Nullify the contents for the next signal instance
            strcpy(contents, "");
            pthread_cond_signal(&empty);
        // Give up the lock
        pthread_mutex_unlock(&mutex);
    }
}
