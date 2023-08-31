#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include <errno.h>
#include <signal.h>
#include "mi_common.h"
#include "mi_sys.h"
#include "mi_vdec.h"
#include "mi_divp_datatype.h"
#include "mi_divp.h"
#include "mi_venc_datatype.h"
#include "mi_venc.h"
#include "md5.h"

#if (1)
typedef struct MI_VDEC_CropCfg_s
{
    MI_BOOL bEnable;            /* Crop region enable */
    MI_SYS_WindowRect_t stRect; /* Crop region */
} MI_VDEC_CropCfg_t;

MI_S32 MI_VDEC_SetDestCrop(MI_VDEC_CHN VdecChn, MI_VDEC_CropCfg_t *pstCropCfg)
{
    return MI_SUCCESS;
}

MI_S32 MI_VDEC_GetDestCrop(MI_VDEC_CHN VdecChn, MI_VDEC_CropCfg_t *pstCropCfg)
{
    return MI_SUCCESS;
}
#endif

#define VDEC_CHN_MAX (33)
#define MI_U32VALUE(pu8Data, index) \
    (pu8Data[index] << 24) | (pu8Data[index + 1] << 16) | (pu8Data[index + 2] << 8) | (pu8Data[index + 3])
#define VESFILE_READER_BATCH (1024 * 1024)

#define INPUT_CMD_MAX_LEN 32
#define LINE_BUF_MAX_LEN 256
#define LINE_PARAM_MAX_NUM 20
#define WAIT_TIMEOUT 6000              // 6s
#define WAIT_CAPTURE_INPUT_TIMEOUT 100 // 100ms

#define TEST_ALIGN_UP(val, alignment) ((((val) + (alignment)-1) / (alignment)) * (alignment))
#define TEST_ALIGN_DOWN(val, alignment) (((val) / (alignment)) * (alignment))

#define MAX_TEST_CASE (1024)

#define ExecFunc(_func_, _ret_)                         \
    if (_func_ != _ret_)                                \
    {                                                   \
        printf("[%d]exec function failed\n", __LINE__); \
        return 1;                                       \
    }                                                   \
    else                                                \
    {                                                   \
        printf("(%d)exec function pass\n", __LINE__);   \
    }

typedef enum
{
    E_PARAM_TYPE_INITPARAM,
    E_PARAM_TYPE_VDECATTR,
    E_PARAM_TYPE_VDECPARAM,
    E_PARAM_TYPE_MAX
} Param_Type_e;

typedef struct Dev_Info_s
{
    MI_U32 u32MaxChnNum;
    MI_U32 au32MaxChnConfNum[E_PARAM_TYPE_MAX];
    MI_BOOL bInitDev;
    MI_BOOL bDisableLowLatency;
} Dev_Info_t;

typedef struct Chn_AttrInfo_s
{
    MI_VDEC_CHN VdecChn;
    MI_U16 u16Width;
    MI_U16 u16Height;
    MI_U32 u32BufSize;
    MI_U32 u32FrmRate;
    MI_U32 u32RefFrameNum;
    MI_VDEC_CodecType_e eCodecType;
    MI_VDEC_VideoMode_e eVideoMode;
    MI_VDEC_DPB_BufMode_e eDpbBufMode;
    MI_U16 u16ScaleW;
    MI_U16 u16ScaleH;
    MI_U8 au8FilePath[128];
    MI_U32 u32DecFrmCount;
    MI_U8 au8MD5Sum[33];
} Chn_AttrInfo_t;

typedef struct Chn_ParamInfo_s
{
    MI_VDEC_CHN VdecChn;
    MI_BOOL bConfig;
    MI_VDEC_CropCfg_t stCropCfg;

} Chn_ParamInfo_t;

// file config
typedef struct Test_Cfg_s
{
    MI_U32 u32Index;
    MI_U8 au8Tile[256];
    Param_Type_e eParamType;
    Dev_Info_t stDevInfo;
    Chn_AttrInfo_t stChnAttrInfo[VDEC_CHN_MAX];
    Chn_ParamInfo_t stChnParamInfo[VDEC_CHN_MAX];
} Test_Cfg_t;

typedef struct PushStreamThreadParam_s
{
    Test_Cfg_t *pstTestCfg;
    MI_U32 u32ChnIdx;
    MI_BOOL bParamTransDone;
} PushStreamThreadParam_t;

static Test_Cfg_t _astTestCfg[MAX_TEST_CASE];
static MI_U32 _u32TestCastCount  = 0;
static MI_BOOL _bAutoTestRunFlag = FALSE;
static MI_U32 _u32RunTestCaseIdx = 0;
static pthread_t _thrPush[VDEC_CHN_MAX];
static pthread_t _thrGet[VDEC_CHN_MAX];
static MI_BOOL _bRun[VDEC_CHN_MAX];
static FILE *_hFile[VDEC_CHN_MAX];
static MI_BOOL _bStop = FALSE;

#define _CHECKPOINT_ printf("xxxxxxxxx [%s][%d] xxxxxxxx\n", __FUNCTION__, __LINE__);

MI_U64 get_pts(MI_U32 u32FrameRate)
{
    if (0 == u32FrameRate)
    {
        return (MI_U64)(-1);
    }

    return (MI_U64)(1000 / u32FrameRate);
}

#define MAX_CHECK_FRAME (5)
static MD5_CTX astMD5Ctx[VDEC_CHN_MAX];

static void check_frame_md5sum(MI_VDEC_CHN VdecChn, MI_SYS_BufInfo_t *pstBufInfo, MI_U8 u8MD5Count)
{
    /// only IP frame case, excep B frame
    MI_SYS_FrameData_t *pstFrmData = &pstBufInfo->stFrameData;
    MI_U8 *pYuv                    = NULL;
    int i                          = 0;

    pYuv = (MI_U8 *)pstFrmData->pVirAddr[0];
    for (i = 0; i < pstFrmData->u16Height; ++i)
    {
        MD5Update(&astMD5Ctx[VdecChn], pYuv, pstFrmData->u16Width);
        pYuv += pstFrmData->u32Stride[0];
    }
}

static MI_U8 string_to_int8(MI_U8 *p)
{
    MI_U8 out = 0;

    if (*p <= '9')
    {
        out = *p - '0';
    }
    else
    {
        out = 10 + *p - 'a';
    }

    p++;
    out <<= 4;

    if (*p <= '9')
    {
        out += (*p - '0');
    }
    else
    {
        out += (10 + *p - 'a');
    }

    return out;
}

void *push_stream(void *args)
{
    MI_S32 s32Ret = MI_SUCCESS;

    MI_S32 u32Index = 0;
    MI_VDEC_CHN VdecChn;
    MI_U32 u32FrmRate = 0;
    MI_VDEC_VideoMode_e eVideoMode;
    MI_U8 *pu8Buf       = NULL;
    MI_U32 u32Len       = 0;
    MI_U32 u32FrameLen  = 0;
    MI_U64 u64Pts       = 0;
    MI_U8 au8Header[16] = {0};
    MI_VDEC_ChnStat_t stChnStat;
    PushStreamThreadParam_t *pstThrParam = (PushStreamThreadParam_t *)args;
    MI_VDEC_VideoStream_t stVdecStream;
    MI_S32 s32TimeOutMs = 20;
    MI_U32 u32MD5Count  = 0;

    MI_U32 u32FpBackLen = 0; // if send stream failed, file pointer back length

    u32Index   = pstThrParam->u32ChnIdx;
    VdecChn    = pstThrParam->pstTestCfg->stChnAttrInfo[u32Index].VdecChn;
    eVideoMode = pstThrParam->pstTestCfg->stChnAttrInfo[u32Index].eVideoMode;
    u32FrmRate = pstThrParam->pstTestCfg->stChnAttrInfo[u32Index].u32FrmRate;
    /// after all param done, main thread need to exit
    pstThrParam->bParamTransDone = TRUE;

    pu8Buf = (MI_U8 *)malloc(VESFILE_READER_BATCH);
    printf("chn(%d) %s\n", VdecChn, (E_MI_VDEC_VIDEO_MODE_STREAM == eVideoMode) ? "stream" : "frame");

    while (_bRun[VdecChn])
    {
        usleep(1000 / u32FrmRate * 1000);

        if (_bStop)
        {
            usleep(10 * 1000);
            continue;
        }

        if (E_MI_VDEC_VIDEO_MODE_STREAM == eVideoMode)
        {
            /// stream mode, read 128k every time
            u32FrameLen = VESFILE_READER_BATCH;
            fseek(_hFile[VdecChn], 0L, SEEK_CUR);
        }
        else
        {
            /// frame mode, read one frame lenght every time
            memset(au8Header, 0, 16);
            fseek(_hFile[VdecChn], 0L, SEEK_CUR);
            u32Len = fread(au8Header, 16, 1, _hFile[VdecChn]);
            if (u32Len <= 0)
            {
                fseek(_hFile[VdecChn], 0, SEEK_SET);
                continue;
            }

            u32FrameLen = MI_U32VALUE(au8Header, 4);
            if (u32FrameLen > VESFILE_READER_BATCH)
            {
                fseek(_hFile[VdecChn], 0, SEEK_SET);
                continue;
            }
        }

        u32Len = fread(pu8Buf, u32FrameLen, 1, _hFile[VdecChn]);
        if (u32Len <= 0)
        {
            fseek(_hFile[VdecChn], 0, SEEK_SET);
            continue;
        }

        stVdecStream.pu8Addr      = pu8Buf;
        stVdecStream.u32Len       = u32FrameLen;
        stVdecStream.u64PTS       = u64Pts;
        stVdecStream.bEndOfFrame  = 1;
        stVdecStream.bEndOfStream = 0;

        u32FpBackLen = stVdecStream.u32Len + 16; // back length

#if 0
        printf("%s, %d, VdecChn: %d, pu8Addr: %p, u32Len: %u, u64PTS: %llu.\n", __FUNCTION__, __LINE__,
            VdecChn,
            stVdecStream.pu8Addr,
            stVdecStream.u32Len,
            stVdecStream.u64PTS);
#endif

        if (MI_SUCCESS != (s32Ret = MI_VDEC_SendStream(VdecChn, &stVdecStream, s32TimeOutMs)))
        {
            // ST_ERR("MI_VDEC_SendStream fail, chn:%d, 0x%X\n", vdecChn, s32Ret);
            fseek(_hFile[VdecChn], -u32FpBackLen, SEEK_CUR);
            continue;
        }

        u64Pts = u64Pts + get_pts(30);

        memset(&stChnStat, 0x0, sizeof(stChnStat));
        if (_bAutoTestRunFlag && (MI_SUCCESS == MI_VDEC_GetChnStat(VdecChn, &stChnStat)))
        {
            if (stChnStat.u32DecodeStreamFrames)
            {
                MI_SYS_ChnPort_t stChnPort;
                MI_SYS_BufInfo_t stBufInfo;
                MI_SYS_BUF_HANDLE hBuf;

                memset(&stChnPort, 0x0, sizeof(stChnPort));
                stChnPort.eModId    = E_MI_MODULE_ID_VDEC;
                stChnPort.u32DevId  = 0;
                stChnPort.u32ChnId  = VdecChn;
                stChnPort.u32PortId = 0;
                memset(&stBufInfo, 0x0, sizeof(stBufInfo));
                /// get frame and calculate md5
                if (MI_SUCCESS == MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &hBuf))
                {
                    check_frame_md5sum(VdecChn, &stBufInfo, u32MD5Count++);
                    MI_SYS_ChnOutputPortPutBuf(hBuf);
                    if (u32MD5Count >= pstThrParam->pstTestCfg->stChnAttrInfo[VdecChn].u32DecFrmCount)
                    {
                        printf("chn:%d, check md5sum done, exit push stream thread, count:%d\n", VdecChn, u32MD5Count);
                        break;
                    }
                }
            }
        }
        // printf("Chn(%d)_%s_Codec:%d, Frame Dec:%d\n", VdecChn, _aeCodecType[VdecChn], stChnStat.eCodecType,
        // stChnStat.u32DecodeStreamFrames);
    }

    printf("[info]chn:%d push stream end\n", VdecChn);
    free(pu8Buf);
    _bRun[VdecChn] = FALSE;
    return NULL;
}

