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
/********************** hd.c ********************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

//#define INT_CTL         0x20	/* I/O port for interrupt controller */
#define INT_CTLMASK     0x21	/* setting bits in this port disables ints */
#define INT2_CTL        0xA0	/* I/O port for second interrupt controller */
#define INT2_MASK       0xA1	/* setting bits in this port disables ints */

#define HD_DATA        0x1F0    // data port for R/W
#define HD_IGNORE      0x1F1    // old pre-compensation; IDE ignores this
#define HD_SEC_COUNT   0x1F2    // R/W sector count
#define HD_LBA_LOW     0x1F3    // LBA low  byte
#define HD_LBA_MID     0x1F4    // LBA mid  byte
#define HD_LBA_HI      0x1F5    // LBA high byte
#define HD_LBA_DRIVE   0x1F6    // 1B1D TOP4 => B=LBA, D=drive => 0xE0 
#define HD_CMD         0x1F7    // command : R=0x20 W=0x30
#define HD_STATUS      0x1F7    // status register

#define HD_CONTROL     0x3F6    // 0x80 (0000 1RE0): Reset, E=1 NO interrupt

/* HD disk controller command bytes. */
#define HD_FORMAT      0x50	// format track 
#define HD_RECALIBRATE 0x10	// recalibrate 
#define HD_READ        0x20	// read 
#define HD_WRITE       0x30	// write
#define HD_SPECIFY     0x91	// accept params

/* Parameters for the disk drive. */
//#define SECTOR_SIZE      512	// sector size in bytes

/* Error codes */
#define ERR		  -1	/* general error */
#define EIO               -1
#define OK                 0
#define TRUE               1
#define FALSE              0

/* Miscellaneous. */
//#define MAX_ERRORS         4	/* how often to try rd/wt before quitting */
#define MAX_RETRY         10000	/* max # times to try to output to HD */

u32 start_sector;

/******** in type.h **********
struct hd {		
  u16   opcode;		
  u16   drive;	
  u32    start_sector;
  u32    size;               // size in number of sectors
}
*****************************/

struct hd  hda[16], hdb[16];            // hda[] for drive 0; hdb[] for drive 1
struct semaphore hdio;

int hd_need_reset = FALSE;

/*==========================================================================*
 *				enable_irq				    *
 *==========================================================================*/
/**********
int enable_irq(irq_nr) unsigned irq_nr;
{
  lock();
  if (irq_nr < 8)
	out_byte(INT_CTLMASK, in_byte(INT_CTLMASK) & ~(1 << irq_nr));
  else
	out_byte(INT2_MASK, in_byte(INT2_MASK) & ~(1 << (irq_nr - 8)));
  unlock();
}
********/

struct partition {
	u8 drive;		/* 0x80 - active */
	u8 head;		/* starting head */
	u8 sector;		/* starting sector */
	u8 cylinder;		/* starting cylinder */
	u8 sys_type;		/* partition type */
	u8 end_head;		/* end head */
	u8 end_sector;	        /* end sector */
	u8 end_cylinder;	/* end cylinder */
	unsigned long start_sector; /* starting sector counting from 0 */
	unsigned long nr_sectors;   /* nr of sectors in partition */
};

int fd, cylSize, ecount, extPar;
u32 extStart;            // Extend partition start sector

//char pbuf[1024], fbuf[1024];

int ICOUNT;
char *bufPtr;
int  hd_dev;
char *type;

char *ptype(t) int t;
{
  char *s = "Other";
  if (t==0x05) s="Extnd";
  if (t==0x06) s="Fat16";
  if (t==0x07) s="Ntfs ";
  if (t==0x0b) s="Fat32";
  if (t==0x0c) s="Fat32";
  if (t==0x0f) s="WnExt";
  if (t==0x82) s="LSwap";
  if (t==0x83) s="Linux";
  if (t==0x90) s="MTX  ";
  return s;
}

