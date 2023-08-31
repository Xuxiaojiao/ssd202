/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.
  
  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (??Sigmastar Confidential Information??) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _AD_RESOURCE_H_
#define _AD_RESOURCE_H_

/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, �㽭�󻪼����ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : Adda_resource.h
  �� �� ��   : ����
  ��    ��       : xu_longfei
  ��������: 2013��11��19��
  ����޸�:
  ��������: Adda_resource.h ��ͷ�ļ�
  �����б�:
  �޸���ʷ:
  1.��    ��    : 2013��11��19��
    ��    ��     : xu_longfei
    �޸�����   : �����ļ�

******************************************************************************/


//#include "i2c_resource.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

 
#define MAX_AD_NUM (24)
 
 
 /*AD ����߼�ͨ���ţ��������߼�˳���Ӧ*/
#define MAX_AD_LOGIC_CHN_NUM	(32)
 /*DA ����߼�ͨ���ţ�������߼�˳���Ӧ*/
#define MAX_DA_LOGIC_CHN_NUM	(10)
 /*�����Խ�����߼�ͨ����*/
#define MAX_MIC_LOGIC_CHN_NUM	(2)

 /*ͨ�ŷ�ʽ*/
typedef enum Ad_Commut_e
{
	AD_COMMUT_UNKNOW = 0,
	AD_COMMUT_I2C = 1,
	AD_COMMUT_PCI = 2,
	AD_COMMUT_I2CSWITCH = 3,
	AD_COMMUT_BUT,
}AD_COMMUT_E;

/*AD֧������ֱ���*/
typedef enum vd_out_resolution_e
{
	VD_OUT_RESOLUTION_UNKNOW  = 0,
	VD_OUT_RESOLUTION_D1	    = 1, 
	VD_OUT_RESOLUTION_960H	= 2, 
	VD_OUT_RESOLUTION_480P    = 3,
	VD_OUT_RESOLUTION_576P    = 4,
	VD_OUT_RESOLUTION_720P25	= 5,
	VD_OUT_RESOLUTION_720P30	= 6,
	VD_OUT_RESOLUTION_720P50  = 7,
	VD_OUT_RESOLUTION_720P60  = 8,
	VD_OUT_RESOLUTION_720I50  = 9,
	VD_OUT_RESOLUTION_720I60  = 10,
	VD_OUT_RESOLUTION_1080P25 = 12,
	VD_OUT_RESOLUTION_1080P30 = 13,
	VD_OUT_RESOLUTION_1080P50 = 14,
	VD_OUT_RESOLUTION_1080P60 = 15,
	VD_OUT_RESOLUTION_1080I50 = 16,
	VD_OUT_RESOLUTION_1080I60 = 17,
	VD_OUT_RESOLUTION_BUT,
}VD_OUT_RESOLUTION_E;

/*AD֧�������ʽ*/
typedef enum video_out_format_e
{
	VIDEO_OUT_FORMAT_UNKNOW  = 0,
	VIDEO_OUT_FORMAT_HDMI_RGB    = 1, 
	VIDEO_OUT_FORMAT_HDMI_YUV444 = 2,
	VIDEO_OUT_FORMAT_HDMI_YUV422 = 3,
	VIDEO_OUT_FORMAT_DVI_RGB  = 4,
	VIDEO_OUT_FORMAT_CVBS  = 5,
	VIDEO_OUT_FORMAT_BUT,
}VIDEO_OUT_FORMAT_E;


/*�ɼ�����ʱ�Ӵ�����ʽ*/
typedef enum LATCH_DATA_CLK_EDGE_e
{
	LATCH_CLK_UNKNOW = 0,
	LATCH_CLK_LEVEL = 1,/*��ƽ����*/
	LATCH_CLK_EDGE = 2, /*���ش���*/
	LATCH_CLK_RISE_EDGE = 3,
	LATCH_CLK_FALL_EDGE = 4,
	LATCH_CLK_LOW = 5,
	LATCH_CLK_HIGH = 6,
	LATCH_CLK_BUT = 7,
}LATCH_DATA_CLK_EDGE_E;

/*AD��DAоƬ����*/
typedef enum ad_da_fuction_e
{
	AD_DA_FUCTION_UNKNOW = 0,
	AD_DA_FUCTION_AUDIO  = 1, /*ֻ֧����Ƶ*/
	AD_DA_FUCTION_VIDEO = 2,/*֧����Ƶ*/
	AD_DA_FUCTION_VIDEO_BLIT = 3, /*2826����Ƶƴ��*/
	AD_DA_FUCTION_VIDEO_TRANS = 4,/*���������Ƶת��*/
	AD_DA_FUCTION_PTZ_485 = 5, /*485 ��̨*/
	AD_DA_FUCTION_BUT,
}AD_DA_FUCTION_E;

