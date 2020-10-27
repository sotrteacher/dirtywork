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
#include "ucode.c"

char buf[1024], c;

main(int argc, char *argv[ ])
{
  int fd, gd, n;

  printf("copy f1 to f2\n");
  fd = open("f1", O_RDONLY);
  if (fd < 0) exit(1);

  while(1){
    printf("append f1 to f2\n");

    gd = open("f2", O_WRONLY|O_CREAT|O_APPEND);
    if (gd < 0) exit(3);

    lseek(fd, (long)0);
    while( n=read(fd, buf, 1024) )
         write(gd, buf, n);
    close(gd);

    printf("again? [y|n]"); c=getc();
    if (c !='y') break;
  }
}

  

