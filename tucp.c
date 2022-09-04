// copy contents of first file to the second file
    // if second files doesn't exist then create it

// copy entire file to a directory

// copy multiple files into a directory
    // if the directory doesn't exist then create it

#include <stdio.h>
#include <string.h>

void copyFiles(char *file, char *copy);

int main(int argc, char *argv[]) {


    // Not enough arguments
    if(argc < 3) {
        printf("Not enough arguments\n");
        return 1;
    }

    // WORKS WITH BOTH
    if (argc == 3) {
        copyFiles(argv[1], argv[2]);
        return 0;
    }

    if (argc > 2) {
        // 0 is tucp, 1 is the first file, argc-1 is the directory
        for (int i=0; i<argc-2; i++) {
            // combine first file to the directory
            //printf("Last argument: %s\n", argv[argc-1]);
            //printf("file source: %s\n", argv[i+1]);
            char plain_copy[50];
            strcpy(plain_copy, argv[argc-1]);
            //printf("copy: %s\n", plain_copy);
            //printf("combined path: %s\n", );
            strcat(plain_copy, argv[i+1]);
            copyFiles(argv[i+1], plain_copy);
        }
    }

    return 0;
}

void copyFiles(char *file, char *copy) {
    FILE *fileOP, *copyOP;
    // open the file for reading, and the copy for writing
    fileOP = fopen(file, "r");
    copyOP = fopen(copy, "w");

    // exit if the file(s) could not be opened
    if (fileOP == NULL || copyOP == NULL) {
        printf("Error opening file(s).\n");
    }

    // copy the content from the file to the copy one character at a time
    char c;
    while ( (c = fgetc(fileOP)) != EOF )
        fputc(c, copyOP);

    // close the files when we are done with them
    fclose(fileOP);
    fclose(copyOP);
}
