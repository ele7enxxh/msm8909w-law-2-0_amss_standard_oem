#ifndef SNS_SMGR_SENSOR_CONFIG_H
#define SNS_SMGR_SENSOR_CONFIG_H
/*=============================================================================
  @file sns_smgr_sensor_config.h

  This header file contains definitions of sensor constant parameters that are
  needed to configure the DSPS Sensor Manager (SMGR) for the sensors that are
  in the environment.

******************************************************************************
*   Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
*   Qualcomm Technologies Proprietary and Confidential. 
******************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_sensor_config.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-06-29  agk  Power rail changes/defaults
  2015-05-08  AH   Support to request SNS_REG_GROUP_SSI_SMGR_CFG_3
  2015-03-23  PS   Updated Sensors configuration for 8996 V2 bringup
  2015-02-08  PS   Enable multiple sensors for 8996 SOD bringup 
  2015-01-28  PS   Enable AKM099xx Mag on SPI for 8996
  2015-01-21  jms  SPI and BMI160 enabled on SLPI 1.0
  2014-06-11  ps   update Sensors config for 8996
  2014-06-11  ps   updated mag bus instance for 8994
  2014-06-11  ps   update Sensors config for 8994 platform
  2013-12-30  ps   update Accel driver for standalone mode
  2013-12-17  ps   Branched out from 8974 directory,
                   add more sensors config data for ADSP_STANDALONE mode
  2013-09-12  MW   Changed number of physical sensors available for RGB addition.
  2013-09-12  DC   Updated SNS_SMGR_NUM_SENSORS_DEFINED
  2013-06-23  lka  Changed number of physical sensors available.
  2013-06-11  ae   Added qdsp sim playback support
  2013-05-06  asr  Added flag to enable/disable FIFO. 
  2013-01-09  sd   Added support for SSI
  2012-12-02  pn   Increased idle-to-ready time for MPU6050
  2012-12-02  ag   Workaround for first multi PD release - define ADSP_STANDALONE
  2012-10-19  ag   Enable 6050 DRI
  2012-10-11  sc   Enabled driver registry flags for 8974 target.
  2012-10-03  sc   Scale timestamp back down to 32768Hz tick.
  2012-08-23  ag   Fix timestamps; handle 19.2MHz QTimer
  2012-08-22  sc   Updated bus instances and sensor slave IDs for 8974 target.
  2012-08-14  sc   Updated sensor list for 8974 target bringup.
  2012-08-06  ag   Enable alsprx sensor
  2012-07-26  vh   Enabled three physical Sensors (LIS3DH, MPU3050, AK8975)
  2012-07-02  sc   Use LIS3DH as default accelerometer for QDSP6
                   Added SNS_SMGR_SENSOR_<N>_BUS_INSTANCE for QDSP6
  2012-06-20  dc   Updated macros for BMA250
  2012-05-18  pn   Updated DD_FN_LIST_PTR for AMI306 driver.
  2012-05-17  pn   Featurized MPU6050 and APDS99xx for DRI modes.
  2012-05-16  br   Added more delay for IDLE_TO_READY delay of MPU6050
  2012-04-02  ag   Added proximity calibration support for apds99xx
  2012-03-23  sd   Added GYRO TEMP for MPU6050
  2012-03-18  br   Disabled GYRO TEMP
  2012-03-10  ag   Added DRI support for APDS99xx
  2012-03-06  sd   change the default gpio number from 0 to 0xffff if not used
  2012-01-18  br   added flag definition SNS_SMGR_DD_SELF_SCHED and apply the flag to DDs
  2012-02-27  ry   Added support for APDS99XX and APDS91XX
  2012-01-05  br   changed OFF_TO_IDLE and IDLE_TO_READY for MPU6050
  2011-11-30  sd   added SMGR_MAX_OFF_TO_IDLE
  2011-11-21  yk   Added support for axis mapping
  2011-11-14  ry   Added support for AMI306
  2011-11-11  ad   Added support for platform with no gyro, feature disabled by default
  2011-10-09  br   Added REG definition for MPU6050 for factory calibration
  2011-10-07  br   Added support for MPU6050
  2011-10-05  ry   Added support for AK8963
  2011-09-29  ag   Add registry item type for als/prx sensor
  2011-09-29  sd   changed ST accel idle to ready time to 100ms to fix inaccurate first few samples
                   since off to idle time is used now, reduced MPU 3050 gyro idle to ready time
  2011-09-20  dc   Move MPU3050 from #Else section to CONFIG_USE_MPU3050 section
  2011-09-09  sc   Update with registry service V02
  2011-08-29  sd   Added BMP support
  2011-08-09  rb   updated #defines with new naming conventions & added registry group IDs for gyro factory cal
  2011-08-05  rb   moved sns_smgr_RegItemType_e to sns_smgr.h
  2011-08-03  br   moved SMGR_MAX_DATA_TYPES_PER_DEVICE to sns_smgr.h,
                   and SNS_SMGR_NUM_SENSORS_DEFINED was moved from smgr_define.h
  2011-08-01  br   Rollback the wrong change made in 7/29, and chnaged MPU3050 InvenGyro rang from 500DPS to 1000DPS
  2011-07-29  br   Chnaged MPU3050 InvenGyro rang from 500DPS to 1000DPS
  2011-06-29  sd   Added ST mag/gyro support
  2011-06-20  jh   Enabled AKM8975 mag driver
  2011-06-16  ag   Enabled alsprx driver for ISL29028
  2011-05-25  rk   Zeroing out all the non accel & gyro sensors
  2011-05-23  rk   corrected the STM slave addr to 0x18
  2011-05-12  rk   Added define check for CONFIG_USE_STM & configured the defines for ST accel
  2011-05-11  jb   Add registry type for calibration. Remove unused constants
  2011-04-01  jb   Reenter calibration from registry
  2011-03-25  jh   Revert registry and calibration related changes
  2011-03-24  sj   Changed mag 8973 max frequency to 30Hz
  2011-03-21  jb   Fix #include so symbolic registry IDs can be used
  2011-03-16  jb   Reduce timeout for completing device initialization
                   Add configuration for loading calibration
  2011-03-04  jh   Properly set SNS_SMGR_SENSOR_3_DD_FN_LIST_PTR
                   Fixed indentation
  2011-03-03  as   Add BMP085 Pressure and Temperature sensor (under #ifdef)
  2011-02-23  jb   Add flag allowing ADXL350 driver to use LOWPASS attribute for
                   internal sampling rate
  2011-02-04  ag   AKM8975 magnetometer support (original checkin by osnat)
  2011-02-04  jb   Change flag names to clarify which devices are asynchronous
  2011-01-28  br   changed constant names for deploying QMI tool ver#2
  2011-01-21  sc   Modified some estimated tick values on PCSIM environment
  2011-01-16  as   Added ADXL 350/346 accelerometer driver.
  2010-12-21  jb   increase estimated read time to allow for irregular read length
  2010-11-15  jb   Fix warning
  2010-11-12  sc   Updated magnetometer and PRX/ALS driver configurations.
  2010-10-12  jb   Prepare for prox/ambient
  2010-10-12  jb   Enable magnetometer
  2010-10-11  jb   Make sensitivity and automatic range selection optional.
  2010-10-07  ad   added gyro driver configuration
  2010-10-05  jb   CC3 initial build
  2010-08-20  JB   Add LPF and dynamic range
  2010-08-04  JB   Merge with revised ucos, use PC timer
  2010-07-16  JB   Add sensor data types to constant table
  2010-07-09  JB   Add to sensor config data as needed for cycle planning
  2010-06-11  JB   Partial code as needed for SOL unit testing

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include  "comdef.h" /* For PACK */
#include "sns_reg_common.h"
#include "sns_reg_api_v02.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Number of physical sensors defined in this build */
#ifdef SNS_QDSP_SIM
 #define SNS_SMGR_NUM_SENSORS_DEFINED         1
