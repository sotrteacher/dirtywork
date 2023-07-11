/** ReadSector.cpp
REF: Let us C - yashwantkanetkar.pdf
(/mnt/sda3/Users/LMC/2023/07_BOOKS/WINDOWS_Programming/
Let us c - yashwantkanetkar.pdf)
page 629/743
*/
#include <stdafx.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>

#pragma pack(1)
struct boot
{
 BYTE jump[3];
 char bsOemName[8];
 WORD bytesperSector;
 BYTE sectorspercluster;
 WORD sectorsreservedarea;
 BYTE copiesFAT;
 WORD maxrootdirentries;
 WORD totalSectors;
 BYTE mediaDescriptor;
 WORD sectorsperFAT;
 WORD sectorsperTrack;
 WORD sides;
 WORD hiddenSectors;
 char reserve[480];
};

void ReadSector(char *src,int ss,int num,void *buf);

int main()
{
 struct boot b;
 ReadSector("\\\\.\\A:",0,1,&b);
 printf("Boot Sector name: %s\n",b.id);
 printf("Bytes per Sector: %d\n",b.bps);
 printf("Sectors per Cluster: %d\n",b.spc);
 /* rest of the statements can be written by referring Figure 19.6
    and Appendix G */

 return 0;
}/*end main()*/

void ReadSector(char *src,int ss,int num,oid *buff)
{
 HANDLE h;
 unsigned int br;
 h = CreateFile(src,GENERIC_READ,
                FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
 SetFilePointer(h,(ss * 512),NULL,FILE_BEGIN);
 ReadFile(h,buff,512*num,&br,NULL);
 CloseHandle(h);
}