u32 pagain(u32 start)
{
  u32 s_cyl, e_cyl, e_sector, nblocks, startSector;
  struct partition *p;
  u32 sector, cyl;
  struct hd *hp;
  struct buf *bp;

  ecount++;                             // recursion count
  startSector = extStart + start;      // current partition start sector#     

  start_sector = startSector;  // fake start_sector for get_block()
  devtab[boot_dev].start_sector = start_sector;  // fake into devtab[ ]
  //  get_block(boot_dev, 0, fbuf);
  bp = bread(boot_dev, 0);

  binit();
  p = (struct partition *)(&bp->buf[0x1be]);

  s_cyl = (startSector + p->start_sector)/cylSize + 1;
  e_sector = startSector + p->start_sector + p->nr_sectors - 1;
  e_cyl = (e_sector)/cylSize + 1;
  nblocks = p->nr_sectors/2;
  type = ptype(p->sys_type);
  /**********
  printf("%d : %s  %l  %l  %l\n", ecount, type,
                  (u32)s_cyl, (u32)e_cyl, (u32)nblocks);
  **********/
  hp = &hda[ecount];
  hp->start_sector = startSector + p->start_sector;
  hp->size = p->nr_sectors;

  p++;

  return (p->start_sector);
}

/************************** 5-26-06 ***********************************
 To support reading MBR and extended partitions, must re-initialize the
 buffer cache after each read with blk=0 relative to its start_sector
 Otherwise, getblk() will find the (incorrect) old blk=0 bp in cache.
 So, after each get_block(dev,0,pbuf), call binit() once to reset the
 buffer cache lists. This worked FINE.
 When MTX starts, the buffer cache is re-initialized, which works FINE.
***********************************************************************/

int hd_init()
{
  int i, j, r;
  struct hd *hp;
  struct partition *p;
  u32 next_sector;
  struct buf *bp;

  int heads, sectors; 
  unsigned long scyl, ecyl, ssector, esector, nblocks, fsector;
  u16 lo, hi; 
  u32 bsector;

  printf("hd_init, ");
  hdio.value = 1; hdio.queue = 0;

  /******************
  lo = get_word(0x9000,504);
  hi = get_word(0x9000,506);
  bsector = hi;
  bsector = (bsector<<16) + (u32)lo;
  printf("bsector = %l\n", (u32)bsector);
  lo = get_word(0x1000,10);
  hi = get_word(0x1000,12);
  bsector = hi;
  bsector = (bsector<<16) + (u32)lo;
  printf("bsector = %l\n", (u32)bsector);

  devtab[boot_dev].start_sector = bsector;
  ******************/

   // read partition tables and initialize start_sector, size fields
  printf("reading MBR\n");
  start_sector = 0;        // fake start_sector for get_block()
  devtab[boot_dev].start_sector = start_sector;  // fake into devtab[ ]
  //get_block(boot_dev, 0, pbuf);
  bp = bread(boot_dev, 0);

  binit();  // reset buffer cache
  /*******************************************
  p = (struct partition *)(&bp->buf[0x1be]);
  sectors = p->end_sector & 0x3F;
  heads   = p->end_head + 1;
  cylSize = sectors * heads;

  printf("heads=%d  sectors=%d  cylSize=%d\n", heads, sectors, cylSize);
  printf("P# : Type   Start   End      Blocks \n");
  printf("---------------------------------------\n");  

  ecount = 0;

  for (i=0; i<4; i++){
    if (p->nr_sectors==0)
      continue;
    type = ptype(p->sys_type);
    scyl = p->start_sector/cylSize + 1;
    esector = p->start_sector + p->nr_sectors - 1;
  
    ecyl = esector/cylSize + 1;
    nblocks = p->nr_sectors/2;
 
    printf("%d : %s  %l  %l  %l\n", 
           i+1, type,(u32)scyl,(u32)ecyl,(u32)nblocks);
    p++;

  }
  printf("---------------------------------------\n");
  ************/

  p = (struct partition *)(&bp->buf[0x1be]);
  for (i=1; i<=4; i++){
     hp = &hda[i];
     hp->start_sector = p->start_sector;
     hp->size = p->nr_sectors;
     p++;
  }

  p = (struct partition *)(&bp->buf[0x1be]);

  for (i=1; i<=4; i++){
      if (p->sys_type == 5){ // EXT type
          ecount = 4;  
          extStart = p->start_sector;
	  //printf("ext %d %l\n", ecount,(u32)extStart);
          next_sector = pagain((u32)0);
          while(next_sector != 0)
            next_sector = pagain(next_sector);
       }
       p++;
   }

  //  for (i=1; i<=4; i++)
  //     hda[0].size += hda[i].size;
  hda[0].size = hda[4].start_sector + hda[4].size;

  //printf("========== Partitions =============\n");
  for (i=0; i<8; i++){
    /*****
    if (i==5)
       printf("----------  Extended  -------------\n"); 
    ****/          
    hp = &hda[i];
    devtab[i].start_sector = hp->start_sector;
    devtab[i].size = hp->size;

    /*******************
    if (hp->start_sector){
      printf("%l %l\n",(u32)hp->start_sector, (u32)hp->size);
    }
    ***********/
  }
}

