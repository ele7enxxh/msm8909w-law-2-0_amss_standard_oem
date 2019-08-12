/*===========================================================================

         S E N S O R S    RGB, AMBIENT LIGHT & PROXIMITY SENSOR    D R I V E R  

DESCRIPTION

  Defines the interface exposed by the Avago RGB, ALS and Proximity driver
  APDS-9950


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

==============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------     ---     ------------------------------------------------------ 
30-Sep-2012   KK      (ver 1.00) Avago Technologies, at Singapore, first draft, derived from apds99xx
09-Oct-2012   KK      (ver 1.01) Implement 64-bit int in apds9950_apds9950_rgb_convert_to_mlux() and
                      apds9950_apds9950_rgb_convert_to_cct()
11-Oct-2012   KK      (ver 1.02) Typo error in apds9950_apds9950_async_notify(), use data_val2
18-Jun-2013   KK      (ver 1.03) Merge with latest apds9950_apds99xx.h from Qcom
05-Aug-2013   KK      (ver 1.04) Added APDS9950F_SENSOR_RGB and SSN_DDF_SENSOR_CT_C
14-Aug-2013   KK      (ver 1.05) Implement as Combo driver
21-Aug-2013   KK      (ver 1.06) Correct typo error
27-Aug-2013   MW      (ver 1.07) Use SSN_DDF_SENSOR_CT_C, swap sub samples for
								 SSN_DDF_SENSOR_CT_C type, DRI mode fix for ALS
29-Aug-2013   KK      (ver 1.08) Revise RGB_COE_X,Y and Z as indicated in App Note (Pg 13)
                                 Revise sns_dd_apds9950_rgb_convert_to_mlux as indicate in App Note (Pg 10)
03-Sep-2013   KK      (ver 1.09) Incorporate 2 sub-devices implementation independently
23-Oct-2013   MW      (ver 1.10) Updated prox near and far default thresholds
29-Oct-2013   MW      (ver 1.11) Updated prox near and far default thresholds
26-Oct-2013   KK      (ver 1.12) Add auto gain control into RGB and CT_C sub device;
                                 Correct PROXMITY calibration parameter where PRX_ENA and WAIT_ENA can't be used together;
28-Oct-2013   MW      (ver 1.13) DRI mode fix
30-Oct-2013   KK      (ver 1.14) DO NOT alter ptime register vallue in apds9950_dd_update_intg_time() as it can work with 0xFF only;
                                 Always use the Power-On-Reset value (default).
12-Nov-2013   MW      (ver 1.15) Update prox Y data format to unsigned int
06-Dec-2013   MW      (ver 1.16) CR 582067 - attribute and data log packet fix
02-Jan-2013   KK      (ver 1.17) Convert ADC raw date to irradiance value
20-Jan-2014   MW      (ver 1.18) Fix 20Hz polling timing CR 590296
22-Jan-2014   MW      (ver 1.19) Fixed RGB and CT 0x14c1 log content CR 590470
10-Mar-2014   KK      (ver 1.20) Add RGBC calibration in irradiance value	
13-Mar-2014	  KK      (ver 1.21) Add prx_factor in PRX calibration

==========================================================================*/

#ifndef _SNSDAPDS9950PRIV_H
#define _SNSDAPDS9950PRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"
#ifndef ALSPRX_OEM_BUILD
#include "msg_diag_service.h"
#endif


/*=======================================================================

                  INTERNAL DEFINITIONS

========================================================================*/

// Enable the following macro to view debug messages
//#define APDS9950_DEBUG

#ifdef ALSPRX_OEM_BUILD
#define NULL  0
#define TRUE  true
#define FALSE false
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#endif

#undef  ALSPRX_EN_DISTANCE
#define ALSPRX_DEBUG_REG
#undef  ALSPRX_DEBUG_TIMER
#undef  ALSPRX_DEBUG_ONE_SENSOR
#undef  ALSPRX_DEBUG_BASIC

#define APDS9950_DD_DFLT_MODE               SNS_NORMAL_MODE
#define APDS9950_DD_SUPPORTED_MODE          (1 << SNS_NORMAL_MODE | 1 << SNS_SLEEP_MODE)
#define APDS9950_DD_DATA_READY_MS           50       /* after issue a command */
#define APDS9950_DD_DATA_READY_US           50000  /* after PRX integration */

