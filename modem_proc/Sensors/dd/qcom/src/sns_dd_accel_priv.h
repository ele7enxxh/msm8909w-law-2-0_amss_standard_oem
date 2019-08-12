/*===========================================================================

         S E N S O R S    A C C E L E R O M E T E R    D R I V E R  

DESCRIPTION

  Defines the interface exposed by the qcom accelerometer driver 


Copyright (c) 2010, 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_priv.h#1 $


when           who     what, where, why
----------     ---     ------------------------------------------------------
06/25/10     jww     Ported from Sensors Framework for AMSS 
10/26/10     RK      Updated some constants that were required for out 
                     of bound check 
01/20/11     br      Updated definitions for attrubutes: Ranges for accel and temp     
01/26/11     br      used FX_FLTTOFIX_Q16 instead of FX_FLTTOFIX
==========================================================================*/

#ifndef _SNSDACCELPRIV_H
#define _SNSDACCELPRIV_H

#include "fixed_point.h"

/* BOSCH Accelerometer Sensor defines */
#define BOSCH_ACCEL_SENSOR_I2C_ADDR             0x70
#define BOSCH_ACCEL_SENSOR_ID_REG_ADDR          0x00

#define BOSCH_ACCEL_SENSOR_CTRL1_REG_ADDR       0x15
#define BOSCH_ACCEL_SENSOR_ENABLE_ADV_INT       0x40     /* B6 */
#define BOSCH_ACCEL_SENSOR_LATCH_INT            0x10     /* B4 */
#define BOSCH_ACCEL_SENSOR_SHADOW_DISABLE       0x08     /* B3 */
#define BOSCH_ACCEL_SENSOR_WAKEUP_PAUSE_2560mS  0x06     /* (B2:1) */
#define BOSCH_ACCEL_SENSOR_WAKEUP_PAUSE_320mS   0x04     /* (B2:1) */     
#define BOSCH_ACCEL_SENSOR_WAKEUP_PAUSE_80mS    0x02     /* (B2:1) */     
#define BOSCH_ACCEL_SENSOR_WAKEUP_PAUSE_20mS    0        /* (B2:1) */
#define BOSCH_ACCEL_SENSOR_WAKEUP               0x01     /* B0 */

#define BOSCH_ACCEL_SENSOR_CTRL2_REG_ADDR       0x14
#define BOSCH_ACCEL_CTRL2_RESERVED_BITS_MASK    0xE0
#define BOSCH_ACCEL_SENSOR_RANGE_2G             0        /* (B4:3) */
#define BOSCH_ACCEL_SENSOR_RANGE_4G             0x08     /* (B4:3) */
#define BOSCH_ACCEL_SENSOR_RANGE_8G             0x10     /* (B4:3) */
#define BOSCH_ACCEL_SENSOR_BW_BITMAP_25Hz       0        /* (B2:0) */
#define BOSCH_ACCEL_SENSOR_BW_BITMAP_50Hz       1        /* (B2:0) */
#define BOSCH_ACCEL_SENSOR_BW_BITMAP_100Hz      2        /* (B2:0) */
#define BOSCH_ACCEL_SENSOR_BW_BITMAP_190Hz      3        /* (B2:0) */
#define BOSCH_ACCEL_SENSOR_BW_BITMAP_375Hz      4        /* (B2:0) */
#define BOSCH_ACCEL_SENSOR_BW_BITMAP_750Hz      5        /* (B2:0) */
#define BOSCH_ACCEL_SENSOR_BW_BITMAP_1500Hz     6        /* (B2:0) */

#define BOSCH_ACCEL_SENSOR_CTRL3_REG_ADDR       0x0A
#define BOSCH_ACCEL_SENSOR_RESET_INT            0x40     /* B6 */
#define BOSCH_ACCEL_SENSOR_EE_W                 0x10     /* B4 */
#define BOSCH_ACCEL_SENSOR_SOFTRESET            0x02     /* B1 */
#define BOSCH_ACCEL_SENSOR_SLEEP_MODE           0x01     /* B0 */
#define BOSCH_ACCEL_SENSOR_NORMAL_MODE          0

#define BOSCH_ACCEL_SENSOR_CTRL4_REG_ADDR       0x0B
#define BOSCH_ACCEL_SENSOR_ENABLE_LG            0x01     /* B0 */
#define BOSCH_ACCEL_SENSOR_ENABLE_HG            0x02     /* B1 */
#define BOSCH_ACCEL_SENSOR_ANY_MOTION           0x40     /* B6 */
#define BOSCH_ACCEL_SENSOR_ALERT                0x80     /* B7 */

#define BOSCH_ACCEL_SENSOR_CTRL5_REG_ADDR       0x10

#define BOSCH_ACCEL_SENSOR_CTRL6_REG_ADDR       0x11
#define BOSCH_ACCEL_SENSOR_ANY_MOTION_DUR_1_SEQ 0        /* (B7:6) */
#define BOSCH_ACCEL_SENSOR_ANY_MOTION_DUR_3_SEQ 0x40     /* (B7:6) */
#define BOSCH_ACCEL_SENSOR_ANY_MOTION_DUR_5_SEQ 0x80     /* (B7:6) */
#define BOSCH_ACCEL_SENSOR_ANY_MOTION_DUR_7_SEQ 0xC0     /* (B7:6) */

