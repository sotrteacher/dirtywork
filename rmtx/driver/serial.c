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

#define INT_CTL   0x20
#define ENABLE    0x20

#define NULLCHAR     0
#define BEEP         7
#define BACKSPACE    8
#define ESC         27
#define HOME        30
#define SPACE       32
#define PLUS        43
//#define BS          0x08
#define BS          0x7F

#define INBUFLEN    64
#define OUTBUFLEN   64
#define EBUFLEN     10
#define LSIZE       64

#define NR_STTY      2

/* register offsets from serial port base */
#define DATA         0   /* Data reg for Rx, Tx   */
#define DIVL         0   /* When used as divisor  */
#define DIVH         1   /* to generate baud rate */
#define IER          1   /* Interrupt Enable reg  */
#define IIR          2   /* Interrupt ID rer      */
#define LCR          3   /* Line Control reg      */
#define MCR          4   /* Modem Control reg     */
#define LSR          5   /* Line Status reg       */
#define MSR          6   /* Modem Status reg      */

/**** serial tty structures ****/
/******************* in type.h *************************
typedef struct stty {
   // input buffer 
   char inbuf[INBUFLEN];
   int inhead, intail;
   struct semaphore inchars, inlines, mutex;

   // output buffer 
   char outbuf[OUTBUFLEN];
   int outhead, outtail;
   struct semaphore outspace;
   char kline[LSIZE];
   int tx_on;
   
   // echo buffer 
   char ebuf[EBUFLEN];
   int ehead, etail, e_count;

   // Control section 
   char echo;   // echo inputs
   char ison;   // on or off
   char erase, kill, intr, quit, x_on, x_off, eof;
   
   // I/O port base address
   int port;
   char *tty;
} TTY;
*************************************************/

STTY stty[NR_STTY];

/* put/get char by polling serial port's Line Status register */
int bputc(int port, int c)
{
  while ((in_byte(port+LSR) & 0x20) == 0);
  out_byte(port+DATA, c);
}

int bgetc(int port)
{
  while ((in_byte(port+LSR) & 0x01) == 0);
  return (in_byte(port+DATA) & 0x7F); 
}

int enable_irq(u8 irq_nr)
{
   out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));
}

char *ttyS0="/dev/ttyS0";
char *ttyS1="/dev/ttyS1";
   
/************ serial ports initialization ***************/
int sinit()
{
  int t; 
  struct stty *tty;
  char *p = "\n\rttyS0 ready\n\r\007";
  char *q = "\n\rttyS1 ready\n\r\007";

  /* initialize stty[] and serial ports */
  for (t = 0; t < NR_STTY; t++){
      tty = &stty[t];

      /* initialize data structures and pointers */
      if (t==0){
          tty->port = 0x3F8;    /* COM1 base address */
          tty->tty = ttyS0;
      }
      else{
          tty->port = 0x2F8;    /* COM2 base address */  
          tty->tty = ttyS1;
      }
      tty->inchars.value = tty->inlines.value = 0;
      tty->inlines.queue = tty->inchars.queue = 0;
      tty->mutex.value  = 1;
      tty->mutex.queue = 0;
      tty->outspace.value = OUTBUFLEN;
      tty->outspace.queue = 0;
      tty->inhead = tty->intail = 0;
      tty->ehead =  tty->etail = tty->e_count = 0;
      tty->outhead =tty->outtail = tty->tx_on = 0;

      lock();
      out_byte(tty->port+MCR,  0x09);  /* IRQ4 on, DTR on */ 
      out_byte(tty->port+IER,  0x00);  /* disable serial port interrupts */

      out_byte(tty->port+LCR,  0x80);  /* ready to use 3f9,3f8 as divisor */
      out_byte(tty->port+DIVH, 0x00);
      out_byte(tty->port+DIVL, 12);    /* divisor = 12 ===> 9600 bauds */

      /******** term 9600 /dev/ttyS0: 8 bits/char, no parity *************/ 
      out_byte(tty->port+LCR, 0x03); 

      /* clear lingering interrupts */
      in_byte(tty->port+LSR);
      in_byte(tty->port+DATA);
      in_byte(tty->port+MSR);
/************************************************************************
  Writing to 3fc ModemControl tells modem : DTR, then RTS ==>
  let modem respond as a DCE.  Here we must let the (crossed)
  cable tell the TVI terminal that the "DCE" has DSR and CTS.  
  So we turn the port's DTR and RTS on.
************************************************************************/

    out_byte(tty->port+MCR, 0x0B);  /* 1011 ==> IRQ4, RTS, DTR on   */
    out_byte(tty->port+IER, 0x01);  /* Enable Rx interrupt, Tx off */
    unlock();
  }
  enable_irq(3);
  enable_irq(4);

  /* show greeting message */
  while (*p){
     sputc(&stty[0],*p);
     p++;
  }
  while(*q){
    sputc(&stty[1], *q);
    q++;
  }
}  