#define APDS9950_DD_ALS_DFLT_MILLI_LUX      100000  /* default value for the last sampled mlux */

#define APDS9950_DD_ALS_MAX_LUX             10000 

#define APDS9950_DD_PILT_INIT             APDS9950_DD_PRX_FAR_THRESHOLD
#define APDS9950_DD_PIHT_INIT             APDS9950_DD_PRX_NEAR_THRESHOLD
#define APDS9950_DD_PILT_NEAR             APDS9950_DD_PRX_FAR_THRESHOLD
#define APDS9950_DD_PIHT_NEAR             0x3FF
#define APDS9950_DD_PILT_FAR              0
#define APDS9950_DD_PIHT_FAR              APDS9950_DD_PRX_NEAR_THRESHOLD

#define APDS9950_DD_ALS_CHANGE_THRESHOLD  0.2 /* any change of +/- 20% will cause an interrupt */
#define APDS9950_DD_AILT                  (1 - APDS9950_DD_ALS_CHANGE_THRESHOLD)
#define APDS9950_DD_AIHT                  (1 + APDS9950_DD_ALS_CHANGE_THRESHOLD)

#define APDS9950_DD_LUX_GA1           900 // 137  /* no cover glass - for Fluorescent Daylight 6500K */
#define APDS9950_DD_LUX_GA2           1000 // 54   /* no cover glass - for Incandescent 2600K */
#define APDS9950_DD_LUX_GA3           1000 // 138   /* no cover glass - for Fluorescent Warm White 2700K */
#define APDS9950_DD_CCT_GA1           1000 // 909  /* no cover glass - for Fluorescent Daylight 6500K */
#define APDS9950_DD_CCT_GA2           1000 // 381  /* no cover glass - for Incandescent 2600K */
#define APDS9950_DD_CCT_GA3           1000 // 411  /* no cover glass - for Fluorescent Warm White 2700K */

/* Register Addresses define */
#define APDS9950_DD_ENABLE_ADDR	0x00
#define APDS9950_DD_ATIME_ADDR	0x01
#define APDS9950_DD_PTIME_ADDR	0x02
#define APDS9950_DD_WTIME_ADDR	0x03
#define APDS9950_DD_AILTL_ADDR	0x04
#define APDS9950_DD_AILTH_ADDR	0x05
#define APDS9950_DD_AIHTL_ADDR	0x06
#define APDS9950_DD_AIHTH_ADDR	0x07
#define APDS9950_DD_PILTL_ADDR	0x08
#define APDS9950_DD_PILTH_ADDR	0x09
#define APDS9950_DD_PIHTL_ADDR	0x0A
#define APDS9950_DD_PIHTH_ADDR	0x0B
#define APDS9950_DD_PERS_ADDR	0x0C
#define APDS9950_DD_CONFIG_ADDR	0x0D
#define APDS9950_DD_PPCOUNT_ADDR	0x0E
#define APDS9950_DD_CONTROL_ADDR	0x0F
#define APDS9950_DD_REV_ADDR	0x11
#define APDS9950_DD_ID_ADDR		0x12
#define APDS9950_DD_STATUS_ADDR	0x13
#define APDS9950_DD_CDATAL_ADDR	0x14
#define APDS9950_DD_CDATAH_ADDR	0x15
#define APDS9950_DD_RDATAL_ADDR	0x16
#define APDS9950_DD_RDATAH_ADDR	0x17
#define APDS9950_DD_GDATAL_ADDR	0x18
#define APDS9950_DD_GDATAH_ADDR	0x19
#define APDS9950_DD_BDATAL_ADDR	0x1A
#define APDS9950_DD_BDATAH_ADDR	0x1B
#define APDS9950_DD_PDATAL_ADDR	0x1C
#define APDS9950_DD_PDATAH_ADDR	0x1D

#define APDS9950_DD_CMD_BYTE	0x80
#define APDS9950_DD_CMD_WORD	0xA0
#define APDS9950_DD_CMD_SPECIAL	0xE0

