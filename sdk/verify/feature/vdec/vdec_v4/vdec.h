#ifndef __DEMO_VDEC_V4_H_
#define __DEMO_VDEC_V4_H_

#include "mi_vdec.h"

#define MAX_CHANNEL_NUM (32)
typedef struct Vdec_Config_s
{
    MI_U8 au8CaseName[256];
    MI_U8 au8BasePath[256];
    MI_BOOL bDumpEs;
    MI_U32 u32RetryCount;
    MI_BOOL bPushStreamThread;
    MI_U16 u16ChnNum;
    struct Vdec_Attr_s
    {
        MI_U8 au8InputEsName[256];
        MI_U8 au8OuputYuvName[256];
        MI_U8 u8ChnnelId;
        MI_U8 u8CodecType;
        MI_U8 u8CmaBufNum;
        MI_U32 u32BsBufSize;
        MI_U32 u32framenum;
        MI_U8 u8TileMode;
        MI_U16 u16ScalingWidth;
        MI_U16 u16ScalingHeight;
        MI_U8 u8InturreptMode;
        MI_U8 u8SkipFrameDecode;
        MI_U8 u8ChnnelToBeCreate;
        MI_U8 u8ParamLv;
        MI_U16 u16CropX;
        MI_U16 u16CropY;
        MI_U16 u16CropW;
        MI_U16 u16CropH;
        MI_U8 au8CheckMD5[33];
    } stVdecAttr[MAX_CHANNEL_NUM];
    /// end of json define

    MI_U32 u32ExecTimeout;
} Vdec_Config_t;

#endif
