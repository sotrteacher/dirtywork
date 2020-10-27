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
/******************************************************************
                     Printer driver:
 Process and interrupt handler share a CIRCULAR buffer, with head, tail 
 pointers. To print, process calls prchar() to deposit chars into pbuf[], 
 waiting if no room, and prints the first  char if the printer is not 
 printing, Interrupt handler will  print the remaining chars in pbuf[],
 waking up the blocked process. 
 prline() is for processes to print one line at a time; not used
*******************************************************************/ 
#ifndef MK
 #include "../type.h"
#endif
#include "../extern.h"

#define NPR         1
#define PORT    0x378       // #define PORT 0x3BC  for LPT2
#define STATUS PORT+1
#define COMD   PORT+2
#define PLEN      128

struct para{
   int port;
   int printing;           // 1 if printer is printing
   char pbuf[PLEN];
   int head, tail;
   struct semaphore mutex;
   struct semaphore room;
   struct semaphore done;
} printer;                 /* printer[NPR] if many printers */

int delay()
{
   int i;
   for (i=0; i<32000; i++);
}

pr_init()
{  
   struct para *p;
   p = &printer;
   printf("pr_init %x\n", PORT);
   p->port = PORT;
   p->head = p->tail = 0;
   p->mutex.value = 1;   p->mutex.queue = 0;
   p->room.value = PLEN; p->room.queue = 0;
   p->done.value = 0;    p->done.queue = 0;

   /* initialize printer at PORT */
   out_byte(p->port+2, 0x08);   /* init */
   out_byte(p->port+2, 0x0C);   /* int, init, select on */

   enable_irq(7);
   p->printing = 0;
}

int printerReady(struct para *p)
{
   int status;
   status = in_byte(p->port+1); 
   if ((status & 0xB0) != 0x90)
      return 0;
   return 1;
}

int phandler()
{
   int status; int c;
   struct para *p = &printer;

   // uncomment to see interrupts
   //printf("printer interrupt!\n");

   status = in_byte(p->port+1);
    if ((status & 0xB0) != 0x90){       /* RARE : interrupted but not ready */ 
     //printf("printer not ready\n");
     goto out;
   }
   if ((status & 0xB0) == 0x90){       /* normal status */
       if (p->room.value == PLEN){     /* pbuf[] empty, nothing to print */
           out_byte(p->port+2, 0x0C);  /* turn off interrupts */
           V(&p->done);                /* tell task print is DONE */
           p->printing = 0;            /* is no longer printing */
           goto out;
       }
       /* p->pbuf[] not empty ==> print next char */
       c = p->pbuf[p->tail++] & 0x7F;
       p->tail %= PLEN;

       out_byte(p->port, c);
       out_byte(p->port+2, 0x1D);
       // pdelay();
       out_byte(p->port+2, 0x1C);
       V(&p->room);
       goto out;
   }
   /* abnormal printer status: should handle it but ignored here  */
 
out:
   out_byte(0x20, 0x20);        /* re-enable the 8259 */
}
/************** Upper half driver ************************/
int prchar(char c)
{
    struct para *p = &printer;
    lock();
    if (p->room.value == PLEN){ // print first char and enable interrupt
         while(!printerReady(p));      
         out_byte(p->port, c);
         out_byte(p->port+2, 0x1D);
         //pdelay();
         out_byte(p->port+2, 0x1C); // turn on printer interrupt
         p->printing = 1;
         goto out;
    }

    P(&p->room);
    p->pbuf[p->head++] = c;
    p->head %= PLEN;
 out:
   unlock();
}


int prline(line) char *line;
{
    struct para *p = &printer;
    P(&p->mutex);              /* one process prints LINE at a time */      
      while (*line)
	prchar(*line++);
      //P(&p->done);          /* wait until pbuf[ ] is DONE */
    V(&p->mutex);             /* allow another process to print */   
}   

int upline(uline) char *uline;
{
  // for syscall to print line from Umode 
}