#define APDS9950_DD_CMD_CLR_PS_INT	    0xE5
#define APDS9950_DD_CMD_CLR_ALS_INT	    0xE6
#define APDS9950_DD_CMD_CLR_PS_ALS_INT  0xE7

/* Register Value define : ENABLE */
#define APDS9950_DD_PWR_DOWN            0x00    /* PON = 0 */
#define APDS9950_DD_PWR_ON              0x01    /* PON = 1 */
#define APDS9950_DD_ALS_ENABLE          0x02    /* AEN */
#define APDS9950_DD_PRX_ENABLE          0x04    /* PEN */
#define APDS9950_DD_WAIT_ENABLE         0x08    /* WEN */
#define APDS9950_DD_ALS_INT_ENABLE      0x10    /* AIEN */
#define APDS9950_DD_PRX_INT_ENABLE      0x20    /* PIEN */

/* Register Value define : ATIME */
#define APDS9950_DD_100MS_ADC_TIME      0xD6  /* 100.8ms integration time */
#define APDS9950_DD_50MS_ADC_TIME       0xEB  /* 50.4ms integration time */
#define APDS9950_DD_2_4MS_ADC_TIME      0xFF  /* 2.4ms integration time */

/* Register Value define : PERS */
#define APDS9950_DD_PPERS_0             0x00  /* Every proximity ADC cycle */
#define APDS9950_DD_PPERS_1             0x10  /* 1 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_2             0x20  /* 2 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_3             0x30  /* 3 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_4             0x40  /* 4 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_5             0x50  /* 5 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_6             0x60  /* 6 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_7             0x70  /* 7 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_8             0x80  /* 8 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_9             0x90  /* 9 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_10            0xA0  /* 10 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_11            0xB0  /* 11 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_12            0xC0  /* 12 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_13            0xD0  /* 13 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_14            0xE0  /* 14 consecutive proximity value out of range */
#define APDS9950_DD_PPERS_15            0xF0  /* 15 consecutive proximity value out of range */

#define APDS9950_DD_APERS_0             0x00  /* Every ADC cycle */
#define APDS9950_DD_APERS_1             0x01  /* 1 consecutive proximity value out of range */
#define APDS9950_DD_APERS_2             0x02  /* 2 consecutive proximity value out of range */
#define APDS9950_DD_APERS_3             0x03  /* 3 consecutive proximity value out of range */
#define APDS9950_DD_APERS_5             0x04  /* 5 consecutive proximity value out of range */
#define APDS9950_DD_APERS_10            0x05  /* 10 consecutive proximity value out of range */
#define APDS9950_DD_APERS_15            0x06  /* 15 consecutive proximity value out of range */
#define APDS9950_DD_APERS_20            0x07  /* 20 consecutive proximity value out of range */
#define APDS9950_DD_APERS_25            0x08  /* 25 consecutive proximity value out of range */
#define APDS9950_DD_APERS_30            0x09  /* 30 consecutive proximity value out of range */
#define APDS9950_DD_APERS_35            0x0A  /* 35 consecutive proximity value out of range */
#define APDS9950_DD_APERS_40            0x0B  /* 40 consecutive proximity value out of range */
#define APDS9950_DD_APERS_45            0x0C  /* 45 consecutive proximity value out of range */
#define APDS9950_DD_APERS_50            0x0D  /* 50 consecutive proximity value out of range */
#define APDS9950_DD_APERS_55            0x0E  /* 55 consecutive proximity value out of range */
#define APDS9950_DD_APERS_60            0x0F  /* 60 consecutive proximity value out of range */

/* Register Value define : CONTROL */
#define APDS9950_DD_AGAIN_1X            0x00  /* 1X ALS GAIN */
#define APDS9950_DD_AGAIN_4X            0x01  /* 4X ALS GAIN */
#define APDS9950_DD_AGAIN_16X           0x02  /* 16X ALS GAIN */
#define APDS9950_DD_AGAIN_64X           0x03  /* 64X ALS GAIN */

#define APDS9950_DD_PRX_IR_DIOD         0x20  /* Proximity uses CH1 diode */

