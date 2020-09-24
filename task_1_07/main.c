#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
int main (int argc, char* argv[])
{
	if(argc!=2)
	{
		errx(1,"Invalid number of arguments: %s <fileName>",argv[0]);
	}
	if((execlp("ls","ls","-l",argv[1],(char*)NULL))<0)
	{
		err(2,"Could not exec ls %s",argv[1]);
	}
	exit(0);
}
