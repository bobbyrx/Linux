#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <err.h>
#include <sys/wait.h>
#include <sys/types.h>

int main (int argc, char * argv[])
{
	if(argc!=4)
	{
		errx(1,"Invalid number of arguments: %s <cmd1> <cmd2> <cmd3>",argv[0]);
	}
	for(int i=1;i<argc;i++)
	{
		const char* cmd=argv[i];
		pid_t a=fork();
		int status;
		if(a<0)
		{
			err(2,"Could not fork normally");
		}
		if(a==0)
		{
			if(execlp(cmd,cmd,(char*)NULL)==-1)
			{
				err(3,"Could not do %s",cmd);
			}
		}

		pid_t wait_status=wait(&status);
		if(wait_status<0)
		{
			err(4,"Error in waiting %s",cmd);
		}
		if(!WIFEXITED(status))
		{
			warn("Command did not exit correctly");
			continue;
		}
		int exit_status=WEXITSTATUS(status);
		printf("COMMAND: %s\nPID: %d EXIT_STATUS: %d\n",cmd,wait_status,exit_status);
	}
	exit(0);
}
