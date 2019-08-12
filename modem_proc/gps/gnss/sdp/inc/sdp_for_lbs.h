#ifndef __SDP_FOR_LBS_H__
#define __SDP_FOR_LBS_H__

/*============================================================================
  @file sdp_for_lbs.h

            API provided by SDP, for LBS/QMI_PDS

GENERAL DESCRIPTION

  This file defines the API betwen the LocMW task and the Sensor Data Processor
  module/task.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sdp/inc/sdp_for_lbs.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/11   mac     Initial version

=============================================================================*/

#define SDP_FOR_LBS_VERSION 1

/* ---------------------------------------------------------------------------
 * Include files
 * ---------------------------------------------------------------------------*/
#ifdef FEATURE_CGPS_PLAYBACK
#include "slim_for_lbs.h"
#endif /* FEATURE_CGPS_PLAYBACK */

/* sensor config parameters */
#define LBS_SENSOR_CONFIG_MASK_CRADLE_MOUNT_STATE    0x00000001
#define LBS_SENSOR_CONFIG_MASK_EXT_POWER_STATE       0x00000002

typedef enum
{
  LBS_CRADLE_MOUNT_STATE_NOT_MOUNTED = 0,
  LBS_CRADLE_MOUNT_STATE_MOUNTED,
  LBS_CRADLE_MOUNT_STATE_UNKNOWN,
  LBS_CRADLE_MOUNT_STATE_MAX = LBS_CRADLE_MOUNT_STATE_UNKNOWN,  
  LBS_CRADLE_MOUNT_STATE_MAX_RESERVED = 0x7FFFFFFF
                        /*< To force the size of enum to 4bytes */
} lbs_cradle_mount_e_type; 

typedef enum
{
  LBS_EXT_POWER_STATE_NOT_CONNECTED = 0,
  LBS_EXT_POWER_STATE_CONNECTED,
  LBS_EXT_POWER_STATE_UNKNOWN,
  LBS_EXT_POWER_STATE_MAX = LBS_EXT_POWER_STATE_UNKNOWN,
  LBS_EXT_POWER_STATE_MAX_RESERVED = 0x7FFFFFFF
                        /*< To force the size of enum to 4bytes */
} lbs_ext_power_state_e_type; 

typedef struct  
{
  uint32 dw_ValidMask;
  lbs_cradle_mount_e_type e_CradleMountState;
  uint8 u_Confidence;
  lbs_ext_power_state_e_type e_ExternalPowerState;
} lbs_qmi_sensor_config_s_type;

/* navigation config parameters */
#define LBS_NAV_CONFIG_MASK_GYRO_DATA_VARIANCE            0x00000001
#define LBS_NAV_CONFIG_MASK_GYRO_BIAS_RANDOM_WALK         0x00000002
#define LBS_NAV_CONFIG_MASK_VIRGO_GRAVITY_AVE_LEN         0x00000004
#define LBS_NAV_CONFIG_MASK_HEADING_FILTER_MODE           0x00000008
#define LBS_NAV_CONFIG_MASK_VELOCITY_CONSTRAINT_VARIANCE  0x00000010
#define LBS_NAV_CONFIG_MASK_POSITIONING_METHOD            0x00000020
#define LBS_NAV_CONFIG_MASK_VELOCITY_RANDOM_WALK_SPECTRAL_DENSITY      0x00000040
#define LBS_NAV_CONFIG_MASK_ACCELERATION_RANDOM_WALK_SPECTRAL_DENSITY  0x00000080
#define LBS_NAV_CONFIG_MASK_ANGLE_RANDOM_WALK_SPECTRAL_DENSITY         0x00000100
#define LBS_NAV_CONFIG_MASK_RATE_RANDOM_WALK_SPECTRAL_DENSITY          0x00000200
#define LBS_NAV_CONFIG_MASK_SENSOR_ALGORITHM_MASK                      0x00000400
#define LBS_NAV_CONFIG_MASK_MAXIMUM_DEAD_RECKONING_TIME                0x00000800
#define LBS_NAV_CONFIG_MASK_INS_FILTER_CROSS_CHECK_SIGMA_LOW_THRESHOLD 0x00001000
#define LBS_NAV_CONFIG_MASK_INS_FILTER_CROSS_CHECK_SIGMA_HIGH_THRESHOLD 0x00002000
#define LBS_NAV_CONFIG_MASK_ALL                                        0x00001FFF

