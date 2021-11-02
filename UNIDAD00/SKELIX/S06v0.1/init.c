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
void m_ltrw();
extern unsigned long long *my_idt_entry;
extern struct TSS_STRUCT *tss0;
extern struct TSS_STRUCT *tss1;
union my_idt_dscr {
  unsigned long long m_idt_entry;
  unsigned char m_charr[8]; 
} My_idt_dscr;
extern unsigned long TASK0_STACK[256];
void go_to_user_mode();
void move_to_user_mode();
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
     //                                           /**!`Creo que este es el orden correcto! */
	write_byte_at_idt(My_idt_dscr.m_charr+0); /**Escribir el byte que se encuentra en */
	write_byte_at_idt(My_idt_dscr.m_charr+1); /**la direcci\'on que pasamos como argumento. */
	write_byte_at_idt(My_idt_dscr.m_charr+2);
	write_byte_at_idt(My_idt_dscr.m_charr+3);
	write_byte_at_idt(My_idt_dscr.m_charr+4);
	write_byte_at_idt(My_idt_dscr.m_charr+5);
	write_byte_at_idt(My_idt_dscr.m_charr+6);
	write_byte_at_idt(My_idt_dscr.m_charr+7);
	//idt[index] = idt_entry;
     #endif
}/*isr_entry()*/

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
     #if 0 //LMC 2021.09.20
	memcpy(task, &TASK0, sizeof(struct TASK_STRUCT));
     #else
        //task->tss.back_link = 0;
       #if 0 //LMC 2021.09.20
	//task->tss.esp0 = (unsigned)&TASK0_STACK+sizeof TASK0_STACK;
       #else
	//task->tss.esp0 = (unsigned)&stack0_krn_ptr;
       #endif
	//task->tss.ss0 = DATA_SEL;
	//task->tss.esp1 = 0;
	//task->tss.ss1 = 0;
	//task->tss.esp2 = 0;
	//task->tss.ss2 = 0;
	//task->tss.cr3 = 0;
	//task->tss.eip = 0;
	//task->tss.eflags = 0;
	//task->tss.eax = 0;
	//task->tss.ecx = 0;
	//task->tss.edx = 0;
	//task->tss.ebx = 0;
	//task->tss.esp = 0;
	//task->tss.ebp = 0;
	//task->tss.esi = 0;
	//task->tss.edi = 0;
	//task->tss.es = USER_DATA_SEL;
	//task->tss.cs = USER_CODE_SEL;
	//task->tss.ds = USER_DATA_SEL;
	//task->tss.ss = USER_DATA_SEL;
	//task->tss.fs = USER_DATA_SEL;
	//task->tss.gs = USER_DATA_SEL;
	//task->tss.ldt = 0x20;
      #if 1 //LMC 2021.09.20
	//task->tss.trace_bitmap = 0x00000000;
      #else
	//task->tss.trace_bitmap = 0x08000000;
      #endif
     #endif
	//task->tss.esp0 = stack0;
	//task->tss.eip = eip;
	//task->tss.eflags = 0x3202;
	//task->tss.esp = stack3;

	task->priority = INITIAL_PRIO;

	task->state = TS_STOPPED;
	task->next = current->next;
	current->next = task;
	task->state = TS_RUNABLE;
//for(;;);
}/*end new_task()*/

void
do_task1(void) {
	print_c('A', BLUE, WHITE);
}

void
do_task2(void) {
	print_c('B', GRAY, BROWN);
}

#if 1 //LMC 2021.09.20
struct TASK_STRUCT task1;
struct TASK_STRUCT task2;
#endif
void 
init(void) {
	char wheel[] = {'\\', '|', '/', '-'};
	int i = 0;
   #if 0 //LMC 2021.09.20
	struct TASK_STRUCT task1;
	struct TASK_STRUCT task2;
   #endif

	idt_install();
	pic_install();
	kb_install();
	timer_install(100);
#if 0 //LMC 2021.09.20
	set_tss((unsigned long long)&TASK0.tss);
#else
	set_tss((unsigned long long)(*((unsigned long long *)tss0)));
#endif
	set_ldt((unsigned long long)&TASK0.ldt);
#if 1 //LMC 2021.09.17
    #if 0 //LMC 2021.09.20
	__asm__ ("pushfl");	                        
	__asm__ ("andl	$0xffffbfff, (%esp)");     
	__asm__ ("popfl");                          
    #endif
	__asm__ ("ltrw	%%ax\n\t"::"a"(TSS_SEL));
#else
	m_ltrw();
#endif
	__asm__ ("lldt	%%ax\n\t"::"a"(LDT_SEL));
#if 0 //LMC 2021.09.20
	sti();
#else
	sti();
#endif
     #if 1 //LMC 2021.09.20
	task1.tss = tss0;
	task2.tss = tss1;
     #endif
	new_task(&task1,
			(unsigned int)task1_run, 
			(unsigned int)task1_stack0+sizeof task1_stack0,
			(unsigned int)task1_stack3+sizeof task1_stack3);
	new_task(&task2,
			(unsigned int)task2_run,
			(unsigned int)task2_stack0+sizeof task2_stack0,
			(unsigned int)task2_stack3+sizeof task2_stack3);
#if 0 //LMC 2021.09.17
for(;;);
#endif
#if 0 //LMC 2021.09.17
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
#else
//	__asm__ ("movl %%esp,%%eax\n\t" \
//			 "pushl %%ecx\n\t" \
//			 "pushl %%eax\n\t" \
//			 "pushfl\n\t" \
//			 "pushl %%ebx\n\t" \
//			 "pushl $1f\n\t" \
//			 "1:\tmovw %%cx,%%ds\n\t" \
//			 ::"b"(USER_CODE_SEL),"c"(USER_DATA_SEL));
	//go_to_user_mode();
	move_to_user_mode();
#endif
#if 0 //LMC 2021.09.17
for(;;);
#endif
	for (;;) {
		__asm__ ("movb	%%al,	0xb8000+160*24"::"a"(wheel[i]));
		if (i == sizeof wheel)
			i = 0;
		else
			++i;
	}
}
