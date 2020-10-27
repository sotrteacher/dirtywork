/********************************************************************
Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

ushort base = 0xB800;
ushort offset = 0;
int    color;
int vid, org, row, column;

#define C_6845         0x3D0	/* port for 6845 color */

#define VINDEX             4	/* 6845's index register */
#define VDATA              5	/* 6845's data register */
#define CUR_SIZE          10	/* 6845's cursor size register */
#define VID_ORG           12	/* 6845's origin register */
#define CURSOR            14	/* 6845's cursor position register */

#define LINE_WIDTH        80	/* # characters on a line */
#define SCR_LINES         25	/* # lines on the screen */
#define SCR_BYTES	8000	/* size video RAM. multiple of 2*LINE_WIDTH */

#define CURSOR_SHAPE      15    /* block cursor for MDA/HGC/CGA/EGA/VGA... */

int vid_port;
// vid_init() initializes the display org=0 (row,col)[(0,0)
int vid_init()
{ 
  int i, w;
  org = 0;
  vid = 0;
  row = 0; column = 0;
  color = 0x0A;
 
  vid_port = C_6845;
  set_6845(CUR_SIZE, CURSOR_SHAPE);  // set cursor size	
  set_6845(VID_ORG, 0);              // display origin to 0
  set_6845(CURSOR, 0);	             // set cursor position to 0
  //clear screen

  w = 0x0700;    // blank            // attribute byte=0000 0111=0000 0RGB
  for (i=0; i<24*80; i++){                               
    put_word(w, base, 0+2*i);        // write 24*80 blanks to VGA memory
  }  
}

ushort vid_mask = 0x3FFF;

/*************************************************************************
 scroll(): scroll UP one line by
**************************************************************************/

int scroll()
{
  int i;
  ushort w, bytes, amount;  

  bytes = 2*23*80;             // where to start write : begin of LAST line
  offset = org + bytes;        // offset = CURRENT org + start position

  w = 0x0700;                  // blank char
  for (i=70; i<80; i++)        // blank out col. 70-79 of line 23
    put_word(w, base, offset + 2*i);


   bytes = 2*24*80;             // 
  // Normal scrolling using the 6845 registers.
  amount = 2 * LINE_WIDTH;

  offset = org + (SCR_LINES + 1) * LINE_WIDTH * 2;

  if (offset > vid_mask) {
    //scr_up(vid_base, org + LINE_WIDTH * 2, 0, 
    //        (SCR_LINES - 1) * LINE_WIDTH);
    // copy 24*80 words from org+160 to 0

    for (i=0; i<24*80; i++){
      w = get_word(base, org+160+2*i);
      put_word(w, base, 0+2*i);
    }  
    org = 0;

  }
  else
     org += 2 * LINE_WIDTH;

  offset = org + bytes;

  // Blank the new line at top or bottom.
  // vid_copy(NIL_PTR, vid_base, offset, LINE_WIDTH);
  //    copy BLANKs to 0xB800:offset; n_words

  w = 0;
  w = w | 0x0C00;

  for (i=0; i<80; i++)                  
    put_word(w, base, offset + 2*i);

  set_6845(VID_ORG, org >> 1);	  /// 6845 thinks in words 

}

int kcolor(int y) // syscall setcolor entry point
{
  color = y;  return 0;
}

int kputc(char c)
{
  putc(c);
}

int putuc(char c)
{
  // write.c sets color=running->uid+10
  putc(c);
}


