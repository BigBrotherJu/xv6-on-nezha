#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

void ugetpid_test();
// void pgaccess_test();
void printpgtbl_test();

int
main(int argc, char *argv[])
{
  printf("\n");
  ugetpid_test();
  // pgaccess_test();
  printf("\n");
  printpgtbl_test();
  printf("\npgtbltest: all tests succeeded\n");
  exit(0);
}

char *testname = "???";

void
err(char *why)
{
  printf("pgtbltest: %s failed: %s, pid=%d\n", testname, why, getpid());
  exit(1);
}

void
ugetpid_test()
{
  int i;

  printf("ugetpid_test starting\n");
  testname = "ugetpid_test";

  for (i = 0; i < 32; i++) {
    int ret = fork();
    if (ret != 0) {
      wait(&ret);
      if (ret != 0)
        exit(1);
      continue;
    }
    int pid = getpid();
    int upid = ugetpid();
    if (pid != upid)
      err("missmatched PID");
    else
      printf("ugetpid returns %d, matching return values from"
             " getpid\n", upid);
    exit(0);
  }
  printf("ugetpid_test: OK\n");
}

void printpgtbl_test()
{
    printf("page table for the current process:\n");
    printpgtbl();
}


/*
void
pgaccess_test()
{
  char *buf;
  unsigned int abits;
  printf("pgaccess_test starting\n");
  testname = "pgaccess_test";
  buf = malloc(32 * PGSIZE);
  if (pgaccess(buf, 32, &abits) < 0)
    err("pgaccess failed");
  buf[PGSIZE * 1] += 1;
  buf[PGSIZE * 2] += 1;
  buf[PGSIZE * 30] += 1;
  if (pgaccess(buf, 32, &abits) < 0)
    err("pgaccess failed");
  if (abits != ((1 << 1) | (1 << 2) | (1 << 30)))
    err("incorrect access bits set");
  free(buf);
  printf("pgaccess_test: OK\n");
}
*/


