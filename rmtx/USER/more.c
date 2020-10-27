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

char lines, in_buf[1024];
char uline[2048];

char mytty[16];
char w;
char cr = '\r';  
char newline = '\n';
char c;

main(int argc, char *argv[])
{
   int fd, gd, n, i, j, r;
   long position;
   STAT st0, st1, sttty;
   int red0, red1;
   
   lines=0; 

   gettty(mytty);
   // printf("tty = %s\n", mytty);
   gd = open(mytty, 0);
 
   r = fstat(0, &st0);
   //printf("fstat r=%d\n", r);

   stat(mytty, &sttty);
   //printf("%d%d %d%d\n", st1.st_dev, st1.st_ino, sttty.st_dev,sttty.st_ino);

   if (st0.st_dev == sttty.st_dev && st0.st_ino == sttty.st_ino)
      red0 = 0;
   else 
      red0 = 1; 
    
   fstat(1, &st1);
   if (st1.st_dev == sttty.st_dev && st1.st_ino == sttty.st_ino)
      red1 = 0;
   else 
      red1 = 1; 

   fd = 0;               /* default infile = stdin */

   print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
   print2f("        This is KC's more in action          \n");
   print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

   if (argv[1]){
      fd = open(argv[1], 0);   /* open input file for READ */
      if (fd < 0)
          exit(1);
   }

   if (fd){
      n = read(fd, in_buf, 1024);

      while (n>0){
         for (i=0; i<n; i++){
             c = in_buf[i];
             write(1, &c, 1);
             if (c == '\n'){
                lines++;
                write(1,&cr,1);
             }
             if (lines > 25){
                 read(gd, &w, 1);
                 if (w=='\r' || w=='\n')
                    lines--;
                 if (w==' ')
                    lines = 0;
             }
         }
         n = read(fd, in_buf, 1024); 
     }
   }
   else{
     if (red0){
       while( getline(uline) ){ // getline() does not show input chars
	     printf("%s\r", uline);
             lines++;
             if (lines > 25){
                 read(gd, &w, 1);
                 if (w == '\r' || w=='\n')
	             lines--;
                 if (w==' ')
	             lines=0;
	     }
	 }
     }
     else{ // 0 is not redirected ==> get from KBD
         while( gets(uline)){
	   if (red1)
 	       print2f("%s\n", uline);
         }
     }
   }
}
