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
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "Mdb_Base.h"
#ifdef INTERFACE_RGN
#include "Mdb_Region.h"
#endif
#include "Mdb_Integrate.h"
#ifdef INTERFACE_VDEC
#include "Mdb_Vdec.h"
#endif

typedef enum
{
    MDB_MODULE_START = 0,
#ifdef INTERFACE_RGN
    MDB_MODULE_RGN,
#endif
    MDB_MODULE_INTEGRATE,
#ifdef INTERFACE_VDEC
    MDB_MODULE_VDEC,
#endif
    MDB_MODULE_MAX
}MDB_MODULE;

#define MDB_MODULE_RGN_NAME "region"
#define MDB_MODULE_INTEGRATE_NAME "integrate"
#ifdef INTERFACE_VDEC
#define MDB_MODULE_VDEC_NAME "vdec"
#endif


Mdb_Base *Mdb_Base::pInstance = NULL;
std::map<std::string, std::string> Mdb_Base::mapModule;

template<class MDBCHILD>
Mdb_Base::Mdb_Setup<MDBCHILD>::Mdb_Setup(const char *pModuleName)
{
    pInstance = new (std::nothrow) MDBCHILD;
    assert(pInstance);
    pInstance->strMdbBaseSubName = pModuleName;
}

Mdb_Base::Mdb_Base()
{
    bExit = 0;
    pthread_mutex_init(&mutexProcess, NULL);

    //START_PROCESS;
}
Mdb_Base::~Mdb_Base()
{
    printf("%s\n", __FUNCTION__);
    //STOP_PROCESS;
    pInstance = NULL;
}

void Mdb_Base::PrepareModule(std::string &strOut)
{
#ifdef INTERFACE_RGN
    PREPARE_MODULE(MDB_MODULE_RGN, MDB_MODULE_RGN_NAME);
#endif
    PREPARE_MODULE(MDB_MODULE_INTEGRATE, MDB_MODULE_INTEGRATE_NAME);
#ifdef INTERFACE_VDEC
    PREPARE_MODULE(MDB_MODULE_VDEC, MDB_MODULE_VDEC_NAME);
#endif
    return;
}
void Mdb_Base::GetName(std::string &strOut)
{
    strOut = strMdbBaseSubName;
    return;
}
unsigned int Mdb_Base::GetTime(void)
{
    struct timespec ts;
    unsigned int ms;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ms = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    if(ms == 0)
    {
        ms = 1;
    }
    return ms;
}
unsigned int Mdb_Base::GetTimeDiff(unsigned int intOldTime)
{
    return GetTime() - intOldTime;
}
void Mdb_Base::DumpCmd(std::string &strOut)
{
    std::map<std::string, MDB_Cmd_t>::iterator it;

    for (it = mapMdbBaseCmd.begin(); it != mapMdbBaseCmd.end(); ++it)
    {
        strOut += it->first;
        strOut += "/";
    }
}
void Mdb_Base::SetPara(std::string &strCmd, std::vector<std::string> &strInStrings)
{
    strMdbBaseSubCmd = strCmd;
    strMdbBaseInStrings = strInStrings;
}
void Mdb_Base::Create(std::string &strModuleName)
{
    switch(Mdb_Base::Atoi(strModuleName))
    {
#ifdef INTERFACE_RGN
        case MDB_MODULE_RGN:
        {
            Mdb_Setup<Mdb_Region> Mdb_Region(MDB_MODULE_RGN_NAME);
        }
        break;
#endif
        case MDB_MODULE_INTEGRATE:
        {
            Mdb_Setup<Mdb_Integrate> Mdb_Integrate(MDB_MODULE_INTEGRATE_NAME);
        }
        break;
#ifdef INTERFACE_VDEC
        case MDB_MODULE_VDEC:
        {
            Mdb_Setup<Mdb_Vdec> Mdb_Vdec(MDB_MODULE_VDEC_NAME);
        }
        break;
#endif
        default:
        {
            if (mapModule.find(strModuleName) != mapModule.end())
            {
                printf("module find end \n");

                Create(mapModule[strModuleName]);
            }
        }
        break;
    }
}
unsigned int Mdb_Base::Atoi(std::string &strOut)
{
    const char * pStr = strOut.c_str();
    int intStrLen = strOut.length();
    unsigned short bUseHex = 0;
    unsigned int intRetNumber = 0;

    if (pStr == NULL)
    {
        return 0xFFFFFFFF;
    }

    if (intStrLen > 2)
    {
        if (pStr[0] == '0' &&(pStr[1] == 'X' || pStr[1] == 'x'))
        {
            bUseHex = 1;
            pStr += 2;
        }
    }
    if (bUseHex == 1)
    {
        for (int i = 0; i < intStrLen - 2; i++)
        {
            if ((pStr[i] > '9' || pStr[i] < '0')    \
                && (pStr[i] > 'f' || pStr[i] < 'a') \
                && (pStr[i] > 'F' || pStr[i] < 'A'))
            {
                return 0xFFFFFFFF;
            }
        }
        sscanf(pStr, "%x", &intRetNumber);
    }
    else
    {
        for (int i = 0; i < intStrLen; i++)
        {
            if (pStr[i] > '9' || pStr[i] < '0')
            {
                return 0xFFFFFFFF;
            }
        }
        intRetNumber =  atoi(strOut.c_str());
    }
    return intRetNumber;
}
void Mdb_Base::Print(std::string &strOut, std::string strContainer, PRINT_COLOR enColor, PRINT_MODE enMode)
{

    if (enColor == PRINT_COLOR_NORMAL && enMode == PRINT_MODE_NORMAL)
    {
        strOut += strContainer;
    }
    else
    {
        std::stringstream sstr;

        sstr<< "\033[" << enMode << ';' << enColor << 'm' << strContainer.c_str() << "\033[0m";
        strOut += sstr.str();
    }
}
void Mdb_Base::ParseStrings(const char *pStr, std::vector<std::string> &strInStrings)
{
    char *pStr1 = (char*)malloc(strlen(pStr)+1);
    char *pStr2 = NULL;
    int intCmdSize = 0;
    std::string strTmp;

    memset(pStr1, 0, strlen(pStr)+1);
    strcpy(pStr1, pStr);

    if (pStr == NULL)
    {
        printf("pStr is NULL!\n");
        return;
    }

    while (1)
    {
        while (*pStr1 == ' ' && *pStr1 != 0)
        {
            pStr1++;
        }
        pStr2 = pStr1;
        while (*pStr2 != ' ' && *pStr2 != 0)
        {
            pStr2++;
        }
        intCmdSize = pStr2 - pStr1;
        if (intCmdSize != 0)
        {
            strTmp.assign(pStr1, intCmdSize);
            strInStrings.push_back(strTmp);
            pStr1 = pStr2;
        }
        else
        {
            break;
        }
    }
}

void Mdb_Base::print_stacktrace()
{
    int size = 16;
    void * array[16];
    int stack_num = backtrace(array, size);
    char ** stacktrace = backtrace_symbols(array, stack_num);
    for (int i = 0; i < stack_num; ++i)
    {
        printf("%s\n", stacktrace[i]);
    }
    free(stacktrace);
}