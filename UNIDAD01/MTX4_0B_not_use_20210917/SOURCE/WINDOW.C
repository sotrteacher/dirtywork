/* ============================================================ */
/* File: WINDOW.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Purpose: Library routines for managing windows.		*/
/* See file LIBEPC.H for function descriptions.			*/
/*								*/
/* Designed for use with the DJGPP port of the GNU C/C++	*/
/* protected mode 386 compiler.					*/
/*								*/
/* Modification History:					*/
/*								*/
/* ============================================================ */

#include <ctype.h>
#include "libepc.h"

PRIVATE void DrawFrame(WINDOW *w) ;
PRIVATE void CR(WINDOW *w) ;
PRIVATE void LF(WINDOW *w) ;
PRIVATE void HT(WINDOW *w) ;
PRIVATE void BS(WINDOW *w) ;

WINDOW *WindowCreate
(char *title, int row_first, int row_last, int col_first, int col_last)
	{
	BOOL frame ;
	WINDOW *w ;

	/* Omit frame if title is a NULL pointer! */
	frame = title != NULL ;
	if (!title) title = "" ;

	w = (WINDOW *) malloc(sizeof(WINDOW) + strlen(title)) ;
	if (!w) return NULL ;

	w->row.first	= row_first ;
	w->row.last	= row_last ;
	w->row.cursor	= 0 ;

	w->col.first	= col_first ;
	w->col.last	= col_last ;
	w->col.cursor	= 0 ;

	strcpy(w->title, title) ;

	if (frame) DrawFrame(w) ;
	WindowErase(w) ;

	w->row.cursor = w->row.first ;
	w->col.cursor = w->col.first ;

	return w ;
	}

void WindowErase(WINDOW *w)
	{
	int row, col ;

	for (row = w->row.first; row <= w->row.last; row++)
		{
		for (col = w->col.first; col <= w->col.last; col++)
			{
			PutCharAt(' ', row, col) ;
			}
		}
	}

void WindowSelect(WINDOW *w)
	{
	SetCursorPosition(w->row.cursor, w->col.cursor) ;
	}

void WindowSetCursor(WINDOW *w, int row, int col)
	{
	w->row.cursor = w->row.first + row ;
	w->col.cursor = w->col.first + col ;
	}

void WindowPutChar(WINDOW *w, char ch)
	{
	switch (ch)
		{
		case '\r':	CR(w) ;	break ;
		case '\n':	LF(w) ;	break ;
		case '\t':	HT(w) ;	break ;
		case '\b':	BS(w) ; break ;
		default:
			if (!isprint(ch) && ch < (char) 0x80) return ;
			*((char *) Cell(w->row.cursor, w->col.cursor)) = ch ;
			if (w->col.cursor >= w->col.last)
				{
				CR(w) ;
				LF(w) ;
				break ;
				}
			++w->col.cursor ;
		}
	}

void WindowPutString(WINDOW *w, char *str)
	{
	while (*str) WindowPutChar(w, *str++) ;
	}

PRIVATE void DrawFrame(WINDOW *w)
	{
	int row, col ;
	char *p ;

	/* Paint Top Row */
	row = w->row.first ; col = w->col.first ;
	PutCharAt('Ú', row, col++) ;
	while (col < w->col.last) PutCharAt('Ä', row, col++) ;
	PutCharAt('¿', row, col) ;

	/* Paint Middle Rows */
	for (row = w->row.first + 1; row < w->row.last; row++)
		{
		PutCharAt('³', row, w->col.first) ;
		PutCharAt('³', row, w->col.last) ;
		}

	/* Paint Botom Row */
	row = w->row.last ; col = w->col.first ;
	PutCharAt('À', row, col++) ;
	while (col < w->col.last) PutCharAt('Ä', row, col++) ;
	PutCharAt('Ù', row, col) ;

	/* Paint Title */
	row = w->row.first ;
	col = w->col.first + 2 ;
	PutCharAt('[', row, col++) ;
	for (p = w->title; *p; p++) PutCharAt(*p, row, col++) ;
	PutCharAt(']', row, col) ;

	w->row.first++ ; w->col.first++ ;
	w->row.last-- ;  w->col.last-- ;
	}

PRIVATE void CR(WINDOW *w)
	{
	w->col.cursor = w->col.first ;
	}

PRIVATE void LF(WINDOW *w)
	{
	int bytes, row, col ;
	WORD16 *p ;

	if (w->row.cursor < w->row.last)
		{
		++w->row.cursor ;
		return ;
		}

	col = w->col.first ;
	bytes = 2 * (w->col.last - w->col.first + 1) ;
	for (row = w->row.first; row < w->row.last; row++)
		{
		memcpy(Cell(row, col), Cell(row + 1, col), bytes) ;
		}

	p = Cell(w->row.last, col = w->col.first) ;
	while (col++ <= w->col.last) *p++ = 0x0720 ;
	}

PRIVATE void HT(WINDOW *w)
	{
	int col = (w->col.cursor & ~0x7) + 8 ;
	if (col <= w->col.last) w->col.cursor = col ;
	}

PRIVATE void BS(WINDOW *w)
	{
	if (w->col.cursor == w->col.first) return ;
	*((char *) Cell(w->row.cursor, --w->col.cursor)) = ' ' ;
	}

