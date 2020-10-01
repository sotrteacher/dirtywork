/******************** archivo.c *******************/
#include <stdio.h>
int getcs();
int get_ebp();
int get_esp();
int get_rbp();
//extern char rbph[];
//extern char rbpl[];

void funcion()
{
  printf("HOLA MUNDO C\nCS=0x%x\nebp=0x%x\nesp=0x%x\n",
         getcs(),get_ebp(),get_esp());
  printf("rbp=0x%lx\n",get_rbp());
  //printf("rbp=0x%x%x\n",rbph,rbpl);
}



