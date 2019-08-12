#ifndef SLIM_FOR_LBS_H
#define SLIM_FOR_LBS_H
/**
  @file
  @brief Interface for LBS-task and SLIM module

  This interface is provided by SLIM for the LBS-task. LBS can inject
  QMI_PDS-requests to SLIM using the interface functions.

  @ingroup slim_QMIPSDProvider slim_MPSS
*/
/*============================================================================
               Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/mpss/inc/slim_for_lbs.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "locEng_sm_common_msg_v01.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_QMIPSDProvider
//! @{
//! @name SLIM interface to LBS Module
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* LBS interface data structures */
/*! @brief Service validity mask for accelerometer service */
#define LBS_STREAMING_READY_STATUS_ACCEL         0x00000001
/*! @brief Service validity mask for gyroscope service */
#define LBS_STREAMING_READY_STATUS_GYRO          0x00000002
/*! @brief Service validity mask for vehicle accelerometer service */
#define LBS_STREAMING_READY_STATUS_VEHICLE_ACCEL    0x00000004
/*! @brief Service validity mask for vehicle gyroscope service */
#define LBS_STREAMING_READY_STATUS_VEHICLE_GYRO     0x00000008
/*! @brief Service validity mask for vehicle odometer service */
#define LBS_STREAMING_READY_STATYS_VEHICLE_ODOMETER 0x00000010

/* Flags:
 * Bit[0:1) : If set means that sign reversal needs to be done
 * Bit[1:2) : If set means that sensor time is synced to modem time
 * Bit[2-8) : Unused
 */
/*! @brief Flag for sign reversal */
#define LBS_SENSOR_CONFIG_MASK_SIGN_REVERSAL   0x01
/*! @brief Time source flag */
#define LBS_SENSOR_CONFIG_MASK_SENSOR_TIME_IS_MODEM_TIME 0x02


/* Validity masks to identify the injected sensor data */
/*! @brief Validity mask for 3-Axis accelerometer */
#define LBS_SENSOR_3AXIS_ACCEL      0x00000001
/*! @brief Validity mask for 3-Axis gyroscope */
#define LBS_SENSOR_3AXIS_GYRO       0x00000002
/*! @brief Validity mask for accelerometer temperature */
#define LBS_SENSOR_TEMP_ACCEL       0x00000004
/*! @brief Validity mask for gyroscope temperature */
#define LBS_SENSOR_TEMP_GYRO        0x00000008

/*! @brief Maximum number of samples that can be injected for a sensor */
#define LBS_MAX_SENSOR_SAMPLE_SETS        50

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/* QMI_PDS-provider error codes */
typedef enum
{
  SLIM_QMI_PDS_SUCCESS                     = 0,         /* No error - success */
  SLIM_QMI_PDS_ERROR_INDICATION_NOT_SENT   = 2,
  SLIM_QMI_PDS_ERROR_NULL_POINTER          = 3,
  SLIM_QMI_PDS_ERROR_INVALID_REQUEST       = 10,
  //! @cond
  SLIM_QMI_PDS_ERROR_MAX = 2147483647 /* used to force enum to 32-bit */
  //! @endcond
} slim_QmiPdsServiceErrorEnumType;


typedef enum
{
  LBS_STREAMING_STATUS_NOT_READY = 0,
  LBS_STREAMING_STATUS_READY,
  //! @cond
  LBSSTREAMING_STATUS_MAX_RESERVED = 0x7FFFFFFF /*< To force the size of enum
                                                    to 4 bytes */
  //! @endcond
} lbs_streaming_ready_status_e_type;

