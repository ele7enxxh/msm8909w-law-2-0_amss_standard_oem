#ifndef LOCENG_SERVICE_01_H
#define LOCENG_SERVICE_01_H
/**
  @file locEng_sm_common_msg_v01.h

  @brief This is the public header file which defines the locEng service Data structures.

  This header file defines the types and structures that were defined in
  locEng. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set 
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved. 
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/inc/locEng_sm_common_msg_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.3 
   It was generated on: Wed Mar 25 2015 (Spin 0)
   From IDL File: locEng_sm_common_msg_v01.idl */

/** @defgroup locEng_qmi_consts Constant values defined in the IDL */
/** @defgroup locEng_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup locEng_qmi_enums Enumerated types used in QMI messages */
/** @defgroup locEng_qmi_messages Structures sent as QMI messages */
/** @defgroup locEng_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup locEng_qmi_accessor Accessor for QMI service object */
/** @defgroup locEng_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup locEng_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define LOCENG_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define LOCENG_V01_IDL_MINOR_VERS 0x08
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define LOCENG_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define LOCENG_V01_MAX_MESSAGE_ID 0x0029
/**
    @}
  */


/** @addtogroup locEng_qmi_consts 
    @{ 
  */

/**  enumeration of loc engine source tasks that can send a loc Eng Msg

 Maximum length of the list containing the SV's that were used to generate 
     a position report.  */
#define LOC_ENG_MAX_SV_USED_LIST_LENGTH_V01 80

/**  Maximum number of Access Point entries per message.  */
#define QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V01 50

/**  Maximum number of vehicle sensor samples that can be injected  */
#define LOC_ENG_VEHICLE_SENSOR_DATA_MAX_SAMPLES_V01 65

/**  Maximum number of axes that can be provided in each sample  */
#define LOC_ENG_VEHICLE_SENSOR_DATA_MAX_AXES_V01 3

/**  Maximum number of measurements from an odometer  */
#define LOC_ENG_VEHICLE_ODOMETRY_MAX_MEASUREMENTS_V01 3
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGSTATUSENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_SUCCESS_V01 = 0, /**<  Request was completed successfully.       */
  eLOC_ENG_GENERAL_FAILURE_V01 = 1, /**<  Request failed because of a general failure.  */
  eLOC_ENG_UNSUPPORTED_V01 = 2, /**<  Request failed because it is not supported.  */
  eLOC_ENG_INVALID_PARAMETER_V01 = 3, /**<  Request failed because it contained invalid parameters.    */
  eLOC_ENG_ENGINE_BUSY_V01 = 4, /**<  Request failed because the engine is busy.  */
  eLOC_ENG_PHONE_OFFLINE_V01 = 5, /**<  Request failed because the phone is offline.  */
  eLOC_ENG_TIMEOUT_V01 = 6, /**<  Request failed because it timed out.  */
  eLOC_ENG_CONFIG_NOT_SUPPORTED_V01 = 7, /**<  Request failed because an undefined configuration was requested  */
  eLOC_ENG_INSUFFICIENT_MEMORY_V01 = 8, /**<  Request failed because the engine could not allocate sufficent
       memory for the request.  */
  LOCENGSTATUSENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngStatusEnumT_v01;
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGMSGSOURCEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_MSG_SOURCE_LOC_MW_V01 = 0, /**<  Message was sent by loc MW task */
  eLOC_ENG_MSG_SOURCE_LBS_V01 = 1, /**<  Message was sent by LBS task */
  eLOC_ENG_MSG_SOURCE_SDP_V01 = 2, /**<  Message was sent by SDP task */
  eLOC_ENG_MSG_SOURCE_NF_V01 = 3, /**<  Message was sent by NF task */
  eLOC_ENG_MSG_SOURCE_SM_V01 = 4, /**<  Message was sent by SM task */
  LOCENGMSGSOURCEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngMsgSourceEnumT_v01;
/**
    @}
  */

/** @addtogroup locEng_qmi_aggregates
    @{
  */
typedef struct {

  float PDOP;
  /**<   Position dilution of precision.\n
       - Type: Floating point \n
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)\n
       - PDOP = square root of (HDOP^2 + VDOP^2)       */

  float HDOP;
  /**<   Horizontal dilution of precision.\n
       - Type: Floating point \n  
       - Range: 1 (highest accuracy) to 50 (lowest accuracy) */

  float VDOP;
  /**<   Vertical dilution of precision.\n
       - Type: Floating point. \n
       - Range: 1 (highest accuracy) to 50 (lowest accuracy) */
}locEngDOPStructT_v01;  /* Type */
/**
    @}
  */

typedef uint32_t locEngPositionTechMaskT_v01;
#define LOC_ENG_POS_TECH_MASK_SATELLITE_V01 ((locEngPositionTechMaskT_v01)0x00000001) /**<  Satellites were used to generate the fix.  */
#define LOC_ENG_POS_TECH_MASK_CELLID_V01 ((locEngPositionTechMaskT_v01)0x00000002) /**<  Cell towers were used to generate the fix.  */
#define LOC_ENG_POS_TECH_MASK_WIFI_V01 ((locEngPositionTechMaskT_v01)0x00000004) /**<  WiFi access points were used to generate the fix.  */
#define LOC_ENG_POS_TECH_MASK_SENSORS_V01 ((locEngPositionTechMaskT_v01)0x00000008) /**<  Sensors were used to generate the fix.  */
#define LOC_ENG_POS_TECH_MASK_REFERENCE_LOCATION_V01 ((locEngPositionTechMaskT_v01)0x00000010) /**<  Reference Location was used to generate the fix.  */
#define LOC_ENG_POS_TECH_MASK_INJECTED_COARSE_POSITION_V01 ((locEngPositionTechMaskT_v01)0x00000020) /**<   Coarse position injected into the location engine was used to 
        generate the fix.    */
/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGPOSITIONSESSIONSTATUSENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_POS_SESS_STATUS_SUCCESS_V01 = 0, /**<  Session was successful.       */
  eLOC_ENG_SESS_STATUS_IN_PROGRESS_V01 = 1, /**<  Session still in progress. Further position reports will be
       generated until either the fix criteria specified by the client
       are met or the client response timeout occurs.    */
  eLOC_ENG_SESS_STATUS_GENERAL_FAILURE_V01 = 2, /**<  Session failed.   */
  eLOC_ENG_SESS_STATUS_TIMEOUT_V01 = 3, /**<  Fix request failed because the session timed out.       */
  eLOC_ENG_SESS_STATUS_USER_END_V01 = 4, /**<  Fix request failed because the session was ended by the user.       */
  eLOC_ENG_SESS_STATUS_BAD_PARAMETER_V01 = 5, /**<  Fix request failed due to bad parameters in the request.  */
  eLOC_ENG_SESS_STATUS_PHONE_OFFLINE_V01 = 6, /**<  Fix request failed because the phone is offline.  */
  eLOC_ENG_SESS_STATUS_ENGINE_LOCKED_V01 = 7, /**<  Fix request failed because the engine is locked.  */
  LOCENGPOSITIONSESSIONSTATUSENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngPositionSessionStatusEnumT_v01;
/**
    @}
  */

typedef uint32_t locEngSensorUsageMaskT_v01;
#define LOC_ENG_SENSOR_MASK_USED_ACCEL_V01 ((locEngSensorUsageMaskT_v01)0x00000001) /**<  Bitmask to specify whether an accelerometer was used.  */
#define LOC_ENG_SENSOR_MASK_USED_GYRO_V01 ((locEngSensorUsageMaskT_v01)0x00000002) /**<  Bitmask to specify whether a gyroscope was used.  */
typedef uint32_t locEngSensorAidedMaskT_v01;
#define LOC_ENG_SENSOR_AIDED_MASK_HEADING_V01 ((locEngSensorAidedMaskT_v01)0x00000001) /**<  Bitmask to specify whether a sensor was used to calculate heading.  */
#define LOC_ENG_SENSOR_AIDED_MASK_SPEED_V01 ((locEngSensorAidedMaskT_v01)0x00000002) /**<  Bitmask to specify whether a sensor was used to calculate speed.  */
#define LOC_ENG_SENSOR_AIDED_MASK_POSITION_V01 ((locEngSensorAidedMaskT_v01)0x00000004) /**<  Bitmask to specify whether a sensor was used to calculate position.  */
#define LOC_ENG_SENSOR_AIDED_MASK_VELOCITY_V01 ((locEngSensorAidedMaskT_v01)0x00000008) /**<  Bitmask to specify whether a sensor was used to calculate velocity.  */
/** @addtogroup locEng_qmi_aggregates
    @{
  */
typedef struct {

  locEngSensorUsageMaskT_v01 usageMask;
  /**<   Specifies which sensors are used.
  
       Valid bitmasks are specified by the following constants: \n       
         - 0x00000001 -- SENSOR_USED_ACCEL \n
         - 0x00000002 -- SENSOR_USED_GYRO */

  locEngSensorAidedMaskT_v01 aidingIndicatorMask;
  /**<   Specifies which results are aided by sensors.

       Valid bitmasks are specified by the following constants: \n
         - 0x00000001 -- AIDED_HEADING \n
         - 0x00000002 -- AIDED_SPEED \n
         - 0x00000004 -- AIDED_POSITION \n
         - 0x00000008 -- AIDED_VELOCITY */
}locEngSensorUsageIndicatorStructT_v01;  /* Type */
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGTIMESOURCEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_TIME_SRC_INVALID_V01 = 0, /**<  Invalid time.  */
  eLOC_ENG_TIME_SRC_NETWORK_TIME_TRANSFER_V01 = 1, /**<  Time is set by the 1x system.  */
  eLOC_ENG_TIME_SRC_NETWORK_TIME_TAGGING_V01 = 2, /**<  Time is set by WCDMA/GSM time tagging (i.e.,
       associating network time with GPS time).  */
  eLOC_ENG_TIME_SRC_EXTERNAL_INPUT_V01 = 3, /**<  Time is set by an external injection.  */
  eLOC_ENG_TIME_SRC_TOW_DECODE_V01 = 4, /**<  Time is set after decoding over-the-air GPS navigation data
       from one GPS satellite.  */
  eLOC_ENG_TIME_SRC_TOW_CONFIRMED_V01 = 5, /**<  Time is set after decoding over-the-air GPS navigation data 
       from multiple satellites.  */
  eLOC_ENG_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V01 = 6, /**<  Both time of the week and the GPS week number are known.  */
  eLOC_ENG_TIME_SRC_NAV_SOLUTION_V01 = 7, /**<  Time is set by the position engine after the fix is obtained.  */
  eLOC_ENG_TIME_SRC_SOLVE_FOR_TIME_V01 = 8, /**<  Time is set by the position engine after performing SFT.
       This is done when the clock time uncertainty is large.  */
  eLOC_ENG_TIME_SRC_GLO_TOW_DECODE_V01 = 9, /**<  Time is set after decoding GLO satellites  */
  eLOC_ENG_TIME_SRC_TIME_TRANSFORM_V01 = 10, /**<  Time is set after transforming the GPS to GLO time  */
  eLOC_ENG_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V01 = 11, /**<  Time is set by the sleep time tag provided by the WCDMA network  */
  eLOC_ENG_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V01 = 12, /**<  Time is set by the sleep time tag provided by the GSM network  */
  eLOC_ENG_TIME_SRC_UNKNOWN_V01 = 13, /**<  Source of the time is unknown  */
  eLOC_ENG_TIME_SRC_SYSTEM_TIMETICK_V01 = 14, /**<   Time is derived from system clock (better known as slow clock). 
       GNSS time is maintained irrespective of the GNSS receiver state   */
  eLOC_ENG_TIME_SRC_QZSS_TOW_DECODE_V01 = 15, /**<  Time is set after decoding QZSS satellites  */
  eLOC_ENG_TIME_SRC_BDS_TOW_DECODE_V01 = 16, /**<  Time is set after decoding BDS satellites  */
  eLOC_ENG_TIME_SRC_GAL_TOW_DECODE_V01 = 17, /**<  Time is set after decoding GAL satellites  */
  LOCENGTIMESOURCEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngTimeSourceEnumT_v01;
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGRELIABILITYENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_RELIABILITY_NOT_SET_V01 = 0, /**<  Location reliability is not set  */
  eLOC_ENG_RELIABILITY_VERY_LOW_V01 = 1, /**<  Location reliability is very low; use it at your own risk  */
  eLOC_ENG_RELIABILITY_LOW_V01 = 2, /**<  Location reliability is low; little or no cross-checking is possible  */
  eLOC_ENG_RELIABILITY_MEDIUM_V01 = 3, /**<  Location reliability is medium; limited cross-check passed   */
  eLOC_ENG_RELIABILITY_HIGH_V01 = 4, /**<  Location reliability is high; strong cross-check passed  */
  LOCENGRELIABILITYENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngReliabilityEnumT_v01;