#else
#ifdef VIRTIO_BRINGUP
 #define SNS_SMGR_NUM_SENSORS_DEFINED         8
#else
 #define SNS_SMGR_NUM_SENSORS_DEFINED         12
#endif /* VIRTIO_BRINGUP */
#endif /* SNS_QDSP_SIM */

 /* Note that message format limits the number of sample values for each item to 3 */
 #define SMGR_MAX_VALUES_PER_DATA_TYPE        3

 #define SMGR_MAX_LPF_FREQ_IN_SENSOR          10

 #define SMGR_MAX_RANGES_IN_SENSOR            5

 /* Timeout for overall device driver initialization process, including
    retrieval of registry data */
 #define SMGR_DD_INIT_TIMEOUT_MSEC           10000    /* 10 seconds */
 #define SMGR_DD_INIT_TIMEOUT_TICK (uint32_t)(1000*SMGR_DD_INIT_TIMEOUT_MSEC/SNS_SMGR_USEC_PER_TICK)

 #ifdef ADSP_STANDALONE
 #define SNS_SMGR_REG_ITEM_TYPE_GROUP_OR_NONE SNS_SMGR_REG_ITEM_TYPE_NONE
 #else
 #define SNS_SMGR_REG_ITEM_TYPE_GROUP_OR_NONE SNS_SMGR_REG_ITEM_TYPE_GROUP
 #endif

 /* Estimate the average number of ticks required to execute key sections of
    SMGR code. The values are based on tuning experience. These estimates are
    used for initialization only, once started, SMGR maintains a running
    average for each code section.  The sections are: Collection, Reporting,
    Message, and Scheduling. Collection accounts for processing, e.g. filtering,
    data after it is read. Reporting accounts for generating reports. Message
    accounts for processing a message into the SOL. Scheduling accounts for
    resolving sample and report rates as the report mix changes. */
