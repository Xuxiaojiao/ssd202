/*
**	********************************************************************************
**                                     adda_APIs
**
**   (c) Copyright 2010-2020, ZheJiang DaHua   Technology  CO.LTD.
**                            All Rights Reserved
**
**	File		: adda.h
**	Description	:
**	Modify		: 	xu_longfei1  Create the file
**	********************************************************************************
*/


//�ؼ���������˵��
#if 0

//****************************s********************************************
�ؼ���������:
1)��ʼ������

Adda_I2COpsRegister();//ע��I2C��д��������Ҫ�û��Լ�ʵ��
			  |
Adda_HWResetOpsRegister();//ע��I2C��д��������Ҫ�û��Լ�ʵ��
              |
Adda_Resource_Init(&AddaInitRes) ;//����AD����I2C��ַ��VOʱ��ģʽ��ͨ��ӳ�䡢��Ƶ���á�ʱ��Դģʽ��,AddaInitRes��ϸ����ʵ��������˵��
              |
      Adda_Init()//��ʼ��AD



AddaInitRes����ʵ��˵��:


��Ʒ���:8·ģ�������Ʒ:2ƬDH9931+ ����(3531A)


AddaInitRes.iCount = 2;/*��Ʒһ��ʹ����2ƬAD*/

/*��һƬAD����*/
AddaInitRes.AdRes[0].iI2CBusId = 0;
AddaInitRes.AdRes[0].iChipAddr = 0x60;
AddaInitRes.AdRes[0].ucChipIndex = 0;
AddaInitRes.AdRes[0].ucChnCount = 4;

/*ͨ��ӳ�����ã��߼�ͨ��0 ~3 ��Ӧ��һƬ����ͨ��3��2��1��0*/
AddaInitRes.AdRes[0].stChnMap[0].ucLogicChn = 0;
AddaInitRes.AdRes[0].stChnMap[0].ucPhyChn = 3;
AddaInitRes.AdRes[0].stChnMap[1].ucLogicChn = 1;
AddaInitRes.AdRes[0].stChnMap[1].ucPhyChn = 2;
AddaInitRes.AdRes[0].stChnMap[2].ucLogicChn = 2;
AddaInitRes.AdRes[0].stChnMap[2].ucPhyChn = 1;
AddaInitRes.AdRes[0].stChnMap[3].ucLogicChn = 3;
AddaInitRes.AdRes[0].stChnMap[3].ucPhyChn = 0;
AddaInitRes.AdRes[0].enEqMode = EQ_MODE_IN;/*ʹ���ڲ�EQ*/

/*����ÿ��VO�ڵ�ʱ��ģʽ*/
AddaInitRes.AdRes[0].stVdPortDev.ucCount=4;/*4��VO�ھ�����*/
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[0].enVdPortMuxMode = VD_PORT_MUX_MODE_1MUX;/*1·����ģʽ*/
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[0].enOutHeadMode = VIDEO_HEAD_SINGEL;/*��ͷģʽ*/
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[0].enVdPortOutFormat = VD_PORT_OUT_FORMAT_BT656;/*8bit bt656ģʽ*/
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[1].enVdPortMuxMode = VD_PORT_MUX_MODE_1MUX;
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[1].enOutHeadMode = VIDEO_HEAD_SINGEL;
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[1].enVdPortOutFormat = VD_PORT_OUT_FORMAT_BT656;
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[2].enVdPortMuxMode = VD_PORT_MUX_MODE_1MUX;
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[2].enOutHeadMode = VIDEO_HEAD_SINGEL;
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[2].enVdPortOutFormat = VD_PORT_OUT_FORMAT_BT656;
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[3].enVdPortMuxMode = VD_PORT_MUX_MODE_1MUX;
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[3].enOutHeadMode = VIDEO_HEAD_SINGEL;
AddaInitRes.AdRes[0].stVdPortDev.stVdPort[3].enVdPortOutFormat = VD_PORT_OUT_FORMAT_BT656;
AddaInitRes.AdRes[0].enDriverPower = DRIVER_POWER_18V;/*BT656���߼���ƽ*/
AddaInitRes.AdRes[0].iClockSource = 0;/*ADʱ��Դʹ���ⲿ����*/

