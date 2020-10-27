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
/********************************************************************
                          queue.c
*********************************************************************/
#ifndef MK
  #include "../type.h"
#endif

/****************** utility.c file ***********************/
PROC *get_proc(PROC **list)  
{
  PROC *p = *list;     // either freeList or tfreeList
        if (p)
	  *list = (*list)->next;    
        return p;
}

int put_proc(PROC **list, PROC *p)   // return a PROC to END of freeList
{
        PROC *q;

        p->status = FREE;
        p->next = 0;
  
        q = *list;
        if (!q)
	  *list = p;
	else{
	  while(q->next)
	    q = q->next;
          q->next = p;
        }
}

enqueue(PROC **queue, PROC *ptr)
{
   PROC *p = *queue;
   if (p == NULL || ptr->pri > p->pri){
         *queue = ptr;
          ptr->next = p;
   }
   else{
         while (p->next && ptr->pri <= p->next->pri)
                p = p->next;
         ptr->next = p->next;
         p->next = ptr;
   }
}

PROC *dequeue(PROC **queue)
{
  PROC *p = *queue;
  if (p)
     *queue = (*queue)->next;
  return p;
}

printQueue(PROC *queue)
{
   PROC *p = queue;
   printf("readyQueue = ");
   while(p){
     printf("%d|%d-> ", p->pid,p->pri);
        p = p->next;
   }
   printf("\n");
}

int removeList(PROC **list, PROC *procp)
{
  PROC *p, *q;

  p = q = *list;
  if (p==procp)
    *list = p->next;
  else{
    q = p->next;
    while(q){
      if (q == procp){
	p->next = q->next;
        return;
      }
      p->next = q;
      q = q->next;
    }
  }
}

int printList(char *name, PROC *p)
{
   printf("%s = ", name);
   while(p){
     printf("%d --> ", p->pid);
       p = p->next;
   }
   printf("NULL\n");
}