/* Bit mask to identify sensor algorithm config params */
#define LBS_SENSOR_ALGORITHM_DISABLE_INS_POSITIONING_FILTER      0x00000001


typedef struct  
{
  uint32  dw_ValidMask;
  float   f_gyroDataVariance;
  float   f_gyroBiasRandomWalk;
  uint32  q_virgoGravityAveragingLength;
  int8    b_headingFilterMode;
  float   f_velocityConstraintVariance;
  uint32  q_positioningMethod;
  float f_velocityRandomWalkSpectralDensity;
  float f_accelerationRandomWalkSpectralDensity;
  float f_angleRandomWalkSpectralDensity;
  float f_rateRandomWalkSpectralDensity;
  uint32 q_sensorAlgorithmMask;
  uint32  q_maximumDeadReckoningTime;
  uint8   u_insFilterCrossCheckSigmaLowThreshold;
  uint8   u_insFilterCrossCheckSigmaHighThreshold;
} lbs_qmi_nav_config_s_type;

#ifdef FEATURE_CGPS_PLAYBACK
/* Definition for Sensor Data usage indicator TLV in EVENT_REPORT_IND */
typedef struct
{
  uint16 sensor_data_usage_mask;
  uint16 sensor_aiding_indicator_mask;
} lbs_qmi_sensor_data_usage_s_type;

/* Structure for VOIS1 data */
/**  Maximum number of vehicle sensor samples that can be injected */
#define LBS_QMI_VEHICLE_SENSOR_DATA_MAX_SAMPLES 65
/**  Maximum number of axes that can be provided in each sample  */
#define LBS_QMI_VEHICLE_SENSOR_DATA_MAX_AXES 3
/**  Maximun number of measurements from an odometer  */
#define LBS_QMI_VEHICLE_ODOMETRY_MAX_MEASUREMENTS 3

typedef struct {
  uint32 w_SampleTimetagOffset;
  /**<   Sample time offset. This time offset must be 
       relative to the sensor time of the first sample. \n 
       - Type: Unsigned integer \n 
       - Units: Microseconds 
       - Range: Up to over 4000 seconds */

  uint32 dw_AxisSampleLen;  
  /**< Must be set to # of elements in axis_sample */
  
  float f_AxisSample[LBS_QMI_VEHICLE_SENSOR_DATA_MAX_AXES];
  /**<   Sensor axis sample.      \n 
       - Type: Floating point     \n
       - Units accelerometer:     \n
         ( (meters)/(seconds^2) ) \n
       - Units gyroscope:         \n
         ( (radians)/(seconds) ) 
         
       Note: The Axes samples should in the following order 
              - X-Axis
              - Y-Axis
              - Z-Axis */
}lbs_qmi_vehicle_sensor_sample_s_type;

typedef uint64 lbs_qmi_axes_mask_type;
#define LBS_QMI_MASK_X_AXIS ((lbs_qmi_axes_mask_type)0x0000000000000001ull) 
/**<  Identifies that X-axis is valid  */
#define LBS_QMI_MASK_Y_AXIS ((lbs_qmi_axes_mask_type)0x0000000000000002ull) 
/**<  Identifies that Y-axis is valid  */
#define LBS_QMI_MASK_Z_AXIS ((lbs_qmi_axes_mask_type)0x0000000000000004ull) 
/**<  Identifies that Z-axis is valid  */

typedef struct {
  uint32 dw_SensorSampleTimeBase;
  /**<   Denotes a 32-bit time tag of a reference time, from which 
       all samples in this message are offset.  Note this time must 
       be the same or (slightly) earlier than the first (oldest) 
       sample in this message.
       -  Type: Unsigned integer \n
       -  Units: 1 Millisecond
       -  Range: ~4 million seconds or almost 50 days between rollovers */

  lbs_qmi_axes_mask_type t_AxesValidity;
  /**<   Identifies the axes that are valid for all the sensor samples */

  uint32 dw_SensorDataLen;  /**< Must be set to # of elements in sensorData */

  lbs_qmi_vehicle_sensor_sample_s_type z_SensorData[LBS_QMI_VEHICLE_SENSOR_DATA_MAX_SAMPLES];
}lbs_qmi_vehicle_sensor_sample_list_s_type;

