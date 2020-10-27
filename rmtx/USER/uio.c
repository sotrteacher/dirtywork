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
#include "../type.h"

char *ctable = "0123456789ABCDEF.................";

int alignlong = 1;

int putc(c) char c;
{
   write(1, &c, 1);
   if (c=='\n')
      putc('\r');
}

eatpat(line,components,name,n)  
   char *line, *components, *name[]; int *n; 
{
  int i; char *cp;
  strcpy(components, line);
  *n = 0; 
  for (i=0; i<16; i++)
      name[i]=0;

  cp = components;
  while (*cp != 0){
       while (*cp == ' ')
              *cp++ = 0;       
       if (*cp != 0)
           name[(*n)++] = cp; 
       while (*cp != ' ' && *cp != 0)
	       cp++; 
       if (*cp != 0)       
	   *cp = 0;        
       else
           break; 
       cp++;
  }

  /*
  for (i=0; i<*n; i++){
      if (name[i]){
         prints(name[i]); prints("  ");
      }
  }
  prints("\n\r");
  */
}

int printc(fd, c) int fd; char c;
{
  return write(fd, &c, 1);
}

int rpo(int fd, u16 x)
{
   char c;
   if (x==0) return;
   c = ctable[x%8];
   rpo(fd, x/8);
   printc(fd, c);
}

int printo(int fd, u16 x)
{
  if (x==0){
    printc(fd, '0'); printc(fd, ' ');
     return;
  }
  rpo(fd, x);
  printc(fd, ' ');
}

int rpi(int fd, int x)
{
   char c;
   if (x==0) return;
   c = ctable[x%10];
   rpi(fd, x/10);
   printc(fd, c);
}
  
int printi(int fd, int x)
{
    if (x==0){
       prints(fd, "0 ");
       return;
    }
    if (x < 0){
       printc(fd, '-');
       x = -x;
    }
    rpi(fd, x);
    printc(fd, ' ');
}

int rpu(int fd, u16 x)
{
   char c;
   if (x==0) return;
   c = ctable[x%10];
   rpu(fd, x/10);
   printc(fd, c);
}
  
int printu(int fd, u16 x)
{
    if (x==0){
       prints(fd, "0 ");
       return;
    }
    rpu(fd, x);
    printc(fd, ' ');
}

int rpli(int fd, long x)
{
   char c;
   if (x==0) return;
   c = ctable[x%10];
   rpli(fd, (long)(x/10));
   printc(fd, c);
}

int printli(int fd, long x)
{
    if (x==0){
       prints(fd, "0 ");
       return;
    }
    if (x < 0){
       printc(fd, '-');
       x = -x;
    }
    rpli(fd, (long)x);
    printc(fd, ' ');
}

int rpx(int fd, u16 x)
{
   char c;
   if (x==0) return;
   c = ctable[x%16];
   rpx(fd, x/16);
   printc(fd, c);
}

int printx(int fd, u16 x)
{  
    prints(fd, "0x");
    if (x==0){
      prints(fd, "0 ");
      return;
   }
   rpx(fd, x);
   printc(fd, ' ');
}

pns(s, n) char *s; int n;
{ 
   while (*s && n){
      putc(*s);
      s++; n--;
   }
}
/***************
gets(s) char *s;
{   
    char c; int len=0; 
    while ( (c=getc()) != '\r' && len < 64){
           *s++ = c; len++;
           putc(c); 
    }
    prints("\n\r");
    *s = '\0';
}
*****************/

int rpl(int fd, u32 x)
{
  char c;
  if (x==0)
    return;
  c = ctable[x % 10];
  rpl(fd, (u32)(x/10));
  printc(fd, c);
}

int align(u32 x)
{
  int count, i;
  count = 8;
  if (x==0) 
     count = 7;
  while (x){
    count--;
    x = (u32)(x/10);
  }

  while(count){
    putc(' ');
    count--;
  }
}

int printl(int fd, u32 x)
{
  if (alignlong)
     align(x);
  if (x==0){
    printc(fd,'0'); printc(fd,' ');
    return;
  }
  rpl(fd, (u32)x);
  printc(fd, ' ');
}

int rplx(int fd, u32 x)
{
   char c;
   if (x==0) return;
   c = ctable[x%16];
   rplx(fd, (u32)x/16);
   printc(fd, c);
}

int printlx(int fd, u32 x)
{
  prints(fd,"0x");
   if (x==0){
      prints(fd,"0 ");
      return;
   }
   rplx(fd, (u32)x);
  putc(' ');
}




prints(int fd, char *s)
{
  write(fd, s, strlen(s));
}


int printf(char *fmt)
{
  return fprintf(1,&fmt);
}

int print2f(char *fmt)
{
  return fprintf(2,&fmt);
}


int fprintf(int fd, int *fmt)
{
  char   *cp; int *ccp;
  ushort *ip;
  ulong  *up;
  
  cp = *fmt;
  ip = fmt + 1;

  while (*cp){
    if (*cp != '%'){
      printc(fd, *cp);
      if (*cp=='\n')
	printc(fd, '\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
      case 'd' : printi(fd, *ip); break;
      case 'u' : printu(fd, *ip); break;

      case 'x' : printx(fd, *ip); break;
      case 'l' : up = (u32 *)ip;
                 printl(fd, *up); ip++; break;
      case 'L' : up = (u32 *)ip;
                 printlx(fd, *up); ip++; break;
      case 's' : prints(fd, *ip);   break;
      case 'c' : printc(fd, *ip);   break;
    }
    cp++; ip++;
  }
}

int atoi(char *s)
{
  int v;
  v = 0;
  while(*s){
    v = 10*v + (*s - '0');
    s++;
  }
  return v;
}

int atoo(char *s)
{
  int v;
  v = 0;
  while(*s){
    v = 8*v + (*s - '0');
    s++;
  }
  return v;
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

