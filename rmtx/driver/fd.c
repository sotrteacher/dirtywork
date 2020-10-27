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
/********************** KCW's fd.c : 6-4-06 *********************************
 fd.c is the final version of my floppy disk driver for MTX.
 Main references are fdc.txt and Minix's floppy.c but with my own ways of
 interacting with timer and dealing with (rare) cases of no fd interrupts.
***************************************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h" 

/* I/O Ports used by floppy disk controller */
#define DOR            0x3F2	/* motor drive control bits */
#define FD_STATUS      0x3F4	/* floppy disk controller status register */
#define FD_DATA        0x3F5	/* floppy disk controller data register */
#define FD_RATE        0x3F7	/* transfer rate register */

/* Status registers returned as result of operation. */
#define ST0             0x00	/* status register 0 */
#define ST1             0x01	/* status register 1 */
#define ST2             0x02	/* status register 2 */

#define ST_CYL          0x03	/* slot where controller reports cylinder */
#define ST_HEAD         0x04	/* slot where controller reports head */
#define ST_SEC          0x05	/* slot where controller reports sector */
#define ST_PCN          0x01	/* slot where controller reports present cyl */

/* Fields within the I/O ports. */
/* Main status register. */
#define CTL_BUSY        0x10	/* bit is set when read or write in progress */
#define DIRECTION       0x40	/* bit is set when reading data reg is valid */
#define MASTER          0x80	/* bit is set when data reg can be accessed */

/* Digital output port (DOR). */
#define MOTOR_MASK      0xF0	/* these bits control the motors in DOR */
#define ENABLE_INT      0x0C	/* used for setting DOR port */

/* ST0. */
#define ST0_BITS        0xF8	/* check top 5 bits of seek status */
#define TRANS_ST0       0x00	/* top 5 bits of ST0 for READ/WRITE */
#define SEEK_ST0        0x20	/* top 5 bits of ST0 for SEEK */

/* ST1. */
#define BAD_SECTOR      0x05	/* if these bits are set in ST1, recalibrate */
#define WRITE_PROTECT   0x02	/* bit is set if diskette is write protected */

/* ST2. */
#define BAD_CYL         0x1F	/* if any of these bits are set, recalibrate */

/* Floppy disk controller command bytes. */
#define FD_SEEK         0x0F	/* command the drive to seek */
#define FD_READ         0xE6	/* command the drive to read */
#define FD_WRITE        0xC5	/* command the drive to write */
//#define FD_READ         0xC6	/* command the drive to read */
//#define FD_WRITE        0xC5	/* command the drive to write */
#define FD_SENSE        0x08	/* command the controller to tell its status */
#define FD_RECALIBRATE    0x07	/* command the drive to go to cyl 0 */
#define FD_SPECIFY      0x03	/* command the drive to accept params */
#define FD_READ_ID      0x4A	/* command the drive to read sector identity */
#define FD_FORMAT       0x4D	/* command the drive to format a track */

/* Parameters of the disk drive. */
#define SECTOR_SIZE      512 /* physical sector size in bytes */
#define NR_HEADS           2	/* two heads */
#define NR_SECTORS        18    // 18 sectors per track
#define DTL             0xFF	/* determines data length (sector size) */
#define SPEC2           0x02	/* second parameter to SPECIFY */

/* Error codes */
#define ERR_SEEK          -1	/* bad seek */
#define ERR_TRANSFER      -2	/* bad transfer */
#define ERR_STATUS        -3	/* something wrong when getting status */
#define ERR_RECALIBRATE     -4	/* recalibrate didn't work properly */
#define ERR_WR_PROTECT    -5	/* diskette is write protected */
#define ERR_DRIVE         -6	/* something wrong with a drive */
#define ERR_READ_ID       -7	/* bad read id */

// DMA  registers
#define DMA_ADDR       0x004	/* port for low 16 bits of DMA address */
#define DMA_TOP        0x081	/* port for top 4 bits of 20-bit DMA addr */
#define DMA_COUNT      0x005	/* port for DMA count (count =  bytes - 1) */
#define DMA_M2         0x00C	/* DMA status port */
#define DMA_M1         0x00B	/* DMA status port */
#define DMA_INIT       0x00A	/* DMA init port */
#define DMA_RESET_VAL   0x06

/* DMA channel commands. */
#define DMA_READ        0x46	/* DMA read opcode */
#define DMA_WRITE       0x4A	/* DMA write opcode */

/* General Error codes */
#define ERR		  -1	/* general error */

/* Miscellaneous. */
#define MAX_TRIES          5	/* how often to try rd/wt before quitting */
#define MAX_RESULTS        7	/* max number of bytes controller returns */
#define MAX_FD_RETRY     100	/* max # times to try to output to FD */
#define UNCALIBRATED       0	/* drive needs to be recalibrated at next use */
#define CALIBRATED         1	/* no calibration needed */

