
bootblock.o:     file format elf32-i386


Disassembly of section .text:

00007c00 <start>:
#define BSECTORS	0x3 
        #-------------------------------------------------------
        # Only one SECTOR loaded at (0000,7C00). Get entire BLOCK in
        #-------------------------------------------------------
start:
movw  $BOOTS,%ax
    7c00:	b8 c0 07 8e d8       	mov    $0xd88e07c0,%eax
movw  %ax,%ds
        movw  $BOOTSEG,%ax
    7c05:	b8 00 98 8e c0       	mov    $0xc08e9800,%eax
        movw  %ax,%es           # set ES to BOOTSEG=0x9800
        xorw  %bx,%bx
    7c0a:	31 db                	xor    %ebx,%ebx
	xorw  %dx,%dx
    7c0c:	31 d2                	xor    %edx,%edx
	xorw  %cx,%cx
    7c0e:	31 c9                	xor    %ecx,%ecx
	incb  %cl
    7c10:	fe c1                	inc    %cl
	movb  $2,%ah
    7c12:	b4 02                	mov    $0x2,%ah
	movb  $BSECTORS,%al
    7c14:	b0 03                	mov    $0x3,%al
	int   $0x13
    7c16:	cd 13                	int    $0x13
#movw  $512,%cx
#subw  %si,%si
#subw  %di,%di
#cld
#rep
        ljmp   $BOOTSEG,$next # ERROR :-(
    7c18:	ea                   	.byte 0xea
    7c19:	1d                   	.byte 0x1d
    7c1a:	7c 00                	jl     7c1c <start+0x1c>
    7c1c:	98                   	cwtl   

00007c1d <next>:

next:                    
  cli                         # BIOS enabled interrupts; disable
    7c1d:	fa                   	cli    

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax             # Set %ax to zero
    7c1e:	31 c0                	xor    %eax,%eax
  movw    %ax,%ds             # -> Data Segment
    7c20:	8e d8                	mov    %eax,%ds
  movw    %ax,%es             # -> Extra Segment
    7c22:	8e c0                	mov    %eax,%es
  movw    %ax,%ss             # -> Stack Segment
    7c24:	8e d0                	mov    %eax,%ss

00007c26 <seta20.1>:

  # Physical address line A20 is tied to zero so that the first PCs 
  # with 2 MB would run software that assumed 1 MB.  Undo that.
seta20.1:
  inb     $0x64,%al               # Wait for not busy
    7c26:	e4 64                	in     $0x64,%al
  testb   $0x2,%al
    7c28:	a8 02                	test   $0x2,%al
  jnz     seta20.1
    7c2a:	75 fa                	jne    7c26 <seta20.1>

  movb    $0xd1,%al               # 0xd1 -> port 0x64
    7c2c:	b0 d1                	mov    $0xd1,%al
  outb    %al,$0x64
    7c2e:	e6 64                	out    %al,$0x64

00007c30 <seta20.2>:

seta20.2:
  inb     $0x64,%al               # Wait for not busy
    7c30:	e4 64                	in     $0x64,%al
  testb   $0x2,%al
    7c32:	a8 02                	test   $0x2,%al
  jnz     seta20.2
    7c34:	75 fa                	jne    7c30 <seta20.2>

  movb    $0xdf,%al               # 0xdf -> port 0x60
    7c36:	b0 df                	mov    $0xdf,%al
  outb    %al,$0x60
    7c38:	e6 60                	out    %al,$0x60

  # Switch from real to protected mode.  Use a bootstrap GDT that makes
  # virtual addresses map directly to physical addresses so that the
  # effective memory map doesn't change during the transition.
  lgdt    gdtdesc
    7c3a:	0f 01 16             	lgdtl  (%esi)
    7c3d:	f8                   	clc    
    7c3e:	7c 0f                	jl     7c4f <start32+0x1>
  movl    %cr0, %eax
    7c40:	20 c0                	and    %al,%al
  orl     $CR0_PE, %eax
    7c42:	66 83 c8 01          	or     $0x1,%ax
  movl    %eax, %cr0
    7c46:	0f 22 c0             	mov    %eax,%cr0

