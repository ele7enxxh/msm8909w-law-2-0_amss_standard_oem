#ifndef SAML_CLIENT_TYPES_H
#define SAML_CLIENT_TYPES_H
/*============================================================================
@file saml_client_types.h

  SAMLite API header containing the client types needed for SAMLite communication.

  Copyright (c) 2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary 
============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/samlite/inc/saml_client_types.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/13/14   mk      Initial Version

=============================================================================*/

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "comdef.h"

/*----------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/
#define SAML_PED_ALIGNMENT_MIN_SAMPLE_RATE_HZ     20
#define SAML_PED_ALIGNMENT_MAX_SAMPLE_RATE_HZ     50

/*! Check if value is set in mask */
#define SAML_MASK_IS_SET(mask, value) ((mask) & (1 << (value)))
/*! Add value to mask */
#define SAML_MASK_SET(mask, value) ((mask) |= (1 << (value)))
/*! Remove value from mask */
#define SAML_MASK_CLEAR(mask, value) ((mask) &= ~(1 << (value)))


/*----------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/

/* NOTE: SENSOR-SERVICE REQUEST TYPE STRUCTURES ARE COPIED FROM SLIM API BUT 
NOT MAPPED BECAUSED THE TWO APIs CAN BE CHANGED INDEPENDENTLY. */

typedef enum
{
  eSAML_INJECT_PEDOMETER_REPORT_ON_STEP_EVENT    = 0,
  /**< Report is injected on detection of a step event */
  eSAML_INJECT_PEDOMETER_REPORT_PERIODICALLY  = 1,
  /**< Report is injected periodically */

  eSAML_INJECT_PEDOMETER_REPORT_MAX = 2147483647 /* Force 32bit */
} saml_InjectPedometerReportEnumT;

typedef struct
{
  uint64            t_Timestamp;
  /**<   Time stamp of the last step event in this report, i.e., the time stamp
         of the step event that caused this report to be generated. 
         The time stamp is in the time reference scale that is 
         used by the pedometer time source. \n
         - Units: Milliseconds */

  uint32            q_Flags;
  /**< SAMLite flags. Combination of:  \n
         SAML_FLAGS_MASK_TIME_JUMP  \n
         SAML_FLAGS_MASK_TIME_UNC_HIGH */

  /*  Step Count */
  uint32            q_StepCount;
  /**<   Number of steps counted during the time interval. */

  uint8             u_StepConfidence_valid;
  /**< Must be set to true if stepConfidence is being passed */
  uint8             u_StepConfidence;
  /**<   Confidence associated with the step. This field is only applicable
         for a single step report, i.e., if the step count is one. \n
         - Range: 0 to 100 \n
         \textbf{Note:} The report is ignored if confidence is 0. */

  uint8             u_StepCountUncertainty_valid;
  /**< Must be set to true if stepCountUncertainty is being passed */
  float             f_StepCountUncertainty;
  /**<   Uncertainty (in steps) associated with the step count. */

  uint8             b_StepRate_valid;
  /**< Must be set to true if stepRate is being passed */
  float             f_StepRate;
  /**<   Current estimate for the rate of steps per second. \n
       - Units: steps/second \n
       - Range: >= 0.0 */

  uint8             u_StrideLength_valid;
  /**< Must be set to true if strideLength is being passed */
  float             f_StrideLength;
  /**<   Current estimate of the stride length. \n
       - Type: Float \n
       - Units: Meters/stride \n
       - Range: >= 0 */

  uint8             b_StrideLengthUncertainty_valid;
  /**< Must be set to true if strideLengthUncertainty is being passed */
  float             f_StrideLengthUncertainty;
  /**<   Uncertainty associated with the stride length, provided at a 1-sigma 
       (68 percent) confidence. \n
       - Type: Float \n
       - Units: Meters/stride \n
       - Range: >= 0 */

  uint8             b_StepCountFractional_valid;
  /**< Must be set to true if stepCountFractional is being passed */
  float             f_StepCountFractional;
  /**<   Sub-integer portion of the number of steps counted during the time 
       interval. If the report is generated on event, this will be 0. \n
       - Type: Float \n
       - Units: steps \n
       - Range: >= 0.0 and < 1.0 */

  uint8             b_ReportType_valid;
  /**< Must be set to true if reportType is being passed */
  saml_InjectPedometerReportEnumT e_ReportType;
  /**< Identifies the type of report generated. One of: \n
         eSAML_INJECT_PEDOMETER_REPORT_ON_STEP_EVENT \n
         eSAML_INJECT_PEDOMETER_REPORT_PERIODICALLY \n */
} saml_PedometerDataStructT;

