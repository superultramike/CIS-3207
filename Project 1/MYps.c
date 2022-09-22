#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void printStime(int pid);
void printUtime(int pid);
void printStat(int pid);
void printSize(int pid);
void printCmdline(int pid);

int main(int argc, char *argv[]) {
    int opt;
    int pid = 0;
    int cflag = 0;
    int uFlag = 0;

    while((opt = getopt(argc, argv, "p:s:U:S:v:c:")) != -1) {
        switch(opt) {
            // NOT DONE
            case 'p':
                //printf("(p) Display process information only for the process whose number is pid.\n");
                pid = atoi(optarg);
                printf("PID: %d ", pid);
                break;

            // DONE
            // using the given user pid, open the specific proc stat file for given pid
            case 's':
                //printf("(s) Display the single-character state information about the process.\n");
                printStat(pid);
                break;


            // DONE
            // If U is present (uflag = 1) don't display the user timer, if not present then display user time
            case 'U':
                //printf("(U) DO NOT Display the amount of user time consumed by this process\n");
                uFlag = 1;
                break;

            // DONE
            // Use strtok to parse through to the 15th element "stime" (INTERMEDIATE);
            case 'S':
                //printf("(S) Display the amount of system time consumed so far by this process\n");
                printStime(pid);
                break;

            // DONE
            // Use the fscanf method to capture this data (EASY)
            case 'v':
                //printf("(v) Display the amount of virtual memory currently used (in pages) by this program\n");
                printSize(pid);
                break;

            // DONE
            // Make 'c' a flag case so that if it's true it doesn't run the print statement below this case
            case 'c':
                //printf("(c) DO NOT Display the command line that started this program\n");
                cflag = 1;
                break;
        }
    }

    // DONE
    if(uFlag == 0) {
        // make a function inside here
        printUtime(pid);
    }

    // DONE
    if(cflag == 0) {
        printCmdline(pid);
    }

    printf("\n");

    for(; optind < argc; optind++) {
        printf("extra arguments: %s\n", argv[optind]);
    }
}

void printUtime(int pid) {
    // Declare variables to print out stat menu
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "rb");
    char *arg;
    size_t size = 0;
    while(getdelim(&arg, &size, 0, f) != -1) {
        // print out the entire contents of stat
        //printf("%s\n", arg);
        const char s[2] = " ";
        char *token;
        int tokenCount = 0;
        token = strtok(arg, s);

        // walk through the other tokens
        while(token != NULL) {
            //printf("%s\n", token );
            token = strtok(NULL, s);
            tokenCount++;
            if(tokenCount == 13) {
                printf(" UTIME: %s ", token);
                break;
            }
        }
    }
    fclose(f);
}

void printStime(int pid) {
    // Declare variables to print out stat menu
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "rb");
    char *arg;
    size_t size = 0;
    while(getdelim(&arg, &size, 0, f) != -1) {
        // print out the entire contents of stat
        //printf("%s\n", arg);
        const char s[2] = " ";
        char *token;
        int tokenCount = 0;
        token = strtok(arg, s);

        // walk through the other tokens
        while(token != NULL) {
            //printf("%s\n", token );
            token = strtok(NULL, s);
            tokenCount++;
            if(tokenCount == 14) {
                printf(" STIME: %s ", token);
                break;
            }
        }
    }
    fclose(f);
}

void printCmdline(int pid) {
    char filename[1000];
    sprintf(filename, "/proc/%d/cmdline", pid);
    FILE *f = fopen(filename, "r");

    char comm[1000];
    fscanf(f, "%s", comm);
    //printf("DO Display the command line that started this program\n");
    printf(" cmdline: %s ", comm);
    fclose(f);
}

void printStat(int pid) {
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "r");

    int unused, ppid;
    char comm[1000], state;
    fscanf(f, "%d %s %c %d", &unused, comm, &state, &ppid);
    printf(" State: %c ", state);
    fclose(f);
}

void printSize(int pid) {
    char filename[1000];
    sprintf(filename, "/proc/%d/statm", pid);
    FILE *f = fopen(filename, "r");

    int size;
    fscanf(f, "%d", &size);
    printf(" Size: %d ", size);
    fclose(f);
}
