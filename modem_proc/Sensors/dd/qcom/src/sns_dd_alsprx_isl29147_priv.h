/*===========================================================================

         S E N S O R S    AMBIENT LIGHT & PROXIMITY SENSOR    D R I V E R  

DESCRIPTION

  Private header file for ISL29147 driver 

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_isl29147_priv.h#1 $


when         who     what, where, why
--------     ---     ------------------------------------------------------
10/04/13     cj      Update the parameters for ISL29147 
09/02/13     cj      Initial version of alsprx driver for ISL29147
==========================================================================*/

#ifndef _SNSDALSPRXPRIV_H
#define _SNSDALSPRXPRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"

/*=======================================================================

                  INTERNAL DEFINITIONS

========================================================================*/
#undef ALSPRX_EN_DISTANCE
#undef ALSPRX_DEBUG_REG
#undef ALSPRX_DEBUG_TIMER
#undef ALSPRX_DEBUG_IRQ
#undef ALSPRX_DEBUG_ONE_SENSOR
#undef ALSPRX_DEBUG_BASIC

#define SNS_DD_ALSPRX_ALS_DATA_READY_US       50000  /* 50ms for ALS */
#define SNS_DD_ALSPRX_PRX_DATA_READY_US       1000   /* 1ms for PRX */
#define SNS_DD_ALS_ACCURACY_DIV_FACTOR        4       /* 4 means 1/4 or 25% */
#define SNS_DD_ALS_HALF_LUX                   500
#define SNS_DD_ALS_DFLT_MILLI_LUX             100000  /* default value for the last sampled mlux */
#define SNS_DD_ALS_LONG_TIME_AGO_US           5000000 /* time for ignoring the previous data - 5 sec */

/******************************************************************************
                        Intersil Sensor(ISL29147) defines
******************************************************************************/
/* Register Addresses define */
#define SNS_DD_ISL29147_ALSPRX_DEVICEID_REG       0x00
#define SNS_DD_ISL29147_ALSPRX_CONFIG0_REG        0x01
#define SNS_DD_ISL29147_ALSPRX_CONFIG1_REG        0x02
#define SNS_DD_ISL29147_ALSPRX_CONFIG2_REG        0x03
#define SNS_DD_ISL29147_ALSPRX_INTERRUPT_REG      0x04
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_TL_REG    0x05
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_TH_REG    0x06
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_TL_REG     0x07
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_TLH_REG    0x08
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_TH_REG     0x09
#define SNS_DD_ISL29147_ALSPRX_PROX_DATA_REG      0x0A
#define SNS_DD_ISL29147_ALSPRX_ALS_DATA_HB_REG    0x0B
#define SNS_DD_ISL29147_ALSPRX_ALS_DATA_LB_REG    0x0C
#define SNS_DD_ISL29147_ALSPRX_PROX_AMBIR_REG     0x0D
#define SNS_DD_ISL29147_ALSPRX_CONFIG3_REG        0x0E

/* Register Value define */

/*DEVICE ID*/
#define SNS_DD_ISL29147_ALSPRX_DEVICEID_MASK      0xF8
#define SNS_DD_ISL29147_ALSPRX_DEVICEID           0xC0

/*CONFIG0*/
#define SNS_DD_ISL29147_ALSPRX_PROX_EN_BIT        5
#define SNS_DD_ISL29147_ALSPRX_PROX_EN_MASK       0x20
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_BIT       2
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_MASK      0x1C
#define SNS_DD_ISL29147_ALSPRX_IRDR_DRV_BIT       0
#define SNS_DD_ISL29147_ALSPRX_IRDR_DRV_MASK      0x03

