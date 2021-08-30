#include <stdio.h>	/* fprintf */
#include <stdlib.h>	/* contains exit */
#include <sys/types.h>	/* unistd.h needs this */
#include <unistd.h>	/* contains read/write */
#include <fcntl.h>

#define MINIX_HEADER 32
#define GCC_HEADER 1024

void die(char * str)
{
	fprintf(stderr,"%s\n",str);
	exit(1);
}

void usage(void)
{
	die("Usage: build boot system [> image]");
}

int main(int argc, char ** argv)
{
	int i,c,id;
	char buf[1024];

	if (argc != 3)
		usage();
	for (i=0;i<sizeof buf; i++) buf[i]=0;
	if ((id=open(argv[1],O_RDONLY,0))<0)
		die("Unable to open 'boot'");
	if (read(id,buf,MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'boot'");
#if 1 //LMC 2021.08.19
        #if 0
	if (((long *) buf)[0]!=0x04100301)
        #else
	if ( (buf[0]==0x01) && (buf[1]==0x03) && (buf[2]==0x10) && (buf[3]==0x04))
          goto yes_minix_header1;
        #endif
		die("Non-Minix header of 'boot' ");
yes_minix_header1:
        #if 0
	if (((long *) buf)[1]!=MINIX_HEADER)
        #else
	if ( buf[4]==MINIX_HEADER)
          goto yes_minix_header2;
        #endif
		die("Non-Minix header of 'boot' ");
yes_minix_header2:
        #if 0
	if (((long *) buf)[3]!=0)
        #else
	if ( buf[12]==0)
          goto yes_minix_header3;
        #endif
		die("Illegal data segment in 'boot'");
yes_minix_header3:
	if (((long *) buf)[4]!=0)
		die("Illegal bss in 'boot'");
	if (((long *) buf)[5] != 0)
		die("Non-Minix header of 'boot'");
	if (((long *) buf)[7] != 0)
		die("Illegal symbol table in 'boot'");
#endif
	i=read(id,buf,sizeof buf);
	fprintf(stderr,"Boot sector %d bytes.\n",i);
	if (i>510)
		die("Boot block may not exceed 510 bytes");
	buf[510]=0x55;
	buf[511]=0xAA;
	i=write(1,buf,512);
	if (i!=512)
		die("Write call failed");
	close (id);
	
	if ((id=open(argv[2],O_RDONLY,0))<0)
		die("Unable to open 'system'");
	if (read(id,buf,GCC_HEADER) != GCC_HEADER)
		die("Unable to read header of 'system'");
//#if 1 //LMC 2021.08.19
#if 0 //LMC 2021.08.29
        #if 0
	if (((long *) buf)[5] != 0)
        #else
	if ((buf[20] == 0) && (buf[21] == 0) && (buf[22] == 0) && (buf[23] == 0))
          goto yes_gcc_header;
        #endif
		die("Non-GCC header of 'system'");
yes_gcc_header:
#endif
	for (i=0 ; (c=read(id,buf,sizeof buf))>0 ; i+=c )
		if (write(1,buf,c)!=c)
			die("Write call failed");
	close(id);
	fprintf(stderr,"System %d bytes.\n",i);
	return(0);
}