//PAGEBREAK!
  # Complete the transition to 32-bit protected mode by using a long jmp
  # to reload %cs and %eip.  The segment descriptors are set up with no
  # translation, so that the mapping is still the identity mapping.
  ljmp    $(SEG_KCODE<<3), $start32
    7c49:	ea                   	.byte 0xea
    7c4a:	4e                   	dec    %esi
    7c4b:	7c 08                	jl     7c55 <start32+0x7>
	...

00007c4e <start32>:
.globl getc
.globl inces
.globl readfd
start32:
  # Set up the protected-mode data segment registers
  movw    $(SEG_KDATA<<3), %ax    # Our data segment selector
    7c4e:	66 b8 10 00          	mov    $0x10,%ax
  movw    %ax, %ds                # -> DS: Data Segment
    7c52:	8e d8                	mov    %eax,%ds
  movw    %ax, %es                # -> ES: Extra Segment
    7c54:	8e c0                	mov    %eax,%es
  movw    %ax, %ss                # -> SS: Stack Segment
    7c56:	8e d0                	mov    %eax,%ss
  movw    $0, %ax                 # Zero segments not ready for use
    7c58:	66 b8 00 00          	mov    $0x0,%ax
  movw    %ax, %fs                # -> FS
    7c5c:	8e e0                	mov    %eax,%fs
  movw    %ax, %gs                # -> GS
    7c5e:	8e e8                	mov    %eax,%gs

  # Set up the stack pointer and call into C.
  movl    $start, %esp
    7c60:	bc 00 7c 00 00       	mov    $0x7c00,%esp
  call    bootmain
    7c65:	e8 3b 02 00 00       	call   7ea5 <bootmain>

  # If bootmain returns (it shouldn't), trigger a Bochs
  # breakpoint if running under Bochs, then loop.
  movw    $0x8a00, %ax            # 0x8a00 -> port 0x8a00
    7c6a:	66 b8 00 8a          	mov    $0x8a00,%ax
  movw    %ax, %dx
    7c6e:	66 89 c2             	mov    %ax,%dx
  outw    %ax, %dx
    7c71:	66 ef                	out    %ax,(%dx)
  movw    $0x8ae0, %ax            # 0x8ae0 -> port 0x8a00
    7c73:	66 b8 e0 8a          	mov    $0x8ae0,%ax
  outw    %ax, %dx
    7c77:	66 ef                	out    %ax,(%dx)

00007c79 <spin>:
spin:
  jmp     spin
    7c79:	eb fe                	jmp    7c79 <spin>

00007c7b <setes>:
setes:
  pushw %bp
    7c7b:	66 55                	push   %bp
  movw  %sp,%bp
    7c7d:	66 89 e5             	mov    %sp,%bp
  movw  4(%bp),%ax
    7c80:	67 66 8b 46 04       	mov    0x4(%bp),%ax
  movw  %ax,%es
    7c85:	8e c0                	mov    %eax,%es
  popw  %bp
    7c87:	66 5d                	pop    %bp
  ret
    7c89:	c3                   	ret    

00007c8a <inces>:
inces: # inces() inc ES by NSEC*0x20
  movw  %es,%ax
    7c8a:	66 8c c0             	mov    %es,%ax
  movw  $NSEC,%bx
    7c8d:	66 bb 3c 81          	mov    $0x813c,%bx
#  shll   bx,$5
  shl   %bx
    7c91:	66 d1 e3             	shl    %bx
  shl   %bx
    7c94:	66 d1 e3             	shl    %bx
  shl   %bx
    7c97:	66 d1 e3             	shl    %bx
  shl   %bx
    7c9a:	66 d1 e3             	shl    %bx
  shl   %bx
    7c9d:	66 d1 e3             	shl    %bx
  addw  %bx,%ax
    7ca0:	66 01 d8             	add    %bx,%ax
  movw  %ax,%es
    7ca3:	8e c0                	mov    %eax,%es
  ret
    7ca5:	c3                   	ret    

