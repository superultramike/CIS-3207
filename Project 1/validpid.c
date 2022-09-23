#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#define MAX_SIZE 1024

void process_all_processes ();
int find_valid_pids (char *charPID);

int main() {
    process_all_processes();

    return 0;
}

// go through all processes
// extract every processes's UID
// if the UID and specific process's UID match,
// then convert specific PID to an int and send it to other functions
void process_all_processes (void) {
    struct dirent *dirent;
    DIR *dir;
    int pid, r;

    // find all processes
    if (!(dir = opendir ("/proc"))) {
        perror (NULL);
        exit (EXIT_FAILURE);
    }

    int p[MAX_SIZE];
    int i=0;

    while (dirent = readdir (dir)) {
        // we are only interested in process IDs
        if (atoi(dirent -> d_name) != 0) {
            // A string version of the PID
            //printf("PID: %s\n", dirent->d_name);
            if(find_valid_pids(dirent->d_name) == 1) {
                pid = atoi (dirent -> d_name);
                p[i] = pid;
                printf("p[%d] = %d\n", i, p[i]);
                i++;
            }
        }
    }

    for(i=0; i<MAX_SIZE; i++) {
        printf("p[%d] = %d\n", i , p[i]);
    }

    closedir (dir);
}

int find_valid_pids (char *charPID) {
    // obtain UID of each process
    int pid;
    char filename[1000];
    sprintf(filename, "/proc/%s/loginuid", charPID);
    FILE *f = fopen(filename, "r");

    char uid[1000];
    fscanf(f, "%s", uid);

    // obtain the TRUE SELF uid
    FILE *uidInfo = fopen("/proc/self/loginuid", "rb");
    char *selfuid = 0;
    size_t size = 0;

    while(getdelim(&selfuid, &size, 0, uidInfo) != -1) {
        fscanf(uidInfo, "%s", selfuid);
    }
    fclose(uidInfo);

    if(strcmp(selfuid, uid) == 0) {
        //printf("PID %s has the same UID %s as selfUID %s\n", charPID, uid, selfuid);
        // Valid pid is found
        return 1;
    }
}
