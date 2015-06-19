#include "MotionDetect.h"

static pthread_t gs_VdaPid;
static VDA_MD_PARAM_S gs_stMdParam;

/****************************
 *创建MD通道
 *****************************/
int HiMD_CreateVdaChn(VDA_CHN VdaChn,SIZE_S stSize)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VDA_CHN_ATTR_S stVdaChnAttr;

    stVdaChnAttr.enWorkMode = VDA_WORK_MODE_MD; // 工作模式：MD/OD
    stVdaChnAttr.u32Width   = stSize.u32Width;  // 通道宽度
    stVdaChnAttr.u32Height  = stSize.u32Height; // 通道高度 

    stVdaChnAttr.unAttr.stMdAttr.enVdaAlg      = VDA_ALG_REF;//VDA 算法。帧差别法。
    stVdaChnAttr.unAttr.stMdAttr.enMbSize      = VDA_MB_16PIXEL; // 宏块大小,16 x 16 
    stVdaChnAttr.unAttr.stMdAttr.enMbSadBits   = VDA_MB_SAD_8BIT;// SAD 输出 bit 位:8bits/16bits
    stVdaChnAttr.unAttr.stMdAttr.enRefMode     = VDA_REF_MODE_DYNAMIC;// 参考帧模式:静态/动态参考帧模式。
    stVdaChnAttr.unAttr.stMdAttr.u32MdBufNum   = 8;// MD 结果缓存个数
    stVdaChnAttr.unAttr.stMdAttr.u32VdaIntvl   = 4;// VDA 间隔,VDA 支持最大性能： D1 为 15fps， CIF为 30fps。  
    stVdaChnAttr.unAttr.stMdAttr.u32BgUpSrcWgt = 64;// 背景更新权重
    stVdaChnAttr.unAttr.stMdAttr.u32SadTh      = 20;// 仅当 VDA 算法选择为背景法时，该项有效。
    stVdaChnAttr.unAttr.stMdAttr.u32ObjNumMax  = 64;// 运动区域输出最大个数,推荐128

    s32Ret = HI_MPI_VDA_CreateChn(VdaChn, &stVdaChnAttr);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err!\n");
        return s32Ret;
    }
    Printf("Create VDA Chn Ok!\n");
    return s32Ret;
}

/**********************************
 *Vda绑定通道
 ********************************/
int HiMD_BindViChn(VDA_CHN VdaChn,SIZE_S stSize)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn, stDestChn;
    VPSS_CHN_MODE_S stVpssChnMode; 
    VPSS_CHN VpssChn = 2; 
    VPSS_GRP VpssGrp = VdaChn;

    stSrcChn.enModId = HI_ID_VPSS;// 模块号
    stSrcChn.s32ChnId = VPSS_PRE0_CHN;// 通道号
    stSrcChn.s32DevId = 0;// 设备号

    stDestChn.enModId = HI_ID_VDA;
    stDestChn.s32ChnId = VdaChn;
    stDestChn.s32DevId = 0;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);// 数据源绑定到接收源
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err!\n");
        return s32Ret;
    }
#if 1 
    stVpssChnMode.bDouble        = HI_FALSE;// 保留
    stVpssChnMode.enChnMode      = VPSS_CHN_MODE_USER;// 用户模式,一个通道绑定多个输出源,这里的输入源指的是VPSS，输出源指的是vo和vda。
    stVpssChnMode.enPixelFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;// 目标图像像素格式
    stVpssChnMode.u32Width       = stSize.u32Width;//目标图像宽
    stVpssChnMode.u32Height      = stSize.u32Height;// 目标图像高
    stVpssChnMode.enCompressMode = COMPRESS_MODE_NONE;//目标图像压缩格式，3535仅支持COMPRESS_MODE_NONE一种。
    s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, &stVpssChnMode);
    if (HI_SUCCESS != s32Ret)
    {
            Printf("set vpss grp%d chn%d mode fail, s32Ret: 0x%x.\n", VpssGrp, VpssChn, s32Ret);
                return HI_FAILURE;
    }
#endif
    
    return 0;
}

/******************************************************************************
* funciton : vda MD mode print -- SAD
******************************************************************************/
HI_S32 SAMPLE_COMM_VDA_MdPrtSad(FILE *fp, VDA_DATA_S *pstVdaData)
{
    HI_S32 i, j;
    HI_VOID *pAddr;
	
    fprintf(fp, "===== %s =====\n", __FUNCTION__);
    if (HI_TRUE != pstVdaData->unData.stMdData.bMbSadValid)
    {
        fprintf(fp, "bMbSadValid = FALSE.\n");
        return HI_SUCCESS;
    }

    for(i=0; i<pstVdaData->u32MbHeight; i++)
    {
		pAddr = (HI_VOID *)((HI_U32)pstVdaData->unData.stMdData.stMbSadData.pAddr
		  			+ i * pstVdaData->unData.stMdData.stMbSadData.u32Stride);
	
		for(j=0; j<pstVdaData->u32MbWidth; j++)
		{
	        HI_U8  *pu8Addr;
	        HI_U16 *pu16Addr;
		
	        if(VDA_MB_SAD_8BIT == pstVdaData->unData.stMdData.stMbSadData.enMbSadBits)
	        {
	            pu8Addr = (HI_U8 *)pAddr + j;

	            fprintf(fp, "%-2d ",*pu8Addr);

	        }
	        else
	        {
	            pu16Addr = (HI_U16 *)pAddr + j;

				fprintf(fp, "%-4d ",*pu16Addr);
	        }
		}
		
        printf("\n");
    }
	
    fflush(fp);
    return HI_SUCCESS;
    
}
/**********************************
 *开始接收图片
 *********************************/
