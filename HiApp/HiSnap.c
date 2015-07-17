// #include "hiLib.h"
// #include "commonLib.h"
// #include "configLib.h"
#include "HiSnap.h"

HI_S32 venc_save_stream(VENC_CHN VencChn, VENC_STREAM_S *pstStream, char *PathFile);
int get_stream_pthread(HI_S32 s32Cnt);

static pthread_t  SnapTh;
static pthread_t  MdThread;

static HI_S32 gs_s32SnapCnt[VENC_MAX_CHN_NUM] = {0};
static HI_S32 gs_s32Snap[VENC_MAX_CHN_NUM] = {0};

static VENC_GETSTREAM_PARA_S gs_stParaSnap;
static SNAP_GETSTAT_S gs_stParaMd;
static HI_S32 gs_s32GetStream  = 0;


/******************************************************************************
* function : get picture size(w*h), according Norm and enPicSize
******************************************************************************/
HI_S32 get_snap_pic_size(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S *pstSize)
{
    switch (enPicSize)
    {
        case PIC_QCIF:
            pstSize->u32Width = D1_WIDTH / 4;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?144:120;
            break;
        case PIC_CIF:
            pstSize->u32Width = D1_WIDTH / 2;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?288:240;
            break;
        case PIC_D1:
            pstSize->u32Width = D1_WIDTH;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;
        case PIC_960H:
            pstSize->u32Width = 960;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;			
        case PIC_2CIF:
            pstSize->u32Width = D1_WIDTH / 2;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;
        case PIC_QVGA:    /* 320 * 240 */
            pstSize->u32Width = 320;
            pstSize->u32Height = 240;
            break;
        case PIC_VGA:     /* 640 * 480 */
            pstSize->u32Width = 640;
            pstSize->u32Height = 480;
            break;
        case PIC_XGA:     /* 1024 * 768 */
            pstSize->u32Width = 1024;
            pstSize->u32Height = 768;
            break;
        case PIC_SXGA:    /* 1400 * 1050 */
            pstSize->u32Width = 1400;
            pstSize->u32Height = 1050;
            break;
        case PIC_UXGA:    /* 1600 * 1200 */
            pstSize->u32Width = 1600;
            pstSize->u32Height = 1200;
            break;
        case PIC_QXGA:    /* 2048 * 1536 */
            pstSize->u32Width = 2048;
            pstSize->u32Height = 1536;
            break;
        case PIC_WVGA:    /* 854 * 480 */
            pstSize->u32Width = 854;
            pstSize->u32Height = 480;
            break;
        case PIC_WSXGA:   /* 1680 * 1050 */
            pstSize->u32Width = 1680;
            pstSize->u32Height = 1050;
            break;
        case PIC_WUXGA:   /* 1920 * 1200 */
            pstSize->u32Width = 1920;
            pstSize->u32Height = 1200;
            break;
        case PIC_WQXGA:   /* 2560 * 1600 */
            pstSize->u32Width = 2560;
            pstSize->u32Height = 1600;
            break;
        case PIC_HD720:   /* 1280 * 720 */
            pstSize->u32Width = 1280;
            pstSize->u32Height = 720;
            break;
        case PIC_HD1080:  /* 1920 * 1080 */
            pstSize->u32Width = 1920;
            pstSize->u32Height = 1080;
            break;
        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : bind venc and vpss. 
******************************************************************************/
HI_S32 venc_bind_vpss(VENC_CHN VencChn, SIZE_S *stSize, HI_BOOL Bind)
{
	HI_S32 s32Ret;
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;
	VPSS_CHN_MODE_S stVpssChnMode;
	// VENC_GRP VencGrp = VencChn;
	VPSS_GRP VpssGrp = VencChn;
	VPSS_CHN VpssChn = 1;

	stSrcChn.enModId = HI_ID_VPSS;
	stSrcChn.s32DevId = VpssGrp;
	stSrcChn.s32ChnId = VpssChn;
	stDestChn.enModId = HI_ID_VENC;
	stDestChn.s32DevId = 0;
	stDestChn.s32ChnId = VencChn;

	if(Bind == HI_TRUE)
	{
		s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
		if (s32Ret != HI_SUCCESS)
		{
			Printf("failed with %#x!\n", s32Ret);
			return HI_FAILURE;
		}
		
		stVpssChnMode.bDouble		 = HI_FALSE;
		stVpssChnMode.enChnMode 	 = VPSS_CHN_MODE_USER;
		stVpssChnMode.enPixelFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		stVpssChnMode.u32Width		 = stSize->u32Width;
		stVpssChnMode.u32Height 	 = stSize->u32Height;
		stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
		s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssChnMode);
		if (HI_SUCCESS != s32Ret)
		{
			Printf("set vpss grp%d chn%d mode fail, s32Ret: 0x%x.\n", VpssGrp, VpssChn, s32Ret);
			return s32Ret;
		}
	}
	else
	{
		s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
		if (s32Ret != HI_SUCCESS)
		{
			Printf("failed with %#x!\n", s32Ret);
			return HI_FAILURE;
		}
		
		stVpssChnMode.bDouble		 = HI_FALSE;
		stVpssChnMode.enChnMode 	 = VPSS_CHN_MODE_AUTO;
		stVpssChnMode.enPixelFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		stVpssChnMode.u32Width		 = stSize->u32Width;
		stVpssChnMode.u32Height 	 = stSize->u32Height;
		stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;
		s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssChnMode);
		if (HI_SUCCESS != s32Ret)
		{
			Printf("set vpss grp%d chn%d mode fail, s32Ret: 0x%x.\n", VpssGrp, VpssChn, s32Ret);
			return s32Ret;
		}
	}

	return HI_SUCCESS;
}


