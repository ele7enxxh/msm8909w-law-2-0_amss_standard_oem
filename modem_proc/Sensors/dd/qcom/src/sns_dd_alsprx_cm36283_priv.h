/*==============================================================================

    S E N S O R S   L I G H T   A N D   P R O X I M I T Y   D R I V E R

DESCRIPTION

   Impelements the driver for the CM36283 driver

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_cm36283_priv.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
12/09/13     yh      Initial version of alsprx driver for cm36283
==============================================================================*/

#ifndef _SNS_CM36283_PRIV_H
#define _SNS_CM36283_PRIV_H

#include "fixed_point.h"
#include "sns_ddf_util.h"
#include <stdbool.h>

#define CAPELLA_MAGIC_NUM	50
#define CAPELLA_DRIVER_ID	1

#define SNS_DD_ALSPRX_NUM_SENSOR_TYPES	2
#define SNS_DD_PRX_THRESH_MIN			0x00
#define SNS_DD_PRX_THRESH_MAX		0xFF
#define SNS_DD_VISIBLE_TRANS_RATIO     25
#define SNS_DD_IR_TRANS_RATIO              60
#define SNS_DD_DC_OFFSET                       10      /* unit of ADC count */
#define SNS_DD_PRX_THRESH_NEAR           15
#define SNS_DD_PRX_THRESH_FAR             9
#define SNS_DD_PRX_FACTOR                     200
#define SNS_DD_ALS_FACTOR                     100
#define SNS_DD_CALIBRATEPHONE             1			
#define SNS_DD_DEVICE                              4
#define SNS_DD_CAL_LUX_LOWER                   250
#define SNS_DD_CAL_LUX_UPPER                    350
#define SNS_DD_ALS_LUX_CORRECTION        (uint32_t)1<<16;			
#define SNS_DD_ALS_THRESH_HIGH            20
#define SNS_DD_ALS_THRESH_LOW             20
#define SNS_DD_ALS_PWR					150
#define SNS_DD_ALS_RES			FX_FLTTOFIX_Q16(0.1)
#define SNS_DD_ALSPRX_LO_PWR			0.0002
#define SNS_DD_PRX_PWR					180
#define SNS_DD_PRX_RES			FX_FLTTOFIX_Q16(0.05)
#define SNS_DD_PRX_ACCURACY			40
#define SNS_DD_ALS_ACCURACY_DIV_FACTOR	4
#define SNS_DD_ALS_HALF_LUX			500
#define SNS_DD_ALSPRX_PRX_BITS			8
#define SNS_DD_ALSPRX_PRX_FREQ		30
#define SNS_DD_ALSPRX_ALS_BITS			16
#define SNS_DD_ALSPRX_ALS_FREQ		20

#define CM36283_RANGE_A		0x0000/*ALS_IT:80ms;max lux:6553.5*/
#define CM36283_RANGE_B		0x0010/*ALS_IT:160ms;max lux:3277*/
#define CM36283_RANGE_C		0x0020/*ALS_IT:320ms;max lux:1638*/
#define CM36283_RANGE_D		0x0030/*ALS_IT:640ms;max lux:819*/

/*the range ration may changed when the sensor is covered with lens*/
#define CM36283_RANGE_A_RATION  0.1
#define CM36283_RANGE_B_RATION  0.05
#define CM36283_RANGE_C_RATION  0.025
#define CM36283_RANGE_D_RATION  0.0125

#define SNS_CLR_ALL_INT		0x03
#define SNS_DD_ALSPRX_PWR_DOWN	0x01

#define SNS_ALS_THR_LOW_F		0xffff
#define SNS_ALS_THR_HIG_F		0x0000
#define SNS_ALSPRO_F_ODR		0
#define SNS_ALSPRO_INT_ODR		5
#define SNS_RANGE_MASK			0x00c0
#define CM36283_CHIP_ID         0x0083

/*registers define*/
#define CM36283_ALS_CONF			0x00
#define CM36283_ALS_THDH			0x01
#define CM36283_ALS_THDL			0x02
#define CM36283_PS_CONF1_CONF2	0x03
#define CM36283_PS_CONF3_MS		0x04
#define CM36283_PS_CANC			0x05
#define CM36283_PS_THD				0x06
#define CM36283_PS_DATA			0x08
#define CM36283_ALS_DATA			0x09
#define CM36283_INT_FLAG			0x0B
#define CM36283_DEV_ID				0x0C

