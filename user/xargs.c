#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
  char *av[MAXARG];

  int ii;
  for (ii = 0; argv[ii] != 0; ii++) {
    av[ii] = argv[ii];
  }
  av[ii] = 0;

  // printf("start\n");

  int i = ii;

  char buf[1], arg[MAXARG][32];

  for (int j = 0, k = 0; read(0, buf, 1) == 1; k++) {
    // printf("%c", buf[0]);
    if (buf[0] == '\n' || buf[0] == ' ') {
      // printf("enter\n");
      arg[j][k] = 0;
      k = -1;

      av[i] = arg[j++];
      av[++i] = 0;

      if (buf[0] == '\n') {
        if (fork() == 0) {
          exec(av[1], av + 1);
        } else {
          wait(0);

          i = ii;
          av[ii] = 0;
          j = 0, k = 0;
        }
      }
    } else {
      arg[j][k] = buf[0];
    }
  }

  exit(0);
}
