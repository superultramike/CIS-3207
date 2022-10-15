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
#include <dirent.h>

// ------------------------------------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------------------------------------
extern char **environ;
char starterPath[100];
// ------------------------------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------------------------------
char** lineParse(char *line);
void execution_time(char **args);
void listFiles(char* dirname);

// ------------------------------------------------------------------------------------------
// Built in Commands (cd, clr, dir, environ, echo, help, pause, quit, exit)
// Functions that need to finished: cd
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

// Function to print all contents (including subdirectories)
// of the current or specified directory
void shell_dir(char **args) {
    // 0 arguments to print contents of current directory
    // dir ./Test
    if(args[1] == NULL) {
        listFiles(".");
    }
    // 1 arguments to print contents of specified directory
    // dir /home/michael/Downloads
    if(args[1] != NULL) {
        listFiles(args[1]);
    }

    // If there is something present after the path name then return error
    if(args[2] != NULL) {
        printf("dir: cannot open directory\n");
        exit(1);
    }
}

// helper function to recursively prints the files
// and directory contents of a current/given directory
void listFiles(char* dirname) {
    DIR* dir = opendir(dirname);

    if(dir == NULL) {
        return;
    }
    printf("Reading files in: %s\n", dirname);

    struct dirent* entity;

    entity = readdir(dir);
    while(entity != NULL) {
        printf("%s/%s\n", dirname, entity->d_name);
        if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            char path[1000] = {0};
            strcat(path, dirname);
            strcat(path, "/");
            strcat(path, entity->d_name);
            listFiles(path);
        }
        entity = readdir(dir);
    }
    closedir(dir);
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
void shell_environ(char **args) {
    char **s = environ;

    for(; *s; s++) {
        printf("%s\n", *s);
    }
}

// Is this working?
void shell_cd(char **args) {
    if(args[1] == NULL) {
        printf("CWD: %s\n", starterPath);
    }
    else if(args[1] != NULL) {
        //char addedPath[100];
        strcat(starterPath, "/");
        strcat(starterPath, args[1]);
        printf("New Path: %s\n", starterPath);
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
    {"clear", shell_clr},
    {"cd", shell_cd},
    {"dir", shell_dir},
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
        // works BUT weird print out error at the beginning
        // grep Romeo < skakespeare.txt
        if(redirectIn == 1) {
            int fd = open(args[count-1], O_RDONLY);
            int sin = dup(0);
            dup2(fd, 0);
            dup2(sin,0);
            close(fd);
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
    getcwd(starterPath, 100);

    while(1) {
        printf("%s/myshell> ", starterPath);
        while((read = getline(&line, &len, stdin)) != 1) {
            char **tokens = lineParse(line);
            execution_time(tokens);
            printf("%s/myshell> ", starterPath);
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
