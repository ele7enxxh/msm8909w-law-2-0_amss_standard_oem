/******************************************************************************
 * MODULE     : sns_dd_als_BU52061.h
 * FUNCTION   : Hall Effect Sensor BU52061 Driver header for Qualcomm System 
 *            : Device Driver Framework
 * PROGRAMMED : US Design Center, Applications Engineering Team
 * REMARKS    : 
 * AUTHOR     : Kristopher Bahar
 * COPYRIGHT  : Copyright (C) 2013 - ROHM CO.,LTD. All rights reserved.
 *            : 
 *            : Redistribution and use in source and binary forms, with or 
 *            : without modification, are permitted provided that the
 *            : following conditions are met:
 *            : 1. Redistributions of source code must retain the above 
 *            : copyright notice, this list of conditions and the following
 *            : disclaimer.
 *            : 2. Redistributions in binary form must reproduce the above
 *            : copyright notice, this list of conditions and the following
 *            : disclaimer in the documentation and/or other materials
 *            : provided with the distribution.
 *            : 3. Neither the name of ROHM CO.,LTD. nor the names of its
 *            : contributors may be used to endorse or promote products
 *            : derived from this software without specific prior written
 *            : permission. 
 *            : THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *            : CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *            : INCLUDING , BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *            : MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *            : DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *            : BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *            : CONTRIBUTORS EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *            : BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *            : SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *            : INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *            : WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *            : NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *            : OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *            : SUCH DAMAGE.
 *****************************************************************************/

/****************************************************************************
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
  ****************************************************************************/

#ifndef _SNS_DD_ALS_H_
#define _SNS_DD_ALS_H_

/* Enable the following to view debug messages */
//#define BU52061_DEBUG

//For Get_Attrib Function
#define HALL_SENSOR_POWER_VAL            	            (4) //micro amp
#define HALL_SENSOR_LOW_POWER_VAL        	            (4) //micro amp
#define HALL_SENSOR_BU52053NVX_POWER_VAL            	(8) //micro amp
#define HALL_SENSOR_BU52053NVX_LOW_POWER_VAL        	(2) //micro amp
#define HALL_SENSOR_RANGE_MIN				            (0)
#define HALL_SENSOR_RANGE_MAX				            (1)

#ifdef BU52061_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define BU52061_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "BU52061 - " msg)
#define BU52061_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "BU52061 - " msg,p1)
#define BU52061_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "BU52061 - " msg,p1,p2)
#define BU52061_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BU52061 - " msg,p1,p2,p3)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define BU52061_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define BU52061_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define BU52061_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define BU52061_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif
#else
#define BU52061_MSG_0(level,msg)
#define BU52061_MSG_1(level,msg,p1)
#define BU52061_MSG_2(level,msg,p1,p2)
#define BU52061_MSG_3(level,msg,p1,p2,p3)
#endif

enum
{
    MAG_FIELD_NO_DETECT = 0,
    MAG_FIELD_DETECT  = 1
};

// enums to define device_select mapping to sensor HW
enum
{
   BU52061NVX,
   BU52054GWZ,
   BU52055GWZ,
   BU52015GUL,
   BU52001GUL,
   BU52053NVX,
   BU52056NVX,
   BU52011HFV,
   BU52021HFV,
   BU52025G,
   BD7411G
};


#endif /* End include guard _SNS_DD_ALS_H_ */