typedef uint64 lbs_qmi_vehicle_odometry_meas_deviation_mask_type;
#define LBS_QMI_MASK_VEHICLE_ODOMETRY_REVERSE_MOVEMENT ((lbs_qmi_vehicle_odometry_meas_deviation_mask_type)0x0000000000000001ull) 
/**<  Odometry data in this message includes at least some data where
       the vehicle may have been moving in the reverse direction. This
       bit must be set, if odometry data may be in reverse, and should
       not be set, if odometry data is all in the forward direction.  */
#define LBS_QMI_MASK_VEHICLE_ODOMETRY_AFFECTED_BY_ERRORS ((lbs_qmi_vehicle_odometry_meas_deviation_mask_type)0x0000000000000002ull) 
/**<  Odometry data in this message includes at least some data affected
       by a major error source affecting distance-travelled accuracy, 
       such as wheel slippage due to skidding, gravel, snow or ice as
       detected by the vehicle, e.g. via an ABS or other system.  */
#define LBS_QMI_MASK_VEHICLE_ODOMETRY_ABSOLUTE_MEASUREMENT ((lbs_qmi_vehicle_odometry_meas_deviation_mask_type)0x0000000000000004ull)
/**<  Odometry data in this message is an absolute amount since vehicle
       began service, and is the same vehicle that is regularly used with 
       this device (so that the offset of this value, since the last time
       this measurement was used by the Location engine can safely be used
       as an likely correct estimate of distance travelled since last 
       use)  */

typedef uint64 lbs_qmi_vehicle_odometry_wheel_flags_mask_type;
#define LBS_QMI_MASK_VEHICLE_ODOMETRY_LEFT_AND_RIGHT_AVERAGE ((lbs_qmi_vehicle_odometry_wheel_flags_mask_type)0x0000000000000001ull)
/**<  Average of left and right non-turning wheels  */
#define LBS_QMI_MASK_VEHICLE_ODOMETRY_LEFT ((lbs_qmi_vehicle_odometry_wheel_flags_mask_type)0x0000000000000002ull) 
/**<  Left side, non-turning wheel  */
#define LBS_QMI_MASK_VEHICLE_ODOMETRY_RIGHT ((lbs_qmi_vehicle_odometry_wheel_flags_mask_type)0x0000000000000004ull) 
/**<  Right side, non-turning wheel  */

typedef struct {
  uint32 w_SampleTimetagOffset;
  /**<   Sample time offset. This time offset must be 
       relative to the sensor time of the first sample. \n 
       - Type: Unsigned integer \n 
       - Units: Microseconds 
       - Range: Up to over 4000 seconds */
  uint32 dw_DistanceTravelledLen;  /**< Must be set to # of elements in distance_travelled */
  uint32 dw_DistanceTravelled[LBS_QMI_VEHICLE_ODOMETRY_MAX_MEASUREMENTS];
  /**<    Distance travelled (odometry) sample offset

        Type: Unsigned integer 
        Units of accumulated distance: (millimeters)
        Range: Over 4000 kilometers

        This measurement (with units of millimeters) is added to 
        the distance_travelled_base measurement (in meters), to 
        get the total distance travelled sample value

        Note: The order of measurements should be as below:
              - Left and Right Average
              - Left
              - Right 
   */
}lbs_qmi_vehicle_odometry_sample_s_type;

