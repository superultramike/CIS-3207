#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    FILE *meminfo = fopen("/proc/meminfo", "rb");
    char *arg = 0;
    size_t size = 0;
    while(getdelim(&arg, &size, 0, meminfo) != -1) {
        printf(" %s\n", arg);
    }
    free(arg);
    fclose(meminfo);
    return 0;
}
