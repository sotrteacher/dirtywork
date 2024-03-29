//
// isr.c -- High level interrupt service routines and interrupt request handlers.
//          Part of this code is modified from Bran's kernel development tutorials.
//          Rewritten for JamesM's kernel development tutorials.
//

#include "common.h"
#include "isr.h"
#include "monitor.h"
#include "libepc.h"

extern	DWORD32	msec			__asm__("msec")		;
isr_t interrupt_handlers[256];

void register_interrupt_handler(u8int n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}


//#define USE_LIBEPC
void isr_handler(registers_t regs) {
#ifdef USE_LIBEPC
    PutString("recieved interrupt: ");
    PutUnsigned(regs.int_no,10,0);
    PutString(" Tick: ");
    PutUnsigned(tick++,10,0);
    PutString("\r\n");
#else
/*
    monitor_write("recieved interrupt: ");
    monitor_write_dec(regs.int_no);
    monitor_write(" Tick: ");
    monitor_write_dec(tick++);
    monitor_put('\n');
*/
    monitor_write("recieved interrupt: ");
    monitor_write_dec(regs.int_no);
    monitor_put('\n');
    msec++;
#endif /*USE_LIBEPC*/
    if (regs.int_no >= IRQ_MASTER_0) {
        if (regs.int_no >= IRQ_SLAVE_0)
            outportb(PIC_SLAVE_CMD, PIC_CMD_RESET);
        outportb(PIC_MASTER_CMD, PIC_CMD_RESET);
    }
}

// This gets called from our ASM interrupt handler stub.
void irq_handler(registers_t regs)
{
    // Send an EOI (end of interrupt) signal to the PICs.
    // If this interrupt involved the slave.
    if (regs.int_no >= 40)
    {
        // Send reset signal to slave.
        //outb(0xA0, 0x20);
        outportb(0xA0, 0x20);
    }
    // Send reset signal to master. (As well as slave, if necessary).
    //outb(0x20, 0x20);
    outportb(0x20, 0x20);

    if (interrupt_handlers[regs.int_no] != 0)
    {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }

}/*end irq_handler()*/


