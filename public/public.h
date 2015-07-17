#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "CfgFileOpera.h"

typedef void *(*sthread)(void*);
void PrintByte(void*p,int len);

int GetCfgDispNum(void);
int SetCfgDispNum(int DispNum);
int DispNumToDispMode(int DispNum);
int DispModeToDispNum(int DispMode);
int CreatThread(sthread func,void* param);

#ifdef __cplusplus
}
#endif
#endif