void *get_meta_data(void *p)
{
    MI_SYS_ChnPort_t stChnPort;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE hSysBuf;
    MI_U32 u32CheckSum = 0;

    MI_VDEC_CHN VdecChn = (MI_VDEC_CHN)p;
    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId    = E_MI_MODULE_ID_VDEC;
    stChnPort.u32DevId  = 0;
    stChnPort.u32ChnId  = VdecChn;
    stChnPort.u32PortId = 0;

    MI_SYS_SetChnOutputPortDepth(&stChnPort, 2, 4);
    while (_bRun[VdecChn])
    {
        hSysBuf = MI_HANDLE_NULL;
        usleep(30 * 1000);
        memset(&stBufInfo, 0x0, sizeof(MI_SYS_BufInfo_t));
        if (MI_SUCCESS != MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &hSysBuf))
        {
            continue;
        }

        if (stBufInfo.eBufType != E_MI_SYS_BUFDATA_META)
        {
            printf("error eBufType:%d\n", stBufInfo.eBufType);
        }
        else
        {
            MI_U32 i = 0;
            char *p  = NULL;

            if (0)
            {
                printf("Chn(%d), Bufino(bEndOfStream:%d, bUsrBuf:%d, eBufType:%d, Pts:%lld)\n", VdecChn, stBufInfo.bEndOfStream,
                       stBufInfo.bUsrBuf, stBufInfo.eBufType, stBufInfo.u64Pts);
            }

            if (0)
            {
                printf("Chn(%d), eDataFromModule:%d, phyAddr:0x%llx, pVirAddr:0x%p, u32Size:%d\n", VdecChn,
                       stBufInfo.stMetaData.eDataFromModule, stBufInfo.stMetaData.phyAddr, stBufInfo.stMetaData.pVirAddr,
                       stBufInfo.stMetaData.u32Size);
            }

            p           = (char *)(stBufInfo.stMetaData.pVirAddr);
            u32CheckSum = 0;
            for (i = 0; i < stBufInfo.stMetaData.u32Size; ++i)
            {
                u32CheckSum += p[i];
            }
            printf("Chn(%d), pts(%lld) u32CheckSum:%d\n", VdecChn, stBufInfo.u64Pts, u32CheckSum);
        }

        if (MI_SUCCESS != MI_SYS_ChnOutputPortPutBuf(hSysBuf))
        {
            continue;
        }
    }

    return NULL;
}

void save_yuv420_data(MI_U32 u32Chn, MI_SYS_FrameData_t *pstFrameData)
{
    FILE *fp = NULL;
    char aName[128];
    MI_U32 u32Width             = pstFrameData->u16Width;
    MI_U32 u32Height            = pstFrameData->u16Height;
    MI_U32 u32Pitch             = pstFrameData->u32Stride[0];
    MI_U32 u32YLen              = 0;
    MI_U32 u32UvLen             = 0;
    static MI_U32 u32Frmcnt[33] = {0};

    if (u32Frmcnt[u32Chn] > 2)
    {
        // printf("Save frame finished, total: %d.\n", u32Frmcnt[u32Chn]);
        return;
    }
    memset(aName, 0x0, sizeof(aName));
    sprintf(aName, "/tmp/app_chn_%d_dump_vdec[%d_%d_%d]_%d_nv12.yuv", u32Chn, u32Width, u32Height, u32Pitch, u32Frmcnt[u32Chn]);

    fp = fopen(aName, "wb+");
    if (!fp)
    {
        printf("Open File Faild\n");
        return;
    }

    // Y buffer
    if (pstFrameData->pVirAddr[0])
    {
        fseek(fp, 0, SEEK_SET);
        u32YLen = u32Pitch * u32Height;
        if (fwrite(pstFrameData->pVirAddr[0], 1, u32YLen, fp) != u32YLen)
        {
            printf("Chn %d Y buffer fwrite %s failed\n", u32Chn, aName);
            goto _END;
        }
    }
    else
    {
        printf("Chn %d Y buffer is NULL.\n", u32Chn);
    }

    // UV buffer
    if (pstFrameData->pVirAddr[1])
    {
        u32UvLen = u32Pitch * u32Height / 2;
        fseek(fp, u32YLen, SEEK_SET);
        if (fwrite(pstFrameData->pVirAddr[1], 1, u32UvLen, fp) != u32UvLen)
        {
            printf("Chn %d UV buffer fwrite %s failed\n", u32Chn, aName);
            goto _END;
        }
    }
    else
    {
        printf("Chn %d UV buffer is NULL.\n", u32Chn);
    }

    printf("dump file(%s) ok ..............[len:%d]\n", aName, u32YLen + u32UvLen);
    u32Frmcnt[u32Chn]++;

_END:
    fclose(fp);
    fp = NULL;
}

void *get_frame_data(void *args)
{
    MI_SYS_ChnPort_t stChnPort;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE hSysBuf;

    MI_VDEC_CHN VdecChn = (MI_VDEC_CHN)args;
    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId    = E_MI_MODULE_ID_VDEC;
    stChnPort.u32DevId  = 0;
    stChnPort.u32ChnId  = VdecChn;
    stChnPort.u32PortId = 0;

    MI_SYS_SetChnOutputPortDepth(&stChnPort, 2, 4);
    while (_bRun[VdecChn])
    {
        hSysBuf = MI_HANDLE_NULL;
        usleep(2000);

        memset(&stBufInfo, 0x0, sizeof(MI_SYS_BufInfo_t));
        if (MI_SUCCESS != MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &hSysBuf))
        {
            continue;
        }

        if (stBufInfo.eBufType != E_MI_SYS_BUFDATA_FRAME)
        {
            printf("error eBufType:%d\n", stBufInfo.eBufType);
        }

        {
            // MI_U32 i = 0;
            // char *p = NULL;

            // printf("Chn(%d), Bufino(bEndOfStream:%d, bUsrBuf:%d, eBufType:%d, Pts:%lld)\n",
            //    VdecChn,
            //    stBufInfo.bEndOfStream,
            //    stBufInfo.bUsrBuf,
            //    stBufInfo.eBufType,
            //    stBufInfo.u64Pts);

            if (0)
            {
                printf("Chn(%d), Pixel:%d, phyAddr:0x%llx, pVirAddr:%p, W:%d, H:%d, BufSize:%u.\n", VdecChn,
                       stBufInfo.stFrameData.ePixelFormat, stBufInfo.stFrameData.phyAddr[0], stBufInfo.stFrameData.pVirAddr[0],
                       stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height, stBufInfo.stFrameData.u32BufSize);
            }

            if (0)
            {
                save_yuv420_data(VdecChn, &stBufInfo.stFrameData);
            }

            // p = (char *)(stBufInfo.stFrameData.pVirAddr);
            // u32CheckSum = 0;
            // for (i = 0; i < stBufInfo.stFrameData.u32BufSize; ++i)
            //{
            //    u32CheckSum += p[i];
            //}
            // printf("Chn(%d), pts(%lld) u32CheckSum:%d\n", VdecChn, stBufInfo.u64Pts, u32CheckSum);
        }

        if (MI_SUCCESS != MI_SYS_ChnOutputPortPutBuf(hSysBuf))
        {
            continue;
        }
    }

    return NULL;
}

void save_yuv422_data(MI_U8 *pYuv422Data, MI_U32 u32Width, MI_U32 u32Height, MI_U32 u32Pitch, MI_U32 u32Chn)
{
    FILE *fp = NULL;
    char aName[128];
    MI_U32 u32Length            = u32Width * u32Height * 2;
    static MI_U32 u32Frmcnt[33] = {0};

    if (u32Frmcnt[u32Chn] > 5)
    {
        printf("get frame count:%d\n", u32Frmcnt[u32Chn]);
        return;
    }
    memset(aName, 0x0, sizeof(aName));
    sprintf(aName, "/mnt/app_chn_%d_jpeg_dump_vdec[%d_%d_%d]_%d.yuv", u32Chn, u32Width, u32Height, u32Pitch, u32Frmcnt[u32Chn]);

    fp = fopen(aName, "wb+");
    if (!fp)
    {
        printf("Open File Faild\n");
        return;
    }

    fseek(fp, 0, SEEK_SET);
    if (fwrite(pYuv422Data, 1, u32Length, fp) != u32Length)
    {
        printf("fwrite %s failed\n", aName);
        goto _END;
    }

    printf("dump file(%s) ok ..............[len:%d]\n", aName, u32Length);
    u32Frmcnt[u32Chn]++;

_END:
    fclose(fp);
    fp = NULL;
}

