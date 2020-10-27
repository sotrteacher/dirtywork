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
#include "ucode.c"

struct fp{
  char name[4];
  u32 addr;
  char len, rev,cksum,feature;
  char fea[4];
};

struct mp{
  char signature[4];
  u16  baseLen;
  char rev, cksum;
  char OEM[20];
  u32  oemPtr;
  u16  oemSize, entryCount;
  u32  localAPICPtr; 
  u16  extlen, extcksum;
} *q; 

/******* these follow the header **********/  
struct processor{
  char type, ID, ver, flag;
  u32  signature;
  u32  flags;
  u32  reseve[2];
} *procp;

struct bus{
  char type,ID;
  char bstring[6];
} *busp;

struct IOapci{
  char type,ID,ver,flag;
  u32  address;
} *ioapcip;

struct IOintAss{
  char type, ID, d1,d2;
  char srcID,srcIRQ, desApciID, desINT;
} *ioIntp;

struct localIntAss{
  char type, ID, d1,d2;
  char srcID,srcIRQ, desApciID, desINT;
} *IntAssp;

char g[1024];

int entries, entlen;

int pentries()
{
  char *cp;
  int i;
  char temp[8];

  printf("entries=%d entlen=%d\n", entries, entlen);
  getc();
  cp = &g[4*11];
  for (i=0; i<entries-1; i++){ // entries includes header
    printf("------ Entry %d type=%d ------\n", i, *cp);
     if (*cp == 0){
       printf("******** CPU #%d ********\n", i);
       procp = (struct processor *)cp;
       printf("%x %x %x %x\n", procp->type, procp->ID, procp->ver, procp->flag);
       printf("cpu_signature=%L\n", procp->signature);
       printf("flags=%L\n", procp->flags);
       cp += 20;
       continue;
     }
     if (*cp == 1){
        printf("bus entry\n");
        busp = (struct bus *)cp;
        printf("%x %x\n", busp->type, busp->ID);
        strncpy(temp, busp->bstring,6); temp[6]=0;
        printf("busString=%s\n", temp);
     }
     if (*cp == 2){
        printf("IO APIC entry\n");
        ioapcip = (struct IOapci *)cp;
        printf("%x %x %x %x\n", 
           ioapcip->type, ioapcip->ID, ioapcip->ver, ioapcip->flag);
        printf("addr=%L\n", ioapcip->address);
     }
     if (*cp == 3){
        printf("IO interrupt entry\n");
        ioIntp = (struct IOintAss *)cp;
        printf("%x %x\n", ioIntp->type, ioIntp->ID);
        printf("%x %x %x %x\n", 
              ioIntp->srcID, ioIntp->srcIRQ, ioIntp->desApciID,ioIntp->desINT); 
     }
     if (*cp == 4){
        printf("Local Interrupt Assignemnt entry\n");
        IntAssp = (struct localIntAss *)cp;
        printf("%x %x\n", IntAssp->type, IntAssp->ID);
        printf("%x %x %x %x\n", 
         IntAssp->srcID, IntAssp->srcIRQ, IntAssp->desApciID, IntAssp->desINT); 
     }
     cp += 8;
     getc();
  }
       
  
}

main(int argc, char *argv[ ])
{
  struct fp *p;
  char temp[8];
  int i,j,k, seg;
  u32 longAddr;
  u16 segment, biosseg, offset;

  printf("size of fp struct = %d\n", sizeof(struct fp));
  biosseg = get_word(0x0040, 0x0E); // segment of extended BIOS data area
  printf("Search 1KB in extended BIOS data area segment=%x\n", biosseg);

  getblk(biosseg);
  p = (struct fp *)g;
  for (i=0; i<64; i++){
      if (strncmp(p->name, "_MP_", 4)==0){
	 printf("found i=%d\n", i);
         getc();
      }
      p++;
  }

  printf("\nTry last 1K of real RAM area: 0x9FC0\n");

  getblk(0x9FC0);
  p = (struct fp *)g;
  for (i=0; i<64; i++){
      if (strncmp(p->name, "_MP_", 4)==0){
	printf("found i=%d\n", i);
        print(p);
        getc();
      }
      p++;
  }

  printf("\nSerach BIOS ROM area 0xF000 to 1M\n");
  for (seg=0; seg<64; seg++){ // 64KB segment
    segment = 0xF000 + seg*0x40;
    getblk(segment);
    p = (struct fp *)g;
    for (i=0; i<64; i++){
        if (strncmp(p->name, "_MP_", 4)==0){
	  longAddr = (u32)segment;
          longAddr = (longAddr << 4) + i*16;
          printf("found: segment=%x  offset=%x  long_addr=%L\n",
		 segment, i*sizeof(struct fp), longAddr);
          print(p);
          break;
	}
        p++;
    }
  }
}

int getblk(u16 segment)
{
  int i;
  for (i=0; i<1024; i++){
    g[i] = get_byte(segment, i);
  }
}

int print(struct fp *p)
{
  char temp[8];
  int i,j,k;
  u32 segment; 
  u32 offset;

  printf("MP Float Pointer Structure\n");
  strncpy(temp, p->name, 4); temp[4]=0;
  printf("signature=%s\n", temp);
  printf("MP_tableAddr=%L\n", (long)p->addr); 
  printf("%x %x %x %x\n", p->len, p->rev, p->cksum, p->feature);
  printf("%x %x %x %x\n", p->fea[0], p->fea[1], p->fea[2], p->fea[3]);
  getc();

  segment = p->addr;
  printf("MP Table Address = %L\n", segment);
  offset = segment & 0x0000000F;
  segment = segment >> 4;
  printf("segment=%x  offset=%x\n", (u16)segment, (u16)offset);

  getblk(segment);
  for (i=0; i<1024-offset; i++)
       g[i] = g[i+offset];
  /*
struct mp{
  char signature[4];
  u16  baseLen;
  char rev, cksum;
  char OEM[20];
  u32  oemPtr;
  u16  oemSize, entryCount;
  u32  localAPICPtr; 
  u16  extlen, extcksum;
} *q; 
  */
  
  printf("--------- MP Table -----------\n");
  q = (struct mp *)g;
  printf("signature = %c %c %c %c\n", g[0],g[1],g[2],g[3]);
  printf("base_len=%d  rev=%x  cksum=%x\n", q->baseLen, q->rev, q->cksum);  
  for (j=2; j<11; j++){
      for (k=0; k<4; k++)
	   printf("%x ", g[j*4 + k]);
      for (k=0; k<4; k++)
           printf("%c ", g[j*4 + k]);
       printf("\n");
  }
  printf("oemTablePtr=%L\n", (u32)q->oemPtr);
  printf("oemSize=%d entryCount=%d\n", q->oemSize, q->entryCount);
  printf("localAPICPtr=%L\n", (u32)q->localAPICPtr);
  printf("extlen=%d\n", q->extlen);

  printf("--------- MP Table -----------\n");        

  entries = q->entryCount;
  entlen  = q->baseLen;

  pentries();
}

