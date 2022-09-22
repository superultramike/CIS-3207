#include <stdio.h>
#include <stdlib.h> // For exit()
#include <dirent.h>
#include <errno.h>

void process_all_processes (void);
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

    while (dirent = readdir (dir)) {
        // we are only interested in process IDs
        if (atoi(dirent -> d_name) != 0) {
            //pid = atoi (dirent -> d_name);
            // A string version of the PID
            //printf("PID: %s\n", dirent->d_name);
            find_valid_pids(dirent->d_name);
        }
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
    //printf("UID of PID %s is %s\n", pid, uid);

    // obtain the TRUE SELF uid
    FILE *uidInfo = fopen("/proc/self/loginuid", "rb");
    char *selfuid = 0;
    size_t size = 0;

    while(getdelim(&selfuid, &size, 0, uidInfo) != -1) {
        fscanf(uidInfo, "%s", selfuid);
    }
    fclose(uidInfo);

    //printf("UID IS: %s\n", selfuid);
    if(strcmp(selfuid, uid) == 0) {
        //printf("PID %s has the same UID %s as selfUID %s\n", charPID, uid, selfuid);
        pid = atoi(charPID);
        printf("Valid PID: %d\n", pid);
        return pid;
    }
}
