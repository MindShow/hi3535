#include "HiMppDec.h"


/*******************
 * 获取分辨率及其尺寸
 * *****************/
HI_S32 HiDec_GetPicSize(PIC_SIZE_E Resolution, SIZE_S *stSize)
{
    VIDEO_NORM_E EncodeMode = VIDEO_ENCODING_MODE_PAL;//编码制式
    HI_S32 s32Ret;
    //PicSizeArr[][0]:当前分屏数，PicSizeArr[][1]:分辨率类型
    // 获取分辨率的宽高
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(EncodeMode, Resolution, stSize);
    if (HI_SUCCESS !=s32Ret)
    {
        Printf("get picture size failed!\n"); 
        return HI_FAILURE;
    }
    if (stSize->u32Width == 704)
    {
        stSize->u32Width = 720;
    }
    if (stSize->u32Width == 352)
    {
        stSize->u32Width = 360;
    }
    if (stSize->u32Width == 176)
    {
        stSize->u32Width = 180;
    }
    return HI_SUCCESS;
}

/*******************
 * 初始化SYS和VB
 * *****************/
HI_S32 HiDec_InitSysAndVB(SIZE_S stSize)
{
    HI_S32 s32Ret;
    VB_CONF_S stVbConf;
	SAMPLE_COMM_VDEC_Sysconf(&stVbConf, &stSize);// 配置解码视频缓存池的属性
	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
#if 0
	if(s32Ret != HI_SUCCESS)
	{
	    SAMPLE_PRT("init sys fail for %#x!\n", s32Ret);
	    SAMPLE_COMM_SYS_Exit();	
        return HI_FAILURE;
	} 
      SAMPLE_COMM_SYS_MemConfig();
#endif
    return HI_SUCCESS;
}

/*******************
 * 初始化mod和common VB
 * *****************/
HI_S32 HiDec_InitCommVB(SIZE_S stSize)
{
    HI_S32 s32Ret;
    VB_CONF_S stModVbConf;
	SAMPLE_COMM_VDEC_ModCommPoolConf(&stModVbConf, PT_H264, &stSize);	
	s32Ret = SAMPLE_COMM_VDEC_InitModCommVb(&stModVbConf);
	if(s32Ret != HI_SUCCESS)
	{	    	
	    SAMPLE_PRT("init mod common vb fail for %#x!\n", s32Ret);
	    SAMPLE_COMM_SYS_Exit();	
        return HI_FAILURE;
	}
    return HI_SUCCESS;
}

/*******************
 * 开启Vdec
 * *****************/
HI_S32 HiDec_VdecStart(HI_S32 s32ChnNum, SIZE_S stSize)
{
    HI_S32 s32Ret;
	static VDEC_CHN_ATTR_S stVdecChnAttr[VDEC_MAX_CHN_NUM];
	SAMPLE_COMM_VDEC_ChnAttr(s32ChnNum, stVdecChnAttr, PT_H264,&stSize);
	s32Ret = SAMPLE_COMM_VDEC_Start(s32ChnNum, stVdecChnAttr);
	if(s32Ret != HI_SUCCESS)
	{	
	    SAMPLE_PRT("start VDEC fail for %#x!\n", s32Ret);
	    SAMPLE_COMM_VDEC_Stop(s32ChnNum);		
        return HI_FAILURE;
	}
    return HI_SUCCESS;
}

/*******************
 * 开启Vpss
 * *****************/
HI_S32 HiDec_VpssStart(HI_S32 s32ChnNum,SIZE_S stSize)
{
    HI_S32 s32Ret;
	SIZE_S stRotateSize;
	VPSS_GRP_ATTR_S stVpssGrpAttr[VDEC_MAX_CHN_NUM];
	stRotateSize.u32Width = stRotateSize.u32Height = MAX2(stSize.u32Width, stSize.u32Height);
    SAMPLE_COMM_VDEC_VpssGrpAttr(s32ChnNum, &stVpssGrpAttr[0], &stRotateSize);
	s32Ret = SAMPLE_COMM_VPSS_Start(s32ChnNum, &stRotateSize, 1, &stVpssGrpAttr[0]);
	if(s32Ret != HI_SUCCESS)
	{	    
	    SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
	    SAMPLE_COMM_VPSS_Stop(s32ChnNum, VPSS_BSTR_CHN);
        return HI_FAILURE;
	}
    return HI_SUCCESS;
}