#ifdef SNS_PCSIM

#define SMGR_AVG_TICKS_COLLECTING             2
#define SMGR_AVG_TICKS_REPORTING              2
#define SMGR_AVG_TICKS_MESSAGE                2
#define SMGR_AVG_TICKS_SCHEDULING             7

#else

#define SMGR_AVG_TICKS_COLLECTING             6
#define SMGR_AVG_TICKS_REPORTING              6
#define SMGR_AVG_TICKS_MESSAGE                6
#define SMGR_AVG_TICKS_SCHEDULING             20

#endif

  /* Estimate the task time to sample a long-read device. Such a device is
     first commanded to start sampling, then the device delays, then the result
     is read. The task time includes the blocking bus time for the command, the
     blocking bus time for reading the result, plus an allowance for context
     switching and other device driver overhead. */
#define SMGR_LONG_READ_USEC                   1700

/* same as MPU3050 gyro */
#define SMGR_MAX_OFF_TO_IDLE                 150000



 /* Enumeration of data types for use with SNS_SMGR_SENSOR_xx_DATA_TYPE_y
    Null                    =0
    Acceleration 3 axis     =1
    Gyro turn rate 3 axis   =2
    Magnetic flux 3 axis    =3
    Atmospheric pressure    =4
    Proximity               =5
    Ambient light           =6
    Temperature             =7
    Humidity                =27
    IR Gesture              =12
	RGB						=28
 */


/*  The sensor devices are in order by criticality of scheduling accuracy. Device 0
    will be sampled close to the heartbeat with the most consistency. Other
    devices will typically have more jitter in sampling time when devices must
    be sampled at different rates. */

/*  The enumerated name of this sensor device, used in client request message
#define SNS_SMGR_SENSOR_xx_ENUM_CODE

    The pointer to the device driver function list, or zero if the driver is not
    present.  A given driver function list may be listed for multiple devices.
#define SNS_SMGR_SENSOR_xx_DD_FN_LIST_PTR

    Identify the specific device driven by the driver. The device driver
    defines an ID for each device it is driving.  This appears as device_select
    in sns_ddf_device_access_s, defined in sns_ddf_driver_if.h
#define SNS_SMGR_SENSOR_xx_DEVICE_ID

    Sensor device bus address
#define SNS_SMGR_SENSOR_xx_BUS_ADDRESS

    Data types for this sensor device are defined by enumeration
    sns_ddf_sensor_e in sns_ddf_common.h
#define SNS_SMGR_SENSOR_xx_DATA_TYPE_1
#define SNS_SMGR_SENSOR_xx_DATA_TYPE_2

   Which data type has range or LPF attribute (primary or secondary)
#define SNS_SMGR_SENSOR_xx_RANGE_TYPE

   The sensor command value (0...n-1 when the device has n range selections) used
   as the fixed range when the flag SNS_SMGR_NO_SENSITIVITY is set
#define SNS_SMGR_SENSOR_xx_SENSITIVITY_DEFAULT

   Flags for various sensor characteristics. This is the sum of pertinent
   FLAG VALUES, as defined below
#define SNS_SMGR_SENSOR_xx_FLAGS

   Time (usec) from power-off to idle. This is the delay time following
   initialization
#define SNS_SMGR_SENSOR_xx_OFF_TO_IDLE

   Time (usec) from idle (low power state) to ready (active). Used when
   going to active power status after the first active state
#define SNS_SMGR_SENSOR_xx_IDLE_TO_READY

    Registry data for device driver, item type and identifier
    Item type NONE, SINGLE, or GROUP
#define SNS_SMGR_SENSOR_xx_REG_ITEM_TYPE
#define SNS_SMGR_SENSOR_xx_REG_ITEM_ID

    Registry calibration data for PRIMARY data type. Calibration for SECONDARY
    data may be added later if needed.
    Type NONE or GROUP
#define SNS_SMGR_SENSOR_xx_CAL_PRI_TYPE
#define SNS_SMGR_SENSOR_xx_CAL_PRI_ID

*/

