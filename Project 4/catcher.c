#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define MSG "SIGUSR1 received\n"
#define MSG2 "SIGUSR2 received\n"

void handle_SIGUSR1(int sig) {
    write(STDOUT_FILENO, MSG, strlen(MSG));
}

void handle_SIGUSR2(int sig) {
    write(STDOUT_FILENO, MSG, strlen(MSG));
}

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
    while(1) {
        pause();
    }
}

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
    while(1) {
        pause();
    }
}

int main(int argc, char **argv) {
    pthread_t thread1, thread2, reporter;
    int error;
    if((error = pthread_create(&thread1, NULL, t1, NULL)) != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(error));
        return 1;
    }

    if((error = pthread_create(&thread2, NULL, t2, NULL)) != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(error));
        return 1;
    }

    /*
    if((error = pthread_create(&reporter, NULL)) != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(error));
        return 1;
    }
    */
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    //pthread_join(reporter, NULL);
}
