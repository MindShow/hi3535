#include "NVR_Stream.h"

void NVR_ReadBYTE(char** pBuf, char* pValue)
{
    *pValue = *(char*)(void*)(*pBuf);
    *pBuf += sizeof(char);
}
void NVR_ReadWORD(char** pBuf, short* pValue)
{
    short temp;
    temp = *(short*)(void*)(*pBuf);
    *pValue = temp; 
    *pBuf += sizeof(short);
}
void NVR_ReadDWORD(char** pBuf, int* pValue)
{
    int temp;
    temp = *(int*)(void*)(*pBuf);
    *pValue = *(int*)(void*)(*pBuf);
    *pBuf += sizeof(int);
}
void NVR_ReadDWORD64(char** pBuf, long long* pValue)
{
    *pValue = *(long long*)(void*)(*pBuf);
    *pBuf += sizeof(long long);
}
void NVR_ReadBYTEArray(char** pbyBuf, char* pbyDst, short wLen)
{
    memcpy(pbyDst, *pbyBuf, wLen);
    *pbyBuf += wLen;
}

void NVR_WriteBYTE(char** pBuf, char value)
{
    *(char*)(void*)(*pBuf) = value;
    *pBuf += sizeof(char);
}
void NVR_WriteWORD(char** pBuf, short value)
{
    *(short*)(void*)(*pBuf) = value;
    *pBuf += sizeof(short);
}
void NVR_WriteDWORD(char** pBuf, int value)
{
    *(int*)(void*)(*pBuf) = value;
    *pBuf += sizeof(int);
}
void NVR_WriteDWORD64(char** pBuf, long long value)
{
    *(long long*)(void*)(*pBuf) = value;
    *pBuf += sizeof(long long);
}
void NVR_WriteBYTEArray(char** pbyBuf, const char* pbySrc, short wLen)
{
    memcpy(*pbyBuf, pbySrc, wLen);
    *pbyBuf += wLen;
}
void NVR_WriteStr(char** pbyBuf, const CHAR* str, short wLen)
{
    if (0 == wLen)
    {
        return;
    }
    short wCopyLen = wLen;
    short wStrLen = (short)strlen(str);
    if (wStrLen < wCopyLen)
    {
        wCopyLen = wStrLen;
    }
    else
    {
        --wCopyLen;
    }
    memcpy(*pbyBuf, str, wCopyLen);
    (*pbyBuf)[wCopyLen] = 0;
    *pbyBuf += wLen;
}
