/*******************************************************************************
* Copyright (c) 2013, ALPS ELECTRIC CO., LTD.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of ALPS ELECTRIC CO., LTD. nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
* IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

/*-----------------------------------------------------------------------------
* Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------------*/

/*==============================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -----------------------------------------------------------
2014-02-03   MW      Updated max freq.
2014-01-29   ALPS    Modified sensor range defined. And Added temperature sensor.
2014-01-27   MW      Added debug message macros
2013-12-26   ALPS    Initial version
==============================================================================*/


#ifndef ALPS_DRIVER_PRS_H
#define ALPS_DRIVER_PRS_H

#include "sns_ddf_driver_if.h"

/*---------------------------------------------------------------------------
Register address
----------------------------------------------------------------------------*/
#define		CMD_ACT			0xAC

// Enable this macro to view driver debug logs
//#define HSPPAD038A_DEBUG

enum {
    HSPPAD_PRS,
    HSPPAD_TMP,
    HSPPAD_NUM_DATATYPES
};


#define		ALPS_PRS_SENSOR_POWER_ACTIVE	(1500)
#define		ALPS_PRS_SENSOR_LOWPOWER	(1)

/* 30-110kPa = 300-1100hPa */
#define		ALPS_PRS_SENSOR_RANGE_MIN	(FX_FLTTOFIX_Q16(300.0))
#define		ALPS_PRS_SENSOR_RANGE_MAX	(FX_FLTTOFIX_Q16(1100.0))

/* 1kPa = 10hPa */
#define		ALPS_PRS_SENSOR_RESOLUTION	(860.0f / 65535.0f)
#define		ALPS_PRS_SENSOR_INTERCEPT	(250.0f)

#define		ALPS_TMP_SENSOR_RANGE_MIN	(FX_FLTTOFIX_Q16(-40.0))
#define		ALPS_TMP_SENSOR_RANGE_MAX	(FX_FLTTOFIX_Q16(85.0))

#define		ALPS_TMP_SENSOR_RESOLUTION	(125.0f / 65535.0f)
#define		ALPS_TMP_SENSOR_INTERCEPT	(-40.0f)

#define		ALPS_PRS_SENSOR_BIT_LEN		(16)
#define		ALPS_PRS_SENSOR_MAX_FREQ	(90)

#define		ALPS_PRS_SENSOR_DELAY_ACTIVE	(10000)
#define		ALPS_PRS_SENSOR_DELAY_DATA	(10000)

#define		ALPS_PRS_SENSOR_DRIVER_NAME	"ALPS Pressure device driver"
#define		ALPS_PRS_SENSOR_DRIVER_VER	(1)

#define		ALPS_PRS_SENSOR_DEVICE_NAME	"Pressure"
#define		ALPS_PRS_SENSOR_DEVICE_VENDOR	"ALPS ELECTRIC CO., LTD."
#define		ALPS_PRS_SENSOR_DEVICE_MODEL	"HSPPAD038"
#define		ALPS_PRS_SENSOR_DEVICE_VER	1

#define		ALPS_PRS_SENSOR_READ_DATA_NUM	(5)

#define  TIMER_10_MS  (10000)

#ifdef DDK
typedef bool		boolean;
#endif

#ifdef HSPPAD038A_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define HSPPAD038A_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "" msg)
#define HSPPAD038A_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "" msg,p1)
#define HSPPAD038A_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "" msg,p1,p2)
#define HSPPAD038A_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "" msg,p1,p2,p3)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define HSPPAD038A_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define HSPPAD038A_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define HSPPAD038A_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define HSPPAD038A_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif
#else /* HSPPAD038A_DEBUG */
#define HSPPAD038A_MSG_0(level,msg)
#define HSPPAD038A_MSG_1(level,msg,p1)
#define HSPPAD038A_MSG_2(level,msg,p1,p2)
#define HSPPAD038A_MSG_3(level,msg,p1,p2,p3)
#endif /* HSPPAD038A_DEBUG */

#endif
