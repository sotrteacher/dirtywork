/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#include <asm.h>
#include <scr.h>
#include <libcc.h>
#if 0 //LMC 2021.10.22
static int csr_x = 0;
static int csr_y = 0;
#else
int csr_x = 0;
int csr_y = 0;
int *csr_x_Pt = &csr_x;
int *csr_y_Pt = &csr_y; 
#endif

static void 
scroll(int lines) {
	int x = MAX_COLUMNS-1, y = MAX_LINES*(lines-1)+MAX_LINES-1;
	short *p = (short *)(VIDEO_RAM+CHAR_OFF(x, y));
	int i = MAX_COLUMNS*(lines-1) + MAX_COLUMNS;
	memcpy((void *)VIDEO_RAM, (void *)(VIDEO_RAM+LINE_RAM*lines),
		   LINE_RAM*(MAX_LINES-lines));
	for (; i>0; --i)
		*p-- = (short)((BLANK_ATTR<<8)|BLANK_CHAR);
}

void 
set_cursor(int x, int y) {
	csr_x = x;
	csr_y = y;
	outb(0x0e, 0x3d4);
	outb(((csr_x+csr_y*MAX_COLUMNS)>>8)&0xff, 0x3d5);
	outb(0x0f, 0x3d4);
	outb(((csr_x+csr_y*MAX_COLUMNS))&0xff, 0x3d5);
}

void
get_cursor(int *x, int *y) {
//goto out_get_cursor;
	*x = csr_x;
	*y = csr_y;
out_get_cursor:	
	return;
}

void 
print_c(char c, COLOUR fg, COLOUR bg) {
	char *p; 
	char attr;
	p = (char *)VIDEO_RAM+CHAR_OFF(csr_x, csr_y);
	attr = (char)(bg<<4|fg);
	switch (c) {
	case '\r':
		csr_x = 0;
		break;
	case '\n':
		for (; csr_x<MAX_COLUMNS; ++csr_x) {
			*p++ = BLANK_CHAR;
			*p++ = attr;
		}
		break;
	case '\t':
		c = csr_x+TAB_WIDTH-(csr_x&(TAB_WIDTH-1));
		c = c<MAX_COLUMNS?c:MAX_COLUMNS;
		for (; csr_x<c; ++csr_x) {
			*p++ = BLANK_CHAR;
			*p++ = attr;
		}
		break;
	case '\b':
		if ((! csr_x) && (! csr_y))
			return;
		if (! csr_x) {
			csr_x = MAX_COLUMNS - 1;
			--csr_y;
		} else
			--csr_x;
		((short *)p)[-1] = (short)((BLANK_ATTR<<8)|BLANK_CHAR);
		break;
	default:
		*p++ = c; 
		*p++ = attr;
		++csr_x;
		break;
	}
	if (csr_x >= MAX_COLUMNS) {
		csr_x = 0;
		if (csr_y < MAX_LINES-1)
			++csr_y;
		else 
			scroll(1);
	}
	set_cursor(csr_x, csr_y);
}