#define SNS_DD_ISL29147_ALSPRX_PROX_ENABLE        1
#define SNS_DD_ISL29147_ALSPRX_PROX_DISABLE       0
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_400MS     0
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_100MS     1
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_50MS      2
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_25MS      3
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_12P5MS    4
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_6P25MS    5
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_3P125MS   6
#define SNS_DD_ISL29147_ALSPRX_PROX_SLP_0MS       7
#define SNS_DD_ISL29147_ALSPRX_IRDR_DRV_31P25MA   0
#define SNS_DD_ISL29147_ALSPRX_IRDR_DRV_62P5MA    1
#define SNS_DD_ISL29147_ALSPRX_IRDR_DRV_125MA     2
#define SNS_DD_ISL29147_ALSPRX_IRDR_DRV_250MA     3

/*CONFIG1*/
#define SNS_DD_ISL29147_ALSPRX_INT_ALG_BIT        7        /* Interrupt Algorithm */
#define SNS_DD_ISL29147_ALSPRX_INT_ALG_MASK       0x80
#define SNS_DD_ISL29147_ALSPRX_PRXO_OFFSET_BIT    3
#define SNS_DD_ISL29147_ALSPRX_PRXO_OFFSET_MASK   0x78
#define SNS_DD_ISL29147_ALSPRX_ALS_EN_BIT         2
#define SNS_DD_ISL29147_ALSPRX_ALS_EN_MASK        0x04
#define SNS_DD_ISL29147_ALSPRX_ALS_RANGE_BIT      0
#define SNS_DD_ISL29147_ALSPRX_ALS_RANGE_MASK     0x03

#define SNS_DD_ISL29147_ALSPRX_INT_ALG_WC         1        /* Window Comparator */
#define SNS_DD_ISL29147_ALSPRX_INT_ALG_HW         0        /* Hysteresis Window */ 
#define SNS_DD_ISL29147_ALSPRX_ALS_ENABLE         1
#define SNS_DD_ISL29147_ALSPRX_ALS_DISABLE        0
#define SNS_DD_ISL29147_ALSPRX_ALP_RANGE_125LUX   0
#define SNS_DD_ISL29147_ALSPRX_ALP_RANGE_250LUX   1
#define SNS_DD_ISL29147_ALSPRX_ALP_RANGE_2000LUX  2
#define SNS_DD_ISL29147_ALSPRX_ALP_RANGE_4000LUX  3

/*CONFIG2*/
#define SNS_DD_ISL29147_ALSPRX_ALS_IRCOMP_BIT     0
#define SNS_DD_ISL29147_ALSPRX_ALS_IRCOMP_MASK    0x0F

/*INTERRUPT*/
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_FLG_BIT   7
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_FLG_MASK  0x80
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_PRST_BIT  5
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_PRST_MASK 0x60
#define SNS_DD_ISL29147_ALSPRX_PWR_FAIL_BIT       4
#define SNS_DD_ISL29147_ALSPRX_PWR_FAIL_MASK      0x10
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_FLG_BIT    3
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_FLG_MASK   0x08
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_PRST_BIT   1
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_PRST_MASK  0x06
#define SNS_DD_ISL29147_ALSPRX_INT_CFG_BIT        0
#define SNS_DD_ISL29147_ALSPRX_INT_CFG_MASK       0x01

#define SNS_DD_ISL29147_ALSPRX_PROX_INT_EVENT     1
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_NO_EVENT  0
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_PRST1     0
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_PRST2     1
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_PRST4     2
#define SNS_DD_ISL29147_ALSPRX_PROX_INT_PRST8     3
#define SNS_DD_ISL29147_ALSPRX_PWR_FAIL_BO        1        /* Brown Out Detected */
#define SNS_DD_ISL29147_ALSPRX_PWR_FAIL_NO        0        /* Normal Operation */
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_EVENT      1
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_NO_EVENT   0
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_PRST1      0
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_PRST2      1
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_PRST4      2
#define SNS_DD_ISL29147_ALSPRX_ALS_INT_PRST8      3
#define SNS_DD_ISL29147_ALSPRX_INT_CFG_AND        1        /* Interrrupt if ALS and PROX Event */
#define SNS_DD_ISL29147_ALSPRX_INT_CFG_OR         0        /* Interrrupt if ALS or PROX Event */