/* for APDS-9930 only */
#define APDS9950_DD_PGAIN_1X            0x00  /* 1X ALS GAIN */
#define APDS9950_DD_PGAIN_2X            0x04  /* 2X ALS GAIN */
#define APDS9950_DD_PGAIN_4X            0x08  /* 4X ALS GAIN */
#define APDS9950_DD_PGAIN_8X            0x0C  /* 8X ALS GAIN */

#define APDS9950_DD_PDRVIE_100MA        0x00  /* PRX 100mA LED drive */
#define APDS9950_DD_PDRVIE_50MA         0x40  /* PRX 50mA LED drive */
#define APDS9950_DD_PDRVIE_25MA         0x80  /* PRX 25mA LED drive */
#define APDS9950_DD_PDRVIE_12_5MA       0xC0  /* PRX 12.5mA LED drive */

/* Register Value define : STATUS */
#define APDS9950_DD_PINT_STATUS         0x20  /* PRX interrupt status */
#define APDS9950_DD_AINT_STATUS         0x10  /* ALS interrupt status */

/* attributes for rgb data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */ 
#define APDS9950_DD_CCT_PWR             235 /* unit of uA */
#define APDS9950_DD_CCT_RES             FX_FLTTOFIX_Q16(500.0) /* unit of this data type is K */
#define APDS9950_DD_CCT_LO_PWR          3   /* unit of uA */
#define APDS9950_DD_CCT_ACCURACY        500 /* unit of this data type is K */

/* attributes for rgb data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */ 
#define APDS9950_DD_RGB_PWR             235 /* unit of uA */
#define APDS9950_DD_RGB_RES             FX_FLTTOFIX_Q16(0.7)   /* unit of this data type is uW/cm2 - worst case @ 50ms integration time */
#define APDS9950_DD_RGB_LO_PWR          3   /* unit of uA */
#define APDS9950_DD_RGB_ACCURACY        FX_FLTTOFIX_Q16(0.7)   /* unit of this data is uW/cm2 - worst case @ 50ms integration time */

/* attributes for als data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */ 
#define APDS9950_DD_ALS_PWR             235 /* unit of uA */
#define APDS9950_DD_ALS_RES             FX_FLTTOFIX_Q16(0.01)   /* unit of this data type is lux and under open air (no cover glass and black ink */
#define APDS9950_DD_ALS_LO_PWR          3       /* unit of uA */
#define APDS9950_DD_ALS_ACCURACY        FX_FLTTOFIX_Q16(0.01)   /* unit of this data type is lux and under open air (no cover glass and black ink */

/* attributes for proximity data type */
#define APDS9950_DD_PRX_PWR             12675   /* unit of uA */ /* LED peak current + ADC current */
#define APDS9950_DD_PRX_RES             FX_FLTTOFIX_Q16(0.001)   /* unit of this data type is meter */
#define APDS9950_DD_PRX_ACCURACY        1      /* unit of this data type unit which is mm */

/* attribute for NV items */

#define APDS9950_DD_PRX_PULSE_NUMBER      8
#define APDS9950_DD_PRX_NEAR_THRESHOLD    460 /* need to be adjusted after cover glass and black ink */
#define APDS9950_DD_PRX_FAR_THRESHOLD     360 /* make 100 less than less but must be higher than crosstalk level */

#define APDS9950_DD_PRX_CAL_THRESHOLD     720

#define APDS9950_DD_PRX_FACTOR                     200
#define APDS9950_DD_ALS_FACTOR                     100

#define APDS9950_DD_PRXDIST_TB_MAX_VALUE           1023    /* 10 bits */
#define APDS9950_DD_NUM_SENSOR_TYPES               2
#define APDS9950_DD_MAX_SUB_DEV                    2
#define APDS9950_DD_HANDLE_ALIGN                   APDS9950_DD_MAX_SUB_DEV

#define APDS9950_DD_ALS_CALIBRATED_LUX             300 /* 300 calibrated lux */

