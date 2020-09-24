#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>
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
struct header
{
	uint32_t magic;
	uint8_t hd_version;
	uint8_t data;
	uint16_t count;
	uint32_t reserved_1;
	uint32_t reserved_2;
}__attribute__((packed));
struct data_00
{
	uint16_t offset;
	uint8_t origin;
	uint8_t new;
}__attribute__((packed));
struct data_01
{
	uint32_t offset;
	uint16_t origin;
	uint16_t new;
}__attribute__((packed));
int main(int argc,char* argv[])
{
	if(argc!=4)
	{
		errx(1,"Invalid number of arguments: %s <fileName> <fileName> <newFileName>",argv[0]);
	}
	const char* patch=argv[1];
	const char* file1=argv[2];
	const char* file2=argv[3];
	struct stat st;
	if(stat(patch, &st)<0)
	{
		err(2,"Cannot stat %s",patch);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"%s is not a regular file",patch);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"%s  can't be read",patch);
	}
	off_t patch_size=st.st_size;
	patch_size-=sizeof(struct header);
	if(patch_size%sizeof(struct data_00)!=0||patch_size%sizeof(struct data_01)!=0)
	{
		errx(5,"%s does not have the right format",patch);
	}
	if(stat(file1, &st)<0)
	{
		err(6,"Cannot stat %s",file1);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(7,"%s is not a regular file",file1);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(8,"%s cannot be read",file1);
	}
	off_t f1_size=st.st_size;
	
	struct header header_t;
	struct data_00 data_00_t;
	struct data_01 data_01_t;
	
	int f1=open(file1, O_RDONLY);
	if(f1<0)
	{
		err(9,"Could not open %s",file1);
	}
	int f2=open(file2, O_CREAT | O_TRUNC | O_RDWR , S_IRUSR | S_IWUSR);
	if(f2<0)
	{
		shut(errno,f1,-1,-1);
		err(10,"Could not open %s",file2);
	}
	void* buff=malloc(f1_size);
	if(!buff)
	{
		shut(errno,f1,f2,-1);
		err(11,"No memory");
	}
	ssize_t read_f1=read(f1,buff,f1_size);
	if(read_f1<0)
	{
		free(buff);
		shut(errno,f1,f2,-1);
		err(12,"Cannot read %s",file1);
	}
	if(read_f1!=f1_size)
	{
		free(buff);
		shut(errno,f1,f2,-1);
		errx(13,"Error while reading %s",file1);
	}
	close(f1);
	ssize_t write_f2=write(f2,buff,f1_size);
	if(write_f2<0)
	{
		free(buff);
		shut(errno,-1,f2,-1);
		err(14,"Could not write in %s",file2);
	}
	if(write_f2!=f1_size)
	{
		free(buff);
		shut(errno,-1,f2,-1);
		errx(15,"Error while writing in %s",file2);
	}
	free(buff);
	int fd_patch=open(patch, O_RDONLY);
	if(fd_patch<0)
	{
		shut(errno,-1,f2,-1);
		err(16,"Could not open %s",patch);
	}
	ssize_t read_patch=read(fd_patch,&header_t,sizeof(header_t));
	if(read_patch<0)
	{
		shut(errno,-1,f2,fd_patch);
		err(17,"Could not read in %s",patch);
	}
	if(read_patch!=sizeof(header_t))
	{
		shut(errno,-1,f2,fd_patch);
		errx(18,"Error while reading in %s",patch);
	}
	if(header_t.data==0x00)
	{
		if(f1_size%sizeof(uint8_t)!=0)
		{
			shut(errno,-1,f2,fd_patch);
			errx(19,"%s has wrong format",file1);
		}
		ssize_t read_patch;
		while((read_patch=read(fd_patch,&data_00_t,sizeof(data_00_t)))>0)
		{
			if(read_patch!=sizeof(data_00_t))
			{
				shut(errno,-1,f2,fd_patch);
				errx(20,"Error while reading in %s",patch);
			}
			uint16_t position=data_00_t.offset;
			uint8_t original=data_00_t.origin;
			off_t check=lseek(f2,position*sizeof(uint8_t),SEEK_SET);
			if(check<0)
			{
				shut(errno,-1,f2,fd_patch);
				err(21,"Error in positioning in %s",file2);
			}
			uint8_t current;
			ssize_t read_f2=read(f2,&current,sizeof(current));
			if(read_f2<0)
			{
				shut(errno,-1,f2,fd_patch);
				err(22,"Could not read in %s",file2);
			}
			if(read_f2!=sizeof(current))
			{
				shut(errno,-1,f2,fd_patch);
				errx(23,"Error while reading in %s",file2);
			}
			if(current==original)
			{
		  		uint8_t new_t=data_00_t.new;
				check=lseek(f2,position*sizeof(uint8_t),SEEK_SET);
				if(check<0)
				{
					shut(errno,-1,f2,fd_patch);
					err(24,"Error in positioning in %s",file2);
				}
				ssize_t over_write=write(f2,&new_t,sizeof(new_t));
				if(over_write<0)
				{
					shut(errno,-1,f2,fd_patch);
					err(25,"Could not write in %s",file2);
				}
				if(over_write!=sizeof(new_t))
				{
					shut(errno,-1,f2,fd_patch);
					errx(26,"Error while writing %s",file2);
				}
			}
			else
			{
				shut(errno,-1,f2,fd_patch);
				errx(27,"The offset in patch.bin does not corespond with the original value in %s",file1);
			}

		}
		if(read_patch<0)
		{
			shut(errno,-1,f2,fd_patch);
			err(28,"Could not read %s",patch);
		}
	}
	if(header_t.data==0x01)
	{
		if(f1_size%sizeof(uint16_t)!=0)
		{
			shut(errno,-1,f2,fd_patch);
			errx(29,"%s has wrong format",file1);
		}
		ssize_t read_patch;
		while((read_patch=read(fd_patch,&data_01_t,sizeof(data_01_t)))>0)
		{
			if(read_patch!=sizeof(data_01_t))
			{
				shut(errno,-1,f2,fd_patch);
				errx(30,"Error while reading in %s",patch);
			}
			uint32_t position=data_01_t.offset;
			uint16_t original=data_01_t.origin;
			off_t check=lseek(f2,position*sizeof(uint16_t),SEEK_SET);
			if(check<0)
			{
				shut(errno,-1,f2,fd_patch);
				err(31,"Error in positioning in %s",file2);
			}
			uint16_t current;
			ssize_t read_f2=read(f2,&current,sizeof(current));
			if(read_f2<0)
			{
				shut(errno,-1,f2,fd_patch);
				err(32,"Could not read in %s",file2);
			}
			if(read_f2!=sizeof(current))
			{
				shut(errno,-1,f2,fd_patch);
				errx(33,"Error while reading in %s",file2);
			}
			if(current==original)
			{
		  		uint16_t new_t=data_00_t.new;
				check=lseek(f2,position*sizeof(uint16_t),SEEK_SET);
				if(check<0)
				{
					shut(errno,-1,f2,fd_patch);
					err(34,"Error in positioning in %s",file2);
				}
				ssize_t over_write=write(f2,&new_t,sizeof(new_t));
				if(over_write<0)
				{
					shut(errno,-1,f2,fd_patch);
					err(35,"Could not write in %s",file2);
				}
				if(over_write!=sizeof(new_t))
				{
					shut(errno,-1,f2,fd_patch);
					errx(36,"Error while writing %s",file2);
				}
			}
			else
			{
				shut(errno,-1,f2,fd_patch);
				errx(37,"The offset in patch.bin does not corespond with the original value in %s",file1);
			}

		}
		if(read_patch<0)
		{
			shut(errno,-1,f2,fd_patch);
			err(38,"Could not read %s",patch);
		}
	}
	close(f2);
	close(fd_patch);

	exit(0);
}

