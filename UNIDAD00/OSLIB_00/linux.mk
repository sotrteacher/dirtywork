CC  = gcc
AS  = gcc
LD  = ld
AR  = ar
INCL   = $(BASE)
VMINCL  = -I$(BASE)/include/i386 -I$(BASE)/include -I$(BASE)/include/sys/ll
#LIB_PATH    = $(BASE)/lib/
LIB_PATH    = $(BASE)
LIB_DIR  = $(BASE)/lib

#C_OPT =  -Wall -O -finline-functions -fno-builtin -nostdinc -D__LINUX__ -I$(INCL)
#ASM_OPT =  -x assembler-with-cpp -D__LINUX__ -I$(INCL)
#LINK_OPT = -Bstatic -Ttext 0x220000 -s -nostartfiles -nostdlib -L$(LIB_PATH)
#ifeq ($(EJEMPLO),1) 
#C_OPT = -m32 -Wall -O -finline-functions -fno-builtin -nostdinc -D__LINUX__ -D__LL_I386_MEM_H__  -D__LL_I386_HW_INSTR_H__  -I$(INCL)
#else
C_OPT = -m32 -Wall -O -finline-functions -fno-builtin -nostdinc -D__LINUX__ -I$(INCL)
#endif
ASM_OPT = -m32 -x assembler-with-cpp -D__LINUX__ -I$(INCL)
LINK_OPT = -Bstatic -m elf_i386 -Ttext 0x220000 -s -nostartfiles -nostdlib -L$(LIB_PATH)/xlib -L$(LIB_PATH)/libc -L$(LIB_PATH)/libm -L$(LIB_PATH)/kl

MKDIR   = mkdir
CP	= cp
CAT	= cat
RM	= rm -f

# Common rules

%.o : %.s
	$(REDIR) $(CC) $(ASM_OPT) $(A_OUTPUT) -c $<
%.o : %.c
	$(REDIR) $(CC) $(C_OPT) $(C_OUTPUT) -c $<
%.s : %.c
	$(REDIR) $(CC) $(C_OPT) $(C_OUTPUT) -S $<