/*�ڶ�ƬAD����*/
AddaInitRes.AdRes[1].iI2CBusId = 0;
AddaInitRes.AdRes[1].iChipAddr = 0x62;
AddaInitRes.AdRes[1].ucChipIndex = 1;
AddaInitRes.AdRes[1].ucChnCount = 4;


/*ͨ��ӳ�����ã��߼�ͨ��4 ~7 ��Ӧ�ڶ�Ƭ����ͨ��3��2��1��0*/
AddaInitRes.AdRes[1].stChnMap[0].ucLogicChn = 4;
AddaInitRes.AdRes[1].stChnMap[0].ucPhyChn = 3;
AddaInitRes.AdRes[1].stChnMap[1].ucLogicChn = 5;
AddaInitRes.AdRes[1].stChnMap[1].ucPhyChn = 2;
AddaInitRes.AdRes[1].stChnMap[2].ucLogicChn = 6;
AddaInitRes.AdRes[1].stChnMap[2].ucPhyChn = 1;
AddaInitRes.AdRes[1].stChnMap[3].ucLogicChn = 7;
AddaInitRes.AdRes[1].stChnMap[3].ucPhyChn = 0;
AddaInitRes.AdRes[1].enEqMode = EQ_MODE_IN;

AddaInitRes.AdRes[1].stVdPortDev.ucCount=4;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[0].enVdPortMuxMode = VD_PORT_MUX_MODE_1MUX;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[0].enOutHeadMode = VIDEO_HEAD_SINGEL;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[0].enVdPortOutFormat = VD_PORT_OUT_FORMAT_BT656;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[1].enVdPortMuxMode = VD_PORT_MUX_MODE_1MUX;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[1].enOutHeadMode = VIDEO_HEAD_SINGEL;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[1].enVdPortOutFormat = VD_PORT_OUT_FORMAT_BT656;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[2].enVdPortMuxMode = VD_PORT_MUX_MODE_1MUX;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[2].enOutHeadMode = VIDEO_HEAD_SINGEL;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[2].enVdPortOutFormat = VD_PORT_OUT_FORMAT_BT656;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[3].enVdPortMuxMode = VD_PORT_MUX_MODE_1MUX;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[3].enOutHeadMode = VIDEO_HEAD_SINGEL;
AddaInitRes.AdRes[1].stVdPortDev.stVdPort[3].enVdPortOutFormat = VD_PORT_OUT_FORMAT_BT656;
AddaInitRes.AdRes[1].enDriverPower = DRIVER_POWER_18V;
AddaInitRes.AdRes[1].iClockSource = 0;



2)��ȡͨ��״̬

Adda_GetVideoInStatus()//��ϸ���ӿ�˵��


3)��ȡ������ź�����
Adda_GetVideoInType()//��ϸ���ӿ�˵��


4)�ֶ�ǿ��ָ�������ź�����
Adda_SetVideoInType()//��ϸ���ӿ�˵��

5)�����������ܺ����������º�������˵��������ϸչ��


**************************************************************************//

#endif



#ifndef __ADDA_LIB_H__
#define __ADDA_LIB_H__

typedef int				BOOL;

typedef signed char		s8;
typedef unsigned char	u8;

typedef u8				U8;
typedef	s8				S8;

typedef	s8				schar;
typedef	u8				uchar;

typedef	u8				BYTE;

typedef u8				uint8_t;
typedef s8				sint8_t;

typedef u8				uint8;
typedef s8				int8_t;

typedef	signed short	sint16_t;
typedef	unsigned short	uint16_t;

