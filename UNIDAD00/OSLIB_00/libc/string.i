# 1 "string/string.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "string/string.c"
# 22 "string/string.c"
# 1 "../ll/i386/string.h" 1
# 27 "../ll/i386/string.h"
# 1 "../ll/i386/mem.h" 1
# 28 "../ll/i386/mem.h"
# 1 "../ll/i386/defs.h" 1
# 29 "../ll/i386/mem.h" 2

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
# 146 "../ll/i386/mem.h"
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
# 223 "../ll/i386/mem.h"
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
# 305 "../ll/i386/mem.h"
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
# 360 "../ll/i386/mem.h"

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


# 23 "string/string.c" 2

char *strcpy(char *dst,const char *src)
{
    char *retval = dst;
    while (*src != 0) *dst++ = *src++;
    *dst = 0;
    return(retval);
}

char *strncpy(char *dst,const char *src,int n)
{
    char *retval = dst;
    while (*src != 0 && n-- > 0) *dst++ = *src++;
    *dst = 0;
    return(retval);
}

int strcmp(const char *s1,const char *s2)
{
    while (*s1 == *s2) {
 if (*s1 == 0) return 0;
 s1++;
 s2++;
    }
    return *(unsigned const char *)s1 - *(unsigned const char *)(s2);
}

int strncmp(const char *s1,const char *s2,int n)
{
    if (n == 0) return 0;
    do {
 if (*s1 != *s2++)
     return *(unsigned const char *)s1 - *(unsigned const char *)--s2;
 if (*s1++ == 0) break;
    } while (--n != 0);
    return 0;
}

char *strupr(char *s)
{
    char *base = s;
    while (*s != 0) {
 if (*s >= 'a' && *s <= 'z')
           *s = *s + 'A' -'a';
 s++;
    }
    return(base);
}

char *strlwr(char *s)
{
    char *base = s;
    while (*s != 0) {
 if (*s >= 'A' && *s <= 'Z')
    *s = *s + 'a' -'A';
 s++;
    }
    return(base);
}

int strlen(const char *s)
{
    register int result = 0;
    while (*s != 0) s++ , result++;
    return(result);
}

char *strcat(char *dst,char *src)
{
    char *base = dst;
    while (*dst != 0) dst++;
    while (*src != 0) *dst++ = *src++;
    *dst = 0;
    return(base);
}

char *strscn(char *s,char *pattern)
{
    char *scan;
    while (*s != 0) {
     scan = pattern;
     while (*scan != 0) {
     if (*s == *scan) return(s);
     else scan++;
 }
 s++;
    }
    return(0L);
}

char *strchr(char *s,int c)
{
    while (*s != 0) {
 if (*s == (char)(c)) return(s);
 else s++;
    }
    return(0L);
}
