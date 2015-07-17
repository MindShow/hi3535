#ifndef __HI_FRAME_BUF_H__
#define __HI_FRAME_BUF_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "hifb.h"
#include "sample_comm.h"
#include "hi_tde_type.h"


int HisiFramBuffer(void);

#ifdef __cplusplus
}
#endif
#endif
