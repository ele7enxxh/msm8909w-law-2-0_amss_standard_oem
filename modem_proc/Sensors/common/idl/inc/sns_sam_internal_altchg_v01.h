#ifndef SNS_SAM_INTERNAL_ALTCHG_SVC_SERVICE_01_H
#define SNS_SAM_INTERNAL_ALTCHG_SVC_SERVICE_01_H
/**
  @file sns_sam_internal_altchg_v01.h

  @brief This is the public header file which defines the SNS_SAM_INTERNAL_ALTCHG_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_SAM_INTERNAL_ALTCHG_SVC. It contains the constant values defined, enums, structures,
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
  
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/idl/inc/sns_sam_internal_altchg_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Wed Apr 27 2016 (Spin 0)
   From IDL File: sns_sam_internal_altchg_v01.idl */

/** @defgroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_sam_common_v01.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_SAM_INTERNAL_ALTCHG_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_SAM_INTERNAL_ALTCHG_SVC_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_SAM_INTERNAL_ALTCHG_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_SAM_INTERNAL_ALTCHG_SVC_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_consts
    @{
  */
#define SNS_SAM_INTERNAL_ALTCHG_SUID_V01 0x9b06913d255b9e2e

/**  Max number of reports in a batch indication  */
#define SNS_SAM_INTERNAL_ALTCHG_MAX_ITEMS_IN_BATCH_V01 100
/**
    @}
  */

   /* Typedefs */

