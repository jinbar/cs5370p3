// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"

struct run {
  struct run *next;
};


struct {
  struct spinlock lock;
  struct run *freelist;
  struct run * allocatedlist[10000]; 
  int alloc_size; 
} kmem;

static void add_allocated(struct run * r){
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
  kmem.alloc_size --;
  if (r_loc == 10001) {
    return; 
  }
  for (int i = r_loc; i < kmem.alloc_size; i ++) {
    kmem.allocatedlist[i] = kmem.allocatedlist[i+1]; 
  } 
}

extern char end[]; // first address after kernel loaded from ELF file

// Initialize free list of physical pages.
void kinit(void) {
  char *p;
  initlock(&kmem.lock, "kmem");
  p = (char*)PGROUNDUP((uint)end);
  for (; p + 2*PGSIZE <= (char*)PHYSTOP; p += 2*PGSIZE) {
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
  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    kmem.freelist = r->next;
    add_allocated(r);  
  }
  release(&kmem.lock);
  return (char*)r;
}

int dump_allocated(int *frames, int numframes) {
  if (numframes > kmem.alloc_size) {
    return -1; 
  }
  for (int i = numframes - 1; i >= 0; i --) {
    frames[i] = (int)kmem.allocatedlist[i]; 
  }
  return 0; 
}