/**
    @}
  */

/** @addtogroup locEng_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t gpsWeek;
  /**<   Current GPS week as calculated from midnight, Jan. 6, 1980. \n
       - Type: Unsigned integer \n
       - Units: Weeks */

  uint32_t gpsTimeOfWeekMs;
  /**<   Amount of time into the current GPS week. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds */
}locEngGPSTimeStructT_v01;  /* Type */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Indication Message; common position structure */
typedef struct {

  /* Mandatory */
  /*   Session Status */
  locEngPositionSessionStatusEnumT_v01 sessionStatus;
  /**<   Session status.
  
        Valid values: \n
          - 0x00000000 -- SESS_STATUS_SUCCESS \n
          - 0x00000001 -- SESS_STATUS_IN_PROGRESS \n
          - 0x00000002 -- SESS_STATUS_GENERAL_FAILURE \n
          - 0x00000003 -- SESS_STATUS_TIMEOUT \n
          - 0x00000004 -- SESS_STATUS_USER_END \n
          - 0x00000005 -- SESS_STATUS_BAD_PARAMETER \n
          - 0x00000006 -- SESS_STATUS_PHONE_OFFLINE \n
          - 0x00000007 -- SESS_STATUS_ENGINE_LOCKED
      */

  /* Mandatory */
  /*   Session ID */
  uint8_t sessionId;
  /**<    ID of the session that was specified in the Start request 
        LOC_ENG_START_REQ. \n
        - Type: Unsigned integer \n
        - Range: 0 to 255 */

  /* Optional */
  /*  Latitude */
  uint8_t latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -90.0 to 90.0 \n
       - Positive values indicate northern latitude \n
       - Negative values indicate southern latitude */

  /* Optional */
  /*   Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -180.0 to 180.0 \n
       - Positive values indicate eastern longitude \n
       - Negative values indicate western longitude */

  /* Optional */
  /*   Circular Horizontal Position Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Type: Floating point \n
       - Units: Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
  uint8_t horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty.\n
       - Type: Floating point \n
       - Units: Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
  uint8_t horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty.\n
       - Type: Floating point \n
       - Units: Meters */

  /* Optional */
  /*  Elliptical Horizontal Uncertainty Azimuth */
  uint8_t horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation.\n
       - Type: Floating point \n
       - Units: Decimal degrees \n
       - Range: 0 to 180 */

  /* Optional */
  /*  Horizontal Circular Confidence */
  uint8_t horCircularConfidence_valid;  /**< Must be set to true if horCircularConfidence is being passed */
  uint8_t horCircularConfidence;
  /**<   Horizontal circular uncertainty confidence.\n
       - Type: Unsigned integer \n 
       - Units: Percent \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Horizontal Elliptical Confidence */
  uint8_t horEllipticalConfidence_valid;  /**< Must be set to true if horEllipticalConfidence is being passed */
  uint8_t horEllipticalConfidence;
  /**<   Horizontal elliptical uncertainty confidence.\n
       - Type: Unsigned integer \n 
       - Units: Percent \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  locEngReliabilityEnumT_v01 horReliability;
  /**<   Specifies the reliability of the horizontal position.

       Valid values: \n
         - 0x00000000 -- RELIABILITY_NOT_SET \n
         - 0x00000001 -- RELIABILITY_VERY_LOW \n
         - 0x00000002 -- RELIABILITY_LOW \n
         - 0x00000003 -- RELIABILITY_MEDIUM \n
         - 0x00000004 -- RELIABILITY_HIGH
   */

  /* Optional */
  /*  Horizontal Speed */
  uint8_t horSpeed_valid;  /**< Must be set to true if horSpeed is being passed */
  float horSpeed;
  /**<   Horizontal speed.\n
       - Type: Floating point \n 
       - Units: Meters/second */

  /* Optional */
  /*  Horizontal Speed Uncertainty */
  uint8_t horSpeedUnc_valid;  /**< Must be set to true if horSpeedUnc is being passed */
  float horSpeedUnc;
  /**<   2D (horizontal) Speed uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters/second */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Type: Floating point \n 
       - Units: Meters \n
       - Range: -500 to 15883 */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level.\n
       - Type: Floating point \n 
       - Units: Meters */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical uncertainty confidence.\n
       - Type: Unsigned integer \n 
       - Units: Percent	\n
       - Range: 0 to 99 */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  locEngReliabilityEnumT_v01 vertReliability;
  /**<   Specifies the reliability of the vertical position.

        Valid values: \n
          - 0x00000000 -- RELIABILITY_NOT_SET \n
          - 0x00000001 -- RELIABILITY_VERY_LOW \n
          - 0x00000002 -- RELIABILITY_LOW \n
          - 0x00000003 -- RELIABILITY_MEDIUM \n
          - 0x00000004 -- RELIABILITY_HIGH */

  /* Optional */
  /*  Vertical Speed */
  uint8_t vertSpeed_valid;  /**< Must be set to true if vertSpeed is being passed */
  float vertSpeed;
  /**<   Vertical speed.\n
         - Type: Floating point \n
         - Units: Meters/second */

  /* Optional */
  /*  Vertical Speed Uncertainty */
  uint8_t vertSpeedUnc_valid;  /**< Must be set to true if vertSpeedUnc is being passed */
  float vertSpeedUnc;
  /**<   1-D (vertical) speed uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters/second */

  /* Optional */
  /*  Heading */
  uint8_t heading_valid;  /**< Must be set to true if heading is being passed */
  float heading;
  /**<   Heading.\n
         - Type: Floating point \n
         - Units: Degrees \n
         - Range: 0 to 359.999  */

  /* Optional */
  /*  Heading Uncertainty */
  uint8_t headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   Heading uncertainty.\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: 0 to 359.999 */

  /* Optional */
  /*  Magnetic Deviation */
  uint8_t magneticDeviation_valid;  /**< Must be set to true if magneticDeviation is being passed */
  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown 
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. \n
      - Type: Floating point */

  /* Optional */
  /*  Technology Used */
  uint8_t technologyMask_valid;  /**< Must be set to true if technologyMask is being passed */
  locEngPositionTechMaskT_v01 technologyMask;
  /**<   Technology used in computing this fix.

       Valid bitmasks: \n
         - 0x00000001 -- SATELLITE \n
         - 0x00000002 -- CELLID \n
         - 0x00000004 -- WIFI \n
         - 0x00000008 -- SENSORS \n 
         - 0x00000010 -- REFERENCE_LOCATION
         - 0x00000020 -- INJECTED_COARSE_POSITION */

  /* Optional */
  /*  Dilution of Precision */
  uint8_t DOP_valid;  /**< Must be set to true if DOP is being passed */
  locEngDOPStructT_v01 DOP;
  /**<   Dilution of Precision */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   UTC timestamp, including the leap seconds \n
       - Type: Unsigned integer \n 
       - Units: Milliseconds since Jan. 1, 1970 */

  /* Optional */
  /*  GPS Time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  locEngGPSTimeStructT_v01 gpsTime;
  /**<   \n The number of weeks since Jan. 5, 1980, and
       milliseconds into the current week. */

  /* Optional */
  /*  Time Uncertainty */
  uint8_t timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty. \n
       - Type: Floating point \n
       - Units: Milliseconds  */

  /* Optional */
  /*  Time Source */
  uint8_t timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  locEngTimeSourceEnumT_v01 timeSrc;
  /**<   Time source.
  
        Valid values: \n
          - 0x00000000 -- TIME_SRC_INVALID \n 
          - 0x00000001 -- TIME_SRC_NETWORK_TIME_TRANSFER \n  
          - 0x00000002 -- TIME_SRC_NETWORK_TIME_TAGGING \n   
          - 0x00000003 -- TIME_SRC_EXTERNAL_ INPUT \n         
          - 0x00000004 -- TIME_SRC_TOW_DECODE \n             
          - 0x00000005 -- TIME_SRC_TOW_CONFIRMED \n          
          - 0x00000006 -- TIME_SRC_TOW_AND_WEEK_CONFIRMED \n 
          - 0x00000007 -- TIME_SRC_NAV_SOLUTION \n  
          - 0x00000008 -- TIME_SRC_SOLVE_FOR_TIME */

  /* Optional */
  /*  Sensor Data Usage */
  uint8_t sensorDataUsage_valid;  /**< Must be set to true if sensorDataUsage is being passed */
  locEngSensorUsageIndicatorStructT_v01 sensorDataUsage;
  /**<   \n Whether sensor data was used in computing the position in this
       position report. */

  /* Optional */
  /*  SV's Used to Calculate the Fix */
  uint8_t gnssSvUsedList_valid;  /**< Must be set to true if gnssSvUsedList is being passed */
  uint32_t gnssSvUsedList_len;  /**< Must be set to # of elements in gnssSvUsedList */
  uint16_t gnssSvUsedList[LOC_ENG_MAX_SV_USED_LIST_LENGTH_V01];

  /* Optional */
  /*  Each entry in the list contains the SV ID of a Satellite
      used for calculating this position report. The following 
      information is associated with each SV ID:
      - Type: Unsigned integer \n 
      - Range: \n
           -- For GPS:     1 to 32 \n
           -- For SBAS:    33 to 64	\n
           -- For GLONASS: 65 to 96 \n 
           -- For QZSS:    193 to 197 
 3D Speed Uncertainty */
  uint8_t speedUnc3d_valid;  /**< Must be set to true if speedUnc3d is being passed */
  float speedUnc3d;
  /**<   3-D speed uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters/second */
}locEngPositionReportMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Indication Message; Controls the injection of pedometer reports into the
                     location engine  */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Request Pedometer Data */
  uint8_t requestPedometerData;
  /**<   \n Whether GNSS location engine is requesting the client to 
          send pedometer data.
       \begin{itemize1}
       \item    0x01 (TRUE)  -- GNSS location engine is requesting 
                                pedometer data
       \item    0x00 (FALSE) -- GNSS location engine is not requesting
                                pedometer data
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Reset Step Count */
  uint8_t resetStepCount_valid;  /**< Must be set to true if resetStepCount is being passed */
  uint8_t resetStepCount;
  /**<   Whether step count should be reset.
       \begin{itemize1}
       \item    0x01 (TRUE)  -- Pedometer step count should be reset. 
       \item    0x00 (FALSE) -- Pedometer step count should not be reset.
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Step Count Threshold */
  uint8_t stepCountThreshold_valid;  /**< Must be set to true if stepCountThreshold is being passed */
  uint32_t stepCountThreshold;
  /**<   Specifies the step count after which a pedometer report must be 
       generated. If the threshold is set to 0 then the pedometer report 
       should be generated at every step.
  */
}locEngEventPedometerControlIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Indication Message; Controls the injection of motion data into
                     the location engine  */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Request Motion Data */
  uint8_t requestMotionData;
  /**<   \n Whether GNSS location engine is requesting the client to 
          send motion data.
       \begin{itemize1}
       \item    0x01 (TRUE)  -- GNSS location engine is requesting 
                                motion data
       \item    0x00 (FALSE) -- GNSS location engine is not requesting 
                                motion data
       \vspace{-0.18in} \end{itemize1}
  */
}locEngEventMotionDataControlIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGSENSORDATATIMESOURCEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_SENSOR_TIME_SOURCE_UNSPECIFIED_V01 = 0, /**<  The sensor time source is unspecified */
  eLOC_ENG_SENSOR_TIME_SOURCE_COMMON_V01 = 1, /**<  The time source is common between the sensors and 
       the location engine */
  LOCENGSENSORDATATIMESOURCEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngSensorDataTimeSourceEnumT_v01;
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOC_ENG_INJECT_PEDOMETER_REPORT_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_INJECT_PEDOMETER_REPORT_ON_STEP_EVENT_V01 = 0, /**<  Report is injected on detection of a step event \n  */
  eLOC_ENG_INJECT_PEDOMETER_REPORT_PERIODICALLY_V01 = 1, /**<  Report is injected periodically  */
  LOC_ENG_INJECT_PEDOMETER_REPORT_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}loc_eng_inject_pedometer_report_enum_type_v01;
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Request Message; Used by the control point to inject pedometer data
                    into the location engine. */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Pedometer report timestamp in modem time */
  uint64_t modemTimeTickMsec;
  /**<   Timestamp of sensor input in modem time which caused this indication. \n
       - Unit: Milliseconds */

  /* Mandatory */
  /*  Time Source */
  locEngSensorDataTimeSourceEnumT_v01 timeSource;
  /**<   Time source for the pedometer. */

  /* Mandatory */
  /*  Pedometer report timestamp */
  uint32_t timestamp;
  /**<   Timestamp of sensor input which caused this indication. \n
       - Unit: Milliseconds */

  /* Mandatory */
  /*  Time Interval */
  float timeInterval;
  /**<   Time interval during which the step count was calculated. 
       Unit : Seconds. */

  /* Mandatory */
  /*  Step Count */
  uint32_t stepCount;
  /**<   Number for steps counted after the last pedometer reset. */

  /* Optional */
  /*  Step Confidence */
  uint8_t stepConfidence_valid;  /**< Must be set to true if stepConfidence is being passed */
  uint8_t stepConfidence;
  /**<   Confidence associated with the stepCount.
       Range: 0 to 100.
       Note: The report will be ignored if confidence is 0. */

  /* Optional */
  /*  Step Count Uncertainty */
  uint8_t stepCountUncertainty_valid;  /**< Must be set to true if stepCountUncertainty is being passed */
  float stepCountUncertainty;
  /**<   Uncertainty (in steps) associated with the step count. */

  /* Optional */
  /*  Step Rate */
  uint8_t stepRate_valid;  /**< Must be set to true if stepRate is being passed */
  float stepRate;
  /**<   Current estimate for the rate of steps per second.
       Units: steps/second
       Range: >= 0.0
  */

  /* Optional */
  /*  Pedometer report timestamp in Gps time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  locEngGPSTimeStructT_v01 gpsTime;
  /**<   Timestamp of sensor input in GPS time. \n */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   opaque user data, if specified the same user data should be
       echoed back in the response indication */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   UTC timestamp, including the leap seconds \n
       - Type: Unsigned integer \n 
       - Units: Milliseconds since Jan. 1, 1970 */

  /* Optional */
  /*  Stride Length Estimate */
  uint8_t strideLength_valid;  /**< Must be set to true if strideLength is being passed */
  float strideLength;
  /**<   Current estimate of the stride length. \n
       - Type: Float \n
       - Units: Meters/stride \n
       - Range: >= 0
  */

  /* Optional */
  /*  Stride Length Uncertainty */
  uint8_t strideLengthUncertainty_valid;  /**< Must be set to true if strideLengthUncertainty is being passed */
  float strideLengthUncertainty;
  /**<   Uncertainty associated with the stride length, provided at a 1-sigma 
       (68 percent) confidence. \n
       - Type: Float \n
       - Units: Meters/stride \n
       - Range: >= 0
  */

  /* Optional */
  /*  Fractional Step Count Data */
  uint8_t stepCountFractional_valid;  /**< Must be set to true if stepCountFractional is being passed */
  float stepCountFractional;
  /**<   Sub-integer portion of the number of steps counted during the time 
       interval. If the report is generated on event, this will be 0. \n
       - Type: Float \n
       - Units: steps \n
       - Range: >= 0.0 and < 1.0
  */

  /* Optional */
  /*  Injected Pedometer Report Type */
  uint8_t reportType_valid;  /**< Must be set to true if reportType is being passed */
  loc_eng_inject_pedometer_report_enum_type_v01 reportType;
  /**<   Identifies the type of report generated. Values: \n
      - eLOC_ENG_INJECT_PEDOMETER_REPORT_ON_STEP_EVENT (0) --  Report is injected on detection of a step event \n 
      - eLOC_ENG_INJECT_PEDOMETER_REPORT_PERIODICALLY (1) --  Report is injected periodically  
 */
}locEngPedometerReportReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to inject pedometer data
                    into the location engine. */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Status of Pedometer report request */
  locEngStatusEnumT_v01 status;
  /**<   Status of the pedometer report request

 Valid values: \n
      - eLOC_ENG_SUCCESS (0) --  Request was completed successfully.      
      - eLOC_ENG_GENERAL_FAILURE (1) --  Request failed because of a general failure. 
      - eLOC_ENG_UNSUPPORTED (2) --  Request failed because it is not supported. 
      - eLOC_ENG_INVALID_PARAMETER (3) --  Request failed because it contained invalid parameters.   
      - eLOC_ENG_ENGINE_BUSY (4) --  Request failed because the engine is busy. 
      - eLOC_ENG_PHONE_OFFLINE (5) --  Request failed because the phone is offline. 
      - eLOC_ENG_TIMEOUT (6) --  Request failed because it timed out. 
      - eLOC_ENG_CONFIG_NOT_SUPPORTED (7) --  Request failed because an undefined configuration was requested 
      - eLOC_ENG_INSUFFICIENT_MEMORY (8) --  Request failed because the engine could not allocate sufficent
       memory for the request. 
 */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   if specified the it contains the same user data that was passed
       in the request */
}locEngPedometerReportIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGMOTIONSTATEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_MOTION_STATE_UNKNOWN_V01 = 0, /**<  Device state is not known.  */
  eLOC_ENG_MOTION_STATE_STATIONARY_V01 = 1, /**<  Device state is Stationary.  */
  eLOC_ENG_MOTION_STATE_IN_MOTION_V01 = 2, /**<  Device state is In Motion.  */
  LOCENGMOTIONSTATEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngMotionStateEnumT_v01;
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGMOTIONMODEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_MOTION_MODE_UNKNOWN_V01 = 0, /**<  Device movement is not known.  */
  eLOC_ENG_MOTION_MODE_STATIONARY_V01 = 1, /**<  Device is not moving.  */
  eLOC_ENG_MOTION_MODE_PEDESTRIAN_UNKNOWN_V01 = 200, /**<  Device movement is in Pedestrian mode; nothing else is known about the movement.  */
  eLOC_ENG_MOTION_MODE_PEDESTRIAN_WALKING_V01 = 201, /**<  Device movement is in pedestrian Walking mode.  */
  eLOC_ENG_MOTION_MODE_PEDESTRIAN_RUNNING_V01 = 202, /**<  Device movement is in pedestrian Running mode.  */
  eLOC_ENG_MOTION_MODE_VEHICLE_UNKNOWN_V01 = 300, /**<  Device movement is in Vehicular mode; nothing else is known about the movement.  */
  LOCENGMOTIONMODEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngMotionModeEnumT_v01;
