#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, char *file)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    fprintf(2, "find: must search in a directory\n");
    // printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      fprintf(2, "find: dircetory path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", buf);
        continue;
      }

      if(st.type == T_FILE) {
        if(strcmp(de.name, file) == 0) {
          printf("%s\n", buf);
        }
      } else if (st.type == T_DIR) {
        if(strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
          find(buf, file);
        }
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc < 3 || argc > 3) {
    fprintf(2, "Usage: find dir filename\n");
    exit(1);
  } else {
    find(argv[1], argv[2]);
    exit(0);
  }
}