int s0handler()
{
  return shandler(0);
}

int s1handler()
{
  return shandler(1);
}
         
int shandler(int port)
{  struct stty *tty;
   int IntID, LineStatus, ModemStatus, intType, c;

   tty = &stty[port];    /* IRQ 4 interrupt : COM1 = stty[0] */

   IntID     = in_byte(tty->port+IIR);       /* read InterruptID Reg */
   LineStatus= in_byte(tty->port+LSR);       /* read LineStatus  Reg */    
   ModemStatus=in_byte(tty->port+MSR);       /* read ModemStatus Reg */

   intType = IntID & 7;  /* mask out all except the lowest 3 bits */
   switch(intType){
      case 6 : do_errors(tty);  break;   /* 110 = errors */
      case 4 : do_rx(tty);      break;   /* 100 = rx interrupt */
      case 2 : do_tx(tty);      break;   /* 010 = tx interrupt */
      case 0 : do_modem(tty);   break;   /* 000 = modem interrupt */
   }
   //   if (running->signal)
   // kpsig(&sesp);
   out_byte(INT_CTL, ENABLE); 
}


int do_errors(){}     /* ignore error and modem interrupts */
int do_modem(){}


/* enable and diable Tx interrupts */
en_tx(struct stty *tty)
{
  out_byte(tty->port+IER, 0x03);   /* 0011 ==> both tx and rx on */
  tty->tx_on = 1;                  /* turn on flag to 1 */
}

disable_tx(tty) struct stty *tty;
{ 
  out_byte(tty->port+IER, 0x01);   /* 0001 ==> tx off, rx on */
  tty->tx_on = 0;                  /* turn off flag */
}

/******** echo char to RS232 **********/
/***************
int secho(struct stty *tty, int c)
{
// insert c into ebuf[]; turn on tx interrupt
   tty->ebuf[tty->ehead++] = c;
   tty->ehead %= EBUFLEN;
   tty->e_count++;
   // if (!tty->tx_on)
        en_tx(tty);     // tx handler will flush them out
}
**************/
int secho()
{
  // no echo
}

int sesc = 0;

int do_rx(tty)          /* interrupts already disabled */
    struct stty *tty;  
{ 
  int c,i;
  c = in_byte(tty->port);  /* read the char from port */
  c = c & 0x7F;

  //printf("\nrx interrupt c="); 
  /***
  putc(c); printx(c); 
  if (c=='\r') putc('\n');
  ****/

  if (c==0x1B){  // ESC key
    sesc++;
    return;
  }
  if (sesc == 1){    // ESC [  keys ==> next char 
    if (c==0x5B){
      sesc++;
      return;
    }
  }
  if (sesc == 2){    //ESC [ 0x41 ==> cursorUP; 0x42 = cursorDown
    if (c==0x41)
      tty->inbuf[tty->inhead++] = 0x0B;  // VT
    if (c==0x42)
      tty->inbuf[tty->inhead++] = 0x0C;  // FF

    tty->inhead %= INBUFLEN;         
    V(&tty->inchars);         

    tty->inbuf[tty->inhead++] = '\n';
    tty->inhead %= INBUFLEN;         
    V(&tty->inchars);         

    sesc = 0;
    return;
  }

  if (c==3){ // Control_C
    for (i=1; i<NPROC; i++){
        if (proc[i].status != FREE && strcmp(proc[i].res->tty, tty->tty)==0)
	    proc[i].res->signal |= (1 << 2); // sh IGNore, so only children die
    }
    printf("Control_C:send #2 signal to procs\n");
    tty->inbuf[tty->inhead++] = '\n'; // force a line
    tty->inhead %= INBUFLEN;          // advance inhead 
    V(&tty->inchars);                 // let proc handle #2 when exit Kmode
    return;
  }