#define APDS9950_DD_PDRVIE                        APDS9950_DD_PDRVIE_12_5MA
/*=======================================================================

                  Macros

========================================================================*/
/* Negative ADC counts will be treated as zero */
#define ALSPRX_CONV_TO_UNSIGNED(var, bits) ((var & (1<<(bits-1))) ? (0) : (var))

#define APDS9950_DD_DEFAULT_ALS_CHANGE_PCNT APDS9950_DD_ALS_CHANGE_0_78_PCNT
#define PCNT_OF_RAW(x, raw) (raw>>x)

/* convert ADC raw data to irradiance value */
#define APDS9950_DD_IRRADIANCE_C_INCAND 5166 // 5.166297462 ==> uWcm-2/count	
#define APDS9950_DD_IRRADIANCE_R_INCAND 6647  // 6.646547315 ==> uWcm-2/count	
#define APDS9950_DD_IRRADIANCE_G_INCAND 10427  // 10.426511 ==> uWcm-2/count	
#define APDS9950_DD_IRRADIANCE_B_INCAND 11227  // 11.22705658 ==> uWcm-2/count

#define APDS9950_DD_IRRADIANCE_C_NON_INCAND 10442 // 10.44258526 ==> uWcm-2/count	
#define APDS9950_DD_IRRADIANCE_R_NON_INCAND 28294 // 28.29434886 ==> uWcm-2/count	
#define APDS9950_DD_IRRADIANCE_G_NON_INCAND 28268 // 28.26806013 ==> uWcm-2/count	
#define APDS9950_DD_IRRADIANCE_B_NON_INCAND 35298  // 35.29782935 ==> uWcm-2/count

/*=======================================================================

                  TYPE DEFINITIONS

========================================================================*/

typedef enum 
{
  APDS9950_DD_ALS_CHANGE_MIN_PCNT = 0,
  APDS9950_DD_ALS_CHANGE_50_PCNT  = 1,  /* x>>1 */ 
  APDS9950_DD_ALS_CHANGE_25_PCNT  = 2,  /* x>>2 */ 
  APDS9950_DD_ALS_CHANGE_12_5_PCNT  = 3,  /* x>>3 */ 
  APDS9950_DD_ALS_CHANGE_6_25_PCNT  = 4,  /* x>>4 */ 
  APDS9950_DD_ALS_CHANGE_3_125_PCNT  = 5,  /* x>>5 */ 
  APDS9950_DD_ALS_CHANGE_1_56_PCNT  = 6,  /* x>>6 */ 
  APDS9950_DD_ALS_CHANGE_0_78_PCNT  = 7,  /* x>>7 */ 
  APDS9950_DD_ALS_CHANGE_MAX_PCNT
} apds9950_dd_als_change_pcnt_e;

typedef enum 
{
  APDS9950_DD_RES_14BIT  = 0,  /* 27ms integration time */ 
  APDS9950_DD_RES_15BIT = 1,   /* 50ms integration time */
  APDS9950_DD_RES_16BIT = 2   /* 100ms integration time */
} apds9950_dd_res_e;

typedef enum 
{
  APDS9950_DD_ALS_GAIN_1X    = 0,    /* 1x AGAIN */ 
  APDS9950_DD_ALS_GAIN_4X    = 1,    /* 4x AGAIN */
  APDS9950_DD_ALS_GAIN_16X   = 2,    /* 16x AGAIN */
  APDS9950_DD_ALS_GAIN_64X   = 3     /* 64x AGAIN */
} apds9950_dd_als_gain_e;

typedef enum 
{
  APDS9950_DD_PEND_STATE_IDLE,                     /* idle state */
  APDS9950_DD_PEND_STATE_PEND,                     /* waiting a response */
  APDS9950_DD_PEND_STATE_RSVD                      /* reserved */
} apds9950_dd_pend_state_e;

typedef enum 
{
  APDS9950_DD_PRX_FAR_AWAY,
  APDS9950_DD_PRX_NEAR_BY,
  APDS9950_DD_PRX_NEAR_BY_UNKNOWN
} apds9950_dd_prx_nearby_e;

/* data structure for proximity common handler */