/******************************************************************************
* funciton : Create venc and Start recv pic . 
******************************************************************************/
HI_S32 venc_snap_start(VENC_CHN VencChn)
{
	HI_S32 s32Ret;
	VENC_CHN_ATTR_S stVencChnAttr;
	VENC_ATTR_JPEG_S stJpegAttr;
	// VPSS_GRP VpssGrp = VencChn;
	SIZE_S picSize;
	VIDEO_NORM_E enNorm = VIDEO_ENCODING_MODE_PAL;
	PIC_SIZE_E enPicSize = PIC_CIF;
	
	s32Ret = get_snap_pic_size(enNorm, enPicSize, &picSize);
	if(s32Ret != HI_SUCCESS)
	{
		Printf("get_snap_pic_size err \n");
	    return HI_FAILURE;
	}
	
	stVencChnAttr.stVeAttr.enType = PT_JPEG;
	stJpegAttr.u32MaxPicWidth     = picSize.u32Width;
	stJpegAttr.u32MaxPicHeight	  = picSize.u32Height;
	stJpegAttr.u32PicWidth 	   	  = picSize.u32Width;
	stJpegAttr.u32PicHeight	      = picSize.u32Height;
	stJpegAttr.u32BufSize 	      = picSize.u32Width * picSize.u32Height * 2;
	stJpegAttr.bByFrame 	      = HI_TRUE;
	
	memcpy(&stVencChnAttr.stVeAttr.stAttrJpeg, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
	s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
	if (HI_SUCCESS != s32Ret)
	{
		Printf("HI_MPI_VENC_CreateChn [%d] faild with %#x!\n",VencChn, s32Ret);
		return HI_FAILURE;
	}
	
	/******************************************
     	step : venc bind vpss
     	******************************************/
	s32Ret = venc_bind_vpss(VencChn, &picSize, HI_TRUE);
	if (HI_SUCCESS != s32Ret)
	{
		Printf("venc_bind_vpss failed!\n");
		return HI_FAILURE;
	}
		
	return HI_SUCCESS;
}



/******************************************************************************
* funciton : Create venc and Start recv pic . 
******************************************************************************/
HI_S32 venc_snap_stop(VENC_CHN VencChn)
{
	HI_S32 s32Ret;
	// VPSS_GRP VpssGrp = VencChn;
	// MPP_CHN_S stSrcChn;
	// MPP_CHN_S stDestChn;
	SIZE_S picSize;
	VIDEO_NORM_E enNorm = VIDEO_ENCODING_MODE_PAL;
	PIC_SIZE_E enPicSize = PIC_CIF;
	
	s32Ret = get_snap_pic_size(enNorm, enPicSize, &picSize);
	if(s32Ret != HI_SUCCESS)
	{
		Printf("get_snap_pic_size err \n");
	    return HI_FAILURE;
	}

	/******************************************
     	step : stop recv pic
     	******************************************/
	s32Ret = HI_MPI_VENC_StopRecvPic(VencChn);
	if (HI_SUCCESS != s32Ret)
	{
		Printf("HI_MPI_VENC_StopRecvPic [%d] faild with %#x!\n",VencChn, s32Ret);
		return HI_FAILURE;
	}
	
	/******************************************
     	step : venc unbind vpss
     	******************************************/
	s32Ret = venc_bind_vpss(VencChn, &picSize, HI_FALSE);
	if (HI_SUCCESS != s32Ret)
	{
		Printf("venc_bind_vpss failed!\n");
		return HI_FAILURE;
	}
	
	s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return HI_FAILURE;
    }
	
	return HI_SUCCESS;
}

