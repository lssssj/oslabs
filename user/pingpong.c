#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int
main(int argc, char *argv[])
{
  int p[2];
  int q[2];
  pipe(p);
  pipe(q);
  char buff[5];
  int pid;
  if (fork() == 0) {
    pid = getpid();
    close(p[0]);
    close(q[1]);
    write(p[1], "ping", 4);
    printf("%d: received ping\n", pid);
    read(q[0], buff, 4);
    exit(0);
  } else {
    pid = getpid();
    close(p[1]);
    close(q[0]);
    read(p[0], buff, 4);
    write(q[1], buff, 4);
    //printf("%s\n", buff);
    printf("%d: received pong\n", pid);
    exit(0);
    
  }
  
}
