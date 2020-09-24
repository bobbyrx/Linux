#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if(argc!=3)
	{
		errx(1,"Invalid number of arguments: %s <fileName1> <fileName2>",argv[0]);
	}
	const char* file1=argv[1];
	const char* file2=argv[2];
	const char* file3="f3_1";
	if(strcmp(file1,file2)==0)
	{
		errx(2,"The files ( %s and %s ) are the same",file1,file2);
	}
	struct stat st;
	if(stat(file1,&st)<0)
	{
		err(3,"Could not stat %s",file1);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(4,"%s is not a regular file",file1);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(5,"Could not read %s",file1);
	}
	off_t st_size1=st.st_size;
	printf("%ld\n",st_size1);
	struct pair_t
	{
		uint32_t start;
		uint32_t length;
	};
	if(st_size1%sizeof(struct pair_t)!=0)
	{
		errx(6,"%s has wrong format",file1);
	}
	if(stat(file2,&st)<0)
	{
		err(7,"Could not stat %s",file2);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(8,"%s is not a regular file",file2);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(9,"%s can't be read",file2);
	}
	off_t st_size2=st.st_size;
	if(st_size2%sizeof(uint32_t)!=0)
	{
		errx(10,"%s has wrong format",file2);
	}
	int fd1=open(file1, O_RDONLY);
	if(fd1<0)
	{
		err(11,"Can't open %s",file1);
	}
	int fd2=open(file2, O_RDONLY);
	if(fd2<0)
	{
		const int olderrno=errno;
		close(fd1);
		errno=olderrno;
		err(12,"Can't open %s",file2);
	}
	int fd3=open(file3, O_CREAT | O_TRUNC | O_WRONLY , S_IRUSR | S_IWUSR);
	if(fd3<0)
	{
		const int olderrno=errno;
		close(fd1);
		close(fd2);
		errno=olderrno;
		err(13,"Can't open %s",file3);
	}
	struct pair_t pair;
	ssize_t read_size;
	while((read_size=read(fd1,&pair,sizeof(pair)))>0)
	{
		if(read_size!=sizeof(pair))
		{
			const int olderrno=errno;
			close(fd1);
			close(fd2);
			close(fd3);
			errno=olderrno;
			errx(15,"Error while reading %s",file1);
		}
		uint32_t position=pair.start-1;
		off_t check=lseek(fd2,position*sizeof(uint32_t),SEEK_SET);
		if(check<0)
		{
			const int olderrno=errno;
			close(fd1);
			close(fd2);
			close(fd3);
			errno=olderrno;
			err(16,"Error in positing in file %s",file2);
		}
		uint32_t current;
		uint32_t len=0;
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
				errx(18,"Error while reading %s",file2);
			}
			ssize_t write_size=write(fd3,&current,sizeof(current));
			if(write_size<0)
			{
				const int olderrno=errno;
				close(fd1);
				close(fd2);
				close(fd3);
				errno=olderrno;
				err(19,"Could not write in %s",file3);
			}
			if(write_size!=sizeof(current))
			{
				const int olderrno=errno;
				close(fd1);
				close(fd2);
				close(fd3);
				errno=olderrno;
				errx(20,"Error while writing in %s",file3);
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
			err(17,"Could not read %s",file2);
		}
		
	}
	if(read_size<0)
	{
		const int olderrno=errno;
		close(fd1);
		close(fd2);
		close(fd3);
		errno=olderrno;
		err(14,"Could not read %s",file1);
	}
	close(fd1);
	close(fd2);
	close(fd3);
	printf("Done\n");
	exit(0);
}