/**
    @}
  */

/** @addtogroup locEng_qmi_aggregates
    @{
  */
typedef struct {

  locEngMotionStateEnumT_v01 motion_state;
  /**<   Current motion state of the user. \n

       Valid values: \begin{itemize1}
       \item eLOC_ENG_MOTION_STATE_UNKNOWN (0) -- Device state is not known. 
       \item eLOC_ENG_MOTION_STATE_STATIONARY (1) -- Device state is Stationary.  
       \item eLOC_ENG_MOTION_STATE_IN_MOTION (2) -- Device state is In Motion.   
       \vspace{0.06in} \end{itemize1} 
       
       Absolute rest and relative rest are both indicated by setting motion_state 
       to Stationary. The relative rest state can be distinguished from absolute 
       rest by reducing probability_of_state.
  */

  locEngMotionModeEnumT_v01 motion_mode;
  /**<   Modes of user motion. \n

       Valid values: \begin{itemize1}
       \item eLOC_ENG_MOTION_MODE_ UNKNOWN (0) -- Device movement is not known. 
       \item eLOC_ENG_MOTION_MODE_ STATIONARY (1) -- Device is not moving.  
       \item eLOC_ENG_MOTION_MODE_ PEDESTRIAN_UNKNOWN (200) -- Device movement
             is in Pedestrian mode; nothing else is known about the movement.   
       \item eLOC_ENG_MOTION_MODE_ PEDESTRIAN_WALKING (201) -- Device movement
             is in Pedestrian Walking mode.   
       \item eLOC_ENG_MOTION_MODE_ PEDESTRIAN_RUNNING (202) -- Device movement
             is in Pedestrian Running mode.   
       \item eLOC_ENG_MOTION_MODE_ VEHICLE_UNKNOWN (300) -- Device movement is
             in Vehicular mode; nothing else is known about the movement.   
       \vspace{0.06in} \end{itemize1} 
       
       The motion_mode value is independent of the motion_state value.
  */

  float probability_of_state;
  /**<   Probability that the device is actually undergoing the motion state 
       specified by the combination of the values of motion_state, motion_mode, 
       and motion_sub_mode. \vspace{0.06in}

       This value is a floating point number in the range of 0 to 100, in 
       units of percent probability. Any value greater than 99.9999 is 
       applied as 99.9999. \vspace{0.06in} 

       It is recommended that if a particular combination of motion_state and
       motion_mode cannot be determined with more than 50 percent confidence, 
       that a more general statement of user motion be made. 
       For example, if the mode of In-Motion + Pedestrian-Running can only be 
       determined with 50 percent probability, and the simpler statement of In-Motion 
       can be determined with 90 percent probability, it is recommended that this field 
       be used to simply state In-Motion with 90 percent probability. \vspace{0.06in}

       If the motion_state is not known, the value in this field is not used.
  */

  uint16_t age;
  /**<   Age of the motion data in milliseconds at the time of injection. 
  */

  uint16_t timeout;
  /**<   If the age of the motion data input exceeds the timeout value, the data
       will no longer be used. The timeout value is in units of milliseconds. 
       Values in the range of 0 to 10000 are accepted. If 65535 is provided, 
       the motion data input is applied until the next input is 
       received. \vspace{0.06in}

       If the determination of motion data is an instantaneous observation 
       and no notice is guaranteed to be given via the QMI on a change in the 
       state of the motion data, it is recommended that this field be set to 0. \vspace{0.06in}

       If the determination of motion data is continuously monitored 
       external to the QMI and an update is always applied to the QMI upon any 
       change in state, a value of 65535 is used for this field. 
       Note that in this case, if a certain mode is set and is not later 
       unset (e.g., by sending in the request message with a user motion 
       state of Unknown), the value is applied indefinitely. 
  */
}locEngMotionDataStructT_v01;  /* Type */
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGWIFIAPDEVICETYPEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_WIFI_AP_DEVICE_TYPE_WLAN_802_11_A_V01 = 0, /**<  Wifi AP device is 802.11a.  */
  eLOC_ENG_WIFI_AP_DEVICE_TYPE_WLAN_802_11_B_V01 = 1, /**<  Wifi AP device is 802.11b.  */
  eLOC_ENG_WIFI_AP_DEVICE_TYPE_WLAN_802_11_G_V01 = 2, /**<  Wifi AP device is 802.11g.  */
  LOCENGWIFIAPDEVICETYPEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngWifiApDeviceTypeEnumT_v01;
/**
    @}
  */

/** @addtogroup locEng_qmi_enums
    @{
  */
