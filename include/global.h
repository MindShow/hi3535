#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifdef __cplusplus
extern "C" {
#endif 

typedef unsigned char  BYTE;
typedef char  CHAR;
typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef long long      DWORD64;

#define MAX_DISPLAY_NUM         25 //最多显示的通道数
// 当前输出显示的通道数。
#define CUR_DISPLAY_1MUX     1 // 一分屏，即全屏显示 ,以下的类似。
#define CUR_DISPLAY_4MUX     4
#define CUR_DISPLAY_9MUX     9
#define CUR_DISPLAY_16MUX    16
#define CUR_DISPLAY_25MUX    25

#define STREAM_HEADER_LEN    12

#define PLAY_MODE_PREVIEW    0xFA
#define PLAY_MODE_PLAYBACK   0xFB

typedef struct tagCmd
{
    short cmdCode;//命令字
    short cmdCtrl;//命令控制字
    short cmdIndex;//命令索引，client用这个字段来匹配返回
    short wRes;//保留
    int dataLen;//命令数据长度
}Cmd;
/***/         

#ifdef __cplusplus
}
#endif


#endif