/*VD PORT ģʽ���÷�ʽ*/
typedef enum vd_port_mux_mode_e
{
	VD_PORT_MODE_UNKNOW = 0,
	VD_PORT_MUX_MODE_1MUX = 1,		// 1X ģʽ
	VD_PORT_MUX_MODE_2MUX = 2,		// 2X ģʽ
	VD_PORT_MUX_MODE_4MUX = 4,		// 4X ģʽ
	VD_PORT_MUX_MODE_1MUXHALF = 5,	// 1X halfģʽ
	VD_PORT_MUX_MODE_2MUXHALF = 6,	// 2X halfģʽ
	VD_PORT_MUX_MODE_4MUXHALF = 7,	// 4X halfģʽ
	VD_PORT_MUX_MODE_BUT,
}VD_PORT_MUX_MODE_E;

/*��Ƶ�����ʽ*/
typedef enum vd_port_out_format_e
{
	VD_PORT_OUT_FORMAT_UNKNOW = 0,
	VD_PORT_OUT_FORMAT_BT656  = 1,
	VD_PORT_OUT_FORMAT_BT1120 = 2,
	VD_PORT_OUT_FORMAT_BT601  = 3,
	VD_PORT_OUT_FORMAT_BUT,
}VD_PORT_OUT_FORMAT_E;


/*��Ƶ�����ʽ*/
typedef enum vd_port_CLK
{
	VD_PORT_CKL_AUTO, // �Զ�����
	VD_PORT_CKL_27M,  //27MHZ
	VD_PORT_CKL_36M ,//36MHZ
	VD_PORT_CKL_37_125M,//37.125MHZ
	VD_PORT_CKL_74_25M,//74.25MHZ
	VD_PORT_CKL_148_5M,//148.5MHZ
	VD_PORT_CKL_297M,//297MHZ
	VD_PORT_CKL_144M,//155MHZ
	VD_PORT_CKL_288M,//288MHZ
}VD_PORT_CLK_E;

/*��Ƶ�����ʽ*/
typedef enum vd_port_EdgeMode
{
	VD_PORT_EDGE_UP, 
	VD_PORT_EDGE_DUAL, 
}VD_PORT_EDGE_E;

/*��Ƶ�����ʽ*/
typedef enum audio_port_out_format_e
{
	AUDIO_PORT_OUT_FORMAT_UNKNOW = 0,
	AUDIO_PORT_OUT_FORMAT_I2S  = 1,
	AUDIO_PORT_OUT_FORMAT_DSP  = 2,
	AUDIO_PORT_OUT_FORMAT_BUT,
}AUDIO_PORT_OUT_FORMAT_E;

typedef enum video_head_mode_e
{
	VIDEO_HEAD_SINGEL = 0,  /*��ͷ�ɼ� ������嶼����ʹ��bt656 ����ʹ�ô˲ɼ���ʽ*/
	VIDEO_HEAD_DOUBLE = 1,  /*˫�ɼ� ������Ҫʹ��bt1120*/
	VIDEO_HEAD_BUT    = 2,
}VIDEO_HEAD_MODE_E;

typedef struct Vd_Port_Caps_s
{
	uint32  enSupportVdPortOutFormat;/*port �����ʱ��VD_PORT_OUT_FORMAT_E*/
	uint32  enSupportVdPortMuxMode;/*port �ڸ��÷�ʽVD_PORT_MUX_MODE_E*/
}Vd_Port_Caps_S;

//����VD PORT����
#define MAX_VD_PORT_COUNT (4)
#define MAX_VD_CHN_PER_PORT (4)

/*��Ƶ���������*/
typedef struct vd_port_s
{
	U8 ucPortId;  /*prot��*/	
	Vd_Port_Caps_S Vd_Port_Caps;
	VD_PORT_CLK_E enVdportClk;//BT656/bt1220 ʱ��ѡ��
	VD_PORT_EDGE_E enVdportEdge;//BT656/bt1220����ѡ��
	VD_PORT_OUT_FORMAT_E enVdPortOutFormat;       /*port �����ʱ��*/
	VD_PORT_MUX_MODE_E enVdPortMuxMode;            /*port �ڸ��÷�ʽ*/
	VIDEO_HEAD_MODE_E enOutHeadMode;               /*ͷģʽ 0Ϊ��ͷ 1Ϊ˫ͷ ��ģʽ��9901��Ч ��ֻ����1X�ɼ���ʱ����Ч*/
	U8 ucOutSequence[MAX_VD_CHN_PER_PORT];         /*��Ƶ���˳��*/
	U8 uClkDelay;                                  /*Clk Delay 0-0xf*/
}VD_PORT_S;

