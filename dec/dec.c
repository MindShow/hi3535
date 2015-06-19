#include "dec.h"
pthread_mutex_t WriteCfgFileMutex = PTHREAD_MUTEX_INITIALIZER;
/********************************
 *功能：获取配置文件中保存的分屏类型
 ********************************/
static int GetCfgDispNum(void)
{
    int DispNum = 0;
    char KeyVal[2] = {0};
    char *File = "/config/HiConfig.ini";
    char *AppName = "DispNum";
    char *KeyName = "CurDispNum";
    int Ret;
    Ret = GetProfileString(File, AppName, KeyName, KeyVal);
    if(Ret != SUCCESS)
    {
        Printf("GetProfileString fail!\n");    
        return FAILURE ;
    }
    DispNum = atoi(KeyVal);
    return DispNum;
}
/********************************
 *功能： 设置配置文件中保存的分屏类型
 ********************************/
int SetCfgDispNum(int DispNum)
{
    char *File = "/config/HiConfig.ini";
    char *AppName = "DispNum";
    char *KeyName = "CurDispNum";
    char OldKeyVal[2] = {0};
    char NewKeyVal[2] = {0};
    int Ret;

    sprintf(NewKeyVal, "%d", DispNum);
    Ret = GetProfileString(File, AppName, KeyName, OldKeyVal);
    if(Ret != SUCCESS)
    {
        Printf("GetProfileString fail!\n");    
        return FAILURE ;
    }
    SetProfileString(File, KeyName, OldKeyVal,NewKeyVal);
    return 0;

    
}
/*********************************
 * 通道总数与分屏类型的转换 
 ********************************/
static int DispNumToDispMode(int DispNum)
 {
    int i;
    int DispArr[][2] = {
        {VO_MODE_1MUX, 1},
        {VO_MODE_4MUX, 4},
        {VO_MODE_9MUX, 9},
        {VO_MODE_16MUX, 16},
        {VO_MODE_25MUX, 25},
    }; 
    for(i = 0; i < sizeof(DispArr)/sizeof(DispArr[0]); i++  )
    {
        if(DispArr[i][1] == DispNum)    
        {
            return DispArr[i][0];
        }
    }
    return SUCCESS;
 }

/*********************************
 * 通道总数与分屏类型的转换 
 ********************************/
 int DispModeToDispNum(int DispMode)
 {
     int i;
     int DispArr[][2] = {
        {VO_MODE_1MUX, 1},
        {VO_MODE_4MUX, 4},
        {VO_MODE_9MUX, 9},
        {VO_MODE_16MUX, 16},
        {VO_MODE_25MUX, 25},
    }; 
    for(i = 0; i < sizeof(DispArr)/sizeof(DispArr[0]); i++  )
    {
        if(DispArr[i][0] == DispMode)    
        {
            return DispArr[i][1];
        }
    }
    return SUCCESS;
 }

int ChnNumToResolution(int s32ChnNum)
{
    int Resolution = 0;
    if(s32ChnNum == 1) Resolution = PIC_HD1080;
    else if(s32ChnNum == 4) Resolution = PIC_HD1080;
    else if(s32ChnNum == 9) Resolution = PIC_HD720;
    else if(s32ChnNum == 16) Resolution = PIC_D1;
    return Resolution;
}
/******************************
 *功能：释放解码通道的资源
 ******************************/
static int ReleseDecRes(int OldDispNum, int NewDispNum)
{
    int i;
    int OldResolution, NewResolution;
    Printf("OldDispNum:%d, NewDispNum:%d\n", OldDispNum, NewDispNum);
    OldResolution = ChnNumToResolution(OldDispNum);
    NewResolution = ChnNumToResolution(NewDispNum);
    Printf("OldResolution:%d, NewResolution:%d\n", OldResolution, NewResolution);
    if(NewResolution != OldResolution)
    {
        for(i = 0; i < OldDispNum; i++)
        {
            HiDec_ReleseDecResource(i);
        }
    }
    else if(OldDispNum > NewDispNum)
    {
        for(i = 0; i < OldDispNum; i++)
        {
            HiDec_ReleseDecResource(i);
        }
    }
    return SUCCESS;
} 
/******************************
 * 功能：添加通道
*****************************/
void DecListOpera_AddChn(int Chn, int nWidth, int nHeight)
{
    Decode_ChnInfo  DecChnInfo; 
    int ListOperaType = DEC_LIST_OPERA_TYPE_ADD; 
    DecChnInfo.WinPos= Chn;
    DecChnInfo.nWidth = nWidth;
    DecChnInfo.nHeight = nHeight;
    Hi_DecLinkManage(ListOperaType, &DecChnInfo); 
}

