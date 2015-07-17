#include "public.h"
#include "common.h"
#include "sample_comm.h"
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

/********************************
 *功能：获取配置文件中保存的分屏类型
 ********************************/
int GetCfgDispNum(void)
{
    int DispNum = 0;
    char KeyVal[2] = {0};
    char *File = "/config/HiConfig.ini";
    char *AppName = "DispNum";
    char *KeyName = "CurDispNum";
    int Ret;
    Ret = GetProfileString(File, AppName, KeyName, KeyVal);
    if(Ret != SUCCESS)
    {
        Printf("GetProfileString fail!\n");    
        return FAILURE ;
    }
    DispNum = atoi(KeyVal);
    return DispNum;
}

/********************************
 *功能： 设置配置文件中保存的分屏类型
 ********************************/
int SetCfgDispNum(int DispNum)
{
    char *File = "/config/HiConfig.ini";
    char *AppName = "DispNum";
    char *KeyName = "CurDispNum";
    char OldKeyVal[2] = {0};
    char NewKeyVal[2] = {0};
    int Ret;

    sprintf(NewKeyVal, "%d", DispNum);
    Ret = GetProfileString(File, AppName, KeyName, OldKeyVal);
    if(Ret != SUCCESS)
    {
        Printf("GetProfileString fail!\n");    
        return FAILURE ;
    }
    SetProfileString(File, KeyName, OldKeyVal,NewKeyVal);
    return 0;
}

/*********************************
 * 通道总数与分屏类型的转换 
 ********************************/
int DispNumToDispMode(int DispNum)
{
    int i;
    int DispArr[][2] = {
        {VO_MODE_1MUX, 1},
        {VO_MODE_4MUX, 4},
        {VO_MODE_9MUX, 9},
        {VO_MODE_16MUX, 16},
        {VO_MODE_25MUX, 25},
    }; 
    for(i = 0; i < sizeof(DispArr)/sizeof(DispArr[0]); i++  )
    {
        if(DispArr[i][1] == DispNum)    
        {
            return DispArr[i][0];
        }
    }
    return SUCCESS;
}

/*********************************
 * 通道总数与分屏类型的转换 
 ********************************/
 int DispModeToDispNum(int DispMode)
 {
     int i;
     int DispArr[][2] = {
        {VO_MODE_1MUX, 1},
        {VO_MODE_4MUX, 4},
        {VO_MODE_9MUX, 9},
        {VO_MODE_16MUX, 16},
        {VO_MODE_25MUX, 25},
    }; 
    for(i = 0; i < sizeof(DispArr)/sizeof(DispArr[0]); i++  )
    {
        if(DispArr[i][0] == DispMode)    
        {
            return DispArr[i][1];
        }
    }
    return SUCCESS;
 }
