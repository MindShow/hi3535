#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "../include/Linux_Watchdog.h" 
#define  HI_WD_DEVICE   "/dev/watchdog"

/**/
int openWtdog(int *fd)
{
    printf("Watchdog Start!\n");
	*fd=open(HI_WD_DEVICE,O_RDWR|O_SYNC);
	if(*fd<=0)
	{
		printf("open wtdog fail \n");
		return -1;
	}
    return 0;
}

/**/
int closeWtdog(int fd)
{
	if(fd>0)
	{
		close(fd);
	}
	return 0;
}
/**/
int feedDog(int fd)
{
	if(ioctl(fd,WDIOC_KEEPALIVE, NULL) == -1)
	{
		printf("Failed to ioctrl\n");
		return -1;
	}
    return 0;
}
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

