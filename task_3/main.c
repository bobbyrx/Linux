#include <stdint.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

int main (int argc , char * argv[])
{
	if(argc!=2)
	{
		errx(1,"Invalid number of arguments: %s <fileName>",argv[0]);
	}
	const char* file=argv[1];
	struct stat st;
	if(stat(file, &st)<0)
	{
		err(2,"Can't stat %s", file);
	}
	if(!S_ISREG(st.st_mode))
	{
		errx(3,"File %s is not a regular file",file);
	}
	if(!(st.st_mode & S_IRUSR))
	{
		errx(4,"File %s is not readable",file);
	}	
	int fd=open(file, O_RDONLY);
	if(fd==-1)
	{
		err(5,"Can't open file %s",file);
	}
	char c;
	int rows=0;
	int words=0;
	int characters=0;
	ssize_t read_size=-1;

	while((read_size=read(fd, &c, sizeof(c)))>0)
	{
		if(read_size!=sizeof(c))
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			errx(6,"Incorrect read of %s",file);
		}
		if(c=='\n')
		{ 
			rows++;
			words++;
		}
		if(c==' ')
		{
			words++;
		}
		characters++;
	}
	if(read_size==-1)
	{
		int olderrno=errno;
                close(fd);
                errno=olderrno;
                errx(7,"Incorrect read of %s",file);
	}

	printf(" %d %d %d %s\n",rows, words, characters, file);
	close(fd);
	exit(0);
}