00007ca6 <putc>:
putc:           
  pushl   %ebp
    7ca6:	55                   	push   %ebp
  movl    %esp,%ebp
    7ca7:	89 e5                	mov    %esp,%ebp
  movb    4(%ebp),%al        # get the char into aL
    7ca9:	8a 45 04             	mov    0x4(%ebp),%al
  movb    $14,%ah            # aH = 14
    7cac:	b4 0e                	mov    $0xe,%ah
  movw    $0x000C,%bx        # bL = cyan color 
    7cae:	66 bb 0c 00          	mov    $0xc,%bx
  int     $0x10               # call BIOS to display the char
    7cb2:	cd 10                	int    $0x10
  popl    %ebp
    7cb4:	5d                   	pop    %ebp
  ret
    7cb5:	c3                   	ret    

00007cb6 <getc>:
getc:
  xorb  %ah,%ah     # clear ah
    7cb6:	30 e4                	xor    %ah,%ah
  int   $0x16   
    7cb8:	cd 16                	int    $0x16
  ret
    7cba:	c3                   	ret    

00007cbb <readfd>:
       #---------------------------------------
       # readfd(cyl, head, sector, buf)
       #        4     6     8      10
       #---------------------------------------
readfd:                             
        pushl %ebp
    7cbb:	55                   	push   %ebp
	movl  %esp,%ebp         # bp = stack frame pointer
    7cbc:	89 e5                	mov    %esp,%ebp
        movb  $0x00,%dl         # drive 0=FD0
    7cbe:	b2 00                	mov    $0x0,%dl
        movb  6(%bp),%dh        # head
    7cc0:	67 8a 76 06          	mov    0x6(%bp),%dh
        movb  8(%bp),%cl        # sector
    7cc4:	67 8a 4e 08          	mov    0x8(%bp),%cl
        incb  %cl
    7cc8:	fe c1                	inc    %cl
        movb  4(%bp),%ch        # cyl
    7cca:	67 8a 6e 04          	mov    0x4(%bp),%ch
        movw  10(%bp),%bx       # BX=buf ==> memory addr=(ES,BX)
    7cce:	67 66 8b 5e 0a       	mov    0xa(%bp),%bx
        movw  $0x0202,%ax       # READ 2 sectors to (EX, BX)
    7cd3:	66 b8 02 02          	mov    $0x202,%ax

        int  $0x13              # call BIOS to read the block 
    7cd7:	cd 13                	int    $0x13
        #jb   _error            # to error if CarryBit is on [read failed]
        jb   spin            # to error if CarryBit is on [read failed]
    7cd9:	72 9e                	jb     7c79 <spin>
        popl  %ebp                
    7cdb:	5d                   	pop    %ebp
	ret
    7cdc:	c3                   	ret    
    7cdd:	8d 76 00             	lea    0x0(%esi),%esi

00007ce0 <gdt>:
	...
    7ce8:	ff                   	(bad)  
    7ce9:	ff 00                	incl   (%eax)
    7ceb:	00 00                	add    %al,(%eax)
    7ced:	9a cf 00 ff ff 00 00 	lcall  $0x0,$0xffff00cf
    7cf4:	00                   	.byte 0x0
    7cf5:	92                   	xchg   %eax,%edx
    7cf6:	cf                   	iret   
	...

00007cf8 <gdtdesc>:
    7cf8:	17                   	pop    %ss
    7cf9:	00 e0                	add    %ah,%al
    7cfb:	7c 00                	jl     7cfd <gdtdesc+0x5>
	...

00007cfe <prints>:
/*u16 BOOTSEG = 0x9000;*/    /* Boot block is loaded again to here.*/
/*u8 BSECTORS = 2;*/         /* load 18 sectors initially */

