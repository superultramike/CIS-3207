// given a specific PID, print out cmdline pathname
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void main(int argc, char **argv) {
    int pid;
    // scan the given pid in user argument
    sscanf(argv[1], "%d", &pid);
    // print the given pid in user argument
    printf("PID = %d\n", pid);

    // using the given user pid, open the specific proc stat file for given pid
    char filename[1000];
    sprintf(filename, "/proc/%d/cmdline", pid);
    FILE *f = fopen(filename, "r");

    char comm[1000];
    fscanf(f, "%s", comm);
    printf("pathname: %s\n", comm);
    fclose(f);
}