typedef struct {
  uint32 dw_SensorSampleTimeBase;
  /**<   Denotes a 32-bit time tag of a reference time, from which 
       all samples in this message are offset.  Note this time must 
       be the same or (slightly) earlier than the first (oldest) 
       sample in this message.
       -  Type: Unsigned integer \n
       -  Units: 1 Millisecond
       -  Range: ~4 million seconds or almost 50 days between rollovers */

  lbs_qmi_vehicle_odometry_meas_deviation_mask_type t_Flags;
  /**< Flags to indicate any deviation from the default measurement 
       assumptions. Valid bitmasks: \n
      - LBS_QMI_MASK_VEHICLE_ODOMETRY_REVERSE_MOVEMENT (0x0000000000000001) -- 
     Odometry data in this message includes at least some data where
       the vehicle may have been moving in the reverse direction. This
       bit must be set, if odometry data may be in reverse, and should
       not be set, if odometry data is all in the forward direction. 
      - LBS_QMI_MASK_VEHICLE_ODOMETRY_AFFECTED_BY_ERRORS (0x0000000000000002) --  
     Odometry data in this message includes at least some data affected
       by a major error source affecting distance-travelled accuracy, 
       such as wheel slippage due to skidding, gravel, snow or ice as
       detected by the vehicle, e.g. via an ABS or other system. 
      - LBS_QMI_MASK_VEHICLE_ODOMETRY_ABSOLUTE_MEASUREMENT (0x0000000000000004) --  
     Odometry data in this message is an absolute amount since vehicle
       began service, and is the same vehicle that is regularly used with 
       this device (so that the offset of this value, since the last time
       this measurement was used by the Location engine can safely be used
       as an likely correct estimate of distance travelled since last 
       use)  */

  lbs_qmi_vehicle_odometry_wheel_flags_mask_type t_WheelFlags;
  /**<  Delineates for which wheels measurements are being provided
        in the following samples, where one or more of the following
        bits must be set, and data samples aligned with these axes must
        appear in groups, in this order. Valid bitmasks: \n
      - LBS_QMI_MASK_VEHICLE_ODOMETRY_LEFT_AND_RIGHT_AVERAGE (0x0000000000000001) --  
      Average of left and right non-turning wheels 
      - LBS_QMI_MASK_VEHICLE_ODOMETRY_LEFT (0x0000000000000002) --  
      Left side, non-turning wheel 
      - LBS_QMI_MASK_VEHICLE_ODOMETRY_RIGHT (0x0000000000000004) --  
      Right side, non-turning wheel  */

  uint32 dw_DistanceTravelledBase;
  /**<   Distance Travelled Base. 
        - Type: Unsigned integer 
        - Units of accumulated distance: (meters)
        - Range: Over 4,000,0000 kilometers
        Distance travelled (odometry) is to be reported in a continuously 
        accumulating way from device power up.  It may be incremental distance 
        starting at 0, or another arbitrary point, from device power up, or the
        absolute distance traveled by the vehicle
        (and if so, please set LBS_QMI_MASK_VEHICLE_ODOMETRY_ABSOLUTE_MEASUREMENT) 
        as long as it grows incrementally from power up of the device.

        This distance_travelled_base is added to the distrance_travelled_offset
        of each sample (below) to get the absolute distance of each sample 
        point

        Distance travelled errors are expected to be primarily due to 
        scale-factor, with some allowance for ‘noise’ due to minor slippage 
        events (e.g. gravel.)  
        Major wheel slippage events that affect odometry, 
        should be flagged - see the flags field.

        Note also that other events such as a vehicle travelling in reverse may 
        also affect the available accuracy of this information, and notification
        of those events should be provided - see flags field. */

  uint32 dw_OdometryDataLen;  
  /**< Must be set to # of elements in odometry_data */
  lbs_qmi_vehicle_odometry_sample_s_type z_OdometryData[LBS_QMI_VEHICLE_SENSOR_DATA_MAX_SAMPLES];
  /**<   Variable length array to specify the odometry samples;
       Max Length of array is: 50 */
}lbs_qmi_vehicle_odometry_sample_list_s_type;