/*int prints(char *s)*/
void prints(char *s)
{
    7cfe:	55                   	push   %ebp
    7cff:	89 e5                	mov    %esp,%ebp
    7d01:	53                   	push   %ebx
    7d02:	83 ec 04             	sub    $0x4,%esp
    7d05:	8b 5d 08             	mov    0x8(%ebp),%ebx
   while(*s)
    7d08:	0f b6 03             	movzbl (%ebx),%eax
    7d0b:	84 c0                	test   %al,%al
    7d0d:	74 19                	je     7d28 <prints+0x2a>
     putc(*s++);
    7d0f:	83 c3 01             	add    $0x1,%ebx
    7d12:	83 ec 0c             	sub    $0xc,%esp
    7d15:	0f be c0             	movsbl %al,%eax
    7d18:	50                   	push   %eax
    7d19:	e8 88 ff ff ff       	call   7ca6 <putc>
   while(*s)
    7d1e:	0f b6 03             	movzbl (%ebx),%eax
    7d21:	83 c4 10             	add    $0x10,%esp
    7d24:	84 c0                	test   %al,%al
    7d26:	75 e7                	jne    7d0f <prints+0x11>
}
    7d28:	8b 5d fc             	mov    -0x4(%ebp),%ebx
    7d2b:	c9                   	leave  
    7d2c:	c3                   	ret    

00007d2d <gets>:

/*int gets(char *s)*/
void gets(char *s)
{ 
    7d2d:	55                   	push   %ebp
    7d2e:	89 e5                	mov    %esp,%ebp
    7d30:	53                   	push   %ebx
    7d31:	83 ec 04             	sub    $0x4,%esp
    7d34:	8b 5d 08             	mov    0x8(%ebp),%ebx
    while ( (*s=getc()) != '\r')
    7d37:	eb 12                	jmp    7d4b <gets+0x1e>
      putc(*s++);
    7d39:	83 c3 01             	add    $0x1,%ebx
    7d3c:	83 ec 0c             	sub    $0xc,%esp
    7d3f:	0f be c0             	movsbl %al,%eax
    7d42:	50                   	push   %eax
    7d43:	e8 5e ff ff ff       	call   7ca6 <putc>
    7d48:	83 c4 10             	add    $0x10,%esp
    while ( (*s=getc()) != '\r')
    7d4b:	e8 66 ff ff ff       	call   7cb6 <getc>
    7d50:	88 03                	mov    %al,(%ebx)
    7d52:	3c 0d                	cmp    $0xd,%al
    7d54:	75 e3                	jne    7d39 <gets+0xc>
    *s = 0;
    7d56:	c6 03 00             	movb   $0x0,(%ebx)
}
    7d59:	8b 5d fc             	mov    -0x4(%ebp),%ebx
    7d5c:	c9                   	leave  
    7d5d:	c3                   	ret    

00007d5e <getblk>:
u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

/*u16 getblk(u16 blk, char *buf)*/
void getblk(u16 blk, char *buf)
{
    7d5e:	55                   	push   %ebp
    7d5f:	89 e5                	mov    %esp,%ebp
    7d61:	53                   	push   %ebx
    7d62:	83 ec 04             	sub    $0x4,%esp
    7d65:	8b 5d 08             	mov    0x8(%ebp),%ebx
    readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
    7d68:	0f b7 c3             	movzwl %bx,%eax
    7d6b:	69 c0 8f e3 00 00    	imul   $0xe38f,%eax,%eax
    7d71:	c1 e8 14             	shr    $0x14,%eax
    7d74:	89 c1                	mov    %eax,%ecx
    7d76:	8d 14 c5 00 00 00 00 	lea    0x0(,%eax,8),%edx
    7d7d:	01 d0                	add    %edx,%eax
    7d7f:	01 c0                	add    %eax,%eax
    7d81:	29 c3                	sub    %eax,%ebx
    7d83:	89 d8                	mov    %ebx,%eax
    7d85:	ff 75 0c             	pushl  0xc(%ebp)
    7d88:	0f b7 d3             	movzwl %bx,%edx
    7d8b:	69 d2 8f e3 00 00    	imul   $0xe38f,%edx,%edx
    7d91:	c1 ea 13             	shr    $0x13,%edx
    7d94:	8d 1c d5 00 00 00 00 	lea    0x0(,%edx,8),%ebx
    7d9b:	01 d3                	add    %edx,%ebx
    7d9d:	29 d8                	sub    %ebx,%eax
    7d9f:	01 c0                	add    %eax,%eax
    7da1:	0f b7 c0             	movzwl %ax,%eax
    7da4:	50                   	push   %eax
    7da5:	0f b7 d2             	movzwl %dx,%edx
    7da8:	52                   	push   %edx
    7da9:	0f b7 c9             	movzwl %cx,%ecx
    7dac:	51                   	push   %ecx
    7dad:	e8 09 ff ff ff       	call   7cbb <readfd>
}
    7db2:	83 c4 10             	add    $0x10,%esp
    7db5:	8b 5d fc             	mov    -0x4(%ebp),%ebx
    7db8:	c9                   	leave  
    7db9:	c3                   	ret    

