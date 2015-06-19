#include "public.h"
/*******************************************
 *创建线程
 *******************************************/
int CreatThread(sthread func,void* param)
{
 	int iRet=0;
    pthread_t		 threadID;
    pthread_attr_t   attr;	
	pthread_attr_init(&attr);	
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	iRet=pthread_create(&threadID,&attr,func,param);
	if(iRet != 0)
	{
		(void)pthread_attr_destroy(&attr);
		return -1;
	}
	return 0;
}

/**********************************************
 * 打印16进制 
 ************************************************/
void PrintByte(void*p,int len)
{
    int i=1;
    while(i<len+1)
    {
        fprintf(stdout,"%02X ",(unsigned char)(unsigned)(((char*)p)[i-1]));
        if (i>0&&0==i%24) fprintf(stdout,"\n");
        i++;
    }
    fprintf(stdout,"\n");
}
