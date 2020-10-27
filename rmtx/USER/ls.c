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

#define OWNER  000700
#define GROUP  000070
#define OTHER  000007

/******************************************************************
typedef struct stat {
  ushort st_dev;		// major/minor device number 
  ushort st_ino;		// i-node number 
  ushort st_mode;		// file mode, protection bits, etc.
  ushort st_nlink;		// # links; TEMPORARY HACK: should be nlink_t
  ushort st_uid;		// uid of the file's owner 
  ushort st_gid;		// gid; TEMPORARY HACK: should be gid_t
  ushort st_rdev;
  long   st_size;		// file size 
  long   st_atime;		// time of last access 
  //******************** KCW : order in ionde is ctime, mtime ***************
  //long   st_mtime;		// time of last data modification
  //long   st_ctime;		// time of last file status change
  ***************** revirse order to make storing time in MTX easier *********
  long   st_ctime;		// time of last data modification
  long   st_mtime;		// time of last file status change
  //****** KCW: order of ctime and mtime are reversed in inode 
} STAT;
**************************************************************************/

STAT utat, *sp;
int fd, n;
DIR *dp;
char f[32], cwdname[64], file[64];
char buf[1024];

DIR *dp;
char *cp;

int pdate(t) u8 t[4];
{
   printf("%c%c%c%c-%c%c-%c%c  ", 
          (t[0]>>4)+'0', (t[0]&0x0F)+'0',
          (t[1]>>4)+'0', (t[1]&0x0F)+'0', 
          (t[2]>>4)+'0', (t[2]&0x0F)+'0',
          (t[3]>>4)+'0', (t[3]&0x0F)+'0');
   printf("  ");
}

int ptime(t) u8 t[4];
{
   printf("%c%c:%c%c:%c%c", 
   (t[0]>>4)+'0', (t[0]&0x0F)+'0',
   (t[1]>>4)+'0', (t[1]&0x0F)+'0', 
   (t[2]>>4)+'0', (t[2]&0x0F)+'0');
}


main(argc,argv) int argc; char *argv[];
{
    sp = &utat;

    showarg(argc, argv);

    print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    print2f("        This is KC's ls in action\n");
    print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    //eatpat(s, components, name, &nk);

    if (argc==1){  /* for uls without any parameter ==> cwd */
      /*******
        getcwd(cwdname);
        name[1] = cwdname;
      *******/
        strcpy(file, "./");

    }
    else
      strcpy(file, argv[1]);

    printf("file=%s\n", file);

    if (stat(file, sp) < 0){
        print2f("cannot stat %s\n",argv[1]);
        exit(2);
    }

    //printf("ls %s\n", file);
    //getc();
  
    if ((sp->st_mode & 0100000)==0100000){
        ls_file(sp, file, file);
    }
    else{
      if ((sp->st_mode & 0040000)==0040000)
	ls_dir(sp, file);
    }

    exit(0);
}

int ls_file(sp,name,path) struct stat *sp; char *name, *path;
{
    ushort mode, mask, k, len;
    char fullname[32], linkname[60], ;

    mode = sp->st_mode;

    if ((mode & 0040000) == 0040000)
        putc('d');

    if ((mode & 0120000) == 0120000)
        putc('s');
    else if ((mode & 0100000) == 0100000)
         putc('-');


    mask = 000400;
    for (k=0; k<3; k++){
        if (mode & mask)
            putc('r');
        else
            putc('-');
        mask = mask >> 1;
        if (mode & mask)
            putc('w');
        else
            putc('-');
        mask = mask >> 1;
        if (mode & mask)
            putc('x');
        else
            putc('-');
        mask = mask >> 1;
    }

    if (sp->st_nlink < 10)
        printf("  %d ", sp->st_nlink);
    else
        printf(" %d ", sp->st_nlink);
    printf(" %d  %d  %l ", sp->st_uid, sp->st_gid, (u32)sp->st_size);
    printf(" ");
    //printf("%l ", (u32)sp->st_size);

    pdate(&sp->st_date); ptime(&sp->st_time);

    printf(" %s", name);

    if ((mode & 0120000)== 0120000){
      strcpy(fullname, path); strcat(fullname, "/"); strcat(fullname,name);
      // symlink file: get its linked string
      len = readlink(fullname, linkname);
      printf(" -> %s", linkname);
    }

    printf("\n");     

}

int ls_dir(sp, path) struct stat *sp; char *path;
{
    STAT dstat, *dsp;
    long size;
    ushort k, nd, m;
    char temp[32];

    size = sp->st_size;

    //printf("ls_dir %s\n", path); getc();

    fd = open(file, 0); /* open dir file for READ */
    n = read(fd, buf, 1024);

    cp = buf;
    dp = (DIR *)cp;

    while (cp < &buf[1024]){
       if (dp->inode != 0){
           dsp = &dstat;
           strncpy(temp, dp->name, dp->name_len); 
           temp[dp->name_len] = 0;  

           strcpy(f, file);
           strcat(f, "/"); strcat(f, temp);

           if (stat(f, dsp) >= 0){
              ls_file(dsp, temp, path);
           }
       }
       cp += dp->rec_len;
       dp = (DIR *)cp;
    }

    close(fd);
}         
