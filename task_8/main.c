#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

int main()
{
	const char* file="/etc/passwd";
	char c;
	int fd=open(file, O_RDONLY);
	if(fd<0)
	{
		err(1,"Cannot open %s",file);
	}
	const char* file_pass="passwd";
	int fd_pass=open(file_pass, O_CREAT | O_TRUNC | O_WRONLY , 0644);
	if(fd_pass<0)
	{
		int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(2,"Could not copy %s to %s",file,file_pass);
	}
	ssize_t read_size;
	while((read_size=read(fd,&c,sizeof(c)))>0)
	{
		if(read_size!=sizeof(c))
		{
			int olderrno=errno;
			close(fd);
			close(fd_pass);
			errno=olderrno;
			errx(3,"Error while reading %s",file);
		}
		if(c==':')
		{
			c='?';
		}
		ssize_t write_size=write(fd_pass,&c,sizeof(c));
		if(write_size<0)
		{
			int olderrno=errno;
			close(fd);
			close(fd_pass);
			errno=olderrno;
			err(4,"Could not write in %s",file_pass);
		}		
		if(write_size!=sizeof(c))
		{
			int olderrno=errno;
			close(fd);
			close(fd_pass);
			errno=olderrno;
			errx(5,"Error while writing in %s",file_pass);
		}		
	}
	if(read_size<0)
	{
		int olderrno=errno;
		close(fd);
		close(fd_pass);
		errno=olderrno;
		err(6,"Could not read %s",file);
	}
	close(fd);
	close(fd_pass);
	exit(0);
}
