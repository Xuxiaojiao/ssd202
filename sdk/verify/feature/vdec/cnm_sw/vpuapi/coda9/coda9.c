//--=========================================================================--
//  This file is a part of VPU Reference API project
//-----------------------------------------------------------------------------
//
//       This confidential and proprietary software may be used only
//     as authorized by a licensing agreement from Chips&Media Inc.
//     In the event of publication, the following notice is applicable:
//
//            (C) COPYRIGHT 2006 - 2013  CHIPS&MEDIA INC.
//                      ALL RIGHTS RESERVED
//
//       The entire notice above must be reproduced on all authorized
//       copies.
//
//--=========================================================================--
#include "coda9_vpuconfig.h"
#include "product.h"
#include "coda9_regdefine.h"

static void LoadBitCode(Uint32 coreIdx, PhysicalAddress codeBase, const Uint16 *codeWord, int codeSize)
{
    int i;
    BYTE code[8];

    for (i=0; i<codeSize; i+=4) {
        // 2byte little endian variable to 1byte big endian buffer
        code[0] = (BYTE)(codeWord[i+0]>>8);
        code[1] = (BYTE)codeWord[i+0];
        code[2] = (BYTE)(codeWord[i+1]>>8);
        code[3] = (BYTE)codeWord[i+1];
        code[4] = (BYTE)(codeWord[i+2]>>8);
        code[5] = (BYTE)codeWord[i+2];
        code[6] = (BYTE)(codeWord[i+3]>>8);
        code[7] = (BYTE)codeWord[i+3];
        VpuWriteMem(coreIdx, codeBase+i*2, (BYTE *)code, 8, VDI_BIG_ENDIAN);
    }

    vdi_set_bit_firmware_to_pm(coreIdx, codeWord); 
}

static RetCode BitLoadFirmware(Uint32 coreIdx, PhysicalAddress codeBase, const Uint16 *codeWord, int codeSize)
{
    int i;
    Uint32 data;

    LoadBitCode(coreIdx, codeBase, codeWord, codeSize);

    VpuWriteReg(coreIdx, BIT_INT_ENABLE, 0);
    VpuWriteReg(coreIdx, BIT_CODE_RUN, 0);
    for (i=0; i<2048; ++i) {
        data = codeWord[i];
        VpuWriteReg(coreIdx, BIT_CODE_DOWN, (i << 16) | data);
    }
    return RETCODE_SUCCESS;
}


void Coda9BitIssueCommand(Uint32 coreIdx, CodecInst *inst, int cmd)
{
    int instIdx = 0;
    int cdcMode = 0;
    int auxMode = 0;


    if (inst != NULL) // command is specific to instance
    {
        instIdx = inst->instIndex;
        cdcMode = inst->codecMode;
        auxMode = inst->codecModeAux;
    }


    if (inst) {
        if (inst->codecMode < AVC_ENC)    {
            VpuWriteReg(coreIdx, BIT_WORK_BUF_ADDR, inst->CodecInfo->decInfo.vbWork.phys_addr);        
        }
    }    

    VpuWriteReg(coreIdx, BIT_BUSY_FLAG, 1);
    VpuWriteReg(coreIdx, BIT_RUN_INDEX, instIdx);
    VpuWriteReg(coreIdx, BIT_RUN_COD_STD, cdcMode);
    VpuWriteReg(coreIdx, BIT_RUN_AUX_STD, auxMode);
    if (inst && inst->loggingEnable)
        vdi_log(coreIdx, cmd, 1);
    VpuWriteReg(coreIdx, BIT_RUN_COMMAND, cmd);
}

static void SetupCoda9Properties(Uint32 coreIdx, Uint32 productId)
{
    VpuAttr*    pAttr = &g_VpuCoreAttributes[coreIdx];
    Int32       val;
    char*       pstr;

    /* Setup Attributes */
    pAttr = &g_VpuCoreAttributes[coreIdx];

    // Hardware version information
    val = VpuReadReg(coreIdx, VPU_PRODUCT_CODE_REGISTER);
    val = DEFAULT_TYPE_CODE;
    if ((val&0xff00) == 0x3200) val = 0x3200;
    val = VpuReadReg(coreIdx, DBG_CONFIG_REPORT_0);
    pstr = (char*)&val;
    pAttr->productName[0] = pstr[3];
    pAttr->productName[1] = pstr[2];
    pAttr->productName[2] = pstr[1];
    pAttr->productName[3] = pstr[0];
    pAttr->productName[4] = 0;

    pAttr->supportDecoders = (1<<STD_AVC)   |
        (1<<STD_VC1)   |
        (1<<STD_MPEG2) |
        (1<<STD_MPEG4) |
        (1<<STD_H263)  |
        (1<<STD_AVS)   |
        (1<<STD_DIV3)  | 
        (1<<STD_RV)    |
        (1<<STD_THO)   |
        (1<<STD_VP8);

    pAttr->supportEncoders = 0;

    /* WTL */
    if (productId == PRODUCT_ID_960 || productId == PRODUCT_ID_980) {
        pAttr->supportWTL = 1;
    }
    /* Tiled2Linear */
    pAttr->supportTiled2Linear = 1;
    /* Maptypes */
    pAttr->supportMapTypes = (1<<LINEAR_FRAME_MAP)           |
        (1<<TILED_FRAME_V_MAP)          |
        (1<<TILED_FRAME_H_MAP)          |
        (1<<TILED_FIELD_V_MAP)          |
        (1<<TILED_MIXED_V_MAP)          |
        (1<<TILED_FRAME_MB_RASTER_MAP)  |
        (1<<TILED_FIELD_MB_RASTER_MAP);
    if (productId == PRODUCT_ID_980) {
        pAttr->supportMapTypes |= (1<<TILED_FRAME_NO_BANK_MAP) |
            (1<<TILED_FIELD_NO_BANK_MAP);
    }
    /* Linear2Tiled */
    if (productId == PRODUCT_ID_960 || productId == PRODUCT_ID_980) {
        pAttr->supportLinear2Tiled = 1;
    }
    /* Framebuffer Cache */
    if (productId == PRODUCT_ID_960)
        pAttr->framebufferCacheType = FramebufCacheMaverickI;
    else if (productId == PRODUCT_ID_980)
        pAttr->framebufferCacheType = FramebufCacheMaverickII;
    else 
        pAttr->framebufferCacheType = FramebufCacheNone;
    /* AXI 128bit Bus */
    pAttr->support128bitBus       = FALSE;
    pAttr->supportEndianMask      = (1<<VDI_LITTLE_ENDIAN) | (1<<VDI_BIG_ENDIAN) | (1<<VDI_32BIT_LITTLE_ENDIAN) | (1<<VDI_32BIT_BIG_ENDIAN);
    pAttr->supportBitstreamMode   = (1<<BS_MODE_INTERRUPT) | (1<<BS_MODE_PIC_END); 
    pAttr->bitstreamBufferMargin  = VPU_GBU_SIZE;
    pAttr->numberOfMemProtectRgns = 6;
}

Uint32 Coda9VpuGetProductId(Uint32 coreIdx)
{
    Uint32  productId;
    Uint32  val;

    val = VpuReadReg(coreIdx, VPU_PRODUCT_CODE_REGISTER);
    val = DEFAULT_TYPE_CODE;

    if (val == BODA950_CODE)
        productId = PRODUCT_ID_950;
    else if (val == CODA960_CODE) 
        productId = PRODUCT_ID_960;
    else if (val == CODA980_CODE) 
        productId = PRODUCT_ID_980; 
    else 
        productId = PRODUCT_ID_NONE;

    if (productId != PRODUCT_ID_NONE) 
        SetupCoda9Properties(coreIdx, productId);

    return productId;
}

