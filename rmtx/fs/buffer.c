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
/************************ buffer.c file ***************************/
/******************* in type.h **********************
#define NBUF 4
struct buf{
  struct buf *next_free;
  struct buf *next_dev;
  int opcode;             // READ | WRITE
  int dev,blk;
  int dirty;
  int busy;
  int async;              //ASYNC write flag
  int valid;              // data valid
  int wanted;
  struct semaphore lock;
  struct semaphore iodone;
  char buf[1024];         // data area
} buffer[NBUF], *freelist;
*****************************************************/
#ifndef MK
  #define NDEC 10
  #include "../type.h"
#endif

#include "../extern.h"

struct buf buffer[NBUF], *freelist;

/******* in type.h ****************
struct devtab{
  int dev;
  u32  start_sector;
  u32  size;
  struct buf *dev_list;
  struct buf *io_queue;
};
********************************/
int requests, hits;
struct devtab devtab[NDEV];
struct semaphore freebuf;

int binit()
{
  int i;
  struct buf *bp;
  //printf("binit ");

  freelist = &buffer[0];  // all lists are signle-linked  

  for (i=0; i<NBUF; i++){
    bp = &buffer[i];
    bp->next_free = &buffer[i+1];
    bp->next_dev = 0;

    bp->dev = -1;
    bp->blk = -1;    // 0 is a valid blk #

    bp->dirty = 0;
    bp->async = 0;
    bp->busy = 0;    // not busy
    bp->valid = 0;
    bp->wanted = 0;
    bp->lock.value   = 1; bp->lock.queue   = 0;
    bp->iodone.value = 0; bp->iodone.queue = 0;
  }
  buffer[NBUF-1].next_free = 0;
  freebuf.value = NBUF; freebuf.queue = 0;
  requests = hits = 0;

  for (i=0; i<NDEV; i++){
    devtab[i].dev = i;
    devtab[i].start_sector = 0;
    devtab[i].size = 0;
    devtab[i].dev_list = 0;
    devtab[i].io_queue = 0;
  }
  //printlist(freelist,0);
}

// buffer lists utility functions

int printlist(struct buf *p, int which)
{ 
  int ps;
  ps = int_off();
    while (p){
      printf("[%d %d]->", p->dev, p->blk);

      if (which==0)
         p = p->next_free;
      else
         p = p->next_dev;
    }
     printf("\n");
  int_on(ps);
}

int out_freelist(struct buf *bp)
{ 
  int ps;
  struct buf *p,*q;

  ps = int_off();

    if (bp==freelist){
      freelist = freelist->next_free;
      return;
    }
    q = freelist;
    p = q->next_free;
    while (p){
      if (p==bp){
        q->next_free = p->next_free;
        p->next_free = 0;
        return;
      }
      q = p;
      p = p->next_free;
    }
    int_on(ps);
}

// remove bp from its dev_list

int out_devlist(struct buf *bp)
{
  struct buf *p, *q;
  struct devtab *dp;
  dp = &devtab[bp->dev];

  if (bp == dp->dev_list){
      dp->dev_list = dp->dev_list->next_dev;
      return;
  }
  q = dp->dev_list;
  p = q->next_dev;
  while(p){
    if (p==bp){
       q->next_dev = p->next_dev;
       return;
    }
    q = p; 
    p = p->next_dev;
  }
}


int enter_freelist(struct buf *bp)
{
  int ps;
  struct buf *p;
 
  ps = int_off();
    if (freelist == 0){
      freelist = bp;
    }
    else{
      p = freelist;
      while (p->next_free)
         p = p->next_free;
      p->next_free = bp;
    }
    bp->next_free = 0;
 int_on(ps);
}


int enter_devlist(struct buf *bp)
{
  struct devtab *dp;
  dp = &devtab[bp->dev];

  // dev_list has no order, so add bp to front of list
  bp->next_dev = dp->dev_list;
  dp->dev_list = bp;
}

char *bufPtr;  // global buffer pointer for interrupt handler to use

