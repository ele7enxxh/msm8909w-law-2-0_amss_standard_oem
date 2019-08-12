/**
 * @file sns_ddf_common.h 
 * @brief Common definitions used across the Sensors Device Driver Framework.
 *  
 * Copyright (c) 2010-2015 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ddf/inc/sns_ddf_common.h#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2015-12-09 mb   Added SNS_DDF_ORIENTATION_EVENT
  2015-11-19 sc   Added SNS_DDF_TILT_EVENT
  2015-08-25 tc   Added SNS_DDF_EVENT_DAF_ACTIVE and SNS_DDF_EVENT_DAF_INACTIVE to SNS_DDF_DRIVER_VTBL_REVISION 0x3
  2015-05-28 pn   Added SNS_DDF_EVENT_FILTER_DELAY_CHANGED to SNS_DDF_DRIVER_VTBL_REVISION 0x2
  2015-04-09 pn   Changed definition of sns_ddf_time_t
  2015-04-03 AH   Added Object Temperature sensor type
  2015-03-12 AH   Added Heart Rate sensor type
  2015-02-26 MW   Added UV sensor type 
  2014-12-10 MW   Removed inclusion of sns_reg_common.h
  2014-04-03 MW   Added ambient temp sensor type
  2014-02-13 tc   Added SNS_DDF_EINVALID_DAF_REQ as a failure type
  2013-12-18 MW   Added BU52061NVX
  2013-10-23 MW   Added  support for SAR sensor type
  2013-10-09 RS   Added SNS_DDF_VENDOR_MEMSIC
  2013-09-12 MW   Added support for RGB sensor type  
  2013-09-12 MW   Added gesture types and SNS_DDF_VENDOR_MAXIM, SNS_DDF_VENDOR_ROHM
  2013-08-14 lka  Re-ordered DDF sensor enum to match that in registry.
  2013-07-15 dc   Support for humidity sensor type
  2013-06-23 lka  Added IR_GESTURE and OEM_SENSOR_xx.
  2013-05-05 sd   Added FIFO events , added entimestamp for sns_ddf_sensor_data_s
  2013-05-01 lka  Added enum values in various data types for Tap and FIFO.
  2013-04-02 ag   Add vendors - PIXART, ALPS, AMS
  2012-03-23 ag   Added event SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP
  2012-03-12 ag   Added vendor - Avago
  2012-03-06 ag   Adding test types
  2011-08-16 yk   Added statuses, events, and test types to support tests.
  2011-04-11 yk   Removed ASSERT macro.
  2010-10-21 yk   Cleaned up code formatting.
  2010-10-20 ad   Added vendor identifier enum 
  2010-09-24 yk   Added PENDING status. Renamed sensors data samples container.
  2010-08-16 yk   Changed handle_t to void*. Added ASSERT macro.
  2010-08-14 yk   Renamed identifiers to conform to DSPS naming convention.
  2010-07-30 yk   Initial revision
==============================================================================*/

#ifndef SNS_DDF_COMMON_H
#define SNS_DDF_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include "fixed_point.h"

/** 
 * Generic handle given to the driver so that it can access libraries and system
 * services without having to know the internal parameters.
 */
typedef void* sns_ddf_handle_t;

/**
 * Device list for the same devices. By maintaining the list, the device driver
 * can explore all of the devices that it drives.
 */
typedef void* sns_ddf_dev_list_t;


/** Time, in the highest resolution available, i.e. ticks. */
typedef uint64_t sns_ddf_time_t;

/**
 * Error codes indicating various failures across the framework.
 */
typedef enum
{
    SNS_DDF_SUCCESS,          /**< No error. */
    SNS_DDF_PENDING,          /**< Status cannot be provided immediately and will
                                   be returned asynchronously instead. */
    SNS_DDF_EFAIL,            /**< Unspecified error. */
    SNS_DDF_ENOMEM,           /**< Insufficient memory to process the request. */
    SNS_DDF_EINVALID_DATA,    /**< Invalid data found in input buffer. */
    SNS_DDF_EINVALID_PARAM,   /**< Invalid value of an input parameter. */
    SNS_DDF_EINVALID_ATTR,    /**< Invalid attribute ID. */
    SNS_DDF_EBUS,             /**< Failure in the underlying bus or bus driver. */
    SNS_DDF_EDEVICE,          /**< Failure in the device. */
    SNS_DDF_EDATA_BOUND,      /**< One or more data samples are out-of-bound. */
    SNS_DDF_EDEVICE_BUSY,     /**< Device is busy and can't service the request. */
    SNS_DDF_EINVALID_TEST,    /**< Invalid test case requested for this device. */
    SNS_DDF_EINVALID_DAF_REQ, /**< Invalid Driver Access request. */
    SNS_DDF_ELAST
} sns_ddf_status_e;


/**
 * Events are asynchronous occurences reported by the driver.
 */
