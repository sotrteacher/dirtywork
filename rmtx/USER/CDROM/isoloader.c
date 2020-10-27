/*******************************************************
*   @ isoloader : load /filename MUST BE under /
********************************************************/

char *name[16], components[64];
char components[64];
int nnames;
char cbuf[2048];

eatpath(filename) char *filename;
{
  int i;   
  char *cp;
  strcpy(components, filename);

  nnames = 0;
  cp = components;

  while (*cp != 0){
       while (*cp == '/')
              *cp++ = 0;       
       if (*cp != 0)
           name[nnames++] = cp; 
       while (*cp != '/' && *cp != 0)
	       cp++;       
       if (*cp != 0)       
	   *cp = 0;        
       else
           break; 
       cp++;
  }
}

struct iso_directory_record * isearch(dirp, name) 
       struct iso_directory_record *dirp; char *name;
{
   char *cp, dname[MLEN];
   struct iso_directory_record *ddp;
   ulong extent;
   long size;
   int i, loop, count;

   if (strcmp(name, "..")==0)
     return getparent(dirp);
   
   extent = *(ulong *)dirp->extent; 
   size   = *(long  *)dirp->size; 
   printf("extent=%l size=%l\n", extent, size);
   printf("step through dir records\n");

   loop = 0;
   while(size){
      count = 0;
      getSector((ulong)extent, rbuf, 1);
      cp = rbuf;

      ddp = (struct iso_directory_record *)rbuf;

      printf("isearch for %s\n", name);

      while (cp < rbuf + 2048){
         if (ddp->length==0)
            break;

         if (sVD){ // supplement VD, must translate unicode-2 name to ascii
            strcpy(dname, dirname(ddp));
         }
         else{
             strncpy(dname, ddp->name, ddp->name_len);
             dname[ddp->name_len] = 0;
	 }

         if (loop==0){ // . and .. only in the first sector
           if (count==0)
	      strcpy(dname, ".");
           if (count==1)
	      strcpy(dname, "..");
         }

         printf("%s  ", dname);
                   
         if (strcasecmp(dname, name)==0){ // ignore case
             printf(" ==> found %s : ", name);
             return ddp;
         }
         count++;
         cp += ddp->length;
         ddp = (struct iso_directory_record *)cp;
      }
      size -= 2048;
      extent++;
      loop++;
   }
   return 0;
}   


/************************** KCW May 2009 ****************************          
 iso9660 CDROM generated with -J -R keeps only 8.3 in primary VD DIRs
 The rest files are in iso_supplement VD which has type=2. 
 (Boot VD type=0, primary VD type=1, supplement VD type=2)
 To find a file: if NO supplement VD ==> use primary VD
                 else must use supplement VD
********************************************************************/
/******************************** 
struct iso_supplementary_descriptor *spd;
int color;

int iload(filename, segment) char *filename;  ushort segment;
{ 
   int i;
   struct iso_directory_record *dirp;

   ulong extent, sector;
   long size;
   char *cp;
   ushort savedEs;

   color = 0x000C;

   printf("KCW's isoiloader: iload %s to segment %x\n", filename, segment);
   savedEs = getes();

   eatpath(filename);

   // find if any supplement VD

   sVD = 0; ssector = 0;
   sector = 16;
 
   for (i=0; i<10; i++){
     getSector(sector, cbuf);
     pd = (struct iso_primary_descriptor *)cbuf;

     if (pd->type==2){
       sVD = 1;
       ssector = sector;
       break;
     }
     if (pd->type==255)
       break;
     sector++;
   }

   if (sVD)
     printf("found supplement VD at sector=%l\n", ssector);
   else{
     sector = 16;
     printf("use primary VD at sector sector=16\n");
   }

   printf("get VD at sector %l\n", ssector);
   getSector(ssector, cbuf);
   pd = (struct iso_primary_descriptor *)cbuf;

   pd->id[5] = 0; // make it a string for printing
   printf("type=%d  id=%s ==> ", pd->type, pd->id);
 
   printf("get root dir record\n");

   dirp = (struct iso_directory_record *)pd->root_directory_record;
   // dirp->extent and dirp->size are both 8 bytes in 733 format

   for (i=0; i<nnames; i++){
      dirp = isearch(dirp, name[i]);
      if (dirp == 0){
	printf("no such name %s\n", name[i]);
        return -1;
      }
      // check DIR type
      if (i < nnames-1){
         if ((dirp->flags & 0x02) == 0){
	     printf("%s is not a DIR\n", name[i]);
             return -1;
         }
      }
   }

   // dirp now points at file record 
   extent = *(ulong *)dirp->extent; 
   size   = *(long  *)dirp->size; 

   printf("iloading: set ES to %x\n", segment);
   setes(segment);

   while(size>0){
      getSector(extent, 0);
      inces(0x80);
      extent++;
      size -= 2048;
      putc('.');
   }
   printf("iloading done\n");
   setes(savedEs);
   color = 0x000B;
   return 0;
}
***********************/
