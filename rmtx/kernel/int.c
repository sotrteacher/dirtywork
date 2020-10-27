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
/*********************************************************************
             int.c file : syscall routing
*********************************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

extern int 
getpid(), getppid(), getpri(), ksetpri(), getuid(), kchuid(), kswitch(),
fork(), kexec(), kwait(), vfork(),  kthread(), mutex_creat(), mutex_lock(), 
mutex_unlock(),mutex_destroy(), kmkdir(), krmdir(), kcreat(), klink(), 
kunlink(), ksymlink(), kreadlink(), kchdir(), kgetcwd(), kstat(), kfstat(), 
kopen(), kclose(), klseek(), kread(), kwrite(), kpipe(), kchmod(), kchown(), 
ktouch(), kfixtty(), kgettty(), kdup(), kdup2(), kps(), kmount(), kumount(), 
kcdSector(), do_cmd(), kkill(), ksignal(), pause(), ktimer(), send(), recv(), 
tjoin(), texit(), thinit(), khits(), kcolor(), ksync(), kexit();

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
  ksync,    thinit 
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

  x = get_word(seg, off+14*2);
  y = get_word(seg, off+15*2);
  z = get_word(seg, off+16*2);
  w = get_word(seg, off+17*2);
  ww= get_word(seg, off+18*2);
  
  //printf("proc%d in kcinth : (%d %d %d)\n", running->pid, x, y, z);

  r = (*f[x])(y,z,w,ww);     // call the syscall function

  if (running->res->signal)  // check and handle signal
      kpsig();

   // MUST use running->usp becaue kpsig() may change it.
  put_uword(r, running->usp + 2*8); // return r

  running->pri = 128-running->cpu;  // drop back to Umode priority
}
