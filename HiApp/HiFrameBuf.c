#include "HiFrameBuf.h"

#define WIDTH                  1280
#define HEIGHT                 720
#define GRAPHICS_LAYER_G0  0
#define FBIOGET_VSCREENINFO	0x4600

typedef struct hiPTHREAD_HIFB_SAMPLE
{
    HI_S32 fd;
    HI_S32 layer;
    HI_S32 ctrlkey;
}PTHREAD_HIFB_SAMPLE_INFO;

static struct fb_bitfield s_a32 = {24,8,0};
static struct fb_bitfield s_r32 = {16,8,0};
static struct fb_bitfield s_g32 = {8,8,0};
static struct fb_bitfield s_b32 = {0,8,0};


HI_VOID *SAMPLE_HIFB_REFRESH(void *arg)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HIFB_LAYER_INFO_S stLayerInfo = {0};
	HI_BOOL bShow; 
    HIFB_POINT_S stPoint = {0};
    struct fb_var_screeninfo stVarInfo;
	HI_CHAR file[12] = "/dev/fb0";
    HI_U32 maxW;
    HI_U32 maxH;
	HIFB_COLORKEY_S stColorKey;
    PTHREAD_HIFB_SAMPLE_INFO pstInfo;
    pstInfo.layer   =  0;
    pstInfo.fd      = -1;
    pstInfo.ctrlkey =  1;

	switch (pstInfo.layer)
	{
		case GRAPHICS_LAYER_G0:
			strcpy(file, "/dev/fb0");
			break;
		default:
			strcpy(file, "/dev/fb0");
			break;
	}
	
	/* 1. open framebuffer device overlay 0 */
	pstInfo.fd = open(file, O_RDWR, 0);
	if(pstInfo.fd < 0)
	{
		SAMPLE_PRT("open %s failed!\n",file);
		return HI_NULL;
	}  
	/*all layer surport colorkey*/  
	stColorKey.bKeyEnable = HI_TRUE;
	stColorKey.u32Key = 0x0;
	if (ioctl(pstInfo.fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
	{
    	SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB!\n");
    	close(pstInfo.fd);
    	return HI_NULL;
	}	
    s32Ret = ioctl(pstInfo.fd, FBIOGET_VSCREENINFO, &stVarInfo);
	if(s32Ret < 0)
	{
		SAMPLE_PRT("GET_VSCREENINFO failed!\n");
        close(pstInfo.fd);
		return HI_NULL;
	} 
	
    if (ioctl(pstInfo.fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        SAMPLE_PRT("set screen original show position failed!\n");
        close(pstInfo.fd);
        return HI_NULL;
    }

    maxW = WIDTH;
	maxH = HEIGHT;

    stVarInfo.transp= s_a32;
    stVarInfo.red = s_r32;
    stVarInfo.green = s_g32;
    stVarInfo.blue = s_b32;
    stVarInfo.bits_per_pixel = 32;
    stVarInfo.activate = FB_ACTIVATE_NOW;
    stVarInfo.xres = stVarInfo.xres_virtual = maxW;
    stVarInfo.yres = stVarInfo.yres_virtual = maxH;
    s32Ret = ioctl(pstInfo.fd, FBIOPUT_VSCREENINFO, &stVarInfo);
    if(s32Ret < 0)
	{
		SAMPLE_PRT("PUT_VSCREENINFO failed!\n");
        close(pstInfo.fd);
		return HI_NULL;
	} 
    switch (pstInfo.ctrlkey)
	{
		case 0 :
		{  
			stLayerInfo.BufMode = HIFB_LAYER_BUF_ONE;
			stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
			break;
		}
		
		case 1 :
	    {	
			stLayerInfo.BufMode = HIFB_LAYER_BUF_DOUBLE;
		    stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
			break;
		}

		default:
		{
			stLayerInfo.BufMode = HIFB_LAYER_BUF_NONE;
			stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
		}		
	}
    s32Ret = ioctl(pstInfo.fd, FBIOPUT_LAYER_INFO, &stLayerInfo);
    if(s32Ret < 0)
	{
		SAMPLE_PRT("PUT_LAYER_INFO failed!\n");
        close(pstInfo.fd);
		return HI_NULL;
	} 
	bShow = HI_TRUE;
    if (ioctl(pstInfo.fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        close(pstInfo.fd);
        return HI_NULL;
    }
    while(1)
    {
        sleep(10);    
    }
	return HI_NULL;   
}

int HisiFramBuffer(void)
{
    pthread_t phifb0 = -1;	
    pthread_create(&phifb0, 0, SAMPLE_HIFB_REFRESH, NULL);
    return 0;
}
