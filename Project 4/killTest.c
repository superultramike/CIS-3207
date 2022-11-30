#include <stdio.h>
#include <unistd.h>
#include <signal.h>


void signal_handler(int sig) {
    printf("You got the signal\n");
    exit(0);
}

/*
void parent_func(int child_pid) {
    kill(child_pid, SIGUSR1); // send USR1 signal
    puts("sent SIGUSR1 to child\n");
    waitpid(child_pid, NULL, 0); // wait for child
}
*/

int main() {
    pid_t pid = fork();

    if(pid == 0) {
        signal(SIGUSR1, signal_handler);

        while(1) {
            sleep(1);
        }
    }
    else {
        sleep(1); // gives child time to spawn
        //parent_func(pid);
        kill(pid, SIGUSR1); // send USR1 signal
        puts("sent SIGUSR1 to child\n");
        waitpid(pid, NULL, 0); // wait for child
    }
}
