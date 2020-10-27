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

char *cp, mytty[64];

char realname[64], linkname[64];

main(argc, argv) int argc; char *argv[];
{
    int fd, gd, n, nbytes, cr, i, count, newline, backspace,r;
    char buf[4096], buf2[1024], line[128];

    STAT st0,st1, sttty;
    int red0, red1;

    //    print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    print2f("             KC's cool cat MEOW!             \n"); 
    // print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");   

    cr = '\r'; 
    newline='\n';
    backspace='\b';

    gettty(mytty);

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
 
    if (argc < 2){ // cat from stdin
      fd = 0; 
      nbytes=1;
      //print2f("cat from stdin\n");
    }
    else{
       strcpy(realname, argv[1]);
dolink:
       if (stat(realname, &st0) < 0){
	  printf("no such file %s\n", argv[1]);
          exit(1);
       }
       // if file is a symlink, get its linked name
       if ((st0.st_mode & 0120000)==0120000){
	  readlink(realname, linkname);
  	  printf("cat : symlink %s->%s\n", realname, linkname);
          strcpy(realname, linkname);
          printf("Enter a key to continue : ");getc(); printf("\n");
          goto dolink;
       }

       fd = open(realname, O_RDONLY);   /* open input file for READ */
       if (fd < 0){
	 printf("open %s failed\n", realname);
         exit(1);
       }
       nbytes=4096;

    }

    n = read(fd, buf, nbytes);

    while(n > 0){
       for (i=0; i<n; i++){
         
         if (fd==0){
	   if (buf[i]=='\r')  // if KBD:
	     buf[i]='\n';     // change \r to \n

             write(1, &buf[i], 1);
             if (red0 && red1) // cat in between pipes as in cat f | CAT | cat
	       continue;       // do not spit out anything

             if ((!red0) && (buf[i]=='\n')){
               write(2, &cr, 1);
	     }

             if (red1){
                write(2, &buf[i], 1);
                if ( buf[i]=='\n')
	           write(2, &cr, 1);
             }

         }
         else{
          write(1, &buf[i], 1);
          if (buf[i]=='\n')
	    write(2, &cr, 1);
	  /*********
          if (buf[i]=='\r')
            write(2,&newline,1);
	  *********/
	 }
       }

       n = read(fd, buf, nbytes);
    }
    exit(0);
}

             

