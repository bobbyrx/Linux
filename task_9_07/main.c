#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

int main()
{
	const char* text1="foo";
       	const char* text2="bar\n";
	const char* file="file.txt";
	int fd =open(file, O_CREAT | O_TRUNC | O_RDWR , S_IRUSR | S_IWUSR);
	if(fd<0)
	{
		err(1,"Could not open %s",file);
	}	
	pid_t a=fork();
	if(a<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(2,"Could not fork");
	}
	if(a==0)
	{
		ssize_t write_size=write(fd,text1,strlen(text1));
		if(write_size<0)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			err(3,"Could not write in %s",file);
		}
		if(write_size!=(ssize_t)strlen(text1))
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			errx(4,"Error while writing in %s",file);
		}
		close(fd);
		exit(0);
	}

	int status;
	pid_t wait_status=wait(&status);
	if(wait_status<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(5,"Could not wait child");
	}
	if(!WIFEXITED(status))
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(6,"Child process did not exit properly");
	}
	if(WEXITSTATUS(status)!=0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		errx(7,"Child process doesn't have exit status equal to 0");
	}
	ssize_t write_size=write(fd,text2,strlen(text2));
	if(write_size<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(8,"Could not write in %s",file);
	}
	if(write_size!=(ssize_t)strlen(text2))
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		errx(9,"Error while writing in %s",file);
	}
	close(fd);
	printf("The processes is done\n");
	exit(0);
}
