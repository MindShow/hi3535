#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif 
#include <pthread.h>
#include <stdio.h>
typedef void *(*sthread)(void*);
void PrintByte(void*p,int len);

int CreatThread(sthread func,void* param);

#ifdef __cplusplus
}
#endif
#endif