/*******************
 * 初始化Vo模块
 * *****************/
HI_S32 HiDec_VoInit(HI_VOID)
{
    HI_S32 s32Ret;
	VO_DEV VoDev = 0;
	VO_PUB_ATTR_S stVoPubAttr;
    HI_S32 s32ChnNum = 0;
	VO_LAYER VoLayer = 0;
	VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
	SAMPLE_COMM_VDEC_VoAttr(s32ChnNum, VoDev ,&stVoPubAttr, &stVoLayerAttr);
	s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stVoPubAttr);
	if(s32Ret != HI_SUCCESS)
	{		
		SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
	    SAMPLE_COMM_VO_StopDev(VoDev);
        return HI_FAILURE;
	}
    
	s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stVoLayerAttr);
	if(s32Ret != HI_SUCCESS)
	{		
		SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
	    SAMPLE_COMM_VO_StopLayer(VoLayer);
        return HI_FAILURE;
	}	

    return HI_SUCCESS;
}

/**********************
 * 启动VO,启动所有的通道
 ********************/ 
HI_S32 HiDec_VoStartAllChn(HI_S32 enMode)
{
    HI_S32 s32Ret;
	VO_LAYER VoLayer = 0;
	s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, enMode);
	if(s32Ret != HI_SUCCESS)
	{		
		SAMPLE_PRT("vdec bind vpss fail!!!\n");
	    SAMPLE_COMM_VO_StopChn(VoLayer, enMode);	
        return HI_FAILURE;
	}
    return HI_SUCCESS; 
}

/**********************
 * 启动VO,将某个通道全屏显示
 ********************/ 
HI_S32 HiDec_VoStartFullSreen(HI_S32 enMode, HI_S32 s32ChnNum)
{
    HI_S32 s32Ret;
	VO_LAYER VoLayer = 0;
	s32Ret = SAMPLE_COMM_VO_StartFullSreen(VoLayer, enMode,s32ChnNum);
	if(s32Ret != HI_SUCCESS)
	{		
		SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
	    SAMPLE_COMM_VO_StopChn(VoLayer, enMode);	
        return HI_FAILURE;
	}
    return HI_SUCCESS;
    
}
/*******************
 * VDEC绑定VPSS
 * *****************/
HI_S32 HiDec_VdecBindVpss(HI_S32 s32ChnNum)
{
    HI_S32 s32Ret;
	s32Ret = SAMPLE_COMM_VDEC_BindVpss(s32ChnNum, s32ChnNum);
	if(s32Ret != HI_SUCCESS)
	{	    
		SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
        s32Ret = SAMPLE_COMM_VDEC_UnBindVpss(s32ChnNum, s32ChnNum);
        if(s32Ret != HI_SUCCESS)
        {	    
            SAMPLE_PRT("vdec unbind vpss fail for %#x!\n", s32Ret);
        }	
        return HI_FAILURE;
	}	
    return HI_SUCCESS;
}

/*******************
 * VDEC绑定VO
 * *****************/
HI_S32 HiDec_VpssBindVo(HI_S32 s32ChnNum,VO_LAYER VoLayer)
{
    HI_S32 s32Ret;
	s32Ret = SAMPLE_COMM_VO_BindVpss(VoLayer, s32ChnNum, s32ChnNum, VPSS_BSTR_CHN);
	if(s32Ret != HI_SUCCESS)
	{	    
		SAMPLE_PRT("vpss bind vo fail for %#x!\n", s32Ret);
        s32Ret = SAMPLE_COMM_VO_UnBindVpss(VoLayer, s32ChnNum, s32ChnNum, VPSS_BSTR_CHN);
        if(s32Ret != HI_SUCCESS)
        {	    
           SAMPLE_PRT("vpss unbind vo fail for %#x!\n", s32Ret);
        }	
        return HI_FAILURE;
	}	

    return HI_SUCCESS;
}

/******************
 * 查询通道信息
 * ***************/
