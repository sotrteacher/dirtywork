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
/******************** timer.c file *************************************/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

u16 tick;

typedef struct tn{
       struct tn *next;
       int    time;
       PROC   *who;
} TNODE;

TNODE tnode[NPROC], *tq, *ft;

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */
  int i;
  printf("timer init\n");
  tick = 0; 
  out_byte(TIMER_MODE, SQUARE_WAVE);	// set timer to run continuously
  out_byte(TIMER0, TIMER_COUNT);	// timer count low byte
  out_byte(TIMER0, TIMER_COUNT >> 8);	// timer count high byte 
  enable_irq(TIMER_IRQ); 

  ft = &tnode[0]; 
  for (i=0; i<NPROC; i++)
    tnode[i].next = &tnode[i+1];
  tnode[NPROC-1].next=0;
  tq = 0;
}

/*===================================================================*
 *		    timer interrupt handler       		     *
 *===================================================================*/
int thandler()
{
  int i; PROC *p; TNODE *tp;
  tick++; 
  tick %= 60;

  if (tick == 0){                      // at each second
     /* processing timer queue elements */
     if (tq){ // do these only if tq not empty
         tp = tq;
         while (tp){
             tp->time--;
             printTQ();   
             if (tp->time <= 0){ // wakeup sleepers
	         printf("timer interrupt handler : wakeup %d\n", tp->who->pid);
                 kwakeup(&tp->who->time);
                 tq = tp->next;
	         put_tnode(tp);
                 tp = tq;
              }
             else{
                   break;
             }
	 }
     }
  }
  out_byte(0x20, 0x20);                // tell 8259 PIC EOI
}

TNODE *get_tnode()
{
    TNODE *tp;
    tp = ft;
    ft = ft->next;
    return tp;
}

int put_tnode(TNODE *tp)
{
    tp->next = ft;
    ft = tp;
}

int printTQ()
{
   TNODE *tp;
   tp = tq;
   printf("timerQueue = ");
   while(tp){
      printf(" [P%d: %d] ==> ", tp->who->pid, tp->time);
      tp = tp->next;
   }
   printf("\n");
}

itimer(int time)
{
    TNODE *t, *p, *q;
    int ps;

    // CR between clock and this process
    ps = int_off();
    t = get_tnode();
    t->time = time;
    t->who = running;
    /******** enter into tq ***********/
    if (tq==0){
        tq = t;
        t->next = 0;
    }
    else{
          q = p = tq;
          while (p){ 
              if (time - p->time < 0) 
                  break;  
              time -= p->time;
              q = p;
              p = p->next;
          }
          if (p){ 
              p->time -= time;
          }
          t->time = time;
          if (p==tq){
              t->next = tq;
              tq = t;
          }
          else{
                t->next = p;
                q->next = t;
          }
    }
    int_on(ps);
    printTQ();
    printf("PROC%d going to sleep\n", running->pid);
    ksleep(&running->time);  // sleep on its own time address
}
