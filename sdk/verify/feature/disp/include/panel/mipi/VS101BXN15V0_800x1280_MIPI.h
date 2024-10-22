#include "mi_panel_datatype.h"

#define FLAG_DELAY            0xFE
#define FLAG_END_OF_TABLE     0xFF   // END OF REGISTERS MARKER

MI_PANEL_ParamConfig_t stPanelParam =
{
    "VS101BXN15V0_800x1280", // const char *m_pPanelName;                ///<  PanelName
    0, //MS_U8 m_bPanelDither :1;                 ///<  PANEL_DITHER, keep the setting
    E_MI_PNL_LINK_MIPI_DSI, //MHAL_DISP_ApiPnlLinkType_e m_ePanelLinkType   :4;  ///<  PANEL_LINK

    0,        //MI_U8 MI_U8DualPort      :1;          ///<  DualPort on/off
    0,        //MI_U8 MI_U8SwapPort      :1;          ///<  Swap Port on/off
    0,        //MI_U8 MI_U8SwapOdd_ML    :1;          ///<  Swap Odd ML
    0,        //MI_U8 MI_U8SwapEven_ML   :1;          ///<  Swap Even ML
    0,        //MI_U8 MI_U8SwapOdd_RB    :1;          ///<  Swap Odd RB
    0,        //MI_U8 MI_U8SwapEven_RB   :1;          ///<  Swap Even RB

    1,        //MI_U8 MI_U8SwapLVDS_POL  :1;          ///<  Swap LVDS Channel Polairyt
    1,        //MI_U8 MI_U8SwapLVDS_CH   :1;          ///<  Swap LVDS channel
    0,        //MI_U8 MI_U8PDP10BIT      :1;          ///<  PDP 10bits on/off
    1,        //MI_U8 MI_U8LVDS_TI_MODE  :1;          ///<  Ti Mode On/Off


    0,        //MI_U8 MI_U8DCLKDelay;                 ///<  DCLK Delay
    0,        //MI_U8 MI_U8InvDCLK   :1;              ///<  CLK Invert
    0,        //MI_U8 MI_U8InvDE     :1;              ///<  DE Invert
    0,        //MI_U8 MI_U8InvHSync  :1;              ///<  HSync Invert
    0,        //MI_U8 MI_U8InvVSync  :1;              ///<  VSync Invert

    ///////////////////////////////////////////////
    // Output driving current setting
    ///////////////////////////////////////////////
    // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
    1,  //MS_U8 m_ucPanelDCKLCurrent;              ///<  define PANEL_DCLK_CURRENT
    1,  //MS_U8 m_ucPanelDECurrent;                ///<  define PANEL_DE_CURRENT
    1,  //MS_U8 m_ucPanelODDDataCurrent;           ///<  define PANEL_ODD_DATA_CURRENT
    1,  //MS_U8 m_ucPanelEvenDataCurrent;          ///<  define PANEL_EVEN_DATA_CURRENT

    ///////////////////////////////////////////////
    // panel on/off timing
    ///////////////////////////////////////////////
    30,  //MS_U16 m_wPanelOnTiming1;                ///<  time between panel & data while turn on power
    400,  //MS_U16 m_wPanelOnTiming2;                ///<  time between data & back light while turn on power
    80,  //MS_U16 m_wPanelOffTiming1;               ///<  time between back light & data while turn off power
    30,  //MS_U16 m_wPanelOffTiming2;               ///<  time between data & panel while turn off power

    20,        //u16 u16HSyncWidth;               ///<  Hsync Width
    20,       //u16 u16HSyncBackPorch;           ///<  Hsync back porch

    4,       //u16 u16VSyncWidth;               ///<  Vsync width
    10,      //u16 u16VSyncBackPorch;           ///<  Vsync back porch

    0,       //u16 u16HStart;                   ///<  HDe start
    0,        //u16 u16VStart;                   ///<  VDe start
    800,      //u16 u16Width;                    ///<  Panel Width
    1280,     //u16 u16Height;                   ///<  Panel Height

    0,      //u16 u16MaxHTotal;                ///<  Max H Total
    860,      //u16 u16HTotal;                   ///<  H Total
    0,      //u16 u16MinHTotal;                ///<  Min H Total

    0,     //u16 u16MaxVTotal;                ///<  Max V Total
    1324,     //u16 u16VTotal;                   ///<  V Total
    0,     //u16 u16MinVTotal;                ///<  Min V Total

    0,       //u16 u16MaxDCLK;                  ///<  Max DCLK
    68,       //u16 u16DCLK;                     ///<  DCLK ( Htt * Vtt * Fps)
    0,       //u16 u16MinDCLK;                  ///<  Min DCLK

    0,    //u16 u16SpreadSpectrumStep;       ///<  Step of SSC
    0,    //u16 u16SpreadSpectrumSpan;       ///<  Span of SSC

    0xA0,      //MI_U8 MI_U8DimmingCtl;                 ///<  Dimming Value
    0xFF,      //MI_U8 MI_U8MaxPWMVal;                  ///<  Max Dimming Value
    0x50,      //MI_U8 MI_U8MinPWMVal;                  ///<  Min Dimming Value

    0,                            //MI_U8 MI_U8DeinterMode   :1;                  ///<  DeInter Mode
    E_MI_PNL_ASPECT_RATIO_WIDE, //MIPnlAspectRatio_e ePanelAspectRatio; ///<  Aspec Ratio

    0,                            //u16 u16LVDSTxSwapValue;         // LVDS Swap Value
    E_MI_PNL_TI_8BIT_MODE,      //MIPnlTiBitMode_e eTiBitMode;  // Ti Bit Mode
    E_MI_PNL_OUTPUT_8BIT_MODE,  //MHAL_DISP_ApiPnlOutPutFormatBitMode_e m_ucOutputFormatBitMode;

    0,        //MI_U8 MI_U8SwapOdd_RG    :1;          ///<  Swap Odd RG
    0,        //MI_U8 MI_U8SwapEven_RG   :1;          ///<  Swap Even RG
    0,        //MI_U8 MI_U8SwapOdd_GB    :1;          ///<  Swap Odd GB
    0,        //MI_U8 MI_U8SwapEven_GB   :1;          ///<  Swap Even GB

    0,        //MI_U8 MI_U8DoubleClk     :1;                      ///<  Double CLK On/off
    0x1C848E, //u32 u32MaxSET;                              ///<  Max Lpll Set
    0x18EB59, //u32 u32MinSET;                              ///<  Min Lpll Set
    E_MI_PNL_CHG_HTOTAL, //MIPnlOutputTimingMode_e eOutTimingMode;   ///<  Define which panel output timing change mode is used to change VFreq for same panel
    0,                     //MI_U8 MI_U8NoiseDith     :1;                      ///<  Noise Dither On/Off
    E_MI_PNL_CH_SWAP_2,
    E_MI_PNL_CH_SWAP_4,
    E_MI_PNL_CH_SWAP_3,
    E_MI_PNL_CH_SWAP_1,
    E_MI_PNL_CH_SWAP_0,
};

