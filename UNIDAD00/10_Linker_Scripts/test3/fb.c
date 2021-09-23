#include "types.h"
#include "x86.h"
#include "fb.h"
//#include "io.h"    /* se incluyen types.h y x86.h en lugar de io.h */
#include "device.h"
#include "defs.h"    /* memmove(), memset()  (2015.11.14) */
#include "memlayout.h" /* P2V(), (2015.11.14) */

static struct framebuffer *fb =		{0};
static unsigned char curr_color =	FB_BLACK | FB_WHITE;
static unsigned short cursor_idx =	0;
struct device fbdev =			{0, fb_write};

void
fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
	fb[i].c = c;
	fb[i].colors = (((fg & 0x0F) << 4) | (bg & 0x0F)); 
}

void
fb_move_cursor(unsigned char pos)
{
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_CMD);
	outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_CMD);
	outb(FB_DATA_PORT, pos & 0x00FF);
}

void
fb_init(void)
{
	register_device(fbdev);
	fb = (struct framebuffer *) 0x000B8000;
}

void
fb_change_color(unsigned char fg, unsigned char bg)
{
	curr_color &= (fg | bg);
}

void
fb_write(unsigned char *buf, unsigned short len)
{
	unsigned int i;
	unsigned char fg = curr_color & 0xF0;
	unsigned char bg = curr_color & 0x0F;
	for (i = 0; i < len; ++i)
	{
		fb_write_cell(cursor_idx, buf[i], fg, bg);
		cursor_idx = (cursor_idx + 1) % FB_NUM_CELLS;
	}
	fb_move_cursor(cursor_idx);
}


static ushort *mcrt=(ushort*)P2V(0xB8000); // memoria CGA
/* basado en void cgaputc(int c) de console.c */
//void mi_fb_write_cell(uint i, uchar c, uchar fg, uchar bg){
void mi_fb_write_cell(uchar c){
  int pos;
  outb(miCRTPORT, 14); /* posicion del cursor: col+80*row, QUE ES col? */
  pos=inb(miCRTPORT+1)<<8;
  outb(miCRTPORT, 15);
  pos|=inb(miCRTPORT+1);
  if(c=='\n'){
    pos+=80-pos%80;
  } else {
    mcrt[pos++]=(c&0xff)|0x0700;  /* negro sobre blanco */
  }
  if((pos/80)>=24){ /* scroll up */
    memmove(mcrt, mcrt+80, sizeof(mcrt[0])*(24*80-pos));
    pos-=80;
    memset(mcrt+pos, 0, sizeof(mcrt[0])*(24*80-pos));
  }
  outb(miCRTPORT, 14);
  outb(miCRTPORT+1, pos>>8);
  outb(miCRTPORT, 15);
  outb(miCRTPORT+1, pos);
  mcrt[pos]=' '|0x0700;
}
