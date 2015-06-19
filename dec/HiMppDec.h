#ifndef __HI_MPP_DEC_H__
#define __HI_MPP_DEC_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <memory.h>
#include "common.h"
#include "sample_comm.h"
#include "global.h"
#include "mylist.h"
#include "public.h"
#include "shm_buf.h"
// #include "dec.h"

// 解码通道的消息
typedef struct Decode_ChnInfo{
    char Url[128];
    int ChnNo;      // 通道号 
    int WinPos;     // 屏幕显示的位置
    int nWidth;     // 图像分辨率的高度值
    int nHeight;    //  图像分辨率的宽度值 
}Decode_ChnInfo;

//解码通道的消息
typedef struct decode_msg_t
{
    unsigned char ChnNo;//设备号
    unsigned char StreamNo;//通道号
    unsigned char WinPos;//显示位置
}decode_msg_t;

typedef struct decode_thread_channel_info_t
{//解码通道的信息      
    char Url[128];//运行状态 
    unsigned char RunStatus;//运行状态 
    unsigned char ChnNo;//设备号
    unsigned char StreamNo;//通道号
    unsigned char WinPos;//显示位置

    int nWidth;   // 分辨率高度值
    int nHeight;  // 分辨率宽度值
    int WaitIFrame;
    int FrameNo;
    int ReadBegin;       
    int ReadEnd;         
// 线程相关
    pthread_t pthID;
    VdecThreadParam stVdecSend;
// 链表
    struct kernel_list_head list;
}decode_thread_channel_info_t;


HI_VOID Hi_DecHandleSig(HI_S32 signo);
int Hi_DecMppInit(HI_VOID);
HI_S32 HiDec_VoInit(HI_VOID);
HI_S32 HiDec_VoStartAllChn(HI_S32 enMode);
HI_S32 HiDec_VoStartFullSreen(HI_S32 enMode, HI_S32 s32ChnNum);
int DecLink_Init(void);
void *Hi_DecUsrPicThread(void *arg);
HI_S32 HiDec_CreateSendThread(decode_thread_channel_info_t *pDecListChnInfo);
int HiDec_ReleseDecResource(int WinPos);
HI_S32 HiDec_Stop(HI_S32 s32ChnNum);
HI_S32 HiDec_GetPicSize(PIC_SIZE_E Resolution, SIZE_S *stSize);
HI_S32 HiDec_EnChn(int WinPos);
HI_S32 HiDec_StartDecode(HI_S32 s32ChnNum, SIZE_S stSize);
HI_S32 HiDec_QueryChnInfo(int Chn, VDEC_CHN_STAT_S *stChnStat);

#ifdef __cplusplus
}
#endif
#endif

