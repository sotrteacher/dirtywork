/**hello.c
   Para editar este archivo, use:
$ vim hello.c
*/
//#include "./include/funciong.h"
#include <funciong.h> /*Add -I./include in your make file*/
#include <stdio.h>
//int printf(char *,...);
int atoi(const char *);


int main(int argc,char *argv[])
{
  int intA;
#ifdef TEST1
  printf("%c\n",g(29%7));
#endif
  if(argc<2){
    printf("FORMA DE USO:%s <n><--|\n",argv[0]);
    return 1;
  }
  intA=atoi(argv[1]);
  printf("%i\n",intA);
  for(intA=0;intA<argc;++intA)
  {
    printf("%5i%20s\n",intA,argv[intA]);
  }
/*  return printf("         HOLA MUNDO C from Debian running on WSL!\n");
*/
  return 0;
}/*end main()*/

/** Alternativamente, podemos utilizar el programa Cygwin */
/** Para instalarlo: 
    User:cursos
    pw:Aprender1
*/
