/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the AL3320 ALS(Light) and Proximity Sensor

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_al3320_priv.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
09/23/13     yh      Initial version of driver for AL3320
==============================================================================*/


#ifndef _SNSAP3216CPRIV_H
#define _SNSAP3216CPRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"
#include <stdbool.h>

#define AL3320B_MAGIC_NUM         50
#define AL3320B_DRIVER_ID         1

#define AL3320B_DD_ALS_PWR        120
#define AL3320B_DD_ALS_RES        FX_FLTTOFIX_Q16(0.096)
#define AL3320B_DD_ALSPRX_LO_PWR  1

#define AL3320B_ALS_F_ODR         0
#define AL3320B_ALS_THR_LOW_F     0xffff
#define AL3320B_ALS_THR_HIG_F     0x0000

#define SNS_REG_ITEM_ALP_VISIBLE_LIGHT_TRANS_RATIO 25
#define SNS_REG_ITEM_ALP_IR_LIGHT_TRANS_RATIO      60
#define SNS_REG_ITEM_ALP_DC_OFFSET                 10
#define SNS_REG_ITEM_ALP_NEAR_THRESHOLD            80
#define SNS_REG_ITEM_ALP_FAR_THRESHOLD             50
#define SNS_REG_ITEM_ALP_PRX_FACTOR                200
#define SNS_REG_ITEM_ALP_ALS_FACTOR                100

/*al3320b register table*/
#define AL3320B_SYSTEM_CON        0x00
#define AL3320B_FLAG_STAT         0x01
#define AL3320B_INT_CON           0x02
#define AL3320B_ALS_WAIT          0x06
#define AL3320B_ALS_CON           0x07
#define AL3320B_ALS_PER           0x08
#define AL3320B_ALS_MTIME         0x09
#define AL3320B_ALS_A_DUM         0x0A
#define AL3320B_ALS_DATAL         0x22
#define AL3320B_ALS_DATAH         0x23
#define AL3320B_ALS_LTHREL        0x30
#define AL3320B_ALS_LTHREH        0x31
#define AL3320B_ALS_HTHREL        0x32
#define AL3320B_ALS_HTHREH        0x33
#define AL3320B_ALS_CAL           0x34
/*al3320b regedit table end*/

#define AL3320B_RANGE_1X1         0x06
#define AL3320B_RANGE_1X2         0x04
#define AL3320B_RANGE_1X3         0x02
#define AL3320B_RANGE_1X4         0x00
#define AL3320B_RANGE_3X1         0x07
#define AL3320B_RANGE_3X2         0x05
#define AL3320B_RANGE_3X3         0x03
#define AL3320B_RANGE_3X4         0x01

#define AL3320B_RANGE_1X1_RES     0.03
#define AL3320B_RANGE_1X2_RES     0.096
#define AL3320B_RANGE_1X3_RES     0.38
#define AL3320B_RANGE_1X4_RES     1.53
#define AL3320B_RANGE_3X1_RES     0.01
#define AL3320B_RANGE_3X2_RES     0.032
#define AL3320B_RANGE_3X3_RES     0.128
#define AL3320B_RANGE_3X4_RES     0.512

#define AL3320B_ALS_CAL_LOW       250
#define AL3320B_ALS_CAL_UP        350
#define AL3320B_ALS_LUX_COR       (uint32_t)1<<16
#define AL3320B_ALS_BITS          16
#define AL3320B_ALS_FREQ          20
#define AL3320B_ALS_THR           20
#define AL3320B_ALS_DEVICE        4
#define AL3320B_ALS_CALI          1
#define NUM_OF_LUX_TO_AVE         8

#define AL3320B_ENABLE_MSK_AEN    0x01
#define AL3320B_INT_ENA_MSK       0x08
#define AL3320B_INT_DIS_MSK       0x00
#define AL3320B_PWD_MSK_AEN       0x00
#define AL3320B_DEFAULT_RANGE     0x04
#define AL3320B_INT_CLR_MSK       0x00

#define AL3320B_MEAN_TIME         0x0F

#define AL3320B_ALS_INT_L         0.8
#define AL3320B_ALS_INT_H         1.2
#define AL3320B_ALS_INT_FULL      0xFFFF
typedef enum {
    IRQ_TYPE,
    TIMER_TYPE
} AL3320B_SIG_TYPE;

typedef union {
    uint16_t   word;
    uint8_t    byte[2];
} uword_u;

typedef struct  {
    uint16_t data;
    uint32_t lux;
    uint32_t luxAve;
    uint32_t mlux;
    bool     lux_ok;
} als_info;

typedef struct {
    uint8_t  again;
    uint16_t cal_target;
    uint16_t thresh_low;
    uint16_t thresh_high;
    uint32_t odr;
    bool     enabled;
} als_setup;

typedef struct {
    uint8_t  enable;
    uint8_t  config;
    uint8_t  control;
} al3320b_reg;

typedef struct  {
    als_setup   als;
    al3320b_reg  reg;
} lsc_setup;

typedef struct {
    als_info      als;
} lsc_cur_info;

typedef struct {
    lsc_cur_info       data;
    lsc_setup          setup;
} al3320b_chip;

typedef enum
{
    SNS_DD_AL3320B_SUCCESS = 0,
    SNS_DD_AL3320B_ALS_CAL_FAILED_LUX = 1,
    SNS_DD_AL3320B_ALS_CAL_FAILED_RST = 2,
    SNS_DD_AL3320B_ALS_CAL_FAILED_REG = 3,
    SNS_DD_AL3320B_ALS_CAL_FAILED_ENB = 4
} sns_dd_test_err_e;

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
  uint32_t lsc_num;         /* unique number for LSM */
  uint32_t drv_ID;          /* identify specific driver */
  uint32_t calibratePhone;  /* if true, calibrate phone (should only have to done during manufacture) */
  uint32_t calLuxLower;     /* ALS value of calibration light lower */
  uint32_t calLuxUpper;     /* ALS value of calibration light upper */
  uint32_t device;          /* current sensor */
  uint32_t luxCorrection;   /* Lux correction factor */
  uint32_t ALS_thresh_high; /* ALS Interrupt Threshold High in % */
  uint32_t ALS_thresh_low;   /* ALS Interrupt Threshold Low in % */
} sns_dd_nv_db_type;

typedef struct
{
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
   sns_dd_alsprx_common_db_type sns_dd_alsprx_common_db;
   bool                         calibration_mode;    /* calibration mode enable flag */
   uint16_t                     nv_db_size;   /* size of NV item */
   al3320b_chip                 chip;
   sns_ddf_handle_t             ZBW_port_handle;
   uint32_t                     int_gpio;
} sns_dd_alsprx_state_t;

#endif
