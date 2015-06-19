#ifndef __MOTION_DETECT_H__
#define __MOTION_DETECT_H___ 

#ifdef __cplusplus
extern "C" {
#endif 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "sample_comm.h"
#include "public.h"
#include "common.h"

typedef struct hiVDA_MD_PARAM_S
{
    HI_BOOL bThreadStart;
    VDA_CHN VdaChn;
}VDA_MD_PARAM_S;

int HisiMotionDetect(void);

#ifdef __cplusplus
}
#endif
#endif

