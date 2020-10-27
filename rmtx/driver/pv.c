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

#define enterCR  ps=int_off()
#define exitCR   int_on(ps)

int P(struct semaphore *s)
{
  PROC *p; int ps;
  //ps=int_off();

  enterCR;

   s->value--;
   if (s->value < 0){
      running->status=BLOCK;
      running->sem = s;
      enqueue(&s->queue, running);
      //does not need schdule() call
      tswitch();
   }
   exitCR;

 if (running->res->signal)
   kpsig();
}

int wV(PROC *p)
{
  PROC *q, *r; int ps;
  struct semaphore *s = p->sem ;

  ps=int_off();
    if (p == s->queue){
      s->queue = p->next;
    }
    else{
      r = s->queue;
      q = r->next;
      while(q){
        if (p == q){
	  r->next = q->next;
          break;
        }
        r = q;
        q = q->next;
      }
    }
    s->value++;
    p->status = READY; p->sem = 0;
    p->pri = 256 - p->cpu;
    schedule(p);

  int_on(ps);
}
 
int V(struct semaphore *s)
{
  PROC *p; int ps;
  ps=int_off();

    s->value++;

    if (s->value <= 0){
        p = dequeue(&s->queue);
        p->sem = 0;
        p->status = READY;
        /***************************************************
         device drivers and file system use semaphore for sync.
         assign HIGH priority to unblocked process
	***************************************************/ 
        p->pri = 256 - p->cpu;
        schedule(p);
    }
  int_on(ps);
}
