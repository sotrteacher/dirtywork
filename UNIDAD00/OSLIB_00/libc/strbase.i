# 1 "string/strbase.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "string/strbase.c"
# 22 "string/strbase.c"
# 1 "../ll/ctype.h" 1
# 27 "../ll/ctype.h"
# 1 "../ll/i386/defs.h" 1
# 28 "../ll/ctype.h" 2



char toupper(char c);
char tolower(char c);
int tonumber(char c);
char todigit(int c);
int isalnum(char c);
int isalpha(char c);
int iscntrl(char c);
int isdigit(char c);
int islower(char c);
int isspace(char c);
int isupper(char c);
int isxdigit(char c);
int isnumber(char c,int base);
int isspecial(double d,char *bufp);


# 23 "string/strbase.c" 2
# 1 "../ll/i386/string.h" 1
# 27 "../ll/i386/string.h"
# 1 "../ll/i386/mem.h" 1
# 29 "../ll/i386/mem.h"

# 40 "../ll/i386/mem.h"
# 1 "../ll/sys/types.h" 1
# 25 "../ll/sys/types.h"
# 1 "../ll/i386/hw-data.h" 1
# 28 "../ll/i386/hw-data.h"

# 39 "../ll/i386/hw-data.h"
# 1 "../ll/i386/sel.h" 1
# 40 "../ll/i386/hw-data.h" 2





    typedef void *LIN_ADDR;
    typedef unsigned long DWORD;
    typedef unsigned short WORD;
    typedef unsigned char BYTE;
    typedef unsigned long TIME;
    typedef unsigned long SYS_FLAGS;







typedef short CONTEXT;



typedef struct gate {
    WORD offset_lo __attribute__ ((packed));
    WORD sel __attribute__ ((packed));
    BYTE dword_cnt __attribute__ ((packed));
    BYTE access __attribute__ ((packed));
    WORD offset_hi __attribute__ ((packed));
} GATE;

typedef struct descriptor {
    WORD lim_lo __attribute__ ((packed));
    WORD base_lo __attribute__ ((packed));
    BYTE base_med __attribute__ ((packed));
    BYTE access __attribute__ ((packed));
    BYTE gran __attribute__ ((packed));
    BYTE base_hi __attribute__ ((packed));
} DESCRIPTOR;




union gdt_entry {
    DESCRIPTOR d __attribute__ ((packed));
    GATE g __attribute__ ((packed));
};
# 140 "../ll/i386/hw-data.h"
typedef struct tss {
    WORD back_link __attribute__ ((packed));
    WORD _fill0 __attribute__ ((packed));
    DWORD esp0 __attribute__ ((packed));
    WORD ss0 __attribute__ ((packed));
    WORD _fill1 __attribute__ ((packed));
    DWORD esp1 __attribute__ ((packed));
    WORD ss1 __attribute__ ((packed));
    WORD _fill2 __attribute__ ((packed));
    DWORD esp2 __attribute__ ((packed));
    WORD ss2 __attribute__ ((packed));
    WORD _fill3 __attribute__ ((packed));
    DWORD cr3 __attribute__ ((packed));
    DWORD eip __attribute__ ((packed));
    DWORD eflags __attribute__ ((packed));
    DWORD eax __attribute__ ((packed));
    DWORD ecx __attribute__ ((packed));
    DWORD edx __attribute__ ((packed));
    DWORD ebx __attribute__ ((packed));
    DWORD esp __attribute__ ((packed));
    DWORD ebp __attribute__ ((packed));
    DWORD esi __attribute__ ((packed));
    DWORD edi __attribute__ ((packed));
    WORD es __attribute__ ((packed));
    WORD _fill5 __attribute__ ((packed));
    WORD cs __attribute__ ((packed));
    WORD _fill6 __attribute__ ((packed));
    WORD ss __attribute__ ((packed));
    WORD _fill7 __attribute__ ((packed));
    WORD ds __attribute__ ((packed));
    WORD _fill8 __attribute__ ((packed));
    WORD fs __attribute__ ((packed));
    WORD _fill9 __attribute__ ((packed));
    WORD gs __attribute__ ((packed));
    WORD _fill10 __attribute__ ((packed));
    WORD ldt __attribute__ ((packed));
    WORD _fill11 __attribute__ ((packed));
    WORD trap __attribute__ ((packed));
    WORD io_base __attribute__ ((packed));
    DWORD control __attribute__ ((packed));
    BYTE ctx_FPU[108] __attribute__ ((packed));
} TSS;
# 200 "../ll/i386/hw-data.h"
typedef void (*INTERRUPT)(void);