RetCode Coda9VpuGetVersion(Uint32 coreIdx, Uint32* versionInfo, Uint32* revision)
{
    /* Get Firmware version */
    VpuWriteReg(coreIdx, RET_FW_VER_NUM , 0);
    Coda9BitIssueCommand(coreIdx, NULL, FIRMWARE_GET);
    if (vdi_wait_vpu_busy(coreIdx, __VPU_BUSY_TIMEOUT, BIT_BUSY_FLAG) == -1) 
        return RETCODE_VPU_RESPONSE_TIMEOUT;

    if (versionInfo != NULL) {
        *versionInfo = VpuReadReg(coreIdx, RET_FW_VER_NUM);
    }
    if (revision != NULL) {
        *revision    = VpuReadReg(coreIdx, RET_FW_CODE_REV);
    }

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuInit(Uint32 coreIdx, void* firmware, Uint32 size)
{
    Uint32          data;
    vpu_buffer_t    vb;
    PhysicalAddress tempBuffer;
    PhysicalAddress paraBuffer;
    PhysicalAddress codeBuffer;

    vdi_get_common_memory((unsigned long)coreIdx, &vb);

    codeBuffer = vb.phys_addr;
    tempBuffer = codeBuffer + CODE_BUF_SIZE;
    paraBuffer = tempBuffer + TEMP_BUF_SIZE;

    BitLoadFirmware(coreIdx, codeBuffer, (const Uint16 *)firmware, size);

    /* Clear registers */
    if (vdi_read_register(coreIdx, BIT_CUR_PC) == 0) {
        Uint32 i;
        for (i=0; i<64; i++) {
            vdi_write_register(coreIdx, (i*4) + 0x100, 0x0); 
        }
    }

    VpuWriteReg(coreIdx, BIT_PARA_BUF_ADDR, paraBuffer);
    VpuWriteReg(coreIdx, BIT_CODE_BUF_ADDR, codeBuffer);
    VpuWriteReg(coreIdx, BIT_TEMP_BUF_ADDR, tempBuffer);

    VpuWriteReg(coreIdx, BIT_BIT_STREAM_CTRL, VPU_STREAM_ENDIAN);
    VpuWriteReg(coreIdx, BIT_FRAME_MEM_CTRL, CBCR_INTERLEAVE<<2|VPU_FRAME_ENDIAN); // Interleave bit position is modified
    VpuWriteReg(coreIdx, BIT_BIT_STREAM_PARAM, 0);

    VpuWriteReg(coreIdx, BIT_AXI_SRAM_USE, 0);
    VpuWriteReg(coreIdx, BIT_INT_ENABLE, 0);
    VpuWriteReg(coreIdx, BIT_ROLLBACK_STATUS, 0);   

    data = (1<<INT_BIT_BIT_BUF_FULL);
    data |= (1<<INT_BIT_BIT_BUF_EMPTY);
    data |= (1<<INT_BIT_DEC_MB_ROWS);
    data |= (1<<INT_BIT_SEQ_INIT);
    data |= (1<<INT_BIT_DEC_FIELD);
    data |= (1<<INT_BIT_PIC_RUN);

    VpuWriteReg(coreIdx, BIT_INT_ENABLE, data);
    VpuWriteReg(coreIdx, BIT_INT_CLEAR,  0x1);
    VpuWriteReg(coreIdx, BIT_BUSY_FLAG,  0x1);
    VpuWriteReg(coreIdx, BIT_CODE_RESET, 1);
    VpuWriteReg(coreIdx, BIT_CODE_RUN,   1);

    if (vdi_wait_vpu_busy(coreIdx, __VPU_BUSY_TIMEOUT, BIT_BUSY_FLAG) == -1) 
        return RETCODE_VPU_RESPONSE_TIMEOUT;

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuReInit(Uint32 coreIdx, void* firmware, Uint32 size)
{
    vpu_buffer_t    vb;
    PhysicalAddress tempBuffer;
    PhysicalAddress paraBuffer;
    PhysicalAddress codeBuffer;
    PhysicalAddress oldCodeBuffer;

    vdi_get_common_memory((unsigned long)coreIdx, &vb);

    codeBuffer = vb.phys_addr;
    tempBuffer = codeBuffer + CODE_BUF_SIZE;
    paraBuffer = tempBuffer + TEMP_BUF_SIZE;

    oldCodeBuffer = VpuReadReg(coreIdx, BIT_CODE_BUF_ADDR);

    VpuWriteReg(coreIdx, BIT_PARA_BUF_ADDR, paraBuffer);
    VpuWriteReg(coreIdx, BIT_CODE_BUF_ADDR, codeBuffer);
    VpuWriteReg(coreIdx, BIT_TEMP_BUF_ADDR, tempBuffer);

    if (oldCodeBuffer != codeBuffer) {
        LoadBitCode(coreIdx, codeBuffer, (const Uint16 *)firmware, size);
    }

    return RETCODE_SUCCESS;
}

Uint32 Coda9VpuIsInit(Uint32 coreIdx)
{
    Uint32 pc;

    pc = VpuReadReg(coreIdx, BIT_CUR_PC);

    return pc;
}

Int32 Coda9VpuIsBusy(Uint32 coreIdx)
{
    return VpuReadReg(coreIdx, BIT_BUSY_FLAG);
}

Int32 Coda9VpuWaitInterrupt(CodecInst* handle, Int32 timeout)
{
	Int32 reason = 0;
#ifdef SUPPORT_MULTI_INST_INTR
	reason = vdi_wait_interrupt(handle->coreIdx, 0, timeout);
#else
	reason = vdi_wait_interrupt(handle->coreIdx, timeout);
#endif

	if (reason != INTERRUPT_TIMEOUT_VALUE) {
		VpuWriteReg(handle->coreIdx, BIT_INT_CLEAR, 1);		// clear HW signal				
	}

	return reason;
}
RetCode Coda9VpuClearInterrupt(Uint32 coreIdx)
{
    VpuWriteReg(coreIdx, BIT_INT_REASON, 0);     // tell to F/W that HOST received an interrupt.

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuReset(Uint32 coreIdx, SWResetMode resetMode)
{
    Uint32 cmd;
    Int32  productId = Coda9VpuGetProductId(coreIdx);

    if (productId == PRODUCT_ID_960 || productId == PRODUCT_ID_980) {

		if (resetMode != SW_RESET_ON_BOOT) {
			cmd = VpuReadReg(coreIdx, BIT_RUN_COMMAND);
			if (cmd == DEC_SEQ_INIT || cmd == PIC_RUN) {
				if (VpuReadReg(coreIdx, BIT_BUSY_FLAG) || VpuReadReg(coreIdx, BIT_INT_REASON)) {
#define MBC_SET_SUBBLK_EN (MBC_BASE+0xA0) // subblk_man_mode[20] cr_subblk_man_en[19:0]
					// stop all of pipeline
					VpuWriteReg(coreIdx, MBC_SET_SUBBLK_EN, ((1<<20) | 0)); 

				// force to set the end of Bitstream to be decoded. 
					cmd = VpuReadReg(coreIdx, BIT_BIT_STREAM_PARAM);
					cmd |= 1 << 2;			
					VpuWriteReg(coreIdx, BIT_BIT_STREAM_PARAM, cmd);

					cmd = VpuReadReg(coreIdx, BIT_RD_PTR);
					VpuWriteReg(coreIdx, BIT_WR_PTR, cmd);

#ifdef SUPPORT_MULTI_INST_INTR
					cmd = vdi_wait_interrupt(coreIdx, 0, __VPU_BUSY_TIMEOUT);
#else
					cmd = vdi_wait_interrupt(coreIdx, __VPU_BUSY_TIMEOUT);
#endif

					if ((Int32)cmd != INTERRUPT_TIMEOUT_VALUE) {
						VpuWriteReg(coreIdx, BIT_INT_REASON, 0);
						VpuWriteReg(coreIdx, BIT_INT_CLEAR, 1);		// clear HW signal				
					}					
					// now all of hardwares would be stop.	
				}
			}	
		}
			
        // Waiting for completion of BWB transaction first 
		if (vdi_wait_vpu_busy(coreIdx, __VPU_BUSY_TIMEOUT, GDI_BWB_STATUS) == -1) {
            vdi_log(coreIdx, 0x10, 2);
            return RETCODE_VPU_RESPONSE_TIMEOUT;
        }

        // Waiting for completion of bus transaction
        // Step1 : No more request
        VpuWriteReg(coreIdx, GDI_BUS_CTRL, 0x11);   // no more request {3'b0,no_more_req_sec,3'b0,no_more_req}

        // Step2 : Waiting for completion of bus transaction
		if (vdi_wait_bus_busy(coreIdx, __VPU_BUSY_TIMEOUT, GDI_BUS_STATUS) == -1) {
            VpuWriteReg(coreIdx, GDI_BUS_CTRL, 0x00);
            vdi_log(coreIdx, 0x10, 2);
            return RETCODE_VPU_RESPONSE_TIMEOUT;
        }
	
		cmd = 0;
        // Software Reset Trigger
		if (resetMode != SW_RESET_ON_BOOT)
			cmd =  VPU_SW_RESET_BPU_CORE | VPU_SW_RESET_BPU_BUS;
		cmd |= VPU_SW_RESET_VCE_CORE | VPU_SW_RESET_VCE_BUS;
		if (resetMode == SW_RESET_ON_BOOT)
            cmd |= VPU_SW_RESET_GDI_CORE | VPU_SW_RESET_GDI_BUS;// If you reset GDI, tiled map should be reconfigured

        VpuWriteReg(coreIdx, BIT_SW_RESET, cmd);

        // wait until reset is done
        if (vdi_wait_vpu_busy(coreIdx, __VPU_BUSY_TIMEOUT, BIT_SW_RESET_STATUS) == -1) {
            VpuWriteReg(coreIdx, BIT_SW_RESET, 0x00);
            VpuWriteReg(coreIdx, GDI_BUS_CTRL, 0x00);
            vdi_log(coreIdx, 0x10, 2);
            return RETCODE_VPU_RESPONSE_TIMEOUT;
        }

        VpuWriteReg(coreIdx, BIT_SW_RESET, 0);

        // Step3 : must clear GDI_BUS_CTRL after done SW_RESET
        VpuWriteReg(coreIdx, GDI_BUS_CTRL, 0x00);
    } 
    else {
        vdi_log(coreIdx, 0x10, 0);
        return RETCODE_NOT_FOUND_VPU_DEVICE;
    }

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuSleepWake(Uint32 coreIdx, int iSleepWake, const Uint16* code, Uint32 size)
{
    static unsigned int regBk[64];
    int i=0;
    const Uint16* bit_code = NULL;
    if (code && size > 0)
        bit_code = code;

    if (!bit_code)
        return RETCODE_INVALID_PARAM;

    if (vdi_wait_vpu_busy(coreIdx, __VPU_BUSY_TIMEOUT, BIT_BUSY_FLAG) == -1) {		
        return RETCODE_VPU_RESPONSE_TIMEOUT;
    }

    if(iSleepWake==1) 
    {
        for ( i = 0 ; i < 64 ; i++)
            regBk[i] = VpuReadReg(coreIdx, BIT_BASE + 0x100 + (i * 4));
    }
    else 
    {
        VpuWriteReg(coreIdx, BIT_CODE_RUN, 0);

        for ( i = 0 ; i < 64 ; i++)
            VpuWriteReg(coreIdx, BIT_BASE + 0x100 + (i * 4), regBk[i]);

        VpuWriteReg(coreIdx, BIT_BUSY_FLAG, 1);
        VpuWriteReg(coreIdx, BIT_CODE_RESET, 1);
        VpuWriteReg(coreIdx, BIT_CODE_RUN, 1);

        if (vdi_wait_vpu_busy(coreIdx, __VPU_BUSY_TIMEOUT, BIT_BUSY_FLAG) == -1) {		
            return RETCODE_VPU_RESPONSE_TIMEOUT;
        }
    }

    return RETCODE_SUCCESS;
}

static RetCode SetupDecCodecInstance(Int32 productId, CodecInst* pCodec)
{
    DecInfo* pDecInfo = &pCodec->CodecInfo->decInfo;

    pDecInfo->streamRdPtrRegAddr      = BIT_RD_PTR;
    pDecInfo->streamWrPtrRegAddr      = BIT_WR_PTR;
    pDecInfo->frameDisplayFlagRegAddr = BIT_FRM_DIS_FLG;
    pDecInfo->currentPC               = BIT_CUR_PC;
    pDecInfo->busyFlagAddr            = BIT_BUSY_FLAG;

    if (productId == PRODUCT_ID_960) {
        pDecInfo->dramCfg.rasBit  = EM_RAS;
        pDecInfo->dramCfg.casBit  = EM_CAS;
        pDecInfo->dramCfg.bankBit = EM_BANK;
        pDecInfo->dramCfg.busBit  = EM_WIDTH;
    }

    return RETCODE_SUCCESS;
}


RetCode Coda9VpuBuildUpDecParam(CodecInst* pCodec, DecOpenParam* param)
{
    RetCode  ret = RETCODE_SUCCESS;
    Uint32   coreIdx;
    Uint32   productId;
    DecInfo* pDecInfo = &pCodec->CodecInfo->decInfo;

    coreIdx   = pCodec->coreIdx;
    productId = Coda9VpuGetProductId(coreIdx);

    if ((ret=SetupDecCodecInstance(productId, pCodec)) != RETCODE_SUCCESS)
        return ret;

    if (param->vbWork.size) {
        pDecInfo->vbWork             = param->vbWork;
        pDecInfo->workBufferAllocExt = 1;
    }
    else {
        pDecInfo->vbWork.size       = WORK_BUF_SIZE;
        if (pCodec->codecMode == AVC_DEC)
            pDecInfo->vbWork.size += PS_SAVE_SIZE;

        if (vdi_allocate_dma_memory(pCodec->coreIdx, &pDecInfo->vbWork) < 0)
            return RETCODE_INSUFFICIENT_RESOURCE;

        param->vbWork = pDecInfo->vbWork;
        pDecInfo->workBufferAllocExt = 0;
    }

    if (productId == PRODUCT_ID_960) {
        pDecInfo->dramCfg.bankBit = EM_BANK;
        pDecInfo->dramCfg.casBit  = EM_CAS;
        pDecInfo->dramCfg.rasBit  = EM_RAS;
        pDecInfo->dramCfg.busBit  = EM_WIDTH;
    }


    return ret;
}

RetCode Coda9VpuDecInitSeq(DecHandle handle)
{
    CodecInst* pCodecInst = (CodecInst*)handle;
    DecInfo*   pDecInfo   = &pCodecInst->CodecInfo->decInfo;
    Uint32     val        = 0;

    VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_BB_START, pDecInfo->streamBufStartAddr);
    VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_BB_SIZE, pDecInfo->streamBufSize / 1024); // size in KBytes

    if(pDecInfo->userDataEnable == TRUE) {
        val  = 0;
        val |= (pDecInfo->userDataReportMode << 10);
        val |= (pDecInfo->userDataEnable << 5);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_USER_DATA_OPTION, val);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_USER_DATA_BASE_ADDR, pDecInfo->userDataBufAddr);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_USER_DATA_BUF_SIZE, pDecInfo->userDataBufSize);
    }
    else {
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_USER_DATA_OPTION, 0);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_USER_DATA_BASE_ADDR, 0);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_USER_DATA_BUF_SIZE, 0);
    }
    val  = 0;

    val |= (pDecInfo->reorderEnable<<1) & 0x2;

    val |= (pDecInfo->openParam.mp4DeblkEnable & 0x1);  
    val |= (pDecInfo->avcErrorConcealMode << 2);
    VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_OPTION, val);                  

    switch(pCodecInst->codecMode) {
    case VC1_DEC:
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_VC1_STREAM_FMT, (0 << 3) & 0x08);
        break;
    case MP4_DEC:
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_MP4_ASP_CLASS, (VPU_GMC_PROCESS_METHOD<<3)|pDecInfo->openParam.mp4Class);
        break;
    case AVC_DEC:
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_X264_MV_EN, VPU_AVC_X264_SUPPORT);
        break;  
    }

    if( pCodecInst->codecMode == AVC_DEC )
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_SEQ_SPP_CHUNK_SIZE, VPU_GBU_SIZE);

    VpuWriteReg(pCodecInst->coreIdx, pDecInfo->streamWrPtrRegAddr, pDecInfo->streamWrPtr);
    VpuWriteReg(pCodecInst->coreIdx, pDecInfo->streamRdPtrRegAddr, pDecInfo->streamRdPtr);

    if (pCodecInst->productId == PRODUCT_ID_980 || pCodecInst->productId == PRODUCT_ID_960) {
        pDecInfo->streamEndflag &= ~(3<<3);
        if (pDecInfo->openParam.bitstreamMode == BS_MODE_PIC_END)
            pDecInfo->streamEndflag |= (2<<3);
        else {  // Interrupt Mode
            if (pDecInfo->seqInitEscape) {
                pDecInfo->streamEndflag |= (2<<3);
            }
        }
    } 
    VpuWriteReg(pCodecInst->coreIdx, BIT_BIT_STREAM_PARAM, pDecInfo->streamEndflag);    

    val = pDecInfo->openParam.streamEndian;
    VpuWriteReg(pCodecInst->coreIdx, BIT_BIT_STREAM_CTRL, val);

    if (pCodecInst->productId == PRODUCT_ID_980) {
        val = 0;
        val |= (pDecInfo->openParam.bwbEnable<<15);
        val |= (pDecInfo->wtlMode<<17)|(pDecInfo->tiled2LinearMode<<13)|(FORMAT_420<<6);            
        val |= ((pDecInfo->openParam.cbcrInterleave)<<2); // Interleave bit position is modified
        val |= pDecInfo->openParam.frameEndian;
        VpuWriteReg(pCodecInst->coreIdx, BIT_FRAME_MEM_CTRL, val);
    }
    else if (pCodecInst->productId == PRODUCT_ID_960) {
        val = 0;
        val |= (pDecInfo->wtlEnable<<17);
        val |= (pDecInfo->openParam.bwbEnable<<12);
        val |= ((pDecInfo->openParam.cbcrInterleave)<<2); // Interleave bit position is modified
        val |= pDecInfo->openParam.frameEndian;
        VpuWriteReg(pCodecInst->coreIdx, BIT_FRAME_MEM_CTRL, val);
    }
    else {
        return RETCODE_NOT_FOUND_VPU_DEVICE;
    }

    VpuWriteReg(pCodecInst->coreIdx, pDecInfo->frameDisplayFlagRegAddr, 0);
    Coda9BitIssueCommand(pCodecInst->coreIdx, pCodecInst, DEC_SEQ_INIT);

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuFiniSeq(CodecInst* instance)
{
    Coda9BitIssueCommand(instance->coreIdx, instance, DEC_SEQ_END);
    if (vdi_wait_vpu_busy(instance->coreIdx, __VPU_BUSY_TIMEOUT, BIT_BUSY_FLAG) == -1) {
        return RETCODE_VPU_RESPONSE_TIMEOUT;        
    }

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuDecode(CodecInst* instance, DecParam* param)
{
    CodecInst*  pCodecInst;
    DecInfo*    pDecInfo;
    Uint32      rotMir;
    Int32       val;
    vpu_instance_pool_t *vip;

    pCodecInst = instance;
    pDecInfo   = &pCodecInst->CodecInfo->decInfo;
    vip = (vpu_instance_pool_t *)vdi_get_instance_pool(pCodecInst->coreIdx);
    if (!vip) {
        return RETCODE_INVALID_HANDLE;
    }

    rotMir = 0;
    if (pDecInfo->rotationEnable) {
        rotMir |= 0x10; // Enable rotator
        switch (pDecInfo->rotationAngle) {
        case 0:
            rotMir |= 0x0;
            break;

        case 90:
            rotMir |= 0x1;
            break;

        case 180:
            rotMir |= 0x2;
            break;

        case 270:
            rotMir |= 0x3;
            break;
        }
    }

    if (pDecInfo->mirrorEnable) {
        rotMir |= 0x10; // Enable rotator
        switch (pDecInfo->mirrorDirection) {
        case MIRDIR_NONE :
            rotMir |= 0x0;
            break;

        case MIRDIR_VER :
            rotMir |= 0x4;
            break;

        case MIRDIR_HOR :
            rotMir |= 0x8;
            break;

        case MIRDIR_HOR_VER :
            rotMir |= 0xc;
            break;

        }
    }

    if (pDecInfo->tiled2LinearEnable) {
        rotMir |= 0x10; 
    }

    if (pDecInfo->deringEnable) {
        rotMir |= 0x20; // Enable Dering Filter
    }

    if (rotMir && !pDecInfo->rotatorOutputValid) {
        return RETCODE_ROTATOR_OUTPUT_NOT_SET;
    }

    VpuWriteReg(pCodecInst->coreIdx, RET_DEC_PIC_CROP_LEFT_RIGHT, 0);               // frame crop information(left, right)
    VpuWriteReg(pCodecInst->coreIdx, RET_DEC_PIC_CROP_TOP_BOTTOM, 0);               // frame crop information(top, bottom)

    if (pCodecInst->productId == PRODUCT_ID_960) {
        if (pDecInfo->mapType>LINEAR_FRAME_MAP && pDecInfo->mapType<=TILED_MIXED_V_MAP) {
            SetTiledFrameBase(pCodecInst->coreIdx, pDecInfo->mapCfg.tiledBaseAddr);
        }
        else {
            SetTiledFrameBase(pCodecInst->coreIdx, 0);
        }
    }

    if (pDecInfo->mapType != LINEAR_FRAME_MAP && pDecInfo->mapType != LINEAR_FIELD_MAP) {
        val = SetTiledMapType(pCodecInst->coreIdx, &pDecInfo->mapCfg, pDecInfo->mapType, 
            (pDecInfo->stride > pDecInfo->frameBufferHeight)?pDecInfo->stride: pDecInfo->frameBufferHeight, 
            pDecInfo->openParam.cbcrInterleave, &pDecInfo->dramCfg);	        
    } else {
        val = SetTiledMapType(pCodecInst->coreIdx, &pDecInfo->mapCfg, pDecInfo->mapType, 
            pDecInfo->stride, pDecInfo->openParam.cbcrInterleave, &pDecInfo->dramCfg);
    }
    if (val == 0) {
        return RETCODE_INVALID_PARAM;
    }

    if (rotMir & 0x30) { // rotator or dering or tiled2linear enabled
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_MODE, rotMir);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_INDEX, pDecInfo->rotatorOutput.myIndex);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_ADDR_Y,  pDecInfo->rotatorOutput.bufY);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_ADDR_CB, pDecInfo->rotatorOutput.bufCb);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_ADDR_CR, pDecInfo->rotatorOutput.bufCr);
        if (pCodecInst->productId == PRODUCT_ID_980) {
            VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_BOTTOM_Y,  pDecInfo->rotatorOutput.bufYBot);
            VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_BOTTOM_CB, pDecInfo->rotatorOutput.bufCbBot);
            VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_BOTTOM_CR, pDecInfo->rotatorOutput.bufCrBot);
        }
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_STRIDE, pDecInfo->rotatorStride);
    }
    else {
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_ROT_MODE, rotMir);		
    }
    if(pDecInfo->userDataEnable) {
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_USER_DATA_BASE_ADDR, pDecInfo->userDataBufAddr);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_USER_DATA_BUF_SIZE, pDecInfo->userDataBufSize);
    }
    else {
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_USER_DATA_BASE_ADDR, 0);
        VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_USER_DATA_BUF_SIZE, 0);
    }

    val = 0;
    if (param->iframeSearchEnable == TRUE) { // if iframeSearch is Enable, other bit is ignore;	
        val |= (pDecInfo->userDataReportMode <<10);

        if (pCodecInst->codecMode == AVC_DEC || pCodecInst->codecMode == VC1_DEC) {
            if (param->iframeSearchEnable==1)
                val |= (1<< 11) | (1<<2);
            else if (param->iframeSearchEnable==2)
                val |= (1<<2);
        }
        else {
            val |= (( param->iframeSearchEnable &0x1)   << 2 );
        }
    } 
    else {
        val |= (pDecInfo->userDataReportMode  <<10);
        val |= (pDecInfo->userDataEnable      << 5);
        val |= (param->skipframeMode          << 3);
    }

    if (pCodecInst->productId == PRODUCT_ID_980) {
        if (pCodecInst->codecMode == AVC_DEC && pDecInfo->lowDelayInfo.lowDelayEn) {
            val |= (pDecInfo->lowDelayInfo.lowDelayEn <<18 );
        }
    }
    if (pCodecInst->codecMode == MP2_DEC) {
        val |= ((param->DecStdParam.mp2PicFlush&1)<<15);
    }
    if (pCodecInst->codecMode == RV_DEC) {
        val |= ((param->DecStdParam.rvDbkMode&0x0f)<<16);
    }

    VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_OPTION, val);

    if (pCodecInst->productId == PRODUCT_ID_980) {
        if (pDecInfo->lowDelayInfo.lowDelayEn == TRUE) {
            VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_NUM_ROWS, pDecInfo->lowDelayInfo.numRows);
        }
        else {
            VpuWriteReg(pCodecInst->coreIdx, CMD_DEC_PIC_NUM_ROWS, 0);
        }
    }

    val = 0;

    val = ( 
        (pDecInfo->secAxiInfo.u.coda9.useBitEnable&0x01)<<0 | 
        (pDecInfo->secAxiInfo.u.coda9.useIpEnable&0x01)<<1 | 
        (pDecInfo->secAxiInfo.u.coda9.useDbkYEnable&0x01)<<2 |
        (pDecInfo->secAxiInfo.u.coda9.useDbkCEnable&0x01)<<3 |
        (pDecInfo->secAxiInfo.u.coda9.useOvlEnable&0x01)<<4 | 
        (pDecInfo->secAxiInfo.u.coda9.useBtpEnable&0x01)<<5 | 
        (pDecInfo->secAxiInfo.u.coda9.useBitEnable&0x01)<<8 |
        (pDecInfo->secAxiInfo.u.coda9.useIpEnable&0x01)<<9 |
        (pDecInfo->secAxiInfo.u.coda9.useDbkYEnable&0x01)<<10 | 
        (pDecInfo->secAxiInfo.u.coda9.useDbkCEnable&0x01)<<11 | 
        (pDecInfo->secAxiInfo.u.coda9.useOvlEnable&0x01)<<12 |
        (pDecInfo->secAxiInfo.u.coda9.useBtpEnable&0x01)<<13 );

    VpuWriteReg(pCodecInst->coreIdx, BIT_AXI_SRAM_USE, val);

    VpuWriteReg(pCodecInst->coreIdx, pDecInfo->streamWrPtrRegAddr, pDecInfo->streamWrPtr);
    VpuWriteReg(pCodecInst->coreIdx, pDecInfo->streamRdPtrRegAddr, pDecInfo->streamRdPtr);

    pDecInfo->streamEndflag &= ~(3<<3);
    if (pDecInfo->openParam.bitstreamMode == BS_MODE_PIC_END)
        pDecInfo->streamEndflag |= (2<<3);

    VpuWriteReg(pCodecInst->coreIdx, BIT_BIT_STREAM_PARAM, pDecInfo->streamEndflag); 

    if (pCodecInst->productId == PRODUCT_ID_980) {
        val = 0;
        val |= (pDecInfo->openParam.bwbEnable<<15);
        val |= (pDecInfo->wtlMode<<17)|(pDecInfo->tiled2LinearMode<<13)|(pDecInfo->mapType<<9)|(FORMAT_420<<6);			
    }
    else if (pCodecInst->productId == PRODUCT_ID_960) {
        val = 0;
        val |= (pDecInfo->wtlEnable<<17);
        val |= (pDecInfo->openParam.bwbEnable<<12);
        if (pDecInfo->mapType) {
            if (pDecInfo->mapType == TILED_FRAME_MB_RASTER_MAP || pDecInfo->mapType == TILED_FIELD_MB_RASTER_MAP)
                val |= (pDecInfo->tiled2LinearEnable<<11)|(0x03<<9)|(FORMAT_420<<6);	
            else
                val |= (pDecInfo->tiled2LinearEnable<<11)|(0x02<<9)|(FORMAT_420<<6);	
        }
    }
    else {
        return RETCODE_NOT_FOUND_VPU_DEVICE;
    }

    val |= ((pDecInfo->openParam.cbcrInterleave)<<2); // Interleave bit position is modified
    val |= pDecInfo->openParam.frameEndian;
    VpuWriteReg(pCodecInst->coreIdx, BIT_FRAME_MEM_CTRL, val);	

    val = pDecInfo->openParam.streamEndian;
    VpuWriteReg(pCodecInst->coreIdx, BIT_BIT_STREAM_CTRL, val);

    Coda9BitIssueCommand(pCodecInst->coreIdx, pCodecInst, PIC_RUN);

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuDecGetResult(CodecInst* instance, DecOutputInfo* result)
{
    CodecInst*  pCodecInst;
    DecInfo*    pDecInfo;
    Uint32      val  = 0;

    pCodecInst = instance;
    pDecInfo   = &pCodecInst->CodecInfo->decInfo;

    if (pCodecInst->loggingEnable)
        vdi_log(pCodecInst->coreIdx, PIC_RUN, 0);

    result->warnInfo = 0;
    val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_SUCCESS);
    result->decodingSuccess = val;
    if (result->decodingSuccess & (1UL<<31)) {
        return RETCODE_MEMORY_ACCESS_VIOLATION;
    }

    if( pCodecInst->codecMode == AVC_DEC ) {
        result->notSufficientPsBuffer = (val >> 3) & 0x1;
        result->notSufficientSliceBuffer = (val >> 2) & 0x1;
        result->refMissingFrameFlag     = (val>>21) & 0x1;
    }

    result->chunkReuseRequired = 0;
    if (pDecInfo->openParam.bitstreamMode == BS_MODE_PIC_END) {
        switch (pCodecInst->codecMode) {
        case AVC_DEC:
            result->chunkReuseRequired = ((val >> 16) & 0x01);	// in case of NPF frame
            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_DECODED_IDX);
            if (val == (Uint32)-1) {
                result->chunkReuseRequired = TRUE;
            }
            break;
        case MP2_DEC:
        case MP4_DEC:
            result->chunkReuseRequired = ((val >> 16) & 0x01);
            break;
        default:
            break;
        }
    }

    result->indexFrameDecoded = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_DECODED_IDX);
    result->indexFrameDisplay = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_DISPLAY_IDX);
    if (pDecInfo->mapType == LINEAR_FRAME_MAP) {
        result->indexFrameDecodedForTiled = -1;
        result->indexFrameDisplayForTiled = -1;
    } 
    else {
        result->indexFrameDecodedForTiled = result->indexFrameDecoded;
        result->indexFrameDisplayForTiled = result->indexFrameDisplay;
    }

    val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_SIZE); // decoding picture size
    result->decPicWidth  = (val>>16) & 0xFFFF;
    result->decPicHeight = (val) & 0xFFFF;

    if (result->indexFrameDecoded >= 0 && result->indexFrameDecoded < MAX_GDI_IDX) {
        switch (pCodecInst->codecMode) {
        case VPX_DEC:
            if ( pCodecInst->codecModeAux == VPX_AUX_VP8 ) {
                // VP8 specific header information
                // h_scale[31:30] v_scale[29:28] pic_width[27:14] pic_height[13:0]
                val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_VP8_SCALE_INFO);
                result->vp8ScaleInfo.hScaleFactor = (val >> 30) & 0x03;
                result->vp8ScaleInfo.vScaleFactor = (val >> 28) & 0x03;
                result->vp8ScaleInfo.picWidth = (val >> 14) & 0x3FFF;
                result->vp8ScaleInfo.picHeight = (val >> 0) & 0x3FFF;
                // ref_idx_gold[31:24], ref_idx_altr[23:16], ref_idx_last[15: 8],
                // version_number[3:1], show_frame[0]
                val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_VP8_PIC_REPORT);
                result->vp8PicInfo.refIdxGold = (val >> 24) & 0x0FF;
                result->vp8PicInfo.refIdxAltr = (val >> 16) & 0x0FF;
                result->vp8PicInfo.refIdxLast = (val >> 8) & 0x0FF;
                result->vp8PicInfo.versionNumber = (val >> 1) & 0x07;
                result->vp8PicInfo.showFrame = (val >> 0) & 0x01;
            }
            break;
        case AVC_DEC:
        case AVS_DEC:
            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_CROP_LEFT_RIGHT);				// frame crop information(left, right)
            pDecInfo->initialInfo.picCropRect.left  = (val>>16) & 0xffff;
            pDecInfo->initialInfo.picCropRect.right = pDecInfo->initialInfo.picWidth - (val&0xffff);
            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_CROP_TOP_BOTTOM);			// frame crop information(top, bottom)
            pDecInfo->initialInfo.picCropRect.top    = (val>>16) & 0xffff;
            pDecInfo->initialInfo.picCropRect.bottom = pDecInfo->initialInfo.picHeight - (val&0xffff);
            break;
        case MP2_DEC:
            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_MP2_BAR_LEFT_RIGHT);
            pDecInfo->initialInfo.mp2BardataInfo.barLeft = ((val>>16) & 0xFFFF);
            pDecInfo->initialInfo.mp2BardataInfo.barRight = (val&0xFFFF);
            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_MP2_BAR_TOP_BOTTOM);
            pDecInfo->initialInfo.mp2BardataInfo.barTop    = ((val>>16) & 0xFFFF);
            pDecInfo->initialInfo.mp2BardataInfo.barBottom = (val&0xFFFF);
            result->mp2BardataInfo = pDecInfo->initialInfo.mp2BardataInfo;

            result->mp2PicDispExtInfo.offsetNum = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_MP2_OFFSET_NUM);

            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_MP2_OFFSET1);
            result->mp2PicDispExtInfo.horizontalOffset1	= (Int16) (val >> 16) & 0xFFFF;
            result->mp2PicDispExtInfo.verticalOffset1	= (Int16) (val & 0xFFFF);

            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_MP2_OFFSET2);
            result->mp2PicDispExtInfo.horizontalOffset2	= (Int16) (val >> 16) & 0xFFFF;
            result->mp2PicDispExtInfo.verticalOffset2	= (Int16) (val & 0xFFFF);

            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_MP2_OFFSET3);
            result->mp2PicDispExtInfo.horizontalOffset3	= (Int16) (val >> 16) & 0xFFFF;
            result->mp2PicDispExtInfo.verticalOffset3	= (Int16) (val & 0xFFFF);
            break;
        }
    }

    val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_TYPE);
    result->interlacedFrame	= (val >> 18) & 0x1;
    result->topFieldFirst   = (val >> 21) & 0x0001;	// TopFieldFirst[21]
    if (result->interlacedFrame) {
        result->picTypeFirst      = (val & 0x38) >> 3;	  // pic_type of 1st field
        result->picType           = val & 7;              // pic_type of 2nd field
    }
    else {
        result->picTypeFirst   = PIC_TYPE_MAX;	// no meaning
        result->picType = val & 7;
    }

    result->pictureStructure  = (val >> 19) & 0x0003;	// MbAffFlag[17], FieldPicFlag[16]
    result->repeatFirstField  = (val >> 22) & 0x0001;
    result->progressiveFrame  = (val >> 23) & 0x0003;

    if( pCodecInst->codecMode == AVC_DEC)
    {
        result->decFrameInfo     = (val >> 15) & 0x0001;
        result->picStrPresent     = (val >> 27) & 0x0001;
        result->picTimingStruct   = (val >> 28) & 0x000f;
        //update picture type when IDR frame
        if (val & 0x40) { // 6th bit
            if (result->interlacedFrame)
                result->picTypeFirst = PIC_TYPE_IDR;
            else
                result->picType = PIC_TYPE_IDR;

        }
        result->decFrameInfo  = (val >> 16) & 0x0003;
        if (result->indexFrameDisplay >= 0) {
            if (result->indexFrameDisplay == result->indexFrameDecoded)
                result->avcNpfFieldInfo = result->decFrameInfo;
            else
                result->avcNpfFieldInfo = pDecInfo->decOutInfo[result->indexFrameDisplay].decFrameInfo;
        }
        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_HRD_INFO);
        result->avcHrdInfo.cpbMinus1 = val>>2;
        result->avcHrdInfo.vclHrdParamFlag = (val>>1)&1;
        result->avcHrdInfo.nalHrdParamFlag = val&1;

        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_VUI_INFO);
        result->avcVuiInfo.fixedFrameRateFlag    = val &1;
        result->avcVuiInfo.timingInfoPresent     = (val>>1) & 0x01;
        result->avcVuiInfo.chromaLocBotField     = (val>>2) & 0x07;
        result->avcVuiInfo.chromaLocTopField     = (val>>5) & 0x07;
        result->avcVuiInfo.chromaLocInfoPresent  = (val>>8) & 0x01;
        result->avcVuiInfo.colorPrimaries        = (val>>16) & 0xff;
        result->avcVuiInfo.colorDescPresent      = (val>>24) & 0x01;
        result->avcVuiInfo.isExtSAR              = (val>>25) & 0x01;
        result->avcVuiInfo.vidFullRange          = (val>>26) & 0x01;
        result->avcVuiInfo.vidFormat             = (val>>27) & 0x07;
        result->avcVuiInfo.vidSigTypePresent     = (val>>30) & 0x01;
        result->avcVuiInfo.vuiParamPresent       = (val>>31) & 0x01;
        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_VUI_PIC_STRUCT);
        result->avcVuiInfo.vuiPicStructPresent = (val & 0x1);
        result->avcVuiInfo.vuiPicStruct = (val>>1);
    }

    if( pCodecInst->codecMode == MP2_DEC)
    {
        result->fieldSequence       = (val >> 25) & 0x0007;
        result->frameDct            = (val >> 28) & 0x0001;
        result->progressiveSequence = (val >> 29) & 0x0001;
    }

    result->fRateNumerator    = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_FRATE_NR); //Frame rate, Aspect ratio can be changed frame by frame.
    result->fRateDenominator  = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_FRATE_DR);
    if (pCodecInst->codecMode == AVC_DEC && result->fRateDenominator > 0)
        result->fRateDenominator  *= 2;
    if (pCodecInst->codecMode == MP4_DEC)
    {
        result->mp4ModuloTimeBase = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_MODULO_TIME_BASE);
        result->mp4TimeIncrement  = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_VOP_TIME_INCREMENT);
    }

    if (pCodecInst->codecMode == RV_DEC) {
        result->rvTr = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_RV_TR);	
        result->rvTrB = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_RV_TR_BFRAME);		
    }

    if (pCodecInst->codecMode == VPX_DEC) {
        result->aspectRateInfo = 0;
    }
    else {
        result->aspectRateInfo = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_ASPECT);
    }


    // User Data
    if (pDecInfo->userDataEnable) {
        int userDataNum;
        int userDataSize;
        BYTE tempBuf[8] = {0,};

        VpuReadMem(pCodecInst->coreIdx, pDecInfo->userDataBufAddr + 0, tempBuf, 8,  VPU_USER_DATA_ENDIAN); 

        val =	((tempBuf[0]<<24) & 0xFF000000) |
            ((tempBuf[1]<<16) & 0x00FF0000) |
            ((tempBuf[2]<< 8) & 0x0000FF00) |
            ((tempBuf[3]<< 0) & 0x000000FF);

        userDataNum = (val >> 16) & 0xFFFF;
        userDataSize = (val >> 0) & 0xFFFF;
        if (userDataNum == 0)
            userDataSize = 0;

        result->decOutputExtData.userDataNum = userDataNum;
        result->decOutputExtData.userDataSize = userDataSize;

        val = ((tempBuf[4]<<24) & 0xFF000000) |
            ((tempBuf[5]<<16) & 0x00FF0000) |
            ((tempBuf[6]<< 8) & 0x0000FF00) |
            ((tempBuf[7]<< 0) & 0x000000FF);

        if (userDataNum == 0)
            result->decOutputExtData.userDataBufFull = 0;
        else
            result->decOutputExtData.userDataBufFull = (val >> 16) & 0xFFFF;

        result->decOutputExtData.activeFormat = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_ATSC_USER_DATA_INFO)&0xf;
    }

    result->numOfErrMBs     = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_ERR_MB);
    val                     = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_SUCCESS);
    result->sequenceChanged = ((val>>20) & 0x1);
    result->streamEndFlag   = ((pDecInfo->streamEndflag>>2) & 0x01);

    if (pCodecInst->codecMode == VC1_DEC && result->indexFrameDisplay != -3) {
        if (pDecInfo->vc1BframeDisplayValid == 0) {
            if (result->picType == 2) {
                result->indexFrameDisplay = -3;
            } else {
                pDecInfo->vc1BframeDisplayValid = 1;
            }
        }
    }
    if (pCodecInst->codecMode == AVC_DEC && pCodecInst->codecModeAux == AVC_AUX_MVC)
    {
        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_MVC_REPORT);
        result->mvcPicInfo.viewIdxDisplay = (val>>0) & 1;
        result->mvcPicInfo.viewIdxDecoded = (val>>1) & 1;
    }

    if (pCodecInst->codecMode == AVC_DEC)
    {
        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_AVC_FPA_SEI0);

        if ((int)val < 0) {
            result->avcFpaSei.exist = 0;
        } 
        else {
            result->avcFpaSei.exist = 1;
            result->avcFpaSei.framePackingArrangementId = val;

            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_AVC_FPA_SEI1);
            result->avcFpaSei.contentInterpretationType               = val&0x3F; // [5:0]
            result->avcFpaSei.framePackingArrangementType             = (val >> 6)&0x7F; // [12:6]
            result->avcFpaSei.framePackingArrangementExtensionFlag    = (val >> 13)&0x01; // [13]
            result->avcFpaSei.frame1SelfContainedFlag                 = (val >> 14)&0x01; // [14]
            result->avcFpaSei.frame0SelfContainedFlag                 = (val >> 15)&0x01; // [15]
            result->avcFpaSei.currentFrameIsFrame0Flag                = (val >> 16)&0x01; // [16]
            result->avcFpaSei.fieldViewsFlag                          = (val >> 17)&0x01; // [17]
            result->avcFpaSei.frame0FlippedFlag                       = (val >> 18)&0x01; // [18]
            result->avcFpaSei.spatialFlippingFlag                     = (val >> 19)&0x01; // [19]
            result->avcFpaSei.quincunxSamplingFlag                    = (val >> 20)&0x01; // [20]
            result->avcFpaSei.framePackingArrangementCancelFlag       = (val >> 21)&0x01; // [21]

            val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_AVC_FPA_SEI2);
            result->avcFpaSei.framePackingArrangementRepetitionPeriod = val&0x7FFF;       // [14:0]
            result->avcFpaSei.frame1GridPositionY                     = (val >> 16)&0x0F; // [19:16]
            result->avcFpaSei.frame1GridPositionX                     = (val >> 20)&0x0F; // [23:20]
            result->avcFpaSei.frame0GridPositionY                     = (val >> 24)&0x0F; // [27:24]
            result->avcFpaSei.frame0GridPositionX                     = (val >> 28)&0x0F; // [31:28]
        }      

        result->avcPocTop = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_POC_TOP);
        result->avcPocBot = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_POC_BOT);

        if (result->interlacedFrame)
        {
            if (result->avcPocTop > result->avcPocBot) {
                result->avcPocPic = result->avcPocBot;
            } else {
                result->avcPocPic = result->avcPocTop;
            }
        }
        else
            result->avcPocPic = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_POC);
    }

	if (pCodecInst->codecMode == AVC_DEC)
	{
		val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_PIC_AVC_SEI_RP_INFO);

		if ((int)val < 0) {
			result->avcRpSei.exist = 0;
		}
		else {
			result->avcRpSei.exist = 1;
			result->avcRpSei.changingSliceGroupIdc      = val & 0x3;       // [1:0]
			result->avcRpSei.brokenLinkFlag             = (val >> 2)&0x01; // [2]
			result->avcRpSei.exactMatchFlag             = (val >> 3)&0x01; // [3]
			result->avcRpSei.recoveryFrameCnt           = (val >> 4)&0x3F; // [9:4]
		}
	}

	result->bytePosFrameStart = VpuReadReg(pCodecInst->coreIdx, BIT_BYTE_POS_FRAME_START);
    result->bytePosFrameEnd   = VpuReadReg(pCodecInst->coreIdx, BIT_BYTE_POS_FRAME_END);

    if (result->indexFrameDecoded >= 0 && result->indexFrameDecoded < MAX_GDI_IDX)
        pDecInfo->decOutInfo[result->indexFrameDecoded] = *result;

    result->frameDisplayFlag = pDecInfo->frameDisplayFlag;
    result->frameCycle = VpuReadReg(pCodecInst->coreIdx, BIT_FRAME_CYCLE);

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuDecSetBitstreamFlag(CodecInst* instance, BOOL running, BOOL eos)
{
    Uint32      val;
    DecInfo*    pDecInfo;

    pDecInfo = &instance->CodecInfo->decInfo;

    if (eos&0x01) {
        val = VpuReadReg(instance->coreIdx, BIT_BIT_STREAM_PARAM);
        val |= 1 << 2;
        pDecInfo->streamEndflag = val;
        if (running == TRUE)
            VpuWriteReg(instance->coreIdx, BIT_BIT_STREAM_PARAM, val);
        return RETCODE_SUCCESS;
    } 
    else {
        val = VpuReadReg(instance->coreIdx, BIT_BIT_STREAM_PARAM);
        val &= ~(1 << 2);
        pDecInfo->streamEndflag = val;
        if (running == TRUE)
            VpuWriteReg(instance->coreIdx, BIT_BIT_STREAM_PARAM, val);

        return RETCODE_SUCCESS;
    }    
}


