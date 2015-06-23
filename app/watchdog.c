#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <pthread.h>

#include "watchdog.h"
#include "common.h"
// #include "commonLib.h"
// #include "global.h"
// #include "param.h"
// #include "myThreadLib.h"


#define  HI_WD_DEVICE   "/dev/watchdog"


// static Hi3531_wtd_info_t   watchdogInfo;
// static int g_run = 0;

/**/
int openWtdog(int *fd)
{
	*fd=open(HI_WD_DEVICE,O_RDWR|O_SYNC);
	if(*fd<=0)
	{
		Printf("open wtdog fail \n");
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
int GetWtdogSuportOption(int fd)
{
	watchdog_info wd_info;

	memset(&wd_info,0,sizeof(wd_info));
	if(ioctl(fd,WDIOC_GETSUPPORT,&wd_info) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		Printf("options=%d firmware_version= %d identity= %s\n",wd_info.options,wd_info.firmware_version,wd_info.identity);
		return 0;
	}
}


/**/
int GetWtdogStatus(int fd)
{
    int t = 0;
	if(ioctl(fd,WDIOC_GETSTATUS, &t) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		Printf("WDIOC_GETSTATUS t=%d\n", t);
		return 0;
	}
}

/**/
int  GetWtdogBootstatus(int fd)
{
	int t=0;
	if(ioctl(fd,WDIOC_GETBOOTSTATUS, &t) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		Printf("WDIOC_GETBOOTSTATUS t=%d\n", t);
		return 0;
	}
}

/**/
int feedDog(int fd)
{
	if(ioctl(fd,WDIOC_KEEPALIVE, NULL) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
    return 0;
}


/**/
int setWtdogTimeout(int fd,int t)
{
	int TimeOut=t;
    Printf("Feed wdt :%ds", TimeOut);
	if(ioctl(fd,WDIOC_SETTIMEOUT, &TimeOut) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
    return 0;
}


/**/
int setWtdogOption(int fd,int *t)
{
	//int t=2;
	if(ioctl(fd,WDIOC_SETOPTIONS,t) == -1)
	{
		Printf("Failed to ioctrl\n");
		return -1;
	}
	else
	{
		Printf("WDIOC_SETOPTIONS t=%d\n",*t);
		return 0;
	}
}



void *WatchdogThread()
{
	int ret=0;
	int fd = -1;

    // 开启看门狗设备
	ret=openWtdog(&fd);
	if(ret<0)
	{
		Printf("open fial\n");
        return NULL;
	}
#if 0
	ret=GetWtdogSuportOption(fd);
	if(ret<0)
	{
		Printf("GetWtdogSuportOption  fail \n");
	}

	ret=GetWtdogStatus(fd);
	if(ret<0)
	{
		Printf("GetWtdogStatus fail\n");
	}

	ret=GetWtdogBootstatus(fd);
	if(ret<0)
	{
		Printf("GetWtdogBootstatus  fail\n");
	}
	t=0;
	ret=feedDog( fd, &t);
	if(ret<0)
	{
		Printf("feedDog  fail\n");
	}
#endif
    // 设置超时时间为20s
	ret=setWtdogTimeout(fd, 20);
	if(ret<0)
	{
		Printf("setWtdogTimeout  fail\n");
        closeWtdog(fd);
        return NULL;
	}

	while(1)
	{
		ret=feedDog(fd);// 每隔4s喂一次狗，也就是说在20s的时间内，最多可以喂5次狗
		if(ret<0)
		{
			Printf("feedDog  fail\n");
			closeWtdog(fd);
			break;
		}
		usleep(4000 * 1000);
	}
	closeWtdog( fd);
	return NULL;
}



int  startWtdThread()
{
    pthread_t pWdtId;    
	if(pthread_create(&pWdtId, NULL, WatchdogThread, NULL) < 0)
	{
		Printf("WatchdogThread error\r\n");
		return -1;
	}
	return 0;
}

