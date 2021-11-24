/*********** t.c User-level Threads ********************/
/**
 * REF: Systems Programming in Unix/Linux, K.C. Wang, editorial 
 * Springer Nature, 2018. Chapter 4 Concurrent Programming, page 166.
 * ISBN: 978-3-319-92428-1 ISBN: 978-3-319-92429-8 (eBook).
 * Programming Project: User-level Threads
 *
 * To compile and run the base code under Linux, enter
 *  gcc -m32 t.c ts.s
 * Then run a.out. While running the program, the reader may
 * enter the commands
 * 'c': create a new task
 * 's': switch to run the next task from readyQueue
 * 'q': let the running task terminate
 * to test and observe task execution in the system.
 */
#include <stdio.h>
#include "type.h"

PROC proc[NPROC];   // NPROC proc structures
PROC *freeList;     // free PROC list
PROC *readyQueue;   // ready proc priority queue
PROC *sleepList;    // sleep PROC list
PROC *running;      // running proc pointer

#include "queue.c"  // enqueue(), dequeue(), printList()
void tswitch();
int create(void (*f)(), void *parm);
void func(void *parm);

int init()
{
  int i, j;
  PROC *p;
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->priority = 0;
    p->status = FREE;
    p->event = 0;
    p->joinPid = 0;
    p->joinPtr = 0;
    p->next = p+1;
  }
  proc[NPROC-1].next = 0;
  freeList = &proc[0]; // all PROCs in freeList
  readyQueue = 0;
  sleepList = 0;
  // create P0 as initial running task
  running = p = dequeue(&freeList);
  p->status = READY;
  p->priority = 0;
  printList("freeList", freeList);
  printf("init complete: P0 running\n");
}/*end init()*/

int texit(int value)
{
  printf("task %d in texit value=%d\n", running->pid, running->pid);
  running->status = FREE;
  running->priority = 0;
  enqueue(&freeList, running);
  printList("freeList", freeList);
  tswitch();
}/*end texit()*/

int do_create()
{
  //int pid = create(func, running->pid); // parm = pid
  int pid = create(func, (void*)(running->pid)); // parm = pid
}/*end do_create()*/

int do_switch()
{
  tswitch();
}/*end do_switch()*/

int do_exit()
{
  texit(running->pid); // for simplicity: exit with pid value
}/*end do_exit()*/

void func(void *parm)
{
  int c;
  printf("task %d start: parm = %d\n", running->pid, (int)parm);
  while(1){
    printf("task %d running\n", running->pid);
    printList("readyQueue", readyQueue);
    printf("enter a key [c|s|q] : ");
    c = getchar(); getchar();
    switch (c){
      case 'c' : do_create(); break;
      case 's' : do_switch(); break;
      case 'q' : do_exit(); break;
    }
  }
}/*end func()*/

int create(void (*f)(), void *parm)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (!p){
    printf("create failed\n");
    return -1;
  }
  p->status = READY;
  p->priority = 1;
  p->joinPid = 0;
  p->joinPtr = 0;
  // initialize new task stack for it to resume to f(parm)
  for (i=1; i<13; i++)                  // zero out stack cells
    p->stack[SSIZE-i] = 0;
  p->stack[SSIZE-1] = (int)parm;        // function parameter
  p->stack[SSIZE-2] = (int)do_exit;     // function return address
  p->stack[SSIZE-3] = (int)f;           // function entry
  p->ksp = (int)&p->stack[SSIZE-12];    // ksp -> stack top
  enqueue(&readyQueue, p);
  printList("readyQueue", readyQueue);
  printf("task %d created a new task %d\n", running->pid, p->pid);
  return p->pid;
}/*end create()*/

int main()
{
  printf("Welcome to the MT User-Level Threads System\n");
  init();
  create((void *)func, 0);
  printf("P0 switch to P1\n");
  while(1){
    if (readyQueue)
      tswitch();
  }
}/*end main()*/

int scheduler()
{
  if (running->status == READY)
    enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  printf("next running = %d\n", running->pid);
}/*end scheduler()*/



