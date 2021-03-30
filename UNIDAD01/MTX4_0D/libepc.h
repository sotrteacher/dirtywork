/* ============================================================ */
/* File: LIBEPC.H						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Purpose: Various #defines, structures, and function		*/
/* prototypes needed to use the corresponding library LIBEPC.A.	*/
/*								*/
/* Designed for use with the DJGPP port of the GNU C/C++	*/
/* protected mode 386 compiler.					*/
/*								*/
/* Modification History:					*/
/*								*/
/* ============================================================ */

#ifndef _LIBEPC_H_	/* Avoid multiple inclusions */
#define	_LIBEPC_H_

/* ------------------------------------------------------------ */
/* A few datatypes to make the operand size more obvious.	*/
/* ------------------------------------------------------------ */
typedef int			BOOL	;
typedef unsigned char		BYTE8	;
typedef unsigned short int	WORD16	;
typedef unsigned long int	DWORD32	;
typedef unsigned long long int	QWORD64	;
typedef signed long int		FIXED32	;       /* 16.16 Fixed-Point */
typedef signed long long int	FIXED64	;       /* 32.32 Fixed-Point */
typedef void			(*ISR)(void) ;	/* Pointer to an ISR */

/* ------------------------------------------------------------ */
/* Constants for use with datatype BOOL (above).		*/
/* ------------------------------------------------------------ */
#ifndef TRUE
#define	TRUE		1
#endif
#ifndef FALSE
#define	FALSE		0
#endif

/* ------------------------------------------------------------ */
/* Macros to extract the LSByte and MSByte of a WORD16 value	*/
/* ------------------------------------------------------------ */
#define	LSB(u)		((u) & 0xFF)
#define	MSB(u)		((u) >> 8)

/* ------------------------------------------------------------ */
/* Returns number of elements in an array. (Use in for loops.)	*/
/* ------------------------------------------------------------ */
#define	ENTRIES(a)	(sizeof(a)/sizeof(a[0]))

/* ------------------------------------------------------------ */
/* Declaration prefix to hide an object from the linker.	*/
/* ------------------------------------------------------------ */
#define	PRIVATE		static

/* ------------------------------------------------------------ */
/* Define a NULL pointer.					*/
/* ------------------------------------------------------------ */
#ifndef NULL
#define	NULL		((void *) 0)
#endif

/* ------------------------------------------------------------ */
/* 386 instructions needed when writing ISR's.	Note that IRET	*/
/* pops the pointer to the stack frame that was established by	*/
/* code that the compiler generates at every function entry.	*/
/* ------------------------------------------------------------ */
#define	PUSHCS		__asm__ __volatile__ ("PUSHL %CS") ;
#define	PUSHF		__asm__ __volatile__ ("PUSHFL")
#define	POPF		__asm__ __volatile__ ("POPFL")
#define	STI		__asm__ __volatile__ ("STI")
#define	CLI		__asm__ __volatile__ ("CLI")
#define	PUSHA		__asm__ __volatile__ ("PUSHAL")
#define	POPA		__asm__ __volatile__ ("POPAL")
#define	ENTER		__asm__ __volatile__ ("ENTER $0,$0")
#define	LEAVE		__asm__ __volatile__ ("LEAVE")
#define	IRET		__asm__ __volatile__ ("IRET")

/* ------------------------------------------------------------ */
/* Support for functions implemented in IO.ASM			*/
/* ------------------------------------------------------------ */
void		outportb(WORD16, BYTE8) ;
BYTE8		inportb(WORD16) ;
void		exit(int) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in INIT-CRT.C		*/
/* ------------------------------------------------------------ */
void *		LastMemoryAddress(void) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in INIT-IDT.C		*/
/* ------------------------------------------------------------ */
#define		IRQ_TICK	0
#define		IRQ_KYBD	1
#define		IRQ_COM2_COM4	3
#define		IRQ_COM1_COM3	4
#define		IRQ_FLOPPY	6
#define		IRQ_PAR_PORT	7
#define		IRQ_RTC		8
#define		IRQ_PS2_MOUSE	12
#define		IRQ_HARD_DISK	14

