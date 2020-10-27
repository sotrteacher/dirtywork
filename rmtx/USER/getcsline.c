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
#include "../type.h"

#define EOF -1
int getc()
{
   int c, n;
   n = read(0, &c, 1);

   /********************************************************************* 
   getc from KBD will NOT get 0 byte but reading file (after redirect 0 
   to file) may get 0 byte ==> MUST return 2-byte -1 to differentiate.
   **********************************************************************/

   if (n==0 || c==4) return EOF;  
                                
   return (c&0x7F);
}


// getline() does NOT show the input chars AND no cooking: 
// for reditected inputs from a file which may contain '\b' chars

int getline(char *s)
{
  int c;  
  char *cp = s;
  
  c = getc();
  while (c != EOF && c != '\r' && c != '\n'){
    *cp++ = c;
    c = getc();
  }
  if (c==EOF) return 0;

  *cp++ = c;         // a string with last char=\n or \r
  *cp = 0;    
  return strlen(s);  // at least 1 because last char=\r
}

// gets() show each input char AND cook input line

int gets(char *s)
{
  int c; char *cp, *cq, temp[128];
  
  cp = temp;    // get chars into temp[] first

  c = getc();
  while (c!= EOF && c != '\r' && c != '\n'){
    *cp++ = c;
    putc(c);
    if (c == '\b'){
       putc(' '); 
       putc('\b');
    }
    c = getc();
  }
  putc('\r'); putc('\n');

  if (c==EOF) return 0;
  
  *cp = 0;   

  // printf("temp=%s\n", temp);

  // cook line in temp[] into s
  cp = temp; cq = s; 

  while (*cp){
    if (*cp == '\b'){
      if (cq > s)
	  cq--; 
      cp++;
      continue;
    }
    *cq++ = *cp++;
  }
  *cq = 0;

  //printf("s=%s\n", s);

  return strlen(s)+1;  // line=CR or \n only return 1
}
