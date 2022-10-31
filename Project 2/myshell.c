// Program Imports
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

// Global Variables
extern char **environ;
char currentWorkingDirectory[250];

// Function Prototypes
char** lineParse(char *line);
void execArg(char **args);
void listFiles(char* dirname);
void execPipe(char **args, int count);

// Input launcher
// Function to parse lines
char** lineParse(char *line) {
    // Starter variables
    int length = 0;
    int cap = 16;
    char **tokens = malloc(cap * sizeof(char*));

    // Identify delimiters
    char *delimiters = " \t\r\n";
    // Get first token
    char *token = strtok(line, delimiters);

    // While there are still tokens available...
    while(token != NULL) {
        // ...Declare spot of token
        tokens[length] = token;

        // Move on to next token
        length++;

        // If we have more then 16 tokens then...
        if(length >= cap) {
            // ...Increase the cap by small increments
            cap = (int) (cap * 1.5);
            tokens = realloc(tokens, cap * sizeof(char*));
            printf("Raised the cap\n");
        }
        // Get next token
        token = strtok(NULL, delimiters);
    }
    // Nullify last token space
    tokens[length] = NULL;
    // Return all tokens back to main
    return tokens;
}

// Parent function that controls flow of all given commands
// regardless of built-in or external
void execution_time(char **args) {
    // Declare flags
    int redirectIn = 0;
    int redirectOut = 0;
    int append = 0;
    int count = 0;
    int pipeFlag = 0;

    // Declare error checking
    char error_message[30] = "an error has occured\n";

    // While there are still arguments to be processed
    while(args[count] != NULL) {
        // If redirect out symbol ">" is found...
        if(strcmp(args[count], ">") == 0) {
            //...raise flag
            redirectOut = 1;
        }
        // If redirect in symbol "<" is found...
        else if(strcmp(args[count], "<") == 0) {
            //...raise flag
            redirectIn = 1;
        }
        // If append symbol ">>" is found...
        else if(strcmp(args[count], ">>") == 0) {
            //...raise flag
            append = 1;
        }
        // If pipe symbol "|" is found...
        else if(strcmp(args[count], "|") == 0) {
            //...raise flag
            pipeFlag = 1;
        }
        // If quit is found
        else if(strcmp(args[count], "quit") == 0) {
            //...exit out of shell
            exit(0);
        }
        // If quit is found...
        else if(strcmp(args[count], "exit") == 0) {
            //...exit out of shell
            exit(0);
        }
        // Increment to the next argument
        count++;
    }

    // error checking if a redirection and a pipe is present
    if( (pipeFlag == 1 && redirectIn == 1) || (pipeFlag == 1 && redirectOut == 1) || (pipeFlag == 1 && append == 1)) {
        write(STDERR_FILENO, error_message, strlen(error_message));
    }

    // ls > foo.txt WORKS
    // If redirectOut flag was raised...
    else if(redirectOut == 1) {
        //..then fork process to execute on command without leaving shell
        int redirectOut_pid = fork();
        // Child process of redirectOut process
        if(redirectOut_pid == 0) {
            // open FD of right side argument
            int filefd = open(args[count-1], O_WRONLY|O_CREAT, 0666);
            // close STDOUT
            close(1);
            // Dup the FD of right side
            dup(filefd);
            // Execute argument set
            execlp(args[0], args[0], NULL);
            // Close FD of right side
            close(filefd);
        }
        // Parent process of redirectOut process
        else {
            // Wait until child process is done to execute
            wait(NULL);
        }
    }

    // grep Romeo < skakespeare.txt WORKS
    // If redirectIn flag was raised...
    else if(redirectIn == 1) {
        // open FD of right side argument
        int fd = open(args[count-1], O_RDONLY);
        // Duplicate FD and declare it in variable
        int sin = dup(0);
        // Put FD of right side argument into FD 0
        dup2(fd, 0);
        // Put Sin FD of right argument into Sin 0
        dup2(sin,0);
        // Close FD and execute args
        close(fd);
        execArg(args);
    }

    // ls >> foo.txt WORKS
    // If append flag was raised...
    else if(append == 1) {
        //..then fork process to execute on command without leaving shell
        int append_pid = fork();
        // Child process of redirectOut process
        if(append_pid == 0) {
            // open FD of right side argument
            int filefd = open(args[count-1], O_CREAT | O_WRONLY | O_APPEND, 0666);
            // Close FD, duplicate it and execute using new FD
            close(1);
            dup(filefd);
            execlp(args[0], args[0], NULL);
            close(filefd);
        }
        // Parent process of append process
        else {
            // Wait until child process is done to execute
            wait(NULL);
        }
    }

    // If pipe flag was raised...
    else if(pipeFlag == 1) {
        //..execute on pipe arguments
        execPipe(args, count);
    }

    // if all flags are zero...
    else if(pipeFlag == 0 && append == 0 && redirectIn == 0 && redirectOut == 0) {
        //...then it's a built in we can execute on
        execArg(args);
    }
}

