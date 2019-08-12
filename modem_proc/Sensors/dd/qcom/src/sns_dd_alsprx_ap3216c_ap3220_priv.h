/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the ap3216c ALS(Light) and Proximity Sensor

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_ap3216c_ap3220_priv.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
08/09/13     yh      Initial version of driver for AP3216C
==============================================================================*/

#ifndef _SNSAP3216CPRIV_H
#define _SNSAP3216CPRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"
#include <stdbool.h>

#define LSC_MAGIC_NUM   50
#define LSC_DRIVER_ID   1

#define AP3216C_STA_PRX_INTR            0x02
#define AP3216C_STA_ALS_INTR            0x01
#define AP3216Cx_ENABLE_MSK_PEN         0x02
#define AP3216Cx_ENABLE_MSK_AEN         0x01
#define AP3216Cx_ALSPRX_PWR_DOWN        0x00

#define SNS_DD_ALSPRX_NUM_SENSOR_TYPES  2
#define SNS_DD_PRX_THRESH_NEAR          0x2000
#define SNS_DD_PRX_THRESH_FAR           0x500
#define SNS_DD_ALS_PWR                  170
#define SNS_DD_ALS_RES                  FX_FLTTOFIX_Q16(1.0)
#define SNS_DD_ALSPRX_LO_PWR            2
#define SNS_DD_PRX_PWR                  325
#define SNS_DD_PRX_RES                  FX_FLTTOFIX_Q16(0.05)
#define SNS_DD_PRX_ACCURACY             40
#define SNS_DD_ALS_ACCURACY_DIV_FACTOR  4
#define SNS_DD_ALS_HALF_LUX             500
#define SNS_DD_ALSPRX_PRX_BITS          16
#define SNS_DD_ALSPRX_PRX_FREQ          80
#define SNS_DD_ALSPRX_ALS_BITS          16
#define SNS_DD_ALSPRX_ALS_FREQ          10

#define AP3216C_AGAIN_1x        0x03/*bit B4 B5*/
#define AP3216C_AGAIN_8x        0x02
#define AP3216C_AGAIN_16x       0x01
#define AP3216C_AGAIN_64x       0x00

#define AP3216C_PGAIN_1x        0x00/*bit B2 B3*/
#define AP3216C_PGAIN_2x        0x01
#define AP3216C_PGAIN_4x        0x02
#define AP3216C_PGAIN_8x        0x03

#define AP3216C_RANGE_A         0x30
#define AP3216C_RANGE_B         0x20
#define AP3216C_RANGE_C         0x10
#define AP3216C_RANGE_D         0x00

#define AP3216C_RANGE_A_RATION  0.0056
#define AP3216C_RANGE_B_RATION  0.022
#define AP3216C_RANGE_C_RATION  0.089
#define AP3216C_RANGE_D_RATION  0.36

#define SNS_CLR_ALL_INT         0x03
#define SNS_DD_ALSPRX_PWR_DOWN  0x00

#define SNS_ALS_INT_CNT_L       27
#define SNS_ALS_INT_CNT_H       0xffe4
#define SNS_ALS_THR_LOW_F       0xffff
#define SNS_ALS_THR_HIG_F       0x0000
#define SNS_ALS_CAL_TARGET      0x40
#define SNS_PRO_LED_PULSE       0x01
#define SNS_PRO_LED_RATIO       0x03
#define SNS_ALSPRO_F_ODR        0
#define SNS_ALSPRO_INT_ODR      5
#define SNS_RANGE_MASK          0x30

//system registers
#define AP3216C_MODE_COMMAND    0x00
#define AP3216C_INT_STATUS      0x01
#define AP3216C_INT_CLEAR       0x02
#define AP3216C_IR_DATA_LOW     0x0A
#define AP3216C_IR_DATA_HIGH    0x0B
#define AP3216C_ALS_ADC_LSB     0x0c
#define AP3216C_ALS_ADC_MSB     0x0d
#define AP3216C_PX_ADC_LSB      0x0e
#define AP3216C_PX_ADC_MSB      0x0f

