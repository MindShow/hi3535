#ifndef __HI_DEC_LIST_H__
#define __HI_DEC_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include "common.h"
#include "HiMppDec.h"

//解码链表的操作类型
typedef enum dec_list_opera_type
{
    DEC_LIST_OPERA_TYPE_ADD = 0,  //添加
    DEC_LIST_OPERA_TYPE_DEL,      //删除
    DEC_LIST_OPERA_TYPE_DEL_ALL,  //删除所有
    DEC_LIST_OPERA_TYPE_PAUSE,    //暂停
    DEC_LIST_OPERA_TYPE_PAUSE_ALL,//暂停所有
    DEC_LIST_OPERA_TYPE_RUN,      //运行
    DEC_LIST_OPERA_TYPE_RUN_ALL,  //运行所有
    DEC_LIST_OPERA_TYPE_GET_RESOLUTION//查询获取链表的内容 
}dec_list_opera_type;
    
HI_S32 Hi_DecLinkManage(int DecLinkOperaType, Decode_ChnInfo *DecChnInfo);
int DecLink_Init(void);

#ifdef __cplusplus
}
#endif
#endif

