/*===========================================================================

         S E N S O R S    AMBIENT LIGHT & PROXIMITY SENSOR    D R I V E R  

DESCRIPTION

  Defines the interface exposed by the qcom accelerometer driver 


Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_priv.h#1 $


when         who     what, where, why
--------     ---     ------------------------------------------------------ 
10/06/10     ag      Initial port of alsprx driver
10/14/10     ag      Basic sanity testing done on PCSIM 
10/15/10     ag      Ambient light tested standalone without smgr 
10/19/10     ag      PRX and simultaneous requests tested standalone without smgr 
10/26/10     ag      Added debug string support 
11/12/10     ag      Remove unused variables, add support for combined response 
11/23/10     ag      Remove unused variables 
01/20/11     br      Changed/inserted definitions for power and resolution. 
02/24/11     ag      Added support to handle negative ADC values for prx 
03/25/11     ag      - featurize distance calculation 
                     - add registry support 
                     - fix ALS stale data timeout check
04/12/11    ag       - fix issue with sat_unsigned macro   
                     - make als_factor and prx_factor 16-bits and in units of %
04/18/11    ag       fix truncation error in als calculations 
05/19/11    ag       Updates for new SMGR scheduler  
06/14/11    dc       undefined ALSPRX_DEBUG_BASIC since it is being stripped
04/02/12    ag       DDF framework updates - registry read, handle_irq

==========================================================================*/

#ifndef _SNSDALSPRXPRIV_H
#define _SNSDALSPRXPRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"

/*=======================================================================

                  INTERNAL DEFINITIONS

========================================================================*/
#define ALSPRX_USE_POLLING             /* Polling is used instead of interrupt */
#undef  ALSPRX_EN_DISTANCE
#define ALSPRX_DEBUG_REG
#undef  ALSPRX_DEBUG_TIMER
#undef  ALSPRX_DEBUG_ONE_SENSOR
#undef  ALSPRX_DEBUG_BASIC

#define SNS_DD_ALSPRX_DFLT_MODE               SNS_NORMAL_MODE
#define SNS_DD_ALSPRX_ISL_SUPPORTED_MODE      (1 << SNS_NORMAL_MODE | 1 << SNS_SLEEP_MODE)
#define SNS_DD_ALSPRX_DATA_READY_MS           8       /* after issue a command */
#define SNS_DD_ALSPRX_DATA_READY_US           8000    /* after issue a command */
#define SNS_DD_ALSPRX_RES_08BIT_MAX_VAL       255
#define SNS_DD_ALSPRX_RES_12BIT_MAX_VAL       4095
#define SNS_DD_ALSPRX_RES_16BIT_MAX_VAL       65535

#define SNS_DD_PRXDIST_SAMPLE_MS              100     /* Sampling period for condition */
#define SNS_DD_PRX_SAMPLE_MS                  100     /* Sampling period for condition */
#define SNS_DD_ALS_SAMPLE_MS                  100     /* Sampling period for condition */
#define SNS_DD_ALS_ACCURACY_DIV_FACTOR        4       /* 4 means 1/4 or 25% */
#define SNS_DD_ALS_HALF_LUX                   500
#define SNS_DD_ALS_SAMPLE_ATTEMPT             2       /* Number of attempts to try to find the right range */

#define SNS_DD_ALS_DFLT_MILLI_LUX             100000  /* default value for the last sampled mlux */
#define SNS_DD_ALS_LONG_TIME_AGO_US           5000000 /* time for ignoring the previous data - 5 sec */

