#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

// Function Prototype
void copyFiles(char *file, char *copy);

// Main Function
int main(int argc, char *argv[]) {
    // Declare stat to eventually get file status
    struct stat buf;

    // If there are less than 3 arguments...
    if(argc < 3) {
        // Print an error message
        printf("Not enough arguments\n");
        return 1;
    }

    // If there are 3 arguments exactly...
    if (argc == 3) {
        // Call stat function to determine the file status of argument[2]
        // argument[2] would be the last argument in user input
        stat(argv[2], &buf);

        // If the last argument is a directory...
        if(S_ISDIR(buf.st_mode)) {
            // Declare the variable we will copy argv[1] too
            char copy[50];
            // copy directory path to modifiable variable
            strcpy(copy, argv[2]);
            // combine modifiable directory path with file source
            strcat(copy, argv[1]);
            // Copy files using file source and newly made directory path
            copyFiles(argv[1], copy);
        }
        // If the last argument is a regular file..
        else {
            // Copy files using file source to file destination
            copyFiles(argv[1], argv[2]);
        }
        return 0;
    }

    // If there are 4 or more arguments...
    if (argc >= 4) {
        // loop through each of the arguments that are files that need to be copied
        // 0 is tucp, 1 is the first file, argc-1 is the directory
        for (int i=0; i<argc-2; i++) {
            // Declare the variable we will copy argv[1] too
            char plain_copy[1024];
            // combine first file to the directory
            strcpy(plain_copy, argv[argc-1]);
            // combine modifiable directory path with file source
            strcat(plain_copy, argv[i+1]);
            // Copy files using ith file source and newly made directory path
            copyFiles(argv[i+1], plain_copy);
        }
    }
    return 0;
}

// Function to copy files with arguments source and destination
void copyFiles(char *file, char *copy) {
    // declare file variables to open/close upon
    FILE *fileOP, *copyOP;
    // declare integer for reading/writing error checking
    int numr,numw;
    // declare a buffer of how many elements you are going to read
    char buffer[100];

    // open the file for reading, and the copy for writing
    fileOP = fopen(file, "rb");
    copyOP = fopen(copy, "wb");

    // exit if the file(s) could not be opened
    if (fileOP == NULL || copyOP == NULL) {
        printf("Error opening file(s).\n");
    }

    // copy the content from the file to the copy one character at a time
    // If the feof function for fileOP is equal to the 0 (end of stream isn't set)
    while (feof(fileOP) == 0) {
        // if # of files read isn't equal to the buffer (100) then you haven't read all the files
        if ((numr=fread(buffer,1,100,fileOP)) != 100) {
            // If the error indicator for the stream wasn't set, then return an error
            if (ferror(fileOP) != 0) {
                fprintf(stderr,"read file error.\n");
            }
            // If not then continue the function
            else if (feof(fileOP)!=0);
        }
        // if # of files written isn't equal to the # of files read, then you didn't write all the files
        if ((numw=fwrite(buffer,1,numr,copyOP)) != numr) {
            // Print an error
            fprintf(stderr,"write file error.\n");
        }
	}

    // close the files when we are done with them
    fclose(fileOP);
    fclose(copyOP);
}

