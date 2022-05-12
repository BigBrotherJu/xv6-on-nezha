#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("backtrace for sleep syscall:\n");
  sleep(1);
  exit(0);
}
