/*===========================================================================

         S E N S O R S    G Y R O S C O P E    D R I V E R  

DESCRIPTION

  Defines the interface exposed by the qcom gyroscope driver 


Copyright (c) 2010 - 2012 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_gyro_priv.h#1 $


when         who    what, where, why
--------     ---    ------------------------------------------------------ 
05/24/12     ag     Remove feature FEATURE_MPU3050_SELF_TEST_IMPL
01/20/12     ag     enable reading bias attrib for all implementations of self test 
11/08/11     yk     Added support for axis mapping. 
10/10/11     ag     Change self test implementation to invoke generic inertial test 
09/06/11     ag     Self test related cleanup and changes from code review 
09/06/11     ag     Self test related cleanup 
08/31/11     ag     Self test ported from MPL_4_0_1 for internal use 
08/10/11     ks     added declarations for functions defined in sns_dd_gyro.c & sns_dd_gyro_ext.c 
11/03/11     ad     implement invensense app note for device bug workaround
                    AN-MPU3000A-18 & AN-MPU-3000A-02-01
11/10/10     ad     add support for raw sensor value bounds and resolution
10/18/10     ad     added device clock source types
10/08/10     ad     added gyroscope and temperature data scaling
09/28/10     ad     initial version
==========================================================================*/

#ifndef _SNSDGYROPRIV_H
#define _SNSDGYROPRIV_H

#include "fixed_point.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_util.h"

//Gyroscope sensor defines
#define GYRO_3050_I2C_ADDR                    0x68

//registers
#define GYRO_3050_REG_I2C_ADDR                0x00
#define GYRO_3050_REG_PRODUCT_ID              0x01
#define GYRO_3050_REG_XG_OFFS_TC              0x05
#define GYRO_3050_REG_YG_OFFS_TC              0x08
#define GYRO_3050_REG_ZG_OFFS_TC              0x0b
#define GYRO_3050_REG_X_OFFS_USRH             0x0c
#define GYRO_3050_REG_X_OFFS_USRL             0x0d
#define GYRO_3050_REG_Y_OFFS_USRH             0x0e
#define GYRO_3050_REG_Y_OFFS_USRL             0x0f
#define GYRO_3050_REG_Z_OFFS_USRH             0x10
#define GYRO_3050_REG_Z_OFFS_USRL             0x11
#define GYRO_3050_REG_FIFO_EN1                0x12
#define GYRO_3050_REG_FIFO_EN2                0x13
#define GYRO_3050_REG_AUX_SLV_ADDR            0x14
#define GYRO_3050_REG_SMPLRT_DIV              0x15
#define GYRO_3050_REG_DLPF_FS_SYNC            0x16
#define GYRO_3050_REG_INT_CFG                 0x17
#define GYRO_3050_REG_ACCEL_BURST_ADDR        0x18
#define GYRO_3050_REG_INT_STATUS              0x1a
#define GYRO_3050_REG_TEMP_OUT_H              0x1b
#define GYRO_3050_REG_TEMP_OUT_L              0x1c
#define GYRO_3050_REG_GYRO_XOUT_H             0x1d
#define GYRO_3050_REG_GYRO_XOUT_L             0x1e
#define GYRO_3050_REG_GYRO_YOUT_H             0x1f
#define GYRO_3050_REG_GYRO_YOUT_L             0x20
#define GYRO_3050_REG_GYRO_ZOUT_H             0x21
#define GYRO_3050_REG_GYRO_ZOUT_L             0x22
#define GYRO_3050_REG_DMP_CFG_1               0x35
#define GYRO_3050_REG_DMP_CFG_2               0x36
#define GYRO_3050_REG_BANK_SEL                0x37
#define GYRO_3050_REG_MEM_START_ADDR          0x38
#define GYRO_3050_REG_MEM_R_W                 0x39
#define GYRO_3050_REG_FIFO_COUNTH             0x3a
#define GYRO_3050_REG_FIFO_COUNTL             0x3b
#define GYRO_3050_REG_FIFO_R_W                0x3c
#define GYRO_3050_REG_USER_CTRL               0x3d
#define GYRO_3050_REG_PWR_MGM                 0x3e

//BIT definitions

//GYRO_3050_REG_FIFO_EN1 register (12)
#define GYRO_3050_BIT_TEMP_OUT                0x80
#define GYRO_3050_BIT_GYRO_XOUT               0x40
#define GYRO_3050_BIT_GYRO_YOUT               0x20
#define GYRO_3050_BIT_GYRO_ZOUT               0x10
#define GYRO_3050_BIT_ACCEL_XOUT              0x08
#define GYRO_3050_BIT_ACCEL_YOUT              0x04
#define GYRO_3050_BIT_ACCEL_ZOUT              0x02
#define GYRO_3050_BIT_AUX_1OUT                0x01

