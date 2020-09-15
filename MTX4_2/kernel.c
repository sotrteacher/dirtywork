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

/****************** kernel.c file **************************************/
int body()
{
  char c;
  printf("proc %d resumes to body()\n", running->pid);
  while(1){
    printf("-----------------------------------------\n");
    printList("freelist  ", freeList);
    printList("readyQueue", readyQueue);
    printf("-----------------------------------------\n");

    printf("proc %d running: parent = %d  enter a char [s|f] : ", 
	   running->pid, running->parent->pid);
    c = getc(); printf("%c\n", c);
    switch(c){
       case 'f' : do_kfork(); break;
       case 's' : tswitch(); break;
    }
  }
}

PROC *kfork()
{
    int i;
    PROC *p = get_proc();
    if (!p){
       printf("no more PROC, kfork() failed\n");
       return -1;
    }
    p->status = READY;
    p->priority = 1;          // priority = 1 for all proc except P0
    p->ppid = running->pid;   // parent = running
    p->parent = running;

    p->kstack[SSIZE-1] = (int)body;
    for (i=1; i<10; i++)
        p->kstack[i]= 0 ;
    p->ksp = &(p->kstack[SSIZE-9]);
    enqueue(&readyQueue, p);  // enter p into readyQueue by priority
    return p;                 // return child PROC pointer
}         

int do_kfork()
{
    PROC *p; 
    printf("proc %d kfork a child\n", running->pid);
    p = kfork((int)body);
    if (p==0)
       printf("kfork failed\n");
    else
       printf("child pid = %d\n", p->pid);
}
