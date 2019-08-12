/********************************************************************************
* Copyright (c) 2013, "ams AG"
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of "ams AG" nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
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
*******************************************************************************/
/*------------------------------------------------------------------------
   Copyright (c) 2013-2014 by Qualcomm Technologies, Inc.  All Rights Reserved
   Qualcomm Technologies Proprietary and Confidential
  ------------------------------------------------------------------------*/

/*===========================================================================

         S E N S O R S    AMBIENT LIGHT & PROXIMITY SENSOR    D R I V E R

DESCRIPTION

  Private header file for TMD277x driver

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
--------     ---     ------------------------------------------------------
12/23/15     rb      Update ALS and PROX power attributes
11/25/15     sc      Update default calibration factors for 8909w
10/08/15     dq      Fix prox can not work on 8996 qrd
02/20/14     MW      Increased ALS max freq
12/17/13     MW      Fix prox in polling mode
12/09/13     MW      Add delay timer for first integ cycle
09/08/13     ag      Fixed issue with zero lux reporting after every reset
06/18/13     rs      Added ALS Threshold to NV Structure, added self-test error codes
05/21/13     rs      Added NV param R/W to registry, fixed Prox DRI startup condition and Prox+ALS DRI stream + updated attributes
04/25/13     ag      Attributes updated
03/21/13     fv      Clean up code, change vendor name to "ams AG", add new copyright text.
11/14/12     fv      Modified refrence driver to use ams/taos sensor
==========================================================================*/

#ifndef _SNSDALSPRXPRIV_H
#define _SNSDALSPRXPRIV_H

#include "fixed_point.h"

#include "sns_ddf_util.h"

//
// AMS-TAOS USA ++++++++++++++++ defs ++++++++++++++++
//

//#define TMD2771_DEBUG

#ifdef TMD2771_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define TMD2771_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO,msg)
#define TMD2771_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO,msg,p1)
#define TMD2771_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO,msg,p1,p2)
#define TMD2771_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO,msg,p1,p2,p3)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define TMD2771_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define TMD2771_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define TMD2771_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define TMD2771_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif
#else
#define TMD2771_MSG_0(level,msg)
#define TMD2771_MSG_1(level,msg,p1)
#define TMD2771_MSG_2(level,msg,p1,p2)
#define TMD2771_MSG_3(level,msg,p1,p2,p3)
#endif

#ifdef __IAR_SYSTEMS_ICC__
// If compiling for the DDK define, else comment out.
#warning Building for the DDK using the IAR compiler.
#define DDK
#endif

#define TMD2771_SENSOR  1
#define TMD2772_SENSOR  2
#define ams_magic_num   50
#define ams_driver_ID   1

// Set to the sensor in use
#define AMS_SENSOR              TMD2772_SENSOR


#include <stdbool.h>

/* These are defined here because all necessary headers were NOT provided
 * for proper porting */
#ifdef DDK
#define BOOLEAN bool
#define TRUE    true
#define FALSE   false

#define SNS_PRINTF_LOW_0()
#define SNS_PRINTF_LOW_1(param1)
#define SNS_PRINTF_LOW_2(param1,param2)
#define SNS_PRINTF_LOW_3(param1,param2,param3)
#define SNS_PRINTF_MEDIUM_0()
#define SNS_PRINTF_MEDIUM_1(param1)
#define SNS_PRINTF_MEDIUM_2(param1,param2)
#define SNS_PRINTF_MEDIUM_3(param1,param2,param3)
#define SNS_PRINTF_HIGH_0()
#define SNS_PRINTF_HIGH_1(param1)
#define SNS_PRINTF_HIGH_2(param1,param2)
#define SNS_PRINTF_HIGH_3(param1,param2,param3)
#define SNS_PRINTF_ERR_0()
#define SNS_PRINTF_ERR_1(param1)
#define SNS_PRINTF_ERR_2(param1,param2)
#define SNS_PRINTF_ERR_3(param1,param2,param3)