int putc(char c)
{
  // display c to cursor position, advance cursor
  
  int pos, w, offset;
  //color = (running->pid % 5) + 10;
  if (c=='\n'){
    row++;
    if (row>=25){
      row = 24;
      scroll();
    }

    pos = 2*(row*80 + column);
    offset = (org + pos) & vid_mask;
    set_6845(CURSOR, offset >> 1);
    return; 
  }

  if (c=='\r'){
    column=0;

    pos = 2*(row*80 + column);
    offset = (org + pos) & vid_mask;
    set_6845(CURSOR, offset >> 1);
    return;
  }

  if (c=='\b'){
    if (column > 0){
      column--;

      pos = 2*(row*80 + column);
      offset = (org + pos) & vid_mask;
      put_word(0x0700, base, offset);

      set_6845(CURSOR, offset >> 1); 
    }
    return;
  }

  pos = 2*(row*80 + column);
  offset = (org + pos) & vid_mask;
  w = color;
  w = (w << 8) + c;
  //  w = c;
  //w = w | 0X0B00;

  put_word(w, base, offset);

  column++;

  if (column >= 80){
    column = 0;
    row++;

    if (row>=25){
      row = 24;
      scroll();
    }
  }

  pos = 2*(row*80 + column);
  offset = (org + pos) & vid_mask;
  set_6845(CURSOR, offset >> 1);
}     

/*===========================================================================*
 *				move_to					     *
 *===========================================================================*/
int move_to(int x, int y) // col (0 <= x <= 79), row (0 <= y <= 24, 0 at top)
{
  if (x < 0 || x >= LINE_WIDTH || y < 0 || y >= SCR_LINES) return;
  column = x;		/* set x co-ordinate */
  row = y;		/* set y co-ordinate */
  vid = (org + 2*y*LINE_WIDTH + 2*x);
  set_6845(CURSOR, vid >> 1);	/* cursor counts in words */
}


/*===========================================================================*
 *				set_6845				     *
 *===========================================================================*/
int set_6845(int reg, int val) 
{
/* Set a register pair inside the 6845.  
 * Registers 10-11 control the format of the cursor (how high it is, etc).
 * Registers 12-13 tell the 6845 where in video ram to start (in WORDS)
 * Registers 14-15 tell the 6845 where to put the cursor (in WORDS)
 *
 * Note that registers 12-15 work in words, i.e. 0x0000 is the top left
 * character, but 0x0001 (not 0x0002) is the next character.  This addressing
 * is different from the way the 8088 addresses the video ram, where 0x0002
 * is the address of the next character.
 */
//  lock();			/* try to stop h/w loading in-between value */
  out_byte(vid_port + VINDEX, reg);	/* set the index register */
  out_byte(vid_port + VDATA, (val>>8) & 0xFF);	/* output high byte */
  out_byte(vid_port + VINDEX, reg + 1);	/* again */
  out_byte(vid_port + VDATA, val&0xFF);	/* output low byte */
  //  unlock();
}

/********************* Main Source of Infomation  ************************
www.osdever.net/FreeVGA : home of FreeVGA programming information
www.osdever.net/FreeVGA/vga/crtcreg.htm : CRT Controller Registers

Index 00h -- Horizontal Total Register 
     Index 01h -- End Horizontal Display Register 
     Index 02h -- Start Horizontal Blanking Register 
     Index 03h -- End Horizontal Blanking Register 
     Index 04h -- Start Horizontal Retrace Register 
     Index 05h -- End Horizontal Retrace Register 
     Index 06h -- Vertical Total Register 
     Index 07h -- Overflow Register 
     Index 08h -- Preset Row Scan Register 
     Index 09h -- Maximum Scan Line Register 
     Index 0Ah -- Cursor Start Register 
     Index 0Bh -- Cursor End Register 
     Index 0Ch -- Start Address High Register 
     Index 0Dh -- Start Address Low Register 
     Index 0Eh -- Cursor Location High Register 
     Index 0Fh -- Cursor Location Low Register 
     Index 10h -- Vertical Retrace Start Register 
     Index 11h -- Vertical Retrace End Register 
     Index 12h -- Vertical Display End Register 
     Index 13h -- Offset Register 
     Index 14h -- Underline Location Register 
     Index 15h -- Start Vertical Blanking Register 
     Index 16h -- End Vertical Blanking 
     Index 17h -- CRTC Mode Control Register 
     Index 18h -- Line Compare Register 
Among these, only Registers 
      10-11  control CURSOR height (# of scand lines)
      12-13  display origin address
      14-15  CURSOR position
are of interests here:
**************************************************************************/
