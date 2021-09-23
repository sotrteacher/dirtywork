#include "fb.h"
#include "device.h"
#include "gdt.h"
//#define CASOUNO
extern int X;
extern char* data_start;

void init(void){
  init_gdt();
  fb_init();
//  serial_init();
}

void clear(void){
	unsigned int i;
	unsigned char s[] = " ";
	for (i = 0; i < FB_NUM_CELLS; ++i)
		dev_write(0, s, 1);	
}

void kmain(){
  unsigned char s[] = "abcdefghijklmnopqrstuvxwyz";
  unsigned char FB[]="HOLA MUNDO FRAMEBUFFER!!";
  init();
  clear();
  fb_write(s, sizeof(s));
  fb_write(FB, sizeof(FB));
  X=X+1;
#ifdef CASOUNO
  unsigned int y=(unsigned int)&data_start;
#endif
//  /*terminamos la funcion ordenadamente*/
//  asm volatile("mov $1,%%eax;\n\t"      /*se coloca 1 en eax*/
//               "mov %0,%%ebx;\n\t"      /*se coloca el valor de X en ebx*/
//               "int $0x80\n\t"          /*se hace una syscall (system call)*/
//               : : "r"(X) : "%eax" );   /*se avisa al compilador que eax esta clobbered*/
}/*end kmain()*/
