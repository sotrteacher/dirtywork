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
/**********
 Assume a.out with header: 8 longs
        0 0x04100301   (combined I,D space) or  0x04200301
        1 0x00000020
        2 text size
        3 data size
        4 bss  size
        5 0x00000000
        6 total memory 
        7 symbolTable size
***/
struct h{
  long sep;
  long gap;
  long tsize;
  long dsize;
  long bsize;
  long zero;
  long tot;
  long ssize;
}*hp;

int fd, gd, n, count, i;
char buf[1024], f[64];
unsigned short *up;
long fsize;
char zero[1024];
char ZERO = 0;

main(argc, argv) int argc; char *argv[];
{
  printf("f=%s\n", argv[1]);
  fd = open(argv[1], 2);
 
  n = read(fd, buf, 1024);
  hp = (struct h *)buf;
  printf("sep   = 0x%08x\n", hp->sep);
  if (hp->sep == 0x10100301)
     printf("non sep\n");
  else
     printf("sep\n");

  printf("tsize  = %d\n", hp->tsize);
  printf("dsize  = %d\n", hp->dsize);
  printf("bsize  = %d\n", hp->bsize);
  printf("totMem = %d\n", hp->tot);
  printf("symsiz = %d\n", hp->ssize);
  fsize = hp->tsize + hp->dsize + hp->ssize + 32;
  printf("fsize  = %d\n\n", fsize);

  printf("tsize = %04x  clicksize=%04x\n", hp->tsize, hp->tsize/16);
  printf("dsize = %04x\n", hp->dsize);
  printf("bsize = %04x\n", hp->bsize);
  up = (unsigned short *)buf;
  up += 17;

  // up point at 2*8 + 1 (for jmp start at 0)==> offset 2 in MTX

  *up = 0x1000 + hp->tsize/16;  // word 2 = DS segment value

  *(up+1) = hp->dsize;          // word 3 = dsize
 
  *(up+2) = hp->bsize;          // word 4 = bsize
 
  printf("********* write DS to MTX:2 ***********\n");
  lseek(fd, (long)0, 0);
  write(fd, buf, 1024);
  printf("DS = 0x1000+%4x = %4x\n", hp->tsize/16, *up);

  /****************************************************  
  printf("********* clear BSS of mtx ************\n");
  lseek(fd, (long)(hp->tsize+hp->dsize), 0);

  for (i=0; i<1024; i++)
    zero[i] = 0;

  count = 0;
  for (i=0; i<hp->bsize / 1024; i++){
    write(fd, zero, 1024);
    count += 1024;
  }
  for (i=0; i<hp->bsize % 1024; i++){
    write(fd, &ZERO, 1);
    count ++;
  }
  printf("%d bytes cleared\n", count);
  ***********************************************/
  printf("done\n");
}