typedef enum {
  LOCENGWIFIAPRTDUNITTYPEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eLOC_ENG_WIFI_AP_RTD_UNIT_MICROSEC_V01 = 0, /**<  microseconds.  */
  eLOC_ENG_WIFI_AP_RTD_UNIT_HUNDREDS_OF_NANOSEC_V01 = 1, /**<  hundreds of nano seconds.  */
  eLOC_ENG_WIFI_AP_RTD_UNIT_TENS_OF_NANOSEC_V01 = 2, /**<  tens of nano seconds.  */
  eLOC_ENG_WIFI_AP_RTD_UNIT_NANOSEC_V01 = 3, /**<  nano seconds.  */
  eLOC_ENG_WIFI_AP_RTD_UNIT_TENTH_OF_NANOSEC_V01 = 4, /**<  tenth nano seconds.  */
  LOCENGWIFIAPRTDUNITTYPEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}locEngWifiApRtdUnitTypeEnumT_v01;
/**
    @}
  */

typedef uint32_t locEngWifiMaskT_v01;
#define LOC_ENG_WIFI_APINFO_MASK_AP_TRANSMIT_POWER_V01 ((locEngWifiMaskT_v01)0x00000001) /**<  ap_transmit valid.  */
#define LOC_ENG_WIFI_APINFO_MASK_AP_ANTENNA_GAIN_V01 ((locEngWifiMaskT_v01)0x00000002) /**<  ap_antenna_gain valid.  */
#define LOC_ENG_WIFI_APINFO_MASK_AP_SNR_V01 ((locEngWifiMaskT_v01)0x00000004) /**<  ap_signal_to_noise valid.  */
#define LOC_ENG_WIFI_APINFO_MASK_AP_DEVICE_TYPE_V01 ((locEngWifiMaskT_v01)0x00000008) /**<  ap_device_type valid.  */
#define LOC_ENG_WIFI_APINFO_MASK_AP_RSSI_V01 ((locEngWifiMaskT_v01)0x00000010) /**<  ap_rssi valid.  */
#define LOC_ENG_WIFI_APINFO_MASK_AP_CHANNEL_V01 ((locEngWifiMaskT_v01)0x00000020) /**<   ap_channel valid.    */
#define LOC_ENG_WIFI_APINFO_MASK_AP_ROUNDTRIP_DELAY_V01 ((locEngWifiMaskT_v01)0x00000040) /**<   ap_roundtrip_delay valid.    */
#define LOC_ENG_WIFI_APINFO_MASK_AP_ROUNDTRIP_DELAY_ACCURACY_V01 ((locEngWifiMaskT_v01)0x00000080) /**<   ap_roundtrip_delay_accuracy valid.    */
#define LOC_ENG_WIFI_APINFO_MASK_MOBILE_SNR_V01 ((locEngWifiMaskT_v01)0x00000100) /**<   mobile_signal_to_noise valid.    */
#define LOC_ENG_WIFI_APINFO_MASK_MOBILE_RSSI_V01 ((locEngWifiMaskT_v01)0x00000200) /**<   mobile_rssi valid.    */
/** @addtogroup locEng_qmi_aggregates
    @{
  */
typedef struct {

  locEngWifiMaskT_v01 wifiInfoMask;
  /**<   Specifies which sensors are used.
  
       Valid bitmasks are specified by the following constants: \n       
         - 0x00000001 -- AP_TRANSMIT_POWER \n
         - 0x00000002 -- AP_ANTENNA_GAIN \n
         - 0x00000004 -- AP_SNR \n
         - 0x00000008 -- AP_DEVICE_TYPE \n
         - 0x00000010 -- AP_RSSI \n
         - 0x00000020 -- AP_CHANNEL \n
         - 0x00000040 -- AP_ROUNDTRIP_DELAY \n
         - 0x00000080 -- AP_ROUNDTRIP_DELAY_ACCURACY \n
         - 0x00000100 -- MOBILE_SNR \n
         - 0x00000200 -- MOBILE_RSSI */

  uint8_t mac_address[6];
  /**<   mac address: [5] containing the 6th octect; mandatory */

  int32_t ap_transmit_power;
  /**<   AP transmit power in dbm, optional;  */

  int32_t ap_antenna_gain;
  /**<   AP antenna gain in dbi, optional */

  int32_t ap_signal_to_noise;
  /**<   AP S/N received at the mobile, optional */

  locEngWifiApDeviceTypeEnumT_v01 ap_device_type;

  /* > AP device type, optional                              */
  int32_t ap_rssi;

  /* > Receiver AP signal strength indicator dBm, optional  */
  uint16_t ap_channel;

  /* > WiFi channel on which a beacon was received, optioanl  */
  uint32_t ap_round_trip_delay;

  /* > round trip delay between the mobile and the AP, in unit of ap_round_trip_delay_unit, optional  */
  locEngWifiApRtdUnitTypeEnumT_v01 ap_round_trip_delay_unit;

  /* > unit of ap_round_trip_delay and its accuracy; mandatory if ap_round_trip_delay is present  */
  uint8_t ap_round_trip_delay_accuracy;

  /* > accuracy of ap_round_trip_delay, in unit of ap_round_trip_delay_unit, optional */
  int32_t mobile_signal_to_noise;

  /* > mobile S/N received at AP, optional  */
  int32_t mobile_rssi;
}locEngWifiApInfoStructT_v01;  /* Type */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Indication Message; Injects wifi data for MSM GPS service use. */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */
}locEngEventInjectWifiApDataRequestIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Request Message; Injects wifi data for MSM GPS service use. */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Wifi AP Data */
  uint32_t wifi_ap_info_len;  /**< Must be set to # of elements in wifi_ap_info */
  locEngWifiApInfoStructT_v01 wifi_ap_info[QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V01];
  /**<   Current motion data of the client */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   opaque user data, if specified the same user data should be
       echoed back in the response indication */
}locEngInjectWifiApDataReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Indication Message; Injects wifi data for MSM GPS service use. */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Inject Motion Data Request Status */
  locEngStatusEnumT_v01 status;
  /**<   Status of the Inject Motion Data request. 

 Valid values: \n
      - eLOC_ENG_SUCCESS (0) --  Request was completed successfully.      
      - eLOC_ENG_GENERAL_FAILURE (1) --  Request failed because of a general failure. 
      - eLOC_ENG_UNSUPPORTED (2) --  Request failed because it is not supported. 
      - eLOC_ENG_INVALID_PARAMETER (3) --  Request failed because it contained invalid parameters.   
      - eLOC_ENG_ENGINE_BUSY (4) --  Request failed because the engine is busy. 
      - eLOC_ENG_PHONE_OFFLINE (5) --  Request failed because the phone is offline. 
      - eLOC_ENG_TIMEOUT (6) --  Request failed because it timed out. 
      - eLOC_ENG_CONFIG_NOT_SUPPORTED (7) --  Request failed because an undefined configuration was requested 
      - eLOC_ENG_INSUFFICIENT_MEMORY (8) --  Request failed because the engine could not allocate sufficent
       memory for the request. 
 */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   if specified the it contains the same user data that was passed
       in the request */
}locEngInjectWifiApDataIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Request Message; Injects wifi data for MSM GPS service use. */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Motion Data report timestamp in modem time  */
  uint64_t modemTimeTickMsec;
  /**<   Timestamp of sensor input in modem time which caused this indication. \n
       - Unit: Milliseconds */

  /* Mandatory */
  /*  Motion Data */
  locEngMotionDataStructT_v01 motion_data;
  /**<   Current motion data of the client */

  /* Optional */
  /*  Motion Data report timestamp in Gps time  */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  locEngGPSTimeStructT_v01 gpsTime;
  /**<   Timestamp of sensor input in GPS time. \n */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   opaque user data, if specified the same user data should be
       echoed back in the response indication */
}locEngInjectMotionDataReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Indication Message; Injects wifi data for MSM GPS service use. */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Inject Motion Data Request Status */
  locEngStatusEnumT_v01 status;
  /**<   Status of the Inject Motion Data request. 
 Valid values: \n
      - eLOC_ENG_SUCCESS (0) --  Request was completed successfully.      
      - eLOC_ENG_GENERAL_FAILURE (1) --  Request failed because of a general failure. 
      - eLOC_ENG_UNSUPPORTED (2) --  Request failed because it is not supported. 
      - eLOC_ENG_INVALID_PARAMETER (3) --  Request failed because it contained invalid parameters.   
      - eLOC_ENG_ENGINE_BUSY (4) --  Request failed because the engine is busy. 
      - eLOC_ENG_PHONE_OFFLINE (5) --  Request failed because the phone is offline. 
      - eLOC_ENG_TIMEOUT (6) --  Request failed because it timed out. 
      - eLOC_ENG_CONFIG_NOT_SUPPORTED (7) --  Request failed because an undefined configuration was requested 
      - eLOC_ENG_INSUFFICIENT_MEMORY (8) --  Request failed because the engine could not allocate sufficent
       memory for the request. 
 */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   if specified the it contains the same user data that was passed
       in the request */
}locEngInjectMotionDataIndMsgT_v01;  /* Message */
/**
    @}
  */

typedef uint32_t locEngNavConfigPosMethodMaskT_v01;
#define LOC_ENG_NAV_CONFIG_POS_METHOD_MASK_SUPPRESS_USE_OF_INJECTED_POSITION_V01 ((locEngNavConfigPosMethodMaskT_v01)0x00000001) /**<   Suppress the use of injected position from direct 
        use in position calculation.  */
#define LOC_ENG_NAV_CONFIG_POS_METHOD_MASK_USE_INSTANTANEOUS_INFORMATION_V01 ((locEngNavConfigPosMethodMaskT_v01)0x00000002) /**<   Report the measurement used and DOPs using 
        instantaneous information.  */
#define LOC_ENG_NAV_CONFIG_POS_METHOD_MASK_DISABLE_ASSIST_DATA_STORE_V01 ((locEngNavConfigPosMethodMaskT_v01)0x00000004) /**<   Disable the storage of assistance data in persistent memory.   */
#define LOC_ENG_NAV_CONFIG_ENABLE_COMPUTING_ACCURATE_AZIMUTH_ELEVATION_V01 ((locEngNavConfigPosMethodMaskT_v01)0x00000008) /**<   Enable the computing of accurate azimuth and elevation in the position engine.   */
typedef uint32_t locEngNavConfigSensorAlgorithmConfigT_v01;
#define LOC_ENG_NAV_CONFIG_SENSOR_ALGORITHM_MASK_DISABLE_INS_POSITIONING_FILTER_V01 ((locEngNavConfigSensorAlgorithmConfigT_v01)0x00000001) /**<   Inertial sensors are not to be used in Accelerometer-integrated fashion 
        with GNSS. They can still be used for aiding in heading improvement.  */
typedef uint64_t locEngVehicleDataUseControlMaskT_v01;
#define LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ACCEL_X_AXIS_V01 ((locEngVehicleDataUseControlMaskT_v01)0x0000000000000001ull) /**<  Enable the use of x-axis vehicle accelerometer sensor data  */
#define LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ACCEL_Y_AXIS_V01 ((locEngVehicleDataUseControlMaskT_v01)0x0000000000000002ull) /**<  Enable the use of y-axis vehicle accelerometer sensor data  */
#define LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ACCEL_Z_AXIS_V01 ((locEngVehicleDataUseControlMaskT_v01)0x0000000000000004ull) /**<  Enable the use of z-axis vehicle accelerometer sensor data  */
#define LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_GYRO_X_AXIS_V01 ((locEngVehicleDataUseControlMaskT_v01)0x0000000000000010ull) /**<  Enable the use of x-axis vehicle gyroscope data  */
#define LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_GYRO_Y_AXIS_V01 ((locEngVehicleDataUseControlMaskT_v01)0x0000000000000020ull) /**<  Enable the use of y-axis vehicle gyroscope data  */
#define LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_GYRO_Z_AXIS_V01 ((locEngVehicleDataUseControlMaskT_v01)0x0000000000000040ull) /**<  Enable the use of z-axis vehicle gyroscope data  */
#define LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ODOMETRY_V01 ((locEngVehicleDataUseControlMaskT_v01)0x0000000000000100ull) /**<  Enable the use of odometry data.  */
/** @addtogroup locEng_qmi_messages
    @{
  */
