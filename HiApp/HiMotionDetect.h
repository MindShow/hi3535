#ifndef __HI_MOTION_DETECT_H__
#define __HI_MOTION_DETECT_H__ 

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
#include "ShareMem.h"

#define PIC_RESULT_LEN 1024*1024*2

typedef struct hiVDA_MD_PARAM_S
{
    HI_BOOL bThreadStart;
    VDA_CHN VdaChn;
}VDA_MD_PARAM_S;

int HisiMotionDetect(void);
void HiMdSetEn(int EnFlg);

#ifdef __cplusplus
}
#endif
#endif

