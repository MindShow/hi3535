#ifndef __HI_CROP_H__
#define __HI_CROP_H__

#ifdef __cplusplus
extern "C" {
#endif 
#include <stdio.h>
#include <pthread.h>
#include "sample_comm.h"
#include "hi_type.h"
#include "hi_comm_vpss.h"
#include "public.h"
#include "common.h"

typedef struct _sdk_zoom_t
{
    int x;
    int y;
    int w;
    int h;
    int ch;
    int full_screen;
}sdk_zoom_t;

HI_S32  ZoomInThread(int ZoomInEn, sdk_zoom_t *pZoom);
#ifdef __cplusplus
}
#endif
#endif
