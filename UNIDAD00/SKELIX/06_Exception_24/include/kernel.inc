# Skelix by Xiaoming Mo (skelixos@gmail.com)
# Licence: GPLv2
.set CODE_SEL, 0x08	# code segment selector in kernel mode 
.set DATA_SEL, 0x10 # data segment selector in kernel mode
.set IDT_ADDR, 0x80000	# IDT start address
.set IDT_SIZE, (256*8)	# IDT has fixed length
.set GDT_ADDR, (IDT_ADDR+IDT_SIZE)
			# GDT starts after IDT
#if 0 //LMC 2021.09.16
#.set GDT_ENTRIES, 5	# GDT has 5 descriptors
#else
.set GDT_ENTRIES, 6	# GDT has 6 descriptors
#endif
			# null descriptor
			# cs segment descriptor for kernel
			# ds segment descriptor for kernel
			# current process tss
			# current process ldt
			# my segment tor write idt
.set GDT_SIZE, (8*GDT_ENTRIES)
			# GDT length
#.set KERNEL_SECT, 72	# Kernel lenght, counted by sectors
.set KERNEL_SECT, 56	# Kernel lenght, counted by sectors
.set STACK_BOT, 0xa0000	# stack starts at 640K

