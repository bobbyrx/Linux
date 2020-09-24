#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>
#include <sys/stat.h>
int main (int argc, char* argv[])
{
	if(argc!=2)
	{
		errx(1,"Invalid number of arguments: %s <fileName>",argv[0]);
	}
	const char* file=argv[1];
	int fd=open(file, O_CREAT | O_TRUNC | O_RDWR , S_IRUSR | S_IWUSR);
	if(fd<0)
	{
		err(2,"Could not read open %s",file);
	}
	const char* text="Borislav is a hero!!!!!\n";
	pid_t a=fork();
	if(a<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(3,"Could not fork");
	}
	if(a==0)
	{
		ssize_t write_size=write(fd,text,strlen(text));
		if(write_size<0)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			err(4,"Could not write in %s",file);
		}
		if(write_size!=(ssize_t)strlen(text))
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			errx(5,"Error while writing in %s",file);
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
		err(6,"Could not wait for child");
	}
	if(!WIFEXITED(status))
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(7,"Process did not exited properly");
	}
	if(WEXITSTATUS(status)!=0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		errx(8,"Process does not have exit status equal to 0");
	}
	off_t file_set=lseek(fd,0,SEEK_SET);
	if(file_set<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(9,"Could not set %s's offset to 0",file);
	}
	char c;
	ssize_t read_size;
	int count=0;
	while((read_size=read(fd,&c,sizeof(c)))>0)
	{
		if(read_size!=sizeof(c))
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			errx(10,"Error while reading %s",file);
		}
		ssize_t write_size;
		if(count%2==0&&count!=0)
		{
			char space=' ';
			write_size=write(1,&space,sizeof(space));
			if(write_size<0)
			{
				int olderrno=errno;
				close(fd);
				errno=olderrno;
				err(11,"Could not write");
			}
			if(write_size!=sizeof(space))
			{
				int olderrno=errno;
				close(fd);
				errno=olderrno;
				errx(12,"Error while writing");
			}
		}
		write_size=write(1,&c,sizeof(c));
		if(write_size<0)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			err(11,"Could not write");
		}
		if(write_size!=sizeof(c))
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			errx(12,"Error while writing");
		}
		count++;
	}
	if(read_size<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(13,"Could not read form %s",file);
	}
	close(fd);
	exit(0);
}
