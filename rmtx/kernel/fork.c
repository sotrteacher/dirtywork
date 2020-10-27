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
/************************************************************************
                    fork.c 
************************************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

int hdinth();

// fork1() creates the Kmode part of a proc; common to both kfork() and fork()
PROC *fork1()
{
  PROC *p;
  int i;

  /*** get a proc for child task: ***/
  if ( (p = get_proc(&freeList)) == NULL){
       printf("\nno more PROC  ");
       return 0;
  }
  /* initialize the new PROC and its kstack */
  p->status = READY;
  p->ppid = running->pid;
  p->parent = running;

  p->pri  = 128;
  p->res->uid = running->res->uid;
  p->res->gid = running->res->gid;
  p->res->cwd = running->res->cwd;
  p->res->cwd->refCount++;
  p->res->name[0] = 0;
  p->inkmode = 1;
  p->time = 0;
  p->type = PROCESS;

  // p->signal, p->sig[] are cleared in kexit()
  p->res->signal = 0;
  for (i=0; i<NSIG; i++)
    p->res->sig[i] = 0;

  /* clear "saved registers" to 0 */ 
  for (i=1; i<=9; i++)
       p->kstack[SSIZE-i] = 0;

  /* fill in the needed entries */
  p->kstack[SSIZE-1] = (int)goUmode;
  p->ksp = &(p->kstack[SSIZE - 9]); 

  /**** copy file descriptors ****/
  for (i=0; i<NFD; i++){
      p->res->fd[i] = running->res->fd[i];
      if (p->res->fd[i] != 0)
          p->res->fd[i]->refCount++;
  }
  /***** clear message queue ******/
  p->res->mqueue = 0; 
  p->res->mlock.value = 1; p->res->mlock.queue = 0;
  p->res->message.value = 0; p->res->message.queue = 0;

  return p;
}


// kfork() is only used by P0 to creat P1 with Umode image=/bin/init

int kfork(char *filename)
{
  PROC *p;
  int i, j, index, *ip;
  ushort es, segment, tsize, TOP;
 
  p = fork1();
  if (p==0){
    return -1;
  }  

  // then, load Uimage for new proc
  tsize = 20480;        // give it a tsize big enough for /bin/init
  p->res->size = tsize/16;

  segment = kmalloc(p->res->size);
  p->res->segment = segment;

  load(filename, segment);  
  strcpy(p->res->name, "init"); // record program name of P1 in its proc.name

   TOP = p->res->size * 16;
  /**************************************************
    We know this task's segment. ustack is at the TOP 
    of this segment. We must make ustak contain:
         1   2   3  4  5  6  7  8  9  10 11 12
       flag uCS uPC ax bx cx dx bp si di es ds
     0x0200 seg  0  0  0  0  0  0  0  0 seg seg
    So, first a loop to set all to 0, then
    put_word(seg, segment, TOP-i*2); i=1,10,11;
   **************************************************/
 
   for (i=1; i<=12; i++){
       put_word(0, segment, TOP-2*i);
   }
   put_word(0x0200,  segment, TOP-2*1);   /* flag */  
   put_word(segment, segment, TOP-2*2);   /* uCS */
   put_word(segment, segment, TOP-2*11);  /* uES */
   put_word(segment, segment, TOP-2*12);  /* uDS */

  /* initial USP relative to USS */
  p->usp = TOP - 2*12; 
  p->uss = segment;
  p->vforked = 0;
  enqueue(&readyQueue, p);
  //schedule(p);
  return(p->pid);
}

int copyImage(u16 s1, u16 s2, u16 size)
{
   ushort i, w;
   for (i=0; i<size*8; i++){
       w = get_word(s1, 2*i);
       put_word(w, s2, 2*i);
   }
}

int fork()
{
  PROC *p;
  int i,index, *ip, *iq;
  ushort segment, size;

  //printf("proc %d forking\n", running->pid);

  segment = kmalloc(running->res->size);  // should check for no memory
  if (segment == 0){
    printf("kernel fork: out of memory\n");
    return -1;
  }

  p = fork1();      // goUmode directly

  if (p==0){
     mfree(segment, running->res->size);
     return -1;
  }

  strcpy(p->res->tty, running->res->tty);

  /***** Fill in U mode information *****/
  p->res->size = running->res->size;
  p->res->segment = segment;

  copyImage(running->res->segment, segment, running->res->size);

  /********************************************************
    copyImage copies parent's U image to child, including
    ustack contents.  However, the child's segment must be
    changed to its own, and its return pid in saved ax must
    be changed to 0
  *********************************************************/

   /*** first, child's uss and usp must be set: *****/
   p->uss = segment;
   p->usp = running->usp;   /* same as parent's usp */

   /*** next, change uDS, uES, uCS in child's ustack ****/
   put_word(segment, segment, p->usp);        /* uDS    */
   put_word(segment, segment, p->usp+2);      /* uES    */
   put_word(0, segment, p->usp+2*8);          /* ax = 0 */
   put_word(segment, segment, p->usp+2*10);   /* uCS    */
   
   //enqueue(&readyQueue, p);
   schedule(p);
   return(p->pid);
}

int setpri(int v)
{
  int r = running->pri;
      running->pri = v;
      return r;
}

int goUmode();

int vfork()
{
  PROC *p;  int pid;  u16 segment;
  int i, w; int count;
  int GAP;

  /********** optional: show it's vfork() **********
  printf("%d vfork() in kernel\n", running->pid);
  *************************************************/
  p = fork1();    // create child but do NOT load any Umode image for child 
  if (p==0){        // fork1() failed 
     return -1;
  }
  strcpy(p->res->tty, running->res->tty);
  p->res->size = running->res->size;
  p->res->segment = running->res->segment;

 /*******************************************************************  
  extend parents ustack with another syscall frame for child to return with
  -------------------------------------------------------------
       |cds ces ......cax....<===..|pDS pES ........PC CS FLAG |         
  -------------------------------------------------------------
        cusp                        pusp            frame from pid=vfork()

  cusp = running->usp -24; <== more also need the return frames before INT 80
  copy psup 24 bytes to cusp:

*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
**************************************************************************/
  
  //printf("fix ustack for child to return to Umode\n");
  GAP = 128; // longest syscall frame = 17 words = 34 bytes
  //count = 0;
  for (i=0; i<24; i++){  // 24 is enough; > 24 should also work
      w = get_word(running->uss, running->usp+(i*2));
      put_word(w, running->uss, running->usp-GAP+(i*2));
      /****** for debugging ****
      printf("%x ", w);
      count++;
      if ((count % 8)==0) printf("\n");
      ************************/
  }
  p->uss = running->uss;
  p->usp = running->usp - GAP;

  //KCW: child saved bp at usp+4*2 must be shifted by 1024 also
  w = get_word(p->uss, p->usp+4*2);
  w -= GAP;
  put_word(w, p->uss, p->usp+4*2);
  
  put_word(0, running->uss, p->usp+8*2);  // child return value=0 
  p->kstack[SSIZE-1] = goUmode;           // resume to goUmode()

  p->vforked = 1; // this proc is vforked: share same image as parent==>exec()
  p->pri = 256;
  
  //enqueue(&readyQueue, p);
  schedule(p);
  return p->pid;
}