/*
 *  Motion data
 */

typedef enum
{
  eSAML_MOTION_STATE_UNKNOWN      = 0,
  /**< Device state is not known. */
  eSAML_MOTION_STATE_STATIONARY   = 1,
  /**< Device state is Stationary. */
  eSAML_MOTION_STATE_IN_MOTION    = 2,
  /**< Device state is In Motion. */

  eSAML_MOTION_STATE_MAX = 2147483647  /* Force 32bit */
} saml_MotionStateEnumT;

typedef enum
{

  eSAML_MOTION_MODE_UNKNOWN              =  0,
  /**< Device movement is not known. */
  /* Add more unknown motion modes here */

  eSAML_MOTION_MODE_STATIONARY,
  /**< Device is not moving. */
  /* Add more stationary motion modes here */

  eSAML_MOTION_MODE_PEDESTRIAN_UNKNOWN   =  200,
  /**< Device movement is in Pedestrian mode; nothing else is known about the movement. */
  eSAML_MOTION_MODE_PEDESTRIAN_WALKING,
  /**< Device movement is in pedestrian Walking mode. */
  eSAML_MOTION_MODE_PEDESTRIAN_RUNNING,
  /**< Device movement is in pedestrian Running mode. */
  /* Add more pedestrian modes here */

  eSAML_MOTION_MODE_VEHICLE_UNKNOWN      =  300,
  /**< Device movement is in Vehicular mode; nothing else is known about the movement. */
  /* Add more vehicles modes here */

  eSAML_MOTION_MODE_MAX                  = 2147483647 /* Force 32bit */
} saml_MotionModeEnumT;

