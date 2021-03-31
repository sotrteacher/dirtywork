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
#include "descriptor_tables.h"
#include "isr.h"
#include "monitor.h"
#include "timer.h"

struct multiboot;    /* In order to avoid compiler warning */
void tswitch(void);  /* In order to avoid compiler warning */
                     /* defined in file tswitch.s */
#define SSIZE  1024


typedef struct proc{
        struct proc *next;
               int  *ksp;
               int   kstack[SSIZE];
}PROC;
//#include "include.h"

PROC proc, *running;
//PROC proc[NPROC+NTHREAD], *freeList, *running, *readyQueue, *sleepList;
//PROC proc, *freeList, *running, *readyQueue, *sleepList;
int procSize = sizeof(PROC);

int scheduler()
{ 
  running = &proc;
}

int main(struct multiboot *mboot_ptr)
{
  init_descriptor_tables();
  
  monitor_clear();
  monitor_write("PROGRAM !\n");
  //monitor_write("IDTR done?\n");
  //asm volatile("int $0x3");
  //asm volatile("int $0x4");
  monitor_write("\n------------------------------------\n");
  __asm__ __volatile__("sti");
  init_timer(60);
  /**
   *load_idt_entry(0x21, (unsigned long) keyboard_handler_int, 0x08, 0x8e);
   *Equivalent line 
   *idt_set_gate(0x21, (u32int)keyboard_handler_int, 0x08, 0x8e);
   *added at descriptor_tables.c in function init_idt()
   */
  return 0;
}/*end main()*/


