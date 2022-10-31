#include <stdio.h>

extern char **environ;

int main() {
    print_envp();
    return 0;
}

void print_envp() {
    char **s = environ;

    for(; *s; s++) {
        printf("%s\n", *s);
    }
    return 0;
}
