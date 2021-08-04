# 1 "ioformat/sprintf.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "ioformat/sprintf.c"
# 22 "ioformat/sprintf.c"
# 1 "../ll/i386/stdlib.h" 1
# 27 "../ll/i386/stdlib.h"
# 1 "../ll/i386/defs.h" 1
# 28 "../ll/i386/stdlib.h" 2

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

# 23 "ioformat/sprintf.c" 2
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
# 49 "../ll/i386/mem.h"
extern inline void * __memcpy(void * to, const void * from, DWORD n)
{
int d0, d1, d2;
__asm__ __volatile__(
 "cld\n\t"
 "rep ; movsl\n\t"
 "testb $2,%b4\n\t"
 "je 1f\n\t"
 "movsw\n"
 "1:\ttestb $1,%b4\n\t"
 "je 2f\n\t"
 "movsb\n"
 "2:"
 : "=&c" (d0), "=&D" (d1), "=&S" (d2)
 :"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
 : "memory");
return (to);
}
# 159 "../ll/i386/mem.h"
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
# 225 "../ll/i386/mem.h"
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
# 307 "../ll/i386/mem.h"
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
# 362 "../ll/i386/mem.h"

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


# 24 "ioformat/sprintf.c" 2
# 1 "../ll/i386/limits.h" 1
# 25 "ioformat/sprintf.c" 2
# 1 "../ll/i386/float.h" 1
# 26 "ioformat/sprintf.c" 2

# 1 "../ll/stdarg.h" 1
# 55 "../ll/stdarg.h"

# 88 "../ll/stdarg.h"

# 28 "ioformat/sprintf.c" 2
# 1 "../ll/ctype.h" 1
# 28 "../ll/ctype.h"



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


# 29 "ioformat/sprintf.c" 2
# 1 "../ll/math.h" 1
# 73 "../ll/math.h"
# 1 "../ll/sys/cdefs.h" 1
# 74 "../ll/math.h" 2






double acos (double);
double asin (double);
double atan (double);
double atan2 (double, double);
double ceil (double);
double cos (double);
double cosh (double);
double exp (double);
double fabs (double);
double floor (double);
double fmod (double, double);
double frexp (double, int *);
double ldexp (double, int);
double log (double);
double log10 (double);
double modf (double, double *);
double pow (double, double);
double sin (double);
double sinh (double);
double sqrt (double);
double tan (double);
double tanh (double);






double acosh (double);
double asinh (double);
double atanh (double);
double cabs();
double cbrt (double) __attribute__((const));
double copysign (double, double) __attribute__((const));
double drem (double, double);
double erf (double);
double erfc (double) __attribute__((const));
double expm1 (double) __attribute__((const));
int finite (double) __attribute__((const));
double hypot (double, double);



int isinf (double) __attribute__((const));
int isnan (double) __attribute__((const));
double j0 (double);
double j1 (double);
double jn (int, double);
double lgamma (double);
double log1p (double) __attribute__((const));
double logb (double) __attribute__((const));
double rint (double) __attribute__((const));
double scalb (double, int);
double y0 (double);
double y1 (double);
double yn (int, double);



# 30 "ioformat/sprintf.c" 2
# 1 "ioformat/sprintf.h" 1
# 31 "ioformat/sprintf.c" 2