MI_U8 JD9365_CMD[] =
{
    0xE0,0x01,0x00,
    //--- PASSWORD  ----//
    0xE1,0x01,0x93,
    0xE2,0x01,0x65,
    0xE3,0x01,0xF8,
    0x80,0x01,0x03,  //4lanes
    //--- Page1  ----//
    0xE0,0x01,0x01,
    //Set VCOM
    0x00,0x01,0x00,
    0x01,0x01,0x6F,   //0x9A
    //Set VCOM_Reverse
    0x03,0x01,0x00,
    0x04,0x01,0x6A,
    //Set Gamma Power,0x01, VGMP,VGMN,VGSP,VGSN
    0x17,0x01,0x00,
    0x18,0x01,0xAF,  //4.3V
    0x19,0x01,0x01,  //0.3V
    0x1A,0x01,0x00,
    0x1B,0x01,0xAF,  //4.3V
    0x1C,0x01,0x01,  //0.3V
    //Set Gate Power
    0x1F,0x01,0x3E,	//VGH_R  = 15V  
    0x20,0x01,0x28,	//VGL_R  = -12V 
    0x21,0x01,0x28,	//VGL_R2 = -12V  
    0x22,0x01,0x7E,	//PA[6]=1, PA[5]=1, PA[4]=1, PA[0]=0 
    //SETPANEL
    0x35,0x01,0x26,	//ASP=0110
    //SETPANEL
    0x37,0x01,0x09,	//SS=1,BGR=1
    //SET RGBCYC
    0x38,0x01,0x04,	//JDT=100 column inversion
    0x39,0x01,0x00,	//RGB_N_EQ1, 0x12
    0x3A,0x01,0x01,	//RGB_N_EQ2, 0x18
    0x3C,0x01,0x7C,	//SET EQ3 for TE_H
    0x3D,0x01,0xFF,	//SET CHGEN_ON,
    0x3E,0x01,0xFF,	//SET CHGEN_OFF,
    0x3F,0x01,0x7F,	//SET CHGEN_OFF2,
    //Set TCON
    0x40,0x01,0x06,	//RSO 06h=800,
    0x41,0x01,0xA0,	//LN=640->1280 line
    0x42,0x01,0x81,	//SLT
    0x43,0x01,0x08,	//VFP=8
    0x44,0x01,0x0B,	//VBP=12
    0x45,0x01,0x28,	//HBP=40
    //--- power voltage  ----//
    0x55,0x01,0x01,	//DCDCM=0001, JD PWR_IC
    0x57,0x01,0x69,	//VGH,0xAC
    0x59,0x01,0x0A,	//VCL = -2.9V
    0x5A,0x01,0x28,	//VGH ,15V
    0x5B,0x01,0x14,	//VGL,-10V
    //--- Gamma  ----//
    0x5D,0x01,0x7C,  //0x7F//255
    0x5E,0x01,0x65,  //0x53//251
    0x5F,0x01,0x55,  //0x3F//247
    0x60,0x01,0x47,  //0x33//243
    0x61,0x01,0x43,  //0x2E//235
    0x62,0x01,0x32,  //0x1F//227
    0x63,0x01,0x34,  //0x24//211
    0x64,0x01,0x1C,  //0x0F//191
    0x65,0x01,0x33,  //0x2A//159
    0x66,0x01,0x31,  //0x2A//128
    0x67,0x01,0x30,  //0x2C//96
    0x68,0x01,0x4E,  //0x4B//64
    0x69,0x01,0x3C,  //0x3A//44
    0x6A,0x01,0x44,  //0x46//28
    0x6B,0x01,0x35,  //0x38//20
    0x6C,0x01,0x31,  //0x34//12
    0x6D,0x01,0x23,  //0x22//8
    0x6E,0x01,0x11,  //0x11//4
    0x6F,0x01,0x00,  //0x00//0
    0x70,0x01,0x7C,  //0x7F//255
    0x71,0x01,0x65,  //0x53//251
    0x72,0x01,0x55,  //0x3F//247
    0x73,0x01,0x47,  //0x33//243
    0x74,0x01,0x43,  //0x2E//235
    0x75,0x01,0x32,  //0x1F//227
    0x76,0x01,0x34,  //0x24//211
    0x77,0x01,0x1C,  //0x0F//191
    0x78,0x01,0x33,  //0x2A//159
    0x79,0x01,0x31,  //0x2A//128
    0x7A,0x01,0x30,  //0x2C//96
    0x7B,0x01,0x4E,  //0x4B//64
    0x7C,0x01,0x3C,  //0x3A//44
    0x7D,0x01,0x44,  //0x46//28
    0x7E,0x01,0x35,  //0x38//20
    0x7F,0x01,0x31,  //0x34//12
    0x80,0x01,0x23,  //0x22//8
    0x81,0x01,0x11,  //0x11//4
    0x82,0x01,0x00,  //0x00//0
    //Page2,0x01, for GIP
    0xE0,0x01,0x02,
    //GIP_L Pin mapping 
    0x00,0x01,0x1E,//1  VDS                        
    0x01,0x01,0x1E,//2  VDS                        
    0x02,0x01,0x41,//3  STV2                       
    0x03,0x01,0x41,//4  STV2                       
    0x04,0x01,0x1F,//5                         
    0x05,0x01,0x1F,//6                       
    0x06,0x01,0x1F,//7  VSD                        
    0x07,0x01,0x1F,//8  VSD                        
    0x08,0x01,0x1F,//9  GCL                        
    0x09,0x01,0x1F,//10                            
    0x0A,0x01,0x1E,//11 GCH                        
    0x0B,0x01,0x1E,//12 GCH                        
    0x0C,0x01,0x1F,//13                            
    0x0D,0x01,0x47,//14 CLK8                       
    0x0E,0x01,0x47,//15 CLK8                       
    0x0F,0x01,0x45,//16 CLK6                       
    0x10,0x01,0x45,//17 CLK6                       
    0x11,0x01,0x4B,//18 CLK4                       
    0x12,0x01,0x4B,//19 CLK4                       
    0x13,0x01,0x49,//20 CLK2                       
    0x14,0x01,0x49,//21 CLK2                       
    0x15,0x01,0x1F,//22 VGL
    //GIP_R Pin mapping 
    0x16,0x01,0x1E,//1  VDS                 
    0x17,0x01,0x1E,//2  VDS                
    0x18,0x01,0x40,//3  STV1               
    0x19,0x01,0x40,//4  STV1               
    0x1A,0x01,0x1F,//5                
    0x1B,0x01,0x1F,//6             
    0x1C,0x01,0x1F,//7  VSD                
    0x1D,0x01,0x1F,//8  VSD                
    0x1E,0x01,0x1F,//9  GCL                
    0x1F,0x01,0x1F,//10                    
    0x20,0x01,0x1E,//11 GCH                
    0x21,0x01,0x1E,//12 GCH                
    0x22,0x01,0x1f,//13                    
    0x23,0x01,0x46,//14 CLK7               
    0x24,0x01,0x46,//15 CLK7               
    0x25,0x01,0x44,//16 CLK5               
    0x26,0x01,0x44,//17 CLK5               
    0x27,0x01,0x4A,//18 CLK3               
    0x28,0x01,0x4A,//19 CLK3               
    0x29,0x01,0x48,//20 CLK1               
    0x2A,0x01,0x48,//21 CLK1               
    0x2B,0x01,0x1F,//22 VGL
    //GIP_L_GS Pin mapping
    0x2C,0x01,0x1F,//1  VDS 		0x1E
    0x2D,0x01,0x1F,//2  VDS          0x1E
    0x2E,0x01,0x40,//3  STV2         0x41
    0x2F,0x01,0x40,//4  STV2         0x41
    0x30,0x01,0x1F,//5 
    0x31,0x01,0x1F,//6  
    0x32,0x01,0x1E,//7  VSD          0x1F
    0x33,0x01,0x1E,//8  VSD          0x1F
    0x34,0x01,0x1F,//9  GCL          0x1F
    0x35,0x01,0x1F,//10              0x1F
    0x36,0x01,0x1E,//11 GCH          0x1E
    0x37,0x01,0x1E,//12 GCH          0x1E
    0x38,0x01,0x1F,//13              0x1F
    0x39,0x01,0x48,//14 CLK8         0x47
    0x3A,0x01,0x48,//15 CLK8         0x47
    0x3B,0x01,0x4A,//16 CLK6         0x45
    0x3C,0x01,0x4A,//17 CLK6         0x45
    0x3D,0x01,0x44,//18 CLK4         0x4B
    0x3E,0x01,0x44,//19 CLK4         0x4B
    0x3F,0x01,0x46,//20 CLK2         0x49
    0x40,0x01,0x46,//21 CLK2         0x49
    0x41,0x01,0x1F,//22 VGL          0x1F

    //GIP_R_GS Pin mapping
    0x42,0x01,0x1F,//1  VDS 		0x1E
    0x43,0x01,0x1F,//2  VDS          0x1E
    0x44,0x01,0x41,//3  STV1         0x40
    0x45,0x01,0x41,//4  STV1         0x40
    0x46,0x01,0x1F,//5  
    0x47,0x01,0x1F,//6  
    0x48,0x01,0x1E,//7  VSD          0x1F
    0x49,0x01,0x1E,//8  VSD          0x1F
    0x4A,0x01,0x1E,//9  GCL          0x1F
    0x4B,0x01,0x1F,//10              0x1f
    0x4C,0x01,0x1E,//11 GCH          0x1E
    0x4D,0x01,0x1E,//12 GCH          0x1E
    0x4E,0x01,0x1F,//13              0x1f
    0x4F,0x01,0x49,//14 CLK7         0x46
    0x50,0x01,0x49,//15 CLK7         0x46
    0x51,0x01,0x4B,//16 CLK5         0x44
    0x52,0x01,0x4B,//17 CLK5         0x44
    0x53,0x01,0x45,//18 CLK3         0x4A
    0x54,0x01,0x45,//19 CLK3         0x4A
    0x55,0x01,0x47,//20 CLK1         0x48
    0x56,0x01,0x47,//21 CLK1         0x48
    0x57,0x01,0x1F,//22 VGL          0x1f
    //GIP Timing  
    0x58,0x01,0x40, 
    0x5B,0x01,0x30, //STV_NUM,STV_S0
    0x5C,0x01,0x03, //STV_S0
    0x5D,0x01,0x30, //STV_W / S1
    0x5E,0x01,0x01, //STV_S2
    0x5F,0x01,0x02, //STV_S3
    0x63,0x01,0x14, //SETV_ON  
    0x64,0x01,0x6A, //SETV_OFF 
    0x67,0x01,0x73, 
    0x68,0x01,0x05, 
    0x69,0x01,0x14, 
    0x6A,0x01,0x6A, 
    0x6B,0x01,0x08, //Dummy clk

    0x6C,0x01,0x00, 
    0x6D,0x01,0x00, 
    0x6E,0x01,0x00, 
    0x6F,0x01,0x88, 

    0x77,0x01,0xDD, 
    0x79,0x01,0x0E,//0x0C 
    0x7A,0x01,0x03,//0x04
    0x7D,0x01,0x14, 
    0x7E,0x01,0x6A, 

    //Page4
    0xE0,0x01,0x04,
    0x09,0x01,0x11,
    0x0E,0x01,0x48,
    0x2B,0x01,0x2B,
    0x2D,0x01,0x03,//defult 0x01
    0x2E,0x01,0x44,

    //Page0
    0xE0,0x01,0x00,

    0xE6,0x01,0x02,
    0xE7,0x01,0x0C,

    FLAG_DELAY, FLAG_DELAY, 20,
    0x11, 0, 0x00,
    FLAG_DELAY, FLAG_DELAY, 120,
    0x29, 0, 0x00,
    FLAG_DELAY, FLAG_DELAY, 20,
    0x35,0x01,0x00,
    FLAG_END_OF_TABLE, FLAG_END_OF_TABLE,
};

MI_PANEL_MipiDsiConfig_t stMipiDsiConfig =
{
    //HsTrail HsPrpr HsZero ClkHsPrpr ClkHsExit ClkTrail ClkZero ClkHsPost DaHsExit ContDet
    0x05,   0x03,  0x05,  0x0A,     0x0E,     0x03,    0x0B,   0x0A,     0x05,    0x00,
    //Lpx   TaGet  TaSure  TaGo
    0x10, 0x1a,  0x16,   0x32,

    //Hac,  Hpw,  Hbp,  Hfp,  Vac,  Vpw, Vbp, Vfp,  Bllp, Fps
    800,    20,    20,  20,   1280, 4,   10,   30,   0,    60,

    E_MI_PNL_MIPI_DSI_LANE_4,      // MIPnlMipiDsiLaneMode_e enLaneNum;
    E_MI_PNL_MIPI_DSI_RGB888,      // MIPnlMipiDsiFormat_e enFormat;
    E_MI_PNL_MIPI_DSI_SYNC_PULSE,  // MIPnlMipiDsiCtrlMode_e enCtrl;

    JD9365_CMD,
    sizeof(JD9365_CMD),
    1, 0x01AF, 0x01B9, 0x80D2, 9,
};

