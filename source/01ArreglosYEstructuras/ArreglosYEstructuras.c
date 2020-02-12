/**ArreglosYEstructuras.c
 */
#include <stdio.h>
#include <header.h>

static
unsigned int boleta[]={
  2009640001,
  2020640999,
  2015640055,
  2019362050
};

unsigned int min_num()
{
  unsigned int r=boleta[0],i;
  for(i=1;i<sizeof(boleta)/sizeof(unsigned int);i++)
  {
    if(boleta[i]<r){
      r=boleta[i];
    }
  }
  return r;
}

unsigned int min_num1(unsigned int *boleta,unsigned int tam)
{
  unsigned int r=boleta[0],i;
  for(i=1;i<tam;i++)
  {
    if(boleta[i]<r){
      r=boleta[i];
    }
  }
  return r;
}

void
print_dumm(struct dumm *dPt)
{
  int i;
  for(i=0;i<dPt->N;i++){
    printf("%5d",*(dPt->intPt+i));
  }
}









