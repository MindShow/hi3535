#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "watchdog.h"


int main()
{
	int ret=0;
	int fd =0;

	ret=openWtdog(&fd);
	if(ret<0)
	{
		printf("open fial\n");
	}
	else
	{
		printf("fd=%d\n",fd);
	}

	while(1)
	{
		ret=feedDog(fd);
		if(ret<0)
		{
			printf("feedDog  fail\n");
			closeWtdog( fd);
			break;
		}
		else
		{
			//printf("feedDog success  \n");
		}

		usleep(500 * 1000);
	}

	closeWtdog( fd);

	return 0;
}

