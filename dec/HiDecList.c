#include "HiDecList.h"

static decode_thread_channel_info_t decodeChannelList;

/***********************
 * 初始化解码链表
 ************************/
int DecLink_Init(void)
{
   INIT_LIST_HEAD(&decodeChannelList.list);
   return SUCCESS;
}

/***********************
 * 添加通道信息到链表 
 * 说明：
 *   1)
 ************************/
static int DecLink_AddChn(decode_thread_channel_info_t *DecChnList, Decode_ChnInfo *DecChnInfo)
{
    decode_thread_channel_info_t *tmp = NULL; 
    decode_thread_channel_info_t *pDecodeChannel = NULL;
    list_for_each_entry(tmp, &DecChnList->list, list)
    {
        if((tmp->ChnNo == DecChnInfo->ChnNo) && (tmp->WinPos == DecChnInfo->WinPos))
        {//解码通道的消息
            Printf("Add Chn Fail!Chn already Exist\n");
            return -1;    
        }
    }
    pDecodeChannel = (decode_thread_channel_info_t *)malloc(sizeof(decode_thread_channel_info_t));
    memset(pDecodeChannel, 0, sizeof(decode_thread_channel_info_t));
    pDecodeChannel->ChnNo = DecChnInfo->ChnNo;
    pDecodeChannel->WinPos   = DecChnInfo->WinPos;
    pDecodeChannel->nWidth = DecChnInfo->nWidth;
    pDecodeChannel->nHeight = DecChnInfo->nHeight;
    // strcpy(pDecodeChannel->Url, DecChnInfo->Url);
    list_add(&(pDecodeChannel->list), &(DecChnList->list));
    HiDec_ReleseDecResource(pDecodeChannel->WinPos);
    HiDec_CreateSendThread(pDecodeChannel);    
    return 0;
}
/***********************
 * 从链表中删除某个通道 
 * 说明：不能清除通道资源，因为用户图片还会用到。 
 ************************/
static int DecLink_DelChn(decode_thread_channel_info_t *DecChnList, Decode_ChnInfo *DecChnInfo)
{
    decode_thread_channel_info_t *tmp = NULL;
    list_for_each_entry(tmp, &DecChnList->list, list)
    {
        if(tmp->WinPos == DecChnInfo->WinPos)
        {
            Printf("Del pid:%lu WinPos:%d ok\n",tmp->pthID,tmp->WinPos);
            list_del(&(tmp->list));
            tmp->stVdecSend.eCtrlSinal=VDEC_CTRL_STOP;// 必须要这一操作 
            pthread_join(tmp->pthID, HI_NULL);
            return HI_SUCCESS;
        }
    }
    return HI_SUCCESS;
}
/***********************
 * 从链表中删除某个通道 
 ************************/
static int DecLink_DelAllChn(decode_thread_channel_info_t *DecChnList, Decode_ChnInfo *DecChnInfo)
{
    decode_thread_channel_info_t *tmp = NULL;
    list_for_each_entry(tmp, &DecChnList->list, list)
    {
        Printf("Del All Chn pid:%lu WinPos:%d ok\n",tmp->pthID,tmp->WinPos);
        list_del(&(tmp->list));
        tmp->stVdecSend.eCtrlSinal=VDEC_CTRL_STOP;// 必须要这一操作 
        pthread_join(tmp->pthID, HI_NULL);
    }
    return HI_SUCCESS;    
}
/**********************************
 * 暂停所有的通道
 ***************************** */
static int DecLink_PauseAllChn(decode_thread_channel_info_t *DecChnList, Decode_ChnInfo *DecChnInfo)
{
    decode_thread_channel_info_t *tmp = NULL;
    Printf("DecLink_PauseAllChn\n");
    list_for_each_entry(tmp, &DecChnList->list, list)
    {
        Printf("=============tmp->WinPos:%d\n",tmp->WinPos);
        if(tmp->pthID != 0)
        {
            tmp->stVdecSend.eCtrlSinal=VDEC_CTRL_STOP;// 必须要这一操作 
            pthread_join(tmp->pthID, HI_NULL);
        }
    }
    return HI_SUCCESS;        
}
/**********************************
 * 暂停单个通道，释放通道资源
 ***************************** */
static int DecLink_PauseSingleChn(decode_thread_channel_info_t *DecChnList, Decode_ChnInfo *DecChnInfo)
{
    decode_thread_channel_info_t *tmp = NULL;
    Printf("DecLinkPauseSingleView:%d\n",DecChnInfo->WinPos);
    list_for_each_entry(tmp, &DecChnList->list, list)
    {
        if(tmp->WinPos == DecChnInfo->WinPos)
        {
            tmp->stVdecSend.eCtrlSinal=VDEC_CTRL_STOP;// 必须要这一操作 
            pthread_join(tmp->pthID, HI_NULL);
        }
    }
    return HI_SUCCESS;        
}
/*************************
 * 运行链表中所有的通道
 * *****************/
