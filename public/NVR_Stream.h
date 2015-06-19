#ifndef _NVR_STREAM_H_
#define _NVR_STREAM_H_
#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <memory.h>
#include "global.h"
#include "common.h"

void NVR_ReadBYTE(char** pBuf, char* pValue);
void NVR_ReadWORD(char** pBuf, short* pValue);
void NVR_ReadDWORD(char** pBuf, int* pValue);
void NVR_ReadDWORD64(char** pBuf, long long* pValue);
void NVR_ReadBYTEArray(char** pbyBuf, char* pbyDst, short wLen);
// void NVR_ReadNVRTime(char** pBuf, NVRTime* pTime);

void NVR_WriteBYTE(char** pBuf, char value);
void NVR_WriteWORD(char** pBuf, short value);
void NVR_WriteDWORD(char** pBuf, int value);
void NVR_WriteDWORD64(char** pBuf, long long value);
void NVR_WriteBYTEArray(char** pbyBuf, const char* pbySrc, short wLen);
// void NVR_WriteNVRTime(char** pBuf, const NVRTime* pTime);
void NVR_WriteStr(char** pbyBuf, const CHAR* str, short wLen);

// check name if it is legal or not.
// such as device name, group name, record plan name, etc.
// ERRCODE NVR_CheckName(const char *name);
// void NVR_NameCopy(char *dst, const char *src, Dshort srcLen);

// char* NVR_IpNToA(Dshort ip);

#ifdef __cplusplus
}
#endif
#endif
