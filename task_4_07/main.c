#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
int main()
{

	int n=100000;
	pid_t a=fork();
	int status;
	if(a==0)
	{
		for(int i=0; i<n;i++)
		{
			printf("Child\n");
		}
	}
	else
	{
		wait(&status);
		for(int i=0;i<n;i++)
		{
			printf("Parent\n");
		}
	}
	exit(0);
}
