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

char pname[16];
int pid, old;
u16 i, j, k;
u16 addr;

int catcher(int sig)
{
  printf("\nenter catcher signal=%d\n", sig);
  printf("install cather again for sig# %d", sig+1); 
  signal(sig+1, catcher, 0);
  printf("\nDELAY FOR A WHILE .....");

  for (i=0; i<2; i++)
      for (j=0; j<65000; j++);

  printf("\nexit catcher\n");
}


main(int argc, char *argv[])
{  
    addr = (u16)catcher;

    printf("install catcher for signal# 2 %x\n", catcher);
 
    signal(2, addr, 0);

    printf("looping ....\n"); 

    while(1){
      for (i=0; i<400; i++)
        for (j=0; j<65000; j++);
      printf("one more loop again\n");
    }

    exit(0);   
}

             