/******************************************************************************
                        Intersil Sensor(ISL29011) defines
******************************************************************************/
/* Register Addresses define */
#define SNS_DD_ISL_ALSPRX_COMMAND1_ADDR         0x00
#define SNS_DD_ISL_ALSPRX_COMMAND2_ADDR         0x01
#define SNS_DD_ISL_ALSPRX_DATA_LSB_ADDR         0x02
#define SNS_DD_ISL_ALSPRX_DATA_MSB_ADDR         0x03
#define SNS_DD_ISL_ALSPRX_INT_LT_LSB_ADDR       0x04
#define SNS_DD_ISL_ALSPRX_INT_LT_MSB_ADDR       0x05
#define SNS_DD_ISL_ALSPRX_INT_HT_LSB_ADDR       0x06
#define SNS_DD_ISL_ALSPRX_INT_HT_MSB_ADDR       0x07

/* Register Value define : COMMAND1 */
#define SNS_DD_ISL_ALSPRX_MODE_MASK             0xE0    /* Operation mode mask: D5-7 */
#define SNS_DD_ISL_ALSPRX_MODE_PWR_DOWN         0x00    /* Power down the device */
#define SNS_DD_ISL_ALSPRX_MODE_ALS_ONCE         0x20    /* ALS once mode */
#define SNS_DD_ISL_ALSPRX_MODE_IR_ONCE          0x40    /* IR once mode */
#define SNS_DD_ISL_ALSPRX_MODE_PRX_ONCE         0x60    /* Proximity once mode */
#define SNS_DD_ISL_ALSPRX_MODE_RSVD             0x80    /* Reserved mode */
#define SNS_DD_ISL_ALSPRX_MODE_ALS_CONT         0xA0    /* ALS continuous mode */
#define SNS_DD_ISL_ALSPRX_MODE_IR_CONT          0xC0    /* IR continuous mode */
#define SNS_DD_ISL_ALSPRX_MODE_PRX_CONT         0xE0    /* Proximity continuous mode */
#define SNS_DD_ISL_ALSPRX_INT_TRIG              0x04    /* Interrupt is triggerred */
#define SNS_DD_ISL_ALSPRX_INT_PRST_MASK         0x03    /* Interrupt persist mask: D0-D1 */

/* Register Value define : COMMAND2 */
#define SNS_DD_ISL_ALSPRX_PRX_SCHEME_AMB_IR_REJ 0x80    /* Proximity Sensing with ambient IR regection */
#define SNS_DD_ISL_ALSPRX_MODE_FRQ_360KHZ       0x40    /* Mode frequency: 360 KHz */
#define SNS_DD_ISL_ALSPRX_MODE_FRQ_DC           0x00    /* Mode frequency: DC */
#define SNS_DD_ISL_ALSPRX_IR_LED_CURR_MASK      0x20    /* Amp of IR LED driver current mask: D4-5 */
#define SNS_DD_ISL_ALSPRX_IR_LED_12MA           0x00    /* IR LED Driver current: 12.5mA */
#define SNS_DD_ISL_ALSPRX_IR_LED_25MA           0x10    /* IR LED Driver current: 25mA */
#define SNS_DD_ISL_ALSPRX_IR_LED_50MA           0x20    /* IR LED Driver current: 50mA */
#define SNS_DD_ISL_ALSPRX_IR_LED_100MA          0x30    /* IR LED Driver current: 100mA */

#define SNS_DD_ISL_ALSPRX_RES_MASK              0x0C    /* Resolution(Bits width) mask: D2-3 */
#define SNS_DD_ISL_ALSPRX_RES_16BIT             0x00    /* Resolution(Bits width): 16 bits */
#define SNS_DD_ISL_ALSPRX_RES_12BIT             0x04    /* Resolution(Bits width): 12 bits */
#define SNS_DD_ISL_ALSPRX_RES_08BIT             0x08    /* Resolution(Bits width): 8 bits */
#define SNS_DD_ISL_ALSPRX_RES_04BIT             0x0C    /* Resolution(Bits width): 4 bits */

