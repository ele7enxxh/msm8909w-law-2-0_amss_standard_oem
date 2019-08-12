/*===========================================================================

         S E N S O R S    AMBIENT LIGHT & PROXIMITY SENSOR    D R I V E R  

DESCRIPTION

  Defines the interface exposed by the Avago ALS and Proximity driver
  APDS-99xx/QPDS-T9x0

********************************************************************************
* Copyright (c) 2012, Avago Technologies.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Avago nor the
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
*******************************************************************************

===========================================================================*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2012 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
  -----------------------------------------------------------------------------*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_apds99xx_priv.h#1 $


when         who     what, where, why
--------     ---     ------------------------------------------------------
14-Jul-2013  lka      Disabled debug messages
12-Jul-2013  lka      Added compile-time configurations
26-Dec-2012   pn      Updated message macros for QDSP6
13-Sep-2012   vh      Eliminated compiler warnings
27-Oct-2011   KK      Avago Technologies, at Singapore, first draft
04-Feb-2012   KK      Avago Technologies, at Taipei, final (integration with Ricky Yuen)
17-Feb-2012   KK      Include support of Q-part number for Nokia (QPDS-T900/T930)  
10-Mar-2012   AG      Enabled DRI 
11-Mar-2012   KK      Fixed APRES definitions
15-Mar-2012   AG      Updated power numbers, added macro to set als thresholds
28-Mar-2012   AG      Updates for proximity calibration
11-May-2012   AG      Incorporate variable als change percentage
==========================================================================*/

#ifndef _SNSDAPDS99XXPRIV_H
#define _SNSDAPDS99XXPRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"
#include "msg_diag_service.h"

/*=======================================================================

                  INTERNAL DEFINITIONS

========================================================================*/
#ifndef NULL
#define NULL  0
#endif

#undef  ALSPRX_EN_DISTANCE
#define ALSPRX_DEBUG_REG
#undef  ALSPRX_DEBUG_TIMER
#undef  ALSPRX_DEBUG_ONE_SENSOR
#undef  ALSPRX_DEBUG_BASIC

#define SNS_DD_ALSPRX_DFLT_MODE               SNS_NORMAL_MODE
#define SNS_DD_ALSPRX_SUPPORTED_MODE          (1 << SNS_NORMAL_MODE | 1 << SNS_SLEEP_MODE)
#define SNS_DD_ALSPRX_DATA_READY_MS           50       /* after issue a command */
#define SNS_DD_ALSPRX_DATA_READY_US           50000  /* after PRX integration */

#define SNS_DD_ALS_DFLT_MILLI_LUX             100000  /* default value for the last sampled mlux */

#define SNS_DD_APDS99XX_ALS_9900_MAX_LUX       100000
#define SNS_DD_APDS99XX_ALS_9930_MAX_LUX       300000

#define SNS_DD_APDS99XX_PRX_PULSE_NUMBER      8
#define SNS_DD_APDS99XX_PRX_NEAR_THRESHOLD    500 /* need to be adjusted after cover glass and black ink */
#define SNS_DD_APDS99XX_PRX_FAR_THRESHOLD     400 /* make 100 less than less but must be higher than crosstalk level */
#define SNS_DD_APDS99XX_PRX_THESHOLD          300

#define SNS_DD_APDS99XX_PRX_CAL_THRESHOLD     720

#define SNS_DD_APDS99XX_PILT_INIT             SNS_DD_APDS99XX_PRX_FAR_THRESHOLD
#define SNS_DD_APDS99XX_PIHT_INIT             SNS_DD_APDS99XX_PRX_NEAR_THRESHOLD
#define SNS_DD_APDS99XX_PILT_NEAR             SNS_DD_APDS99XX_PRX_FAR_THRESHOLD
#define SNS_DD_APDS99XX_PIHT_NEAR             0x3FF
#define SNS_DD_APDS99XX_PILT_FAR              0
#define SNS_DD_APDS99XX_PIHT_FAR              SNS_DD_APDS99XX_PRX_NEAR_THRESHOLD

#define SNS_DD_APDS99XX_ALS_CHANGE_THRESHOLD  0.2 /* any change of +/- 20% will cause an interrupt */
#define SNS_DD_APDS99XX_AILT                  (1 - SNS_DD_APDS99XX_ALS_CHANGE_THRESHOLD)
#define SNS_DD_APDS99XX_AIHT                  (1 + SNS_DD_APDS99XX_ALS_CHANGE_THRESHOLD)