int start_io(struct buf *bp)
{
  int ps;
  u32 sector;
  struct buf *p;
  struct devtab *dp;

  if (bp->dev==FD){  // FD I/O are SYNC: both r/w wait until completion. So
                     // there is no I/O-queue for FD; only fdio semaphore queue
     fdrw(bp->opcode, bp->blk, bp->buf);
     return;
  } 

  // HD: enter bp into dev IO_queue
  ps = int_off();
    dp = &devtab[bp->dev];
  
    if (dp->io_queue == 0){
       dp->io_queue = bp;
    }
    else{
       p = dp->io_queue;
         while (p->next_free)
            p = p->next_free;
       p->next_free = bp;
    }
    bp->next_free = 0;

    if (dp->io_queue == bp){  // must be HD 
        sector = devtab[bp->dev].start_sector + 2*bp->blk;
        //printf("sector=%l %d %l\n", devtab[bp->dev].start_sector, 2*bp->blk, sector);
        // set globals hd_dev, bufPtr (in hd.c) for hdhandler to use
        hd_dev = bp->dev;
        bufPtr = bp->buf;

        hd_rw(bp->opcode, (u32)sector, bufPtr, 2);
    }
    int_on(ps);
}
 
// awrite is for writing dirty HD buffer out ASYNC
int awrite(struct buf *bp)
{
  if (bp->dev==FD){       
      bp->opcode = 0xC5;  // FD write opcode
      start_io(bp);       // write bp out until completion
      brelse(bp);         // then release bp (it is not the bp proc wants)
    return;
  }   

  // HD
  bp->opcode = 0x30;    // HD_WRITE
  bp->async = 1;        // ASYNC write ==> brelse() by interrupt handler
  bp->dirty = 0;        // no longer dirty
  start_io(bp);         // enter HD dev's IO-queue and return
}

struct buf *search_dev(int dev, int blk)
{
  struct buf *bp;

  bp = devtab[dev].dev_list;

  while(bp){
    if (bp->dev==dev && bp->blk==blk)
      return bp;
    bp = bp->next_dev;
  }
  return 0;
}

struct buf *getblk();

struct buf *bread(int dev, int blk)
{
  int i; 
  struct buf *bp, *bbp;

  bp = getblk(dev, blk);

  if (bp->valid == 0){
      if (dev==FD){ // FD fdrw() is SYNC; already waited for data valid. 
	bp->opcode = 0xE6; // high bits 1110 = multiTrack,Encoding | READ=0x06; 

	// FOUND OUT: 1. FD_READ must be 0xE6; else read wrong block?
        //            2. even with 0xE6, did not work for Thinkpad A30

          start_io(bp);
          bp->valid = 1;
      }
      else{        // HD
          bp->opcode = 0x20;
          start_io(bp);
          P(&bp->iodone);              // proc BLOCK until I/O completes
      }
  }
  return bp;
}  

// bwrite() called from ubwrite() in dev.c
int bwrite(struct buf *bp)
{
  if (bp->dev == FD){  // call fdrw() to write bp out until done
     // FD bp never dirty
     bp->valid = 1;    // data in bp certainly good
     bp->opcode = 0xC5; 
     fdrw(bp->opcode, bp->blk, bp->buf);
     // upon return, data has been written out     
  }
  else if (bp->dev == HD){
     //printf("HD bwrite: bp->dev=%x\n", bp->dev);
     bp->valid = 1;
     bp->dirty = 1;
  }
  brelse(bp);        // both FD and HD
}


/*********************** 5-28-06 ******************************************
            Buffer management using P/V on semaphores.
 This is the cleanest PV implementation of getblk()/brelse()
 It guarantees buffer uniqueness and no starvation. Its cache effect seems
 to be as good as that of the "optimal" algorithm in pr.io.pv
**************************************************************************/
//**************************************************************************