00007dba <strcmp>:
  prints("ready to go?"); getc();
}  
/**Taken from file ulib.c of xv6*/
int
strcmp(const char *p, const char *q)
{
    7dba:	55                   	push   %ebp
    7dbb:	89 e5                	mov    %esp,%ebp
    7dbd:	8b 4d 08             	mov    0x8(%ebp),%ecx
    7dc0:	8b 55 0c             	mov    0xc(%ebp),%edx
  while(*p && *p == *q)
    7dc3:	0f b6 01             	movzbl (%ecx),%eax
    7dc6:	84 c0                	test   %al,%al
    7dc8:	74 15                	je     7ddf <strcmp+0x25>
    7dca:	3a 02                	cmp    (%edx),%al
    7dcc:	75 11                	jne    7ddf <strcmp+0x25>
    p++, q++;
    7dce:	83 c1 01             	add    $0x1,%ecx
    7dd1:	83 c2 01             	add    $0x1,%edx
  while(*p && *p == *q)
    7dd4:	0f b6 01             	movzbl (%ecx),%eax
    7dd7:	84 c0                	test   %al,%al
    7dd9:	74 04                	je     7ddf <strcmp+0x25>
    7ddb:	3a 02                	cmp    (%edx),%al
    7ddd:	74 ef                	je     7dce <strcmp+0x14>
  /*return (uchar)*p - (uchar)*q;*/
  return (u8)*p - (u8)*q;
    7ddf:	0f b6 c0             	movzbl %al,%eax
    7de2:	0f b6 12             	movzbl (%edx),%edx
    7de5:	29 d0                	sub    %edx,%eax
}
    7de7:	5d                   	pop    %ebp
    7de8:	c3                   	ret    

