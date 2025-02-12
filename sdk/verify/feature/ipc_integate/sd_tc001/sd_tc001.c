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
#include <pthread.h>
#include <sys/prctl.h>
#include <poll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include "mi_sys.h"
#include "mi_sd.h"
#include "../common/st_common.h"
#include "../common/st_vpe.h"
#include "../common/st_vif.h"
#define BUFNUM 10
#ifndef ASSERT
#define ASSERT(_x_)                                                                         \
    do  {                                                                                   \
        if ( ! ( _x_ ) )                                                                    \
        {                                                                                   \
            printf("ASSERT FAIL: %s %s %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);     \
            abort();                                                                        \
        }                                                                                   \
    } while (0)
#endif

typedef struct Test_Parames_s
{
    int cnt;
    const char *pDestFile;
    MI_U16 u16PortId ;
    MI_U16 u16InWidth;
    MI_U16 u16InHeight; 
    MI_SYS_PixelFormat_e eInPixelFormat;
    
    MI_U16 u16OutWidth;
    MI_U16 u16OutHeight; 
    MI_SYS_PixelFormat_e eOutPixelFormat;
    int sd;
}Test_Parames_t;

static MI_S32 St_WriteFile(Test_Parames_t     *pstParamer)
{
    MI_SYS_ChnPort_t stSDChnOutputPort;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE hHandle;
    MI_U16  u16BufInfoStride =0;
    MI_U16  u16BufInfoHeight =0;
    MI_U32  u32FrameSize =0;
    int offset =0;
    MI_U8 u8BufNum = BUFNUM;

    if(pstParamer->cnt > u8BufNum)
        pstParamer->cnt = u8BufNum;
    
    stSDChnOutputPort.eModId = E_MI_MODULE_ID_SD;
    stSDChnOutputPort.u32DevId = 0;
    stSDChnOutputPort.u32ChnId = 0;
    stSDChnOutputPort.u32PortId = 0;

    do{
        if (MI_SUCCESS == MI_SYS_ChnOutputPortGetBuf(&stSDChnOutputPort , &stBufInfo,&hHandle))
        {
            // Add user write buffer to file
            u16BufInfoStride  = stBufInfo.stFrameData.u32Stride[0];
            u16BufInfoHeight = stBufInfo.stFrameData.u16Height;
            u32FrameSize = u16BufInfoStride*u16BufInfoHeight;
            // put frame
            printf("getbuf sucess, size(%dx%d), stride(%d, %d, %d), Pixel %d, viraddr(%p, %p, %p)\n", stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height,
            stBufInfo.stFrameData.u32Stride[0], stBufInfo.stFrameData.u32Stride[1], stBufInfo.stFrameData.u32Stride[2], stBufInfo.stFrameData.ePixelFormat,
            stBufInfo.stFrameData.pVirAddr[0], stBufInfo.stFrameData.pVirAddr[1], stBufInfo.stFrameData.pVirAddr[2]);

            if(pstParamer->cnt >= u8BufNum)
            {
                printf("begin write file\n");
                STCHECKRESULT(ST_Write_OneFrame(pstParamer->sd, offset, stBufInfo.stFrameData.pVirAddr[0], u16BufInfoStride, u16BufInfoStride, u16BufInfoHeight));
                offset += u32FrameSize;
                MI_SYS_FlushInvCache(stBufInfo.stFrameData.pVirAddr[0], u32FrameSize);

                if(pstParamer->eOutPixelFormat == E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420)
                {
                    STCHECKRESULT(ST_Write_OneFrame(pstParamer->sd, offset, stBufInfo.stFrameData.pVirAddr[1], u16BufInfoStride, u16BufInfoStride, u16BufInfoHeight/2));
                    offset += u32FrameSize/2;
                    MI_SYS_FlushInvCache(stBufInfo.stFrameData.pVirAddr[1], u32FrameSize/2);
                }
            }
            STCHECKRESULT(MI_SYS_ChnOutputPortPutBuf(hHandle));
            printf("put buf %d done\n", u8BufNum);
            u8BufNum--;
        }
    }while(u8BufNum > 0);

    return MI_SUCCESS;
}

static MI_S32 St_BaseModuleInit(Test_Parames_t     *pstParamer)
{
    MI_SYS_ChnPort_t stChnPort;
    ST_Sys_BindInfo_t stBindInfo;
    ST_VIF_PortInfo_t stVifPortInfoInfo;
    ST_VPE_ChannelInfo_t stVpeChannelInfo;
    ST_VPE_PortInfo_t stPortInfo;
    
    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_t));
    memset(&stVifPortInfoInfo, 0x0, sizeof(ST_VIF_PortInfo_t));
    memset(&stVpeChannelInfo, 0x0, sizeof(ST_VPE_ChannelInfo_t));
    memset(&stPortInfo, 0x0, sizeof(ST_VPE_PortInfo_t));
    
    /************************************************
    Step1:  init VIF
    *************************************************/
    STCHECKRESULT(ST_Sys_Init());
    STCHECKRESULT(ST_Vif_CreateDev(0, SAMPLE_VI_MODE_MIPI_1_1080P_REALTIME));
    stVifPortInfoInfo.u32RectX = 0;
    stVifPortInfoInfo.u32RectY = 0;
    stVifPortInfoInfo.u32RectWidth = 1920;
    stVifPortInfoInfo.u32RectHeight = 1080;
    stVifPortInfoInfo.u32DestWidth = 1920;
    stVifPortInfoInfo.u32DestHeight = 1080;
    stVifPortInfoInfo.ePixFormat = E_MI_SYS_PIXEL_FRAME_RGB_BAYER_12BPP_RG;
    STCHECKRESULT(ST_Vif_CreatePort(0, 0, &stVifPortInfoInfo));
    printf("vif port(%dx%d), pixel %d\n", stVifPortInfoInfo.u32DestWidth, stVifPortInfoInfo.u32DestHeight, stVifPortInfoInfo.ePixFormat);
    /************************************************
    Step4:  init VPE
    *************************************************/
    stVpeChannelInfo.u16VpeMaxW = 1920;
    stVpeChannelInfo.u16VpeMaxH = 1080;
    stVpeChannelInfo.u32X = 0;
    stVpeChannelInfo.u32Y = 0;
    stVpeChannelInfo.u16VpeCropW = 0;
    stVpeChannelInfo.u16VpeCropH = 0;
    stVpeChannelInfo.eRunningMode = E_MI_VPE_RUN_REALTIME_MODE;
    stVpeChannelInfo.eFormat = E_MI_SYS_PIXEL_FRAME_RGB_BAYER_12BPP_RG;
    STCHECKRESULT(ST_Vpe_CreateChannel(0, &stVpeChannelInfo));
    STCHECKRESULT(ST_Vpe_StartChannel(0));
    stPortInfo.DepVpeChannel = 0;
    stPortInfo.ePixelFormat = pstParamer->eInPixelFormat;
    stPortInfo.u16OutputWidth = pstParamer->u16InWidth;
    stPortInfo.u16OutputHeight = pstParamer->u16InHeight;
    STCHECKRESULT(ST_Vpe_CreatePort(pstParamer->u16PortId, &stPortInfo)); //default support port0 --->>> vdisp
    printf("vpe port%d, (%dx%d), pixel %d, runmode %d\n", pstParamer->u16PortId, stPortInfo.u16OutputWidth, 
        stPortInfo.u16OutputHeight, stPortInfo.ePixelFormat, stVpeChannelInfo.eRunningMode);
    
    /************************************************
    Step4:  init SD
    *************************************************/
    MI_SD_ChannelAttr_t  stSdChnlAttr;
    MI_SD_OuputPortAttr_t stSdPortMode;
    memset(&stSdPortMode, 0x0, sizeof(stSdPortMode));
    memset(&stSdChnlAttr, 0x0, sizeof(stSdChnlAttr));
    MI_SD_CHANNEL SdChannel = 0;

    stSdChnlAttr.u16MaxW = 1920;
    stSdChnlAttr.u16MaxH = 1080;
    MI_SD_CreateChannel(SdChannel, &stSdChnlAttr);

    stSdPortMode.ePixelFormat = pstParamer->eOutPixelFormat;
    stSdPortMode.u16Width = pstParamer->u16OutWidth;
    stSdPortMode.u16Height = pstParamer->u16OutHeight;
    MI_SD_SetOutputPortAttr(SdChannel, &stSdPortMode);
    MI_SD_StartChannel(SdChannel);
    
    stChnPort.eModId = E_MI_MODULE_ID_SD;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = SdChannel;
    stChnPort.u32PortId = 0;
    MI_SYS_SetChnOutputPortDepth(&stChnPort, 2, 5);

    STCHECKRESULT(ST_Vif_StartPort(0, 0));
    /************************************************
    Step7:  Bind VIF->VPE
    *************************************************/
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_t));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = 0; //VIF dev == 0
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0; //Main stream
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    /************************************************
    Step7:  Bind VPE->SD
    *************************************************/
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_t));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0; //VIF dev == 0
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = pstParamer->u16PortId; //Main stream
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SD;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    return MI_SUCCESS;
}