/***************************** 10-20-2010 **********************************
 Tried to re-write FD driver in such a way that a process calls fdrw() and 
 immediately returns, as in HD driver, in order to support ASYNC writes.
 To do this, the FD driver must be splited into 2 parts; process part and 
 interrupt handler part. For each FD command that generates an interrupt 
 (reset, clibrate, seek) the interrupt handler MUST issue fd_out(SENSE) to read 
 status, but fd_out() may incur reset/seek, etc. which makes the interrupt 
 handler extremely complex. After several tries, such FD drivers still hang up,
 mostly after a seek() in the process part, so none worked successfully.
 These were very frustrating experiences. Whoever designed the NCE FD controller
 ought to be shot or go to hell. Just a lot of non-sense. It's amazing any
 FD driver actually worked at all.
 -------------------------------------------------------------------------
 After several frustrating tries, decided to give up and keep the FD driver 
 as is, in which everything is done entirely by the process itself. After 
 each command, the process P(fdsem) to wait for interrupt. The interrupt handler
 simply V(fdsem) to let the process itself read status and handle RETRIES.

 So the FD driver is SYNCHRONOUS and can be blocked many times while in fdrw().
 On each blocking, process switch will occur ==> in buffer management we cannot
 do ASYNC FD writes and immediately continue, which means no delayed writes for
 buffers associated with FD. (But delayed write buffers may exist for HD).
 -------------------------------------------------------------------------
 So the decision is to check for FD requests in getblk():
 if FD request: any buffer found existing in cache must not be for dealyed 
                write. Only free bufs belonging to HD may be delayed write, 
                but writing out such DIRTY bufs is still ASYNC.
 For FD R/W, both are SYNC by the process.
 if HD request: no change.

 TESTed results: seems to work fine. After booting up, 
        mount /dev/fd0 /fd0
        then cat cp files in /fd0
 all work OK
**************************************************************************/ 
 
struct buf *getblk(int dev, int blk)
{
  struct buf *bp; 
  requests++;

  while(1){
    P(&freebuf);           // get a free buf
    bp = search_dev(dev,blk);
    if (bp){               // buf in cache
       
       hits++;
       if (bp->busy){      // buf busy
         V(&freebuf);      // bp is not in freelist, so give up the free buf
         P(&bp->lock);     // wait for bp 

         return bp;
       }
       // bp in cache and not busy
       bp->busy = 1;       // mark bp busy
       out_freelist(bp);
       P(&bp->lock);       // lock bp
       return bp;
    }

    // buf not in cache; already has a free buf in hand
    lock();
      bp = freelist;
      freelist = freelist->next_free;
    unlock();

    bp->busy = 1;         // mark bp busy; redundant, bp->lock is enough
    P(&bp->lock);         // lock the buffer

    if (bp->dirty){       // delayed write buf, can't use it
      // bp MUST belong to HD : awrite bp out and try next free buf immediately
      if (bp->dev == HD){
	awrite(bp);
        continue;         // continue while(1) loop
      }
      if (bp->dev == FD){
	printf("DIRTY bp of FD, this should never happen\n");
        kgetc();          // panic error
      }
    }
    // bp is a new buffer; reassign it to (dev,blk)
    if (bp->dev != dev){
      if (bp->dev >= 0)
	out_devlist(bp);
      bp->dev = dev;
      enter_devlist(bp);
    }
    bp->dev = dev;
    bp->blk = blk;
    bp->valid = 0;
    bp->async = 0;
    bp->dirty = 0;
    return bp;
  }
}

int brelse(struct buf *bp)
{
  if (bp->lock.value < 0){             // bp has waiter
    V(&bp->lock);
    return;
  }  
  if (freebuf.value < 0 && bp->dirty){ // no dirty buf to waiter as free 
      awrite(bp);
      return;
  }
  enter_freelist(bp);
  bp->busy = 0;
  V(&bp->lock);
  V(&freebuf);

}

int khits(int y, int z) // syscall hits entry
{
   put_uword(requests, y); 
   put_uword(hits,z); 
   return NBUF;  
}

