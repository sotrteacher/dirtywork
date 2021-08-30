#include $(BASE)/../shark.cfg 
#MEM_START = 0x1720000
MEM_START = 0x10000   # KRN_BASE = 0x10000

CC  = gcc
AS  = gcc
LD  = ld
AR  = ar
INCL   = $(BASE)
LIB_PATH    = $(BASE)/lib/
LIB_DIR  = $(BASE)/lib

CFG_OPT += -D__LINUX__
CFG_OPT += -D__DEBUG__

#LMC 2021.08.17
#ifeq ($(TSC),TRUE)
#CFG_OPT += -D__TSC__
#ifeq ($(APIC),TRUE)
#CFG_OPT += -D__APIC__
#endif
#endif
#
#ifeq ($(findstring 1000,$(TIMER_OPT)) , 1000)
#CFG_OPT += -D__O1000__
#else
#ifeq ($(findstring 2000,$(TIMER_OPT)) , 2000)
#CFG_OPT += -D__O2000__
#else
#ifeq ($(findstring 4000,$(TIMER_OPT)) , 4000)
#CFG_OPT += -D__O4000__
#else
#CFG_OPT += -D__O8000__
#endif
#endif
#endif
#
#ifeq ($(findstring NEW,$(TRACER)) , NEW)
#ifeq ($(TSC),TRUE)
#CFG_OPT += -D__NEW_TRACER__
#endif
#endif
#ifeq ($(findstring OLD,$(TRACER)) , OLD)
#CFG_OPT += -D__OLD_TRACER__
#endif

#LMC 2021.08.06
#C_OPT =  -Wall -O -fno-builtin -nostdinc $(CFG_OPT) -DMAIN=__kernel_init__ -I$(INCL)
#ASM_OPT =  -x assembler-with-cpp $(CFG_OPT) -I$(INCL)
#LINK_OPT = -Bstatic -Ttext $(MEM_START) -s -nostartfiles -nostdlib -L$(LIB_PATH)
#C_OPT = -m32 -Wall -O -fno-builtin -nostdinc $(CFG_OPT) -DMAIN=__kernel_init__ -D__LL_DEBUG__ -I$(INCL)
#C_OPT = -m32 -Wall -O -fno-builtin -nostdinc $(CFG_OPT) -DMAIN=__kernel_init__  -I$(INCL)
#C_OPT = -m32 -Wall -O -fno-builtin -nostdinc $(CFG_OPT) -DMAIN=main  -I$(INCL)
#ASM_OPT = -m32 -x assembler-with-cpp $(CFG_OPT) -I$(INCL)
#LINK_OPT = -m elf_i386 -z muldefs -Bstatic -Ttext $(MEM_START) -s -nostartfiles -nostdlib -L$(LIB_PATH)
#C_OPT = -m32 -Wall -O -fno-builtin -nostdinc $(CFG_OPT) -DMAIN=main  -I$(INCL)
#ASM_OPT = -m32 -x assembler-with-cpp $(CFG_OPT) -I$(INCL)
#LINK_OPT = --verbose -m elf_i386 -Bstatic -Ttext $(MEM_START) -s -nostartfiles -nostdlib -L$(LIB_PATH)
C_OPT = -m32 -Wall -O -fno-PIC -nostdlib -nostdinc -fno-builtin -fno-stack-protector $(CFG_OPT) -DMAIN=main -I$(INCL)
ASM_OPT = -m32 -x assembler-with-cpp $(CFG_OPT) -I$(INCL)
LINK_OPT = -m elf_i386 -Bstatic -Ttext 0 -e startup_32 -s -nostartfiles -nostdlib -L$(LIB_PATH)
#LINK_OPT = -m elf_i386 -Bstatic -Tlink.ld -s -nostartfiles -nostdlib -L$(LIB_PATH)

MKDIR   = mkdir
CP	= cp
CAT	= cat
RM	= rm -f
RMDIR	= rm -rf

# Common rules

%.o : %.s
	$(REDIR) $(CC) $(ASM_OPT) -c $<
%.o : %.c
	$(REDIR) $(CC) $(C_OPT) -c $<
%.s : %.c
	$(REDIR) $(CC) $(C_OPT) -S $<

