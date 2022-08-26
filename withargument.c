#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent* dp;

    // no path
    if (argc == 1) {
        argv[1] = ".";
        dir = opendir(argv[1]);

        do {
            dp = readdir(dir);
            if (dp) {
                printf("%s \n", dp->d_name);
            }
        } while (dp);

        closedir(dir);
    }

    // path provided
    if (argc > 1) {
        dir = opendir(argv[1]);

        do {
            dp = readdir(dir);
            if (dp) {
                printf("%s \n", dp->d_name);
            }
        } while (dp);

        closedir(dir);
    }

    return 0;
}