/*! modem uses this to send the client its ready status */
typedef struct
{
  uint32 dw_ValidMask;
  /*!< @brief Service validity mask.

   The mask is a combination of:
   - LBS_STREAMING_READY_STATUS_ACCEL
   - LBS_STREAMING_READY_STATUS_GYRO
   - LBS_STREAMING_READY_STATUS_VEHICLE_ACCEL
   - LBS_STREAMING_READY_STATUS_VEHICLE_GYRO
   - LBS_STREAMING_READY_STATUS_VEHICLE_ODOMETRY
   */
  lbs_streaming_ready_status_e_type e_AccelStreamingReadyStatus;
  /*!< @brief Accelerometer status

   This field can be used when @a dw_ValidMask has
   #LBS_STREAMING_READY_STATUS_ACCEL flag set.
   */
  lbs_streaming_ready_status_e_type e_GyroStreamingReadyStatus;
  /*!< @brief Gyroscope status

   This field can be used when @a dw_ValidMask has
   #LBS_STREAMING_READY_STATUS_GYRO flag set.
   */
  lbs_streaming_ready_status_e_type e_VehicleAccelStreamingReadyStatus;
  /*!< @brief Vehicle accelerometer status

   This field can be used when @a dw_ValidMask has
   #LBS_STREAMING_READY_STATUS_VEHICLE_ACCEL flag set.
   */
  lbs_streaming_ready_status_e_type e_VehicleGyroStreamingReadyStatus;
  /*!< @brief Vehicle gyroscope status

   This field can be used when @a dw_ValidMask has
   #LBS_STREAMING_READY_STATUS_VEHICLE_GYRO flag set.
   */
  lbs_streaming_ready_status_e_type e_VehicleOdometryStreamingReadyStatus;
  /*!< @brief Vehicle odometry status

   This field can be used when @a dw_ValidMask has
   #LBS_STREAMING_READY_STATYS_VEHICLE_ODOMETER flag set.
   */
} lbs_qmi_streaming_ready_status_s_type;

/*! used for the time sync request and time sync data injection */
typedef struct
{
  uint32 dw_RefCounter;
  /*!< @brief Transaction identifier */
  uint32 dw_SensorProcRxTime;
  /*!< @brief Remote processor RX time in milliseconds */
  uint32 dw_SensorProcTxTime;
  /*!< @brief Remote processor TX time in milliseconds */
} lbs_qmi_time_sync_s_type;

/* Sensor Data Injection */
/*!
@brief Sensor data sample
*/
typedef struct
{
  uint16 w_SampleTimeOffset;
  /*!< @brief Time offset of this sample in milliseconds */
  float f_xAxisSample;
  /*!< @brief Sample value for X-Axis */
  float f_yAxisSample;
  /*!< @brief Sample value for Y-Axis */
  float f_zAxisSample;
  /*!< @brief Sample value for Z-Axis */
} lbs_qmi_3axis_sample_s_type;

/*!
@brief Sensor data
*/
typedef struct
{
  uint32 dw_SensorTimeOfFirstSample;
  /*!< @brief Time of the first sample in milliseconds */
  uint8 u_Flags;
  /*!< @brief Flags

   Combination of flags:
   - LBS_SENSOR_CONFIG_MASK_SIGN_REVERSAL
   - LBS_SENSOR_CONFIG_MASK_SENSOR_TIME_IS_MODEM_TIME
   */
  uint8 u_NumSampleSets;
  /*!< @brief Size of sample array

   Number of samples in this message. Range is [1..#LBS_MAX_SENSOR_SAMPLE_SETS]
   */
  lbs_qmi_3axis_sample_s_type z_Samples[LBS_MAX_SENSOR_SAMPLE_SETS];
  /*!< Sample data */
} lbs_qmi_3axis_samples_s_type;

/*!
@brief Temperature sensor data sample
*/
typedef struct {

  uint16 sample_timetag_offset;
  /**<   Sample time offset. This time offset must be
       relative to the sensor time of the first sample. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds */

  float temperature;
  /**<   Temperature of the Sensor.
       Type:  float
       Units: Degree Celsius
       Range:  -50.00 to +100.00 Degree Celsius */
}lbs_qmi_sensor_temp_sample_s_type;  /* Type */

/*!
@brief Temperature sensor data
*/
typedef struct {

  uint32 sensor_timetag_of_first_sample;
  /**<   Denotes a full 32-bit time tag of the first (oldest) sample in this
       message. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds */

  uint32 sensorData_len;  /**< Must be set to # of elements in sensorData */
  lbs_qmi_sensor_temp_sample_s_type sensorData[LBS_MAX_SENSOR_SAMPLE_SETS];
  /**<   Variable length array to specify sensor temperature samples;
       maximum length of the array is 50.                             */
}lbs_qmi_sensor_temp_samples_s_type;  /* Type */