#define SNS_PRINTF_STRING_ID_LOW_0(mod_id,str_id)
#define SNS_PRINTF_STRING_ID_LOW_1(mod_id,str_id,param1)
#define SNS_PRINTF_STRING_ID_LOW_2(mod_id,str_id,param1,param2)
#define SNS_PRINTF_STRING_ID_LOW_3(mod_id,str_id,param1,param2,param3)
#define SNS_PRINTF_STRING_ID_MEDIUM_0(mod_id,str_id)
#define SNS_PRINTF_STRING_ID_MEDIUM_1(mod_id,str_id,param1)
#define SNS_PRINTF_STRING_ID_MEDIUM_2(mod_id,str_id,param1,param2)
#define SNS_PRINTF_STRING_ID_MEDIUM_3(mod_id,str_id,param1,param2,param3)
#define SNS_PRINTF_STRING_ID_HIGH_0(mod_id,str_id)
#define SNS_PRINTF_STRING_ID_HIGH_1(mod_id,str_id,param1)
#define SNS_PRINTF_STRING_ID_HIGH_2(mod_id,str_id,param1,param2)
#define SNS_PRINTF_STRING_ID_HIGH_3(mod_id,str_id,param1,param2,param3)
#define SNS_PRINTF_STRING_ID_ERROR_0(mod_id,str_id)
#define SNS_PRINTF_STRING_ID_ERROR_1(mod_id,str_id,param1)
#define SNS_PRINTF_STRING_ID_ERROR_2(mod_id,str_id,param1,param2)
#define SNS_PRINTF_STRING_ID_ERROR_3(mod_id,str_id,param1,param2,param3)

#else

#define SNS_PRINTF_LOW_0()                        SNS_PRINTF_STRING_ID_LOW_0   (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define SNS_PRINTF_LOW_1(param1)                  SNS_PRINTF_STRING_ID_LOW_1   (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,param1)
#define SNS_PRINTF_LOW_2(param1,param2)           SNS_PRINTF_STRING_ID_LOW_2   (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,param1,param2)
#define SNS_PRINTF_LOW_3(param1,param2,param3)    SNS_PRINTF_STRING_ID_LOW_3   (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,param1,param2,param3)
#define SNS_PRINTF_MEDIUM_0()                     SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define SNS_PRINTF_MEDIUM_1(param1)               SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,param1)
#define SNS_PRINTF_MEDIUM_2(param1,param2)        SNS_PRINTF_STRING_ID_MEDIUM_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,param1,param2)
#define SNS_PRINTF_MEDIUM_3(param1,param2,param3) SNS_PRINTF_STRING_ID_MEDIUM_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,param1,param2,param3)
#define SNS_PRINTF_HIGH_0()                       SNS_PRINTF_STRING_ID_HIGH_0  (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define SNS_PRINTF_HIGH_1(param1)                 SNS_PRINTF_STRING_ID_HIGH_1  (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,param1)
#define SNS_PRINTF_HIGH_2(param1,param2)          SNS_PRINTF_STRING_ID_HIGH_2  (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,param1,param2)
#define SNS_PRINTF_HIGH_3(param1,param2,param3)   SNS_PRINTF_STRING_ID_HIGH_3  (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,param1,param2,param3)
#define SNS_PRINTF_ERR_0()                        SNS_PRINTF_STRING_ID_ERROR_0 (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define SNS_PRINTF_ERR_1(param1)                  SNS_PRINTF_STRING_ID_ERROR_1 (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,param1)
#define SNS_PRINTF_ERR_2(param1,param2)           SNS_PRINTF_STRING_ID_ERROR_2 (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,param1,param2)
#define SNS_PRINTF_ERR_3(param1,param2,param3)    SNS_PRINTF_STRING_ID_ERROR_3 (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,param1,param2,param3)

