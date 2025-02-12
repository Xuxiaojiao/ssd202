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
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "mi_sys.h"

#include "st_disp.h"

static MI_S32 _ST_Disp_DevInit(ST_DispDevInfo_t *pstDispDevInfo)
{
    MI_DISP_DEV DispDev = pstDispDevInfo->DispDev;
    MI_DISP_PubAttr_t stPubAttr;

    MI_DISP_LAYER DispLayer = pstDispDevInfo->DispLayer;
    MI_U32 u32Toleration = pstDispDevInfo->u32Toleration;
    MI_DISP_VideoLayerAttr_t stLayerAttr;

    STDBG_ENTER();
    memset(&stPubAttr, 0, sizeof(stPubAttr));
    stPubAttr.eIntfSync = pstDispDevInfo->eTiming;
    stPubAttr.eIntfType = pstDispDevInfo->eIntfType;
    ExecFunc(MI_DISP_SetPubAttr(DispDev, &stPubAttr), MI_SUCCESS);
    ExecFunc(MI_DISP_Enable(DispDev), MI_SUCCESS);

    memset(&stLayerAttr, 0, sizeof(stLayerAttr));

    stLayerAttr.stVidLayerSize.u16Width  = pstDispDevInfo->stLayerAttr.stVidLayerSize.u16Width;
    stLayerAttr.stVidLayerSize.u16Height = pstDispDevInfo->stLayerAttr.stVidLayerSize.u16Height;

    stLayerAttr.stVidLayerDispWin.u16X      = pstDispDevInfo->stLayerAttr.stVidLayerDispWin.u16X;
    stLayerAttr.stVidLayerDispWin.u16Y      = pstDispDevInfo->stLayerAttr.stVidLayerDispWin.u16Y;
    stLayerAttr.stVidLayerDispWin.u16Width  = pstDispDevInfo->stLayerAttr.stVidLayerDispWin.u16Width;
    stLayerAttr.stVidLayerDispWin.u16Height = pstDispDevInfo->stLayerAttr.stVidLayerDispWin.u16Height;
    // must before set layer attribute
    ExecFunc(MI_DISP_BindVideoLayer(DispLayer, DispDev), MI_SUCCESS);
    ExecFunc(MI_DISP_SetVideoLayerAttr(DispLayer, &stLayerAttr), MI_SUCCESS);
    ExecFunc(MI_DISP_GetVideoLayerAttr(DispLayer, &stLayerAttr), MI_SUCCESS);
    printf("\n");
    printf("[%s %d]Get Video Layer Size [%d, %d] !!!\n", __FUNCTION__, __LINE__, stLayerAttr.stVidLayerSize.u16Width,
        stLayerAttr.stVidLayerSize.u16Height);
    printf("[%s %d]Get Video Layer DispWin [%d, %d, %d, %d] !!!\n", __FUNCTION__, __LINE__,\
        stLayerAttr.stVidLayerDispWin.u16X, stLayerAttr.stVidLayerDispWin.u16Y,
        stLayerAttr.stVidLayerDispWin.u16Width, stLayerAttr.stVidLayerDispWin.u16Height);
    printf("\n");
    ExecFunc(MI_DISP_EnableVideoLayer(DispLayer), MI_SUCCESS);
    STDBG_LEAVE();

	return MI_SUCCESS;
}

