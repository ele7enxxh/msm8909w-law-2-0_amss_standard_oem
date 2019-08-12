/*===========================================================================

         S E N S O R S    AMBIENT LIGHT & PROXIMITY SENSOR    D R I V E R  

DESCRIPTION

  Private header file for LTR55XALS driver 

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_ltr55x_priv.h#1 $


when         who     what, where, why
--------     ---     ------------------------------------------------------ 
08/21/13     cj      Add calibration code
05/24/13     fp      Initial port of alsprx driver
==========================================================================*/

#ifndef _SNSDALSPRXPRIV_H
#define _SNSDALSPRXPRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"

/*=======================================================================

                  INTERNAL DEFINITIONS

========================================================================*/
//#define ALSPRX_USE_POLLING             /* support polling and DRI by flag */
#define ALSPRX_USE_INTERRUPT
#undef  ALSPRX_EN_DISTANCE
#define ALSPRX_DEBUG_REG
#undef ALSPRX_DEBUG_TIMER
#define ALSPRX_DEBUG_IRQ
#undef  ALSPRX_DEBUG_ONE_SENSOR
#define ALSPRX_DEBUG_BASIC

#define SNS_DD_ALSPRX_ALS_DATA_READY_US       100000       /* 100ms for ALS */
#define SNS_DD_ALSPRX_PRX_DATA_READY_US       100000       /* 70ms for PRX */
#define SNS_DD_ALSPRX_ALPHA_LO                326
#define SNS_DD_ALSPRX_ALPHA_HI                5220

#define SNS_DD_ALS_ACCURACY_DIV_FACTOR        4       /* 4 means 1/4 or 25% */
#define SNS_DD_ALS_HALF_LUX                   500

#define SNS_DD_ALS_DFLT_MILLI_LUX             100  /* default value for the last sampled mlux */
#define SNS_DD_ALS_LONG_TIME_AGO_US           5000000 /* time for ignoring the previous data - 5 sec */

/******************************************************************************
                        Liteon Sensor(LTR-55X) defines
******************************************************************************/
/* LTR-55X Registers */ 
#define LTR55X_ALS_CONTR  0x80 
#define LTR55X_PS_CONTR  0x81 
#define LTR55X_PS_LED   0x82 
#define LTR55X_PS_N_PULSES  0x83 
#define LTR55X_PS_MEAS_RATE 0x84 
#define LTR55X_ALS_MEAS_RATE 0x85 
#define LTR55X_PART_ID 0x86
#define LTR55X_MANUFACTURER_ID 0x87 
 
#define LTR55X_INTERRUPT  0x8F 
#define LTR55X_PS_THRES_UP_0 0x90 
#define LTR55X_PS_THRES_UP_1 0x91 
#define LTR55X_PS_THRES_LOW_0 0x92 
#define LTR55X_PS_THRES_LOW_1 0x93 
 
#define LTR55X_ALS_THRES_UP_0 0x97 
#define LTR55X_ALS_THRES_UP_1 0x98 
#define LTR55X_ALS_THRES_LOW_0 0x99 
#define LTR55X_ALS_THRES_LOW_1 0x9A 
 
#define LTR55X_INTERRUPT_PERSIST 0x9E 
 
/* 558's Read Only Registers */ 
#define LTR55X_ALS_DATA_CH1_0 0x88 
#define LTR55X_ALS_DATA_CH1_1 0x89 
#define LTR55X_ALS_DATA_CH0_0 0x8A 
#define LTR55X_ALS_DATA_CH0_1 0x8B 
#define LTR55X_ALS_PS_STATUS 0x8C 
#define LTR55X_PS_DATA_0  0x8D 
#define LTR55X_PS_DATA_1  0x8E 

/* Basic Operating Modes  */
#define MODE_ALS_ON_Range1  0x0B 
#define MODE_ALS_ON_Range2  0x03 
#define MODE_ALS_StdBy   0x00 
 
#define MODE_PS_ON_Gain1  0x03 
#define MODE_PS_ON_Gain2  0x07 
#define MODE_PS_ON_Gain4  0x0B 
#define MODE_PS_ON_Gain8  0x0C 
#define MODE_PS_StdBy   0x00 

#define PS_RANGE1 1 
#define PS_RANGE2 2 
#define PS_RANGE4 4 
#define PS_RANGE8 8 
 
#define ALS_RANGE1_320  1 
#define ALS_RANGE2_64K  2 

#define SNS_DD_MANUFAC_ID  0x05

#define SNS_DD_LTR558_ID  0x80
#define SNS_DD_LTR559_ID  0x92