typedef enum
{
    SNS_DDF_EVENT__NONE,                 /**< No event generated. */
    SNS_DDF_EVENT_MOTION,                /**< Motion was detected. */
    SNS_DDF_EVENT_INIT,                  /**< Initialization process complete. */
    SNS_DDF_EVENT_EBUS,                  /**< Bus error occured while processing an event. */
    SNS_DDF_EVENT_EDEVICE,               /**< Device error occured while processing event. */
    SNS_DDF_EVENT_BIAS_READY,            /**< New bias values are ready for retrieval. */
    SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP, /**< The device has new registry values ready to be read */
    SNS_DDF_EVENT_ODR_CHANGED,           /**< The ODR of the device has been changed because of
                                              the other sub-device in a combo device */
    SNS_DDF_EVENT_FIFO_WM_INT,           /**< FIFO watermark interrupt event */
    SNS_DDF_EVENT_FIFO_OVERFLOW,         /**< FIFO data overflow event */
    SNS_DDF_EVENT_FIFO_UNAVAILABLE,      /**< FIFO not available under some condition such as ESP enabled */
    SNS_DDF_EVENT_FIFO_AVAILABLE,        /**< FIFO is available event under some condition such as ESP disabled  */
    SNS_DDF_EVENT_FILTER_DELAY_CHANGED,  /**< Indicates driver is applying new filter delay
                                              available in SNS_DDF_DRIVER_VTBL_REVISION 0x2 and later */
    SNS_DDF_EVENT_DAF_ACTIVE,            /**< The driver is going to utilize the sensor for a DAF request
                                              available in SNS_DDF_DRIVER_VTBL_REVISION 0x3 and later */
    SNS_DDF_EVENT_DAF_INACTIVE,          /**< The driver is done utilizing the sensor for all DAF requests
                                              available in SNS_DDF_DRIVER_VTBL_REVISION 0x3 and later */
    SNS_DDF_EVENT_LAST
} sns_ddf_event_e;


/**
 * Available sensor types.
 *
 * Same enum values should appear in sns_reg_common.h
 */
typedef enum
{
  SNS_DDF_SENSOR__NONE, // 0
  SNS_DDF_SENSOR_ACCEL, // 1
  SNS_DDF_SENSOR_MAG, // 2
  SNS_DDF_SENSOR_GYRO, // 3
  SNS_DDF_SENSOR_TEMP, // 4
  SNS_DDF_SENSOR_PROXIMITY, // 5
  SNS_DDF_SENSOR_AMBIENT, // 6
  SNS_DDF_SENSOR_PRESSURE, // 7
  SNS_DDF_SENSOR_MAG_6D, // 8
  SNS_DDF_SENSOR_GYRO_6D, // 9
  SNS_DDF_SENSOR_DOUBLETAP, // 10
  SNS_DDF_SENSOR_SINGLETAP, // 11
  SNS_DDF_SENSOR_IR_GESTURE, //12
  SNS_DDF_SENSOR_OEM_SENSOR_01, // 13
  SNS_DDF_SENSOR_OEM_SENSOR_02, // 14
  SNS_DDF_SENSOR_OEM_SENSOR_03, // 15
  SNS_DDF_SENSOR_OEM_SENSOR_04, // 16
  SNS_DDF_SENSOR_OEM_SENSOR_05, // 17
  SNS_DDF_SENSOR_OEM_SENSOR_06, // 18
  SNS_DDF_SENSOR_OEM_SENSOR_07, // 19
  SNS_DDF_SENSOR_OEM_SENSOR_08, // 20
  SNS_DDF_SENSOR_OEM_SENSOR_09, // 21
  SNS_DDF_SENSOR_OEM_SENSOR_10, // 22
  SNS_DDF_SENSOR_STEP_EVENT, // 23
  SNS_DDF_SENSOR_STEP_COUNT, // 24
  SNS_DDF_SENSOR_SMD, // 25
  SNS_DDF_SENSOR_GAME_RV, // 26
  SNS_DDF_SENSOR_HUMIDITY, // 27
  SNS_DDF_SENSOR_RGB, // 28
  SNS_DDF_SENSOR_CT_C, // 29
  SNS_DDF_SENSOR_SAR, // 30
  SNS_DDF_SENSOR_HALL_EFFECT, // 31 
  SNS_DDF_SENSOR_AMBIENT_TEMP, // 32
  SNS_DDF_SENSOR_ULTRA_VIOLET, // 33
  SNS_DDF_SENSOR_HEART_RATE, //34
  SNS_DDF_SENSOR_HEART_RATE_RAW, //35
  SNS_DDF_SENSOR_OBJECT_TEMP, //36
  SNS_DDF_SENSOR_TILT_EVENT, //37
  SNS_DDF_SENSOR_ORIENTATION_EVENT, //38
  SNS_DDF_SENSOR__ALL,        /**< Addresses all sensors */
  SNS_DDF_SENSOR_LAST
} sns_ddf_sensor_e;

/**
 * Available IR gesture values.
 */
