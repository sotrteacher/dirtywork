#include <stdio.h>
#include <string.h>

extern char data_start[];
//extern char data_size[];
//extern char data_load_start[];

int A=10;

//void copy_data(void){
//  if(data_start!=data_load_start){
//    memcpy(data_start,data_load_start,(size_t)data_size);
//  }
//}

void mymain(void)
{
  int a = 42;
  int *p=&data_start;
  printf("%p\n",p);
  a++;
  asm volatile("mov $1,%%eax; mov %0,%%ebx; int $0x80" 
               : : "r"(a) : "%eax" );
}

