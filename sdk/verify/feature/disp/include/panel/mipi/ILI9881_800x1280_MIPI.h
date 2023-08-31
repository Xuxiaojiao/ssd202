#include "mi_panel_datatype.h"

#define FLAG_DELAY            0xFE
#define FLAG_END_OF_TABLE     0xFF   // END OF REGISTERS MARKER

MI_PANEL_ParamConfig_t stPanelParam =
{
    "ILI9881_mipi_800x1280", // const char *m_pPanelName;                ///<  PanelName
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
    80,       //u16 u16HSyncBackPorch;           ///<  Hsync back porch

    10,       //u16 u16VSyncWidth;               ///<  Vsync width
    20,      //u16 u16VSyncBackPorch;           ///<  Vsync back porch

    0,       //u16 u16HStart;                   ///<  HDe start
    0,        //u16 u16VStart;                   ///<  VDe start
    800,      //u16 u16Width;                    ///<  Panel Width
    1280,     //u16 u16Height;                   ///<  Panel Height

    0,      //u16 u16MaxHTotal;                ///<  Max H Total
    960,      //u16 u16HTotal;                   ///<  H Total
    0,      //u16 u16MinHTotal;                ///<  Min H Total

    0,     //u16 u16MaxVTotal;                ///<  Max V Total
    1330,     //u16 u16VTotal;                   ///<  V Total
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
    E_MI_PNL_CH_SWAP_2, //clk  def  SSD20x   pannel  jiefan  //3 - 0, 0 - 3.
    E_MI_PNL_CH_SWAP_4, 	//4  3    3 -->	3        ssd201  0 --> 3
	E_MI_PNL_CH_SWAP_3, 	//3  2    2 -->	2
	E_MI_PNL_CH_SWAP_1, 	//1  1    1 -->	1
	E_MI_PNL_CH_SWAP_0, 	//0  0    0 -->	0
};