#ifdef ADSP_HWCONFIG_L
#define SNS_DD_APDS99XX_ALS_GA                105
#define SNS_DD_APDS99XX_ALS_COE_B             187
#else
#define SNS_DD_APDS99XX_ALS_GA                48
#define SNS_DD_APDS99XX_ALS_COE_B             223
#endif

#define SNS_DD_APDS99XX_ALS_COE_C             70
#define SNS_DD_APDS99XX_ALS_COE_D             142
#define SNS_DD_APDS99XX_ALS_DF                52

/* Register Addresses define */
#define SNS_DD_APDS99XX_ENABLE_ADDR	0x00
#define SNS_DD_APDS99XX_ATIME_ADDR	0x01
#define SNS_DD_APDS99XX_PTIME_ADDR	0x02
#define SNS_DD_APDS99XX_WTIME_ADDR	0x03
#define SNS_DD_APDS99XX_AILTL_ADDR	0x04
#define SNS_DD_APDS99XX_AILTH_ADDR	0x05
#define SNS_DD_APDS99XX_AIHTL_ADDR	0x06
#define SNS_DD_APDS99XX_AIHTH_ADDR	0x07
#define SNS_DD_APDS99XX_PILTL_ADDR	0x08
#define SNS_DD_APDS99XX_PILTH_ADDR	0x09
#define SNS_DD_APDS99XX_PIHTL_ADDR	0x0A
#define SNS_DD_APDS99XX_PIHTH_ADDR	0x0B
#define SNS_DD_APDS99XX_PERS_ADDR	0x0C
#define SNS_DD_APDS99XX_CONFIG_ADDR	0x0D
#define SNS_DD_APDS99XX_PPCOUNT_ADDR	0x0E
#define SNS_DD_APDS99XX_CONTROL_ADDR	0x0F
#define SNS_DD_APDS99XX_REV_ADDR	0x11
#define SNS_DD_APDS99XX_ID_ADDR		0x12
#define SNS_DD_APDS99XX_STATUS_ADDR	0x13
#define SNS_DD_APDS99XX_CDATAL_ADDR	0x14
#define SNS_DD_APDS99XX_CDATAH_ADDR	0x15
#define SNS_DD_APDS99XX_IRDATAL_ADDR	0x16
#define SNS_DD_APDS99XX_IRDATAH_ADDR	0x17
#define SNS_DD_APDS99XX_PDATAL_ADDR	0x18
#define SNS_DD_APDS99XX_PDATAH_ADDR	0x19

#define SNS_DD_APDS99XX_CMD_BYTE	0x80
#define SNS_DD_APDS99XX_CMD_WORD	0xA0
#define SNS_DD_APDS99XX_CMD_SPECIAL	0xE0

#define SNS_DD_APDS99XX_CMD_CLR_PS_INT	    0xE5
#define SNS_DD_APDS99XX_CMD_CLR_ALS_INT	    0xE6
#define SNS_DD_APDS99XX_CMD_CLR_PS_ALS_INT  0xE7

/* Register Value define : ENABLE */
#define SNS_DD_APDS99XX_PWR_DOWN            0x00    /* PON = 0 */
#define SNS_DD_APDS99XX_PWR_ON              0x01    /* PON = 1 */
#define SNS_DD_APDS99XX_ALS_ENABLE          0x02    /* AEN */
#define SNS_DD_APDS99XX_PRX_ENABLE          0x04    /* PEN */
#define SNS_DD_APDS99XX_WAIT_ENABLE         0x08    /* WEN */
#define SNS_DD_APDS99XX_ALS_INT_ENABLE      0x10    /* AIEN */
#define SNS_DD_APDS99XX_PRX_INT_ENABLE      0x20    /* PIEN */

/* Register Value define : ATIME */
#define SNS_DD_APDS99XX_400MS_ADC_TIME      0x6D  /* 399.84ms integration time */
#define SNS_DD_APDS99XX_100MS_ADC_TIME      0xDB  /* 100.64ms integration time */
#define SNS_DD_APDS99XX_50MS_ADC_TIME       0xED  /* 51.68ms integration time */
#define SNS_DD_APDS99XX_2_72MS_ADC_TIME     0xFF  /* 2.72ms integration time */

/* Register Value define : PRXCNFG for APDS-9930 only */
#define SNS_DD_APDS99XX_ALS_REDUCE          0x04  /* ALSREDUCE - ALS Gain reduced by 6x */

