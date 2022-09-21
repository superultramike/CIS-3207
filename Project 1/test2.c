#include <stdio.h>
#include <stdlib.h> // For exit()
#include <dirent.h>
#include <errno.h>

void iterate_processes (void);

int main() {
    iterate_processes();

    return 0;
}

void iterate_processes (void) {
  struct dirent   *dirent;
  DIR*             dir;
  int             r;
  int             pid;

  // 1) find all processes
  if (!(dir = opendir ("/proc"))) {
    perror (NULL);
    exit (EXIT_FAILURE);
  }

  while (dirent = readdir (dir)) {
    // 2) we are only interested in process IDs
    if (atoi(dirent -> d_name) != 0) {
      pid = atoi (dirent -> d_name);
      printf("%d\n", pid);
    }
  }
  closedir (dir);
}
