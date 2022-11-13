#include <stdio.h>

main() {
   FILE *fp;

   fp = fopen("foo.txt", "w+");
   fprintf(fp, "This is testing for fprintf...\n");

   fclose(fp);
}