/* Register Value define : PERS */
#define SNS_DD_APDS99XX_PPERS_0             0x00  /* Every proximity ADC cycle */
#define SNS_DD_APDS99XX_PPERS_1             0x10  /* 1 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_2             0x20  /* 2 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_3             0x30  /* 3 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_4             0x40  /* 4 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_5             0x50  /* 5 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_6             0x60  /* 6 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_7             0x70  /* 7 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_8             0x80  /* 8 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_9             0x90  /* 9 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_10            0xA0  /* 10 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_11            0xB0  /* 11 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_12            0xC0  /* 12 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_13            0xD0  /* 13 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_14            0xE0  /* 14 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_PPERS_15            0xF0  /* 15 consecutive proximity value out of range */

#define SNS_DD_APDS99XX_APERS_0             0x00  /* Every ADC cycle */
#define SNS_DD_APDS99XX_APERS_1             0x01  /* 1 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_2             0x02  /* 2 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_3             0x03  /* 3 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_5             0x04  /* 5 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_10            0x05  /* 10 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_15            0x06  /* 15 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_20            0x07  /* 20 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_25            0x08  /* 25 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_30            0x09  /* 30 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_35            0x0A  /* 35 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_40            0x0B  /* 40 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_45            0x0C  /* 45 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_50            0x0D  /* 50 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_55            0x0E  /* 55 consecutive proximity value out of range */
#define SNS_DD_APDS99XX_APERS_60            0x0F  /* 60 consecutive proximity value out of range */

/* Register Value define : CONTROL */
#define SNS_DD_APDS99XX_AGAIN_1X            0x00  /* 1X ALS GAIN */
#define SNS_DD_APDS99XX_AGAIN_8X            0x01  /* 8X ALS GAIN */
#define SNS_DD_APDS99XX_AGAIN_16X           0x02  /* 16X ALS GAIN */
#define SNS_DD_APDS99XX_AGAIN_120X          0x03  /* 120X ALS GAIN */

#define SNS_DD_APDS99XX_PRX_IR_DIOD         0x20  /* Proximity uses CH1 diode */

/* for APDS-9930 only */
#define SNS_DD_APDS99XX_PGAIN_1X            0x00  /* 1X ALS GAIN */
#define SNS_DD_APDS99XX_PGAIN_2X            0x04  /* 2X ALS GAIN */
#define SNS_DD_APDS99XX_PGAIN_4X            0x08  /* 4X ALS GAIN */
#define SNS_DD_APDS99XX_PGAIN_8X            0x0C  /* 8X ALS GAIN */

#define SNS_DD_APDS99XX_PDRVIE_100MA        0x00  /* PRX 100mA LED drive */
#define SNS_DD_APDS99XX_PDRVIE_50MA         0x40  /* PRX 50mA LED drive */
#define SNS_DD_APDS99XX_PDRVIE_25MA         0x80  /* PRX 25mA LED drive */
#define SNS_DD_APDS99XX_PDRVIE_12_5MA       0xC0  /* PRX 12.5mA LED drive */

/* Register Value define : STATUS */
#define SNS_DD_APDS99XX_PINT_STATUS         0x20  /* PRX interrupt status */
#define SNS_DD_APDS99XX_AINT_STATUS         0x10  /* ALS interrupt status */

/* attributes for als data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */ 
#define SNS_DD_ALS_PWR                        175      /* unit of uA */
#define SNS_DD_ALS_RES                        FX_FLTTOFIX_Q16(0.01)   /* unit of this data type is lux and under open air (no cover glass and black ink */
#define SNS_DD_ALS_LO_PWR                     4       /* unit of uA */
#define SNS_DD_ALS_ACCURACY                   FX_FLTTOFIX_Q16(0.01)   /* unit of this data type is lux and under open air (no cover glass and black ink */

/* attributes for proximity data type */
#define SNS_DD_PRX_PWR                      12675   /* unit of uA */ /* LED peak current + ADC current */
#define SNS_DD_PRX_RES                      FX_FLTTOFIX_Q16(0.001)   /* unit of this data type is meter */
#define SNS_DD_PRX_ACCURACY                 1      /* unit of this data type unit which is mm */

/* attribute for NV items */
#define SNS_DD_VISIBLE_TRANS_RATIO            25
#define SNS_DD_IR_TRANS_RATIO                 60
#define SNS_DD_DC_OFFSET                      10      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_NEAR                80      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_FAR                 50      /* unit of ADC count */
#define SNS_DD_PRX_FACTOR                     200
#define SNS_DD_ALS_FACTOR                     100