/* The state of a device */
typedef enum 
{
  APDS9950_DD_DEV_STOPPED,                         /* 0 */
  APDS9950_DD_DEV_CONFIGURED,                      /* 1 */
  APDS9950_DD_DEV_GET_DATA,                        /* 2 */
  APDS9950_DD_DEV_MAX_STATES                       /* 3 */
}apds9950_dd_device_state_e;

typedef enum
{
  APDS9950_DD_NV_SOURCE_DEFAULT,
  APDS9950_DD_NV_SOURCE_REG
}apds9950_dd_nv_source_e;

/* Error codes indicating reasons for test failures */
typedef enum
{
    APDS9950_DD_SUCCESS        = 0,
    APDS9950_DD_PRX_CAL_FAILED = 1,
    APDS9950_DD_ALS_CAL_FAILED = 2
} apds9950_dd_test_err_e;

/* data structure for CCT driver */
typedef struct
{
  bool      enable;
  uint8_t als_res_index;    /* index for the als res */
  uint16_t als_gain_index;   /* needed for Lux calculation */
  uint16_t als_poll_delay;   /* needed for light sensor polling : micro-second (us) */
  uint16_t als_atime;	      /* storage for als integratiion time */
  uint16_t als_threshold_l;  /* low threshold */
  uint16_t als_threshold_h;  /* high threshold */
  uint32_t last_get_time;
  uint32_t last_cct;
  bool timer_active;
  sns_ddf_timer_s timer;      /* for CT_C */
  uint32_t saturated_cct;
  bool polling_data;  /* polling to get data for CT_C */
  bool dri_data;      /* DIR to get data for CT_C */
  sns_ddf_sensor_sample_s  sensor_sample[3];
  uint32_t data_cct;
  uint16_t cdata;
  uint16_t rdata;
  uint16_t gdata;
  uint16_t bdata;
  uint32_t cdata_irradiance;  
} apds9950_dd_cct_s;

/* data structure for rgb driver */
typedef struct
{
  bool  enable;
  uint8_t als_res_index;    /* index for the als res */
  uint16_t als_gain_index;   /* needed for Lux calculation */
  uint16_t als_poll_delay;   /* needed for light sensor polling : micro-second (us) */
  uint16_t als_atime;	      /* storage for als integratiion time */
  uint16_t als_threshold_l;  /* low threshold */
  uint16_t als_threshold_h;  /* high threshold */
  uint32_t last_get_time;
  bool timer_active;
  sns_ddf_timer_s timer;      /* for RGB */
  bool polling_data;  /* polling to get data for RGB */
  bool dri_data;      /* DIR to get data for RGB */
  sns_ddf_sensor_sample_s  sensor_sample[3];
  uint16_t cdata;
  uint16_t rdata;
  uint16_t gdata;
  uint16_t bdata;
  uint32_t rdata_irradiance;  
  uint32_t gdata_irradiance;  
  uint32_t bdata_irradiance;  
} apds9950_dd_rgb_s;

/* data structure for light(ALS) driver */
typedef struct
{
  bool  enable;
  uint8_t als_res_index;    /* index for the als res */
  uint16_t  als_gain_index;   /* needed for Lux calculation */
  uint16_t  als_poll_delay;   /* needed for light sensor polling : micro-second (us) */
  uint16_t  als_atime;	      /* storage for als integratiion time */
  uint16_t  als_threshold_l;  /* low threshold */
  uint16_t  als_threshold_h;  /* high threshold */
  uint32_t  last_get_time;
  uint32_t  last_mlux;
  bool timer_active;
  sns_ddf_timer_s timer;      /* for AMBIENT */
  uint32_t  saturated_lux;
  bool calibration;
  bool polling_data;  /* polling to get data for AMBIENT */
  bool dri_data;      /* DIR to get data for AMBIENT */
  sns_ddf_sensor_sample_s  sensor_sample[2];
  uint32_t data_mlux;
  uint16_t cdata;
  uint16_t rdata;
  uint16_t gdata;
  uint16_t bdata;
  uint32_t cdata_irradiance;  
} apds9950_dd_als_s;

