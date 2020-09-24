#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		errx(1,"Invalid number of arguments: %s <fileName1> <fileName2>",argv[0]);
	}
	const char* file1=argv[1];
	const char* file2=argv[2];
	struct stat st;
	if(stat(file1,&st)<0)
	{
		err(2,"Could not stat %s",file1);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"%s is not a regular file",file1);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"%s can't be read",file1);	
	}	
	off_t st_size=st.st_size;
	struct pair_t
	{
		uint32_t start;
		uint32_t length;
	};
	if(st_size%sizeof(struct pair_t)!=0)
	{
		errx(19,"The file %s does not have the right format",file1);
	}
	if(stat(file2,&st)<0)
	{
		err(5,"Could not stat %s",file2);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(6,"%s is not a regular file",file2);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(7,"%s can't be read",file2);
	}
	off_t st_size_2=st.st_size;
	if(st_size_2%sizeof(uint32_t)!=0)
	{
		errx(8,"The file %s has wrong format",file2);
	}
	int fd1=open(file1, O_RDONLY);
	if(fd1<0)
	{
		err(9,"Could not open %s",file1);
	}
	int fd2=open(file2, O_RDONLY);
	if(fd2<0)
	{
		int olderrno=errno;
		close(fd1);
		errno=olderrno;
		err(10,"Could not open %s",file2);
	}
	int fd3=open("f3", O_CREAT | O_TRUNC | O_WRONLY , 0644);
	if(fd3<0)
	{
		int olderrno=errno;
		close(fd1);
		close(fd2);
		errno=olderrno;
		err(11,"Could not open f3");
	}
	struct pair_t pair;
	ssize_t read_size;
	while((read_size=read(fd1,&pair,sizeof(pair)))>0)
	{
		if(read_size!=sizeof(pair))
		{
			int olderrno=errno;
			close(fd1);
			close(fd2);
			close(fd3);
			errno=olderrno;
			errx(13,"Error while reading in %s",file1);
		}
		uint32_t begin=pair.start-1;
		off_t check=lseek(fd2,begin*sizeof(uint32_t),SEEK_SET);
		if(check<0)
		{
			const int olderrno=errno;
			close(fd1);
			close(fd2);
			close(fd3);
			errno=olderrno;
			err(14,"Error in positioning in file %s",file2);
		}
		uint32_t len=0;
		uint32_t current;
		ssize_t read_fd2;
		while((read_fd2=read(fd2,&current,sizeof(current)))>0)
		{
			if(read_fd2!=sizeof(current))
			{
				const int olderrno=errno;
				close(fd1);
				close(fd2);
				close(fd3);
				errno=olderrno;
				errx(16,"Error while reading in %s",file2);
			}
			ssize_t write_size=write(fd3,&current,sizeof(current));
			if(write_size<0)
			{
				const int olderrno=errno;
				close(fd1);
				close(fd2);
				close(fd3);
				errno=olderrno;
				err(17,"Could not write in f3");
			}
			if(write_size!=sizeof(current))
			{
				const int olderrno=errno;
				close(fd1);
				close(fd2);
				close(fd3);
				errno=olderrno;
				errx(18,"Error while writing in f3");
			}
			len++;
			if(len==pair.length)
			{
				break;
			}
		}
		if(read_fd2<0)
		{
			const int olderrno=errno;
			close(fd1);
			close(fd2);
			close(fd3);
			errno=olderrno;
			err(15,"Could not read in %s",file2);
		}
	}
	if(read_size<0)
	{
		int olderrno=errno;
		close(fd1);
		close(fd2);
		close(fd3);
		errno=olderrno;
		err(12,"Could not read in %s",file1);
	}
	close(fd1);
	close(fd2);
	close(fd3);
	exit(0);
}	