#define ALS_CONF_VAL     0x0000
#define ALS_DISABLE		0x0001
#define PS_CONF12_VAL   0x4092
#define PS_DISABLE          0x0001
#define PS_CONF3_VAL     0x0000

#define CM36283_ALS_PWR_DOWN			0x0001
#define CM36283_PRX_PWR_DOWN			0x0001

#define ALS_INT_ENABLE			0x0002
#define PS_INT_DISABLE			0x0000
#define PS_INT_CLOSING			0x0100
#define PS_INT_AWAY				0x0200
#define PS_INT_CLOSING_AWAY	0x0300

#define ALS_INT_FLG				0x3000
#define ALS_INT_FLG_LOW		0x2000
#define ALS_INT_FLG_HIGH		0x1000
#define PS_INT_FLG_CLOSE		0x0200
#define PS_INT_FLG_AWAY		0x0100

#define PS_STEPS_63			0x0000
#define PS_STEPS_127		0x1000
#define PS_STEPS_191		0x2000
#define PS_STEPS_255		0x3000

#define ALS_THRESH_SELECT   20/* set als threshold to be +/- 20% of current value for next interrupt.*/

#define NUM_OF_LUX_TO_AVE  8

typedef enum {
    IRQ_TYPE,
    TIMER_TYPE
} CM36283_SIG_TYPE;

typedef union {
    uint16_t   word;
    uint8_t    byte[2];
} uword_u;

typedef struct  {
    uint16_t data;
    uint32_t lux;
    uint32_t mlux;
    bool     lux_ok;
    uint32_t luxAve;	
} als_info;

/* proximity data */
typedef struct  {
    uint16_t data;
    int16_t  event;
} prox_info;

typedef struct {
  bool  int_enable;	
  uint16_t  enable;
  uint16_t cal_target;
  uint16_t thresh_low;
  uint16_t thresh_high;
  uint16_t range;
  uint32_t odr;
} als_setup;

typedef struct {
  bool  int_enable;	
  uint16_t  enable;
  uint8_t  led_pulse;
  uint8_t  led_ratio;
  uint16_t detect;
  uint16_t release;
  uint8_t thresh_low;
  uint8_t thresh_high;
  uint32_t odr;
  uint16_t steps;
} prox_setup;

typedef struct {
    uint8_t  config;
    uint8_t  control;
} cm36283_reg;

typedef struct  {
    als_setup   als;
    prox_setup  prox;
    cm36283_reg  reg;
} lsc_setup;

typedef struct {
    als_info      als;
    prox_info     prox;
} lsc_cur_info;

typedef struct {
    lsc_cur_info       data;
    lsc_setup          setup;
} cm36283_chip;

typedef enum
{
    SNS_DD_CM36283_SUCCESS = 0,
    SNS_DD_CM36283_ALS_CAL_FAILED_LUX = 1, /*ALS CAL error - Received lux value out of range*/
    SNS_DD_CM36283_ALS_CAL_FAILED_RST = 2, /*ALS CAL error - cannot reset the device*/
    SNS_DD_CM36283_ALS_CAL_FAILED_REG = 3, /*ALS CAL error - cannot write back to registry*/
    SNS_DD_CM36283_ALS_CAL_FAILED_ENB = 4  /*ALS CAL error - cannot enable (error in set power state)*/
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
  uint32_t calLuxLower;     /* ALS value of calibration light lower */
  uint32_t calLuxUpper;     /* ALS value of calibration light upper */
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
   sns_dd_als_db_type           sns_dd_als_db;
   sns_dd_alsprx_common_db_type sns_dd_alsprx_common_db;
   bool                         calibration_mode;    /* calibration mode enable flag */
   uint16_t                     nv_db_size;   /* size of NV item */
   cm36283_chip                 chip;
   sns_ddf_handle_t             ZBW_port_handle;
   uint32_t                     int_gpio;
} sns_dd_alsprx_state_t;

#endif
