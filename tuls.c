#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

// Prototype for recursive function
// void recursive_dir(int indent, char * pathname);

int main(int argc, char **argv) {
    struct dirent **namelist;
    struct stat sb;
    int n;

    // print the # of arguments passed to program
    //printf("Argv[1]: %s\n", argv[1]);
    //printf("Number of arguments: %d\n", argc);

    //print out the string for each argument
    /*
    for(int i=0; i<argc; i++) {
        printf("Argument: %s\n", argv[i]);
    }
    */

    n = scandir(".", &namelist, NULL, alphasort);
    if (n == -1) {
        perror("scandir");
        exit(1);
    }

    while (n--) {
        //printf("%s\n", namelist[n]->d_name);
        stat(namelist[n]->d_name, &sb);
        if(S_ISREG(sb.st_mode)) {
            printf("%s is a file\n", namelist[n]->d_name);
        }
        if(S_ISDIR(sb.st_mode)) {
            printf("%s is a directory\n", namelist[n]->d_name);
        }
        free(namelist[n]);
    }
    free(namelist);

    exit(0);
}
