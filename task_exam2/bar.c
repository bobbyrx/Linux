#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc,char* argv[])
{
	if(argc!=2)
	{
		errx(1,"Invalid number of arguments: %s <cmd>",argv[0]);
	}
	const char* cmd=argv[1];
	struct stat st;
	const char* pipe="mkpipe";
	if(stat(pipe,&st)<0)
	{
		if(mkfifo(pipe, 0664)<0)
		{
			err(2,"Cannot make pipe");
		}
	}
	int fd=open(pipe, O_RDONLY);
	if(fd<0)
	{
		err(3,"Cannot open %s",pipe);
	}
	if(dup2(fd,0)<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(4,"Could not dup2 mkpipe");
	}
	if(execlp(cmd,cmd,(char*)NULL)==-1)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(5,"Could not execlp %s",cmd);
	}
	exit(0);
}
