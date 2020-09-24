#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

int main (int argc, char * argv[])
{
	if(argc!=2)
	{
		errx(1,"Invalid number of arguments: %s <fileName>",argv[0]);
	}
	const char* file=argv[1];
	struct stat st;
	if(stat(file,&st)==-1)
	{
		err(2,"Can't stat the file %s", file);
 	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"The %s file is not regular", file);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"The %s file is not readable", file);
	}	
	char c;
	ssize_t read_size=-1;
	uint8_t row=0;
	int fd_file=open(file, O_RDONLY);
	if(fd_file<0)
	{
		err(5,"Cannot open %s", file);
	}
	while((read_size=read(fd_file,&c,sizeof(c)))!=-1)
	{
		if(read_size!=sizeof(c))
		{
			int olderrno=errno;
			close(fd_file);
			errno=olderrno;
			errx(6,"Incorrect read of %s", file);
		}
		ssize_t write_size=write(1,&c,read_size);
		if(write_size==-1)
		{
			int olderrno=errno;
			close(fd_file);
			errno=olderrno;
			err(7,"Can't write from %s", file);
		}
		if(write_size!=read_size)
		{	
			int olderrno=errno;
			close(fd_file);
			errno=olderrno;
			errx(8,"Incorrect write from %s", file);
		}	
		if(c=='\n')
		{
			row++;
		}
		if(row==10)
		{
		   close(fd_file);
		   break;
		}
          
	}

	if(read_size==-1)
	{
		int olderrno=errno;
		close(fd_file);
		errno=olderrno;
		err(9,"Can't read %s",file);
	}
	exit(0);
}

