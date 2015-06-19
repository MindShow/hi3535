#ifndef __HI_SHM_H__
#define __HI_SHM_H_

#ifdef __cplusplus
extern "C" {
#endif 
#include <stdio.h>
#include <unistd.h>
#include "common.h"
#include "public.h"
#include "ShareMem.h"
 #include "appFuncTable.h"

#define LEN_BUF 256

int CreatThread_GetShrMemPara(void);

#ifdef __cplusplus
}
#endif
#endif

