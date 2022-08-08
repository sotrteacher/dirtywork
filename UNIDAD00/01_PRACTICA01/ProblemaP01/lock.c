/** lock.c - Advisory locking, sets a writable lock on the whole of the
 * file myfile and calls pause () to wait for a SIGUSR1 signal. After the 
 * signal arrives, a call is made to unlock the file.
 * Steve D.  Pate, Steve D. Pate - Unix filesystems_ evolution, design, 
 * and implementation-Wiley (2003).djvu, page 49.
 */
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#if 1 //LMC 2022.02.07
#define __USE_XOPEN_EXTENDED
#include <signal.h>
#include <stdio.h>
#endif

void
mysig(int signo)
{
 return;
}

int
main()
{
 struct flock lk;
 int fd, err;
 
 sigset(SIGUSR1, mysig);
 fd = open("myfile", O_WRONLY);
 lk.l_type = F_WRLCK;
 lk.l_whence = SEEK_SET;
 lk.l_start = 0;
 lk.l_len = 0;
 lk.l_pid = getpid();
 
 err = fcntl(fd, F_SETLK, &lk) ;
 printf("lock: File is locked\n");
 pause();
 lk.l_type = F_UNLCK;
 err = fcntl(fd, F_SETLK, &lk);
#if 1 //LMC 2022.02.07
 if (err == -1) {
   perror("F_SETLK command failed :-(");
 }
#endif
 printf("lock: File is unlocked\n");
 
 return 0;
}/*end main()*/ 
 
 
