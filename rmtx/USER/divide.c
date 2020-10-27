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
#include "ucode.c"

char ans[16], any[16];
int pc, bp;
int mainpc;

// bcc -S src.c ==> bcc push si,di before allocating locals
int setjmp()
{
  int *p;
  p = &p;
  bp = *(p+3);
  pc = *(p+4);
  return 0;
}

int longjmp()
{
  int *p;
  p = &p;

  *(p+3) = bp;
  *(p+4) = pc;

  return 1;
}
  

int catcher(sig) int sig;
{
  printf("in catcher : signal# = %d\n",sig);
  longjmp();
}  


main(argc, argv) int argc; char *argv[];
{
  int *p, r;
  /********** for some reason it does not return to u.s *******************
    REASON: bcc retu code: does NOT use mov sp,bp pop bp
    instead, it assume ustack doe sNOT have any extra pushed on
  p = &p;
  mainpc = *(p+4);
  prints("mainpc=");
 
  printx(*(p+3)); prints(" ");
  printx(*(p+4)); prints(" ");
  prints(*(p+5)); prints(" ");
  prints("\n\r");
  *****************************************************************/

  printf("try to divide by zero\n");
  printf("install catcher ? (y|n) : ");
  gets(ans);

  if (strcmp(ans, "y")==0){
    signal(8, catcher); // trap to vector 0 but let signal=8
    printf("catcher installed\n");
  }

  r = setjmp();
  if (r==0)
    divide();
  else{
    printf("Amazing Grace! I survived divide by zero!\n");     
    exit(0);  // KCW: must exit() directly rather than return to u.c
  }
}

int divide()
{
  int a,b,c;
  a = 1; b = 0;
  printf("here it goes : c = a/0 !\n");
  c = a/b;
}