typedef enum
{
  SNS_DDF_IR_GESTURE_NONE,
  SNS_DDF_IR_GESTURE_UNKNOWN, /* Gesture happened, but the type is not defined */
  SNS_DDF_IR_GESTURE_RIGHT,
  SNS_DDF_IR_GESTURE_LEFT,
  SNS_DDF_IR_GESTURE_UP,
  SNS_DDF_IR_GESTURE_DOWN,
  SNS_DDF_IR_GESTURE_FORWARD,
  SNS_DDF_IR_GESTURE_BACKWARD,
  SNS_DDF_IR_GESTURE_VENDOR_SPECIFIC_1,
  SNS_DDF_IR_GESTURE_VENDOR_SPECIFIC_2,
  SNS_DDF_IR_GESTURE_VENDOR_SPECIFIC_3,
  SNS_DDF_IR_GESTURE_VENDOR_SPECIFIC_4,
  SNS_DDF_IR_GESTURE_VENDOR_SPECIFIC_5,
  SNS_DDF_IR_GESTURE_VENDOR_SPECIFIC_6,
  SNS_DDF_IR_GESTURE_VENDOR_SPECIFIC_7,
  SNS_DDF_IR_GESTURE_VENDOR_SPECIFIC_8,
  SNS_DDF_IR_GESTURE_LAST
} sns_ddf_ir_gesture_e;

/**
 * Available tap direction values.
 */
typedef enum
{
  SNS_DDF_TAP_DIR_NONE,     /* Not a Tap event */
  SNS_DDF_TAP_DIR_POS_X,
  SNS_DDF_TAP_DIR_NEG_X,
  SNS_DDF_TAP_DIR_POS_Y,
  SNS_DDF_TAP_DIR_NEG_Y,
  SNS_DDF_TAP_DIR_POS_Z,
  SNS_DDF_TAP_DIR_NEG_Z,
  SNS_DDF_TAP_DIR_X,        /* Tap event along X axis, polarity unknown */
  SNS_DDF_TAP_DIR_Y,        /* Tap event along Y axis, polarity unknown */
  SNS_DDF_TAP_DIR_Z,        /* Tap event along Z axis, polarity unknown */
  SNS_DDF_TAP_DIR_UNKNOWN,  /* Tap event, axis and polarity unknown */
  SNS_DDF_TAP_DIR_LAST
} sns_ddf_tap_dir_e;

/**
 * Sensor vendors.
 *  
 * @warning Do not delete or reorder vendor IDs.
 */
typedef enum
{
    SNS_DDF_VENDOR_BOSCH,
    SNS_DDF_VENDOR_INVENSENSE,
    SNS_DDF_VENDOR_STMICRO,
    SNS_DDF_VENDOR_INTERSIL,
    SNS_DDF_VENDOR_AKM,
    SNS_DDF_VENDOR_HONEYWELL,
    SNS_DDF_VENDOR_KIONIX,
    SNS_DDF_VENDOR_ADI,
    SNS_DDF_VENDOR_AVAGO,
    SNS_DDF_VENDOR_PIXART,
    SNS_DDF_VENDOR_ALPS,
    SNS_DDF_VENDOR_AMS,
    SNS_DDF_VENDOR_SENSIRION,
    SNS_DDF_VENDOR_MAXIM,
    SNS_DDF_VENDOR_ROHM,
    SNS_DDF_VENDOR_MEMSIC,
    SNS_DDF_VENDOR_PARTRON,
    SNS_DDF_VENDOR_NUM_SUPPORTED  /**< Add new entries above this line. */

} sns_ddf_vendor_e;


/**
 * Factory tests.
 */
typedef enum
{
    SNS_DDF_TEST_SELF,         /**< Self test. */
    SNS_DDF_TEST_IRQ,          /**< Interrupt test. */
    SNS_DDF_TEST_CONNECTIVITY, /**< Basic connectivity test. */
    SNS_DDF_TEST_SELF_HW,      /**< Hardware self test. */
    SNS_DDF_TEST_SELF_SW,      /**< Software self test. */
    SNS_DDF_TEST_OEM           /**< OEM test. */

} sns_ddf_test_e;


/** 
 * A single sample of sensor data.
 */
typedef struct
{
    sns_ddf_status_e   status; /**< SNS_DDF_EDATA_BOUND or SNS_DDF_SUCCESS. */
    q16_t              sample; /**< Single data sample in the sensor's units. */

} sns_ddf_sensor_sample_s;


/** 
 * A collection of sensor data samples.
 */
typedef struct
{
    sns_ddf_sensor_e          sensor;      /**< Sensor providing this data. */
    sns_ddf_status_e          status;      /**< Health of this data set. */
    sns_ddf_time_t            timestamp;   /**< Time first sample received. */
    sns_ddf_time_t            end_timestamp; /**< Time last sample received. */
    uint32_t                  num_samples; /**< Number of samples included. */
    sns_ddf_sensor_sample_s*  samples;     /**< Collection of sensor data. */

} sns_ddf_sensor_data_s;


#endif
