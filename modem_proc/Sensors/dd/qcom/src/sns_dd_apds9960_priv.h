/*==============================================================================

    SENSORS RGB, CCT, AMBIENT LIGHT AND PROXIMITY  COMBO DRIVER

DESCRIPTION

   Implements the driver for Avago RGB, CCT, ALS and Proximity Sensor APDS-9960

********************************************************************************
* Copyright (c) 2014 Avago Technologies.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in the
*        documentation and/or other materials provided with the distribution.
*     3. Neither the name of Avago nor the
*        names of its contributors may be used to endorse or promote products
*        derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************

==============================================================================*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------------*/
/**************************************************************************
*
* REVISON HISTORY
*
* VERSION	DATE        WHO  DESCRIPTION
*
* 1.0     10/10/2014  KK   Initial version 
                           Derived from sns_dd_apds9960 combo driver of 
                           ALS/PRX/RGB/CCT
  1.1     16Mar2015   KK   (i) Enable flag was not disabled at the end of 
                           handle_device_indication which should be disabled.
                           This caused the sensor not being enabled again in
                           next polling cycle.
                           (ii) Revert back IRQ trigger from SNS_DDF_SIGNAL_IRQ_LOW
                           to SNS_DDF_SIGNAL_IRQ_FALLING.
                           (iii) Set dri_enabled in enable_sched_data() instead of 
                           init()

  1.2     19May2015   KK   Revise force interrupt during enable_interrupt for prox

  1.3     11Jun2015   KK   Reset enable flag in set_powerstate()
  1.4     15Jun2015   AH   Refactored for uImage
  1.4.1   22Jun2015   KK   Revise RGB/CCT change gain mechanism in apds9960_dd_als_raw_data_ready()
*
**************************************************************************/

/*============================================================================
INCLUDE FILES
============================================================================*/

#ifndef _SNSDAPDS9960PRIV_H
#define _SNSDAPDS9960PRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"

/* TODO - set to 0 before deliver to customers */
#define USE_DRAGONBOARD_BUILD 0 

/* Enable following macro for uImage support
*/
#define APDS9960_ENABLE_UIMG_SUPPORT

/* TODO - remove before deliver to customers */
/* Enable this flag when compiling on OpenSSC HD22 builds.
*/
//#define APDS9960_COMPILE_FOR_HD22

/*=======================================================================

INTERNAL DEFINITIONS

========================================================================*/

// Enable the following macro to view debug messages
//#define APDS9960_DD_DEBUG
#define ALSPRX_MAX_NUM_AXES 3

#if USE_DRAGONBOARD_BUILD
#define NULL  0
#define abs(a) (((a)>=0)?(a): (-a))
#endif

#undef  ALSPRX_EN_DISTANCE
#define ALSPRX_DEBUG_REG
#undef  ALSPRX_DEBUG_TIMER
#undef  ALSPRX_DEBUG_ONE_SENSOR
#undef  ALSPRX_DEBUG_BASIC