#define OK    0
#define TRUE  1
#define FALSE 0

#define NR_DRIVES          1	/* redefine this if more than 1 drive */

/***************** in type.h ******************
struct floppy {		
  u16 opcode;	
  u16 cylinder;	
  u16 sector;	
  u16 head;		
  u16 count;		
  u16 address;	

  char results[MAX_RESULTS];
  char calibration;		
  u16  curcyl;		

} floppy[NR_DRIVES];
**********************************************/

struct floppy floppy[NR_DRIVES];		

int motor_status;	// current motor status flag: in 4 high bits
int fd_motor_off;       // how many timer ticks to turn off FD motor

int need_reset;		// set to 1 when FD controller needs reset

struct semaphore fdio;  // for procs to do FD I/O one at a time
struct semaphore fdsem; // for proc  to wait for FD interrupts

int tout, fd_timeout;   // for (rare) missing FD interrupt after a rW cmd

int fd_init()
{
  struct floppy *fp;

  printf("fd_init, ");

  fdio.value = 1; fdsem.queue = 0;
  fdsem.value = fdsem.queue = 0;

  fp = &floppy[0];	
  fp->curcyl = 0;		
  fp->calibration = 0;	
  need_reset = 0;

  /***************************************************************************
  1. Specify parameters to drive: 
       cmd=0x03; 2 parameters (standard rate, etc); no result; no interrupt;

  2. Sense: MUST sense to get status after a Reset, Seek, Recalibrate command
            that generates an interrupt 
       cmd=0x08; no parameter; first result byte in ST0, 2nd byte=CYL

  3. Recalibrate:
       cmd=0x07; parameter=drive#; no result but interrupt

  4. Seek:
       cmd=0x0F;parameter=head<<2 | drive#, cylinder#; no result but interrupt

  5. Read/Write:
       8 command bytes;
       Results bytes: 1    2    3    4     5       6      7
                     ST0  ST1  ST2  cyl endHead endSector 2
  **************************************************************************/

  /************************  FD DOR bits **********************************
  //             7 6 5 4  3     2     1    0
  // DOR bits =  D C B A IRQ !RESET drive_slect
  // A:drive  =  0 0 0 1  1    1      0    0 
  // turn off A:motor, DOR = 0000 1100 => with DMA_INT and NOT_reset
  // FD doc says write to DOR does not generate interrupt, also no results
  *************************************************************************/
  out_byte(DOR,  0x0C);  // DOR = 0000 1100
  motor_status = 0x0C;   // set motor_status flag to 0x0C; (A: is off) 

  // sepcify paramters to FDC; can do these without motor running
  fd_out(FD_SPECIFY);
  fd_out(0xAF);
  fd_out(SPEC2);
  out_byte(FD_RATE, 0);
}

int fdhandler()
{
  // printf("FD interrupt ");
  V(&fdsem);         // all actions are in fdrw(), just unblock the process
  out_byte(0x20,0x20);
}

#define INT2_CTL        0xA0	/* I/O port for second interrupt controller */
#define INT2_MASK       0xA1	/* setting bits in this port disables ints */

#define CYL_SIZE        36
#define TRK_SIZE        18

int fdrw(u16 rw, u16 blk, char *addr)
{
  struct floppy *fp;
  int r, tries;
  
  P(&fdio);                  // one process does fd_rw() at a time
 
  motor_on();                // turn on motor if needed
  
  lock();
    fd_motor_off = 120;      // renew ticks for timer to turn motor off
  unlock();

  fp = &floppy[0];
  fp->opcode = rw;

  r = (2*blk) % CYL_SIZE;
  fp->cylinder = (2*blk) / CYL_SIZE;
  fp->head = r / TRK_SIZE;
  fp->sector = (r % TRK_SIZE) + 1;     // sector counts from 1, NOT 0

  fp->count = BLOCK_SIZE;
  fp->address = addr;

  tries = 0;
  while (tries++ <= MAX_TRIES) { // try 5-10 times per RW, quite arbitrary!

        // First check to see if FDC needs reset; the variable need_reset 
        // may be set in many places, each returns -1 to retry here
	if (need_reset){
	   // printf("fd reset\n"); 
           fd_reset();
        }
	// May also need to recalibrate, especially follwoing a reset
        if (fp->calibration == UNCALIBRATED)
           if (recalibrate(fp) != OK) continue;   // retry loop again

        // Seek to the correct cynlinder if needed
        if (fp->curcyl != fp->cylinder)
	  if (seek(fp) != OK) continue;         // retry loop again

	// Set up DMA controller
	setup_dma(fp);

	// write commands to FDC to start RW
        r = commands(fp);

        if (r == OK || r == ERR_WR_PROTECT) 
           break;	
  }
  V(&fdio);                                      // unlock fdio semaphore 
  return(r == OK ? fp->count : ERR);
}

