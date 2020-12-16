#include <stdio.h>   /*putchar()*/
#include <stdlib.h>  /*exit()*/
#include "display.h"

#define HEIGHT		9
#define HALF_HEIGHT	5
#define WIDTH		31
#define HALF_WIDTH	16

/*--------------------------
  Draw some boxes
--------------------------*/
int main(){
  int r,c;
  /*top row*/
  putchar(C_UL);
  for(c=2;c<HALF_WIDTH;c++)
    putchar(C_H);
  putchar(C_XD);
  for(c=HALF_WIDTH+1;c<WIDTH;c++)
    putchar(C_H);
  putchar(C_UR);
  putchar('\n');
  /*upper half*/
  for(r=2;r<HALF_HEIGHT;r++) {
    putchar(C_V);
    for(c=2;c<HALF_WIDTH;c++)
      putchar(' ');
    putchar(C_V);
    for(c=HALF_WIDTH+1;c<WIDTH;c++)
      putchar(' ');
    putchar(C_V);
    putchar('\n');
  }
  /*middle row*/
  putchar(C_XR);
  for(c=2;c<HALF_WIDTH;c++)
    putchar(C_H);
  putchar(C_XX);
  for(c=HALF_WIDTH+1;c<WIDTH;c++)
    putchar(C_H);
  putchar(C_XL);
  putchar('\n');
  /*lower half*/
  for(r=2;r<HALF_HEIGHT;r++) {
    putchar(C_V);
    for(c=2;c<HALF_WIDTH;c++)
      putchar(' ');
    putchar(C_V);
    for(c=HALF_WIDTH+1;c<WIDTH;c++)
      putchar(' ');
    putchar(C_V);
    putchar('\n');
  }
  /*bottom row*/
  putchar(C_LL);
  for(c=2;c<HALF_WIDTH;c++)
    putchar(C_H);
  putchar(C_XU);
  for(c=HALF_WIDTH+1;c<WIDTH;c++)
    putchar(C_H);
  putchar(C_LR);
  putchar('\n');
  exit(0);
}/*end main()*/