//als registers
#define AP3216C_ALS_CON     0x10
#define AP3216C_ALS_CAL     0x19
#define AP3216C_ALS_LTHL    0x1a
#define AP3216C_ALS_LTHH    0x1b
#define AP3216C_ALS_HTHL    0x1c
#define AP3216C_ALS_HTHH    0x1d

//ps registers
#define AP3216C_PX_CON       0x20
#define AP3216C_PX_LED_DRI   0x21
#define AP3216C_PX_INT_FORM  0x22
#define AP3216C_PX_MEAN_TIME 0x23
#define AP3216C_PX_LED_WTIME 0x24
#define AP3216C_PX_CAL_L     0x28
#define AP3216C_PX_CAL_H     0x29
#define AP3216C_PX_LTHL      0x2a
#define AP3216C_PX_LTHH      0x2b
#define AP3216C_PX_HTHL      0x2c
#define AP3216C_PX_HTHH      0x2d

typedef enum {
    IRQ_TYPE,
    TIMER_TYPE
} AP3216C_SIG_TYPE;

typedef union {
    uint16_t   word;
    uint8_t    byte[2];
} uword_u;

typedef struct  {
    uint16_t data;
    uint32_t lux;
    uint32_t mlux;
    bool     lux_ok;
} als_info;

/* proximity data */
typedef struct  {
    uint16_t data;
    int16_t  event;
} prox_info;

typedef struct {
    uint8_t  again;
    uint16_t cal_target;
    uint16_t thresh_low;
    uint16_t thresh_high;
    uint32_t odr;
    bool     enabled;
} als_setup;

typedef struct {
    uint8_t  pgain;
    uint8_t  led_pulse;
    uint8_t  led_ratio;
    uint16_t thresh_low;
    uint16_t thresh_high;
    uint32_t odr;
    bool     enabled;
} prox_setup;

typedef struct {
    uint8_t  enable;
    uint8_t  config;
    uint8_t  control;
} ap3216c_reg;

typedef struct  {
    als_setup   als;
    prox_setup  prox;
    ap3216c_reg  reg;
} lsc_setup;

typedef struct {
    als_info      als;
    prox_info     prox;
} lsc_cur_info;

typedef struct {
    lsc_cur_info       data;
    lsc_setup          setup;
} ap3216c_chip;

typedef enum
{
    SNS_DD_AP3216C_SUCCESS = 0,
    SNS_DD_AP3216C_ALS_CAL_FAILED_LUX = 1, /*ALS CAL error - Received lux value out of range*/
    SNS_DD_AP3216C_ALS_CAL_FAILED_RST = 2, /*ALS CAL error - cannot reset the device*/
    SNS_DD_AP3216C_ALS_CAL_FAILED_REG = 3, /*ALS CAL error - cannot write back to registry*/
    SNS_DD_AP3216C_ALS_CAL_FAILED_ENB = 4  /*ALS CAL error - cannot enable (error in set power state)*/
} sns_dd_test_err_e;

typedef enum
{
  SNS_PRX_FAR_AWAY,
  SNS_PRX_NEAR_BY,
  SNS_PRX_NEAR_BY_UNKNOWN
} sns_dd_prx_nearby_type;

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
  uint32_t                  last_mlux;
  uint16_t                  accuracy;
} sns_dd_als_db_type;

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
  uint32_t device;          /* current sensor */
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
   sns_dd_als_db_type           sns_dd_als_db;
   sns_dd_alsprx_common_db_type sns_dd_alsprx_common_db;
   bool                         calibration_mode;    /* calibration mode enable flag */
   uint16_t                     nv_db_size;   /* size of NV item */
   ap3216c_chip                 chip;
   sns_ddf_handle_t             ZBW_port_handle;
   uint32_t                     int_gpio;
} sns_dd_alsprx_state_t;

#endif
