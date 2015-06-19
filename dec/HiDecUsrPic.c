#include "HiDecUsrPic.h"

static int g_TotalChn;
void GetCurTotalChn(int TotalChn)
{
    g_TotalChn = TotalChn;
}
/******************************************************************************
 * function : read frame
 ******************************************************************************/
static HI_VOID hi_vpss_ReadFrame(FILE * fp, HI_U8 * pY, HI_U8 * pU, HI_U8 * pV, HI_U32 width, HI_U32 height, HI_U32 stride, HI_U32 stride2)
{
 	HI_U8 * pDst;
	HI_U32 u32Row;
	pDst = pY;
	for ( u32Row = 0; u32Row < height; u32Row++ )
	{
		fread( pDst, width, 1, fp );
		pDst += stride;
	}
	pDst = pU;
	for ( u32Row = 0; u32Row < height/2; u32Row++ )
	{
		fread( pDst, width/2, 1, fp );
		pDst += stride2;
	}

	pDst = pV;
	for ( u32Row = 0; u32Row < height/2; u32Row++ )
	{
		fread( pDst, width/2, 1, fp );
		pDst += stride2;
	}
}
/******************************************************************************
 * function : Plan to Semi
 ******************************************************************************/
static HI_S32 hi_vpss_PlanToSemi(HI_U8 *pY, HI_S32 yStride,
		HI_U8 *pU, HI_S32 uStride,
		HI_U8 *pV, HI_S32 vStride,
		HI_S32 picWidth, HI_S32 picHeight)
{
 	HI_S32 i;
	HI_U8* pTmpU, *ptu;
	HI_U8* pTmpV, *ptv;
	HI_S32 s32HafW = uStride >>1 ;
	HI_S32 s32HafH = picHeight >>1 ;
	HI_S32 s32Size = s32HafW*s32HafH;

	pTmpU = malloc( s32Size ); ptu = pTmpU;
	pTmpV = malloc( s32Size ); ptv = pTmpV;

	memcpy(pTmpU,pU,s32Size);
	memcpy(pTmpV,pV,s32Size);

	for(i = 0;i<s32Size>>1;i++)
	{
		*pU++ = *pTmpV++;
		*pU++ = *pTmpU++;

	}
	for(i = 0;i<s32Size>>1;i++)
	{
		*pV++ = *pTmpV++;
		*pV++ = *pTmpU++;
	}

	free( ptu );
	free( ptv );

	return HI_SUCCESS;
}
/******************************************************************************
 * function : Get from YUV
 ******************************************************************************/
static HI_S32 hi_vpss_GetFrameFromYUV(FILE *pYUVFile, SIZE_S *stSize, VIDEO_FRAME_INFO_S *pstVFrameInfo)
{
 	HI_U32 u32LStride;
	HI_U32 u32CStride;
	HI_U32 u32LumaSize;
	HI_U32 u32ChrmSize;
	HI_U32 u32Size;
	VB_BLK VbBlk;
	HI_U32 u32PhyAddr;
	HI_U8 *pVirAddr;
	HI_U32 u32BlkCnt = 1;

	u32LStride  = stSize->u32Width;
	u32CStride  = stSize->u32Width;

	u32LumaSize = (u32LStride * stSize->u32Height);
	u32ChrmSize = (u32CStride * stSize->u32Height) >> 2;/* YUV 420 */
	u32Size = u32LumaSize + (u32ChrmSize << 1);

	/* create a video buffer pool*/ 
	VB_POOL VbPool;
	VbPool = HI_MPI_VB_CreatePool(u32Size, u32BlkCnt, NULL); 
	if ( VB_INVALID_POOLID == VbPool ) 
	{ 
		Printf("create vb err\n"); 
		return HI_FAILURE; 
	} 

	/* alloc video buffer block ---------------------------------------------------------- */
	VbBlk = HI_MPI_VB_GetBlock(VbPool, u32Size, NULL);
	if (VB_INVALID_HANDLE == VbBlk)
	{
		Printf("HI_MPI_VB_GetBlock err! size:%d\n",u32Size);
		return HI_FAILURE;
	}
	/* get video buffer PhyAddr*/
	u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
	if (0 == u32PhyAddr)
	{
		Printf("HI_MPI_VB_Handle2PhysAddr err! phyAddr is null\n");
		return HI_FAILURE;
	}
	/* map video buffer VirAddr*/
	pVirAddr = (HI_U8 *) HI_MPI_SYS_Mmap(u32PhyAddr, u32Size);
	if (NULL == pVirAddr)
	{
		Printf("HI_MPI_SYS_Mmap err! virAddr is null\n");
		return HI_FAILURE;
	}

	pstVFrameInfo->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
	if (VB_INVALID_POOLID == pstVFrameInfo->u32PoolId)
	{
		Printf("HI_MPI_VB_Handle2PoolId err!\n");
		return HI_FAILURE;
	}
	pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
	pstVFrameInfo->stVFrame.u32PhyAddr[1] = pstVFrameInfo->stVFrame.u32PhyAddr[0] + u32LumaSize;
	pstVFrameInfo->stVFrame.u32PhyAddr[2] = pstVFrameInfo->stVFrame.u32PhyAddr[1] + u32ChrmSize;

	pstVFrameInfo->stVFrame.pVirAddr[0] = pVirAddr;
	pstVFrameInfo->stVFrame.pVirAddr[1] = pstVFrameInfo->stVFrame.pVirAddr[0] + u32LumaSize;
	pstVFrameInfo->stVFrame.pVirAddr[2] = pstVFrameInfo->stVFrame.pVirAddr[1] + u32ChrmSize;

	pstVFrameInfo->stVFrame.u32Width  = stSize->u32Width;
	pstVFrameInfo->stVFrame.u32Height = stSize->u32Height;
	pstVFrameInfo->stVFrame.u32Stride[0] = u32LStride;
	pstVFrameInfo->stVFrame.u32Stride[1] = u32CStride;
	pstVFrameInfo->stVFrame.u32Stride[2] = u32CStride;
	pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
	pstVFrameInfo->stVFrame.u32Field = VIDEO_FIELD_FRAME;/* Intelaced D1,otherwise VIDEO_FIELD_FRAME */
	pstVFrameInfo->stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
	pstVFrameInfo->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	pstVFrameInfo->stVFrame.u64pts = 0;

	/* read Y U V data from file to the addr ----------------------------------------------*/
	hi_vpss_ReadFrame(pYUVFile, pstVFrameInfo->stVFrame.pVirAddr[0],
			pstVFrameInfo->stVFrame.pVirAddr[1], pstVFrameInfo->stVFrame.pVirAddr[2],
			pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height,
			pstVFrameInfo->stVFrame.u32Stride[0], pstVFrameInfo->stVFrame.u32Stride[1] >> 1 );

	/* convert planar YUV420 to sem-planar YUV420 -----------------------------------------*/
	hi_vpss_PlanToSemi(pstVFrameInfo->stVFrame.pVirAddr[0], pstVFrameInfo->stVFrame.u32Stride[0],
			pstVFrameInfo->stVFrame.pVirAddr[1], pstVFrameInfo->stVFrame.u32Stride[1],
			pstVFrameInfo->stVFrame.pVirAddr[2], pstVFrameInfo->stVFrame.u32Stride[1],
			pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height);

	HI_MPI_SYS_Munmap(pVirAddr, u32Size);
	return HI_SUCCESS;
    
}
/***********************************
 *获取用户图片
 **********************************/
