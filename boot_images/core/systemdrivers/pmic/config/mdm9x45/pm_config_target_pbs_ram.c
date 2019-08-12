/*! \file  pm_config_target_pbs_ram.c
 *  
 *  \brief  File Contains the PMIC Set Mode Driver Implementation
 *  \details Set Mode Driver implementation is responsible for setting and getting 
 *  all mode settings such as Register values, memory values, etc.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: PMD9635-x.x-OTPx.x-MDM9x45-01292015v2 - Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2015 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/mdm9x45/pm_config_target_pbs_ram.c#1 $ 
$DateTime: 2015/09/01 00:30:35 $  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_target_information.h"
#include "pm_config_sbl.h"
#include "pm_pbs_driver.h"

/*========================== PBS RAM LUT =============================*/

// To handle multiple PBS RAM configuration for different rev of the same PMIC or for multiple PMICs,
// a double dimension array of PBS RAM data is used. The data field of the specific command (in pm_config_target_sbl_sequence.c)
// to program PBS RAM will hold the index to the PBS RAM that needs to be used. during programming.
// Example:
// 1.sid; 2.data; 3.base_address; 4.offset;  5.reg_operation; 6.rev_id_operation; 7.rev_id;
// {0, 0x00, 0x0000, 0x000, PM_SBL_PBS_RAM, EQUAL, REV_ID_2_0},  //data = 0:  Use the 1st set of PBS RAM data if PMIC Rev ID = REV_ID_2_0
// {0, 0x01, 0x0000, 0x000, PM_SBL_PBS_RAM, EQUAL, REV_ID_1_0},  //data = 1:  Use the 2nd set of PBS RAM data if PMIC Rev ID = REV_ID_1_0

