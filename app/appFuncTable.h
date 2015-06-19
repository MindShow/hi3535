#ifndef __APP_FUNC_TABLE_H__
#define __APP_FUNC_TABLE_H___ 

#ifdef __cplusplus
extern "C" {
#endif 
#include <stdio.h>
#include "dec.h"
#include "NVR_Stream.h"
#include "NvrCmdCode.h"
typedef int (*AppFunc)(void *pRecvData, int Len);
typedef struct AppMainCfg{
    int cmdCode;
    AppFunc AppFunction;
}AppMainCfg;

int AppFuncSearch(char *pRecv, int Len);
#ifdef __cplusplus
}
#endif
#endif

