#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		errx(1,"Invalid number of arguments: %s <command> <command>",argv[0]);
	}
	const char* cmd1=argv[1];
	const char* cmd2=argv[2];
	pid_t a=fork();
	if(a<0)
	{
		err(2,"Could not fork");
	}
	if(a==0)
	{
		if(execlp(cmd1,cmd1,(char*)NULL)==-1)
		{
			err(3,"Could not exec %s",cmd1);
		}
	}
	bool success=false;
	int status;
	pid_t wait_status=wait(&status);
	if(wait_status<0)
	{
		err(4,"Could not wait for child");
	}
	if(WIFEXITED(status))
	{
		if(WEXITSTATUS(status)==0)
		{
			success=true;
		}
	}
	if(!success)
	{
		exit(42);
	}
	if(execlp(cmd2,cmd2,(char*)NULL)==-1)
	{
		err(5,"Could not exec %s",cmd2);
	}
	exit(0);
}
