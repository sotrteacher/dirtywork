#include "types.h"
#include "defs.h"

#include "mmu.h"

#include "traps.h"
#include "spinlock.h"
#include "idt.h"
#include "device.h"

//static struct idt_entry idt[NUM_IRQS];
// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[NUM_IRQS];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

/*esto esta definido en trap.c*/
//void
//tvinit(void)
//{
//  int i;
//
//  for(i = 0; i < 256; i++)
//    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
//  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
//  
//  initlock(&tickslock, "time");
//}
//void

//init_idt(void)
//{
//  load_idt(idt);
//}

//void
//interrupt_handler(struct cpu_state cpu,
//	struct stack_state stack,
//	unsigned int irq)
//{
//	unsigned char s1[] = "executing IRQ...";
//	dev_write(0, s1, sizeof(s1));
//}