static MI_S32 St_BaseModuleDeinit(Test_Parames_t     * pstParamer)
{
    ST_Sys_BindInfo_t stBindInfo;
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_t));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = 0; //VIF dev == 0
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0; //Main stream
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_t));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VPE;
    stBindInfo.stSrcChnPort.u32DevId = 0; //VIF dev == 0
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = pstParamer->u16PortId; //Main stream
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SD;
    stBindInfo.stDstChnPort.u32DevId = 0;
    stBindInfo.stDstChnPort.u32ChnId = 0;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    STCHECKRESULT(ST_Vpe_StopPort(0, pstParamer->u16PortId));
    STCHECKRESULT(ST_Vpe_StopChannel(0));
    STCHECKRESULT(ST_Vpe_DestroyChannel(0));
    STCHECKRESULT(ST_Vif_StopPort(0, 0));
    STCHECKRESULT(ST_Vif_DisableDev(0));
    STCHECKRESULT(MI_SD_StopChannel(0));
    STCHECKRESULT(MI_SD_DestroyChannel(0));
    
    STCHECKRESULT(ST_Sys_Exit());
    return MI_SUCCESS;
}

int checkParame(Test_Parames_t     *pstParamer)
{
    int cnt =pstParamer->cnt;
    const char *pDestFile = pstParamer->pDestFile;
    MI_U16 u16Width=pstParamer->u16InWidth, u16Height=pstParamer->u16InHeight, u16PortId =pstParamer->u16PortId;
    MI_SYS_PixelFormat_e ePixelFormat = pstParamer->eInPixelFormat;

    if(u16PortId <0 || u16PortId > 3)
    {
        printf("portid %d err\n", u16PortId);
        return 1;
    }

    if(u16Width < 0 || u16Height < 0)
    {
        printf("width %d, height %d err \n", u16Width, u16Height);
        return 1;
    }

    if(ePixelFormat != E_MI_SYS_PIXEL_FRAME_YUV422_YUYV 
        && ePixelFormat != E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_422
        && ePixelFormat != E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420)
    {
        printf("pixel %d err \n", ePixelFormat);
        return 1;
    }

    if(pDestFile == NULL || cnt ==0)
    {
        printf("destfill is %p, cnt %d \n", pDestFile, cnt);
        return 1;
    }

    return MI_SUCCESS;
}