typedef uint16_t		uint16;
typedef uint16_t		u16;

typedef signed int		int32;
typedef unsigned int	uint32;

typedef signed int		Int32;

typedef	int32			int32_t;
typedef uint32			uint32_t;


#include "adda_resource.h"

typedef enum ad_video_in_format_e
{
	AD_VIDEO_IN_SD_NTSC = 0,
	AD_VIDEO_IN_SD_PAL,
	AD_VIDEO_IN_HD_720P_25HZ = 10,
	AD_VIDEO_IN_HD_720P_30HZ,
	AD_VIDEO_IN_HD_720P_50HZ,
	AD_VIDEO_IN_HD_720P_60HZ,
	AD_VIDEO_IN_HD_1080P_25HZ,
	AD_VIDEO_IN_HD_1080P_30HZ,
	AD_VIDEO_IN_HD_1080P_50HZ,
	AD_VIDEO_IN_HD_1080P_60HZ,
	AD_VIDEO_IN_HD_720I_50HZ,
	AD_VIDEO_IN_HD_720I_60HZ,
	AD_VIDEO_IN_HD_1080I_50HZ,
	AD_VIDEO_IN_HD_1080I_60HZ,
	/*�ʼǱ����÷ֱ���*/
	AD_VIDEO_IN_HD_1600x900_60HZ = 22,
	AD_VIDEO_IN_HD_1440x900_60HZ,
	AD_VIDEO_IN_HD_1366x768_60HZ,
	AD_VIDEO_IN_HD_1280x1024_60HZ,
	AD_VIDEO_IN_HD_1280x800_60HZ,
	AD_VIDEO_IN_HD_1024x768_60HZ,
	AD_VIDEO_IN_HD_800x600_60HZ,
	AD_VIDEO_IN_HD_640x480_60HZ,
	AD_VIDEO_IN_HD_2560x1440_25HZ,	  /*2560x1440*/
	AD_VIDEO_IN_HD_2560x1440_30HZ,	  /*2560x1440*/
	AD_VIDEO_IN_HD_3840x2160_12HZ,	  /*3840x2160*/
	AD_VIDEO_IN_HD_3840x2160_15HZ,	  /*3840x2160*/
	AD_VIDEO_IN_HD_3840x2160_25HZ,	  /*3840x2160*/
	AD_VIDEO_IN_HD_3840x2160_30HZ,	  /*3840x2160*/
	AD_VIDEO_IN_HD_2560x1440_15HZ,	  /*2560x1440 15FPS*/
	AD_VIDEO_IN_HD_2048x1536_25HZ,	  /*2048x1536*/
	AD_VIDEO_IN_HD_2048x1536_30HZ,	  /*2048x1536*/
	AD_VIDEO_IN_HD_2048x1536_18HZ,	  /*2048x1536 18FPS*/
	AD_VIDEO_IN_HD_2592x1944_12HZ,	  /*5M 2592x1944 12FPS*/
	AD_VIDEO_IN_HD_2592x1944_20HZ,	  /*5M 2592x1944 20FPS*/
	AD_VIDEO_IN_HD_1920x1536_18HZ,    /*3M 1920x1536 18FPS*/
}AD_VIDEO_IN_FORMAT_E;

typedef struct AdDetectStatus_s
{
  int32 iVideoFormat;/*ȡֵAD_VIDEO_IN_FORMAT_E*/
  int32 iLostStatus;
  int32 iReportFormat;/*�ϱ�ʹ����ʽ*/
  int32 iVideoSignalType; /* AD�ϱ��ĵ�ǰǰ�˵��ź�����, VideoType_e*/
  int32 iVideoChipType;/*��ǰADоƬ���ͣ�ȡö������ VIDEO_CHIP_E */
  int32 reserved[3];
}AD_DETECT_STATUS_S;

