#include <stdio.h>
#include <stdlib.h>  /*malloc(),atoi(),atol(),atoll()*/ 

#include <ArrYEst.h> /*put -I<whatever/necesarry> at make file*/
#include <header.h>  /*idem*/

unsigned int boleta[]={
  2009640001,
  2020640999,
  2015640055,
  2019362050,
  2008550123
};
int main(int argc,char *argv[])
{
  int i;
#ifndef VERS2
  printf("%d\n",min_num1(boleta,TAM(boleta)));
#endif /*VERS2*/
  struct dumm struct_dumm;
  struct dumm *dummPt=&struct_dumm;
  if(argc<2){
    printf("FORMA DE USO:%s <num1> <num2> ... <numN>\n",argv[0]);
    return 1;
  }
  dummPt->N=argc-1;
  dummPt->intPt=(int*)malloc(dummPt->N*sizeof(int));
  for(i=0;i<dummPt->N;i++){
    *(dummPt->intPt+i)=atoi(argv[i+1]);
  }
  
  print_dumm(&struct_dumm);
  printf("\n");
  
  return 0;
}/*end main()*/
