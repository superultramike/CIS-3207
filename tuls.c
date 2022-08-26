#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

// Function Prototype
void listFiles(char* dirname);

int main(int argc, char *argv[]) {
    // If the user gives no path, then print the current directory tuls in placed in
    if(argc == 1) {
        listFiles(".");
        return 0;
    }

    // If the user gives a path, then print all the contents of user given path
    if(argc == 2) {
        listFiles(argv[1]);
        return 0;
    }

    // If more then 1 path is given then print out the error message that there are too many arguments
    if(argc > 2) {
        printf("Too many arguments");
        return 1;
    }

    return 0;
}

// Function that recursively prints the files and directory contents of a current/given directory
void listFiles(char* dirname) {
    DIR* dir = opendir(dirname);

    if(dir == NULL) {
        return;
    }

    // Visual cue for when it's reading into a directory
    printf("Reading files in: %s\n", dirname);

    // Declare
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
        // assign the entity to the contents when it is read by readdir function
        entity = readdir(dir);
    }
    // close the directory once you are done reading
    closedir(dir);
}
