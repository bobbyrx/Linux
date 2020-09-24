#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
	if(argc==1)
	{
		errx(1,"Invalid number of arguments: %s <cmd1> ... <cmdN>",argv[0]);
	}
	int success=0;
	int fail=0;
	for(int i=1;i<argc;i++)
	{
		const char* cmd=argv[i];
		pid_t p=fork();
		if(p<0)
		{
			err(2,"Could not fork");
		}
		if(p==0)
		{
			if(execlp(cmd,cmd,(char*)NULL)==-1)
			{
				err(3,"Could not exec %s",cmd);
			}
		}
		bool if_success=false;
		int status;
		pid_t wait_status=wait(&status);
		if(wait_status<0)
		{
			err(4,"Could not wait for %s",cmd);
		}
		if(WIFEXITED(status))
		{
			if(WEXITSTATUS(status)==0)
			{
				if_success=true;
			}
		}
		if(if_success)
		{
			success++;
		}
		else 
		{
			fail++;
			if(!WIFEXITED(status))
			{
				warn("Child prosess could not exit propperly");
			}
			if(WEXITSTATUS(status)!=0)
			{
				warnx("Exit status of child is not 0");
			}
		}
	}
	printf("SUCCESS: %d FAIL: %d\n",success,fail);
	exit(0);
}
