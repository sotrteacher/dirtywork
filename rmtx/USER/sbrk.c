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

main(int argc, char *argv[ ])
{
  int brk, v;
  int a,b,c;

  a=1; b=2; c=3;
  printf("local variables a=%d b=%d c=%d\n", a,b,c);

  v = 0;
  if (argc > 1)
    v = atoi(argv[1]);
  
  printf("This program demo sbrk [size] syscall: EXPAND image by %d\n", v);
  printf("sbrk v=%d %x\n", v ,v);

  brk = sbrk(v);

  printf("back from sbrk() syscall brk = %x\n", brk);
  printf("local variables a=%d b=%d c=%d\n", a,b,c);
}
