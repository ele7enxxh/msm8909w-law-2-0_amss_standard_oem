#ifndef SNS_SAM_FAST_AMD_SVC_SERVICE_01_H
#define SNS_SAM_FAST_AMD_SVC_SERVICE_01_H
/**
  @file sns_sam_fast_amd_v01.h

  @brief This is the public header file which defines the SNS_SAM_FAST_AMD_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_SAM_FAST_AMD_SVC. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved. 
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2 
   It was generated on: Thu Dec 11 2014 (Spin 0)
   From IDL File: sns_sam_fast_amd_v01.idl */

/** @defgroup SNS_SAM_FAST_AMD_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_SAM_FAST_AMD_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_SAM_FAST_AMD_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_SAM_FAST_AMD_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_SAM_FAST_AMD_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_SAM_FAST_AMD_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_SAM_FAST_AMD_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_sam_common_v01.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_SAM_FAST_AMD_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_SAM_FAST_AMD_SVC_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_SAM_FAST_AMD_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_SAM_FAST_AMD_SVC_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_consts 
    @{ 
  */
#define SNS_SAM_FAST_AMD_SUID_V01 0xa28cb1431bcb2a77
/**
    @}
  */

/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_enums
    @{
  */
typedef enum {
  SNS_SAM_FAST_AMD_MOTION_STATE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_SAM_FAST_AMD_MOTION_UNKNOWN_V01 = 0, 
  SNS_SAM_FAST_AMD_MOTION_REST_V01 = 1, 
  SNS_SAM_FAST_AMD_MOTION_MOVE_V01 = 2, 
  SNS_SAM_FAST_AMD_MOTION_STATE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sns_sam_fast_amd_motion_state_e_v01;
/**
    @}
  */

/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_messages
    @{
  */
/** Request Message; This command enables Fast-AMD algorithm. */
typedef struct {

  /* Mandatory */
  sns_sam_report_e_v01 report_mode;
  /**<  
    Client requested report mode.
    Fast-AMD will support two reporting mode:
      SNS_SAM_ASYNC_REPORT = 0x02, which can report on every motion state change.
      SNS_SAM_SYNC_REPORT  = 0x04, which can report on every algo batch processing.
  */

  /* Mandatory */
  uint32_t detection_period_in_motion_state;
  /**<  
    Client defined detection period in motion state, in microsecond.
    The algorithm may run more often than the specified window size.
  */

  /* Mandatory */
  uint32_t detection_period_in_stationary_state;
  /**<  
    Client defined detection period in stationary state , in microsecond.
    The algorithm may run more often than the specified window size.
  */

  /* Optional */
  uint8_t state_detection_threshold_valid;  /**< Must be set to true if state_detection_threshold is being passed */
  uint32_t state_detection_threshold;
  /**<   Specify the threshold of motion/stationary output required to
       generate an Fast-AMD motion event, in unit of percentage. Default
       value defined in registry. */
}sns_sam_fast_amd_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_messages
    @{
  */
/** Response Message; This command enables Fast-AMD algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<  
    Instance ID is assigned by SAM.
    The client shall use this instance ID for future messages associated with
    this algorithm instance.
  */
}sns_sam_fast_amd_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_messages
    @{
  */
/** Request Message; This command disables an Fast-AMD algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */
}sns_sam_fast_amd_disable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_messages
    @{
  */
/** Response Message; This command disables an Fast-AMD algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */
}sns_sam_fast_amd_disable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_messages
    @{
  */
/** Indication Message; Report containing FAST_AMD algorithm output */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */

  /* Mandatory */
  uint32_t timestamp;
  /**<  
    Timestamp of the last accel samples in the batched accel data used to
    generate the algorithm output. */

  /* Mandatory */
  sns_sam_fast_amd_motion_state_e_v01 state;
  /**<  
    Motion state output of Fast-AMD algorithm instance.
    This output is the final determination of Fast-AMD motion state, based on
    processing batched accel data. */
}sns_sam_fast_amd_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_messages
    @{
  */
/** Indication Message; Asynchronous error indication for a Fast-AMD algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t error;
  /**<   Sensor1 error code.  */

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */
}sns_sam_fast_amd_error_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_SNS_COMMON_CANCEL_V01 
//#define REMOVE_SNS_COMMON_VERSION_V01 
//#define REMOVE_SNS_SAM_FAST_AMD_DISABLE_V01 
//#define REMOVE_SNS_SAM_FAST_AMD_ENABLE_V01 
//#define REMOVE_SNS_SAM_FAST_AMD_ERROR_V01 
//#define REMOVE_SNS_SAM_FAST_AMD_REPORT_V01 
//#define REMOVE_SNS_SAM_GET_ALGO_ATTRIBUTES_V01 

/*Service Message Definition*/
/** @addtogroup SNS_SAM_FAST_AMD_SVC_qmi_msg_ids
    @{
  */
#define SNS_SAM_FAST_AMD_CANCEL_REQ_V01 0x0000
#define SNS_SAM_FAST_AMD_CANCEL_RESP_V01 0x0000
#define SNS_SAM_FAST_AMD_VERSION_REQ_V01 0x0001
#define SNS_SAM_FAST_AMD_VERSION_RESP_V01 0x0001
#define SNS_SAM_FAST_AMD_ENABLE_REQ_V01 0x0002
#define SNS_SAM_FAST_AMD_ENABLE_RESP_V01 0x0002
#define SNS_SAM_FAST_AMD_DISABLE_REQ_V01 0x0003
#define SNS_SAM_FAST_AMD_DISABLE_RESP_V01 0x0003
#define SNS_SAM_FAST_AMD_REPORT_IND_V01 0x0005
#define SNS_SAM_FAST_AMD_ERROR_IND_V01 0x0006
#define SNS_SAM_FAST_AMD_GET_ATTRIBUTES_REQ_V01 0x0024
#define SNS_SAM_FAST_AMD_GET_ATTRIBUTES_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_SAM_FAST_AMD_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_SAM_FAST_AMD_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define SNS_SAM_FAST_AMD_SVC_get_service_object_v01( ) \
          SNS_SAM_FAST_AMD_SVC_get_service_object_internal_v01( \
            SNS_SAM_FAST_AMD_SVC_V01_IDL_MAJOR_VERS, SNS_SAM_FAST_AMD_SVC_V01_IDL_MINOR_VERS, \
            SNS_SAM_FAST_AMD_SVC_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

