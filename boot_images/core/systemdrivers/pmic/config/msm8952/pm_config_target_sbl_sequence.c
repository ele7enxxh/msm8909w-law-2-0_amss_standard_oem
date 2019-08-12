/*! \file  pm_config_target_sbl_sequence.c
 *  
 *  \brief  File Contains the PMIC Set Mode Driver Implementation
 *  \details Set Mode Driver implementation is responsible for setting and getting 
 *  all mode settings such as Register values, memory values, etc.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: PM8950-x.x-OTPx.x-MSM8952-01212015v1_b0_v009 - Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2015 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/config/msm8952/pm_config_target_sbl_sequence.c#1 $ 
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
   
   // MODE - PON_INIT: 3
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0xBC,	0x800,	0x0F2,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x11,	0x2C00,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x20,	0x800,	0x08A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0x48,	0x800,	0x082,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x40,	0x900,	0x047,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x00,	0x800,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x00,	0x800,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0x99,	0x999,	0x0A0,	PM_SBL_DELAY,	EQUAL,	REV_ID_COMMON},	// 9	   Offset: 160ms. Address Offset represent delay time in this case.
   { 0,	0x80,	0x800,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0x99,	0x999,	0x0A0,	PM_SBL_DELAY,	EQUAL,	REV_ID_COMMON},	// 11	   Offset: 160ms. Address Offset represent delay time in this case.
   { 0,	0x00,	0x800,	0x04C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 0,	0x00,	0x800,	0x04D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 0,	0x99,	0x999,	0x0A0,	PM_SBL_DELAY,	EQUAL,	REV_ID_COMMON},	// 14	   Offset: 160ms. Address Offset represent delay time in this case.
   { 0,	0x80,	0x800,	0x04F,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 0,	0x00,	0x800,	0x0D9,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 0,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 0,	0x03,	0x800,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 0,	0x00,	0x900,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 0,	0x01,	0x2400,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   
   // MODE - CLOCKS: 4
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x5200,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x03,	0x5200,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0xA5,	0x5000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x02,	0x5000,	0x0E2,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0x20,	0x5000,	0x05C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x05,	0x5000,	0x04C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x00,	0x5000,	0x04D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0xFA,	0x5000,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 0,	0xA5,	0x5000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x03,	0x5000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
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
   { 0,	0x03,	0x5900,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
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
   
   // MODE - S6_FTS_LUT: 5
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x0A,	0x2400,	0x080,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x08,	0x2400,	0x081,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x04,	0x2400,	0x082,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x3F,	0x2400,	0x083,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x03,	0x2400,	0x087,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x04,	0x2400,	0x088,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x3F,	0x2400,	0x089,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x40,	0x2400,	0x08C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x2B,	0x2400,	0x08D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x44,	0x2400,	0x08E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x00,	0x2400,	0x08F,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x01,	0x2400,	0x090,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x00,	0x2400,	0x091,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   
   // MODE - S5_6_GANG_SEQ: 6
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x2300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x20,	0x2400,	0x0C0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0xA5,	0x2300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x20,	0x2300,	0x0C0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0xA5,	0x2300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x80,	0x2300,	0x0C1,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x80,	0x2000,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x80,	0x2100,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x01,	0x2300,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x01,	0x2400,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0xC0,	0x2300,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0xC0,	0x2400,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0x80,	0x2000,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x01,	0x2000,	0x054,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   
   // MODE - FTS_5_6_CONFIG: 7
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x05,	0x2000,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x1A,	0x2000,	0x063,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x07,	0x2000,	0x0AC,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x01,	0x2000,	0x0AD,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x80,	0x2000,	0x0A6,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x00,	0x2000,	0x0D9,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x00,	0x2100,	0x0D9,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0x00,	0x2300,	0x0D9,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x00,	0x2400,	0x0D9,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0x01,	0x2000,	0x054,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0xC0,	0x2000,	0x064,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0x40,	0x2000,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0x08,	0x2100,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0x08,	0x2400,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   { 1,	0x01,	0x2100,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 28	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 29	   
   { 1,	0x04,	0x2100,	0x063,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 30	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 31	   
   { 1,	0x04,	0x2400,	0x063,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 32	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 33	   
   { 1,	0x01,	0x2400,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 34	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 35	   
   { 1,	0x05,	0x2100,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 36	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 37	   
   { 1,	0x05,	0x2000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 38	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 39	   
   { 1,	0x05,	0x2300,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 40	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 41	   
   { 1,	0x05,	0x2400,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 42	   
   { 1,	0x04,	0x2000,	0x071,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 43	   
   { 1,	0x04,	0x2300,	0x071,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 44	   
   { 1,	0x04,	0x2300,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 45	   
   { 1,	0x00,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 46	   
   
   // MODE - BUCK_STEPPER: 9
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x83,	0x1400,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x83,	0x1400,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x83,	0x1700,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x83,	0x1700,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x83,	0x1A00,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x83,	0x1A00,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x84,	0x1D00,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x84,	0x1D00,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x0C,	0x2000,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x8C,	0x2000,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x83,	0x4200,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   
   // MODE - BUCK_CURRENT_LIMITS: 10
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xB8,	0x1500,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x84,	0x1500,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0xB8,	0x1800,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x84,	0x1800,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0xB9,	0x1B00,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x83,	0x1B00,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0xB2,	0x1E00,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x84,	0x1E00,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xAA,	0x2100,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   
   // MODE - BUCK_UL_LL_AND_PD_CONFIG: 11
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x1400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x50,	0x1400,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0xA5,	0x1400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x21,	0x1400,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0xA5,	0x1400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0xC0,	0x1400,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0xA5,	0x1700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x50,	0x1700,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0xA5,	0x1700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x0A,	0x1700,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0xA5,	0x1700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0xC0,	0x1700,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0xA5,	0x1A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x55,	0x1A00,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0xA5,	0x1A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0x40,	0x1A00,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0xA5,	0x1A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0xC0,	0x1A00,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0xA5,	0x1D00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x14,	0x1D00,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0xA5,	0x1D00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0x08,	0x1D00,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0xA5,	0x1D00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0xC0,	0x1D00,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0xFF,	0x2000,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   { 1,	0x90,	0x2000,	0x06B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 28	   
   { 1,	0xA5,	0x2000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 29	   
   { 1,	0xC0,	0x2000,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 30	   
   { 1,	0x08,	0x2300,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 31	   
   
   // MODE - S1_HFS_CONFIG: 12
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x1600,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x05,	0x1600,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x44,	0x1400,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x00,	0x1400,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x09,	0x1400,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x05,	0x1400,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x05,	0x1400,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x15,	0x1400,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x4F,	0x1400,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xB2,	0x1400,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x0E,	0x1400,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x04,	0x1400,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x00,	0x1400,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x88,	0x1400,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0x01,	0x1500,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x40,	0x1400,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0xA5,	0x1400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x05,	0x1400,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   
   // MODE - S2_HFS_CONFIG: 13
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x1900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x05,	0x1900,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x44,	0x1700,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x00,	0x1700,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x09,	0x1700,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x08,	0x1700,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x05,	0x1700,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x15,	0x1700,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x4F,	0x1700,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xB2,	0x1700,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x0E,	0x1700,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x04,	0x1700,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x00,	0x1700,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x88,	0x1700,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0x01,	0x1800,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x40,	0x1700,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0xA5,	0x1700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x05,	0x1700,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   
   // MODE - S3_HFS_CONFIG: 14
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x1C00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x0B,	0x1C00,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x49,	0x1A00,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x00,	0x1A00,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x0A,	0x1A00,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x09,	0x1A00,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x05,	0x1A00,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x15,	0x1A00,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0x4F,	0x1A00,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0xB2,	0x1A00,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x0E,	0x1A00,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x04,	0x1A00,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x00,	0x1A00,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0x88,	0x1A00,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x01,	0x1B00,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0x40,	0x1A00,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0xA5,	0x1A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0x05,	0x1A00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   
   // MODE - S4_HFS_CONFIG: 15
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0xA5,	0x1F00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x0B,	0x1F00,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x14,	0x1D00,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x02,	0x1D00,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x0A,	0x1D00,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x07,	0x1D00,	0x053,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x05,	0x1D00,	0x056,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x44,	0x1D00,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x67,	0x1D00,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xB2,	0x1D00,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 1,	0x0C,	0x1D00,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x0F,	0x1D00,	0x05E,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0x00,	0x1D00,	0x055,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0x88,	0x1D00,	0x059,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0x01,	0x1E00,	0x079,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x40,	0x1D00,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0xA5,	0x1D00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x05,	0x1D00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   
   // MODE - LDOs_CONFIG: 16
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 1,	0x80,	0x4000,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 1,	0x00,	0x4300,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 1,	0x00,	0x4500,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 1,	0x00,	0x4600,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 1,	0x00,	0x4800,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 1,	0x00,	0x5100,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 1,	0x10,	0x4100,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 1,	0x10,	0x4200,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 1,	0x10,	0x4400,	0x045,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 1,	0xA5,	0x4700,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 1,	0x0D,	0x4700,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 1,	0xA5,	0x4A00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 1,	0x0D,	0x4A00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 1,	0xA5,	0x4B00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 1,	0x0D,	0x4B00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 1,	0xA5,	0x4C00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 1,	0x0D,	0x4C00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 1,	0xA5,	0x4100,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 1,	0x05,	0x4100,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 1,	0xA5,	0x4400,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 1,	0x05,	0x4400,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 1,	0xA5,	0x4500,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 1,	0x05,	0x4500,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   { 1,	0xA5,	0x4600,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 25	   
   { 1,	0x05,	0x4600,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 26	   
   { 0,	0xA5,	0xA00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 27	   
   { 0,	0x05,	0xA00,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 28	   
   { 1,	0xA5,	0x4200,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 29	   
   { 1,	0x05,	0x4200,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 30	   
   { 1,	0x80,	0x4100,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 31	   
   { 1,	0x80,	0x4500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 32	   
   { 1,	0x80,	0x5000,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 33	   
   
   // MODE - GPIO_MPP_CONFIG: 17
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x51,	0xA000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x02,	0xA000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0xA5,	0xA000,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x01,	0xA000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0x11,	0xC300,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x80,	0xC300,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0xA5,	0xC300,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x01,	0xC300,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   
   // MODE - INTERRUPT: 18
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x00,	0x500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x80,	0x500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   
   // MODE - RESET: 19
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x04,	0x800,	0x075,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x00,	0x800,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 0,	0x00,	0x800,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 0,	0x00,	0x800,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 0,	0x00,	0x800,	0x063,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 0,	0x00,	0x800,	0x067,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 0,	0x99,	0x999,	0x1F4,	PM_SBL_DELAY,	EQUAL,	REV_ID_COMMON},	// 8	   Offset: 500ms. Address Offset represent delay time in this case.
   { 0,	0x07,	0x800,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 0,	0x80,	0x800,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 0,	0x0E,	0x800,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 0,	0x07,	0x800,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 0,	0x05,	0x800,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 0,	0x80,	0x800,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 0,	0x0C,	0x800,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 0,	0x07,	0x800,	0x049,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 0,	0x08,	0x800,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 0,	0x80,	0x800,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 0,	0x00,	0x800,	0x062,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 0,	0x80,	0x800,	0x063,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 0,	0x05,	0x800,	0x066,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 0,	0x80,	0x800,	0x067,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   
   // MODE - PBS_CONFIG: 20
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
   { 0,	0,	0x999,	0x80,	PM_SBL_PBS_RAM,	EQUAL,	REV_ID_COMMON},	// 11	   PBS Version: PBS_RAM_MSM8952.PMIC.HW.PM8950_1p0_1_0_3  PBS RAM Size = 128
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
   
   // MODE - BUA_BATT_GONE_SEL: 21
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x96,	0x1C00,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   
   // MODE - DUMMY_PMi8950: 22
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 2,	0x81,	0x100,	0x004,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   
   // MODE - PON_INIT_PMi8950: 23
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 2,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 2,	0xA4,	0x800,	0x0F2,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 2,	0x60,	0x800,	0x08A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   
   // MODE - PBS_CONFIG_PMi8950: 24
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 2,	0xA5,	0x800,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 2,	0x07,	0x7000,	0x0DA,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 2,	0x80,	0x7000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 2,	0xC0,	0x7000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 2,	0x00,	0x7000,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 2,	0x04,	0x7000,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 2,	1,	0x999,	0x80,	PM_SBL_PBS_RAM,	EQUAL,	REV_ID_COMMON},	// 7	   PBS Version: PBS_RAM_MSM8952.PMIC.HW.PMi8950_1p0_2_0_4  PBS RAM Size = 128
   { 2,	0x00,	0x7000,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 2,	0x00,	0x7000,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 2,	0x00,	0x7000,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 2,	0x00,	0x7000,	0x043,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 2,	0x04,	0x7000,	0x065,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 2,	0x00,	0x7000,	0x064,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   { 2,	0x80,	0x7600,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 14	   
   { 2,	0x04,	0x7000,	0x06D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 15	   
   { 2,	0x04,	0x7000,	0x06C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 16	   
   { 2,	0x80,	0x7800,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 17	   
   { 2,	0x04,	0x7000,	0x069,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 18	   
   { 2,	0x08,	0x7000,	0x068,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 19	   
   { 2,	0x80,	0x7700,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 20	   
   { 2,	0x80,	0x7700,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 21	   
   { 2,	0x04,	0x7000,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 22	   
   { 2,	0x0C,	0x7000,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 23	   
   { 2,	0x80,	0x7100,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 24	   
   
   // MODE - SCHG_PMi8950: 25
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 2,	0x01,	0x7100,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 2,	0x40,	0x1300,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 2,	0x00,	0x1300,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 2,	0xA5,	0x1600,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   { 2,	0x18,	0x1600,	0x0F6,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 13	   
   
   // MODE - RESET_PMi8950: 26
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 2,	0x00,	0x800,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x99,	0x999,	0x1F4,	PM_SBL_DELAY,	EQUAL,	REV_ID_COMMON},	// 2	   Offset: 500ms. Address Offset represent delay time in this case.
   { 2,	0x01,	0x800,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 2,	0x80,	0x800,	0x05B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   
   // MODE - INTERRUPT_PMI8950: 27
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 2,	0x00,	0x500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 2,	0x80,	0x500,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   
   // MODE - FLASH_PMi8950: 28
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 3,	0x81,	0xD300,	0x05C,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 3,	0x03,	0xD300,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 3,	0x40,	0xD300,	0x04F,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 3,	0xA1,	0xD300,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 3,	0xAD,	0xD300,	0x054,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 3,	0x80,	0xD300,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 3,	0xE1,	0xD300,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 3,	0x4F,	0xD300,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 3,	0x81,	0xD300,	0x05D,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 2,	0xA5,	0x1600,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 2,	0x25,	0x1600,	0x0F4,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   
   // MODE - HAPTICS_PMi8950: 29
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 3,	0x00,	0xC000,	0x04F,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 3,	0x01,	0xC000,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 3,	0x01,	0xC000,	0x052,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   
   // MODE - WLED_PMi8950: 30
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 3,	0x04,	0xD900,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 3,	0x80,	0xD800,	0x05A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 3,	0x6C,	0xD900,	0x04A,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 3,	0x00,	0xD900,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 3,	0x00,	0xD900,	0x060,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 3,	0x00,	0xD900,	0x070,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   { 3,	0x00,	0xD900,	0x080,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 10	   
   { 3,	0xA5,	0xD900,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 11	   
   { 3,	0x14,	0xD900,	0x0E6,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 12	   
   
   // MODE - LCD_AMOLED_PMi8950: 31
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 3,	0xA5,	0xDE00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 3,	0x07,	0xDE00,	0x048,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 3,	0x80,	0xDE00,	0x049,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 3,	0xA5,	0xDC00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 3,	0xBF,	0xDC00,	0x04B,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   { 3,	0x85,	0xDC00,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 6	   
   { 3,	0xA5,	0xDC00,	0x0D0,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 7	   
   { 3,	0xF7,	0xDC00,	0x058,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 8	   
   { 3,	0x00,	0xDC00,	0x061,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 9	   
   
   // MODE - MPP_GPIO_PMi8950: 32
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 2,	0x01,	0xC100,	0x040,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 2,	0x00,	0xC100,	0x041,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 2,	0x05,	0xC100,	0x042,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   { 2,	0x02,	0xC100,	0x044,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 4	   
   { 2,	0x80,	0xC100,	0x046,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 5	   
   
   // MODE - SBL_ID: 33
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x00,	0x100,	0x051,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 1	   
   { 0,	0x08,	0x100,	0x050,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   
   // MODE - WATCHDOG_DIS: 34
   //sid  data  base_addr  offset  reg_op  rev_id_op  rev_id
   { 0,	0x01,	0x800,	0x058,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 2	   
   { 0,	0x00,	0x800,	0x057,	PM_SBL_WRITE,	EQUAL,	REV_ID_COMMON},	// 3	   
   
   	// This line of data is created by PDM per request from Embedded SW Driver. It is not part of database.
   { 0,	0x00,	0x0000,	0x000,	PM_SBL_OPERATION_INVALID,	EQUAL,	REV_ID_COMMON}
};
