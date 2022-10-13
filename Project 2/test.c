// ------------------------------------------------------------------------------------------
// Program Imports
// ------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

// Function to repeat user input after 'echo'
void shell_echo(char **args) {
    while(*++args) {
        printf("%s", *args);
        if(args[1]) printf(" ");
    }
    printf("\n");
}

// Function to print all environ variables
void shell_environ() {
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
    {"cd", shell_cd},
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

// ------------------------------------------------------------------------------------------
// Batch "main" function - NOT WORKING
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

void execution_time(char **args) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtins[i].name) == 0) {
            builtins[i].func(args);
            return;
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