void  *venc_get_stream(void *p)
{
	struct timeval TimeoutVal;
	fd_set read_fds;
	HI_S32 s32VencFd[VENC_MAX_CHN_NUM];
	int s32ChnTotal = 0;
	VENC_CHN_STAT_S stStat;
	VENC_STREAM_S stStream;
	// VENC_PACK_S *packet = NULL;
	HI_S32 s32Ret,i;
	VENC_CHN VencChn;
	int maxfd = 0;
	HI_CHAR snapPathFile[100] = "/tmp/";
	VENC_GETSTREAM_PARA_S *pstPara;
	
	pstPara = (VENC_GETSTREAM_PARA_S *)p;
    s32ChnTotal = pstPara->s32Cnt;
	if (s32ChnTotal > VENC_MAX_CHN_NUM)
    {
        Printf("input count invaild\n");
        return NULL ;
    }
	/* get Venc Fd. */
	for(i = 0; i < s32ChnTotal; i++)
	{
		s32VencFd[i] = HI_MPI_VENC_GetFd(i);
        if (s32VencFd[i] < 0)
        {
            Printf("HI_MPI_VENC_GetFd failed with %#x!\n", s32VencFd[i]);
            return NULL;
        }
        if (maxfd <= s32VencFd[i])
        {
            maxfd = s32VencFd[i];
        }
	}
	
	/* Start to get streams of each channel.*/
	while(HI_TRUE == pstPara->bThreadStart)
	{
		FD_ZERO(&read_fds);
		for (i = 0; i < s32ChnTotal; i++)
		{
		    FD_SET(s32VencFd[i], &read_fds);
		}
		
		/*select return success, all packages in the complete frame*/
		TimeoutVal.tv_sec  = 1;
		TimeoutVal.tv_usec = 0;
		s32Ret = select(maxfd+1, &read_fds, NULL, NULL, &TimeoutVal);
		if (s32Ret < 0) 
		{
			Printf("snap select failed!\n");
			return NULL;
		}
		else if (0 == s32Ret) 
		{
			continue;
		}
		else
		{
			for(i = 0; i < s32ChnTotal; i++)
			{
				VencChn = i;
				if (FD_ISSET(s32VencFd[VencChn], &read_fds))
				{
					/*query how many packs in one-frame stream.*/
					s32Ret = HI_MPI_VENC_Query(VencChn, &stStat);
					if (s32Ret != HI_SUCCESS)
					{
						Printf("HI_MPI_VENC_Query chn[%d] failed with %#x!\n", VencChn, s32Ret);
						return NULL;
					}
					/*malloc corresponding number of pack nodes*/
					stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
					if (NULL == stStream.pstPack)
					{
						Printf("malloc memory failed!\n");
						return NULL;
					}
					/* call mpi to get one-frame stream*/
					stStream.u32PackCount = stStat.u32CurPacks;
					s32Ret = HI_MPI_VENC_GetStream(VencChn, &stStream, HI_TRUE);
					if (HI_SUCCESS != s32Ret)
					{
						Printf("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
						free(stStream.pstPack);
						stStream.pstPack = NULL;
						return NULL;
					}
					/* save frame to file*/
					s32Ret = venc_save_stream(VencChn, &stStream, snapPathFile);
					if (HI_SUCCESS != s32Ret)
					{
						Printf("venc_save_stream failed with %#x!\n", s32Ret);
						free(stStream.pstPack);
						stStream.pstPack = NULL;
						return NULL;
					}
					/* release stream*/
					s32Ret = HI_MPI_VENC_ReleaseStream(VencChn, &stStream);
					if (s32Ret)
					{
						Printf("HI_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);
						free(stStream.pstPack);
						stStream.pstPack = NULL;
						return NULL;
					}
					/*free pack nodes*/
					free(stStream.pstPack);
					stStream.pstPack = NULL;
				}

			}
			
		}
	}
	return NULL;
}


void  *venc_start_snap_query(void *p)
{
	HI_S32 s32VencStartRecv[VENC_MAX_CHN_NUM] = {0};
	int s32ChnTotal = 0;
	HI_S32 s32Ret, i;
	static HI_S32 MdOnFlag = 0;
	SNAP_GETSTAT_S *pstPara;
	VENC_RECV_PIC_PARAM_S pstRecvParam;
	
	pstPara = (SNAP_GETSTAT_S*)p;
    s32ChnTotal = pstPara->s32Cnt;
	if (s32ChnTotal > VENC_MAX_CHN_NUM)
    {
        Printf("input count invaild\n");
        return NULL;
    }
	
	/* Start to get mdStat of each channel.*/
	while(HI_TRUE == pstPara->bThreadMd)
	{
        for (i = 0; i < s32ChnTotal; i++)
        {	
        	/*motion on with chn*/
        	// if((getActionAlarmStatus(i) == 1))
	        {
	        	if(gs_s32Snap[i] == 0)
	        	{
	        		/*Create venc chn*/
					venc_snap_start(i);
					/*start get stream thread*/
					if(!gs_s32GetStream)
					{
						get_stream_pthread(s32ChnTotal);
						gs_s32GetStream = 1;
						Printf("get stream thread running!\n");
					}
					gs_s32Snap[i] = 1;
					MdOnFlag++;
				}
				
				
				/*stop recv pic with chn, don't care  whether the last complete receiving all pictures*/
				if(1 == s32VencStartRecv[i])
				{
					s32Ret = HI_MPI_VENC_StopRecvPic(i);
					if (HI_SUCCESS != s32Ret)
					{
						Printf("HI_MPI_VENC_StopRecvPic faild with%#x!\n", s32Ret);
						return NULL;
					}
					//Printf("stop recv pic with chn = %d!\n", i);
					s32VencStartRecv[i] = 0;
				}
				/*motion status*/
				// if((VideoMDGetStatus(i) != 1))
				// {
					// // // continue;
				// }
				/*start a new recv  pic with chn .*/
				if(pstPara->bSnapEn[i] == HI_TRUE && s32VencStartRecv[i] == 0)
				{
					//Printf("MD !!! start recv pic with chn = %d!\n", i);
					pstRecvParam.s32RecvPicNum = 3;
					s32Ret = HI_MPI_VENC_StartRecvPicEx(i,&pstRecvParam);
					if (HI_SUCCESS != s32Ret)
					{
						Printf("HI_MPI_VENC_StartRecvPicEx faild with %#x!\n", s32Ret);
						return NULL;
					}
					s32VencStartRecv[i] = 1;
				}
				
	        }
			/*motion off with chn ,stop venc*/
			// if((getActionAlarmStatus(i) == 0))
			{
				if(gs_s32Snap[i] == 1)
	        	{
					venc_snap_stop(i);
					gs_s32Snap[i] = 0;
					MdOnFlag--;
				}
				/*wait all motion off  exit get stream thread*/
				if(MdOnFlag == 0)
				{
					if (HI_TRUE == gs_stParaSnap.bThreadStart && gs_s32GetStream == 1)
					{
						gs_stParaSnap.bThreadStart = HI_FALSE;
						pthread_join(SnapTh, 0);
						gs_s32GetStream = 0;
						Printf("get stream thread exit!\n");
					}
					
				}
			}
			
		}
		usleep(200*1000);/*wait until all pictures have been encoded*/
	}

	return NULL;
    
}


int start_snap_pthread(HI_S32 s32Cnt)
{
	int i;
	/*motion detect thread*/
	gs_stParaMd.bThreadMd = HI_TRUE; 
	gs_stParaMd.s32Cnt = s32Cnt;
	for(i = 0; i < s32Cnt; i++)
	{
		gs_stParaMd.bSnapEn[i] = HI_TRUE;
	}
	return pthread_create(&MdThread, 0, venc_start_snap_query, (HI_VOID*)&gs_stParaMd);
}


int get_stream_pthread(HI_S32 s32Cnt)
{
	gs_stParaSnap.bThreadStart = HI_TRUE;
	gs_stParaSnap.s32Cnt = s32Cnt;
	return pthread_create(&SnapTh, 0, venc_get_stream, (HI_VOID*)&gs_stParaSnap);
}


/******************************************************************************
* funciton : save mjpeg stream. 
* WARNING: in Hi3531, user needn't write SOI & EOI.
******************************************************************************/
HI_S32 venc_save_jpge(FILE* fpJpegFile, VENC_STREAM_S *pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;
	for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr+pstData->u32Offset, pstData->u32Len-pstData->u32Offset, 1, fpJpegFile);
        fflush(fpJpegFile);
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save snap stream & stream path
******************************************************************************/
HI_S32 venc_save_stream(VENC_CHN VencChn, VENC_STREAM_S *pstStream, char *PathFile)
{
    char acFile[50]  = {0};
	char snapPath[100] = {0};
    FILE *pFile;
    HI_S32 s32Ret;
    //sprintf(acFile, "/nfsroot/jpg/snap_%d_%d.jpg", VencChn, gs_s32SnapCnt[VencChn]);
    //pFile = fopen(acFile, "wb");
    strcpy(snapPath, PathFile);
    sprintf(acFile, "snap_%d.jpg", VencChn);
	strcat(snapPath, acFile);
    pFile = fopen(snapPath, "wb");
    if (pFile == NULL)
    {
        Printf("open file err\n");
        return HI_FAILURE;
    }
    s32Ret = venc_save_jpge(pFile, pstStream);
    if (HI_SUCCESS != s32Ret)
    {
        Printf("save snap picture failed!\n");
        return HI_FAILURE;
    }
	//Printf("snapPath = %s\n", snapPath);
    fclose(pFile);
    gs_s32SnapCnt[VencChn]++;
	if(gs_s32SnapCnt[VencChn] >= 1024)
	{
		gs_s32SnapCnt[VencChn] = 0;
	}
    return HI_SUCCESS;
}

int SnapThreadStart(void)
{
	int displayNum = 0;
	displayNum = GetCfgDispNum();// 注意，这里的displayNum是最大分屏数
	start_snap_pthread(displayNum);
    return HI_SUCCESS;
}