/******************************
 * 功能：删除通道
*****************************/
void DecListOpera_DelChn(int Chn)
{
    Decode_ChnInfo  DecChnInfo; 
    int ListOperaType = DEC_LIST_OPERA_TYPE_DEL; 
    DecChnInfo.WinPos= Chn;
    Hi_DecLinkManage(ListOperaType, &DecChnInfo); 
}

/******************************
 * 功能：获取链表中已存在的通道的宽高（分辨率）
*****************************/
void DecListOpera_GetResolution(int Chn, int *nWidth, int *nHeight)
{
    Decode_ChnInfo  DecChnInfo; 
    memset(&DecChnInfo, 0, sizeof(DecChnInfo));
    int ListOperaType = DEC_LIST_OPERA_TYPE_GET_RESOLUTION; 
    DecChnInfo.WinPos= Chn;
    Hi_DecLinkManage(ListOperaType, &DecChnInfo); 
    // Printf("WW:%d, hhh:%d\n",DecChnInfo.nWidth, DecChnInfo.nHeight);
        
}

/******************************
 *系统启动或者切换分屏时先初始化所有通道
 ************************** */
int Hi_DecStartAllChn(HI_S32 s32ChnNum)
{
    int i;
    HI_S32 s32Ret;
    int Resolution = 0;
	SIZE_S stSize;
    VDEC_CHN_STAT_S stChnStat;
    int w,h;
    Resolution = ChnNumToResolution(s32ChnNum);
    s32Ret = HiDec_GetPicSize(Resolution, &stSize);
    if(s32Ret != HI_SUCCESS)
    {
         Printf("HiDec_GetPicSize fail!\n");
         return HI_FAILURE;
    }
    for(i = 0; i < s32ChnNum; i++)
    {
        s32Ret = HiDec_QueryChnInfo(i, &stChnStat);
	    if(s32Ret != HI_SUCCESS)
        {
            s32Ret = HiDec_EnChn(i);
            if(s32Ret != HI_SUCCESS)
            {
                Printf("HiDec_EnChn fail!\n");
                return HI_FAILURE;
            }
            w = 0;
            h = 0;
            DecListOpera_GetResolution(i,&w,&h);
            if((w != 0) && (h != 0))
            {
                stSize.u32Width = w;
                stSize.u32Height = h;
            }
            s32Ret = HiDec_StartDecode(i, stSize);
            if(s32Ret != HI_SUCCESS)
            {
                Printf("HiDec_StartDecode fail!\n");
                return HI_FAILURE;
            }
        }
        else
        {
            Printf("SAMPLE_COMM_VO_StartChn%d OK!\n",i);
        }
    }
    return HI_SUCCESS;
}

/********************************
 *
 ********************************/
int Hi_DecStart(int DispMode)
{
    int OldDispNum, NewDispNum;
    if(DispMode >= 3)
    {
        Printf("DispMode Error!\n");    
        return FAILURE;
    }
    OldDispNum = GetCfgDispNum();
    NewDispNum = DispModeToDispNum(DispMode);
    Printf("Old:%d, New:%d\n",OldDispNum, NewDispNum);
    pthread_mutex_lock(&WriteCfgFileMutex); 
    SetCfgDispNum(NewDispNum);
    pthread_mutex_unlock(&WriteCfgFileMutex); 
    ReleseDecRes(OldDispNum, NewDispNum);
    initailize_recieve_buf(NewDispNum);
    HiDec_VoStartAllChn(DispMode);
    GetCurTotalChn(NewDispNum);
    Hi_DecStartAllChn(NewDispNum);
    return 0;
}

/*******************************************
 * *功能:海思解码模块
 ******************************************** */
int HisiDecodeInit(void)
{
    int DispMode = 0;
    int DispNum = 1;
    // pthread_mutex_init(&WriteCfgFileMutex, NULL);
    // 处理ctrl + c，kill等异常退出信号
    signal(SIGINT, Hi_DecHandleSig);
    signal(SIGTERM, Hi_DecHandleSig);
    Hi_DecMppInit();
    DecLink_Init();
    CreatThread(Hi_DecUsrPicThread, NULL);// 创建用户图片线程
    Hi_DecMppInit();
    DispNum = GetCfgDispNum();
    DispMode = DispNumToDispMode(DispNum);
    initailize_recieve_buf(DispNum);
    HiDec_VoStartAllChn(DispMode);
    GetCurTotalChn(DispNum);
    Hi_DecStartAllChn(DispNum);
#if 0
    sleep(3);
    while(1)
    {
        Hi_DecStart(0);
        sleep(5);
        Hi_DecStart(1);
        sleep(5);
        Hi_DecStart(2);
        sleep(5);
    }
#endif
    return 0;
}
