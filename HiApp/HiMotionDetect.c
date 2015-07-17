#include "HiMotionDetect.h"

static pthread_t g_VdaPid;
static pthread_t g_mdpid;
static VDA_MD_PARAM_S g_stMdParam;
static int MdEnFlg = 1;

/********************************
 * 用户设置移动侦测的使能标志
*******************************/
void HiMdSetEn(int EnFlg)
{
    Printf("MdEnFlg = %d\n",MdEnFlg);
    MdEnFlg = EnFlg;        
}

int mySAMPLE_COMM_VDA_MdPrtObj(char *pData,HI_U32 len, VDA_DATA_S *pstVdaData)
{
    VDA_OBJ_S *pstVdaObj;
    HI_U32 i;
	int nIndex = 0;
    
    if (HI_TRUE != pstVdaData->unData.stMdData.bObjValid) return -1;
	
	if (len < 12+pstVdaData->unData.stMdData.stObjData.u32ObjNum*2*4) return -2;
	len = 12+pstVdaData->unData.stMdData.stObjData.u32ObjNum*2*4;
	
//    fprintf(fp, "ObjNum=%d, IndexOfMaxObj=%d, SizeOfMaxObj=%d, SizeOfTotalObj=%d\n", 
//                   pstVdaData->unData.stMdData.stObjData.u32ObjNum, 
//		     pstVdaData->unData.stMdData.stObjData.u32IndexOfMaxObj, 
//		     pstVdaData->unData.stMdData.stObjData.u32SizeOfMaxObj,
//		     pstVdaData->unData.stMdData.stObjData.u32SizeOfTotalObj);
	 *(HI_U32 *)(pData+nIndex) = pstVdaData->unData.stMdData.stObjData.u32ObjNum;
	 nIndex += 4;
	 *(HI_U32 *)(pData+nIndex) = pstVdaData->unData.stMdData.stObjData.u32IndexOfMaxObj;
	 nIndex += 4;
	 *(HI_U32 *)(pData+nIndex) = pstVdaData->unData.stMdData.stObjData.u32SizeOfMaxObj;
	 nIndex += 4;
	 *(HI_U32 *)(pData+nIndex) = pstVdaData->unData.stMdData.stObjData.u32SizeOfTotalObj;
	 nIndex += 4;
    for (i=0; i<pstVdaData->unData.stMdData.stObjData.u32ObjNum; i++)
    {
        pstVdaObj = pstVdaData->unData.stMdData.stObjData.pstAddr + i;
//        fprintf(fp, "[%d]\t left=%d, top=%d, right=%d, bottom=%d\n", i, 
//			  pstVdaObj->u16Left, pstVdaObj->u16Top, 
//			  pstVdaObj->u16Right, pstVdaObj->u16Bottom);
		*(HI_U16 *)(pData+nIndex) = pstVdaObj->u16Left;
		nIndex += 2;
		*(HI_U16 *)(pData+nIndex) = pstVdaObj->u16Top;
		nIndex += 2;
		*(HI_U16 *)(pData+nIndex) = pstVdaObj->u16Right;
		nIndex += 2;
		*(HI_U16 *)(pData+nIndex) = pstVdaObj->u16Bottom;
		nIndex += 2;
    }

    return len;
}

int mySAMPLE_COMM_VDA_MdPrtAp(char *pData,HI_U32 len, VDA_DATA_S *pstVdaData)
{
    if (HI_TRUE != pstVdaData->unData.stMdData.bPelsNumValid) return -1;
	
	if (len < 4) return -2;
	len = 4;
	
	int nIndex = 0;
	
    //fprintf(fp, "AlarmPixelCount=%d\n", pstVdaData->unData.stMdData.u32AlarmPixCnt);
	*(HI_U32 *)(pData+nIndex) = pstVdaData->unData.stMdData.u32AlarmPixCnt;
	nIndex += 4;

    return len;
}

