#ifndef __HI_DEC_USRPIC_H__
#define __HI_DEC_USRPIC_H__

#ifdef __cplusplus
extern "C" {
#endif 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <pthread.h>
#include "common.h"
#include "sample_comm.h"
#include "global.h"
#include "hi_type.h"
    
#define UserPicName                 "UserPic_960x576.yuv" 
 // #define UserPicName                 "pic_704_396_p420_novideo01.yuv" 

void *Hi_DecUsrPicThread(void *arg);
void GetCurTotalChn(int TotalChn);

#ifdef __cplusplus
}
#endif
#endif

