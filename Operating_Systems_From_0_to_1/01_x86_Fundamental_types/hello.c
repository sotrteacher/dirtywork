/** x86 Fundamental types 
 * REF: Operating Systems From 0 to 1, Chapter 4 x86 Assembly and C, 
 * 4.8.1 Examine compiled data, page 72 (86/309) of Operating_Systems_From_0_to_1.pdf, 
 * download url: 
 * https://lsi.vc.ehu.eus/pablogn/docencia/ISO/Operating_Systems_From_0_to_1.pdf 
 * 
 * Specific objdump command:
 * objdump -z -M intel -S -D -j .data -j .bss <object file> | less
 * Note: zero bytes are hidden with three dot symbols: ... To show all the zero bytes, 
 * we add -z option.
 */
#include <stdint.h>

uint8_t byte = 0x12;
uint16_t word = 0x1234;
uint32_t dword = 0x12345678;
uint64_t qword = 0x123456789abcdef;
unsigned __int128 dqword1 = (__int128) 0x123456789abcdef;
unsigned __int128 dqword2 = (__int128) 0x123456789abcdef << 64;

int main(int argc, char *argv[]) {
 return 0;
}


/**
REF: Operating_Systems_From_0_to_1.pdf, Chapter 4, 4.8 Examine compiled data, 
4.8.1 Fundamental data types, page 76.

# Intel is a little-endian machine, which means smaller addresses 
# hold bytes with smaller values, larger addresses hold byte with 
# larger values.

Isn’t it redundant when char type is always 1 byte already and
why do we bother adding int8_t? The truth is, char type is not guaranteed
to be 1 byte in size, but only the minimum of 1 byte in size.

In C, a byte is defined to be the size of a char, and a char is defined 
to be smallest addressable unit of the underlying hardware platform. There 
are hardware devices that the smallest addressable unit is 16 bit or even 
bigger, which means char is 2 bytes in size and a “byte” in such platforms 
is actually 2 units of 8-bit bytes.

Not all architectures support the double quadword type. Still, gcc does
provide support for 128-bit number and generate code when a CPU supports
it (that is, a CPU must be 64-bit). By specifying a variable of type
__int128 or unsigned __int128, we get a 128-bit variable. If a CPU does
not support 64-bit mode, gcc throws an error.

The data types in C, which represents the fundamental data types,
are also called unsigned numbers. Other than numerical calculations, unsigned
numbers are used as a tool for structuring data in memory; we
will see this application later on the book, when various data structures
are organized into bit groups.
*/
