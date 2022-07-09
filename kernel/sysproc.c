#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
/* syscall */
#include "sysinfo.h"
/* syscall */

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;

  if (myproc()->sz + n >= BOTTOM_DEV_VA) {
    return -1;
  }

  addr = myproc()->sz;

  if(n < 0) {
    uvmdealloc(myproc()->pagetable, myproc()->sz, myproc()->sz + n);
    // printf("test\n");
    kvmdealloc(myproc()->kernelpgtbl, myproc()->sz, myproc()->sz + n);
    // printf("test\n");
  }
  myproc()->sz += n;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  /* trap */
  // backtrace();
  /* trap */
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

/* trap */
uint64 
sys_sigalarm(void)
{
  int n;
  uint64 p;

  if(argint(0, &n) < 0)
    return -1;
  if(argaddr(1, &p) < 0)
    return -1;
  if (n == 0 && p == 0) {
    myproc()->alarm_status = 0;
  } else {
    myproc()->interval_ticks = n;
    myproc()->handler = p;
    myproc()->alarm_status = 1;

    if((myproc()->alarm_reg = (struct trapframe *)kalloc()) == 0)
      return -1;
  }
  return 0;
}

uint64 
sys_sigreturn(void)
{
  myproc()->alarm_status = 1;
  memmove(myproc()->trapframe, myproc()->alarm_reg, sizeof(struct trapframe));
  return 0;
}
/* trap */

/* syscall */
uint64
sys_trace(void)
{
  int n;
  argint(0, &n);
  myproc()->trace_num = n;
  return 0;
}

uint64
sys_sysinfo(void)
{
  uint64 si;
  argaddr(0, &si);

  struct proc *p = myproc();
  struct sysinfo sysi;

  sysi.freemem = free_mem();
  sysi.nproc = num_proc();

  if(copyout(p->pagetable, si, (char *)&sysi, sizeof(sysi)) < 0)
    return -1;
  return 0;

}
/* syscall */


/* pgtbl */
uint64
sys_printpgtbl(void)
{
  struct proc *p = myproc();
  vmprint(p->pagetable);
  return 0;
}
/* pgtbl */