/***********************  5-26-06 ********************************************
 Modified hd driver to support block buffer cache as follows:
 1. proc call get_block/put_block(dev,blk,buf) to read/write a disk block.

 2. get_block(): call getblk() to get a bp assigned to (dev,blk);
                 if bp's data invalid ==> start_io(bp), which enqueues bp
                 to dev's IOQ. start hd read if bp is first in IOQ
                 task blocks on bp.iodone semaphore; wait for iodone;
MODIFIED TO:
    READ: proc call bp=bread(dev, blk) to get a bp->buf with valid data
               read data from bp->buf;
               then brelse(bp);
    WRITE:proc call bp=bread(dev,blk) to get a bp->buf with data from disk
               write to the buf;
               then brelse(bp) as dirty.

 3. hd_rw(READ, sector, bp->buf, 2) reads 2 sectors into bp's buf[];
    The caller to hd_rw() writes sector and commands and immediately returns;
      If it's called by a task, the task will block awaitng iodone
      if it's called by hd interrupt handler, it returns without BLOCKING.

    Since proc is blocked, the hd interrupt handler must read each sector on 
    an interrupt. When 2 sectors are read, it V(bp.iodone) to wakeup the proc.
 
4. On write: all disk writes are for dirty buffers as ASYNC. The operation is
             issued by either the calling proc or the hd interrupt handler.
             In either case, it writes ONE sector and returns.
             The hd interrupt handler will write the second sector.
             When the write completes (2 sectors), the hd interrupt handler 
             issued brelse() to release the ASYNC write buffer.

In summary: the HD driver is now split into two halves:
   hd_rw() is called by either a proc or the hd interrupt handler on next
   bp in IOQ, and the HD interrupt handler, which continues the R/W operation 
   on each interrupt. When a R completes, it V() bp's waiting task. 
   When a W completes, it brelse() the bp since the Write is ASYNC.
******************************************************************************/

int read_port(u16 port, u16 segment, u16 *offset, int count)
{
  int i;
  for (i=0; i<count/2; i++){
    put_word(in_word(port), segment, offset++);
  }
}

//int write_port(0x1F0, getds(), bufPtr, 512);
int write_port(u16 port, u16 segment, u16 *offset, int count)
{
  int i;
  for (i=0; i<count/2; i++){
    out_word(port, get_word(segment, offset++));
  }
}

int hdhandler()
{ 
  struct hd *hp;
  struct devtab * dp;
  struct buf *bp;
  u32 sector;

  //printf("HD interrupt ");

  dp = &devtab[hd_dev];
  bp = dp->io_queue;    // IO is for this buffer  
    
  // ONE interrupt per sector read|write; transfer data via DATA port

  if (bp->opcode==HD_READ){

     if (check_error() != OK){
       goto out;
     }

     read_port(0x1F0, getds(), bufPtr, 512);
     bufPtr += 512;      
 
  }

  else{  // HD_WRITE 
    // all disk writes are ASYNC, so no task to wakeup, just release buffer
     if (ICOUNT > 1){
       unlock();   // allow interrupts
       while ((in_byte(0x1F7) & (0x80 | 0x08)) != 0x08);
         write_port(0x1F0, getds(), bufPtr, 512);
         bufPtr += 512;      
     }
     // check for any error
     if (check_error() != OK){
       goto out;
     }
  }

  ICOUNT--;

  if (ICOUNT==0){
    
    // take bp out of devtab io_queue

    dp->io_queue = bp->next_free;
    bp->next_free = 0;
    
    // porcess the bp that caused the interrupt
    if (bp->opcode == HD_READ){
       bp->valid = 1;
       V(&bp->iodone);           // wakeup waiting task
    }
    else{                        // all writes are ASYNC
      bp->async = 0;             // turn off ASYNC write flag
      brelse(bp);
    }

    if (dp->io_queue){           // if IO-queue not empty, start disk IO
       bp = dp->io_queue;
       sector = devtab[bp->dev].start_sector + 2*bp->blk;
       bufPtr = bp->buf;
       hd_rw(bp->opcode, (u32)sector, bufPtr, 2);
    }
  }
 out:
  out_byte(0xA0, 0x20);   // enable 8259
  out_byte(0x20, 0x20);
}

