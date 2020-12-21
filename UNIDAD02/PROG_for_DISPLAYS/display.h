/**Marc Rochkind, Advanced C Programming for Displays, 
 * Character Displays, Windows, and Keyboards for the Unix 
 * and the MS-DOS Operating Systems. Prentice-Hall, 1988.
 * pag. 21*/
#define C_UL	218	/*upper left*/
#define C_UR	191	/*upper right*/
#define C_LL	192	/*lower left*/
#define C_LR	217	/*lower right*/
#define C_XD	194	/*T intersection down*/
#define C_XU	193	/*T instersection up*/
#define C_XR	195	/*T intersection right*/
#define C_XL	180	/*T intersection left*/
#define C_XX	197	/*4-way intersection*/
#define C_H	196	/*horizontal line*/
#define C_V	179	/*vertical line*/
#define C_ULD	201	/*next group similar but double*/
#define C_URD	187
#define C_LLD	200
#define C_LRD	188
#define C_XDD	203
#define C_XUD	202
#define C_XRD	204
#define C_XLD	185
#define C_XXD	206
#define C_HD	205
#define C_VD	186
#define C_HATCH	176	/*hatching*/
#define C_SPACE	255	/*alternate space*/

#define A_NORM		0x07	/*normal*/
#define A_INVERSE	0x70	/*inverse*/
#define A_INTENSE	0x0F	/*intense*/
#define A_BLINK		0x87	/*blink*/
#define A_UNDER		0x01	/*underline*/

typedef struct s_cell {
  char chr;            /*character*/
  char att;            /*attribute*/
} CELL;

#define ROWS	24
#define COLS	80

CELL dspbuf[ROWS][COLS];

typedef int BOOLEAN;
#define FALSE	0
#define TRUE	1

#define MIN(x,y)	((x)<(y) ? (x) : (y))
#define MAX(x,y)	((x)>(y) ? (x) : (y))

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

typedef struct s_rect {
  short r1,c1;         /*upper left corner*/
  short r2,c2;         /*lower right corner*/
} RECT;

#define RWIDTH(a)	((a)->c2 - (a)->c1 + 1)
#define RHEIGHT(a)	((a)->r2 - (a)->r1 + 1)
#define RASG(a,row1,col1,row2,col2)	((a)->r1=row1;(a)->c1=col1;\
(a)->r2=row2;(a)->c2=col2;)
#define RCPY(dst,src)	((dst)->r1=(src)->r1;(dst)->c1=(src)->c1;\
(dst)->r2=(src)->r2;(dst)->c2=(src)->c2;)

extern RECT GIANT_RECT;
#define GIANT	9999

#include <stdio.h>
#include <ctype.h>
//#include "port.h"
//#include "dsputil.h"



