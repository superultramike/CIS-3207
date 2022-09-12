// given a specific PID, print out the first
// given element of the filesystem for statm
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
    sprintf(filename, "/proc/%d/statm", pid);
    FILE *f = fopen(filename, "r");

    int size;
    fscanf(f, "%d", &size);
    printf("Size: %d\n", size);
    fclose(f);
}
