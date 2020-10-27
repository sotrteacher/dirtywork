#include "asm.h"
#include "memlayout.h"
#include "mmu.h"

# Start the first CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

.code16                       # Assemble for 16-bit mode
.text
.globl start
#define BOOTS 		0x07c0 
#define BOOTSEG 	0x9800
#define BSECTORS	0x3 
        #-------------------------------------------------------
        # Only one SECTOR loaded at (0000,7C00). Get entire BLOCK in
        #-------------------------------------------------------
start:
movw  $BOOTS,%ax
movw  %ax,%ds
        movw  $BOOTSEG,%ax
        movw  %ax,%es           # set ES to BOOTSEG=0x9800
        xorw  %bx,%bx
	xorw  %dx,%dx
	xorw  %cx,%cx
	incb  %cl
	movb  $2,%ah
	movb  $BSECTORS,%al
	int   $0x13
#	jmpi next,BOOTSEG
#movw  $512,%cx
#subw  %si,%si
#subw  %di,%di
#cld
#rep
        ljmp   $BOOTSEG,$next # ERROR :-(

next:                    
  cli                         # BIOS enabled interrupts; disable

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax             # Set %ax to zero
  movw    %ax,%ds             # -> Data Segment
  movw    %ax,%es             # -> Extra Segment
  movw    %ax,%ss             # -> Stack Segment

  # Physical address line A20 is tied to zero so that the first PCs 
  # with 2 MB would run software that assumed 1 MB.  Undo that.
seta20.1:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     seta20.1

  movb    $0xd1,%al               # 0xd1 -> port 0x64
  outb    %al,$0x64

seta20.2:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     seta20.2

  movb    $0xdf,%al               # 0xdf -> port 0x60
  outb    %al,$0x60

  # Switch from real to protected mode.  Use a bootstrap GDT that makes
  # virtual addresses map directly to physical addresses so that the
  # effective memory map doesn't change during the transition.
  lgdt    gdtdesc
  movl    %cr0, %eax
  orl     $CR0_PE, %eax
  movl    %eax, %cr0

//PAGEBREAK!
  # Complete the transition to 32-bit protected mode by using a long jmp
  # to reload %cs and %eip.  The segment descriptors are set up with no
  # translation, so that the mapping is still the identity mapping.
  ljmp    $(SEG_KCODE<<3), $start32

.code32  # Tell assembler to generate 32-bit code now.
.globl setes
.globl NSEC
.globl putc
.globl getc
.globl inces
.globl readfd
start32:
  # Set up the protected-mode data segment registers
  movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %ss                # -> SS: Stack Segment
  movw    $0, %ax                 # Zero segments not ready for use
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS

  # Set up the stack pointer and call into C.
  movl    $start, %esp
  call    bootmain

  # If bootmain returns (it shouldn't), trigger a Bochs
  # breakpoint if running under Bochs, then loop.
  movw    $0x8a00, %ax            # 0x8a00 -> port 0x8a00
  movw    %ax, %dx
  outw    %ax, %dx
  movw    $0x8ae0, %ax            # 0x8ae0 -> port 0x8a00
  outw    %ax, %dx
spin:
  jmp     spin
setes:
  pushw %bp
  movw  %sp,%bp
  movw  4(%bp),%ax
  movw  %ax,%es
  popw  %bp
  ret
inces: # inces() inc ES by NSEC*0x20
  movw  %es,%ax
  movw  $NSEC,%bx
#  shll   bx,$5
  shl   %bx
  shl   %bx
  shl   %bx
  shl   %bx
  shl   %bx
  addw  %bx,%ax
  movw  %ax,%es
  ret
putc:           
  pushl   %ebp
  movl    %esp,%ebp
  movb    4(%ebp),%al        # get the char into aL
  movb    $14,%ah            # aH = 14
  movw    $0x000C,%bx        # bL = cyan color 
  int     $0x10               # call BIOS to display the char
  popl    %ebp
  ret
getc:
  xorb  %ah,%ah     # clear ah
  int   $0x16   
  ret
       #---------------------------------------
       # readfd(cyl, head, sector, buf)
       #        4     6     8      10
       #---------------------------------------
readfd:                             
        pushl %ebp
	movl  %esp,%ebp         # bp = stack frame pointer
        movb  $0x00,%dl         # drive 0=FD0
        movb  6(%bp),%dh        # head
        movb  8(%bp),%cl        # sector
        incb  %cl
        movb  4(%bp),%ch        # cyl
        movw  10(%bp),%bx       # BX=buf ==> memory addr=(ES,BX)
        movw  $0x0202,%ax       # READ 2 sectors to (EX, BX)

        int  $0x13              # call BIOS to read the block 
        #jb   _error            # to error if CarryBit is on [read failed]
        jb   spin            # to error if CarryBit is on [read failed]
        popl  %ebp                
	ret

# Bootstrap GDT
.p2align 2                                # force 4 byte alignment
gdt:
  SEG_NULLASM                             # null seg
  SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # code seg
  SEG_ASM(STA_W, 0x0, 0xffffffff)         # data seg

gdtdesc:
  .word   (gdtdesc - gdt - 1)             # sizeof(gdt) - 1
  .long   gdt                             # address gdt