00007de9 <search>:
{
    7de9:	55                   	push   %ebp
    7dea:	89 e5                	mov    %esp,%ebp
    7dec:	57                   	push   %edi
    7ded:	56                   	push   %esi
    7dee:	53                   	push   %ebx
    7def:	83 ec 1c             	sub    $0x1c,%esp
    7df2:	8b 45 08             	mov    0x8(%ebp),%eax
    7df5:	8d 78 28             	lea    0x28(%eax),%edi
    7df8:	83 c0 58             	add    $0x58,%eax
    7dfb:	89 45 e0             	mov    %eax,-0x20(%ebp)
    7dfe:	eb 23                	jmp    7e23 <search+0x3a>
                 prints("\n\r"); 
    7e00:	83 ec 0c             	sub    $0xc,%esp
    7e03:	68 10 80 00 00       	push   $0x8010
    7e08:	e8 f1 fe ff ff       	call   7cfe <prints>
                 return((u16)dp->inode);
    7e0d:	0f b7 03             	movzwl (%ebx),%eax
    7e10:	83 c4 10             	add    $0x10,%esp
}
    7e13:	8d 65 f4             	lea    -0xc(%ebp),%esp
    7e16:	5b                   	pop    %ebx
    7e17:	5e                   	pop    %esi
    7e18:	5f                   	pop    %edi
    7e19:	5d                   	pop    %ebp
    7e1a:	c3                   	ret    
    7e1b:	83 c7 04             	add    $0x4,%edi
   for (i=0; i<12; i++){
    7e1e:	3b 7d e0             	cmp    -0x20(%ebp),%edi
    7e21:	74 78                	je     7e9b <search+0xb2>
       if ( (u16)ip->i_block[i] ){
    7e23:	8b 07                	mov    (%edi),%eax
    7e25:	66 85 c0             	test   %ax,%ax
    7e28:	74 f1                	je     7e1b <search+0x32>
          getblk((u16)ip->i_block[i], buf2);
    7e2a:	83 ec 08             	sub    $0x8,%esp
    7e2d:	68 40 85 00 00       	push   $0x8540
    7e32:	0f b7 c0             	movzwl %ax,%eax
    7e35:	50                   	push   %eax
    7e36:	e8 23 ff ff ff       	call   7d5e <getblk>
    7e3b:	83 c4 10             	add    $0x10,%esp
          dp = (DIR *)buf2;
    7e3e:	bb 40 85 00 00       	mov    $0x8540,%ebx
              c = dp->name[dp->name_len];  // save last byte
    7e43:	0f b6 43 06          	movzbl 0x6(%ebx),%eax
    7e47:	0f b6 54 03 08       	movzbl 0x8(%ebx,%eax,1),%edx
    7e4c:	88 55 e7             	mov    %dl,-0x19(%ebp)
              dp->name[dp->name_len] = 0;   
    7e4f:	c6 44 03 08 00       	movb   $0x0,0x8(%ebx,%eax,1)
	      prints(dp->name); putc(' ');
    7e54:	8d 73 08             	lea    0x8(%ebx),%esi
    7e57:	83 ec 0c             	sub    $0xc,%esp
    7e5a:	56                   	push   %esi
    7e5b:	e8 9e fe ff ff       	call   7cfe <prints>
    7e60:	c7 04 24 20 00 00 00 	movl   $0x20,(%esp)
    7e67:	e8 3a fe ff ff       	call   7ca6 <putc>
              if ( strcmp(dp->name, name) == 0 ){
    7e6c:	83 c4 08             	add    $0x8,%esp
    7e6f:	ff 75 0c             	pushl  0xc(%ebp)
    7e72:	56                   	push   %esi
    7e73:	e8 42 ff ff ff       	call   7dba <strcmp>
    7e78:	83 c4 10             	add    $0x10,%esp
    7e7b:	85 c0                	test   %eax,%eax
    7e7d:	74 81                	je     7e00 <search+0x17>
              dp->name[dp->name_len] = c; // restore that last byte
    7e7f:	0f b6 43 06          	movzbl 0x6(%ebx),%eax
    7e83:	0f b6 4d e7          	movzbl -0x19(%ebp),%ecx
    7e87:	88 4c 03 08          	mov    %cl,0x8(%ebx,%eax,1)
              dp = (DIR*)((char *)dp + dp->rec_len);
    7e8b:	0f b7 43 04          	movzwl 0x4(%ebx),%eax
    7e8f:	01 c3                	add    %eax,%ebx
          while ((char *)dp < &buf2[1024]){
    7e91:	81 fb 40 89 00 00    	cmp    $0x8940,%ebx
    7e97:	72 aa                	jb     7e43 <search+0x5a>
    7e99:	eb 80                	jmp    7e1b <search+0x32>
   return 0x0;
    7e9b:	b8 00 00 00 00       	mov    $0x0,%eax
    7ea0:	e9 6e ff ff ff       	jmp    7e13 <search+0x2a>

00007ea5 <bootmain>:
{ 
    7ea5:	55                   	push   %ebp
    7ea6:	89 e5                	mov    %esp,%ebp
    7ea8:	57                   	push   %edi
    7ea9:	56                   	push   %esi
    7eaa:	53                   	push   %ebx
    7eab:	83 ec 68             	sub    $0x68,%esp
  name[0] = "boot";
    7eae:	c7 45 e0 13 80 00 00 	movl   $0x8013,-0x20(%ebp)
  name[1] = filename;
    7eb5:	8d 5d a0             	lea    -0x60(%ebp),%ebx
    7eb8:	89 5d e4             	mov    %ebx,-0x1c(%ebp)
  prints("boot (mtx) : ");  
    7ebb:	68 18 80 00 00       	push   $0x8018
    7ec0:	e8 39 fe ff ff       	call   7cfe <prints>
  gets(filename);
    7ec5:	89 1c 24             	mov    %ebx,(%esp)
    7ec8:	e8 60 fe ff ff       	call   7d2d <gets>
  if (filename[0]==0)
    7ecd:	83 c4 10             	add    $0x10,%esp
    7ed0:	80 7d a0 00          	cmpb   $0x0,-0x60(%ebp)
    7ed4:	75 07                	jne    7edd <bootmain+0x38>
     name[1]="mtx";
    7ed6:	c7 45 e4 26 80 00 00 	movl   $0x8026,-0x1c(%ebp)
  getblk(2, buf1);
    7edd:	83 ec 08             	sub    $0x8,%esp
    7ee0:	68 40 81 00 00       	push   $0x8140
    7ee5:	6a 02                	push   $0x2
    7ee7:	e8 72 fe ff ff       	call   7d5e <getblk>
  iblk = (u16)gp->bg_inode_table;
    7eec:	a1 48 81 00 00       	mov    0x8148,%eax
    7ef1:	89 c7                	mov    %eax,%edi
  getblk(iblk, buf1);       // read first inode block block
    7ef3:	83 c4 08             	add    $0x8,%esp
    7ef6:	68 40 81 00 00       	push   $0x8140
    7efb:	0f b7 c0             	movzwl %ax,%eax
    7efe:	50                   	push   %eax
    7eff:	e8 5a fe ff ff       	call   7d5e <getblk>
    7f04:	8d 75 e0             	lea    -0x20(%ebp),%esi
    7f07:	83 c4 10             	add    $0x10,%esp
  ip = (INODE *)buf1 + 1;   // ip->root inode #2
    7f0a:	bb c0 81 00 00       	mov    $0x81c0,%ebx
      me = search(ip, name[i]) - 1;
    7f0f:	83 ec 08             	sub    $0x8,%esp
    7f12:	ff 36                	pushl  (%esi)
    7f14:	53                   	push   %ebx
    7f15:	e8 cf fe ff ff       	call   7de9 <search>
    7f1a:	8d 58 ff             	lea    -0x1(%eax),%ebx
      getblk(iblk+(me/8), buf1);    // read block inode of me
    7f1d:	83 c4 08             	add    $0x8,%esp
    7f20:	68 40 81 00 00       	push   $0x8140
    7f25:	89 d8                	mov    %ebx,%eax
    7f27:	66 c1 e8 03          	shr    $0x3,%ax
    7f2b:	01 f8                	add    %edi,%eax
    7f2d:	0f b7 c0             	movzwl %ax,%eax
    7f30:	50                   	push   %eax
    7f31:	e8 28 fe ff ff       	call   7d5e <getblk>
      ip = (INODE *)buf1 + (me % 8);
    7f36:	83 e3 07             	and    $0x7,%ebx
    7f39:	c1 e3 07             	shl    $0x7,%ebx
    7f3c:	81 c3 40 81 00 00    	add    $0x8140,%ebx
    7f42:	83 c6 04             	add    $0x4,%esi
  for (i=0; i<2; i++){
    7f45:	83 c4 10             	add    $0x10,%esp
    7f48:	8d 45 e8             	lea    -0x18(%ebp),%eax
    7f4b:	39 c6                	cmp    %eax,%esi
    7f4d:	75 c0                	jne    7f0f <bootmain+0x6a>
  if ((u16)ip->i_block[12])         // only if has indirect blocks 
    7f4f:	8b 43 58             	mov    0x58(%ebx),%eax
    7f52:	66 85 c0             	test   %ax,%ax
    7f55:	0f 85 9c 00 00 00    	jne    7ff7 <bootmain+0x152>
  setes(0x1000);
    7f5b:	83 ec 0c             	sub    $0xc,%esp
    7f5e:	68 00 10 00 00       	push   $0x1000
    7f63:	e8 13 fd ff ff       	call   7c7b <setes>
    7f68:	8d 73 28             	lea    0x28(%ebx),%esi
    7f6b:	8d 7b 58             	lea    0x58(%ebx),%edi
    7f6e:	83 c4 10             	add    $0x10,%esp
      getblk((u16)ip->i_block[i], 0);
    7f71:	83 ec 08             	sub    $0x8,%esp
    7f74:	6a 00                	push   $0x0
    7f76:	0f b7 06             	movzwl (%esi),%eax
    7f79:	50                   	push   %eax
    7f7a:	e8 df fd ff ff       	call   7d5e <getblk>
      putc('*');
    7f7f:	c7 04 24 2a 00 00 00 	movl   $0x2a,(%esp)
    7f86:	e8 1b fd ff ff       	call   7ca6 <putc>
      inces();
    7f8b:	e8 fa fc ff ff       	call   7c8a <inces>
    7f90:	83 c6 04             	add    $0x4,%esi
  for (i=0; i<12; i++){
    7f93:	83 c4 10             	add    $0x10,%esp
    7f96:	39 f7                	cmp    %esi,%edi
    7f98:	75 d7                	jne    7f71 <bootmain+0xcc>
  if ((u16)ip->i_block[12]){ // only if file has indirect blocks
    7f9a:	66 83 7b 58 00       	cmpw   $0x0,0x58(%ebx)
    7f9f:	74 39                	je     7fda <bootmain+0x135>
     while(*up){
    7fa1:	a1 40 85 00 00       	mov    0x8540,%eax
    7fa6:	85 c0                	test   %eax,%eax
    7fa8:	74 30                	je     7fda <bootmain+0x135>
    7faa:	bb 40 85 00 00       	mov    $0x8540,%ebx
        getblk((u16)*up, 0); putc('.');
    7faf:	83 ec 08             	sub    $0x8,%esp
    7fb2:	6a 00                	push   $0x0
    7fb4:	0f b7 c0             	movzwl %ax,%eax
    7fb7:	50                   	push   %eax
    7fb8:	e8 a1 fd ff ff       	call   7d5e <getblk>
    7fbd:	c7 04 24 2e 00 00 00 	movl   $0x2e,(%esp)
    7fc4:	e8 dd fc ff ff       	call   7ca6 <putc>
        inces();
    7fc9:	e8 bc fc ff ff       	call   7c8a <inces>
        up++;
    7fce:	83 c3 04             	add    $0x4,%ebx
     while(*up){
    7fd1:	8b 03                	mov    (%ebx),%eax
    7fd3:	83 c4 10             	add    $0x10,%esp
    7fd6:	85 c0                	test   %eax,%eax
    7fd8:	75 d5                	jne    7faf <bootmain+0x10a>
  prints("ready to go?"); getc();
    7fda:	83 ec 0c             	sub    $0xc,%esp
    7fdd:	68 2a 80 00 00       	push   $0x802a
    7fe2:	e8 17 fd ff ff       	call   7cfe <prints>
    7fe7:	e8 ca fc ff ff       	call   7cb6 <getc>
}  
    7fec:	83 c4 10             	add    $0x10,%esp
    7fef:	8d 65 f4             	lea    -0xc(%ebp),%esp
    7ff2:	5b                   	pop    %ebx
    7ff3:	5e                   	pop    %esi
    7ff4:	5f                   	pop    %edi
    7ff5:	5d                   	pop    %ebp
    7ff6:	c3                   	ret    
     getblk((u16)ip->i_block[12], buf2);
    7ff7:	83 ec 08             	sub    $0x8,%esp
    7ffa:	68 40 85 00 00       	push   $0x8540
    7fff:	0f b7 c0             	movzwl %ax,%eax
    8002:	50                   	push   %eax
    8003:	e8 56 fd ff ff       	call   7d5e <getblk>
    8008:	83 c4 10             	add    $0x10,%esp
    800b:	e9 4b ff ff ff       	jmp    7f5b <bootmain+0xb6>
