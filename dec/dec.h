#ifndef __DEC_H__
#define __DEC_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <pthread.h>
#include "hi_comm_video.h"
#include "common.h"
#include "global.h"
#include "HiMppDec.h"
#include "HiDecUsrPic.h"
#include "CfgFileOpera.h"
#include "HiDecList.h"

// Macro
#define MAX_DECODE_THREAD_NUM       1//最大解码线程   
#define CUR_DISP_NUM                4// 相当于configGetDisplayNum() 

typedef struct decode_dev_channel_info_t 
{
    unsigned char isUsed;//是否在使用
    pthread_mutex_t mutex;    
    pthread_t threadId;//Id   
    int runFlag;//解码线程运行标志

    unsigned char channelNo;//设备号
    unsigned char streamNo;//通道号
    unsigned char viewPos;//显示位置
}decode_dev_channel_info_t;

//func
int Hi_DecStart(int NewDispMode);
int HisiDecodeInit(void);
void DecListOpera_AddChn(int Chn, int nWidth, int nHeight);
void DecListOpera_DelChn(int Chn);
void DecListOpera_GetResolution(int Chn, int *nWidth, int *nHeight);

#ifdef __cplusplus
}
#endif
#endif//__DEC_LIB__H__

