/*! \file  pm_config_target_pbs_ram.c
 *  
 *  \brief  File Contains the PMIC Set Mode Driver Implementation
 *  \details Set Mode Driver implementation is responsible for setting and getting 
 *  all mode settings such as Register values, memory values, etc.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: PM8909-x.x-OTPx.x-MSM8909-02242015_ver0A - Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2015 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/msm8909/pm8909/pm_config_target_pbs_ram.c#1 $ 
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
   // PBS_RAM_MSM8909.PMIC.HW.PM8909_1p0_1_0_5
   {
      //data  offset  base_addr  sid
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#0	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#1	-	
      { 0xDC,	0x04,	0x00,	0x80},	// W#2	-	POFF PBS-RAM Trigger
      { 0x24,	0x04,	0x00,	0x80},	// W#3	-	BUA PBS-RAM Trigger
      { 0x64,	0x04,	0x00,	0x80},	// W#4	-	Sleep_Wake PBS-RAM trigger
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#5	-	
      { 0xBC,	0x04,	0x00,	0x80},	// W#6	-	WARM_RESET PBS-RAM Trigger
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#7	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#8	-	
      { 0x00,	0x18,	0x1C,	0x40},	// W#9	-	Read BUA_4UICC_INT_LATCHED_STS Register to check for BATT_ALARM_LATCHED_STS
      { 0x01,	0x01,	0x01,	0x90},	// W#10	-	Check within value of Data (in Read-Buffer): Bit-0 for BATT_ALARM_LATCHED_STS. Skip Next line if Value is EQUAL to 0x01 (Mask: 0x01)
      { 0x38,	0x04,	0x00,	0x80},	// W#11	-	Read Data != 0x01 => NO BATT_ALARM_LATCHED. Jump to the code that will check for each UIM Removal Status individually
      { 0x50,	0x04,	0x4D,	0x89},	// W#12	-	BATT_GONE Alarm Detected. Disable LDO14 - UIM1
      { 0x50,	0x04,	0x4E,	0x89},	// W#13	-	BATT_GONE Alarm Detected. Disable LDO15 - UIM2
      { 0x00,	0x09,	0x1C,	0x40},	// W#14	-	Read BUA_4UICC_STATUS2 Register to check for UICCx_ALARM_DETECTED
      { 0x01,	0x00,	0x01,	0x90},	// W#15	-	Check within value of Data (in Read-Buffer): Bit-0 for UICC1_ALARM. Skip Next line if Value is NOT EQUAL to 0x01 (Mask: 0x01)
      { 0x50,	0x04,	0x4D,	0x89},	// W#16	-	UICC1_ALARM Detected. Disable UIM1 LDO14
      { 0x02,	0x00,	0x02,	0x90},	// W#17	-	Check within value of Data (in Read-Buffer): Bit-1 for UICC2_ALARM. Skip Next line if Value is NOT EQUAL to 0x02 (Mask: 0x02)
      { 0x50,	0x04,	0x4E,	0x89},	// W#18	-	UICC2_ALARM Detected. Disable UIM2 LDO15
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#19	-	EOS (End of BUA_RAM_SEQ)
      { 0xA5,	0xD0,	0x00,	0x04},	// W#20	-	Unlock SEC_ACCESS
      { 0x01,	0xDB,	0x00,	0x04},	// W#21	-	Perform LOCAL_SOFT_RESET on LDO (to reset all other voltage settings in addition to disabling it)
      { 0x00,	0xFF,	0xFF,	0x8C},	// W#22	-	Return to the calling line
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#23	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#24	-	
      { 0x00,	0x40,	0xA0,	0x40},	// W#25	-	Read MPP1 MODE_CTL register (if the value is 0x51 => analog output mode => Active state ; if the value is 0x11 => digital output mode => Sleep state)
      { 0x51,	0x01,	0xFF,	0x90},	// W#26	-	IF MPP1.MODE_CTL = 0x51 => System is Awake , SKIP Next Line to ENTER_SLEEP
      { 0x94,	0x04,	0x00,	0x80},	// W#27	-	Goto: BEGIN_RAM_WAKE_SEQ
      { 0x01,	0x46,	0x51,	0x00},	// W#28	-	Configure BBCLK1 to (only) follow BBCLK1_EN (CXO_EN) Pin-Control
      { 0x00,	0x45,	0x14,	0x01},	// W#29	-	Force PFM Mode on S1
      { 0x00,	0x45,	0x17,	0x01},	// W#30	-	Force PFM Mode on S2
      { 0x02,	0x40,	0x50,	0x00},	// W#31	-	CR-0000163990: Force VMUX to LDO5 (SW-Workaround)
      { 0x00,	0x52,	0x59,	0x40},	// W#32	-	Read CLK_DIST_SPARE2 & store value in Local Buffer
      { 0x00,	0x5C,	0x50,	0x08},	// W#33	-	Write (Copy) the local-buffer (SPARE2) value to XO_ADJ_FINE Register
      { 0x11,	0x40,	0xA0,	0x00},	// W#34	-	Change MPP1 Mode from 1.25V Analog Output to Digital Output Mode (for low power consumption) [DIG_VIN already configured for 1.2V in SBL]
      { 0x00,	0x8A,	0x08,	0x00},	// W#35	-	Put AVDD Regulator into LPM
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#36	-	EOS (End on RAM_SLEEP_SEQ)
      { 0x00,	0x40,	0x50,	0x00},	// W#37	-	CR-0000163990: Force VMUX back to Auto (SW-Workaround)
      { 0x81,	0x46,	0x51,	0x00},	// W#38	-	Configure BBCLK1 to also get SW-Enabled (it has already been enabled by BBCLK1_EN (CXO_EN) Pin that got asserted-high before starting Wake-Up)
      { 0x20,	0x8A,	0x08,	0x00},	// W#39	-	Put AVDD Regulator back to HPM & Enable Auto-selection to Master BandGap
      { 0x00,	0x51,	0x59,	0x40},	// W#40	-	Read CLK_DIST_SPARE1 & store value in Local Buffer
      { 0x00,	0x5C,	0x50,	0x08},	// W#41	-	Write (Copy) the local-buffer (SPARE1) value to XO_ADJ_FINE Register
      { 0x51,	0x40,	0xA0,	0x00},	// W#42	-	Revert MPP1 Mode from 1.20V Digital Output to 1.25V Analog Output Mode [DIG_VIN already configured for 1.2V in SBL]
      { 0x80,	0x45,	0x14,	0x01},	// W#43	-	Force PWM Mode on S1
      { 0x80,	0x45,	0x17,	0x01},	// W#44	-	Force PWM Mode on S2
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#45	-	EOS (End of RAM_WAKE_SEQ)
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#46	-	
      { 0x20,	0x8A,	0x08,	0x00},	// W#47	-	Put AVDD Regulator back to HPM & Enable Auto-selection to Master BandGap
      { 0x03,	0x42,	0x09,	0x00},	// W#48	-	Selecting ADC source as LDO7 since during warm reset ADC source changed to AVDD
      { 0x00,	0x40,	0x50,	0x00},	// W#49	-	Restore XO VMUX to Auto - so that it chooses VREG_XO that runs on AVDD
      { 0x54,	0x03,	0x01,	0xC0},	// W#50	-	Wait for 26ms (= 852 cycles of 32kHz clock)
      { 0x40,	0x91,	0x08,	0x00},	// W#51	-	Write Sequence Complete ACK to PON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#52	-	EOS (Finish Warm-Reset-Sequence)
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#53	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#54	-	
      { 0x29,	0xB2,	0xF1,	0x01},	// W#55	-	CDC_A_SPKR_DRV_CTL: Disable SPKR Drive from Tombak-Analog Codec Module
      { 0x00,	0x48,	0x5A,	0x00},	// W#56	-	Disable SMPL
      { 0x3C,	0x05,	0x4A,	0x89},	// W#57	-	[16] Disable LDO11
      { 0x3C,	0x05,	0x4B,	0x89},	// W#58	-	[15] Disable LDO12
      { 0x3C,	0x05,	0x47,	0x89},	// W#59	-	[14] Disable LDO8
      { 0x3C,	0x05,	0x4C,	0x89},	// W#60	-	[13] Disable LDO13
      { 0x3C,	0x05,	0x0A,	0x88},	// W#61	-	[12] Disable VREF_LPDDR
      { 0x3C,	0x05,	0x41,	0x89},	// W#62	-	[11] Disable LDO2
      { 0x80,	0x48,	0x45,	0x01},	// W#63	-	Enable Pull-Down on LDO6 before disabling it, so that VREG_L6 falls quickly before LDO5 disables. (MSM QFPROM protection)
      { 0x3C,	0x05,	0x45,	0x89},	// W#64	-	[10] Disable LDO6 (earlier, to ensure safe QFPROM shutdown before disabling MSM PX, CX, MX Rails)
      { 0x3C,	0x05,	0x44,	0x89},	// W#65	-	[7] Disable LDO5 (This will also pull BBCLK1_EN Pin Low [unless it is already low, during PMIC Sleep], thus actual BB_CLK1 clock & LDO7 o/p will respond to their SW-Enable states)
      { 0x3C,	0x05,	0x51,	0x88},	// W#66	-	[8] Remove SW-Enable from BB_CLK1 Peripheral (Actual clock disable will happen here, i.e. after BBCLK1_EN Pin is Low AND SW-Control is also disabled [internal OR-gate for enabling])
      { 0x3C,	0x05,	0x46,	0x89},	// W#67	-	[9] Remove SW-Enable from LDO7 Peripheral. Actual LDO7 o/p will go down here, i.e. after this setting AND BB_CLK1 clock are disabled. Unless in Sleep, BB_CLK1 will get disabled after being SW-Disabled AND BBCLK1_EN Pin going low
      { 0x3C,	0x05,	0x5A,	0x88},	// W#68	-	[6] Disable Sleep Clock (L5 Shutdown already gates-off the Clock outside, this command disables the Clock-core)
      { 0x3C,	0x05,	0x50,	0x88},	// W#69	-	Disable XO (19.2MHz Clock) - Shuts down the XO-Core
      { 0x3C,	0x05,	0x52,	0x89},	// W#70	-	Disable LDO_XO - Shuts down the supply to the Core
      { 0x3C,	0x05,	0xA0,	0x88},	// W#71	-	[5] Disable MPP1
      { 0x3C,	0x05,	0x14,	0x89},	// W#72	-	[4] Disable S1
      { 0x3C,	0x05,	0x42,	0x89},	// W#73	-	[3] Disable LDO3
      { 0x3C,	0x05,	0x17,	0x89},	// W#74	-	[2] Disable S2
      { 0x3C,	0x05,	0x40,	0x88},	// W#75	-	[1] Disable BMS (Can be left enabled & thus automatically killed after POFF, for slightly more BMS-On-Time & marginally improved SoC calculation - KKGK)
      { 0x40,	0x91,	0x08,	0x00},	// W#76	-	Write Sequence Complete ACK to PON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#77	-	EOS (Finish POFF-Sequence)
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#78	-	
      { 0x00,	0x46,	0x00,	0x04},	// W#79	-	Write Data: 0x00 to Addr-Offset: 0x46 (EN_CTL register in the peripheral)
      { 0x20,	0x00,	0x01,	0xC0},	// W#80	-	Wait 1ms (= 32 cycles of 32kHz clock)
      { 0x00,	0xFF,	0xFF,	0x8C},	// W#81	-	RETURN: Go back to next line of the calling function
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#82	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#83	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#84	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#85	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#86	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#87	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#88	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#89	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#90	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#91	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#92	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#93	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#94	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#95	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#96	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#97	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#98	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#99	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#100	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#101	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#102	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#103	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#104	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#105	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#106	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#107	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#108	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#109	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#110	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#111	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#112	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#113	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#114	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#115	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#116	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#117	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#118	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#119	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#120	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#121	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#122	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#123	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#124	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#125	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#126	-	
      { 0x00,	0x05,	0xFF,	0xF8},	// W#127	-	
   },
};
