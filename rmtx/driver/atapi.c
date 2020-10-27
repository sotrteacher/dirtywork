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
/******************* IDE ports ***********************
ide0 at 0x1F0-0x1F7, 0x3F6 at IRQ 14  vector=0x76
ide1 at 0x170-0x177, 0x376 at IRQ 15  vector=0x77
****************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

struct semaphore cd_sem;

u8 pkt[12];
u16 psegment;

int zeropkt()
{
  int i;
  for (i=0; i<12; i++)
    pkt[i] = 0;
}

int cd_init()
{
  printf("cd_init : initialize CD/DVD driver  ");
  zeropkt();
  cd_sem.value=cd_sem.queue = 0;
  printf("cd_init done\n");
}

int  cderror;
char *bufPtr;

/**************************************************
// ATA registers 171,172 are REDEFINED in ATAPI
#define CD_DATA        0x170    // data port for R/W
#define CD_ERROR       0x171    // error register
#define CD_INTREA      0x172    // Read: interrupt reason register with DRQ
#define CD_NOT_USED    0x173    // not used by ATAPI
#define CD_LBA_MID     0x174    // byte count LOW byte bits0-7
#define CD_LBA_HI      0x175    // byte count HI byte  bits 8-15
#define CD_DRIVE       0x176    // drive select
#define CD_CMD         0x177    // command : R=0x20 W=0x30
#define CD_STATUS      0x177    // status register

// interrupt reason register = 0x172
// bits 7 6 5 4 3 2  1 0  with DRQ=1 in status register
//                   - -
//                   0 1  ready to accept command packet bytes
//                   1 0  data to host    (CDROM)
//                   0 0  data from host  (write data)
// status register = 0x177
*******************************************************/   

/*************
                   7     6     5    4    3   2   1   0
 status : 0x177 = BUSY  REDY FAULT SEKC DRQ  CO IDX ERR ==> errors in
***********/

int cd_busy()
{
  return (in_byte(0x177) & 0x80);
}

int cd_ready()
{
    return (in_byte(0x177) & 0x40);
}

int cd_DRQ()
{
  return (in_byte(0x177) & 0x08);
}

int cdhandler()
{ 
  u8  reason, status, err;
  u16 byteCount;

  //printf("CD ROM interrupt: ");

  // read int reason register and status DRQ: 
  reason = in_byte(0x172) & 0x03;
  status = in_byte(0x177);
  err = in_byte(0x171);

  //printf("reason=%x  status=%x  err=%x\n", reason, status, err);

  if (status & 0x01){
    // status.ERROR bit on ==> set cderror flag
    cderror = 1;
    V(&cd_sem);      // wakeup proc

    /*******************
    if (err & 0x08){ // err=xxxxMyyy : M=need media change => ignore
      printf("media change error\n");
    } 
    printf("CD error : status=%x  err=%x\n", status, err);
    goto out;
    *******************/
  }
 
  if ((reason==0x01) && (status & 0x08)){
    //printf("CD ready for commands\n");
    goto out;
  }

  if ((reason==0x02) && cd_DRQ() && !cd_busy()){
     //printf("CD data ready for PIO ");
     // read byte-count in 175 174
     byteCount=(in_byte(0x175) << 8 ) | (in_byte(0x174));

     //printf("byteCount=%d\n", byteCount);
     /********** read port bytes to Uspace *****************/
     read_port(0x170, psegment, bufPtr, byteCount);
     bufPtr += byteCount;
     // printf("after PIO\n");    
     goto out;
  }

  if (reason==0x03 && !(status & 0x08)){
    //printf("final CD interrupt: status=%x : V(cd_sem)\n", status);
    V(&cd_sem);
  }

out:
  out_byte(0xA0, 0x20);   // enable 8259
  out_byte(0x20, 0x20);
}

// read/write nsectors of drive from sector to rbuf[]

/*************************************************************************
                            7     6     5    4    3   2   1   0
 status register : 0x1F7 = BUSY  REDY FAULT SEKC DRQ  CO IDX ERR ==> errors in
 error  register : 0x1F1 = BLKE  DERR   (etc)
**************************************************************************/ 
int cd_error()
{
   int r;
   // check for any error
   r = in_byte(0x177); // read status REG

   // look for BUSY=0 && READY && noFAULT &&  
   if ( (r & 0x80)==0 && (r & (0x40|0x20|0x10|0x01)) != (0x40 | 0x10)){
      if (r & 0x01) in_byte(0x171);
      printf("CD RD ERROR\n");
      return r;
   }
   return 0;         // return 0 for OK
}

/******** syscall entry to getSector **********/
int kcdSector(int y, int z, int w, int ww)
{
 // assemble x,y as sector# z as buf, w as nsector 
  u32 sector;
  sector = (u32)z;
  sector = (sector << 16) + (u32)y;
  return getSector(sector, w, ww);
}



