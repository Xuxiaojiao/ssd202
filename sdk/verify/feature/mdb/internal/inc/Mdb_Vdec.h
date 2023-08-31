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
#ifndef MDB_VDEC
#define MDB_VDEC
#include <map>

#include "Mdb_Base.h"
#include "cjson.h"

#include "../../../vdec/vdec_v4/vdec.h"

class Mdb_Vdec : public Mdb_Base
{
  public:
    Mdb_Vdec();
    virtual ~Mdb_Vdec();
    virtual void ShowWelcome(std::string &strOut);

  private:
    void H26x_Check_API(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);

    MI_S32 JSONGetString(cJSON *pItem, const char *const pu8Key, MI_U8 *pu8Value, MI_U32 u32MaxLen);
    MI_S32 JSONGetUInt8(cJSON *pItem, const char *const pu8Key, MI_U8 *pu8Value);
    MI_S32 JSONGetUInt16(cJSON *pItem, const char *const pu8Key, MI_U16 *pu16Value);
    MI_S32 JSONGetUInt32(cJSON *pItem, const char *const pu8Key, MI_U32 *pu32Value);

    MI_S32 GetTCastCfg(const char *filePath);
    MI_S32 GetInfoFromJson(cJSON *pJsRoot);
    MI_U8 ConvertCodecType(MI_U8 *pu8CodecType);
    MI_U8 ConvertSwitchType(MI_U8 *pu8SwitchKey);
    MI_U8 ConvertProfileLV(MI_U8 *pu8LV);
    MI_U8 ConvertTileMode(MI_U8 *pu8LV);
    void ShowVdecCfgInfo(void);
    static void HandleSingle(MI_S32 u32SigNo);
    const char *GetDecCaseType(const char *pFile);

    MI_S32 H26x_DecCase_Resolution(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_Ref(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_Profile(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_TileMode(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_Scaling(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_DestCrop(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_MultiSlice(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_SkipFrmDec(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_IntMode(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_Deblocking(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
    MI_S32 H26x_DecCase_SpsVUI(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);

public:
  static Mdb_Vdec *m_pObjVdec;

  private:
    Vdec_Config_t m_stVdecCfg;
    std::string m_strHelpMsg;
};

#endif
