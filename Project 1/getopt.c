#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void printStat(int pid);
void printSize(int pid);
void printCmdline(int pid);

int main(int argc, char *argv[]) {
    int opt;
    int pid = 0;
    int cflag = 0;
    int uFlag = 0;

    while((opt = getopt(argc, argv, "p:sUSvc")) != -1) {
        switch(opt) {
            case 'p':
                printf("(p) Display process information only for the process whose number is pid.\n");
                pid = atoi(optarg);
                printf("PID given = %d\n", pid);
                break;

            // using the given user pid, open the specific proc stat file for given pid
            case 's':
                printf("(s) Display the single-character state information about the process.\n");
                printStat(pid);
                break;

            // Make 'U' a flag case where 'S' can't run it's 'U' is true
            case 'U':
                printf("(U) DO NOT Display the amount of user time consumed by this process\n");
                uFlag = 1;
                break;

            // Use strtok to parse through to the 15th element "stime" (INTERMEDIATE);
            // and if 'U' is true, it can't be ran
            case 'S':
                printf("(S) Display the amount of system time consumed so far by this process\n");
                if (uFlag == 0) {
                    // make a function inside here
                    printf("DISPLAY AMOUNT OF SYSTEM TIME");
                }
                break;

            // Use the fscanf method to capture this data (EASY)
            case 'v':
                printf("(v) Display the amount of virtual memory currently used (in pages) by this program\n");
                printSize(pid);
                break;

            // Make 'c' a flag case so that if it's true it doesn't run the print statement below this case
            case 'c':
                printf("(c) DO NOT Display the command line that started this program\n");
                cflag = 1;
                break;
        }
    }

    if(cflag == 0) {
        printCmdline(pid);
    }

    for(; optind < argc; optind++) {
        printf("extra arguments: %s\n", argv[optind]);
    }
}

void printCmdline(int pid) {
    char filename[1000];
    sprintf(filename, "/proc/%d/cmdline", pid);
    FILE *f = fopen(filename, "r");

    char comm[1000];
    fscanf(f, "%s", comm);
    printf("DO Display the command line that started this program\n");
    printf("pathname: %s\n", comm);
    fclose(f);
}

void printStat(int pid) {
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "r");

    int unused, ppid;
    char comm[1000], state;
    fscanf(f, "%d %s %c %d", &unused, comm, &state, &ppid);
    printf("State = %c\n", state);
    fclose(f);
}

void printSize(int pid) {
    char filename[1000];
    sprintf(filename, "/proc/%d/statm", pid);
    FILE *f = fopen(filename, "r");

    int size;
    fscanf(f, "%d", &size);
    printf("size = %d\n", size);
    fclose(f);
}
