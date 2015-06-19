#include "HiShm.h"

/******************************************
 *函数功能：从共享内存获取NVR的配置参数
 *****************************************/
static void *ShrMem_GetNvrCfgPara(void *arg)
{
    int ShmLen = LEN_BUF;
    char ret;
    char sBuf[LEN_BUF+1] = {0};
    shm_initial(SHM_ID_HIFB_CMD);
    while(1)
    {
        ShmLen = LEN_BUF;
        ret = ShmRecv(SHM_ID_HIFB_CMD, sBuf, &ShmLen);
        if (ret == 0)
        {
              usleep(1000*200);
              continue;
        }
        if(ShmLen <= 0)
        {
            fprintf(stdout, "need more buffer\n");    
        }
        AppFuncSearch(sBuf, ShmLen);
    }
     return NULL;    
}

/*****************************************
 *创建线程，该线程的作用是从共享内存中获取NVR的配置参数
 ****************************************/
int CreatThread_GetShrMemPara(void)
{
    CreatThread(ShrMem_GetNvrCfgPara,NULL);// 创建用户图片线程
    return 0;
}