HI_S32 HiDec_QueryChnInfo(int Chn, VDEC_CHN_STAT_S *stChnStat)
{
    return (HI_MPI_VDEC_Query(Chn, stChnStat));
}
/******************
 *使能通道
 * ***************/
HI_S32 HiDec_EnChn(int WinPos)
{
    HI_S32 s32Ret;
	VO_LAYER VoLayer = 0;
    s32Ret = HI_MPI_VO_EnableChn(VoLayer, WinPos);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;    
}

/*******************
 * 开始解码 
 * *****************/
HI_S32 HiDec_StartDecode(HI_S32 s32ChnNum, SIZE_S stSize)
{
 	HI_S32  s32Ret = HI_SUCCESS;
	VO_LAYER VoLayer = 0;
	/************************************************
	  step1:  start VDEC
    *************************************************/
    s32Ret = HiDec_VdecStart(s32ChnNum, stSize);
    if(s32Ret != HI_SUCCESS)
    {
        Printf("HiDec_VdecStart Failed!\n");    
        return HI_FAILURE;
    }    

	/************************************************
	  step2:  start VPSS
    *************************************************/
    s32Ret = HiDec_VpssStart(s32ChnNum, stSize);
    if(s32Ret != HI_SUCCESS)
    {
        Printf("HiDec_VpssStart Failed!\n");    
        return HI_FAILURE;
    }    

	/************************************************
	  step4:  VDEC bind VPSS
    *************************************************/	
    s32Ret = HiDec_VdecBindVpss(s32ChnNum);
    if(s32Ret != HI_SUCCESS)
    {
        Printf("HiDec_VdecBindVpss Failed!\n");    
        return HI_FAILURE;
    }    
		
	/************************************************
	  step5:  VPSS bind VO
    *************************************************/
    s32Ret = HiDec_VpssBindVo(s32ChnNum, VoLayer);
    if(s32Ret != HI_SUCCESS)
    {
        Printf("HiDec_VpssBindVo Failed!\n");    
        return HI_FAILURE;
    }    
    return s32Ret;
}

/*******************
 * 设置发送线程所需的参数 
 * *****************/
HI_S32 HiDec_SetSendPara(VdecThreadParam  *stVdecSend, HI_S32 ChnNo, int WinPos, SIZE_S stSize)
{
    stVdecSend->s32MilliSec     = 0;
    stVdecSend->s32ChnId        = WinPos;
    stVdecSend->s32IntervalTime = 1;
    stVdecSend->u64PtsInit      = 0;
    stVdecSend->u64PtsIncrease  = 0;
    stVdecSend->eCtrlSinal      = VDEC_CTRL_START;
    stVdecSend->bLoopSend       = HI_TRUE;
    stVdecSend->enType          = PT_H264;
    stVdecSend->s32MinBufSize   = stSize.u32Width * stSize.u32Height;
    if (PT_H264 == stVdecSend->enType  || PT_MP4VIDEO == stVdecSend->enType)
    {
            // stVdecSend->s32StreamMode  = VIDEO_MODE_FRAME; 
             stVdecSend->s32StreamMode  = VIDEO_MODE_STREAM; 
    }
    else
    {
        stVdecSend->s32StreamMode = VIDEO_MODE_FRAME;
    }
    return HI_SUCCESS;
}


/*******************
 * 发送H264吗流 
 * *****************/