/** Request Message; Sets various navigation configuration parameters.  */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Navigation config timestamp in modem time  */
  uint64_t modemTimeTickMsec;
  /**<   Timestamp of sensor input in modem time which caused this indication. \n
       - Unit: Milliseconds */

  /* Optional */
  /*  Gyro Data Variance */
  uint8_t gyro_data_variance_valid;  /**< Must be set to true if gyro_data_variance is being passed */
  float gyro_data_variance;
  /**<   Gyroscope data variance. \n
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians^2/seconds^2     \n
       - Valid values: 0 to 0.25  \n
       - Default: 0.0625
  */

  /* Optional */
  /*  Gyro Bias Random Walk */
  uint8_t gyro_bias_random_walk_valid;  /**< Must be set to true if gyro_bias_random_walk is being passed */
  float gyro_bias_random_walk;
  /**<   Gyroscope bias random walk. \n
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians^2/seconds^4     \n
       - Valid values: 0, 1             \n
       - Default: 1.0e-5
  */

  /* Optional */
  /*  Virtual Gyro Gravity Averaging Length  */
  uint8_t virtual_gyro_gravity_averaging_length_valid;  /**< Must be set to true if virtual_gyro_gravity_averaging_length is being passed */
  uint32_t virtual_gyro_gravity_averaging_length;
  /**<   Virtual gyroscope gravity averaging length. \n
       - Type: 32-bit unsigned integer \n
       - Units: Seconds \n
       - Range: 1 to 60 \n
       - Default: 20 sec
  */

  /* Optional */
  /*  Heading Filter Mode (M) */
  uint8_t heading_filter_mode_valid;  /**< Must be set to true if heading_filter_mode is being passed */
  uint8_t heading_filter_mode;
  /**<   Heading filter mode. 

       \begin{itemize1}
       \item   Type: 8-bit unsigned integer
       \item   Valid values:\begin{itemize1}
          \item   -128 to -1 -- Disable heading filter
          \item   0 to 127   -- Enable heading filter
          \end{itemize1}
       \item  Default: 0     \vspace{-0.18in}
       \end{itemize1}
  */

  /* Optional */
  /*  Velocity Constraint Variance */
  uint8_t velocity_constraint_variance_valid;  /**< Must be set to true if velocity_constraint_variance is being passed */
  float velocity_constraint_variance;
  /**<   Velocity constraint variance. \n
       - Type: IEEE-754 32-bit float   \n
       - Units: Meters^2/seconds^2     \n
       - Valid values: Positive values \n
       - Default: 25.0
  */

  /* Optional */
  /*  Positioning Method */
  uint8_t positioning_method_valid;  /**< Must be set to true if positioning_method is being passed */
  locEngNavConfigPosMethodMaskT_v01 positioning_method;
  /**<   Bitmask of navigation configuration settings. 

       \begin{itemize1}
       \item   Type: 32-bit unsigned integer
       \item   Valid values:\begin{itemize1}
          \item   0x00000001 -- Suppress the use of injected position from direct 
                                use in the position calculation.
          \item   0x00000002 -- Report the measurement used and DOPs using 
                                instantaneous information.
          \item   0x00000004 -- Disable the storage of assistance data in 
                                persistent memory.
          \item   0x00000008 -- Enable the computing of accurate azimuth and elevation in 
                                the position engine.
          \item   Default (0x00000000) -- Normal operation -- Use the injected 
                  position directly in the position calculation, when possible. 
                  Report the measurement used and DOPs using filtered 
                  (i.e., non-instantaneous) information. Store the assistance 
                  data in persistent memory.  
                  Report azimuth, elevation with normal accuracy and normal precision.\vspace{0.06in}
          \end{itemize1}
       \end{itemize1}

       All unlisted bits are reserved for future use and must be set to 0.
  */

  /* Optional */
  /*  Velocity Random Walk Spectral Density  */
  uint8_t velocity_random_walk_spectral_density_valid;  /**< Must be set to true if velocity_random_walk_spectral_density is being passed */
  float velocity_random_walk_spectral_density;
  /**<   Velocity random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Meters/seconds^2/Hz^0.5    \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Acceleration Random Walk Spectral Density  */
  uint8_t acceleration_random_walk_spectral_density_valid;  /**< Must be set to true if acceleration_random_walk_spectral_density is being passed */
  float acceleration_random_walk_spectral_density;
  /**<   Acceleration random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Meters/seconds^3/Hz^0.5    \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Angle Random Walk Spectral Density  */
  uint8_t angle_random_walk_spectral_density_valid;  /**< Must be set to true if angle_random_walk_spectral_density is being passed */
  float angle_random_walk_spectral_density;
  /**<   Angle random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians/seconds/Hz^0.5    \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Rate Random Walk Spectral Density  */
  uint8_t rate_random_walk_spectral_density_valid;  /**< Must be set to true if rate_random_walk_spectral_density is being passed */
  float rate_random_walk_spectral_density;
  /**<   Rate random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians/seconds^2/Hz^0.5    \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Sensor Algorithm Configuration */
  uint8_t sensor_algorithm_config_valid;  /**< Must be set to true if sensor_algorithm_config is being passed */
  locEngNavConfigSensorAlgorithmConfigT_v01 sensor_algorithm_config;
  /**<   Bitmask of sensor algorithm configuration settings. 

       \begin{itemize1}
       \item   Type: 32-bit unsigned integer
       \item   Valid values:\begin{itemize1}
          \item   0x00000001 -- INS positioning filter is disabled. Sensors may
                                still be used by the heading filter.
          \item   Default (0x00000000) -- Normal operation -- INS positioning 
                  filter is enabled.\vspace{0.06in}
          \end{itemize1}
       \end{itemize1}

       All unlisted bits are reserved for future use and must be set to 0.
  */

  /* Optional */
  /*  Maximum Dead Reckoning Time  */
  uint8_t maximum_dead_reckoning_time_valid;  /**< Must be set to true if maximum_dead_reckoning_time is being passed */
  uint32_t maximum_dead_reckoning_time;
  /**<   Maximum dead reckoning time to propagate a navigation fix using an 
       INS filter without a GNSS signal. \n  
       - Type: 32-bit unsigned integer   \n
       - Units: Seconds                        \n
       - Valid values: Zero or positive values \n
       - Default: 30 sec
  */

  /* Optional */
  /*  INS Filter Cross Check Sigma Low Threshold  */
  uint8_t ins_filter_cross_check_sigma_low_threshold_valid;  /**< Must be set to true if ins_filter_cross_check_sigma_low_threshold is being passed */
  uint8_t ins_filter_cross_check_sigma_low_threshold;
  /**<   Low threshold for averaging the position outputs from an INS filter and 
       non-INS filter (includes heading filter). \n
       Sigma = horiz_distance (INS filter position, non-INS filter position)/(non-INS filter HEPE). \n
       The reporting logic is: 

       \begin{itemize1}
       \item   If 0 < sigma < low_threshold, the INS filter position is reported 
       \item   Else if low_threshold < sigma < high_threshold, 
               the weighted average of the INS filter and non-INS filter position is 
               reported 
       \item   Else if sigma < high_threshold, the non-INS filter position is reported  \vspace{0.12in}
       \end{itemize1}

       - Type: 8-bit unsigned integer   \n
       - Valid values: Positive values  \n
       - Default: 2
  */

  /* Optional */
  /*  INS Filter Cross Check Sigma High Threshold  */
  uint8_t ins_filter_cross_check_sigma_high_threshold_valid;  /**< Must be set to true if ins_filter_cross_check_sigma_high_threshold is being passed */
  uint8_t ins_filter_cross_check_sigma_high_threshold;
  /**<   High threshold for averaging the position outputs from an INS filter and 
       non-INS filter (includes heading filter). \n
       Sigma = horiz_distance (INS filter position, non-INS filter position)/(non-INS filter HEPE). \n
       The reporting logic is: 

       \begin{itemize1}
       \item   If 0 < sigma < low_threshold, the INS filter position is reported
       \item   Else if low_threshold < sigma < high_threshold, 
               the weighted average of the INS filter and non-INS filter position is 
               reported 
       \item   Else if sigma < high_threshold, the non-INS filter position is reported  \vspace{0.12in}
       \end{itemize1}

       - Type: 8-bit unsigned integer   \n
       - Valid values: Positive values  \n
       - Default: 10
  */

  /* Optional */
  /*  Vehicle Data Use Control */
  uint8_t vehicle_data_use_valid;  /**< Must be set to true if vehicle_data_use is being passed */
  locEngVehicleDataUseControlMaskT_v01 vehicle_data_use;
  /**<   Identifies which portions of the vehicle data to use in location 
 estimation (information provided by message 
 LOC_ENG_INJECT_VEHICLE_ SENSOR_DATA. Valid bitmasks: \n
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ACCEL_X_AXIS (0x0000000000000001) --  Enable the use of x-axis vehicle accelerometer sensor data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ACCEL_Y_AXIS (0x0000000000000002) --  Enable the use of y-axis vehicle accelerometer sensor data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ACCEL_Z_AXIS (0x0000000000000004) --  Enable the use of z-axis vehicle accelerometer sensor data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_GYRO_X_AXIS (0x0000000000000010) --  Enable the use of x-axis vehicle gyroscope data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_GYRO_Y_AXIS (0x0000000000000020) --  Enable the use of y-axis vehicle gyroscope data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_GYRO_Z_AXIS (0x0000000000000040) --  Enable the use of z-axis vehicle gyroscope data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ODOMETRY (0x0000000000000100) --  Enable the use of odometry data.  
 
 \vspace{3pt}
 Default: 0x0000000000000000, which means that
 usage of all vehicle sensor inputs is disabled 
 
 Note: All other bits are reserved for future use and must be set to 0 */

  /* Optional */
  /*  Vehicle Velocity Random Walk Spectral Density */
  uint8_t vehicle_velocity_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_velocity_random_walk_spectral_density is being passed */
  float vehicle_velocity_random_walk_spectral_density;
  /**<   Vehicle velocity random walk spectral density. \n  
       - Type: IEEE-754 32-bit float     \n
       - Units: Meters/seconds^2/Hz^0.5  \n
       - Valid values: Positive values   \n
       - Default: None
  */

  /* Optional */
  /*  Vehicle Accelerometer Random Walk Spectral Density */
  uint8_t vehicle_accel_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_accel_random_walk_spectral_density is being passed */
  float vehicle_accel_random_walk_spectral_density;
  /**<   Vehicle accelerometer random walk spectral density. \n  
       - Type: IEEE-754 32-bit float     \n
       - Units: Meters/seconds^3/Hz^0.5  \n
       - Valid values: Positive values   \n
       - Default: None
  */

  /* Optional */
  /*  Vehicle Angle Random Walk Spectral Density */
  uint8_t vehicle_angle_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_angle_random_walk_spectral_density is being passed */
  float vehicle_angle_random_walk_spectral_density;
  /**<   Vehicle angle random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians/seconds/Hz^0.5  \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Vehicle Angular Rate Random Walk Spectral Density */
  uint8_t vehicle_angular_rate_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_angular_rate_random_walk_spectral_density is being passed */
  float vehicle_angular_rate_random_walk_spectral_density;
  /**<   Vehicle angular rate random walk spectral density. \n  
       - Type: IEEE-754 32-bit float      \n
       - Units: Radians/seconds^2/Hz^0.5  \n
       - Valid values: Positive values    \n
       - Default: None
  */

  /* Optional */
  /*  Vehicle Odometry Scale Factor Random Walk Spectral Density */
  uint8_t vehicle_odometry_scale_factor_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_odometry_scale_factor_random_walk_spectral_density is being passed */
  float vehicle_odometry_scale_factor_random_walk_spectral_density;
  /**<   Vehicle odometry scale factor random walk spectral density. \n  
       - Type: IEEE-754 32-bit float  \n
       - Units: (1/seconds)/Hz^0.5    \n 
       - Range: Approximately 0.0001 to 0.001  \n
       - Default: 0.001 (actual is calibration recommended)
  */

  /* Optional */
  /*  Vehicle Odometry Variance */
  uint8_t vehicle_odometry_variance_valid;  /**< Must be set to true if vehicle_odometry_variance is being passed */
  float vehicle_odometry_variance;
  /**<   Vehicle odometry variance of each odometry sample
       (coarseness of measurement). \n
       - Type: IEEE-754 32-bit float    \n
       - Units: Meters^2                \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Navigation config request timestamp in Gps time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  locEngGPSTimeStructT_v01 gpsTime;
  /**<   Timestamp of the navigation config request in GPS time. \n */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   opaque user data, if specified the same user data should be
       echoed back in the response indication */

  /* Optional */
  /*  Maximum Dead Reckoning Time with Vehicle Sensor Inputs */
  uint8_t vehicle_maximum_dead_reckoning_time_valid;  /**< Must be set to true if vehicle_maximum_dead_reckoning_time is being passed */
  uint32_t vehicle_maximum_dead_reckoning_time;
  /**<   Maximum dead reckoning time to propagate a navigation fix with vehicle sensor
       inputs using a Path filter without a GNSS signal. \n
       - Type: 32-bit unsigned integer \n
       - Units: seconds \n
       - Valid values: Zero or Positive values \n
       - Default: None
  */

  /* Optional */
  /*  Maximum Dead Reckoning Time for Pedestrian Use Cases */
  uint8_t pedestrian_maximum_dead_reckoning_time_valid;  /**< Must be set to true if pedestrian_maximum_dead_reckoning_time is being passed */
  uint32_t pedestrian_maximum_dead_reckoning_time;
  /**<   Maximum dead reckoning time to propagate a navigation fix for pedestrian 
       use cases using a path filter without a GNSS signal. \n
       - Type: 32-bit unsigned integer \n
       - Units: Seconds \n
       - Valid values: Zero or positive values \n
       - Default: None
  */
}locEngSetNavigationConfigReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Response Message; Sets various navigation configuration parameters.  */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Inject Vehicle Sensor Data Request Status */
  locEngStatusEnumT_v01 status;
  /**<   Status of the Inject Vehicle Sensor Data request. 
 Valid values: \n
      - eLOC_ENG_SUCCESS (0) --  Request was completed successfully.      
      - eLOC_ENG_GENERAL_FAILURE (1) --  Request failed because of a general failure. 
      - eLOC_ENG_UNSUPPORTED (2) --  Request failed because it is not supported. 
      - eLOC_ENG_INVALID_PARAMETER (3) --  Request failed because it contained invalid parameters.   
      - eLOC_ENG_ENGINE_BUSY (4) --  Request failed because the engine is busy. 
      - eLOC_ENG_PHONE_OFFLINE (5) --  Request failed because the phone is offline. 
      - eLOC_ENG_TIMEOUT (6) --  Request failed because it timed out. 
      - eLOC_ENG_CONFIG_NOT_SUPPORTED (7) --  Request failed because an undefined configuration was requested 
      - eLOC_ENG_INSUFFICIENT_MEMORY (8) --  Request failed because the engine could not allocate sufficent
       memory for the request. 
 */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   if specified the it contains the same user data that was passed
       in the request */
}locEngSetNavigationConfigRespMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Request Message; Gets various navigation configuration parameters.  */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Navigation config request timestamp in modem time  */
  uint64_t modemTimeTickMsec;
  /**<   Timestamp of sensor input in modem time which caused this indication. \n
       - Unit: Milliseconds */

  /* Optional */
  /*  Navigation config request timestamp in Gps time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  locEngGPSTimeStructT_v01 gpsTime;
  /**<   Timestamp of the navigation config request in GPS time. \n */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   opaque user data, if specified the same user data should be
       echoed back in the response indication */
}locEngGetNavigationConfigReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Response Message; Gets various navigation configuration parameters.  */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Get Navigation Config Request Status */
  locEngStatusEnumT_v01 status;
  /**<   Status of the Get Navigation Config request. 
 Valid values: \n
      - eLOC_ENG_SUCCESS (0) --  Request was completed successfully.      
      - eLOC_ENG_GENERAL_FAILURE (1) --  Request failed because of a general failure. 
      - eLOC_ENG_UNSUPPORTED (2) --  Request failed because it is not supported. 
      - eLOC_ENG_INVALID_PARAMETER (3) --  Request failed because it contained invalid parameters.   
      - eLOC_ENG_ENGINE_BUSY (4) --  Request failed because the engine is busy. 
      - eLOC_ENG_PHONE_OFFLINE (5) --  Request failed because the phone is offline. 
      - eLOC_ENG_TIMEOUT (6) --  Request failed because it timed out. 
      - eLOC_ENG_CONFIG_NOT_SUPPORTED (7) --  Request failed because an undefined configuration was requested 
      - eLOC_ENG_INSUFFICIENT_MEMORY (8) --  Request failed because the engine could not allocate sufficent
       memory for the request. 
 */

  /* Optional */
  /*  Gyro Data Variance */
  uint8_t gyro_data_variance_valid;  /**< Must be set to true if gyro_data_variance is being passed */
  float gyro_data_variance;
  /**<   Gyroscope data variance. \n
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians^2/seconds^2     \n
       - Valid values: 0 to 0.25  \n
       - Default: 0.0625
  */

  /* Optional */
  /*  Gyro Bias Random Walk */
  uint8_t gyro_bias_random_walk_valid;  /**< Must be set to true if gyro_bias_random_walk is being passed */
  float gyro_bias_random_walk;
  /**<   Gyroscope bias random walk. \n
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians^2/seconds^4     \n
       - Valid values: 0, 1             \n
       - Default: 1.0e-5
  */

  /* Optional */
  /*  Virtual Gyro Gravity Averaging Length  */
  uint8_t virtual_gyro_gravity_averaging_length_valid;  /**< Must be set to true if virtual_gyro_gravity_averaging_length is being passed */
  uint32_t virtual_gyro_gravity_averaging_length;
  /**<   Virtual gyroscope gravity averaging length. \n
       - Type: 32-bit unsigned integer \n
       - Units: Seconds \n
       - Range: 1 to 60 \n
       - Default: 20 sec
  */

  /* Optional */
  /*  Heading Filter Mode (M) */
  uint8_t heading_filter_mode_valid;  /**< Must be set to true if heading_filter_mode is being passed */
  uint8_t heading_filter_mode;
  /**<   Heading filter mode. 

       \begin{itemize1}
       \item   Type: 8-bit unsigned integer
       \item   Valid values:\begin{itemize1}
          \item   -128 to -1 -- Disable heading filter
          \item   0 to 127   -- Enable heading filter
          \end{itemize1}
       \item  Default: 0     \vspace{-0.18in}
       \end{itemize1}
  */

  /* Optional */
  /*  Velocity Constraint Variance */
  uint8_t velocity_constraint_variance_valid;  /**< Must be set to true if velocity_constraint_variance is being passed */
  float velocity_constraint_variance;
  /**<   Velocity constraint variance. \n
       - Type: IEEE-754 32-bit float   \n
       - Units: Meters^2/seconds^2     \n
       - Valid values: Positive values \n
       - Default: 25.0
  */

  /* Optional */
  /*  Positioning Method */
  uint8_t positioning_method_valid;  /**< Must be set to true if positioning_method is being passed */
  locEngNavConfigPosMethodMaskT_v01 positioning_method;
  /**<   Bitmask of navigation configuration settings. 

       \begin{itemize1}
       \item   Type: 32-bit unsigned integer
       \item   Valid values:\begin{itemize1}
          \item   0x00000001 -- Suppress the use of injected position from direct 
                                use in the position calculation.
          \item   0x00000002 -- Report the measurement used and DOPs using 
                                instantaneous information.
          \item   0x00000004 -- Disable the storage of assistance data in 
                                persistent memory.
          \item   0x00000008 -- Enable the computing of accurate azimuth and elevation in 
                                the position engine.
          \item   Default (0x00000000) -- Normal operation -- Use the injected 
                  position directly in the position calculation, when possible. 
                  Report the measurement used and DOPs using filtered 
                  (i.e., non-instantaneous) information. Store the assistance 
                  data in persistent memory.  
                  Report azimuth, elevation with normal accuracy and normal precision.\vspace{0.06in}
          \end{itemize1}
       \end{itemize1}

       All unlisted bits are reserved for future use and must be set to 0.
  */

  /* Optional */
  /*  Velocity Random Walk Spectral Density  */
  uint8_t velocity_random_walk_spectral_density_valid;  /**< Must be set to true if velocity_random_walk_spectral_density is being passed */
  float velocity_random_walk_spectral_density;
  /**<   Velocity random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Meters/seconds^2/Hz^0.5    \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Acceleration Random Walk Spectral Density  */
  uint8_t acceleration_random_walk_spectral_density_valid;  /**< Must be set to true if acceleration_random_walk_spectral_density is being passed */
  float acceleration_random_walk_spectral_density;
  /**<   Acceleration random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Meters/seconds^3/Hz^0.5    \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Angle Random Walk Spectral Density  */
  uint8_t angle_random_walk_spectral_density_valid;  /**< Must be set to true if angle_random_walk_spectral_density is being passed */
  float angle_random_walk_spectral_density;
  /**<   Angle random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians/seconds/Hz^0.5    \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Rate Random Walk Spectral Density  */
  uint8_t rate_random_walk_spectral_density_valid;  /**< Must be set to true if rate_random_walk_spectral_density is being passed */
  float rate_random_walk_spectral_density;
  /**<   Rate random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians/seconds^2/Hz^0.5    \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Sensor Algorithm Configuration */
  uint8_t sensor_algorithm_config_valid;  /**< Must be set to true if sensor_algorithm_config is being passed */
  locEngNavConfigSensorAlgorithmConfigT_v01 sensor_algorithm_config;
  /**<   Bitmask of sensor algorithm configuration settings. 

       \begin{itemize1}
       \item   Type: 32-bit unsigned integer
       \item   Valid values:\begin{itemize1}
          \item   0x00000001 -- INS positioning filter is disabled. Sensors may
                                still be used by the heading filter.
          \item   Default (0x00000000) -- Normal operation -- INS positioning 
                  filter is enabled.\vspace{0.06in}
          \end{itemize1}
       \end{itemize1}

       All unlisted bits are reserved for future use and must be set to 0.
  */

  /* Optional */
  /*  Maximum Dead Reckoning Time  */
  uint8_t maximum_dead_reckoning_time_valid;  /**< Must be set to true if maximum_dead_reckoning_time is being passed */
  uint32_t maximum_dead_reckoning_time;
  /**<   Maximum dead reckoning time to propagate a navigation fix using an 
       INS filter without a GNSS signal. \n  
       - Type: 32-bit unsigned integer   \n
       - Units: Seconds                        \n
       - Valid values: Zero or positive values \n
       - Default: 30 sec
  */

  /* Optional */
  /*  INS Filter Cross Check Sigma Low Threshold  */
  uint8_t ins_filter_cross_check_sigma_low_threshold_valid;  /**< Must be set to true if ins_filter_cross_check_sigma_low_threshold is being passed */
  uint8_t ins_filter_cross_check_sigma_low_threshold;
  /**<   Low threshold for averaging the position outputs from an INS filter and 
       non-INS filter (includes heading filter). \n
       Sigma = horiz_distance (INS filter position, non-INS filter position)/(non-INS filter HEPE). \n
       The reporting logic is: 

       \begin{itemize1}
       \item   If 0 < sigma < low_threshold, the INS filter position is reported 
       \item   Else if low_threshold < sigma < high_threshold, 
               the weighted average of the INS filter and non-INS filter position is 
               reported 
       \item   Else if sigma < high_threshold, the non-INS filter position is reported  \vspace{0.12in}
       \end{itemize1}

       - Type: 8-bit unsigned integer   \n
       - Valid values: Positive values  \n
       - Default: 2
  */

  /* Optional */
  /*  INS Filter Cross Check Sigma High Threshold  */
  uint8_t ins_filter_cross_check_sigma_high_threshold_valid;  /**< Must be set to true if ins_filter_cross_check_sigma_high_threshold is being passed */
  uint8_t ins_filter_cross_check_sigma_high_threshold;
  /**<   High threshold for averaging the position outputs from an INS filter and 
       non-INS filter (includes heading filter). \n
       Sigma = horiz_distance (INS filter position, non-INS filter position)/(non-INS filter HEPE). \n
       The reporting logic is: 

       \begin{itemize1}
       \item   If 0 < sigma < low_threshold, the INS filter position is reported
       \item   Else if low_threshold < sigma < high_threshold, 
               the weighted average of the INS filter and non-INS filter position is 
               reported 
       \item   Else if sigma < high_threshold, the non-INS filter position is reported  \vspace{0.12in}
       \end{itemize1}

       - Type: 8-bit unsigned integer   \n
       - Valid values: Positive values  \n
       - Default: 10
  */

  /* Optional */
  /*  Vehicle Data Use Control */
  uint8_t vehicle_data_use_valid;  /**< Must be set to true if vehicle_data_use is being passed */
  locEngVehicleDataUseControlMaskT_v01 vehicle_data_use;
  /**<   Identifies which portions of the vehicle data to use in location 
 estimation (information provided by message 
 LOC_ENG_INJECT_VEHICLE_ SENSOR_DATA. Valid bitmasks: \n
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ACCEL_X_AXIS (0x0000000000000001) --  Enable the use of x-axis vehicle accelerometer sensor data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ACCEL_Y_AXIS (0x0000000000000002) --  Enable the use of y-axis vehicle accelerometer sensor data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ACCEL_Z_AXIS (0x0000000000000004) --  Enable the use of z-axis vehicle accelerometer sensor data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_GYRO_X_AXIS (0x0000000000000010) --  Enable the use of x-axis vehicle gyroscope data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_GYRO_Y_AXIS (0x0000000000000020) --  Enable the use of y-axis vehicle gyroscope data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_GYRO_Z_AXIS (0x0000000000000040) --  Enable the use of z-axis vehicle gyroscope data 
      - LOC_ENG_VEHICLE_DATA_ENABLE_USE_MASK_ODOMETRY (0x0000000000000100) --  Enable the use of odometry data.  
 
 \vspace{3pt}
 Default: 0x0000000000000000, which means that
 usage of all vehicle sensor inputs is disabled 
 
 Note: All other bits are reserved for future use and must be set to 0 */

  /* Optional */
  /*  Vehicle Velocity Random Walk Spectral Density */
  uint8_t vehicle_velocity_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_velocity_random_walk_spectral_density is being passed */
  float vehicle_velocity_random_walk_spectral_density;
  /**<   Vehicle velocity random walk spectral density. \n  
       - Type: IEEE-754 32-bit float     \n
       - Units: Meters/seconds^2/Hz^0.5  \n
       - Valid values: Positive values   \n
       - Default: None
  */

  /* Optional */
  /*  Vehicle Accelerometer Random Walk Spectral Density */
  uint8_t vehicle_accel_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_accel_random_walk_spectral_density is being passed */
  float vehicle_accel_random_walk_spectral_density;
  /**<   Vehicle accelerometer random walk spectral density. \n  
       - Type: IEEE-754 32-bit float     \n
       - Units: Meters/seconds^3/Hz^0.5  \n
       - Valid values: Positive values   \n
       - Default: None
  */

  /* Optional */
  /*  Vehicle Angle Random Walk Spectral Density */
  uint8_t vehicle_angle_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_angle_random_walk_spectral_density is being passed */
  float vehicle_angle_random_walk_spectral_density;
  /**<   Vehicle angle random walk spectral density. \n  
       - Type: IEEE-754 32-bit float    \n
       - Units: Radians/seconds/Hz^0.5  \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  Vehicle Angular Rate Random Walk Spectral Density */
  uint8_t vehicle_angular_rate_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_angular_rate_random_walk_spectral_density is being passed */
  float vehicle_angular_rate_random_walk_spectral_density;
  /**<   Vehicle angular rate random walk spectral density. \n  
       - Type: IEEE-754 32-bit float      \n
       - Units: Radians/seconds^2/Hz^0.5  \n
       - Valid values: Positive values    \n
       - Default: None
  */

  /* Optional */
  /*  Vehicle Odometry Scale Factor Random Walk Spectral Density */
  uint8_t vehicle_odometry_scale_factor_random_walk_spectral_density_valid;  /**< Must be set to true if vehicle_odometry_scale_factor_random_walk_spectral_density is being passed */
  float vehicle_odometry_scale_factor_random_walk_spectral_density;
  /**<   Vehicle odometry scale factor random walk spectral density. \n  
       - Type: IEEE-754 32-bit float  \n
       - Units: (1/seconds)/Hz^0.5    \n 
       - Range: Approximately 0.0001 to 0.001  \n
       - Default: 0.001 (actual is calibration recommended)
  */

  /* Optional */
  /*  Vehicle Odometry Variance */
  uint8_t vehicle_odometry_variance_valid;  /**< Must be set to true if vehicle_odometry_variance is being passed */
  float vehicle_odometry_variance;
  /**<   Vehicle odometry variance of each odometry sample
       (coarseness of measurement). \n
       - Type: IEEE-754 32-bit float    \n
       - Units: Meters^2                \n
       - Valid values: Positive values  \n
       - Default: None
  */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   if specified the it contains the same user data that was passed
       in the request */

  /* Optional */
  /*  Maximum Dead Reckoning Time with Vehicle Sensor Inputs */
  uint8_t vehicle_maximum_dead_reckoning_time_valid;  /**< Must be set to true if vehicle_maximum_dead_reckoning_time is being passed */
  uint32_t vehicle_maximum_dead_reckoning_time;
  /**<   Maximum dead reckoning time to propagate a navigation fix with vehicle sensor
       inputs using a Path filter without a GNSS signal. \n
       - Type: 32-bit unsigned integer \n
       - Units: seconds \n
       - Valid values: Zero or Positive values \n
       - Default: None
  */

  /* Optional */
  /*  Maximum Dead Reckoning Time for Pedestrian Use Cases */
  uint8_t pedestrian_maximum_dead_reckoning_time_valid;  /**< Must be set to true if pedestrian_maximum_dead_reckoning_time is being passed */
  uint32_t pedestrian_maximum_dead_reckoning_time;
  /**<   Maximum dead reckoning time to propagate a navigation fix for pedestrian 
       use cases using a path filter without a GNSS signal. \n
       - Type: 32-bit unsigned integer \n
       - Units: Seconds \n
       - Valid values: Zero or positive values \n
       - Default: None
  */
}locEngGetNavigationConfigRespMsgT_v01;  /* Message */
/**
    @}
  */

