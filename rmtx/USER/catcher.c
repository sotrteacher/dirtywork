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
/******************** catcher.c file ************************
         test interval timer, umode signal catcher               
*************************************************************/
#include "ucode.c"

void catcher(int sig)
{
  signal(14, catcher);  // install catcher again
  printf("catcher() in Umode, sig=%d\n", sig);  
  itimer(1);              // set a 1 sec. interval timer
}
  
main(argc, argv) int argc; char *argv[];
{
  int t = 2;               // default timer interval 
  signal(14, catcher);     // install catcher() for SIGALRM (14)
  if (argc > 1)
     t = atoi(argv[1]);
  printf("request interval timer = %d\n", t);
  itimer(t);               // call ktimer() in kernel
  while(1);
}

