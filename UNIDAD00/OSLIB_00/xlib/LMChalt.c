/** From xlib/x1.c */
#include <ll/i386/hw-data.h> /* DWORD */
#include <ll/stdlib.h>
#include <ll/i386/mb-info.h>

/** From libc/cons.c */
//#include <ll/i386/hw-data.h>
//#include <ll/i386/hw-io.h> /**VER NOTA EN LINE 23 DE hw-io.h*/
#include <ll/i386/hw-instr.h>
//#include <ll/i386/cons.h>
///* #include <xsys.h>*/
#include <ll/i386/string.h>
#include <ll/i386/stdlib.h>
#include <ll/i386/stdio.h>
#include <ll/stdarg.h>
#include <ll/i386/limits.h>
#include <ll/i386/float.h>
#include <ll/i386/mem.h>
#include <ll/ctype.h>
#include <ll/math.h>
#include "../libc/ioformat/sprintf.h"

FILE(LMCHALT);

extern TSS TSS_table[];

BYTE LL_FPU_savearea[FPU_CONTEXT_SIZE]; /* Global FPU scratch SaveArea */
#ifdef __FPU_DEBUG__
    long int ndp_called = 0,ndp_switched = 0;
#endif

//#define message cprintf
int cprintf(char *fmt,...);
/* Stack base address; use this to check stack overflow!    */
/* With Flat model I do not think we can use 386 protection     */
/* to detect a stack overflow; anyway Watcom C use a standard   */
/* function __CHK to detect it! The compiler place it whenever  */
/* it calls a function to detect overflow           */

DWORD _stkbase; 
DWORD _stktop;

/* This is some extra stuff we need to compile with argument    */
/* passing and math extensions                  */
DWORD _argc = 0;
typedef char *charp;
charp _argv[100];

extern void main(int argc,char *argv[]);
extern void bios_save(void);
extern void bios_restore(void);
extern void DOS_mem_init(void);

void halt(void)
{
	cprintf("Halt called %d",0);
	__asm__("hlt");
}

#ifndef __NOH4__

void __kernel_init__(struct multiboot_info *m);

void _startup(void)
{
	struct multiboot_info *mbi = mbi_address();

	/* Init the DOS memory allocator */
	DOS_mem_init();

	bios_save();

	/* Call init procedure using standard C convention   */
	/* Remember you cannot call any console I/O function */
	/* if you do not call bios_save()            */

	#ifdef __DUMP_MEM__
	cprintf("X/MEM   : %u\n",mbi->mem_upper);
	cprintf("DOS/MEM : %u\n",mbi->mem_lower);
	cprintf("x_bios Size : %u\n",sizeof(X_BIOSCALL));
	cprintf("mbi Size : %u\n",sizeof(struct multiboot_info));
	cprintf("Cmdline : %s\n",mbi->cmdline);
	/*  cprintf("Argc : %u",_argc);
	cprintf("Argv[0] : %s / %s\n",_argv[0]);
	cprintf("Argv[1] : %s\n",_argv[1]);
	cprintf("Argv[2] : %s\n",_argv[2]);
	cprintf("Argv[3] : %s\n",_argv[3]); */
	#endif

	__kernel_init__(mbi);

	bios_restore();
}
#else
void _startup(void)
{
	register int i = 0;
	struct multiboot_info *mbi = mbi_address();
	char *cmdline = (char *)(mbi->cmdline);

	/* Init the DOS memory allocator */
	if (!(mbi->flags & MB_INFO_MEMORY)) {
	cprintf("X/Runtime library error!!! Unable to find memory information!\n");
	exit(-1);
	}
	DOS_mem_init();
	
	if (mbi->flags & MB_INFO_CMDLINE) {
	/* Build parameter list, up to 100 parms... */
	while (cmdline[i] != 0) {
		_argv[_argc] = &(cmdline[i]);
		while (cmdline[i] != ' ' && cmdline[i] != 0) i++;
		if (cmdline[i] == ' ') {
		cmdline[i] = 0; i++; _argc++;
		}
	}
	_argc++;
	}
		
	bios_save();
	/* Call main procedure using standard C convention   */
	/* Remember you cannot call any console I/O function */
	/* if you do not call bios_save()            */
	
	#ifdef __DUMP_MEM__
	cprintf("X/MEM   : %u\n",mbi->mem_upper);
	cprintf("DOS/MEM : %u\n",mbi->mem_lower);
	cprintf("x_bios Size : %u\n",sizeof(X_BIOSCALL));
	cprintf("mbi Size : %u\n",sizeof(struct multiboot_info));
	cprintf("Cmdline : %s\n",mbi->cmdline);
	cprintf("Argc : %u",_argc);
	cprintf("Argv[0] : %s / %s\n",_argv[0]);
	cprintf("Argv[1] : %s\n",_argv[1]);
	cprintf("Argv[2] : %s\n",_argv[2]);
	cprintf("Argv[3] : %s\n",_argv[3]);
	#endif
	main(_argc,_argv);
	bios_restore();
}
#endif
/** cons.c */

