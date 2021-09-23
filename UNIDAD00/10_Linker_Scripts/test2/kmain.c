#include "fb.h"
//#include "serial.h"
#include "device.h"
//#include "gdt.h"

void init(void){
//	init_gdt();
	fb_init();
//	serial_init();
}

void clear(void){
	unsigned int i;
	unsigned char s[] = " ";
	for (i = 0; i < FB_NUM_CELLS; ++i)
		dev_write(0, s, 1);	
}

void kmain(){
	unsigned char s[] = "abcdefghijklmnopqrstuvxwyz";
//	unsigned int i;
	unsigned char FB[]="HOLA MUNDO FRAMEBUFFER!!";

	init();
	clear();
/*
	for(i=0; i<sizeof(FB); ++i){
	  fb_write_cell(i, FB[i], FB_BLACK, FB_GREEN);
	}
*/
	fb_write(s, sizeof(s));
//	fb_move_cursor(80);
	fb_write(FB, sizeof(FB));

//	dev_write(1, s, sizeof(s));
}
