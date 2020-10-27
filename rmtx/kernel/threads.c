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
/*************************************************************************
      threads.c : implement thread(), texit(), and  mutex
*************************************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

#define TMAX 16

int thinit()
{
   int i;
   PROC *p;

   for (i=NPROC; i<NPROC+NTHREAD; i++){
     p = &proc[i];
     p->pid = i;
     p->status = FREE;
     p->pri = 0;
     p->next = p+1;
     p->proc = p;   // each PORC points to itself
     p->inkmode = 1;
     p->type = THREAD;
     p->res = 0;
   }
   tfreeList = &proc[NPROC];
   proc[NPROC+NTHREAD-1].next = 0;
}


int tjoin(int n)
{
  int i, status;
  for (i=0; i<n; i++)
     kwait(&status);
}

// no need for texit(); from U mode same as exit()
int texit(int v)
{
  kexit(v);
}

#define USSIZE 512

int kthread(int fn, stack, flag, ptr)
{  
  // create a thread executing fn with stack; return thread's pid
  // flag = 1 : use ustack by thread pid ustack=tid9 tid10 .... tidTMAX
  // flag = 0 : use passed in ustack
  PROC *p, *q;
  int i, j, *ip, *iq, tcount, index;
  u16 offset, segment;

  printf("kernel thread(): fn=%x  stack=%x\n", fn, stack);

  // check process tcount
  p = running->proc; 
  tcount = p->res->tcount;
  printf("process %d tcount = %d\n", running->pid, tcount);
        
  if (tcount > TMAX){
      printf("max proc tcount %d reached\n", tcount);
      return -1;
  }

  // 1. need a new thread sturct
  /*** get a proc for child thread: ***/
  if ( (p = get_proc(&tfreeList)) == NULL){
       printf("\nno more THREAD PROC  ");
       return(-1);
  }
  /* initialize the new thread PROC */
  p->status = READY;
  p->ppid = running->pid;
  p->parent = running;
  p->pri  = 128;
  p->event = 0;
  p->exitValue = 0;
  p->inkmode = 1;

  p->type = THREAD;
  p->proc = running->proc;   // point to process PROC
  p->res = running->res;     // point to SAME resource struct of PROCess

  /* clear "kernel mode saved registers" to 0 */ 
  for (i=1; i<=9; i++)
       p->kstack[SSIZE-i] = 0;

  /* fill in the needed entries */
  p->kstack[SSIZE-1] = (int)goUmode;
  p->ksp = &(p->kstack[SSIZE - 9]); 

  // 2. same segment as caller so no new segment
  //    but has its own stack at passed in stack + 4KB   
  // must create its ustack frame for it to return to Umode
  
   /*** first, child's uss and usp must be set: *****/

   segment = running->uss;    // sgement of caller
   p->uss = segment;
   
   /* if flag=1: based on pid of thread, use ustack accordingly*/
   if (flag==1){
     /* stack is very high end of a total ustack area; each thread
        uses its portion of the ustack area */
     index = p->pid-NPROC;            // 9-9=0 10-9=1, etc
     //     printf("index=%d ", index);
     stack -= (index*USSIZE);         // assume 1024 bytes ustack per thread 
     //kgetc();
   }  
   offset = stack - 2;                // VA of ustack
   put_word(ptr, segment, stack-2);   // parameter ptr
   put_word(3,   segment, stack-4);

   offset = stack - 6;
   //                 0    1   2  3  4  5  6  7  8  9  10  11 
   // frame contains flag  CS  PC ax bx cx dx bp si di es  ds
   //                0x20 seg  fn 0  0  0  0  0  0  0 seg seg

   put_word(0x0200,  segment, offset);
   put_word(segment, segment, offset-2*1);
   put_word(fn,      segment, offset-2*2);

   for (j=3; j<10 ; j++){
        put_word(0,  segment, offset-2*j);
   }

   put_word(segment, segment, offset-2*10);
   put_word(segment, segment, offset-2*11);
   
   p->usp = offset - 22;
   p->status = READY;   
   enqueue(&readyQueue, p);
   printList("readyQueue ", readyQueue);
 
   // update total tcount in process PROC
   running->proc->res->tcount++;
   printf("proc %d created a thread %d in segment=%x tcount=%d\n",
           running->pid, p->pid, segment, p->proc->res->tcount);
   nproc++;
   return(p->pid);
}

