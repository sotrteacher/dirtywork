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
/************* tc.c file ******************/
#include "libepc.h"
struct multiboot;    /* In order to avoid compiler warning */
void tswitch(void);  /* In order to avoid compiler warning */
                     /* defined in file tswitch.s */
#define SSIZE  1024

typedef struct proc{
        struct proc *next;
               int  *ksp;
               int   kstack[SSIZE];
}PROC;

PROC proc, *running;
int procSize = sizeof(PROC);

int scheduler()
{ 
  running = &proc;
}

//main()
int main(struct multiboot *mboot_ptr)
{
  ClearScreen(0x07);
  //SetCursorPosition(0,0);
  //PutString("Hello World\r\n");
  //printf("\nIn mtx kernel main() 2021.03.10...\n");
  running = &proc;
  //printf("call tswitch()\n");
     tswitch();
  //printf("back to main()\n");
  return 0;
}/*end main()*/


