/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#ifndef TASK_H
#define TASK_H

#include <kernel.h>

#define TS_RUNNING	0
#define TS_RUNABLE	1
#define TS_STOPPED	2

struct TSS_STRUCT {
	unsigned int back_link;
	unsigned int esp0, ss0;
	unsigned int esp1, ss1;
	unsigned int esp2, ss2;
	unsigned int cr3;
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax,ecx,edx,ebx;
	unsigned int esp, ebp;
	unsigned int esi, edi;
	unsigned int es, cs, ss, ds, fs, gs;
	unsigned int ldt;
	unsigned int trace_bitmap;
};

struct TASK_STRUCT {
	struct TSS_STRUCT tss;
	unsigned long long tss_entry;
	unsigned long long ldt[2];
	unsigned long long ldt_entry;
	int state;
	int priority;
	struct TASK_STRUCT *next;
};

#define DEFAULT_LDT_CODE	0x00cffa000000ffffULL
#define DEFAULT_LDT_DATA	0x00cff2000000ffffULL

#define INITIAL_PRIO		200

extern struct TASK_STRUCT TASK0;
extern struct TASK_STRUCT *current;

unsigned long long set_tss(unsigned long long tss);
unsigned long long set_ldt(unsigned long long ldt);
unsigned long long get_tss(void);
unsigned long long get_ldt(void);

void scheduler(void);

#endif
