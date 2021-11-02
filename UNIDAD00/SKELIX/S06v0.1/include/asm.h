/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#ifndef _ASM_H_
#define _ASM_H_

#define cli() __asm__ ("cli\n\t")
#define sti() __asm__ ("sti\n\t")

#define halt() __asm__ ("cli;hlt\n\t");
#define idle() __asm__ ("jmp .\n\t");

#define inb(port) (__extension__({	\
unsigned char __res;	\
__asm__ ("inb	%%dx,	%%al\n\t"	\
					 :"=a"(__res)	\
					 :"dx"(port));	\
__res;	\
}))

#define outb(value, port) __asm__ (	\
"outb	%%al,	%%dx\n\t"::"al"(value), "dx"(port))

#define insl(port, buf, nr) \
__asm__ ("cld;rep;insl\n\t"	\
::"d"(port), "D"(buf), "c"(nr))

#define outsl(buf, nr, port) \
__asm__ ("cld;rep;outsl\n\t"	\
::"d"(port), "S" (buf), "c" (nr))

#endif
