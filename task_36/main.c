#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

void func(int index,int num,char* files[],bool isN)
{
	int count=1;
	for(int i=index;i<num;i++)
	{
		const char* file=files[i];
		const char* f1="-";
		if(strcmp(file,f1)==0)
		{
			char c;
			ssize_t read_size;
			bool isBegin=true;
			while((read_size=read(0,&c,sizeof(c)))>0)
			{
				if(read_size!=sizeof(c))
				{
					warnx("Error while reading from STDIN");
					continue;
				}
				ssize_t write_size;
				if(isBegin&&isN)
				{
					char bar[sizeof(count)];
					sprintf(bar,"%d",count);
					strcat(bar," ");
					write_size=write(1,bar,sizeof(bar));
					if(write_size<0)
					{
						warn("Could not write in STDOUT");
						continue;
					}
					if(write_size!=sizeof(bar))
					{
						warnx("Error while writing in STOUT");
						continue;
					}
					isBegin=false;
					count++;
				}
				write_size=write(1,&c,sizeof(c));
				if(write_size<0)
				{
					warn("Could not write in STDOUT");
					continue;
				}
				if(write_size!=sizeof(c))
				{
					warnx("Error while writing in STDOUT");
					continue;
				}
				if(c=='\n')
				{
					isBegin=true;
				}
			}
			if(read_size<0)
			{
				warnx("Error while reading from STDIN");
				continue;
			}
			continue;
		}
		else
		{
			char c;
			ssize_t read_size;
			bool isBegin=true;
			int fd=open(file, O_RDONLY);
			if(fd<0)
			{
				warn("Could not open %s",file);
				continue;
			}
			while((read_size=read(fd,&c,sizeof(c)))>0)
			{
				if(read_size!=sizeof(c))
				{
					int olderrno=errno;
					close(fd);
					errno=olderrno;
					warnx("Error while reading from STDIN");
					continue;
				}
				ssize_t write_size;
				if(isBegin&&isN)
				{
					char bar[sizeof(count)];
					sprintf(bar,"%d",count);
					strcat(bar," ");
					write_size=write(1,bar,sizeof(bar));
					if(write_size<0)
					{
						int olderrno=errno;
						close(fd);
						errno=olderrno;
						warn("Could not write in STDOUT");
						continue;
					}
					if(write_size!=sizeof(count))
					{
						int olderrno=errno;
						close(fd);
						errno=olderrno;
						warnx("Error while writing in STOUT");
						continue;
					}
					isBegin=false;
					count++;
				}
				write_size=write(1,&c,sizeof(c));
				if(write_size<0)
				{
					int olderrno=errno;
					close(fd);
					errno=olderrno;
					warn("Could not write in STDOUT");
					continue;
				}
				if(write_size!=sizeof(c))
				{
					int olderrno=errno;
					close(fd);
					errno=olderrno;
					warnx("Error while writing in STDOUT");
					continue;
				}
				if(c=='\n')
				{
					isBegin=true;
				}
			}
			if(read_size<0)
			{
				int olderrno=errno;
				close(fd);
				errno=olderrno;
				warnx("Error while reading from STDIN");
				continue;
			}
			close(fd);
			continue;
		}
		
	}
}

int main(int argc,char* argv[])
{
	if(argc==1)
	{
		errx(1,"Ivalid number of arguments: %s (-n) (<fileName> ... <fileName>)",argv[0]);
	}
	if(strcmp(argv[1],"-n")==0)
	{
		if(argc==2)
		{
			errx(2,"Ivalid number of arguments: %s (-n) (<fileName> ... <fileName>)",argv[0]);
		}
		func(2,argc,argv,true);
	}
	else
	{
		func(1,argc,argv,false);
	}
	exit(0);
}