// implementation of  mutex

typedef struct{
  int status;     // created OR destroyed
  int owner;     // owner pid, only owner can unlcok
  int value;     // lock value: 0 for locked 1 for FREE
  PROC *queue;   // waiting proc queue; ==> unlock will wakeup one as owner 
}MUTEX;

MUTEX  mutex[10];
int mutexuse[10] = {0};  // all mutex[] are FREE

int mutex_creat() // no need for value because initial always 1==FREE
{
  int i;
  for (i=0; i<10; i++){
    if (mutexuse[i]==0){
      mutexuse[i] = 1;
      break;
    }
  }
  // allocated mutex[i];
  mutex[i].status = 1;
  mutex[i].value = 1;
  mutex[i].queue = 0;
  mutex[i].owner = -1;   // no owner initially
  printf("\nkernel: mutex_creat(): mutex==%x  mutex->value=%d\n",
          &mutex[i], mutex[i].value);
  return &mutex[i];
}   

int mutex_destroy(MUTEX *m)
{
  int i;
  printf("mutex_destroy : ");

  // owner ==> deallocate mutex m
  if (m->owner != running->pid){
    printf("not owner\n");
    return -1;
  }

  if (m->value < 0){
    printf("mutex still has waiters\n");
    return -1;
  }
  for (i=0; i<10; i++){
    if (m==&mutex[i]){
      mutexuse[i] = 0;
      mutex[i].status = 0;
      mutex[i].owner = -1;
    }
  }
  printf("destroyed\n");
}

int mutex_lock(MUTEX *m)
{
  int i;
  int found = 0;
  PROC *p;
  
  printf("\nmutex_lock : ");

  for (i=0; i<10; i++){
    if (m==&mutex[i]){
      found = 1;
      break;
    }
  }

  if (!found){
      printf("invalid mutex %x\n", m);
      return -1;
  }

  if (m->status == 0){
    printf("invalid mutex\n");
    return -1;
  }

  if (m->value <= 0 && m->owner==running->pid){
    printf("mutex is already locked by you!\n");
    return -1;
  }

  m->value--;

  if (m->value < 0){
    // block caller in mutex queue   

    p = m->queue;
    if (m->queue==0)
      m->queue = running;
    else{
      while(p->next)
	p = p->next;
      p->next = running;
    }
    p->next = 0;
    running->status = BLOCK;
    printf("%d BLOCKed in mutex %x queue\n", running->pid, m);
    tswitch();
  }

  m->owner = running->pid;
  printf("Proc%d locked mutex=%x\n", running->pid, m);
  return 0;
}

int mutex_unlock(MUTEX *m)
{
  int i;
  int found = 0;
  PROC *p;

  printf("\nmutex_unlock");

  for (i=0; i<10; i++){
    if (m==&mutex[i]){
      found = 1;
      break;
    }
  }

  if (!found){
      printf("invalid mutex %x\n", m);
      return -1;
  }

  if (m->status == 0){
    printf("invalid mutex\n");
    return -1;
  }


  if (m->owner != running->pid){
    printf("%d is NOT owner of this mutex\n", running->pid);
    return -1;
  }

  if (m->value > 0){
    printf("mutex %x is NOT locked\n", m);
    return -1;
  }
  
  m->value++;
  if (m->value <= 0){
     p = m->queue;
     m->queue = p->next;
     m->owner = p->pid;
     p->status = READY;

     enqueue(&readyQueue, p);
     printf("UNBLOCK %d=new owner\n", p->pid);
  }
}