typedef struct vd_port_dev_s
{
	U8 ucCount;		/*���õ�port����*/
	VD_PORT_S stVdPort[MAX_VD_PORT_COUNT];
}VD_PORT_DEV_S;

/*��Ƶ��������ģʽ*/
typedef enum audio_cascade_mode_e
{
	AUDIO_CASCADE_UNKOWN = 0,
	AUDIO_CASCADE_MASTER = 1,
	AUDIO_CASCADE_SLAVE1 = 2,
	AUDIO_CASCADE_SLAVE2 = 3,
	AUDIO_CASCADE_SLAVE3 = 4,
	AUDIO_CASCADE_BUT
}AUDIO_CASCADE_MODE_E;

/*��Ƶʱ������ģʽ*/
typedef enum audio_clk_slave_mode_e
{
	AUDIO_CLK_UNKOWN = 0,
	AUDIO_CLK_SLAVE  = 1,
	AUDIO_CLK_MASTER = 2,
	AUDIO_CLK_BUT
}AUDIO_CLK_SLAVE_MODE_E;

/*��Ƶ�ɼ�ͬ����ʽ*/
typedef enum video_sample_sync_mode_e 
{
	SYNC_MODE_UNKOWN  = 0,
	SYNC_MODE_EMBEDED = 1, /*��ͬ��*/
	SYNC_MODE_EXTEND  = 2, /*��ͬ��*/
	SYNC_MODE_BUT
}VIDEO_SAMPLE_SYNC_MODE_E;

/*��Ƶ�ɼ��ź�Դ��ʽ*/
typedef enum video_source_format_e 
{
	VIDEO_SOURCE_FORMAT_UNKOWN  = 0,
	VIDEO_SOURCE_FORMAT_BT1120  = 1, 
	VIDEO_SOURCE_FORMAT_RGB888  = 2, 
	VIDEO_SOURCE_FORMAT_BUT
}VIDEO_SOURCE_FORMAT_E;

typedef enum ad_audio_chn_mux_e
{
	AD_AUDIO_CHN_NONE  = 0,
	AD_AUDIO_CHN_1MUX  = 1,
	AD_AUDIO_CHN_2MUX  = 2,
	AD_AUDIO_CHN_4MUX  = 3,
	AD_AUDIO_CHN_8MUX  = 4,
	AD_AUDIO_CHN_16MUX= 5,
	AD_AUDIO_CHN_BUT
}AD_AUDIO_CHN_MUX_E;

typedef struct audio_port_s
{
	U8 ucPortId;  /*port��*/	

	AD_AUDIO_CHN_MUX_E enChnMulti;	/*���ͨ����*/
	AUDIO_CLK_SLAVE_MODE_E enSlaveMode; /*ʱ������ģʽ*/
	AUDIO_PORT_OUT_FORMAT_E enAudioOutFormat; /*��Ƶ���ģʽ*/
}AUDIO_PORT_S;

typedef struct audio_connect_mode_s
{
	BOOL ucCascade;	/*��Ƶ�Ƿ��� ture / false*/	
	AUDIO_CASCADE_MODE_E enCascadeMode;
	int iCascadeNum; /*��Ƶ��������*/
}AUDIO_CONNECT_MODE_S;

#define MAX_AUDIO_PORT_NUM (2)
typedef struct audio_port_dev_s
{
    U8 ucAuDACPowerDown;            /*�Ƿ�ʹ����ƵDAC Ĭ��0Ϊ�� 1Ϊ�ر�*/
	U8 ucAudioPortCount;            /*ʹ��PORT��Ŀ*/
	AUDIO_CONNECT_MODE_S stAudioConnect;  /*�Ƿ���*/
	AUDIO_PORT_S stAuDioPorts[MAX_AUDIO_PORT_NUM];
}AUDIO_PORT_DEV_S, *AUDIO_PORT_DEV_P;

/*��Ƶ������*/ 
typedef enum ad_audio_samplerate_e
{
	AD_SAMPLE_RATE_8000,
	AD_SAMPLE_RATE_16000,
	AD_SAMPLE_RATE_32000,
	AD_SAMPLE_RATE_44100,
	AD_SAMPLE_RATE_48000,
	AD_SAMPLE_RATE_BUTT
}AD_AUDIO_SAMPLERATE_E;