/* Register Value define : CONFIG */
#define SNS_DD_LTR55X_ALSPRX_ALS_RANGE_LOW         0x00    /* D1 - ALS_RANGE: Low Lux */
#define SNS_DD_LTR55X_ALSPRX_ALS_RANGE_HIGH        0x02    /* D1 - ALS_RANGE: High Lux */
/* Register Value define : CONFIG */
#define SNS_DD_LTR55X_ALSPRX_PROX_MODE_MASK        0x03    /* D7 - PROX_EN */
#define SNS_DD_LTR55X_ALSPRX_PROX_MODE_EN          0x03    /* D7 - PROX_EN */
#define SNS_DD_LTR55X_ALSPRX_PROX_GAIN             0x00<<0x2    /* D7 - PROX_EN */
#define SNS_DD_LTR55X_ALSPRX_PROX_GAIN_4           0x01<<0x2    /* D7 - PROX_EN */
#define SNS_DD_LTR55X_ALSPRX_PROX_GAIN_8           0x02<<0x2    /* D7 - PROX_EN */
#define SNS_DD_LTR55X_ALSPRX_PROX_GAIN_16          0x03<<0x2    /* D7 - PROX_EN */
#define SNS_DD_LTR55X_ALSPRX_ALS_MODE_MASK         0x03    /* D2 - ALS_MASK */
#define SNS_DD_LTR55X_ALSPRX_ALS_MODE_EN           0x03    /* D2 - ALS_EN */
#define SNS_DD_LTR559_ALSPRX_ALS_MODE_EN           0x19    /* D2 - ALS_EN */
#define SNS_DD_LTR55X_ALSPRX_ALS_GAIN_1         0x01<<3    /* D1 - ALS_RANGE: Low Lux */
#define SNS_DD_LTR55X_ALSPRX_ALS_GAIN_2         0x00<<3    /* D1 - ALS_RANGE: High Lux */
#define SNS_DD_LTR55X_ALSPRX_ALSIR_SW_RST       0x01<<2    /* D0 - ALSIR_MODE: visible light sensing */
#define SNS_DD_LTR55X_ALSPRX_INT_MASK               0xa    /* D3, D1 */
#define SNS_DD_LTR55X_ALSPRX_ALS_INT_MASK          0x0c    /* D3 - ALS_FLAG */
#define SNS_DD_LTR55X_ALSPRX_PROX_INT_MASK         0x03    /* D7 - PROX_FLAG */
#define SNS_DD_LTR55X_ALSPRX_PWR_DOWN              0x00    /* Power down the device */ 
/* Register Value define : PROX_LT */
#define SNS_DD_LTR55X_ALSPRX_PROX_LT_1               0x83    /* D7:0 - PROX_LT[7:0] */
#define SNS_DD_LTR55X_ALSPRX_PROX_LT_2               0x07    /* D7:0 - PROX_LT[7:0] */

/* Register Value define : PROX_HT */
#define SNS_DD_LTR55X_ALSPRX_PROX_HT_1               0x00    /* D7:0 - PROX_HT[7:0] */
#define SNS_DD_LTR55X_ALSPRX_PROX_HT_2               0x00    /* D7:0 - PROX_HT[7:0] */

/* Register Value define : ALSIR_LT */
#define SNS_DD_LTR55X_ALSPRX_ALSIR_LH1             0x01    /* D7:0 - ALSIR_LT[7:0] */
#define SNS_DD_LTR55X_ALSPRX_ALSIR_LH2             0x00    /* D7:0 - ALSIR_LT[7:0] */
/* Register Value define : ALSIR_HT[3:0], ALSIR_LT[11:8] */
#define SNS_DD_LTR55X_ALSPRX_ALSIR_TH1             0xC8    /* D7:0 - PROX_LT[7:0] */

/* Register Value define : ALSIR_HT[11:4] */
#define SNS_DD_LTR55X_ALSPRX_ALSIR_TH2             0x00    /* D7:0 - PROX_LT[7:0] */
#define SNS_DD_LTR55X_ALSPRX_ALSIR_DT_MASK         0xFFFF  /* 12 bit data */
#define SNS_DD_LTR55X_ALS_DEF_RNG                  SNS_DD_LTR55X_ALSPRX_ALS_RANGE_HIGH  /* default range to use */

#define SNS_DD_LTR55X_ALS_CYCLE_TIME      100000
#define SNS_DD_LTR55X_PRX_CYCLE_TIME      100000

/* Sensor HW configuration */ 
#define SNS_DD_ALSPRX_LTR55X_CFG_ID           0x01    /* Configurations for LTR55XALS family (for logging) */
#define SNS_DD_PRX_DFLT_MODE_FRQ_DC           SNS_DD_LTR55X_ALSPRX_MODE_FRQ_DC

