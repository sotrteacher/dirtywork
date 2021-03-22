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

union offsdgdt {
 char c[4];
 int  offset;
} OdGDT;  //Offset de GDT - The Offset is the linear address of tehe table itself.
union gdtentry {
 char c[8];
 long long d;   /** sizeof(long long) showed 8. */    
};

int main(struct multiboot *mboot_ptr)
{
  ClearScreen(0x07);
  SetCursorPosition(0,0);
  //PutUnsigned(sizeof(long long),16,0);
  PutString("\r\n");
  PutString("GDTR\r\n");
  char zone[6]; 
  __asm__ __volatile__ ("sgdt %0" : : "m" (zone)) ;
  PutString("Size of GDT minus 1 (in hex):\r\n");
  PutUnsigned(zone[1],16,0);// 00
  PutChar(' ');
  PutUnsigned(zone[0],16,0);// 27h equivale a 2*16+7=32+7=39. 39 es size de la GDT - 1
                            // y 39 equivale a 40-1. 40 es igual a 5*8, lo que significa 
                            // que la GDT en uso tiene 5 descriptores de segmento.
                            // (cada descriptor de segmento es de 8 bytes). Se buscar\'a
                            // imprimir la direccion de la GDT.
  PutString("\r\n");
  //PutUnsigned(zone[5],16,0);// 00
  //PutString("\r\n");
  //PutUnsigned(zone[4],16,0);// 0c
  //PutString("\r\n");
  //PutUnsigned(zone[3],16,0);// aa 
  //PutString("\r\n");
  //PutUnsigned(zone[2],16,0);// 68 
  PutString("\r\n");
  OdGDT.c[0]=zone[2];
  OdGDT.c[1]=zone[3];
  OdGDT.c[2]=zone[4];
  OdGDT.c[3]=zone[5];
  PutString("GDT address:\r\n");
  PutUnsigned(OdGDT.offset,16,0);// Como cada entrada de la GDT es de 8 bytes, Los dos 
                                 // enteros de 4 bytes contenidos en el primer descriptor
                                 // deben ser ambos 0.
  PutString("\r\n");
  int *gI = (int*)OdGDT.offset;
/*  PutUnsigned(*gI,16,0);
  PutString("\r\n");
  PutUnsigned(*(gI+1),16,0);
  PutString("\r\n");
  PutString("\r\n");
  PutUnsigned(*(gI+2),16,0);
  PutString("\r\n");
  PutUnsigned(*(gI+3),16,0);
  PutString("\r\n");
  PutString("\r\n");*/
  int j,k;
  char ch;
/*  for (k=0;k<10;k++){
    PutUnsigned(*(gI+k),16,0);
    PutString("\r\n");
    if (k%2){
      PutString("\r\n");
    }
  }*/
  long long *GDT=(long long *)gI;
  union gdtentry GEntry[5];
  for (k = 0;k<5;k++){
    GEntry[k].d=*(GDT + k);
  }
  PutString("\r\nDescriptores de segmento (Aprox)\r\n"); 
  PutString("(use solo los \\'ultimos dos d\\'igitos de cada n\\'umero):\r\n");
  for (j = 0;j<5;j++){
    for (k=7;k>=0;k--){
      if(GEntry[j].c[k]==0){PutChar('0');PutChar('0');}else{
        PutUnsigned(GEntry[j].c[k],16,0);
      }
      PutChar(' ');
    } 
    PutString("\r\n");
  }
/*Init_CPU();*/
//DWORD32 tiempo = Now_Plus(1);
  running = &proc;
  //printf("call tswitch()\n");
     tswitch();
  //printf("back to main()\n");
  return 0;
}/*end main()*/


