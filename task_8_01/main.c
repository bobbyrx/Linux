#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		errx(1,"Invalid number of arguments: %s <fileName>",argv[0]);
	}
	const char* file=argv[1];
	const char* line1="foo\n";
	const char* line2="bar\n";
	int fd=open(file, O_CREAT | O_TRUNC | O_WRONLY , S_IRUSR | S_IWUSR);
	if(fd<0)
	{
		err(2,"Could not open %s",file);
	}
	ssize_t write_size=write(fd,line1,2);
	if(write_size<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(3,"Could not write in %s",file);
	}
	if(write_size!=2)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		errx(4,"Error while writing in %s",file);
	}
	pid_t p=fork();
	if(p<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(5,"Could not fork");
	}
	if(p==0)
	{
		write_size=write(fd,line2,strlen(line2));
		if(write_size<0)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			err(6,"Could not write in %s",file);
		}
		if(write_size!=(ssize_t)strlen(line2))
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			errx(7,"Error while writing in %s",file);
		}
		exit(0);
	}

	int status;
	pid_t wait_status=wait(&status);
	if(wait_status<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(8,"Could not wait for child");
	}
	if(!WIFEXITED(status))
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(9,"Process didn't exit properly");
	}
	if(WEXITSTATUS(status)!=0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		errx(10,"Exit status of child is not 0");
	}
	write_size=write(fd,line1+2,2);
	if(write_size<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(11,"Could not write in %s",file);
	}
	if(write_size!=2)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		errx(12,"Error while writing in %s",file);
	}
	close(fd);
	printf("Done\n");
	exit(0);
}