int motor_on()
{
  if (motor_status & 0x10)           // 0001 0000: motor already running
     return;                         // ---A ----: A: drive ON ?

  motor_status = 0x1C;               // motor ON status=0001 INT,!RESET,0,0
  out_byte(DOR, motor_status);       // write to DOR

  lock();
    fd_motor_off = 2*60;             // timer ticks to turn FD motor off
    tout = 0;                        // flag for timer to count down
    fd_timeout = 30;                 // ticks to wait for motor to spin up
  unlock();

  P(&fdsem);                         // wait for fd_timeout=30 ticks
}

/*===========================================================================*
 *			setup_dma       				     * 
 *===========================================================================*/
int setup_dma(struct floppy *fp)
{
  u8 mode, low_addr, hi_addr, top_addr, low_count, hi_count, *cp;
  u16 offset, count;
  u32 real_addr;

  mode = (fp->opcode == FD_READ ? DMA_READ : DMA_WRITE);
  offset = fp->address;
  count  = fp->count - 1;
  
  // MTX may use separate I/D spaces, getds() gets the true DS segment
  real_addr = (u32)((u32)( (u32)getds() << 4 ) + offset);
  cp = (u8 *)&real_addr;
  
  // DMA registers need BYTE values
  low_addr  = *(cp+0);  
  hi_addr   = *(cp+1);  
  top_addr  = *(cp+2);  

  cp = (u8 *)&count;
  low_count = *(cp+0);  
  hi_count  = *(cp+1);  

  /* Write to DMA registers. */
  out_byte(DMA_INIT, DMA_RESET_VAL); /* reset the dma controller */
  out_byte(DMA_M2, mode);	     /* set the DMA mode */
  out_byte(DMA_M1, mode);	     /* set it again */

  out_byte(DMA_ADDR, low_addr);	     /* output low-order 8 bits */
  out_byte(DMA_ADDR, hi_addr);       /* output next 8 bits */
  out_byte(DMA_TOP,  top_addr);	     /* output highest 4 bits */

  out_byte(DMA_COUNT, low_count);    /* output low 8 bits of count - 1 */
  out_byte(DMA_COUNT, hi_count);     /* output high 8 bits of count - 1 */
  out_byte(DMA_INIT, 2);	     /* initialize DMA */
}

/*===========================================================================*
 *				seek					     * 
 *===========================================================================*/
int seek(struct floppy *fp)
{
  int r;
  // Issue a SEEK and wait for interrupt.
  fd_out(FD_SEEK);		    // start issuing the SEEK command
  fd_out(fp->head << 2);            // seek parameters in byte1-2
  fd_out(fp->cylinder);             // fd cylinder
  if (need_reset) return(ERR_SEEK); // if FDC is sick, abort seek 

  // wait for SEEK interrupt
  P(&fdsem);

  // After interrupt : Check drive status. 
  fd_out(FD_SENSE);		   // SENSE FDC to make it return status
  r = fd_results(fp);		   // get FDC status bytes
  if ( (fp->results[ST0] & ST0_BITS) != SEEK_ST0) r = ERR_SEEK;
  if (fp->results[ST1] != fp->cylinder) r = ERR_SEEK;
  if (r != OK) 
	if (recalibrate(fp) != OK) return(ERR_SEEK);
  fp->curcyl = (r == OK ? fp->cylinder : -1);
  return(r);
}

/*===========================================================================*
 *				commands				     * 
 *===========================================================================*/