/* #define SNS_PRINTF_LOW_0(str_id)                         SNS_PRINTF_STRING_ID_LOW_0   (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0) */
/* #define SNS_PRINTF_LOW_1(str_id,param1)                  SNS_PRINTF_STRING_ID_LOW_1   (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,param1) */
/* #define SNS_PRINTF_LOW_2(str_id,param1,param2)           SNS_PRINTF_STRING_ID_LOW_2   (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,param1,param2) */
/* #define SNS_PRINTF_LOW_3(str_id,param1,param2,param3)    SNS_PRINTF_STRING_ID_LOW_3   (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,param1,param2,param3) */
/* #define SNS_PRINTF_MEDIUM_0(str_id)                      SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0) */
/* #define SNS_PRINTF_MEDIUM_1(str_id,param1)               SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,param1) */
/* #define SNS_PRINTF_MEDIUM_2(str_id,param1,param2)        SNS_PRINTF_STRING_ID_MEDIUM_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,param1,param2) */
/* #define SNS_PRINTF_MEDIUM_3(str_id,param1,param2,param3) SNS_PRINTF_STRING_ID_MEDIUM_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,param1,param2,param3) */
/* #define SNS_PRINTF_HIGH_0(str_id)                        SNS_PRINTF_STRING_ID_HIGH_0  (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0) */
/* #define SNS_PRINTF_HIGH_1(str_id,param1)                 SNS_PRINTF_STRING_ID_HIGH_1  (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,param1) */
/* #define SNS_PRINTF_HIGH_2(str_id,param1,param2)          SNS_PRINTF_STRING_ID_HIGH_2  (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,param1,param2) */
/* #define SNS_PRINTF_HIGH_3(str_id,param1,param2,param3)   SNS_PRINTF_STRING_ID_HIGH_3  (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,param1,param2,param3) */
/* #define SNS_PRINTF_ERR_0(str_id)                         SNS_PRINTF_STRING_ID_ERROR_0 (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0) */
/* #define SNS_PRINTF_ERR_1(str_id,param1)                  SNS_PRINTF_STRING_ID_ERROR_1 (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,param1) */
/* #define SNS_PRINTF_ERR_2(str_id,param1,param2)           SNS_PRINTF_STRING_ID_ERROR_2 (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,param1,param2) */
/* #define SNS_PRINTF_ERR_3(str_id,param1,param2,param3)    SNS_PRINTF_STRING_ID_ERROR_3 (SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,param1,param2,param3) */

#endif

/*
 * TAOS definitions and structures
 */


/* 2772 register offsets */
#define TMD277x_MAX_DEVICE_REGS 32
#define TMD277x_RW_REG_MAX      16
#define TMD277x_RO_REG_MAX       4

/* Device Registers and Masks */
#define TMD277x_CNTRL           0x00
#define TMD277x_ALS_TIME        0x01

#define TMD277x_ALS_MINTHRESHLO 0x04
#define TMD277x_ALS_MINTHRESHHI 0x05
#define TMD277x_ALS_MAXTHRESHLO 0x06
#define TMD277x_ALS_MAXTHRESHHI 0x07

#define TMD277x_PRX_MINTHRESHLO 0x08
#define TMD277x_PRX_MINTHRESHHI 0x09
#define TMD277x_PRX_MAXTHRESHLO 0x0A
#define TMD277x_PRX_MAXTHRESHHI 0x0B

#define TMD277x_PERSISTENCE     0x0c
#define TMD277x_PRX_COUNT       0x0E
#define TMD277x_GAIN            0x0f
#define TMD277x_STATUS          0x13
#define TMD277x_REVID           0x11
#define TMD277x_CHIPID          0x12
#define TMD277x_ALS_CHAN0LO     0x14
#define TMD277x_ALS_CHAN0HI     0x15
#define TMD277x_ALS_CHAN1LO     0x16
#define TMD277x_ALS_CHAN1HI     0x17
#define TMD277x_PRX_LO          0x18
#define TMD277x_PRX_HI          0x19

/*
 * Chip Ids for 2771x and 2772x
 */
#define DEVICEID_TMD27711 0x20
#define DEVICEID_TMD27713 0x29
#define DEVICEID_TMD27721 0x30
#define DEVICEID_TMD27723 0x39

