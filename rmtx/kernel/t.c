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

#ifndef MK
#include "../type.h"
#endif
#include "../extern.h"

PRES pres[NPROC];
PROC proc[NPROC+NTHREAD], *running, *freeList, *readyQueue, *sleepList;
PROC *tfreeList;   // threads PROCs freeList

int procsize = sizeof(PROC);
int nproc = 0;
int ntthreads = NTHREAD;

// proc switch flag:  when a proc is scheduled to run, set to 0. When wakeup/V 
// a proc with higher priority, set to 1: When a proc returns to Umode check 
// sw_flag: tswitch if sw_flag =1
int sw_flag;               
int boot_dev;                // boot HD partition; passed in by booter
int HD;                      // boot partition# (3,4,5,..) as index to devtab[ ]

extern MINODE *root;         // in mount_root(); root minode[ ] pointer  
extern int goUmode();        // in ts.x for proc to goUmode
extern int mutexuse[10];

//  function types
int divide(), trap(), int80h(), s0inth(), s1inth(), kbinth(), tinth();
int hdinth(), fdinth(), pinth();
int HDinth();
int set_vec();
int cdinth();

/***** remap IRQ0-15 vectors to 0x20-0x2F ********/
int remap_IRQ( ) 
{
  out_byte(0x20, 0x11);
  out_byte(0xA0, 0x11);
  out_byte(0x21, 0x20);
  out_byte(0xA1, 0x28);
  out_byte(0x21, 0x04);
  out_byte(0xA1, 0x02);
  out_byte(0x21, 0x01);
  out_byte(0xA1, 0x01);
  
  /* leave all IRQ enabled */ 
  out_byte(0x21,0x00);
  out_byte(0xA1,0x00);
}

int init(int pno)
{
   int i,j,fd; 
   PROC *p = &proc[0];
 
   remap_IRQ();
  
   for (i=0; i<NPROC+NTHREAD; i++){
       p->pid = i;
       p->status = FREE;
       p->next = p+1;
       p->res->name[0] = 0;
       p->proc = p;       // each PROC points to itself
       for (j=0; j<NFD; j++)
            pres[i].fd[j] = 0;
       pres[i].signal = 0;
       for (j=0; j<NSIG; j++)
	    pres[i].sig[j] = 0;
       pres[i].segment=0;

       if (i<NPROC){
           p->type = PROCESS;
           p->res = &pres[i];
           p->res->tcount = 1;
           p->res->uid = p->res->gid = 0;
           p->res->message.value=0; p->res->message.queue=0;
           p->res->mlock.value = 1; p->res->mlock.queue = 0;
           p->res->mqueue = 0;
       }
       else{
	   p->type = THREAD;
           p->res = 0;
       }
       p++;
   }

   freeList = &proc[0]; // freelist    
   proc[NPROC-1].next = 0;
   
   tfreeList = 0;
   if (NTHREAD)
       tfreeList = &proc[NPROC];
   proc[NPROC+NTHREAD-1].next = 0;

   //printList("free ", freeList);
   //printList("tfree", tfreeList);

   for (i=0; i<10; i++)
     mutexuse[i] = 0;

   readyQueue = 0;      // readyQueue
   sleepList = 0;       // sleepList
   root = &minode[0];   // root INODE is loaded by mount_root() in fs_init()

   running = get_proc(&freeList); // create P0, which runs only in Kmode
   running->next = 0;
   running->res->uid = 0;
   running->pri = 0;   // P0 has lowest priority
   running->status = READY;
   running->res->cwd = root; 
   running->res->cwd->refCount++;
   running->res->signal = 0;    

   running->time = 0;
   running->inkmode = 1;

   //boot_dev = root_dev & 0x00FF; // HD boot partition, passed in as rootdev
   set_vec(80, int80h);     // syscall vector
   /********************
   set_vec(8, tinth);       // set timer vector and start timer LAST
   set_vec( 9, kbinth);     // KBD vector
   set_vec(11, s1inth);     // serail port vectors
   set_vec(12, s0inth);
   set_vec(14, fdinth);   
   set_vec(15, pinth);
   set_vec(0x76, hdinth);   // HD and FD interrupt vectors
   set_vec(0x77, cdinth);   // CDROM interrupt vector
   *****************/

   set_vec(0x20, tinth);    // set timer vector and start timer LAST
   set_vec(0x21, kbinth);   // KBD vector
   set_vec(0x23, s1inth);   // serail port vectors
   set_vec(0x24, s0inth);
   set_vec(0x26, fdinth);   // FD
   set_vec(0x27, pinth);    // printer
   set_vec(0x2E, hdinth);   // HD interrupt vector
   set_vec(0x2F, cdinth);   // CDROM interrupt vector

   set_vec(0,  divide);     // trap vectors; most are not used
   for (i=1; i<8; i++)
       set_vec(i, trap);

   mem_init();              // initialize memory manager: free memory = Kernel_
   pipe_init();             // initialize PIPEs in Kernel
   binit();                 // initialize I/O buffers
   kbinit();                // initialize KBD driver
   pr_init();               // initialize printer
   sinit();                 // initialize serial ports
   fd_init();               // initialize FD driver  
   hd_init();               // initialize HD driver
   cd_init();               // initialize CD driver

   // show devtab[ ] set up by hd_init(): 0=FD 1=HD1 2=HD2, etc
   for (i=0; i<NDEV; i++){
      if (devtab[i].size)
         printf("%d [%l %l]\n", i, devtab[i].start_sector, devtab[i].size);
   }
   HD = pno;
   fs_init(pno);               // initialize FS and mount_root()
   mbuf_init();
   printf("init complete\n");
}

