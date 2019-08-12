/*  Date: 2013/07/25 10:00:00
 *  Revision: 1.0
 */

/*==============================================================================

    S E N S O R S   ACCELEROMETER  D R I V E R

DESCRIPTION

   Implements the driver for the KXTIK/kxtj2 accelerometer Sensor

   Copyright (c) 2013-2014 by Qualcomm Technologies, Inc.  All Rights Reserved
   Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/pkg/dsps/dev/yihengz.adsp821_3/adsp_proc/Sensors/dd/qcom/src/sns_dd_kxtik.h


when         who     what, where, why
----------   ---     -----------------------------------------------------------
03/12/14     MW      Fixed KW issue
09/05/13     yh      Initial version of driver for kxtik
==============================================================================*/

#ifndef _SNSDACCELPRIV_H
#define _SNSDACCELPRIV_H

#include "fixed_point.h"


#define LOW_CURRENT 0
#define HIGH_CURRENT 1
#define KXTIK_OUTX_L 0x06

#define KXTIK_ACCEL_SENSOR_RANGE_2G_MAX         FX_FLTTOFIX_Q16(2*9.81)
#define KXTIK_ACCEL_SENSOR_RANGE_2G_MIN         FX_FLTTOFIX_Q16(-2*9.81)
#define KXTIK_ACCEL_SENSOR_RANGE_4G_MAX         FX_FLTTOFIX_Q16(4*9.81)
#define KXTIK_ACCEL_SENSOR_RANGE_4G_MIN         FX_FLTTOFIX_Q16(-4*9.81)
#define KXTIK_ACCEL_SENSOR_RANGE_8G_MAX         FX_FLTTOFIX_Q16(8*9.81)
#define KXTIK_ACCEL_SENSOR_RANGE_8G_MIN         FX_FLTTOFIX_Q16(-8*9.81)

#define KXTIK_ACCEL_SENSOR_ACC_BIT_LEN 12

#define KXTIK_BW_7_81HZ        0x08
#define KXTIK_BW_15_63HZ       0x09
#define KXTIK_BW_31_25HZ       0x0A
#define KXTIK_BW_62_50HZ       0x0B
#define KXTIK_BW_12_5HZ         0x00
#define KXTIK_BW_25HZ         0x01
#define KXTIK_BW_50HZ         0x02
#define KXTIK_BW_100HZ        0x03
#define KXTIK_BW_200HZ        0x04
#define KXTIK_BW_400HZ        0x05
#define KXTIK_BW_800HZ        0x06
#define KXTIK_BW_1600HZ       0x07

    
#define KXTIK_ACCEL_SENSOR_BW_VALUE_7_81Hz      FX_FLTTOFIX_Q16(0.781)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_15_63Hz     FX_FLTTOFIX_Q16(1.563)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_31_25Hz     FX_FLTTOFIX_Q16(3.125)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_62_50Hz     FX_FLTTOFIX_Q16(6.25)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_12_5Hz       FX_FLTTOFIX_Q16(6.75)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_25Hz       FX_FLTTOFIX_Q16(12.5)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_50Hz       FX_FLTTOFIX_Q16(25)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_100Hz      FX_FLTTOFIX_Q16(50)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_200Hz      FX_FLTTOFIX_Q16(100)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_400Hz      FX_FLTTOFIX_Q16(200)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_800Hz      FX_FLTTOFIX_Q16(400)
#define KXTIK_ACCEL_SENSOR_BW_VALUE_1600Hz     FX_FLTTOFIX_Q16(800)

#define KXTIK_IEA			    0x10
#define KXTIK_IEL				0x08
#define KXTIK_IEN				0x20
#define KXTIK_INT_ENABLE 		0x38
#define KXTIK_INT_ENABLE2 		0xE0
#define KXTIK_WAKEUP_ODR        0x01
#define KXTIK_WAKEUP_TIMER		0x0A
#define KXTIK_WAKEUP_THRESHOLD  0x04 //0.25g
#define KXTIK_MOTION_DETECT     0xC2
#define KXTIK_SELFTEST		    0xCA
#define KXTIK_OFFSETX			(4.9)
#define KXTIK_OFFSETX1			(3.92)
#define KXTIK_OFFSETY			(0)
#define KXTIK_OFFSETY1			(4.9)
#define KXTIK_OFFSETZ			(0.98)	
#define KXTIK_OFFSETZ1			(5.88)
#define MAX_LSB_14BIT			16383
#define MAX_LSB_12BIT			4095
#define MAX_LSB_10BIT			1023
#define MAX_LSB_8BIT			255
#define KXTIK					0x05



#define KXTIK_CTRL_REG1                         0x1B
#define KXTIK_CTRL_REG2							0x1D
#define KXTIK_BW_SEL_REG                        0x21
#define KXTIK_INT_CTRL_REG 						0x1E
#define KXTIK_INT_CTRL_REG2 					0x1F
#define KXTIK_WAKEUP_TIMER_REG					0x29
#define KXTIK_WAKEUP_THRESHOLD_REG				0x6A
#define KXTIK_SELFTEST_REG						0x3A
#define KXTIK_INT_REL							0x1A
#define KXTIK_INT_SOURCE						0x16
#define KXTIK_CHIP_ID_REG                       0x0F 



#define KXTIK_ACCEL_SENSOR_SLEEP_MODE           0x00     
#define KXTIK_ACCEL_SENSOR_NORMAL_MODE          0x40

#define SNS_DD_ACCEL_MAX_RANGE                  3
#define SNS_DD_ACCEL_MAX_BW                     KXTIK_BW_1600HZ

#define KXTIK_ACCEL_SENSOR_RANGE_2G             0       
#define KXTIK_ACCEL_SENSOR_RANGE_4G             8     
#define KXTIK_ACCEL_SENSOR_RANGE_8G             16     

#define KIONIX_KXTIK_ODR_NUM         8
#define KIONIX_KXTIK_MAX_ODR         1600




#endif /*! End include guard  SNSDACCELPRIV_H */
