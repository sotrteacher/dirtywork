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

main(int argc, char *argv[])
{
    int fd, gd, n, c, cr, i; 
    char buf[1024];

    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("        This is KC's %s in action\n", argv[0]);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    if (argc<2){ printf("usage : lpr filename\n"); exit(1);}
    fd = open(argv[1], 0);
    if (fd < 0){ printf("non-existing src file\n"); exit(2); }

    gd = open("/dev/lp0", 1);

    if (gd < 0){ printf("can't open special file /dev/lp0\n"); exit(3);}

    n = read(fd, buf, 1024); 
    cr = '\r';
    while(n){
           for (i=0; i<n; i++){
              c = buf[i];
              write(gd, &c, 1);
              if (c=='\n')
                 write(gd, &cr, 1);
           }
           n = read(fd, buf, 1024);
    }
    close(fd);  close(gd);
}

             
