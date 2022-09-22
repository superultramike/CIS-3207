#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char **argv) {
    FILE *uidInfo = fopen("/proc/self/loginuid", "rb");
    char *selfuid = 0;
    size_t size = 0;

    while(getdelim(&selfuid, &size, 0, uidInfo) != -1) {
        fscanf(uidInfo, "%s", selfuid);
    }
    //free(selfuid);
    fclose(uidInfo);

    printf("UID IS: %s", selfuid);
    return 0;
}
