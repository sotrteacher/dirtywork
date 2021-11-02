/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#include <task.h>
#include <kernel.h>
#include <asm.h>
#if 1 //LMC 2021.09.17
void write_byte_at_gdt(char *byte,unsigned int offset);
extern unsigned long long *my_gdt_entry;
extern char *the_gdt_sel;
union my_gdt_dscr {
  unsigned long long m_gdt_entry;
  unsigned char m_charr[8]; 
} My_gdt_dscr;
#endif

#if 0 //LMC 2021.09.20
static unsigned long TASK0_STACK[256] = {0xf};
#else
unsigned long TASK0_STACK[256] = {0xf};
#endif

struct TASK_STRUCT TASK0 = {
	/* tss */
	{	/* back_link */
		0,
		/* esp0                                    ss0 */
		(unsigned)&TASK0_STACK+sizeof TASK0_STACK, DATA_SEL, 
		/* esp1 ss1 esp2 ss2 */
		0, 0, 0, 0, 
		/* cr3 */
		0, 
		/* eip eflags */
		0, 0, 
		/* eax ecx edx ebx */
		0, 0, 0, 0,
		/* esp ebp */
		0, 0,
		/* esi edi */
		0, 0, 
		/* es          cs             ds */
		USER_DATA_SEL, USER_CODE_SEL, USER_DATA_SEL, 
		/* ss          fs             gs */
		USER_DATA_SEL, USER_DATA_SEL, USER_DATA_SEL, 
		/* ldt */
		0x20,
		/* trace_bitmap */
	#if 0 //LMC 2021.09.20
		0x00000000},
	#else
		0x08000000},
	#endif
	/* tss_entry */
	0,
	/* idt[2] */
	{DEFAULT_LDT_CODE, DEFAULT_LDT_DATA},
	/* idt_entry */
	0,
	/* state */
	TS_RUNNING,
	/* priority */
	INITIAL_PRIO,
	/* next */
	0,
}; /*end struct TASK_STRUCT TASK0 */

unsigned long long 
set_tss(unsigned long long tss) {
	unsigned long long tss_entry = 0x0080890000000067ULL;
	tss_entry |= ((tss)<<16) & 0xffffff0000ULL;
	tss_entry |= ((tss)<<32) & 0xff00000000000000ULL;
     #if 0 //LMC 2021.09.17 
	return gdt[CURR_TASK_TSS] = tss_entry;
     #else
	unsigned char i;
        My_gdt_dscr.m_gdt_entry = tss_entry;
        /** Escribir el byte que se encuentra en la direcci\'on que pasamos como argumento */
        /** en la GDT en el offset que se pasa como 2/o argumento. */	
	for(i=0;i<8;i++) {
	  write_byte_at_gdt(My_gdt_dscr.m_charr+i,TSS_SEL+i); 
	}
	//return gdt[CURR_TASK_TSS] = tss_entry;
	return tss_entry;
     #endif
}/*end set_tss()*/

unsigned long long 
set_ldt(unsigned long long ldt) {
	unsigned long long ldt_entry = 0x008082000000000fULL;
	ldt_entry |= ((ldt)<<16) & 0xffffff0000ULL;
	ldt_entry |= ((ldt)<<32) & 0xff00000000000000ULL;
     #if 0 //LMC 2021.09.17 
	return gdt[CURR_TASK_LDT] = ldt_entry;
     #else
	unsigned char i;
        My_gdt_dscr.m_gdt_entry = ldt_entry;
        /** Escribir el byte que se encuentra en la direcci\'on que pasamos como argumento */
        /** en la GDT en el offset que se pasa como 2/o argumento. */	
	for(i=0;i<8;i++) {
	  write_byte_at_gdt(My_gdt_dscr.m_charr+i,LDT_SEL+i); 
	}
	//return gdt[CURR_TASK_LDT] = ldt_entry;
	return ldt_entry;
     #endif
}/*end set_ldt()*/

unsigned long long
get_tss(void) {
	return gdt[CURR_TASK_TSS];
}

unsigned long long
get_ldt(void) {
	return gdt[CURR_TASK_LDT];
}

struct TASK_STRUCT *current = &TASK0;

void scheduler(void) {
	struct TASK_STRUCT *v = &TASK0, *tmp = 0;
	int cp = current->priority;

	for (; v; v = v->next) {
		if ((v->state==TS_RUNABLE) && (cp>v->priority)) {
			tmp = v;
			cp = v->priority;
		}
	}
	if (tmp && (tmp!=current)) {
		current->tss_entry = get_tss();
		current->ldt_entry = get_ldt();
		tmp->tss_entry = set_tss((unsigned long long)((unsigned int)&tmp->tss));
		tmp->ldt_entry = set_ldt((unsigned long long)((unsigned int)&tmp->ldt));
		current->state = TS_RUNABLE;
		tmp->state = TS_RUNNING;
		current = tmp;
		__asm__ __volatile__("ljmp	$" TSS_SEL_STR ",	$0\n\t");
	}
}