#if 0
void *get_frame_data(void* args)
{
    MI_SYS_ChnPort_t stChnPort;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE hSysBuf;
    MI_U32 u32CheckSum = 0;
    MI_VDEC_CHN VdecChn = 0;

    memcpy(&stChnPort, args, sizeof(MI_SYS_ChnPort_t));
    VdecChn = stChnPort.u32ChnId;
    MI_SYS_SetChnOutputPortDepth(&stChnPort,5,20);
    while (_bRun[VdecChn])
    {
        hSysBuf = MI_HANDLE_NULL;
        //usleep(1000 / _u8FrameRate * 1000);
        memset(&stBufInfo, 0x0, sizeof(MI_SYS_BufInfo_t));
        if (MI_SUCCESS != MI_SYS_ChnOutputPortGetBuf(&stChnPort, &stBufInfo, &hSysBuf))
        {
            continue;
        }

        if (stBufInfo.eBufType != E_MI_SYS_BUFDATA_FRAME)
        {
            printf("error eBufType:%d\n", stBufInfo.eBufType);
        }
#if 0
        else
        {
            if(strncmp(_aeCodecType[VdecChn], "jpeg", 4) == 0)
            {
                MI_U32 u32BufSize = stBufInfo.stFrameData.u16Width * stBufInfo.stFrameData.u16Height * 2;
                //get buffer virtual address
                void *pvirFramAddr = NULL;
                if(MI_SYS_Mmap(stBufInfo.stFrameData.phyAddr[0], u32BufSize, &pvirFramAddr, TRUE))
                {
                    printf(" MI_SYS_Mmap failed!.\n");
                }
                else
                {
                    printf("pvirFramAddr = %p. \n", pvirFramAddr);
                }
                stBufInfo.stFrameData.pVirAddr[0] = pvirFramAddr;
            }
            save_yuv422_data(stBufInfo.stFrameData.pVirAddr[0], stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height, stBufInfo.stFrameData.u32Stride[0], VdecChn);
        }
#endif

        if (MI_SUCCESS != MI_SYS_ChnOutputPortPutBuf(hSysBuf))
        {
            continue;
        }
    }

    return NULL;
}
#endif

void st_create_push_stream_thread(MI_S32 u32Index, MI_BOOL bCreateGetThead, MI_VDEC_ChnAttr_t *pstChnAttr,
                                  Test_Cfg_t *pstTestCfg)
{
    MI_VDEC_CHN VdecChn = 0;
    FILE *readfp        = NULL;
    PushStreamThreadParam_t stThrdParam;

    VdecChn = pstTestCfg->stChnAttrInfo[u32Index].VdecChn;

    printf("\033[1;32m"
           "fopen file:%s\n"
           "\033[0m",
           pstTestCfg->stChnAttrInfo[u32Index].au8FilePath);

    memset(&stThrdParam, 0x0, sizeof(stThrdParam));
    stThrdParam.pstTestCfg = pstTestCfg;
    stThrdParam.u32ChnIdx  = u32Index;
    readfp                 = fopen((const char *)pstTestCfg->stChnAttrInfo[u32Index].au8FilePath, "rb");
    if (readfp)
    {
        _hFile[VdecChn]             = readfp;
        _bRun[VdecChn]              = TRUE;
        stThrdParam.bParamTransDone = FALSE;
        if (pthread_create(&_thrPush[VdecChn], NULL, push_stream, &stThrdParam))
        {
            assert(0);
        }

        if (!bCreateGetThead)
        {
            return;
        }

        if (pstTestCfg->stDevInfo.bDisableLowLatency)
        {
            // if (pthread_create(&_thrGet[VdecChn], NULL, get_meta_data, VdecChn))
            //{
            //    assert(0);
            //}
        }
        else
        {
            // if (pthread_create(&_thrGet[VdecChn], NULL, get_frame_data, VdecChn))
            //{
            //    assert(0);
            //}
        }

        /// need to wait thread create and run done
        while (!stThrdParam.bParamTransDone)
        {
            printf("wait thread create and run done...\n");
            usleep(10);
        }
    }
    else
    {
        printf("\033[1;31m"
               "VdecChn %d fopen File:%s Error, errno: %s.\n"
               "\033[0m",
               VdecChn, pstTestCfg->stChnAttrInfo[u32Index].au8FilePath, strerror(errno));
    }
}

void st_destroy_push_stream_thread(MI_VDEC_CHN VdecChn)
{
    if (_bRun[VdecChn])
    {
        _bRun[VdecChn] = FALSE;
        if (_thrPush[VdecChn])
        {
            pthread_join(_thrPush[VdecChn], NULL);
        }

        if (_thrGet[VdecChn])
        {
            pthread_join(_thrGet[VdecChn], NULL);
        }
    }

    if (_hFile[VdecChn])
    {
        fclose(_hFile[VdecChn]);
        _hFile[VdecChn] = NULL;
    }
}

int st_start_vdec(MI_U32 u32TCaseIdx)
{
    MI_U32 i      = 0;
    MI_U32 j      = 0;
    MI_S32 s32Ret = E_MI_ERR_FAILED;
    MI_VDEC_ChnAttr_t stChnAttr;
    MI_VDEC_CHN VdecChn     = 0;
    MI_BOOL bCreateGetThead = TRUE;
    MI_SYS_ChnPort_t stChnPort;
    MI_VDEC_InitParam_t stVdecInitParam;

    memset(_thrPush, 0x0, sizeof(_thrPush));
    memset(_thrGet, 0x0, sizeof(_thrGet));
    memset(_bRun, 0x0, sizeof(_bRun));
    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    memset(&stVdecInitParam, 0x0, sizeof(MI_VDEC_InitParam_t));

    if (_astTestCfg[u32TCaseIdx].stDevInfo.bInitDev)
    {
        stVdecInitParam.bDisableLowLatency = _astTestCfg[u32TCaseIdx].stDevInfo.bDisableLowLatency;
        MI_VDEC_InitDev(&stVdecInitParam);
    }

    for (i = 0; i < _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum; i++)
    {
        memset(&stChnAttr, 0x0, sizeof(MI_VDEC_ChnAttr_t));

        VdecChn                                  = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].VdecChn;
        stChnAttr.eCodecType                     = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].eCodecType;
        stChnAttr.u32PicWidth                    = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16Width;
        stChnAttr.u32PicHeight                   = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16Height;
        stChnAttr.eVideoMode                     = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].eVideoMode;
        stChnAttr.u32BufSize                     = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u32BufSize;
        stChnAttr.eDpbBufMode                    = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].eDpbBufMode;
        stChnAttr.stVdecVideoAttr.u32RefFrameNum = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u32RefFrameNum;
        stChnAttr.u32Priority                    = 0;

        printf("create chn(%d) codec:%d\n", VdecChn, stChnAttr.eCodecType);

        ExecFunc(MI_VDEC_CreateChn(VdecChn, &stChnAttr), MI_SUCCESS);
        ExecFunc(MI_VDEC_StartChn(VdecChn), MI_SUCCESS);
        if (_astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16ScaleW != 0
            && _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16ScaleH != 0)
        {
            MI_VDEC_OutputPortAttr_t stOutputPortAttr;
            memset(&stOutputPortAttr, 0x0, sizeof(MI_VDEC_OutputPortAttr_t));
            stOutputPortAttr.u16Width  = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16ScaleW;
            stOutputPortAttr.u16Height = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16ScaleH;

            ExecFunc(MI_VDEC_SetOutputPortAttr(VdecChn, &stOutputPortAttr), MI_SUCCESS);
        }

        for (j = 0; j < _astTestCfg[u32TCaseIdx].stDevInfo.au32MaxChnConfNum[E_PARAM_TYPE_VDECPARAM]; j++)
        {
            if (_astTestCfg[u32TCaseIdx].stChnParamInfo[j].VdecChn == VdecChn
                && _astTestCfg[u32TCaseIdx].stChnParamInfo[j].bConfig)
            {
                MI_VDEC_CropCfg_t stCropCfg;
                memset(&stCropCfg, 0x0, sizeof(MI_VDEC_CropCfg_t));
                memcpy(&stCropCfg, &_astTestCfg[u32TCaseIdx].stChnParamInfo[j].stCropCfg, sizeof(MI_VDEC_CropCfg_t));

                ExecFunc(MI_VDEC_SetDestCrop(VdecChn, &stCropCfg), MI_SUCCESS);
                break;
            }
        }

        stChnPort.eModId    = E_MI_MODULE_ID_VDEC;
        stChnPort.u32DevId  = 0;
        stChnPort.u32ChnId  = VdecChn;
        stChnPort.u32PortId = 0;

        if (_bAutoTestRunFlag)
        {
            MD5Init(&astMD5Ctx[VdecChn]);
            MI_SYS_SetChnOutputPortDepth(&stChnPort, 4, 4);
        }
        else
        {
            MI_SYS_SetChnOutputPortDepth(&stChnPort, 2, 4);
        }

        st_create_push_stream_thread(i, bCreateGetThead, &stChnAttr, &_astTestCfg[u32TCaseIdx]);
    }

    return MI_SUCCESS;
}

int st_stop_vdec(MI_U32 u32TCaseIdx)
{
    MI_U32 i = 0;

    for (i = 0; i < _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum; i++)
    {
        MI_VDEC_CHN VdecChn = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].VdecChn;

        st_destroy_push_stream_thread(VdecChn);
        ExecFunc(MI_VDEC_StopChn(VdecChn), MI_SUCCESS);
        ExecFunc(MI_VDEC_DestroyChn(VdecChn), MI_SUCCESS);
    }

    return MI_SUCCESS;
}

void st_save_initparam_info(Test_Cfg_t *pstTestCfg, MI_U8 **pstCfg)
{
    MI_S32 j = 0;

    pstTestCfg->stDevInfo.bInitDev = TRUE;

    // bDisableLowLatency
    if (strlen((const char *)pstCfg[j++]))
    {
        pstTestCfg->stDevInfo.bDisableLowLatency = (MI_BOOL)atoi((const char *)pstCfg[j - 1]);
    }
    else
    {
        pstTestCfg->stDevInfo.bDisableLowLatency = FALSE;
    }

    return;
}

