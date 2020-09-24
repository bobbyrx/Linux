#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <err.h>

int main()
{
	const char* file="/etc/passwd";
	int a[2];
	if(pipe(a)<0)
	{
		err(1,"Could not pipe");
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
		if(execlp("cut","cut","-d:","-f7",file,(char*)NULL)==-1)
		{
			int olderrno=errno;
			close(a[1]);
			errno=olderrno;
			err(4,"Could not cut");
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
		err(6,"Could not fork");
	}
	if(f==0)
	{
		close(b[0]);
		if(dup2(a[0],0)<0)
		{
			int olderrno=errno;
			close(a[0]);
			close(b[1]);	
			errno=olderrno;
			err(6,"Could not dup");
		}
		if(dup2(b[1],1)<0)
		{
			int olderrno=errno;
			close(a[0]);
			close(b[1]);	
			errno=olderrno;
			err(7,"Could not dup");
		}
		if(execlp("sort","sort",(char*)NULL)==-1)
		{
			int olderrno=errno;
			close(a[0]);
			close(b[1]);	
			errno=olderrno;
			err(8,"Could not exec sort");
		}
	}
	close(a[0]);
	close(b[1]);
	wait(NULL);
	if(dup2(b[0],0)<0)
	{
		int olderrno=errno;
		close(b[0]);
		errno=olderrno;
		err(9,"Could not dup");
	}
	if(execlp("uniq","uniq",(char*)NULL)==-1)
	{
		int olderrno=errno;
		close(b[0]);
		errno=olderrno;
		err(10,"Could not exec uniq");
	}
	exit(0);
}
