/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#include <scr.h>
#include <isr.h>
#include <asm.h>
#include <kernel.h>
#include <task.h>
#include <libcc.h>
#include <timer.h>
#include <kprintf.h>
#include <kb.h>

unsigned long long *idt = ((unsigned long long *)IDT_ADDR);
unsigned long long *gdt = ((unsigned long long *)GDT_ADDR);
#if 1 //LMC 2021.09.16
void write_byte_at_idt(char *);
extern unsigned long long *my_idt_entry;
union my_idt_dscr {
  unsigned long long m_idt_entry;
  unsigned char m_charr[8]; 
} My_idt_dscr;
#endif

static void 
isr_entry(int index, unsigned long long offset) {
	unsigned long long idt_entry = 0x00008e0000000000ULL |
			((unsigned long long)CODE_SEL<<16);
	idt_entry |= (offset<<32) & 0xffff000000000000ULL;
	idt_entry |= (offset) & 0xffff;
     #if 0 //LMC 2021.09.16
	idt[index] = idt_entry;
     #else
        My_idt_dscr.m_idt_entry = idt_entry;
      #if 1 //LMC 2021.09.16                      /**!`Creo que este es el orden correcto! */
	write_byte_at_idt(My_idt_dscr.m_charr+0); /**Escribir el byte que se encuentra en */
	write_byte_at_idt(My_idt_dscr.m_charr+1); /**la direcci\'on que pasamos como argumento. */
	write_byte_at_idt(My_idt_dscr.m_charr+2);
	write_byte_at_idt(My_idt_dscr.m_charr+3);
	write_byte_at_idt(My_idt_dscr.m_charr+4);
	write_byte_at_idt(My_idt_dscr.m_charr+5);
	write_byte_at_idt(My_idt_dscr.m_charr+6);
	write_byte_at_idt(My_idt_dscr.m_charr+7);
     #else
//	write_byte_at_idt(My_idt_dscr.m_charr+7); /**Escribir el byte que se encuentra en */
//	write_byte_at_idt(My_idt_dscr.m_charr+6); /**la direcci\'on que pasamos como argumento. */
//	write_byte_at_idt(My_idt_dscr.m_charr+5);
//	write_byte_at_idt(My_idt_dscr.m_charr+4);
//	write_byte_at_idt(My_idt_dscr.m_charr+3);
//	write_byte_at_idt(My_idt_dscr.m_charr+2);
//	write_byte_at_idt(My_idt_dscr.m_charr+1);
//	write_byte_at_idt(My_idt_dscr.m_charr+0);
     #endif
     #endif
}

static void 
idt_install(void) {
	unsigned int i = 0;
	struct DESCR {
		unsigned short length;
		unsigned long address;
	} __attribute__((packed)) idt_descr = {256*8-1, IDT_ADDR};
	for (i=0; i<VALID_ISR; ++i)
		isr_entry(i, (unsigned int)(isr[(i<<1)+1]));
	for (++i; i<256; ++i)
		isr_entry(i, (unsigned int)default_isr);
	__asm__ ("lidt	%0\n\t"::"m"(idt_descr));
#if 0 //LMC 2021.09.16
for(;;);
#endif
}

static void 
pic_install(void) {
	outb(0x11, 0x20);
	outb(0x11, 0xa0);
	outb(0x20, 0x21);
	outb(0x28, 0xa1);
	outb(0x04, 0x21);
	outb(0x02, 0xa1);
	outb(0x01, 0x21);
	outb(0x01, 0xa1);
	outb(0xff, 0x21);
	outb(0xff, 0xa1);
}

extern void task1_run(void);
extern void task2_run(void);

static long task1_stack0[1024] = {0xf, };
static long task1_stack3[1024] = {0xf, };
static long task2_stack0[1024] = {0xf, };
static long task2_stack3[1024] = {0xf, };

static void
new_task(struct TASK_STRUCT *task, unsigned int eip, 
				unsigned int stack0, unsigned int stack3) {
	memcpy(task, &TASK0, sizeof(struct TASK_STRUCT));
	task->tss.esp0 = stack0;
	task->tss.eip = eip;
	task->tss.eflags = 0x3202;
	task->tss.esp = stack3;

	task->priority = INITIAL_PRIO;

	task->state = TS_STOPPED;
	task->next = current->next;
	current->next = task;
	task->state = TS_RUNABLE;
}

void
do_task1(void) {
	print_c('A', BLUE, WHITE);
}

void
do_task2(void) {
	print_c('B', GRAY, BROWN);
}

void 
init(void) {
	char wheel[] = {'\\', '|', '/', '-'};
	int i = 0;
	struct TASK_STRUCT task1;
	struct TASK_STRUCT task2;

	idt_install();
	pic_install();
	kb_install();
	timer_install(100);
	set_tss((unsigned long long)&TASK0.tss);
	set_ldt((unsigned long long)&TASK0.ldt);
	__asm__ ("ltrw	%%ax\n\t"::"a"(TSS_SEL));
	__asm__ ("lldt	%%ax\n\t"::"a"(LDT_SEL));
	sti();
	new_task(&task1,
			(unsigned int)task1_run, 
			(unsigned int)task1_stack0+sizeof task1_stack0,
			(unsigned int)task1_stack3+sizeof task1_stack3);
	new_task(&task2,
			(unsigned int)task2_run,
			(unsigned int)task2_stack0+sizeof task2_stack0,
			(unsigned int)task2_stack3+sizeof task2_stack3);
	__asm__ ("movl %%esp,%%eax\n\t" \
			 "pushl %%ecx\n\t" \
			 "pushl %%eax\n\t" \
			 "pushfl\n\t" \
			 "pushl %%ebx\n\t" \
			 "pushl $1f\n\t" \
			 "iret\n" \
			 "1:\tmovw %%cx,%%ds\n\t" \
			 "movw %%cx,%%es\n\t" \
			 "movw %%cx,%%fs\n\t" \
			 "movw %%cx,%%gs" \
			 ::"b"(USER_CODE_SEL),"c"(USER_DATA_SEL));
	for (;;) {
		__asm__ ("movb	%%al,	0xb8000+160*24"::"a"(wheel[i]));
		if (i == sizeof wheel)
			i = 0;
		else
			++i;
	}
}
