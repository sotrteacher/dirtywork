#ifndef _IDT_H
#define _IDT_H

//#define NUM_IRQS	255
#define NUM_IRQS	256

struct idt_entry
{
	unsigned short	offset_low;
	unsigned short	segment_selector;
	unsigned char	reserved;
	unsigned char	flags;
	unsigned short	offset_high;
} __attribute__((packed));

struct cpu_state
{
	unsigned	edi;
	unsigned	esi;
	unsigned	ebp;
	unsigned	edx;
	unsigned	ecx;
	unsigned	ebx;
	unsigned	eax;
	unsigned	esp;
} __attribute__((packed));

struct stack_state
{
	unsigned	eip;
	unsigned	cs;
	unsigned	eflags;
	unsigned	user_esp;
	unsigned	user_ss;
} __attribute__((packed));

void
init_idt(void);

#if 0
void
load_idt_entry(
#endif

extern void
load_idt(struct idt_entry *idt);

void
interrupt_handler(struct cpu_state cpu,
	struct stack_state stack,
	unsigned int irq);


#endif /* _IDT_H */