//GYRO_3050_REG_FIFO_EN2 register (13)
#define GYRO_3050_BIT_AUX_2OUT                0x02
#define GYRO_3050_BIT_AUX_3OUT                0x01

//GYRO_3050_REG_DLPF_FS_SYNC register (16)
#define GYRO_3050_BITS_EXT_SYNC_NONE          0x00
//#define GYRO_3050_BITS_EXT_SYNC_TEMP          0x20
//#define GYRO_3050_BITS_EXT_SYNC_GYROX         0x40
//#define GYRO_3050_BITS_EXT_SYNC_GYROY         0x60
//#define GYRO_3050_BITS_EXT_SYNC_GYROZ         0x80
//#define GYRO_3050_BITS_EXT_SYNC_ACCELX        0xA0
//#define GYRO_3050_BITS_EXT_SYNC_ACCELY        0xC0
//#define GYRO_3050_BITS_EXT_SYNC_ACCELZ        0xE0
#define GYRO_3050_BITS_EXT_SYNC_MASK          0xE0

#define GYRO_3050_BITS_FS_250DPS              0x00
#define GYRO_3050_BITS_FS_500DPS              0x08
#define GYRO_3050_BITS_FS_1000DPS             0x10
#define GYRO_3050_BITS_FS_2000DPS             0x18
#define GYRO_3050_BITS_FS_MASK                0x18

#define GYRO_3050_BITS_DLPF_CFG_256HZ_NOLPF2  0x00
#define GYRO_3050_BITS_DLPF_CFG_188HZ         0x01
#define GYRO_3050_BITS_DLPF_CFG_98HZ          0x02
#define GYRO_3050_BITS_DLPF_CFG_42HZ          0x03
#define GYRO_3050_BITS_DLPF_CFG_20HZ          0x04
#define GYRO_3050_BITS_DLPF_CFG_10HZ          0x05
#define GYRO_3050_BITS_DLPF_CFG_5HZ           0x06
#define GYRO_3050_BITS_DLPF_CFG_MASK          0x07

//GYRO_3050_REG_INT_CFG (17)
#define GYRO_3050_BIT_ACTL                    0x80
#define GYRO_3050_BIT_ACTL_LOW                0x80
#define GYRO_3050_BIT_ACTL_HIGH               0x00

#define GYRO_3050_BIT_OPEN                    0x40
#define GYRO_3050_BIT_OPEN_DRAIN              0x40
#define GYRO_3050_BIT_PUSH_PULL               0x00

#define GYRO_3050_BIT_LATCH_INT_EN            0x20
#define GYRO_3050_BIT_INT_PULSE_WIDTH_50US    0x00

#define GYRO_3050_BIT_INT_ANYRD_2CLEAR        0x10
#define GYRO_3050_BIT_INT_STAT_READ_2CLEAR    0x00

#define GYRO_3050_BIT_MPU_RDY_EN              0x04
#define GYRO_3050_BIT_DMP_INT_EN              0x02
#define GYRO_3050_BIT_RAW_RDY_EN              0x01

//GYRO_3050_REG_INT_STATUS (1A)
#define GYRO_3050_BIT_INT_STATUS_FIFO_OVERLOW 0x80
#define GYRO_3050_BIT_MPU_RDY                 0x04
#define GYRO_3050_BIT_DMP_INT                 0x02
#define GYRO_3050_BIT_RAW_RDY                 0x01

//GYRO_3050_REG_BANK_SEL register (37)
//#define GYRO_3050_BIT_PRFTCH_EN               0x20
//#define GYRO_3050_BIT_CFG_USER_BANK           0x10
//#define GYRO_3050_BITS_MEM_SEL                0x0f

//GYRO_3050_REG_USER_CTRL register (3D)

//GYRO_3050_REG_PWR_MGM register (3E)
#define GYRO_3050_BIT_H_RESET                 0x80
#define GYRO_3050_BIT_SLEEP                   0x40

#define GYRO_3050_BIT_STBY_XG                 0x20
#define GYRO_3050_BIT_STBY_YG                 0x10
#define GYRO_3050_BIT_STBY_ZG                 0x08

#define GYRO_3050_BITS_CLKSEL                 0x07

//Gyroscope data limit in rad/sec in Q16 format
#define GYRO_3050_CONV_DPS_TO_RPSQ16(x)      FX_FLTTOFIX_Q16((x) * PI / 180.0)

