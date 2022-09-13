#include <stdio.h>
#include <stdlib.h> // For exit()

int main(int argc, char **argv) {
    FILE *fptr;
    char c;

    fptr = fopen("/proc/1721/stat", "r");
    if(fptr == NULL) {
        printf("Cannot open file\n");
        exit(0);
    }

    c = fgetc(fptr);
    while(c != EOF) {
        printf("%c", c);
        c = fgetc(fptr);
    }

    fclose(fptr);
    return 0;
}
