#include "main.h"
int main(int argc, char *argv[])
{
    startWtdThread();// 开启看门狗
	// HisiDecodeInit();
    // HisiFramBuffer();
    // CreatThread_GetShrMemPara();// 创建线程，轮询共享内存,从中获取配置参数。
    // HisiMotionDetect();
    while(1)
    {
        sleep(10);    
    }
	return 1;
}