// Register names
#define TMD277x_rENABLE                 0x00
#define TMD277x_rATIME                  0x01
#define TMD277x_rPTIME                  0x02
#define TMD277x_rWTIME                  0x03
#define TMD277x_rAILTL                  0x04
#define TMD277x_rAILTH                  0x05
#define TMD277x_rAIHTL                  0x06
#define TMD277x_rAIHTH                  0x07
#define TMD277x_rPILTL                  0x08
#define TMD277x_rPILTH                  0x09
#define TMD277x_rPIHTL                  0x0A
#define TMD277x_rPIHTH                  0x0B
#define TMD277x_rPERS                   0x0C
#define TMD277x_rCONFIG                 0x0D
#define TMD277x_rPPULSE                 0x0E
#define TMD277x_rCONTROL                0x0F
#define TMD277x_rID                     0x12
#define TMD277x_rSTATUS                 0x13
#define TMD277x_rC0DATA                 0x14
#define TMD277x_rC0DATAH                0x15
#define TMD277x_rC1DATA                 0x16
#define TMD277x_rC1DATAH                0x17
#define TMD277x_rPDATA                  0x18
#define TMD277x_rPDATAH                 0x19

// Enable Register Masks
#define TMD277x_ENABLE_MSK_PON          0x01
#define TMD277x_ENABLE_MSK_AEN          0x02
#define TMD277x_ENABLE_MSK_PEN          0x04
#define TMD277x_ENABLE_MSK_WEN          0x08
#define TMD277x_ENABLE_MSK_AIEN         0x10
#define TMD277x_ENABLE_MSK_PIEN         0x20
#define TMD277x_ENABLE_MSK_SAI          0x40

/* tsl2772 cmd reg masks */
#define TMD277x_CMD_REG         0x80
#define TMD277x_CMD_SPL_FN      0x60
#define TMD277x_CMD_AUTO_INC    0x20
#define TMD277x_CMD_ALS_INT_CLR 0x06

/* tsl2772 cntrl reg masks */
#define TMD277x_CNTL_ADC_ENBL   0x02
#define TMD277x_CNTL_PWR_ON     0x01

/* tsl2772 status reg masks */
#define TMD277x_STA_ADC_VALID   0x01
#define TMD277x_STA_ALS_INTR    0x10
#define TMD277x_STA_PRX_INTR    0x20

/* tmd277x config reg masks */
#define TMD277X_CONFIG_AGL_MSK   0x04
#define TMD277X_CONFIG_WLONG_MSK 0x02
#define TMD277X_CONFIG_PDL_MSK   0x01

/* tsl2772 cntrl reg masks */
#define CNTL_REG_CLEAR          0x00
#define CNTL_PROX_INT_ENBL      0x20
#define CNTL_ALS_INT_ENBL       0x10
#define TMD277x_CNTL_WAIT_TMR_ENBL  0x08
#define CNTL_PROX_DET_ENBL      0x04
#define CNTL_ADC_ENBL           0x02
#define TMD277x_CNTL_PWRON      0x01
#define CNTL_ALSPON_ENBL        0x03
#define CNTL_INTALSPON_ENBL     0x13
#define CNTL_PROXPON_ENBL       0x0F
#define CNTL_INTPROXPON_ENBL    0x2F
#define TMD277x_CMD_PROX_INTCLR 0x05
#define TMD277x_CMD_ALS_INTCLR  0x06
#define TMD277x_CMD_PROXALS_INTCLR  0x07

/* Lux calculation constants */
#define TMD277x_LUX_CALC_OVER_FLOW  65535

// TSL/TMD 2771 CONTROL REGISTER
#define TSL2771_PDRIVE_100mA    0x00
#define TSL2771_PDRIVE_50mA     0x40
#define TSL2771_PDRIVE_25mA     0x80
#define TSL2771_PDRIVE_12p5mA   0xC0

#define TMD2771_PDRIVE_100pc    0x00
#define TMD2771_PDRIVE_50pc     0x40
#define TMD2771_PDRIVE_25pc     0x80
#define TMD2771_PDRIVE_12p5pc   0xC0

#define TSL2771_PDIODE_RES      0x00
#define TSL2771_PDIODE_CH0      0x10
#define TSL2771_PDIODE_CH1      0x20
#define TSL2771_PDIODE_CH0CH1   0x30

