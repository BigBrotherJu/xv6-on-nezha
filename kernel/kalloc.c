// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

/* cow */
struct spinlock ref_cnt_lock;
int ref_cnt[(PHYSTOP - KERNBASE) / PGSIZE];

void
inc_ref_cnt(uint64 pa)
{
  acquire(&ref_cnt_lock);
  ref_cnt[(pa - KERNBASE) / PGSIZE]++;
  release(&ref_cnt_lock);
}

// if ref cnt for old_pa is less than or equal to 1,
// no need to do allocation and copy, directly
// return old_pa;
// else, allocate a new physical page:
//   if a new page can be allocated,
//   copy content of old page to new page
//   return new page address;
//   if a new page cannot be allocated,
//   return 0;
uint64
kcow_alloc_copy(uint64 old_pa)
{
  acquire(&ref_cnt_lock);

  if (ref_cnt[(old_pa - KERNBASE) / PGSIZE] <= 1) {
    release(&ref_cnt_lock);
    return old_pa;
  } else {
    uint64 new_pa = (uint64)kalloc();

    if (new_pa != 0) {
      memmove((void*)new_pa, (void*)old_pa, PGSIZE);
      ref_cnt[(old_pa - KERNBASE) / PGSIZE]--;
    }

    release(&ref_cnt_lock);
    return new_pa;
  }

}
/* cow */

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  /* cow */
  initlock(&ref_cnt_lock, "ref_cnt");
  /* cow */
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  /* cow */
  // < 0 to account for kfree in freerange
  acquire(&ref_cnt_lock);
  ref_cnt[((uint64)pa - KERNBASE) / PGSIZE]--;
  if (ref_cnt[((uint64)pa - KERNBASE) / PGSIZE] <= 0) {
  /* cow */

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);

  /* cow */
  }
  release(&ref_cnt_lock);
  /* cow */
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk

  /* cow */
  if(r)
    ref_cnt[((uint64)r - KERNBASE) / PGSIZE] = 1;
  /* cow */

  return (void*)r;
}

/* syscall */
int
free_mem(void)
{
  struct run *r = kmem.freelist;

  int result = 0;

  while (r != 0) {
    result += PGSIZE;
    r = r -> next;
  }

  return result;
}
/* syscall */