void st_save_vdecattr_info(Test_Cfg_t *pstTestCfg, MI_U8 **pstCfg)
{
    MI_S32 j           = 0;
    MI_S32 k           = 0;
    MI_S8 *ps8Arg      = NULL;
    MI_U16 u16ChnStart = 0;
    MI_U16 u16ChnEnd   = 0;
    MI_U32 u32Index    = 0;

    // parse VdecChn
    if (strlen((const char *)pstCfg[j++]))
    {
        ps8Arg = (MI_S8 *)strtok((char *)pstCfg[j - 1], "-");
        if (!ps8Arg)
        {
            u16ChnStart = (MI_U16)atoi((const char *)pstCfg[j - 1]);
            u16ChnEnd   = u16ChnStart;
        }
        else
        {
            u16ChnStart = (MI_U16)atoi((const char *)ps8Arg);
            ps8Arg      = (MI_S8 *)strtok(NULL, "-");
            if (!ps8Arg)
            {
                u16ChnEnd = u16ChnStart;
            }
            else
            {
                u16ChnEnd = (MI_U16)atoi((const char *)ps8Arg);
                if (u16ChnEnd < u16ChnStart)
                {
                    printf("[%s][%d]: vdec chn config error.\n", __FUNCTION__, __LINE__);
                    return;
                }
            }
        }
    }
    else
    {
        printf("[%s][%d]: vdec chn not config.\n", __FUNCTION__, __LINE__);
        return;
    }

    for (k = u16ChnStart; k <= u16ChnEnd; k++)
    {
        j        = 1;
        u32Index = pstTestCfg->stDevInfo.au32MaxChnConfNum[E_PARAM_TYPE_VDECATTR];

        // VdecChn
        pstTestCfg->stChnAttrInfo[u32Index].VdecChn = k;

        // Type
        if (0 == strncasecmp((const char *)pstCfg[j++], "h264", strlen("h264")))
        {
            pstTestCfg->stChnAttrInfo[u32Index].eCodecType = E_MI_VDEC_CODEC_TYPE_H264;
        }
        else if (0 == strncasecmp((const char *)pstCfg[j - 1], "h265", strlen("h265")))
        {
            pstTestCfg->stChnAttrInfo[u32Index].eCodecType = E_MI_VDEC_CODEC_TYPE_H265;
        }
        else
        {
            pstTestCfg->stChnAttrInfo[u32Index].eCodecType = (MI_VDEC_CodecType_e)atoi((const char *)pstCfg[j - 1]);
        }

        // Width
        pstTestCfg->stChnAttrInfo[u32Index].u16Width = (MI_U16)atoi((const char *)pstCfg[j++]);

        // Height
        pstTestCfg->stChnAttrInfo[u32Index].u16Height = (MI_U16)atoi((const char *)pstCfg[j++]);

        // VideoMode
        if (strlen((const char *)pstCfg[j++]))
        {
            if (0 == strncasecmp((const char *)pstCfg[j - 1], "frame", strlen("frame")))
            {
                pstTestCfg->stChnAttrInfo[u32Index].eVideoMode = E_MI_VDEC_VIDEO_MODE_FRAME;
            }
            else if (0 == strncasecmp((const char *)pstCfg[j - 1], "stream", strlen("stream")))
            {
                pstTestCfg->stChnAttrInfo[u32Index].eVideoMode = E_MI_VDEC_VIDEO_MODE_STREAM;
            }
            else
            {
                pstTestCfg->stChnAttrInfo[u32Index].eVideoMode = (MI_VDEC_VideoMode_e)atoi((const char *)pstCfg[j - 1]);
            }
        }
        else // default value
        {
            pstTestCfg->stChnAttrInfo[u32Index].eVideoMode = E_MI_VDEC_VIDEO_MODE_FRAME;
        }

        // u32BufSize
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnAttrInfo[u32Index].u32BufSize = (MI_U32)atoi((const char *)pstCfg[j - 1]);
        }
        else // default value
        {
            pstTestCfg->stChnAttrInfo[u32Index].u32BufSize = 1048576;
        }

        // eDpbBufMode
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnAttrInfo[u32Index].eDpbBufMode = (MI_VDEC_DPB_BufMode_e)atoi((const char *)pstCfg[j - 1]);
        }
        else // default value
        {
            pstTestCfg->stChnAttrInfo[u32Index].eDpbBufMode = E_MI_VDEC_DPB_MODE_NORMAL;
        }

        // maxRefNum
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnAttrInfo[u32Index].u32RefFrameNum = (MI_U32)atoi((const char *)pstCfg[j - 1]);
        }
        else // default value
        {
            pstTestCfg->stChnAttrInfo[u32Index].u32RefFrameNum = 2;
        }

        // FrameRate
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnAttrInfo[u32Index].u32FrmRate = (MI_U32)atoi((const char *)pstCfg[j - 1]);
        }
        else // default value
        {
            pstTestCfg->stChnAttrInfo[u32Index].u32FrmRate = 30;
        }

        // ScaleW
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnAttrInfo[u32Index].u16ScaleW = (MI_U16)atoi((const char *)pstCfg[j - 1]);
        }
        else // default value
        {
            pstTestCfg->stChnAttrInfo[u32Index].u16ScaleW = 0;
        }

        // ScaleH
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnAttrInfo[u32Index].u16ScaleH = (MI_U16)atoi((const char *)pstCfg[j - 1]);
        }
        else // default value
        {
            pstTestCfg->stChnAttrInfo[u32Index].u16ScaleH = 0;
        }

        // Path
        if (strlen((const char *)pstCfg[j++]))
        {
            snprintf((char *)pstTestCfg->stChnAttrInfo[u32Index].au8FilePath, 128, "%s", pstCfg[j - 1]);
        }
        else // default value
        {
            printf("[%s][%d]: vdec chn %d not config stream file path.\n", __FUNCTION__, __LINE__, k);
            return;
        }

        pstTestCfg->stDevInfo.au32MaxChnConfNum[E_PARAM_TYPE_VDECATTR]++;
    }
}

void st_save_vdecparam_info(Test_Cfg_t *pstTestCfg, MI_U8 **pstCfg)
{
    MI_S32 j           = 0;
    MI_S32 k           = 0;
    MI_S8 *ps8Arg      = NULL;
    MI_U16 u16ChnStart = 0;
    MI_U16 u16ChnEnd   = 0;
    MI_U32 u32Index    = 0;

    // parse VdecChn
    if (strlen((const char *)pstCfg[j++]))
    {
        ps8Arg = (MI_S8 *)strtok((char *)pstCfg[j - 1], "-");
        if (!ps8Arg)
        {
            u16ChnStart = (MI_U16)atoi((const char *)pstCfg[j - 1]);
            u16ChnEnd   = u16ChnStart;
        }
        else
        {
            u16ChnStart = (MI_U16)atoi((const char *)ps8Arg);
            ps8Arg      = (MI_S8 *)strtok(NULL, "-");
            if (!ps8Arg)
            {
                u16ChnEnd = u16ChnStart;
            }
            else
            {
                u16ChnEnd = (MI_U16)atoi((const char *)ps8Arg);
                if (u16ChnEnd < u16ChnStart)
                {
                    printf("[%s][%d]: vdec chn config error.\n", __FUNCTION__, __LINE__);
                    return;
                }
            }
        }
    }
    else
    {
        printf("[%s][%d]: vdec chn not config.\n", __FUNCTION__, __LINE__);
        return;
    }

    for (k = u16ChnStart; k <= u16ChnEnd; k++)
    {
        j        = 1;
        u32Index = pstTestCfg->stDevInfo.au32MaxChnConfNum[E_PARAM_TYPE_VDECPARAM];

        // VdecChn
        pstTestCfg->stChnParamInfo[u32Index].VdecChn = k;

        // bEnable
        if (0 == strncasecmp((const char *)pstCfg[j++], "Y", strlen("Y")))
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.bEnable = TRUE;
        }
        else if (0 == strncasecmp((const char *)pstCfg[j - 1], "N", strlen("N")))
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.bEnable = FALSE;
        }
        else
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.bEnable = (MI_BOOL)atoi((const char *)pstCfg[j - 1]);
        }

        // CropX
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.stRect.u16X = (MI_U16)atoi((const char *)pstCfg[j - 1]);
        }
        else
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.stRect.u16X = 0;
        }

        // CropY
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.stRect.u16Y = (MI_U16)atoi((const char *)pstCfg[j - 1]);
        }
        else
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.stRect.u16Y = 0;
        }

        // CropW
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.stRect.u16Width = (MI_U16)atoi((const char *)pstCfg[j - 1]);
        }
        else // default value
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.stRect.u16Width = 0;
        }

        // CropH
        if (strlen((const char *)pstCfg[j++]))
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.stRect.u16Height = (MI_U16)atoi((const char *)pstCfg[j - 1]);
        }
        else // default value
        {
            pstTestCfg->stChnParamInfo[u32Index].stCropCfg.stRect.u16Height = 0;
        }

        pstTestCfg->stChnParamInfo[u32Index].bConfig = TRUE;
        pstTestCfg->stDevInfo.au32MaxChnConfNum[E_PARAM_TYPE_VDECPARAM]++;
    }
}

int st_save_param(Test_Cfg_t *pstTestCfg, MI_U8 **pstCfg)
{
    if (!pstTestCfg)
    {
        printf("%s[%d]: pstTestCfg is NULL.\n", __FUNCTION__, __LINE__);
    }
    if (!pstCfg)
    {
        printf("%s[%d]: pstCfg is NULL.\n", __FUNCTION__, __LINE__);
    }

    // parse header
    if (E_PARAM_TYPE_INITPARAM == pstTestCfg->eParamType)
    {
        st_save_initparam_info(pstTestCfg, pstCfg);
    }
    else if (E_PARAM_TYPE_VDECATTR == pstTestCfg->eParamType)
    {
        st_save_vdecattr_info(pstTestCfg, pstCfg);
    }
    else if (E_PARAM_TYPE_VDECPARAM == pstTestCfg->eParamType)
    {
        st_save_vdecparam_info(pstTestCfg, pstCfg);
    }

    return 0;
}