MI_S32 ST_Disp_DevInit(MI_DISP_DEV dispDev, MI_DISP_OutputTiming_e eTiming)
{
    ST_DispDevInfo_t stDispDevInfo;
    ST_DispChnInfo_t stDispChnInfo;

    STDBG_ENTER();

    switch (eTiming)
    {
        case E_MI_DISP_OUTPUT_720P60:
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Width = 1280;
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Height = 720;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16X = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Y = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Width = 1280;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Height = 720;

            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Width = 1280;
            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Height = 720;
            break;
        case E_MI_DISP_OUTPUT_1080P60:
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Width = 1920;
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Height = 1080;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16X = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Y = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Width = 1920;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Height = 1080;

            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Width = 1920;
            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Height = 1080;
            break;
        case E_MI_DISP_OUTPUT_1024x768_60:
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Width = 1024;
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Height = 768;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16X = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Y = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Width = 1024;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Height = 768;

            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Width = 1024;
            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Height = 768;
            break;
        case E_MI_DISP_OUTPUT_1280x1024_60:
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Width = 1280;
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Height = 1024;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16X = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Y = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Width = 1280;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Height = 1024;

            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Width = 1280;
            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Height = 1024;
            break;
        case E_MI_DISP_OUTPUT_1440x900_60:
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Width = 1440;
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Height = 900;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16X = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Y = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Width = 1440;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Height = 900;

            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Width = 1440;
            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Height = 900;
            break;
        case E_MI_DISP_OUTPUT_1600x1200_60:
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Width = 1600;
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Height = 1200;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16X = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Y = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Width = 1600;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Height = 1200;

            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Width = 1600;
            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Height = 1200;
            break;
        case E_MI_DISP_OUTPUT_3840x2160_60:
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Width = 3840;
            stDispDevInfo.stLayerAttr.stVidLayerSize.u16Height = 2160;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16X = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Y = 0;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Width = 3840;
            stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Height = 2160;

            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Width = 3840;
            stDispChnInfo.stInputPortAttr[0].stDispWin.u16Height = 2160;
            break;
        default:
            printf("Unsupported timing!\n");
            return -1;
    }

    stDispDevInfo.eTiming   = eTiming;
    stDispDevInfo.DispDev   = dispDev;
    stDispDevInfo.DispLayer = ST_DISP_LAYER0;
    stDispDevInfo.eIntfType = E_MI_DISP_INTF_HDMI;
    printf("layer size(%d-%d) window(%d-%d-%d-%d)...\n",
        stDispDevInfo.stLayerAttr.stVidLayerSize.u16Width,
        stDispDevInfo.stLayerAttr.stVidLayerSize.u16Height,
        stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16X,
        stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Y,
        stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Width,
        stDispDevInfo.stLayerAttr.stVidLayerDispWin.u16Height);
    STCHECKRESULT(_ST_Disp_DevInit(&stDispDevInfo));

    stDispChnInfo.InputPortNum = DISP_MAX_CHN; //TEST USE 1
    stDispChnInfo.stInputPortAttr[0].stDispWin.u16X = 0;
    stDispChnInfo.stInputPortAttr[0].stDispWin.u16Y = 0;
    STCHECKRESULT(ST_Disp_ChnInit(stDispDevInfo.DispLayer, &stDispChnInfo));
    STDBG_LEAVE();

    return MI_SUCCESS;
}

MI_S32 ST_Disp_ChnInit(MI_DISP_LAYER DispLayer, const ST_DispChnInfo_t *pstDispChnInfo)
{
    MI_S32 s32InputPort = 0;
    MI_S32 InputPortNum = pstDispChnInfo->InputPortNum; //test use 0

    MI_DISP_InputPortAttr_t stInputPortAttr;

    STDBG_ENTER();
    for (s32InputPort = 0; s32InputPort < InputPortNum; s32InputPort++)
    {
        memset(&stInputPortAttr, 0, sizeof(stInputPortAttr));
        ExecFunc(MI_DISP_GetInputPortAttr(DispLayer, s32InputPort, &stInputPortAttr), MI_SUCCESS);
        stInputPortAttr.stDispWin.u16X      = pstDispChnInfo->stInputPortAttr[s32InputPort].stDispWin.u16X;
        stInputPortAttr.stDispWin.u16Y      = pstDispChnInfo->stInputPortAttr[s32InputPort].stDispWin.u16Y;
        stInputPortAttr.stDispWin.u16Width  = pstDispChnInfo->stInputPortAttr[s32InputPort].stDispWin.u16Width;
        stInputPortAttr.stDispWin.u16Height = pstDispChnInfo->stInputPortAttr[s32InputPort].stDispWin.u16Height;

        ExecFunc(MI_DISP_SetInputPortAttr(DispLayer, s32InputPort, &stInputPortAttr), MI_SUCCESS);
        ExecFunc(MI_DISP_GetInputPortAttr(DispLayer, s32InputPort, &stInputPortAttr), MI_SUCCESS);
        ExecFunc(MI_DISP_EnableInputPort(DispLayer, s32InputPort), MI_SUCCESS);
    }
    STDBG_LEAVE();

	return MI_SUCCESS;
}

MI_S32 ST_Disp_DeInit(MI_DISP_DEV DispDev, MI_DISP_LAYER DispLayer, MI_S32 s32InputPortNum)
{
    MI_S32 s32InputPort = 0;
    for (s32InputPort = 0; s32InputPort < s32InputPortNum; s32InputPort++)
    {
        ExecFunc(MI_DISP_DisableInputPort(DispLayer, s32InputPort), MI_SUCCESS);
    }
    ExecFunc(MI_DISP_DisableVideoLayer(DispLayer), MI_SUCCESS);
    ExecFunc(MI_DISP_UnBindVideoLayer(DispLayer, DispDev), MI_SUCCESS);
    ExecFunc(MI_DISP_Disable(DispDev), MI_SUCCESS);

    return MI_SUCCESS;
}
