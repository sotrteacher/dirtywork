// pit_timer.c -- Initialises the PIT, and handles clock updates.
//            Written for JamesM's kernel development tutorials.

#include "timer.h"
#include "isr.h"
#include "monitor.h"

u32int tick = 0;

static void timer_callback(registers_t regs)
{
  static int TICK = 0;
  tick++;
    //monitor_write("Tick: ");
    //monitor_write_dec(tick);
    //monitor_write("\n");
  TICK++;
  TICK %= 60;
  if (TICK == 0) {  // At each second
    monitor_SetCursorPosition(0,1);
    monitor_write("received interrupt: ");
    monitor_write_dec(regs.int_no);
    monitor_write(" Tick: ");
    monitor_write_dec(tick);
    monitor_put('\n');
  }
}

void init_timer(u32int frequency)
{
    // Firstly, register our timer callback.
    register_interrupt_handler(IRQ0, &timer_callback);

    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency. Important to note is
    // that the divisor must be small enough to fit into 16-bits.
    u32int divisor = 1193180 / frequency;
          /** LMC 2021.08.10 At main this function is called with:
              init_timer(60); Then divisor=1193180/60=19886 (integer div),
              19886 = 0x4DAE
              ok, it is lesser than 0xFFFF=65535. So, some lines down we'll
              have outb(0x40,l)=outb(0x40,0xAE) and 
                   outb(0x40,h)=outb(0x40,0x4D)
          */

    // Send the command byte.
    /** PIT 8254 
http://www.brokenthorn.com/Resources/OSDevPit.html*/
    /**| 0       0 | 1       1   | 0  1  1 |  0     |*/
    /**| counter 0 | LSByte then | Mode 3  | Format |*/
    /**|           | MSByte      |         | binary |*/
    /** 00110110b = 0x36 */
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    u8int l = (u8int)(divisor & 0xFF);
    u8int h = (u8int)( (divisor>>8) & 0xFF );

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
}