typedef struct AddaResInitParam_s
{
	uint32 u32ProductType; //��Ʒ����
	uint32 u32VideoCapMask; //�߱������豸�ɼ���������

	int reserved[8];
}AddaResInitParam_S;

/**�ֱ��������Ϣ��*��ͬADDA�ɿ����ò�ͬ�Ĳ���*/
typedef union VideoResolution_u
{
    struct _ResInfo0_s
    {
        int width; 		/*���*/
        int height; 	/*�߶�*/
        int frmrate; 	/*֡��*/
        int Interlaed; 	/*���л�����ɨ��*/
    }ResInfo0_s;

    struct _ResInfo1_s
    {
        int activeWidth; 	/*���*/
        int activeHeight; 	/*�߶�*/
        int toltalWidth; 	/*֡��*/
        int toltalHeight; 	/*���л�����ɨ��*/
    }ResInfo1_s;
    uint8 reserved[64];
}VideoResolution_u;


/** ��ƵEQ ������ȡ*/
typedef struct VideoEqParam_s
{
    int32 iCurrEqLevel; //��ǰEQ�ȼ�
    int32 iOptimumEqLevel; //��ǰ����EQ��ѵȼ�
    int32 res[14]; //�����ֽ�
}VideoEqParam_s;


/** ��ƵEQ �������� */
typedef struct VideoEqSetParam_s
{
    int32 iEqSetMode; //EQ�ȼ�����ģʽ 0:ƫ��ֵģʽ  1:����ֵģʽ��ȡֵvideo_in_eq_set_mode_e
    int32 iEqLevel; //EQֵ ���õ�ʱ��ΧΪ[-10, 10] ��ȡ��ʱ��ΧΪ[0,10]
    int32 res[14]; //�����ֽ�
}VideoEqSetParam_s;


/*��Ƶ�ź�����*/
typedef enum VideoType_e
{
    VIDEO_TYPE_TV    = 0,
    VIDEO_TYPE_SDI   = 1,
    VIDEO_TYPE_VGA   = 2,
    VIDEO_TYPE_HDMI  = 3,
    VIDEO_TYPE_DVI   = 4,
    VIDEO_TYPE_HDCVI = 5,
    VIDEO_TYPE_AHD   = 6,
    VIDEO_TYPE_TVI   = 7,
    VIDEO_TYPE_AUTO  = 8,/*AUTOָ�ź����Ͳ���Ӧ��ָ����Ӧ���·���Ҳ��AUTO����������AD�Զ���Ⲣ��Ӧǰ������Դ��������CVI/AHD/TVI�ȵ�����һ��*/
    VIDEO_TYPE_CVBS  = 9,/*ǰ���ź�ΪCVBS*/
    VIDEO_TYPE_DETECTING  = 0x80,/*����ʶ��*/
    VIDEO_TYPE_UNKNOW  = 0x81,/*û�н�����Ƶ*/
    VIDEO_TYPE_BUT,
}VideoType_e;

typedef enum AdChipCategory_s
{
	CATE_VIDEO,   /* ��Ƶ���� */
	CATE_AUDIO,	  /* ��Ƶ���� */
	CATE_BUT
} AdChipCategory;

/*HDMI���ģʽ*/
typedef enum hdmiTransOutputMode_e
{
    HDMITRANS_MODE_DETECT_AUTO = 0,
    HDMITRANS_MODE_FORCE_HDMI  = 1,
    HDMITRANS_MODE_FORCE_DVI   = 2,
    HDMITRANS_MODE_NO_OUT      = 3,
    HDMITRANS_MODE_BUT
}hdmiTransOutputMode_e;


/**
 * ����ʱ�Ӵ�����ʽ
 * ��Դ��Ҳ������һ��ClockEdgeö��,
 * Ϊ��ͷ�ļ�ӳ�䷽�㣬Ū�ɶ����ġ�
 * ����Դͷ�ļ��е�ClockEdgeֻ��������, �������κ�API��ʹ��
 */