/*	Console output functions	*/

////#include <ll/i386/hw-data.h>
////#include <ll/i386/hw-io.h> /**VER NOTA EN LINE 23 DE hw-io.h*/
//#include <ll/i386/hw-instr.h>
////#include <ll/i386/cons.h>
/////* #include <xsys.h>*/
//#include <ll/i386/string.h>
//#include <ll/i386/stdlib.h>
//#include <ll/i386/stdio.h>
//#include <ll/stdarg.h>

/* CGA compatible registers value */

#define CURSOR_START		0x0A
#define CURSOR_END		0x0B
#define VIDEO_ADDRESS_MSB	0x0C
#define VIDEO_ADDRESS_LSB	0x0D
#define CURSOR_POS_MSB		0x0E
#define CURSOR_POS_LSB		0x0F

/* CGA compatible registers */

#define CGA_INDEX_REG	0x3D4
#define CGA_DATA_REG	0x3D5

/* Standard tab size */

#define TAB_SIZE	8

/* Store bios settings */

static unsigned char bios_x,bios_y,bios_start,bios_end,bios_attr;

/* Access directly to video & BIOS memory through linear addressing */

/* Active video page-buffer */
#define PAGE_SIZE		2048
#define PAGE_MAX		8

static int active_page = 0;
static int visual_page = 0;
static int curs_x[PAGE_MAX];
static int curs_y[PAGE_MAX];

void set_visual_page(int page)
{
    unsigned short page_offset = page*PAGE_SIZE;
    visual_page = page;
    outp(CGA_INDEX_REG,VIDEO_ADDRESS_LSB);
    outp(CGA_DATA_REG,page_offset & 0xFF);
    outp(CGA_INDEX_REG,VIDEO_ADDRESS_MSB);
    outp(CGA_DATA_REG,(page_offset >> 8) & 0xFF);
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
    unsigned short cursor_word = x + y*80 + active_page*PAGE_SIZE;
    /* Set cursor position				  */
    /* CGA is programmed writing first the Index register */
    /* to specify what internal register we are accessing */
    /* Then we load the Data register with the wanted val */
    outp(CGA_INDEX_REG,CURSOR_POS_LSB);
    outp(CGA_DATA_REG,cursor_word & 0xFF);
    outp(CGA_INDEX_REG,CURSOR_POS_MSB);
    outp(CGA_DATA_REG,(cursor_word >> 8) & 0xFF);
    /* Adjust temporary cursor bios position */
    bios_x = x;
    bios_y = y;
}

void cursor(int start,int end)
{
    /* Same thing as above; Set cursor scan line */
    outp(CGA_INDEX_REG,CURSOR_START);
    outp(CGA_DATA_REG,start);
    outp(CGA_INDEX_REG,CURSOR_END);
    outp(CGA_DATA_REG,end);
}

void bios_save(void)
{
    /* This function must be called to init CONSole output */
#if 1
    bios_attr = lmempeekb((LIN_ADDR)0xB8000 + 159);
    bios_x = lmempeekb((LIN_ADDR)0x00450);
    bios_y = lmempeekb((LIN_ADDR)0x00451);
    bios_end = lmempeekb((LIN_ADDR)0x00460);
    bios_start = lmempeekb((LIN_ADDR)0x00461);
    active_page = visual_page = 0;
#else
    LIN_ADDR p;

    p = (LIN_ADDR)(0xB8000 + 159);
    bios_attr = *p;
    p = (LIN_ADDR)0x00450;
    bios_x = *p;
    p = (LIN_ADDR)0x00451;
    bios_y = *p;
    p = (LIN_ADDR)0x00460;
    bios_end = *p;
    p = (LIN_ADDR)0x00461;
    bios_start = *p;
    active_page = visual_page = 0;
#endif
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
            lmempokew((LIN_ADDR)(0xB8000 + 2*i+160*j + 2*active_page*PAGE_SIZE),w);
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
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + active_page*(2*PAGE_SIZE));
      
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
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + active_page*(2*PAGE_SIZE));
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
	default   : lmempokeb((LIN_ADDR)(v + 2*(x + y*80)),c);
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
    va_list parms;
    int result;
    va_start(parms,fmt);
    result = vsprintf(cbuf,fmt,parms);
    va_end(parms);
    cputs(cbuf);
    return(result);
}

