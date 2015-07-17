#include "HiCrop.h"

HI_S32 hi_vpss_StartClip(VPSS_GRP VpssClipGrp, VPSS_CROP_INFO_S *pstCropInfo, HI_BOOL StartClip)
{
	HI_S32 s32Ret;
	VPSS_CROP_INFO_S stCropInfo;

	if(StartClip == HI_TRUE)
	{
		stCropInfo.bEnable = HI_TRUE;
		stCropInfo.enCropCoordinate = VPSS_CROP_ABS_COOR;
		stCropInfo.stCropRect.s32X = pstCropInfo->stCropRect.s32X;
		stCropInfo.stCropRect.s32Y = pstCropInfo->stCropRect.s32Y;
		stCropInfo.stCropRect.u32Width = pstCropInfo->stCropRect.u32Width;
		stCropInfo.stCropRect.u32Height = pstCropInfo->stCropRect.u32Height;

		s32Ret = HI_MPI_VPSS_SetGrpCrop(VpssClipGrp, &stCropInfo);
		if (HI_SUCCESS != s32Ret)
		{
			Printf("HI_MPI_VPSS_SetGrpCrop failed with %#x!\n", s32Ret);
		}
	}
	else
	{
		stCropInfo.bEnable = HI_FALSE;
		stCropInfo.enCropCoordinate = VPSS_CROP_ABS_COOR;
		stCropInfo.stCropRect.s32X = 0;
		stCropInfo.stCropRect.s32Y = 0;
		stCropInfo.stCropRect.u32Width = 0;
		stCropInfo.stCropRect.u32Height = 0;

		s32Ret = HI_MPI_VPSS_SetGrpCrop(VpssClipGrp, &stCropInfo);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI_MPI_VPSS_SetGrpCrop failed with %#x!\n", s32Ret);
		}
	}

	return HI_SUCCESS;
}

