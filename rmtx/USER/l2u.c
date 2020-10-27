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
/*************** lower2upper case conversion ****************/

#include "ucode.c"

int main(int argc, char *argv[ ])
{  
  int fd, gd, n, c;
  char buf[1024], *cp;


  printf("\nargc = %d  argv = ", argc);

  for (n=0; n<argc; n++)
    printf("%s ", argv[n]);
  printf("\n");

  if (argc==1){ // stdin to stdout
    while( (c = getc()) != EOF ){
      c &= 0x7F;
      if ( c >= 'a' && c <= 'z')
         putc(c - 'a' + 'A');
      else
	putc(c);

      if (c == '\r')
	putc('\n');

    }
    exit(0);

  }
  if (argc < 3){
    printf("usage : l2u f1 f2\n");
    exit(1);
  }
  fd = open(argv[1], O_RDONLY);
  if (fd < 0){
    exit(1);
  }
  gd = open(argv[2], O_WRONLY|O_CREAT);

  while( n = read(fd, buf, 1024) ){
    cp = buf;
    while(cp < buf + n){
      if (*cp >= 'a' && *cp <= 'z'){
         *cp = *cp - 'a' + 'A';
      }
      cp++;
    }
    write(gd, buf, n);
  }
  printf("done\n");

}

