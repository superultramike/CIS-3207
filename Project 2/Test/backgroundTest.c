#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

// Function to parse lines
char** lineParse(char *line) {
    int length = 0;
    int cap = 16;
    char **tokens = malloc(cap * sizeof(char*));

    char *delimiters = " \t\r\n";
    char *token = strtok(line, delimiters);

    // THIS RUNS FOR EACH INDIVIDUAL TOKEN
    while(token != NULL) {
        tokens[length] = token;

        length++;
        if(length >= cap) {
            cap = (int) (cap * 1.5);
            tokens = realloc(tokens, cap * sizeof(char*));
            printf("Raised the cap\n");
        }
        token = strtok(NULL, delimiters);
    }
    tokens[length] = NULL;
    return tokens;
}

void interactive() {
    char *line = NULL;
    size_t len = 0;
    size_t read;

    while(1) {
        printf("> ");
        while((read = getline(&line, &len, stdin)) != 1) {
            char **tokens = lineParse(line);
            execution_time(tokens);
            printf("> ");
        }
    }
}

void execution_time(char **args) {
        int background;
        pid_t pid;
        int *status;

        pid = fork();  //here I fork and create a child process
        if(pid && !background) {
            printf("Waiting on child (%d).\n", pid);
            pid = wait(status);
            printf("Child (%d) finished.\n", pid);
        }
        else {
            if (background && pid == 0) { //if it's a child process and i need to run the command in background

            pid = fork(); //fork child and create a grandchild
            if (pid) exit(0); //exit child and orphan grandchild

            execvp(args[0], args); //orphan execs the command then exits
            exit(1);
            } else exit(0);
        }
}

int main(int argc, char *argv[]) {
    // if only 1 argument go to interactive mode
    if(argc == 1) {
        interactive();
    }

    // if 3 arguments or more go to error
    if(argc >= 3) {
        perror("Too many arguments");
        exit(0);
    }
}