typedef uint64_t locEngAxesMaskT_v01;
#define LOC_ENG_MASK_X_AXIS_V01 ((locEngAxesMaskT_v01)0x0000000000000001ull) /**<  The x-axis is valid \n  */
#define LOC_ENG_MASK_Y_AXIS_V01 ((locEngAxesMaskT_v01)0x0000000000000002ull) /**<  The y-axis is valid \n  */
#define LOC_ENG_MASK_Z_AXIS_V01 ((locEngAxesMaskT_v01)0x0000000000000004ull) /**<  The z-axis is valid.  */
/** @addtogroup locEng_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t sample_timetag_offset;
  /**<   Sample time offset. This time offset must be 
       relative to the sensor time of the first sample. \n 
       - Type: Unsigned integer  \n 
       - Units: Microseconds     \n
       - Range: Up to over 4000 sec */

  uint32_t axis_sample_len;  /**< Must be set to # of elements in axis_sample */
  float axis_sample[LOC_ENG_VEHICLE_SENSOR_DATA_MAX_AXES_V01];
  /**<   Sensor axis sample.        \n 
       - Type: Floating point     \n
       - Units accelerometer:     \n
         ( (meters)/(seconds^2) ) \n
       - Units gyroscope:         \n
         ( (radians)/(seconds) ) 
         
       \vspace{3pt}
       Note: The axes samples must be in the following order: \n 
             - x-axis \n
             - y-axis \n
             - z-axis */
}locEngVehicleSensorSampleStructT_v01;  /* Type */
/**
    @}
  */

