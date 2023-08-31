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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>

#include "mi_sys.h"
#include "mi_vdec.h"

#include "Mdb_Vdec.h"
#include "Mdb_Print.h"
//#include "md5.h"
#include "../../../vdec/vdec_v4/md5.c"
#include "../../../vdec/vdec_v4/vdec.c"

#define _RET_IF_FAILD_(_x_)                                  \
    do                                                       \
    {                                                        \
        if (!(_x_))                                          \
        {                                                    \
            mi_auto_testing_info("Parse json file faild\n"); \
            return MI_TEST_FAILE;                            \
        }                                                    \
    } while (0)

#define _ADD_CMD_(_x_, _y_)                                  \
    do                                                       \
    {                                                        \
        std::string s;                                       \
        s.assign(#_x_);                                      \
        transform(s.begin(), s.end(), s.begin(), ::tolower); \
        m_strHelpMsg.append("\t");                           \
        m_strHelpMsg.append(s);                              \
        m_strHelpMsg.append("\n");                           \
        PREPARE_COMMAND(s, &Mdb_Vdec::_x_, _y_);             \
    } while (0)

#define MI_VDEC_AUTO_TEST_API(_func_, _type_, _timeout_second_)             \
    do                                                                      \
    {                                                                       \
        MI_S32 s32Ret = MI_TEST_FAILE;                                      \
        time_t start;                                                       \
        time(&start);                                                       \
        s32Ret = _func_;                                                    \
        if (difftime(start, time(NULL)) >= _timeout_second_)                \
        {                                                                   \
            s32Ret = MI_TEST_TIMEOUT;                                       \
        }                                                                   \
        else                                                                \
        {                                                                   \
            s32Ret = MI_SUCCESS;                                            \
        }                                                                   \
        mi_unit_testing_result(MI_VDEC_AUTO_TEST, _type_, #_func_, s32Ret); \
    } while (0)
        
#define _RET_IF_FILE_NOT_EXIST_(_file_) \
    do { \
        if (access(_file_, F_OK)) \
        { \
            printf("File:%s not exist\n", _file_); \
            mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(_file_), m_stVdecCfg.au8CaseName, MI_TEST_FAILE); \
            return MI_TEST_FAILE; \
        } \
    } while (0)

typedef enum
{
    MI_VDEC_PROFILE_LEVEL_BASE = 0,
    MI_VDEC_PROFILE_LEVEL_DYNAMIC,
    MI_VDEC_PROFILE_LEVEL_MAX,
} MI_VDEC_Profile_Level_e;

Mdb_Vdec *Mdb_Vdec::m_pObjVdec = NULL;

void Mdb_Vdec::HandleSingle(MI_S32 u32SigNo)
{
    switch (u32SigNo)
    {
        case SIGABRT:
        case SIGINT:
        case SIGILL:
        case SIGSEGV:
        {
            mi_auto_testing_err("receiv signal value=%d force exit programe\n", u32SigNo);
            Mdb_Vdec::m_pObjVdec->print_stacktrace();
        }
        break;
        case SIGFPE:
        default:
            mi_auto_testing_err("invalid signal value=%d\n", u32SigNo);
            break;
    }
    exit(0);
}

Mdb_Vdec::Mdb_Vdec()
{
    Mdb_Vdec::m_pObjVdec = this;
    MI_SYS_Init();
    signal(SIGINT, Mdb_Vdec::HandleSingle);

    m_strHelpMsg.assign("vdec cmd list:\n");
    _ADD_CMD_(H26x_DecCase_Resolution, 2);
    _ADD_CMD_(H26x_DecCase_Ref, 2);
    _ADD_CMD_(H26x_DecCase_Profile, 2);
    _ADD_CMD_(H26x_DecCase_TileMode, 2);
    _ADD_CMD_(H26x_DecCase_Scaling, 2);
    _ADD_CMD_(H26x_DecCase_DestCrop, 2);
    _ADD_CMD_(H26x_DecCase_MultiSlice, 2);
    _ADD_CMD_(H26x_DecCase_SkipFrmDec, 2);
    _ADD_CMD_(H26x_DecCase_IntMode, 2);
    _ADD_CMD_(H26x_DecCase_Deblocking, 2);
    _ADD_CMD_(H26x_DecCase_SpsVUI, 2);
    _ADD_CMD_(H26x_Check_API, 0);
}

Mdb_Vdec::~Mdb_Vdec()
{
    MI_SYS_Exit();
}

void Mdb_Vdec::ShowWelcome(std::string &strOut)
{
    strOut.assign(m_strHelpMsg);
}

MI_S32 Mdb_Vdec::JSONGetString(cJSON *pItem, const char *const pu8Key, MI_U8 *pu8Value, MI_U32 u32MaxLen)
{
    cJSON *pTarget = NULL;
    _RET_IF_FAILD_(pTarget = cJSON_GetObjectItem(pItem, pu8Key));
    memset(pu8Value, 0x0, u32MaxLen);
    strncpy((char *)pu8Value, pTarget->valuestring, u32MaxLen - 1);

    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::JSONGetUInt8(cJSON *pItem, const char *const pu8Key, MI_U8 *pu8Value)
{
    cJSON *pTarget = NULL;
    _RET_IF_FAILD_(pTarget = cJSON_GetObjectItem(pItem, pu8Key));
    *pu8Value = (MI_U8)pTarget->valueint;

    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::JSONGetUInt16(cJSON *pItem, const char *const pu8Key, MI_U16 *pu16Value)
{
    cJSON *pTarget = NULL;
    _RET_IF_FAILD_(pTarget = cJSON_GetObjectItem(pItem, pu8Key));
    *pu16Value = (MI_U16)pTarget->valueint;

    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::JSONGetUInt32(cJSON *pItem, const char *const pu8Key, MI_U32 *pu32Value)
{
    cJSON *pTarget = NULL;
    _RET_IF_FAILD_(pTarget = cJSON_GetObjectItem(pItem, pu8Key));
    *pu32Value = (MI_U32)pTarget->valueint;

    return MI_SUCCESS;
}

MI_U8 Mdb_Vdec::ConvertCodecType(MI_U8 *pu8CodecType)
{
    if (0 == strcmp((const char *)pu8CodecType, "h264"))
    {
        return E_MI_VDEC_CODEC_TYPE_H264;
    }
    else if (0 == strcmp((const char *)pu8CodecType, "h265"))
    {
        return E_MI_VDEC_CODEC_TYPE_H265;
    }

    return E_MI_VDEC_CODEC_TYPE_MAX;
}

MI_U8 Mdb_Vdec::ConvertSwitchType(MI_U8 *pu8SwitchKey)
{
    if (0 == strcmp((const char *)pu8SwitchKey, "off"))
    {
        return FALSE;
    }
    else if (0 == strcmp((const char *)pu8SwitchKey, "on"))
    {
        return TRUE;
    }

    return FALSE;
}

MI_U8 Mdb_Vdec::ConvertProfileLV(MI_U8 *pu8LV)
{
    if (0 == strcmp((const char *)pu8LV, "Based"))
    {
        return MI_VDEC_PROFILE_LEVEL_BASE;
    }
    else if (0 == strcmp((const char *)pu8LV, "Dynamic"))
    {
        return MI_VDEC_PROFILE_LEVEL_DYNAMIC;
    }

    return MI_VDEC_PROFILE_LEVEL_MAX;
}

MI_U8 Mdb_Vdec::ConvertTileMode(MI_U8 *pu8LV)
{
    if (0 == strcmp((const char *)pu8LV, "LinerMode"))
    {
        return E_MI_VDEC_OUTBUF_LAYOUT_LINEAR;
    }
    else if (0 == strcmp((const char *)pu8LV, "TileMode"))
    {
        return E_MI_VDEC_OUTBUF_LAYOUT_TILE;
    }

    return MI_VDEC_PROFILE_LEVEL_MAX;
}

void Mdb_Vdec::ShowVdecCfgInfo(void)
{
    MI_S32 i = 0;

    printf("CaseName:%s\n", m_stVdecCfg.au8CaseName);
    printf("BasePath:%s\n", m_stVdecCfg.au8BasePath);
    printf("DumpEs:%d\n", m_stVdecCfg.bDumpEs);
    printf("RetryCount:%d\n", m_stVdecCfg.u32RetryCount);
    printf("bPushStreamThread:%d\n", m_stVdecCfg.bPushStreamThread);
    printf("chn number:%d\n", m_stVdecCfg.u16ChnNum);

    for (i = 0; i < m_stVdecCfg.u16ChnNum; ++i)
    {
        printf("------------------------------------------------------------\n");
        printf("== chn_%d: ==\n", i);
        printf("InputEsName:%s\n", m_stVdecCfg.stVdecAttr[i].au8InputEsName);
        printf("OuputYuvName:%s\n", m_stVdecCfg.stVdecAttr[i].au8OuputYuvName);
        printf("ChnnelId:%d\n", m_stVdecCfg.stVdecAttr[i].u8ChnnelId);
        printf("CodecType:%d\n", m_stVdecCfg.stVdecAttr[i].u8CodecType);
        printf("CmaBufNum:%d\n", m_stVdecCfg.stVdecAttr[i].u8CmaBufNum);
        printf("BsBufSize:%d\n", m_stVdecCfg.stVdecAttr[i].u32BsBufSize);
        printf("framenum:%d\n", m_stVdecCfg.stVdecAttr[i].u32framenum);
        printf("TileMode:%d\n", m_stVdecCfg.stVdecAttr[i].u8TileMode);
        printf("ScalingWidth:%d\n", m_stVdecCfg.stVdecAttr[i].u16ScalingWidth);
        printf("ScalingHeight:%d\n", m_stVdecCfg.stVdecAttr[i].u16ScalingHeight);
        printf("InturreptMode:%d\n", m_stVdecCfg.stVdecAttr[i].u8InturreptMode);
        printf("SkipFrameDecode:%d\n", m_stVdecCfg.stVdecAttr[i].u8SkipFrameDecode);
        printf("ChnnelToBeCreate:%d\n", m_stVdecCfg.stVdecAttr[i].u8ChnnelToBeCreate);
        printf("ParamLv:%d\n", m_stVdecCfg.stVdecAttr[i].u8ParamLv);
        printf("CropX:%d\n", m_stVdecCfg.stVdecAttr[i].u16CropX);
        printf("CropY:%d\n", m_stVdecCfg.stVdecAttr[i].u16CropY);
        printf("CropW:%d\n", m_stVdecCfg.stVdecAttr[i].u16CropW);
        printf("CropH:%d\n", m_stVdecCfg.stVdecAttr[i].u16CropH);
        printf("CheckMd5:%s\n", m_stVdecCfg.stVdecAttr[i].au8CheckMD5);
    }
}

MI_S32 Mdb_Vdec::GetInfoFromJson(cJSON *pJsRoot)
{
    cJSON *pItem          = NULL;
    cJSON *pVdecAttrItem  = NULL;
    MI_S32 s32VdecAttrLen = 0;
    MI_S32 i              = 0;
    MI_U8 au8Tmp[32];

    _RET_IF_FAILD_(pItem = cJSON_GetObjectItem(pJsRoot, "Vdec_Config"));
    /// common
    _RET_IF_FAILD_(MI_SUCCESS == JSONGetString(pItem, "CaseName", m_stVdecCfg.au8CaseName, sizeof(m_stVdecCfg.au8CaseName)));
    _RET_IF_FAILD_(MI_SUCCESS == JSONGetString(pItem, "BasePath", m_stVdecCfg.au8BasePath, sizeof(m_stVdecCfg.au8BasePath)));
    _RET_IF_FAILD_(MI_SUCCESS == JSONGetUInt8(pItem, "DumpEs", &m_stVdecCfg.bDumpEs));
    _RET_IF_FAILD_(MI_SUCCESS == JSONGetUInt32(pItem, "RetryCount", &m_stVdecCfg.u32RetryCount));
    _RET_IF_FAILD_(MI_SUCCESS == JSONGetUInt8(pItem, "bPushStreamThread", &m_stVdecCfg.bPushStreamThread));

    /// vdec_attr
    _RET_IF_FAILD_(pItem = cJSON_GetObjectItem(pItem, "vdec_attr"));
    _RET_IF_FAILD_(cJSON_Array == pItem->type);
    s32VdecAttrLen        = cJSON_GetArraySize(pItem);
    m_stVdecCfg.u16ChnNum = (MI_U16)s32VdecAttrLen;
    for (i = 0; (i < s32VdecAttrLen) && (i < MAX_CHANNEL_NUM); ++i)
    {
        _RET_IF_FAILD_(pVdecAttrItem = cJSON_GetArrayItem(pItem, i));
        memcpy(&m_stVdecCfg.stVdecAttr[i], &m_stVdecCfg.stVdecAttr[0], sizeof(m_stVdecCfg.stVdecAttr[0]));
        JSONGetString(pVdecAttrItem, "InputEsName", m_stVdecCfg.stVdecAttr[i].au8InputEsName,
                      sizeof(m_stVdecCfg.stVdecAttr[i].au8InputEsName));
        JSONGetString(pVdecAttrItem, "OuputYuvName", m_stVdecCfg.stVdecAttr[i].au8OuputYuvName,
                      sizeof(m_stVdecCfg.stVdecAttr[i].au8OuputYuvName));
        JSONGetUInt8(pVdecAttrItem, "ChnnelId", &m_stVdecCfg.stVdecAttr[i].u8ChnnelId);
        if (MI_SUCCESS == JSONGetString(pVdecAttrItem, "CodecType", au8Tmp, sizeof(au8Tmp)))
        {
            m_stVdecCfg.stVdecAttr[i].u8CodecType = (MI_U8)ConvertCodecType(au8Tmp);
        }
        JSONGetUInt8(pVdecAttrItem, "CmaBufNum", &m_stVdecCfg.stVdecAttr[i].u8CmaBufNum);
        JSONGetUInt32(pVdecAttrItem, "BsBufSize", &m_stVdecCfg.stVdecAttr[i].u32BsBufSize);
        JSONGetUInt32(pVdecAttrItem, "framenum", &m_stVdecCfg.stVdecAttr[i].u32framenum);
        if (MI_SUCCESS == JSONGetString(pVdecAttrItem, "TileMode", au8Tmp, sizeof(au8Tmp)))
        {
            m_stVdecCfg.stVdecAttr[i].u8TileMode = (MI_U8)ConvertTileMode(au8Tmp);
        }
        JSONGetUInt16(pVdecAttrItem, "ScalingWidth", &m_stVdecCfg.stVdecAttr[i].u16ScalingWidth);
        JSONGetUInt16(pVdecAttrItem, "ScalingHeight", &m_stVdecCfg.stVdecAttr[i].u16ScalingHeight);
        if (MI_SUCCESS == JSONGetString(pVdecAttrItem, "InturreptMode", au8Tmp, sizeof(au8Tmp)))
        {
            m_stVdecCfg.stVdecAttr[i].u8InturreptMode = (MI_U8)ConvertSwitchType(au8Tmp);
        }
        if (MI_SUCCESS == JSONGetString(pVdecAttrItem, "SkipFrameDecode", au8Tmp, sizeof(au8Tmp)))
        {
            m_stVdecCfg.stVdecAttr[i].u8SkipFrameDecode = (MI_U8)ConvertSwitchType(au8Tmp);
        }
        JSONGetUInt8(pVdecAttrItem, "ChnnelToBeCreate", &m_stVdecCfg.stVdecAttr[i].u8ChnnelToBeCreate);
        if (MI_SUCCESS == JSONGetString(pVdecAttrItem, "ParamLv", au8Tmp, sizeof(au8Tmp)))
        {
            m_stVdecCfg.stVdecAttr[i].u8ParamLv = (MI_U8)ConvertProfileLV(au8Tmp);
        }
        JSONGetUInt16(pVdecAttrItem, "CropX", &m_stVdecCfg.stVdecAttr[i].u16CropX);
        JSONGetUInt16(pVdecAttrItem, "CropY", &m_stVdecCfg.stVdecAttr[i].u16CropY);
        JSONGetUInt16(pVdecAttrItem, "CropW", &m_stVdecCfg.stVdecAttr[i].u16CropW);
        JSONGetUInt16(pVdecAttrItem, "CropH", &m_stVdecCfg.stVdecAttr[i].u16CropH);
        JSONGetString(pVdecAttrItem, "CheckMd5", m_stVdecCfg.stVdecAttr[i].au8CheckMD5,
                      sizeof(m_stVdecCfg.stVdecAttr[i].au8CheckMD5));
    }

    ShowVdecCfgInfo();
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::GetTCastCfg(const char *filePath)
{
    char *content = NULL;
    cJSON *root   = NULL;
    MI_S32 s32Ret = MI_TEST_FAILE;

    content = cJSON_FileOpen(filePath);
    _RET_IF_FAILD_(root = cJSON_Parse(content));
    s32Ret = GetInfoFromJson(root);
    cJSON_Delete(root);
    return MI_SUCCESS;
}

const char *Mdb_Vdec::GetDecCaseType(const char *pFile)
{
    if (strstr(pFile, "H264"))
    {
        return "H264";
    }
    else if (strstr(pFile, "H265"))
    {
        return "H265";
    }

    return "Other";
}

// ************************************************************************************************************************************
// * *
// ************************************************************************************************************************************

void Mdb_Vdec::H26x_Check_API(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_VDEC_InitParam_t stInitParams;
    MI_VDEC_ChnAttr_t stChnAttr;
    MI_VDEC_OutputPortAttr_t stOutputPortAttr;
    MI_SYS_ChnPort_t stChnPort;
    MI_VDEC_ChnStat_t stChnStatus;
    MI_S32 s32TimeOut = 0;

    printf("run %s begin\n", __FUNCTION__);
    MI_SYS_Init();
    s32TimeOut = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    

    memset(&stInitParams, 0x0, sizeof(MI_VDEC_InitParam_t));
    stInitParams.bDisableLowLatency = FALSE;
    MI_VDEC_AUTO_TEST_API(MI_VDEC_InitDev(&stInitParams), "API", s32TimeOut);

    memset(&stChnAttr, 0x0, sizeof(MI_VDEC_ChnAttr_t));
    stChnAttr.eCodecType                     = E_MI_VDEC_CODEC_TYPE_H264;
    stChnAttr.u32PicWidth                    = 1920;
    stChnAttr.u32PicHeight                   = 1080;
    stChnAttr.eVideoMode                     = E_MI_VDEC_VIDEO_MODE_FRAME;
    stChnAttr.u32BufSize                     = 0x10000;
    stChnAttr.eDpbBufMode                    = E_MI_VDEC_DPB_MODE_NORMAL;
    stChnAttr.stVdecVideoAttr.u32RefFrameNum = 2;
    stChnAttr.u32Priority                    = 0;
    MI_VDEC_AUTO_TEST_API(MI_VDEC_CreateChn(0, &stChnAttr), "API", s32TimeOut);

    MI_VDEC_AUTO_TEST_API(MI_VDEC_StartChn(0), "API", s32TimeOut);

    stOutputPortAttr.u16Width  = 1280;
    stOutputPortAttr.u16Height = 720;
    MI_VDEC_AUTO_TEST_API(MI_VDEC_SetOutputPortAttr(0, &stOutputPortAttr), "API", s32TimeOut);

    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId    = E_MI_MODULE_ID_VDEC;
    stChnPort.u32DevId  = 0;
    stChnPort.u32ChnId  = 0;
    stChnPort.u32PortId = 0;
    MI_VDEC_AUTO_TEST_API(MI_SYS_SetChnOutputPortDepth(&stChnPort, 2, 4), "API", s32TimeOut);

    MI_VDEC_AUTO_TEST_API(MI_VDEC_ResetChn(0), "API", s32TimeOut);

    MI_VDEC_AUTO_TEST_API(MI_VDEC_GetChnAttr(0, &stChnAttr), "API", s32TimeOut);

    memset(&stChnStatus, 0x0, sizeof(MI_VDEC_ChnStat_t));
    MI_VDEC_AUTO_TEST_API(MI_VDEC_GetChnStat(0, &stChnStatus), "API", s32TimeOut);

    MI_VDEC_AUTO_TEST_API(MI_VDEC_FlushChn(0), "API", s32TimeOut);

    MI_VDEC_DisplayMode_e eDisplayMode = E_MI_VDEC_DISPLAY_MODE_PREVIEW;
    MI_VDEC_AUTO_TEST_API(MI_VDEC_SetDisplayMode(0, eDisplayMode), "API", s32TimeOut);

    MI_VDEC_AUTO_TEST_API(MI_VDEC_GetDisplayMode(0, &eDisplayMode), "API", s32TimeOut);

    MI_VDEC_AUTO_TEST_API(MI_VDEC_GetOutputPortAttr(0, &stOutputPortAttr), "API", s32TimeOut);

    MI_VDEC_OutbufLayoutMode_e eBufTileMode = E_MI_VDEC_OUTBUF_LAYOUT_TILE;
    MI_VDEC_AUTO_TEST_API(MI_VDEC_SetOutputPortLayoutMode(eBufTileMode), "API", s32TimeOut);

    MI_VDEC_AUTO_TEST_API(MI_VDEC_GetOutputPortLayoutMode(&eBufTileMode), "API", s32TimeOut);

    /// not support api: MI_VDEC_SetChnParam, MI_VDEC_GetChnParam, MI_VDEC_IMPL_GetUserData, MI_VDEC_ReleaseUserData

    MI_VDEC_AUTO_TEST_API(MI_VDEC_StopChn(0), "API", s32TimeOut);

    MI_VDEC_AUTO_TEST_API(MI_VDEC_DestroyChn(0), "API", s32TimeOut);

    MI_VDEC_AUTO_TEST_API(MI_VDEC_DeInitDev(), "API", s32TimeOut);

    MI_SYS_Exit();
    printf("run %s end\n", __FUNCTION__);
}

MI_S32 Mdb_Vdec::H26x_DecCase_Resolution(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_Ref(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_Profile(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_TileMode(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_Scaling(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_DestCrop(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_MultiSlice(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_SkipFrmDec(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_IntMode(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_Deblocking(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}

MI_S32 Mdb_Vdec::H26x_DecCase_SpsVUI(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp)
{
    MI_S32 s32Ret = MI_TEST_FAILE;
    time_t start;

    _RET_IF_FILE_NOT_EXIST_(inStrings[0].c_str());
    time(&start);
    memset(&m_stVdecCfg, 0x0, sizeof(m_stVdecCfg));
    m_stVdecCfg.u32ExecTimeout = static_cast<int>(strtol(inStrings[1].c_str(), NULL, 10));
    printf("\n %s timeout:%d ------>\n", __FUNCTION__, m_stVdecCfg.u32ExecTimeout);
    _RET_IF_FAILD_(MI_SUCCESS == GetTCastCfg(inStrings[0].c_str()));

    s32Ret = h26x_dec_stream(&m_stVdecCfg);
    if (difftime(start, time(NULL)) >= m_stVdecCfg.u32ExecTimeout)
    {
        s32Ret = MI_TEST_TIMEOUT;
    }

    mi_unit_testing_result(MI_VDEC_AUTO_TEST, GetDecCaseType(inStrings[0].c_str()), m_stVdecCfg.au8CaseName, s32Ret);
    return MI_SUCCESS;
}
