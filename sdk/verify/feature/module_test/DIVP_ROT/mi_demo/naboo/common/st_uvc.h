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
#ifndef _ST_UVC_H_
#define _ST_UVC_H_

#include "st_uvc_datatype.h"

extern int pthread_setname_np(pthread_t __target_thread, 
        const char *__name);

MI_S32 ST_UVC_Init(char *uvc_name);
MI_S32 ST_UVC_Uninit(void);
MI_S32 ST_UVC_CreateDev(const ST_UVC_ChnAttr_t* pstAttr);
MI_S32 ST_UVC_DestroyDev();
MI_S32 ST_UVC_SetChnAttr(const ST_UVC_ChnAttr_t* pstAttr);
MI_S32 ST_UVC_GetChnAttr(const ST_UVC_ChnAttr_t* pstAttr);
MI_S32 ST_UVC_StartDev(void);
MI_S32 ST_UVC_StopDev(void);
void save_file(void *buf,MI_U32 length,char type);
char* uvc_get_format(MI_U32 fcc);
#endif //_ST_UVC_H_
