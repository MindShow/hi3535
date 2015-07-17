#ifndef __HI_SNAP_H__
#define __HI_SNAP_H__ 
#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "common.h"
#include "public.h"
#include "hi_comm_video.h"
#include "sample_comm.h"
#include "hi_comm_venc.h"
// #define VENC_MAX_CHN_NUM 64

int SnapThreadStart(void);
int set_snapFlag(unsigned char channel,unsigned char status);
HI_S32 get_snap_pic_size(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S *pstSize);

typedef enum snap_status_e
{
	SNAP_RESET=0,
	SNAP_RUN,
	SNAP_PAUSE,
	SNAP_EXIT	
}snap_status_e;

typedef struct venc_getstream_s
{
	HI_BOOL bThreadStart;
	HI_S32  s32Cnt;
}VENC_GETSTREAM_PARA_S;

typedef struct snap_getStat_s
{
	HI_BOOL bThreadMd;
	HI_S32  s32Cnt;
	HI_BOOL bSnapEn[VENC_MAX_CHN_NUM];
}SNAP_GETSTAT_S;

#ifdef __cplusplus
}
#endif
#endif