#define SNS_DD_PRXDIST_TB_MAX_VALUE           1023    /* 10 bits */
#define SNS_DD_APDS99XX_NUM_SENSOR_TYPES        2


#define SNS_DD_ALS_CALIBRATED_LUX             300 /* 300 calibrated lux */

/*=======================================================================

                  Macros

========================================================================*/
/* Negative ADC counts will be treated as zero */
#define ALSPRX_CONV_TO_UNSIGNED(var, bits) ((var & (1<<(bits-1))) ? (0) : (var))

#define SNS_DD_APDS99XX_DEFAULT_ALS_CHANGE_PCNT SNS_DD_ALSPRX_ALS_CHANGE_0_78_PCNT
#define PCNT_OF_RAW(x, raw) (raw>>x)

///#define ALSPRX_DEBUG
#ifdef ALSPRX_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define ALSPRX_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "ALSPRX - " msg)
#define ALSPRX_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "ALSPRX - " msg,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "ALSPRX - " msg,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "ALSPRX - " msg,p1,p2,p3)
#elif ! defined(ADSP_STANDALONE)
#define MED MEDIUM
#include "sns_debug_str.h"
#define ALSPRX_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_DD_ALSPRX_STRING0)
#define ALSPRX_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_DD_ALSPRX_STRING1,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_DD_ALSPRX_STRING2,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_DD_ALSPRX_STRING3,p1,p2,p3)
#else
#define ALSPRX_MSG_0(level,msg)          printf(msg)
#define ALSPRX_MSG_1(level,msg,p1)       printf(msg,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)    printf(msg,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3) printf(msg,p1,p2,p3)
#endif
#else
#define ALSPRX_MSG_0(level,msg)
#define ALSPRX_MSG_1(level,msg,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3)
#endif

/*=======================================================================

                  TYPE DEFINITIONS

========================================================================*/

typedef enum 
{
  SNS_DD_ALSPRX_ALS_CHANGE_MIN_PCNT = 0,
  SNS_DD_ALSPRX_ALS_CHANGE_50_PCNT  = 1,  /* x>>1 */ 
  SNS_DD_ALSPRX_ALS_CHANGE_25_PCNT  = 2,  /* x>>2 */ 
  SNS_DD_ALSPRX_ALS_CHANGE_12_5_PCNT  = 3,  /* x>>3 */ 
  SNS_DD_ALSPRX_ALS_CHANGE_6_25_PCNT  = 4,  /* x>>4 */ 
  SNS_DD_ALSPRX_ALS_CHANGE_3_125_PCNT  = 5,  /* x>>5 */ 
  SNS_DD_ALSPRX_ALS_CHANGE_1_56_PCNT  = 6,  /* x>>6 */ 
  SNS_DD_ALSPRX_ALS_CHANGE_0_78_PCNT  = 7,  /* x>>7 */ 
  SNS_DD_ALSPRX_ALS_CHANGE_MAX_PCNT
} sns_dd_alsprx_als_change_pcnt_e;

typedef enum 
{
  SNS_DD_ALSPRX_RES_14BIT  = 0,  /* 27ms integration time */ 
  SNS_DD_ALSPRX_RES_15BIT = 1,   /* 50ms integration time */
  SNS_DD_ALSPRX_RES_16BIT = 2   /* 100ms integration time */
} sns_dd_alsprx_res_e;

typedef enum 
{
  SNS_DD_ALS_GAIN_1X    = 0,    /* 1x AGAIN */ 
  SNS_DD_ALS_GAIN_8X    = 1,    /* 8x AGAIN */
  SNS_DD_ALS_GAIN_16X   = 2,   /* 16x AGAIN */
  SNS_DD_ALS_GAIN_120X  = 3   /* 120x AGAIN */
} sns_dd_als_gain_e;

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

/* timer object type that is a parameter of sns_dd_start_general_timer/sns_dd_stop_general_timer  */
typedef struct
{   
  bool             active;
} sns_dd_timer_type;

/* data structure for proximity common handler */

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
  SNS_DD_APDS99XX_NV_SOURCE_DEFAULT,
  SNS_DD_APDS99XX_NV_SOURCE_REG
}sns_dd_nv_source;

/* Error codes indicating reasons for test failures */
typedef enum
{
    SNS_DD_APDS99XX_SUCCESS        = 0,
    SNS_DD_APDS99XX_PRX_CAL_FAILED = 1,
    SNS_DD_APDS99XX_ALS_CAL_FAILED = 2
} sns_dd_test_err_e;