// Function to execute on pipes
// Test cases:       cat test.txt | sort        ls -l | more
void execPipe(char **args, int count) {
    // Declare position of the pipe
    int position = 0;

    // find position of pipe and nullify it
    for(int j=0; j<count-1; j++) {
        if((strcmp(args[j], "|") == 0)) {
            args[j] = NULL;
            position = j;
        }
    }
    position++;

    // Declare file descriptors
    int fds[2];

    // error checking
    if(pipe(fds) != 0) {
        printf("%s\n", strerror(errno));
    }
    // If no error then continue
    else {
        // Declaring first process
        int pid = fork();
        // error checking
        if(pid == -1) {
            printf("%s\n", strerror(errno));
        }
        // Child Process to handle entirety of argument
        if(pid == 0) {
            close(1);
            dup2(fds[1], 1);
            close(fds[0]);
            execArg(args);
            exit(0);
        }
        // Parent process to handle the left args
        else {
            // Calculate size of left argument execution
            int newArraySize = count - position;
            char *leftArgs[newArraySize];

            for(int k=0; k<newArraySize; k++) {
                leftArgs[k] = args[position];
                position++;
            }

            // Fork for left side arguments
            int pid2 = fork();

            // Child process of left args
            if(pid2 == 0) {
                close(0);
                dup2(fds[0], 0);
                close(fds[1]);
                execlp(leftArgs[0], leftArgs[0], NULL);
                exit(0);
            }
            // Parent process of left args
            else {
                wait(NULL);
            }
        }
    }
    // Close all FDs
    close(fds[0]);
    close(fds[1]);
}

