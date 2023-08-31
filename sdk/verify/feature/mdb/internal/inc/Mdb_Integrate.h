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
#ifndef MDB_INTEGRATE
#define MDB_INTEGRATE
#include "Mdb_Base.h"
#ifdef INTERNAL_RTSP
#include "Live555RTSPServer.hh"
#endif

class Mdb_Integrate : public Mdb_Base
{
    public:
        Mdb_Integrate();
        virtual ~Mdb_Integrate();
        virtual void ShowWelcome(std::string &strOut);
    private:
#ifdef INTERFACE_VIF
        void VifInit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VifDeinit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
#endif
#ifdef INTERFACE_VPE
        void VpeInit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VpeDeinit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void YuvStretchWriteFile(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VpeCreatePort(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VpeDestroyPort(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VpeSetRotation(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
#endif
#ifdef INTERFACE_DIVP
        void DivpInit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void DivpCreatePort(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void DivpStart(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void DivpStop(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void DivpDeInit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
#endif
#ifdef INTERFACE_LDC
        void LdcCreat(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void LdcInit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void LdcSetBin(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void LdcCreatePort(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void LdcStart(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void LdcStop(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void LdcDeInit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
#endif
#ifdef INTERFACE_VENC
        void VencInit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VencDeinit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VencWriteFile(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VencInjectFrame(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VencStart(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VencStop(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void VencSetInput(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
#endif
#ifdef INTERFACE_IQSERVER
        void IqOpenServer(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void IqCloseServer(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void IqLoadApiBin(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
#endif
#ifdef INTERFACE_SYS
        void SysInit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SysDeinit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetBindInfo(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetBindInfo2(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetDepth(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetUnBindInfo(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void SetMmaConf(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void ConfigPrivatePool(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void YuvWriteFile(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
#endif
        void ExportFile(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
#ifdef MISC_FBDEV
        void FbInit(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void FbFillRect(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void FbInitMouse(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void FbSetMouse(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void FbShow(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);

        void FbFillRectTest(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void FbSetColorKey(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void FbGetColorKey(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void FbDrawText(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
#endif
#ifdef INTERNAL_RTSP
        void RtspServerStart(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        void RtspServerStop(std::vector<std::string> &inStrings, std::string &strOut, int bShowHelp);
        static void* OpenStream(char const* szStreamName, void* arg);
        static int VideoReadStream(void* handle, unsigned char* ucpBuf, int BufLen, struct timeval *p_Timestamp, void* arg);
        static int CloseStream(void* handle, void* arg);
        static std::map<std::string, Live555RTSPServer*> mapRTSPServer;
        static Live555RTSPServer *pRTSPServer;
#endif
};
#endif