/**
@brief Inject sensor data request
*/
typedef struct
{
  uint32 dw_ValidityMask;
  /**< @brief Validity mask.

       Validity mask, non-empty combination of:
       - #LBS_SENSOR_3AXIS_ACCEL
       - #LBS_SENSOR_3AXIS_GYRO
       - #LBS_SENSOR_TEMP_ACCEL
       - #LBS_SENSOR_TEMP_GYRO
   */
  lbs_qmi_3axis_samples_s_type z_3AxisAccelSamples;
  /**< 3-axis accelerometer samples */
  lbs_qmi_3axis_samples_s_type z_3AxisGyroSamples;
  /**< 3-axis gyroscope samples */
  lbs_qmi_sensor_temp_samples_s_type z_AccelTemperatureSamples;
  /**< accelerometer temperature samples */
  lbs_qmi_sensor_temp_samples_s_type z_GyroTemperatureSamples;
  /**< gyroscope temperature samples */
} lbs_qmi_injected_sensor_data_s_type;

/*!
@brief Inject Sensor Data Response
*/
typedef struct
{
  uint8 u_NumSamplesAccel;
  /*!< Number of injected accelerometer samples */
  uint8 u_NumSamplesGyro;
  /*!< Number of injected gyroscope samples */
  uint8 u_NumTempSamplesAccel;
  /*!< Number of injected accelerometer temperature samples */
  uint8 u_NumTempSamplesGyro;
  /*!< Number of injected gyroscope temperature samples */
} lbs_qmi_injected_sensor_data_report_s_type;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Handler for QMI_PDS time synchronization injection message.

This function should be called when LBS task receives time synchronization
request from an external sensor data provider. SLIM forwards the received time
synchronization data to registered clients.

@param[in] p_UserData User data pointer in the request. Will be copied over to
                      the response.
@param[in] pz_Request QMI request

@return 0 if the function is successful.
*/
uint32 slim_LbsInjectTimeSyncDataRequest
(
  void *p_UserData,
  const lbs_qmi_time_sync_s_type * pz_Request
);

/**
@brief Handler for QMI_PDS sensor data injection message.

This function should be called when LBS task receives sensor data request from
an external sensor data provider. SLIM forwards the received sensor data to
registered clients.

@param[in] p_UserData User data pointer in the request. Will be copied over to
                      the response.
@param[in] pz_Request QMI request

@return 0 if the function is successful.
*/
uint32 slim_LbsInjectSensorDataRequest
(
  void * p_UserData,
  const lbs_qmi_injected_sensor_data_s_type *pz_Request
);

/**
@brief Handler for QMI_PDS motion data injection message.

This function should be called when LBS task receives motion data request from
an external sensor data provider. SLIM forwards the received motion data to
registered clients.

@param[in] pz_Request Pointer to the request data.

@return TRUE if function is successful.
*/
boolean slim_LbsInjectMotionDataRequest
(
  const locEngInjectMotionDataReqMsgT_v01 *pz_Request
);

/**
@brief Handler for QMI_PDS pedometer injection message.

This function should be called when LBS task receives pedometer request from an
external sensor data provider. SLIM forwards the received pedometer data to
registered clients.

@param[in] pz_Request Pointer to the request data.

@return TRUE if function is successful.
*/
boolean slim_LbsInjectPedometerRequest
(
  const locEngPedometerReportReqMsgT_v01 *pz_Request
);

/**
@brief Handler for QMI_PDS vehicle data injection message.

This function should be called when LBS task receives vehicle data request from
an external sensor data provider. SLIM forwards the received vehicle data to
registered clients.

@param[in] pz_Request Pointer to the request data.

@return TRUE if function is successful.
*/
boolean slim_LbsInjectVehicleDataRequest
(
  const locEngInjectVehicleSensorDataReqMsgT_v01 *pz_Request
);

//! @}
//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_FOR_LBS_H */