int HiMD_StartRevPic(VDA_CHN VdaChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_MPI_VDA_StartRecvPic(VdaChn);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err!\n");
        return s32Ret;
    }
    return s32Ret;
}

/**********************************
 *
 **********************************/
HI_VOID *HiMD_GetResultThread(HI_VOID *pdata)
{
    HI_S32 s32Ret;
    VDA_CHN VdaChn;
    VDA_DATA_S stVdaData;
    VDA_MD_PARAM_S *pgs_stMdParam;
    HI_S32 maxfd = 0;
    FILE *fp = stdout;
    HI_S32 VdaFd;
    fd_set read_fds;
    struct timeval TimeoutVal;

    pgs_stMdParam = (VDA_MD_PARAM_S *)pdata;

    VdaChn   = pgs_stMdParam->VdaChn;
    SAMPLE_PRT("VdaChn:%d\n", VdaChn);

    /* decide the stream file name, and open file to save stream */
    /* Set Venc Fd. */
    VdaFd = HI_MPI_VDA_GetFd(VdaChn);
    if (VdaFd < 0)
    {
        SAMPLE_PRT("HI_MPI_VDA_GetFd failed with %#x!\n",VdaFd);
        return NULL;
    }
    SAMPLE_PRT("HI_MPI_VDA_GetFd with %#x!\n",VdaFd);

    if (maxfd <= VdaFd)
    {
        maxfd = VdaFd;
    }
    while (HI_TRUE == pgs_stMdParam->bThreadStart)
    {
        FD_ZERO(&read_fds);
        FD_SET(VdaFd, &read_fds);

        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get vda result time out, exit thread\n");
            break;
        }
        else
        {
            if (FD_ISSET(VdaFd, &read_fds))
            {
                /*******************************************************
                   step 2.3 : call mpi to get one-frame stream
                   *******************************************************/
                s32Ret = HI_MPI_VDA_GetData(VdaChn, &stVdaData, -1);
                if(s32Ret != HI_SUCCESS)
                {
                    SAMPLE_PRT("HI_MPI_VDA_GetData failed with %#x!\n", s32Ret);
                    return NULL;
                }
                /*******************************************************
                   *step 2.4 : save frame to file
                *******************************************************/
                printf("\033[0;0H");/*move cursor*/
		        SAMPLE_COMM_VDA_MdPrtSad(fp, &stVdaData);
                /*******************************************************
                   *step 2.5 : release stream
                   *******************************************************/
                s32Ret = HI_MPI_VDA_ReleaseData(VdaChn,&stVdaData);
                if(s32Ret != HI_SUCCESS)
	            {
	                SAMPLE_PRT("HI_MPI_VDA_ReleaseData failed with %#x!\n", s32Ret);
	                return NULL;
	            }
            }
        }
    }

    return HI_NULL;
}
/*********************************
 *获取MD结果
 ***********************************/
void HiMD_GetResult(VDA_CHN VdaChn)
{
    gs_stMdParam.bThreadStart = HI_TRUE;
    gs_stMdParam.VdaChn   = 0;
    pthread_create(&gs_VdaPid, 0, HiMD_GetResultThread, (HI_VOID *)&gs_stMdParam);
}
/*****************************
 *移动侦测初始化
 ******************************/
int HiMD_Start(VDA_CHN VdaChn)
{
    SIZE_S stSize;     
    stSize.u32Width = 352;
    stSize.u32Height = 288;
    if (VDA_MAX_WIDTH < stSize.u32Width || VDA_MAX_HEIGHT < stSize.u32Height)
    {
        SAMPLE_PRT("Picture size invaild!\n");
        return HI_FAILURE;
    }
   /*step1:create VDA Chn*/ 
   HiMD_CreateVdaChn(VdaChn, stSize);
   /* step 2: vda chn bind vi chn */
   HiMD_BindViChn(VdaChn,stSize);
   /* step 3: vda chn start recv picture */
   HiMD_StartRevPic(VdaChn);
   return 0;
}

/***********************************
 *功能：
 ***********************************/
static void *Hi_MotionDetectThread(void *arg)
{
    VDA_CHN VdaChn = 0;
    HiMD_Start(VdaChn); 
   /* create thread to get result */
    HiMD_GetResult(VdaChn);
    while(1)
    {
        sleep(10);    
    }
    return NULL;
}
/***********************************
 *功能：
 ***********************************/
int HisiMotionDetect(void)
{
    CreatThread(Hi_MotionDetectThread,NULL);// 创建用户图片线程
    return 0;
}