static HI_S32 HiDec_GetUsrPic(HI_CHAR *pszYuvFile, VIDEO_FRAME_INFO_S *stFrame)
{
  	FILE *pfd;
	HI_S32 s32Ret;
	SIZE_S stSize;

	  // stSize.u32Width = 704;
	  // stSize.u32Height = 396;
	stSize.u32Width = 960;
	stSize.u32Height = 576;

	/* open YUV file */
	pfd = fopen(pszYuvFile, "rb");
	if (!pfd)
	{
		Printf("open file -> %s fail \n", pszYuvFile);
		return HI_FAILURE;
	}

	/* read YUV file. WARNING: we only support planar 420) */
	s32Ret = hi_vpss_GetFrameFromYUV(pfd, &stSize, stFrame);
	if(s32Ret != HI_SUCCESS)
	{
		Printf("hi_vpss_GetFrameFromYUV error\n");
		return HI_FAILURE;
	}
	fclose(pfd);
	return HI_SUCCESS;
}

/***********************************
 *功能：发送用户图片
 **********************************/
static void HiDec_SendUsrPic(VIDEO_FRAME_INFO_S *stFrame,VDEC_CHN_STAT_S stStatStart[MAX_DISPLAY_NUM], VDEC_CHN_STAT_S stStatEnd[MAX_DISPLAY_NUM])
{
    int i, ret;
    for(i = 0; i < g_TotalChn; i++)
    {
        ret = HI_MPI_VDEC_Query(i, &stStatEnd[i]);
        if(ret == HI_SUCCESS)
        {
            if(stStatEnd[i].u32DecodeStreamFrames - stStatStart[i].u32DecodeStreamFrames != 0)
            {
                stStatStart[i].u32DecodeStreamFrames = stStatEnd[i].u32DecodeStreamFrames;
            }
            else// 通道中不存留待解的帧，也就是说，这个通道不再收到码流！
            {
                ret = HI_MPI_VPSS_SendFrame(i, stFrame, 0);
                if(ret != HI_SUCCESS)
                {
                    Printf("HI_MPI_VPSS_SendFrame Chn %d error 0x%x\n", i, ret);
                }
            }
        }
        else
        {
            // Printf("HI_MPI_VDEC_Query Chn%d Fail!\n",i);    
        }
    }
}
// extern pthread_mutex_t WriteCfgFileMutex;
/*用户图片*/
void *Hi_DecUsrPicThread(void *arg)
{
    VIDEO_FRAME_INFO_S stFrame;
	struct  timeval tv_start;
	struct  timeval tv_end;
	VDEC_CHN_STAT_S stStatStart[MAX_DISPLAY_NUM];
	VDEC_CHN_STAT_S stStatEnd[MAX_DISPLAY_NUM];
    memset(stStatStart, 0, MAX_DISPLAY_NUM * sizeof(VDEC_CHN_STAT_S));
    memset(stStatEnd, 0, MAX_DISPLAY_NUM * sizeof(VDEC_CHN_STAT_S));
    HiDec_GetUsrPic(UserPicName, &stFrame);
    usleep(150 * 1000);
    HiDec_SendUsrPic(&stFrame, stStatStart,stStatEnd);
	gettimeofday(&tv_start, NULL);
	while(1)
	{
		gettimeofday(&tv_end, NULL);
		if(tv_end.tv_sec - tv_start.tv_sec >= 3)
		{
			{
                HiDec_SendUsrPic(&stFrame, stStatStart, stStatEnd);
			}
			tv_start.tv_sec = tv_end.tv_sec;
		}
		usleep(1000*1000);
	}
}