int		IRQ2INT(int irq) ;
ISR		GetISR(int int_numb) ;
void		SetISR(int int_numb, ISR isr) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in KEYBOARD.C		*/
/* ------------------------------------------------------------ */
BYTE8		GetScanCode(void) ;
BOOL		ScanCodeRdy(void) ;
BOOL		SetsKybdState(BYTE8) ;
WORD16		ScanCode2Ascii(BYTE8 code) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in SPEAKER.C		*/
/* ------------------------------------------------------------ */
void		Sound(int hertz) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in CYCLES.ASM		*/
/* ------------------------------------------------------------ */
QWORD64		CPU_Clock_Cycles(void) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in TIMER.C			*/
/* ------------------------------------------------------------ */
DWORD32		Milliseconds(void) ;
DWORD32		Now_Plus(int seconds) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in DISPLAY.C		*/
/* ------------------------------------------------------------ */
WORD16 *	Cell(int row, int col) ;
void		ClearScreen(BYTE8 attb) ;
char *		FormatUnsigned
		(char *bfr, unsigned val, int base, int width, char fill) ;
int		GetCursorCol(void) ;
int		GetCursorRow(void) ;
void		PutAttb(BYTE8 attb, int cells) ;
void		PutChar(char ch) ;
void		PutCharAt(char ch, int row, int col) ;
void		PutString(char *string) ;
void		PutUnsigned(unsigned val, int base, int width) ;
void		SetCursorPosition(int row, int col) ;
void		SetCursorVisible(BOOL visible) ;
char *		Unsigned2Ascii(char *bfr, unsigned val, int base) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in WINDOW.C		*/
/* ------------------------------------------------------------ */
typedef struct ROWCOL
	{
	int	first ;
	int	last ;
	int	cursor ;
	} ROWCOL ;

typedef struct WINDOW
	{
	ROWCOL	row ;
	ROWCOL	col ;
	char	title[1] ;
	} WINDOW ;

WINDOW *	WindowCreate(char *title, int row_first, int row_last,
			int col_first, int col_last) ;
void		WindowErase(WINDOW *w) ;
void		WindowPutChar(WINDOW *w, char ch) ;
void		WindowPutString(WINDOW *w, char *str) ;
void		WindowSelect(WINDOW *w) ;
void		WindowSetCursor(WINDOW *w, int row, int col) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in HEAP.C			*/
/* ------------------------------------------------------------ */
void *		malloc(long unsigned int) ;
void		free(void *) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in QUEUE.C			*/
/* ------------------------------------------------------------ */
typedef	struct QUEUE
	{
	int	item_size ;
	int	max_items ;
	int	item_count ;
	int	nq_index ;
	int	dq_index ;
	char	bfr[0] ;
	} QUEUE ;

QUEUE *		QueueCreate(int numb_items, int item_size) ;
BOOL		QueueInsert(QUEUE *q, void *data) ;
BOOL		QueueRemove(QUEUE *q, void *data) ;

/* ------------------------------------------------------------ */
/* Support for functions implemented in FIXEDPT.ASM		*/
/* ------------------------------------------------------------ */
#define FIXED32_ONE		65536L
#define FIXED32_PI		205887L
#define FIXED32_2PI		411775L
#define FIXED32_E		178144L
#define FIXED32_ROOT2		74804L
#define FIXED32_ROOT3		113512L
#define FIXED32_GOLDEN		106039L

#define FLOAT32(x)		((FIXED32) ((x) << 16))
#define TRUNC32(x)		((int) ((x) >> 16))
#define ROUND32(x)		((int) (((x) + 0x8000) >> 16))

FIXED32		Product32(FIXED32 multiplier, FIXED32 multiplicand) ;
FIXED32		Quotient32(FIXED32 dividend, FIXED32 divisor) ;
FIXED32		Inverse32(FIXED32 n) ;
FIXED32		Sqrt32(FIXED32 n) ;
FIXED64		Product64(FIXED64 multiplier, FIXED64 multiplicand) ;

#endif
