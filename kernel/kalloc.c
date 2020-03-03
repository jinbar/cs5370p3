// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"
#include "rand.h"
#include "rand.c"

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  struct run * allocatedlist[10000]; 
  int alloc_size;
  int free_size;
} kmem;

int seed = 1;

static void add_allocated(struct run * r) {
  kmem.allocatedlist[kmem.alloc_size] = r; 
  kmem.alloc_size ++; 
}

static void remove_allocated(struct run *r) {
  int r_loc = 10001; 
  for (int i = 0; i < kmem.alloc_size; i ++) {
    if (kmem.allocatedlist[i] == r) {
      r_loc = i; 
      break; 
    }
  }
  kmem.alloc_size--;
  if (r_loc != 10001) {
    for (int i = r_loc; i < kmem.alloc_size; i ++) {
      kmem.allocatedlist[i] = kmem.allocatedlist[i+1]; 
    }
  }
}

extern char end[]; // first address after kernel loaded from ELF file

// Initialize free list of physical pages.
void kinit(void) {
  char *p;
  initlock(&kmem.lock, "kmem");
  p = (char*)PGROUNDUP((uint)end);
  for (; p + PGSIZE <= (char*)PHYSTOP; p += PGSIZE) {
    kfree(p);
  }
  kmem.alloc_size = 0;
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(char *v) {
  struct run *r;
  if ((uint)v % PGSIZE || v < end || (uint)v >= PHYSTOP) {
    panic("kfree");
  }

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);
  acquire(&kmem.lock);
  kmem.free_size++;
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  remove_allocated(r);
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char* kalloc(void) {
  struct run *r;
  struct run *temp;
  acquire(&kmem.lock);
  r = kmem.freelist;
  xv6_srand(seed);
  if(r) {
    int remainder = xv6_rand() % kmem.free_size;
    if (remainder == 0) {
      kmem.freelist = r->next;
      temp = r;
    } else {
      for (int i = 0; i < remainder - 1; i++) {
        r = r->next;
      }
      temp = r->next;
      r->next = r->next->next;
    }
    kmem.free_size--;
    add_allocated(temp); 
  }
  release(&kmem.lock);
  return (char*) temp;
}

int dump_allocated(int *frames, int numframes) {
  cprintf("Print %d frames, size is %d", numframes, kmem.alloc_size); 
  if (numframes > kmem.alloc_size) {
    return -1; 
  }
  int j = 0;
  for (int i = numframes - 1; i >= 0; i --) {
    frames[j++] = (int)kmem.allocatedlist[i]; 
    cprintf("frame: %x %d \n", frames[i], i); 
  }
  return 0; 
}