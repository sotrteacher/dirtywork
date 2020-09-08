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
#define NPROC 9                // number of PROCs
#define SSIZE 1024             // per proc stack area 
#define RED 

typedef struct proc{
    struct proc *next;
    int    *ksp;               // saved ksp when not running
    int    pid;                // add pid for identify the proc
    int    kstack[SSIZE];      // proc stack area
}PROC;

int  procSize = sizeof(PROC);

PROC proc[NPROC], *running;    // define NPROC procs
extern int color;

int body()
{ 
   char c;
   printf("proc %d resumes to body()\n", running->pid);
   while(1){
     color = running->pid + 7;
     printf("proc %d running : enter a key : ", running->pid);
     c = getc(); 
     printf("%c\n", c);
     tswitch();
   }
}

int init()
{
   PROC *p;
   int i, j;

   /* initialize all proc's */
   for (i=0; i<NPROC; i++){
       p = &proc[i];
       p->pid = i;                        // pid = 0,1,2,..NPROC-1
       p->next = &proc[i+1];              // point to next proc
       if (i){                            // not for P0
          p->kstack[SSIZE-1] = (int)body; // entry address of body()
          for (j=2; j<10; j++)            // kstack[ ] high end entries = 0
               p->kstack[SSIZE-j] = 0;
          p->ksp = &(p->kstack[SSIZE-9]);
       }
   }
   proc[NPROC-1].next = &proc[0];         // all procs form a circular link list

   running = &proc[0];                    // P0 is running 

   printf("init complete\n");
 }
   
int scheduler()
{
   running = running->next;
}

main()
{
  printf("\nMTX starts in main().....................2020.09.08\n");
  init();
  while(1){
    printf("proc 0  running : enter a key : \n");
    getc();
    tswitch();
  }
}