void puts_xy(int x,int y,char attr,char *s)
{
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + (80*y+x)*2 + active_page*(2*PAGE_SIZE));
    while (*s != 0) {
	/* REMEMBER! This is a macro! v++ is out to prevent side-effects */
	lmempokeb(v,*s); s++; v++;
	lmempokeb(v,attr); v++;
    }
}

void putc_xy(int x,int y,char attr,char c)
{
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + (80*y+x)*2 + active_page*(2*PAGE_SIZE));
    /* REMEMBER! This is a macro! v++ is out to prevent side-effects */
    lmempokeb(v,c); v++;
    lmempokeb(v,attr);
}

char getc_xy(int x,int y,char *attr,char *c)
{
    LIN_ADDR v = (LIN_ADDR)(0xB8000 + (80*y+x)*2 + active_page*(2*PAGE_SIZE));
    char r;
    r = lmempeekb(v); v++;
    if (c != NULL) *c = r;
    r = lmempeekb(v);
    if (attr != NULL) *attr = r;
    return(r);
}

int printf_xy(int x,int y,char attr,char *fmt,...)
{
    char cbuf[200];
    va_list parms;
    int result;

    va_start(parms,fmt);
    result = vsprintf(cbuf,fmt,parms);
    va_end(parms);
    puts_xy(x,y,attr,cbuf);
    return(result);
}

// libc/ioformat/sprintf.c

#define STD_SIZE	0
#define SHORT_SIZE	1
#define LONG_SIZE	2

