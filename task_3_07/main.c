#include <unistd.h>
#include <stdlib.h>
#include <err.h>

int main ()
{
	if(execlp("sleep","sleep","60",(char*)NULL)==-1)
	{
		err(1,"Could not do sleep 60");
	}
	exit(0);
}

