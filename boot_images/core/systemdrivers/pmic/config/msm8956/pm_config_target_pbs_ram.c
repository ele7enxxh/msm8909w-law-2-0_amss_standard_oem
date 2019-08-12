/*! \file  pm_config_target_pbs_ram.c
 *  
 *  \brief  File Contains the PMIC Set Mode Driver Implementation
 *  \details Set Mode Driver implementation is responsible for setting and getting 
 *  all mode settings such as Register values, memory values, etc.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: PM8916-x.x-OTPx.x-MSM893x-10102014v1 - Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2014 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/msm8956/pm_config_target_pbs_ram.c#1 $ 
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
   // PBS_RAM_MSM893x.PMIC.HW.PM8916_2p0_1_0_1
   {
      //data  offset  base_addr  sid
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#0	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#1	-	
      { 0x24,	0x05,	0x00,	0x80},	// W#2	-	POFF PBS-RAM Trigger
      { 0x24,	0x04,	0x00,	0x80},	// W#3	-	BUA PBS-RAM Trigger
      { 0x94,	0x04,	0x00,	0x80},	// W#4	-	Sleep_Wake PBS-RAM trigger
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#5	-	
      { 0xFC,	0x04,	0x00,	0x80},	// W#6	-	WARM_RESET PBS-RAM Trigger
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#7	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#8	-	
      { 0x00,	0x18,	0x1C,	0x40},	// W#9	-	Read BUA_4UICC_INT_LATCHED_STS in BUA
      { 0x01,	0x01,	0x01,	0x90},	// W#10	-	Check within value of Data (in Read-Buffer): Bit-0 for BATT_ALARM. Skip Next line if Value is EQUAL to 0x01
      { 0x64,	0x04,	0x00,	0x80},	// W#11	-	Read Data != 0x01 => NO BATT_GONE Alarm. Jump to the code that will check for each UIM Removal Status individually
      { 0x01,	0x45,	0xC0,	0x00},	// W#12	-	GPIO1 is first re-configured from its original Open-Drain-PMOS Mode to (new) CMOS-Mode
      { 0x21,	0x40,	0xC0,	0x00},	// W#13	-	GPIO1 is driven High, so that MSM knows about Battery Removal & stops communicating with the UIMs
      { 0x0A,	0x00,	0x01,	0xC0},	// W#14	-	Sufficient time is waited for MSM to de-activate all the 3 UIMs (Min 290us needed ==> 10 cycles of 32kHz clock = ~312us)
      { 0x00,	0x15,	0x1C,	0x40},	// W#15	-	Read BUA_4UICC_INT_EN_SET Register for those LDOs that actually have UICC installed on them
      { 0x02,	0x00,	0x02,	0x90},	// W#16	-	Check within value of Data (in BUA_4UICC_INT_EN_SET Read-Buffer): Bit-1 for UICC1_INT_ENABLED.. Skip Next line if Value is NOT EQUAL to 0x02
      { 0x80,	0x04,	0x4D,	0x89},	// W#17	-	BATT_GONE Alarm Detected. Disable LDO14 - UIM1
      { 0x04,	0x00,	0x04,	0x90},	// W#18	-	Check within value of Data (in BUA_4UICC_INT_EN_SET Read-Buffer): Bit-2 for UICC2_INT_ENABLED. Skip Next line if Value is NOT EQUAL to 0x04
      { 0x80,	0x04,	0x4E,	0x89},	// W#19	-	BATT_GONE Alarm Detected. Disable LDO15 - UIM2
      { 0x08,	0x00,	0x08,	0x90},	// W#20	-	Check within value of Data (in BUA_4UICC_INT_EN_SET Read-Buffer): Bit-3 for UICC3_INT_ENABLED. Skip Next line if Value is NOT EQUAL to 0x08
      { 0x80,	0x04,	0x4F,	0x89},	// W#21	-	BATT_GONE Alarm Detected. Disable LDO16 - UIM3
      { 0x24,	0x40,	0xC0,	0x00},	// W#22	-	GPIO1 is reverted to Digital Input-Output with Special Function 1 (BUA)  - only in Battery Removal cases after disabling UIM LDOs.
      { 0x22,	0x45,	0xC0,	0x00},	// W#23	-	GPIO1 is reverted as Open-Drain-PMOS Mode - only in Battery Removal cases after disabling UIM LDOs.
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#24	-	EOS (End of Battery-Removal BUA_RAM_SEQ)
      { 0x02,	0x00,	0x02,	0x90},	// W#25	-	Check within value of Data (in BUA_4UICC_INT_LATCHED_STS Read-Buffer): Bit-1 for UICC1_ALARM. Skip Next line if Value is NOT EQUAL to 0x02
      { 0x80,	0x04,	0x4D,	0x89},	// W#26	-	UICC1_ALARM Detected. Disable UIM1 LDO14
      { 0x04,	0x00,	0x04,	0x90},	// W#27	-	Check within value of Data (in BUA_4UICC_INT_LATCHED_STS Read-Buffer): Bit-2 for UICC2_ALARM. Skip Next line if Value is NOT EQUAL to 0x04
      { 0x80,	0x04,	0x4E,	0x89},	// W#28	-	UICC2_ALARM Detected. Disable UIM2 LDO15
      { 0x08,	0x00,	0x08,	0x90},	// W#29	-	Check within value of Data (in BUA_4UICC_INT_LATCHED_STS Read-Buffer): Bit-3 for UICC3_ALARM. Skip Next line if Value is NOT EQUAL to 0x08
      { 0x80,	0x04,	0x4F,	0x89},	// W#30	-	UICC3_ALARM Detected. Disable UIM3 LDO16
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#31	-	EOS (End of UICC-Removal BUA_RAM_SEQ)
      { 0xA5,	0xD0,	0x00,	0x04},	// W#32	-	Unlock SEC_ACCESS
      { 0x01,	0xDB,	0x00,	0x04},	// W#33	-	Perform LOCAL_SOFT_RESET on LDO (to reset all other voltage settings in addition to disabling it)
      { 0x00,	0xFF,	0xFF,	0x8C},	// W#34	-	Return to the calling line
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#35	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#36	-	
      { 0x00,	0x8A,	0x08,	0x40},	// W#37	-	Read PON:AVDD_VPH Register to determine if we are Entering-into or Exiting-from Sleep
      { 0x20,	0x01,	0x20,	0x90},	// W#38	-	IF AVDD_VPH:HPM_EN (Bit 5) == 1 => System is Awake => SKIP Next Line to ENTER_SLEEP
      { 0xCC,	0x04,	0x00,	0x80},	// W#39	-	Goto: BEGIN_RAM_WAKE_SEQ
      { 0x01,	0x46,	0x51,	0x00},	// W#40	-	Configure BBCLK1 to (only) follow BBCLK1_EN (CXO_EN) Pin-Control
      { 0x00,	0x45,	0x14,	0x01},	// W#41	-	Force PFM Mode on S1 (CX/MSS on MSM8916; MSS on MSM893x)
      { 0x00,	0x45,	0x17,	0x01},	// W#42	-	Force PFM Mode on S2 (APC on MSM8916; CX on MSM893x)
      { 0x00,	0x45,	0x1A,	0x01},	// W#43	-	Force PFM Mode on S3 (Parent to MX, on both MSM8916 & MSM893x)
      { 0x00,	0x45,	0x1D,	0x01},	// W#44	-	Force PFM Mode on S4
      { 0x02,	0x40,	0x50,	0x00},	// W#45	-	CR-0000163990: Force VMUX to LDO5 (SW-Workaround)
      { 0x00,	0x52,	0x59,	0x40},	// W#46	-	Read CLK_DIST_SPARE2 & store value in Local Buffer
      { 0x00,	0x5C,	0x50,	0x08},	// W#47	-	Write (Copy) the local-buffer (SPARE2) value to XO_ADJ_FINE Register
      { 0x11,	0x40,	0xA0,	0x00},	// W#48	-	Change MPP1 Mode from 1.25V Analog Output to Digital Output Mode (for low power consumption) [DIG_VIN already configured for 1.2V in SBL]
      { 0x00,	0x8A,	0x08,	0x00},	// W#49	-	Put AVDD Regulator into LPM
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#50	-	EOS (End on RAM_SLEEP_SEQ)
      { 0x00,	0x40,	0x50,	0x00},	// W#51	-	CR-0000163990: Force VMUX back to Auto (SW-Workaround)
      { 0x81,	0x46,	0x51,	0x00},	// W#52	-	Configure BBCLK1 to also get SW-Enabled (it has already been enabled by BBCLK1_EN (CXO_EN) Pin that got asserted-high before starting Wake-Up)
      { 0x20,	0x8A,	0x08,	0x00},	// W#53	-	Put AVDD Regulator back to HPM & Enable Auto-selection to Master BandGap
      { 0x00,	0x51,	0x59,	0x40},	// W#54	-	Read CLK_DIST_SPARE1 & store value in Local Buffer
      { 0x00,	0x5C,	0x50,	0x08},	// W#55	-	Write (Copy) the local-buffer (SPARE1) value to XO_ADJ_FINE Register
      { 0x51,	0x40,	0xA0,	0x00},	// W#56	-	Revert MPP1 Mode from 1.20V Digital Output to 1.25V Analog Output Mode [DIG_VIN already configured for 1.2V in SBL]
      { 0x80,	0x45,	0x14,	0x01},	// W#57	-	Force PWM Mode on S1 (CX/MSS on MSM8916; MSS on MSM893x)
      { 0x80,	0x45,	0x17,	0x01},	// W#58	-	Force PWM Mode on S2 (APC on MSM8916; CX on MSM893x)
      { 0x80,	0x45,	0x1A,	0x01},	// W#59	-	Force PWM Mode on S3 (Parent to MX, on both MSM8916 & MSM893x)
      { 0x80,	0x45,	0x1D,	0x01},	// W#60	-	Force PWM Mode on S4
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#61	-	EOS (End of RAM_WAKE_SEQ)
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#62	-	
      { 0x20,	0x8A,	0x08,	0x00},	// W#63	-	Put AVDD Regulator back to HPM & Enable Auto-selection to Master BandGap
      { 0x00,	0x08,	0x01,	0x40},	// W#64	-	REVID_STATUS1: Check for Option-2 Pin
      { 0x00,	0x00,	0x0C,	0x90},	// W#65	-	Skip Next Line IF OPT2 != 0 i.e. Data is NOT EQUAL to 0x00 (mask 0x0C)
      { 0x11,	0x40,	0xC3,	0x00},	// W#66	-	If OPT2 ==0 => Ext S5 APC Buck Present. Configure GPIO4 as Digital Output with HIGH (Default Dig VIN0)
      { 0x00,	0x40,	0x50,	0x00},	// W#67	-	Restore XO VMUX to Auto - so that it chooses VREG_XO that runs on AVDD
      { 0x54,	0x03,	0x01,	0xC0},	// W#68	-	Wait for 26ms (= 852 cycles of 32kHz clock)
      { 0x40,	0x91,	0x08,	0x00},	// W#69	-	Write Sequence Complete ACK to PON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#70	-	EOS (Finish Warm-Reset-Sequence)
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#71	-	
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#72	-	
      { 0x29,	0xB2,	0xF1,	0x01},	// W#73	-	CDC_A_SPKR_DRV_CTL: Disable SPKR Drive from Tombak-Analog Codec Module
      { 0x1F,	0xC3,	0xF1,	0x01},	// W#74	-	CDC_A_BOOST_EN_CTL: Disable Boost Regulator from Tombak-Analog Codec Module
      { 0x00,	0x48,	0x5A,	0x00},	// W#75	-	Disable SMPL
      { 0x98,	0x05,	0x4A,	0x89},	// W#76	-	[19] Disable LDO11
      { 0x98,	0x05,	0x4B,	0x89},	// W#77	-	[18] Disable LDO12
      { 0x98,	0x05,	0x47,	0x89},	// W#78	-	[17] Disable LDO8
      { 0x98,	0x05,	0x4C,	0x89},	// W#79	-	[16] Disable LDO13
      { 0x98,	0x05,	0x40,	0x88},	// W#80	-	[13] Disable BMS (Can be left enabled & thus automatically killed after POFF, for slightly more BMS-On-Time & marginally improved SoC calculation - KKGK. Update - needs to be disabled, so that it doesn't request LDO6 ON for ADC-Calculation that in-turn protects QFPROM from getting corrupted)
      { 0x98,	0x05,	0x0A,	0x88},	// W#81	-	[15] Disable VREF_LPDDR
      { 0x98,	0x05,	0x41,	0x89},	// W#82	-	[14] Disable LDO2
      { 0x80,	0x48,	0x45,	0x01},	// W#83	-	Enable Pull-down on LDO6 before disabling it, so that it falls before LDO5
      { 0x98,	0x05,	0x45,	0x89},	// W#84	-	[12] Disable L6 (earlier, to ensure safe QFPROM shutdown before turning off MSM APC, MX, CX rails)
      { 0x98,	0x05,	0x5A,	0x88},	// W#85	-	[8] Disable Sleep Clock (before L5)
      { 0x98,	0x05,	0x51,	0x88},	// W#86	-	[10] Remove SW-Enable to disable BB_CLK1 (Actual disable will happen only after BBCLK1_EN Pin is Low AND SW-Control is also disabled [internal OR-gate for enabling])
      { 0x98,	0x05,	0x44,	0x89},	// W#87	-	[12] Disable LDO5 (This will also pull BBCLK1_EN Pin Low)
      { 0x98,	0x05,	0x50,	0x88},	// W#88	-	Disable XO (19.2MHz Clock)
      { 0x98,	0x05,	0x52,	0x89},	// W#89	-	Disable LDO_XO
      { 0x80,	0x48,	0x46,	0x01},	// W#90	-	Enable Pull-down on LDO7 before disabling it, so that it falls before S4
      { 0x98,	0x05,	0x46,	0x89},	// W#91	-	[11] Disable LDO7
      { 0x98,	0x05,	0xA0,	0x88},	// W#92	-	[7] Disable MPP1
      { 0x98,	0x05,	0xC3,	0x88},	// W#93	-	[6] Disable GPIO4 (Goes Low, for Ext S5 Buck Disable)
      { 0x98,	0x05,	0x17,	0x89},	// W#94	-	[5] Disable S2
      { 0x98,	0x05,	0x14,	0x89},	// W#95	-	[4] Disable S1
      { 0x98,	0x05,	0x42,	0x89},	// W#96	-	[3] Disable LDO3
      { 0x98,	0x05,	0x1A,	0x89},	// W#97	-	[2] Disable S3
      { 0x98,	0x05,	0x1D,	0x89},	// W#98	-	[1] Disable S4
      { 0x40,	0x91,	0x08,	0x00},	// W#99	-	Write Sequence Complete ACK to PON
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#100	-	EOS (Finish POFF-Sequence)
      { 0xFF,	0xFF,	0xFF,	0xFF},	// W#101	-	
      { 0x00,	0x46,	0x00,	0x04},	// W#102	-	Write 0x00 to 0x46 (EN_CTL register in the peripheral)
      { 0x20,	0x00,	0x01,	0xC0},	// W#103	-	Wait 1ms (= 32 cycles of 32kHz clock)
      { 0x00,	0xFF,	0xFF,	0x8C},	// W#104	-	RETURN: Go back to next line of the calling function
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
      { 0x00,	0x01,	0xFF,	0xF8},	// W#127	-	
   },
};