typedef struct
{
  uint64             t_Timestamp;
  /**<   Time stamp of the motion data in this report.
         The time stamp is in the time reference scale that is 
         used by the motion data time source. \n
         - Units: Milliseconds */

  uint32             q_Flags;
  /**< SAMLite flags. Combination of:  \n
         SAML_FLAGS_MASK_TIME_JUMP  \n
         SAML_FLAGS_MASK_TIME_UNC_HIGH */

  saml_MotionStateEnumT e_MotionState;
  /**< Current motion state of the user. \n
 
       Absolute rest and relative rest are both indicated by setting motion_state 
       to Stationary. The relative rest state can be distinguished from absolute 
       rest by reducing probabilityOfState.
  */

  saml_MotionModeEnumT  e_MotionMode;
  /**< Modes of user motion. \n
       
       The motionMode value is independent of the motionState value.
  */
  
  float                f_ProbabilityOfState;
  /**< Probability that the device is actually undergoing the motion state 
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
  
  uint16             w_Age;
  /**< Age of the motion data in milliseconds at the time of injection. 
  */
  
  uint16             w_Timeout;
  /**< If the age of the motion data input exceeds the timeout value, the data
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

} saml_MotionDataStructT;

/** Enum for pedestrian alignment algorithm type */
typedef enum
{
  eSAML_PED_ALIGNMENT_INPUT_TYPE_ROTATION_VECTOR        = 0,
  /**<  9-axis fusion algorithm  */
  eSAML_PED_ALIGNMENT_INPUT_TYPE_GAME_ROTATION_VECTOR   = 1,
  /**<  6-axis accel + gyro fusion algorithm */
  eSAML_PED_ALIGNMENT_INPUT_TYPE_ORIENTATION            = 2,
  /**<  6-axis accel + mag fusion algorithm */

  eSAML_PED_ALIGNMENT_INPUT_TYPE_MAX = 2147483647 /* Force 32bit */
} saml_PedAlignmentInputTypeEnumT;

/** Pedestrian alignment input mask can be set to represent one or a group
    of algorithm types.
    The index of each bit corresponds a value in
    saml_slimPedAlignmentInputTypeEnumT. Mask can be easily
    manipulated using following macros:
    - Set value:              SAML_MASK_SET
      Example:                SAML_MASK_SET(mask, eSAML_PED_ALIGNMENT_INPUT_TYPE_ROTATION_VECTOR);
    - Clear value:            SAML_MASK_CLEAR
      Example:                SAML_MASK_CLEAR(mask, eSAML_PED_ALIGNMENT_INPUT_TYPE_ROTATION_VECTOR);
    - Check if value is set:  SAML_MASK_IS_SET
      Example:                if (SAML_MASK_IS_SET(mask, eSAML_PED_ALIGNMENT_INPUT_TYPE_ROTATION_VECTOR))
                              {
                                ...
                              }
*/
typedef uint32 saml_PedAlignmentInputTypeMaskT;

/** Pedestrian alignment event */
typedef enum
{
  eSAML_PED_ALIGNMENT_EVENT_TURN_DETECTED           = 0,
  /**< Pedestrian turn detected  */
  eSAML_PED_ALIGNMENT_EVENT_ALIGNMENT_CHANGED       = 1,
  /**< Device alignment change detected */
  eSAML_PED_ALIGNMENT_EVENT_RTA_COG_INCONSISTENT    = 2,
  /**< RTA and CoG inconsistent */
  eSAML_PED_ALIGNMENT_EVENT_PERIODIC_FAKE_STEP      = 3,
  /**< Pedestrian alignment trigged by fake step */  
  eSAML_PED_ALIGNMENT_EVENT_UNSTABLE_DEVICE_MOTION  = 4,
  /**< Unstable device motion detected */
  /* Add new events here. */
  eSAML_PED_ALIGNMENT_EVENT_COUNT,
  /**< Event count used to validate data */
  eSAML_PED_ALIGNMENT_EVENT_MAX         = 2147483647 /* Force 32bit */
} saml_PedAlignmentEventEnumT;

/** Pedestrian alignment event mask can be set to represent one or a group
    of events.
    The index of each bit corresponds a value in
    saml_PedAlignmentEventEnumT. Mask can be easily
    manipulated using following macros:
    - Set value:              SAML_MASK_SET
      Example:                SAML_MASK_SET(mask, eSAML_PED_ALIGNMENT_EVENT_TURN_DETECTED);
    - Clear value:            SAML_MASK_CLEAR
      Example:                SAML_MASK_CLEAR(mask, eSAML_PED_ALIGNMENT_EVENT_TURN_DETECTED);
    - Check if value is set:  SAML_MASK_IS_SET
      Example:                if (SAML_MASK_IS_SET(mask, eSAML_PED_ALIGNMENT_EVENT_TURN_DETECTED))
                              {
                                ...
                              }
*/
typedef uint32 saml_PedAlignmentEventTypeMaskT;

typedef struct
{
  uint64                       t_Timestamp;
  /**<   Time stamp of the pedestrian alignment data in this report.
         The time stamp is in the time reference scale that is 
         used by the pedestrian alignment data time source. \n
         - Units: Milliseconds */

  uint32                       q_Flags;
  /**< SAMLite flags. Combination of:  \n
         SAML_FLAGS_MASK_TIME_JUMP  \n
         SAML_FLAGS_MASK_TIME_UNC_HIGH */

  saml_PedAlignmentInputTypeEnumT e_Algorithm;
  /**< Pedestrian alignment algorithm type of this sample. One of: \n
		 eSAML_PED_ALIGNMENT_INPUT_TYPE_ROTATION_VECTOR \n
         eSAML_PED_ALIGNMENT_INPUT_TYPE_GAME_ROTATION_VECTOR \n
         eSAML_PED_ALIGNMENT_INPUT_TYPE_ORIENTATION */

  float                        f_CogAngle;
  /**<   Course over ground (CoG) angle, in radians.
         Relative to magnetic north, if magnetometer is available
         Relative to arbitrary azimuth, if magnetometer is not available */

  float                        f_CogAccuracy;
  /**<   Predicted standard deviation of the computed CoG, in radians.
         Ranges from 0 to pi. Lower values correspond to more reliable
         estimates.
         Value < pi/12: highly reliable CoG estimate.
         Value > pi/3: CoG cannot be trusted. */

  float                        f_RelativeTurnAngle;
  /**<   Estimated relative turn angle since last device alignment change,
         in radians. */

  uint8                        b_RelativeTurnAngleAccuracy_valid;
  /**<   Must be set to true if RelativeTurnAngleAccuracy is being passed */

  float                        f_RelativeTurnAngleAccuracy;
  /**<   Estimated uncertainty of the relative turn angle since last device 
         alignment change, in radians. */

  uint32                       q_AlignmentChangeAge;
  /**<   Age of the last detected alignment change event.\n
         - Units: Milliseconds */

  saml_PedAlignmentEventTypeMaskT   m_EventsMask;
  /**<   Pedestrian events detected since the last step. */

} saml_PedestrianAlignmentDataStructT;

typedef struct
{
  saml_PedAlignmentInputTypeMaskT m_InputTypeMask;
  /**< Mask containing all enabled algorithm types of a successfull enable
       request. */

} saml_PedestrianAlignmentEnableResponseStructT;

/** Enum for SAMLite service ids */
typedef enum
{
  eSAML_SERVICE_NONE                 = -1,
  /**< Service not defined */

  /* Derived data */
  /**< Significant Motion Data service */
  eSAML_SERVICE_MOTION_DATA          = 0,
  /**< Motion Data service */
  eSAML_SERVICE_PEDOMETER            = 1,
  /**< Pedometer service */
  eSAML_SERVICE_PED_ALIGNMENT        = 2,
  /**< Pedestrian alignment data service */

  eSAML_SERVICE_LAST = eSAML_SERVICE_PED_ALIGNMENT,
  /**< Last SAMLite service */

  eSAML_SERVICE_MAX = 2147483647 /* Force 32bit */
} saml_ServiceEnumT;

typedef struct
{
  uint32    q_AvailableServiceMask;
  /**< Mask containing all available services. */
  /** q_AvailableServiceMask represents the services that are currently 
      available in SAMLite.
    The index of each bit corresponds a value in
    saml_ServiceEnumT. Mask can be easily
    manipulated using following macros:
    - Set value:              SAML_MASK_SET
      Example:                SAML_MASK_SET(mask, eSAML_SERVICE_MOTION_DATA);
    - Clear value:            SAML_MASK_CLEAR
      Example:                SAML_MASK_CLEAR(mask, eSAML_SERVICE_PEDOMETER);
    - Check if value is set:  SAML_MASK_IS_SET
      Example:                if (SAML_MASK_IS_SET(mask, eSAML_SERVICE_PED_ALIGNMENT))
                              {
                                ...
                              } */
  uint32    q_PedAlignmentSubMask;
  /** q_PedAlignmentSubMask represents the ped-alignement service
      is currently supported by SAMLite
    The index of each bit corresponds a value in saml_PedAlignmentInputTypeEnumT */
} saml_ServiceStatusEventStructT;

/** Enum for SAMLite message types */
typedef enum
{
  eSAML_MESSAGE_TYPE_INDICATION = 0,
   /**< Message is unsolicited indication */
  eSAML_MESSAGE_TYPE_RESPONSE = 1,
     /**< Message is a response to a request */
  eSAML_MESSAGE_TYPE_MAX = 2147483647 /* Force 32bit */
} saml_MessageTypeEnumT;

/** Enum for SAMLite message ids. Payload is valid only with specific message id
    only if the error code is eSAML_SUCCESS in the message header. */
typedef enum
{
  /* SAMLite service related messages */
  eSAML_MESSAGE_ID_NONE = 0,
  /**< no payload */

  /* Service status indication */
  eSAML_MESSAGE_ID_SERVICE_STATUS_IND,
  /**< saml_ServiceStatusEventStructT */

  /* Motion data messages */
  eSAML_MESSAGE_ID_MOTION_DATA_IND,
  /**< This IND can only work with two error codes:
  eSAML_SUCCESS: payload = saml_MotionDataStructT
  eSAML_ERROR_SERVICE_OUTAGE: payload = NULL
   */
  eSAML_MESSAGE_ID_MOTION_DATA_ENABLE_RESP,
  /**< no payload */

  /* Pedometer messages */
  eSAML_MESSAGE_ID_PEDOMETER_IND,
  /**< This IND can only work with two error codes:
  eSAML_SUCCESS: payload = saml_PedometerDataStructT
  eSAML_ERROR_SERVICE_OUTAGE: payload = NULL
   */
  eSAML_MESSAGE_ID_PEDOMETER_ENABLE_RESP,
  /**< no payload */

  /* Pedestrian alignment data messages */
  eSAML_MESSAGE_ID_PED_ALIGNMENT_IND,
  /**< This IND can only work with two error codes:
  eSAML_SUCCESS: payload = saml_PedestrianAlignmentDataStructT. This is
    valid ped-alignment data
  eSAML_ERROR_SERVICE_OUTAGE: payload = saml_PedAlignmentInputTypeEnumT. This is
    to indicate which ped-alignment service is experiencing data outage */
  eSAML_MESSAGE_ID_PED_ALIGNMENT_ENABLE_RESP,
  /**< saml_PedestrianAlignmentEnableResponseStructT */

  eSAML_MESSAGE_ID_MAX = 2147483647 /* Force 32bit */
} saml_MessageIdEnumT;

/** Enum for SAMLite error codes */
typedef enum
{
  eSAML_SUCCESS                        = 0,
  /**< No error - success */
  eSAML_ERROR_CLIENT_HANDLE_INVALID    = -1,
  /**< Invalid client handle */
  eSAML_ERROR_UNSUPPORTED              = -2,
  /**< Operation is not supported */
  eSAML_ERROR_IPC_COMMUNICATION        = -3,
  /**< Failure with IPC communication */
  eSAML_ERROR_INVALID_CALLBACK         = -4,
  /**< Client callback is invalid. */
  eSAML_ERROR_INTERNAL                 = -5,
  /**< Internal SAMLite error */
  eSAML_ERROR_MAX_TXN_COUNT_REACHED    = -6,
  /**< Maximum transaction count exceeded.*/
  eSAML_ERROR_BAD_ARGUMENT             = -7,
  /**< Client provided bad argument. */
  eSAML_ERROR_QMI_INTERFACE            = -8,
  /**< Error happened in QMI-communication. */
  eSAML_ERROR_ALLOCATION               = -9,
  /**< Error happened in memory allocation. */
  eSAML_ERROR_SERVICE_RESERVED         = -10,
  /**< Service connection is already reserved for another client.*/
  eSAML_ERROR_SERVICE_NOT_ENABLED      = -11,
  /**< Service is not enabled.*/
  eSAML_ERROR_SNS_DATA_NOT_AVAILABLE   = -12,
  /**< Sensor data is not available to support the service */
  eSAML_ERROR_DUPLICATED_REQUESTS      = -13,
  /**< duplicated requests */
  eSAML_ERROR_SERVICE_OUTAGE           = -14,
  /**< service outage, e.g. due to temporary sns data outage */
  eSAML_ERROR_MIN                      = -2147483647 /* Force 32bit */
} saml_ErrorEnumT;

/*! Client handle type to be used with SAMLite communication */
typedef const void* saml_ClientHandleT;

/*! Struct for SAMLite message header */
typedef struct
{
  saml_ServiceEnumT e_Service;
  /**< SAMLite service type */

  saml_MessageTypeEnumT e_MsgType;
  /**< SAMLite message type */

  saml_ErrorEnumT e_MsgError;
  /**< SAMLite message error.
       Do not handle message payload if value is not eSAML_SUCCESS.
   */

  saml_MessageIdEnumT e_MsgId;
  /**< SAMLite message id */

  uint16 w_Size;
  /**< SAMLite message size */

  int32 l_TxnId;
  /**< Transaction id provided by client */

} saml_MessageHeaderStructT;

/*! Client callback function type */
typedef void ( *saml_NotifyCallbackFunctionT )
(
  const saml_MessageHeaderStructT* pz_MessageHeader,
  void *p_Message
);

/*! Struct for motion data reporting configuration parameters */
typedef struct 
{

  uint8 b_Enable;
  /**< TRUE if reporting should be enabled.
       FALSE if reporting should be disabled.
   */

} saml_EnableMotionDataRequestStructT;

/*! Struct for pedometer reporting configuration parameters*/
typedef struct
{

  uint8  b_Enable;
  /**< TRUE if reporting should be enabled.
       FALSE if reporting should be disabled.
   */

  uint8  b_ResetStepCount_valid;
  /**< Must be set to TRUE if resetStepCount is being passed */

  uint8  b_ResetStepCount;
  /**< Indicates whether the service should reset the step count.
       This value is used only if reporting is already enabled.
   */

  uint8  b_StepCountThreshold_valid;
  /**< Must be set to TRUE if stepCountThreshold is being passed */

  uint32 q_StepCountThreshold;
  /**< Specifies the number of steps to be sampled in a pedometer report.
       If the threshold is set to 0, every step event is reported.
   */

  uint32 q_ReportPeriod;
  /**< Specifies interval for periodic reporting period in seconds, Q16.
       P = 0 for asynchronous reporting.
       P > 0 for periodic reporting. Maximum reporting period is 3600 seconds.
   */

  uint8  b_SampleRate_valid;
  /**< Must be set to TRUE if sampleRate is being passed */

  uint32 q_SampleRate;
  /**< Sampling rate in Hz, Q16. Default value of 20 Hz. Requires S >= 20 Hz. */

  uint8  b_DutyCycleOnPercent_valid;
  /**< Must be set to TRUE if dutyCycleOnPercent is being passed */

  uint8  u_DutyCycleOnPercent;
  /**< Duty cycle ON percentage (0 to 100) for pedometer service when it is
       duty-cycled. Sensor stream to pedometer service is active only during
       the duty cycle ON period.
   */

} saml_EnablePedometerRequestStructT;

/*! Struct for pedestrian alignment data reporting configuration parameters */
typedef struct
{

  uint8 b_Enable;
  /**< TRUE if reporting should be enabled.
       FALSE if reporting should be disabled.
   */

  saml_PedAlignmentInputTypeMaskT m_InputTypeMask;
  /**< SLIM shall try to use all algorithm types defined in the given mask for
       the pedestrian alignment service.
   */

  uint8 b_SampleRate_valid;
  /**<  Must be set to true if sampleRate is being passed */
  uint32 q_SampleRate;
  /**<  Sampling rate in Hz, Q16. Can range from 20-50Hz. Defaults to 20 Hz.
   */

} saml_EnablePedestrianAlignmentRequestStructT;
#endif /* SAML_CLIENT_TYPES_H */

