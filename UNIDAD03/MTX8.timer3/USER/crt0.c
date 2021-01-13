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
// crt0.c : main0(s) called from u.s, where s = oigianl command string
//          tokenlize s into char *argv[ ] and call main(argc, argv).
 
// token() breaks up a string into argc of tokens, pointed by argv[]

int argc;
char *argv[32];

int token(line) char *line;
{
  int i;
  char *cp;
  cp = line;
  argc = 0;
  
  while (*cp != 0){
       while (*cp == ' ') *cp++ = 0;        
       if (*cp != 0)
           argv[argc++] = cp;         
       while (*cp != ' ' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  argv[argc] = 0;
}

int showarg(argc, argv) int argc; char *argv[];
{
  int i;
  printf("argc=%d ", argc);
  for (i=0; i<argc; i++)
    printf("argv[%d]=%s ", i, argv[i]);
  printf("\n");
}
 
int main0(s) char *s;
{
  printf("main0: s=%s\n", s);
  token(s);
  main(argc, argv);
}

