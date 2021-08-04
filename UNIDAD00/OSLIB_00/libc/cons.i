# 1 "cons.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "cons.c"
# 24 "cons.c"
# 1 "../ll/i386/hw-data.h" 1
# 27 "../ll/i386/hw-data.h"
# 1 "../ll/i386/defs.h" 1
# 28 "../ll/i386/hw-data.h" 2

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








# 25 "cons.c" 2
# 1 "../ll/i386/hw-instr.h" 1
# 29 "../ll/i386/hw-instr.h"







# 1 "../ll/i386/hw-io.h" 1
# 28 "../ll/i386/hw-io.h"

# 41 "../ll/i386/hw-io.h"
__inline__ static unsigned char inp(unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0"
   : "=a" (rv)
   : "d" (_port));
    return(rv);
}

__inline__ static unsigned short inpw (unsigned short _port)
{
    unsigned short rv;
    __asm__ __volatile__ ("inw %1, %0"
   : "=a" (rv)
   : "d" (_port));
    return(rv);
}

__inline__ static unsigned long inpd(unsigned short _port)
{
    unsigned long rv;
    __asm__ __volatile__ ("inl %1, %0"
   : "=a" (rv)
   : "d" (_port));
    return(rv);
}

__inline__ static void outp(unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0"
   :
   : "d" (_port),
     "a" (_data));
}

__inline__ static void outpw(unsigned short _port, unsigned short _data)
{
    __asm__ __volatile__ ("outw %1, %0"
   :
   : "d" (_port),
     "a" (_data));
}

__inline__ static void outpd(unsigned short _port, unsigned long _data)
{
    __asm__ __volatile__ ("outl %1, %0"
   :
   : "d" (_port),
     "a" (_data));
}


# 37 "../ll/i386/hw-instr.h" 2

__inline__ static WORD get_CS(void)
{WORD r; __asm__ __volatile__ ("movw %%cs,%0" : "=q" (r)); return(r);}

__inline__ static WORD get_DS(void)
{WORD r; __asm__ __volatile__ ("movw %%ds,%0" : "=q" (r)); return(r);}
__inline__ static WORD get_FS(void)
{WORD r; __asm__ __volatile__ ("movw %%fs,%0" : "=q" (r)); return(r);}



__inline__ static DWORD get_SP(void)
{
    DWORD rv;
    __asm__ __volatile__ ("movl %%esp, %0"
   : "=a" (rv));
    return(rv);
}

__inline__ static DWORD get_BP(void)
{
    DWORD rv;
    __asm__ __volatile__ ("movl %%ebp, %0"
   : "=a" (rv));
    return(rv);
}

__inline__ static WORD get_TR(void)
{WORD r; __asm__ __volatile__ ("strw %0" : "=q" (r)); return(r); }

__inline__ static void set_TR(WORD n)
{__asm__ __volatile__("ltr %%ax": :"a" (n)); }

__inline__ static void set_LDTR(WORD addr)
{ __asm__ __volatile__("lldt %%ax": :"a" (addr)); }



__inline__ static void clts(void)
{__asm__ __volatile__ ("clts"); }


__inline__ static void hlt(void)
{__asm__ __volatile__ ("hlt"); }


__inline__ static void sti(void) {__asm__ __volatile__ ("sti"); }
__inline__ static void cli(void) {__asm__ __volatile__ ("cli"); }

__inline__ static SYS_FLAGS ll_fsave(void)
{
    SYS_FLAGS result;

    __asm__ __volatile__ ("pushfl");
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("popl %eax");
    __asm__ __volatile__ ("movl %%eax,%0"
 : "=r" (result)
 :
 : "eax" );
    return(result);
}

__inline__ static void ll_frestore(SYS_FLAGS f)
{
    __asm__ __volatile__ ("mov %0,%%eax"
 :
 : "r" (f)
 : "eax");
    __asm__ __volatile__ ("pushl %eax");
    __asm__ __volatile__ ("popfl");
}
# 118 "../ll/i386/hw-instr.h"
__inline__ static void save_fpu(TSS *t)
{
    __asm__ __volatile__("fnsave %0\n\tfwait":"=m" (t->ctx_FPU));
}

__inline__ static void restore_fpu(TSS *t)
{

    __asm__ __volatile__("frstor %0": :"m" (t->ctx_FPU));




}

__inline__ static void smartsave_fpu(TSS *t)
{
    if (t->control & 0x4000) save_fpu(t);
}

__inline__ static void reset_fpu(void) { __asm__ __volatile__ ("fninit"); }

__inline__ static int check_fpu(void)
{
    int result;

    __asm__ __volatile__ ("movl %cr0,%eax");
    __asm__ __volatile__ ("movl %eax,%edi");
    __asm__ __volatile__ ("andl $0x0FFFFFFEF,%eax");
    __asm__ __volatile__ ("movl %eax,%cr0");
    __asm__ __volatile__ ("movl %cr0,%eax");
    __asm__ __volatile__ ("xchgl %edi,%eax");
    __asm__ __volatile__ ("movl %eax,%cr0");
    __asm__ __volatile__ ("xorl %eax,%eax");
    __asm__ __volatile__ ("movb %bl,%al");
    __asm__ __volatile__ ("shrb $4,%al");
    __asm__ __volatile__ ("movl %%eax,%0"
 : "=r" (result)
 :
 : "eax" );
    return(result);
}