/** @addtogroup locEng_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t sensor_sample_time_base;
  /**<   Denotes a 32-bit time tag of a reference time, from which 
       all samples in this request are offset. Note that this time must 
       be the same or (slightly) earlier than the first (oldest) 
       sample in this request. \n
       - Type: Unsigned integer  \n
       - Units: 1 ms             \n
       - Range: Approximately 4 million sec or almost 50 days between 
                rollovers */

  locEngAxesMaskT_v01 axes_validity;
  /**<   Identifies the axes that are valid for all the sensor samples. 
 Valid bitmasks: 
      - LOC_ENG_MASK_X_AXIS (0x0000000000000001) --  The x-axis is valid \n 
      - LOC_ENG_MASK_Y_AXIS (0x0000000000000002) --  The y-axis is valid \n 
      - LOC_ENG_MASK_Z_AXIS (0x0000000000000004) --  The z-axis is valid.  */

  uint32_t sensorData_len;  /**< Must be set to # of elements in sensorData */
  locEngVehicleSensorSampleStructT_v01 sensorData[LOC_ENG_VEHICLE_SENSOR_DATA_MAX_SAMPLES_V01];
}locEngVehicleSensorSampleListStructT_v01;  /* Type */
/**
    @}
  */

typedef uint64_t locEngVehicleOdometryMeasDeviationMaskT_v01;
#define LOC_ENG_MASK_VEHICLE_ODOMETRY_REVERSE_MOVEMENT_V01 ((locEngVehicleOdometryMeasDeviationMaskT_v01)0x0000000000000001ull) /**<  Odometry data includes at least some data where
       the vehicle may have been moving in the reverse direction. 
       If odometry data may be in reverse, this bit must be set.
       If odometry data is all in the forward direction, this bit 
       must not be set.  */
#define LOC_ENG_MASK_VEHICLE_ODOMETRY_AFFECTED_BY_ERRORS_V01 ((locEngVehicleOdometryMeasDeviationMaskT_v01)0x0000000000000002ull) /**<  Odometry data includes at least some data affected
       by a major error source affecting distance-traveled accuracy, 
       such as wheel slippage due to skidding, gravel, snow, or ice as
       detected by the vehicle, e.g., via an ABS or other system.  */
#define LOC_ENG_MASK_VEHICLE_ODOMETRY_ABSOLUTE_MEASUREMENT_V01 ((locEngVehicleOdometryMeasDeviationMaskT_v01)0x0000000000000004ull) /**<  Odometry data is an absolute amount since the vehicle
       began service, and is the same vehicle that is regularly used with 
       this device (so that the offset of this value, since the last time
       this measurement was used by the location engine, can be safely used
       as a likely correct estimate of the distance traveled since last 
       use).  */
typedef uint64_t locEngVehicleOdometryWheelFlagsMaskT_v01;
#define LOC_ENG_MASK_VEHICLE_ODOMETRY_LEFT_AND_RIGHT_AVERAGE_V01 ((locEngVehicleOdometryWheelFlagsMaskT_v01)0x0000000000000001ull) /**<  Average of left and right non-turning wheels  */
#define LOC_ENG_MASK_VEHICLE_ODOMETRY_LEFT_V01 ((locEngVehicleOdometryWheelFlagsMaskT_v01)0x0000000000000002ull) /**<  Left side, non-turning wheel  */
#define LOC_ENG_MASK_VEHICLE_ODOMETRY_RIGHT_V01 ((locEngVehicleOdometryWheelFlagsMaskT_v01)0x0000000000000004ull) /**<  Right side, non-turning wheel  */
/** @addtogroup locEng_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t sample_timetag_offset;
  /**<   Sample time offset. This time offset must be 
       relative to the sensor time of the first sample. \n 
       - Type: Unsigned integer  \n 
       - Units: Microseconds     \n
       - Range: Up to over 4000 sec */

  uint32_t distance_travelled_len;  /**< Must be set to # of elements in distance_travelled */
  uint32_t distance_travelled[LOC_ENG_VEHICLE_ODOMETRY_MAX_MEASUREMENTS_V01];
  /**<    Distance traveled (odometry) sample offset. \n 
        - Type: Unsigned integer                      \n
        - Units of accumulated distance: Millimeters  \n
        - Range: Over 4000 km

        \vspace{3pt}
        This measurement (in units of millimeters) is added to 
        the distance_travelled_base measurement (in meters) to 
        get the total distance traveled sample value.

        \vspace{3pt}
        Note: The order of measurements must be: \n
              - Average of left and right  \n
              - Left                       \n
              - Right 
   */
}locEngVehicleOdometrySampleStructT_v01;  /* Type */
/**
    @}
  */