int vsprintf(char *buf,char *fmt,void* parms)
{
    int scanned = 0,w = 0,prec = 0,l = 0,size = 0;
    int n1 = 0;
    unsigned n2 = 0,parsing = 0,flag = 0;
    double n3 = 0.0;
    char *base = buf;
    char *sp = 0L;
    void *fp = 0L;
    while (*fmt != 0) {
 if (*fmt != '%' && !parsing) {

     *buf++ = *fmt++;
     scanned++;
 }
 else {

     if (*fmt == '%') {
  fmt++;
  parsing = 1;
  w = 10;
  prec = 4;
  size = 0;
  flag = 0;
     }

     switch(*fmt) {
  case '%' : *buf++ = '%';
      scanned++;
      parsing = 0;
      break;
  case 'c' : *buf++ = (parms = (void*) ((char *) (parms) + (((sizeof (char) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((char *) (void *) ((char *) (parms) - (((sizeof (char) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
      scanned++;
      parsing = 0;
      break;
  case 'i' :
   case 'd' : switch (size) {
        case 0 : n1 = (parms = (void*) ((char *) (parms) + (((sizeof (int) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((int *) (void *) ((char *) (parms) - (((sizeof (int) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
      break;
    case 2 : n1 = (parms = (void*) ((char *) (parms) + (((sizeof (long int) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((long int *) (void *) ((char *) (parms) - (((sizeof (long int) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
       break;
    case 1 : n1 = (parms = (void*) ((char *) (parms) + (((sizeof (short int) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((short int *) (void *) ((char *) (parms) - (((sizeof (short int) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
        break;
      }
      l = dcvt(n1,buf,10,w,flag);
      scanned += l;
      buf += l;
      parsing = 0;
      break;
  case 'u' : switch (size) {
        case 0 : n2 = (parms = (void*) ((char *) (parms) + (((sizeof (unsigned) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((unsigned *) (void *) ((char *) (parms) - (((sizeof (unsigned) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
      break;
    case 2 : n2 = (parms = (void*) ((char *) (parms) + (((sizeof (unsigned long) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((unsigned long *) (void *) ((char *) (parms) - (((sizeof (unsigned long) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
       break;
    case 1 : n2 = (parms = (void*) ((char *) (parms) + (((sizeof (unsigned short) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((unsigned short *) (void *) ((char *) (parms) - (((sizeof (unsigned short) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
        break;
      }
      l = ucvt(n2,buf,10,w,flag);
      scanned += l;
      buf += l;
      parsing = 0;
      break;
  case 'x' : switch (size) {
        case 0 : n2 = (parms = (void*) ((char *) (parms) + (((sizeof (unsigned) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((unsigned *) (void *) ((char *) (parms) - (((sizeof (unsigned) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
      break;
    case 2 : n2 = (parms = (void*) ((char *) (parms) + (((sizeof (unsigned long) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((unsigned long *) (void *) ((char *) (parms) - (((sizeof (unsigned long) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
       break;
    case 1 : n2 = (parms = (void*) ((char *) (parms) + (((sizeof (unsigned short) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((unsigned short *) (void *) ((char *) (parms) - (((sizeof (unsigned short) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
        break;
      }
      l = ucvt(n2,buf,16,w,flag);
      scanned += l;
      buf += l;
      parsing = 0;
      break;
  case 'p' : fp = (parms = (void*) ((char *) (parms) + (((sizeof (void *) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((void * *) (void *) ((char *) (parms) - (((sizeof (void *) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
      n2 = (unsigned long)(fp);
      l = ucvt(n2,buf,16,w,0);
      scanned += l;
      buf += l;
      parsing = 0;
      break;
  case 's' : sp = (parms = (void*) ((char *) (parms) + (((sizeof (char *) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((char * *) (void *) ((char *) (parms) - (((sizeof (char *) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
      while (*sp != 0) {
          *buf++ = *sp++;
          l++;
      }
      scanned += l;
      parsing = 0;
      break;
  case 'f' : switch (size) {
        case 0 : n3 = (parms = (void*) ((char *) (parms) + (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((double *) (void *) ((char *) (parms) - (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
       break;
        case 2 : n3 = (parms = (void*) ((char *) (parms) + (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((double *) (void *) ((char *) (parms) - (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
        break;




        case 1 : n3 = (parms = (void*) ((char *) (parms) + (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((double *) (void *) ((char *) (parms) - (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
         break;
      }
      l = fcvt(n3,buf,w,prec,flag);
      scanned += l;
      buf += l;
      parsing = 0;
      break;
  case 'e' : switch (size) {
        case 0 : n3 = (parms = (void*) ((char *) (parms) + (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((double *) (void *) ((char *) (parms) - (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
       break;
        case 2 : n3 = (parms = (void*) ((char *) (parms) + (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((double *) (void *) ((char *) (parms) - (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
        break;




        case 1 : n3 = (parms = (void*) ((char *) (parms) + (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((double *) (void *) ((char *) (parms) - (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
         break;
      }
      l = ecvt(n3,buf,w,prec,flag);
      scanned += l;
      buf += l;
      parsing = 0;
      break;
  case 'g' : switch (size) {
        case 0 : n3 = (parms = (void*) ((char *) (parms) + (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((double *) (void *) ((char *) (parms) - (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
       break;
        case 2 : n3 = (parms = (void*) ((char *) (parms) + (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((double *) (void *) ((char *) (parms) - (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
        break;




        case 1 : n3 = (parms = (void*) ((char *) (parms) + (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))), *((double *) (void *) ((char *) (parms) - (((sizeof (double) + sizeof (int) - 1) / sizeof (int)) * sizeof (int)))));
         break;
      }
      l = gcvt(n3,buf,w,prec,flag);
      scanned += l;
      buf += l;
      parsing = 0;
      break;
  case 'l' : size = 2;
      break;
  case 'n' :
  case 'h' : size = 1;
      break;
  case '+' : flag |= 2;
      break;
  case '-' : flag |= 16;
      break;
  case '.' : parsing = 2;
      flag |= 1;
      break;
  case '1' :
  case '2' :
  case '3' :
  case '4' :
  case '5' :
  case '6' :
  case '7' :
  case '8' :
  case '9' :
  case '0' : if (parsing == 1) {
             w = strtou(fmt,10,&base);




         if (*fmt!='0'||flag&16)
      flag |= 4 ;
         else
      flag |= 8 ;
         fmt = base-1;
      } else if (parsing == 2) {
          prec = strtou(fmt,10,&base);
          fmt = base-1;
          parsing = 1;
      }
      break;
  default : parsing = 0;
      break;
     }
     fmt++;
 }
    }
    *buf = 0;
    return(scanned);
}

int sprintf(char *buf,char *fmt,...)
{
    void* parms;
    int result;
    (parms = ((void*) __builtin_next_arg (fmt)));
    result = vsprintf(buf,fmt,parms);
    ;
    return(result);
}
