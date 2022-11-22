#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

int main() {
    srand(time(NULL));

    pid_t pid = fork();
    // Child Process
    if(pid == 0) {
        printf("We are in child\n");
    }
    // Parent Process
    else {
        // Create the child process
        // Then randomly send SIGUSR1 or 2 to the process using kill
            // every 0.01-0.1 seconds
        // Log signals to a sent_signal file
            // Type of signal sent
            // Time signal was sent (clock_gettime())
        while(1) {
            // sleepRandomInterval(0.01, 0.1)
            // calculates a random number between 0.01 and 0.1
            // signal = randomSignal()
            // kill(0, signal)
            // log(signal)
        }
    }

    int num = (rand() % (10 - 1 + 1)) + 1;
    double newNum = (double)num/100;
    printf("Time: %lf\n", newNum);

    sleep(newNum);

    printf("Ok I slept\n");

    // Use kill in the parent and signal in the child thread
}