char *st_ltrim(char *str)
{
    if (!str || *str == '\0')
    {
        return str;
    }

    int len = 0;
    char *p = str;
    while (*p != '\0' && isspace(*p))
    {
        ++p;
        ++len;
    }

    memmove(str, p, strlen((const char *)str) - len + 1);

    return str;
}

char *st_rtrim(char *str)
{
    if (!str || *str == '\0')
    {
        return str;
    }

    int len = strlen((const char *)str);
    char *p = str + len - 1;
    while (p >= str && isspace(*p))
    {
        *p = '\0';
        --p;
    }

    return str;
}

void st_config_default_value(void)
{
    MI_U32 i = 0;
    memset(&_astTestCfg, 0, sizeof(_astTestCfg));
    for (i = 0; i < sizeof(_astTestCfg) / sizeof(_astTestCfg[0]); ++i)
    {
        _astTestCfg[i].eParamType         = E_PARAM_TYPE_MAX;
        _astTestCfg[i].stDevInfo.bInitDev = FALSE;
    }
}

#define START_INDEX "case/> <Index: "
#define END_INDEX "/case>"
int st_parse_script_file(MI_S8 *pFileName)
{
    MI_S32 ret  = 0;
    FILE *pFile = NULL;
    MI_U8 LineBufTmp[LINE_BUF_MAX_LEN];
    char *pLineBufTmp                 = NULL;
    char *pLineBuf                    = NULL;
    MI_U32 u32ParamIndex              = 0;
    MI_U8 *pstCfg[LINE_PARAM_MAX_NUM] = {NULL};

    pFile = fopen((const char *)pFileName, "r");
    if (!pFile)
    {
        printf("fopen file %s failed\n", pFileName);
        return -1;
    }

    memset(LineBufTmp, 0, LINE_BUF_MAX_LEN);
    memset(_astTestCfg, 0, sizeof(_astTestCfg));
    _u32TestCastCount = 0;

    MI_U8 *pStartCmd = NULL;
    MI_U8 *pEndCmd   = NULL;
    while (NULL != fgets((char *)LineBufTmp, LINE_BUF_MAX_LEN, pFile))
    {
        if (!pStartCmd)
        {
            /// get test case start pos
            pStartCmd = (MI_U8 *)strstr((const char *)LineBufTmp, START_INDEX);
            if (!pStartCmd)
            {
                continue;
            }

            /// get test case number
            pStartCmd += strlen((const char *)START_INDEX);
            pEndCmd = (MI_U8 *)strchr((const char *)pStartCmd, ',');
            if (!pEndCmd)
            {
                continue;
            }
            *pEndCmd                                = '\0';
            _astTestCfg[_u32TestCastCount].u32Index = atoi((const char *)pStartCmd);
            /// get test case title
            pStartCmd = pEndCmd + 1;
            pEndCmd   = (MI_U8 *)strchr((const char *)pStartCmd, '>');
            memset(_astTestCfg[_u32TestCastCount].au8Tile, 0x0, sizeof(_astTestCfg[_u32TestCastCount].au8Tile));
            if (pEndCmd)
            {
                *pEndCmd = '\0';
                strncpy((char *)_astTestCfg[_u32TestCastCount].au8Tile, (const char *)pStartCmd,
                        sizeof(_astTestCfg[_u32TestCastCount].au8Tile) - 1);
            }
            continue;
        }
        else
        {
            /// check case end
            if (strstr((const char *)LineBufTmp, END_INDEX))
            {
                _u32TestCastCount++;
                pStartCmd = NULL;
                continue;
            }
        }

        /// parse one test case
        char *p = NULL;
        char *q = NULL;

        // Remove a blank space from the beginning and end of a row
        pLineBufTmp = (char *)st_ltrim((char *)LineBufTmp);
        pLineBuf    = (char *)st_rtrim((char *)pLineBufTmp);

        // skip blank line
        if (pLineBuf[0] == '\0' || pLineBuf[0] == '\n' || pLineBuf[0] == '\r' || 0 == strlen(pLineBuf))
        {
            memset(LineBufTmp, 0, LINE_BUF_MAX_LEN);
            continue;
        }

        // ignore a line if there is a "//" or "#" at the beginning of a line
        if (strstr(pLineBuf, "//") || strstr(pLineBuf, "#"))
        {
            memset(LineBufTmp, 0, LINE_BUF_MAX_LEN);
            continue;
        }

        // find "=" in a line
        if (NULL != (p = strtok(pLineBuf, "=")))
        {
            if (0 == strncasecmp(p, "VDEC", strlen("VDEC")))
            {
                _astTestCfg[_u32TestCastCount].eParamType = E_PARAM_TYPE_VDECATTR;
            }
            else if (0 == strncasecmp(p, "INITPARAM", strlen("INITPARAM")))
            {
                _astTestCfg[_u32TestCastCount].eParamType = E_PARAM_TYPE_INITPARAM;
            }
            else if (0 == strncasecmp(p, "VDECPARAM", strlen("VDECPARAM")))
            {
                _astTestCfg[_u32TestCastCount].eParamType = E_PARAM_TYPE_VDECPARAM;
            }
            else
            {
                printf("[%s][%d]: Script file config error.\n", __FUNCTION__, __LINE__);
                return -1;
            }

            p = strtok(NULL, "=");
            if (!p)
            {
                printf("[%s][%d]: Script file config error.\n", __FUNCTION__, __LINE__);
                return -1;
            }
        }
        else
        {
            printf("[%s][%d]: Script file config error.\n", __FUNCTION__, __LINE__);
            return -1;
        }

        q = strtok(p, ",");
        while (NULL != q)
        {
            char *tmp = NULL;

            tmp                   = (char *)st_ltrim((char *)q);
            pstCfg[u32ParamIndex] = (MI_U8 *)st_rtrim((char *)tmp);

            q = strtok(NULL, ",");
            u32ParamIndex++;
        }

        ret = st_save_param(&_astTestCfg[_u32TestCastCount], pstCfg);
        if (ret < 0)
        {
            printf("[%s][%d]: vdec set param error.\n", __FUNCTION__, __LINE__);
            return -1;
        }

        u32ParamIndex = 0;
        memset(LineBufTmp, 0, LINE_BUF_MAX_LEN);

        _astTestCfg[_u32TestCastCount].stDevInfo.u32MaxChnNum =
            _astTestCfg[_u32TestCastCount].stDevInfo.au32MaxChnConfNum[E_PARAM_TYPE_VDECATTR];
        if (_astTestCfg[_u32TestCastCount].stDevInfo.u32MaxChnNum
            < _astTestCfg[_u32TestCastCount].stDevInfo.au32MaxChnConfNum[E_PARAM_TYPE_VDECPARAM])
        {
            printf("[%s][%d]: Script file config error.\n", __FUNCTION__, __LINE__);
            printf("[%s][%d]: Input param num(%d) should not be more than MaxChnNum(%d).\n", __FUNCTION__, __LINE__,
                   _astTestCfg[_u32TestCastCount].stDevInfo.au32MaxChnConfNum[E_PARAM_TYPE_VDECPARAM],
                   _astTestCfg[_u32TestCastCount].stDevInfo.u32MaxChnNum);
        }
    }

    return MI_SUCCESS;
}