#define TMD2771_PDIODE_RES      0x00
#define TMD2771_PDIODE_CH0      0x10
#define TMD2771_PDIODE_CH1      0x20
#define TMD2771_PDIODE_CH0CH1   0x30

#define TSL2771_AGAIN_1x        0x00
#define TSL2771_AGAIN_8x        0x01
#define TSL2771_AGAIN_16x       0x02
#define TSL2771_AGAIN_120x      0x03

#define TMD2771_AGAIN_1x        0x00
#define TMD2771_AGAIN_8x        0x01
#define TMD2771_AGAIN_16x       0x02
#define TMD2771_AGAIN_120x      0x03

// TSL/TMD 2772 CONTROL REGISTER
#define TSL2772_PDRIVE_120mA    0x00
#define TSL2772_PDRIVE_60mA     0x40
#define TSL2772_PDRIVE_30mA     0x80
#define TSL2772_PDRIVE_15mA     0xC0

#define TMD2772_PDRIVE_100mA    0x00
#define TMD2772_PDRIVE_50mA     0x40
#define TMD2772_PDRIVE_25mA     0x80
#define TMD2772_PDRIVE_12p5mA   0xC0

#define TMD2772_PDIODE_NONE     0x00
#define TMD2772_PDIODE_CH0      0x10
#define TMD2772_PDIODE_CH1      0x20
#define TMD2772_PDIODE_RES      0x30

#define TMD2772_PGAIN_1x        0x00
#define TMD2772_PGAIN_2x        0x04
#define TMD2772_PGAIN_4x        0x08
#define TMD2772_PGAIN_8x        0x0C

#define TMD2772_AGAIN_1x        0x00
#define TMD2772_AGAIN_8x        0x01
#define TMD2772_AGAIN_16x       0x02
#define TMD2772_AGAIN_120x      0x03




/*Prox diode to use */
#define DIODE0              0x10
#define DIODE1              0x20
#define DIODE_BOTH          0x30

/* LED Power */
#define mA100               0x00
#define mA50                0x40
#define mA25                0x80
#define mA13                0xC0

/* Cal defs*/
#define PROX_STAT_CAL   0
#define PROX_STAT_SAMP  1
#define MAX_SAMPLES_CAL 200

/* TSL2772 Device ID */
#define SWORDFISH_ID    0x30

typedef enum {
    TSL2571,
    TSL2671,
    TSL2771,

    TMD2671,
    TMD2771,

    TSL2572,
    TSL2672,
    TSL2772,

    TMD2672,
    TMD2772
} TMD277x_NAME_DEF;


typedef enum {
    IRQ_TYPE,
    TIMER_TYPE
} TMD277x_SIG_TYPE;


typedef union {
    uint16_t   word;
    uint8_t    byte[2];
} uword_u;

/* Per-device data */
typedef struct  {
    uint16_t ch0;
    uint16_t ch1;
    uint32_t lux;
    uint32_t Lux_Extended;
    bool     Lux_OK;
    bool     reset;
    uint32_t luxAve;
    int32_t  Last_C1C0_Ratio1000;
} ams_als_info;

typedef struct {
     int32_t DGF;
     int32_t iCoefB;
     int32_t iCoefC;
     int32_t iCoefD;
 } ams_als_lux_coef;

/* proximity data */
typedef struct  {
    uint16_t data;
    int16_t  event;
} ams_prox_info;

/* struct prox_stat { */
/*     uint16_t min; */
/*     uint16_t max; */
/*     uint16_t mean; */
/*     unsigned long stddev; */
/* } ; */

typedef struct {
    uint32_t atime_us;               // in uSec
    uint8_t  again;
    uint16_t gain_trim;
    uint16_t cal_target;
    uint16_t thresh_low;
    uint16_t thresh_high;
    uint32_t odr;
    bool     enabled;
    uint32_t luxCorrection;
	
} ams_als_setup;


