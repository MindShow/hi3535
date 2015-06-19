#include "appFuncTable.h"

/******************************************
 * 功能：设置不同的分屏类型
 * 消息结构说明:
 *  1.消息头部
 *      命令字:0x01B1
 *  2.命令参数
 *      DispMode(int)
 *******************************************/ 
static int ShmMsg_SetDispMode(void *pRecvData, int Len)
{
    int DispMode = 0;
    char *buf = pRecvData;
    NVR_ReadDWORD(&buf, &DispMode);
    Printf("Mode:0x%08X\n",DispMode);
    Hi_DecStart(DispMode);
    return SUCCESS;
}

/******************************************
 * 功能：解码控制--播放
 * 消息结构说明:
 *  1.消息头部
 *      命令字:0x01A1
 *  2.命令参数
 *      chn(int)|width(int)|height(int)|url(string) 
 *
 *******************************************/ 
static int ShmMsg_VdecCtrlPlay(void *pRecvData, int Len)
{
   int Chn = 0;
   int nWidth = 0, nHeight = 0;
   char *pbuf = (char *)pRecvData;
   NVR_ReadDWORD(&pbuf, &Chn);
   NVR_ReadDWORD(&pbuf, &nWidth);
   NVR_ReadDWORD(&pbuf, &nHeight);
   Printf("Chn:%d,Width:%d,Height:%d\n",Chn,nWidth,nHeight);
   DecListOpera_AddChn(Chn, nWidth, nHeight);
   return SUCCESS;   
}

/******************************************
 * 功能：解码控制--停止
 * 消息结构说明:
 *  1.消息头部
 *      命令字:0x01A2
 *  2.命令参数
 *      chn(int)
 *
 *******************************************/ 
static int ShmMsg_VdecCtrlStop(void *pRecvData, int Len)
{
   int Chn = 0;
   char *pbuf = (char *)pRecvData;
   NVR_ReadDWORD(&pbuf, &Chn);
   Printf("Chn:%d\n",Chn);
   DecListOpera_DelChn(Chn);   
   return SUCCESS;
}

/*****************************************
 * 功能:12字节消息头部解析
 ******************************************/
static int ReadStreamHeader(char **pbyBuf, Cmd *pCmdHeader)
{
    NVR_ReadWORD(pbyBuf, &pCmdHeader->cmdCode);
    NVR_ReadWORD(pbyBuf, &pCmdHeader->cmdCtrl);
    NVR_ReadWORD(pbyBuf, &pCmdHeader->cmdIndex);
    NVR_ReadWORD(pbyBuf, &pCmdHeader->wRes);
    NVR_ReadDWORD(pbyBuf, &pCmdHeader->dataLen);
    return 0;
}

static  AppMainCfg AppMainCfgTable[] = 
{
    {CMD_VDEC_DISPLAY_MODE, ShmMsg_SetDispMode} 
   ,{CMD_VDEC_CTRL_PLAY, ShmMsg_VdecCtrlPlay} 
   ,{CMD_VDEC_CTRL_STOP, ShmMsg_VdecCtrlStop} 
};

int AppFuncSearch(char *pRecv, int Len)
{
    int i;
    Cmd CmdHeader; 
    char *pbyBuf;
    pbyBuf = pRecv;
    ReadStreamHeader(&pbyBuf, &CmdHeader);
    Printf("cmdCode:0x%04X\n", CmdHeader.cmdCode);
    for(i = 0; i < sizeof(AppMainCfgTable)/sizeof(AppMainCfgTable[0]); i++)            
    {
       if(AppMainCfgTable[i].cmdCode == CmdHeader.cmdCode)      
       {
            AppMainCfgTable[i].AppFunction(pbyBuf, Len - STREAM_HEADER_LEN); 
            break;
       }
    }
    return 0;
}