int st_read_input_timeout(int maxfdp, unsigned int wait_ms)
{
    int ret = 0;

    if (wait_ms > 0)
    {
        fd_set read_fdset;
        struct timeval timeout;

        FD_ZERO(&read_fdset);
        FD_SET(maxfdp, &read_fdset);

        timeout.tv_sec  = 0;
        timeout.tv_usec = wait_ms * 1000;

        do
        {
            ret = select(maxfdp + 1, &read_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        // check timeout
        if (ret == 0)
        {
            ret   = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            ret = 0;
        }
    }

    return ret;
}

#define CAPTURE_PATH "/vendor/dump/out/capture"
void st_capture_SaveStreamToFile(MI_VENC_Stream_t *pstStream)
{
    MI_U32 i = 0;
    char szFileName[128];
    time_t now              = 0;
    struct tm *tm           = NULL;
    MI_U32 len              = 0;
    int fd                  = 0;
    MI_VENC_Pack_t *pstPack = NULL;

    memset(szFileName, 0, sizeof(szFileName));

    now = time(NULL);
    tm  = localtime(&now);
    len = 0;

    struct stat st;
    if (stat(CAPTURE_PATH, &st) == -1)
    {
        if (mkdir(CAPTURE_PATH, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1)
        {
            return;
        }
    }

    len += sprintf(szFileName + len, "%s/", CAPTURE_PATH);
    len += sprintf(szFileName + len, "%d_", tm->tm_year + 1900);
    len += sprintf(szFileName + len, "%02d_", tm->tm_mon);
    len += sprintf(szFileName + len, "%02d-", tm->tm_mday);
    len += sprintf(szFileName + len, "%02d_", tm->tm_hour);
    len += sprintf(szFileName + len, "%02d_", tm->tm_min);
    len += sprintf(szFileName + len, "%02d", tm->tm_sec);
    len += sprintf(szFileName + len, ".%s", "jpg");

    fd = open(szFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd <= 0)
    {
        // ST_WARN("create file %s error\n", szFileName);
    }
    else
    {
        // ST_DBG("open %s success\n", szFileName);
    }

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstPack = &pstStream->pstPack[i];
        write(fd, pstPack->pu8Addr + pstPack->u32Offset, pstPack->u32Len - pstPack->u32Offset);
    }

    close(fd);
}

int st_capture_create()
{
    MI_U32 u32DevId     = 0;
    MI_VDEC_CHN VdecChn = 0; // default: 0
    MI_DIVP_CHN DivpChn = 6; // default: 6
    MI_U16 u16Width     = 0;
    MI_U16 u16Height    = 0;
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;
    MI_VENC_ChnAttr_t stChnAttr;
    MI_DIVP_ChnAttr_t stDivpChnAttr;

    /*Get W/H of vdec output image */
    {
        MI_VDEC_CropCfg_t stCropCfg;
        memset(&stCropCfg, 0x0, sizeof(MI_VDEC_CropCfg_t));

        ExecFunc(MI_VDEC_GetDestCrop(VdecChn, &stCropCfg), MI_SUCCESS);
        if (stCropCfg.bEnable)
        {
            u16Width  = stCropCfg.stRect.u16Width;
            u16Height = stCropCfg.stRect.u16Height;
        }
        else
        {
            MI_VDEC_OutputPortAttr_t stOutputPortAttr;
            memset(&stOutputPortAttr, 0x0, sizeof(MI_VDEC_OutputPortAttr_t));
            ExecFunc(MI_VDEC_GetOutputPortAttr(VdecChn, &stOutputPortAttr), MI_SUCCESS);
            if (stOutputPortAttr.u16Width != 0 && stOutputPortAttr.u16Height != 0)
            {
                u16Width  = stOutputPortAttr.u16Width;
                u16Height = stOutputPortAttr.u16Height;
            }
            else
            {
                MI_VDEC_ChnAttr_t stChnAttr;
                memset(&stChnAttr, 0x0, sizeof(MI_VDEC_ChnAttr_t));
                ExecFunc(MI_VDEC_GetChnAttr(VdecChn, &stChnAttr), MI_SUCCESS);
                u16Width  = stChnAttr.u32PicWidth;
                u16Height = stChnAttr.u32PicHeight;
            }
        }
    }
    if (0 == u16Width || 0 == u16Height)
    {
        printf("Get vdec chn(%d) output W/H(%d %d) error.\n", VdecChn, u16Width, u16Height);
        return -1;
    }

    memset(&stChnAttr, 0, sizeof(MI_VENC_ChnAttr_t));
    stChnAttr.stRcAttr.eRcMode                           = E_MI_VENC_RC_MODE_MJPEGFIXQP;
    stChnAttr.stRcAttr.stAttrMjpegFixQp.u32SrcFrmRateNum = 30;
    stChnAttr.stRcAttr.stAttrMjpegFixQp.u32SrcFrmRateDen = 30;
    stChnAttr.stVeAttr.stAttrJpeg.u32PicWidth            = u16Width;
    stChnAttr.stVeAttr.stAttrJpeg.u32PicHeight           = u16Height;
    stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicWidth         = u16Width;
    stChnAttr.stVeAttr.stAttrJpeg.u32MaxPicHeight        = u16Height;
    stChnAttr.stVeAttr.stAttrJpeg.bByFrame               = TRUE;
    stChnAttr.stVeAttr.eType                             = E_MI_VENC_MODTYPE_JPEGE;

    ExecFunc(MI_VENC_CreateChn(VdecChn, &stChnAttr), MI_SUCCESS);

    if (E_MI_VENC_MODTYPE_JPEGE == stChnAttr.stVeAttr.eType)
    {
        MI_VENC_ParamJpeg_t stParamJpeg;

        memset(&stParamJpeg, 0, sizeof(MI_VENC_ParamJpeg_t));
        ExecFunc(MI_VENC_GetJpegParam(VdecChn, &stParamJpeg), MI_SUCCESS);

        stParamJpeg.u32Qfactor = 50;
        ExecFunc(MI_VENC_SetJpegParam(VdecChn, &stParamJpeg), MI_SUCCESS);
    }

    memset(&stSrcChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stSrcChnPort.eModId    = E_MI_MODULE_ID_VDEC;
    stSrcChnPort.u32DevId  = 0;
    stSrcChnPort.u32ChnId  = VdecChn;
    stSrcChnPort.u32PortId = 1; // for capture
    ExecFunc(MI_SYS_SetChnOutputPortDepth(&stSrcChnPort, 0, 4), MI_SUCCESS);

    ExecFunc(MI_VENC_GetChnDevid(VdecChn, &u32DevId), MI_SUCCESS);
    memset(&stDstChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stDstChnPort.u32DevId  = u32DevId;
    stDstChnPort.eModId    = E_MI_MODULE_ID_VENC;
    stDstChnPort.u32ChnId  = VdecChn;
    stDstChnPort.u32PortId = 0;
    ExecFunc(MI_SYS_BindChnPort(&stSrcChnPort, &stDstChnPort, 30, 30), MI_SUCCESS);

    memset(&stDivpChnAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
    stDivpChnAttr.u32MaxWidth  = 1920;
    stDivpChnAttr.u32MaxHeight = 1920;
    stDivpChnAttr.eTnrLevel    = E_MI_DIVP_TNR_LEVEL_OFF;
    stDivpChnAttr.eDiType      = E_MI_DIVP_DI_TYPE_OFF;
    stDivpChnAttr.eRotateType  = E_MI_SYS_ROTATE_NONE;
    stDivpChnAttr.bHorMirror   = false;
    stDivpChnAttr.bVerMirror   = false;
    MI_DIVP_CreateChn(DivpChn, &stDivpChnAttr);

    return MI_SUCCESS;
}

int st_capture_get_stream()
{
    MI_S32 s32Ret                    = MI_SUCCESS;
    char InputCmd[INPUT_CMD_MAX_LEN] = {0};
    MI_VENC_CHN VencChn              = 0; // default: 0
    MI_VENC_Stream_t stStream;
    MI_VENC_Pack_t stPack;

    ExecFunc(MI_VENC_StartRecvPic(VencChn), MI_SUCCESS);

    memset(&stStream, 0, sizeof(MI_VENC_Stream_t));
    memset(&stPack, 0, sizeof(MI_VENC_Pack_t));
    stStream.pstPack      = &stPack;
    stStream.u32PackCount = 1;
    while (1)
    {
        setbuf(stdin, NULL);
        memset(InputCmd, 0, sizeof(InputCmd));
        // printf("[Stop capture?<y/Y>]:");
        if (0 == st_read_input_timeout(fileno(stdin), WAIT_CAPTURE_INPUT_TIMEOUT))
        {
            fgets((char *)(InputCmd), (sizeof(InputCmd) - 1), stdin);
            if (0 == strncasecmp(InputCmd, "y", strlen("y")))
            {
                printf("Stop capture!\n\n");
                break;
            }
            else
            {
                usleep(10 * 1000);
            }
        }

#if 1
        s32Ret = MI_VENC_GetStream(VencChn, &stStream, 40);
        if (MI_SUCCESS == s32Ret)
        {
            printf("##########Start to write file!!!##########\n");
            st_capture_SaveStreamToFile(&stStream);
            printf("###########End to write file!!!###########\n");

            ExecFunc(MI_VENC_ReleaseStream(VencChn, &stStream), MI_SUCCESS);
            break;
        }
        else
            printf("Continue, ret:0x%x!!!\n", s32Ret);
#endif
        usleep(200 * 1000);
    }

    ExecFunc(MI_VENC_StopRecvPic(VencChn), MI_SUCCESS);
    return 0;
}

int st_capture_destroy()
{
    MI_U32 u32DevId     = 0;
    MI_VENC_CHN VencChn = 0; // default: 0
    MI_DIVP_CHN DivpChn = 6; // default: 6
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;

    memset(&stSrcChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stSrcChnPort.eModId    = E_MI_MODULE_ID_VDEC;
    stSrcChnPort.u32DevId  = 0;
    stSrcChnPort.u32ChnId  = VencChn;
    stSrcChnPort.u32PortId = 1; // for capture

    ExecFunc(MI_VENC_GetChnDevid(VencChn, &u32DevId), MI_SUCCESS);
    memset(&stDstChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stDstChnPort.eModId    = E_MI_MODULE_ID_VENC;
    stDstChnPort.u32DevId  = u32DevId;
    stDstChnPort.u32ChnId  = VencChn;
    stDstChnPort.u32PortId = 0;

    ExecFunc(MI_SYS_UnBindChnPort(&stSrcChnPort, &stDstChnPort), MI_SUCCESS);
    ExecFunc(MI_VENC_DestroyChn(VencChn), MI_SUCCESS);
    MI_DIVP_DestroyChn(DivpChn);
    return 0;
}

int st_dyn_scaling(MI_U32 u32TCaseIdx)
{
    MI_U32 i           = 0;
    MI_U32 j           = 0;
    char InputCmd[256] = {0};

    MI_VDEC_CHN VdecChn = 0;
    MI_U16 u16WAlign    = 0;
    MI_U16 u16HAlign    = 0;
    MI_VDEC_OutputPortAttr_t stOutputPortAttr;

    memset(&stOutputPortAttr, 0, sizeof(MI_VDEC_OutputPortAttr_t));

    while (1)
    {
        j++;

        setbuf(stdin, NULL);
        memset(InputCmd, 0, sizeof(InputCmd));
        printf("[Stop dynamic scaling?<y/Y>]:");
        if (0 == st_read_input_timeout(fileno(stdin), WAIT_TIMEOUT))
        {
            fgets((char *)(InputCmd), (sizeof(InputCmd) - 1), stdin);
            if (0 == strncasecmp(InputCmd, "y", strlen("y")))
            {
                printf("Stop dynamic scaling!\n\n");
                break;
            }
            else
            {
                usleep(1 * 1000 * 1000);
            }
        }

        for (i = 0; i < _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum; i++)
        {
            VdecChn = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].VdecChn;

            if (E_MI_VDEC_CODEC_TYPE_H264 == _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].eCodecType)
            {
                u16WAlign = 16;
                u16HAlign = 16;
            }
            else if (E_MI_VDEC_CODEC_TYPE_H265 == _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].eCodecType)
            {
                u16WAlign = 32;
                u16HAlign = 32;
            }

            if (_astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16ScaleW != 0
                && _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16ScaleH != 0)
            {
                stOutputPortAttr.u16Width  = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16ScaleW + j * 10;
                stOutputPortAttr.u16Height = _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16ScaleH + j * 10;

                if (stOutputPortAttr.u16Width > _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16Width)
                {
                    stOutputPortAttr.u16Width =
                        TEST_ALIGN_UP(_astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16Width, u16WAlign) >> 3;
                }
                if (stOutputPortAttr.u16Height > _astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16Height)
                {
                    stOutputPortAttr.u16Height =
                        TEST_ALIGN_UP(_astTestCfg[u32TCaseIdx].stChnAttrInfo[i].u16Height, u16HAlign) >> 3;
                }

                if (MI_SUCCESS != MI_VDEC_SetOutputPortAttr(VdecChn, &stOutputPortAttr))
                {
                    printf("[%d]exec MI_VDEC_SetOutputPortAttr failed\n", __LINE__);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int st_dyn_crop(MI_U32 u32TCaseIdx)
{
    MI_U32 i = 0;
    MI_U32 j = 0;

    char InputCmd[256] = {0};

    MI_S32 s32Ret       = MI_SUCCESS;
    MI_VDEC_CHN VdecChn = 0;
    MI_U16 u16WAlign    = 0;
    MI_U16 u16HAlign    = 0;
    MI_VDEC_CropCfg_t stCropCfg;
    MI_VDEC_ChnAttr_t stChnAttr;

    while (1)
    {
        j++;

        setbuf(stdin, NULL);
        memset(InputCmd, 0, sizeof(InputCmd));
        printf("[Stop dynamic scaling?<y/Y>]:");
        if (0 == st_read_input_timeout(fileno(stdin), WAIT_TIMEOUT))
        {
            fgets((char *)(InputCmd), (sizeof(InputCmd) - 1), stdin);
            if (0 == strncasecmp(InputCmd, "y", strlen("y")))
            {
                printf("Stop dynamic scaling!\n\n");
                break;
            }
            else
            {
                usleep(1 * 1000 * 1000);
            }
        }

        for (i = 0; i < _astTestCfg[u32TCaseIdx].stDevInfo.au32MaxChnConfNum[E_PARAM_TYPE_VDECPARAM]; i++)
        {
            if (FALSE == _astTestCfg[u32TCaseIdx].stChnParamInfo[i].bConfig)
            {
                continue;
            }
            VdecChn = _astTestCfg[u32TCaseIdx].stChnParamInfo[i].VdecChn;

            u16WAlign = 16;
            u16HAlign = 2;

            memset(&stCropCfg, 0, sizeof(MI_VDEC_CropCfg_t));
            memset(&stChnAttr, 0, sizeof(MI_VDEC_ChnAttr_t));
            s32Ret = MI_VDEC_GetChnAttr(VdecChn, &stChnAttr);
            if (MI_SUCCESS != s32Ret)
            {
                printf("[%s][%d]: Chn %d get chn attr error, ret:0x%x.\n", __FUNCTION__, __LINE__, VdecChn, s32Ret);
                continue;
            }
            s32Ret = MI_VDEC_GetDestCrop(VdecChn, &stCropCfg);
            if (MI_SUCCESS != s32Ret)
            {
                printf("[%s][%d]: Chn %d get dest crop error, ret:0x%x.\n", __FUNCTION__, __LINE__, VdecChn, s32Ret);
                continue;
            }

            if (stCropCfg.bEnable)
            {
                stCropCfg.stRect.u16Width += u16WAlign;
                stCropCfg.stRect.u16Height += u16HAlign;

                if (stCropCfg.stRect.u16Width > stChnAttr.u32PicWidth
                    || stCropCfg.stRect.u16X + stCropCfg.stRect.u16Width > stChnAttr.u32PicWidth)
                {
                    stCropCfg.stRect.u16Width = TEST_ALIGN_UP(stChnAttr.u32PicWidth, u16WAlign) >> 3;
                    stCropCfg.stRect.u16Width = TEST_ALIGN_UP(stCropCfg.stRect.u16Width, u16WAlign);
                    stCropCfg.stRect.u16X += u16WAlign;
                    if (stCropCfg.stRect.u16X >= stChnAttr.u32PicWidth
                        || stCropCfg.stRect.u16X + stCropCfg.stRect.u16Width > stChnAttr.u32PicWidth)
                    {
                        stCropCfg.stRect.u16X = 0;
                    }
                }
                if (stCropCfg.stRect.u16Height > stChnAttr.u32PicHeight
                    || stCropCfg.stRect.u16Y + stCropCfg.stRect.u16Height > stChnAttr.u32PicHeight)
                {
                    stCropCfg.stRect.u16Height = TEST_ALIGN_UP(stChnAttr.u32PicHeight, u16HAlign) >> 3;
                    stCropCfg.stRect.u16Height = TEST_ALIGN_UP(stCropCfg.stRect.u16Height, u16WAlign);
                    stCropCfg.stRect.u16Y += u16HAlign;
                    if (stCropCfg.stRect.u16Y >= stChnAttr.u32PicHeight
                        || stCropCfg.stRect.u16Y + stCropCfg.stRect.u16Height > stChnAttr.u32PicHeight)
                    {
                        stCropCfg.stRect.u16Y = 0;
                    }
                }

                ExecFunc(MI_VDEC_SetDestCrop(VdecChn, &stCropCfg), MI_SUCCESS);
            }
        }
    }

    return 0;
}

int st_start_capture(MI_U32 u32TCaseIdx)
{
    st_capture_create();
    st_capture_get_stream();
    st_capture_destroy();
    return 0;
}

void st_print_menu1()
{
    printf("*****************************************************\n");
    printf("     q/Q    exit \t\n");
    printf("   scale    enable dynamic scaling \t\n");
    printf("    crop    enable dynamic crop \t\n");
    printf("    stop    Stop sending stream \t\n");
    printf("   start    Start sending stream \t\n");
    printf(" capture    Start capture \t\n");
    printf("*****************************************************\n");
    printf("cmd:");
}

int st_stop_all_chn(MI_U32 u32TCaseIdx)
{
    MI_U32 i = 0;
    for (i = 0; i < _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum; i++)
    {
        ExecFunc(MI_VDEC_StopChn(i), MI_SUCCESS);
    }

    return MI_SUCCESS;
}

int st_start_all_chn(MI_U32 u32TCaseIdx)
{
    MI_U32 i = 0;
    for (i = 0; i < _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum; i++)
    {
        ExecFunc(MI_VDEC_StartChn(i), MI_SUCCESS);
    }

    return MI_SUCCESS;
}

#define AUTO_TEST_CFG_FILE_NAME "./auto_test_config.xml"
static void test_case_info(MI_U32 u32TCaseIdx)
{
    MI_U32 j = 0;
    printf("\n============ test case:%d ==============\n", u32TCaseIdx);
    printf("u32Index:%d\n", _astTestCfg[u32TCaseIdx].u32Index);
    printf("au8Tile:%s\n", _astTestCfg[u32TCaseIdx].au8Tile);
    printf("eParamType:%d\n", _astTestCfg[u32TCaseIdx].eParamType);
    printf("u32MaxChnNum:%d\n", _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum);
    printf("bDisableLowLatency:%d\n", _astTestCfg[u32TCaseIdx].stDevInfo.bDisableLowLatency);
    for (j = 0; j < _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum; ++j)
    {
        printf("\t--------------------------------------------------------------------------------------------------------\n");
        printf("\tchn:%d.au8FilePath:%s\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].au8FilePath);
        printf("\tchn:%d.u16Width:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].u16Width);
        printf("\tchn:%d.u16Height:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].u16Height);
        printf("\tchn:%d.u32BufSize:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].u32BufSize);
        printf("\tchn:%d.u32FrmRate:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].u32FrmRate);
        printf("\tchn:%d.u32RefFrameNum:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].u32RefFrameNum);
        printf("\tchn:%d.eCodecType:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].eCodecType);
        printf("\tchn:%d.eVideoMode:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].eVideoMode);
        printf("\tchn:%d.eDpbBufMode:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].eDpbBufMode);
        printf("\tchn:%d.u16ScaleW:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].u16ScaleW);
        printf("\tchn:%d.u16ScaleH:%d\n", _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].VdecChn,
               _astTestCfg[u32TCaseIdx].stChnAttrInfo[j].u16ScaleH);
    }
}

static void all_test_case_info(void)
{
    MI_U32 i = 0;
    for (i = 0; i < _u32TestCastCount; ++i)
    {
        test_case_info(i);
    }
}

static MI_U32 check_all_test_case_result(MI_U32 u32TCaseIdx)
{
    MI_U32 i     = 0;
    MI_U32 j     = 0;
    MI_BOOL bRes = TRUE;
    MI_U8 au8MD5Value[16];

    /// wait all chanel dec thread done
    i = 0;
    while (i < _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum)
    {
        if (_bRun[i])
        {
            i = 0;
            usleep(10);
            continue;
        }
        ++i;
    }

    printf("wait all chanel dec done\n");
    for (i = 0; i < _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum; ++i)
    {
        int k = 0;

        MD5Final(&astMD5Ctx[i], au8MD5Value);
        printf("chn_%d, MD5 result:\n", i);
        printf("\tframe:");
        for (k = 0; k < 16; ++k)
        {
            printf("%02x", au8MD5Value[k]);
        }
        printf("\tfile:");
        for (k = 0; k < 16; ++k)
        {
            printf("%02x", string_to_int8(&_astTestCfg[u32TCaseIdx].stChnAttrInfo[i].au8MD5Sum[k << 1]));
        }
        printf("\n");
    }

    bRes = TRUE;
    for (i = 0; i < _astTestCfg[u32TCaseIdx].stDevInfo.u32MaxChnNum; ++i)
    {
        for (j = 0; j < 16; ++j)
        {
            if (au8MD5Value[j] != string_to_int8(&_astTestCfg[u32TCaseIdx].stChnAttrInfo[i].au8MD5Sum[j << 1]))
            {
                bRes = FALSE;
            }
        }

        if (!bRes)
        {
            break;
        }
    }

    ///mi_unit_testing_result(MI_VDEC_AUTO_TEST, DEC, _astTestCfg[u32TCaseIdx].au8Tile, (bRes ? MI_SUCCESS : MI_TEST_FAILE));
    printf("run case:%s done =================================================>>>>\n", _astTestCfg[u32TCaseIdx].au8Tile);
    return (bRes ? MI_SUCCESS : MI_TEST_FAILE);
}

static MI_S32 auto_test_dec_stream(void)
{
    MI_S32 res = 0;

    for (_u32RunTestCaseIdx = 0; _u32RunTestCaseIdx < _u32TestCastCount; ++_u32RunTestCaseIdx)
    {
        st_start_vdec(_u32RunTestCaseIdx);
        check_all_test_case_result(_u32RunTestCaseIdx);
        st_stop_vdec(_u32RunTestCaseIdx);
    }
    return res;
}

static void handle_signel(MI_S32 signo)
{
    printf("got int signo:%d\n", signo);
    if (signo == SIGINT)
    {
        printf("[error]got int sig, need to force exit test\n");
        st_stop_vdec(_u32RunTestCaseIdx);
        MI_SYS_Exit();
        exit(0);
    }
}

static void register_signel(void)
{
    struct sigaction action;

    action.sa_handler = handle_signel;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
}

static MI_S32 auto_test_install_env(void)
{
    MI_S32 res = 0;

    register_signel();

    _u32RunTestCaseIdx = 0;
    _bAutoTestRunFlag  = TRUE;
    _bStop             = FALSE;
    memset(_astTestCfg, 0x0, sizeof(_astTestCfg));
    memset(_thrPush, 0x0, sizeof(_thrPush));
    memset(_thrGet, 0x0, sizeof(_thrGet));
    memset(_bRun, 0x0, sizeof(_bRun));
    memset(_hFile, 0x0, sizeof(_hFile));

    res |= MI_SYS_Init();
    res |= st_parse_script_file((MI_S8 *)AUTO_TEST_CFG_FILE_NAME);
    all_test_case_info();
    return res;
}

static MI_S32 auto_test_uninstall_env(void)
{
    MI_S32 res = 0;

    _bStop            = TRUE;
    _bAutoTestRunFlag = FALSE;
    res               = MI_SYS_Exit();
    return res;
}

static MI_S32 auto_test_vdec(void)
{
    MI_S32 res = 0;

    auto_test_install_env();
    res = auto_test_dec_stream();
    auto_test_uninstall_env();
    return res;
}

static MI_S32 h26x_dec_stream(Vdec_Config_t *pstVdecCfg)
{
    MI_S32 u32Res = 0;
    MI_S32 i      = 0;

    _u32RunTestCaseIdx = 0;
    _bAutoTestRunFlag  = TRUE;
    _bStop             = FALSE;
    memset(_astTestCfg, 0x0, sizeof(_astTestCfg));
    memset(_thrPush, 0x0, sizeof(_thrPush));
    memset(_thrGet, 0x0, sizeof(_thrGet));
    memset(_bRun, 0x0, sizeof(_bRun));
    memset(_hFile, 0x0, sizeof(_hFile));
    memset(astMD5Ctx, 0x0, sizeof(astMD5Ctx));

    _u32TestCastCount  = 1;
    _u32RunTestCaseIdx = 0;

    _astTestCfg[_u32RunTestCaseIdx].u32Index                     = 0;
    _astTestCfg[_u32RunTestCaseIdx].eParamType                   = E_PARAM_TYPE_VDECATTR;
    _astTestCfg[_u32RunTestCaseIdx].stDevInfo.u32MaxChnNum       = pstVdecCfg->u16ChnNum;
    _astTestCfg[_u32RunTestCaseIdx].stDevInfo.bDisableLowLatency = FALSE;
    _astTestCfg[_u32RunTestCaseIdx].stDevInfo.bInitDev           = TRUE;
    strncpy((char *)_astTestCfg[_u32RunTestCaseIdx].au8Tile, (char *)pstVdecCfg->au8CaseName,
            sizeof(_astTestCfg[_u32RunTestCaseIdx].au8Tile));

    for (i = 0; i < _astTestCfg[_u32RunTestCaseIdx].stDevInfo.u32MaxChnNum; ++i)
    {
        strncpy((char *)_astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].au8FilePath,
                (char *)pstVdecCfg->stVdecAttr[i].au8InputEsName,
                sizeof(_astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].au8FilePath));
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].eCodecType =
            (MI_VDEC_CodecType_e)pstVdecCfg->stVdecAttr[i].u8CodecType;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].eDpbBufMode = E_MI_VDEC_DPB_MODE_NORMAL;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].eVideoMode =
            pstVdecCfg->stVdecAttr[i].u8InturreptMode ? E_MI_VDEC_VIDEO_MODE_STREAM : E_MI_VDEC_VIDEO_MODE_FRAME;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].u16Width       = 3820;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].u16Height      = 2160;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].u16ScaleW      = pstVdecCfg->stVdecAttr[i].u16ScalingWidth;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].u16ScaleH      = pstVdecCfg->stVdecAttr[i].u16ScalingHeight;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].u32BufSize     = pstVdecCfg->stVdecAttr[i].u32BsBufSize;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].u32FrmRate     = 30;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].u32RefFrameNum = pstVdecCfg->stVdecAttr[i].u8CmaBufNum;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].VdecChn        = i;
        _astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].u32DecFrmCount = pstVdecCfg->stVdecAttr[i].u32framenum;
        strncpy((char *)_astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].au8MD5Sum,
                (char *)pstVdecCfg->stVdecAttr[i].au8CheckMD5,
                sizeof(_astTestCfg[_u32RunTestCaseIdx].stChnAttrInfo[i].au8MD5Sum));

        if (pstVdecCfg->stVdecAttr[i].u16CropX || pstVdecCfg->stVdecAttr[i].u16CropY || pstVdecCfg->stVdecAttr[i].u16CropW
            || pstVdecCfg->stVdecAttr[i].u16CropH)
        {
            _astTestCfg[_u32RunTestCaseIdx].stChnParamInfo[i].VdecChn                    = i;
            _astTestCfg[_u32RunTestCaseIdx].stChnParamInfo[i].bConfig                    = TRUE;
            _astTestCfg[_u32RunTestCaseIdx].stChnParamInfo[i].stCropCfg.bEnable          = TRUE;
            _astTestCfg[_u32RunTestCaseIdx].stChnParamInfo[i].stCropCfg.stRect.u16X      = pstVdecCfg->stVdecAttr[i].u16CropX;
            _astTestCfg[_u32RunTestCaseIdx].stChnParamInfo[i].stCropCfg.stRect.u16Y      = pstVdecCfg->stVdecAttr[i].u16CropY;
            _astTestCfg[_u32RunTestCaseIdx].stChnParamInfo[i].stCropCfg.stRect.u16Width  = pstVdecCfg->stVdecAttr[i].u16CropW;
            _astTestCfg[_u32RunTestCaseIdx].stChnParamInfo[i].stCropCfg.stRect.u16Height = pstVdecCfg->stVdecAttr[i].u16CropH;
        }
        else
        {
            _astTestCfg[_u32RunTestCaseIdx].stChnParamInfo[i].bConfig           = FALSE;
            _astTestCfg[_u32RunTestCaseIdx].stChnParamInfo[i].stCropCfg.bEnable = FALSE;
        }
    }

    u32Res = MI_SUCCESS;
    if (MI_SUCCESS == st_start_vdec(_u32RunTestCaseIdx))
    {
        if (MI_SUCCESS != check_all_test_case_result(_u32RunTestCaseIdx))
        {
            printf("check_all_test_case_result fail\n");
            u32Res = MI_TEST_FAILE;
        }

        if (MI_SUCCESS != st_stop_vdec(_u32RunTestCaseIdx))
        {
            printf("st_stop_vdec fail\n");
            u32Res = MI_TEST_FAILE;
        }
    }
    else
    {
        printf("st_start_vdec fail\n");
        u32Res = MI_TEST_FAILE;
    }

    return u32Res;
}

#ifndef FOR_MDB_BUILD
int main(int argc, const char *argv[])
{
    int ret                          = 0;
    char InputCmd[INPUT_CMD_MAX_LEN] = {'\0'};
    MI_U32 u32CurTCaseIdx            = 0;

    if (argc < 2)
    {
        printf("Input err!!\n");
        printf("Usage: %s 0:manual, 1:auto test.\n", argv[0]);
        return -1;
    }

    register_signel();

    /* default value*/
    st_config_default_value();

    if (atoi(argv[1]) == 1)
    {
        auto_test_vdec();
    }
    else
    {
        if (argc < 4)
        {
            printf("Input err!!\n");
            printf("Usage: %s 0 [config.xml] [test case index].\n", argv[0]);
            return -1;
        }

        ExecFunc(MI_SYS_Init(), MI_SUCCESS);

        u32CurTCaseIdx = (MI_U32)atoi((const char *)argv[3]);

        /* parse script file */
        ret = st_parse_script_file((MI_S8 *)argv[2]);
        if (ret < 0)
        {
            printf("[%s][%d]: st_parse_script_file failed.\n", __FUNCTION__, __LINE__);
            return -1;
        }

        if (u32CurTCaseIdx >= _u32TestCastCount)
        {
            printf("[%s][%d]: u32CurTCaseIdx:%d >= _u32TestCastCount:%d\n", __FUNCTION__, __LINE__, u32CurTCaseIdx,
                   _u32TestCastCount);
            return -1;
        }

        st_start_vdec(u32CurTCaseIdx);
        printf("Start decoding!\n");

        while (1)
        {
            setbuf(stdin, NULL);
            memset(InputCmd, 0, sizeof(InputCmd));
            st_print_menu1();
            fgets((char *)(InputCmd), (sizeof(InputCmd) - 1), stdin);
            if (0 == strncasecmp(InputCmd, "q", strlen("q")))
            {
                printf("prepare to exit!\n\n");
                goto EXIT_1;
                break;
            }
            else if (0 == strncasecmp(InputCmd, "scale", strlen("scale")))
            {
                printf("Start dynamic scaling!\n\n");
                st_dyn_scaling(u32CurTCaseIdx);
            }
            else if (0 == strncasecmp(InputCmd, "crop", strlen("crop")))
            {
                printf("Start dynamic crop!\n\n");
                st_dyn_crop(u32CurTCaseIdx);
            }
            else if (0 == strncasecmp(InputCmd, "stop", strlen("stop")))
            {
                printf("Stop sending stream!\n\n");
                _bStop = TRUE;
                st_stop_all_chn(u32CurTCaseIdx);
            }
            else if (0 == strncasecmp(InputCmd, "start", strlen("start")))
            {
                printf("Start sending stream!\n\n");
                _bStop = FALSE;
                st_start_all_chn(u32CurTCaseIdx);
            }
            else if (0 == strncasecmp(InputCmd, "capture", strlen("capture")))
            {
                printf("Start capture!\n\n");
                st_start_capture(u32CurTCaseIdx);
            }
        }

    EXIT_1:
        st_stop_vdec(u32CurTCaseIdx);
        printf("Stop decoding!\n");

        ExecFunc(MI_SYS_Exit(), MI_SUCCESS);
        printf("Sys exit.!\n\n");
    }

    return 0;
}
#endif
