#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 1024

void printStime(int pid);
void printUtime(int pid);
void printStat(int pid);
void printSize(int pid);
void printCmdline(int pid);

int main(int argc, char *argv[]) {
    int opt;
    int pid = 0;
    int pflag = 0;
    int sflag = 0;
    int Uflag = 0;
    int Sflag = 0;
    int vflag = 0;
    int cflag = 0;
    int pids[1024];

    while((opt = getopt(argc, argv, "p:sUSvc")) != -1) {
        switch(opt) {
            // NOT DONE
            case 'p':
                //printf("(p) Display process information only for the process whose number is pid.\n");
                pid = atoi(optarg);
                pflag = 1;
                break;

            // DONE
            // using the given user pid, open the specific proc stat file for given pid
            case 's':
                //printf("(s) Display the single-character state information about the process.\n");
                //printStat(pid);
                sflag = 1;
                break;

            // DONE
            // If U is present (uflag = 1) don't display the user timer, if not present then display user time
            case 'U':
                //printf("(U) DO NOT Display the amount of user time consumed by this process\n");
                Uflag = 1;
                break;

            // DONE
            // Use strtok to parse through to the 15th element "stime" (INTERMEDIATE);
            case 'S':
                //printf("(S) Display the amount of system time consumed so far by this process\n");
                //printStime(pid);
                Sflag = 1;
                break;

            // DONE
            // Use the fscanf method to capture this data (EASY)
            case 'v':
                //printf("(v) Display the amount of virtual memory currently used (in pages) by this program\n");
                //printSize(pid);
                vflag = 1;
                break;

            // DONE
            // Make 'c' a flag case so that if it's true it doesn't run the print statement below this case
            case 'c':
                //printf("(c) DO NOT Display the command line that started this program\n");
                cflag = 1;
                break;
        }
    }
    printf("OUTSIDE\n");

    // if a PID is provided
    if(pflag == 1) {
        printf("INSIDE\n");
        printf("PID is %d", pid);
        if(sflag == 1) {
            printStat(pid);
        }
        if(Uflag == 0) {
            printUtime(pid);
        }
        if(Sflag == 1) {
            printStime(pid);
        }
        if(vflag == 1) {
            printSize(pid);
        }
        if(cflag == 0) {
            printCmdline(pid);
        }
    }

    // if a PID isn't provided aka run through all pids
    if(pflag == 0) {
        for(int i=0; i<MAX_SIZE; i++) {
            // go use functions with every pid
        }
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
