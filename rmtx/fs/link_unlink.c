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
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

int klink(char *y, char *z)
{
  // link oldfile=pathname newfile=parameter; both strings in Uspace

 MINODE *omip, *nmip, *pmip;
 char oldfile[32], newfile[32];
 char parent[32], child[32];

 int r, odev, ndev;
 unsigned long oino, nino;
 unsigned long pino, cino;

 get_param(y, oldfile);
 get_param(z, newfile);

 printf("klink: %s %s\n", oldfile, newfile);
 //kgetc();

 if (oldfile[0] == '/') 
     odev = root->dev;
 else
     odev = running->res->cwd->dev;

 // OLD must exist
 oino = getino(&odev, oldfile);
 if (!oino){
   printf("klink: oldfile %s does not exist\n", oldfile);
   return -1;
 }

 if (newfile[0] == '/') 
     ndev = root->dev;
 else
     ndev = running->res->cwd->dev;

 // NEW must not exist
 nino = getino(&ndev, newfile);
 if (nino){
   printf("klink: newfile %s already exists, can't link\n", newfile);
   return -1;
 }

 // OLD cannot be DIR   
 omip = iget(odev, oino);

 if ((omip->INODE.i_mode & 0040000)==0040000){
    printf("klink: %s is a DIR, can't link to DIR\n", oldfile);
    iput(omip);
    return -1;
 }
 if (running->res->uid && running->res->uid != omip->INODE.i_uid){
    printf("not owner\n");
    iput(omip);
    return -1;
 }
 // creat entry in NEW_parent with same ino

 strcpy(child, basename(newfile));
 strcpy(parent, dirname(newfile));
 printf("parent=%s  child=%s\n", parent, child);

 pino = getino(&ndev, parent);

 if (!pino){
    printf("OLD_DIR %s does not exists\n", parent);
    iput(omip);
    return(-1);
 }

 if (odev != ndev){
   printf("OLD dev %d != NEW dev %d, can't link\n", odev, ndev);
   iput(omip);
   return -1;
 }

 pmip = iget(ndev, pino);     

 r = enter_name(pmip, omip->ino, child);

 omip->INODE.i_links_count++;
 omip->dirty = 1;
 printf("inode's link_count=%d\n", omip->INODE.i_links_count); 

 iput(omip);
 iput(pmip);

 return(r);
} 

int kunlink(char *y)   // y->string in Usapce
{
  int i,j;
  int dev,r;
  unsigned long pino, ino;
  
  MINODE *mip, *pmip;
  char filename[32];
  char parent[32], child[32];

  get_param(y, filename);   // fetch filename from Uspace

  if (filename[0]==0){
    return -1;
  }

  if (filename[0] == '/') 
      dev = root->dev;
  else
      dev = running->res->cwd->dev;
  
  ino = getino(&dev, filename);
  if (!ino){
    printf("no such file %s\n", filename);
    return -1;
  }

  mip = iget(dev, ino);

  /***** if symlink, INODE has no data block *****/
  if ((mip->INODE.i_mode & 00100000) != 00100000){
    printf("%s is not a REG or SLINK file\n", filename);
    iput(mip);
    return -1;
  }
  if (running->res->uid && running->res->uid != mip->INODE.i_uid){
     printf("not owner\n");
     iput(mip);
     return -1;
  }  
  // remove basename from parent DIR
  strcpy(child, basename(filename));
  strcpy(parent, dirname(filename));
  printf("parent=%s  child=%s\n", parent, child);
 
  if (filename[0] == '/') 
      dev = root->dev;
  else
      dev = running->res->cwd->dev;

  pino = getino(&dev, parent);
  pmip = iget(dev, pino);

  rm_child(pmip, mip->ino, child); 
  pmip->dirty = 1;
  iput(pmip);

  // dec inode's link_count
  mip->INODE.i_links_count--;
  printf("inode's link_count=%d\n", mip->INODE.i_links_count);

  if (mip->INODE.i_links_count > 0){
     mip->dirty = 1;
     iput(mip);
     return 0;
  }

  printf("%s link_count=0 => deallocate data blocks and INODE\n", filename);
  if (!(mip->INODE.i_mode & 012000)==0120000) // symlink file has no data block
      truncate(mip);

  idalloc(mip->dev, (u16)mip->ino);

  iput(mip);         // must unlock mip 

  return(0);
}
 