HI_S32 hi_vpss_CheckInfo(VO_CHN_ATTR_S *stChnAttr, VPSS_CROP_INFO_S *stCropInfo, SIZE_S *stSize)
{
	HI_S32 s32X, s32Y, s32W, s32H;
	HI_S32 s32CropX, s32CropY, s32CropW, s32CropH;

	s32X = stChnAttr->stRect.s32X;
	s32Y = stChnAttr->stRect.s32Y;
	s32W = stChnAttr->stRect.u32Width;
	s32H = stChnAttr->stRect.u32Height;

	s32CropX = stCropInfo->stCropRect.s32X;
	s32CropY = stCropInfo->stCropRect.s32Y;
	s32CropW = stCropInfo->stCropRect.u32Width;
	s32CropH = stCropInfo->stCropRect.u32Height;

	/*vo 屏幕坐标 转 换图像实际坐标*/
	s32X = (s32CropX - s32X);
	s32Y = (s32CropY - s32Y);

	/*vo 图像坐标转化成vpss 图像坐标*/
	s32CropX = (s32X * stSize->u32Width)/s32W;
	s32CropY = (s32Y * stSize->u32Height)/s32H;
	s32CropW = (s32CropW * stSize->u32Width)/s32W;
	s32CropH = (s32CropH * stSize->u32Height)/s32H;

	/*对齐*/
	stCropInfo->stCropRect.s32X = ALIGN_BACK(s32CropX, 4);
	stCropInfo->stCropRect.s32Y = ALIGN_BACK(s32CropY, 4);
	stCropInfo->stCropRect.u32Width = ALIGN_BACK(s32CropW, 8);
	stCropInfo->stCropRect.u32Height = ALIGN_BACK(s32CropH, 8);

	if(stCropInfo->stCropRect.u32Width < 64 || stCropInfo->stCropRect.u32Height < 64)
	{
		Printf("Crop Width*Height < 64*64 \n");
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}

HI_S32 hi_vpss_CropMoveStep(VO_CHN_ATTR_S *stChnAttr, VPSS_CROP_INFO_S *stCropInfo, VPSS_CROP_INFO_S *stMoveInfo, SIZE_S *stSize)
{
	HI_S32 s32X, s32Y, s32W, s32H;
	HI_S32 s32CropX, s32CropY, s32CropW, s32CropH;
	HI_S32 s32MoveX, s32MoveY, s32MoveW, s32MoveH;
	HI_S32 stepX = 0, stepY = 0;

	/*vo 屏幕坐标和图像宽高*/
	s32X = stChnAttr->stRect.s32X;
	s32Y = stChnAttr->stRect.s32X;
	s32W = stChnAttr->stRect.u32Width; //ALIGN_BACK(stChnAttr->stRect.u32Width, 8);
	s32H = stChnAttr->stRect.u32Height;//ALIGN_BACK(stChnAttr->stRect.u32Height, 8);

	/*crop 起始点和宽高(vpss 坐标点)*/
	s32CropX = stCropInfo->stCropRect.s32X;
	s32CropY = stCropInfo->stCropRect.s32Y;
	s32CropW = stCropInfo->stCropRect.u32Width;
	s32CropH = stCropInfo->stCropRect.u32Height;

	/*拖动的起始点和终点坐标(vo 屏幕坐标点)*/
	s32MoveX = stMoveInfo->stCropRect.s32X;
	s32MoveY = stMoveInfo->stCropRect.s32Y;
	s32MoveW = stMoveInfo->stCropRect.u32Width;
	s32MoveH = stMoveInfo->stCropRect.u32Height;

	/*拖动坐标点(vo图像坐标点)*/
	s32MoveX = s32MoveX - s32X;
	s32MoveY = s32MoveY - s32Y;
	s32MoveW = s32MoveW - s32X;
	s32MoveH = s32MoveH - s32Y;

	stepX = ALIGN_UP((abs(s32MoveW - s32MoveX) * stSize->u32Width)/s32W, 4);
	stepY = ALIGN_UP((abs(s32MoveH - s32MoveY) * stSize->u32Height)/s32H, 4);

	/*左边界最小坐标是0*/
	if((s32MoveW - s32MoveX) >= 4)
	{
		s32CropX = s32CropX - stepX;
		s32CropX = s32CropX < 0 ? 0 : s32CropX;
	}
	/*右边界最大坐标(0 + s32W)*/
	if((s32MoveX - s32MoveW) >= 4)
	{
		s32CropX = s32CropX + stepX;
		s32CropX = (s32CropX + s32CropW) >= (0 + stSize->u32Width) ? (stSize->u32Width - s32CropW) : s32CropX;
	}
	/*上边界最小坐标是0*/
	if((s32MoveH - s32MoveY) >= 4)
	{
		s32CropY = s32CropY - stepY;
		s32CropY = s32CropY < 0 ? 0 : s32CropY;
	}
	/*下边界最大坐标(0 + s32H)*/
	if((s32MoveY - s32MoveH) >= 4)
	{
		s32CropY = s32CropY + stepY;
		s32CropY = ((s32CropY + s32CropH) > (0 + stSize->u32Height)) ? (stSize->u32Height - s32CropH) : s32CropY;
	}

	stCropInfo->stCropRect.s32X = s32CropX;
	stCropInfo->stCropRect.s32Y = s32CropY;
	stCropInfo->stCropRect.u32Width = s32CropW;
	stCropInfo->stCropRect.u32Height = s32CropH;

	return HI_SUCCESS;
}

HI_S32  ZoomInThread(int ZoomInEn, sdk_zoom_t *pZoom)
{	
	HI_S32 s32Ret;
	VPSS_GRP VpssGrp;
	VPSS_CROP_INFO_S stCropInfo;
	VPSS_CROP_INFO_S stMoveInfo;
	VO_CHN_ATTR_S stChnAttr;
	VO_CHN VoChn;
	VO_LAYER VoLayer = SAMPLE_VO_LAYER_VHD0;
	VDEC_CHN VdecChn;
	SIZE_S stPicSize;
	VIDEO_FRAME_INFO_S stFrameInfo;
	/*vpss clip grop*/
	if(pZoom->full_screen == 1)//fullScreen 
	{
		VoChn   = 0;
		VdecChn = 0;
		VpssGrp = 0;
	}
	else
	{
		VoChn   = pZoom->ch;
		VdecChn = pZoom->ch;
		VpssGrp = pZoom->ch;
	}

	if(ZoomInEn != 1)
	{
		/*get vo Attr*/
		s32Ret = HI_MPI_VO_GetChnAttr(VoLayer, VoChn, &stChnAttr);
		if (s32Ret != HI_SUCCESS)
		{
			Printf("%s(%d):failed with %#x!\n",__FUNCTION__,__LINE__,  s32Ret);
			return HI_FAILURE;
		}

		/*get vpss Attr*/
		s32Ret = HI_MPI_VPSS_GetGrpFrame(VpssGrp, &stFrameInfo, 0);
		if (s32Ret != HI_SUCCESS)
		{
			Printf("HI_MPI_VPSS_GetGrpFrame failed with %#x!\n", s32Ret);
			return HI_FAILURE;
		}
		stPicSize.u32Width = stFrameInfo.stVFrame.u32Width;
		stPicSize.u32Height = stFrameInfo.stVFrame.u32Height;
		s32Ret = HI_MPI_VPSS_ReleaseGrpFrame(VpssGrp, &stFrameInfo);
		if (s32Ret != HI_SUCCESS)
		{
			Printf("HI_MPI_VPSS_ReleaseGrpFrame failed with %#x!\n", s32Ret);
			return HI_FAILURE;
		}
	}
	/*start ZoomIn*/
	if(ZoomInEn == 0)
	{
		stCropInfo.stCropRect.s32X = pZoom->x;
		stCropInfo.stCropRect.s32Y = pZoom->y;
		stCropInfo.stCropRect.u32Width = pZoom->w;
		stCropInfo.stCropRect.u32Height = pZoom->h;
		s32Ret = hi_vpss_CheckInfo(&stChnAttr, &stCropInfo, &stPicSize);
		if(s32Ret != HI_SUCCESS)
		{
			return HI_FAILURE;
		}
		hi_vpss_StartClip(VpssGrp, &stCropInfo, HI_TRUE);
	}	
	/*stop ZoomIn*/
	if(ZoomInEn == 1)
	{
		hi_vpss_StartClip(VpssGrp, &stCropInfo, HI_FALSE);
	}
	/*Move ZoomIn*/
	if(ZoomInEn == 2)
	{
		stMoveInfo.stCropRect.s32X = pZoom->x;
		stMoveInfo.stCropRect.s32Y = pZoom->y;
		stMoveInfo.stCropRect.u32Width = pZoom->w;
		stMoveInfo.stCropRect.u32Height = pZoom->h;

		s32Ret = HI_MPI_VPSS_GetGrpCrop(VpssGrp, &stCropInfo);
		if(HI_SUCCESS != s32Ret)
		{
			Printf("HI_MPI_VPSS_SetGrpCrop failed with %#x!\n", s32Ret);
		}

		hi_vpss_CropMoveStep(&stChnAttr, &stCropInfo, &stMoveInfo, &stPicSize);

		s32Ret = HI_MPI_VPSS_SetGrpCrop(VpssGrp, &stCropInfo);
		if(HI_SUCCESS != s32Ret)
		{
			Printf("HI_MPI_VPSS_SetGrpCrop failed with %#x!\n", s32Ret);
		}

	}
	return HI_SUCCESS;
    
}