void *HiDec_SendH264Stream(void *pArgs)
{
    VdecThreadParam *pstVdecThreadParam =(VdecThreadParam *)pArgs;
	FILE *fpStrm=NULL;
	unsigned char *pu8Buf = NULL;
	VDEC_STREAM_S stStream;
	HI_S32 s32Ret, start = 0;
    HI_S32 s32UsedBytes = 0, s32ReadLen = 0;
    HI_U64 u64pts = 0;
    
    fpStrm = myfopen(pstVdecThreadParam->s32ChnId);
    if(fpStrm == NULL)
    {
        printf("SAMPLE_TEST:can't open file %s in send stream thread:%d\n",pstVdecThreadParam->cFileName, pstVdecThreadParam->s32ChnId);
        return (HI_VOID *)(HI_FAILURE);
    }

	pu8Buf = malloc(pstVdecThreadParam->s32MinBufSize);
	if(pu8Buf == NULL)
	{
		printf("SAMPLE_TEST:can't alloc %d in send stream thread:%d\n", pstVdecThreadParam->s32MinBufSize, pstVdecThreadParam->s32ChnId);
		myfclose(fpStrm);
		return (HI_VOID *)(HI_FAILURE);
	}     
	fflush(stdout);
	
	u64pts = pstVdecThreadParam->u64PtsInit;
	while (1)
    {
        if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_STOP)
        {
            break;
        }
        else if (pstVdecThreadParam->eCtrlSinal == VDEC_CTRL_PAUSE)
        {
			sleep(MIN2(pstVdecThreadParam->s32IntervalTime,1000));
            continue;
        }

        if((pstVdecThreadParam->s32StreamMode==VIDEO_MODE_STREAM) && (pstVdecThreadParam->enType == PT_H264))
        {
            s32ReadLen = myfread((char *)pu8Buf, pstVdecThreadParam->s32MinBufSize, fpStrm);
            if (0==s32ReadLen) 
            {
                usleep(1000*20);
                continue;
            }
        }
        
        stStream.u64PTS  = u64pts;
		stStream.pu8Addr = pu8Buf + start;
		stStream.u32Len  = s32ReadLen; 
		stStream.bEndOfFrame  = (pstVdecThreadParam->s32StreamMode==VIDEO_MODE_FRAME)? HI_TRUE: HI_FALSE;
		stStream.bEndOfStream = HI_FALSE;       
        s32Ret=HI_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, pstVdecThreadParam->s32MilliSec);
        if (HI_SUCCESS != s32Ret)
        {
			usleep(100);
        }
        else
        {
            s32UsedBytes = s32UsedBytes +s32ReadLen + start;			
            u64pts += pstVdecThreadParam->u64PtsIncrease;            
        }
		usleep(20000);
	}

    /* send the flag of stream end */
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.bEndOfStream = HI_TRUE;
    HI_MPI_VDEC_SendStream(pstVdecThreadParam->s32ChnId, &stStream, -1);
	if (pu8Buf != HI_NULL)
	{
        free(pu8Buf);
	}
	myfclose(fpStrm);
	
	return (HI_VOID *)HI_SUCCESS;

    
    
}
/*******************
 * 创建码流发送线程
 * *****************/
HI_S32 HiDec_CreateSendThread(decode_thread_channel_info_t *pDecListChnInfo)
{
	SIZE_S stSize;
    HI_S32 s32Ret;
    int WinPos    = pDecListChnInfo->WinPos;
    int ChnNo      = pDecListChnInfo->ChnNo;

    stSize.u32Width = pDecListChnInfo->nWidth;
    stSize.u32Height = pDecListChnInfo->nHeight;
    s32Ret = HiDec_EnChn(WinPos);
    if(s32Ret != HI_SUCCESS)
    {
        Printf("HiDec_EnChn fail!\n");
        return HI_FAILURE;
    }
    HiDec_StartDecode(WinPos, stSize);
    if(s32Ret != HI_SUCCESS)
    {
        Printf("HiDec_StartDecode fail!\n");
        return HI_FAILURE;
    }
    HiDec_SetSendPara(&pDecListChnInfo->stVdecSend, ChnNo, WinPos, stSize);
    pthread_create(&pDecListChnInfo->pthID, 0, HiDec_SendH264Stream, (HI_VOID *)&pDecListChnInfo->stVdecSend);
    return HI_SUCCESS;
}

/*******************
 * Decode Stop
 * *****************/
