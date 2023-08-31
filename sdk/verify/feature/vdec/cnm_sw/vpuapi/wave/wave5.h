//--=========================================================================--
//  This file is a part of VPU Reference API project
//-----------------------------------------------------------------------------
//
//       This confidential and proprietary software may be used only
//     as authorized by a licensing agreement from Chips&Media Inc.
//     In the event of publication, the following notice is applicable:
//
//            (C) COPYRIGHT 2006 - 2011  CHIPS&MEDIA INC.
//                      ALL RIGHTS RESERVED
//
//       The entire notice above must be reproduced on all authorized
//       copies.
//
//--=========================================================================--
#ifndef __WAVE5_FUNCTION_H__
#define __WAVE5_FUNCTION_H__

#include "vpuapi.h"
#include "product.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define WAVE5_SUBSAMPLED_ONE_SIZE(_w, _h)       ((((_w/4)+15)&~15)*(((_h/4)+7)&~7))
#define WAVE5_AVC_SUBSAMPLED_ONE_SIZE(_w, _h)   ((((_w/4)+31)&~31)*(((_h/4)+3)&~3))

#define BSOPTION_ENABLE_EXPLICIT_END        (1<<0)

#define WTL_RIGHT_JUSTIFIED          0
#define WTL_LEFT_JUSTIFIED           1
#define WTL_PIXEL_8BIT               0
#define WTL_PIXEL_16BIT              1
#define WTL_PIXEL_32BIT              2

extern Uint32 Wave5VpuIsInit(
    Uint32      coreIdx
    );

extern Int32 Wave5VpuIsBusy(
    Uint32 coreIdx
    );

extern Int32 WaveVpuGetProductId(
    Uint32      coreIdx
    );

extern RetCode Wave5VpuEncGiveCommand(
    CodecInst *pCodecInst,
    CodecCommand cmd,
    void *param
    );

extern void Wave5BitIssueCommand(
    CodecInst* instance,
    Uint32 cmd
    );

extern RetCode Wave5VpuGetVersion(
    Uint32  coreIdx,
    Uint32* versionInfo,
    Uint32* revision
    );

extern RetCode Wave5VpuInit(
    Uint32      coreIdx,
    void*       firmware,
    Uint32      size
    );

extern RetCode Wave5VpuSleepWake(
    Uint32 coreIdx,
    int iSleepWake,
    const Uint16* code,
    Uint32 size,
    BOOL reset
    );

extern RetCode Wave5VpuReset(
    Uint32 coreIdx,
    SWResetMode resetMode
    );

extern RetCode Wave5VpuBuildUpDecParam(
    CodecInst* instance,
    DecOpenParam* param
    );

extern RetCode Wave5VpuDecSetBitstreamFlag(
    CodecInst* instance,
    BOOL running,
    BOOL eos,
    BOOL explictEnd
    );

extern RetCode Wave5VpuDecRegisterFramebuffer(
    CodecInst* inst,
    FrameBuffer* fbArr,
    TiledMapType mapType,
    Uint32 count
    );

extern RetCode Wave5VpuDecUpdateFramebuffer(
    CodecInst*      inst,
    FrameBuffer*    fbcFb,
    FrameBuffer*    linearFb,
    Int32           mvIndex,
    Int32           picWidth,
    Int32           picHeight
    );

extern RetCode Wave5VpuDecGetVlcInfo(
    CodecInst* instance, 
    VLCBufInfo* vlcInfo
    );

extern RetCode Wave5VpuDecUpdateVlcInfo(
    CodecInst* instance, 
    VLCBufInfo* vlcInfo
    );

extern RetCode Wave5VpuDecFlush(
    CodecInst* instance,
    FramebufferIndex* framebufferIndexes,
    Uint32 size
    );

extern RetCode Wave5VpuReInit(
    Uint32 coreIdx,
    void* firmware,
    Uint32 size
    );

extern RetCode Wave5VpuDecInitSeq(
    CodecInst* instance
    );

extern RetCode Wave5VpuDecGetSeqInfo(
    CodecInst* instance,
    DecInitialInfo* info
    );

extern RetCode Wave5VpuDecode(
    CodecInst* instance,
    DecParam* option
    );

extern RetCode Wave5VpuDecGetResult(
    CodecInst* instance,
    DecOutputInfo* result
    );

extern RetCode Wave5VpuDecFiniSeq(
    CodecInst* instance
    );

extern RetCode Wave5DecWriteProtect(
    CodecInst* instance
    );

extern RetCode Wave5DecClrDispFlag(
    CodecInst* instance,
    Uint32 index
    );

extern RetCode Wave5DecSetDispFlag(
    CodecInst* instance,
    Uint32 index
    );

extern Int32 Wave5VpuWaitInterrupt(
    CodecInst*  instance,
    Int32       timeout,
    BOOL        pending
    );

extern RetCode Wave5VpuClearInterrupt(
    Uint32 coreIdx,
    Uint32 flags
    );

extern RetCode Wave5VpuGetProductInfo(
    Uint32          coreIdx,
    ProductInfo*    productInfo
    );

extern RetCode Wave5VpuDecGetRdPtr(
    CodecInst* instance,
    PhysicalAddress *rdPtr
    );
#ifdef FIX_SET_GET_RD_PTR_BUG
extern RetCode Wave5VpuDecSetRdPtr(
	CodecInst* instance, 
	PhysicalAddress rdPtr
	);
#endif

extern RetCode Wave5VpuGetBwReport(
    CodecInst*  instance,
    VPUBWData*  bwMon
    );


/***< WAVE5 Encoder >******/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WAVE5_FUNCTION_H__ */

