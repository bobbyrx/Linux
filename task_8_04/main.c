#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		errx(11,"Invalid number of arguments: %s <fileName>",argv[0]);
	}	
	const char * file=argv[1];
	int a[2];
	if(pipe(a)<0)
	{
		err(1,"Could not make a pipe");
	}
	pid_t p=fork();
	if(p<0)
	{
		int olderrno=errno;
		close(a[0]);
		close(a[1]);
		errno=olderrno;
		err(2,"Could not fork");
	}
	if(p==0)
	{
		close(a[0]);
		if(dup2(a[1],1)<0)
		{
			int olderrno=errno;
			close(a[1]);
			errno=olderrno;
			err(3,"Could not dup2");
		}
		if(execlp("cat","cat",file,(char*)NULL)==-1)
		{
			int olderrno=errno;
			close(a[1]);
			errno=olderrno;
			err(4,"Could cat %s",file);
		}
	}
	close(a[1]);
	int b[2];
	if(pipe(b)<0)
	{
		int olderrno=errno;
		close(a[0]);
		errno=olderrno;
		err(5,"Could not pipe");
	}
	pid_t f=fork();
	if(f<0)
	{
		int olderrno=errno;
		close(a[0]);
		close(b[0]);
		close(b[1]);
		errno=olderrno;
		err(5,"Could not pipe");
	}
	if(f==0)
	{
		close(b[0]);
		if(dup2(a[0],0)<0)
		{
			int olderrno=errno;
			close(b[1]);
			close(a[0]);
			errno=olderrno;
			err(6,"Could not dup2");
		}
		if(dup2(b[1],1)<0)
		{
			int olderrno=errno;
			close(b[1]);
			close(a[0]);
			errno=olderrno;
			err(7,"Could not dup2");
		}
		if(execlp("sort","sort",(char*)NULL)==-1)
		{
			int olderrno=errno;
			close(b[1]);
			close(a[0]);
			errno=olderrno;
			err(8,"Could not sort");
		}
	}
	close(a[0]);
	close(b[1]);
	if(dup2(b[0],0)<0)
	{
		int olderrno=errno;
		close(b[0]);
		errno=olderrno;
		err(9,"Could not dup2");
	}
	if(execlp("uniq","uniq",(char*)NULL)==-1)
	{
		int olderrno=errno;
		close(b[0]);
		errno=olderrno;
		err(10,"Could not do uniq");
	}
	wait(NULL);
	exit(0);
}
