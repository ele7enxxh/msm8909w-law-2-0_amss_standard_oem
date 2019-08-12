#ifndef SNS_SAM_DEV_ORI_SVC_SERVICE_01_H
#define SNS_SAM_DEV_ORI_SVC_SERVICE_01_H
/**
  @file sns_sam_dev_ori_v01.h

  @brief This is the public header file which defines the SNS_SAM_DEV_ORI_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_SAM_DEV_ORI_SVC. It contains the constant values defined, enums, structures,
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
   It was generated on: Fri Jun 24 2016 (Spin 0)
   From IDL File: sns_sam_dev_ori_v01.idl */

/** @defgroup SNS_SAM_DEV_ORI_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_SAM_DEV_ORI_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_SAM_DEV_ORI_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_SAM_DEV_ORI_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_SAM_DEV_ORI_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_SAM_DEV_ORI_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_SAM_DEV_ORI_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_sam_common_v01.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_SAM_DEV_ORI_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_SAM_DEV_ORI_SVC_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_SAM_DEV_ORI_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_SAM_DEV_ORI_SVC_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_consts
    @{
  */
#define SNS_SAM_DEV_ORI_SUID_V01 0x9beea197625543ea
/**
    @}
  */

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_enums
    @{
  */
typedef enum {
  SNS_SAM_DEV_ORI_STATE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_SAM_DEV_ORI_UNKNOWN_V01 = -1,
  SNS_SAM_DEV_ORI_X_UP_V01 = 0,
  SNS_SAM_DEV_ORI_Y_UP_V01 = 1,
  SNS_SAM_DEV_ORI_X_DOWN_V01 = 2,
  SNS_SAM_DEV_ORI_Y_DOWN_V01 = 3,
  SNS_SAM_DEV_ORI_STATE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sns_sam_dev_ori_state_e_v01;
/**
    @}
  */

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_messages
    @{
  */
/** Request Message; This command enables the device orientation service. */
typedef struct {

  /* Optional */
  /*  Notify Suspend */
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
}sns_sam_dev_ori_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_messages
    @{
  */
/** Response Message; This command enables the device orientation service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  sns_common_resp_s_v01 resp;

  /* Optional */
  /*  Instance ID */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<
    Algorithm instance ID, which is maintained/assigned by the SAM.
       The client must use this instance ID for future messages associated with
       the current algorithm instance.
    */
}sns_sam_dev_ori_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_messages
    @{
  */
/** Request Message; This command disables the device orientation service. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance to be disabled.  */
}sns_sam_dev_ori_disable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_messages
    @{
  */
/** Response Message; This command disables the device orientation service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  sns_common_resp_s_v01 resp;

  /* Optional */
  /*  Instance ID */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */
}sns_sam_dev_ori_disable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_messages
    @{
  */
/** Request Message; This command fetches latest report output of device orientation service. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */
}sns_sam_dev_ori_get_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_messages
    @{
  */
/** Response Message; This command fetches latest report output of device orientation service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  sns_common_resp_s_v01 resp;

  /* Optional */
  /*  Instance ID */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Optional */
  /*  Event Timestamp */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint32_t timestamp;
  /**<   Timestamp of input with which latest device orientation report was computed
  in SSC ticks  */

  /* Optional */
  /*  Orientation state report */
  uint8_t dev_ori_state_valid;  /**< Must be set to true if dev_ori_state is being passed */
  sns_sam_dev_ori_state_e_v01 dev_ori_state;
  /**<   device orientation algorithm report  */
}sns_sam_dev_ori_get_report_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_messages
    @{
  */
/** Indication Message; Report from the device orientation service. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */

  /* Mandatory */
  /*  Event Timestamp */
  uint32_t timestamp;
  /**<   Timestamp of input with which latest device orientation report was computed
  in SSC ticks  */

  /* Mandatory */
  /*  Orientation value */
  sns_sam_dev_ori_state_e_v01 dev_ori_state;
  /**<   device orientation algorithm report  */
}sns_sam_dev_ori_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_messages
    @{
  */
/** Indication Message; Asynchronous error report from the device orientation service. */
typedef struct {

  /* Mandatory */
  /*  Error */
  uint8_t error;
  /**<   Sensors error code */

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Mandatory */
  /*  Timestamp */
  uint32_t timestamp;
  /**<   Timestamp of when the error was detected, in SSC ticks */
}sns_sam_dev_ori_error_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_SNS_SAM_DEV_ORI_CANCEL_V01
//#define REMOVE_SNS_SAM_DEV_ORI_DISABLE_V01
//#define REMOVE_SNS_SAM_DEV_ORI_ENABLE_V01
//#define REMOVE_SNS_SAM_DEV_ORI_ERROR_V01
//#define REMOVE_SNS_SAM_DEV_ORI_GET_REPORT_V01
//#define REMOVE_SNS_SAM_DEV_ORI_REPORT_V01
//#define REMOVE_SNS_SAM_DEV_ORI_VERSION_V01
//#define REMOVE_SNS_SAM_GET_ALGO_ATTRIBUTES_V01

/*Service Message Definition*/
/** @addtogroup SNS_SAM_DEV_ORI_SVC_qmi_msg_ids
    @{
  */
#define SNS_SAM_DEV_ORI_CANCEL_REQ_V01 0x0000
#define SNS_SAM_DEV_ORI_CANCEL_RESP_V01 0x0000
#define SNS_SAM_DEV_ORI_VERSION_REQ_V01 0x0001
#define SNS_SAM_DEV_ORI_VERSION_RESP_V01 0x0001
#define SNS_SAM_DEV_ORI_ENABLE_REQ_V01 0x0002
#define SNS_SAM_DEV_ORI_ENABLE_RESP_V01 0x0002
#define SNS_SAM_DEV_ORI_DISABLE_REQ_V01 0x0003
#define SNS_SAM_DEV_ORI_DISABLE_RESP_V01 0x0003
#define SNS_SAM_DEV_ORI_GET_REPORT_REQ_V01 0x0004
#define SNS_SAM_DEV_ORI_GET_REPORT_RESP_V01 0x0004
#define SNS_SAM_DEV_ORI_REPORT_IND_V01 0x0005
#define SNS_SAM_DEV_ORI_ERROR_IND_V01 0x0006
#define SNS_SAM_DEV_ORI_GET_ATTRIBUTES_REQ_V01 0x0024
#define SNS_SAM_DEV_ORI_GET_ATTRIBUTES_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_SAM_DEV_ORI_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_SAM_DEV_ORI_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define SNS_SAM_DEV_ORI_SVC_get_service_object_v01( ) \
          SNS_SAM_DEV_ORI_SVC_get_service_object_internal_v01( \
            SNS_SAM_DEV_ORI_SVC_V01_IDL_MAJOR_VERS, SNS_SAM_DEV_ORI_SVC_V01_IDL_MINOR_VERS, \
            SNS_SAM_DEV_ORI_SVC_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

