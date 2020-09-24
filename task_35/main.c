#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <err.h>
#include <errno.h>

void shut(int olderrno,int f1,int f2,int f3)
{
	if(f1!=-1)
	{
		close(f1);
	}
	if(f2!=-1)
	{
		close(f2);
	}
	if(f3!=-1)
	{
		close(f3);
	}
	if(olderrno!=-1)
	{
		errno=olderrno;
	}
}	
int main(int argc,char* argv[])
{
	if(argc!=4)
	{
		errx(1,"Invalid number of arguments: %s <fileName> <fileName> <fileName>",argv[0]);
	}
	const char* file1=argv[1];
	const char* file2=argv[2];
	const char* file3=argv[3];
	struct stat st;
	if(stat(file1,&st)<0)
	{
		err(2,"Cannot stat %s",file1);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"%s is not a regular file",file1);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"%s cannot be read",file1);
	}
	off_t size_f1=st.st_size;
	if(stat(file2,&st)<0)
	{
		err(5,"Cannot stat %s",file2);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(6,"%s is not a regular file",file2);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(7,"%s cannot be read",file2);
	}
	off_t size_f2=st.st_size;
	if(size_f1!=size_f2)
	{
		errx(8,"These files must have the same size");
	}
	int fd1=open(file1, O_RDONLY);
	if(fd1<0)
	{
		err(9,"Cannot open %s",file1);
	}
	int fd2=open(file2, O_RDONLY);
	if(fd2<0)
	{
		shut(errno,fd1,-1,-1);
		err(10,"Cannot open %s",file2);
	}
	int fd3=open(file3, O_CREAT | O_TRUNC | O_WRONLY , S_IRUSR | S_IWUSR);
	if(fd3<0)
	{
		shut(errno,fd1,fd2,-1);
		err(11,"Cannot open %s",file3);
	}
	struct triple
	{
		uint16_t position;
		uint8_t origin;
		uint8_t new;
	};
	struct triple triple_t;
	uint8_t f1;
	uint8_t f2;
	uint16_t count=0;
	ssize_t read_f1;
	ssize_t read_f2;
	while((read_f1=read(fd1,&f1,sizeof(f1)))>0&&(read_f2=read(fd2,&f2,sizeof(f2)))>0)
	{
		if(read_f1!=sizeof(f1))
		{
			shut(errno,fd1,fd2,fd3);
			errx(12,"Error while reading in %s",file1);
		}
		if(read_f2!=sizeof(f2))
		{
			shut(errno,fd1,fd2,fd3);
			errx(13,"Error while reading in %s",file2);
		}
		if(f1==f2)
		{
			continue;
		}
		triple_t.origin=f1;
		triple_t.new=f2;
		triple_t.position=count;
		ssize_t write_size=write(fd3,&triple_t,sizeof(triple_t));
		if(write_size<0)
		{
			shut(errno,fd1,fd2,fd3);
			err(14,"Could not write in %s",file3);
		}
		if(write_size!=sizeof(triple_t))
		{
			shut(errno,fd1,fd2,fd3);
			errx(15,"Error while writing in %s",file3);
		}
		count++;
	}
	if(read_f1<0)
	{
		shut(errno,fd1,fd2,fd3);
		err(16,"Could not read in %s",file1);
	}
	if(read_f2<0)
	{
		shut(errno,fd1,fd2,fd3);
		err(17,"Could not read in %s",file2);
	}
	shut(-1,fd1,fd2,fd3);
	exit(0);
}
