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

int pid, sig;
char *cp;

main(argc, argv) int argc; char *argv[];
{
    if (argc<2){
      printf("usage : kill -signal#  pid\n");
      exit(1);   
    }
    if (argc==2){
       pid = atoi(argv[1]);
       kill(9,pid,0);
    }
    if (argc==3){
       sig = atoi(argv[1]);
       pid = atoi(argv[2]);
       kill(sig, pid);
    }
}

             