#define SNS_DD_ISL_ALSPRX_RNG_MASK              0x03    /* Range mask: D0-1 */
#define SNS_DD_ISL_ALSPRX_RNG1                  0x00    /* Range 1 ( ~1,000 lux) */
#define SNS_DD_ISL_ALSPRX_RNG2                  0x01    /* Range 2 ( ~4,000 lux) */
#define SNS_DD_ISL_ALSPRX_RNG3                  0x02    /* Range 3 ( ~16,000 lux) */
#define SNS_DD_ISL_ALSPRX_RNG4                  0x03    /* Range 4 ( ~64,000 lux) */

#define SNS_DD_ISL_ALS_DEF_RNG                  SNS_DD_ISL_ALSPRX_RNG3  /* default range to use */

/* Sensor HW configuration */ 
#define SNS_DD_ALSPRX_ISL29011_CFG_ID           0x01    /* Configurations for ISL29011 family (for logging) */
#define SNS_DD_PRX_DFLT_MODE_FRQ_DC             SNS_DD_ISL_ALSPRX_MODE_FRQ_DC

/* attributes for light data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */ 
#define SNS_DD_ALS_PWR                        90      /* unit of uA */
#define SNS_DD_ALS_RES                        FX_FLTTOFIX_Q16(0.015)   /* unit of this data type is lux */
#define SNS_DD_ALSPRX_LO_PWR                  0       /* unit of uA */

/* attributes for proximitydistance data type */
#define SNS_DD_PRXDIST_PWR                    12590   /* unit of uA */
#define SNS_DD_PRXDIST_RES                    FX_FLTTOFIX_Q16(0.001)   /* unit of this data type is meter */
#define SNS_DD_PRXDIST_ACCURACY               40      /* unit of this data type unit which is mm */

/* attributes for proximity data type */
#define SNS_DD_PRX_PWR                        221     /* unit of uA */

/* attribute for NV items */
#define SNS_DD_VISIBLE_TRANS_RATIO            25
#define SNS_DD_IR_TRANS_RATIO                 60
#define SNS_DD_DC_OFFSET                      10      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_NEAR                80      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_FAR                 50      /* unit of ADC count */
#define SNS_DD_PRX_FACTOR                    200
#define SNS_DD_ALS_FACTOR                    100

#define SNS_DD_PRXDIST_TB_MAX_VALUE           2047    /* 11 bits + 1 sign bit */
#define SNS_DD_ALSPRX_NUM_SENSOR_TYPES        2

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
  SNS_DD_ALSPRX_RES_8_BIT   = SNS_DD_ISL_ALSPRX_RES_08BIT,
  SNS_DD_ALSPRX_RES_12_BIT  = SNS_DD_ISL_ALSPRX_RES_12BIT,
  SNS_DD_ALSPRX_RES_16_BIT  = SNS_DD_ISL_ALSPRX_RES_16BIT
} sns_dd_alsprx_res_e;

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

/* State of the arbitrator */
typedef enum
{
  SNS_DD_ARBIT_STATE_IDLE           = 0x00,
  SNS_DD_ARBIT_STATE_ALS_PEND       = 0x01,
  SNS_DD_ARBIT_STATE_PRX_PEND       = 0x02,
  SNS_DD_ARBIT_STATE_RSVD           = 0x04
} sns_dd_arbit_state_type;

/* Proximity common module state for proximity and proximity distance */
typedef enum 
{
  SNS_PRXCOMM_STATE_IDLE          = 0x00,
  SNS_PRXCOMM_STATE_PRX_PEND      = 0x01,
  SNS_PRXCOMM_STATE_RSVD          = 0x02 
} sns_dd_prxcomm_state_type;

/* timer object type that is a parameter of sns_dd_start_general_timer/sns_dd_stop_general_timer  */
typedef struct
{   
  BOOLEAN             active;
  BOOLEAN             defined;
} sns_dd_timer_type;

