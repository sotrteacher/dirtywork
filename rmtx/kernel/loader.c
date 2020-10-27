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
/*******************************************************
*                  @ new loader:                       *
*******************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h" 

int SEP;
typedef struct h{
  long sep, pad, tsize, dsize, bsize, zero, tot, symsize;
} HEADER;


u16 header(char *filename, u16 *dsize, u16 *bsize)
{ 
   int fd, n;

   char header[32];
   HEADER *hp;

   fd = myopen(filename, 0);
   if (fd<0){
       printf("%d open %s fail\n", running->pid, filename);
       return 0;
   }
   hp = &header;
   n = myread(fd, hp, 32, 1);

   SEP = 0;
   if (hp->sep & 0x00200000)
      SEP = 1;
   *dsize=(u16)hp->dsize; *bsize=(u16)hp->bsize;

   /*
   printf("[tsize dsize bsize tot]=[%l %l %l %l\n", 
	  hp->tsize, hp->dsize, hp->bsize, hp->tot);
   */
 
  myclose(fd);

  //V(&mutex);
  return (u16)hp->tsize;
}

int loadseg(int fd, u16 segment, u16 start, u16 count) 
{
  u16 saveuss, i, j, blks, rem, ct;

  /*
  if (start==32)
    printf("load code: "); 
  else
    printf("load data: %x %l %l\n",segment,(long)start,(ulong)count);
  */
  saveuss = running->uss;
  running->uss = segment;

  mylseek(fd, (long)start, 0);

  blks = count / 1024;
  rem = count % 1024;

  //printf(%d %d", blks, rem);

  for (i=0; i<blks; i++){
    j = i*1024;
    myread(fd, j, 1024, 0);
  }

  j = i*1024;
  myread(fd, j, rem, 0);

  running->uss = saveuss;
}

int clear_bss(u16 segment, u16 dsize, u16 bsize)
{
   u16 i,j, seg, rem;
   seg = segment + dsize/16;
   rem = dsize % 16;

   for (i=0; i<rem; i++)
     put_byte(0, seg, i);

   for (j=0; j<bsize; j++)
    put_byte(0, seg, j + i);
}

int load(char *filename, u16 segment)
{ 
   int fd;
   u16 blks, i, j, offset, start;
   u16 tsize,dsize,bsize;

   //printf("%d loader %s  %x", running->pid, filename,segment);

   tsize = header(filename,&dsize,&bsize); // set tsize, dsize, bsize

   fd = myopen(filename, 0); // open for READ again
   if (fd < 0) return 0;
   //printf("fd=%d", fd);

   if (!SEP){
     loadseg(fd, segment, 32, tsize+dsize+bsize);
   }
   else{
     loadseg(fd, segment, 32, tsize);
     loadseg(fd, segment+tsize/16, 32+tsize, dsize+bsize);
   }
   clear_bss(segment+tsize/16, dsize, bsize);

   myclose(fd);
}

