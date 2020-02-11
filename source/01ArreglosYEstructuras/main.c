#include <stdio.h>

#include <ArrYEst.h>

unsigned int boleta[]={
  2009640001,
  2020640999,
  2015640055,
  2019362050,
  2008550123
};
int main(int argc,char *argv[])
{
  printf("%d\n",min_num1(boleta,TAM(boleta)));
  return 0;
}/*end main()*/
