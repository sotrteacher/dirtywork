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

char ans[16];
int pc, bp;

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
  

int catcher(int sig)
{
  printf("in catcher : signal# = %d\n", sig);
  longjmp();
}  


main(int argc, char *argv[])
{
  int r, n;
  printf("simulate hardware traps by INT #\n");

  if (argc<2){
    printf("usage : trap 1-7\n");
    exit(1);
  }
  n = atoi(argv[1]);
  if (n<1 || n>7){
    printf("bad trap number %d\n", n);
    exit(2);
  }
  printf("install catcher for signal# %d ? (y|n) : ", n);
  gets(ans);

  if (strcmp(ans, "y")==0){
    signal(n, catcher);
    printf("catcher for singal# %d installed\n",n);
  }

  r = setjmp();
  if (r==0){
    if (n==1) int1(1);
    if (n==2) int2(2);
    if (n==3) int3(3);
    if (n==4) int4(4);
    if (n==5) int5(5);
    if (n==6) int6(6);
    if (n==7) int7(7);
    
  }
  else{
    printf("haha! I survived trap %d\n", n);
  }
}





