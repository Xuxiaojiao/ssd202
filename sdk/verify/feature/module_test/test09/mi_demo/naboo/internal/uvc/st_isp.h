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
#ifndef _ST_ISP_H_
#define _ST_ISP_H_

#define FW_VER_BCD 0x02	//for test.
typedef struct FW_RELEASE_VERSION {
	unsigned char   major;
	unsigned char minor;
	unsigned short build;
} FW_RELEASE_VERSION;

void ISP_usb_iqsvr_open(void);
void ISP_usb_iqsvr_close(void);
PCAM_IQSVR_HANDLE *ISP_usb_get_iqsvr_handle(void);

#endif