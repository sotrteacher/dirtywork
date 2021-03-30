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
/************* tc.c file ******************/
#include "libepc.h"
#include "descriptor_tables.h"
#include "isr.h"
#include "monitor.h"
struct multiboot;    /* In order to avoid compiler warning */
void tswitch(void);  /* In order to avoid compiler warning */
                     /* defined in file tswitch.s */
#define SSIZE  1024

typedef struct proc{
        struct proc *next;
               int  *ksp;
               int   kstack[SSIZE];
}PROC;

PROC proc, *running;
int procSize = sizeof(PROC);

int scheduler()
{ 
  running = &proc;
}
/*
static void interrupt8_callback(registers_t regs)
{
 (void*)&regs;
}
static void interrupt13_callback(registers_t regs)
{
 (void*)&regs;
}
*/
#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ   1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT  TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0

void init_timer() {
    /* If we wanted to set a frequency. */
    
    int frequency = 1;
    //u32int divisor = 1193180 * frequency;
    //u32int divisor = 1193182 * frequency;
    u32int divisor = TIMER_FREQ/60;
    u8int l = (u8int)(divisor & 0xFF);       // TIMER_COUNT=TIMER_FREQ/60=1193182L/60 
    u8int h = (u8int)( (divisor>>8) & 0xFF );
    

    ///* But we just want the maximum divisor instead. */
    //u8int l = 0xFF;
    //u8int h = 0xFF;

    /* Command byte. */          // set timer to run continuously
    outportb(0x43, 0x36);        // (TIMER_MODE,SQUARE_WAVE)

    // Send the frequency divisor. // timer count low byte
    outportb(0x40, l);           // (TIMER0,TIMER_COUNT=TIMER_FREQ/60)
                                   // timer count high byte
    outportb(0x40, h);           // (TIMER0,TIMER_COUNT >> 8)
                                 // enable_irq(TIMER_IRQ);
                                 // enable_irq(TIMER_IRQ) code:
                                 //   lock();
                                 //   out_byte(0x21, in_byte(0x21) & ~(1 << TIMER_IRQ));
    outportb(0x21, inportb(0x21) & ~(1 << TIMER_IRQ)) ;
}

typedef union char_array_to_ushort {
 unsigned char c[2];
 unsigned short Limit; /* Limit = size - 1 */
} cha2ushort;
   
typedef union char_array_to_uint {
 unsigned char c[4];
 unsigned int Base;
} cha2uint;

/* sidt returns idt in this format */ 
typedef struct {    
  unsigned short IDTLimit;    
  unsigned short LowIDTbase;    
  unsigned short HiIDTbase; 
} IDTINFO;

typedef struct {       
  unsigned short LowOffset;       
  unsigned short selector;       
  unsigned char unused_lo;       
  unsigned char segment_type:4; //0x0E is interrupt gate       
  unsigned char system_segment_flag:1;       
  unsigned char DPL:2;          // descriptor privilege level       
  unsigned char P:1;            // present       
  unsigned short HiOffset; 
}__attribute__((packed)) IDTENTRY;

#define MAKELONG(a, b) ((unsigned long) (((unsigned short) (a)) | ((unsigned long) ((unsigned short) (b))) << 16))

#define MAX_IDT_ENTRIES 0xFF

//#define CERO_A_VEINTE
#define VEINTIUNO_A_CUARENTA
//#define CUARENTAYUNO_A_SESENTA

int main(struct multiboot *mboot_ptr)
{
  // Initialise all the ISRs and segmentation
  init_descriptor_tables();
  
//  register_interrupt_handler(8,(isr_t)&interrupt8_callback);
//  register_interrupt_handler(13,(isr_t)&interrupt13_callback);
  ClearScreen(0x07);
  /*SetCursorPosition(0,0);
  PutString("PROGRAM 1\r\n");
  PutString("IDTR done?\r\n");*/
    asm volatile("int $0x3");
    asm volatile("int $0x4");
  /*PutString("\r\n------------------------------------\r\n");*/
  int s_ushort = sizeof(unsigned short);
  monitor_write("sizeof(unsigned short) = ");
  monitor_write_dec(s_ushort);
  monitor_put('\n');
  char regcont[6]; 
  __asm__ __volatile__ ("sidt %0" : : "m" (regcont)) ;
  cha2ushort c2ush;
  c2ush.c[0] = regcont[0];
  c2ush.c[1] = regcont[1];
  monitor_write("Limit = ");
  monitor_write_hex(c2ush.Limit);//Limit = 0x7ff, Limit + 1 = 0x800 = 2**11 = 2048, (2**11)/(2**8)=2**3= 8 bytes 
  monitor_put('\n');             //i.e., 2**8=256 entradas de 8 bytes cada una.
  cha2uint c2ui;
  c2ui.c[0] = regcont[2];
  c2ui.c[1] = regcont[3];
  c2ui.c[2] = regcont[4];
  c2ui.c[3] = regcont[5];
  monitor_write("Base = ");
  monitor_write_hex(c2ui.Base);
  monitor_put('\n');
  IDTINFO   idt_info;    // this structure is obtained by                                            
                         // calling STORE IDT (sidt)       
  IDTENTRY* idt_entries; // and then this pointer is                                                     
                         // obtained from idt_info       
  unsigned long count;   // load idt_info       
  __asm__ __volatile__ ("sidt %0" : : "m" (idt_info)) ;
  idt_entries = (IDTENTRY*)   MAKELONG(idt_info.LowIDTbase,idt_info.HiIDTbase);
  monitor_write("idt_entries = ");
  monitor_write_hex((u32int)idt_entries);
  monitor_put('\n');
  //for(count = 0;count <= MAX_IDT_ENTRIES;count++)       {             
#ifdef CERO_A_VEINTE
  for(count = 0;count <= 20;count++)       {             
#endif
#ifdef VEINTIUNO_A_CUARENTA
  for(count = 21;count <= 40;count++)       {             
#endif
#ifdef CUARENTAYUNO_A_SESENTA
  for(count = 41;count <= 60;count++)       {             
#endif
    char _t[255];             
    IDTENTRY *i = &idt_entries[count];             
    unsigned long addr = 0;             
    addr = MAKELONG(i->LowOffset, i->HiOffset);             
    //sprintf(_t,"Interrupt %d: ISR 0x%08x",count,addr);             
    monitor_write("Interrupt ");       
    monitor_write_dec((u32int)count);
    monitor_write(": ISR ");       
    monitor_write_hex(addr);
    monitor_put('\n');
  }

  
  init_timer();
  __asm__ __volatile__("sti");
//
//  DWORD32 timeout;
//  timeout = Now_Plus(1);
//  PutString("\r\ntimeout = ");
//  PutUnsigned(timeout,10,0);
//  PutString("\r\n");
//  while (Now_Plus(0) < timeout) { /*do nothing*/ }
//  PutString("\r\n------------------------------------\r\n");
//
  return 0;
}/*end main()*/