typedef uint32_t sns_sam_internal_altchg_timestamp_t_v01;

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_aggregates
    @{
  */
typedef struct {

  float speed;
  /**<  
    Vertical speed in m/s. Negative value is descending, 
    positive value is ascending.
    */
}sns_sam_internal_altchg_report_data_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Request Message; This command enables the Altitude Change algorithm. */
typedef struct {

  /* Optional */
  uint8_t notify_suspend_valid;  /**< Must be set to true if notify_suspend is being passed */
  sns_suspend_notification_s_v01 notify_suspend;
  /**<   Identifies if indications for this request should be sent
       when the processor is in suspend state.

       If this field is not specified, default value will be set to
       notify_suspend->proc_type                  = SNS_PROC_MODEM
       notify_suspend->send_indications_during_suspend  = TRUE

       This field does not have any bearing on error indication
       messages, which will be sent even during suspend.
    */
}sns_sam_internal_altchg_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Response Message; This command enables the Altitude Change algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<  
    Algorithm instance ID maintained/assigned by SAM.
    The client shall use this instance ID for future messages associated with
    current algorithm instance.
    */
}sns_sam_internal_altchg_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Request Message; This command disables the Altitude Change algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   To identify the algorithm instance to be disabled.  */
}sns_sam_internal_altchg_disable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Response Message; This command disables the Altitude Change algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */
}sns_sam_internal_altchg_disable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Request Message; This command fetches latest report output of Altitude Change algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */
}sns_sam_internal_altchg_get_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Response Message; This command fetches latest report output of Altitude Change algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */

  /* Optional */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint32_t timestamp;
  /**<   Timestamp of input with which report applies to; in SSC ticks  */

  /* Optional */
  uint8_t report_data_valid;  /**< Must be set to true if report_data is being passed */
  sns_sam_internal_altchg_report_data_s_v01 report_data;
  /**<   report data payload */
}sns_sam_internal_altchg_get_report_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Indication Message; Output report from the Altitude Change algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */

  /* Mandatory */
  uint32_t timestamp;
  /**<   Timestamp of the report; in SSC ticks */

  /* Mandatory */
  sns_sam_internal_altchg_report_data_s_v01 report_data;
  /**<   report data payload */
}sns_sam_internal_altchg_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Indication Message; Asynchronous error report from the altchg algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t error;
  /**<   sensors error code */

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */

  /* Mandatory */
  uint32_t timestamp;
  /**<   Timestamp of when the error was detected; in SSC ticks */
}sns_sam_internal_altchg_error_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Request Message; This command handles batch mode for the Internal Altitude Change service. */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */

  /* Mandatory */
  /*  Batch Period */
  int32_t batch_period;
  /**<   Specifies the interval over which reports are to be batched, in seconds;
       Q16 format. If AP is in suspend and notify_suspend is FALSE, undelivered
       reports will be batched in circular FIFO and delivered upon wakeup.
       P = 0 to disable batching.
       P > 0 to enable batching.
    */
}sns_sam_internal_altchg_batch_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Response Message; This command handles batch mode for the Internal Altitude Change service. */
typedef struct {

  /* Mandatory */
  /*  Response */
  sns_common_resp_s_v01 resp;

  /* Optional */
  /*  Instance ID */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Algorithm instance ID maintained/assigned by SAM */

  /* Optional */
  /*  Max Batch Size */
  uint8_t max_batch_size_valid;  /**< Must be set to true if max_batch_size is being passed */
  uint32_t max_batch_size;
  /**<   Max supported batch size */

  /* Optional */
  /*  Timestamp */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint32_t timestamp;
  /**<   Timestamp when the batch request was processed in SSC ticks */
}sns_sam_internal_altchg_batch_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_messages
    @{
  */
/** Indication Message; Report containing a batch of Internal Altitude Change outputs */
typedef struct {

  /* Mandatory */
  /*  Instance ID */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance */

  /* Mandatory */
  /*  Internal Altitude Change event timestamps */
  uint32_t event_timestamps_len;  /**< Must be set to # of elements in event_timestamps */
  sns_sam_internal_altchg_timestamp_t_v01 event_timestamps[SNS_SAM_INTERNAL_ALTCHG_MAX_ITEMS_IN_BATCH_V01];
  /**<   Timestamp of the Altitude Change events; in SSC ticks */

  /* Mandatory */
  /*  Internal Altitude Change event data details */
  uint32_t event_data_len;  /**< Must be set to # of elements in event_data */
  sns_sam_internal_altchg_report_data_s_v01 event_data[SNS_SAM_INTERNAL_ALTCHG_MAX_ITEMS_IN_BATCH_V01];

  /* Optional */
  /*  Indication Type */
  uint8_t ind_type_valid;  /**< Must be set to true if ind_type is being passed */
  uint8_t ind_type;
  /**<   Optional batch indication type
       SNS_BATCH_ONLY_IND - Standalone batch indication. Not part of a back to back indication stream
       SNS_BATCH_FIRST_IND - First indication in stream of back to back indications
       SNS_BATCH_INTERMEDIATE_IND - Intermediate indication in stream of back to back indications
       SNS_BATCH_LAST_IND - Last indication in stream of back to back indications
    */
}sns_sam_internal_altchg_batch_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_SNS_SAM_GET_ALGO_ATTRIBUTES_V01 
//#define REMOVE_SNS_SAM_INTERNAL_ALTCHG_BATCH_V01 
//#define REMOVE_SNS_SAM_INTERNAL_ALTCHG_BATCH_REPORT_V01 
//#define REMOVE_SNS_SAM_INTERNAL_ALTCHG_CANCEL_V01 
//#define REMOVE_SNS_SAM_INTERNAL_ALTCHG_DISABLE_V01 
//#define REMOVE_SNS_SAM_INTERNAL_ALTCHG_ENABLE_V01 
//#define REMOVE_SNS_SAM_INTERNAL_ALTCHG_ERROR_V01 
//#define REMOVE_SNS_SAM_INTERNAL_ALTCHG_GET_REPORT_V01 
//#define REMOVE_SNS_SAM_INTERNAL_ALTCHG_REPORT_V01 
//#define REMOVE_SNS_SAM_INTERNAL_ALTCHG_VERSION_V01 

/*Service Message Definition*/
/** @addtogroup SNS_SAM_INTERNAL_ALTCHG_SVC_qmi_msg_ids
    @{
  */
#define SNS_SAM_INTERNAL_ALTCHG_CANCEL_REQ_V01 0x0000
#define SNS_SAM_INTERNAL_ALTCHG_CANCEL_RESP_V01 0x0000
#define SNS_SAM_INTERNAL_ALTCHG_VERSION_REQ_V01 0x0001
#define SNS_SAM_INTERNAL_ALTCHG_VERSION_RESP_V01 0x0001
#define SNS_SAM_INTERNAL_ALTCHG_ENABLE_REQ_V01 0x0002
#define SNS_SAM_INTERNAL_ALTCHG_ENABLE_RESP_V01 0x0002
#define SNS_SAM_INTERNAL_ALTCHG_DISABLE_REQ_V01 0x0003
#define SNS_SAM_INTERNAL_ALTCHG_DISABLE_RESP_V01 0x0003
#define SNS_SAM_INTERNAL_ALTCHG_GET_REPORT_REQ_V01 0x0004
#define SNS_SAM_INTERNAL_ALTCHG_GET_REPORT_RESP_V01 0x0004
#define SNS_SAM_INTERNAL_ALTCHG_REPORT_IND_V01 0x0005
#define SNS_SAM_INTERNAL_ALTCHG_ERROR_IND_V01 0x0006
#define SNS_SAM_INTERNAL_ALTCHG_BATCH_REQ_V01 0x0021
#define SNS_SAM_INTERNAL_ALTCHG_BATCH_RESP_V01 0x0021
#define SNS_SAM_INTERNAL_ALTCHG_BATCH_IND_V01 0x0022
#define SNS_SAM_INTERNAL_ALTCHG_GET_ATTRIBUTES_REQ_V01 0x0024
#define SNS_SAM_INTERNAL_ALTCHG_GET_ATTRIBUTES_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_SAM_INTERNAL_ALTCHG_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_SAM_INTERNAL_ALTCHG_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define SNS_SAM_INTERNAL_ALTCHG_SVC_get_service_object_v01( ) \
          SNS_SAM_INTERNAL_ALTCHG_SVC_get_service_object_internal_v01( \
            SNS_SAM_INTERNAL_ALTCHG_SVC_V01_IDL_MAJOR_VERS, SNS_SAM_INTERNAL_ALTCHG_SVC_V01_IDL_MINOR_VERS, \
            SNS_SAM_INTERNAL_ALTCHG_SVC_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

