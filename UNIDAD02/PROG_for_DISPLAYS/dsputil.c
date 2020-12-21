#include "display.h"
RECT GIANT_RECT = {0,0,GIANT,GIANT};


/*----------
 intersect - intersection between 2 rectangles
 RETURN: Is intesection non-empty?
------------*/
BOOLEAN intersect(RECT *srcp,   /*source rectangle*/
                  int maxrow,   /*max row of clipping rectangle*/
                  int maxcol,   /*max column of clipping rectangle*/
                  RECT *dstp    /*destination rectangle*/
)
{
  dstp->r1=MAX(0,srcp->r1);
  dstp->c1=MAX(0,srcp->c1);
  dstp->r2=MIN(maxrow,srcp->r2);
  dstp->c2=MIN(maxcol,srcp->c2);
  return ((BOOLEAN)(dstp->r1 <= dstp->r2 && dstp->c1 <= dstp->c2));
}

/*---------------
 enclosing - rectangle enclosing 2 rectangles
----------------*/
void enclosing(RECT *srcp1,     /*first source rectangle*/
               RECT *srcp2,     /*second source rectangle*/
               RECT *dstp       /*enclosing rectangle*/
)
{
  dstp->r1=MIN(srcp1->r1,srcp2->r1);
  dstp->c1=MIN(srcp1->c1,srcp2->c1);
  dstp->r2=MAX(srcp1->r2,srcp2->r2);
  dstp->c2=MAX(srcp1->c2,srcp2->c2);
}






