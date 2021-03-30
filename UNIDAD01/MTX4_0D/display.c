/* ============================================================ */
/* File: DISPLAY.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Purpose: Library routines for access to the display.		*/
/* See file LIBEPC.H for function descriptions.			*/
/*								*/
/* Designed for use with the DJGPP port of the GNU C/C++	*/
/* protected mode 386 compiler.					*/
/*								*/
/* Modification History:					*/
/*								*/
/* ============================================================ */

#include "libepc.h"

#define	DISPLAY_ROWS		25
#define	DISPLAY_COLS		80

#define	CHAR_ATTB(ch, attb)	(((attb) << 8) | (ch))

PRIVATE int		Digits(unsigned, int) ;
PRIVATE void		Detect_CRTC(void) ;
PRIVATE void		CR(void) ;
PRIVATE void		LF(void) ;

PRIVATE WORD16 *	_display_buffer = 0 ;
PRIVATE int		_crtc_ndex_port ;
PRIVATE int		_crtc_data_port ;
PRIVATE int		_cursRow = 0 ;
PRIVATE int		_cursCol = 0 ;

#define	MDA_NDEX_PORT	0x3B4
#define	MDA_DATA_PORT	0x3B5

#define	CGA_NDEX_PORT	0x3D4
#define	CGA_DATA_PORT	0x3D5

#define	INDEX_MSB_CURS_POS	0x0E
#define	INDEX_LSB_CURS_POS	0x0F

#define	INDEX_CURS_START	0x0A

/* ------------------------------------------------------------ */
/* Public functions ...						*/
/* ------------------------------------------------------------ */

void SetCursorVisible(BOOL visible)
	{
	BYTE8 top ;

	Detect_CRTC() ;

	PUSHF ;
	CLI ;
	outportb(_crtc_ndex_port, INDEX_CURS_START) ;
	top = inportb(_crtc_data_port) ;
	if (visible) top &= ~0x20 ;
	else top |= 0x20 ;
	outportb(_crtc_ndex_port, INDEX_CURS_START) ;
	outportb(_crtc_data_port, top) ;
	POPF ;
	}


int GetCursorRow(void)
	{
	return _cursRow ;
	}

int GetCursorCol(void)
	{
	return _cursCol ;
	}

void SetCursorPosition(int row, int col)
	{
	WORD16 cursorPos ;

	Detect_CRTC() ;

	_cursRow = row ;
	_cursCol = col ;
	cursorPos = DISPLAY_COLS * row + col ;

	PUSHF ;
	CLI ;
	outportb(_crtc_ndex_port, INDEX_LSB_CURS_POS) ;
	outportb(_crtc_data_port, LSB(cursorPos)) ;
	outportb(_crtc_ndex_port, INDEX_MSB_CURS_POS) ;
	outportb(_crtc_data_port, MSB(cursorPos)) ;
	POPF ;
	}


void ClearScreen(BYTE8 attb)
	{
	WORD16 char_attb = CHAR_ATTB(' ', attb) ;
	WORD16 *p = Cell(0, 0) ;
	int cell ;

	for (cell = 0; cell < DISPLAY_ROWS * DISPLAY_COLS; cell++)
		{
		*p++ = char_attb ;
		}
	}

void PutAttb(BYTE8 attb, int cells)
	{
	BYTE8 *p ;

	p = (BYTE8 *) Cell(_cursRow, _cursCol) ;
	while (cells--)
		{
		p[1] = attb ;
		p += 2 ;
		}
	}

void PutCharAt(char ch, int row, int col)
	{
	*((BYTE8 *) Cell(row, col)) = ch ;
	}


void PutChar(char ch)
	{
	if (ch == '\r') CR() ;
	else if (ch == '\n') LF() ;
	else
		{
		*((BYTE8 *) Cell(_cursRow, _cursCol)) = ch ;
		//if (_cursCol >= DISPLAY_COLS - 1) CR(), LF() ;
		if (_cursCol >= DISPLAY_COLS - 1) {CR() ; LF() ;}
		else SetCursorPosition(_cursRow, ++_cursCol) ;
		}
	}


void PutString(char *string)
	{
	while (*string) PutChar(*string++) ;
	}


/* ---------------------------------------------------- */
/* Displays at least 'width' digits of a base 'base' 	*/
/* value at the current cursor position, zero-filled on	*/
/* the left.  If 'width' is zero, the number of digits	*/
/* will be the minimum necessary to display the number.	*/
/* ---------------------------------------------------- */

