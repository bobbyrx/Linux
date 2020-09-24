#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	if(argc<=1)
	{
		errx(1,"Invalid number of arguments: %s <fileName1> ... <fileNameN>", argv[0]);
	}
	for(int i=1;i<argc;i++)
	{
		const char* file=argv[i];
		struct stat st;
		if(stat(file, &st)<0)
		{
			warn("Cannot stat %s",file);
			continue;
		}
		if(!S_ISREG(st.st_mode))
		{
			warnx("File %s is not a regular file",file);
			continue;
		}
		if(!(st.st_mode & S_IRUSR))
		{
			warnx("File %s cannot be read", file);
			continue;
		}
		off_t st_size=st.st_size;
		char c[st_size];
		int fd=open(file, O_RDONLY);
		if(fd<0)
		{
			warn("Cannot open %s",file);
			continue;
		}
		ssize_t read_size=read(fd,&c,st_size);
		if(read_size<0)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			warn("Cannot read %s",file);
			continue;
		}
		if(read_size!=st_size)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			warnx("Error while reading %s",file);
			continue;
		}
		close(fd);
		ssize_t write_size=write(1,&c,st_size);
		if(write_size<0)
		{
			warn("Cannot write in terminal");
			continue;
		}
		if(write_size!=st_size)
		{
			warnx("Error while writing in terminal");
			continue;
		}
	}
	exit(0);
}