typedef enum LatchDataClockEdge_e
{
	LATCH_DATA_CLK_UNKNOW = 0,
	LATCH_DATA_CLK_RISE_EDGE = 1, /* �����ػ���������λ */
	LATCH_DATA_CLK_FALL_EDGE = 2, /* �½��ػ���180����λ */
	LATCH_DATA_CLK_HIGH = 3,	  /* �ߵ�ƽ */
	LATCH_DATA_CLK_LOW = 4,       /* �͵�ƽ */
	LATCH_DATA_CLK_BUT,
}LatchDataClockEdge_e;

#ifndef CaptureUserLineType_e_GUARD
#define CaptureUserLineType_e_GUARD
typedef enum CaptureUserLineType_e
{
    USER_LINE_TYPE_COAXIAL = 0,//ͬ��
    USER_LINE_TYPE_UTP_10OHM,    //10ŷķ�迹˫����
    USER_LINE_TYPE_UTP_17OHM,    //17ŷķ�迹˫����
    USER_LINE_TYPE_UTP_25OHM,    //25ŷķ�迹˫����
    USER_LINE_TYPE_UTP_35OHM,    //35ŷķ�迹˫����

    USER_LINE_TYPE_BUT,
}CaptureUserLineType_e;
#endif

/*��������Ϊ��ǰ�ϵ�������ʽ����������Ϊ�µ�������ʽ���µ�������ʽ�����ٶȲ�ͬ
�ֿ��Է�Ϊ����������ʽ��ǰ���������ʱ���Զ��л���720p��ʽ��
��������������720p@25��720p@50������ʽ�����*/

typedef struct MetaParam
{
    int32_t iTransMode;    /*> ��������ģʽ 0 ��ͨģʽ��1 ����ģʽ 2 �������ݹر� 3 �������ݹر�*/
    int32_t iSpeedMode;    /*�����ٶ�ģʽ���ֿ�����������������*/
    int32_t iStdType;      /*������ʽ����������ģʽ���ַ�����������ʽ������ʽ����*/
    int32_t aiRes[29];      /*> ������ */
}MetaParam_s;

/*AD ��Ƶ�ɼ�״̬ 0 ���� 1�Ĵ����쳣*/
typedef enum ADDA_VIDEO_IN_STATE
{
    ADDA_VIDEO_IN_STATE_NORMAL,         /*����״̬*/
    ADDA_VIDEO_IN_STATE_REGISTER_ERR,   /*�Ĵ����쳣*/
}ADDA_VIDEO_IN_STATE;

typedef enum AudioInType_e
{
    AUDIO_IN_HDCVI = 1,
    AUDIO_IN_NORMAL = 2,
    AUDIO_IN_BUT,
}AudioInType_e;

/* AD ��Ƶ�ɼ�����ģʽ */
typedef enum Adda_Video_In_Mode_e
{
    ADDA_VIDEO_IN_MODE_SD,   /* ����ģʽ */
    ADDA_VIDEO_IN_MODE_HD,   /* ����ģʽ */
}ADDA_VIDEO_IN_MODE_E;

typedef enum ADDA_META_UPDATE_SPEED
{
    ADDA_META_UPDATE_LOW_SPEED,  /*��������*/
    ADDA_META_UPDATE_FSAT_SPEED, /*��������*/
}ADDA_META_UPDATE_SPEED;