/* data structure for the arbitration manager */
typedef struct
{
  sns_dd_arbit_state_type   state;
  uint8_t                   prx_req_rng;            /* the requested range value */
  uint32_t                  last_issue_mode;
  uint8_t                   als_req_rng;            /* the requested range value */
  uint32_t                  prx_req_cnt;            /* for history */
  uint32_t                  als_req_cnt;            /* for history */
} sns_dd_arbit_db_type;

/* data structure for proximity common handler */
typedef struct
{
  sns_dd_prxcomm_state_type state;
  uint32_t                  prx_req_cnt;      /* for history */
} sns_dd_prxcomm_db_type;

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

/* data structure for light(ALS) driver */
typedef struct
{
  int32_t                   als_data_cache;
  sns_dd_pend_state_type    pend_state;           /* idle or wait state for the response */
  uint8_t                   last_rng_sel;         /* range value that was used for the previous sampling */
  uint8_t                   next_rng_sel;         /* range value that should be used for next sampling */
  uint8_t                   arbit_req_rng_sel;    /* Configuratin during the adaptive range selection */
  uint8_t                   sample_attempt;       /* number of sampling attempts to find the correct range */
  uint32_t                  last_get_time;
  uint32_t                  last_mlux;
  uint16_t                  accuracy;
  uint16_t                  res;
  uint8_t                   resolution;
  uint8_t                   next_resolution;
  uint32_t                  als_conversion;
} sns_dd_als_db_type;

/* data structure for proximitydistance driver */
typedef struct
{
  int32_t                   prxdist_data_cache;
  sns_dd_pend_state_type    pend_state;                /* idle of wait the response */
  int32_t                   prx_data_cache;
  sns_dd_prx_nearby_type    last_nearby;
  uint16_t                  accuracy;
  uint16_t                  res;
  uint8_t                   resolution;
  uint8_t                   next_resolution;
} sns_dd_prx_db_type;

/* data structure for NV items */
typedef struct
{
  uint8_t                   visible_ratio;          /* visible light transmission ratio of the glass/pipe in % */
  uint8_t                   ir_ratio;               /* IR transmission ratio of light glass/pipe in % */
  uint16_t                  dc_offset;              /* DC offset in ADC count */
  uint16_t                  thresh_near;            /* near detection threshold in ADC count adjusted by DC offset */
  uint16_t                  thresh_far;             /* far detection threshold in ADC count adjusted by DC offset */
  uint16_t                  prx_factor;             /* PRX multiplicative factor in % */
  uint16_t                  als_factor;             /* ALS multiplicative factor in % */
#if 0
  uint8_t                   version_num;            /* Version of NV Database */
  uint8_t                   magic_num;              /* Version of implementation */
#endif 
} sns_dd_nv_db_type;

/* common data structure belong to all sub modules */
typedef struct
{   
  /* timer for reading data from the sensor instead of ISR sig handler */
  sns_dd_timer_type         sns_dd_timer;
  sns_dd_arbit_db_type      arbit_db;
  sns_dd_prxcomm_db_type    prxcomm_db;
  sns_dd_nv_db_type         nv_db;                  /* NV item DB type */  
  sns_dd_nv_source          nv_source;
  uint16_t                  nv_size;
  sns_ddf_powerstate_e      pwr_mode;  
  sns_dd_device_state_type  state;
} sns_dd_alsprx_common_db_type;

/* State struct for ALS/PRX ISL29011 */
typedef struct {
   sns_ddf_handle_t         smgr_handle;    /* SDDI handle used to notify_data */
   uint32_t                 dropped_logs;
   sns_ddf_handle_t         port_handle; /* handle used to access the I2C bus */
   sns_dd_als_db_type       sns_dd_als_db;
   sns_dd_prx_db_type       sns_dd_prx_db;
   sns_dd_alsprx_common_db_type sns_dd_alsprx_common_db;
   sns_ddf_timer_s          timer;
   uint8_t                  num_requests;
   BOOLEAN                  two_requests;
} sns_dd_alsprx_state_t;

#endif /* End include guard  SNSDALSPRXPRIV_H */
