/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#ifndef KERNEL_H
#define KERNEL_H

/* BIOS could load boot sector to 07c0:0000 or 0000:07c0, 
   ds will be set to 0x0000, the LOAD_ADDR is the offset in 
   segment 0x0000 */
#define LOAD_ADDR 	0x7c00
/* in protected mode, set the stack just under the video
   ram section which locates at 0xa0000 */
#define STACK_BOT 	0xa0000

/* code selector in gdt */
#define CODE_SEL	0x08

/* data and stack selector in gdt */
#define DATA_SEL	0x10

/* code selector in ldt */
#if 0 //LMC 2021.09.22
#define USER_CODE_SEL	0x07         /* 00000000 00000111 = 0000000000000 1 11 */
#else
#define USER_CODE_SEL	0x0f         /* 00000000 00001111 = 0000000000001 1 11 = 0x0f */
#endif

/* data and stack selector in ldt */
#if 0 //LMC 2021.09.23
#define USER_DATA_SEL	0x0f
#else
#define USER_DATA_SEL	0x17         /* 00000000 00010111 = 0000000000010 1 11 = 0x17 */
#endif

/* IDT_ADDR and GDT_ADDR must align to 16 */
#define IDT_ADDR	0x80000
#define IDT_SIZE	(256*8)
#define GDT_ADDR	((IDT_ADDR)+IDT_SIZE)
#if 0 //LMC 2021.09.17
#define GDT_ENTRIES	5
#else
#define GDT_ENTRIES	9
#endif
#define GDT_SIZE	(8*GDT_ENTRIES)
/* 
   NULL				0	0
   CODE_SEL			8	1
   DATA_SEL			10	2
   CURR_TASK_TSS	18	3
   CURR_TASK_LDT	20	4
 */

#define CURR_TASK_TSS 	3
#define	CURR_TASK_LDT	4

#define TSS_SEL		0x18
#define	LDT_SEL		0x20
#define TSS1_SEL	0x28
#define	LDT1_SEL	0x30

#define TSS_SEL_STR	"0x18"
#define LDT_SEL_STR	"0x20"

extern unsigned long long *gdt;
extern unsigned long long *ldt;

#endif
