#ifndef SNS_SAM_DPC_SVC_SERVICE_02_H
#define SNS_SAM_DPC_SVC_SERVICE_02_H
/**
  @file sns_sam_dpc_v02.h

  @brief This is the public header file which defines the SNS_SAM_DPC_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_SAM_DPC_SVC. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Wed Jan 27 2016 (Spin 0)
   From IDL File: sns_sam_dpc_v02.idl */

/** @defgroup SNS_SAM_DPC_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_SAM_DPC_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_SAM_DPC_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_SAM_DPC_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_SAM_DPC_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_SAM_DPC_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_SAM_DPC_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_sam_common_v01.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_SAM_DPC_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_SAM_DPC_SVC_V02_IDL_MAJOR_VERS 0x02
/** Revision Number of the IDL used to generate this file */
#define SNS_SAM_DPC_SVC_V02_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_SAM_DPC_SVC_V02_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_SAM_DPC_SVC_V02_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup SNS_SAM_DPC_SVC_qmi_consts
    @{
  */
#define SNS_SAM_DPC_SUID_V02 0x1f446c97c3686c70
/**
    @}
  */

/** @addtogroup SNS_SAM_DPC_SVC_qmi_enums
    @{
  */
typedef enum {
  SNS_SAM_DPC_STATE_E_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_SAM_DPC_DEV_UNKNOWN_V02 = 0,
  SNS_SAM_DPC_DEV_FLAT_STATIC_V02 = 1,
  SNS_SAM_DPC_DEV_HIDDEN_V02 = 2,
  SNS_SAM_DPC_DEV_IN_HAND_V02 = 3,
  SNS_SAM_DPC_DEV_FACING_V02 = 4,
  SNS_SAM_DPC_STATE_E_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sns_sam_dpc_state_e_v02;
/**
    @}
  */

/** @addtogroup SNS_SAM_DPC_SVC_qmi_messages
    @{
  */
/** Request Message; This command enables the dpc algorithm. */
typedef struct {

  /* Optional */
  uint8_t notify_suspend_valid;  /**< Must be set to true if notify_suspend is being passed */
  sns_suspend_notification_s_v01 notify_suspend;
  /**<   Identifies whether indications for this request are to be sent
       when the processor is in the Suspend state.

       If this field is not specified, the default value is set to:
       notify_suspend->proc_type                  = SNS_PROC_APPS
       notify_suspend->send_indications_during_suspend  = FALSE

       This field does not have any bearing on error indication
       messages, which are sent even during Suspend.
    */
}sns_sam_dpc_enable_req_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DPC_SVC_qmi_messages
    @{
  */
/** Response Message; This command enables the dpc algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Algorithm instance ID, which is maintained/assigned by the SAM.
       The client must use this instance ID for future messages associated with
       the current algorithm instance.
    */
}sns_sam_dpc_enable_resp_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DPC_SVC_qmi_messages
    @{
  */
/** Request Message; This command disables the dpc algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance to be disabled.  */
}sns_sam_dpc_disable_req_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DPC_SVC_qmi_messages
    @{
  */
/** Response Message; This command disables the dpc algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */
}sns_sam_dpc_disable_resp_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DPC_SVC_qmi_messages
    @{
  */
/** Request Message; This command fetches latest report output of dpc algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */
}sns_sam_dpc_get_report_req_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DPC_SVC_qmi_messages
    @{
  */
/** Response Message; This command fetches latest report output of dpc algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Optional */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint32_t timestamp;
  /**<   Timestamp of input with which latest dpc report was computed
  in SSC ticks  */

  /* Optional */
  uint8_t dpc_state_valid;  /**< Must be set to true if dpc_state is being passed */
  sns_sam_dpc_state_e_v02 dpc_state;
  /**<   dpc algorithm output report  */
}sns_sam_dpc_get_report_resp_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DPC_SVC_qmi_messages
    @{
  */
/** Indication Message; Output report from the dpc algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */

  /* Mandatory */
  uint32_t timestamp;
  /**<   Timestamp of input with which latest dpc report was computed
  in SSC ticks */

  /* Mandatory */
  sns_sam_dpc_state_e_v02 dpc_state;
  /**<   dpc algorithm output report  */
}sns_sam_dpc_report_ind_msg_v02;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DPC_SVC_qmi_messages
    @{
  */
/** Indication Message; This command requests algorithm service version. */
typedef struct {

  /* Mandatory */
  uint8_t error;
  /**<   Sensors error code. */

  /* Mandatory */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Mandatory */
  uint32_t timestamp;
  /**<   Timestamp of when the error was detected, in SSC ticks. */
}sns_sam_dpc_error_ind_msg_v02;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_SNS_SAM_DPC_CANCEL_V02
//#define REMOVE_SNS_SAM_DPC_DISABLE_V02
//#define REMOVE_SNS_SAM_DPC_ENABLE_V02
//#define REMOVE_SNS_SAM_DPC_ERROR_V02
//#define REMOVE_SNS_SAM_DPC_GET_REPORT_V02
//#define REMOVE_SNS_SAM_DPC_REPORT_V02
//#define REMOVE_SNS_SAM_DPC_VERSION_V02
//#define REMOVE_SNS_SAM_GET_ALGO_ATTRIBUTES_V02

/*Service Message Definition*/
/** @addtogroup SNS_SAM_DPC_SVC_qmi_msg_ids
    @{
  */
#define SNS_SAM_DPC_CANCEL_REQ_V02 0x0000
#define SNS_SAM_DPC_CANCEL_RESP_V02 0x0000
#define SNS_SAM_DPC_VERSION_REQ_V02 0x0001
#define SNS_SAM_DPC_VERSION_RESP_V02 0x0001
#define SNS_SAM_DPC_ENABLE_REQ_V02 0x0002
#define SNS_SAM_DPC_ENABLE_RESP_V02 0x0002
#define SNS_SAM_DPC_DISABLE_REQ_V02 0x0003
#define SNS_SAM_DPC_DISABLE_RESP_V02 0x0003
#define SNS_SAM_DPC_GET_REPORT_REQ_V02 0x0004
#define SNS_SAM_DPC_GET_REPORT_RESP_V02 0x0004
#define SNS_SAM_DPC_REPORT_IND_V02 0x0005
#define SNS_SAM_DPC_ERROR_IND_V02 0x0006
#define SNS_SAM_DPC_GET_ATTRIBUTES_REQ_V02 0x0024
#define SNS_SAM_DPC_GET_ATTRIBUTES_RESP_V02 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_SAM_DPC_SVC_get_service_object_v02( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_SAM_DPC_SVC_get_service_object_internal_v02
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define SNS_SAM_DPC_SVC_get_service_object_v02( ) \
          SNS_SAM_DPC_SVC_get_service_object_internal_v02( \
            SNS_SAM_DPC_SVC_V02_IDL_MAJOR_VERS, SNS_SAM_DPC_SVC_V02_IDL_MINOR_VERS, \
            SNS_SAM_DPC_SVC_V02_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