int mySAMPLE_COMM_VDA_MdPrtSad(char *pData,HI_U32 len,VDA_DATA_S *pstVdaData)
{
    HI_U32 i, j;
    HI_VOID *pAddr;
	
    if (HI_TRUE != pstVdaData->unData.stMdData.bMbSadValid) return -1;

	HI_U32  u32MbHeight = pstVdaData->u32MbHeight;
	HI_U32  u32MbWidth = pstVdaData->u32MbWidth;
	
	if (len < 8+u32MbHeight*u32MbWidth*2) return -2;
	len = 8+u32MbHeight*u32MbWidth*2;
	
	int nIndex = 8;
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
	            //fprintf(fp, "%-2d ",*pu8Addr);
				*(HI_U16 *)(pData+nIndex) = *pu8Addr;
				nIndex += 2;
	        }
	        else
	        {
	            pu16Addr = (HI_U16 *)pAddr + j;
				//fprintf(fp, "%-4d ",*pu16Addr);
				*(HI_U16 *)(pData+nIndex) = *pu16Addr;
				nIndex += 2;
	        }
		}
    }
	
    return len;
}

void mySend2HIAI(VDA_DATA_S *pstVdaData)
{
    static const int len = PIC_RESULT_LEN;
	char sBuf[PIC_RESULT_LEN]={0};
	int nIndex = 0;
	int nRes = 0;
	
	nRes = mySAMPLE_COMM_VDA_MdPrtSad(sBuf+nIndex+4,len-nIndex-4,pstVdaData);
	if (nRes<0)
	{
		*(int*)(sBuf+nIndex) = 0;
	}
	else
	{
		*(int*)(sBuf+nIndex) = nRes;
		nIndex += 4;
		nIndex += nRes;
	}

	nRes = mySAMPLE_COMM_VDA_MdPrtObj(sBuf+nIndex+4,len-nIndex-4,pstVdaData);
	if (nRes<0)
	{
		*(int*)(sBuf+nIndex) = 0;
	}
	else
	{
		*(int*)(sBuf+nIndex) = nRes;
		nIndex += 4;
		nIndex += nRes;
	}
	
	nRes = mySAMPLE_COMM_VDA_MdPrtAp(sBuf+nIndex+4,len-nIndex-4,pstVdaData);
	if (nRes<0)
	{
		*(int*)(sBuf+nIndex) = 0;
	}
	else
	{
		*(int*)(sBuf+nIndex) = nRes;
		nIndex += 4;
		nIndex += nRes;
	}
	
	shm_send(SHM_ID_AI,sBuf,nIndex); 
    
}
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
int HiMD_BindViChn(VDA_CHN Chn,SIZE_S stSize)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn, stDestChn;
    VPSS_CHN_MODE_S stVpssChnMode; 
    VPSS_CHN VpssChn = 1; 
    VPSS_GRP VpssGrp = Chn;
    VDEC_CHN VdaChn = Chn;

    stSrcChn.enModId = HI_ID_VPSS;// 模块号
    stSrcChn.s32DevId = VpssGrp;// 设备号
    stSrcChn.s32ChnId = VpssChn;// 通道号

    stDestChn.enModId = HI_ID_VDA;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VdaChn;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);// 数据源绑定到接收源
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("err!\n");
        return s32Ret;
    }

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
    system("clear");
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
            SAMPLE_PRT("get vda result time out, continue!\n");
            continue;
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
    g_stMdParam.bThreadStart = HI_TRUE;
    g_stMdParam.VdaChn   = 0;
    pthread_create(&g_VdaPid, 0, HiMD_GetResultThread, (HI_VOID *)&g_stMdParam);
}

/*****************************
 *移动侦测开启,初始化
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
 *功能： 关闭移动侦测功能，释放资源
 ***********************************/
int HiMD_Stop(VDA_CHN VdaChn)
{

    return 0;
}

/***********************************
 *功能：
 ***********************************/
static void *Hi_MotionDetectThread(void *arg)
{
    VDA_CHN VdaChn = 0;
    while(1)
    {
        if(MdEnFlg == 1)
        {
            MdEnFlg = 0;
            Printf("HiMD_Start!\n\n");
            HiMD_Start(VdaChn); 
            /* create thread to get result */
            HiMD_GetResult(VdaChn);
        }
        sleep(10);    
    }
    return NULL;
}

/***********************************
 *功能：
 ***********************************/
int HisiMotionDetect(void)
{
    pthread_create(&g_mdpid, NULL, Hi_MotionDetectThread, NULL);// 创建用户图片线程
    return 0;
}
