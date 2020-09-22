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
/************************ ps.c *****************************/
#include "ucode.c"

main(int argc, char *argv[])
{
  int i, r;
  char name[64];
  if (argc<2){
    printf("usage : exec filename param_list\n");
    exit(1);
  }

  strcpy(name, argv[1]);

  for (i=2; i<argc; i++){
      strcat(name, " ");
      strcat(name, argv[i]);
  }            
  r = exec(name);
  printf("proc %d: exec %s failed\n", argv[1]);
  exit(1);
}
