/** mycatl.c - Will only display the file if there are no write locks held on the 
 * file. If a lock is detected, the program loops up to 5 times waiting for the 
 * lock to be released. Because the lock will still be held by the lock program, 
 * mycatl will extract the process ID from the flock structure returned by fcntl() 
 * and post a SIGUSRl signal. This is handled by the lock program which then unlocks 
 * the file.
 * REF
 * Steve D.  Pate, Steve D. Pate - Unix filesystems_ evolution, design, 
 * and implementation-Wiley (2003).djvu, page 50.
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#if 1 //LMC 2022.02.07
#define BUFSZ 512
void catfile(pid_t);
#endif 

pid_t
is_locked(int fd)
{
 struct flock lk;
 
 lk.l_type = F_RDLCK;
 lk.l_whence = SEEK_SET;
 lk.l_start = 0;
 lk.l_len = 0;
 lk.l_pid = 0;
 
 fcntl(fd, F_GETLK, &lk);
 return (lk.l_type == F_UNLCK) ? 0 : lk.l_pid;
} 
 
int
main()
{
 //struct flock lk;
 //int i, fd, err;
 int i, fd;
 pid_t pid;
 
 fd = open("myfile", O_RDONLY);
 
 for (i=0;i<5;i++) {
   if ((pid = is_locked(fd)) == 0) {
     catfile(fd);
     exit(0);
   }else{
     printf("mycatl: File is locked ...\n");
     sleep(1);
   }
 }
 kill(pid, SIGUSR1);
 while ((pid = is_locked(fd)) != 0) {
   printf("mycatl: Waiting for lock release\n");
   sleep(1);
 }
 catfile(fd);
 return 0;
}
 
#if 1 //LMC 2022.02.07
void catfile(pid_t ifd){
  int nread;
  char buf [BUFSZ] ;
  while ((nread = read(ifd, buf, BUFSZ)) != 0)  {
    write(STDOUT_FILENO, buf, nread);
  }
}
#endif 
