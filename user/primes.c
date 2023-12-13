#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void runprocess(int rfd) {
  int p[2];
  pipe(p);
  int n, b;
  int first = -1;
  int send_num = 0;
  while ((b = read(rfd, &n, sizeof(int))) > 0) {
    if (first == -1) {
      first = n;
      printf("prime %d\n", first);
    } else if (n % first != 0) {
      write(p[1], &n, sizeof(int));
      send_num++;
    }
  }
  close(rfd);
  if (send_num == 0) {
    close(p[0]);
    close(p[1]);
    //exit(0);
    return;
  }

  if (fork() == 0) {
    // child
    close(p[1]);
    runprocess(p[0]);
    // exit(0);
  } else {
    // parent
    close(p[0]);
    close(p[1]);
    wait(0);
    exit(0);
  }
}

int main() {
  int p[2];
  pipe(p);
  for (int i = 2; i <= 35; i++) {
    write(p[1], &i, sizeof(int));
  }
  close(p[1]);
  runprocess(p[0]);
  return 0;
}