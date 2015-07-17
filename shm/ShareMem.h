#ifndef __SHARE_MEM__H__
#define __SHARE_MEM__H__

#ifdef __cplusplus

extern "C" {
#endif 

#define SHM_ID_SERVER	    1
#define SHM_ID_HIFB_QT	    2
#define SHM_ID_HIFB_VIDEO	3
#define SHM_ID_HIFB_FB	    4
#define SHM_ID_HIFB_CMD	    4
#define SHM_ID_AI           5

int ShmRecv(int id, void *pData, int *pLen);
void shm_initial(int ID);
int shm_send(int dID,void *pData,int len);
void byte_print(void*p,int len);

#ifdef __cplusplus
}
#endif
#endif
