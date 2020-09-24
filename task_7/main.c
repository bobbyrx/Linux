#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
	if(argc<2)
	{
		errx(1,"Invalid number of arguments: %s <fileName1> ... <fileNameN>",argv[0]);
	}

		const char* file=argv[1];
		struct stat st;
		if(stat(file, &st)<0)
		{
			err(2,"Cannot stat %s",file);
		}
		off_t st_size=st.st_size;
		char c[st_size];
		int fd=open(file, O_RDONLY);
		if(fd<0)
		{
			err(3,"Cannot open %s",file);
		}
		ssize_t read_size=read(fd,&c,st_size);
		if(read_size<0)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			err(4,"Cannot read %s",file);
		}
		if(read_size!=st_size)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			errx(5,"Error while reading %s",file);
		}

	for(int i=2;i<argc;i++)
	{
		const char* file_copy=argv[i];
		if(strcmp(file,file_copy)==0)
		{
			printf("Cannot copy with the same name\n");
			continue;
		}
		int fd_copy=open(file_copy, O_CREAT | O_TRUNC | O_WRONLY , S_IRUSR | S_IWUSR);
		if(fd_copy<0)
		{
			warn("Cannot copy %s", file_copy);
			continue;
		}
		ssize_t write_size=write(fd_copy,&c,st_size);
		if(write_size<0)
		{
			int olderrno=errno;
			close(fd_copy);
			errno=olderrno;
			warn("Could not write in %s",file_copy);
			continue;
		}
		if(write_size!=st_size)
		{
			int olderrno=errno;
			close(fd_copy);
			errno=olderrno;
			warnx("Error while coppiing %s",file_copy);
			continue;
		}
		close(fd_copy);
	}


	close(fd);
	exit(0);
}	
