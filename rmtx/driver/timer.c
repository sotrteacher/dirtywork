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
/************************** timer.c ****************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

/* Clock parameters. */
#define LATCH_COUNT     0x00	/* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	/* ccaammmb, a = access, m = mode, b = BCD */

/************************ NOTICE THE DIVISOR VALUE ***********************/
#define TIMER_FREQ   1193182L	/* clock frequency for timer in PC and AT */
#define TIMER_COUNT ((unsigned) (TIMER_FREQ/60)) /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define CLOCK_IRQ       0

typedef struct tn{
       struct tn *next;
       int    time;
       PROC   *who;
} TNODE;

TNODE tnode[NPROC], *tq, *ft;
int row, col;
ushort tick, sec, min, hr;
extern u8 btime[8];           // btime[] in t.c

/*===========================================================================*
 *				clock_init				     *
 *===========================================================================*/
int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */
  int i;
  tick = 0; 

  // set hr min sec to BIOS real time
  /******** u8 btime[4-6]=|hr|mm|sec| **********/
  hr  = 10*(btime[4]>>4); hr  += btime[4]&0x0F;
  min = 10*(btime[5]>>4); min += btime[5]&0x0F;
  sec = 10*(btime[6]>>4); sec += btime[6]&0x0F;

  ft = &tnode[0]; 
  for (i=0; i<NPROC; i++)
    tnode[i].next = &tnode[i+1];
  tnode[NPROC-1].next=0;
  tq = 0;

  out_byte(TIMER_MODE, SQUARE_WAVE);	/* set timer to run continuously */
  out_byte(TIMER0, TIMER_COUNT);	/* load timer low byte */
  out_byte(TIMER0, TIMER_COUNT >> 8);	/* load timer high byte */
  enable_irq(CLOCK_IRQ); 
}

/*===========================================================================*
 *				timer_handler				     *
 *===========================================================================*/
char *wall="00:00:ss";
//          01234567
ushort w;
extern int motor_status, fd_motor_off;   // in floppy disk driver fd.c
extern int tout, fd_timeout;
extern struct semaphore fdsem;
extern int need_reset, fdreset, IFLAG;                 // FD need reset flag
extern PROC *readyqueue;
extern int sw_flag;
PROC *newQ;

#define QUANTA 20

int thandler()
{ 
    int i; PROC *p; TNODE *tp;
    tick++; tick %= 60;
  
    running->time--;           
    running->cpu++;            // every tick ++ CPU time

    if (running->cpu > 127)  
       running->cpu = 127;     // max CPU usage time

    if (motor_status & 0x10){  // if FD motor is running
      fd_motor_off--;          // count down fd_motor_off from 120
      if (fd_motor_off <= 0){
        motor_status = 0x0C;   // turn off FD motor
	out_byte(0x3F2, motor_status);
      }
    }
    if (tout==0){              // if task blocked on fdd
      fd_timeout--;            // count down on fd_timeout
      if (fd_timeout == 0){
        tout = 1;              // set timeout flag (also stop count down)
        V(&fdsem);             // unblock process in fd driver
      }
    }

    // at each second: update TOD and show wall clock, process sleepers,
    // process itimer request queue
    if (tick == 0){
       sec++; sec %= 60;
       if (sec == 0){
          min++; min %= 60;
          if (min  == 0){
              hr++;
          }
       }
       // display wall clock, write to VGA memory directly
       wall[7] = '0'+ (sec % 10); wall[6]='0'+ sec/10;
       wall[4] = '0'+ (min % 10); wall[3]='0'+ min/10;
       wall[1] = '0'+ (hr % 10);  wall[0]='0'+ hr/10;

       for (i=0; i<8; i++){
         w = wall[i];
         w = w | 0x0E00;
         put_word(w, 0xB800, org + 2*(24*80 + 70+i) );
       }

       if (sec == 2) out_byte(0x3F2, 0x0C);

       // process sleepers
       for (i=0; i<NPROC; i++){
         p = &proc[i];
	 if (p->status==SLEEP && p->event==PAUSE){
	   p->pause--;
           printf("%d", p->pause);
           if (p->pause <= 0){
 	     //can't kwakeup(PAUSE) because this would wakeup all on PAUSE
             // must take p out of sleepList and make it READY
             outSleep(p);
             p->status = READY;
             p->event = 0;
             p->pri = 256 - p->cpu;
             schedule(p);
             printf("wakeup %d\n", p->pid);
           }
	 }
       }  
       /* processing timer queue elements */
       if (tq){ // do these only if tq not empty
	  printTQ();
          tp = tq;
          while (tp){
             tp->time--;
             if (tp->time <= 0){ // send SIGALRM = 14
                 tp->who->res->signal |= (1 << 14);
                 tq = tp->next;
                 put_tnode(tp);
                 tp = tq;
                 //printTQ();
             }
             else{
                   break;
             }
          }
       }
    }
    // at each QUANTA ticks, recompute process priority: must be <= run-time
    if ((tick % QUANTA) == 0){ 
    // at every QUANTA ticks, re-compute ALL process priority: ready or blocked 
     for (i=0; i<NPROC+NTHREAD; i++){
         p = &proc[i];
	 if (p->status != FREE && p->status != ZOMBIE){ // a valid PROC
	   if (p->pid)  // never change P0's priority or CPU time
               p->pri = 128 - (p->cpu); // p->time max = 127 
	       p->cpu = 0;
	 }
      } 
      // then reorder the readyQueue
      newQ = 0;
      while( p = dequeue(&readyQueue) ){
	   enqueue(&newQ, p);
      }
      readyQueue = newQ;

      // optional: show re-ordered readyQueue
      if (readyQueue && readyQueue->pid){  // do not print if only P0
	//printf("%d : ", running->pid); printQueue(readyQueue);
      }

    }
    // reschedule if proc was in Umode and readyQ has higher prioirty procs
    if (running->inkmode==1){ 
       if (running->time<=0 || (readyQueue && readyQueue->pri > running->pri)){

          // optional: show timer based proc switching:
	 /****** uncomment to see all timer based process switches
         if (readyQueue && readyQueue->pri > running->pri){
              printf("[%d|%d -> %d|%d]\n", running->pid, running->pri,
		   readyQueue->pid, readyQueue->pri);
          }
	 *********************/
          sw_flag = 1;       // turn on rw_flag for rescheduling
       }
    }
    out_byte(0x20, 0x20);  
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

   printf("timer = ");

   while(tp){
      printf(" [%d, %d] ==> ", tp->who->pid,tp->time);
      tp = tp->next;
   }
   printf("\n");
}

ktimer(int time)
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

    //running->status = TIMER;
    int_on(ps);

    printTQ();

    //    tswitch();
    // return to umode ==> will get SIGALRM when timer expires
}