__inline__ static void init_fpu(void)
{
    __asm__ __volatile__ ("movl %cr0,%eax");
    __asm__ __volatile__ ("orl  $34,%eax");
    __asm__ __volatile__ ("movl %eax,%cr0");
    __asm__ __volatile__ ("fninit");
}


extern BYTE LL_FPU_savearea[];
# 195 "../ll/i386/hw-instr.h"
__inline__ static void lmempokeb(LIN_ADDR a, BYTE v)
{
 *((BYTE *)a) = v;
}
__inline__ static void lmempokew(LIN_ADDR a, WORD v)
{
 *((WORD *)a) = v;
}
__inline__ static void lmempoked(LIN_ADDR a, DWORD v)
{
 *((DWORD *)a) = v;
}

__inline__ static BYTE lmempeekb(LIN_ADDR a)
{
 return *((BYTE *)a);
}
__inline__ static WORD lmempeekw(LIN_ADDR a)
{
 return *((WORD *)a);
}
__inline__ static DWORD lmempeekd(LIN_ADDR a)
{
 return *((DWORD *)a);
}


# 26 "cons.c" 2
# 1 "../ll/i386/cons.h" 1
# 28 "../ll/i386/cons.h"



extern int cons_columns;
extern int cons_rows;







void set_visual_page(int page);
void set_active_page(int page);
int get_visual_page(void);
int get_active_page(void);
void place(int x,int y);
void cursor(int start,int end);
void _clear(char c,char attr,int x1,int y1,int x2,int y2);
void clear(void);
void _scroll(char attr,int x1,int y1,int x2,int y2);
void scroll(void);
void bios_save(void);
void bios_restore(void);
void cputc(char c);
void cputs(char *s);
int cprintf(char *fmt,...) __attribute__((format(printf,1,2)));






void putc_xy(int x,int y,char attr,char c);
char getc_xy(int x,int y,char *attr,char *c);
void puts_xy(int x,int y,char attr,char *s);
int printf_xy(int x,int y,char attr, char *fmt,...) __attribute__((format(printf,4,5)));
# 92 "../ll/i386/cons.h"

# 27 "cons.c" 2

# 1 "../ll/i386/string.h" 1
# 27 "../ll/i386/string.h"
# 1 "../ll/i386/mem.h" 1
# 29 "../ll/i386/mem.h"

# 40 "../ll/i386/mem.h"
# 1 "../ll/sys/types.h" 1
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


# 29 "cons.c" 2
# 1 "../ll/i386/stdlib.h" 1
# 28 "../ll/i386/stdlib.h"

# 39 "../ll/i386/stdlib.h"
typedef int wchar_t;




long strtoi(char *s,int base,char **scan_end);
unsigned long strtou(char *s,int base,char **scan_end);
double strtod(char *s,char **scan_end);
long strtol(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);


unsigned ecvt(double v,char *buffer,int width,int prec,int flag);
unsigned fcvt(double v,char *buffer,int width,int prec,int flag);
unsigned gcvt(double v,char *buffer,int width,int prec,int flag);
unsigned dcvt(long v,char *buffer,int base,int width,int flag);
unsigned ucvt(unsigned long v,char *buffer,int base,int width,int flag);
# 68 "../ll/i386/stdlib.h"
void srand(long int seed);
long int rand(void);
unsigned abs(int x);


void exit(int code);
# 93 "../ll/i386/stdlib.h"

# 30 "cons.c" 2
# 1 "../ll/i386/stdio.h" 1
# 26 "../ll/i386/stdio.h"


# 1 "../ll/stdarg.h" 1
# 55 "../ll/stdarg.h"

# 88 "../ll/stdarg.h"

# 29 "../ll/i386/stdio.h" 2

int vsprintf(char *buf,char *fmt,void* parms);
int sprintf(char *buf,char *fmt,...) __attribute__((__format__(printf,2,3)));
int vsscanf(char *buf,char *fmt,void* parms);
int sscanf(char *buf,char *fmt,...) __attribute__((__format__(scanf,2,3)));

int ll_printf(char *fmt,...);


# 31 "cons.c" 2
# 53 "cons.c"
static unsigned char bios_x,bios_y,bios_start,bios_end,bios_attr;







static int active_page = 0;
static int visual_page = 0;
static int curs_x[8];
static int curs_y[8];

void set_visual_page(int page)
{
    unsigned short page_offset = page*2048;
    visual_page = page;
    outp(0x3D4,0x0D);
    outp(0x3D5,page_offset & 0xFF);
    outp(0x3D4,0x0C);
    outp(0x3D5,(page_offset >> 8) & 0xFF);
}

void set_active_page(int page)
{
    curs_x[active_page] = bios_x;
    curs_y[active_page] = bios_y;
    bios_x = curs_x[page];
    bios_y = curs_y[page];
    active_page = page;
}

