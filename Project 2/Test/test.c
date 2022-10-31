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
char currentWorkingDirectory[250];
// ------------------------------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------------------------------
char** lineParse(char *line);
void execArg(char **args);
void listFiles(char* dirname);

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
    int redirectIn = 0;
    int redirectOut = 0;
    int append = 0;
    int count = 0;

    while(args[count] != NULL) {
        //printf("args[%d] = %s\n", count, args[count]);
        // redirectOut >
        if(strcmp(args[count], ">") == 0) {
            redirectOut = 1;
        }
        // redirectIn <
        else if(strcmp(args[count], "<") == 0) {
            redirectIn = 1;
        }
        // append >>
        else if(strcmp(args[count], ">>") == 0) {
            append = 1;
        }
        // if quit is found
        else if(strcmp(args[count], "quit") == 0) {
            exit(0);
        }
        // if quit is found
        else if(strcmp(args[count], "exit") == 0) {
            exit(0);
        }
        count++;
    }

    // ls > foo.txt WORKS
    if(redirectOut == 1) {
        int filefd = open(args[count-1], O_WRONLY|O_CREAT, 0666);
        close(1);//Close stdout
        dup(filefd);
        // CHANGE THIS ONE
        execlp(args[0], args[0], NULL);
        close(filefd);
    }
    // grep Romeo < skakespeare.txt WORKS
    else if(redirectIn == 1) {
        int fd = open(args[count-1], O_RDONLY);
        int sin = dup(0);
        dup2(fd, 0);
        dup2(sin,0);
        close(fd);
    }

    // ls >> foo.txt WORKS
    else if(append == 1) {
        int filefd = open(args[count-1], O_CREAT | O_WRONLY | O_APPEND, 0666);
        close(1);
        dup(filefd);
        // CHANGE THIS ONE
        execlp(args[0], args[0], NULL);
        close(filefd);
    }

    execArg(args);
}

void execArg(char **args) {
    char filePath[250];
    char *path = "/bin/";
    int command = 0;
    int current = 0;

    // check all arguments
    while(args[current] != NULL) {
        // if cd is found sorta works
        if(strcmp(args[current], "cd") == 0) {
            command = 1;
            if(args[current + 1] != NULL) {
                if(args[current + 2] != NULL) {
                    printf("Too many arguments!\n");
                    break;
                }
                if(chdir(args[current + 1]) != 0) {
                    printf("%s\n", strerror(errno));
                }
            }
            else {
                printf("%s\n", currentWorkingDirectory);
            }
        }
        // if environ is found
        else if(strcmp(args[current], "environ") == 0) {
            command = 1;
            char **s = environ;

            for(; *s; s++) {
                printf("%s\n", *s);
            }
        }
        // if echo is found
        else if(strcmp(args[current], "echo") == 0) {
            command = 1;
            while(*++args) {
                printf("%s", *args);
                if(args[1]) printf(" ");
            }
            printf("\n");
        }
        // if pause is found
        else if(strcmp(args[current], "pause") == 0) {
            command = 1;
            printf("Program is paused\n");
            printf("Press ENTER key to Continue\n");
            getchar();
        }
        // if help is found
        if(strcmp(args[current], "help") == 0) {
            command = 1;
            printf("Welcome to the help function\n");
        }
        // if clear is found
        else if(strcmp(args[current], "clear") == 0 || strcmp(args[current], "clr") == 0) {
            command = 1;
            printf("\e[1;1H\e[2J");
        }
        // if dir is found
        else if(strcmp(args[current], "dir") == 0) {
            command = 1;
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
        current++;
    }

    if(command == 0) {
        //setenv("PATH", "/bin/", 1);
        //filePath = getenv("PATH");
        strcpy(filePath, path);
        strcat(filePath, args[0]);

        // fork here
        int pid = fork();

        // child process
        if(pid == 0) {
            if(execv(filePath, args) != -1) {
                // run argument
            }
            // try current directory
            else {
                strcpy(filePath, currentWorkingDirectory);
                strcat(filePath, "/");
                strcat(filePath, args[0]);
                if(execv(filePath, args) != -1) {
                    exit(0);
                    // does nothing
                }
                else {
                    printf("%s\n", strerror(errno));
                    exit(0);
                }
            }
            perror("shell");
            exit(1);
        // parent process
        } else if(pid > 0){
            int status;
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        } else {
            perror("shell");
        }
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

// ------------------------------------------------------------------------------------------
// Interactive "main" function
// ------------------------------------------------------------------------------------------
// Function to execute shell in interactive mode
void interactive() {
    char *line = NULL;
    size_t len = 0;
    size_t read;
    getcwd(currentWorkingDirectory, 100);

    while(1) {
        printf("%s/myshell> ", currentWorkingDirectory);
        while((read = getline(&line, &len, stdin)) != 1) {
            char **tokens = lineParse(line);
            execution_time(tokens);
            printf("%s/myshell> ", currentWorkingDirectory);
        }
    }
}

// ------------------------------------------------------------------------------------------
// Main function
// ------------------------------------------------------------------------------------------
int main(int argc, char *argv[], char *envp[]) {
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
