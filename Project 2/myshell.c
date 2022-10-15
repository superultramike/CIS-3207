// ------------------------------------------------------------------------------------------
// Program Imports
// ------------------------------------------------------------------------------------------
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

// ------------------------------------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------------------------------------
extern char **environ;

// ------------------------------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------------------------------
char** lineParse(char *line);
void execution_time(char **args);

// ------------------------------------------------------------------------------------------
// Built in Commands (cd, clr, dir, environ, echo, help, pause, quit, exit)
// Functions that need to finished: cd, dir
// relative path (from current working directory) ./
// full path - call perror
// if they didn't send, print cwd
// ------------------------------------------------------------------------------------------
// Function to quit the shell (same as exit)
void shell_quit(char **args) {
    exit(0);
}

// Function to pause the shell until Enter is pressed
void shell_pause(char **args) {
    printf("Program is paused\n");
    printf("Press ENTER key to Continue\n");
    getchar();
}

// Function to print a help command
void shell_help(char **args) {
    printf("Welcome to the help function\n");
}

// Function to exit the shell (same as quit)
void shell_exit(char **args) {
    exit(0);
}

// Function to clear the screen
void shell_clr(char **args) {
    printf("\e[1;1H\e[2J");
}

/*
// Is this working?
void shell_cd(char **args) {
    if(args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n");
    }
    else {
        if(chdir(args[1]) != 0) {
            perror("shell: cd");
        }
    }
}
*/

// Function to repeat user input after 'echo'
void shell_echo(char **args) {
    while(*++args) {
        printf("%s", *args);
        if(args[1]) printf(" ");
    }
    printf("\n");
}

// Function to print all environ variables
void shell_environ(char **args) {
    char **s = environ;

    for(; *s; s++) {
        printf("%s\n", *s);
    }
}

struct builtin {
    char *name;
    void (*func)(char **args);
};

struct builtin builtins[] = {
    {"quit", shell_quit},
    {"pause", shell_pause},
    {"help", shell_help},
    {"exit", shell_exit},
    {"clr", shell_clr},
    //{"cd", shell_cd},
    {"echo", shell_echo},
    {"environ", shell_environ}
};

int num_builtins() {
    return sizeof(builtins) / sizeof(struct builtin);
}

// ------------------------------------------------------------------------------------------
// Input launcher
// ------------------------------------------------------------------------------------------
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

// put redirect flags in here
// while there is still programs to execute keep going
void execution_time(char **args) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtins[i].name) == 0) {
            builtins[i].func(args);
            return;
        }
    }

    // while there is another program to run, keep searching for possible special cases
    // flags are all stored in here!!!!!!!!!!
    // if redirect and pipe flags are present then run error
    int redirectIn = 0;
    int redirectOut = 0;
    int append = 0;
    int count = 0;
    while(args[count] != NULL) {
        //printf("args[%d] = %s\n", count, args[count]);
        // redirectOut >
        if(strcmp(args[count], ">") == 0) {
            //printf("FOUND IT\n");
            redirectOut = 1;
        }
        // redirectIn <
        if(strcmp(args[count], "<") == 0) {
            //printf("FOUND IT\n");
            redirectIn = 1;
        }
        // append >>
        if(strcmp(args[count], ">>") == 0) {
            //printf("FOUND IT\n");
            append = 1;
        }
        count++;
    }
    //printf("Count = %d\n", count);

    int pid = fork();

    if(pid == 0) {
        // works
        // ls > foo.txt
        // open foo.txt, duplicate opened filefd, execute using the first arg, then close
        if(redirectOut == 1) {
            int filefd = open(args[count-1], O_WRONLY|O_CREAT, 0666);
            close(1);//Close stdout
            dup(filefd);
            execlp(args[0], args[0], NULL);
            close(filefd);
        }
        // < should be implemented to redirect Input, read from a file
        // grep "Romeo" < skakespeare.txt
        // progress
        // grep foo < foo.txt
        if(redirectIn == 1) {
            int filefd = open(args[count-1], O_RDONLY);
            //close(1);//Close stdout
            dup2(filefd,0);
            close(filefd);
            execvp(args[1], args);
        }
        // works
        // ls >> foo.txt
        if(append == 1) {
            int filefd = open(args[count-1], O_CREAT | O_WRONLY | O_APPEND, 0666);
            close(1);
            dup(filefd);
            execlp(args[0], args[0], NULL);
            close(filefd);
        }
        execvp(args[0], args);
        perror("shell");
        exit(1);
    } else if(pid > 0){
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    } else {
        perror("shell");
    }
}

// ------------------------------------------------------------------------------------------
// Batch "main" function
// ------------------------------------------------------------------------------------------
// Function to execute shell in batch mode
void batch(char *filename) {
    printf("We are in batch mode reading file: %s\n", filename);
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    size_t read;

    file = fopen(filename, "rb");

    while((read = getline(&line, &len, file)) != -1) {
        char **tokens = lineParse(line);
        execution_time(tokens);
    }

    fclose(file);

    if(line) {
        free(line);
    }

    exit(0);
}

// ------------------------------------------------------------------------------------------
// Interactive "main" function
// ------------------------------------------------------------------------------------------
// Function to execute shell in interactive mode
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

// ------------------------------------------------------------------------------------------
// Main function
// ------------------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    // if only 1 argument go to interactive mode
    if(argc == 1) {
        interactive();
    }

    // if only 2 arguments go to batch mode
    if(argc == 2) {
        batch(argv[1]);
    }

    // if 3 arguments or more go to error
    if(argc >= 3) {
        perror("Too many arguments");
        exit(0);
    }
}
