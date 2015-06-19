#ifndef __SAMPLEBUF__H__
#define __SAMPLEBUF__H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif 

/* --------------------------------------------------------------//
// 参数：
// int nChnNum			: 初始化通道数
//
// 返回值			    : void
//
// 函数说明			    : 创建通道，用户接受server模块送过来的vedio数据
*/
void initailize_recieve_buf(int nChnNum);

/* --------------------------------------------------------------//
// 参数：
// int id           : 通道id
// char *URL        : video source
//
// 返回值			: 通道描述符
//
// 函数说明			: 初始化通道
*/
FILE *myfopen(int id);
FILE *myfopenEx(int id, char *URL);

/* --------------------------------------------------------------//
// 参数：
// FILE* fd			: 描述
//
// 返回值			: void
//
// 函数说明			: 关闭通道
*/
void myfclose(FILE* fd);

/* --------------------------------------------------------------//
// 参数：
// char *pBuf          : 接受数据buffer
// int len             : buffer的长度
// FILE* fd            : 通道描述符
//
// 返回值              : 总是>0,没数据时会挂起
//
// 函数说明            : 用于读取数据，
*/
unsigned int myfread(char *pBuf,int len,FILE* fd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif