// ------------------------------------------------------------------------------------------
// Program Imports
// ------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ------------------------------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------------------------------
void lineParse(char *line);

// ------------------------------------------------------------------------------------------
// Built in Commands (cd, clr, dir, environ, echo, help, pause, quit, exit)
// Functions that need to finished: cd, dir, environ, echo
// ------------------------------------------------------------------------------------------
// Function to execute built-in command quit
void shell_quit() {
    exit(0);
}

// Function to execute built-in command exit
void shell_exit() {
    exit(0);
}

// Function to execute built-in command clr
void shell_clr() {
    printf("\e[1;1H\e[2J");
}

// Function to execute built-in command help
void shell_help() {
    printf("Welcome to the help function\n");
}

// Function to execute built-in command pause
void shell_pause(char **args) {
    printf("Program is paused\n");
    printf("Press ENTER key to Continue\n");
    getchar();
}

struct builtin {
    char *name;
    void (*func)();
};

struct builtin builtins[] = {
    {"quit", shell_quit},
    {"pause", shell_pause},
    {"help", shell_help},
    {"exit", shell_exit},
    {"clr", shell_clr}
};

int num_builtins() {
    return sizeof(builtins) / sizeof(struct builtin);
}

// ------------------------------------------------------------------------------------------
// Input launcher
// ------------------------------------------------------------------------------------------
// Function to parse lines
void lineParse(char *line) {
    int length = 0;
    int cap = 16;
    char **tokens = malloc(cap * sizeof(char*));
    char a[cap][10];

    char *delimiters = " \t\r\n";
    char *token = strtok(line, delimiters);

    // THIS RUNS FOR EACH INDIVIDUAL TOKEN
    while(token != NULL) {
        strcpy(a[length], token);
        tokens[length] = token;

        // Check each individual token and see if it's a builtin
        for(int i=0; i<num_builtins(); i++) {
            if(strcmp(a[length], builtins[i].name) == 0) {
                builtins[i].func();
            }
        }

        length++;
        if(length >= cap) {
            cap = (int) (cap * 1.5);
            tokens = realloc(tokens, cap * sizeof(char*));
            printf("Raised the cap\n");
        }
        token = strtok(NULL, delimiters);
    }
    tokens[length] = NULL;
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

    if (file == NULL) {
        exit(EXIT_FAILURE);
    }

    while((read = getline(&line, &len, file)) != -1) {
        lineParse(line);
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
    printf("We are in interactive mode\n");
}

// ------------------------------------------------------------------------------------------
// Main function
// ------------------------------------------------------------------------------------------
int main(int argc, char *argv[], char *envp[]) {
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