/* PROX_AMBIR */
#define SNS_DD_ISL29147_ALSPRX_PROX_AMBIR_IR_BIT  1
#define SNS_DD_ISL29147_ALSPRX_PROX_AMBIR_IR_MASK 0xFE
#define SNS_DD_ISL29147_ALSPRX_PROX_WASH_BIT      0
#define SNS_DD_ISL29147_ALSPRX_PROX_WASH_MASK     0x01

#define SNS_DD_ISL29147_ALSPRX_PROX_WASH_DETECTED 1
#define SNS_DD_ISL29147_ALSPRX_PROX_WASH_NORMAL   0


/******************************************************************************
                        Intersil Sensor common defines
******************************************************************************/
/* Register Value define : CONFIG */
#define SNS_DD_ISL_ALSPRX_PROX_MODE_MASK        0x80    /* D7 - PROX_EN */
#define SNS_DD_ISL_ALSPRX_PROX_MODE_EN          0x80    /* D7 - PROX_EN */
#define SNS_DD_ISL_ALSPRX_PROX_SLP_MASK         0x70    /* D6:4 - PROX_SLP */
#define SNS_DD_ISL_ALSPRX_PROX_SLP_800MS        0x00    /* D6:4 - PROX_SLP @ 800ms or 1.25Hz */
#define SNS_DD_ISL_ALSPRX_PROX_DR_110MA         0x00    /* D3 - PROX_DR: 110mA */
#define SNS_DD_ISL_ALSPRX_PROX_DR_220MA         0x08    /* D3 - PROX_DR: 220mA */
#define SNS_DD_ISL_ALSPRX_ALS_MODE_MASK         0x04    /* D2 - ALS_EN */
#define SNS_DD_ISL_ALSPRX_ALS_MODE_EN           0x04    /* D2 - ALS_EN */
#define SNS_DD_ISL_ALSPRX_ALS_RANGE_LOW         0x01    /* D1 - ALS_RANGE: Low Lux */
#define SNS_DD_ISL_ALSPRX_ALS_RANGE_HIGH        0x02    /* D1 - ALS_RANGE: High Lux */
#define SNS_DD_ISL_ALSPRX_ALSIR_MODE_VISIBLE    0x00    /* D0 - ALSIR_MODE: visible light sensing */
#define SNS_DD_ISL_ALSPRX_ALSIR_MODE_IR         0x01    /* D0 - ALSIR_MODE: IR sensing */
#define SNS_DD_ISL_ALSPRX_PWR_DOWN              0x00    /* Power down the device */ 

/* Register Value define : INTERRUPT */
#define SNS_DD_ISL_ALSPRX_PROX_INT_MASK         0x80    /* D7 - PROX_FLAG */
#define SNS_DD_ISL_ALSPRX_PROX_PRST_1           0x00    /* D6:5 - PROX_PRST: 1 conversion */
#define SNS_DD_ISL_ALSPRX_PROX_PRST_4           0x20    /* D6:5 - PROX_PRST: 4 conversions */
#define SNS_DD_ISL_ALSPRX_PROX_PRST_8           0x40    /* D6:5 - PROX_PRST: 8 conversions */
#define SNS_DD_ISL_ALSPRX_PROX_PRST_16          0x60    /* D6:5 - PROX_PRST: 16 conversions */
#define SNS_DD_ISL_ALSPRX_ALS_INT_MASK          0x08    /* D3 - ALS_FLAG */
#define SNS_DD_ISL_ALSPRX_ALS_PRST_1            0x00    /* D2:1 - PROX_PRST: 1 conversion */
#define SNS_DD_ISL_ALSPRX_ALS_PRST_4            0x02    /* D2:1 - PROX_PRST: 4 conversions */
#define SNS_DD_ISL_ALSPRX_ALS_PRST_8            0x04    /* D2:1 - PROX_PRST: 8 conversions */
#define SNS_DD_ISL_ALSPRX_ALS_PRST_16           0x06    /* D2:1 - PROX_PRST: 16 conversions */
#define SNS_DD_ISL_ALSPRX_INT_CTRL_OR           0x00    /* D0 - INT_CTRL: interrupt when prox OR als flag is high */
#define SNS_DD_ISL_ALSPRX_INT_CTRL_AND          0x01    /* D0 - INT_CTRL: interrupt when prox AND als flags are high */
#define SNS_DD_ISL_ALSPRX_INT_MASK              0x88    /* D7, D3 */