#define BOSCH_ACCEL_SENSOR_TEMP_REG_ADDR        0x08
#define BOSCH_ACCEL_SENSOR_TEMP_DEG_PER_LSB     0.5        /* .5C per LSB */
#define BOSCH_ACCEL_SENSOR_TEMP_OFFSET_DEG      30        /* 0 = -30C */
#define BOSCH_ACCEL_SENSOR_Z_MSB_REG_ADDR       0x07
#define BOSCH_ACCEL_SENSOR_Z_LSB_REG_ADDR       0x06
#define BOSCH_ACCEL_SENSOR_Z_STATUS_REG_ADDR    0x06
#define BOSCH_ACCEL_SENSOR_Y_MSB_REG_ADDR       0x05
#define BOSCH_ACCEL_SENSOR_Y_LSB_REG_ADDR       0x04
#define BOSCH_ACCEL_SENSOR_Y_STATUS_REG_ADDR    0x04
#define BOSCH_ACCEL_SENSOR_X_MSB_REG_ADDR       0x03
#define BOSCH_ACCEL_SENSOR_MSB_SHIFT_LEFT       2
#define BOSCH_ACCEL_SENSOR_X_LSB_REG_ADDR       0x02
#define BOSCH_ACCEL_SENSOR_LSB_MASK             (0x80 | 0x40)  /* (B7|B6) */
#define BOSCH_ACCEL_SENSOR_LSB_SHIFT_RIGHT      6    
#define BOSCH_ACCEL_SENSOR_X_STATUS_REG_ADDR    0x02
#define BOSCH_ACCEL_SENSOR_NEW_DATA_MASK        0x01     /* B0 */
#define BOSCH_ACCEL_SENSOR_GAIN_T_REG_ADDR      0x19
#define BOSCH_ACCEL_SENSOR_GAIN_T_MASK          0x3F    /* bit 5:0 */
#define BOSCH_ACCEL_SENSOR_LSB_T_OFFSET_MASK    0x03    /* bits 1:0 */
#define BOSCH_ACCEL_SENSOR_LSB_T_OFFSET_L_SHIFT 6        /* in bits 7:6 */
#define BOSCH_ACCEL_SENSOR_OFFSET_T_REG_ADDR    0x1d
#define BOSCH_ACCEL_SENSOR_MSB_T_OFFSET_MASK    0x3fc    /* bit 9:2 */
#define BOSCH_ACCEL_SENSOR_MSB_T_OFFSET_R_SHIFT 2        /* in bits 7:0 */

/* Min/Max accel in milli-g for different range */
#define BOSCH_ACCEL_SENSOR_RANGE_2G_MAX         FX_FLTTOFIX_Q16(2*9.81)
#define BOSCH_ACCEL_SENSOR_RANGE_2G_MIN         FX_FLTTOFIX_Q16(-2*9.81)
#define BOSCH_ACCEL_SENSOR_RANGE_4G_MAX         FX_FLTTOFIX_Q16(4*9.81)
#define BOSCH_ACCEL_SENSOR_RANGE_4G_MIN         FX_FLTTOFIX_Q16(-4*9.81)
#define BOSCH_ACCEL_SENSOR_RANGE_8G_MAX         FX_FLTTOFIX_Q16(8*9.81)
#define BOSCH_ACCEL_SENSOR_RANGE_8G_MIN         FX_FLTTOFIX_Q16(-8*9.81)
#define BOSCH_ACCEL_SENSOR_TEMPER_MIN           FX_FLTTOFIX_Q16(-40)
#define BOSCH_ACCEL_SENSOR_TEMPER_MAX           FX_FLTTOFIX_Q16(97.5)
#define SNS_DD_ACCEL_MAX_RANGE                  3
#define BOSCH_ACCEL_SENSOR_BW_VALUE_25Hz        25        
#define BOSCH_ACCEL_SENSOR_BW_VALUE_50Hz        50        
#define BOSCH_ACCEL_SENSOR_BW_VALUE_100Hz       100       
#define BOSCH_ACCEL_SENSOR_BW_VALUE_190Hz       190       
#define BOSCH_ACCEL_SENSOR_BW_VALUE_375Hz       375       
#define BOSCH_ACCEL_SENSOR_BW_VALUE_750Hz       750       
#define BOSCH_ACCEL_SENSOR_BW_VALUE_1500Hz      1500      

/* Resolution for different range in milli-g for accel or 1/10 deg C for temp */
#define BOSCH_ACCEL_SENSOR_RANGE_2G_RES         4
#define BOSCH_ACCEL_SENSOR_RANGE_4G_RES         8
#define BOSCH_ACCEL_SENSOR_RANGE_8G_RES         16
#define BOSCH_ACCEL_SENSOR_TEMPER_RES           5

#define BOSCH_ACCEL_SENSOR_ACC_BIT_LEN          10
#define BOSCH_ACCEL_SENSOR_ACC_CONV_TIME        333
#define BOSCH_ACCEL_SENSOR_TEMPER_BIT_LEN       8
#define BOSCH_ACCEL_SENSOR_TEMPER_CONV_TIME     333

#define BOSCH_ACCEL_MIN_RAW_DATA_VALUE 0xFFFFFE00
#define BOSCH_ACCEL_MAX_RAW_DATA_VALUE 0x1FF
#define BOSCH_TEMP_MAX_RAW_DATA_VALUE 0xFF
#define BOSCH_TEMP_MIN_RAW_DATA_VALUE 0x0


#endif /* End include guard  SNSDACCELPRIV_H */
