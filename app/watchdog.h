#ifndef __WATCH_DOG_H__
#define __WATCH_DOG_H__ 

#ifdef __cplusplus
extern "C" {
#endif 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <pthread.h>
#include "common.h"
#include "Linux_Watchdog.h"

#define  HI_WD_DEVICE   "/dev/watchdog"

int openWtdog(int *fd);
int closeWtdog(int fd);
int feedDog(int fd);
#ifdef __cplusplus
}
#endif
#endif