/*��ƵоƬ*/
typedef struct AUDIO_CHIP_PRI_RES_s
{
	u8 ucInPgaGain;        /* ��Ƶ����ģ������*/
	u8 ucOutPgaGain;       /* ��Ƶ���ģ������*/
	u8 ucInDigitGain;      /* ��Ƶ������������*/
	u8 ucOutDigitGain;     /* ��Ƶ������������*/
	u8 ucBitWidth;	       /*����λ��*/
	u8 ucAudMasterCtrlMode;/*��Ƶ���ӿ���ģʽ ��Ҫ��Ƶ����(��techwell 0xdb)*/
	u8 ucAudioChnStart;    /* AD оƬ��Ͻͨ��*/
	u8 ucAudioChnCount;    /* ���� ��Ҫ���ݲ�ͬоƬ����*/
	u8 aucSeque[8];        /*��Ƶ����˳��*/
	AD_AUDIO_SAMPLERATE_E enSamlerate;/*������*/
}AUDIO_CHIP_PRI_RES_S,*AUDIO_CHIP_PRI_RES_P;

typedef struct VIDEO_CHIP_PRI_RES_s
{
	u8 ucVdClockPloarity; /*��Ƶ�����clock����*/
	u8 ucDatePhaseDelay;  /*������λ��ʱ*/
	u8 ucClockPhaseDelay; /*ʱ����λ��ʱ*/
	u8 ucDataPinDriver;   /*��������������*/
	u8 ucClkPinDriver;    /*ʱ������������*/
	u8 ucVideoChnStart;   /* AD оƬ��Ͻͨ��*/
	u8 ucVideoChnCount;   /* ���� ��Ҫ���ݲ�ͬоƬ����*/
	u8 aucSeque[8];       /*��Ƶ����˳��*/
}VIDEO_CHIP_PRI_RES_S,*VIDEO_CHIP_PRI_RES_P;


#ifndef Size_GUARD
#define Size_GUARD
/// �ߴ�
typedef struct Size
{
	int w;
	int h;
} Size;
#endif

#define CHNS_PER_CHIP_1		(1)
#define CHNS_PER_CHIP_4		(4)
#define CHNS_PER_CHIP_8		(8)

/*�߼�ͨ����������ͨ���Ŷ�Ӧ��ϵ*/
typedef struct chn_map_s
{
	u8 ucLogicChn; /*�߼�ͨ����*/
	u8 ucPhyChn;   /*оƬ����ͨ����*/
}CHN_MAP_S;

/*ͼ�����������ģʽ*/
typedef enum eq_mode_e
{
	EQ_MODE_IN = 0,  /*�ڲ�����*/
	EQ_MODE_EXTEND = 1,/*�ⲿ����*/
}EQ_MODE_E;


/* оƬIO�������� */
typedef enum driver_power_e
{
	DRIVER_POWER_18V = 0,   /* 1.8V */
	DRIVER_POWER_33V = 1,   /* 3.3V */
}DRIVER_POWER_E;


typedef struct tagAdRes_s
{
	int32 iChipAddr;		/*оƬI2C��ַ*/
	int32 iBusId;
	u8 ucChipIndex;         /*��ǰAD ��ţ�AHD�豸���뱣֤�����CVIȫ����ǰ��AHDȫ���ں�!!! ʹ��Adda_Inner_GetChipIndex���·����޴�Ҫ�󣬵�Ҫ��֤ͨ����оƬ˳��һ��*/ 
	u8 u8Bt656Reverse;		/*��Ǹ���port��BT656�Ƿ���Ҫ����ÿλ��ʾһ��port�ڣ�����λ��������λ������Ϊ 0��1��2��3 */
	u8 ucChnCount;			/*��ƬADʹ�õ�ͨ����*/
	
	CHN_MAP_S stChnMap[CHNS_PER_CHIP_8];	/*ͨ��ӳ����Ϣ*/

	EQ_MODE_E enEqMode;						/*EQ��ʽ*/
	VD_PORT_DEV_S stVdPortDev;				/*VO����*/			
	AUDIO_PORT_DEV_S stAudioPortDev;		/*��Ƶ����*/
	DRIVER_POWER_E enDriverPower;    		/* 9931оƬ����������1.8V/3.3V */

	int32 res[4];
}AD_RES_S;

/*�弶����AD/DAоƬ��Դ����*/
typedef struct tagAdsRes_s
{
	int32 iCount;   /*AD /DA ����*/
	int32 iChipTypeNum; /* AD/DA �������� */
	int32 iProductType;
	int32 iCustomType;
	int32 iAddaVideoStandard;
	Size  stViCapLimit[MAX_AD_LOGIC_CHN_NUM]; /*�ɼ����ƣ������ɼ���С����֧��*/
	AD_RES_S AdRes[MAX_AD_NUM];
}ADS_RES_S;


/// ��ȡAD ������Դ��Ϣ
/// 
/// \param [in] pAdsRes AD��Դ��Ϣ�ṹָ��
/// \param [out] ��
/// \return 0 �ɹ�������ʧ��
int32 Ads_GetRes(ADS_RES_S *pAdsRes);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
