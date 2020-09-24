#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[])
{
	if(argc!=3)
	{
		errx(1,"Invalid number of arguments: %s (--max,--min,--print) <fileName>",argv[0]);
	}
	const char * cmd=argv[1];
	if(!(strcmp(cmd,"--max")==0||strcmp(cmd,"--min")==0||strcmp(cmd,"--print")==0))
	{
		errx(2,"The second argument must be --max, --min or --print");
	}
	const char * file=argv[2];
	struct stat st;
	if(stat(file,&st)<0)
	{
		err(3,"Cannot stat %s",file);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"Cannot read %s",file);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(5,"File %s is not a regular file",file);
	}
	off_t  st_size=st.st_size;
	if(st_size%sizeof(uint16_t)!=0)
	{
		errx(6,"Error in %s format",file);
	}
	int fd=open(file, O_RDONLY);
	if(fd<0)
	{
		err(7,"Could not open %s",file);
	}
	uint16_t c;
	uint16_t min=65535;
	uint16_t max=0;
	ssize_t read_size;
	while((read_size=read(fd,&c,sizeof(c)))>0)
	{
		if(read_size!=sizeof(c))
		{
			const int olderrno=errno;
			close(fd);
			errno=olderrno;
			errx(9,"Error while reading %s",file);
		}
		if(c<min)
		{
			min=c;
		}
		if(c>max)
		{
			max=c;
		}
		if(strcmp(cmd,"--print")==0)
		{
			printf("%d\n",c);
		}
	}
	if(strcmp(cmd,"--min")==0)
	{
		printf("Min: %d\n",min);
	}
	if(strcmp(cmd,"--max")==0)
	{
		printf("Max: %d\n",max);
	}
	if(read_size<0)
	{
		const int olderrno=errno;
		close(fd);
		errno=olderrno;
		err(8,"Could not read %s",file);
	}
	close(fd);
	exit(0);
}