void PutUnsigned(unsigned val, int base, int width)
	{
	char bfr[50] ;
	FormatUnsigned(bfr, val, base, width, '0') ;
	PutString(bfr) ;
	}


/* ---------------------------------------------------- */
/* Converts unsigned to character string. Minimum field	*/
/* width given by 'width'; if negative, number is left-	*/
/* justified within field. Field is filled with 'fill'.	*/
/* ---------------------------------------------------- */
char *FormatUnsigned(char *bfr, unsigned val, int base, int width, char fill)
	{
	int digits = Digits(val, base) ;
	while (digits <  width) digits++, *bfr++ = fill ;
	bfr = Unsigned2Ascii(bfr, val, base) ;
	while (digits < -width) digits++, *bfr++ = fill ;
	*bfr = '\0' ;
	return bfr ;
	}

char *Unsigned2Ascii(char *bfr, unsigned val, int base)
	{
	static char digits[] = "0123456789ABCDEF" ;

	if (base <  2) base = 2 ;
	if (base > 16) base = 16 ;
	if (val >= base) bfr = Unsigned2Ascii(bfr, val / base, base) ;
	*bfr++ = digits[(int) (val % base)] ;
	*bfr = '\0' ;
	return bfr ;
	}

WORD16 *Cell(int row, int col)
	{
	Detect_CRTC() ;
	return &_display_buffer[DISPLAY_COLS * row + col] ;
	}

/* ------------------------------------------------------------ */
/* Private functions ...					*/
/* ------------------------------------------------------------ */

PRIVATE void CR(void)
	{
	SetCursorPosition(_cursRow, 0) ;
	}


PRIVATE void Copy(void *dst, void *src, int bytes)
	{
	register char *fm = src ;
	register char *to = dst ;
	while (bytes--) *to++ = *fm++ ;
	}

PRIVATE void LF(void)
	{
	static int bytes = (DISPLAY_ROWS - 1) * DISPLAY_COLS * sizeof(WORD16);
	static WORD16 char_attb = CHAR_ATTB(' ', 0x07) ;
	WORD16 *p ;
	int col ;

	if (_cursRow < DISPLAY_ROWS)
		{
		SetCursorPosition(++_cursRow, _cursCol) ;
		return ;
		}

	Copy(Cell(0, 0), Cell(1, 0), bytes) ;
	p = Cell(24, 0) ;
	for (col = 0; col < DISPLAY_COLS; col++) *p++ = char_attb ;
	}


PRIVATE void Detect_CRTC(void)
	{
	BYTE8 old, chk55, chkAA ;

	if (_display_buffer) return ;

	PUSHF ;
	CLI ;

	outportb(CGA_NDEX_PORT, INDEX_LSB_CURS_POS) ;
	old = inportb(CGA_DATA_PORT) ;

	outportb(CGA_NDEX_PORT, INDEX_LSB_CURS_POS) ;
	outportb(CGA_DATA_PORT, 0x55) ;

	outportb(CGA_NDEX_PORT, INDEX_LSB_CURS_POS) ;
	chk55 = inportb(CGA_DATA_PORT) ;

	outportb(CGA_NDEX_PORT, INDEX_LSB_CURS_POS) ;
	outportb(CGA_DATA_PORT, 0xAA) ;

	outportb(CGA_NDEX_PORT, INDEX_LSB_CURS_POS) ;
	chkAA = inportb(CGA_DATA_PORT) ;

	outportb(CGA_NDEX_PORT, INDEX_LSB_CURS_POS) ;
	outportb(CGA_DATA_PORT, old) ;

	POPF ;

	if (chk55 == 0x55 && chkAA == 0xAA)
		{
		_display_buffer = (WORD16 *) 0xB8000 ;
		_crtc_ndex_port = CGA_NDEX_PORT ;
		_crtc_data_port = CGA_DATA_PORT ;
		}
	else
		{
		_display_buffer = (WORD16 *) 0xB0000 ;
		_crtc_ndex_port = MDA_NDEX_PORT ;
		_crtc_data_port = MDA_DATA_PORT ;
		}
	}

PRIVATE int Digits(unsigned val, int base)
	{
	register int digits ;

	digits = 1 ;
	while (val >= base)
		{
		val /= base ;
		digits++ ;
		}
	return digits ;
	}