/* ====================== FLAG VALUES ============================
*/

/* Flag to enable/disable FIFO functionality. */
#define SNS_SMGR_ENABLE_FIFO                            0x10

/* Flag true if sensor allows setting of internal sampling rate and that rate
   determines the hardware filter bandwidth. */
#define SNS_SMGR_SENSOR_LPF_FROM_RATE                   0x20

/* Flag to disable sensitivity and dynamic range control. Ignore
   sensitivity parameter in sensor request.
   Use SNS_SMGR_SENSOR_xx_SENSITIVITY_DEFAULT value as the fixed range */
#define SNS_SMGR_NO_SENSITIVITY                         0x40

/* Flag true if the device driver is capable of self scheduling */
#define SNS_SMGR_DD_SELF_SCHED                          0x80

#ifdef ADSP_STANDALONE
#ifdef VIRTIO_BRINGUP
 #define SNS_SMGR_SENSOR_0_ENUM_CODE                 SNS_SMGR_ID_ACCEL_V01
 #define SNS_SMGR_SENSOR_0_DD_FN_LIST_PTR            &sns_dd_fn_list_bmi160
 #define SNS_SMGR_SENSOR_0_DEVICE_ID                 0
 #define SNS_SMGR_SENSOR_0_BUS_ADDRESS               0 //CS for SPI
 #define SNS_SMGR_SENSOR_0_DATA_TYPE_1               SNS_DDF_SENSOR_ACCEL
 #define SNS_SMGR_SENSOR_0_DATA_TYPE_2               SNS_DDF_SENSOR__NONE
 #define SNS_SMGR_SENSOR_0_RANGE_TYPE                SNS_SMGR_DATA_TYPE_PRIMARY_V01
 #define SNS_SMGR_SENSOR_0_SENSITIVITY_DEFAULT       1 /* 16g fixed. */
 #define SNS_SMGR_SENSOR_0_FLAGS                     SNS_SMGR_NO_SENSITIVITY //( SNS_SMGR_DD_SELF_SCHED | SNS_SMGR_NO_SENSITIVITY | SNS_SMGR_ENABLE_FIFO )
 #define SNS_SMGR_SENSOR_0_OFF_TO_IDLE               100000 /* usec */
 #define SNS_SMGR_SENSOR_0_IDLE_TO_READY             250000 /* usec */
 #define SNS_SMGR_SENSOR_0_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_0_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_ACCEL_V02
 #define SNS_SMGR_SENSOR_0_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_0_CAL_PRI_ID                SNS_REG_SCM_GROUP_ACCEL_FAC_CAL_PARAMS_V02  /* 0=SNS_REG_SMGR_GROUP_ACCEL_V02 in sns_reg_api_v02.h */
 #define SNS_SMGR_SENSOR_0_GPIO_FIRST                0xffff
 #define SNS_SMGR_SENSOR_0_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_0_BUS_INSTANCE              0x1001
 #define SNS_SMGR_SENSOR_0_VDD                       0x93
 #define SNS_SMGR_SENSOR_0_VDDIO                     0x2