/** @addtogroup locEng_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t sensor_sample_time_base;
  /**<   Denotes a 32-bit time tag of a reference time, from which 
       all samples in this request are offset. Note that this time must 
       be the same or (slightly) earlier than the first (oldest) 
       sample in this request. \n
       - Type: Unsigned integer  \n
       - Units: 1 ms             \n
       - Range: Approximately 4 million sec or almost 50 days between 
                rollovers */

  locEngVehicleOdometryMeasDeviationMaskT_v01 flags;
  /**<   Flags to indicate any deviation from the default measurement 
 assumptions. Valid bitmasks: \n
      - LOC_ENG_MASK_VEHICLE_ODOMETRY_REVERSE_MOVEMENT (0x0000000000000001) --  Odometry data includes at least some data where
       the vehicle may have been moving in the reverse direction. 
       If odometry data may be in reverse, this bit must be set.
       If odometry data is all in the forward direction, this bit 
       must not be set. 
      - LOC_ENG_MASK_VEHICLE_ODOMETRY_AFFECTED_BY_ERRORS (0x0000000000000002) --  Odometry data includes at least some data affected
       by a major error source affecting distance-traveled accuracy, 
       such as wheel slippage due to skidding, gravel, snow, or ice as
       detected by the vehicle, e.g., via an ABS or other system. 
      - LOC_ENG_MASK_VEHICLE_ODOMETRY_ABSOLUTE_MEASUREMENT (0x0000000000000004) --  Odometry data is an absolute amount since the vehicle
       began service, and is the same vehicle that is regularly used with 
       this device (so that the offset of this value, since the last time
       this measurement was used by the location engine, can be safely used
       as a likely correct estimate of the distance traveled since last 
       use).  */

  locEngVehicleOdometryWheelFlagsMaskT_v01 wheel_flags;
  /**<   Delineates for which wheels the measurements are provided
 in the samples, where one or more of the following
 bits must be set, and data samples aligned with these axes must
 appear in groups, in this order. Valid bitmasks: \n
      - LOC_ENG_MASK_VEHICLE_ODOMETRY_LEFT_AND_RIGHT_AVERAGE (0x0000000000000001) --  Average of left and right non-turning wheels 
      - LOC_ENG_MASK_VEHICLE_ODOMETRY_LEFT (0x0000000000000002) --  Left side, non-turning wheel 
      - LOC_ENG_MASK_VEHICLE_ODOMETRY_RIGHT (0x0000000000000004) --  Right side, non-turning wheel  */

  uint32_t distance_travelled_base;
  /**<   Distance traveled base. \n
        - Type: Unsigned integer                 \n
        - Units of accumulated distance: Meters  \n
        - Range: Over 4,000,0000 km 

        \vspace{3pt}
        Distance traveled (odometry) is to be reported in a continuously 
        accumulating way from device power-up. It may be an incremental 
        distance starting at zero, or another arbitrary point from device 
        power-up, or the absolute distance traveled by the vehicle
        (and if so, set LOC_ENG_MASK_VEHICLE_ODOMETRY_ ABSOLUTE_MEASUREMENT), 
        as long as the distance grows incrementally from device power-up.
        
        \vspace{3pt}
        This distance_travelled_base is added to the distrance_travelled offset
        of each sample to get the absolute distance of each sample 
        point.

        \vspace{3pt}
        Distance traveled errors are expected to be primarily due to 
        scale-factor, with some allowance for "noise" due to minor slippage 
        events, e.g., gravel. Major wheel slippage events that affect 
        odometry, must be flagged; see the flags field.

        Note: Other events, such as a vehicle traveling in reverse, may 
        also affect the available accuracy of this information, and notification
        of those events must be provided; see the flags field. */

  uint32_t odometry_data_len;  /**< Must be set to # of elements in odometry_data */
  locEngVehicleOdometrySampleStructT_v01 odometry_data[LOC_ENG_VEHICLE_SENSOR_DATA_MAX_SAMPLES_V01];
  /**<   Variable length array to specify the odometry samples.
       Max Length of array is: 50 */
}locEngVehicleOdometrySampleListStructT_v01;  /* Type */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Request Message; Injects on-vehicle sensor data into the location engine.
             \label{idl:injectVehicleSensorData} */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Motion Data report timestamp in modem time  */
  uint64_t modemTimeTickMsec;
  /**<   Timestamp of sensor input in modem time which caused this indication. \n
       - Unit: Milliseconds */

  /* Optional */
  /*  On-Vehicle Accelerometer Data */
  uint8_t accel_data_valid;  /**< Must be set to true if accel_data is being passed */
  locEngVehicleSensorSampleListStructT_v01 accel_data;

  /* Optional */
  /*  On-Vehicle Angular Rotation Data */
  uint8_t ang_rotation_data_valid;  /**< Must be set to true if ang_rotation_data is being passed */
  locEngVehicleSensorSampleListStructT_v01 ang_rotation_data;

  /* Optional */
  /*  Odometry Data */
  uint8_t odometry_data_valid;  /**< Must be set to true if odometry_data is being passed */
  locEngVehicleOdometrySampleListStructT_v01 odometry_data;

  /* Optional */
  /*  External Time Sync Information */
  uint8_t change_in_time_scales_valid;  /**< Must be set to true if change_in_time_scales is being passed */
  int32_t change_in_time_scales;
  /**<   This field is to be used in conjunction with an external 
       timesync mechanism that is aligning the vehicle sensor time scale, 
       with the on-device sensor time scale, to ensure that updates in
       that time offset do not appear as jumps in the relative sensor time
       of the samples provided in this request. If there is no such sync
       mechanism, e.g., if only the vehicle time is provided, this field
       may be left at zero.

       \vspace{3pt}
       This field is defined as the change from the previously sent QMI
       request, with similar TLVs 0x10, 0x11, or 0x12 in it, to this QMI 
       request, in the amount that the sensor time is ahead of an 
       "external vehicle time." \n

       - Type: Signed integer  \n
       - Units: 1 ms           \n
       - Range: Approximately -2100 sec to +2100 sec, where
                full-scale (minimum and maximum value) is interpreted
                as equal to or more than this value of an offset change
                (unlikely to be reached in practice, unless there is a 
                startup, major resync, or some other rollover event) */

  /* Optional */
  /*  Vehicle sensor data report timestamp in Gps time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  locEngGPSTimeStructT_v01 gpsTime;
  /**<   Timestamp of vehicle sensor input in GPS time. \n */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   opaque user data, if specified the same user data should be
       echoed back in the response indication */
}locEngInjectVehicleSensorDataReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup locEng_qmi_messages
    @{
  */
/** Response Message; Injects on-vehicle sensor data into the location engine.
             \label{idl:injectVehicleSensorData} */
typedef struct {

  /* Mandatory */
  /*  Message Source */
  locEngMsgSourceEnumT_v01 messageSource;
  /**<   the GNSS task that sent this message
 Valid Values :\n
      - eLOC_ENG_MSG_SOURCE_LOC_MW (0) --  Message was sent by loc MW task
      - eLOC_ENG_MSG_SOURCE_LBS (1) --  Message was sent by LBS task
      - eLOC_ENG_MSG_SOURCE_SDP (2) --  Message was sent by SDP task
      - eLOC_ENG_MSG_SOURCE_NF (3) --  Message was sent by NF task
      - eLOC_ENG_MSG_SOURCE_SM (4) --  Message was sent by SM task */

  /* Mandatory */
  /*  Inject Vehicle Sensor Data Request Status */
  locEngStatusEnumT_v01 status;
  /**<   Status of the Inject Vehicle Sensor Data request. 

 Valid values: \n
      - eLOC_ENG_SUCCESS (0) --  Request was completed successfully.      
      - eLOC_ENG_GENERAL_FAILURE (1) --  Request failed because of a general failure. 
      - eLOC_ENG_UNSUPPORTED (2) --  Request failed because it is not supported. 
      - eLOC_ENG_INVALID_PARAMETER (3) --  Request failed because it contained invalid parameters.   
      - eLOC_ENG_ENGINE_BUSY (4) --  Request failed because the engine is busy. 
      - eLOC_ENG_PHONE_OFFLINE (5) --  Request failed because the phone is offline. 
      - eLOC_ENG_TIMEOUT (6) --  Request failed because it timed out. 
      - eLOC_ENG_CONFIG_NOT_SUPPORTED (7) --  Request failed because an undefined configuration was requested 
      - eLOC_ENG_INSUFFICIENT_MEMORY (8) --  Request failed because the engine could not allocate sufficent
       memory for the request. 
 */

  /* Optional */
  /*  user data */
  uint8_t userData_valid;  /**< Must be set to true if userData is being passed */
  uint64_t userData;
  /**<   if specified the it contains the same user data that was passed
       in the request */
}locEngInjectVehicleSensorDataRespMsgT_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_LOC_ENG_EVENT_INJECT_WIFI_AP_DATA_REQUEST_V01 
//#define REMOVE_LOC_ENG_EVENT_MOTION_DATA_CONTROL_V01 
//#define REMOVE_LOC_ENG_EVENT_PEDOMETER_CONTROL_V01 
//#define REMOVE_LOC_ENG_GET_NAVIGATION_CONFIG_V01 
//#define REMOVE_LOC_ENG_INJECT_MOTION_DATA_V01 
//#define REMOVE_LOC_ENG_INJECT_VEHICLE_SENSOR_DATA_V01 
//#define REMOVE_LOC_ENG_INJECT_WIFI_AP_DATA_V01 
//#define REMOVE_LOC_ENG_PEDOMETER_REPORT_V01 
//#define REMOVE_LOC_ENG_POS_V01 
//#define REMOVE_LOC_ENG_SET_NAVIGATION_CONFIG_V01 

/*Service Message Definition*/
/** @addtogroup locEng_qmi_msg_ids
    @{
  */
#define LOC_ENG_POS_V01 0x0020
#define LOC_ENG_EVENT_PEDOMETER_CONTROL_IND_V01 0x0021
#define LOC_ENG_EVENT_MOTION_DATA_CONTROL_IND_V01 0x0022
#define LOC_ENG_PEDOMETER_REPORT_REQ_V01 0x0023
#define LOC_ENG_PEDOMETER_REPORT_IND_V01 0x0023
#define LOC_ENG_INJECT_MOTION_DATA_REQ_V01 0x0024
#define LOC_ENG_INJECT_MOTION_DATA_IND_V01 0x0024
#define LOC_ENG_SET_NAVIGATION_CONFIG_REQ_V01 0x0025
#define LOC_ENG_SET_NAVIGATION_CONFIG_RESP_V01 0x0025
#define LOC_ENG_GET_NAVIGATION_CONFIG_REQ_V01 0x0026
#define LOC_ENG_GET_NAVIGATION_CONFIG_RESP_V01 0x0026
#define LOC_ENG_INJECT_VEHICLE_SENSOR_DATA_REQ_V01 0x0027
#define LOC_ENG_INJECT_VEHICLE_SENSOR_DATA_RESP_V01 0x0027
#define LOC_ENG_EVENT_INJECT_WIFI_AP_DATA_REQUEST_IND_V01 0x0028
#define LOC_ENG_INJECT_WIFI_AP_DATA_REQ_V01 0x0029
#define LOC_ENG_INJECT_WIFI_AP_DATA_IND_V01 0x0029
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro locEng_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type locEng_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define locEng_get_service_object_v01( ) \
          locEng_get_service_object_internal_v01( \
            LOCENG_V01_IDL_MAJOR_VERS, LOCENG_V01_IDL_MINOR_VERS, \
            LOCENG_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

