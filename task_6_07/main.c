#include <sys/wait.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char * argv[])
{
	if(argc!=2)
	{ 
		errx(1,"Invalid number of arguments: %s <cmd>",argv[0]);
	}
	const char* cmd=argv[1];
	pid_t a =fork();
	int status;
	if(a<0)
	{
		err(2,"Could not fork");
	}
	if(a==0)
	{
		if(execlp(cmd,cmd,(char*)NULL)==-1)
		{
			err(3,"Cannot exec %s",cmd);
		}
	}

	pid_t wait_status=wait(&status);
	if(wait_status<0)
	{
		err(4,"Error in waiting");
	}
	if(!WIFEXITED(status))
	{
		err(5,"Command %s didn't execute normally",cmd);
	}
	if(WEXITSTATUS(status)!=0)
	{
		errx(6,"Exit status of command %s is not 0",cmd);
	}
	printf("%s\n",cmd);
	exit(0);
}