/* data structure for proximitydistance driver */
typedef struct
{
  bool  enable;
  uint16_t thresh_near;
  uint16_t thresh_far;
  apds9950_dd_prx_nearby_e detection_state;	
  apds9950_dd_prx_nearby_e last_nearby;
  bool  calibration;
  uint16_t  calibration_loop;
  uint16_t  calibration_pdata;  
  bool timer_active;
  sns_ddf_timer_s timer;
  bool polling_data;  /* polling to get data for PROXIMITY */
  bool dri_data;      /* DIR to get data for PROXIMITY */
  sns_ddf_sensor_sample_s  sensor_sample[2];
  uint16_t pdata;
} apds9950_dd_prx_s;

/* data structure for NV items */
typedef struct
{
  uint8_t  visible_ratio;    /* visible light transmission ratio of the glass/pipe in % */
  uint8_t  ir_ratio;         /* IR transmission ratio of light glass/pipe in % */
  uint16_t dc_offset;        /* DC offset */
  uint16_t thresh_near;      /* near detection threshold in ADC count adjusted by DC offset */
  uint16_t thresh_far;       /* far detection threshold in ADC count adjusted by DC offset */
  uint16_t prx_factor;    	/* Proximity crosstalk */
  uint16_t als_factor;       /* ALS calibration factor */
  uint32_t version_num;      /* Version of NV Database */
  uint32_t id;               /* Unique identifier for sensor/vendor combination */
  uint32_t als_change_pcnt;  /* ALS change threshold for DRI */
  uint32_t rgb_c_factor;	/* Clear irradiance calibration factor */
  uint32_t rgb_r_factor;	/* Red irradiance calibration factor */
  uint32_t rgb_g_factor;	/* Green irradiande calbration factor */
  uint32_t rgb_b_factor;	/* Blue irradiance calibration factor */
} apds9950_dd_nv_s;

/* common data structure belong to all sub modules */
typedef struct
{   
  /* timer for reading data from the sensor instead of ISR sig handler */
  apds9950_dd_nv_s  nv_db;                  /* NV item DB type */  
  apds9950_dd_nv_source_e  nv_source;
  uint16_t  nv_size;
  sns_ddf_sensor_data_s  sensor_data[APDS9950_DD_NUM_SENSOR_TYPES];
  uint32_t  als_odr;
  uint32_t  prx_odr;
  uint32_t  rgb_odr;
  uint32_t  cct_odr;
  uint32_t  als_req_odr;
  uint32_t  prx_req_odr;
  uint32_t  rgb_req_odr;
  uint32_t  cct_req_odr;  
  uint32_t  cycle_time;
  sns_ddf_sensor_e	calibrating_sensor;
  uint32_t   calibrating_sub_dev;
} apds9950_dd_common_s;

typedef struct
{
  sns_ddf_handle_t  smgr_handle;  /* SDDI handle used to notify_data */
  sns_ddf_powerstate_e  pwr_mode;
  apds9950_dd_device_state_e state;
  bool dri_enabled;
  uint32_t interrupt_gpio;
  bool dri_registration;
  uint32_t  num_sensors;
  sns_ddf_sensor_e  sensors[APDS9950_DD_NUM_SENSOR_TYPES];
}apds9950_dd_sub_dev_s;

/* State struct for APDS99XX */
typedef struct {
   sns_ddf_dev_list_t next;
   sns_ddf_port_id_s port_id;
   uint32_t dropped_logs;
   sns_ddf_handle_t port_handle; /* handle used to access the I2C bus */
   sns_ddf_handle_t ZBW_port_handle; /* handle used to access the I2C bus for ZBW(zero byte writes)*/
   sns_ddf_time_t int_timestamp;
   apds9950_dd_als_s  als;
   apds9950_dd_prx_s  prx;
   apds9950_dd_rgb_s  rgb;
   apds9950_dd_cct_s  cct;
   apds9950_dd_common_s common;
   uint32_t  sub_dev_cnt;
   apds9950_dd_sub_dev_s  sub_dev[APDS9950_DD_MAX_SUB_DEV];
} apds9950_dd_dev_s;

typedef struct {
  sns_ddf_dev_list_t  next;
  sns_ddf_driver_info_s dd_info;
}apds9950_dd_head_s;

#endif /* End include guard  _SNSDAPDS9950PRIV_H */
