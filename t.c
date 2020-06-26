/************ t.c User-level Threads *************/
/**
 * REF: Systems Programming in Unix/Linux, K.C. Wang, editorial 
 * Springer Nature, 2018. Chapter 4 Concurrent Programming.
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
 * 'j': let the running task join a target thread
 * to test and observe task execution in the system.
 */
#include <stdio.h>
#include <stdlib.h>
#include "type.h"

PROC proc[NPROC];
PROC *freeList;
PROC *sleepList;
PROC *readyQueue;
PROC *running;

#include "queue.c"
int create(void (*f)(), void *parm);
void func(void *parm);
int do_exit();
void tswitch();


/****** implement these functions ******/
int tsleep(int event)
{ 
  return -1;//AGREGUE SU CODIGO AQUI
}

int twakeup(int event)
{ 
  return -1;//AGREGUE SU CODIGO AQUI
}

int texit(int status)
{ 
  /*ESCRIBA SU CODIGO AQUI*/
  if(20) /* Condicion a deducir ser\'a verdadera si no hay un task */
  {              /* que se quiera unir a runnning */
    running->status=FREE;        /* 2. If no task want to join with this task: exit as FREE */
    enqueue(&freeList,running);
  }else{
    running->exitStatus=status;  /* 3. Record status value in proc.exitStatus */
    running->status=ZOMBIE;      /* 4. Become a ZOMBIE */
    twakeup(running->pid);       /* 5. Wake up joining tasks */
  }
  printList("freeList",freeList);
  tswitch();                     /* 6. Give up CPU  tswitch() */
}/*end texit()*/

int join(int pid, int *status)
{ 
  running->joinPid=1;        /*FIXME*/
  running->joinPtr=&proc[1]; /*FIXME*/
  if(30){
    return 1; /*Suponiendo que nos unimos a la task 1 */
  }
}
/****** end of implementations *********/

int init()// SAME AS in BASE CODE
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

int do_exit()
{
  // for simplicity: exit with pid value as status
  texit(running->pid);
}/*end do_exit()*/

int do_switch()
{
  tswitch();
}/*end do_switch()*/

int do_join()
{
  int status;
  join(1,&status);
}/*end do_join()*/

int do_create()
{
  int pid = create(func, (void*)(running->pid)); // parm = pid
  return pid;
}/*end do_create()*/

void task1(void *parm) // task1: demonstrate create-join operations
{
  int pid[2];
  int i, status;
  //printf("task %d create subtasks\n", running->pid);
  for (i=0; i<2; i++){ // P1 creates P2, P3
    pid[i] = create(func, (void*)running->pid);
  }
  join(5, &status); // try to join with targetPid=5
  for (i=0; i<2; i++){ // try to join with P2, P3
    pid[i] = join(pid[i], &status);
    printf("task%d joined with task%d: status = %d\n",
           running->pid, pid[i], status);
  }
}/*end task1()*/

void func(void *parm) // subtasks: enter q to exit
{
  char c;
  printf("task %d start: parm = %d\n", running->pid, parm);
  while(1){
    printList("readyQueue", readyQueue);
    printf("task %d running\n", running->pid);
    printf("enter a key [c|s|q|j]: ");
    c = getchar(); getchar(); // kill \r
    switch (c){
      case 'c' : do_create(); break;
      case 's' : do_switch(); break;
      case 'q' : do_exit(); break;
      case 'j' : do_join(); break;
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
  for (i=1; i<13; i++)
    p->stack[SSIZE-i] = 0;
  p->stack[SSIZE-1] = (int)parm;
  p->stack[SSIZE-2] = (int)do_exit;
  p->stack[SSIZE-3] = (int)f;
  p->ksp = (int)&p->stack[SSIZE-12];

  enqueue(&readyQueue, p);
  printList("readyQueue", readyQueue);
  printf("task%d created a new task%d\n", running->pid, p->pid);
  return p->pid;
}/*end create()*/

int main()
{
  int i, pid, status,k;
  printf("Welcome to the MT User-Threads System\n");
  init();
  create((void *)task1, 0);
  printf("P0 switch to P1\n");
  for(k=0;k<NPROC;k++){
    printf("&proc[%d]=%p\n",k,&proc[k]);
  }
  tswitch();
  printf("All tasks ended: P0 loops\n");
  while(1);
}/*end main()*/

int scheduler()
{
  printf("running=%p\n",running);
  if (running->status == READY)
    enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
}/*end scheduler()*/




