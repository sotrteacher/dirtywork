#ifndef _GDT_H
#define _GDT_H

#define NUM_GDT_ENTRIES	3
#define SEGMENT_BASE	0
#define SEGMENT_LIMIT	0xFFFFF
#define CODE_RX_TYPE	0xA
#define DATA_RW_TYPE	0x2
#define PL0		0x0
#define PL1		0x3

struct gdt
{
	unsigned int	address;
	unsigned short	size;
} __attribute__((packed));

struct gdt_entry
{
	unsigned short	limit;
	unsigned short	base1;
	unsigned char	base2;
	unsigned char	dpl;
	unsigned char	limit2;
	unsigned char	base3;
} __attribute__((packed));

typedef unsigned short	gdt_selector;

void
init_gdt(void);

extern void
load_gdt(struct gdt g);

void
load_gdt_entry(unsigned idx, unsigned char pl, unsigned char type);

#endif /* _GDT_H */