HI_S32 HiDec_Stop(HI_S32 s32ChnNum)
{
    int i;
    HI_S32 s32Ret;
    VO_LAYER VoLayer = 0;
    VO_DEV VoDev = 0;
	for(i=0; i<s32ChnNum; i++)
	{
        s32Ret = HI_MPI_VDEC_DestroyChn(i);
		s32Ret = SAMPLE_COMM_VO_UnBindVpss(VoLayer, i, i, VPSS_BSTR_CHN);
		if(s32Ret != HI_SUCCESS)
		{	    
			SAMPLE_PRT("vpss unbind vo fail for %#x!\n", s32Ret);
            return HI_FAILURE;
		}	
		s32Ret = SAMPLE_COMM_VDEC_UnBindVpss(i, i);
		if(s32Ret != HI_SUCCESS)
		{	    
			SAMPLE_PRT("vdec unbind vpss fail for %#x!\n", s32Ret);
            return HI_FAILURE;
		}	
        SAMPLE_COMM_VO_StopChn(VoLayer, i);
	    SAMPLE_COMM_VPSS_Stop(i, VPSS_BSTR_CHN);
	    SAMPLE_COMM_VDEC_Stop(i);		
	}		
    SAMPLE_COMM_VO_StopLayer(VoLayer);
	SAMPLE_COMM_VO_StopDev(VoDev);
	SAMPLE_COMM_SYS_Exit();	
    return HI_SUCCESS;
    
}

/***************
 * 释放通道的资源
 * **************/
int HiDec_ReleseDecResource(int WinPos)
{
    HI_S32 s32Ret;
	VO_LAYER VoLayer = 0;

    s32Ret = SAMPLE_COMM_VO_UnBindVpss(VoLayer, WinPos, WinPos, VPSS_BSTR_CHN);
    if(s32Ret != HI_SUCCESS)
	{	    
        SAMPLE_PRT("vpss unbind vo fail for %#x!\n", s32Ret);
        return HI_FAILURE;
	}	

	s32Ret = SAMPLE_COMM_VDEC_UnBindVpss(WinPos, WinPos);
	if(s32Ret != HI_SUCCESS)
	{	    
		SAMPLE_PRT("vdec unbind vpss fail for %#x!\n", s32Ret);
        return HI_FAILURE;
	}	


	s32Ret = SAMPLE_COMM_VO_StopChn(VoLayer, WinPos);	
	if(s32Ret != HI_SUCCESS)
	{	    
		SAMPLE_PRT("Stop Chn fail for %#x!\n", s32Ret);
        return HI_FAILURE;
	}	

	s32Ret = SAMPLE_COMM_VPSS_Stop(WinPos, VPSS_BSTR_CHN);
	if(s32Ret != HI_SUCCESS)
	{	    
		SAMPLE_PRT("Stop Vpss fail for %#x!\n", s32Ret);
        return HI_FAILURE;
	}	

    s32Ret = SAMPLE_COMM_VDEC_Stop(WinPos);
	if(s32Ret != HI_SUCCESS)
	{	    
		SAMPLE_PRT("Stop Vdec fail for %#x!\n", s32Ret);
        return HI_FAILURE;
	}	

    return HI_SUCCESS;


}

/********************************
 * 异常信号处理，如Ctrl+c kill等。
 *******************************/
HI_VOID Hi_DecHandleSig(HI_S32 signo)
{
    if(SIGINT == signo || SIGTERM == signo)
    {   
         SAMPLE_COMM_SYS_Exit();// 释放3535资源 
    }   
    exit(0);
}
/*************************************
 * 海思解码模块的初始化
 ***************************************/
int Hi_DecMppInit(HI_VOID)
{
	HI_S32  s32Ret = HI_SUCCESS;
	SIZE_S stSize;

    /************************************************
       step1:  init SYS and common VB 
    *************************************************/
	s32Ret = HiDec_InitSysAndVB(stSize);
    if(s32Ret == HI_FAILURE)
    {
        Printf("HiDec_InitSysAndVB Failed!\n");    
        return HI_FAILURE;
    }    

	/************************************************
       step2:  init mod common VB
    *************************************************/
    s32Ret = HiDec_InitCommVB(stSize);
    if(s32Ret == HI_FAILURE)
    {
        Printf("HiDec_InitCommVB Failed!\n");    
        return HI_FAILURE;
    }    

	/************************************************
       step3:  init VO
    *************************************************/
    s32Ret = HiDec_VoInit();
    if(s32Ret == HI_FAILURE)
    {
        Printf("HiDec_InitVo Failed!\n");    
        return HI_FAILURE;
    }    
    return 	HI_SUCCESS;
}