int commands(struct floppy *fp)
{
  // drive is now on the proper cylinder.  Read or write 1 block.

  int r, s, retries;

  /* The command is issued by writing 9 bytes to the FDC */
  fd_out(fp->opcode);	  // byte 0 = command
  fd_out(fp->head << 2);  // byte 1 = drive = 0

  fd_out(fp->cylinder);   // byte 2
  fd_out(fp->head);       // byte 3
  fd_out(fp->sector);     // byte 4

  fd_out(2);	          // byte 5 = sector size code 
  fd_out(NR_SECTORS);     // byte 6 = number of sectors 
  fd_out(0x1B);           // byte 7 = sector gap 
  fd_out(0xFF);           // byte 8 = data length

  /*************** fd_out() may raise need_rest flag to 1 *******************/
  if (need_reset) return(ERR_TRANSFER);	// if controller is sick, abort op

  // wait for FD interrupt.
  // KCW : for some reason, FD interrupt often does not come, especially
  //       during Writes. Devised the following time-out scheme to overcome 
  //       this problem and it seems to work FINE (May 30-June 2-06)

  lock();
    tout = 0;               // for timer to dec fd_timeout to 0 
    fd_timeout = 30;        // then V(&fdsem) to unblock the task here
  unlock();

  P(&fdsem);                // wait for interrupt; normally from FD handler

  if (tout){                // flag set by clock, indicating time out
    //printf("fd time out\n");
     return -1;             // return -1 to retry ==> fd reset and continue
  } 

  // Get controller status and check for errors.
  r = fd_results(fp);

  if (r != OK) return(r);

  if ( (fp->results[ST1] & BAD_SECTOR) || (fp->results[ST2] & BAD_CYL) )
	fp->calibration = UNCALIBRATED;

  if (fp->results[ST1] & WRITE_PROTECT) {
      printf("Disk is write protected.\n");
      return(ERR_WR_PROTECT);
  }
  if ((fp->results[ST0] & ST0_BITS) != TRANS_ST0) return(ERR_TRANSFER);
  if (fp->results[ST1] | fp->results[ST2]) return(ERR_TRANSFER);

  if (fp->opcode == FD_FORMAT) return(OK);

  // Compare actual numbers of sectors transferred with expected number
  s =  (fp->results[ST_CYL] - fp->cylinder) * NR_HEADS * 18;
  s += (fp->results[ST_HEAD] - fp->head) * 18;
  s += (fp->results[ST_SEC] - fp->sector);
  if (s * SECTOR_SIZE != fp->count) return(ERR_TRANSFER);
  return(OK);
}

/*==========================================================================*
 *				fd_results				    *
 *==========================================================================*/
int fd_results(struct floppy *fp)
{
  int result_nr, retries, status;
  result_nr = 0;
  retries = MAX_FD_RETRY;
  while (TRUE) {
	status = in_byte(FD_STATUS) & (MASTER | DIRECTION | CTL_BUSY);
	if (status == (MASTER | DIRECTION | CTL_BUSY)) {
	  if (result_nr >= MAX_RESULTS) break;	// too many results
             fp->results[result_nr++] = in_byte(FD_DATA);
	     retries = MAX_FD_RETRY;
	     continue;
	}
	if (status == MASTER) {	   // all read 
	  return(OK);	           // this is the only good exit
	}
	if (--retries == 0) break; // retried out
  }
  need_reset = TRUE;	   	   // controller chip must be reset
  return(ERR_STATUS);
}

/*===========================================================================*
 *				fd_out					     * 
 *===========================================================================*/
int fd_out(int val)
{
  register int retries;

  if (need_reset) return ERR_TRANSFER;	// controller not listening, return

  // may take many tries to get the FDC to accept a command.
  retries = MAX_FD_RETRY;
  while ( (in_byte(FD_STATUS) & (MASTER | DIRECTION)) != (MASTER | 0) )
    if (--retries == 0) {
	need_reset = TRUE;
	return ERR_TRANSFER;
    }
  out_byte(FD_DATA, val);
}


/*===========================================================================*
 *			 	recalibrate				     * 
 *===========================================================================*/
int recalibrate(struct floppy *fp)
{
  int r;
  motor_on();	        	
  fd_out(FD_RECALIBRATE);	
  fd_out(0);           
  if (need_reset) 
     return(ERR_SEEK);

  P(&fdsem);

  fd_out(FD_SENSE);	       // issue SENSE command to request results
  r = fd_results(fp);	     
  fp->curcyl = -1;	     
  if (r != OK ||	     
     (fp->results[ST0]&ST0_BITS) != SEEK_ST0 || fp->results[ST_PCN] !=0){
    // Recalibration failed. must be reset.
	need_reset = TRUE;
	fp->calibration = UNCALIBRATED;
	return(ERR_RECALIBRATE);
  } else { // Recalibration OK
	fp->calibration = CALIBRATED;
	return(OK);
  }
}

/*===========================================================================*
 *				reset					     * 
 *===========================================================================*/
int fd_reset()
{
  int i;
  need_reset = FALSE;
  lock();
    motor_status = 0;
    out_byte(DOR, 0);	
    out_byte(DOR, ENABLE_INT);
  unlock();

  P(&fdsem);

  fd_out(FD_SENSE);	
  fd_results(&floppy[0]);

  for (i=0; i<NR_DRIVES; i++)
	floppy[i].calibration = UNCALIBRATED;

  // Tell FDC drive parameters.
  fd_out(FD_SPECIFY);		
  fd_out(0xAF);         // step-rate and head-unload-time 
  fd_out(SPEC2);	// head-load-time and non-dma
}
