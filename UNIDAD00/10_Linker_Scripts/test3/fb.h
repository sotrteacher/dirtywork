#ifndef _FB_H
#define _FB_H

#define FB_COMMAND_PORT		0x3D4
#define FB_DATA_PORT		0x3D5

#define FB_HIGH_BYTE_CMD	14
#define FB_LOW_BYTE_CMD		15

#define FB_BLACK		0
#define FB_BLUE			1
#define FB_GREEN		2
#define FB_CYAN			3
#define FB_RED			4
#define FB_MAGENTA		5
#define FB_BROWN		6
#define FB_LIGHT_GREY		7
#define FB_DARK_GREY		8
#define FB_LIGHT_BLUE		9
#define FB_LIGHT_GREEN		10
#define FB_LIGHT_CYAN		11
#define FB_LIGHT_RED		12
#define FB_LIGHT_MAGENTA	13
#define FB_LIGHT_BROWN		14
#define FB_WHITE		15

#define FB_TERM_HEIGHT		24
#define FB_TERM_WIDTH		80
#define FB_NUM_CELLS		(FB_TERM_WIDTH * FB_TERM_HEIGHT)

#define miCRTPORT 0x3d4		/* (2015.11.14) */

struct framebuffer
{
	char		c;
	unsigned char	colors;
} __attribute__((packed));

void
fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);

void
fb_move_cursor(unsigned char pos);

void
fb_init(void);

void
fb_change_color(unsigned char fg, unsigned char bg); 

void
fb_write(unsigned char *buf, unsigned short len);

#endif /* _FB_H */
