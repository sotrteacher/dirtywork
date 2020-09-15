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

#define NPROC    9
#define SSIZE 1024

/******* PROC status ********/
#define FREE     0
#define READY    1
#define RUNNING  2

typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    pid;                // add pid for identify the proc

  int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
  int    ppid;               // parent pid
  struct proc *parent;
  int    priority;
  int    kstack[SSIZE];      // per proc stack area
}PROC;

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
extern int color;
#include "kernel.c"

int init()
{
    PROC *p;
    int i;
    printf("init ....");

    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;     
        p->next = &proc[i+1];
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

    /**** create P0 as running ******/
    p = get_proc();
    p->status = RUNNING;
    running = p;
    printf("done\n");
} 

            
 main()
{
    printf("\nMTX starts in main()...............2020.09.11\n");
    init();          // initialize and create P0 as running
    kfork();         // P0 kfork() P1
    while(1){
        printf("P0 running\n");
        while(!readyQueue);
        printf("P0 switch process\n");
        tswitch();   // P0 switch to run P1
    }
}


int scheduler()
{
    if (running->status == RUNNING){
       running->status = READY;
       enqueue(&readyQueue, running);
    }
    running = dequeue(&readyQueue);
    running->status = RUNNING;
    color = 0x000A + (running->pid % 6);
}
