// if int argc is 1, then just search current directory
// if int argc is greater then one

#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

void recursive_dir(int indent, char * pathname);

int main(int argc, char *argv[]) {
    // Directory path to list files
    char path[100];

    //Input path from user
    printf("path: ");
    scanf("%s", path);

    recursive_dir(0, path);

    return 0;
}

void recursive_dir(int indent, char * pathname) {
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(pathname);

    if (!dir) {
        return 1;
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            for (int i=0; i<indent; i++) {
                if (i%2 == 0 || i == 0)
                    printf("_");
            }

            printf("%s\n", dp->d_name);

            strcpy(path, pathname);
            strcat(path, "/");
            strcat(path, dp->d_name);
            recursive_dir(indent + 2, path);
        }
    }

    closedir(dir);
    return 0;
}