#else
 #define SNS_SMGR_SENSOR_0_ENUM_CODE                 SNS_SMGR_ID_ACCEL_V01
 #define SNS_SMGR_SENSOR_0_DD_FN_LIST_PTR            &sns_dd_fn_list_bmi160
 #define SNS_SMGR_SENSOR_0_DEVICE_ID                 0
 #define SNS_SMGR_SENSOR_0_BUS_ADDRESS               0 //CS for SPI
 #define SNS_SMGR_SENSOR_0_DATA_TYPE_1               SNS_DDF_SENSOR_ACCEL
 #define SNS_SMGR_SENSOR_0_DATA_TYPE_2               SNS_DDF_SENSOR__NONE
 #define SNS_SMGR_SENSOR_0_RANGE_TYPE                SNS_SMGR_DATA_TYPE_PRIMARY_V01
 #define SNS_SMGR_SENSOR_0_SENSITIVITY_DEFAULT       1 /* 16g fixed. */
 #define SNS_SMGR_SENSOR_0_FLAGS                     ( SNS_SMGR_DD_SELF_SCHED | SNS_SMGR_NO_SENSITIVITY | SNS_SMGR_ENABLE_FIFO )
 #define SNS_SMGR_SENSOR_0_OFF_TO_IDLE               100000 /* usec */
 #define SNS_SMGR_SENSOR_0_IDLE_TO_READY             250000 /* usec */
 #define SNS_SMGR_SENSOR_0_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_0_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_ACCEL_V02
 #define SNS_SMGR_SENSOR_0_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_0_CAL_PRI_ID                SNS_REG_SCM_GROUP_ACCEL_FAC_CAL_PARAMS_V02  /* 0=SNS_REG_SMGR_GROUP_ACCEL_V02 in sns_reg_api_v02.h */
 #define SNS_SMGR_SENSOR_0_GPIO_FIRST                117
 #define SNS_SMGR_SENSOR_0_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_0_BUS_INSTANCE              0x1001
 #define SNS_SMGR_SENSOR_0_VDD                       0x93
 #define SNS_SMGR_SENSOR_0_VDDIO                     0x2
#endif /* VIRTIO_BRINGUP */

 #define SNS_SMGR_SENSOR_1_ENUM_CODE                 SNS_SMGR_ID_GYRO_V01
 #define SNS_SMGR_SENSOR_1_DD_FN_LIST_PTR            &sns_dd_fn_list_bmi160
 #define SNS_SMGR_SENSOR_1_DEVICE_ID                 10
 #define SNS_SMGR_SENSOR_1_BUS_ADDRESS               0 //0x6A
 #define SNS_SMGR_SENSOR_1_DATA_TYPE_1               SNS_DDF_SENSOR_GYRO
 #define SNS_SMGR_SENSOR_1_DATA_TYPE_2               SNS_DDF_SENSOR__NONE
 #define SNS_SMGR_SENSOR_1_RANGE_TYPE                SNS_SMGR_DATA_TYPE_PRIMARY_V01
 #define SNS_SMGR_SENSOR_1_SENSITIVITY_DEFAULT       4
 #define SNS_SMGR_SENSOR_1_FLAGS                     SNS_SMGR_NO_SENSITIVITY //( SNS_SMGR_DD_SELF_SCHED | SNS_SMGR_NO_SENSITIVITY | SNS_SMGR_ENABLE_FIFO )
 #define SNS_SMGR_SENSOR_1_OFF_TO_IDLE               150000
 #define SNS_SMGR_SENSOR_1_IDLE_TO_READY             250000
 #define SNS_SMGR_SENSOR_1_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_1_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_GYRO_V02
 #define SNS_SMGR_SENSOR_1_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_1_CAL_PRI_ID                SNS_REG_SCM_GROUP_GYRO_FAC_CAL_PARAMS_V02
#ifdef VIRTIO_BRINGUP
 #define SNS_SMGR_SENSOR_1_GPIO_FIRST                0xffff
#else
 #define SNS_SMGR_SENSOR_1_GPIO_FIRST                117
#endif /* VIRTIO_BRINGUP */
 #define SNS_SMGR_SENSOR_1_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_1_BUS_INSTANCE              0x1001
 #define SNS_SMGR_SENSOR_1_VDD                       0x93
 #define SNS_SMGR_SENSOR_1_VDDIO                     0x2

#ifdef VIRTIO_BRINGUP
 #define SNS_SMGR_SENSOR_2_ENUM_CODE                 SNS_SMGR_ID_MAG_V01
 #define SNS_SMGR_SENSOR_2_DD_FN_LIST_PTR            &sns_dd_mag_akm09914_akm09915_fn_list
 #define SNS_SMGR_SENSOR_2_DEVICE_ID                 20
 #define SNS_SMGR_SENSOR_2_BUS_ADDRESS               0x01
 #define SNS_SMGR_SENSOR_2_DATA_TYPE_1               SNS_DDF_SENSOR_MAG
 #define SNS_SMGR_SENSOR_2_DATA_TYPE_2               SNS_DDF_SENSOR__NONE
 #define SNS_SMGR_SENSOR_2_RANGE_TYPE                0
 #define SNS_SMGR_SENSOR_2_SENSITIVITY_DEFAULT       0
 #define SNS_SMGR_SENSOR_2_FLAGS                     0
 #define SNS_SMGR_SENSOR_2_OFF_TO_IDLE               3000
 #define SNS_SMGR_SENSOR_2_IDLE_TO_READY             100000
 #define SNS_SMGR_SENSOR_2_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_2_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_MAG_V02
 #define SNS_SMGR_SENSOR_2_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_2_CAL_PRI_ID                0xffff
 #define SNS_SMGR_SENSOR_2_GPIO_FIRST                0xffff
 #define SNS_SMGR_SENSOR_2_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_2_BUS_INSTANCE              0x1001
 #define SNS_SMGR_SENSOR_2_VDD                       0x93
 #define SNS_SMGR_SENSOR_2_VDDIO                     0x2

