#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

void recursive_dir(int indent, char * pathname);

int main(int argc, char *argv[]) {
    char pathname[100];

    printf("path: ");
    scanf("%s", pathname);

    recursive_dir(0, pathname);

    return 0;
}

void recursive_dir(int indent, char * pathname) {
    char path[1000];
    DIR *dir = opendir(pathname);
    struct dirent* dp;

    if (!dir) {
        return;
    }

    // While there is still a file or directory to scan do this
    while((dp = readdir(dir)) != NULL) {
        //
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            printf("%s\n", dp->d_name);

            strcpy(path, pathname);
            strcat(path, "/");
            strcat(path, dp->d_name);
            recursive_dir(indent+2, path);
        }
    }

    closedir(dir);

    /*
    //If no args
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

    //If specified directory
    if (argc > 1) {
        argv[1] = "..";
        dir = opendir(argv[1]);

        do {
            dp = readdir(dir);
            if (dp) {
                printf("%s \n", dp->d_name);
            }
        } while (dp);

        closedir(dir);
    }
    */
}
