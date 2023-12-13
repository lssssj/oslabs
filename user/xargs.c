#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void xargs(int argc, char* argv[]) {
  
  char *args[MAXARG] = {0};
  char *cmd = argv[1];
  int args_index = 0;
  for (int i = 1; i < argc; i++) {
    args[args_index++] = argv[i];
  }

  char buff[1024];

  int n;//uuu

  while ((n = read(0, buff, sizeof(buff))) > 0) {
    if (fork() == 0) {
      char* param = (char *) malloc(n+1);
      int idx = 0;
      for (int i = 0; i < n; i++) {
        if (buff[i] == ' ' || buff[i] == '\n') {
          param[idx] = 0;
          args[args_index++] = param;
          param = (char *)malloc(n+1);
          idx = 0;
        } else {
          param[idx++] = buff[i];
        }
      }
      param[idx] = 0;
      args[args_index++] = param;
      exec(cmd, args);
    } else {
      wait(0);
    }
  }

  exit(0);
}


int
main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage xargs comand ...\n");
    exit(0);
  }
  xargs(argc, argv);

}