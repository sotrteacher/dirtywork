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
/******************* syscall.c *******************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

/************ syscall functions *************/
int getpid()
{
  return running->pid;
}
       
int getppid()
{
  return running->ppid;
}

int getpri()
{
  return running->pri;
}

int ksetpri(int pri)
{
  running->pri = pri;
  return 0;
}

int getuid()
{
  return running->res->uid;
}

int kchuid(int uid, int gid)  // syscall chudi entry
{
    running->res->uid = uid;             // change gid,uid
    running->res->gid = gid;
    return 0;
}

int kswitch()  // syscall yield entry
{
  schedule(running);
  tswitch();
}

int kps(char *y)
{
  PROC *p;
  PS  ps, *q;
  int i, j;
  char *cp;

  q = &ps;

  for (i=0; i<NPROC+NTHREAD; i++){
      p = &proc[i];
      q->pid    = p->pid;
      q->ppid   = p->ppid;
      q->uid    = p->res->uid;
      q->status = p->status;
      strcpy(q->pgm, p->res->name);

      cp = &ps;
      for (j=0; j<sizeof(PS); j++){
	 put_byte(*cp, running->uss, y);
         cp++; y++;
      }
  }
}

int kfixtty(char *y)
{
  char *p; char c;
  p = running->res->tty;

  while(c = get_ubyte(y)){
    *p = c;
    p++; y++;
  }
  *p = 0;
}

int kgettty(char *y)
{
  char *p;

  p = running->res->tty;
  while(*p){
    put_ubyte(*p, y);
    p++; y++;
  }
  put_ubyte(0, y);

}

int pause(int y)
{
  printf("%d pause ", running->pid);
  if (y<=0) return -1;
  running->pause = y;
  ksleep(PAUSE);
  return 0;
}

extern int 
getpid(), getppid(), getpri(), ksetpri(), getuid(), kchuid(), kswitch(),
fork(), kexec(), kwait(), vfork(),  kthread(), mutex_creat(), mutex_lock(), 
mutex_unlock(),mutex_destroy(), kmkdir(), krmdir(), kcreat(), klink(), 
kunlink(), ksymlink(), kreadlink(), kchdir(), kgetcwd(), kstat(), kfstat(), 
kopen(), kclose(), klseek(), kread(), kwrite(), kpipe(), kchmod(), kchown(), 
ktouch(), kfixtty(), kgettty(), kdup(), kdup2(), kps(), kmount(), kumount(), 
kcdSector(), do_cmd(), kkill(), ksignal(), pause(), ktimer(), send(), recv(), 
  tjoin(), texit(), thinit(), khits(), kcolor(), ksync(), kexit(),sbrk(),rbrk();

extern int nocall();

int (*f[ ])() = {
  getpid,   getppid,   getpri,    ksetpri,  getuid, 
  kchuid,   kswitch,   nocall,    nocall,   kexit,
  fork,     kexec,     kwait,     vfork,    kthread, 
  mutex_creat, mutex_lock, mutex_unlock, mutex_destroy,  nocall,
  kmkdir,   krmdir,    kcreat,    klink,    kunlink, 
  ksymlink, kreadlink, kchdir,    kgetcwd,  kstat, 
  kfstat,   kopen,     kclose,    klseek,   kread, 
  kwrite,   kpipe,     kchmod,    kchown,   ktouch,
  kfixtty,  kgettty,   kdup,      kdup2,    kps, 
  kmount,   kumount,   kcdSector, do_cmd,   nocall, 
  kkill,    ksignal,   pause,     ktimer,   send, 
  recv,     tjoin,     texit,     khits,    kcolor,   
  ksync,    sbrk,      rbrk,      thinit 
}; 

int kcinth() 
{
  u16 x, y, z, w, ww;
  int r; 
  u16 seg, off;

  unlock();   // syscall with interrupts masked in
 
  if (running->res->signal)   // check and handle pending signals
     kpsig();

  seg = running->uss; off = running->usp;

  x = get_word(seg, off+13*2);
  y = get_word(seg, off+14*2);
  z = get_word(seg, off+15*2);
  w = get_word(seg, off+16*2);
  ww= get_word(seg, off+17*2);
  
  //printf("proc%d in kcinth : (%d %d %d)\n", running->pid, x, y, z);

  r = (*f[x])(y,z,w,ww);     // call the syscall function

  if (running->res->signal)  // check and handle signal
      kpsig();

   // MUST use running->usp becaue kpsig() may change it.
  put_uword(r, running->usp + 2*8); // return r

  running->pri = 128-running->cpu;  // drop back to Umode priority
}
