/*
* U3PCIE_CP0_CP7_pattern_m_PHY_p0.h- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/
//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
//******************************************************************************
//<MStar Software>
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103380), 0x34);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103381), 0x12);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00101e38), 0x00);
//--------------------------------------
//Enable U3PCIE_PHY 1G clock pass       
//--------------------------------------
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102106), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102107), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102306), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102307), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00002306), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00002307), 0x01);
//--------------------------------------
//Enable UPLL 
//--------------------------------------
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00100800), 0xc0);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00100804), 0x10);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00100805), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00100880), 0xc0);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00100884), 0x10);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00100885), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010a00), 0xc0);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010a04), 0x10);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010a05), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00100840), 0xc0);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00100844), 0x10);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00100845), 0x01);
//--------------------------------------
//Be careful of the order of utmi_ss    
//Initial UTMI_SS0                      
//--------------------------------------
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a88), 0x0f);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a89), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a82), 0x84);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a83), 0x90);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a94), 0x0b);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a95), 0x40);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a80), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a81), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a08), 0x0f);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a09), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a02), 0x84);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a03), 0x90);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a14), 0x0b);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a15), 0x40);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a00), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103a01), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103888), 0x0f);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103889), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103882), 0x84);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103883), 0x90);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103894), 0x0b);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103895), 0x40);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103880), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00103881), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010b08), 0x0f);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010b09), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010b02), 0x84);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010b03), 0x90);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010b14), 0x0b);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010b15), 0x40);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010b00), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00010b01), 0x00);
//==============================
// UTMI_SS0 wait for clock ready
//==============================
//--------------------------------------
//Wait UTMI clock ready for U3PCIE_PHY  
//--------------------------------------
//////////////////////////
//  PHY POWER ON        //
//////////////////////////
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x001021a1), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102102), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102103), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102100), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102101), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x001023a1), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102302), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102303), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102300), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102301), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x000023a1), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00002302), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00002303), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00002300), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00002301), 0x00);
//////////////////////////
// 1. SIM ONLY SETING   //
//////////////////////////
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102002), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102003), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102202), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102203), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102008), 0x81);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102009), 0x11);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102208), 0x81);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102209), 0x11);
//////////////////////////////////
// 3. enable PIPE3 interface      
//////////////////////////////////
///////////////////////////////
//wriu    0x001020c4 0x03
//wriu    0x001020c5 0x30
//wriu    0x001020c6 0xa0
//wriu    0x001020c7 0x04
//wriu    0x001020c8 0x04
//wriu    0x001020c9 0x00
////////SSC////////////
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x001020c0), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x001020c1), 0x8e);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x001020c2), 0x18);
//wriu    0x001020c3 0x00
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x001020e0), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x001020e1), 0x8e);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x001020e2), 0x18);
////////SSC////////////
//wriu    0x001020e3 0x00
//wriu    0x001020e4 0x00
//wriu    0x001020e5 0x00
//wriu    0x001020e6 0x00
//wriu    0x001020e7 0x00
//wriu    0x001020e8 0x04
//wriu    0x001020e9 0x00
///////////////////////////////
//wriu    0x001022c4 0x03
//wriu    0x001022c5 0x30
//wriu    0x001022c6 0xa0
//wriu    0x001022c7 0x04
//wriu    0x001022c8 0x04
//wriu    0x001022c9 0x00
//wriu    0x001022c0 0x75
//wriu    0x001022c1 0x93
//wriu    0x001022c2 0x18
//wriu    0x001022c3 0x00
//wriu    0x001022e0 0x75
//wriu    0x001022e1 0x93
//wriu    0x001022e2 0x18
//wriu    0x001022e3 0x00
//wriu    0x001022e4 0x00
//wriu    0x001022e5 0x00
//wriu    0x001022e6 0x00
//wriu    0x001022e7 0x00
//wriu    0x001022e8 0x04
//wriu    0x001022e9 0x00
///////////////////////////////
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102112), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102113), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102312), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102313), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102018), 0x60);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102019), 0x05);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102218), 0x60);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102219), 0x05);
/////////////////////////////////////
// 4. Release PHY PD overrade enable 
/////////////////////////////////////
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x0010213a), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x0010213b), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x0010233a), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x0010233b), 0x00);
/////////////////////////
// 5. clkgen enable      
/////////////////////////
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102020), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102021), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102022), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102023), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102220), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102221), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102222), 0x00);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00102223), 0x00);
///////////////////////
// 6 .Wait PIPE ready  
///////////////////////
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00380020), 0x02);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386817), 0x00); //hang
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x0010213a), 0x40);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x0010213b), 0x00);

//Polling if reset is completed! 
//Delay 1ms 
//Delay 1ms 
//Connect SS device !! 
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386863), 0x01);
//Delay 360ms to let link into Compliance State!! 
//SS is now in CP0!! 
//Set following command to have CP1!! 
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386862), 0x01);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386863), 0x01);
//Set following command to have CP2!! 
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386862), 0x02);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386863), 0x01);
//Set following command to have CP3!! 
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386862), 0x03);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386863), 0x01);
//Set following command to have CP4!! 
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386862), 0x04);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386863), 0x01);
//Set following command to have CP5!! 
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386862), 0x05);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386863), 0x01);
//Set following command to have CP6!! 
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386862), 0x06);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386863), 0x01);
//Set following command to have CP7!! 
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386862), 0x07);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386863), 0x01);
//Set following command to have CP8!! 
OUTREG16(GET_REG_ADDR(RIU_BASE_ADDR, 0x0010013C), 0x0040);
mdelay(100);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386862), 0x08);
OUTREG8(GET_REG8_ADDRE(RIU_BASE_ADDR, 0x00386863), 0x01);