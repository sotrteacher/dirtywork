/********************************************************************
Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

/*********************** mm.c file ************************
  allocate/deallocat memory for a new image:
  initially, all memory from 0x20000 to 0xA0000 are free
  The mem_list consists of a single element of size 512K.
  The unit is 16-byte clicks, so the initial size = 0x8000
**********************************************************/


typedef struct m{
   struct m *next;
   u16   size;
   u16   addr;
} MEM;

MEM mnode[NPROC], *mem_free, *mem_list;  

int mem_init()
{
   int i;
   for (i=1; i<NPROC; i++)
       mnode[i].next = &mnode[i+1];
   mnode[31].next = 0;
   mem_free = &mnode[1];

   mem_list = &mnode[0];
   mnode[0].size = 0x7000;    // size in 16-byte clicks
   mnode[0].addr = 0x3000;    // free memory start segment
   mem_list->next = 0;
}

MEM *get_mnode()
{
   MEM *p;
   p = mem_free;
   mem_free = p->next;
   return(p);
}

put_mnode(MEM *p)
{
  p->next = mem_free;
  mem_free = p;
}


u16 kmalloc(u16 size)
{
  MEM *p, *q;   u16 a;
  p = q = mem_list;

  while (p && p->size < size){
         q = p;
         p = p->next;
  }
  if (p==0){
     printf("mem : out of memory\n");
     return 0;
  }
  if (p->size == size){
     a = p->addr;
     q->next = p->next;

     if (p == mem_list)
        mem_list = p->next;
  
     put_mnode(p);

     return(a);
  }

  a = p->addr + p->size - size;
  p->size -= size;
  return(a);
}

int mfree(u16 addr, u16 size)
{
   /* put the block addr,size back to free mem list */
  MEM *p, *q, *r; 
  u16 osize;

  // if released area is before the first hole
  if (addr + size == mem_list->addr){
    mem_list->addr = addr;
    mem_list->size += size;
    return;
  }

  p = q = mem_list;

  while(q && q->addr < addr){
    p = q;
    q = q->next;
  }
  if (q==0){
     if (p->addr + p->size == addr){  /* left adj to a hole */
       p->size += size;
       return;
     }
     /* no left adj hole ==> create a new hole mnode */
     r = get_mnode();
     r->addr = addr; r->size = size;
     p->next = r;    r->next = q;
     return;
  }

  /******** q != 0 cases : returned mem is in between ************/
  if (q == mem_list){
     r = get_mnode();
     r->addr = addr; r->size = size;
     r->next = q;
     mem_list = r;
      return;
  }

  if (p->addr + p->size < addr && addr + size < q->addr){ // case (1)
     r = get_mnode();
     r->addr = addr; r->size = size;
     p->next = r;    r->next = q;
  }
  else if (p->addr + p->size == addr && addr + size < q->addr){ // left hole
           p->size += size;
  }
  else if (p->addr + p->size < addr  && addr + size == q->addr){ // right hole
           q->size += size;
           q->addr = addr;
           if (q == mem_list)
	       mem_list = q;
  }
  else {                                                        // case (3)
    p->size = p->size + size + q->size;
    p->next = q->next;
    put_mnode(q);
  }
}

extern PROC *running;

