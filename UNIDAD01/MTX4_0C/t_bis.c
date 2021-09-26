#define	DISPLAY_COLS		80
extern char c;
//extern char _cursRow;
//extern char _cursCol;
char _cursRow;
char _cursCol;
void copy_data_byte(const char *s,unsigned short int *dest);

int
main_bis(){
	copy_data_byte(&c,(unsigned short int *)0x0b8000 + DISPLAY_COLS * _cursRow + _cursCol);
	return 0;
}

