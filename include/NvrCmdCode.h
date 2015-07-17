#ifndef __NVR_CMD_CODE_H__
#define __NVR_CMD_CODE_H___

#ifdef __cplusplus
extern "C" {
#endif 

//以下命令的处理流程是QT等控制界面先通过server再转发到vdec模块
#define CMD_VDEC_SERVER_BASE   0x01A0
#define CMD_VDEC_CTRL_PLAY       (CMD_VDEC_SERVER_BASE + 0x01) // 播放,分辨率
#define CMD_VDEC_CTRL_STOP       (CMD_VDEC_SERVER_BASE + 0x02) // 停止

#define CMD_PLAY_MODE_PLAYBACK   (CMD_VDEC_SERVER_BASE + 0x03) // 播放模式 -- 回放
#define CMD_PLAY_MODE_PREVIEW    (CMD_VDEC_SERVER_BASE + 0x04) // 播放模式 -- 实时预览 

//以下命令的处理流程是QT等控制界面直接发送到vdec模块
#define CMD_VDEC_BASE   0x01B0
#define CMD_VDEC_DISPLAY_MODE     (CMD_VDEC_BASE + 0x01) // 分屏模式，也就是几分屏




#ifdef __cplusplus
}
#endif
#endif
