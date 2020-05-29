/*********** t.c file of A Multitasking System *********/
#include <stdio.h>
#include "type.h"

PROC proc[NPROC]; // NPROC PROCs
PROC *freeList;   // freeList of PROCs
PROC *readyQueue; // priority queue of READY procs
PROC *running;    // current running proc pointer

#include "queue.c" // include queue.c file
int body();
int kexit();
void tswitch();
/*******************************************************
kfork() creates a child process; returns child pid.
When scheduled to run, child PROC resumes to body();
********************************************************/
int kfork()
{
  int i;
  PROC *p = dequeue(&freeList);
  if (!p){
    printf("no more proc\n");
    return(-1);
  }

  /* initialize the new proc and its stack */
  p->status = READY;
  p->priority = 1; // ALL PROCs priority=1,except P0
  p->ppid = running->pid;

  /************ new task initial stack contents ************
   kstack contains: |retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
                       -1   -2  -3  -4  -5  -6  -7  -8  -9
   **********************************************************/
  for (i=1; i<10; i++)              // zero out kstack cells
    p->kstack[SSIZE - i] = 0;
  p->kstack[SSIZE-1] = (int)body;   // retPC -> body()
  p->ksp = &(p->kstack[SSIZE - 9]); // PROC.ksp -> saved eflag
  enqueue(&readyQueue, p);          // enter p into readyQueue
  return p->pid;
}/*end kfork()*/

int kexit()
{
  running->status = FREE;
  running->priority = 0;
  enqueue(&freeList, running);
  printList("freeList", freeList);
  tswitch();
}/*end kexit()*/

int do_kfork()
{
  int child = kfork();
  if (child < 0)
    printf("kfork failed\n");
  else{
    printf("proc %d kforked a child = %d\n", running->pid, child);
    printList("readyQueue", readyQueue);
  }
  return child;
}/*end do_kfork()*/

int do_switch()
{
  tswitch();
}/*end do_switch()*/

int do_exit()
{
  kexit();
}/*end do_exit()*/

int body() // process body function
{
  int c;
  printf("proc %d starts from body()\n", running->pid);
  while(1){
    printf("***************************************\n");
    printf("proc %d running: Parent=%d\n", running->pid,running->ppid);
    printf("child = %s\n","NULL");
    printf("input a char [f|s|q] : ");
    c = getchar(); getchar(); // kill the \r key
    switch(c){
      case 'f': do_kfork(); break;
      case 's': do_switch(); break;
      case 'q': do_exit(); break;
    }
  }
}/*end body()*/

// initialize the MT system; create P0 as initial running process
int init()
{
  int i;
  PROC *p;
  for (i=0; i<NPROC; i++){ // initialize PROCs
    p = &proc[i];
    p->pid = i;            // PID = 0 to NPROC-1
    p->status = FREE;
    p->priority = 0;
    p->next = p+1;
  }
  proc[NPROC-1].next = 0;
  freeList = &proc[0];     // all PROCs in freeList
  readyQueue = 0;          // readyQueue = empty

  // create P0 as the initial running process
  p = running = dequeue(&freeList); // use proc[0]
  p->status = READY;
  p->ppid = 0; // P0 is its own parent
  printList("freeList", freeList);
  printf("init complete: P0 running\n");
}/*end init()*/

/*************** main() function ***************/
int main()
{
  printf("Welcome to the MT Multitasking System\n");
  init(); // initialize system; create and run P0
  kfork(); // kfork P1 into readyQueue
  while(1){
    printf("P0: switch task\n");
    if (readyQueue)
      tswitch();
  }
}/*end main()*/

/*********** scheduler *************/
int scheduler()
{
  printf("proc %d in scheduler()\n", running->pid);
  if (running->status == READY)
    enqueue(&readyQueue, running);
  printList("readyQueue", readyQueue);
  running = dequeue(&readyQueue);
  printf("next running = %d\n", running->pid);
}/*end scheduler()*/



