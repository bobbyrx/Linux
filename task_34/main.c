#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <string.h>

void shut(int olderrno, int f1,int f2,int f3,int f4)
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
	if(f4!=-1)
	{
		close(f4);
	}
	if(olderrno!=-1)
	{
		errno=olderrno;
	}
}

int main(int argc,char* argv[])
{
	if(argc!=5)
	{
		errx(1,"Invalid number of arguments: %s <file1.dat> <file1.idx> <file2.dat> <file2.idx>",argv[0]);
	}
	const char* f1_dat=argv[1];
	const char* f1_idx=argv[2];
	const char* f2_dat=argv[3];
	const char* f2_idx=argv[4];
	struct stat st;
	if(stat(f1_dat,&st)<0)
	{
		err(2,"Could not stat %s",f1_dat);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"%s is not a regular file",f1_dat);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"%s can't be read",f1_dat);
	}
	if(stat(f1_idx,&st)<0)
	{
		err(5,"Could not stat %s",f1_idx);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(6,"%s is not a regular file",f1_idx);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(7,"%s can't be read",f1_idx);
	}
	off_t st_size=st.st_size;
	struct triple
	{
		uint16_t start;
		uint8_t length;
		uint8_t save;
	};
	if(st_size%sizeof(struct triple)!=0)
	{
		errx(8,"%s has a wrong format",f1_idx);
	}
	int fd_1_dat;
	int fd_1_idx;
	int fd_2_dat;
	int fd_2_idx;
	fd_1_dat=open(f1_dat, O_RDONLY);
	if(fd_1_dat<0)
	{
		err(9,"Could not open %s",f1_dat);
	}
	fd_1_idx=open(f1_idx, O_RDONLY);
	if(fd_1_idx<0)
	{
		shut(errno,fd_1_dat,-1,-1,-1);
		err(10,"Could not open %s",f1_idx);
	}
	fd_2_dat=open(f2_dat, O_CREAT | O_TRUNC | O_WRONLY , 0644);
	if(fd_2_dat<0)
	{
		shut(errno,fd_1_dat,fd_1_idx,-1,-1);
		err(11,"Could not open %s",f2_dat);
	}
	fd_2_idx=open(f2_idx, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if(fd_2_idx<0)
	{
		shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,-1);
		err(12,"Could not open %s",f2_idx);
	}
	struct triple triple_f1;
	struct triple triple_f2;
	triple_f2.start=0;
	ssize_t read_size;
	while((read_size=read(fd_1_idx,&triple_f1,sizeof(triple_f1)))>0)
	{
		if(read_size!=sizeof(triple_f1))
		{
			shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
			errx(13,"Error while reading %s",f1_idx);
		}
		uint16_t position=triple_f1.start;
		uint8_t end=triple_f1.length;
		off_t check=lseek(fd_1_dat,position*sizeof(uint8_t),SEEK_SET);
		if(check<0)
		{
			shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
			err(14,"Error while positioning in %s",f1_dat);
		}
		char c;
		uint8_t len=0;
		bool isBegin=true;
		ssize_t read_size_1;
		while((read_size_1=read(fd_1_dat,&c,sizeof(c)))>0)
		{
			if(read_size_1!=sizeof(c))
			{
				shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
				errx(15,"Error while reading in %s",f1_dat);
			}
			if(isBegin)
			{
				if(c<'A'||c>'Z')
				{
					break;
				}
			}
			isBegin=false;
			ssize_t write_size=write(fd_2_dat,&c,sizeof(c));
			if(write_size<0)
			{
				shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
				err(16,"Could not write in %s",f2_dat);
			}
			if(write_size!=sizeof(c))
			{
				shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
				errx(17,"Error while writing in %s",f2_dat);
			}
			len++;
			if(len==end)
			{
				triple_f2.length=len;
				triple_f2.save=triple_f1.save;
				ssize_t write_size_1=write(fd_2_idx,&triple_f2,sizeof(triple_f2));
				if(write_size_1<0)
				{
					shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
					err(18,"Could not write in %s",f2_idx);
				}
				if(write_size_1!=sizeof(triple_f2))
				{
					shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
					errx(19,"Error while writing in %s",f2_idx);
				}
				triple_f2.start+=len;
				break;
			}
		}
		if(read_size_1<0)
		{
			shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
			err(20,"Could not read in %s",f1_dat);
		}
	}
	if(read_size<0)
	{
		shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
		errx(21,"Could not read in %s",f1_idx);
	}
	shut(errno,fd_1_dat,fd_1_idx,fd_2_dat,fd_2_idx);
	exit(0);
}
