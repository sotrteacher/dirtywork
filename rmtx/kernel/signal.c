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
/******************* signals.c *******************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

/*********************** KCW: 4-20-06 ************************************  
 kill delivers a signal to a process. If the process is BLOCK or SLEEP,
 we should V()/wakeup it.
 If the process is BLOCKed on a sempahore, we must know which semaphore it is 
 blocked on. So, if a proc is blocked on a semaphore, we let proc.sem -> the
 blocking semaphore. Then we can unblock the process from the semaphore queue
 by the wV(running) operation.
***************************************************************************/

int kkill(int pid, int sig)
{
  // kill pid sig ;
  PROC *p;
  int i, oldsig;

  if (pid < 1 || pid > NPROC) return -1;
  if (sig < 1 || sig >= NSIG) return -1;

  p = &proc[pid];
  if (p->status==FREE || p->status == ZOMBIE) 
     return -1;
  /**** allow any one to kill **************
  if (running->uid != 0){
     if (running->uid != p->uid) 
        return -1;
  }
  ****************************************/
  oldsig = running->res->signal;
  p->res->signal = p->res->signal | (1 << sig);  // turn sig-th bit on

  if (p->status==SLEEP)  // wakeup p if it's SLEEP
     kwakeup(p->event);

  if (p->status==BLOCK){
    // this task is BLOCKed on p->sem semaphore; it may NOT be the first in
    // that semQ, so must do a different V
    /**********************
    // unblock p only if it is waiting for terminal inputs 
    if (p->sem==&kbData || p->sem==&stty[0].inchars || p->sem==&stty[1].inchars)
          wV(p);
    // if blocked on message, unblock it
    if (p->sem == &p->res->message)
       wV(p);
    ***********************/
    wV(p); // unblock p from its semaphore queue
  }
  printf("Kernel: killed pid=%d with sig# %d", pid, sig);

  return oldsig;
}

int ksignal(u16 sig, u16 ucatcher)
{
  int oldsig;

  if (sig < 1 || sig > NSIG) return -1;
  if (sig==9){
      printf2u("can't change signal 9\n");
      return -1;
  }
  oldsig = running->res->sig[sig];
  running->res->sig[sig] = ucatcher;
  //printf("installed sig%d %x\n", sig, ucatcher);
  return oldsig;
}

int setsig(int n)
{
  running->res->signal |= (1 << n);
}

int cksig()
{
  // check pending signal
  int i;
  for (i=1; i<NSIG; i++){
    if (running->res->signal & (1 << i)){
       running->res->signal &= ~(1 << i);
      //      printf("csig: found sig=%d\n", i);
      return i;
    }
  }
  return 0;
}

// when a process is about return to Umode, handle any outstanding signal 
int kpsig()
{
  int  i, n, upc, newpc, w;
  u16 offset = running->usp;

  while(n=cksig()){
     //printf("hsignal: %d's sig=%d\n", running->pid, running->sig[n]);
     if (running->res->sig[n] == 1){  // IGNore this signal
        //printf("tpsig: %d ignore %d\n", running->pid, n);
        continue;
     }
     if (running->res->sig[n] == 0){
        printf("proc %d dying by signal# %d\n", running->pid, n);
        kexit(n<<8);
     }

     newpc = running->res->sig[n];
     running->res->sig[n] = 0;      // reset catcher to default

     upc = get_uword( offset+2*9);  // REPLACE pc at trap point by
     put_uword(newpc, offset+2*9);  // handler entry address

     // move interrupted stack frame downward 2 slots; for catcher's PC and sig#

     for (i=0; i<12; i++){
         w = get_uword(offset+i*2);
         put_uword(w,  offset+(i-2)*2);
     }
     put_uword(upc, offset+2*10);     // return pc
     put_uword(n,   offset+2*11);    // signal# for catcher(signal)

     running->usp -= 4;
     return;
  }
}

int kdivide()
{
  printf("proc%d divide error => signal# 8\n", running->pid);
  running->res->signal |= (1 << 8);
  kpsig();
}

// trap.c in USER demos INT i for "illegal" traps.
// Fisrt, set trap vectors 0-8 all pointing at _traph:  which calls
// ktrap() HERE. The Umode image has done inti(i) before INT i, so the
// trap number i is in ustack at 13. Use it as signal#. Then kpsig();

// !!!! can NOT use any KBD input because we are still in interrupt
// processing mode HERE ==> will mess up KBD 

int ktrap()
{
  u16 w;

  w = get_uword(running->usp + 2*13);
  printf("proc%d traps to kernel by INT %d\n", running->pid, w);
  w = w % 16; // only 0 to 15 types
  running->res->signal |= (1 << w);  
  kpsig(w);
}
