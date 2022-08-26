#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    DIR *d;
    struct dirent* dir;

    // if one argument is provided do this process
    if (argc == 1) {
        // specifies that we are looking at the current directory with ./
        argv[1] = ".";
        // declare the DIR stream pointer to the position of argument 1 which is a "./"
        d = opendir(argv[1]);

        // Do this process while we still have a directory or file available.
        do {

            dir = readdir(d);
            if (dir) {
                printf("%s \n", dir->d_name);
            }
        } while (dir);

        closedir(d);
    }

    // if more then one argument is provided do this process
    if (argc > 1) {
        // declare the DIR stream pointer to the position of argument 1 which is something..?
        d = opendir(argv[1]);

        // Do this process while we still have a directory or file available
        do {
            dir = readdir(d);
            if (dir) {
                printf("%s \n", dir->d_name);
            }
        } while (dir);

        closedir(d);
    }

    return 0;
}
