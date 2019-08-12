/*  Date: 2013/07/25 10:00:00
 *  Revision: 1.0
 */

/*==============================================================================

    S E N S O R S   ACCELEROMETER  D R I V E R

DESCRIPTION

   Implements the driver for the kxcjk/kxtj2 accelerometer Sensor

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_kxcjk.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
10/18/13     MW      Fix KW issues
07/25/13     yh      Initial version of driver for kxcjk
==============================================================================*/

#ifndef _SNSDACCELPRIV_H
#define _SNSDACCELPRIV_H

#include "fixed_point.h"


#define LOW_CURRENT 0
#define HIGH_CURRENT 1
#define KXCJK_OUTX_L 0x06

#define KXCJK_ACCEL_SENSOR_RANGE_2G_MAX         FX_FLTTOFIX_Q16(2*9.81)
#define KXCJK_ACCEL_SENSOR_RANGE_2G_MIN         FX_FLTTOFIX_Q16(-2*9.81)
#define KXCJK_ACCEL_SENSOR_RANGE_4G_MAX         FX_FLTTOFIX_Q16(4*9.81)
#define KXCJK_ACCEL_SENSOR_RANGE_4G_MIN         FX_FLTTOFIX_Q16(-4*9.81)
#define KXCJK_ACCEL_SENSOR_RANGE_8G_MAX         FX_FLTTOFIX_Q16(8*9.81)
#define KXCJK_ACCEL_SENSOR_RANGE_8G_MIN         FX_FLTTOFIX_Q16(-8*9.81)

#define KXCJK_ACCEL_SENSOR_ACC_BIT_LEN 12

#define KXCJK_BW_7_81HZ        0x08
#define KXCJK_BW_15_63HZ       0x09
#define KXCJK_BW_31_25HZ       0x0A
#define KXCJK_BW_62_50HZ       0x0B
#define KXCJK_BW_12_5HZ         0x00
#define KXCJK_BW_25HZ         0x01
#define KXCJK_BW_50HZ         0x02
#define KXCJK_BW_100HZ        0x03
#define KXCJK_BW_200HZ        0x04
#define KXCJK_BW_400HZ        0x05
#define KXCJK_BW_800HZ        0x06
#define KXCJK_BW_1600HZ       0x07

    
#define KXCJK_ACCEL_SENSOR_BW_VALUE_7_81Hz      FX_FLTTOFIX_Q16(0.781)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_15_63Hz     FX_FLTTOFIX_Q16(1.563)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_31_25Hz     FX_FLTTOFIX_Q16(3.125)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_62_50Hz     FX_FLTTOFIX_Q16(6.25)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_12_5Hz       FX_FLTTOFIX_Q16(6.75)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_25Hz       FX_FLTTOFIX_Q16(12.5)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_50Hz       FX_FLTTOFIX_Q16(25)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_100Hz      FX_FLTTOFIX_Q16(50)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_200Hz      FX_FLTTOFIX_Q16(100)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_400Hz      FX_FLTTOFIX_Q16(200)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_800Hz      FX_FLTTOFIX_Q16(400)
#define KXCJK_ACCEL_SENSOR_BW_VALUE_1600Hz     FX_FLTTOFIX_Q16(800)

#define KXCJK_IEA			    0x10
#define KXCJK_IEL				0x08
#define KXCJK_IEN				0x20
#define KXCJK_INT_ENABLE 		0x38
#define KXCJK_INT_ENABLE2 		0x3F
#define KXCJK_WAKEUP_ODR        0x07
#define KXCJK_WAKEUP_TIMER		0x0A
#define KXCJK_WAKEUP_THRESHOLD  0x04 //0.25g
#define KXCJK_MOTION_DETECT     0xC2
#define KXCJK_SELFTEST		    0xCA
#define KXCJK_OFFSETX			(4.9)
#define KXCJK_OFFSETX1			(3.92)
#define KXCJK_OFFSETY			(0)
#define KXCJK_OFFSETY1			(4.9)
#define KXCJK_OFFSETZ			(0.98)	
#define KXCJK_OFFSETZ1			(5.88)
#define MAX_LSB_14BIT			16383
#define MAX_LSB_12BIT			4095
#define MAX_LSB_10BIT			1023
#define MAX_LSB_8BIT			255
#define KXCJK					0x11
#define KXTJ2					0x09


#define KXCJK_CTRL_REG1                         0x1B
#define KXCJK_CTRL_REG2							0x1D
#define KXCJK_BW_SEL_REG                        0x21
#define KXCJK_INT_CTRL_REG 						0x1E
#define KXCJK_INT_CTRL_REG2 					0x1F
#define KXCJK_WAKEUP_TIMER_REG					0x29
#define KXCJK_WAKEUP_THRESHOLD_REG				0x6A
#define KXCJK_SELFTEST_REG						0x3A
#define KXCJK_INT_REL							0x1A
#define KXCJK_INT_SOURCE						0x16
#define KXCJK_CHIP_ID_REG                       0x0F 



#define KXCJK_ACCEL_SENSOR_SLEEP_MODE           0x3F     
#define KXCJK_ACCEL_SENSOR_NORMAL_MODE          0x40

#define SNS_DD_ACCEL_MAX_RANGE                  3
#define SNS_DD_ACCEL_MAX_BW                     KXCJK_BW_1600HZ

#define KXCJK_ACCEL_SENSOR_RANGE_2G             0       
#define KXCJK_ACCEL_SENSOR_RANGE_4G             8     
#define KXCJK_ACCEL_SENSOR_RANGE_8G             16     

#define KIONIX_KXCJK_ODR_NUM         8
#define KIONIX_KXCJK_MAX_ODR         1600




#endif /*! End include guard  SNSDACCELPRIV_H */