  if (c==BS){  
    c = '\b';  // BS and '\b' my differ!!!
  }

  if (tty->inchars.value >= INBUFLEN){
      secho(tty, BEEP);
      return;
  }

  if (c >= ' ' || c=='\r')
     secho(tty, c);

  if (c == '\r'){
    c = '\n';                      // change CR to LF 
       secho(tty,c);
  }

  tty->inbuf[tty->inhead++] = c;   // put char into inbuf[]
  tty->inhead %= INBUFLEN;         // advance inhead 

  V(&tty->inchars);          // inc inchars and wakeup sgetc()

  // printf("lines = %d\n",tty->inlines.value)
}

/*********************************************************************/

int do_tx(struct stty *tty)
{
  int c;
  //printf("serial tx interrupt\n");
  if (tty->e_count == 0 && tty->outspace.value == OUTBUFLEN){
        /* nothing to do */
        out_byte(tty->port, NULLCHAR); /* to ensure interrupt is cleared */
        disable_tx(tty);               /* turn off tx interrupt */
        return;
  }

  if (tty->e_count) {                  /* there are chars to echo */
        c = tty->ebuf[tty->etail++];  tty->etail %= EBUFLEN;
        tty->e_count--;
        out_byte(tty->port, c);       /* write the sucker out */
        return;                       /* that's all for now */
  } 

  if (tty->outspace.value < OUTBUFLEN){ /* something to output */
        c = tty->outbuf[tty->outtail++];
        tty->outtail %= OUTBUFLEN;
        out_byte(tty->port, c);
        V(&tty->outspace);
        return;
  }
}

int sgetc(struct stty *tty)
{ 
  int c;
    P(&tty->inchars);   /* wait if no input char yet */
    lock();             /* disable interrupts */
      c = tty->inbuf[tty->intail++];
      tty->intail %= INBUFLEN;
    unlock();
    return(c);
}

int sputc(struct stty *tty, char c)
{
    P(&tty->outspace);    /* wait for out-space in outbuf[] */  
    lock();               /* disalble interrupts */
        tty->outbuf[tty->outhead++] = c;
        tty->outhead %= OUTBUFLEN;
    unlock();
    /* if (!tx_on) */                     /* enable tx interrupt */      
       en_tx(tty);
    return(1);
}

int sgetline(int port, char *line)
{  
   char *p;
   int c, n;

   struct stty *tty;

   if (port==0x3F8)
      tty = &stty[0];
   if (port==0x2F8)
      tty = &stty[1];

   n = 0;      
   //P(&tty->inlines);     /* no longer needed */
   P(&tty->mutex);       /* one task at a time */
     p = line;
     while ( (c = sgetc(tty)) != '\n'){
         *p = c;
          p++; n++;      /* char count */
     }
     *p = c; *(++p) = 0;   /* an extra null char */
   V(&tty->mutex);
   return(n);
}

int sputline(int port, char *line)
{
   struct stty *tty;

   if (port==0x3F8)
      tty = &stty[0];
   if (port==0x2F8)
      tty = &stty[1];

   while (*line){
         sputc(tty, *line);
         line++;
   }
}

int usgets(int port, char *y)
{  
   int n;
   char c, *tp, *cp;
   struct stty *tty;
   char temp[64], temp2[64];

   if (port==0x3F8)
      tty = &stty[0];
   if (port==0x2F8)
      tty = &stty[1];

   tp = temp;
   while (1){
     c = sgetc(tty);
     if (c == '\n')
        break;
     *tp = c;
     tp++;
   }
   *tp = 0;
   //printi(strlen(temp));

   // cook the temp[] line
   tp = temp; cp = temp2;

   while (*tp){
     if (*tp == '\b'){
       cp--; tp++;
       continue;
     }
     if (*cp == '\n')
       break;
     *cp = *tp;
     cp++; tp++;
   }
   *cp = 0;
   //printi(strlen(temp2));

   n = 0;
   tp = temp2;
   while(*tp){
     put_ubyte(*tp, y);
     tp++; y++; n++;
   }
   put_ubyte(0, y);
   return n;
}