/* attributes for light data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */ 
#define SNS_DD_ALS_PWR                        100      /* unit of uA */
#define SNS_DD_ALS_RES                        FX_FLTTOFIX_Q16(0.015)   /* unit of this data type is lux */
#define SNS_DD_ALSPRX_LO_PWR                  5       /* unit of uA */

/* attributes for proximity data type */
#define SNS_DD_PRX_PWR                        150   /* unit of uA */
#define SNS_DD_PRX_RES                        FX_FLTTOFIX_Q16(0.001)   /* unit of this data type is meter */
#define SNS_DD_PRX_ACCURACY                   40      /* unit of this data type unit which is mm */

/* attribute for NV items */
#define SNS_DD_VISIBLE_TRANS_RATIO            25
#define SNS_DD_IR_TRANS_RATIO                 60
#define SNS_DD_DC_OFFSET                      10      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_NEAR                0x320     /* unit of ADC count */
#define SNS_DD_PRX_THRESH_FAR                 0x320     /* unit of ADC count */
#define SNS_DD_PRX_FACTOR                    100
#define SNS_DD_ALS_FACTOR                    100

#define SNS_DD_PRXDIST_TB_MAX_VALUE           2047    /* 11 bits + 1 sign bit */
#define SNS_DD_ALSPRX_NUM_SENSOR_TYPES        2
#define SNS_DD_ALSPRX_PRX_BITS                11
#define SNS_DD_ALSPRX_PRX_FREQ                20  /* Max measurement repeat rate 20ms */
#define SNS_DD_ALSPRX_ALS_BITS                16
#define SNS_DD_ALSPRX_ALS_FREQ                20
#define SNS_DD_ALS_ODR              5
#define SNS_DD_PRX_ODR              5

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
  SNS_DD_LTR558,            /* ltr558 */
  SNS_DD_LTR559,            /* ltr559 */
  SNS_DD_UNKNOWN            /* reserved */
} sns_dd_LTR_type;

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

/* timer object type that is a parameter of sns_dd_start_general_timer/sns_dd_stop_general_timer  */
typedef struct
{   
  BOOLEAN             active;
  BOOLEAN             defined;
  sns_ddf_timer_s     timer_obj;
} sns_dd_timer_type;

/* data structure for light(ALS) driver */
typedef struct
{
  int32_t                   als_data_cache;
  sns_dd_pend_state_type    pend_state;           /* idle or wait state for the response */
  uint8_t                   last_rng_sel;         /* range value that was used for the previous sampling */
  uint8_t                   next_rng_sel;         /* range value that should be used for next sampling */
  uint8_t                   req_rng_sel;          /* Configuratin during the adaptive range selection */
  uint32_t                  last_get_time;
  uint32_t                  last_mlux;
  uint16_t                  accuracy;
  uint32_t                  als_conversion_lo;
  uint32_t                  als_conversion_hi;
  uint32_t                  als_conversion;
  uint32_t                  als_req_cnt;      /* for history */
  uint32_t          als_odr;
  uint32_t                  cycle_time;
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
  uint32_t          prx_odr;
  uint32_t                  cycle_time;
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
  uint8_t                   version_num;      /* Version of NV Database */
  uint8_t                   id;               /* Unique identifier for sensor/vendor combination */
} sns_dd_nv_db_type;

/* common data structure belong to all sub modules */
typedef struct
{   
  sns_dd_timer_type         als_timer;           /* timer to wait for data to be ready */
  sns_dd_timer_type         prx_timer;           /* timer to wait for data to be ready */
  sns_dd_nv_db_type         nv_db;               /* NV item DB type */  
  sns_dd_nv_source          nv_source;
  sns_ddf_powerstate_e      pwr_mode;  
  sns_dd_device_state_type  state;
  bool                      prx_calibration;
  bool                      als_calibration;
} sns_dd_alsprx_common_db_type;

/* State struct for ALS/PRX */
typedef struct {
   sns_ddf_handle_t         smgr_handle;    /* SDDI handle used to notify_data */
   uint32_t                 dropped_logs;
   sns_ddf_handle_t         port_handle;    /* handle used to access the I2C bus */
   sns_dd_als_db_type       sns_dd_als_db;
   sns_dd_prx_db_type       sns_dd_prx_db;
   uint32_t                 interrupt_gpio;
   sns_dd_alsprx_common_db_type sns_dd_alsprx_common_db;
   sns_dd_LTR_type       ltr_type;
   bool dri_enable;
} sns_dd_alsprx_state_t;

#endif /* End include guard  SNSDALSPRXPRIV_H */
