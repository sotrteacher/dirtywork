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
/***********************************************************************
                      io.c file of MTX
***********************************************************************/
#ifndef MK
 #include "../type.h"
#endif

extern PROC *running;

char dummy[64];
char *ctable = "0123456789ABCDEF";

int align(u32 x)
{
  int count, i;
  count = 10;
  if (x==0) 
     count = 9;
  while (x){
    count--;
    x = (u32)(x/10);
  }

  while(count){
    putc(' ');
    count--;
  }
}

int rpi(int x)
{
   char c;
   if (x==0) return;
   c = ctable[x%10];
   rpi(x/10);
   putc(c);
}
  
int printi(int x)
{
    if (x==0){
       prints("0 ");
       return;
    }
    if (x < 0){
       putc('-');
       x = -x;
    }
    rpi(x);
    putc(' ');
}

int rpu(u16 x)
{
   char c;
   if (x==0) return;
   c = ctable[x%10];
   rpi(x/10);
   putc(c);
}

int printu(u16 x)
{
    if (x==0){
       prints("0 ");
       return;
    }
    rpu(x);
    putc(' ');
}

int rpli(long x)
{
   char c;
   if (x==0) return;
   c = ctable[x%10];
   rpli((long)x/10);
   putc(c);
}

int printli(u32 x)
{
    if (x==0){
       prints("0 ");
       return;
    }
    if (x < 0){
       putc('-');
       x = -x;
    }
    rpli((u32)x);
    putc(' ');
}

int rpx(u16 x)
{
   char c;
   if (x==0) return;
   c = ctable[x%16];
   rpx(x/16);
   putc(c);
}

int printx(u16 x)
{  
  prints("0x");
   if (x==0){
      prints("0 ");
      return;
   }
   rpx(x);
  putc(' ');
}

int rplx(u32 x)
{
   char c;
   if (x==0) return;
   c = ctable[x%16];
   rplx((u32)x/16);
   putc(c);
}

int printlx(u32 x)
{
   prints("0x");
   if (x==0){
      prints("0 ");
      return;
   }
   rplx((u32)x);
  putc(' ');
}

prints(char *s)
{
   while (*s){
      putc(*s);
      s++;
   }
}

pns(char *s, int n)
{ 
   while (*s && n){
      putc(*s);
      s++; n--;
   }
}

mgets(char *s)
{   char c; int len=0; 
    while ( (c=kgetc()) != '\n' && len < 64){
           *s++ = c; len++;
           //putc(c); 
    }
    //prints("\n\r");
    *s = '\0';
}


int rpl(u32 x)
{
  char c;
  if (x==0)
    return;
  c = ctable[x % 10];
  rpl((u32)(x/10));
  putc(c);
}

int printl(u32 x)
{
  align(x);
  if (x==0){
    putc('0'); putc(' ');
    return;
  }
  rpl((u32)x);
  putc(' ');
}

int printf(char *fmt)
{
  char   *cp;
  u16 *ip;
  u32  *up;
 
  cp = fmt;
  ip = (int *)&fmt + 1;

  while (*cp){
    if (*cp != '%'){
      putc(*cp);
      if (*cp=='\n')
	putc('\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
      case 'd' : printi(*ip); break;
      case 'u' : printu(*ip); break;
      case 'x' : printx(*ip); break;
      case 'l' : up = (u32 *)ip;
                 printl(*up); ip++; break;
      case 's' : prints(*ip); break;
      case 'c' : putc(*ip);   break;
    }
    cp++; ip++;
  }
}


// From Kmode, print to U space, depending on running's tty

int putcCon(char c)
{
  putc(c);
}

int putcS0(char c)
{
  bputc(0x3F8, c);
}

int putcS1(char c)
{
  bputc(0x2F8, c);
}

struct putcsw{
  int (*f)();
} putcsw[];

struct putcsw putcsw[] =
{
  putcCon,
  putcS0,
  putcS1
};

int myttynum()
{
  int tty = 0;
  if (strcmp(running->res->tty, "/dev/ttyS0")==0)
    tty = 1;
  if (strcmp(running->res->tty, "/dev/ttyS1")==0)
    tty = 2;
  return tty;
}

int cps2u(char *s, char *y)
{
  while(*s){
    put_ubyte(*s, y);
    s++; y++;
  }
  put_ubyte(0, y);
}

int ps2u(char *s)
{
  int tty = myttynum();

  while(*s){
    (*putcsw[tty].f)(*s);
    s++;
  }
}

int rpiu(int x, int tty)
{
  char c;
  if (x){
    c = ctable[x % 10];
    rpiu(x/10, tty);
    (*putcsw[tty].f)(c);
  }
}

int pi2u(int x)
{
  int tty = myttynum();
  
  if (x==0){
    (*putcsw[tty].f)('0');
    return;
  }
  if (x<0){
    (*putcsw[tty].f)('-');
    x = -x;
  }
  rpiu(x, tty);
}

int rpx2u(u16 x, int tty)
{
   char c;
   if (x){
     c = ctable[x%16];
     rpx2u(x/16, tty);
     (*putcsw[tty].f)(c);
   }
}

int px2u(u16 x)
{
  int tty = myttynum();
  ps2u("0x");
  if (x==0){
    ps2u("0");
    return;
  }
  rpx2u(x, tty);
}

int pc2u(char c)
{
  int tty = myttynum;
  (*putcsw[tty].f)(c);
}


int rpl2u(u32 x, int tty)
{
  char c;
  if (x){
     c = ctable[x % 10];
     rpl(x/10, tty);
     (*putcsw[tty].f)(c);
  }
}

int pl2u(u32 x)
{
  int tty = myttynum();

  if (x==0){
    pc2u('0');
    return;
  }
  rpl2u(x,tty);
}

int printf2u(char *fmt)
{
  char   *cp;
  u16 *ip;
  u32  *up;
 
  cp = fmt;
  ip = (int *)&fmt + 1;

  while (*cp){
    if (*cp != '%'){
      putc(*cp);
      if (*cp=='\n')
	putc('\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
      case 'd' : pi2u(*ip); break;
      case 'x' : px2u(*ip); break;
      case 'l' : up = (u32 *)ip;
                 pl2u(*up); ip++; break;
      case 's' : ps2u(*ip); break;
      case 'c' : pc2u(*ip); break;
    }
    cp++; ip++;
  }
}



int get_word(u16 segment, u16 offset)
{
  u16 word; 
  u16 ds = getds();
  setds(segment);
  word = *(u16 *)offset;
  setds(ds);
  return word;
}

u8 get_byte(u16 segment, u16 offset)
{
  u8 byte; 
  u16 ds = getds();
  setds(segment);
  byte = *(u8 *)offset;
  setds(ds);
  return byte;
}

int put_byte(u8 byte, u16 segment, u16 offset)
{
  u16 ds = getds();
  setds(segment);
  *(u8 *)offset = byte;
  setds(ds);
  return byte;
}

int put_word(u16 word, u16 segment, u16 offset)
{
  u16 ds = getds();
  setds(segment);
  *(u16 *)offset = word;
  setds(ds);
}


int get_ubyte(u16 offset)
{
  return get_byte(running->uss, offset);
}

int put_ubyte(u8 byte, u16 offset)
{
  return put_byte(byte, running->uss, offset);
}

int get_uword(u16 offset)
{
  return get_word(running->uss, offset);
}

int put_uword(u16 word, u16 offset)
{
  return put_word(word, running->uss, offset);
}

