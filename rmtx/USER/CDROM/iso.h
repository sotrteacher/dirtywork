/* This part borrowed from the bsd386 isofs */
/**************
typedef unsigned long  ulong;
typedef unsigned short ushort;
typedef unsigned char  u8;
typedef unsigned long  u32;
typedef unsigned short u16;
****************/

#define ISODCL(from, to) (to - from + 1)

struct iso_volume_descriptor {
	char type[ISODCL(1,1)]; /* 711 */
	char id[ISODCL(2,6)];
	char version[ISODCL(7,7)];
	char data[ISODCL(8,2048)];
};

/* volume descriptor types */
#define ISO_VD_PRIMARY 1
#define ISO_VD_SUPPLEMENTARY 2
#define ISO_VD_END 255

#define ISO_STANDARD_ID "CD001"

struct iso_primary_descriptor {
        unsigned char type;		//[ISODCL (  1,   1)]; /* 711 */
	char id				[ISODCL (  2,   6)];
	char version			[ISODCL (  7,   7)]; /* 711 */
	char unused1			[ISODCL (  8,   8)];
	char system_id			[ISODCL (  9,  40)]; /* achars */
	char volume_id			[ISODCL ( 41,  72)]; /* dchars */
	char unused2			[ISODCL ( 73,  80)];
	char volume_space_size		[ISODCL ( 81,  88)]; /* 733 */
	char unused3			[ISODCL ( 89, 120)];
	char volume_set_size		[ISODCL (121, 124)]; /* 723 */
	char volume_sequence_number	[ISODCL (125, 128)]; /* 723 */
	char logical_block_size		[ISODCL (129, 132)]; /* 723 */
	char path_table_size		[ISODCL (133, 140)]; /* 733 */
	char type_l_path_table		[ISODCL (141, 144)]; /* 731 */
	char opt_type_l_path_table	[ISODCL (145, 148)]; /* 731 */
	char type_m_path_table		[ISODCL (149, 152)]; /* 732 */
	char opt_type_m_path_table	[ISODCL (153, 156)]; /* 732 */
	char root_directory_record	[ISODCL (157, 190)]; /* 9.1 */
	char volume_set_id		[ISODCL (191, 318)]; /* dchars */
	char publisher_id		[ISODCL (319, 446)]; /* achars */
	char preparer_id		[ISODCL (447, 574)]; /* achars */
	char application_id		[ISODCL (575, 702)]; /* achars */
	char copyright_file_id		[ISODCL (703, 739)]; /* 7.5 dchars */
	char abstract_file_id		[ISODCL (740, 776)]; /* 7.5 dchars */
	char bibliographic_file_id	[ISODCL (777, 813)]; /* 7.5 dchars */
	char creation_date		[ISODCL (814, 830)]; /* 8.4.26.1 */
	char modification_date		[ISODCL (831, 847)]; /* 8.4.26.1 */
	char expiration_date		[ISODCL (848, 864)]; /* 8.4.26.1 */
	char effective_date		[ISODCL (865, 881)]; /* 8.4.26.1 */
	char file_structure_version	[ISODCL (882, 882)]; /* 711 */
	char unused4			[ISODCL (883, 883)];
	char application_data		[ISODCL (884, 1395)];
	char unused5			[ISODCL (1396, 2048)];
};

/* Almost the same as the primary descriptor but two fields are specified */
struct iso_supplementary_descriptor {
        unsigned char type;		//[ISODCL (  1,   1)]; /* 711 */
	char id				[ISODCL (  2,   6)];
	char version			[ISODCL (  7,   7)]; /* 711 */
	char flags			[ISODCL (  8,   8)]; /* 853 */
	char system_id			[ISODCL (  9,  40)]; /* achars */
	char volume_id			[ISODCL ( 41,  72)]; /* dchars */
	char unused2			[ISODCL ( 73,  80)];
	char volume_space_size		[ISODCL ( 81,  88)]; /* 733 */
	char escape			[ISODCL ( 89, 120)]; /* 856 */
	char volume_set_size		[ISODCL (121, 124)]; /* 723 */
	char volume_sequence_number	[ISODCL (125, 128)]; /* 723 */
	char logical_block_size		[ISODCL (129, 132)]; /* 723 */
	char path_table_size		[ISODCL (133, 140)]; /* 733 */
	char type_l_path_table		[ISODCL (141, 144)]; /* 731 */
	char opt_type_l_path_table	[ISODCL (145, 148)]; /* 731 */
	char type_m_path_table		[ISODCL (149, 152)]; /* 732 */
	char opt_type_m_path_table	[ISODCL (153, 156)]; /* 732 */
	char root_directory_record	[ISODCL (157, 190)]; /* 9.1 */
	char volume_set_id		[ISODCL (191, 318)]; /* dchars */
	char publisher_id		[ISODCL (319, 446)]; /* achars */
	char preparer_id		[ISODCL (447, 574)]; /* achars */
	char application_id		[ISODCL (575, 702)]; /* achars */
	char copyright_file_id		[ISODCL (703, 739)]; /* 7.5 dchars */
	char abstract_file_id		[ISODCL (740, 776)]; /* 7.5 dchars */
	char bibliographic_file_id	[ISODCL (777, 813)]; /* 7.5 dchars */
	char creation_date		[ISODCL (814, 830)]; /* 8.4.26.1 */
	char modification_date		[ISODCL (831, 847)]; /* 8.4.26.1 */
	char expiration_date		[ISODCL (848, 864)]; /* 8.4.26.1 */
	char effective_date		[ISODCL (865, 881)]; /* 8.4.26.1 */
	char file_structure_version	[ISODCL (882, 882)]; /* 711 */
	char unused4			[ISODCL (883, 883)];
	char application_data		[ISODCL (884, 1395)];
	char unused5			[ISODCL (1396, 2048)];
};