pm_pbs_ram_data_type
pm_pbs_seq [ ][PBS_RAM_DATA_SIZE] =
{
   // PBS_RAM_MDM9x45.PMIC.HW.PMD9635_1p1_1_0_23
   {
      //data  offset  base_addr  sid
      { 0x28,	0x04,	0x00,	0x80},	// W#0	-	Go to BUA seq in RAM
      { 0x70,	0x04,	0x00,	0x80},	// W#1	-	Go to sleep/wake seq in RAM
      { 0x08,	0x06,	0x00,	0x80},	// W#2	-	Go to POFF seq in RAM
      { 0x98,	0x06,	0x00,	0x80},	// W#3	-	Go to S1 setting change
      { 0xEC,	0x06,	0x00,	0x80},	// W#4	-	Go to S1 OCP enable seq
      { 0x3C,	0x07,	0x00,	0x80},	// W#5	-	Go to Warm Reset seq in RAM
      { 0xCC,	0x07,	0x00,	0x80},	// W#6	-	Go to PCIe handling Sub in RAM
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#7	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#8	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#9	-	
      { 0x00,	0x18,	0x1C,	0x40},	// W#10	-	Read  INT_LATCHED_STS in BUA 
      { 0x01,	0x00,	0x01,	0x90},	// W#11	-	Check_Batt_Alarm
      { 0x5C,	0x04,	0x4A,	0x89},	// W#12	-	
      { 0x00,	0x18,	0x1C,	0x40},	// W#13	-	Read  INT_LATCHED_STS in BUA 
      { 0x01,	0x00,	0x01,	0x90},	// W#14	-	Check Batt_Alarm
      { 0x5C,	0x04,	0x4C,	0x89},	// W#15	-	
      { 0x00,	0x18,	0x1C,	0x40},	// W#16	-	Read  INT_LATCHED_STS in BUA 
      { 0x02,	0x00,	0x02,	0x90},	// W#17	-	Check UICC1_Alarm
      { 0x5C,	0x04,	0x4A,	0x89},	// W#18	-	
      { 0x00,	0x18,	0x1C,	0x40},	// W#19	-	Read  INT_LATCHED_STS in BUA 
      { 0x04,	0x00,	0x04,	0x90},	// W#20	-	Check UICC2_Alarm
      { 0x5C,	0x04,	0x4C,	0x89},	// W#21	-	
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#22	-	
      { 0xA5,	0xD0,	0x00,	0x04},	// W#23	-	Unlock the register for writing
      { 0x01,	0xDB,	0x00,	0x04},	// W#24	-	Reset LDO 
      { 0xFF,	0xFF,	0xFF,	0x8C},	// W#25	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#26	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#27	-	
      { 0x00,	0x8A,	0x08,	0x40},	// W#28	-	Read AVDD Regulator to determine if we are entering or exiting sleep.
      { 0x20,	0x01,	0x20,	0x90},	// W#29	-	If AVDD_HPM_EN (bit 5) == 1 then skip over to sleep sequence
      { 0x20,	0x05,	0x00,	0x80},	// W#30	-	Else AVDD_HPM_EN == 0 so we are waking, goto wake sequence
      { 0x00,	0x52,	0x59,	0x40},	// W#31	-	Read to see if RPM has written trim XO info to Spare2 to prevent drift issue
      { 0x00,	0x01,	0xFF,	0x90},	// W#32	-	skip if register is 0x00
      { 0x00,	0x5C,	0x50,	0x08},	// W#33	-	set XO trim to value in Spare2
      { 0x11,	0x40,	0xA2,	0x00},	// W#34	-	
      { 0x42,	0x41,	0x17,	0x01},	// W#35	-	Set S2 to 1.2 V in sleep
      { 0x0A,	0x41,	0x20,	0x01},	// W#36	-	Set S4 voltage to 1.8 V
      { 0x02,	0x40,	0x50,	0x00},	// W#37	-	Use S4 directly when in sleep to reduce battery current
      { 0x00,	0x45,	0x17,	0x01},	// W#38	-	S2 force PFM
      { 0x90,	0x01,	0x00,	0xC0},	// W#39	-	Wait 20 uS for bias current to settle after setting the last buck into forced PFM mode.
      { 0x20,	0x45,	0x40,	0x01},	// W#40	-	LDO1 in bypass
      { 0x20,	0x45,	0x41,	0x01},	// W#41	-	LDO2 in bypass
      { 0x00,	0x45,	0x48,	0x01},	// W#42	-	LDO9 in LPM
      { 0x00,	0x45,	0x23,	0x01},	// W#43	-	S5 force PFM
      { 0x00,	0x46,	0x46,	0x41},	// W#44	-	check to see if L7 is enabled (for ADC)
      { 0x00,	0x01,	0x80,	0x90},	// W#45	-	skip if L7 is off
      { 0xC4,	0x04,	0x00,	0x80},	// W#46	-	go and put L7 in bypass
      { 0x00,	0x46,	0x42,	0x01},	// W#47	-	turn off L3 if L7 is off (meaning no vote from ADC)
      { 0xCC,	0x04,	0x00,	0x80},	// W#48	-	do not set L3 at 0.6V and L7 in bypass
      { 0x20,	0x45,	0x46,	0x01},	// W#49	-	Put L7 in BYPASS during sleep.
      { 0x12,	0x41,	0x42,	0x01},	// W#50	-	Set L3 to 0.6V
      { 0x00,	0x46,	0x47,	0x41},	// W#51	-	check to see if LDO8 is enabled
      { 0x80,	0x00,	0x80,	0x90},	// W#52	-	if L8 is not enabled, skip bypass and do nothing (L8 will turn off after BBCLK is off)
      { 0x20,	0x45,	0x47,	0x01},	// W#53	-	put L8 into bypass
      { 0x20,	0x45,	0x4E,	0x01},	// W#54	-	LDO15 in bypass
      { 0x20,	0x45,	0x45,	0x01},	// W#55	-	LDO6 in bypass
      { 0x00,	0x45,	0x20,	0x01},	// W#56	-	S4 force PFM
      { 0x00,	0x46,	0x43,	0x41},	// W#57	-	Read L4 EN register
      { 0x80,	0x01,	0x80,	0x90},	// W#58	-	skip if L4 is enabled
      { 0x0C,	0x00,	0x00,	0x84},	// W#59	-	
      { 0x1E,	0x41,	0x1A,	0x01},	// W#60	-	Set S3 to 0.75V
      { 0x20,	0x45,	0x43,	0x01},	// W#61	-	LDO4 bypass
      { 0x00,	0x7A,	0x1A,	0x41},	// W#62	-	Read SPARE_1 register for S3 retention voltage
      { 0x00,	0x41,	0x1A,	0x09},	// W#63	-	Set S3 to the voltage read from SPARE_1
      { 0x00,	0x45,	0x1A,	0x01},	// W#64	-	S3 force PFM
      { 0x00,	0x08,	0x24,	0x40},	// W#65	-	Read Temp Alarm Status Register
      { 0x00,	0x00,	0x03,	0x90},	// W#66	-	If temp alarm is in state zero, then disable the temp alarm otherwise do nothing (leave enabled)
      { 0x00,	0x46,	0x24,	0x00},	// W#67	-	Disable the temp alarm (save ~8 uA)
      { 0x00,	0x45,	0x09,	0x00},	// W#68	-	Enable MBG current switching delay when transitioning from sleep to PWM (we need this delay if we turn off Iref)
      { 0x10,	0x44,	0x2C,	0x00},	// W#69	-	turn off Iref
      { 0x00,	0x8A,	0x08,	0x00},	// W#70	-	Put AVDD regualtor into LPM
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#71	-	End Sleep Sequence
      { 0x00,	0x51,	0x59,	0x40},	// W#72	-	Read value that RPM wrote into Spare1. should be trim+2
      { 0x00,	0x01,	0xFF,	0x90},	// W#73	-	If Spare1 is 0x00 then RPM did not write
      { 0x00,	0x5C,	0x50,	0x08},	// W#74	-	write the value that RPM wrote into Spare1
      { 0x91,	0x44,	0x2C,	0x00},	// W#75	-	turn on Iref
      { 0x08,	0x45,	0x09,	0x00},	// W#76	-	disable MBG current switching delay when transitioning from sleep to PWM (we need this delay if we turn off Iref)
      { 0x01,	0x46,	0x24,	0x00},	// W#77	-	
      { 0x20,	0x8A,	0x08,	0x00},	// W#78	-	Set AVDD regulator into HPM
      { 0x40,	0x45,	0x17,	0x01},	// W#79	-	S2 Auto mode
      { 0x40,	0x45,	0x1A,	0x01},	// W#80	-	S3 Auto mode
      { 0x00,	0x8F,	0x08,	0x40},	// W#81	-	Read S3 active voltage from SPARE_2 register
      { 0x00,	0x41,	0x1A,	0x09},	// W#82	-	Restore S3 active voltage based on what was read from SPARE_2 register
      { 0x40,	0x45,	0x20,	0x01},	// W#83	-	S4 Auto mode
      { 0x40,	0x45,	0x23,	0x01},	// W#84	-	S5 Auto mode
      { 0x80,	0x45,	0x48,	0x01},	// W#85	-	LDO9 in NPM
      { 0x60,	0x45,	0x43,	0x01},	// W#86	-	LDO4 in active bypass
      { 0x3F,	0x41,	0x43,	0x01},	// W#87	-	Set this to a high voltage to prevent the undershoot. No risk of damaging anything, voltage can't exceed buck voltage (1.85 V)
      { 0x60,	0x45,	0x46,	0x01},	// W#88	-	LDO7 in active bypass
      { 0x3F,	0x41,	0x46,	0x01},	// W#89	-	Set this to a high voltage to prevent the undershoot. No risk of damaging anything, voltage can't exceed buck voltage (1.85 V)
      { 0x60,	0x45,	0x47,	0x01},	// W#90	-	LDO8 in active bypass
      { 0x3F,	0x41,	0x47,	0x01},	// W#91	-	Set this to a high voltage to prevent the undershoot. No risk of damaging anything, voltage can't exceed buck voltage (1.85 V)
      { 0x60,	0x45,	0x4E,	0x01},	// W#92	-	LDO15 in active bypass
      { 0x3F,	0x41,	0x4E,	0x01},	// W#93	-	Set this to a high voltage to prevent the undershoot. No risk of damaging anything, voltage can't exceed buck voltage (1.85 V)
      { 0x60,	0x45,	0x40,	0x01},	// W#94	-	LDO1 in active bypass
      { 0x5D,	0x41,	0x40,	0x01},	// W#95	-	Set this to a high voltage to prevent the undershoot. No risk of damaging anything, voltage can't exceed S2 (1.2 V)
      { 0x60,	0x45,	0x41,	0x01},	// W#96	-	LDO2 in active bypass
      { 0x5D,	0x41,	0x41,	0x01},	// W#97	-	Set this to a high voltage to prevent the undershoot. No risk of damaging anything, voltage can't exceed S2 (1.2 V)
      { 0x60,	0x45,	0x45,	0x01},	// W#98	-	LDO6 in active bypass
      { 0x3F,	0x41,	0x45,	0x01},	// W#99	-	Set this to a high voltage to prevent the undershoot. No risk of damaging anything, voltage can't exceed S4 (1.85 V)
      { 0x04,	0x00,	0x01,	0xC0},	// W#100	-	Wait 100 us
      { 0x80,	0x45,	0x43,	0x01},	// W#101	-	LDO4 in NPM
      { 0x14,	0x41,	0x43,	0x01},	// W#102	-	LDO4 = 1 V
      { 0x00,	0x46,	0x46,	0x41},	// W#103	-	Read LDO7 EN
      { 0x80,	0x01,	0x80,	0x90},	// W#104	-	skip if LDO7 is on
      { 0xB4,	0x05,	0x00,	0x80},	// W#105	-	
      { 0x00,	0x46,	0x42,	0x01},	// W#106	-	turn off L3
      { 0x10,	0x00,	0x01,	0xC0},	// W#107	-	wait 500us
      { 0x32,	0x41,	0x42,	0x01},	// W#108	-	set L3 to 1V
      { 0x80,	0x46,	0x42,	0x01},	// W#109	-	turn on L3
      { 0x80,	0x45,	0x46,	0x01},	// W#110	-	LDO7 in NPM
      { 0x0C,	0x41,	0x46,	0x01},	// W#111	-	LDO7 = 1.8 V
      { 0x80,	0x45,	0x47,	0x01},	// W#112	-	LDO8 in NPM
      { 0x0C,	0x41,	0x47,	0x01},	// W#113	-	LDO8 = 1.8 V
      { 0x80,	0x45,	0x4E,	0x01},	// W#114	-	LDO15 in NPM
      { 0x0C,	0x41,	0x4E,	0x01},	// W#115	-	LDO15 = 1.8 V
      { 0x80,	0x45,	0x40,	0x01},	// W#116	-	LDO1 in NPM
      { 0x26,	0x41,	0x40,	0x01},	// W#117	-	LDO1 = 1.225 V
      { 0x80,	0x45,	0x41,	0x01},	// W#118	-	LDO2 in NPM
      { 0x42,	0x41,	0x41,	0x01},	// W#119	-	LDO2 = 1.2 V
      { 0x80,	0x45,	0x45,	0x01},	// W#120	-	LDO6 in NPM
      { 0x0C,	0x41,	0x45,	0x01},	// W#121	-	LDO6 = 1.8 V
      { 0x0C,	0x41,	0x20,	0x01},	// W#122	-	Set S4 voltage to 1.85 V
      { 0x46,	0x41,	0x17,	0x01},	// W#123	-	Set S2 voltage to 1.25 V
      { 0x01,	0x40,	0x50,	0x00},	// W#124	-	Switch to VREG_XO
      { 0x51,	0x40,	0xA2,	0x00},	// W#125	-	Put MPP3 into analog output mode
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#126	-	End Sleep Sequence
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#127	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#128	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#129	-	
      { 0xA5,	0xD0,	0x5A,	0x00},	// W#130	-	
      { 0x00,	0x48,	0x5A,	0x00},	// W#131	-	Disable SMPL
      { 0x04,	0x90,	0x08,	0x00},	// W#132	-	Set PON OUT to LOW
      { 0x84,	0x06,	0x46,	0x89},	// W#133	-	Explicitly turn OFF LDO7 so that probability of accidental fuse blow is further reduced
      { 0x84,	0x06,	0x5A,	0x88},	// W#134	-	Turn off SLEEP_CLK
      { 0x84,	0x06,	0x52,	0x88},	// W#135	-	Turn off LN_BBCLK
      { 0x84,	0x06,	0x51,	0x88},	// W#136	-	Turn off BBCLK1
      { 0x84,	0x06,	0x49,	0x89},	// W#137	-	Turn off L10 (high-voltage USB)
      { 0x84,	0x06,	0x4D,	0x89},	// W#138	-	Turn off L14 (SDC)
      { 0x84,	0x06,	0x0A,	0x88},	// W#139	-	Turn off VREF_LPDDR
      { 0x84,	0x06,	0x41,	0x89},	// W#140	-	Turn off L2 (DDR)
      { 0x84,	0x06,	0xC5,	0x88},	// W#141	-	Turn off GPIO6 (BT_EN)
      { 0x84,	0x06,	0x47,	0x89},	// W#142	-	Turn off L8 (BBCLK, USB, PCIe)
      { 0x84,	0x06,	0xC2,	0x88},	// W#143	-	Turn off GPIO3 (WLAN_PWR_EN)
      { 0x84,	0x06,	0x50,	0x89},	// W#144	-	Turn off VREG_XO
      { 0x84,	0x06,	0x45,	0x89},	// W#145	-	Turn off L6 (PX, NAND)
      { 0x84,	0x06,	0x20,	0x89},	// W#146	-	Turn off S4
      { 0x84,	0x06,	0xA2,	0x88},	// W#147	-	Turn off MPP3
      { 0x80,	0x48,	0x43,	0x01},	// W#148	-	Enable pulldown on L4 in POFF to prevent slow decay
      { 0x84,	0x06,	0x43,	0x89},	// W#149	-	Turn off L4 (USBSS)
      { 0x80,	0x48,	0x4F,	0x01},	// W#150	-	Enable pulldown on L16 in POFF to prevent slow decay
      { 0x84,	0x06,	0x4F,	0x89},	// W#151	-	Turn off L16 (USBSS)
      { 0x84,	0x06,	0x23,	0x89},	// W#152	-	Turn off S5
      { 0x84,	0x06,	0x48,	0x89},	// W#153	-	Turn off L9
      { 0x84,	0x06,	0x42,	0x89},	// W#154	-	Turn off L3
      { 0x84,	0x06,	0x17,	0x89},	// W#155	-	Turn off S2 (DDR, BBRX, USB)
      { 0x84,	0x06,	0x1A,	0x89},	// W#156	-	Turn off S3
      { 0x40,	0x91,	0x08,	0x00},	// W#157	-	ACK the PON module
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#158	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#159	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#160	-	
      { 0x00,	0x46,	0x00,	0x04},	// W#161	-	Write 0x00 to 0x46 (enable register in the peripheral)
      { 0x21,	0x00,	0x01,	0xC0},	// W#162	-	Wait 33 32kHz clock cycles. Increased for PM8019 2.0.
      { 0xFF,	0xFF,	0xFF,	0x8C},	// W#163	-	Go back to next line of the calling function
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#164	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#165	-	
      { 0x00,	0x41,	0x14,	0x41},	// W#166	-	Read VOLTAGE_CTL2
      { 0x22,	0x03,	0xFF,	0x90},	// W#167	-	If S1 is outputting < 800 mV, skip the go-to high-voltage settings and do low-voltage settings
      { 0xC4,	0x06,	0x00,	0x80},	// W#168	-	
      { 0x0B,	0x52,	0x14,	0x01},	// W#169	-	Buck voltage regulation loop error amplifier gain (Gm) decreased to 38.3 uA/V
      { 0x04,	0x53,	0x14,	0x01},	// W#170	-	Buck voltage regulation loop zero compensation resistance (Rz) increased to 125kOhm
      { 0x17,	0x5A,	0x14,	0x01},	// W#171	-	pulse skip reset voltage (PS_VRST) decreased to 250mV
      { 0x1E,	0x5B,	0x14,	0x01},	// W#172	-	changes PWM to PFM threshold based on CCM-DCM boundary (inzero_comp) in addition to already used ps_comp
      { 0x83,	0x4B,	0x15,	0x01},	// W#173	-	increases PFM current limit (Iplimit) to 420mA
      { 0xAC,	0x5D,	0x14,	0x01},	// W#174	-	increases the PFM_COUNT by 4 (set it to 13)
      { 0x16,	0x44,	0x14,	0x01},	// W#175	-	changes PFM IPLIM delay from 150ns to 300ns
      { 0xE0,	0x06,	0x00,	0x80},	// W#176	-	
      { 0x04,	0x52,	0x14,	0x01},	// W#177	-	revert it back to what it is set in SBL
      { 0x02,	0x53,	0x14,	0x01},	// W#178	-	revert it back to what it is set in SBL
      { 0x4F,	0x5A,	0x14,	0x01},	// W#179	-	revert it back to what it is set in SBL
      { 0x0E,	0x5B,	0x14,	0x01},	// W#180	-	revert it back to what it is set in SBL
      { 0x84,	0x4B,	0x15,	0x01},	// W#181	-	revert it back to what it is set in SBL
      { 0xA8,	0x5D,	0x14,	0x01},	// W#182	-	revert it back to what it is set in SBL
      { 0x15,	0x44,	0x14,	0x01},	// W#183	-	revert it back to what it is set in SBL
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#184	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#185	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#186	-	
      { 0xA5,	0xD0,	0x14,	0x01},	// W#187	-	Workaround seq for OCP issue in MSS rail
      { 0x00,	0x6C,	0x14,	0x01},	// W#188	-	disable S1 OCP
      { 0x00,	0x45,	0x14,	0x41},	// W#189	-	read S1 mode so we can restore it upon exit
      { 0x00,	0x7A,	0x14,	0x09},	// W#190	-	save the mode info into the spare register
      { 0x80,	0x45,	0x14,	0x01},	// W#191	-	force S1 to PWM
      { 0x80,	0x46,	0x14,	0x01},	// W#192	-	enable S1
      { 0x04,	0x00,	0x01,	0xC0},	// W#193	-	wait ~125usec
      { 0x00,	0x08,	0x14,	0x41},	// W#194	-	read S1 status
      { 0x80,	0x00,	0x80,	0x90},	// W#195	-	check if S1 VREG_OK is high
      { 0x20,	0x07,	0x00,	0x80},	// W#196	-	S1 VREG_OK is high, so continue
      { 0x00,	0x46,	0x14,	0x41},	// W#197	-	check enable setting. if disabled (by MDM) then exit the loop
      { 0x80,	0x00,	0x80,	0x90},	// W#198	-	there is an error, so exit the loop
      { 0xEC,	0xFF,	0x00,	0x84},	// W#199	-	if S1 setting still enabled, go back to the loop
      { 0xA5,	0xD0,	0x14,	0x01},	// W#200	-	Unlock S1
      { 0x80,	0x6C,	0x14,	0x01},	// W#201	-	enable S1 OCP
      { 0x00,	0x7A,	0x14,	0x41},	// W#202	-	read mode data that was stored in the beginning
      { 0x00,	0x45,	0x14,	0x09},	// W#203	-	restore the mode to what it was in the beggining
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#204	-	end of seq
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#205	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#206	-	
      { 0xD0,	0x52,	0x41,	0x01},	// W#207	-	Disable clamp control for LDO2 (N600) to prevent output voltage from floating up
      { 0xD0,	0x52,	0x42,	0x01},	// W#208	-	Disable clamp control for LDO3 (N1200) to prevent output voltage from floating up
      { 0xD0,	0x52,	0x48,	0x01},	// W#209	-	Disable clamp control for LDO9 (N1200) to prevent output voltage from floating up
      { 0x00,	0x40,	0x1A,	0x01},	// W#210	-	Set S3 (MX parent) range
      { 0x3F,	0x41,	0x1A,	0x01},	// W#211	-	Set S3 (Mx parent) to 1.1625 V
      { 0x01,	0x40,	0x48,	0x01},	// W#212	-	Set L9 (MX) range
      { 0x3B,	0x41,	0x48,	0x01},	// W#213	-	Set L9 (MX) to 1.1125
      { 0x80,	0x45,	0x48,	0x01},	// W#214	-	Set L9 to NPM
      { 0x00,	0x40,	0x23,	0x01},	// W#215	-	Set S5 (CX) range
      { 0x33,	0x41,	0x23,	0x01},	// W#216	-	Set S5 (CX) to 1.0125 V
      { 0x14,	0x41,	0x43,	0x01},	// W#217	-	change L4 voltage to 1V
      { 0x00,	0x57,	0x08,	0x00},	// W#218	-	CR-16114 Disable watchdog timer when doing a warm reset so that we can gather a RAM dump without worrying about petting the timer
      { 0x00,	0x08,	0x01,	0x40},	// W#219	-	Read OPTION2 to see if it is Mav platform
      { 0x0C,	0x00,	0x0C,	0x90},	// W#220	-	skip if OPTION PIN2 is not VDD
      { 0x8C,	0x07,	0x08,	0x88},	// W#221	-	config for the PCIe PERST handling
      { 0x54,	0x03,	0x01,	0xC0},	// W#222	-	was changed from 10ms to 20ms on 9/8, and to 26ms on 12/15
      { 0x40,	0x91,	0x08,	0x00},	// W#223	-	Write Sequence Complete ACK to PON
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#224	-	Finished Warm Reset Sequence
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#225	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#226	-	
      { 0x00,	0x63,	0x08,	0x00},	// W#227	-	disable SW_RESET to be able to cinfig it
      { 0x0A,	0x00,	0x01,	0xC0},	// W#228	-	delay needed after disabling SW_RESET to be able to config it
      { 0x01,	0x62,	0x08,	0x00},	// W#229	-	cinfigure SW_RESET to generate a warm reset
      { 0x80,	0x63,	0x08,	0x00},	// W#230	-	enable SW_RESET
      { 0x80,	0x46,	0xC3,	0x00},	// W#231	-	enable GPIO4
      { 0x0C,	0x40,	0xC3,	0x00},	// W#232	-	set it to digital input and route it to DTEST3
      { 0x03,	0x41,	0xC3,	0x00},	// W#233	-	set the input to L6 (1.8V)
      { 0x04,	0x43,	0xC3,	0x00},	// W#234	-	route DTEST3 to GPIO4
      { 0x05,	0x42,	0xC3,	0x00},	// W#235	-	disable 10uA PD (no pull)
      { 0x40,	0x40,	0x7F,	0x00},	// W#236	-	enable falling edge trigger
      { 0x18,	0x88,	0x70,	0x00},	// W#237	-	start address for the seq (LSB)
      { 0x04,	0x89,	0x70,	0x00},	// W#238	-	start address for the seq (MSB)
      { 0x80,	0x46,	0x7F,	0x00},	// W#239	-	enable the trigger
      { 0xFF,	0xFF,	0xFF,	0x8C},	// W#240	-	Go back to next line of the calling function
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#241	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#242	-	
      { 0xA5,	0x64,	0x08,	0x00},	// W#243	-	initiates SW reset when a DTEST3 toggling happens
      { 0xFF,	0xFF,	0xFF,	0xFC},	// W#244	-	end of seq
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#245	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#246	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#247	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#248	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#249	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#250	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#251	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#252	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#253	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#254	-	
      { 0x00,	0x17,	0xFF,	0xF8},	// W#255	-	
   },
};
