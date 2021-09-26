/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#ifndef SCR_H
#define SCR_H

#define MAX_LINES	25
#define MAX_COLUMNS	80
#define TAB_WIDTH	8			/* must be power of 2 */

/* color text mode, the video ram starts from 0xb8000,
   we all have color text mode, right? :) */
#define VIDEO_RAM	0xb8000

#define LINE_RAM	(MAX_COLUMNS*2)

#define PAGE_RAM	(MAX_LINE*MAX_COLUMNS)

#define BLANK_CHAR	(' ')
#define BLANK_ATTR	(0x07)		/* white fg, black bg */

#define CHAR_OFF(x,y)	(LINE_RAM*(y)+2*(x))

typedef enum COLOUR_TAG {
	BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE,
	GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, 
	LIGHT_RED, LIGHT_MAGENTA, YELLOW, BRIGHT_WHITE
} COLOUR;

void set_cursor(int, int);
void get_cursor(int *, int *);
void print_c(char, COLOUR, COLOUR);

#endif
