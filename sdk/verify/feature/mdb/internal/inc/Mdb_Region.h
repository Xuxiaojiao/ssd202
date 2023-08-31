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
#ifndef MDB_REGION
#define MDB_REGION
#include "Mdb_Base.h"


#define MDB_RGN_USE_SingleTestCase(testcaseName, vecStrIn, strOut)  do{ \
    MI_U32 idx; \
    testcaseName(vecStrIn, strOut, 0); \
    idx=strOut.find("Test Fail"); \
    if(idx != std::string::npos ) \
    { \
        SetDefaultParam(); \
        ClearRegion(vecStrIn, strOut, 0); \
        return; \
    } \
}while(0);

typedef struct MDB_REGION_DrawPara_s
{
    int intMaxChannelNum;
    int intModId; //0 vpe, 1 divp
}MDB_REGION_DrawPara_t;

class Mdb_Region : public Mdb_Base
{
    public:
        Mdb_Region();
        virtual ~Mdb_Region();
        virtual void ShowWelcome(std::string &strOut);
    private:
        void AutoTest(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void Init(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void Deinit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void Create(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void Destroy(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void AttachCover(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void AttachOsd(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void Dettach(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void GetAttr(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetCoverDisplayAttr(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetOsdDisplayAttr(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetBitMap(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void GetCanvas(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void UpdateCanvas(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase001(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase002(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase003(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase004(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase005(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase006(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase007(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase008(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase009(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase010(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void TestCase011(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void OneRegionOneAttach(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void OneRegionMaxAttach(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void MaxRegionMaxAttach(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void OneChnOnePortSetBitmap(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void ClearRegion(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void MaxRegion(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void CreateAttachDetechDestroy(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetDefaultParam();
        void VpeDivpProcess(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VpeDivpAddData(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VpeDivpRmData(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VpeDivpAttachData(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetCanvas(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SelectFillDataMode(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void InjectPic(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void InjectWord(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void ElicitWord(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void UpdateWord(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void InjectCover(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetOsdPos(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetOnOff(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetOsdLayer(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetOsdAlpha(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetCoverColor(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetCoverLayer(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetRamdomPos(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);

        unsigned int Rgb2Yuv(unsigned int u32RgbColor);

        int intVpeChCnt;
        int intDivpChCnt;
        int intCurFormat;

};
#endif