typedef struct {
    uint8_t  pgain;
    uint32_t ptime_us;               // in uSec
    uint16_t pulse_count;
    uint16_t max_samples_cal;   /* for calibration mode*/
    /* uint16_t thres; */
    /* uint16_t mul; */
    uint16_t detect;
    uint16_t release;
    /*uint16_t mul;*/
    uint16_t thresh_low;
    uint16_t thresh_high;
    uint32_t odr;
    bool     enabled;
} ams_prox_setup;


typedef struct {
    uint32_t wtime_us;      // in uSec
    uint8_t  wtime;
    uint8_t  lTime;
    uint8_t  wen;
    bool     lTimeChanged;
} tmd277x_wait;


typedef struct {
    uint8_t  enable;
    uint8_t  config;
    uint8_t  control;
} tmd277x_reg;

typedef struct  {
    ams_als_setup   als;
    ams_prox_setup  prox;

    uint8_t   interrupts_en;
    uint8_t   persistence;
    uint32_t  Min_Integ_Time_us;
    uint32_t  wait_time_us;

    tmd277x_reg  reg;
    tmd277x_wait wait;
} ams_setup;


/* static const uint8_t persistence_available[] = { */
/*     0, 1, 2, 3, 5, 10, 15, 20, */
/*     25, 30, 35, 40, 45, 50, 55, 60 */
/* }; */

typedef struct {
    ams_als_info      als;
    ams_prox_info     prox;
} ams_cur_info;

typedef struct {
    TMD277x_NAME_DEF   ams_sensor;  // Device type
    ams_cur_info       data;         // Current data
    ams_als_lux_coef   als_setup;
    ams_setup          setup;

    sns_ddf_sensor_e   slow_sensor;
    uint16_t           cycles;
    uint16_t           cycleCnt;
} tsl277X_chip;




#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

//
// AMS-TAOS USA ---------------- defs ----------------
//

/*=======================================================================

                  INTERNAL DEFINITIONS

========================================================================*/
#define ALSPRX_USE_POLLING             /* Polling is used instead of interrupt */
#undef  ALSPRX_EN_DISTANCE
#define ALSPRX_DEBUG_REG
#undef ALSPRX_DEBUG_TIMER
#undef ALSPRX_DEBUG_IRQ
#undef  ALSPRX_DEBUG_ONE_SENSOR
#undef ALSPRX_DEBUG_BASIC

#define SNS_DD_ALSPRX_ALS_DATA_READY_US       115000  /* 115ms for ALS */
#define SNS_DD_ALSPRX_PRX_DATA_READY_US       540     /* 0.54ms for PRX */
#define SNS_DD_ALSPRX_ALPHA_LO                326
#define SNS_DD_ALSPRX_ALPHA_HI                5220

#define SNS_DD_ALS_ACCURACY_DIV_FACTOR        4       /* 4 means 1/4 or 25% */
#define SNS_DD_ALS_HALF_LUX                   500

#define SNS_DD_ALS_MAX_MILLI_LUX              10000000 /* Actual value for TMD2771 is 9035, but using a nice round number */
#define SNS_DD_ALS_DFLT_MILLI_LUX             100000  /* default value for the last sampled mlux */
#define SNS_DD_ALS_LONG_TIME_AGO_US           5000000 /* time for ignoring the previous data - 5 sec */

#define SNS_DD_ISL_ALSPRX_ALS_RANGE_THRESH    58000   /* 58 lux - based on information in datasheet
                                                         1800*0.0326=58.68 */
 
#define SNS_DD_ISL_ALSPRX_PWR_DOWN            0x00    /* Power down the device */
#define SNS_DD_ISL_ALSPRX_CONFIG_REG          0x01

/* Error codes indicating reasons for test failures */
typedef enum
{
    SNS_DD_TMD277X_SUCCESS = 0,
    SNS_DD_TMD277X_ALS_CAL_FAILED_LUX = 1, //ALS CAL error - Received lux value out of range
    SNS_DD_TMD277X_ALS_CAL_FAILED_RST = 2, //ALS CAL error - cannot reset the device
    SNS_DD_TMD277X_ALS_CAL_FAILED_REG = 3, //ALS CAL error - cannot write back to registry
    SNS_DD_TMD277X_ALS_CAL_FAILED_ENB = 4  //ALS CAL error - cannot enable (error in set power state)
} sns_dd_test_err_e;

