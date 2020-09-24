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
		errx(1,"Invalid number of arguments: %s <fileName>",argv[0]);
	}
	const char* file=argv[1];
	struct stat st;
	if(stat(file,&st)<0)
	{
		err(2,"Cannot stat %s",file);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"%s is not a regular file",file);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"%s cannot be read",file);
	}
	const char* pipe="mkpipe";
	if(stat(pipe,&st)<0)
	{
		if(mkfifo(pipe, 0644)<0)
		{
			err(5,"Cannot make mkpipe");
		}
	}
	int fd=open(pipe, O_WRONLY);
	if(fd<0)
	{
		err(6,"Cannot open pipe");
	}
	if(dup2(fd,1)<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(7,"Cannot dup2");
	}
	if(execlp("cat","cat",file,(char*)NULL)==-1)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(8,"Cannot exec cat %s",file);
	}

	exit(0);
}