int sbrk(u16 v)
{
  u16 size,segment;
  u16 start, w, i, newsize,w1;

  printf("Kernel sbrk: v=%x\n", v);

  if (v==0)
    return running->res->brk;

  size = running->res->size;

  //printf("asize=%x  oldsize=%x ", asize, size);
        
  newsize = size + v;

  if (newsize > 4096){
     newsize = 4096;
     printf("v=%x too big ==> reduced to %x for a 64KB image\n", 
              v, newsize-size);
     v = newsize - size;

  }
  printf("oldImage_size=%x  newImage_size=%x\n", size, newsize);

  segment = kmalloc(v);
  if (segment + v == running->res->segment){ // v on left side of image
    // copy image's code|data|bss|heap| to segment, leaving stack alone
    printf("expand to left by v\n");

    start = 0;
    printf("brk=%x size=%x\n", running->res->brk, running->res->size);
    while(start < running->res->brk){
      for (i=0; i<16; i+=2){
	w = get_word(running->res->segment, start*16 + i);
	put_word(w, segment, start*16+i);
      }
      start++;
    }

    printf("adjust stack frame pointers\n");
    w1 = running->usp+4*2;
    while(1){
         printf("%x  ", w1);
         w = get_word(running->uss, w1);
         if (w==0) 
            break;
         put_word(w+v*16, running->uss,  w1);
         w1 = w;
     }
     printf("\n");
  }
  else{  // cannot expand toword left
     mfree(segment, v);   // release v
     segment = kmalloc(newsize);
     if (segment==0)
        return -1;

     //printf("segment=%x newsize=%x\n",segment, newsize);

     printf("adjust stack frame pointers\n");
     w1 = running->usp+4*2;
     while(1){
         printf("%x  ", w1);
         w = get_word(running->uss, w1);
         if (w==0) 
            break;
         put_word(w+v*16, running->uss,  w1);
         w1 = w;
     }
     printf("\n");

    /******************************************************************
     should copy old CODE+DATA+BSS+HEAP to new segment
          copy old STACK (splimt to size) to high end of new image
     ******************************************************************/
     start = 0;
     while(start < running->res->brk){
        for (i=0; i<16; i += 2){
           w=get_word(running->uss, start*16+i);
           put_word(w, segment,     start*16+i);
        }
        start++;
     }

     start = 0;
     while (start < size){
        for (i=0; i<16; i+=2){
           w=get_word(running->uss+running->res->brk, start*16+i);
           put_word(w, segment+running->res->brk+v,   start*16+i);
        }
        start++;
     }
     mfree(running->uss, size);
  }
  printf("olduss=%x  oldusp=%x\n", running->uss, running->usp);
  running->res->segment = segment;
  running->uss = segment;
  running->usp = running->usp + v*16;
  /*****************************************************************
    DS ES di si bp dx cx bx ax PC CS flag|ret|v |0 | 0
    0  1  2  3  4  5  6  7  8  9  10  11 |
  ****************************************************************/
  put_word(segment, running->uss, running->usp);    // DS
  put_word(segment, running->uss, running->usp+2);  // ES
  put_word(segment, running->uss, running->usp+20); // CS

  //KCW saved Umode bp should also be shifted by v*16
  w = running->usp+4*2;
  while(w){
      printf("%x  ", w);
      w = get_word(running->uss, w);
  }
  printf("\n----------------------------------\n");

  printf("newsegment=%x  newusp=%x\n", running->res->segment, running->usp);
  running->res->segment = segment;
  running->res->size = newsize;
  return running->res->brk;
 }
 

/***** rbrk shrink by v ***********/
int rbrk(u16 v)
{

  u16 start, w, i, size, w1;
  u16 brk;
  int newsize;

  size = running->res->size;
  printf("Kernel rbrk v=%x\n", v);
      
  newsize = size - v;

  if (newsize < 20*1024/16){
    printf("newsize too small, set v to 1280 clicks\n");
    newsize = 1280;
    v = running->res->size - 1280;

  }
  printf("oldImage_size=%x  newImage_size=%x\n", size, newsize);

  printf("adjust stack frame pointers\n");

  w1 = running->usp+4*2;
  while(1){
      printf("%x  ", w1);
      w = get_word(running->uss, w1);
      if (w==0) 
          break;
      put_word(w-v*16, running->uss,  w1);
      w1 = w;
  }
  printf("\n");
  //  getc();

  start = 0;
  while (start < size-running->res->brk-v){
     for (i=0; i<16; i+=2){
         w = get_word(running->uss+running->res->brk+v, start*16+i);
         put_word(w,  running->uss+running->res->brk,   start*16+i);
    }
    start++;
  }

  mfree(running->res->segment + newsize, v);  // release extra

  //printf("olduss=%x  oldusp=%x\n", running->uss, running->usp);

  running->usp = running->usp - v*16;
  
   w = running->usp+4*2;
  while(w){
      printf("%x  ", w);
      w = get_word(running->uss, w);
  }
  printf("\n----------------------------------\n");
  running->res->size = newsize;
 }
 