/* attributes for light data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */
#define SNS_DD_ALS_PWR                        195      /* unit of uA */
#define SNS_DD_ALS_RES                        FX_FLTTOFIX_Q16(0.01)   /* unit of this data type is lux */
#define SNS_DD_ALSPRX_LO_PWR                  1       /* unit of uA */



/* attributes for proximity data type */
#define SNS_DD_PRX_PWR                        53000  /* unit of uA */  
#define SNS_DD_PRX_RES                        FX_FLTTOFIX_Q16(0.05)   /* unit of this data type is meter */
#define SNS_DD_PRX_ACCURACY                   40      /* unit of this data type unit which is mm */

/* attribute for NV items */
#define SNS_DD_VISIBLE_TRANS_RATIO            25
#define SNS_DD_IR_TRANS_RATIO                 60
#define SNS_DD_DC_OFFSET                      10      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_NEAR                80      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_FAR                 50      /* unit of ADC count */
#define SNS_DD_PRX_FACTOR                    200
#define SNS_DD_ALS_FACTOR                     88

#define SNS_DD_PRXDIST_TB_MAX_VALUE           2047    /* 11 bits + 1 sign bit */
#define SNS_DD_ALSPRX_NUM_SENSOR_TYPES        2
#define SNS_DD_ALSPRX_PRX_BITS                16  //set to 16 bits frm 8
#define SNS_DD_ALSPRX_PRX_FREQ                30
#define SNS_DD_ALSPRX_ALS_BITS                16
#define SNS_DD_ALSPRX_ALS_FREQ                25

/*=======================================================================

                  Macros

========================================================================*/
/* Negative ADC counts will be treated as zero */
#define ALSPRX_CONV_TO_UNSIGNED(var, bits) ((var & (1<<(bits-1))) ? (0) : (var))

/*=======================================================================

                  TYPE DEFINITIONS

========================================================================*/
typedef enum
{
  SNS_DD_PEND_STATE_IDLE,                     /* idle state */
  SNS_DD_PEND_STATE_PEND,                     /* waiting a response */
  SNS_DD_PEND_STATE_RSVD                      /* reserved */
} sns_dd_pend_state_type;

typedef enum
{
  SNS_PRX_FAR_AWAY,
  SNS_PRX_NEAR_BY,
  SNS_PRX_NEAR_BY_UNKNOWN
} sns_dd_prx_nearby_type;

/* The state of a device */
typedef  enum
{
  SNS_DD_DEV_STOPPED,                         /* 0 */
  SNS_DD_DEV_CONFIGURED,                      /* 1 */
  SNS_DD_DEV_GET_DATA,                        /* 2 */
  SNS_DD_DEV_MAX_STATES                       /* 3 */
}sns_dd_device_state_type;

typedef enum
{
  SNS_DD_ALSPRX_NV_SOURCE_DEFAULT,
  SNS_DD_ALSPRX_NV_SOURCE_REG
}sns_dd_nv_source;

#ifdef ALSPRX_USE_POLLING
/* timer object type that is a parameter of sns_dd_start_general_timer/sns_dd_stop_general_timer  */
typedef struct
{
  BOOLEAN             active;
  BOOLEAN             defined;
  sns_ddf_timer_s     timer_obj;
} sns_dd_timer_type;
#endif /* ALSPRX_USE_POLLING */

/* data structure for light(ALS) driver */
typedef struct
{
  int32_t                   als_data_cache;
  sns_dd_pend_state_type    pend_state;           /* idle or wait state for the response */
  uint8_t                   next_rng_sel;         /* range value that should be used for next sampling */
  uint8_t                   req_rng_sel;          /* Configuratin during the adaptive range selection */
  uint32_t                  last_get_time;
  uint32_t                  last_mlux;
  uint16_t                  accuracy;
  uint32_t                  als_conversion_lo;
  uint32_t                  als_conversion_hi;
  uint32_t                  als_req_cnt;      /* for history */
} sns_dd_als_db_type;

