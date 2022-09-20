#include <string.h>
#include <stdio.h>

int main() {
    char str[1024] = "1:pid 2:(exec-file-name) 3:state 4:ppid 5:pgrp 6:session 7 8 9 10 11 12 13 14:utime 15:stime 16:cstime";
    const char s[2] = " ";
    char *token;
    int tokenCount = 0;

    // get the first token
    token = strtok(str, s);

    // walk through the other tokens
    while(token != NULL) {
        token = strtok(NULL, s);
        tokenCount++;
        if(tokenCount == 14) {
            printf("%s\n", token);
            break;
        }
    }

    return 0;
}