/** Request Message; Injects on-vehicle sensor data into the location engine. */
typedef struct {

  /* Optional */
  /*  On-Vehicle Accelerometer Data */
  uint8 u_AccelDataValid;  
  /**< Must be set to true if z_AccelData is being passed */
  lbs_qmi_vehicle_sensor_sample_list_s_type z_AccelData;

  /* Optional */
  /*  On-Vehicle Angular rotation Data */
  uint8 u_AngRotationDataValid;  
  /**< Must be set to true if z_AngRotationData is being passed */
  lbs_qmi_vehicle_sensor_sample_list_s_type z_AngRotationData;

  /* Optional */
  /*  Odometry Data */
  uint8 u_OdometryDataValid;  
  /**< Must be set to true if z_OdometryData is being passed */
  lbs_qmi_vehicle_odometry_sample_list_s_type z_OdometryData;

  /* Optional */
  uint8 u_ChangeInTimeScalesValid;  
  /**< Must be set to true if l_ChangeInTimeScales is being passed */
  int32 l_ChangeInTimeScales;
  /**<   This field is to be used in conjunction with an external 
       timesync mechanism that is aligning the vehicle sensor time scale, 
       with the on-device sensor time scale, to ensure that updates in
       that time offset don’t appear as jumps in the relative sensor time
       of the samples provided in this message.  If there is no such sync
       mechanism, e.g. if only the vehicle time is provided, then this field
       may be left at 0.

       This field is defined as the change, from the previously-sent QMI
       message with similar TLV’s 0x10, 0x11, or 0x12 in it, to this QMI 
       message, in the amount that the “sensor_time” is ahead of an 
       "external vehicle time".

       -  Type: Signed integer
       -  Units: 1 Microsecond
       -  Range: approx.. -2100 seconds to + 2100 seconds, where
                full-scale (min and max value) shall be interpreted
                as equal-to-or-more-than-this value of an offset change
                (unlikely to be reached in practice, unless there is a 
                startup, major resync or some other rollover event) */
}lbs_qmi_inject_vois_sensor_data_req_s_type;  /* Message */

/* ---------------------------------------------------------------------------
 * Data type definitions 
 * ---------------------------------------------------------------------------*/
/** Up-revved (only for IPCs) Request Message; Used by the control point to inject time sync data. */
typedef struct {

  lbs_qmi_time_sync_s_type z_req; /* inject time-sync data request */
  uint64 t_ttms;                  /* current time-tick msec */

} lbs_qmi_time_sync_s_type_v2; 

/** Up-revved (only for IPCs) Request Message; Used by the control point to inject sensor data. */
typedef struct {

  lbs_qmi_injected_sensor_data_s_type z_req; /* inject sensor data request */
  uint64 t_ttms;                             /* current time-tick msec */

} lbs_qmi_injected_sensor_data_s_type_v2; 

/** Up-revved (only for IPCs) Request Message; Used by the control point to inject sensor data. */
typedef struct {

  lbs_qmi_inject_vois_sensor_data_req_s_type z_req; /* inject vehicle sensor data request */
  uint64 t_ttms;                                       /* current time-tick msec */

}lbs_qmi_inject_vois_sensor_data_req_s_type_v2;
#endif /* FEATURE_CGPS_PLAYBACK */

/* ---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ---------------------------------------------------------------------------*/

/**
 * called from LBS task to handle a QMI command get sensor config
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.<p>
 * 
 * @param p_user_data
 *               [in] user data pointer in the request. will be copied over to the response
 * 
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 sdp_qmipds_get_sensor_config_req_proxy(void * const p_user_data);

/**
 * called from LBS task to handle a QMI command set sensor config
 * this is a proxy because it runs under the caller task context, while the actual processing
 * would happen later in SDP task.<p>
 * 
 * @param p_user_data
 *               [in] user data pointer in the request. will be copied over to the response
 * @param p_req  [in] QMI request
 * 
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 sdp_qmipds_set_sensor_config_req_proxy(void * const p_user_data, 
                                              const lbs_qmi_sensor_config_s_type * const p_req);

/**
 * called by LBS task to reset the state machine in case of timeout or some other error
 * 
 * @return 0 if it's accepted
 */
uint32 sdp_qmipds_reset_proxy (void);

/**
 * called by LBS task to stop SDP processing
 * this is called when tracking mode is stoped by HLOS
 * 
 * @return 0 if the stop request is sent
 */
uint32 sdp_qmipds_stop_processing_proxy (void);

#endif  /* __SDP_FOR_LBS_H__ */