/* data structure for proximitydistance driver */
typedef struct
{
  int32_t                   prxdist_data_cache;
  sns_dd_pend_state_type    pend_state;                /* idle of wait the response */
  int32_t                   prx_data_cache;
  sns_dd_prx_nearby_type    last_nearby;
  uint16_t                  accuracy;
  uint32_t                  prx_req_cnt;      /* for history */
} sns_dd_prx_db_type;

/* data structure for NV items */
typedef struct
{
  uint8_t  visible_ratio;   /* visible light transmission ratio of the glass/pipe in % */
  uint8_t  ir_ratio;        /* IR transmission ratio of light glass/pipe in % */
  uint16_t dc_offset;       /* DC offset in ADC count */
  uint16_t thresh_near;     /* near detection threshold in ADC count adjusted by DC offset */
  uint16_t thresh_far;      /* far detection threshold in ADC count adjusted by DC offset */
  uint16_t prx_factor;      /* PRX multiplicative factor in % */
  uint16_t als_factor;      /* ALS multiplicative factor in % */
  uint32_t ams_num;         /* unique number for AMS */
  uint32_t drv_ID;          /* identify specific driver */
  uint32_t calibratePhone;  /* if true, calibrate phone (should only have to done during manufacture) */
  uint32_t device;          /* current sensor */
  uint32_t calLuxLower;     /* ALS value of calibration light lower */
  uint32_t calLuxUpper;     /* ALS value of calibration light upper */
  uint32_t DGF;             /* ALS coeficients to use for current sensor */
  uint32_t iCoefB;          /* ALS coeficients to use for current sensor */
  uint32_t iCoefC;          /* ALS coeficients to use for current sensor */
  uint32_t iCoefD;          /* ALS coeficients to use for current sensor */
  uint32_t proxDetect;      /* Proximity Detect count */
  uint32_t proxRelease;     /* Proximity Release count */
  uint32_t luxCorrection;   /* Lux correction factor */
  uint32_t ALS_thresh_high; /* ALS Interrupt Threshold High in % */
  uint32_t ALS_thresh_low;   /* ALS Interrupt Threshold Low in % */
} sns_dd_nv_db_type;

typedef struct
{
  /* timer for reading data from the sensor instead of ISR sig handler */
  sns_dd_timer_type         sns_dd_timer;
  sns_dd_timer_type         self_sched_data;
  sns_dd_timer_type         ddk_timer;
  sns_dd_nv_db_type         nv_db;                  /* NV item DB type */
  sns_dd_nv_source          nv_source;
  sns_ddf_powerstate_e      pwr_mode;
  sns_dd_device_state_type  state;
} sns_dd_alsprx_common_db_type;

/* State struct for ALS/PRX */
typedef struct {
   sns_ddf_handle_t             smgr_handle;    /* SDDI handle used to notify_data */
   sns_ddf_handle_t             port_handle;    /* handle used to access the I2C bus */
   uint32_t                     dropped_logs;
   sns_dd_als_db_type           sns_dd_als_db;
   sns_dd_prx_db_type           sns_dd_prx_db;
   sns_dd_alsprx_common_db_type sns_dd_alsprx_common_db;
   sns_ddf_timer_s              timer;
   uint8_t                      num_requests;
   bool                         two_requests;
   bool                         calibration_mode;    /* calibration mode enable flag */
   uint16_t                     nv_db_size;   /* size of NV item */
   tsl277X_chip                 chip;
   sns_ddf_handle_t             ZBW_port_handle;
   uint32_t                     int_gpio;
   bool                         first_intg_cycle_complete;
   sns_ddf_timer_s              first_integ_cycle_timer;
   uint32_t                     first_integ_cycle_timer_arg;
   bool                         first_als_indication;
   bool                         first_prox_indication;
} sns_dd_alsprx_state_t;


#endif /* End include guard  SNSDALSPRXPRIV_H */