// Executing commands function
void execArg(char **args) {
    // Declare starter variables for mostly paths
    char filePath[250];
    char *path = "/bin/";
    int command = 0;
    int current = 0;

    // check all arguments
    while(args[current] != NULL) {
        // If cd is found then execute
        if(strcmp(args[current], "cd") == 0) {
            command = 1;
            // If no argument after cd is provided...
            if(args[1] == NULL) {
                //...then just print CWD
                printf("CWD: %s\n", currentWorkingDirectory);
            }
            // If a argument after cd is provided...
            else {
                //...If user-given directory isn't found then print error
                if(chdir(args[1]) == -1) {
                    printf(" %s: no directory found\n", args[1]);
                }
                //...If user-given directory is found...
                else {
                    //...Change CWD to user-input
                    strcat(currentWorkingDirectory, "/");
                    strcat(currentWorkingDirectory, args[1]);
                    // Print new CWD
                    printf("CHANGED DIRECTORY to %s\n", currentWorkingDirectory);
                    setenv("PWD", currentWorkingDirectory, 1);
                }
            }
        }

        // If environ is found then execute
        else if(strcmp(args[current], "environ") == 0) {
            command = 1;
            char **s = environ;

            // Loop through and print all given environ variables
            for(; *s; s++) {
                printf("%s\n", *s);
            }
        }

        // If echo is found then execute
        else if(strcmp(args[current], "echo") == 0) {
            command = 1;
            int count = 1;

            // While there is a command present...
            while(args[count] != NULL) {
                //...print it out
                printf("%s ", args[count]);
                count++;
            }
            printf("\n");
        }
        // If pause is found then execute
        else if(strcmp(args[current], "pause") == 0) {
            command = 1;
            printf("Program is paused\n");
            printf("Press ENTER key to Continue\n");
            getchar();
        }
        // If help is found then execute
        if(strcmp(args[current], "help") == 0) {
            command = 1;
            // Fork for new process to execute more command
            int help_pid = fork();
            // Child process of help
            if(help_pid == 0) {
                char *help_args[] = {"more", "-d", "readme_doc", NULL};
                execvp("more", help_args);
            }
            // Parent process of help
            else {
                // Wait until child process is done to execute
                wait(NULL);
            }
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

    // If given argument isn't a built execute here
    if(command == 0) {
        // Execute using added file path like adding ls to /bin/
        strcpy(filePath, path);
        strcat(filePath, args[0]);

        // fork here
        int pid = fork();

        // child process
        if(pid == 0) {
            if(execv(filePath, args) != -1) {
                // run argument
            }
            // See if the command is present in the CWD
            else {
                strcpy(filePath, currentWorkingDirectory);
                strcat(filePath, "/");
                strcat(filePath, args[0]);

                // If execution doesn't work...
                if(execv(filePath, args) != -1) {
                    //...do nothing and exit
                    exit(0);
                }
                // If command isn't in bin or CWD...
                else {
                    //...then print error
                    printf("%s\n", strerror(errno));
                    exit(0);
                }
            }
            // Error checking
            perror("shell");
            exit(1);
        }
        // Parent Process
        else if(pid > 0){
            wait(NULL);
        }
        // Error checking
        else {
            perror("shell");
        }
    }
}

// Helper Function that recursively prints the files
// and directory contents of a current/given directory
void listFiles(char* dirname) {
    DIR* dir = opendir(dirname);

    if(dir == NULL) {
        return;
    }
    printf("Reading files in: %s\n", dirname);

    struct dirent* entity;

    // either gives us NULL or something to read
    entity = readdir(dir);
    // while it still has something to read perform this action
    while(entity != NULL) {
        // prints the filepath and then the name of element
        printf("%s/%s\n", dirname, entity->d_name);
        // if the entity is a directory and the name of the entity isn't a relative or parent directory that was already printed
        if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            // gives enough characters to build new strings for new pathnames
            char path[1000] = {0};
            // adds the original pathname
            strcat(path, dirname);
            // adds the slash to the pathname
            strcat(path, "/");
            // adds new file path to old file path
            strcat(path, entity->d_name);
            // then sends the newly constructed file path to the function over again
            // to actually print out it contents
            listFiles(path);
        }
        // assign the entity to the contents when it is read by readdir function
        // if there is nothing to read then you break out of the loop
        entity = readdir(dir);
    }
    // close the directory once you are done reading
    closedir(dir);
}

// Function to execute shell in batch mode
void batch(char *filename) {
    // Starter variables
    printf("We are in batch mode reading file: %s\n", filename);
    FILE *file = fopen(filename, "rb");
    char *line = NULL;
    size_t len = 0;
    size_t read;

    // Set and get environment variables from given starter path
    setenv("PATH", "/bin/", 1);
    getcwd(currentWorkingDirectory, 100);

    // Error checking for opening files
    if(file == NULL) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    // Scan given file while there is content
    while((read = getline(&line, &len, file)) != -1) {
        // Declare a string of tokens collected by lineParser...
        char **tokens = lineParse(line);
        // ...then execute using said tokens
        execution_time(tokens);
    }
    fclose(file);

    // Error checking on memory contents of line
    if(line) {
        free(line);
    }

    exit(0);
}

// Interactive "main" function
// Function to execute shell in interactive mode
void interactive() {
    // Starter variables
    char *line = NULL;
    size_t len = 0;
    size_t read;

    // Set and get environment variables from given starter path
    setenv("PATH", "/bin/", 1);
    getcwd(currentWorkingDirectory, 100);

    // Constant loop until user-given exit command
    while(1) {
        printf("%s/myshell> ", currentWorkingDirectory);
        // Scan given file while there is content
        while((read = getline(&line, &len, stdin)) != 1) {
            // Declare a string of tokens collected by lineParser...
            char **tokens = lineParse(line);
            // ...then execute using said tokens
            execution_time(tokens);
            printf("%s/myshell> ", currentWorkingDirectory);
        }
    }
}

// Main function
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
