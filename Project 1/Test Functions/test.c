// library imports
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX_SIZE 1024

// function prototypes
void process_all_processes ();
int find_valid_pids (char *charPID);
void printStime(int pid);
void printUtime(int pid);
void printStat(int pid);
void printSize(int pid);
void print_cmdline(int pid);

// global variables
int p[MAX_SIZE];
int i = 0;

// main function
int main(int argc, char *argv[]) {
    // Initialized variable
    int opt;
    int pid = 0, pflag = 0, sflag = 0, Uflag = 0, Sflag = 0, vflag = 0, cflag = 0;

    // command line parser
    while((opt = getopt(argc, argv, "p:sUSvc")) != -1) {
        // based on the char command it's looking at, execute it
        switch(opt) {
            // Extract PID given by user and raise flag
            case 'p':
                pid = atoi(optarg);
                pflag = 1;
                break;

            // Raise flag to print STATE of a PID
            case 's':
                sflag = 1;
                break;

            // Raise flag to NOT print Utime (print otherwise)
            case 'U':
                Uflag = 1;
                break;

            // Raise flag to print Stime
            case 'S':
                Sflag = 1;
                break;

            // Raise flag to print V-memory of a PID
            case 'v':
                vflag = 1;
                break;

            // Raise flag to NOT print command line (print otherwise)
            case 'c':
                cflag = 1;
                break;
        }
    }

    // if a PID is provided
    if(pflag == 1) {
        // print the PID
        printf("PID is %d", pid);
        // if -s is present, print STATE
        if(sflag == 1) {
            printStat(pid);
        }
        // if -U wasn't present then print Utime
        if(Uflag == 0) {
            printUtime(pid);
        }
        // if -S is present, print Stime
        if(Sflag == 1) {
            printStime(pid);
        }
        // if -v is present, print Size
        if(vflag == 1) {
            printSize(pid);
        }
        // if -c wasn't present then print cmdline
        if(cflag == 0) {
            print_cmdline(pid);
        }
    }

    // if a PID isn't provided aka run through all pids
    if(pflag == 0) {
        // gather all valid PIDs and build PID array
        process_all_processes();
        // loop through PID array and apply print functions to every PID
        for(int j=0; j<i; j++) {
            // print the PID
            printf("PID: %d", p[j]);
            // if -s is present, print STATE
            if(sflag == 1) {
                printStat(p[j]);
            }
            // if -U wasn't present then print Utime
            if(Uflag == 0) {
                printUtime(p[j]);
            }
            // if -S is present, print Stime
            if(Sflag == 1) {
                printStime(p[j]);
            }
            // if -v is present, print Size
            if(vflag == 1) {
                printSize(p[j]);
            }
            // if -c wasn't present then print cmdline
            if(cflag == 0) {
                print_cmdline(p[j]);
            }
            printf("\n");
        }
    }

    // print extra arguments if present in cmdline
    for(; optind < argc; optind++) {
        printf("extra arguments: %s\n", argv[optind]);
    }
}

// parent function that finds all valid PIDs
// and adds valid PIDs to global p[] array
void process_all_processes (void) {
    // starter variables
    struct dirent *dirent;
    DIR *dir;
    int pid;

    // find all processes inside parent proc file
    if (!(dir = opendir ("/proc"))) {
        perror (NULL);
        exit (EXIT_FAILURE);
    }

    // Keep reading dirent if there is a directory present
    while ((dirent = readdir(dir))) {
        // If it's integer...
        if (atoi(dirent -> d_name) != 0) {
            // ...then find if it's a valid PID
            // If it's valid...
            if(find_valid_pids(dirent->d_name) == 1) {
                // ...then convert valid pid to an int
                pid = atoi (dirent -> d_name);
                // and place valid pid into PID array
                p[i] = pid;
                i++;
            }
        }
    }
    // close directory since we have needed data
    closedir(dir);
}