MI_S32 main(int argc, char **argv)
{
    Test_Parames_t  stParamer;
    memset(&stParamer, 0x0, sizeof(Test_Parames_t));
    
    if(argc < 7)
    {
       printf("paramer cnt %d < 7 \n", argc);
       printf("portid[1], Inwidth[2], Inheight[3], Inpixel[4](YUV422:0, YUV420:10, YUV422SP:9), outwidth[5], outHeight[6], outPixel[7],filepath[8], cnt[9]\n");
       return 1;
    }
    
    stParamer.u16PortId = atoi(argv[1]);
    stParamer.u16InWidth = atoi(argv[2]);
    stParamer.u16InHeight = atoi(argv[3]);
    stParamer.eInPixelFormat = atoi(argv[4]);
    stParamer.u16OutWidth = atoi(argv[5]);
    stParamer.u16OutHeight = atoi(argv[6]);
    stParamer.eOutPixelFormat = atoi(argv[7]);
    stParamer.pDestFile = argv[8];
    stParamer.cnt = atoi(argv[9]);

    printf("portid %d, width %d, height %d, pixel %d, filename %s, cnt %d\n", stParamer.u16PortId, 
        stParamer.u16InWidth, stParamer.u16InHeight,stParamer.eInPixelFormat, stParamer.pDestFile,stParamer.cnt);
    
    STCHECKRESULT(checkParame(&stParamer));
    STCHECKRESULT(ST_OpenDestFile(stParamer.pDestFile, &stParamer.sd));
    STCHECKRESULT(St_BaseModuleInit(&stParamer));
    
    STCHECKRESULT(St_WriteFile(&stParamer));
    STCHECKRESULT(St_BaseModuleDeinit(&stParamer));
    return 0;
}