#else
 #define SNS_SMGR_SENSOR_2_ENUM_CODE                 SNS_SMGR_ID_MAG_V01
 #define SNS_SMGR_SENSOR_2_DD_FN_LIST_PTR            &sns_dd_mag_akm09914_akm09915_fn_list
 #define SNS_SMGR_SENSOR_2_DEVICE_ID                 20
 #define SNS_SMGR_SENSOR_2_BUS_ADDRESS               0x01
 #define SNS_SMGR_SENSOR_2_DATA_TYPE_1               SNS_DDF_SENSOR_MAG
 #define SNS_SMGR_SENSOR_2_DATA_TYPE_2               SNS_DDF_SENSOR__NONE
 #define SNS_SMGR_SENSOR_2_RANGE_TYPE                0
 #define SNS_SMGR_SENSOR_2_SENSITIVITY_DEFAULT       0
 #define SNS_SMGR_SENSOR_2_FLAGS                     0
 #define SNS_SMGR_SENSOR_2_OFF_TO_IDLE               3000
 #define SNS_SMGR_SENSOR_2_IDLE_TO_READY             100000
 #define SNS_SMGR_SENSOR_2_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_2_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_MAG_V02
 #define SNS_SMGR_SENSOR_2_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_2_CAL_PRI_ID                0xffff
 #define SNS_SMGR_SENSOR_2_GPIO_FIRST                0xffff
 #define SNS_SMGR_SENSOR_2_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_2_BUS_INSTANCE              0x1001
 #define SNS_SMGR_SENSOR_2_VDD                       0x93
 #define SNS_SMGR_SENSOR_2_VDDIO                     0x2
#endif /* VIRTIO_BRINGUP */

 #define SNS_SMGR_SENSOR_3_ENUM_CODE                 SNS_SMGR_ID_PRESSURE_V01
 #define SNS_SMGR_SENSOR_3_DD_FN_LIST_PTR            &sns_alt_bmp280_driver_fn_list
 #define SNS_SMGR_SENSOR_3_DEVICE_ID                 0
 #define SNS_SMGR_SENSOR_3_BUS_ADDRESS               0x76
 #define SNS_SMGR_SENSOR_3_DATA_TYPE_1               SNS_DDF_SENSOR_PRESSURE
 #define SNS_SMGR_SENSOR_3_DATA_TYPE_2               SNS_DDF_SENSOR_TEMP
 #define SNS_SMGR_SENSOR_3_RANGE_TYPE                0
 #define SNS_SMGR_SENSOR_3_SENSITIVITY_DEFAULT       0
 #define SNS_SMGR_SENSOR_3_FLAGS                     0
 #define SNS_SMGR_SENSOR_3_OFF_TO_IDLE               10000
 #define SNS_SMGR_SENSOR_3_IDLE_TO_READY             0
 #define SNS_SMGR_SENSOR_3_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_3_REG_ITEM_ID               0
 #define SNS_SMGR_SENSOR_3_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_3_CAL_PRI_ID                0
 #define SNS_SMGR_SENSOR_3_GPIO_FIRST                0xffff
 #define SNS_SMGR_SENSOR_3_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_3_BUS_INSTANCE              3
 #define SNS_SMGR_SENSOR_3_VDD                       0x93
 #define SNS_SMGR_SENSOR_3_VDDIO                     0x2