struct path_table{
       unsigned char name_len;
       unsigned char ext_len;
       ulong   extent;	
       ushort  parent;
       char name[0];
};

struct iso_directory_record {
        unsigned char length; 		//[ISODCL (1, 1)]; /* 711 */
        unsigned char ext_attr_length;	//[ISODCL (2, 2)]; /* 711 */
	char extent			[ISODCL (3, 10)]; /* 733 */
	char size			[ISODCL (11, 18)]; /* 733 */
	char date			[ISODCL (19, 25)]; /* 7 by 711 */
        unsigned char flags;		//[ISODCL (26, 26)];
	char file_unit_size		[ISODCL (27, 27)]; /* 711 */
	char interleave			[ISODCL (28, 28)]; /* 711 */
	char volume_sequence_number	[ISODCL (29, 32)]; /* 723 */
        unsigned char name_len;		//[ISODCL (33, 33)]; /* 711 */
	char name			[0];
};/*__attribute__((packed));*/


/******** globals ******************************/
#define MLEN 128
struct iso_primary_descriptor *pd;
struct iso_directory_record   *dirp;

/******** testing get NSECTOR=2 SECTOR at a time *******/

#define NSECTOR   2
//char patable[2048];
char patable[4096];
u32  psector;
long psize;

//char rbuf[2050];
char rbuf[4098];
char temp[MLEN];  // name_len can reach 255 but take a chance here

struct iso_directory_record *cwd, *root;
char root_dir[64], cwd_dir[64];

int sVD = 0;
u32 ssector;

char *dirname(struct iso_directory_record *dirp)
{
  int i;
  for (i=0; i<dirp->name_len; i+=2){
      temp[i/2] = dirp->name[i+1];
  }
  temp[dirp->name_len/2] = 0;
  return temp;
}

char *ipathname(struct path_table *path)
{
  int i;

  for (i=0; i<path->name_len; i+=2){
      temp[i/2] = path->name[i+1];
  }
  temp[path->name_len/2] = 0;
  return temp;
}


ulong dirsector(struct iso_directory_record *dirp)
{
  return *(ulong *)dirp->extent;
}

long dirsize(struct iso_directory_record *dirp)
{
  return *(long *)dirp->size;
}


// return pointer to nth entry in path table
struct path_table *pitem(u16 n)
{
  char *cp;
  struct path_table *path;
  int i;

  cp = patable;
  path = (struct path_table *)patable;

  // step to nth entry 
  for (i=0; i<n-1; i++){
       cp += path->name_len + 8;
       if (path->name_len % 2)      // if name_len is ODD, has a pad byte
           cp++;
       path = (struct path_table *)cp;
  }
  return path;
}


// search path table for (name, sector); return its parent number
ushort getpno(char *name, u32 sector)
{ 
  char *cp;
  struct path_table *path;

  cp = patable;
  path = (struct path_table *)patable;

  while(cp < patable+psize){
    //printf("[%d %l %d %s]\n", path->name_len, path->extent, path->parent,
    //                        ipathname(path));

     if (strcmp(ipathname(path), name)==0 && path->extent==sector){
       //printf("found %s %l parent=%d\n", name, sector, path->parent);
         return path->parent;
     }
     cp += path->name_len + 8;
     if (path->name_len % 2)      // if name_len is ODD, has a pad byte
        cp++;
        
     path = (struct path_table *)cp;
     if (cp >= patable+psize)
         break;
  }
  return 0;
}

struct iso_directory_record *getparent(struct iso_directory_record *dp)
{
  struct iso_directory_record *drp;
  struct path_table *path;

  u16   pno, ppno;
  u32   dsector, psector, fsector;
  char  dname[32], pname[32];
  int i;
  char *cp;

  dsector = dirsector(dp);
  strcpy(dname, dirname(dp));
  printf("getparent of %s %l\n",dname, dsector);

  pno = getpno(dname, dsector);
  printf("pno=%d\n", pno);

  if (!pno){
     printf("getparent : no match\n");
     return 0;
  }

  // continue to use pnumber to find parent's DIR struct
  if (pno == 1)  // easy case
    return root;

  // pno is not 1 ==> find sectors of parent's parent and seach for
  // parent's DIR entry, which MUST exist somewhere

  path = pitem(pno);

  // get parent's (name, sector) in path entry 
  psector = path->extent;
  strcpy(pname, ipathname(path));
  ppno = path->parent;
  //printf("ppno=%d ", ppno);

  if (ppno == 1) // get root sectors
    fsector = dirsector(root);
  else // search ppno's sector#
    fsector = pitem(ppno)->extent;

  while(1){
    //printf("fsector=%l pname=%s psector=%l\n", fsector, pname, psector);
    getSector((u32)fsector, rbuf, NSECTOR);
    cp = rbuf;
    drp = (struct iso_directory_record *)rbuf;

    while(cp < rbuf + NSECTOR*2048){
      if (drp->length==0) 
          break;  
      //printf("%s %l\n", dirname(drp), dirsector(drp));
      
      if (dirsector(drp)==psector && strcmp(dirname(drp), pname)==0){
	//printf("found [%s %l]\n", pname, psector);
         return drp; // DIR struct will be copy64()ed from rbuf[ ]
      }
      cp += drp->length;
      drp = (struct iso_directory_record *)cp;

    }
    fsector += NSECTOR;
  }
}
