#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    // scan user input
    int pid;
    sscanf(argv[1], "%d", &pid);

    // Declare variables to print out stat menu
    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "rb");
    char *arg;
    size_t size = 0;
    while(getdelim(&arg, &size, 0, f) != -1) {
        // print out the entire contents of stat
        printf("%s\n", arg);
        const char s[2] = " ";
        char *token;
        int tokenCount = 0;
        token = strtok(arg, s);

        // walk through the other tokens
        while(token != NULL) {
            printf("%s\n", token );
            token = strtok(NULL, s);
            tokenCount++;
            if(tokenCount == 13) {
                printf("UTIME: %s\n", token);
                break;
            }
        }
    }
    fclose(f);

    return 0;
}
