#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fname(char* path) {
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  memset(buf, 0, sizeof(buf));
  memmove(buf, p, strlen(p));
  buf[strlen(p) + 1] = 0;
  return buf;
}

void find(char* path, const char* file_pattern) {
  char buf[512], *p;
  struct dirent de;
  struct stat st;
  int fd;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    exit(0);
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    exit(0);
  }

  char* fn = fname(path);
  switch (st.type) {
    case T_FILE:
      if (strcmp(fn, file_pattern) == 0) {
        printf("%s\n", path);
      }
      close(fd);
      break;
    case T_DIR:
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("ls: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf+strlen(buf);
      *p++ = '/';
      while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
          continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        find(buf, file_pattern);
      }
      break;
  }
}

int 
main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: find [dir] [file-pattern]\n");
    exit(0);
  } 
  char* path = argv[1];
  const char* file_pattern = argv[2];
  
  find(path, file_pattern);
  exit(0);
}