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

char *cp, mytty[32];
char uline[2048], buf[1024],zero;

int str(src, target) char *src, *target;
{
  int i;
  for (i=0; i<strlen(src)-strlen(target); i++){
    if (strncmp(&src[i], target, strlen(target))==0)
      return 1;
  }
  return 0;
}

main(argc, argv) int argc; char *argv[];
{
    int fd, gd, n, nbytes, cr, i,j,  newline, backspace;

    STAT st0,st1, sttty;
    int red0, red1;

    print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    print2f("             KC's grep in action             \n"); 
    print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");   

    cr = '\r'; 
    newline='\n';
    backspace='\b';


    gettty(mytty);
 
    fstat(0, &st0);
    fstat(1, &st1); 
    stat(mytty, &sttty);

    red0 = 1;
    if (st0.st_dev == sttty.st_dev && st0.st_ino == sttty.st_ino)
        red0 = 0; 

    red1 = 1;
    if (st1.st_dev == sttty.st_dev && st1.st_ino == sttty.st_ino)
       red1 = 0; 
 
    if (argc < 2){ // grep from stdin
      printf("usage : grep pattern filename\n");
      exit(1);
    }


    // printf("argc=%d argv[1]=%s\n", argc, argv[1]);

    if (argc == 2){
      // if 0 has bee redirected ==> do NOT show the lines read==>getline()
      // otherwise, must show each char typed ==> call gets()
      if (red0){
         while( getline(uline)){
        	if (strstr(uline, argv[1]))
	        printf("%s\r", uline);
         }
      }
      else{
         while( gets(uline)){
        	if (strstr(uline, argv[1]))
	        printf("%s\n", uline);
         }
      }
    }
    else{
       fd = open(argv[1], O_RDONLY);   /* open input file for READ */
       if (fd < 0){
	 printf("open %s failed\n", argv[1]);
         exit(2);
       }
       n = read(fd, buf, 1024);
 
       buf[n] = 0;
       uline[0] = 0;

       while( n ){
          j = strlen(uline);

          for (i=0; i<n; i++){
             if (buf[i]=='\n' || buf[i]=='\r')
	        break;
	     uline[j++] = buf[i++];
          }
          uline[j] = 0;

          if (strstr(uline, argv[1]))
             printf("%s\n", uline);
 
          strcpy(uline, &buf[i]); 
       
          n = read(fd, buf, 1024-strlen(uline));
          buf[n] = 0;
       }
    }
}