#ifndef VIDEO_COLOR_GUARD
#define VIDEO_COLOR_GUARD
/// ��Ƶ��ɫ��ʽ�������ֶηǷ�ֵΪ255
typedef struct VIDEO_COLOR
{
	BYTE	Brightness;		///< ���ȣ�ȡֵ0-100��
	BYTE	Contrast;		///< �Աȶȣ�ȡֵ0-100��
	BYTE 	Saturation;		///< ���Ͷȣ�ȡֵ0-100��
	BYTE 	Hue;			///< ɫ����ȡֵ0-100��
	BYTE 	Gain;			///< ���棬ȡֵ0-100��bit7��λ��ʾ�Զ����棬����λ�����ԡ�
	BYTE	WhiteBalance;	///< �Զ��׵�ƽ���ƣ�bit7��λ��ʾ�����Զ�����.0x0,0x1,0x2�ֱ�����,��,�ߵȼ�
	BYTE	Sharpness;		///< ��ȣ�ȡֵ0-15��
	BYTE	Phase;			///<��λ,ȡֵ0-100
}VIDEO_COLOR;
#endif

/// ��Ƶλ�õ���
#ifndef VIDEO_POSITION_GUARD
#define VIDEO_POSITION_GUARD
typedef struct VIDEO_POSITION{
 int    TopOffset;         ///����ƫ��0--1024
 int    LeftOffset;         ///����ƫ��0--1024
 int    reserved[2];      //����
}VIDEO_POSITION;
#endif


#ifndef Size_GUARD
#define Size_GUARD
/// �ߴ�
typedef struct Size
{
	int w;
	int h;
} Size;
#endif