// given a specific PID, verify if it's valid
int find_valid_pids (char *charPID) {
    // obtain UID of specific PID
    char filename[1000];
    sprintf(filename, "/proc/%s/loginuid", charPID);
    FILE *f = fopen(filename, "r");
    char uid[1000];
    fscanf(f, "%s", uid);

    // obtain the TRUE UID by opening proc file
    FILE *uidInfo = fopen("/proc/self/loginuid", "rb");
    char *selfuid = 0;
    size_t size = 0;

    // scan given file while there is content
    while(getdelim(&selfuid, &size, 0, uidInfo) != -1) {
        // the only content present is the TRUE UID
        fscanf(uidInfo, "%s", selfuid);
    }
    fclose(uidInfo);

    // if true UID and UID of specific PID match...
    if(strcmp(selfuid, uid) == 0) {
        // ...then return true
        return 1;
    }
    return 0;
}

// function to print Utime
void printUtime(int pid) {
    // Starter variables
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "rb");
    char *arg;
    size_t size = 0;

    // scan given file while there is content
    while(getdelim(&arg, &size, 0, f) != -1) {
        // use a space as a token since all elements in stat are separated by a space
        const char s[2] = " ";
        char *token;
        // keep a counter to know when 14th element (Utime) is reached
        int tokenCount = 0;
        token = strtok(arg, s);

        // walk through proc/[pid]/stat file
        while(token != NULL) {
            token = strtok(NULL, s);
            tokenCount++;
            // once token reaches 13th element the next token (the 14th) is the uTime
            if(tokenCount == 13) {
                printf(" UTIME: %s ", token);
                break;
            }
        }
    }
    // close file since we have needed data
    fclose(f);
}

// function to print Stime
void printStime(int pid) {
    // Starter variables
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "rb");
    char *arg;
    size_t size = 0;

    // scan given file while there is content
    while(getdelim(&arg, &size, 0, f) != -1) {
        // use a space as a token since all elements in stat are separated by a space
        const char s[2] = " ";
        char *token;
        // keep a counter to know when 14th element (Utime) is reached
        int tokenCount = 0;
        token = strtok(arg, s);

        // walk through proc/[pid]/stat file
        while(token != NULL) {
            token = strtok(NULL, s);
            tokenCount++;
            // once token reaches 13th element, the next token (the 14th) is the sTime
            if(tokenCount == 14) {
                printf(" STIME: %s ", token);
                break;
            }
        }
    }
    // close file since we have needed data
    fclose(f);
}

// prints the argument list, of the process
void print_cmdline(int pid) {
    int fd;
    char filename[24];
    char arg_list[1024];
    size_t length;
    char* next_arg;

    // Generate the name of the cmdline for the process
    snprintf(filename, sizeof(filename), "/proc/%d/cmdline", pid);
    // read the contents of the file
    fd = open(filename, O_RDONLY);
    length = read(fd, arg_list, sizeof(arg_list));
    close(fd);
    // read does not NUL-terminate the buffer, so do it here
    arg_list[length] = '\0';

    printf("cmdline: ");
    // loop over arguments. arguments are separated by NULs
    next_arg = arg_list;
    while(next_arg < arg_list + length) {
        // print the argument. each is NUL-terminated so just treat it like a string
        printf("%s ", next_arg);
        // advance to the next argument. Since each argument is NUL-terminated, strlen counts the length of the next argument, not the entire argument list
        next_arg += strlen(next_arg) + 1;
    }
}

// function to print state
void printStat(int pid) {
    // open proc file where state is stored
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "r");

    // acquire state from proc file
    // with how fscanf works we can't "skip" to the 4th element where state is
    int unused, ppid;
    char arr[1000], state;
    fscanf(f, "%d %s %c %d", &unused, arr, &state, &ppid);
    printf(" State: %c ", state);

    // close file since we have needed data
    fclose(f);
}

// function to print v-memory size
void printSize(int pid) {
    // open proc file where size is stored
    char filename[1000];
    sprintf(filename, "/proc/%d/statm", pid);
    FILE *f = fopen(filename, "r");

    // acquire size from proc file
    int size;
    fscanf(f, "%d", &size);
    printf(" Size: %d ", size);

    // close file since we have needed data
    fclose(f);
}
