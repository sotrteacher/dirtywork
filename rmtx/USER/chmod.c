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

main(argc, argv) int argc; char *argv[];
{
    u16 mode;
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("        This is KC's %s in action\n", argv[0]);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    if (argc<3){
      printf("Usage: chmod filename 0xxx\n");
      exit(1);
    }
    
    // mode = 64*(argv[2][1]-'0') + 8*(argv[2][2]-'0') + (argv[2][3]-'0');
    mode = atoo(argv[2]);

    
    printf("argv[2]=%s  mode=%d  %x\n", argv[2], mode, mode);

    chmod(argv[1], mode);
    exit(0);   
}

             
