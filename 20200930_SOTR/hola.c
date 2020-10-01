/******************** hola.c *******************/
#include <stdio.h>
void funcion();

int
main()
{
  printf("Testing percent lx:%lx\n",0x100000000);
  printf("Testing percent x:%x\n",0x100000000);
  printf("Testing percent lx:%lx\n",funcion);
  printf("Testing percent x:%x\n",funcion);
  funcion();
  return 0;
}