//Gyroscope data scale
#define GYRO_3050_LSB_PER_DPS_SF             131.0
#define GYRO_3050_LSB_PER_DPS_SF_COR         115.0
#define GYRO_3050_DATA_SCALE(x,y)            ((FX_FLTTOFIX_Q16((y) * PI / 180.0 / (x))) << 9)

//Temperature measurement limits in degrees
#define GYRO_3050_FS_TEMP_MIN                FX_CONV_Q16(-30,0)
#define GYRO_3050_FS_TEMP_MAX                FX_CONV_Q16(85,0)

//Temperature data scale and offset
#define GYRO_3050_TEMP_OFFSET_LSB            -13200
#define GYRO_3050_TEMP_OFFSET_CELSIUS        FX_CONV_Q16(35,0)
#define GYRO_3050_TEMP_SCALE                 FX_FLTTOFIX_Q16(1.0/280.0)

//Startup delay of gyroscope is 50 millisec
#define GYRO_3050_STARTUP_DELAY_MICROSEC     50000
//Expected delay for gyroscope data is 0.5 millisec
#define GYRO_3050_DATA_DELAY_MICROSEC        500

//Raw data value limits
#define GYRO_3050_MIN_RAW_DATA_VALUE         0xFFFF8000
#define GYRO_3050_MAX_RAW_DATA_VALUE         0x7FFF

//Raw data bit length
#define GYRO_3050_RAW_DATA_BIT_LEN           16


// Definitions for sns_dd_gyro.c & sns_dd_gyro_ext.c

//Number of supported gyroscope ranges
#define GYRO_3050_NUM_FS_RANGES           4
//Default gyroscope range
#define GYRO_3050_DEF_RANGE_INDEX         2   /*GYRO_3050_BITS_FS_1000DPS*/

//Number of supported gyroscope low pass filter bandwidths
#define GYRO_3050_NUM_DLPF_BW             7
//Default gyroscope low pass filter bandwidth
#define GYRO_3050_DEF_DLPF_BW_INDEX       4   /*GYRO_3050_BITS_DLPF_CFG_98HZ*/

//Device clock source
typedef enum
{
    SDD_GYRO_CLK_SEL_INTERNAL = 0,
    SDD_GYRO_CLK_SEL_PLLGYROX,
    SDD_GYRO_CLK_SEL_PLLGYROY,
    SDD_GYRO_CLK_SEL_PLLGYROZ,
    SDD_GYRO_CLK_SEL_PLLEXT32K,
    SDD_GYRO_CLK_SEL_PLLEXT19M,
    SDD_GYRO_CLK_SEL_RESERVED,
    SDD_GYRO_CLK_SEL_STOP,
    SDD_GYRO_NUM_CLK_SEL
} sns_dd_gyro_clock_sel_e;

//Supported datatypes
typedef enum
{
   SDD_GYRO_X = 0,
   SDD_GYRO_Y,
   SDD_GYRO_Z,
   SDD_GYRO_NUM_AXES,
   SDD_TEMP = SDD_GYRO_NUM_AXES,
   SDD_GYRO_NUM_DATATYPES
} sns_dd_gyro_data_type_e;

// Definitions for sns_dd_gyro.c & sns_dd_gyro_ext.c

/* State struct for driver */
typedef struct
{
   sns_ddf_handle_t smgr_hndl;         /*SDDI handle used to notify_data*/
   q16_t            data_cache[SDD_GYRO_NUM_DATATYPES]; /*data cache*/
   sns_ddf_status_e data_status[SDD_GYRO_NUM_DATATYPES]; 
                                       /*status of data in cache*/
   sns_ddf_handle_t port_handle;       /*handle used to access the I2C bus*/
   uint8_t          range;             /*current range selection*/
   uint8_t          lpf_bw;            /*current lowpass filter BW*/
   sns_ddf_axes_map_s axes_map;
   int32_t           biases[SDD_GYRO_NUM_AXES];
} sns_dd_gyro3050_state_s;

// Function Declarations for sns_dd_gyro.c & sns_dd_gyro_ext.c

sns_ddf_status_e sns_dd_gyro3050_init(
   sns_ddf_handle_t*,
   sns_ddf_handle_t,
   sns_ddf_nv_params_s*,
   sns_ddf_device_access_s*,
   uint32_t,
   sns_ddf_memhandler_s*,
   sns_ddf_sensor_e**,
   uint32_t*);

sns_ddf_status_e sns_dd_gyro3050_get_attr(
   sns_ddf_handle_t,
   sns_ddf_sensor_e,
   sns_ddf_attribute_e,
   sns_ddf_memhandler_s*,
   void**,
   uint32_t*);

sns_ddf_status_e sns_dd_gyro3050_reset(sns_ddf_handle_t);

#endif /* SNSDGYROPRIV_H */