MI_U8 JD9365_CMD[] =
{
	0xFF,3,0x98,0x81,0x03,
				//GIP_1
	0x01,1,0x00, 
	0x02,1,0x00,
	0x03,1,0x53,		 //STVA
	0x04,1,0x00,		 //STVB
	0x05,1,0x00,		 //STVC
	0x06,1,0x08,		 //STVA_Rise
	0x07,1,0x00,		 //STVB_Rise
	0x08,1,0x00,		 //STVC_Rise
	0x09,1,0x00,		 //FTI1R(A)
	0x0a,1,0x00,		 //FTI2R(B)
	0x0b,1,0x00,		 //FTI3R(C)
	0x0c,1,0x00,		 //FTI1F(A)
	0x0d,1,0x00,		 //FTI2F(B)
	0x0e,1,0x00,		 //FTI2F(C)
	0x0f,1,0x26,		 //CLW1(ALR)	00
	0x10,1,0x26,		 //CLW2(ARR)	00
	0x11,1,0x00,			
	0x12,1,0x00,		 
	0x13,1,0x00,		 //CLWX(ATF)
	0x14,1,0x00,
	0x15,1,0x00,		 //GPMRi(ALR)
	0x16,1,0x00,		 //GPMRii(ARR)
	0x17,1,0x00,		 //GPMFi(ALF)
	0x18,1,0x00,		 //GPMFii(AFF)
	0x19,1,0x00,
	0x1a,1,0x00,
	0x1b,1,0x00,	
	0x1c,1,0x00,
	0x1d,1,0x00,
	0x1e,1,0x40,		 //CLKA 40笆は C0も笆は(X8把σCLKB)
	0x1f,1,0xC0,		 //C0
	0x20,1,0x06,		 //CLKA_Rise
	0x21,1,0x01,		 //CLKA_Fall
	0x22,1,0x07,		 //CLKB_Rise(keep toggle惠砞CLK A)
	0x23,1,0x00,		 //CLKB_Fall
	0x24,1,0x8A,		 //CLK keep toggle(AL) 8X┕オ
	0x25,1,0x8A,		 //CLK keep toggle(AR) 8X┕オ
	0x26,1,0x00,
	0x27,1,0x00,
	0x28,1,0x33,		//CLK Phase   3B
	0x29,1,0x33,		//CLK overlap 
	0x2a,1,0x00,  
	0x2b,1,0x00,
	0x2c,1,0x08,		//GCH R
	0x2d,1,0x08,		//GCL R 
	0x2e,1,0x0B,		//GCH F 	   
	0x2f,1,0x0B,		//GCL F
	0x30,1,0x00,
	0x31,1,0x00,
	0x32,1,0x42,		//GCH/L ext2/1︽  5E 01:31	 5E 00:42
	0x33,1,0x00,
	0x34,1,0x00,		//VDD1&2 non-overlap 04:2.62us
	0x35,1,0x0A,		//GCH/L 跋丁 00:VS玡 01:VS 10:阁VS 11:frameい		
	0x36,1,0x00,
	0x37,1,0x08,		//GCH/L
	0x38,1,0x3C,	//VDD1&2 toggle 1sec
	0x39,1,0x00,
	0x3a,1,0x00, 
	0x3b,1,0x00,
	0x3c,1,0x00,
	0x3d,1,0x00,
	0x3e,1,0x00,
	0x3f,1,0x00,
	0x40,1,0x00,
	0x41,1,0x00,
	0x42,1,0x00,
	0x43,1,0x08,		//GCH/L
	0x44,1,0x00,
			//GIP_2
	0x50,1,0x01,
	0x51,1,0x23,
	0x52,1,0x45,
	0x53,1,0x67,
	0x54,1,0x89,
	0x55,1,0xab,
	0x56,1,0x01,
	0x57,1,0x23,
	0x58,1,0x45,
	0x59,1,0x67,
	0x5a,1,0x89,
	0x5b,1,0xab,
	0x5c,1,0xcd,
	0x5d,1,0xef,
			//GIP_3
	0x5e,1,0x00,
	0x5f,1,0x01,	  //FW_CGOUT_L[1]	 VDS
	0x60,1,0x01,	  //FW_CGOUT_L[2]	 VDS
	0x61,1,0x06,	  //FW_CGOUT_L[3]	 STV2
	0x62,1,0x06,	  //FW_CGOUT_L[4]	 STV2
	0x63,1,0x06,	  //FW_CGOUT_L[5]	 STV4
	0x64,1,0x06,	  //FW_CGOUT_L[6]	 STV4
	0x65,1,0x00,	  //FW_CGOUT_L[7]	 VSD
	0x66,1,0x00,	  //FW_CGOUT_L[8]	 VSD
	0x67,1,0x17,	  //FW_CGOUT_L[9]	 GCL
	0x68,1,0x02,	  //FW_CGOUT_L[10]	 
	0x69,1,0x16,	  //FW_CGOUT_L[11]	 GCH  
	0x6a,1,0x16,	  //FW_CGOUT_L[12]	 GCH
	0x6b,1,0x02,	  //FW_CGOUT_L[13]	 
	0x6c,1,0x0D,	  //FW_CGOUT_L[14]	 CLK8	
	0x6d,1,0x0D,	  //FW_CGOUT_L[15]	 CLK8
	0x6e,1,0x0C,	  //FW_CGOUT_L[16]	 CLK6	 
	0x6f,1,0x0C,	  //FW_CGOUT_L[17]	 CLK6
	0x70,1,0x0F,	  //FW_CGOUT_L[18]	 CLK4
	0x71,1,0x0F,	  //FW_CGOUT_L[19]	 CLK4
	0x72,1,0x0E,	  //FW_CGOUT_L[20]	 CLK2
	0x73,1,0x0E,	  //FW_CGOUT_L[21]	 CLK2
	0x74,1,0x02,	  //FW_CGOUT_L[22]	 VGL
	
	0x75,1,0x01,	  //BW_CGOUT_L[1]	
	0x76,1,0x01,	  //BW_CGOUT_L[2]	 
	0x77,1,0x06,	  //BW_CGOUT_L[3]	 
	0x78,1,0x06,	  //BW_CGOUT_L[4]	 
	0x79,1,0x06,	  //BW_CGOUT_L[5]	  
	0x7a,1,0x06,	  //BW_CGOUT_L[6]	  
	0x7b,1,0x00,	  //BW_CGOUT_L[7]	
	0x7c,1,0x00,	  //BW_CGOUT_L[8]	 
	0x7d,1,0x17,	  //BW_CGOUT_L[9]	   
	0x7e,1,0x02,	  //BW_CGOUT_L[10]
	0x7f,1,0x16,	  //BW_CGOUT_L[11]	  
	0x80,1,0x16,	  //BW_CGOUT_L[12]	 
	0x81,1,0x02,	  //BW_CGOUT_L[13] 
	0x82,1,0x0D,	  //BW_CGOUT_L[14]		
	0x83,1,0x0D,	  //BW_CGOUT_L[15]	 
	0x84,1,0x0C,	  //BW_CGOUT_L[16]		
	0x85,1,0x0C,	  //BW_CGOUT_L[17]
	0x86,1,0x0F,	  //BW_CGOUT_L[18]
	0x87,1,0x0F,	  //BW_CGOUT_L[19]
	0x88,1,0x0E,	  //BW_CGOUT_L[20]	 
	0x89,1,0x0E,	  //BW_CGOUT_L[21]	 
	0x8A,1,0x02,	  //BW_CGOUT_L[22]	 
			  //CMD_Page 4
	0xFF,3,0x98,0x81,0x04,
	
	0x6E,1,0x2B,			//VGH 15V
	0x6F,1,0x37,			// reg vcl + pumping ratio VGH=3x VGL=-3x
	0x3A,1,0xA4,			//POWER SAVING
	0x8D,1,0x1A,			//VGL -11V
	0x87,1,0xBA,			//ESD
	0xB2,1,0xD1,
	0x88,1,0x0B,
	0x38,1,0x01,	   
	0x39,1,0x00,
	0xB5,1,0x02,			//gamma bias
	0x31,1,0x25,			//source bias
	0x3B,1,0x98,			
				//CMD_Page 1
	0xFF,3,0x98,0x81,0x01,
	0x22,1,0x0A,		   //BGR, SS
	0x31,1,0x00,		   //Column inversion
	0x53,1,0x3D,		   //VCOM1
	0x55,1,0x3D,		   //VCOM2 
	0x50,1,0x95,		   //VREG1OUT 4.5V
	0x51,1,0x90,		   //VREG2OUT -4.5V
	0x60,1,0x06,		   //SDT
	0x62,1,0x20,
				   //============Gamma START=============
				   //Pos Register
	0xA0,1,0x00,
	0xA1,1,0x21,
	0xA2,1,0x35,
	0xA3,1,0x19,
	0xA4,1,0x1E,
	0xA5,1,0x33,
	0xA6,1,0x27,
	0xA7,1,0x26,
	0xA8,1,0xAF,
	0xA9,1,0x1B,
	0xAA,1,0x27,
	0xAB,1,0x8D,
	0xAC,1,0x1A,
	0xAD,1,0x1B,
	0xAE,1,0x50,
	0xAF,1,0x26,
	0xB0,1,0x2B,
	0xB1,1,0x54,
	0xB2,1,0x5E,
	0xB3,1,0x23,
				//Neg Register
	0xC0,1,0x00,
	0xC1,1,0x21,
	0xC2,1,0x35,
	0xC3,1,0x19,
	0xC4,1,0x1E,
	0xC5,1,0x33,
	0xC6,1,0x27,
	0xC7,1,0x26,
	0xC8,1,0xAF,
	0xC9,1,0x1B,
	0xCA,1,0x27,
	0xCB,1,0x8D,
	0xCC,1,0x1A,
	0xCD,1,0x1B,
	0xCE,1,0x50,
	0xCF,1,0x26,
	0xD0,1,0x2B,
	0xD1,1,0x54,
	0xD2,1,0x5E,
	0xD3,1,0x23,
	
	///0xFF,3,0x98,0x81,0x04,
	///0x2F,1,0x01,
				//============ Gamma END=========== 		
				//CMD_Page 0			
	0xFF,3,0x98,0x81,00,
	0x35,1,0x00,
	0x11,1,0x00,  
	FLAG_DELAY, FLAG_DELAY, 120,
	0x29,1,0x00,
	FLAG_DELAY, FLAG_DELAY, 20,
	FLAG_END_OF_TABLE, FLAG_END_OF_TABLE,

};

MI_PANEL_MipiDsiConfig_t stMipiDsiConfig =
{
    //HsTrail HsPrpr HsZero ClkHsPrpr ClkHsExit ClkTrail ClkZero ClkHsPost DaHsExit ContDet
    0x05,   0x03,  0x05,  0x0A,     0x0E,     0x03,    0x0B,   0x0A,     0x05,    0x00,
    //Lpx   TaGet  TaSure  TaGo
    0x10, 0x1a,  0x16,   0x32,

    //Hac,  Hpw,  Hbp,  Hfp,  Vac,  Vpw, Vbp, Vfp,  Bllp, Fps
    800,    20,    80,  60,   1280,  10,   20,   20,   0,    60,

    E_MI_PNL_MIPI_DSI_LANE_4,      // MIPnlMipiDsiLaneMode_e enLaneNum;
    E_MI_PNL_MIPI_DSI_RGB888,      // MIPnlMipiDsiFormat_e enFormat;
    E_MI_PNL_MIPI_DSI_SYNC_PULSE,  // MIPnlMipiDsiCtrlMode_e enCtrl;

    JD9365_CMD,
    sizeof(JD9365_CMD),
    1, 0x01AF, 0x01B9, 0x80D2, 7,   
    2,2,2,2,2,
};

