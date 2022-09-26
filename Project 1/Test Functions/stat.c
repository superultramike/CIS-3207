// given a specific PID, print out the first
// 4 given elements of the filesystem for stat
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
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "r");

    int unused, ppid;
    char comm[1000], state;
    fscanf(f, "%d %s %c %d", &unused, comm, &state, &ppid);
    printf("Exec-file-name = %s\n", comm);
    printf("State = %c\n", state);
    printf("Parent PID (ppid) = %d\n", ppid);
    fclose(f);
}