RetCode Coda9VpuDecGetSeqInfo(CodecInst* instance, DecInitialInfo* info)
{
    CodecInst*  pCodecInst = NULL;
    DecInfo*    pDecInfo   = NULL;
    Uint32      val, val2;

    pCodecInst = instance;
    pDecInfo   = &pCodecInst->CodecInfo->decInfo;

    if (pCodecInst->loggingEnable) {
        vdi_log(pCodecInst->coreIdx, DEC_SEQ_INIT, 0);
    }

    info->warnInfo = 0;
    if (pDecInfo->openParam.bitstreamMode == BS_MODE_INTERRUPT && pDecInfo->seqInitEscape) {
        pDecInfo->streamEndflag &= ~(3<<3);
        VpuWriteReg(pCodecInst->coreIdx, BIT_BIT_STREAM_PARAM, pDecInfo->streamEndflag);
        pDecInfo->seqInitEscape = 0;
    }
    pDecInfo->streamRdPtr      = VpuReadReg(instance->coreIdx, pDecInfo->streamRdPtrRegAddr);	
    pDecInfo->frameDisplayFlag = VpuReadReg(pCodecInst->coreIdx, pDecInfo->frameDisplayFlagRegAddr);
    pDecInfo->streamEndflag    = VpuReadReg(pCodecInst->coreIdx, BIT_BIT_STREAM_PARAM);

    info->seqInitErrReason = 0;
    val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_SUCCESS);
    if (val & (1UL<<31)) {
        return RETCODE_MEMORY_ACCESS_VIOLATION;
    }

    if ( pDecInfo->openParam.bitstreamMode == BS_MODE_PIC_END) {
        if (val & (1<<4)) {
            info->seqInitErrReason = (VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_SEQ_ERR_REASON));
            return RETCODE_FAILURE;
        }
    }

    if (val == 0) {
        info->seqInitErrReason = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_SEQ_ERR_REASON);
        return RETCODE_FAILURE;
    }

    val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_SRC_SIZE);
    info->picWidth          = ( (val >> 16) & 0xffff );
    info->picHeight         = ( val & 0xffff );
    info->lumaBitdepth      = 8;
    info->chromaBitdepth    = 8;
    info->fRateNumerator    = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_FRATE_NR);
    info->fRateDenominator  = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_FRATE_DR);
    if (pCodecInst->codecMode == AVC_DEC && info->fRateDenominator > 0) {
        info->fRateDenominator  *= 2;
    }

    if (pCodecInst->codecMode  == MP4_DEC) {
        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_INFO);
        info->mp4ShortVideoHeader    = (val >> 2) & 1;
        info->mp4DataPartitionEnable = val & 1;
        info->mp4ReversibleVlcEnable = info->mp4DataPartitionEnable ? ((val >> 1) & 1) : 0;
        info->h263AnnexJEnable       = (val >> 3) & 1;
    }
    else if (pCodecInst->codecMode == VPX_DEC && pCodecInst->codecModeAux == VPX_AUX_VP8) {
        // h_scale[31:30] v_scale[29:28] pic_width[27:14] pic_height[13:0]
        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_VP8_SCALE_INFO);
        info->vp8ScaleInfo.hScaleFactor = (val >> 30) & 0x03;
        info->vp8ScaleInfo.vScaleFactor = (val >> 28) & 0x03;
        info->vp8ScaleInfo.picWidth     = (val >> 14) & 0x3FFF;
        info->vp8ScaleInfo.picHeight    = (val >> 0) & 0x3FFF;
    }

    info->minFrameBufferCount = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_FRAME_NEED);

    info->frameBufDelay = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_FRAME_DELAY);

    if (pCodecInst->codecMode == AVC_DEC || pCodecInst->codecMode == MP2_DEC || pCodecInst->codecMode == AVS_DEC) {
        val  = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_CROP_LEFT_RIGHT);
        val2 = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_CROP_TOP_BOTTOM);

        info->picCropRect.left   = ((val>>16) & 0xFFFF);
        info->picCropRect.right  = info->picWidth - (val & 0xFFFF);;
        info->picCropRect.top    = ((val2>>16) & 0xFFFF);
        info->picCropRect.bottom = info->picHeight - (val2 & 0xFFFF);

        val = (info->picWidth * info->picHeight * 3 / 2) / 1024;
        info->normalSliceSize = val / 4;
        info->worstSliceSize = val / 2;
    }
    else {
        info->picCropRect.left   = 0;
        info->picCropRect.right  = info->picWidth;
        info->picCropRect.top    = 0;
        info->picCropRect.bottom = info->picHeight;
    }

    if (pCodecInst->codecMode == MP2_DEC) {
        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_MP2_BAR_LEFT_RIGHT);
        val2 = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_MP2_BAR_TOP_BOTTOM);

        info->mp2BardataInfo.barLeft   = ((val>>16) & 0xFFFF);
        info->mp2BardataInfo.barRight  = (val&0xFFFF);
        info->mp2BardataInfo.barTop    = ((val2>>16) & 0xFFFF);
        info->mp2BardataInfo.barBottom = (val2&0xFFFF);               
    }

    val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_HEADER_REPORT);
    info->profile                = (val >> 0) & 0xFF;
    info->level                  = (val >> 8) & 0xFF;
    info->interlace              = (val >> 16) & 0x01;
    info->direct8x8Flag          = (val >> 17) & 0x01;
    info->vc1Psf                 = (val >> 18) & 0x01;
    info->constraint_set_flag[0] = (val >> 19) & 0x01;
    info->constraint_set_flag[1] = (val >> 20) & 0x01;
    info->constraint_set_flag[2] = (val >> 21) & 0x01;
    info->constraint_set_flag[3] = (val >> 22) & 0x01;
    info->chromaFormatIDC        = (val >> 23) & 0x03;
    info->isExtSAR               = (val >> 25) & 0x01;
    info->maxNumRefFrm           = (val >> 27) & 0x0f;
    info->maxNumRefFrmFlag       = (val >> 31) & 0x01;
    val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_ASPECT);
    info->aspectRateInfo = val;

    val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_BIT_RATE);
    info->bitRate = val;

    if (pCodecInst->codecMode == AVC_DEC) {
        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_VUI_INFO);
        info->avcVuiInfo.fixedFrameRateFlag    = val &1;
        info->avcVuiInfo.timingInfoPresent     = (val>>1) & 0x01;
        info->avcVuiInfo.chromaLocBotField     = (val>>2) & 0x07;
        info->avcVuiInfo.chromaLocTopField     = (val>>5) & 0x07;
        info->avcVuiInfo.chromaLocInfoPresent  = (val>>8) & 0x01;
        info->avcVuiInfo.colorPrimaries        = (val>>16) & 0xff;
        info->avcVuiInfo.colorDescPresent      = (val>>24) & 0x01;
        info->avcVuiInfo.isExtSAR              = (val>>25) & 0x01;
        info->avcVuiInfo.vidFullRange          = (val>>26) & 0x01;
        info->avcVuiInfo.vidFormat             = (val>>27) & 0x07;
        info->avcVuiInfo.vidSigTypePresent     = (val>>30) & 0x01;
        info->avcVuiInfo.vuiParamPresent       = (val>>31) & 0x01;

        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_VUI_PIC_STRUCT);
        info->avcVuiInfo.vuiPicStructPresent   = (val & 0x1);
        info->avcVuiInfo.vuiPicStruct          = (val>>1);
    }

    if (pCodecInst->codecMode == MP2_DEC) {
        // seq_ext info
        val = VpuReadReg(pCodecInst->coreIdx, RET_DEC_SEQ_EXT_INFO);
        info->mp2LowDelay    = val & 1;
        info->mp2DispVerSize = (val>>1) & 0x3fff;
        info->mp2DispHorSize = (val>>15) & 0x3fff;

        if (pDecInfo->userDataEnable) {
            Uint32 userDataNum  = 0;
            Uint32 userDataSize = 0;
            BYTE   tempBuf[8]   = {0,};     

            // user data
            VpuReadMem(pCodecInst->coreIdx, pDecInfo->userDataBufAddr, tempBuf, 8, VPU_USER_DATA_ENDIAN); 

            val = ((tempBuf[0]<<24) & 0xFF000000) |
                  ((tempBuf[1]<<16) & 0x00FF0000) |
                  ((tempBuf[2]<< 8) & 0x0000FF00) |
                  ((tempBuf[3]<< 0) & 0x000000FF);

            userDataNum  = (val >> 16) & 0xFFFF;
            userDataSize = (val >> 0) & 0xFFFF;
            if (userDataNum == 0) {
                userDataSize = 0;
            }

            info->userDataNum  = userDataNum;
            info->userDataSize = userDataSize;

            val = ((tempBuf[4]<<24) & 0xFF000000) |
                  ((tempBuf[5]<<16) & 0x00FF0000) |
                  ((tempBuf[6]<< 8) & 0x0000FF00) |
                  ((tempBuf[7]<< 0) & 0x000000FF);

            if (userDataNum == 0) {
                info->userDataBufFull = 0;
            }
            else {
                info->userDataBufFull = (val >> 16) & 0xFFFF;
            }
        }
    }

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuDecRegisterFramebuffer(CodecInst* instance)
{
    CodecInst*      pCodecInst;
    DecInfo*        pDecInfo;
    PhysicalAddress paraBuffer;
    vpu_buffer_t    vb;
    Uint32          val;
    int             i;
    BYTE            frameAddr[MAX_GDI_IDX][3][4];
    BYTE            colMvAddr[MAX_GDI_IDX][4];

    pCodecInst = instance;
    pDecInfo   = &pCodecInst->CodecInfo->decInfo;

    vdi_get_common_memory(pCodecInst->coreIdx, &vb);
    paraBuffer = vb.phys_addr + CODE_BUF_SIZE + TEMP_BUF_SIZE;

    pDecInfo->mapCfg.productId = pCodecInst->productId;

    if (pDecInfo->mapType != LINEAR_FRAME_MAP && pDecInfo->mapType != LINEAR_FIELD_MAP) {
        val = SetTiledMapType(pCodecInst->coreIdx, &pDecInfo->mapCfg, pDecInfo->mapType, (pDecInfo->stride > pDecInfo->frameBufferHeight)?pDecInfo->stride:pDecInfo->frameBufferHeight, 
            pDecInfo->openParam.cbcrInterleave, &pDecInfo->dramCfg);
    } 
    else {
        val = SetTiledMapType(pCodecInst->coreIdx, &pDecInfo->mapCfg, pDecInfo->mapType,pDecInfo->stride, pDecInfo->openParam.cbcrInterleave, &pDecInfo->dramCfg);      
    }

    if (val == 0) {
        return RETCODE_INVALID_PARAM;
    }

    //Allocate frame buffer
    for (i=0; i<pDecInfo->numFbsForDecoding; i++) {
        frameAddr[i][0][0] = (pDecInfo->frameBufPool[i].bufY  >> 24) & 0xFF;
        frameAddr[i][0][1] = (pDecInfo->frameBufPool[i].bufY  >> 16) & 0xFF;
        frameAddr[i][0][2] = (pDecInfo->frameBufPool[i].bufY  >>  8) & 0xFF;
        frameAddr[i][0][3] = (pDecInfo->frameBufPool[i].bufY  >>  0) & 0xFF;
        if (pDecInfo->openParam.cbcrOrder == CBCR_ORDER_NORMAL) {
            frameAddr[i][1][0] = (pDecInfo->frameBufPool[i].bufCb >> 24) & 0xFF;
            frameAddr[i][1][1] = (pDecInfo->frameBufPool[i].bufCb >> 16) & 0xFF;
            frameAddr[i][1][2] = (pDecInfo->frameBufPool[i].bufCb >>  8) & 0xFF;
            frameAddr[i][1][3] = (pDecInfo->frameBufPool[i].bufCb >>  0) & 0xFF;
            frameAddr[i][2][0] = (pDecInfo->frameBufPool[i].bufCr >> 24) & 0xFF;
            frameAddr[i][2][1] = (pDecInfo->frameBufPool[i].bufCr >> 16) & 0xFF;
            frameAddr[i][2][2] = (pDecInfo->frameBufPool[i].bufCr >>  8) & 0xFF;
            frameAddr[i][2][3] = (pDecInfo->frameBufPool[i].bufCr >>  0) & 0xFF;
        }
        else {
            frameAddr[i][2][0] = (pDecInfo->frameBufPool[i].bufCb >> 24) & 0xFF;
            frameAddr[i][2][1] = (pDecInfo->frameBufPool[i].bufCb >> 16) & 0xFF;
            frameAddr[i][2][2] = (pDecInfo->frameBufPool[i].bufCb >>  8) & 0xFF;
            frameAddr[i][2][3] = (pDecInfo->frameBufPool[i].bufCb >>  0) & 0xFF;
            frameAddr[i][1][0] = (pDecInfo->frameBufPool[i].bufCr >> 24) & 0xFF;
            frameAddr[i][1][1] = (pDecInfo->frameBufPool[i].bufCr >> 16) & 0xFF;
            frameAddr[i][1][2] = (pDecInfo->frameBufPool[i].bufCr >>  8) & 0xFF;
            frameAddr[i][1][3] = (pDecInfo->frameBufPool[i].bufCr >>  0) & 0xFF;
        }       
    }

    VpuWriteMem(pCodecInst->coreIdx, paraBuffer, (BYTE*)frameAddr, sizeof(frameAddr), VDI_BIG_ENDIAN); 

    // MV allocation and register
    if (pCodecInst->codecMode == AVC_DEC || pCodecInst->codecMode == VC1_DEC || pCodecInst->codecMode == MP4_DEC ||
        pCodecInst->codecMode == RV_DEC || pCodecInst->codecMode == AVS_DEC)
    {
        int size_mvcolbuf;
        vpu_buffer_t vbBuffer;
        size_mvcolbuf =  ((pDecInfo->initialInfo.picWidth+31)&~31)*((pDecInfo->initialInfo.picHeight+31)&~31);
        size_mvcolbuf = (size_mvcolbuf*3)/2;
        size_mvcolbuf = (size_mvcolbuf+4)/5;
        size_mvcolbuf = ((size_mvcolbuf+7)/8)*8;
        vbBuffer.size  = size_mvcolbuf;
        vbBuffer.phys_addr = 0;
        for (i=0  ; i<pDecInfo->numFbsForDecoding ; i++) {
            if (vdi_allocate_dma_memory(pCodecInst->coreIdx, &vbBuffer)<0){
                return RETCODE_FAILURE;
            }
            pDecInfo->vbMV[i] = vbBuffer;
        }
        if (pCodecInst->codecMode == AVC_DEC) {
            for (i=0; i<pDecInfo->numFbsForDecoding; i++) {
                colMvAddr[i][0] = (pDecInfo->vbMV[i].phys_addr >> 24) & 0xFF;
                colMvAddr[i][1] = (pDecInfo->vbMV[i].phys_addr >> 16) & 0xFF;
                colMvAddr[i][2] = (pDecInfo->vbMV[i].phys_addr >>  8) & 0xFF;
                colMvAddr[i][3] = (pDecInfo->vbMV[i].phys_addr >>  0) & 0xFF;
            }
        }
        else {
            colMvAddr[0][0] = (pDecInfo->vbMV[0].phys_addr >> 24) & 0xFF;
            colMvAddr[0][1] = (pDecInfo->vbMV[0].phys_addr >> 16) & 0xFF;
            colMvAddr[0][2] = (pDecInfo->vbMV[0].phys_addr >>  8) & 0xFF;
            colMvAddr[0][3] = (pDecInfo->vbMV[0].phys_addr >>  0) & 0xFF;
        }
        VpuWriteMem(pCodecInst->coreIdx, paraBuffer+384, (BYTE*)colMvAddr, sizeof(colMvAddr), VDI_BIG_ENDIAN);
    }

    if (pCodecInst->productId == PRODUCT_ID_980) {
        for (i=0; i<pDecInfo->numFbsForDecoding; i++) {
            frameAddr[i][0][0] = (pDecInfo->frameBufPool[i].bufYBot  >> 24) & 0xFF;
            frameAddr[i][0][1] = (pDecInfo->frameBufPool[i].bufYBot  >> 16) & 0xFF;
            frameAddr[i][0][2] = (pDecInfo->frameBufPool[i].bufYBot  >>  8) & 0xFF;
            frameAddr[i][0][3] = (pDecInfo->frameBufPool[i].bufYBot  >>  0) & 0xFF;
            if (pDecInfo->openParam.cbcrOrder == CBCR_ORDER_NORMAL) {
                frameAddr[i][1][0] = (pDecInfo->frameBufPool[i].bufCbBot >> 24) & 0xFF;
                frameAddr[i][1][1] = (pDecInfo->frameBufPool[i].bufCbBot >> 16) & 0xFF;
                frameAddr[i][1][2] = (pDecInfo->frameBufPool[i].bufCbBot >>  8) & 0xFF;
                frameAddr[i][1][3] = (pDecInfo->frameBufPool[i].bufCbBot >>  0) & 0xFF;
                frameAddr[i][2][0] = (pDecInfo->frameBufPool[i].bufCrBot >> 24) & 0xFF;
                frameAddr[i][2][1] = (pDecInfo->frameBufPool[i].bufCrBot >> 16) & 0xFF;
                frameAddr[i][2][2] = (pDecInfo->frameBufPool[i].bufCrBot >>  8) & 0xFF;
                frameAddr[i][2][3] = (pDecInfo->frameBufPool[i].bufCrBot >>  0) & 0xFF;
            }
            else {
                frameAddr[i][2][0] = (pDecInfo->frameBufPool[i].bufCbBot >> 24) & 0xFF;
                frameAddr[i][2][1] = (pDecInfo->frameBufPool[i].bufCbBot >> 16) & 0xFF;
                frameAddr[i][2][2] = (pDecInfo->frameBufPool[i].bufCbBot >>  8) & 0xFF;
                frameAddr[i][2][3] = (pDecInfo->frameBufPool[i].bufCbBot >>  0) & 0xFF;
                frameAddr[i][1][0] = (pDecInfo->frameBufPool[i].bufCrBot >> 24) & 0xFF;
                frameAddr[i][1][1] = (pDecInfo->frameBufPool[i].bufCrBot >> 16) & 0xFF;
                frameAddr[i][1][2] = (pDecInfo->frameBufPool[i].bufCrBot >>  8) & 0xFF;
                frameAddr[i][1][3] = (pDecInfo->frameBufPool[i].bufCrBot >>  0) & 0xFF;
            }
        }

        VpuWriteMem(pCodecInst->coreIdx, paraBuffer+384+128, (BYTE*)frameAddr, sizeof(frameAddr), VDI_BIG_ENDIAN);

        if (pDecInfo->wtlEnable) {
            int num = pDecInfo->numFbsForDecoding; /* start index of WTL fb array */
            int end = pDecInfo->numFrameBuffers;
            for (i=num; i<end; i++) {
                frameAddr[i-num][0][0] = (pDecInfo->frameBufPool[i].bufY  >> 24) & 0xFF;
                frameAddr[i-num][0][1] = (pDecInfo->frameBufPool[i].bufY  >> 16) & 0xFF;
                frameAddr[i-num][0][2] = (pDecInfo->frameBufPool[i].bufY  >>  8) & 0xFF;
                frameAddr[i-num][0][3] = (pDecInfo->frameBufPool[i].bufY  >>  0) & 0xFF;
                if (pDecInfo->openParam.cbcrOrder == CBCR_ORDER_NORMAL) {
                    frameAddr[i-num][1][0] = (pDecInfo->frameBufPool[i].bufCb >> 24) & 0xFF;
                    frameAddr[i-num][1][1] = (pDecInfo->frameBufPool[i].bufCb >> 16) & 0xFF;
                    frameAddr[i-num][1][2] = (pDecInfo->frameBufPool[i].bufCb >>  8) & 0xFF;
                    frameAddr[i-num][1][3] = (pDecInfo->frameBufPool[i].bufCb >>  0) & 0xFF;
                    frameAddr[i-num][2][0] = (pDecInfo->frameBufPool[i].bufCr >> 24) & 0xFF;
                    frameAddr[i-num][2][1] = (pDecInfo->frameBufPool[i].bufCr >> 16) & 0xFF;
                    frameAddr[i-num][2][2] = (pDecInfo->frameBufPool[i].bufCr >>  8) & 0xFF;
                    frameAddr[i-num][2][3] = (pDecInfo->frameBufPool[i].bufCr >>  0) & 0xFF;
                }
                else {
                    frameAddr[i-num][2][0] = (pDecInfo->frameBufPool[i].bufCb >> 24) & 0xFF;
                    frameAddr[i-num][2][1] = (pDecInfo->frameBufPool[i].bufCb >> 16) & 0xFF;
                    frameAddr[i-num][2][2] = (pDecInfo->frameBufPool[i].bufCb >>  8) & 0xFF;
                    frameAddr[i-num][2][3] = (pDecInfo->frameBufPool[i].bufCb >>  0) & 0xFF;
                    frameAddr[i-num][1][0] = (pDecInfo->frameBufPool[i].bufCr >> 24) & 0xFF;
                    frameAddr[i-num][1][1] = (pDecInfo->frameBufPool[i].bufCr >> 16) & 0xFF;
                    frameAddr[i-num][1][2] = (pDecInfo->frameBufPool[i].bufCr >>  8) & 0xFF;
                    frameAddr[i-num][1][3] = (pDecInfo->frameBufPool[i].bufCr >>  0) & 0xFF;
                }
            }

            VpuWriteMem(pCodecInst->coreIdx, paraBuffer+384+128+384, (BYTE*)frameAddr, sizeof(frameAddr), VDI_BIG_ENDIAN);          

            if (pDecInfo->wtlMode == FF_FIELD ) {
                for (i=num; i<num*2; i++) {
                    frameAddr[i-num][0][0] = (pDecInfo->frameBufPool[i].bufYBot  >> 24) & 0xFF;
                    frameAddr[i-num][0][1] = (pDecInfo->frameBufPool[i].bufYBot  >> 16) & 0xFF;
                    frameAddr[i-num][0][2] = (pDecInfo->frameBufPool[i].bufYBot  >>  8) & 0xFF;
                    frameAddr[i-num][0][3] = (pDecInfo->frameBufPool[i].bufYBot  >>  0) & 0xFF;
                    if (pDecInfo->openParam.cbcrOrder == CBCR_ORDER_NORMAL) {
                        frameAddr[i-num][1][0] = (pDecInfo->frameBufPool[i].bufCbBot >> 24) & 0xFF;
                        frameAddr[i-num][1][1] = (pDecInfo->frameBufPool[i].bufCbBot >> 16) & 0xFF;
                        frameAddr[i-num][1][2] = (pDecInfo->frameBufPool[i].bufCbBot >>  8) & 0xFF;
                        frameAddr[i-num][1][3] = (pDecInfo->frameBufPool[i].bufCbBot >>  0) & 0xFF;
                        frameAddr[i-num][2][0] = (pDecInfo->frameBufPool[i].bufCrBot >> 24) & 0xFF;
                        frameAddr[i-num][2][1] = (pDecInfo->frameBufPool[i].bufCrBot >> 16) & 0xFF;
                        frameAddr[i-num][2][2] = (pDecInfo->frameBufPool[i].bufCrBot >>  8) & 0xFF;
                        frameAddr[i-num][2][3] = (pDecInfo->frameBufPool[i].bufCrBot >>  0) & 0xFF;
                    }
                    else {
                        frameAddr[i-num][2][0] = (pDecInfo->frameBufPool[i].bufCbBot >> 24) & 0xFF;
                        frameAddr[i-num][2][1] = (pDecInfo->frameBufPool[i].bufCbBot >> 16) & 0xFF;
                        frameAddr[i-num][2][2] = (pDecInfo->frameBufPool[i].bufCbBot >>  8) & 0xFF;
                        frameAddr[i-num][2][3] = (pDecInfo->frameBufPool[i].bufCbBot >>  0) & 0xFF;
                        frameAddr[i-num][1][0] = (pDecInfo->frameBufPool[i].bufCrBot >> 24) & 0xFF;
                        frameAddr[i-num][1][1] = (pDecInfo->frameBufPool[i].bufCrBot >> 16) & 0xFF;
                        frameAddr[i-num][1][2] = (pDecInfo->frameBufPool[i].bufCrBot >>  8) & 0xFF;
                        frameAddr[i-num][1][3] = (pDecInfo->frameBufPool[i].bufCrBot >>  0) & 0xFF;
                    }
                }
                VpuWriteMem(pCodecInst->coreIdx, paraBuffer+384+128+384+384, (BYTE*)frameAddr, sizeof(frameAddr), VDI_BIG_ENDIAN);          
            }
        }
    }
    else {
        if (pDecInfo->wtlEnable) {
            int num = pDecInfo->numFbsForDecoding; /* start index of WTL fb array */
            int end = pDecInfo->numFrameBuffers;
            for (i=num; i<end; i++) {
                frameAddr[i-num][0][0] = (pDecInfo->frameBufPool[i].bufY  >> 24) & 0xFF;
                frameAddr[i-num][0][1] = (pDecInfo->frameBufPool[i].bufY  >> 16) & 0xFF;
                frameAddr[i-num][0][2] = (pDecInfo->frameBufPool[i].bufY  >>  8) & 0xFF;
                frameAddr[i-num][0][3] = (pDecInfo->frameBufPool[i].bufY  >>  0) & 0xFF;
                if (pDecInfo->openParam.cbcrOrder == CBCR_ORDER_NORMAL) {
                    frameAddr[i-num][1][0] = (pDecInfo->frameBufPool[i].bufCb >> 24) & 0xFF;
                    frameAddr[i-num][1][1] = (pDecInfo->frameBufPool[i].bufCb >> 16) & 0xFF;
                    frameAddr[i-num][1][2] = (pDecInfo->frameBufPool[i].bufCb >>  8) & 0xFF;
                    frameAddr[i-num][1][3] = (pDecInfo->frameBufPool[i].bufCb >>  0) & 0xFF;
                    frameAddr[i-num][2][0] = (pDecInfo->frameBufPool[i].bufCr >> 24) & 0xFF;
                    frameAddr[i-num][2][1] = (pDecInfo->frameBufPool[i].bufCr >> 16) & 0xFF;
                    frameAddr[i-num][2][2] = (pDecInfo->frameBufPool[i].bufCr >>  8) & 0xFF;
                    frameAddr[i-num][2][3] = (pDecInfo->frameBufPool[i].bufCr >>  0) & 0xFF;
                }
                else {
                    frameAddr[i-num][2][0] = (pDecInfo->frameBufPool[i].bufCb >> 24) & 0xFF;
                    frameAddr[i-num][2][1] = (pDecInfo->frameBufPool[i].bufCb >> 16) & 0xFF;
                    frameAddr[i-num][2][2] = (pDecInfo->frameBufPool[i].bufCb >>  8) & 0xFF;
                    frameAddr[i-num][2][3] = (pDecInfo->frameBufPool[i].bufCb >>  0) & 0xFF;
                    frameAddr[i-num][1][0] = (pDecInfo->frameBufPool[i].bufCr >> 24) & 0xFF;
                    frameAddr[i-num][1][1] = (pDecInfo->frameBufPool[i].bufCr >> 16) & 0xFF;
                    frameAddr[i-num][1][2] = (pDecInfo->frameBufPool[i].bufCr >>  8) & 0xFF;
                    frameAddr[i-num][1][3] = (pDecInfo->frameBufPool[i].bufCr >>  0) & 0xFF;
                }
            }

            VpuWriteMem(pCodecInst->coreIdx, paraBuffer+384+128+384, (BYTE*)frameAddr, sizeof(frameAddr), VDI_BIG_ENDIAN);
        }
    }

    if (!ConfigSecAXICoda9(pCodecInst->coreIdx, pCodecInst->codecMode, &pDecInfo->secAxiInfo, pDecInfo->initialInfo.picWidth, 
        pDecInfo->frameBufferHeight, pDecInfo->initialInfo.profile&0xff)) {
            return RETCODE_INSUFFICIENT_RESOURCE;
    }


    // Tell the decoder how much frame buffers were allocated.
    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_BUF_NUM,         pDecInfo->numFrameBuffers);
    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_BUF_STRIDE,      pDecInfo->stride);
    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_AXI_BIT_ADDR,    pDecInfo->secAxiInfo.u.coda9.bufBitUse);
    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_AXI_IPACDC_ADDR, pDecInfo->secAxiInfo.u.coda9.bufIpAcDcUse);
    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_AXI_DBKY_ADDR,   pDecInfo->secAxiInfo.u.coda9.bufDbkYUse);
    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_AXI_DBKC_ADDR,   pDecInfo->secAxiInfo.u.coda9.bufDbkCUse);
    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_AXI_OVL_ADDR,    pDecInfo->secAxiInfo.u.coda9.bufOvlUse);
    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_AXI_BTP_ADDR,    pDecInfo->secAxiInfo.u.coda9.bufBtpUse);
    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_DELAY,           pDecInfo->frameDelay);

    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_CACHE_CONFIG, pDecInfo->cacheConfig.type2.CacheMode);  

    if (pCodecInst->codecMode == VPX_DEC) {
        vpu_buffer_t    *pvbSlice = &pDecInfo->vbSlice;
        if (pvbSlice->size == 0) {
            pvbSlice->size = VP8_MB_SAVE_SIZE;
            if (vdi_allocate_dma_memory(pCodecInst->coreIdx, pvbSlice) < 0) {
                return RETCODE_INSUFFICIENT_RESOURCE;
            }
        }
        VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_MB_BUF_BASE, pvbSlice->phys_addr);
    }

    if (pCodecInst->codecMode == AVC_DEC) {
        vpu_buffer_t    *pvbSlice = &pDecInfo->vbSlice;
        if (pvbSlice->size == 0) {
            pvbSlice->size = SLICE_SAVE_SIZE;
            if (vdi_allocate_dma_memory(pCodecInst->coreIdx, pvbSlice) < 0) {
                return RETCODE_INSUFFICIENT_RESOURCE;
            }
        }
        VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_SLICE_BB_START, pvbSlice->phys_addr);
        VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_SLICE_BB_SIZE, (pvbSlice->size/1024));
    }

    if (pCodecInst->productId == PRODUCT_ID_980) {
        val = 0;
        val |= (pDecInfo->openParam.bwbEnable<<15);
        val |= (pDecInfo->wtlMode<<17)|(pDecInfo->tiled2LinearMode<<13)|(pDecInfo->mapType<<9)|(FORMAT_420<<6);         
        val |= ((pDecInfo->openParam.cbcrInterleave)<<2); // Interleave bit position is modified
        val |= pDecInfo->openParam.frameEndian;
        VpuWriteReg(pCodecInst->coreIdx, BIT_FRAME_MEM_CTRL, val);
    }
    else if (pCodecInst->productId == PRODUCT_ID_960) {
        val = 0;
        val |= (pDecInfo->wtlEnable<<17);
        val |= (pDecInfo->openParam.bwbEnable<<12);
        if (pDecInfo->mapType) {
            if (pDecInfo->mapType == TILED_FRAME_MB_RASTER_MAP || pDecInfo->mapType == TILED_FIELD_MB_RASTER_MAP)
                val |= (pDecInfo->tiled2LinearEnable<<11)|(0x03<<9)|(FORMAT_420<<6);    
            else
                val |= (pDecInfo->tiled2LinearEnable<<11)|(0x02<<9)|(FORMAT_420<<6);    
        }
        val |= ((pDecInfo->openParam.cbcrInterleave)<<2); // Interleave bit position is modified
        val |= pDecInfo->openParam.frameEndian;
        VpuWriteReg(pCodecInst->coreIdx, BIT_FRAME_MEM_CTRL, val);
    }
    else {
        return RETCODE_NOT_FOUND_VPU_DEVICE;
    }

    VpuWriteReg(pCodecInst->coreIdx, CMD_SET_FRAME_MAX_DEC_SIZE, 0);
    Coda9BitIssueCommand(pCodecInst->coreIdx, pCodecInst, SET_FRAME_BUF);
    if (vdi_wait_vpu_busy(pCodecInst->coreIdx, __VPU_BUSY_TIMEOUT, BIT_BUSY_FLAG) == -1) {
        if (pCodecInst->loggingEnable)
            vdi_log(pCodecInst->coreIdx, SET_FRAME_BUF, 2);
        return RETCODE_VPU_RESPONSE_TIMEOUT;
    }
    if (pCodecInst->loggingEnable)
        vdi_log(pCodecInst->coreIdx, SET_FRAME_BUF, 0);

    if (VpuReadReg(pCodecInst->coreIdx, RET_SET_FRAME_SUCCESS) & (1UL<<31)) {
        return RETCODE_MEMORY_ACCESS_VIOLATION;
    }

    return RETCODE_SUCCESS;
}

RetCode Coda9VpuDecFlush(CodecInst* instance, FramebufferIndex* framebufferIndexes, Uint32 size)
{
    Uint32   i;
    DecInfo* pDecInfo = &instance->CodecInfo->decInfo;

    Coda9BitIssueCommand(instance->coreIdx, instance, DEC_BUF_FLUSH);
    if (vdi_wait_vpu_busy(instance->coreIdx, __VPU_BUSY_TIMEOUT, BIT_BUSY_FLAG) == -1) {
        return RETCODE_VPU_RESPONSE_TIMEOUT;
    }

    pDecInfo->frameDisplayFlag = 0;

    if (framebufferIndexes != NULL) {
        for (i=0; i<size; i++) {
            framebufferIndexes[i].linearIndex = -2;
            framebufferIndexes[i].tiledIndex  = -2;
        }
    }

    return RETCODE_SUCCESS;
}

 
