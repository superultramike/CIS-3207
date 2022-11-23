#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

double sleepTime;
int choice;
pthread_t cons[2];

void signal1_handler(int sig) {
    if(sig == SIGUSR1) {
        puts("child received SIGUSR1\n");
    }
    exit(0);
}

void signal2_handler(int sig) {
    if(sig == SIGUSR2) {
        puts("child received SIGUSR2\n");
    }
    exit(0);
}

void parent_func(int pid) {
    //printf("Time: %.2lf\n", sleepTime);
    choice = (double)sleepTime*100;
    printf("Choice: %d\n", choice);

    // if number is 5 or above then send SIGUSR1
    if(choice >= 5) {
        kill(pid, SIGUSR1); // send USR1 signal
        printf("sent SIGUSR1 to child\n");
        waitpid(pid, NULL, 0); // wait for child
    }
    // If the number is 1 to 4.9 then send SIGUSR2
    else {
        kill(pid, SIGUSR2); // send USR1 signal
        printf("sent SIGUSR2 to child\n");
        waitpid(pid, NULL, 0); // wait for child
    }
}

int main() {
    srand(time(NULL));

    pid_t pid = fork();
    // Child Process
    if(pid == 0) {
        // Installs the handler for SIGUSR1/2
        // with function signal_handler
        signal(SIGUSR1, signal1_handler);
        signal(SIGUSR2, signal2_handler);
        //pthread_create(&cons[0], NULL, signal1_handler, NULL);
        //pthread_create(&cons[1], NULL, signal2_handler, NULL);

        while(1) {
            sleep(1);
        }
    }
    // Parent Process
    else {
        int num = (rand() % (10 - 1 + 1)) + 1;
        sleepTime = (double)num/100;
        //printf("Time: %.2lf\n", sleepTime);

        sleep(sleepTime);
        parent_func(pid);
    }
}

