#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
	if(argc!=3)
	{
		errx(1,"Invalid number of arguments: %s <cmd1> <cmd2>",argv[0]);
	}
	const char* cmd1=argv[1];
	const char* cmd2=argv[2];
	pid_t p1=fork();
	if(p1<0)
	{
		err(2,"Could not fork");
	}
	if(p1==0)
	{
		if(execlp(cmd1,cmd1,(char*)NULL)==-1)
		{
			err(3,"Could not exec %s",cmd1);
		}
	}
	pid_t p2=fork();
	if(p2<0)
	{
		err(4,"Could not fork");
	}
	if(p2==0)
	{
		if(execlp(cmd2,cmd2,(char*)NULL)==-1)
		{
			err(5,"Could not exec %s",cmd2);
		}
	}
	bool first=false;
	int status;
	pid_t wait_status=wait(&status);
	if(wait_status<0)
	{
		err(7,"Could not wait");
	}
	if(WIFEXITED(status))
	{
		if(WEXITSTATUS(status)==0)
		{
			first=true;
			printf("FIRST SUCCESSFULL COMMAND: %d\n",wait_status);
		}
	}
	if(!first)
	{
		int status2;
		pid_t wait_2=wait(&status);
		if(WIFEXITED(status2))
		{
			if(WEXITSTATUS(status2)==0)
			{
				first=true;
				printf("FIRST SUCCESSFULL COMMAND: %d\n",wait_2);
			}
		}

	}
	if(wait(NULL)<0)
	{
		err(8,"Could not wait for children");
	}		
	if(!first)
	{
		printf("NONE: -1\n");
	}
	exit(0);
}
