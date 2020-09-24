#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

int main (int argc, char* argv[])
{
	if(argc<=3)
	{
		errx(1,"Invalid number of arguments: %s <fileName> ... <dirName>",argv[0]);
	}	
	const char* dir=argv[argc-1];
	struct stat st;
	if(stat(dir,&st)<0)
	{
		err(2,"Cannot stat %s",dir);
	}
	if(!S_ISDIR(st.st_mode))
	{
		errx(3,"The last argument must be a directory");
	}
	for(int i=1;i<argc-1;i++)
	{
		const char* file=argv[i];
		struct stat st_1;
		if(stat(file, &st_1)<0)
		{
			warn("Canntot stat %s",file);
			continue;
		}
		off_t st_size=st_1.st_size;
		int fd_file=open(file, O_RDONLY);
		if(fd_file<0)
		{
			warn("Cannot open %s",file);
			continue;
		}
		void *buff=malloc(st_size);
		if(!buff)
		{
			int olderrno=errno;
			close(fd_file);
			errno=olderrno;
			warn("No memory");
			continue;
		}
		ssize_t read_size=read(fd_file,buff,st_size);
		if(read_size<0)
		{
			int olderrno=errno;
			close(fd_file);
			free(buff);
			errno=olderrno;
			warn("Could not read %s",file);
			continue;
		}
		if(read_size!=st_size)
		{
			int olderrno=errno;
			close(fd_file);
			free(buff);
			errno=olderrno;
			warnx("Error while reading %s",file);
			continue;
		}
		close(fd_file);
		char destination[strlen(dir)+1+strlen(file)+1];
		strcpy(destination,dir);
		strcat(destination,"/");
		strcat(destination,file);
		int fd_dir=open(destination, O_CREAT | O_TRUNC | O_WRONLY , 0664);
		if(fd_dir<0)
		{
			int olderrno=errno;
			free(buff);
			errno=olderrno;
			warn("Could not copy %s to %s",file,destination);
			continue;
		}
		ssize_t write_size=write(fd_dir,buff,st_size);
		if(write_size<0)
		{
			int olderrno=errno;
			free(buff);
			close(fd_dir);
			errno=olderrno;
			warn("Could not write in %s",destination);
			continue;
		}
		if(write_size!=st_size)
		{
			int olderrno=errno;
			free(buff);
			close(fd_dir);
			errno=olderrno;
			warnx("Error while writing in %s",destination);
			continue;
		}
		free(buff);
		close(fd_dir);

	}
	exit(0);
}
