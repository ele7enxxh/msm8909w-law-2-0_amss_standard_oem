/*! \file  pm_config_target_sbl_sequence.c
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

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/mdm9x45/pm_config_target_sbl_sequence.c#1 $ 
$DateTime: 2015/09/01 00:30:35 $  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_target_information.h"
#include "pm_config_sbl.h"

/*========================== SBL Sequence LUT =============================*/

pm_sbl_seq_type
pm_sbl_seq [ ] =
{
   
   // MODE - INIT: 1
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x3F,	0x1A00,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x3B,	0x4800,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0xBC,	0x800,	0x0F2,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x01,	0x900,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x15,	0x2C00,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x20,	0x800,	0x08A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0x00,	0x500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x80,	0x500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0xA5,	0x3200,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 0,	0x0F,	0x3200,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 0,	0xA5,	0x3100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 0,	0x0F,	0x3100,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 0,	0x01,	0x900,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0xA5,	0x900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 0,	0x00,	0x900,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   
   // MODE - CLOCKS: 2
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x80,	0x5900,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x20,	0x5000,	0x05C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0xFA,	0x5000,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x05,	0x5000,	0x04C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0x00,	0x5000,	0x04D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0xA5,	0x5900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x3C,	0x5900,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0xA5,	0x5900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0x01,	0x5900,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0xA5,	0x5900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0x00,	0x5900,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 0,	0xA5,	0x5900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 0,	0x00,	0x5900,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 0,	0xA0,	0x5A00,	0x058,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 0,	0x0F,	0x5A00,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0x01,	0x5A00,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 0,	0x80,	0x5A00,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 0,	0xFF,	0x5A00,	0x05C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 0,	0x45,	0x6000,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 0,	0x80,	0x6000,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 0,	0x80,	0x5A00,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 0,	0x0F,	0x5400,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 0,	0x0F,	0x5500,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 0,	0x0F,	0x5200,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 0,	0x06,	0x5100,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 0,	0x01,	0x5100,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 0,	0xA5,	0x5000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   { 0,	0x02,	0x5000,	0x0E2,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 28	   
   { 0,	0x00,	0x5900,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 29	   
   { 0,	0x00,	0x5900,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 30	   
   
   // MODE - RESET: 3
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x00,	0x800,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x00,	0x800,	0x047,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x00,	0x800,	0x063,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x99,	0x999,	0x1F4,	PM_SBL_DELAY,	EQUAL,	REV_ID_COMMON},	// 4	   Offset: 500ms. Address Offset represent delay time in this case.
   { 0,	0x01,	0x800,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x80,	0x800,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x06,	0x800,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x80,	0x800,	0x047,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0x00,	0x800,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x80,	0x800,	0x063,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   
   // MODE - LDOs_CONFIG: 4
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x00,	0x4B00,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x80,	0x4600,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x80,	0x4E00,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x00,	0x4D00,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x04,	0x4900,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x1D,	0x4900,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0xF0,	0x4000,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0xD0,	0x4100,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0xA5,	0x4100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x04,	0x4100,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0xD0,	0x4200,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x81,	0x4200,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x32,	0x4200,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x81,	0x4800,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x00,	0x4700,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   
   // MODE - BUCK_UL_LL_CONFIG: 5
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x1400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x42,	0x1400,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0xA5,	0x1400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x00,	0x1400,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0xA5,	0x1400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0xC0,	0x1400,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0xA5,	0x1700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x4A,	0x1700,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0xA5,	0x1700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x42,	0x1700,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0xA5,	0x1700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0xC0,	0x1700,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0xA5,	0x1A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x42,	0x1A00,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0xA5,	0x1A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0x00,	0x1A00,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0xA5,	0x1A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0xC0,	0x1A00,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x0E,	0x2000,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0x06,	0x2000,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0xC0,	0x2000,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0xA5,	0x2300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0x3E,	0x2300,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 1,	0xA5,	0x2300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   { 1,	0x00,	0x2300,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 28	   
   { 1,	0xA5,	0x2300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 29	   
   { 1,	0xC0,	0x2300,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 30	   
   
   // MODE - HFS_CONFIG_S1: 6
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x40,	0x1400,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0xA5,	0x1600,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x01,	0x1600,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0xA5,	0x1600,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x08,	0x1600,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x03,	0x1400,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x04,	0x1400,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x02,	0x1400,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x0B,	0x1400,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xB8,	0x1500,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x15,	0x1400,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x0E,	0x1400,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x84,	0x1500,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x4F,	0x1400,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x02,	0x1400,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0xA8,	0x1400,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0x01,	0x1400,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x30,	0x1500,	0x072,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0x00,	0x1400,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x32,	0x1400,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0x0F,	0x1500,	0x075,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0x88,	0x1400,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0xD0,	0x1400,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0x00,	0x1500,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0x83,	0x1400,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0xF9,	0x1500,	0x070,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 1,	0x80,	0x1400,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   
   // MODE - HFS_CONFIG_S2: 7
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x40,	0x1700,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0xA5,	0x1900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x0B,	0x1900,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0xA5,	0x1900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x04,	0x1900,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x03,	0x1700,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x07,	0x1700,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x05,	0x1700,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x09,	0x1700,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xAC,	0x1800,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x15,	0x1700,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x0C,	0x1700,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x84,	0x1800,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x3E,	0x1700,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x56,	0x1700,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0x07,	0x1700,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0xB6,	0x1700,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x88,	0x1700,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0x0F,	0x1800,	0x075,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x30,	0x1800,	0x072,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0x01,	0x1700,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0x00,	0x1800,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0xD0,	0x1700,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0xA5,	0x1700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0x05,	0x1700,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0xF9,	0x1800,	0x070,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   
   // MODE - HFS_CONFIG_S3: 8
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x40,	0x1A00,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0xA5,	0x1C00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x04,	0x1C00,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0xA5,	0x1C00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x05,	0x1C00,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x03,	0x1A00,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x04,	0x1A00,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x02,	0x1A00,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x0B,	0x1A00,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xB0,	0x1B00,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x15,	0x1A00,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x0E,	0x1A00,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x84,	0x1B00,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x5E,	0x1A00,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x02,	0x1A00,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0xB0,	0x1A00,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0x01,	0x1A00,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x30,	0x1B00,	0x072,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0x00,	0x1A00,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x3F,	0x1A00,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0x0F,	0x1B00,	0x075,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0x88,	0x1A00,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0xD0,	0x1A00,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0x00,	0x1B00,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0xF9,	0x1B00,	0x070,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0xA5,	0x1A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 1,	0x05,	0x1A00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   
   // MODE - HFS_CONFIG_S4: 9
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x40,	0x2000,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0xA5,	0x2200,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x0B,	0x2200,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0xA5,	0x2200,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x08,	0x2200,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x03,	0x2000,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x07,	0x2000,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x03,	0x2000,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x03,	0x2000,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xB0,	0x2100,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x15,	0x2000,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x0C,	0x2000,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x84,	0x2100,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x64,	0x2000,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x0F,	0x2000,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0xBC,	0x2000,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0x88,	0x2000,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x0F,	0x2100,	0x075,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0x30,	0x2100,	0x072,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x01,	0x2000,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0x00,	0x2100,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0xC8,	0x2000,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0x01,	0x2000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0x0C,	0x2000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0x05,	0x2000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 1,	0x81,	0x2000,	0x058,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   { 1,	0xF9,	0x2100,	0x070,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 28	   
   
   // MODE - HFS_CONFIG_S5: 10
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x40,	0x2300,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0xA5,	0x2500,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x05,	0x2500,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0xA5,	0x2500,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x0C,	0x2500,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x03,	0x2300,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x07,	0x2300,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x05,	0x2300,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x09,	0x2300,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xB0,	0x2400,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x06,	0x2300,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x0C,	0x2300,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x85,	0x2400,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x3E,	0x2300,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x56,	0x2300,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0x07,	0x2300,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0xBC,	0x2300,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x88,	0x2300,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0x0F,	0x2400,	0x075,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x30,	0x2400,	0x072,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0x01,	0x2300,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0x00,	0x2400,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0xD0,	0x2300,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0x00,	0x2300,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0x33,	0x2300,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0xA5,	0x2300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 1,	0x05,	0x2300,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   { 1,	0x84,	0x2300,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 28	   
   { 1,	0xF9,	0x2400,	0x070,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 29	   
   
   // MODE - BUCK_STEPPER: 12
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x8A,	0x1400,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x8A,	0x1700,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x8A,	0x1A00,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x8A,	0x2000,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x8A,	0x2300,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   
   // MODE - LDO07: 13
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x03,	0x4600,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x0C,	0x4600,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x80,	0x4600,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x80,	0x4600,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   
   // MODE - BUA: 14
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x80,	0x1C00,	0x047,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   
   // MODE - GPIO_MPP_CONFIG: 15
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xA5,	0xC000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x08,	0xC000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x11,	0xC000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x03,	0xC000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0x80,	0xC000,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x03,	0xC100,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0xA5,	0xC100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x08,	0xC100,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0x00,	0xC300,	0x046,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 9	   
   { 0,	0x05,	0xC300,	0x042,	PM_SBL_WRITE,	EQUAL,	0x01010000},	// 10	   
   { 0,	0x03,	0xC300,	0x041,	PM_SBL_WRITE,	EQUAL,	0x01010000},	// 11	   
   { 0,	0x03,	0xC300,	0x045,	PM_SBL_WRITE,	EQUAL,	0x01010000},	// 12	   
   { 0,	0x01,	0xC300,	0x043,	PM_SBL_WRITE,	EQUAL,	0x01010000},	// 13	   
   { 0,	0x03,	0xC400,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 0,	0x22,	0xC400,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0x04,	0xC400,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 0,	0x01,	0xA200,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 0,	0x03,	0xA000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 0,	0x00,	0xC500,	0x046,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 19	   
   { 0,	0x02,	0xC200,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   
   // MODE - PBS_CONFIG: 16
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x80,	0x7000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0xC0,	0x7000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x00,	0x7000,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x04,	0x7000,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0,	0x999,	0x100,	PM_SBL_PBS_RAM,	EQUAL,	REV_ID_COMMON},	// 5	   PBS Version: PBS_RAM_MDM9x45.PMIC.HW.PMD9635_1p1_1_0_23  PBS RAM Size = 256
   { 0,	0x00,	0x7000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x00,	0x7000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x00,	0x7000,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0x00,	0x7000,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x00,	0x7000,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0x04,	0x7000,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 0,	0x04,	0x7000,	0x074,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 0,	0x04,	0x7000,	0x075,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 0,	0x80,	0x7A00,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 0,	0x08,	0x7000,	0x05C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0x04,	0x7000,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 0,	0x0C,	0x7000,	0x078,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 0,	0x04,	0x7000,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 0,	0x80,	0x7B00,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 0,	0x10,	0x7000,	0x07C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 0,	0x04,	0x7000,	0x07D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 0,	0x80,	0x7C00,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 0,	0x14,	0x7000,	0x058,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 0,	0x04,	0x7000,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 0,	0x18,	0x7000,	0x088,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 0,	0x80,	0x7F00,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 0,	0x04,	0x7000,	0x089,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   
   // MODE - ADC: 17
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x07,	0x3500,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x00,	0x3500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   
   	// This line of data is created by PDM per request from Embedded SW Driver. It is not part of database.
   { 0,	0x00,	0x0000,	0x000,	PM_SBL_OPERATION_INVALID,	EQUAL,	REV_ID_COMMON}
};
