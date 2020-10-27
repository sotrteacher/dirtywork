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
/*********************** pipe.c file **********************************/
#ifndef MK
#include "../type.h"
#endif

#include "../extern.h"

/* here we only need to define the pipe[ ] structures */
PIPE pipe[NPIPE];

/* pipe_init() is called in init() of t.c */

pipe_init()
{
   int i;
   for (i=0; i<NPIPE; i++)
       pipe[i].busy = 0;
}

int read_pipe(int fd, char *buf, int n)
{
  char c; int ret; 
  PIPE *p; OFT *op;

  if (fd < 0 || fd > NFD || running->res->fd[fd] == 0){
     printf("bad fd %d", fd);
     return -1;
  }
  op = running->res->fd[fd];
  p  = op->pipe_ptr;

  if (n<=0)
    return 0;

  ret = 0;
  while(n){
    while(p->data){
        c = p->buf[p->tail++];
        put_ubyte(c, buf);
        p->tail %= PSIZE;
        buf++;  ret++; 
        p->data--; p->room++; n--;
        if (n <= 0)
            break;
    }
    if (ret){   /* has read something */
       kwakeup(&p->room);
       return ret;
    }
    // pipe has no data
    if (p->nwriter){ // still has writer
       kwakeup(&p->room);
       ksleep(&p->data);
       continue;
    }
    // pipe has no data and no writer
    return 0;
  }
}

int write_pipe(int fd, char *buf, int n)
{
  char c;   int ret = 0; 
  PIPE *p;  OFT *op;
  if (fd < 0 || fd > NFD || running->res->fd[fd] == 0){
     printf("bad fd %d\n", fd);
     return -1;
  }
  op = running->res->fd[fd];
  p = op->pipe_ptr;

  ret = 0;
  while (n){
        if (p->nreader == 0){ /* no more readers */
	    printf("proc %d: Broken PIPE : no more readers\n", running->pid);
            kexit(123);    /* MUST die, else ==> many loops */
            return(-1); 
        }
        if (p->room){
               p->buf[p->head++] = get_ubyte(buf); 
               p->head  %= PSIZE;
               buf++;  ret++; 
               p->data++; p->room--; n--;
        }
        else{
               /* wakeup readers before going to sleep */
               kwakeup(&p->data);
               ksleep(&p->room);
        }
  }
  if (ret)                 /* truly wrote something */
      kwakeup(&p->data);
  return(ret);
}

int kpipe(int *y, int *z)
{
  PIPE *p; 
  OFT  *ofp[2]; 
  int i, count, pd[2], I;
  
  for (i=0; i<NPIPE; i++){
    if (pipe[i].busy == 0){
       p = &pipe[i]; p->busy = 1;
       break;
    }
  }
  if (i>=NPIPE){ // no more pipe structs
    printf("out of pipes\n");
    return -1;
  }

  /* allocate 2 ofte's */
  count = 0;
  for (i=0; i<NOFT && count<2; i++){
    if (oft[i].refCount == 0){
       ofp[count++] = &oft[i];
       continue;
    }
  }
  if (count < 2){ // can't allocat 2 OFT's
     p->busy=0;
     printf("out of OFTs\n");
     return -1;
  }

  count = 0;
  for (i=0; i<NFD && count<2; i++){
    if (running->res->fd[i] == 0){
       pd[count++] = i;
       continue;
    }
  }
  if (count < 2){  // can't allocate 2 fd's
     p->busy=0; 
     printf("no fds\n");
     return -1;
  }
  //printf("proc %d : KPIPE: pd[ ]=[%d %d]\n", running->pid, pd[0], pd[1]);
 
  // initialize pipe struct
  p->head = p->tail = p->data = 0;
  p->room = PSIZE;
  
  // initialize PPIE_READ, PIPE_WRITE OFT's
  ofp[0]->mode = READ_PIPE;
  ofp[1]->mode = WRITE_PIPE;
  ofp[0]->refCount = ofp[1]->refCount = 1;
  ofp[0]->pipe_ptr = ofp[1]->pipe_ptr = p;
  // KCW: must zero out inodeptr to prevent confusion in open()
  ofp[0]->inodeptr = ofp[1]->inodeptr = 0;  
  p->nreader = ofp[0];  p->nwriter = ofp[1];
  strcpy(ofp[0]->name, "PIPE_READ"); 
  strcpy(ofp[1]->name, "PIPE_WRITE");

  // initialize running's fd[ ]'s
  running->res->fd[pd[0]] = ofp[0];
  running->res->fd[pd[1]] = ofp[1];

  //printf("proc %d use pipe %d\n", running->pid, I);

  // write pd[0], pd[1] to USpace pd[2]
  put_uword(pd[0], y); 
  put_uword(pd[1], y+1);
  }

int close_pipe(int fd)
{
  OFT *op; PIPE *pp;

  op = running->res->fd[fd];
  running->res->fd[fd] = 0;                     /* clear fd[fd] entry */   

  if (op->mode == READ_PIPE){
      pp = op->pipe_ptr;
      if (--op->refCount == 0){            /* last reader  */
            pp->nreader = 0;                  /* no more readers */   
            if (pp->nwriter == 0){           /* no more writers */
                pp->busy = 0;              /* free the pipe   */
                return; 
            }
      }
      kwakeup(&pp->room); 
      return;
  }

  if (op->mode == WRITE_PIPE){
      pp = op->pipe_ptr;
      if (--op->refCount == 0){           /* last writer */
            pp->nwriter = 0;                /* no more writers */ 
            if (pp->nreader == 0){           /* no more readers */
                pp->busy = 0;             /* free pipe also */
                return;
            }
      }
      kwakeup(&pp->data);
      return;
  }
}

