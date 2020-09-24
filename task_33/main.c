#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <string.h>

int compare(const void* a, const void *b)
{
	return *((uint32_t*)a)-*((uint32_t*)b);
}
int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		errx(1,"Invalid number of arguments: %s <fileName>",argv[0]);
	}
	const char* file=argv[1];
	struct stat st;
	if(stat(file,&st)<0)
	{
		err(2,"Can't stat %s",file);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"%s is not a regular file",file);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"Can not read in %s",file);
	}
	off_t st_size=st.st_size;
	if(st_size%sizeof(uint32_t)!=0)
	{
		errx(5,"%s has wrong format",file);
	}
	uint32_t numbers=st_size/sizeof(uint32_t);
	if(numbers > 100000000)
	{
		errx(6,"File %s can't have more than 100 000 000 numbers",file);
	}
	uint32_t numbers_half1=numbers/2;
	uint32_t* half1=malloc(numbers_half1*sizeof(uint32_t));
	if(!half1)
	{
		err(7,"No memory for the first half");
	}
	int fd=open(file,O_RDONLY);
	if(fd<0)
	{       const int olderrno=errno;
		free(half1);
		errno=olderrno;
		err(8,"Could not open %s",file);
	}
	size_t read_size1=read(fd,half1,numbers_half1*sizeof(uint32_t));
	/*if(read_size1<0)
	{
		const int olderrno=errno;
		close(fd);
		free(half1);
		errno=olderrno;
		err(9,"Could not read %s",file);
	}*/
	if(read_size1!=numbers_half1*sizeof(uint32_t))
	{
		const int olderrno=errno;
		close(fd);
		free(half1);
		errno=olderrno;
		errx(10,"Error while reading in %s",file);
	}
	qsort(half1,numbers_half1,sizeof(uint32_t),compare);
	int l=open("l",O_CREAT | O_TRUNC | O_RDWR , 0644);
	if(l<0)
	{
		const int olderrno=errno;
		close(fd);
		free(half1);
		errno=olderrno;
		err(11,"Could not open l");
	}
	size_t write_size1=write(l,half1,numbers_half1*sizeof(uint32_t));
	/*if(write_size1<0)
	{
		const int olderrno=errno;
		close(fd);
		unlink("l");
		free(half1);
		errno=olderrno;
		err(12,"Could not write l");
	}*/
	if(write_size1!=numbers_half1*sizeof(uint32_t))
	{
		const int olderrno=errno;
		close(fd);
		unlink("l");
		free(half1);
		errno=olderrno;
		errx(13,"Error while writing l");
	}
	free(half1);
	uint32_t numbers_half2=numbers-numbers_half1;
	uint32_t* half2=malloc(numbers_half2*sizeof(uint32_t));
	if(!half2)
	{
		const int olderrno=errno;
		close(fd);
		unlink("l");
		errno=olderrno;
		err(14,"No memory for second half");
	}
	int r=open("r",O_CREAT | O_TRUNC | O_RDWR , 0644);
	if(r<0)
	{
		const int olderrno=errno;
		close(fd);
		unlink("l");
		free(half2);
		errno=olderrno;
		err(15,"Can't open r");
	}
	read_size1=read(fd,half2,numbers_half2*sizeof(uint32_t));
	/*if(read_size1<0)
	{
		const int olderrno=errno;
		close(fd);
		unlink("l");
		unlink("r");
		free(half2);
		errno=olderrno;
		err(16,"Read second half from %s",file);
	}*/
	if(read_size1!=numbers_half1*sizeof(uint32_t))
	{
		const int olderrno=errno;
		close(fd);
		unlink("l");
		unlink("r");
		free(half2);
		errno=olderrno;
		errx(17,"Error while reading second half from %s",file);
	}
	qsort(half2,numbers_half2,sizeof(uint32_t),compare);
	write_size1=write(r,half2,numbers_half2*sizeof(uint32_t));
	/*if(write_size1<0)
	{
		const int olderrno=errno;
		close(fd);
		unlink("l");
		unlink("r");
		free(half2);
		errno=olderrno;
		err(18,"Can't write second half in r");
	}*/
	if(write_size1!=numbers_half2*sizeof(uint32_t))
	{
		const int olderrno=errno;
		close(fd);
		unlink("l");
		unlink("r");
		free(half2);
		errno=olderrno;
		errx(19,"Error while writing second half in r");
	}
	free(half2);
	close(fd);
	off_t check=lseek(l,0,SEEK_SET);
	if(check<0)
	{
		const int olderrno=errno;
		unlink("l");
		unlink("r");
		errno=olderrno;
		err(22,"Error in positioning in l");
	}
	check=lseek(r,0,SEEK_SET);
	if(check<0)
	{
		const int olderrno=errno;
		unlink("l");
		unlink("r");
		errno=olderrno;
		err(23,"Error in positioning in r");
	}
	int sorted=open("sorted", O_CREAT | O_TRUNC | O_RDWR, 0644);
	if(sorted<0)
	{
		const int olderrno=errno;
		unlink("l");
		unlink("r");
		errno=olderrno;
		errx(19,"Error while writing second half in r");
	}

	uint32_t l_num;
	uint32_t r_num;

	ssize_t l_read;
	ssize_t r_read;

	ssize_t l_write;
	ssize_t r_write;

	while(((l_read=read(l,&l_num,sizeof(l_num)))>0)&&((r_read=read(r,&r_num,sizeof(r_num)))>0))
	{
		if(l_read!=sizeof(l_num))	
		{
			const int olderrno=errno;
			unlink("l");
			close(sorted);
			unlink("r");
			errno=olderrno;
			errx(24,"Error while reading in l");
		}
		if(r_read!=sizeof(r_num))
		{
			const int olderrno=errno;
			unlink("l");
			close(sorted);
			unlink("r");
			errno=olderrno;
			errx(25,"Error while reading in r");
		}
		if(l_num<=r_num)
		{
			l_write=write(sorted,&l_num,sizeof(l_num));
			if(l_write<0)
			{
				const int olderrno=errno;
				unlink("l");
				close(sorted);
				unlink("r");
				errno=olderrno;
				err(26,"Coudl not write in sorted");
			}
			if(l_write!=sizeof(l_num))
			{
				const int olderrno=errno;
				unlink("l");
				close(sorted);
				unlink("r");
				errno=olderrno;
				errx(27,"Error while writing in sorted");
			}
			off_t check_r=lseek(r,-1*sizeof(r_num),SEEK_CUR);
			if(check_r<0)
			{
				const int olderrno=errno;
				unlink("l");
				close(sorted);
				unlink("r");
				errno=olderrno;
				err(28,"Error in positioning in r");
			}
		}
		else
		{
			r_write=write(sorted,&r_num,sizeof(r_num));
			if(r_write<0)
			{
				const int olderrno=errno;
				unlink("l");
				close(sorted);
				unlink("r");
				errno=olderrno;
				err(29,"Could not write in sorted");
			}
			if(r_write!=sizeof(r_num))
			{
				const int olderrno=errno;
				unlink("l");
				close(sorted);
				unlink("r");
				errno=olderrno;
				errx(30,"Error writing in sorted");
			}
			off_t check_l=lseek(l,-1*sizeof(r_num),SEEK_CUR);
			if(check_l<0)
			{
				const int olderrno=errno;
				unlink("l");
				close(sorted);
				unlink("r");
				errno=olderrno;
				err(31,"Error in positioning in l");
			}
		}

	}
	if(l_read<0)
	{
		const int olderrno=errno;
		unlink("l");
		unlink("r");
		close(sorted);
		errno=olderrno;
		err(20,"Could not read l");
	}
	if(r_read<0)
	{
		const int olderrno=errno;
		unlink("l");
		unlink("r");
		close(sorted);
		errno=olderrno;
		err(21,"Could not read r");
	}
	while((l_read=read(l,&l_num,sizeof(l_num)))>0)
	{
		if(l_read!=sizeof(l_num))	
		{
			const int olderrno=errno;
			unlink("l");
			close(sorted);
			unlink("r");
			errno=olderrno;
			errx(33,"Error while reading in l");
		}
		l_write=write(sorted,&l_num,sizeof(l_num));
		if(l_write<0)
		{
			const int olderrno=errno;
			unlink("l");
			close(sorted);
			unlink("r");
			errno=olderrno;
			err(34,"Coudl not write in sorted");
		}
		if(l_write!=sizeof(l_num))
		{
			const int olderrno=errno;
			unlink("l");
			close(sorted);
			unlink("r");
			errno=olderrno;
			errx(35,"Error while writing in sorted");
		}

	}
	if(l_read<0)
	{
		const int olderrno=errno;
		unlink("l");
		unlink("r");
		close(sorted);
		errno=olderrno;
		err(32,"Could not read l");
	}
	while((r_read=read(r,&r_num,sizeof(r_num)))>0)
	{

		if(r_read!=sizeof(r_num))
		{
			const int olderrno=errno;
			unlink("l");
			close(sorted);
			unlink("r");
			errno=olderrno;
			errx(37,"Error while reading in r");
		}
		r_write=write(sorted,&r_num,sizeof(r_num));
		if(r_write<0)
		{
			const int olderrno=errno;
			unlink("l");
			close(sorted);
			unlink("r");
			errno=olderrno;
			err(38,"Could not write in sorted");
		}
		if(r_write!=sizeof(r_num))
		{
			const int olderrno=errno;
			unlink("l");
			close(sorted);
			unlink("r");
			errno=olderrno;
			errx(39,"Error writing in sorted");
		}

	}
	if(r_read<0)
	{
		const int olderrno=errno;
		unlink("l");
		unlink("r");
		close(sorted);
		errno=olderrno;
		err(36,"Could not read r");
	}
	close(sorted);
	unlink("l");
	unlink("r");
	exit(0);
}
