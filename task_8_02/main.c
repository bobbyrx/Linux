#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

int main (int argc,char* argv[])
{
	if(argc!=2)
	{
		errx(1,"Invalid number of arguments: %s <string>",argv[0]);
	}
	int a[2];
	if(pipe(a)<0)
	{
		err(2,"Could not make a pipe");
	}
	const char* str=argv[1];
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
		char c[strlen(str)];
		ssize_t read_size=read(a[0],&c,strlen(c));
		if(read_size<0)
		{
			int olderrno=errno;
			close(a[0]);
			errno=olderrno;
			err(4,"Could not read from parent");
		}
		if(read_size!=(ssize_t)strlen(c))
		{
			int olderrno=errno;
			close(a[0]);
			errno=olderrno;
			errx(5,"Error while reading from parent");
		}
		ssize_t write_size=write(1,&c,strlen(c));
		if(write_size<0)
		{
			int olderrno=errno;
			close(a[0]);
			errno=olderrno;
			err(6,"Could not write");
		}
		if(write_size!=(ssize_t)strlen(c))
		{
			int olderrno=errno;
			close(a[0]);
			errno=olderrno;
			errx(7,"Error while writing");
		}
		close(a[0]);
		exit(0);
	}
	close(a[0]);
	ssize_t write_size=write(a[1],str,strlen(str));
	if(write_size<0)
	{	
		int olderrno=errno;
		close(a[1]);
		errno=olderrno;
		err(8,"Could not write in pipe");
	}
	if(write_size!=(ssize_t)strlen(str))
	{
		int olderrno=errno;
		close(a[1]);
		errno=olderrno;
		errx(9,"Error while writing in pipe");
	}
	close(a[1]);
	if(wait(NULL)<0)
	{
		err(10,"Could not wait for child");
	}
	printf("\n");
	exit(0);
}
