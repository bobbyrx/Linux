#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <string.h>

int compare(const void* a,const void *b)
{
	return *((unsigned char*)a)-*((unsigned char*)b);
}

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
		err(2,"Could not stat %s",file);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"%s is not a regular file",file);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"%s cannot be read",file);
	}
	off_t st_size=st.st_size;
	void * buff=malloc(st_size);
	if(!buff)
	{
		err(5,"No memory");
	}
	int fd=open(file, O_RDONLY);
	if(fd<0)
	{
		int olderrno=errno;
		free(buff);
		errno=olderrno;
		err(6,"Could not open %s",file);
	}
	ssize_t read_size=read(fd,buff,st_size);
	if(read_size<0)
	{
		int olderrno=errno;
		close(fd);
		free(buff);
		errno=olderrno;
		err(7,"Could not read %s",file);
	}
	if(read_size!=st_size)
	{
		int olderrno=errno;
		close(fd);
		free(buff);
		errno=olderrno;
		errx(8,"Error while reading from %s",file);
	}
	close(fd);
	qsort(buff,st_size,1,compare);
	int fd_sort=open("sorted",O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR);
	if(fd_sort<0)
	{
		int olderrno=errno;
		free(buff);
		errno=olderrno;
		err(9,"Could not open sorted");
	}
	ssize_t write_size=write(fd_sort,buff,st_size);
	if(write_size<0)
	{
		int olderrno=errno;
		free(buff);
		close(fd_sort);
		errno=olderrno;
		err(10,"Could not write in sorted");
	}
	if(write_size!=st_size)
	{
		int olderrno=errno;
		free(buff);
		close(fd_sort);
		errno=olderrno;
		errx(11,"Error while writing in sorted");
	}
	close(fd_sort);
	free(buff);
	exit(0);
}