#ifdef __cplusplus
 extern "C" {
#endif


int32 Adda_DumpReg(int32 iChannel);


///��ʼ��ADDA��, ���������ӿ�ǰ�����ȵ��øú���
/// \param none
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32_t Adda_Resource_Init(ADS_RES_S *pAdsRes) __attribute__((weak));

///ȥ��ʼ��ADDA��, ���������ӿ�ǰ�����ȵ��øú���
/// \param none
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_Resource_DeInit(void);

///I2C ��д����ע��
/// \param in pWriteRegFunc I2Cдʵ�ֺ���
/// \param in pReadRegFunc I2C��ʵ�ֺ���
/// \retval 0  �ɹ�
/// \retval <0  ʧ��

int32 Adda_I2COpsRegister(uint8_t (* pWriteRegFunc)(uint8_t i2c_addr,uint16_t reg_addr, uint8_t val, uint8_t bus_id),uint8_t (* pReadRegFunc)(uint8_t i2c_addr, uint16_t reg_addr, uint8_t bus_id)) __attribute__((weak));

///AD Ӳ����λ�ܽŵĸ�λ����ע�ắ��
/// \param in pRestFunc Ӳ����λ��������Ҫ�û�ʵ�֣�������ʵ�ʵ�Ӳ������

int32 Adda_HWResetOpsRegister(void (* pResetFunc)(void)) __attribute__((weak));

///\��ʼ��ADDAоƬ
///
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_Init(void) __attribute__((weak));

///\ȥ��ʼ��ADDAоƬ
///
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_DeInit(void);


/*********************************��Ƶ�ɼ�������ؽӿ�**********************************************/

///\������Ƶ�����ź�����
///
///\param [in]  iChannel ͨ����
///\param [in]  videoType���ο� videoType_e
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_SetVideoInType(int32 channel, VideoType_e videoType);

///\��ȡ��Ƶ�����ź�����
///
///\param [in]  iChannel ͨ����
///\param [out]  videoType���ο� videoType_e
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_GetVideoInType(int32 channel, VideoType_e *videoType);


///\��������ͼ��Ĳ���
///\param [in]  iChannel ͨ���ţ�
///\param [in]  pstColor ɫ�ʲ���ָ��
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_SetVideoInColor(int32 iChannel, const VIDEO_COLOR *pstColor);


/// ��������ͼ��Ĳ���
///
/// \param [in] iChannel Ӧ���߼�ͨ���š�
/// \param [out] pColor ָ����ɫ�ṹVIDEO _ COLOR��ָ�롣
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_GetVideoInColor(int32 iChannel, VIDEO_COLOR *pstColor);

/// \�����Ƶƫ�Ƶ���
///
/// \�ڻ�ȡ�Ĳ��������ϵ���
/// \param [in] iChannel Ӧ���߼�ͨ���š�
/// \param [in] pos VIDEO_POSITIONƫ�Ʋ���ָ��
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_SetVideoInPosition (int32 iChannel, const VIDEO_POSITION *pstPosition);


/// \��ȡ������Ƶƫ��
///
/// \param [in] iChannel Ӧ���߼�ͨ���š�
/// \param [out] pos VIDEO_POSITIONƫ�Ʋ���ָ��
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_GetVideoInPosition (int32 iChannel, VIDEO_POSITION *pstPosition);


/// \��ȡ��ǰ�߼�ͨ��״̬
/// \param [in] iChannel ͨ����
/// \param [in] pStatus ͨ����ʧ״̬
/// \retval  0  ��ȡ�ɹ�
/// \retval <0  ��ȡʧ��
int32 Adda_GetVideoInStatus(int32 iChannel, AD_DETECT_STATUS_S *pStatus) __attribute__((weak));

///\���͸������ݵ�ǰ���豸�����ӿ�
///
/// \param [in] channel ͨ���š�
/// \param [in] buf  ����buf
/// \param [in] buf ����
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
Int32 Adda_PutVideoInMetaData(int32 iChannel, const void *buf, uint32 len);

 ///\���͸������ݵ�ǰ���豸�Ƕ����ӿ�
///
/// \param [in] channel ͨ���š�
/// \param [in] buf  ����buf
/// \param [in] buf ����
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
Int32 Adda_PutVideoInMetaDataNoBlock(int32 iChannel, const void *buf, uint32 len);

///\����ǰ���豸���صĸ�������
///
///\����оƬ��DH9901
/// \param [in] channel ͨ���š�
/// \param [in out] buf  ����buf
/// \param [in out] buf ����
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
Int32 Adda_GetVideoInMetaData(int32 iChannel, void *buf, uint32 *len);


/// \����ͬ������������
///
/// \param[in] iChannel��Ƶ ͨ����
/// \iMetaParam ��������
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_SetVideoInMetaParam(int32 iChannel, MetaParam_s iMetaParam);


/// \����ͼ�����
///
///\����оƬ��DH9901
/// \param[in] iChannel ͨ����
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_SetVideoInEquation(int32 iChannel);


///���ô��������ͣ���Ҫ����������������Ӧ��AD����
///\param [in]ichannel ͨ����
///\param [in]elinetype ���������ߵ�����
//\param  [out]����0��ʾ�ɹ�
int32 Adda_SetVideoInLineType(int32 iChannel, CaptureUserLineType_e elinetype);


/// \AD �ֶ�������Ƶ����EQ����
///
/// \param[in]  iChannel  ��Ƶ����ͨ����
/// \param[in]  EqParam   Eq����
/// \retval 0   �ɹ�
/// \retval <0  ʧ��
int32 Adda_SetVideoInEqParam(int32 iChannel, const VideoEqSetParam_s *pEqParam);

/// \AD ��ȡ��Ƶ����EQ����
///
/// \param[in]  iChannel  ��Ƶ����ͨ����
/// \param[in]  EqParam   Eq����
/// \retval 0   �ɹ�
/// \retval <0  ʧ��
int32 Adda_GetVideoInEqParam(int32 iChannel, VideoEqParam_s *pEqParam);


/*********************************��Ƶ��ʾ�����ؽӿ�**********************************************/

///\���������Ƶ����
///\param [in]  iChannel ͨ���ţ�
///\param [in]  pstColorɫ�ʲ���ָ��
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_SetVideoOutColor(int32 iChannel, const VIDEO_COLOR *pstColor);


/// ��ȡ�����Ƶ����ɫ������
///
/// \param [in] iChannel Ӧ���߼�ͨ���š�
/// \param [in] pColor ָ����ɫ�ṹVIDEO _ COLOR��ָ�롣
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_GetVideoOutColor(int32 iChannel, VIDEO_COLOR *pstColor);

/// \�����Ƶƫ�Ƶ���
/// \�ڻ�ȡ�Ĳ��������ϵ���
/// \param [in] iChannel Ӧ���߼�ͨ���š�
/// \param [in] pos VIDEO_POSITIONƫ�Ʋ���ָ��
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_SetVideoOutPosition(int32 iChannel, const VIDEO_POSITION * pstPosition);

/// \��ȡ��Ƶƫ��
///
/// \param [in] iChannel Ӧ���߼�ͨ���š�
/// \param [out] pos VIDEO_POSITIONƫ�Ʋ���ָ��
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_GetVideoOutPosition(int32 iChannel, VIDEO_POSITION * pstPosition);

/*********************************��Ƶ���������ؽӿ�**********************************************/

/// �л���Ƶ���ͨ����
///
/// \param [in] iChannel ��Ƶͨ���š�
/// \retval 0  �л��ɹ���
/// \retval <0  �л�ʧ�ܡ�
int32 Adda_SetAudioOutCh(int32 iChannel);


/// ���������
///
/// \param [in] iChannel ��Ƶͨ���š�
/// \retval 0  �л��ɹ���
/// \retval <0  �л�ʧ�ܡ�
int32 Adda_MuteAudioOut(int32 iChannel);


/// \�����������
///
/// \param [in] iChannel Ӧ���߼�ͨ���š�
/// \param [in] ucValue ����ֵ0 - 100
/// \retval 0  ���óɹ�
/// \retval <0  ����ʧ��
int32 Adda_SetAudioOutVolume(int32 iChannel, uint8 ucValue);

/// \������������
///
/// \param [in] iChannel Ӧ���߼�ͨ���š�
/// \param [in] ucValue ����ֵ0 - 100
/// \retval 0  ���óɹ�
/// \retval <0  ����ʧ��
int32 Adda_SetAudioInVolume(int32 iChannel, uint8 ucValue);


/// \ ������Ƶ����ͨ��ʹ��
///
///\param [in] i32Port ��Ƶ����ͨ���š�
///\param [in] enable  ʹ��0:�ر�;��0:����
/// \retval 0  ���óɹ���
/// \retval <0  ����ʧ�ܡ�
int32 Adda_SetAudioInEnable(int32_t iChannel, int32_t iEnable);

/// \AD ������ƵԴ
///
/// \param[in]  AudioInCh  ��Ƶ����ͨ����
/// \param[in]  pAudioInType   ��ƵԴ����
/// \retval 0   �ɹ�
/// \retval <0  ʧ��
int32 Adda_SetAudioInSource(int32 iChannel, AudioInType_e AudioInType);


/// \��ȡADDA����״̬ ���ڼ��AD�Ƿ��ڸ�λ���쳣״̬
///
/// \param[in] iChannel ͨ����
/// \�޷���ֵ
int32 Adda_GetDeviceState(int32 iChannel, ADDA_VIDEO_IN_STATE *pVideoInState);

/// \��ȡָ��ͨ����ӦAD�Ĺ���״̬
///
/// \param[in] iChannel ͨ����
/// \param[in] eAdCate оƬ����
/// \param[out] pVideoInState оƬ״̬
/// \retval 0  �ɹ�
/// \retval <0  ʧ��
int32 Adda_GetChipState(int32 iChannel, AdChipCategory eAdCate, ADDA_VIDEO_IN_STATE *pVideoInState);

/// \��ȡָ��ͨ����Ӧ��оƬ��
///
/// \param[in] iChannel ͨ����
/// \param[in] eAdCate оƬ����
/// \retval >=0: ͨ����Ӧ��оƬ��
/// \retval <0:  ʧ��
int32 Adda_GetAdIndex(int iChannel, AdChipCategory eAdCate);


#ifdef __cplusplus
}
#endif


#endif
