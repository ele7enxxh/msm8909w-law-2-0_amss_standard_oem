#ifndef SNS_SYNC_PROXY_SVC_SERVICE_01_H
#define SNS_SYNC_PROXY_SVC_SERVICE_01_H
/**
  @file sns_sync_proxy_v01.h

  @brief This is the public header file which defines the SNS_SYNC_PROXY_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_SYNC_PROXY_SVC. It contains the constant values defined, enums, structures,
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
  
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14 
   It was generated on: Tue Oct 28 2014 (Spin 0)
   From IDL File: sns_sync_proxy_v01.idl */

/** @defgroup SNS_SYNC_PROXY_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_SYNC_PROXY_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_SYNC_PROXY_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_SYNC_PROXY_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_SYNC_PROXY_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_SYNC_PROXY_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_SYNC_PROXY_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_SYNC_PROXY_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_SYNC_PROXY_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_SYNC_PROXY_SVC_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_SYNC_PROXY_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_SYNC_PROXY_SVC_V01_MAX_MESSAGE_ID 0x0021
/**
    @}
  */


/** @addtogroup SNS_SYNC_PROXY_SVC_qmi_consts 
    @{ 
  */

/** ============================================================================
============================================================================
 Maximum number of DRX schedules which can be tracked by the sync proxy
    at one time  */
#define SNS_SYNC_PROXY_MAX_SCHEDS_V01 10
/**
    @}
  */

/** @addtogroup SNS_SYNC_PROXY_SVC_qmi_enums
    @{
  */
typedef enum {
  SNS_SYNC_PROXY_MOTION_STATE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_SYNC_PROXY_MOTION_UNKNOWN_V01 = 0, 
  SNS_SYNC_PROXY_MOTION_REST_V01 = 1, 
  SNS_SYNC_PROXY_MOTION_MOVE_V01 = 2, 
  SNS_SYNC_PROXY_MOTION_STATE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sns_sync_proxy_motion_state_e_v01;
/**
    @}
  */

/** @addtogroup SNS_SYNC_PROXY_SVC_qmi_aggregates
    @{
  */
/**  This structure defines a periodic synchronization schedule 
 */
typedef struct {

  uint64_t absolute_qtimer_value;
  /**<   An absolute qtimer value when the client processor will wake up */

  uint32_t qtimer_interval;
  /**<   Interval period (in qtimer units) at which the client processor wakes up */
}sns_sync_proxy_sync_schedule_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup SNS_SYNC_PROXY_SVC_qmi_messages
    @{
  */
/** Request Message; Enables the motion detection algorithm. Service will send indications
    when the state changes */
typedef struct {

  /* Mandatory */
  uint32_t sync_schedule_len;  /**< Must be set to # of elements in sync_schedule */
  sns_sync_proxy_sync_schedule_s_v01 sync_schedule[SNS_SYNC_PROXY_MAX_SCHEDS_V01];
  /**<   Client wakeup schedule. The service may support multiple schedules. If
     the service supports fewer schedules than provided, it will honor the first
     schedule(s) in the array */

  /* Mandatory */
  uint32_t stationary_to_motion_latency;
  /**<   State change detection latency requirements, in QTimer ticks */

  /* Mandatory */
  uint32_t motion_to_stationary_latency;
  /**<   State change detection latency requirements, in QTimer ticks */
}sns_sync_proxy_motion_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SYNC_PROXY_SVC_qmi_messages
    @{
  */
/** Response Message; Enables the motion detection algorithm. Service will send indications
    when the state changes */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 Resp;

  /* Optional */
  uint8_t num_sync_schedules_processed_valid;  /**< Must be set to true if num_sync_schedules_processed is being passed */
  uint32_t num_sync_schedules_processed;
  /**<   Number of sync schedules which will be honored by the service */
}sns_sync_proxy_motion_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SYNC_PROXY_SVC_qmi_messages
    @{
  */
/** Indication Message;  */
typedef struct {

  /* Mandatory */
  uint32_t timestamp;
  /**<   Timestamp of the event, in SSC ticks */

  /* Mandatory */
  sns_sync_proxy_motion_state_e_v01 state;
}sns_sync_proxy_motion_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_SNS_COMMON_CANCEL_V01 
//#define REMOVE_SNS_COMMON_VERSION_V01 
//#define REMOVE_SNS_SYNC_PROXY_MOTION_ENABLE_V01 
//#define REMOVE_SNS_SYNC_PROXY_MOTION_IND_V01 

/*Service Message Definition*/
/** @addtogroup SNS_SYNC_PROXY_SVC_qmi_msg_ids
    @{
  */
#define SNS_SYNC_PROXY_CANCEL_REQ_V01 0x0000
#define SNS_SYNC_PROXY_CANCEL_RESP_V01 0x0000
#define SNS_SYNC_PROXY_VERSION_REQ_V01 0x0001
#define SNS_SYNC_PROXY_VERSION_RESP_V01 0x0001
#define SNS_SYNC_PROXY_MOTION_ENABLE_REQ_V01 0x0020
#define SNS_SYNC_PROXY_MOTION_ENABLE_RESP_V01 0x0020
#define SNS_SYNC_PROXY_MOTION_IND_V01 0x0021
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_SYNC_PROXY_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_SYNC_PROXY_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define SNS_SYNC_PROXY_SVC_get_service_object_v01( ) \
          SNS_SYNC_PROXY_SVC_get_service_object_internal_v01( \
            SNS_SYNC_PROXY_SVC_V01_IDL_MAJOR_VERS, SNS_SYNC_PROXY_SVC_V01_IDL_MINOR_VERS, \
            SNS_SYNC_PROXY_SVC_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

