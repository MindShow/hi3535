/******************************************
 *本文件的主要功能是对C++函数封装。将其返回值
  重新封装为C语言支持的数据类型。
 ******************************************/
#include <stdio.h>
#include "ShareMem.h"
#include "shm_interface.h"
int ShmRecv(int id, void *pData, int *pLen)
{
    int len=*pLen;
    bool bRet;
    bRet = (shm_receive_from_id(id, pData, len));
    *pLen = len;
    return bRet?1:0;
}

