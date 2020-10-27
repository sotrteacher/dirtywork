/*********** #include <mtx_type.h> ***********/
#include "../ucode.c"

main(int argc, char *argv[])
{
  int pid, status; 
  char cmd[64];
  char msg[128];

    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("        This is KC's %s in action\n", argv[0]);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    pid = fork();
    if (pid){
      while(1){
        printf("cdclient: enter command: [ls|cd|pwd|cat|cp|quit]:");
        gets(cmd); printf("\n");
        strcpy(msg, cmd);
        printf("%d sending request [%s] to %d\n", getpid(), msg, pid);
        printf("parent %d waits for a reply from %d\n", getpid(), pid);
        send(msg, pid);

        if (!strcmp(msg,"quit"))
	  exit(0);
        
        printf("%d tries to receive from %d\n", getpid(), msg, pid);
        recv(msg);
        printf("%d received reply=[%s] from %d\n", getpid(), msg, pid);

      }
    }
    else{ // child: execute cdserver
      exec("cdserver");
    }
}



             