/*********************************************************************
                        Eevent Sequence:
1. host polls for BSY=0,DRQ=0; then write to feature,byteCount,Drive Regs
                                             171     174-175   176   
2. host write packet command 0xA0 to command reg.177
3. device sets BSY, prepares for command packet transfer

4. device: set (IO,coD) to 01 and assert DRQ, deassert BSY (some will INTRQ)

5. host: detecting DRQ, write 6 command words to Data Reg=170 
6. device: clear DRQ, set BSY, read feature&cyteCount Regs, then prepare DT
7. device: put byte count into regs, set IO,coD=10,DRQ=1 BSY=0 ==>INTRQ
8. host  : intr: read DRQ (DRQ=0 ==> device has terminated the command) 
             DRQ=1 ==> PIO data. (intr turn off by reading status Reg).
9. device: clears DRQ=0 (if more data: BSY=1==> repeat 7 to 9)
10.final status by device to status reg. set coD IO DRDY=1 ==> INTRQ, BSY,DRQ=0
11.host: intr & DRQ=0 => read status Reg & error Reg.
*********************************************************************/
int getSector(u32 sector, char *buf, u16 nsector)
{
    int i; 
    u16 *usp;
    u8  *cp;

    //printf("getSector: sector=%l\n", sector);    

    out_byte(0x376, 0x08);        // enable drive interrupt

    cderror = 0;  // clear error flag
    bufPtr = buf;    
    cd_sem.value = cd_sem.queue = 0;  // initialize cd_sem to 0

    /****************  read 10 packet:******************************
                  0   1  | 2 3 4 5| 6 |7 8 9 | 10 11|
          .byte  0x28 0  |  LBA   | 0 | len  |  0  0|
    ***************************************************************/

    // create packet: read(sector, buf)==> sector into into packet.lba[4]:
    // low byte = MSB  hi byte = LSB 
    zeropkt();
    psegment = running->uss;   // caller's segment

    cp = (u8 *)&sector;

    pkt[0] = 0x28;   // opcode = READ 10

    pkt[5] = *cp;
    pkt[4] = *(cp+1);
    pkt[3] = *(cp+2);
    pkt[2] = *(cp+3);

    pkt[7] = 0;              // high byte of sectorCount
    pkt[8] = nsector;        // pket[7][8]=# of sectors to READ !!!

    // nsector not in 0x174-175, which is the # of bytes host needs AND 
    // after DRQ it's the acutal # of bytes drives has returned for PIO.
    
    // 1. poll for not busy then ready:
    out_byte(0x376, 0x08);   // software reset?   

    out_byte(0x176, 0x00);   // device = 0 = master

    while (cd_busy() || cd_DRQ());
 
    // 2. write 0 to feature register: PIO, not DMA
    out_byte(0x171, 0);  // feature REG = PIO (not DMA) 
 
    /* host:  needed byte_count = nsector*2048 bytes */
    out_byte(0x174, (nsector*2048) & 0xFF); // Low byte
    out_byte(0x175, (nsector*2048) >> 8);   // high byte

    // 3.write 0xA0 to command reg 177
    //printf("write 0xA0 to cmd register\n");

    out_byte(0x177, 0xA0);

    // wait until notBUSY, READY and DRQ is on
    while (cd_busy() || !cd_DRQ());

    // printf("writing CD commands\n");
    //printf("write 6 words of packet to 170\n");
    usp = &pkt;
    cp = usp;
    for (i=0; i<6; i++){
       out_word(0x170, *usp);
       usp++;
    }

    //printf("process blocks for final status interrupt\n");
    P(&cd_sem);
    if (cderror) return -1;
    return 0;        // each getSector() return CD sector size
}

char line[64], *cmd, *filename;

int do_cmd(u16 cmd, u16 v)
{
    int i;
    u16 *usp;
    u8 *cp;

    zeropkt();

    cderror = 0;
    cd_sem.value=cd_sem.queue=0;

    pkt[0] = cmd;
    pkt[4] = v;
    
    //(1). poll for not busy then ready:
    out_byte(0x376, 0x08);        // enable drive interrupt

    out_byte(0x176, 0);           // device = 0 = master

    while (cd_busy() || !cd_ready() || cd_DRQ());

    // 2. write 0 to 0x171 for PIO mode
    out_byte(0x171, 0);  // feature REG = PIO (not DMA)

    // write 0 as needed byte_count to 0x174-175
    out_byte(0x174, 0);  // Low byte
    out_byte(0x175, 0);   // high byte

    // 3. write packet command 0xA0 to command reg 0x177
    //printf("write 0xA0 to cmd register\n");
    out_byte(0x177, 0xA0);

    while (cd_busy() || !cd_ready() || !cd_DRQ());  // wait for DRQ

    //printf("write 6 words of packet to 170\n");
    usp = &pkt;
    cp = usp;

    for (i=0; i<6; i++){
       out_word(0x170, *usp);
       usp++; 
    }
    P(&cd_sem);
    return cderror;
}