#if USE_DRAGONBOARD_BUILD
#include "qurt_elite_diag.h"
#ifndef APDS9960_ENABLE_UIMG_SUPPORT
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define ALSPRX_MSG_0(level,msg)          MSG(MSG_SSID_QDSP6,DBG_##level##_PRIO, "APDS9960 - " msg)
#define ALSPRX_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_QDSP6,DBG_##level##_PRIO, "APDS9960 - " msg,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_QDSP6,DBG_##level##_PRIO, "APDS9960 - " msg,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "APDS9960 - " msg,p1,p2,p3)
#else // APDS9960_ENABLE_UIMG_SUPPORT
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define ALSPRX_MSG_0(level,msg)          UMSG(MSG_SSID_QDSP6,DBG_##level##_PRIO, "APDS9960 - " msg)
#define ALSPRX_MSG_1(level,msg,p1)       UMSG_1(MSG_SSID_QDSP6,DBG_##level##_PRIO, "APDS9960 - " msg,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)    UMSG_2(MSG_SSID_QDSP6,DBG_##level##_PRIO, "APDS9960 - " msg,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3) UMSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "APDS9960 - " msg,p1,p2,p3)
#endif // APDS9921_ENABLE_UIMG_SUPPORT
#else // USE_DRAGONBOARD_BUILD
#ifdef APDS9960_DD_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define ALSPRX_MSG_0(level,msg)          UMSG(MSG_SSID_SNS,DBG_##level##_PRIO, "APDS9960 - " msg)
#define ALSPRX_MSG_1(level,msg,p1)       UMSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "APDS9960 - " msg,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)    UMSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "APDS9960 - " msg,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "APDS9960 - " msg,p1,p2,p3)
#else // QDSP6
#define MED MEDIUM
#include "sns_debug_str.h"
#define ALSPRX_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define ALSPRX_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif // QDSP6
#else /* APDS9960_DD_DEBUG */
#define ALSPRX_MSG_0(level,msg)
#define ALSPRX_MSG_1(level,msg,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3)
#endif /* APDS9960_DD_DEBUG */
#endif /* USE_DRAGONBOARD_BUILD */

#ifndef APDS9960_ENABLE_UIMG_SUPPORT
#define sns_ddf_malloc_ex(destination_ptr, size, smgr_handle) sns_ddf_malloc(destination_ptr, size)
#define sns_ddf_mfree_ex(destination_ptr, smgr_handle) sns_ddf_mfree(destination_ptr)
#define sns_ddf_memhandler_malloc_ex(memhandler, size, smgr_handle) sns_ddf_memhandler_malloc(memhandler, size)
#endif

#define CHECK_STATUS(status)        \
  if ( status != SNS_DDF_SUCCESS )  \
  {                                 \
    return status;                  \
  }

#ifdef APDS9960_COMPILE_FOR_HD22
#define APDS9960_LOG_PACKET SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC
sns_ddf_driver_if_s sns_dd_vendor_if_1;
#define APDS9960_DD_IF sns_dd_vendor_if_1
#else
#define APDS9960_LOG_PACKET SNS_LOG_CONVERTED_SENSOR_DATA
sns_ddf_driver_if_s sns_dd_apds9960_driver_if;
#define APDS9960_DD_IF sns_dd_apds9960_driver_if
#endif

#define APDS9960_DD_DFLT_MODE SNS_NORMAL_MODE
#define APDS9960_DD_SUPPORTED_MODE (1 << SNS_NORMAL_MODE | 1 << SNS_SLEEP_MODE)
#define APDS9960_DD_DATA_READY_MS 200       /* after issue a command, assume 100ms integration worst case */
#define APDS9960_DD_DATA_READY_US 200000	/* after PRX integration, assume 100ms integration worst case */

#define APDS9960_DD_ALS_DFLT_MILLI_LUX 100000  /* default value for the last sampled mlux */

#define APDS9960_DD_ALS_MAX_LUX 30000

#define APDS9960_DD_PILT_INIT APDS9960_DD_PRX_FAR_THRESHOLD
#define APDS9960_DD_PIHT_INIT APDS9960_DD_PRX_NEAR_THRESHOLD
#define APDS9960_DD_PILT_NEAR APDS9960_DD_PRX_FAR_THRESHOLD
#define APDS9960_DD_PIHT_NEAR 0xFF
#define APDS9960_DD_PILT_FAR 0
#define APDS9960_DD_PIHT_FAR APDS9960_DD_PRX_NEAR_THRESHOLD

#define APDS9960_DD_ALS_CHANGE_THRESHOLD  (0.2) /* any change of +/- 20% will cause an interrupt */
#define APDS9960_DD_AILT (1 - APDS9960_DD_ALS_CHANGE_THRESHOLD)
#define APDS9960_DD_AIHT (1 + APDS9960_DD_ALS_CHANGE_THRESHOLD)

#define APDS9960_DD_LUX_GA1 (0.634) // for Fluorescent Daylight 6500K, production white LED
#define APDS9960_DD_LUX_GA2 (0.478) // for Incandescent/Halogen 2600K
#define APDS9960_DD_LUX_GA3 (0.738) // Fluorescent Warm White 2700K
#define APDS9960_DD_LUX_GA4 (0.248) // for sunlight light source
#define APDS9960_DD_LUX_GA5 (1.000) // 
#define APDS9960_DD_LUX_GA6 (1.000) // outdoor

#define APDS9960_DD_CCT_GA1 (1.062) // for Fluorescent Daylight 6500K, production whilte LED
#define APDS9960_DD_CCT_GA2 (0.730) // for Incandescent/Halogen 2600K
#define APDS9960_DD_CCT_GA3 (1.000) // Fluorescent Warm White 2700K
#define APDS9960_DD_CCT_GA4 (1.580) // for sunlight light source
#define APDS9960_DD_CCT_GA5 (1.000) // 
#define APDS9960_DD_CCT_GA6	(1.000) // for outdoor

/* convert ADC raw data to irradiance value */
#define APDS9960_DD_IRRADIANCE_C_GA1 (9.622) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_R_GA1 (31.118) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_G_GA1 (27.267) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_B_GA1 (28.138)  // uWcm-2/count/GAIN/ATIME

#define APDS9960_DD_IRRADIANCE_C_GA2 (7.490) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_R_GA2 (9.742)  // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_G_GA2 (23.708)  // uWcm-2/count/GAIN/ATIME
#define APDS9960_DD_IRRADIANCE_B_GA2 (26.629)  // uWcm-2/count/GAIN/ATIME

#define APDS9960_DD_IRRADIANCE_C_GA3 (10.954) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_R_GA3 (21.628) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_G_GA3 (36.812) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_B_GA3 (55.035)  // uWcm-2/count/GAIN/ATIME

#define APDS9960_DD_IRRADIANCE_C_GA4 (8.146) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_R_GA4 (15.237) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_G_GA4 (30.257) // uWcm-2/count/GAIN/ATIME	
#define APDS9960_DD_IRRADIANCE_B_GA4 (44.157)  // uWcm-2/count/GAIN/ATIME

/* Register Addresses define */
#define APDS9960_DD_ENABLE_ADDR 0x80
#define APDS9960_DD_ATIME_ADDR 0x81
#define APDS9960_DD_WTIME_ADDR 0x83
#define APDS9960_DD_AILTL_ADDR 0x84
#define APDS9960_DD_AILTH_ADDR 0x85
#define APDS9960_DD_AIHTL_ADDR 0x86
#define APDS9960_DD_AIHTH_ADDR 0x87
#define APDS9960_DD_PITLO_ADDR 0x89
#define APDS9960_DD_PITHI_ADDR 0x8B
#define APDS9960_DD_PERS_ADDR 0x8C
#define APDS9960_DD_CONFIG_ADDR 0x8D
#define APDS9960_DD_PPULSE_ADDR 0x8E
#define APDS9960_DD_CONTROL_ADDR 0x8F
#define APDS9960_DD_REV_ADDR 0x91
#define APDS9960_DD_ID_ADDR 0x92
#define APDS9960_DD_STATUS_ADDR 0x93
#define APDS9960_DD_CDATAL_ADDR 0x94
#define APDS9960_DD_CDATAH_ADDR 0x95
#define APDS9960_DD_RDATAL_ADDR 0x96
#define APDS9960_DD_RDATAH_ADDR  0x97
#define APDS9960_DD_GDATAL_ADDR 0x98
#define APDS9960_DD_GDATAH_ADDR 0x99
#define APDS9960_DD_BDATAL_ADDR 0x9A
#define APDS9960_DD_BDATAH_ADDR 0x9B
#define APDS9960_DD_PDATA_ADDR 0x9C
#define APDS9960_DD_POFFSET_UR_ADDR 0x9D
#define APDS9960_DD_POFFSET_DL_ADDR 0x9E
#define	APDS9960_DD_CONFIG2_ADDR 0x9F

#define APDS9960_DD_CMD_CLR_PS_INT 0xE5
#define APDS9960_DD_CMD_CLR_ALS_INT 0xE6
#define APDS9960_DD_CMD_CLR_ALL_INT 0xE7

/* Register Value define : ENABLE */
#define APDS9960_DD_PWR_DOWN 0x00    /* PON = 0 */
#define APDS9960_DD_PWR_ON 0x01    /* PON = 1 */
#define APDS9960_DD_ALS_ENABLE 0x02    /* AEN */
#define APDS9960_DD_PRX_ENABLE 0x04    /* PEN */
#define APDS9960_DD_WAIT_ENABLE 0x08    /* WEN */
#define APDS9960_DD_ALS_INT_ENABLE 0x10    /* AIEN */
#define APDS9960_DD_PRX_INT_ENABLE 0x20    /* PIEN */

/* Register Value define : ATIME */
#define APDS9960_DD_100MS_ADC_TIME 0xDB  /* 100.08ms integration time */
#define APDS9960_DD_50MS_ADC_TIME 0xED  /* 50.04ms integration time */
#define APDS9960_DD_27MS_ADC_TIME 0xF6  /* 27.8ms integration time */
#define APDS9960_DD_2_78MS_ADC_TIME  0xFF  /* 2.78ms integration time */

/* Register Value define : PERS */
#define APDS9960_DD_PPERS_0 0x00  /* Every proximity ADC cycle */
#define APDS9960_DD_PPERS_1 0x10  /* 1 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_2 0x20  /* 2 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_3 0x30  /* 3 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_4 0x40  /* 4 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_5 0x50  /* 5 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_6 0x60  /* 6 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_7 0x70  /* 7 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_8 0x80  /* 8 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_9 0x90  /* 9 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_10 0xA0  /* 10 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_11 0xB0  /* 11 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_12 0xC0  /* 12 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_13 0xD0  /* 13 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_14 0xE0  /* 14 consecutive proximity value out of range */
#define APDS9960_DD_PPERS_15 0xF0  /* 15 consecutive proximity value out of range */

#define APDS9960_DD_APERS_0 0x00  /* Every ADC cycle */
#define APDS9960_DD_APERS_1 0x01  /* 1 consecutive als value out of range */
#define APDS9960_DD_APERS_2 0x02  /* 2 consecutive als value out of range */
#define APDS9960_DD_APERS_3 0x03  /* 3 consecutive als value out of range */
#define APDS9960_DD_APERS_5 0x04  /* 5 consecutive als value out of range */
#define APDS9960_DD_APERS_10 0x05  /* 10 consecutive als value out of range */
#define APDS9960_DD_APERS_15 0x06  /* 15 consecutive als value out of range */
#define APDS9960_DD_APERS_20 0x07  /* 20 consecutive als value out of range */
#define APDS9960_DD_APERS_25 0x08  /* 25 consecutive als value out of range */
#define APDS9960_DD_APERS_30 0x09  /* 30 consecutive als value out of range */
#define APDS9960_DD_APERS_35 0x0A  /* 35 consecutive als value out of range */
#define APDS9960_DD_APERS_40 0x0B  /* 40 consecutive als value out of range */
#define APDS9960_DD_APERS_45 0x0C  /* 45 consecutive als value out of range */
#define APDS9960_DD_APERS_50 0x0D  /* 50 consecutive als value out of range */
#define APDS9960_DD_APERS_55 0x0E  /* 55 consecutive als value out of range */
#define APDS9960_DD_APERS_60 0x0F  /* 60 consecutive als value out of range */

/* for PPULSE and GPULSE */
#define	APDS9960_DD_PULSE_LEN_4US 0x00
#define	APDS9960_DD_PULSE_LEN_8US 0x40
#define	APDS9960_DD_PULSE_LEN_16US 0x80
#define	APDS9960_DD_PULSE_LEN_32US 0xC0

/* Register Value define : CONTROL */
#define APDS9960_DD_AGAIN_1X 0x00  /* 1X ALS GAIN */
#define APDS9960_DD_AGAIN_4X 0x01  /* 4X ALS GAIN */
#define APDS9960_DD_AGAIN_16X 0x02  /* 16X ALS GAIN */
#define APDS9960_DD_AGAIN_64X 0x03  /* 64X ALS GAIN */

#define APDS9960_DD_PGAIN_1X 0x00  /* 1X PRX GAIN */
#define APDS9960_DD_PGAIN_2X 0x04  /* 2X PRX GAIN */
#define APDS9960_DD_PGAIN_4X 0x08  /* 4X PRX GAIN */
#define APDS9960_DD_PGAIN_8X 0x0C  /* 8X PRX GAIN */

#define APDS9960_DD_PDRIVE_100MA 0x00  /* PRX 100mA LED drive */
#define APDS9960_DD_PDRIVE_50MA 0x40  /* PRX 50mA LED drive */
#define APDS9960_DD_PDRIVE_25MA 0x80  /* PRX 25mA LED drive */
#define APDS9960_DD_PDRIVE_12_5MA 0xC0  /* PRX 12.5mA LED drive */

/* Register Value define : STATUS */
#define	APDS9960_DD_ASAT_STATUS 0x80 /* ALS Saturation */
#define	APDS9960_DD_PSAT_STATUS 0x40 /* PS Saturation - analog saturated, not a proximity detection */
#define	APDS9960_DD_PINT_STATUS 0x20 /* PS Interrupt status */
#define	APDS9960_DD_AINT_STATUS 0x10 /* ALS Interrupt status */
#define	APDS9960_DD_PVALID_STATUS 0x02 /* PS data valid status */
#define	APDS9960_DD_AVALID_STATUS 0x01 /* ALS data valid status */

/* attributes for rgb data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */
#define APDS9960_DD_CCT_PWR 128 /* unit of uA */ /* [(200uA*(27.8ms/50ms)) + (38uA*(22.2ms/50ms)], wait time is 22.2 ms (20 Hz) */
#define APDS9960_DD_CCT_RES FX_FLTTOFIX_Q16(500) /* unit of this data type is K */
#define APDS9960_DD_CCT_LO_PWR 1   /* unit of uA */
#define APDS9960_DD_CCT_ACCURACY 500 /* unit of this data type is K */

/* attributes for rgb data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */
#define APDS9960_DD_RGB_PWR 128 /* unit of uA */
#define APDS9960_DD_RGB_RES FX_FLTTOFIX_Q16(1)   /* unit of this data type is uW/cm2 */
#define APDS9960_DD_RGB_LO_PWR 1   /* unit of uA */
#define APDS9960_DD_RGB_ACCURACY 1   /* unit of this data is 1 ADC count */

/* attributes for als data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */
#define APDS9960_DD_ALS_PWR 128 /* unit of uA */
#define APDS9960_DD_ALS_RES FX_FLTTOFIX_Q16(1)   /* unit of this data type is lux */
#define APDS9960_DD_ALS_LO_PWR 1       /* unit of uA */
#define APDS9960_DD_ALS_ACCURACY 1 //FX_FLTTOFIX_Q16(1)   /* unit of this data type is lux */

/* attributes for proximity data type */
#define APDS9960_DD_PRX_PWR 100   /* unit of uA */ /* LED average current + IC current , 8-pulse 8us, 50mA, 196ms wait time (5 Hz) */
#define APDS9960_DD_PRX_LO_PWR 1     /* unit of uA */
#define APDS9960_DD_PRX_RES FX_FLTTOFIX_Q16(0.001)   /* unit of this data type is meter */
#define APDS9960_DD_PRX_ACCURACY 1      /* unit of this data type unit which is mm */

/* attribute for NV items */

#define APDS9960_DD_PRX_CAL_THRESHOLD 100

#define APDS9960_DD_PRX_FACTOR 50
#define APDS9960_DD_ALS_FACTOR 100

#define APDS9960_DD_CCT_FACTOR 100

#define APDS9960_DD_PRXDIST_TB_MAX_VALUE 255    /* 8 bits */
#define APDS9960_DD_NUM_SENSOR_TYPES 2
#define APDS9960_DD_MAX_SUB_DEV 2
#define APDS9960_DD_HANDLE_ALIGN 2

#define APDS9960_DD_ALS_CALIBRATED_LUX 1000 /* 300 calibrated lux */
#define APDS9960_DD_ALS_CALIBRATED_CCT 4500 /* 5600K calibrated CCT */

#define	APDS9960_DD_PPULSE_FOR_PS 8
#define APDS9960_DD_PPULSE_LEN_FOR_PS APDS9960_DD_PULSE_LEN_8US
#define APDS9960_DD_PDRIVE_FOR_PS APDS9960_DD_PDRIVE_50MA
#define APDS9960_DD_PGAIN_FOR_PS APDS9960_DD_PGAIN_4X

#define APDS9960_DD_PRX_NEAR_THRESHOLD 100
#define APDS9960_DD_PRX_FAR_THRESHOLD 80

#define APDS9960_DD_PS_CALIBRATED_XTALK_BASELINE 10
#define APDS9960_DD_PS_CALIBRATED_XTALK 20

/*=======================================================================

Macros

========================================================================*/
/* Negative ADC counts will be treated as zero */
#define ALSPRX_CONV_TO_UNSIGNED(var, bits) ((var & (1<<(bits-1))) ? (0) : (var))

#define APDS9960_DD_DEFAULT_ALS_CHANGE_PCNT APDS9960_DD_ALS_CHANGE_0_78_PCNT
#define PCNT_OF_RAW(x, raw) (raw>>x)

/*=======================================================================

TYPE DEFINITIONS

========================================================================*/

typedef enum
{
	APDS9960_DD_ALS_CHANGE_MIN_PCNT = 0,
	APDS9960_DD_ALS_CHANGE_50_PCNT = 1,  /* x>>1 */
	APDS9960_DD_ALS_CHANGE_25_PCNT = 2,  /* x>>2 */
	APDS9960_DD_ALS_CHANGE_12_5_PCNT = 3,  /* x>>3 */
	APDS9960_DD_ALS_CHANGE_6_25_PCNT = 4,  /* x>>4 */
	APDS9960_DD_ALS_CHANGE_3_125_PCNT = 5,  /* x>>5 */
	APDS9960_DD_ALS_CHANGE_1_56_PCNT = 6,  /* x>>6 */
	APDS9960_DD_ALS_CHANGE_0_78_PCNT = 7,  /* x>>7 */
	APDS9960_DD_ALS_CHANGE_MAX_PCNT
} apds9960_dd_als_change_pcnt_e;

typedef enum
{
	APDS9960_DD_RES_14BIT = 0,  /* 27ms integration time */
	APDS9960_DD_RES_15BIT = 1,   /* 50ms integration time */
	APDS9960_DD_RES_16BIT = 2   /* 100ms integration time */
} apds9960_dd_res_e;

typedef enum
{
	APDS9960_DD_ALS_GAIN_1X = 0,    /* 1x AGAIN */
	APDS9960_DD_ALS_GAIN_4X = 1,    /* 4x AGAIN */
	APDS9960_DD_ALS_GAIN_16X = 2,    /* 16x AGAIN */
	APDS9960_DD_ALS_GAIN_64X = 3     /* 64x AGAIN */
} apds9960_dd_als_gain_e;

typedef enum
{
	APDS9960_DD_PEND_STATE_IDLE,                     /* idle state */
	APDS9960_DD_PEND_STATE_PEND,                     /* waiting a response */
	APDS9960_DD_PEND_STATE_RSVD                      /* reserved */
} apds9960_dd_pend_state_e;

typedef enum
{
	APDS9960_DD_PRX_FAR_AWAY,
	APDS9960_DD_PRX_NEAR_BY,
	APDS9960_DD_PRX_NEAR_BY_UNKNOWN
} apds9960_dd_prx_nearby_e;

/* data structure for proximity common handler */

/* The state of a device */
typedef enum
{
	APDS9960_DD_DEV_STOPPED,                         /* 0 */
	APDS9960_DD_DEV_CONFIGURED,                      /* 1 */
	APDS9960_DD_DEV_GET_DATA,                        /* 2 */
	APDS9960_DD_DEV_MAX_STATES                       /* 3 */
}apds9960_dd_device_state_e;

typedef enum
{
	APDS9960_DD_NV_SOURCE_DEFAULT,
	APDS9960_DD_NV_SOURCE_REG
}apds9960_dd_nv_source_e;

/* Error codes indicating reasons for test failures */
typedef enum
{
    APDS9960_DD_SUCCESS = 0,
    APDS9960_DD_PRX_CAL_FAILED = 1,
    APDS9960_DD_ALS_CAL_FAILED = 2,
    APDS9960_DD_RGB_CAL_FAILED = 3,
    APDS9960_DD_CCT_CAL_FAILED = 4
} apds9960_dd_test_err_e;

/* data structure for CCT driver */
typedef struct
{
	bool enable;
	uint32_t last_cct;
	bool timer_active;
	sns_ddf_timer_s timer;      /* for CT_C */
	uint32_t saturated_cct;
	bool calibration;
	bool polling_data;  /* polling to get data for CT_C */
	sns_ddf_sensor_sample_s  sensor_sample[3];
	uint32_t data_cct;
	uint16_t cdata;
	uint16_t rdata;
	uint16_t gdata;
	uint16_t bdata;
	uint32_t rdata_irradiance;  
	uint32_t gdata_irradiance;  
	uint32_t bdata_irradiance;  
	uint32_t cdata_irradiance;  
	uint32_t r_c_ratio;
	uint32_t timer_value;
} apds9960_dd_cct_s;

/* data structure for rgb driver */
typedef struct
{
	bool enable;
	bool timer_active;
	sns_ddf_timer_s timer;      /* for RGB */
	bool calibration;
	bool polling_data;  /* polling to get data for RGB */
	sns_ddf_sensor_sample_s  sensor_sample[3];
	uint16_t cdata;
	uint16_t rdata;
	uint16_t gdata;
	uint16_t bdata;
	uint32_t rdata_irradiance;  
	uint32_t gdata_irradiance;  
	uint32_t bdata_irradiance;  
	uint32_t cdata_irradiance;  
	uint32_t r_c_ratio;
	uint32_t timer_value;
} apds9960_dd_rgb_s;

/* data structure for light(ALS) driver */
typedef struct
{
	bool enable;
	int8_t als_res_index;    /* index for the als res */
	uint8_t als_gain_index;   /* needed for Lux calculation */
	uint16_t als_threshold_l;  /* low threshold */
	uint16_t als_threshold_h;  /* high threshold */
	uint32_t last_mlux;
	bool timer_active;
	sns_ddf_timer_s timer;      /* for AMBIENT */
	uint32_t  saturated_lux;
	bool calibration;
	bool polling_data;  /* polling to get data for AMBIENT */
	sns_ddf_sensor_sample_s  sensor_sample[2];
	uint32_t data_mlux;
	uint16_t cdata;
	uint16_t rdata;
	uint16_t gdata;
	uint16_t bdata;
	uint32_t cdata_irradiance;  
	uint32_t r_c_ratio;
	uint32_t timer_value;
} apds9960_dd_als_s;

/* data structure for proximitydistance driver */
typedef struct
{
	bool  enable;
	uint8_t thresh_near;
	uint8_t thresh_far;
	apds9960_dd_prx_nearby_e detection_state;	
	apds9960_dd_prx_nearby_e last_nearby;
	uint8_t calibration;
	uint8_t calibration_loop;
	uint8_t calibration_pdata;
	bool timer_active;
	sns_ddf_timer_s timer;
	bool polling_data;  /* polling to get data for PROXIMITY */
	bool dri_data;      /* DIR to get data for PROXIMITY */
	sns_ddf_sensor_sample_s  sensor_sample[2];
	uint8_t ppulse;
	uint8_t poffset_ur; /* POFFSET_UR in PS mode */
	uint8_t poffset_dl; /* POFFSET_DL in PS mode */
	uint8_t pdata;
	uint32_t timer_value;
	bool first_int_enable; /* kk 19-May-2015 */
} apds9960_dd_prx_s;

/* data structure for NV items */
typedef struct
{
	uint8_t  visible_ratio;    /* visible light transmission ratio of the glass/pipe in % */
	uint8_t  ir_ratio;         /* IR transmission ratio of light glass/pipe in % */
	uint16_t dc_offset;        /* DC offset */
	uint16_t thresh_near;      /* near detection threshold in ADC count adjusted by DC offset */
	uint16_t thresh_far;       /* far detection threshold in ADC count adjusted by DC offset */
	uint16_t prx_crosstalk;    /* Proximity crosstalk */
	uint16_t als_factor;       /* ALS calibration factor */
	uint32_t version_num;      /* Version of NV Database */
	uint32_t id;               /* Unique identifier for sensor/vendor combination */
	uint32_t als_change_pcnt;  /* ALS change threshold for DRI */
	uint32_t ppulse;
	uint32_t poffset_ur; /* POFFSET_UR in PS mode */
	uint32_t poffset_dl; /* POFFSET_DL in PS mode */
	uint16_t cct_factor;       /* CCT calibration factor */	
	uint32_t rgb_c_factor;	/* Clear irradiance calibration factor */
	uint32_t rgb_r_factor;	/* Red irradiance calibration factor */
	uint32_t rgb_g_factor;	/* Green irradiande calbration factor */
	uint32_t rgb_b_factor;	/* Blue irradiance calibration factor */
} apds9960_dd_nv_s;

/* common data structure belong to all sub modules */
typedef struct
{
	/* timer for reading data from the sensor instead of ISR sig handler */
	apds9960_dd_nv_s  nv_db;                  /* NV item DB type */
	apds9960_dd_nv_source_e  nv_source;
	uint16_t  nv_size;
	sns_ddf_sensor_data_s  sensor_data[APDS9960_DD_NUM_SENSOR_TYPES];
	uint32_t als_odr;
	uint32_t prx_odr;
	uint32_t rgb_odr;
	uint32_t cct_odr;
	uint32_t cycle_time;
	int32_t temp_offset; // for calibration purpose only
    int32_t poffset_ur;
    int32_t poffset_dl;
} apds9960_dd_common_s;

typedef struct
{
	sns_ddf_handle_t  smgr_handle;  /* SDDI handle used to notify_data */
	sns_ddf_powerstate_e  pwr_mode;
	apds9960_dd_device_state_e state;
	bool dri_enabled;
	uint32_t interrupt_gpio;
	bool dri_registration;
	uint32_t  num_sensors;
	sns_ddf_sensor_e  sensors[APDS9960_DD_NUM_SENSOR_TYPES];
}apds9960_dd_sub_dev_s;

/* State struct for APDS99XX */
typedef struct {
	sns_ddf_dev_list_t next;
	sns_ddf_port_id_s port_id;
	uint32_t dropped_logs;
	sns_ddf_handle_t port_handle; /* handle used to access the I2C bus */
	sns_ddf_handle_t ZBW_port_handle; /* handle used to access the I2C bus for ZBW(zero byte writes)*/
	sns_ddf_time_t int_timestamp;
	apds9960_dd_als_s  als;
	apds9960_dd_prx_s  prx;
	apds9960_dd_rgb_s  rgb;
	apds9960_dd_cct_s  cct;
	apds9960_dd_common_s common;
	uint32_t  sub_dev_cnt;
	apds9960_dd_sub_dev_s  sub_dev[APDS9960_DD_MAX_SUB_DEV];
} apds9960_dd_dev_s;

typedef struct {
	sns_ddf_dev_list_t  next;
	sns_ddf_driver_info_s dd_info;
}apds9960_dd_head_s;

sns_ddf_status_e sns_dd_apds9960_init
(
sns_ddf_handle_t*        dd_handle_ptr,
sns_ddf_handle_t         smgr_handle,
sns_ddf_nv_params_s*     nv_params,
sns_ddf_device_access_s  device_info[],
uint32_t                 num_devices,
sns_ddf_memhandler_s*    memhandler,
sns_ddf_sensor_e*        sensors[],
uint32_t*                num_sensors
);

sns_ddf_status_e sns_dd_apds9960_get_data
(
sns_ddf_handle_t        dd_handle,
sns_ddf_sensor_e        sensors[],
uint32_t                num_sensors,
sns_ddf_memhandler_s*   memhandler,
sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

sns_ddf_status_e sns_dd_apds9960_set_attrib
(
sns_ddf_handle_t     dd_handle,
sns_ddf_sensor_e     sensor,
sns_ddf_attribute_e  attrib,
void*                value
);

sns_ddf_status_e sns_dd_apds9960_get_attrib
(
sns_ddf_handle_t     dd_handle,
sns_ddf_sensor_e     sensor,
sns_ddf_attribute_e  attrib,
sns_ddf_memhandler_s* memhandler,
void**               value,
uint32_t*            num_elems
);

void sns_dd_apds9960_handle_timer
(
sns_ddf_handle_t dd_handle,
void* arg
);

void sns_dd_apds9960_handle_irq
(
sns_ddf_handle_t dd_handle,
uint32_t          gpio_num,
sns_ddf_time_t    timestamp
);

sns_ddf_status_e sns_dd_apds9960_reset
(
sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_apds9960_enable_sched_data
(
sns_ddf_handle_t  dd_handle,
sns_ddf_sensor_e  sensor,
bool              enable
);

sns_ddf_status_e sns_dd_apds9960_calibration
(
sns_ddf_handle_t  dd_handle,
sns_ddf_sensor_e  sensor,
sns_ddf_test_e    test,
uint32_t*         err
);

sns_ddf_status_e sns_dd_apds9960_probe
(
sns_ddf_device_access_s* device_info,
sns_ddf_memhandler_s*    memhandler,
uint32_t*                num_sensors,
sns_ddf_sensor_e**       sensors
);



#endif /* End include guard  _SNSDAPDS9960PRIV_H */
