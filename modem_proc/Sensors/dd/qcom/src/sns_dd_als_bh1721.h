/******************************************************************************
 * MODULE     : sns_dd_als_bh1721.h
 * FUNCTION   : Ambient light Sensor BH1721 Driver header for Qualcomm Sensors 
 *            : Device Driver Framework
 * PROGRAMMED : Sensor system Development Group, ROHM CO.,LTD.
 * REMARKS    : 
 * AUTHOR     : Masafumi Seike
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
#ifndef _SNS_DD_ALS_H_
#define _SNS_DD_ALS_H_

/* Enable the following to view debug messages */
//#define BH1721_DEBUG

#define ALS_SENSOR_I2C_ADDR             (0x23)

#define ALS_SENSOR_POWER_VAL            (140)
#define ALS_SENSOR_LOW_POWER_VAL        (1)
#define ALS_SENSOR_RANGE_MIN            (0)
#define ALS_SENSOR_RANGE_MAX1           (8191)
#define ALS_SENSOR_RANGE_MAX2           (65528)

#define ALS_INIT_SET_TIMING             (300)

#define _ALS_BIG_ENDIAN_   (1)
#ifdef _ALS_BIG_ENDIAN_
#define CONVERT_TO_BE(value) \
            ((((value) >> 8) & 0xFF) | (((value) << 8) & 0xFF00))
#else
#define CONVERT_TO_BE(value) (value)
#endif

#define CALC_UNIT_MAX          (26214)
/************ define parameter for register ************/
/* Operate */
#define CMD_POWER_DOWN         (0x00)
#define CMD_POWER_ON           (0x01)
#define CMD_AUTO_RESOLUTION    (0x10)
#define CMD_HIGH_RESOLUTION    (0x12)
#define CMD_LOW_RESOLUTION     (0x13)
#define CMD_HIGH_MESURE_TIME   (0x40)
#define CMD_LOW_MESURE_TIME    (0x60)
#define CMD_DUMMY_READ         (0x80)

/* for rang value */
#define ALS_RANGE_TIME_MIN_VAL (140)
#define ALS_RANGE_TIME_TYP_VAL (300)
#define ALS_RANGE_TIME_MAX_VAL (1020)

/* attribute for NV items */
//"SNS_DD_ALS_FACTOR"  represents the ALS Transmission Rate of the Optical Window
// This value is a percentage with a 1% step.
// Thus, if the optical window blocks 50% of incoming ALS light, "SNS_DD_ALS_FACTOR" = 50.
#define SNS_DD_ALS_FACTOR                     100	

#define SNS_DD_ROHM_NUM						  0x5A5A5A5A

#ifdef BH1721_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define BH1721_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "BH1721 - " msg)
#define BH1721_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "BH1721 - " msg,p1)
#define BH1721_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "BH1721 - " msg,p1,p2)
#define BH1721_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BH1721 - " msg,p1,p2,p3)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define BH1721_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define BH1721_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define BH1721_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define BH1721_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif
#else
#define BH1721_MSG_0(level,msg)
#define BH1721_MSG_1(level,msg,p1)
#define BH1721_MSG_2(level,msg,p1,p2)
#define BH1721_MSG_3(level,msg,p1,p2,p3)
#endif


#endif /* End include guard _SNS_DD_ALS_H_ */