/* Register Value define : PROX_LT */
#define SNS_DD_ISL_ALSPRX_PROX_LT               0xFF    /* D7:0 - PROX_LT[7:0] */

/* Register Value define : PROX_HT */
#define SNS_DD_ISL_ALSPRX_PROX_HT               0x00    /* D7:0 - PROX_HT[7:0] */

/* Register Value define : ALSIR_LT */
#define SNS_DD_ISL_ALSPRX_ALSIR_TH1             0x00    /* D7:0 - ALSIR_LT[7:0] */

/* Register Value define : ALSIR_HT[3:0], ALSIR_LT[11:8] */
#define SNS_DD_ISL_ALSPRX_ALSIR_TH2             0x00    /* D7:0 - PROX_LT[7:0] */

/* Register Value define : ALSIR_HT[11:4] */
#define SNS_DD_ISL_ALSPRX_ALSIR_TH3             0x00    /* D7:0 - PROX_LT[7:0] */
#define SNS_DD_ISL_ALSPRX_ALSIR_DT_MASK         0x0FFF  /* 12 bit data */
#define SNS_DD_ISL_ALS_DEF_RNG                  SNS_DD_ISL29147_ALSPRX_ALP_RANGE_125LUX  /* default range to use */

#define SNS_DD_ISL_ALS_CYCLE_TIME               100000
#define SNS_DD_ISL_PRX_CYCLE_TIME               10000

/* Sensor HW configuration */ 
#define SNS_DD_ALSPRX_ISL29011_CFG_ID           0x01    /* Configurations for ISL29028 family (for logging) */
#define SNS_DD_PRX_DFLT_MODE_FRQ_DC             SNS_DD_ISL_ALSPRX_MODE_FRQ_DC

/* attributes for light data type */
/* The res and accuracy for ALS are not static values. They're calculated from the previous data */ 
#define SNS_DD_ALS_PWR                          95      /* unit of uA */
#define SNS_DD_ALS_RES                          FX_FLTTOFIX_Q16(0.015)   /* unit of this data type is lux */
#define SNS_DD_ALSPRX_LO_PWR                    1       /* unit of uA */

/* attributes for proximity data type */
#define SNS_DD_PRX_PWR                          95   /* unit of uA */
#define SNS_DD_PRX_RES                          FX_FLTTOFIX_Q16(0.001)   /* unit of this data type is meter */
#define SNS_DD_PRX_ACCURACY                     40      /* unit of this data type unit which is mm */

/* attribute for NV items */
#define SNS_DD_VISIBLE_TRANS_RATIO              10
#define SNS_DD_IR_TRANS_RATIO                   60
#define SNS_DD_DC_OFFSET                        20       /* unit of ADC count */
#define SNS_DD_PRX_THRESH_NEAR                  200      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_FAR                   200      /* unit of ADC count */
#define SNS_DD_PRX_FACTOR                       100
#define SNS_DD_ALS_FACTOR                       100

#define SNS_DD_ALSPRX_NUM_SENSOR_TYPES          2
#define SNS_DD_ALSPRX_PRX_BITS                  8
#define SNS_DD_ALSPRX_PRX_FREQ                  100
#define SNS_DD_ALSPRX_ALS_BITS                  12
#define SNS_DD_ALSPRX_ALS_FREQ                  20
#define SNS_DD_ALS_ODR                          10
#define SNS_DD_PRX_ODR                          10
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
  uint32_t                  als_odr;
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
  uint32_t                  prx_odr;
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
   bool                     prx_enabled;
   bool                     als_enabled;
} sns_dd_alsprx_state_t;

#endif /* End include guard  SNSDALSPRXPRIV_H */