#ifdef VIRTIO_BRINGUP
 #define SNS_SMGR_SENSOR_4_ENUM_CODE                 SNS_SMGR_ID_PROX_LIGHT_V01
 #define SNS_SMGR_SENSOR_4_DD_FN_LIST_PTR            &sns_ams_tmg399x_alsprx_driver_fn_list
 #define SNS_SMGR_SENSOR_4_DEVICE_ID                 0
 #define SNS_SMGR_SENSOR_4_BUS_ADDRESS               0x39
 #define SNS_SMGR_SENSOR_4_DATA_TYPE_1               SNS_DDF_SENSOR_PROXIMITY
 #define SNS_SMGR_SENSOR_4_DATA_TYPE_2               SNS_DDF_SENSOR_AMBIENT
 #define SNS_SMGR_SENSOR_4_RANGE_TYPE                SNS_SMGR_DATA_TYPE_SECONDARY_V01
 #define SNS_SMGR_SENSOR_4_SENSITIVITY_DEFAULT       0
 #define SNS_SMGR_SENSOR_4_FLAGS                     0
 #define SNS_SMGR_SENSOR_4_OFF_TO_IDLE               0
 #define SNS_SMGR_SENSOR_4_IDLE_TO_READY             0
 #define SNS_SMGR_SENSOR_4_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_4_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_PROX_LIGHT_V02
 #define SNS_SMGR_SENSOR_4_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_4_CAL_PRI_ID                0xffff
 #define SNS_SMGR_SENSOR_4_GPIO_FIRST                0xffff
 #define SNS_SMGR_SENSOR_4_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_4_BUS_INSTANCE              3
 #define SNS_SMGR_SENSOR_4_VDD                       0x93
 #define SNS_SMGR_SENSOR_4_VDDIO                     0x2
#else
 #define SNS_SMGR_SENSOR_4_ENUM_CODE                 SNS_SMGR_ID_PROX_LIGHT_V01
 #define SNS_SMGR_SENSOR_4_DD_FN_LIST_PTR            &sns_dd_apds9960_driver_if
 #define SNS_SMGR_SENSOR_4_DEVICE_ID                 0
 #define SNS_SMGR_SENSOR_4_BUS_ADDRESS               0x39
 #define SNS_SMGR_SENSOR_4_DATA_TYPE_1               SNS_DDF_SENSOR_PROXIMITY
 #define SNS_SMGR_SENSOR_4_DATA_TYPE_2               SNS_DDF_SENSOR_AMBIENT
 #define SNS_SMGR_SENSOR_4_RANGE_TYPE                SNS_SMGR_DATA_TYPE_SECONDARY_V01
 #define SNS_SMGR_SENSOR_4_SENSITIVITY_DEFAULT       0
 #define SNS_SMGR_SENSOR_4_FLAGS                     0
 #define SNS_SMGR_SENSOR_4_OFF_TO_IDLE               0
 #define SNS_SMGR_SENSOR_4_IDLE_TO_READY             0
 #define SNS_SMGR_SENSOR_4_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_4_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_PROX_LIGHT_V02
 #define SNS_SMGR_SENSOR_4_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_4_CAL_PRI_ID                0
 #define SNS_SMGR_SENSOR_4_GPIO_FIRST                0xffff
 #define SNS_SMGR_SENSOR_4_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_4_BUS_INSTANCE              3
 #define SNS_SMGR_SENSOR_4_VDD                       0x93
 #define SNS_SMGR_SENSOR_4_VDDIO                     0x2
#endif /* VIRTIO_BRINGUP */

 #define SNS_SMGR_SENSOR_5_ENUM_CODE                 SNS_SMGR_ID_HUMIDITY_V01
 #define SNS_SMGR_SENSOR_5_DD_FN_LIST_PTR            &sns_rht_shtc1_driver_fn_list
 #define SNS_SMGR_SENSOR_5_DEVICE_ID                 0
#ifdef VIRTIO_BRINGUP
 #define SNS_SMGR_SENSOR_5_BUS_ADDRESS               0x38
#else
 #define SNS_SMGR_SENSOR_5_BUS_ADDRESS               0x70
