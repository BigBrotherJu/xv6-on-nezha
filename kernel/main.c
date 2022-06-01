#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    consoleinit();
    printfinit();
    printf("\n");
    printf(
      "   _  __  _    __  _____                             __  ___\n"
      "  | |/ / | |  / / / ___/     ____    ____       ____/ / <  /\n"
      "  |   /  | | / / / __ \\     / __ \\  / __ \\     / __  /  / /\n"
      " /   |   | |/ / / /_/ /    / /_/ / / / / /    / /_/ /  / /\n"
      "/_/|_|   |___/  \\____/     \\____/ /_/ /_/     \\__,_/  /_/\n"
    );
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    // kinit kvminit kvminithart moved to top
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    ramdiskinit();   // ram disk
    userinit();      // first user process
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();        
}
