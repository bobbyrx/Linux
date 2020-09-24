#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <err.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>

int main(int argc,char* argv[])
{
	if(argc!=2)
	{
		errx(1,"Invalid number of arguments: %s <string>",argv[0]);
	}
	const char* str=argv[1];
	int a[2];
	if(pipe(a)<0)
	{
		err(2,"Could not make a pipe");
	}
	pid_t p=fork();
	if(p<0)
	{
		int olderrno=errno;
		close(a[0]);
		close(a[1]);
		errno=olderrno;
		err(3,"Could not fork");
	}
	if(p==0)
	{
		close(a[1]);
		if(dup2(a[0],0)<0)
		{
			int olderrno=errno;
			close(a[0]);
			errno=olderrno;
			err(4,"Could not dup2");
		}
		if(execlp("wc","wc","-c",(char*)NULL)==-1)
		{
			int olderrno=errno;
			close(a[0]);
			errno=olderrno;
			err(5,"Could not exec wc -c %s",str);
		}
	}
	close(a[0]);
	ssize_t write_size=write(a[1],str,strlen(str));
	if(write_size<0)
	{
		int olderrno=errno;
		close(a[1]);
		errno=olderrno;
		err(6,"Could not write in pipe");
	}
	if(write_size!=(ssize_t)strlen(str))
	{
		int olderrno=errno;
		close(a[1]);
		errno=olderrno;
		errx(7,"Error while writing in pipe");
	}
	close(a[1]);
	if(wait(NULL)<0)
	{
		err(8,"Could not wait for child");
	}
	exit(0);
}
