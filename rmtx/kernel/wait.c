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

int enterSleep(PROC *p)
{
  PROC *q;
  if (sleepList == 0)
    sleepList = p;
  else{
    q = sleepList;
    while (q->next)
       q = q->next;
    q->next = p;
  }
  p->next = 0;
}

int outSleep(PROC *ptr)
{
  PROC *p, *q; 
  p = q = sleepList;

  while(p){
    if (p->pid != ptr->pid){
      q = p;
      p = p->next;
      continue;
    }
    // found ptr at p
    if (p==sleepList){      // if first in sleepList
	 sleepList = p->next;
    }
    else{                   // not first in sleepList   
       q->next = p->next;   // delete p from list
    }
    return p;
  }
}

int ksleep(int event)
{
  int sr;
  sr = int_off();

  /*********************************************************************
   //printf("\n*************************************\n");
   //printf("Proc %d going to sleep on event=%x", running->pid, event);
   //printf("\n*************************************\n");
   ********************************************************************/

   running->status = SLEEP;
   running->event = event;
   // enter sleepList FIFO
   enterSleep(running);
   // ksleep does not need call schedule()
  int_on(sr);

  tswitch();

  if (running->res->signal)  // if killed by signal, abort from sleeped reason
     kpsig(); 
}
  

/* wake up ALL tasks sleeping on event */
int kwakeup(int event)
{
  PROC *p, *q; 
  int i, sr;
  sr = int_off();

  p = q = sleepList;

  while(p){
    if (p->event != event){
      q = p;
      p = p->next;
      continue;
    }
    // found a p->event==event
    if (p==sleepList){
	 sleepList = p->next;
         p->status = READY;
         p->event = 0;
         schedule(p);
         p = q = sleepList;
         continue;
    }
    // not the first element in sleepList   
    q->next = p->next;   // delete p from list
    p->event = 0;
    p->status = READY;
    schedule(p);
    p = q->next;
  }
  int_on(sr);          
}

int kexit(int exitValue)
{
  int i, pid, status, wake1; PROC *p, *procp;
  //  printf("%d in kexit\n", running->pid);

  /* PROCESS : send children (dead or alive) to P1's orphanage */
  if (running->type == PROCESS){

    if (NTHREAD){ // only if has any threads
       if (running->res->tcount > 1)
    printf("%d is a PROCESS; wait for child threads to die\n", running->pid); 

        while(running->res->tcount > 1){
           pid = kwait(&status);
           printf("%d got a dead child %d\n", running->pid, pid);
       }
    }
    wake1 = 0;    
    for (i = 1; i < NPROC+NTHREAD; i++){ // send children to P1
      p = &proc[i];
      if (p->status != FREE && p->ppid == running->pid){
          p->ppid = 1;
          p->parent = &proc[1];
          wake1 = 1;         
      }
    }
    /* close opened file descriptors */
    for (i=0; i<NFD; i++){
        if (running->res->fd[i] != 0)
           myclose(i);
    }

    /* release Umode memory space */
    /******  debugging
    if (running->pid > 4){
      printf("proc%d in kexit %x %x\n", running->pid, running->res->segment,
	     running->res->size);
    }
    ***********/

    mfree(running->res->segment, running->res->size);
    running->uss = running->res->size = 0;
    running->res->name[0] = 0;  /* clear filename */

    // release PROC's cwd
    ilock(running->res->cwd);
    iput(running->res->cwd);

    // clear PROC'ssignal  
    for (i=0; i<NSIG; i++)
        running->res->sig[i] = 0;
    running->res->signal = 0;
  }
  else{  // dying proc is a thread
      if (running->type == THREAD){
	printf("thread %d in kexit\n", running->pid);
         running->proc->res->tcount--;   // dec PROC's tcount by 1

         // if any child ==> send to main thread of process
         for (i = NPROC; i < NPROC+NTHREAD; i++){ // search thread PROCs only
             p = &proc[i];
             if (p->status != FREE && p->ppid == running->pid){
	         p->ppid = p->proc->pid;  // become mainthread's child
	         p->parent = p->proc;     // p's parent points at PROC
                 p->ppid = p->proc->pid;
             }
         }
      }
  }

  /* record exitValue and become a ZOMBIE */
  running->exitValue = exitValue;
  running->status = ZOMBIE;

  /* wakeup parent and P1 */
  kwakeup(running->parent);
  if (wake1)  // only if has sent orphans to P1
     kwakeup(&proc[1]);
  tswitch();
}

int kwait(int *status)
{
  PROC *p;
  int  i, found = 0;
  while(1){
     for (i=0; i<NPROC+NTHREAD; i++){ 
          p = &proc[i];
          if (p->ppid == running->pid && p->status != FREE){ 
             found = 1;
             /* lay the dead child to rest */
             if (p->status == ZOMBIE){
	         // *status = p->exitValue;
	         put_word(p->exitValue, running->uss, status);

                 p->status = FREE;       /* free its PROC */
                 // restore PROC.name
                 //strcpy(p->name, pname[p->pid]);

                 if (p->type==PROCESS)
		    put_proc(&freeList, p);
                 else
		    put_proc(&tfreeList, p);
		 //printList("freeList  ", freeList);
                 nproc--;
                 return(p->pid);         /* return its pid */
             }
          }
     }
     if (!found)                         /* no child */
          return(-1);
     ksleep(running);                     /* has kids still alive */
  }
}
