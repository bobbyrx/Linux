#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>

void swap(const char* f_1,const char* f_2, off_t size_1,off_t size_2)
{

	void* buff1=malloc(size_1);
	if(!buff1)
	{
		err(10,"No memory");
	}
	int fd=open(f_1, O_RDONLY);
	if(fd==-1)
	{	
		free(buff1);
		err(11,"Could not open %s",f_1);
	}
	ssize_t read_size=read(fd,buff1,size_1);
	if(read_size==-1)
	{
		int olderrno=errno;
		close(fd);
		free(buff1);
		errno=olderrno;
		err(12,"Could not read %s",f_1);
	}
	if(read_size!=size_1)
	{
		int olderrno=errno;
		close(fd);
		free(buff1);
		errno=olderrno;
		errx(13,"Incorrect reading of %s",f_1);
	}
	close(fd);
	int fd_1=open(f_2, O_RDONLY);
	if(fd_1==-1)
	{	
		free(buff1);
		err(14,"Cound not open %s",f_2);
	}	
	void* buff2=malloc(size_2);
	if(!buff2)
	{
		int olderrno=errno;
		free(buff1);
		close(fd_1);
		errno=olderrno;
		err(15,"No memory");
	}
	ssize_t read_size_2=read(fd_1,buff2,size_2);
	if(read_size_2==-1)
	{
		int olderrno=errno;
		free(buff1);
		close(fd_1);
		free(buff2);
		errno=olderrno;
		err(16,"Can't read %s",f_2);
	}
	if(read_size_2!=size_2)
	{
		int olderrno=errno;
		free(buff1);
		close(fd_1);
		free(buff2);
		errno=olderrno;
		errx(16,"Error while reading %s",f_2);
	}
	close(fd_1);
	fd=open(f_1, O_TRUNC | O_WRONLY);
	if(fd==-1)
	{
		free(buff1);
		free(buff2);
		err(17,"Cannot open %s",f_1);
	}
	fd_1=open(f_2, O_TRUNC | O_WRONLY);
	if(fd_1==-1)
	{
		int olderrno=errno;
		free(buff1);
		close(fd);
		free(buff2);
		errno=olderrno;
		err(18,"Cannot open %s",f_2);
	}
	ssize_t write_size=write(fd,buff2,size_2);
	if(write_size==-1)
	{
		int olderrno=errno;
		free(buff1);
		close(fd);
		close(fd_1);
		free(buff2);
		errno=olderrno;
		err(19,"Cannot write in %s",f_1);
	}
	if(write_size!=size_2)
	{
		int olderrno=errno;
		free(buff1);
		close(fd);
		close(fd_1);
		free(buff2);
		errno=olderrno;
		errx(20,"Error while writting in %s",f_1);
	}
	close(fd);
	free(buff2);
	ssize_t write_size_1=write(fd_1,buff1,size_1);
	if(write_size_1==-1)
	{
		int olderrno=errno;
		free(buff1);
		close(fd_1);
		errno=olderrno;
		err(21,"Cannot write in %s",f_2);
	}
	if(write_size_1!=size_1)
	{
		int olderrno=errno;
		free(buff1);
		close(fd_1);
		errno=olderrno;
		errx(22,"Error while writing in %s",f_2);
	}
	
	close(fd_1);
	free(buff1);
	printf("Swapped %s with %s\n",f_1,f_2);	
}

int main(int argc, char* argv[])
{
	if(argc!=3)
	{
		errx(1,"Invalid number of arguments: %s <fileName> <fileName>",argv[0]);
	}

	const char* file1=argv[1];
	const char* file2=argv[2];
	if(strcmp(file1,file2)==0)
	{
		printf("Will not swap: Input files are the same\n");
		exit(0);
	}
	struct stat st;
	if(stat(file1,&st)==-1)
	{
		err(2,"Cannot stat %s",file1);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"File %s is not a regular file.",file1);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"File %s cannot be read",file1);
	}
	if(!(st.st_mode & S_IWUSR))
	{
		errx(5,"File %s cannot be written",file1);
	}
	off_t st_size_1=st.st_size;
	if(stat(file2,&st)==-1)
	{
		err(6,"Cannot stat %s",file2);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(7,"File %s is not a regular file",file2);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(8,"File %s cannot be read",file2);
	}
	if(!(st.st_mode & S_IWUSR))
	{
		errx(9,"File %s cannot be written",file2);
	}
	off_t st_size_2=st.st_size;
	swap(file1,file2,st_size_1,st_size_2);
	exit(0);
}

