#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(int argc, char *argv[])
{
  printf("\nkernel page table test starts\n\n");

  int fd = open("kernelpgtbltest", O_CREATE|O_WRONLY);
  if (fd < 0){
      printf("open failed\n");
      exit(1);
  } else {
    printf("open succeeded\n");
    printf("open system call uses copyinstr to copy file name\n");
  }
  int n = write(fd, "kernel page table test", 22);
  if(n != 22){
    printf("write got %d, expected 22\n", n);
    exit(1);
  } else {
    printf("\nwrite succeeded\n");
    printf("write system call uses copyin to copy user buffer content\n");
  }

  close(fd);
  unlink("kernelpgtbltest");

  exit(0);
}