// read/write nsectors of drive from sector to rbuf[]
/*************************************************************************
                            7     6     5    4    3   2   1   0
 status register : 0x1F7 = BUSY  REDY FAULT SEKC  RQ  CO IDX ERR ==> errors in
 error  register : 0x1F1 = BLKE  DERR   (etc)

 To output a command:
    1. read status REG : until not_BUSY && READY 
    2. write command
 
 To check for error:
    Read status REG, must be not_BUSY && READY 
                     then check for ERR (0x01).
**************************************************************************/ 
int check_error()
{
   int r;
   // check for any error
   r = in_byte(0x1F7); // read status REG

   // look for BUSY=0 && READY && noFAULT &&  
   if ( (r & 0x80)==0 && (r & (0x40|0x20|0x10|0x01)) != (0x40 | 0x10)){
      if (r & 0x01) in_byte(0x1F1);
      printf("HD R|W ERROR\n");
      return r;
   }
   return OK;
}

int hd_ready()
{
  // Wait until controller is ready for output; return zero if this times out.
  int retries = 100;

  while ( (in_byte(0x1F7) & (0x80 | 0x40)) != 0x40 ){
     retries--;
     if (retries <= 0)   // tune this value
        return 0;
  }
  return 1;
}

hd_rw(u16 rw, u32 sector, char *buf, u16 nsectors)
{
    u8 *cp, low, mid, hi, top;
    int i, r;

    P(&hdio);      // one proc at a time

    ICOUNT = nsectors;
    
    cp = &sector;
    low = *(cp);  mid = *(cp+1);  hi = *(cp+2); 
    top = (*(cp+3) & 0x0F) | 0xE0;

    while (!hd_ready());  //printf("1 ");

    out_byte(0x3F6, 0x08);                         // control = 0x08
    
    out_byte(0x1F2, nsectors);                         // sector count

    out_byte(0x1F3, low);                           // LBA low  byte
    
    out_byte(0x1F4, mid);                           // LBA mid  byte
    
    out_byte(0x1F5, hi);                            // LBA high byte
    
    out_byte(0x1F6, top);                         // use LBA for drive 0 or 1
    
    out_byte(0x1F7, rw);                            // READ or WRITE command

    if (rw==HD_WRITE){
       // to write data, must wait until notBUSY and (bit3) DRQ=1
       // HD will hang if write too soon !!!!!
       while ((in_byte(0x1F7) & (0x80 | 0x08)) != 0x08);

       write_port(0x1F0, getds(), buf, 512);
       buf += 512;
       bufPtr += 512;
    }
    V(&hdio);
}

/*===========================================================================*
 *			hd_reset					     * 
 *===========================================================================*/
int hd_reset()
{
  int i, r;

  // bit2 of 0x3F6 = RESET: 1 for reset, 0 for active ==> write a 1, then 0
  out_byte(0x3F6, (0x08 | 0x04));  // bit3 is always 1
  for (i = 0; i < 10; i++);

  out_byte(0x3F6, (0x08 | 0x00));  // bit3 is always 1
  for (i = 0; i < 10000 && drive_busy(); i++)
	;
  if (drive_busy()) {
	printf("HD reset: drive busy\n");
	return(ERR);
  }
  r = in_byte(0x1F1);
  if (r != 1) {
	printf("HD reset: drive error\n");
	return(ERR);
  }

  /* Reset succeeded.  Tell HD drive parameters. */

  hd_need_reset = FALSE;
  return(hd_init());
}

/*============================================================================*
 *				hd_results				      *
*============================================================================*/
int hd_results()
{
  register int r;

  r = in_byte(0x1F7);
  if ( (r & 0x80) == 0 && (r & (0x40 | 0x20 | 0x10 | 0x01)) != (0x40 | 0x10)) {
	if (r & 0x01) in_byte(0x1F1);
	return(ERR);
  }
  return(OK);
}

/*============================================================================*
 *				drive_busy				      *
*============================================================================*/
int drive_busy()
{
  // Wait until the controller is ready to receive a command or send status 

  hd_ready(); // this says HD is notBUSY && READY

  // check for bit4 = SEEK_COMPLETE ? should be bit3

  if ( (in_byte(0x1F7) & (0x80 | 0x40 | 0x10)) != (0x40 | 0x10)) {
	hd_need_reset = TRUE;
	return(ERR);
  }
  return(OK);
}

