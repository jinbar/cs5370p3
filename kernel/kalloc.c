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
  struct run *allocated; 
} kmem;

extern char end[]; // first address after kernel loaded from ELF file

// Initialize free list of physical pages.
void
kinit(void)
{
  char *p;

  initlock(&kmem.lock, "kmem");
  p = (char*)PGROUNDUP((uint)end);
  for(; p + PGSIZE <= (char*)PHYSTOP; p += 2*PGSIZE)
    kfree(p);

  kmem.allocated = NULL; 
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r,*p;

  if((uint)v % PGSIZE || v < end || (uint)v >= PHYSTOP) 
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;

  //remove freed node from allocated list. 
  r = kmem.allocated; 
  p = NULL; 
//  cprintf("LOOKING FOR %d\n", (int)v); 
  while(r != NULL){
 //   cprintf("Found %d \n", (int)r); 
    //check if we've found our node. 
    if ((struct run*)v == r){
      //check if we're at head of list. 
   //   cprintf("FOUND"); 
      if (p != NULL){
        p->next = r->next; 
      }else {
        kmem.allocated = r->next; 
      }
    }
    p = r; 
    r = r->next; 
  }
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;

  //add to allocated list.
 // cprintf("ADDED %d\n", (int)r); 
  r->next = kmem.allocated; 
  kmem.allocated = r; 

  r = kmem.allocated; 

  // cprintf("LIST: ");
  // while (r!=NULL){
  //   cprintf("[%x] ", r);
  //   r = r->next;
  // }
  // cprintf("\n"); 
  
  release(&kmem.lock);
  return (char*)r;
}


int dump_allocated(int *frames, int numframes) {
  struct run *r;  
  
  acquire(&kmem.lock);
  
  //get the number of allocated frames
  r = kmem.allocated; 
  int num_allocated = 0; 
  while(r != NULL){
    num_allocated ++; 
    r = r->next; 
  }

  if (numframes > num_allocated){
    return -1; 
  }

  r = kmem.allocated; 
  for(int i = 0; i < numframes; i ++){
    frames[i] = (int) r;
    r = r->next;  
  }

  release(&kmem.lock);

  return 0; 
}