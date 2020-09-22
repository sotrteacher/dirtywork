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

/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/
#define PA 13
#define PB 14
#define PC 15
#define PD 16
#define AX  8

/****************** syscall handler in C ***************************/
int kcinth()
{
   u16    segment, offset;
   int    a,b,c,d, r;
   segment = running->uss; 
   offset = running->usp;

   /** get syscall parameters from ustack **/
   a = get_word(segment, offset + 2*PA);
   b = get_word(segment, offset + 2*PB);
   c = get_word(segment, offset + 2*PC);
   d = get_word(segment, offset + 2*PD);
   /*route syscall call to kernel functions by call# a */
   switch(a){
       case 0 : {r = running->pid;     break;}
       //case 0 : {r = getpid();     break;}
       case 1 : {r = do_ps();          break;}
       case 2 : {r = chname(b);        break;}
       case 3 : {r = kkfork();         break;}
       case 4 : {r = tswitch();        break;}
       case 5 : {r = do_wait(b);       break;}
       case 6 : {r = do_exit(b);       break;}
       case 7 : {r =  getc();          break;}
       case 8 : {color=running->pid+11;
                r =  putc(b);         break;}       
       case 99: {do_exit(b);           break;}
       default: {
          printf("----------------->invalid syscall # : %d\n", a); }
   }
   put_word(r, segment, offset + 2*AX);
   //return r;
}/*end kcinth()*/