int get_visual_page(void)
{
    return(visual_page);
}

int get_active_page(void)
{
    return(active_page);
}

void place(int x,int y)
{
    unsigned short cursor_word = x + y*80 + active_page*2048;




    outp(0x3D4,0x0F);
    outp(0x3D5,cursor_word & 0xFF);
    outp(0x3D4,0x0E);
    outp(0x3D5,(cursor_word >> 8) & 0xFF);

    bios_x = x;
    bios_y = y;
}

void cursor(int start,int end)
{

    outp(0x3D4,0x0A);
    outp(0x3D5,start);
    outp(0x3D4,0x0B);
    outp(0x3D5,end);
}

void bios_save(void)
{


    bios_attr = lmempeekb((LIN_ADDR)0xB8000 + 159);
    bios_x = lmempeekb((LIN_ADDR)0x00450);
    bios_y = lmempeekb((LIN_ADDR)0x00451);
    bios_end = lmempeekb((LIN_ADDR)0x00460);
    bios_start = lmempeekb((LIN_ADDR)0x00461);
    active_page = visual_page = 0;
# 145 "cons.c"
}

void bios_restore(void)
{
    lmempokeb((LIN_ADDR)0x00450,bios_x);
    lmempokeb((LIN_ADDR)0x00451,bios_y);
    place(bios_x,bios_y);
    cursor(bios_start,bios_end);
}

void _clear(char c,char attr,int x1,int y1,int x2,int y2)
{
    register int i,j;
    WORD w = attr;
    w <<= 8; w |= c;
    for (i = x1; i <= x2; i++)
     for (j = y1; j <= y2; j++)
            lmempokew((LIN_ADDR)(0xB8000 + 2*i+160*j + 2*active_page*2048),w);
    place(x1,y1);
    bios_y = y1;
    bios_x = x1;
}

void clear()
{
    _clear(' ',bios_attr,0,0,79,24);
}

void _scroll(char attr,int x1,int y1,int x2,int y2)
{
    register int x,y;
    WORD xattr = attr << 8,w;
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + active_page*(2*2048));

    for (y = y1+1; y <= y2; y++)
       for (x = x1; x <= x2; x++) {
    w = lmempeekw((LIN_ADDR)(v + 2*(y*80+x)));
    lmempokew((LIN_ADDR)(v + 2*((y-1)*80+x)),w);
    }
    for (x = x1; x <= x2; x++)
    lmempokew((LIN_ADDR)(v + 2*((y-1)*80+x)),xattr);
}

void scroll(void)
{
    _scroll(bios_attr,0,0,79,24);
}

void cputc(char c)
{
    static unsigned short scan_x,x,y;
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + active_page*(2*2048));
    x = bios_x;
    y = bios_y;
    switch (c) {
 case '\t' : x += 8;
          if (x >= 80) {
   x = 0;
   if (y == 24) scroll();
   else y++;
          } else {
   scan_x = 0;
   while ((scan_x+8) < x) scan_x += 8;
   x = scan_x;
      }
      break;
 case '\n' : x = 0;
      if (y == 24) scroll();
      else y++;
      break;
 case '\b' : x--;
      lmempokeb((LIN_ADDR)(v + 2*(x + y*80)),' ');
      x++;
      break;
 default : lmempokeb((LIN_ADDR)(v + 2*(x + y*80)),c);
      x++;
          if (x > 80) {
   x = 0;
   if (y == 24) scroll();
   else y++;
          }
    }
    place(x,y);
}

void cputs(char *s)
{
    char c;
    while (*s != '\0') {
 c = *s++;
 cputc(c);
    }
}

int cprintf(char *fmt,...)
{
    static char cbuf[500];
    void* parms;
    int result;
    (parms = ((void*) __builtin_next_arg (fmt)));
    result = vsprintf(cbuf,fmt,parms);
    ;
    cputs(cbuf);
    return(result);
}

void puts_xy(int x,int y,char attr,char *s)
{
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + (80*y+x)*2 + active_page*(2*2048));
    while (*s != 0) {

 lmempokeb(v,*s); s++; v++;
 lmempokeb(v,attr); v++;
    }
}

void putc_xy(int x,int y,char attr,char c)
{
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + (80*y+x)*2 + active_page*(2*2048));

    lmempokeb(v,c); v++;
    lmempokeb(v,attr);
}

char getc_xy(int x,int y,char *attr,char *c)
{
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + (80*y+x)*2 + active_page*(2*2048));
    char r;
    r = lmempeekb(v); v++;
    if (c != 0L) *c = r;
    r = lmempeekb(v);
    if (attr != 0L) *attr = r;
    return(r);
}

int printf_xy(int x,int y,char attr,char *fmt,...)
{
    char cbuf[200];
    void* parms;
    int result;

    (parms = ((void*) __builtin_next_arg (fmt)));
    result = vsprintf(cbuf,fmt,parms);
    ;
    puts_xy(x,y,attr,cbuf);
    return(result);
}
