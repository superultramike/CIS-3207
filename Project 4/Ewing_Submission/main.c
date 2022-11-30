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

// Global time declaration
time_t curtime;

// Main function to generate signals
int main(int argc, char **argv) {
    // Setup signal mask
    sigset_t new_sig_set;
    sigemptyset(&new_sig_set);
    sigaddset(&new_sig_set, SIGUSR1);
    sigaddset(&new_sig_set, SIGUSR2);

    // error checking when creating signal mask
    int error;
    if((error = pthread_sigmask(SIG_BLOCK, &new_sig_set, NULL)) != 0) {
        fprintf(stderr, "pthread_sigmask: %s\n", strerror(error));
        return 1;
    }

    // Fork
    int pid = fork();
    // Child Process to fork & exec the catcher program
    if(pid == 0) {
        char * arguments[2];
        arguments[0] = "catcher2";
        arguments[1] = NULL;

        // Error checking if execvp fails
        if(execvp("./catcher2", arguments) < 0) {
            perror("execvp");
            return 1;
        }
    }

    // Parent process that generates signal
    else if(pid > 0) {
        srand((unsigned)time(NULL));

        // Keep generating signals until kill switch is found
        while(1) {
            printf("Generating Signal\n");
            fflush(stdout);

            // Randomly figure out if to send signal1/2
            int random = rand() % ((2+1)-1)+1;

            // If 1 then send SIGUSR1
            if(random == 1) {
                printf("Sending signal: SIGUSR1\n");
                fflush(stdout);

                // Send signal
                if(kill(pid, SIGUSR1) < 0) {
                    perror("kill");
                }

                // Declare file pointer
                FILE *fp1;
                fp1 = fopen("sent_signal.txt", "a");

                // Print contents to the file above
                time(&curtime);
                fprintf(fp1, "Type: SIGUSR1, Current Time Sent: %s", ctime(&curtime));
                // Close file pointer
                fclose(fp1);
            }
            // If not then send SIGUSR2
            else {
                printf("Sending signal: SIGUSR2\n");
                fflush(stdout);

                // Send signal
                if(kill(pid, SIGUSR2) < 0) {
                    perror("kill");
                }

                // Declare file pointer
                FILE *fp2;
                fp2 = fopen("sent_signal.txt", "a");

                // Print contents to the file above
                time(&curtime);
                fprintf(fp2, "Type: SIGUSR2, Current Time Sent: %s", ctime(&curtime));
                // Close file pointer
                fclose(fp2);
            }
            // Sleep for a random amount of time between 0.01-0.1
            int sleepTime = (rand() % (100000 - 10000 + 1)) + 10000;
            usleep(sleepTime);
        }
    }
    // Error checking for forking
    else {
        perror("fork");
    }
}
