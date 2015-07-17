#ifndef _SHM_INTERFACE_H_HEADER  
#define _SHM_INTERFACE_H_HEADER  
	
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* --------------------------------------------------------------//
// 参数：
// int ID 			: 指定当前进程模块ID，我们定义Server：1，HiFB和QT:2
//
// 函数说明			：初始化接收数据模块，用完调用shm_free函数释放
	
*/
#define SHM_ID_SERVER	    1
#define SHM_ID_HIFB_QT	    2
#define SHM_ID_HIFB_VIDEO	3
#define SHM_ID_HIFB_FB	    4
#define SHM_ID_HIFB_CMD	    4
#define SHM_ID_AI           5
#define SHM_ID_SERVER1	    101
#define SHM_ID_HIFB1_VIDEO	103
#define SHM_ID_HIFB1_FB	    104
#define SHM_ID_HIFB1_CMD    104
#define SHM_ID_SERVER2	    201
#define SHM_ID_HIFB2_VIDEO	203
#define SHM_ID_HIFB2_FB	    204
#define SHM_ID_HIFB2_CMD    204
#define SHM_ID_SERVER3	    301
#define SHM_ID_HIFB3_VIDEO	303
#define SHM_ID_HIFB3_FB	    304
#define SHM_ID_HIFB3_CMD    304
void shm_initial(int ID);

/* --------------------------------------------------------------//
// 参数：
// int dID			: 目标ID
// void *pData		: 发送的数据首地址
// int len			: 发送的数据长度
//
// 返回值			：0表示成功，非0表示失败
//
// 函数说明			：用于发送数据，
*/
int shm_send(int dID,void *pData,int len);
int shm_send_to_server(void *pData,int len);
int shm_send_to_fb_buf(int dID,void *pData,int len);
int shm_send_to_cur_board(int dID,void *pData,int len);

/* --------------------------------------------------------------//
// 参数：
// int dID			: 目标ID
// void *pData		: 接收数据缓冲区的首地址
// int len			: 输入时表示缓冲区的大小，输出时表示接收到的数据长度
//
// 返回值			：true表示接收到数据，false表示没有接收到数据，注意，当返回值为true，len参数为0时，表示有数据过来，但接收缓冲区放不下。
//
// 函数说明			：用于接收数据，函数直接返回
*/
bool shm_receive(void *pData,int &len);
bool shm_receive_from_id(int id,void *pData,int &len);
bool shm_recv_from_fb_buf(int &dID,void *pData,int &len);

/* --------------------------------------------------------------//
// 参数：无
//
// 函数说明			：释放模块
*/
void shm_free();

/* --------------------------------------------------------------//
// 参数：无
//
// 返回值			: 0->主hi3535板，1->子板1号板，2->2号板，3->3号板
//
// 函数说明			: 获取所在的板号
*/
int shm_getBoardIndex();

/* --------------------------------------------------------------//
// 参数：无
//
// 返回值			:
//
// 函数说明			: 保存自定义数据
*/
bool shm_saveData(int id,char *pData,int len);

/* --------------------------------------------------------------//
// 参数：无
//
// 返回值			:
//
// 函数说明			: 载入自定义数据
*/
bool shm_loadData(int id,char *pData,int &len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  