int ksymlink(char *y, char *z)
{
  // symlink oldfile=pathname newfile=parameter

 MINODE *omip, *nmip, *pmip;
 char oldfile[32], newfile[32];
 char parent[32], child[32];

 int r, odev, ndev;
 unsigned long oino, nino;
 unsigned long pino, cino;

 get_param(y, oldfile);
 get_param(z, newfile);

 if (oldfile[0]==0){
   return -1;
 }

 if (newfile[0]==0){
   return -1;
 }
 printf("ksymlink: %s %s\n", oldfile, newfile);
 // kgetc();

 if (oldfile[0] == '/') 
     odev = root->dev;
 else
     odev = running->res->cwd->dev;

 // OLD must exist
 oino = getino(&odev, oldfile);
 if (!oino){
   printf("OLD_file %s does not exist\n", oldfile);
   return -1;
 }

 if (newfile[0] == '/') 
     ndev = root->dev;
 else
     ndev = running->res->cwd->dev;

 // NEW must not exist
 nino = getino(&ndev, newfile);
 if (nino){
   printf("NEW_file %s already exists, can't link\n", newfile);
   return -1;
 }

 // creat a newfile /x/y/z 
 strcpy(child, basename(newfile));
 strcpy(parent, dirname(newfile));
 printf("parent=%s  child=%s\n", parent, child);

 pino = getino(&ndev, parent);

 if (!pino){
    printf("OLD_DIR %s does not exists\n", parent);
    iput(omip);
    return(-1);
 }

 /********* symlink OK even if dev are not the same ***********/

 pmip = iget(odev, pino);     
 // call mycreat(pmip, child) to create a REG file
 printf("CALL MYCREAT\n");

 mycreat(pmip, child);
 // get INODE of parameter
 printf("AFTER CREAT\n");

 iput(pmip);     // with minode[].lock, must iput(pmip) to unlock;

 nino = getino(&ndev, newfile); // should exist now
 if (nino==0){
   printf("mycreat() did not creat %s\n", newfile);
   //   iput(pmip);
   return -1;
 }
 printf("ndev=%d ino=%d\n", ndev, nino);

 nmip = iget(ndev, nino);

 nmip->INODE.i_mode = 0120777;  // symlink type rwx rwx rwx

 strcpy((char *)nmip->INODE.i_block, oldfile);
 printf("iblock=%s\n", (char *)nmip->INODE.i_block);
 nmip->INODE.i_size = strlen(oldfile);

 nmip->dirty = 1;

 printf("name=%s type=%x size=%d refCount=%d\n",(char *)nmip->INODE.i_block, 
	nmip->INODE.i_mode, nmip->INODE.i_size, nmip->refCount);

 iput(nmip);

 pmip->dirty = 1;
 iput(pmip);
 printf("symlink %s %s OK\n", oldfile, newfile);
 return 0;
}
     
int kreadlink(char *y, char *z) // z->char[60] in Uspace
{
  unsigned long ino;
  MINODE *mip;
  char filename[32];
  char *cp;
  int dev;

  //  char parent[32], child[32], temp[32], saveParent[32];

  get_param(y, filename);   // fetch filename from Uspace    

  //  printf("\nkreadlinK %s\n", filename); kgetc();

  if (filename[0]==0){
    return -1;
  }

  if (filename[0] == '/') 
      dev = root->dev;
  else
      dev = running->res->cwd->dev;
  
  ino = getino(&dev, filename);
  if (!ino){
    printf("no such file %s\n", filename);
    return -1;
  }
  mip = iget(dev, ino);

  if ((mip->INODE.i_mode & 0120000) != 0120000){
    printf("%s is not a symlink\n", filename);
    iput(mip);
    return -1;
  }
  // copy linked string to Uspace

  cp = (char *)mip->INODE.i_block;

  while(*cp){
    put_byte(*cp, running->uss, z);
    cp++; z++;
  }
  put_byte(0, running->uss, z);

  iput(mip);

  return strlen((char *)mip->INODE.i_block);
}
