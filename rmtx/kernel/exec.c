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
/************************* exec.c ***********************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

int kexec(char *y)   /* umode string has no leading blanks */
{
   int i, *ip; 
   u16 segment, offset, psize, ttsize, TOP, dsize, bsize;
   char line[32], filename[32], temp[32], f2[16], *cp, *cq, c;
   PROC *p;
   int w;

   p = running;
   if (p->type == PROCESS){
      if (p->res->tcount > 1){
         printf("EXEC: proc tcount=%d NOT yet 1\n", p->res->tcount);
         return -1;
      }
   }
   
   segment = running->res->segment; // if vforked, we are using parent's segment
     /*********
   if (running->vforked)
      printf("%d parent segment=%x  size=%x\n", 
             running->pid, segment, running->res->size);
     **********/
   /* get parameter line from U space */
   cp = line; 
   while( (*cp=get_byte(segment, y)) != 0 ){
          y++; cp++;
   }
   //printf("kexec line=%s\n", line);
  
   /* extract filename to exec */
   cp = line;        cq = f2;

   while(*cp == ' ')  /* SAFETY: skip over leading blanks */
     cp++;

   while (*cp != ' ' && *cp != 0){
         *cq = *cp;
          cq++; cp++;
   }
   *cq = 0;

   if (f2[0]==0){
       return -1;     /* should NOT happen but just in case */
   }
   
   // printf("%s = %d", f2, strlen(f2));
   if (f2[0]=='/'){
     strcpy(filename, f2);
   }
   else{
       strcpy(filename, "/bin/");   /* all executables are in /bin */
       strcat(filename, f2);
   }
   strcpy(running->res->name, f2);       /* program name */
  
 
   strcpy(temp, filename);          /* header destroys string */

   psize = header(temp,&dsize,&bsize); /* get file tsize */

   if (psize==0){
       printf("Proc %d EXEC error : command not found %s\n",
		running->pid, filename);
       /*************** KCW ******************
        We could let the proc die here, but that would be incompatible with
        Unix/Linux, which returns -f if exec fails.
        kexit(2);
       **************************************/
       return -1;
   }
   ttsize = (psize + dsize + bsize +15)/16;  // in clicks
   running->res->brk = ttsize;
   running->res->splimit = running->res->brk;

   if (SEP){
     printf("%s has sep I&D space\n", f2);
     ttsize = ttsize + 4096;   // total=ttsize + 64KB
   }
   else{
     ttsize = ttsize + 1280;   // total=ttsize + 20KB
     //ttsize = ttsize + 2280;   // total=ttsize + 36KB
   }

   // printf("size=%d file=%s ttsize=%d\n", running->size, filename, ttsize);

   if (running->vforked){ // we are vforked: cannot release parent's image

     /**************************************
     printf("%d in exec vforked uss=%x usp=%x\n", 
              running->pid, running->uss,running->usp);
     for (i=0; i<15; i++){
          w = get_word(running->uss, running->usp+2*i);
          printf("%x ", w);
     }
     printf("\n");
     ***************************************/

       segment = kmalloc(ttsize);
       if (segment==0){
	 printf2u("kexec: no memory\n");
         return -1;
       }
       // this is running's own Umode image
       running->res->segment = segment;
       running->res->size = ttsize;
       running->vforked = 0;             // turn off vforked flag
   }
   else{   // not vforked: try to use the same old image area
       if (ttsize > running->res->size){
          // printli((long)ttsize*16); printlx((long)ttsize*16);
          segment = kmalloc(ttsize);
          if (segment==0){
	      printf2u("kexec: no memory\n");
            return -1;
          }
          /********
          printf("%d mfree in exec:oldseg=%x newseg=%x\n",
          running->pid, running->segment, segment);
          *********/
          mfree(running->res->segment, running->res->size); /*rel old segment */
          running->res->segment = segment;
          running->res->size = ttsize;
      }
   }
   /*****************
   printf("proc %d exec to %s in segment %x\n",
             running->pid, filename, segment);
   ******************/
   load(filename, segment);       
   running->uss = segment;

   /*********************** KCW 10-2-99 ****************************
    In order to pass command line string to the new Umode image, 
    which has the form   main(char *s);  the ustack must contain:
       |CmdString | *s | uflag,uCS,uPC,ax,bc,....,uDS |......
    with uSP pointing at uDS.
   ****************************************************************/
   /* copy command line to ustack high end */   

   if (SEP){
     running->uss += running->res->tsize;
     offset = -128;                      // really 64KB - 128 bytes
   }
   else{
     offset = (running->res->size)*16 - 128;  // max. 128 chars 
   }

   // put_uword()/put_ubyte() are relative to running->uss
   for (i=0; i<128; i++)
       put_ubyte(line[i], offset+i);

   /* *s pointing at the command line string */
   put_uword(offset, offset-2);
  
   TOP = offset - 4;  /* followed by INT stack frame */

   /* zero out U mode registers in ustack di to ax */
   for (i=2; i<10; i++){
       put_uword(0, TOP-2*i);
   }

   /* re-initialize usp to new ustack top */

   running->usp = TOP-2*11;

   /*   0    1   2  3  4  5  6  7  8  9  10 11
   /*  flag uCS uPC ax bx cx dx bp si di es ds */
   /*  TOP                                  sp */

   put_uword(running->uss, TOP-2*11);   // uDS=uSS
   put_uword(running->uss, TOP-2*10);   // uES=uSS
   put_uword(0,            TOP-2*2);    // uPC=0
   put_uword(segment,      TOP-2*1);    // uCS=segment
   put_uword(0x0200,       TOP);        // flag
   //  printf("%d end exec\n", running->pid);
}