static int DecLink_RunAllChn(decode_thread_channel_info_t *DecChnList, Decode_ChnInfo *DecChnInfo)
{
    decode_thread_channel_info_t *tmp = NULL;
    list_for_each_entry(tmp, &DecChnList->list, list)
    {
        Printf("view:%d\n",tmp->WinPos);
        HiDec_CreateSendThread(tmp);    
    }
    return HI_SUCCESS; 
}
/*************************
 * 运行链表中某个通道,主要是用于全屏某个画面
 * *****************/
static int DecLink_RunSingleChn(decode_thread_channel_info_t *DecChnList, Decode_ChnInfo *DecChnInfo)
{
    decode_thread_channel_info_t *tmp = NULL;
    list_for_each_entry(tmp, &DecChnList->list, list)
    {
        if(tmp->WinPos == DecChnInfo->WinPos)
        {
            Printf("Run view:%d ok\n", tmp->WinPos);
            HiDec_CreateSendThread(tmp);    
            return 0;
        }
    }
    return HI_SUCCESS;
}

/*************************
 * 切换分屏时，先获取链表中通道的分辨率，用来初始化通道。
 * *****************/
static int DecLink_GetResolution(decode_thread_channel_info_t *DecChnList, Decode_ChnInfo *DecChnInfo)
{
    decode_thread_channel_info_t *tmp = NULL;
    list_for_each_entry(tmp, &DecChnList->list, list)
    {
        if(tmp->WinPos == DecChnInfo->WinPos)
        {
            DecChnInfo->nWidth = tmp->nWidth;    
            DecChnInfo->nHeight = tmp->nHeight;    
        }
        
    }
    return HI_SUCCESS;
}

/*************************
 * 保存链表的信息，以便进入实时预览模式时能够恢复通道图像。 
 * *****************/
static int DecLink_SaveChnInfo(decode_thread_channel_info_t *DecChnList, Decode_ChnInfo *DecChnInfo)
{   
    decode_thread_channel_info_t *tmp = NULL;
    list_for_each_entry(tmp, &DecChnList->list, list)
    {
        DecChnList -> WinPos = tmp -> WinPos; 
        DecChnList -> nWidth = tmp -> nWidth; 
        DecChnList -> nHeight= tmp -> nHeight; 
        DecChnList++; 
    }
    return HI_SUCCESS;
}
/****************************
 *解码链表，完成通道的添加/删除等操作。
 ****************************/
HI_S32 Hi_DecLinkManage(int DecLinkOperaType, Decode_ChnInfo *DecChnInfo)
{
    switch(DecLinkOperaType)
    {
        case  DEC_LIST_OPERA_TYPE_ADD:
        {
            Printf("Add WinPos:%d\n",DecChnInfo->WinPos);
            DecLink_AddChn(&decodeChannelList,DecChnInfo);
        }
        break;
        case  DEC_LIST_OPERA_TYPE_DEL:
        {
            Printf("Del WinPos:%d\n",DecChnInfo->WinPos);
            DecLink_DelChn(&decodeChannelList,DecChnInfo);
        }
        break;
        case  DEC_LIST_OPERA_TYPE_DEL_ALL:
        {
            Printf("Del All WinPos:%d\n",DecChnInfo->WinPos);
            DecLink_DelAllChn(&decodeChannelList,DecChnInfo);
        }
        break;
        case  DEC_LIST_OPERA_TYPE_PAUSE_ALL:
        {
            Printf("Pause All WinPos\n");
            DecLink_PauseAllChn(&decodeChannelList,DecChnInfo);
        }
        break;
        case  DEC_LIST_OPERA_TYPE_RUN_ALL:
        {
            Printf("Run All WinPos\n");
            DecLink_RunAllChn(&decodeChannelList,DecChnInfo);
        }
        break;
        case  DEC_LIST_OPERA_TYPE_RUN:
        {
            Printf("Run Single WinPos\n"); 
            DecLink_RunSingleChn(&decodeChannelList,DecChnInfo);
        }
        break;
        case  DEC_LIST_OPERA_TYPE_PAUSE:
        {
            Printf("Pause Single WinPos\n"); 
            DecLink_PauseSingleChn(&decodeChannelList,DecChnInfo);
        }
        break;
        case  DEC_LIST_OPERA_TYPE_GET_RESOLUTION:
        {
            Printf("Get declist resolution!Chn:%d\n",DecChnInfo->WinPos); 
            DecLink_GetResolution(&decodeChannelList,DecChnInfo);
        }
        break;
        case  DEC_LIST_OPERA_TYPE_GET_CHN_INFO:
        {
            Printf("Get declist resolution!Chn:%d\n",DecChnInfo->WinPos); 
            DecLink_SaveChnInfo(&decodeChannelList,DecChnInfo);
        }
        break;
        default:
        break;
    }
    return HI_SUCCESS;    
}