int vsprintf(char *buf,char *fmt,va_list parms)
{
    int scanned = 0,w = 0,prec = 0,l = 0,size = 0;
    int n1 = 0;
    unsigned n2 = 0,parsing = 0,flag = 0;
    double n3 = 0.0;
    char *base = buf;
    char *sp = NULL;
    void *fp = NULL;
    while (*fmt != 0) {
	if (*fmt != '%' && !parsing) {
	    /* No token detected */
	    *buf++ = *fmt++;
	    scanned++;
	}
	else {	    
	    /* We need to make a conversion */
	    if (*fmt == '%') {
		fmt++;
		parsing = 1;
		w = 10;
		prec = 4;
		size = STD_SIZE;
		flag = 0;
	    }
	    /* Parse token */
	    switch(*fmt) {
		case '%' : *buf++ = '%';
			   scanned++;
			   parsing = 0;
			   break;
		case 'c' : *buf++ = va_arg(parms, char);
			   scanned++;
			   parsing = 0;
			   break;
		case 'i' :
	 	case 'd' : switch (size) {
		    		case STD_SIZE : n1 = va_arg(parms, int);
						break;
				case LONG_SIZE : n1 = va_arg(parms, long int);
						 break;
				case SHORT_SIZE : n1 = va_arg(parms, short int);
						  break;
			   }
			   l = dcvt(n1,buf,10,w,flag);
			   scanned += l;
			   buf += l;
			   parsing = 0;
			   break;
		case 'u' : switch (size) {
		    		case STD_SIZE : n2 = va_arg(parms, unsigned);
						break;
				case LONG_SIZE : n2 = va_arg(parms, unsigned long);
						 break;
				case SHORT_SIZE : n2 = va_arg(parms, unsigned short);
						  break;
			   }
			   l = ucvt(n2,buf,10,w,flag);
			   scanned += l;
			   buf += l;
			   parsing = 0;
			   break;
		case 'x' : switch (size) {
		    		case STD_SIZE : n2 = va_arg(parms, unsigned);
						break;
				case LONG_SIZE : n2 = va_arg(parms, unsigned long);
						 break;
				case SHORT_SIZE : n2 = va_arg(parms, unsigned short);
						  break;
			   }
			   l = ucvt(n2,buf,16,w,flag);
			   scanned += l;
			   buf += l;
			   parsing = 0;
			   break;
		case 'p' : fp = va_arg(parms, void *);
			   n2 = (unsigned long)(fp);
			   l = ucvt(n2,buf,16,w,0);
			   scanned += l;
			   buf += l;
			   parsing = 0;
			   break;
		case 's' : sp = va_arg(parms, char *);
			   while (*sp != 0) {
			       *buf++ = *sp++;
			       l++;
			   }
			   scanned += l;
			   parsing = 0;
			   break;
		case 'f' : switch (size) {
		    		case STD_SIZE : n3 = va_arg(parms, double);
					 	break;
		    		case LONG_SIZE : n3 = va_arg(parms, double);
					 	 break;
				/*
				It seems that the compilers push a float as
				a double ... Need to check!
				*/
		    		case SHORT_SIZE : n3 = va_arg(parms, double);
					 	  break;
			   }
			   l = fcvt(n3,buf,w,prec,flag);
			   scanned += l;
			   buf += l;
			   parsing = 0;
			   break;
		case 'e' : switch (size) {
		    		case STD_SIZE : n3 = va_arg(parms, double);
					 	break;
		    		case LONG_SIZE : n3 = va_arg(parms, double);
					 	 break;
				/*
				It seems that the compilers push a float as
				a double ... Need to check!
				*/
		    		case SHORT_SIZE : n3 = va_arg(parms, double);
					 	  break;
			   }
			   l = ecvt(n3,buf,w,prec,flag);
			   scanned += l;
			   buf += l;
			   parsing = 0;
			   break;			   
		case 'g' : switch (size) {
		    		case STD_SIZE : n3 = va_arg(parms, double);
					 	break;
		    		case LONG_SIZE : n3 = va_arg(parms, double);
					 	 break;
				/*
				It seems that the compilers push a float as
				a double ... Need to check!
				*/
		    		case SHORT_SIZE : n3 = va_arg(parms, double);
					 	  break;
			   }
			   l = gcvt(n3,buf,w,prec,flag);
			   scanned += l;
			   buf += l;
			   parsing = 0;
			   break;
		case 'l' : size = LONG_SIZE;
			   break;
		case 'n' :
		case 'h' : size = SHORT_SIZE;
			   break;
		case '+' : flag |= ADD_PLUS;
			   break;
		case '-' : flag |= LEFT_PAD;
			   break;
		case '.' : parsing = 2;
			   flag |= RESPECT_WIDTH;
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
			      /* MG */
			      /* if the first numeral is zero a ZERO pad is */
			      /* required */
			      /* but not if LEFT_PAD is set*/
			      if (*fmt!='0'||flag&LEFT_PAD)
				  flag |= SPACE_PAD ;
			      else
				  flag |= ZERO_PAD ;
			      fmt = base-1;
			   } else if (parsing == 2) {
			       prec = strtou(fmt,10,&base);
			       fmt = base-1;
			       parsing = 1;
			   }
			   break;
		default :  parsing = 0;
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
    va_list parms;
    int result;
    va_start(parms,fmt);
    result = vsprintf(buf,fmt,parms);
    va_end(parms);
    return(result);
}

// libc/ioformat/ucvt.c

unsigned ucvt(unsigned long v,char *buffer,int base,int width,int flag)
{
    register int i = 0,j;
    unsigned ret = 0,abs_base;
    unsigned long abs_v;
    char tmp[12];
    /* Extract the less significant digit */
    /* Put it into temporary buffer       */
    /* It has to be local to have 	  */
    /* reentrant functions		  */
    /*
    MG: fix to handle zero correctly
    if (v == 0) {
	*buffer++ = '0';
	*buffer = 0;
	return(1);
    }
    */
    
    /* MG: is this required? */
    /* the vsprintf() function seems to call this procedure with */
    /* this flag inverted */
    flag ^= LEFT_PAD;
    
    abs_base = (base >= 0) ? base : -base;
    if (base < 0) abs_v = ((long)(v) < 0) ? -v : v;
    else abs_v = v;
    /* Process left space-padding flags */
    if (flag & ADD_PLUS || ((base < 0) && ((long)(v) < 0))) {
	ret++;
    }
    /* MG: fix to handle zero correctly */
    if (v == 0)
        tmp[i++]='0';
    else
        while (abs_v > 0) {
	    tmp[i++] = todigit(abs_v % abs_base);
	    abs_v = abs_v / abs_base;
        }
    ret += i;
    if ((flag & LEFT_PAD) && (flag & SPACE_PAD)) {
	j = ret;
	while (j < width) {
	    *buffer++ = ' ';
	    ret++;
	    j++;
	}
    }
    /* Add plus if wanted */
    if (base < 0) {
	if (((long)(v)) < 0) *buffer++ = '-';
        else if (flag & ADD_PLUS) *buffer++ = '+';
    } else if (flag & ADD_PLUS) *buffer++ = '+';
    /* Process left zero-padding flags */
    if ((flag & LEFT_PAD) && (flag & ZERO_PAD)) {
	j = ret;
	while (j < width) {
	    *buffer++ = '0';
	    ret++;
	    j++;
	}
    }
    /* Reverse temporary buffer into output buffer */
    /* If wanted left pad with zero/space; anyway only one at once is ok */
    for (j = i-1; j >= 0; j--) *buffer++ = tmp[j];
    if ((flag & (SPACE_PAD)) && !(flag & LEFT_PAD)) {
	/* If wanted, pad with space to specified width */
	j = ret;
	while (j < width) {
	    *buffer++ = ' ';
	    ret++;
	    j++;
	}
    }
    /* Null terminate the output buffer */
    *buffer = 0;
    return(ret);
}

unsigned dcvt(long v,char *buffer,int base,int width,int flag)
{
    return(ucvt((unsigned long)(v),buffer,-base,width,flag));
}