int set_vec(u16 vector, u16 addr)
{
    put_word(addr, 0, vector<<2);
    put_word(0x1000,0,(vector<<2) + 2);
}


// dsize, bsize of MTX from ts.x; use dsize as a word pointer 

extern u32 start_sector;
extern u16 scyl, cylsize;

int clrbss(u16 *dsize, u16 bsize)
{
  int i;
  for (i=0; i<bsize/2; i++){
    *dsize = 0;
    dsize++;
  }
}

/*************************************************************************
 In addition to dsize and tsize, booter may also pass in MTX's partition# 
 and startSector. It is decided NOT to do so but let MTX's HD_init() read 
 MBR to get the partition info (so that MTX may mount other HD partitions).
 The only thing MTX must know is its boot_dev or partition#. It's hard coded 
 here as boot_dev=3 ==> ASSUMING MTX is installed in partition 3 of HD
***********************************************************************/
u8 btime[8]; // centry, year, month, day, bhr, bmin, bsec, bpad; 

int main(u16 bootdev, u16 dsize, u16 bsize)
{
   int i, pid, status;  char c, d;

   // MUST CLEAR bss first before initializing any globals
   // bss was cleared in ts.x when starting up

   vid_init();   //  initialize console display globals; MUST do this first.

   printf("\nWelcome to MTX in 16-bit real mode\n");
   printf("initializing ....\n");
   boot_dev = bootdev;
   HD = bootdev & 0xFF;     // only one-byte value
   printf("bootdev=%x dsize=%u bsize=%u HD=%d\n", bootdev, dsize, bsize, HD);
   
   biostime();              // call BIOS get date and time into u8 btime[8];
   ptime(btime);            // print date and time
 
   init(HD);                // initialize kernel data structs and P0

   kfork("/bin/init");      // create P1 with Umode image /bin/init
   
   lock(); 
      timer_init();         // reason: myfork() must load Uimage, which needs
                            // interrupts enabled==> P0 may block ==> no proc 
                            // available to handle timer interrupts
      printf("ready to run init process P1\n");
      running->pri = 0;

      while(1){             // P0 resumes when ALL others are SLEEP|BLOCKed
        unlock();           // If so, P0 loops here until readyQ not empty
        if (readyQueue){
	    schedule(running); 
            tswitch();      // tswitch() to a ready proc in readyQ
        }
        else
          idle();
     }
}

int nextrun()
{
  // needed by P0 when it P(sem) in hd.c to block => readyQ=empty
  if (running->pid==0 && running->status != READY){ // if P0 is blocked
      unlock();             // MUST unlock!! else HD interrupts will NOT occur.
      while(readyQueue==0); // loop until P0 is V-ed into ReadyQueue
  }
  running = dequeue(&readyQueue);
  running->time = 100;
  running->cpu = 0;
  sw_flag = 0;
}

int schedule(PROC *p)
{
  if (p->status==READY)
     enqueue(&readyQueue, p);
  if (p->pri > running->pri)
    sw_flag = 1;
}

int reschedule()
{
  if (sw_flag){
    ///******* uncomment to see every process switching *********************
    //printf("%d[%d] -> %d[%d]\n", running->pid, running->pri, readyQueue->pid,
    //    readyQueue->pri);
    //***********************************************************************/
    schedule(running);     
    tswitch();
  }
}