#endif
 #define SNS_SMGR_SENSOR_5_DATA_TYPE_1               SNS_DDF_SENSOR_HUMIDITY
 #define SNS_SMGR_SENSOR_5_DATA_TYPE_2               SNS_DDF_SENSOR_AMBIENT_TEMP
 #define SNS_SMGR_SENSOR_5_RANGE_TYPE                SNS_SMGR_DATA_TYPE_SECONDARY_V01
 #define SNS_SMGR_SENSOR_5_SENSITIVITY_DEFAULT       0
 #define SNS_SMGR_SENSOR_5_FLAGS                     0x80
 #define SNS_SMGR_SENSOR_5_OFF_TO_IDLE               10000
 #define SNS_SMGR_SENSOR_5_IDLE_TO_READY             10000
 #define SNS_SMGR_SENSOR_5_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_5_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_HUMIDITY_V02
 #define SNS_SMGR_SENSOR_5_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_5_CAL_PRI_ID                0xffff
 #define SNS_SMGR_SENSOR_5_GPIO_FIRST                0xffff
 #define SNS_SMGR_SENSOR_5_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_5_BUS_INSTANCE              3
 #define SNS_SMGR_SENSOR_5_VDD                       0x93
 #define SNS_SMGR_SENSOR_5_VDDIO                     0x2

 #define SNS_SMGR_SENSOR_6_ENUM_CODE                 SNS_SMGR_ID_RGB_V01
 #define SNS_SMGR_SENSOR_6_DD_FN_LIST_PTR            &sns_dd_apds9960_driver_if
 #define SNS_SMGR_SENSOR_6_DEVICE_ID                 0
 #define SNS_SMGR_SENSOR_6_BUS_ADDRESS               0x39
 #define SNS_SMGR_SENSOR_6_DATA_TYPE_1               SNS_DDF_SENSOR_RGB
 #define SNS_SMGR_SENSOR_6_DATA_TYPE_2               SNS_DDF_SENSOR__NONE
 #define SNS_SMGR_SENSOR_6_RANGE_TYPE                SNS_SMGR_DATA_TYPE_SECONDARY_V01
 #define SNS_SMGR_SENSOR_6_SENSITIVITY_DEFAULT       0
 #define SNS_SMGR_SENSOR_6_FLAGS                     0
 #define SNS_SMGR_SENSOR_6_OFF_TO_IDLE               5000
 #define SNS_SMGR_SENSOR_6_IDLE_TO_READY             0
 #define SNS_SMGR_SENSOR_6_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_6_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_PROX_LIGHT_V02
 #define SNS_SMGR_SENSOR_6_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_6_CAL_PRI_ID                0xffff
 #define SNS_SMGR_SENSOR_6_GPIO_FIRST                0xffff
 #define SNS_SMGR_SENSOR_6_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_6_BUS_INSTANCE              3
 #define SNS_SMGR_SENSOR_6_VDD                       0x93
 #define SNS_SMGR_SENSOR_6_VDDIO                     0x2

 #define SNS_SMGR_SENSOR_7_ENUM_CODE                 SNS_SMGR_ID_SAR_V01
 #define SNS_SMGR_SENSOR_7_DD_FN_LIST_PTR            0
 #define SNS_SMGR_SENSOR_7_DEVICE_ID                 0
 #define SNS_SMGR_SENSOR_7_BUS_ADDRESS               0x2C
 #define SNS_SMGR_SENSOR_7_DATA_TYPE_1               SNS_DDF_SENSOR_SAR
 #define SNS_SMGR_SENSOR_7_DATA_TYPE_2               SNS_DDF_SENSOR__NONE
 #define SNS_SMGR_SENSOR_7_RANGE_TYPE                SNS_SMGR_DATA_TYPE_PRIMARY_V01
 #define SNS_SMGR_SENSOR_7_SENSITIVITY_DEFAULT       0
 #define SNS_SMGR_SENSOR_7_FLAGS                     0x80
 #define SNS_SMGR_SENSOR_7_OFF_TO_IDLE               1000000
 #define SNS_SMGR_SENSOR_7_IDLE_TO_READY             1000000
 #define SNS_SMGR_SENSOR_7_REG_ITEM_TYPE             SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_7_REG_ITEM_ID               SNS_REG_DRIVER_GROUP_SAR_V02
 #define SNS_SMGR_SENSOR_7_CAL_PRI_TYPE              SNS_SMGR_REG_ITEM_TYPE_NONE
 #define SNS_SMGR_SENSOR_7_CAL_PRI_ID                0xffff
 #define SNS_SMGR_SENSOR_7_GPIO_FIRST                0xffff
 #define SNS_SMGR_SENSOR_7_GPIO_SECOND               0xffff
 #define SNS_SMGR_SENSOR_7_BUS_INSTANCE              7
 #define SNS_SMGR_SENSOR_7_VDD                       0x93
 #define SNS_SMGR_SENSOR_7_VDDIO                     0x2


#endif /* ADSP_STANDALONE */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


#endif /* #ifndef SNS_SMGR_SENSOR_CONFIG_H */
