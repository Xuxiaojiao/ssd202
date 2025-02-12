/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.
  
  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _ST_VPE_H
#define _ST_VPE_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "mi_sys.h"
#include "mi_vpe_datatype.h"
#include "st_vif.h"

typedef struct ST_VPE_ChannelInfo_s
{
    MI_U16 u16VpeMaxW;
    MI_U16 u16VpeMaxH;
    MI_U16 u16VpeCropW;
    MI_U16 u16VpeCropH;
    MI_S32 u32X;
    MI_S32 u32Y;
    MI_SYS_PixelFormat_e eFormat;
    MI_VPE_RunningMode_e eRunningMode;
    MI_BOOL bRot;
} ST_VPE_ChannelInfo_t;

typedef struct ST_VPE_PortInfo_s
{
    MI_VPE_CHANNEL DepVpeChannel;
    MI_U16 u16OutputWidth;                         // Width of target image
    MI_U16 u16OutputHeight;                        // Height of target image
    MI_SYS_PixelFormat_e  ePixelFormat;      // Pixel format of target image
    MI_SYS_CompressMode_e eCompressMode;     // Compression mode of the output
} ST_VPE_PortInfo_t;

//vpe module
MI_S32 ST_Vpe_Init(void);
MI_S32 ST_Vpe_Exit(void);

//vpe channel
MI_S32 ST_Vpe_CreateChannel(MI_VPE_CHANNEL VpeChannel, ST_VPE_ChannelInfo_t *pstChannelInfo);
MI_S32 ST_Vpe_StartChannel(MI_VPE_CHANNEL VpeChannel);
MI_S32 ST_Vpe_StopChannel(MI_VPE_CHANNEL VpeChannel);

//vpe port
MI_S32 ST_Vpe_DestroyChannel(MI_VPE_CHANNEL VpeChannel);
MI_S32 ST_Vpe_CreatePort(MI_VPE_PORT VpePort, ST_VPE_PortInfo_t *pstPortInfo);
MI_S32 ST_Vpe_StopPort(MI_VPE_CHANNEL VpeChannel, MI_VPE_PORT VpePort);

MI_VPE_RunningMode_e ST_Vpe_GetRunModeByWorkMode(VIF_AD_WORK_MODE_E e_WorkMode);

#endif //_ST_VPE_H