# 26 "../ll/sys/types.h" 2
# 41 "../ll/i386/mem.h" 2
# 72 "../ll/i386/mem.h"
extern inline void * __constant_memcpy(void * to, const void * from, DWORD n)
{
 switch (n) {
  case 0:
   return to;
  case 1:
   *(unsigned char *)to = *(const unsigned char *)from;
   return to;
  case 2:
   *(unsigned short *)to = *(const unsigned short *)from;
   return to;
  case 3:
   *(unsigned short *)to = *(const unsigned short *)from;
   *(2+(unsigned char *)to) = *(2+(const unsigned char *)from);
   return to;
  case 4:
   *(unsigned long *)to = *(const unsigned long *)from;
   return to;
  case 6:
   *(unsigned long *)to = *(const unsigned long *)from;
   *(2+(unsigned short *)to) = *(2+(const unsigned short *)from);
   return to;
  case 8:
   *(unsigned long *)to = *(const unsigned long *)from;
   *(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
   return to;
  case 12:
   *(unsigned long *)to = *(const unsigned long *)from;
   *(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
   *(2+(unsigned long *)to) = *(2+(const unsigned long *)from);
   return to;
  case 16:
   *(unsigned long *)to = *(const unsigned long *)from;
   *(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
   *(2+(unsigned long *)to) = *(2+(const unsigned long *)from);
   *(3+(unsigned long *)to) = *(3+(const unsigned long *)from);
   return to;
  case 20:
   *(unsigned long *)to = *(const unsigned long *)from;
   *(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
   *(2+(unsigned long *)to) = *(2+(const unsigned long *)from);
   *(3+(unsigned long *)to) = *(3+(const unsigned long *)from);
   *(4+(unsigned long *)to) = *(4+(const unsigned long *)from);
   return to;
 }
# 125 "../ll/i386/mem.h"
{
 int d0, d1, d2;
 switch (n % 4) {
  case 0: __asm__ __volatile__( "cld\n\t" "rep ; movsl" "" : "=&c" (d0), "=&D" (d1), "=&S" (d2) : "0" (n/4),"1" ((long) to),"2" ((long) from) : "memory");; return to;
  case 1: __asm__ __volatile__( "cld\n\t" "rep ; movsl" "\n\tmovsb" : "=&c" (d0), "=&D" (d1), "=&S" (d2) : "0" (n/4),"1" ((long) to),"2" ((long) from) : "memory");; return to;
  case 2: __asm__ __volatile__( "cld\n\t" "rep ; movsl" "\n\tmovsw" : "=&c" (d0), "=&D" (d1), "=&S" (d2) : "0" (n/4),"1" ((long) to),"2" ((long) from) : "memory");; return to;
  default: __asm__ __volatile__( "cld\n\t" "rep ; movsl" "\n\tmovsw\n\tmovsb" : "=&c" (d0), "=&D" (d1), "=&S" (d2) : "0" (n/4),"1" ((long) to),"2" ((long) from) : "memory");; return to;
 }
}


}







extern inline void *lmemcpy(LIN_ADDR t, LIN_ADDR f, DWORD n)
{
 void *p1;
 void *p2;

 p1 = (void *)(t);
 p2 = (void *)(f);
 return (__builtin_constant_p(n) ? __constant_memcpy((p1),(p2),(n)) : __memcpy((p1),(p2),(n)));
}


extern inline void * memmove(void * dest,const void * src, DWORD n)
{
int d0, d1, d2;
if (dest<src)
__asm__ __volatile__(
 "cld\n\t"
 "rep\n\t"
 "movsb"
 : "=&c" (d0), "=&S" (d1), "=&D" (d2)
 :"0" (n),"1" (src),"2" (dest)
 : "memory");
else
__asm__ __volatile__(
 "std\n\t"
 "rep\n\t"
 "movsb\n\t"
 "cld"
 : "=&c" (d0), "=&S" (d1), "=&D" (d2)
 :"0" (n),
  "1" (n-1+(const char *)src),
  "2" (n-1+(char *)dest)
 :"memory");
return dest;
}




extern inline void * memchr(const void * cs,int c,DWORD count)
{
int d0;
register void * __res;
if (!count)
 return 0L;
__asm__ __volatile__(
 "cld\n\t"
 "repne\n\t"
 "scasb\n\t"
 "je 1f\n\t"
 "movl $1,%0\n"
 "1:\tdecl %0"
 :"=D" (__res), "=&c" (d0) : "a" (c),"0" (cs),"1" (count));
return __res;
}

extern inline void * __memset_generic(void * s, char c,DWORD count)
{
int d0, d1;
__asm__ __volatile__(
 "cld\n\t"
 "rep\n\t"
 "stosb"
 : "=&c" (d0), "=&D" (d1)
 :"a" (c),"1" (s),"0" (count)
 :"memory");
return s;
}
# 221 "../ll/i386/mem.h"
extern inline void * __constant_c_memset(void * s, unsigned long c, DWORD count)
{
int d0, d1;
__asm__ __volatile__(
 "cld\n\t"
 "rep ; stosl\n\t"
 "testb $2,%b3\n\t"
 "je 1f\n\t"
 "stosw\n"
 "1:\ttestb $1,%b3\n\t"
 "je 2f\n\t"
 "stosb\n"
 "2:"
 : "=&c" (d0), "=&D" (d1)
 :"a" (c), "q" (count), "0" (count/4), "1" ((long) s)
 :"memory");
return (s);
}





extern inline void * __constant_c_and_count_memset(void * s, unsigned long pattern, DWORD count)
{
 switch (count) {
  case 0:
   return s;
  case 1:
   *(unsigned char *)s = pattern;
   return s;
  case 2:
   *(unsigned short *)s = pattern;
   return s;
  case 3:
   *(unsigned short *)s = pattern;
   *(2+(unsigned char *)s) = pattern;
   return s;
  case 4:
   *(unsigned long *)s = pattern;
   return s;
 }







{
 int d0, d1;
 switch (count % 4) {
  case 0: __asm__ __volatile__("cld\n\t" "rep ; stosl" "" : "=&c" (d0), "=&D" (d1) : "a" (pattern),"0" (count/4),"1" ((long) s) : "memory"); return s;
  case 1: __asm__ __volatile__("cld\n\t" "rep ; stosl" "\n\tstosb" : "=&c" (d0), "=&D" (d1) : "a" (pattern),"0" (count/4),"1" ((long) s) : "memory"); return s;
  case 2: __asm__ __volatile__("cld\n\t" "rep ; stosl" "\n\tstosw" : "=&c" (d0), "=&D" (d1) : "a" (pattern),"0" (count/4),"1" ((long) s) : "memory"); return s;
  default: __asm__ __volatile__("cld\n\t" "rep ; stosl" "\n\tstosw\n\tstosb" : "=&c" (d0), "=&D" (d1) : "a" (pattern),"0" (count/4),"1" ((long) s) : "memory"); return s;
 }
}


}
# 303 "../ll/i386/mem.h"
extern inline void * memscan(void * addr, int c, DWORD size)
{
 if (!size)
  return addr;






 __asm__("cld\n\t"
       "repnz; scasb\n\t"
       "jnz 1f\n\t"
       " dec %%edi\n\t"
 "1:		"
  : "=D" (addr), "=c" (size)
  : "0" (addr), "1" (size), "a" (c));
 return addr;
}

void fmemcpy(unsigned short ds,unsigned long dof,unsigned short ss,unsigned long sof,unsigned n);
# 358 "../ll/i386/mem.h"

# 28 "../ll/i386/string.h" 2








char *strcpy(char *dst,const char *src);
char *strncpy(char *dst,const char *src,int n);
int strcmp(const char *s1,const char *s2);
int strncmp(const char *s1,const char *s2,int n);
int strlen(const char *s);
char *strscn(char *s,char *pattern);
char *strchr(char *s,int c);
char *strupr(char *s);
char *strlwr(char *s);
char *strcat(char *dst,char *src);


# 24 "string/strbase.c" 2

int isalnum(char c)
{
 return (isalpha(c) || isdigit(c));
}

int isalpha(char c)
{
    if ((c >= 'A' && c <= 'z'))
       return(1);
    else return(0);
}

int iscntrl(char c)
{
 if (c < '0') {
  return 1;
 } else {
  return 0;
 }
}

int isdigit(char c)
{
    if ((c >= '0' && c <= '9'))
       return(1);
    else return(0);
}

int islower(char c)
{
    if ((c >= 'a' && c <= 'z'))
       return(1);
    else return(0);
}

int isspace(char c)
{
    if ((c >= 0x09 && c <= 0x0d) || (c == 0x20)) return(1);
    return(0);
}

int isupper(char c)
{
    if ((c >= 'A' && c <= 'Z'))
       return(1);
    else return(0);
}

int isxdigit(char c)
{
    if ((c >= '0' && c <= 'F') || (c >= 'a' && c <= 'f'))
       return(1);
    else return(0);
}

char toupper(char c)
{
    if (c >= 'a' && c <= 'z') return(c-'a'+'A');
    else return(c);
}

char tolower(char c)
{
    if (c >= 'A' && c <= 'A') return(c-'A'+'a');
    else return(c);
}

int tonumber(char c)
{
    if (c >= '0' && c <= '9') return(c - '0');
    else if (c >= 'A' && c <= 'F') return(c - 'A' + 10);
    else if (c >= 'a' && c <= 'f') return(c - 'a' + 10);
    else return(c);
}

char todigit(int c)
{
    if (c >= 0 && c <= 9) return(c+'0');
    else if (c >= 10 && c <= 15) return(c + 'A' - 10);
    else return(c);
}

int isnumber(char c,int base)
{
    static char *digits = "0123456789ABCDEF";
    if ((c >= '0' && c <= digits[base-1]))
       return(1);
    else return(0);
}

int isspecial(double d,char *bufp)
{

    register struct IEEEdp {
 unsigned manl:32;
 unsigned manh:20;
 unsigned exp:11;
 unsigned sign:1;
    } *ip = (struct IEEEdp *)&d;

    if (ip->exp != 0x7ff) return(0);
    if (ip->manh || ip->manl) {
 if (bufp != 0L) strcpy(bufp,"NaN");
 return(1);
    }
    else if (ip->sign) {
 if (bufp != 0L) strcpy(bufp,"+Inf");
 return(2);
    } else {
 if (bufp != 0L) strcpy(bufp,"-Inf");
 return(3);
    }
}


int __isinf(double x)
{
 return (isspecial(x,0L) > 1);
}

int __isnan(double x)
{
 return (isspecial(x,0L) == 1);
}
