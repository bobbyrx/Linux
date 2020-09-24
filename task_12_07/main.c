#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>
#include <string.h>

int main (int argc,char* argv[])
{
	if(argc!=4)
	{
		errx(1,"Invalid number of arguments: %s <cmd1> <cmd2> <fileName>",argv[0]);
	}
	const char* file=argv[3];
	for(int i=1;i<argc-1;i++)
	{
		const char* cmd=argv[i];
       		int fd=open(file, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
		if(fd<0)
		{
			err(2,"Could not open %s",file);
		}
		pid_t p=fork();
		if(p<0)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			err(3,"Could not fork");
		}
		if(p==0)
		{
			if(execlp(cmd,cmd,(char*)NULL)==-1)
			{
				int olderrno=errno;
				close(fd);
				errno=olderrno;
				err(4,"Could not exec %s",cmd);
			}
		}

		int status;
		pid_t wait_status=wait(&status);
		if(wait_status<0)
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			err(5,"Could not wait for %s",cmd);
		}
		if(WIFEXITED(status))
		{
			if(WEXITSTATUS(status)==0)
			{
				ssize_t write_size=write(fd,cmd,strlen(cmd));
				if(write_size<0)
				{
					int olderrno=errno;
					close(fd);
					errno=olderrno;
					err(6,"Could not write in %s",file);
				}	
				if(write_size!=(ssize_t)strlen(cmd))
				{
					int olderrno=errno;
					close(fd);
					errno=olderrno;
					errx(7,"Error while writing in %s",file);
				}
				const char* newline="\n";
				ssize_t write_size1=write(fd,newline,strlen(newline));
				if(write_size1<0)
				{
					int olderrno=errno;
					close(fd);
					errno=olderrno;
					err(8,"Could not  n  write in %s",file);
				}	
				if(write_size1!=(ssize_t)strlen(newline))
				{
					int olderrno=errno;
					close(fd);
					errno=olderrno;
					errx(9,"Error while writing in %s",file);
				}
				close(fd);
				continue;
			}
			else
			{
				int olderrno=errno;
				close(fd);
				errno=olderrno;
				warnx("The child process's exit status is not 0");
				continue;
			}
		}
		else
		{
			int olderrno=errno;
			close(fd);
			errno=olderrno;
			warn("Could not exit process");
			continue;
		}
	}	
	exit(0);
}
