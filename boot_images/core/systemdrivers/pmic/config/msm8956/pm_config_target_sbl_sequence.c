/*! \file  pm_config_target_sbl_sequence.c
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

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/msm8956/pm_config_target_sbl_sequence.c#1 $ 
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
   
   // MODE - DUMMY: 1
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xFF,	0x100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x00,	0x100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   
   // MODE - WATCHDOG_EN: 2
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x01,	0x800,	0x058,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x00,	0x800,	0x057,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x99,	0x999,	0x1F4,	PM_SBL_DELAY,	EQUAL,	REV_ID_COMMON},	// 3	   Offset: 500ms. Address Offset represent delay time in this case.
   { 0,	0x0F,	0x800,	0x054,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0x03,	0x800,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x08,	0x800,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x80,	0x800,	0x057,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   
   // MODE - SPMI_RESET: 3
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x01,	0x800,	0x08C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   
   // MODE - PON_INIT: 4
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0xBC,	0x800,	0x0F2,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x11,	0x2C00,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x20,	0x800,	0x08A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x00,	0x800,	0x0D9,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x01,	0x800,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   
   // MODE - GPIO_MPP_CONFIG: 5
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x51,	0xA000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x02,	0xA000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0xA5,	0xA000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x01,	0xA000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0xA5,	0xA300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x01,	0xA300,	0x0D8,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0xA5,	0xA300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x01,	0xA300,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0xA5,	0xA300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x00,	0xA300,	0x0D9,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0xA5,	0xC300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 0,	0x01,	0xC300,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   
   // MODE - BUA: 6
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x10,	0x1C00,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   
   // MODE - CLOCKS: 7
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x5200,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x01,	0x5200,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0xA5,	0x5000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x02,	0x5000,	0x0E2,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0x05,	0x5000,	0x04C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x00,	0x5000,	0x04D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0xFA,	0x5000,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0xA5,	0x5000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x01,	0x5000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0x80,	0x5900,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 0,	0xA5,	0x5900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 0,	0x3C,	0x5900,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 0,	0xA5,	0x5900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 0,	0x01,	0x5900,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0xA5,	0x5900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 0,	0x00,	0x5900,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 0,	0xA5,	0x5900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 0,	0x00,	0x5900,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 0,	0xA5,	0x5900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 0,	0x01,	0x5900,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 0,	0x01,	0x2800,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 0,	0x80,	0x2800,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 0,	0x45,	0x6000,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 0,	0x80,	0x6000,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 0,	0xA0,	0x5A00,	0x058,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 0,	0x0F,	0x5A00,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   { 0,	0x01,	0x5A00,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 28	   
   { 0,	0x80,	0x5A00,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 29	   
   { 0,	0xFF,	0x5A00,	0x05C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 30	   
   { 0,	0x80,	0x5A00,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 31	   
   { 0,	0xA5,	0x5A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 32	   
   { 0,	0x09,	0x5A00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 33	   
   { 0,	0x81,	0x5100,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 34	   
   { 0,	0xA5,	0x5100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 35	   
   { 0,	0x01,	0x5100,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 36	   
   
   // MODE - PMOS_LDOs_Workaround: 8
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x4700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 5	   
   { 1,	0x02,	0x4700,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 6	   
   { 1,	0xA5,	0x4800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 7	   
   { 1,	0x02,	0x4800,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 8	   
   { 1,	0xA5,	0x4900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 9	   
   { 1,	0x02,	0x4900,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 10	   
   { 1,	0xA5,	0x4A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 11	   
   { 1,	0x02,	0x4A00,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 12	   
   { 1,	0xA5,	0x4B00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 13	   
   { 1,	0x02,	0x4B00,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 14	   
   { 1,	0xA5,	0x4C00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 15	   
   { 1,	0x02,	0x4C00,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 16	   
   { 1,	0xA5,	0x4D00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 17	   
   { 1,	0x02,	0x4D00,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 18	   
   { 1,	0xA5,	0x4E00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 19	   
   { 1,	0x02,	0x4E00,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 20	   
   { 1,	0xA5,	0x4F00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 21	   
   { 1,	0x02,	0x4F00,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 22	   
   { 1,	0xA5,	0x5000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 23	   
   { 1,	0x02,	0x5000,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 24	   
   { 1,	0xA5,	0x5100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 25	   
   { 1,	0x02,	0x5100,	0x0E2,	PM_SBL_WRITE,	EQUAL,	0x01000000},	// 26	   
   
   // MODE - LDOs_CONFIG: 9
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x80,	0x4000,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x00,	0x4300,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x80,	0x4500,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x00,	0x4600,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x00,	0x4800,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x00,	0x5100,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x10,	0x4100,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x10,	0x4200,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x10,	0x4400,	0x045,	PM_SBL_WRITE,	GREATER_OR_EQUAL,	0x02000000},	// 9	   
   { 1,	0x10,	0x4600,	0x045,	PM_SBL_WRITE,	GREATER_OR_EQUAL,	0x02000000},	// 10	   
   { 1,	0xA5,	0x4700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x0D,	0x4700,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0xA5,	0x4A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x0D,	0x4A00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0xA5,	0x4B00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0x0D,	0x4B00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0xA5,	0x4C00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x0D,	0x4C00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0xA5,	0x4100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x01,	0x4100,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0xA5,	0x4400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0x01,	0x4400,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0xA5,	0x4500,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0x01,	0x4500,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0xA5,	0x4600,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0x01,	0x4600,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 0,	0xA5,	0xA00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   { 0,	0x01,	0xA00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 28	   
   { 1,	0xA5,	0x4200,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 29	   
   { 1,	0x05,	0x4200,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 30	   
   { 1,	0x80,	0x4100,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 31	   
   { 1,	0x80,	0x4500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 32	   
   { 1,	0x80,	0x5000,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 33	   
   
   // MODE - BUCK_CURRENT_LIMITS: 10
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x81,	0x1500,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x84,	0x1500,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x80,	0x1800,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x84,	0x1800,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x00,	0x1700,	0x06C,	PM_SBL_WRITE,	LESS_OR_EQUAL,	0x01010000},	// 5	   
   { 1,	0x80,	0x1B00,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x84,	0x1B00,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x81,	0x1E00,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x84,	0x1E00,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   
   // MODE - BUCK_STEPPER: 11
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x83,	0x1400,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x83,	0x1400,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x83,	0x1700,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x83,	0x1700,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x83,	0x1A00,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x83,	0x4200,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   
   // MODE - S1_HFS_CONFIG: 12
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x3E,	0x1400,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x05,	0x1400,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x09,	0x1400,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x05,	0x1400,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x0E,	0x1400,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x81,	0x1400,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x28,	0x1400,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x05,	0x1500,	0x072,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x00,	0x1400,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x90,	0x1400,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x84,	0x1400,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x00,	0x1500,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x05,	0x1400,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   
   // MODE - S2_HFS_CONFIG: 13
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x3E,	0x1700,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x05,	0x1700,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x09,	0x1700,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x05,	0x1700,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x0E,	0x1700,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x81,	0x1700,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x28,	0x1700,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x05,	0x1800,	0x072,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x00,	0x1700,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x90,	0x1700,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x84,	0x1700,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x00,	0x1800,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x05,	0x1700,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   
   // MODE - S3_HFS_CONFIG: 14
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x4E,	0x1A00,	0x041,	PM_SBL_WRITE,	GREATER_OR_EQUAL,	0x02000000},	// 1	   
   { 1,	0x76,	0x1A00,	0x041,	PM_SBL_WRITE,	LESS_OR_EQUAL,	0x01010000},	// 2	   
   { 1,	0x01,	0x1A00,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x05,	0x1A00,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x01,	0x1A00,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x0E,	0x1A00,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x01,	0x1A00,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x28,	0x1A00,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x05,	0x1B00,	0x072,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x00,	0x1A00,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x90,	0x1A00,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x84,	0x1A00,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x01,	0x1B00,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0xA5,	0x1C00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x0B,	0x1C00,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0x05,	0x1A00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   
   // MODE - S4_HFS_CONFIG: 15
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x1A,	0x1D00,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x01,	0x1D00,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x05,	0x1D00,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x01,	0x1D00,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x0E,	0x1D00,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x01,	0x1D00,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x28,	0x1D00,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x05,	0x1E00,	0x072,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x00,	0x1D00,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x90,	0x1D00,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x84,	0x1D00,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x01,	0x1E00,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x05,	0x1D00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   
   // MODE - CHARGER: 16
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x87,	0x1000,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x80,	0x1000,	0x064,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x80,	0x1000,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0xC0,	0x1200,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0xA5,	0x1300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x01,	0x1300,	0x0DB,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0xA5,	0x1300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x00,	0x1300,	0x0DB,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0xA5,	0x1000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x01,	0x1000,	0x0DB,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0xA5,	0x1000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 0,	0x00,	0x1000,	0x0DB,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 0,	0xA5,	0x1300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 0,	0x0B,	0x1300,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 0,	0xA5,	0x1000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0x0B,	0x1000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   
   // MODE - WLED_PWM_DIMMING: 17
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xA5,	0xA300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x01,	0xA300,	0x0D8,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0xA5,	0xA300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x01,	0xA300,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0xA5,	0xA300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x00,	0xA300,	0x0D9,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0xA5,	0xBC00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x01,	0xBC00,	0x0E2,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   
   // MODE - RESET: 18
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x04,	0x800,	0x075,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x00,	0x800,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x00,	0x800,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0x00,	0x800,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x00,	0x800,	0x063,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x99,	0x999,	0x1F4,	PM_SBL_DELAY,	EQUAL,	REV_ID_COMMON},	// 7	   Offset: 500ms. Address Offset represent delay time in this case.
   { 0,	0x01,	0x800,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0x80,	0x800,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x0E,	0x800,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0x07,	0x800,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 0,	0x05,	0x800,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 0,	0x80,	0x800,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 0,	0x0C,	0x800,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 0,	0x07,	0x800,	0x049,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0x08,	0x800,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 0,	0x80,	0x800,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 0,	0x00,	0x800,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 0,	0x80,	0x800,	0x063,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   
   // MODE - VM_BMS: 19
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xA5,	0x4000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x01,	0x4000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   
   // MODE - INTERRUPT: 20
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x00,	0x500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x80,	0x500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   
   // MODE - PBS_CONFIG: 21
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xC0,	0x7500,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x80,	0x7500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0xA5,	0x7500,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x01,	0x7500,	0x0D8,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0xA5,	0x7000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x07,	0x7000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x80,	0x7000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0xC0,	0x7000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0x00,	0x7000,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x04,	0x7000,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0,	0x999,	0x999,	PM_SBL_PBS_RAM,	EQUAL,	REV_ID_COMMON},	// 11	   PBS Version: PBS_RAM_MSM893x.PMIC.HW.PM8916_2p0_1_0_1
   { 0,	0x00,	0x7000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 0,	0x00,	0x7000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 0,	0x00,	0x7000,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 0,	0x00,	0x7000,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0x08,	0x7000,	0x058,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 0,	0x04,	0x7000,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 0,	0x0C,	0x7000,	0x05C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 0,	0x04,	0x7000,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 0,	0x10,	0x7000,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 0,	0x04,	0x7000,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 0,	0x18,	0x7000,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 0,	0x04,	0x7000,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 0,	0xA5,	0x7000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 0,	0x01,	0x7000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   
   // MODE - ADC: 22
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xA5,	0x3100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x0F,	0x3100,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0xA5,	0x3400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x0F,	0x3400,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0xA5,	0x3200,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x0F,	0x3200,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0xA5,	0x3300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x0F,	0x3300,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0xA5,	0x3500,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x0F,	0x3500,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   
   // MODE - CODEC: 23
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0xF000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x0F,	0xF000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0xA5,	0xF100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x0F,	0xF100,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x82,	0xF100,	0x0C0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0xE1,	0xF100,	0x0B4,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   
   // MODE - WATCHDOG_DIS: 24
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x12,	0x900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x01,	0x800,	0x058,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x00,	0x800,	0x057,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   
   	// This line of data is created by PDM per request from Embedded SW Driver. It is not part of database.
   { 0,	0x00,	0x0000,	0x000,	PM_SBL_OPERATION_INVALID,	EQUAL,	REV_ID_COMMON}
};
