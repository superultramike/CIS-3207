#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;

    // put ':' in the starting of the
    // string so that program can
    // distinguish between '?' and ':'
    while((opt = getopt(argc, argv, ":if:psUSvc")) != -1) {
    //while((opt = getopt(argc, argv, ":if:lrx")) != -1) {
        switch(opt) {
            // What does it specifically mean by process info?
            case 'p':
                printf("(p) Display process information only for the process whose number is pid.\n");
                break;
            // Use the fscanf method to capture this data (EASY)
            case 's':
                printf("(s) Display the single-character state information about the process.\n");
                break;
            // Make 'U' a flag case where 'S' can't run it's 'U' is true
            case 'U':
                printf("(U) DO NOT Display the amount of user time consumed by this process\n");
                break;
            // Use strtok to parse through to the 14th element "stime" (INTERMEDIATE);
            // and if 'U' is true, it can't be ran
            case 'S':
                printf("(S) Display the amount of system time consumed so far by this process\n");
                break;
            // Use the fscanf method to capture this data (EASY)
            case 'v':
                printf("(v) Display the amount of virtual memory currently used (in pages) by this program\n");
                break;
            // Make 'c' a flag case so that if it's true it doesn't run the print statement below this case
            case 'c':
                printf("(c) DO NOT Display the command line that started this program\n");
                break;
        }
        // Make a print out of the command line of said process ONLY IF 'c' is false
    }

    // optind is for the extra arguments
    // which are not parsed
    for(; optind < argc; optind++) {
        printf("extra arguments: %s\n", argv[optind]);
    }
}
