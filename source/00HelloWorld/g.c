/**g.c*/
char D[]={
  'm',  /*martes*/
  'M',  /*Mi\'{e}rcoles*/
  'j',  /*jueves*/
  'v',  /*viernes*/
  's',  /*s\'{a}bado*/
  'd',  /*domingo*/
  'L',  /*Lunes*/
};

char
g(unsigned int n)
{
  if(n>6)
    return ' ';
  return D[n];
}

