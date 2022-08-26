#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

void listFiles(char* dirname);

int main(int argc, char *argv[]) {
    if(argc == 1) {
        listFiles(".");
        return 0;
    }

    if(argc == 2) {
        listFiles(argv[1]);
        return 0;
    }

    if(argc > 2) {
        printf("Too many arguments");
        return 1;
    }

    return 0;
}

void listFiles(char* dirname) {
    DIR* dir = opendir(dirname);

    if(dir == NULL) {
        return;
    }

    printf("Reading files in: %s\n", dirname);

    struct dirent* entity;

    // either gives us NULL or something
    entity = readdir(dir);
    // while it still has something to read perform this action
    while(entity != NULL) {
        printf("%s/%s\n", dirname, entity->d_name);
        if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            char path[1000] = {0};
            strcat(path, dirname);
            // adds the slash to the path name
            strcat(path, "/");
            // adds file path to that name
            strcat(path, entity->d_name);
            // then sends the newly constructed char to the function over again
            listFiles(path);
        }
        entity = readdir(dir);
    }

    closedir(dir);
}
