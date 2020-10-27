/*********** #include <mtx_type.h> ***********/
#include "../ucode.c"

main(int argc, char *argv[])
{
  int pid, child, status, cdserver; 
  char line[128], temp[128], cmd[64], filename[64], *token;
  char msg[128];

    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("        This is KC's %s in action\n", argv[0]);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    if (argc < 2){
      printf("Usage: cdclient serverPid\n");
      exit(1);
    }
    cdserver = atoi(argv[1]);
    while(1){
       printf("CDCLIENT: enter command: [lls|lcd|lpwd|lcat| ls|cd|pwd|cat|cp|quit]:");
       gets(line); printf("\n");
       if (line[0]==0)
	 continue;
       strcpy(temp, line);

       cmd[0] = 0; filename[0] = 0;
       token = strtok(temp, " ");
       strcpy(cmd, token);
       token = strtok(0, " ");
       if (token)
	 strcpy(filename, token);

       if (strcmp(cmd, "lls") && strcmp(cmd,"lcd") && strcmp(cmd,"lpwd") && strcmp(cmd,"lcat")){
          strcpy(msg, line);
          printf("%d sending request [%s] to %d\n", getpid(), msg, cdserver);

          send(msg, cdserver);
          if (!strcmp(msg,"quit"))
	     exit(0);

          printf("%d waits for reply from %d\n", getpid(), cdserver);
          recv(msg);
          printf("%d received reply=[%s] from %d\n", getpid(), msg, cdserver);
          continue;
        }

        // local commands == lls, lcd, lcat
        // line=lls... lcd... lcat ... ==> kill leading l
        if (!strcmp(cmd, "lcd")){
           chdir(filename);
           continue;
        }
        if (!strcmp(cmd, "lpwd")){
	   pwd();
           continue;
        }
        strcpy(temp, &line[1]); 
        child = fork();
        if (child){
	     wait(&status);
             continue;
        }
        else
	  exec(temp);
    }
}
