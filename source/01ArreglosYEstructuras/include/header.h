/** header.h
*/
#ifndef HEADER_H
#define HEADER_H
struct dumm{
  unsigned int N;/** it will be (by now) argc - 1 */ 
  int *intPt;    /** points to N integers */
};/*end struct dumm*/
void
print_dumm(struct dumm*);

#endif /*HEADER_H*/

