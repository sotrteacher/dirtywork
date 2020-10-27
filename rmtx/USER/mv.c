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
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("        This is %s in action\n", argv[0]);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    if (argc < 3){
      printf("Usage: mv f1 f2\n");
      exit(1);
    }
    return mmv(argv[1], argv[2]);   
}

STAT stat1, stat2, *sp1, *sp2;
             
int mmv(f1, f2) char *f1,*f2;
{
   char buf[1024];
   int fd, gd, n, r;

   if (strcmp(f1 ,f2)==0){
     printf("can't mv file to itself\n");
     exit(2);
   } 

   sp1 = &stat1;
   if ((r=stat(f1, sp1))<0){
     printf("can't stat src %s\n", f1);
     exit(3);
   }

   sp2 = &stat2;
   r = stat(f2, sp2);

   if (r==0){
      if (sp2->st_mode & 0x8000 == 0x8000){
          printf("dest %s is not REG\n", f2);
          exit(4);
       }
    } 

   fd = open(f1, 0);
   if (fd<0){
     printf("bad src file %s\n", f1);
     exit(3);
   }
   gd = open(f2, O_WRONLY|O_CREAT);


   while(n = read(fd, buf, 1024)){
     write(gd, buf, n);
   }

   close(fd); close(gd);

   r = unlink(f1);
}