/* data structure for light(ALS) driver */
typedef struct
{
  bool      enable;
  uint8_t   als_res_index;    /* index for the als res */
  uint16_t  als_gain_index;   /* needed for Lux calculation */
  uint16_t  als_poll_delay;   /* needed for light sensor polling : micro-second (us) */
  uint16_t  als_atime;	      /* storage for als integratiion time */
  uint16_t  als_threshold_l;  /* low threshold */
  uint16_t  als_threshold_h;  /* high threshold */
  uint32_t  last_get_time;
  uint32_t  last_mlux;
  uint16_t  cdata;
  uint16_t  irdata;
  bool      als_reduce;       /* 6x ALS Gain reduction for APDS-9930 only */
  sns_dd_timer_type         sns_dd_timer;
  sns_ddf_timer_s           timer;
  uint32_t                saturated_lux;
} sns_dd_als_db_type;

/* data structure for proximitydistance driver */
typedef struct
{
  bool                      enable;
  uint16_t                  thresh_near;
  uint16_t                  thresh_far;
  uint16_t                  pdata;			/* to store PS data */
  int32_t                   prxdist_data_cache;
  sns_dd_prx_nearby_type    prx_detection_state;	
  sns_dd_prx_nearby_type    last_nearby;
  sns_dd_timer_type         sns_dd_timer;
  sns_ddf_timer_s           timer;
} sns_dd_prx_db_type;

/* data structure for NV items */
typedef struct
{
  uint8_t  visible_ratio;    /* visible light transmission ratio of the glass/pipe in % */
  uint8_t  ir_ratio;         /* IR transmission ratio of light glass/pipe in % */
  uint16_t dc_offset;        /* DC offset */
  uint16_t thresh_near;      /* near detection threshold in ADC count adjusted by DC offset */
  uint16_t thresh_far;       /* far detection threshold in ADC count adjusted by DC offset */
  uint16_t prx_factor;       /* Proximity crosstalk offset */
  uint16_t als_factor;       /* ALS calibration factor */
  uint32_t version_num;      /* Version of NV Database */
  uint32_t id;               /* Unique identifier for sensor/vendor combination */
  uint32_t als_change_pcnt;  /* ALS change threshold for DRI */

} sns_dd_nv_db_type;

/* common data structure belong to all sub modules */
typedef struct
{   
  /* timer for reading data from the sensor instead of ISR sig handler */
  sns_dd_nv_db_type         nv_db;                  /* NV item DB type */  
  sns_dd_nv_source          nv_source;
  uint16_t                  nv_size;
  sns_ddf_powerstate_e      pwr_mode;  
  sns_dd_device_state_type  state;
  bool                      als_get_data;
  bool                      prx_get_data;  
  uint32_t                  als_odr;
  uint32_t                  prx_odr;
  uint32_t                  als_req_odr;
  uint32_t                  prx_req_odr;
  uint32_t                  cycle_time;
  bool                      prx_calibration;
  bool                      als_calibration;
  uint16_t                  prx_calibration_loop;
  uint16_t                  prx_calibration_pdata;

} sns_dd_apds99xx_common_db_type;

/* State struct for APDS99XX */
typedef struct {
   sns_ddf_handle_t               smgr_handle;    /* SDDI handle used to notify_data */
   uint32_t                       dropped_logs;
   uint8_t                        device_select;  /* 0 = APDS-9900/QPDS-T900, 1 = APDS-9930/QPDS-T930 */
   sns_ddf_handle_t               port_handle; /* handle used to access the I2C bus */
   sns_ddf_handle_t               ZBW_port_handle; /* handle used to access the I2C bus for ZBW(zero byte writes)*/
   sns_dd_als_db_type             sns_dd_als_db;
   sns_dd_prx_db_type             sns_dd_prx_db;
   sns_dd_apds99xx_common_db_type sns_dd_apds99xx_common_db;
   sns_ddf_sensor_sample_s        sensor_sample[2];
   sns_ddf_sensor_data_s          sensor_data;
   uint32_t                       interrupt_gpio;
   bool                           dri_enabled;
   uint32_t                       driver_state; /* TODO - remove */
   sns_ddf_time_t                 int_timestamp;
   uint16_t                       enable_reg_data;
} sns_dd_apds99xx_state_t;

#endif /* End include guard  SNSDALSPRXPRIV_H */
