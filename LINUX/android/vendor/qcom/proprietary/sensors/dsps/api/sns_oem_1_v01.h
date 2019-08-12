#ifndef SNS_OEM_1_SVC_SERVICE_01_H
#define SNS_OEM_1_SVC_SERVICE_01_H
/**
  @file sns_oem_1_v01.h

  @brief This is the public header file which defines the SNS_OEM_1_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_OEM_1_SVC. It contains the constant values defined, enums, structures,
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

  Copyright (c) 2013-2016 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Tue Feb  2 2016 (Spin 0)
   From IDL File: sns_oem_1_v01.idl */

/** @defgroup SNS_OEM_1_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_OEM_1_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_OEM_1_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_OEM_1_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_OEM_1_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_OEM_1_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_OEM_1_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_OEM_1_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_OEM_1_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_OEM_1_SVC_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_OEM_1_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_OEM_1_SVC_V01_MAX_MESSAGE_ID 0x0022
/**
    @}
  */


/** @addtogroup SNS_OEM_1_SVC_qmi_consts
    @{
  */
#define SNS_OEM_1_SUID_V01 0x78518008917553

/**  Maximum number of reports in a batch indication. This is set
     based on the maximum payload size supported by the transport framework. */
#define SNS_SAM_OEM_1_MAX_ITEMS_IN_BATCH_V01 100

/**  Maximum number of sensor values in one report  */
#define SNS_OEM_1_MAX_ITEMS_PER_OUTPUT_DATA_V01 3
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_aggregates
    @{
  */
typedef struct {

  /*  This structure shall have the output data */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint32_t data[SNS_OEM_1_MAX_ITEMS_PER_OUTPUT_DATA_V01];
}sns_oem_1_output_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Request Message; This command enables the OEM_1 algorithm. */
typedef struct {

  /* Mandatory */
  uint32_t report_period;
  /**<   unit of seconds, Q16; determines algorithm output report rate
   specify 0 to report at sampling rate

   Note: This is a sample request parameter, the OEM may replace
   it with an appropriate parameter
  */

  /* Optional */
  uint8_t sample_rate_valid;  /**< Must be set to true if sample_rate is being passed */
  uint32_t sample_rate;
  /**<   sample rate in Hz, Q16;
   if sample rate is less than report rate, it is set to report rate

   Note: This is a sample request parameter, the OEM may replace
   it with an appropriate parameter
  */
}sns_oem_1_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Response Message; This command enables the OEM_1 algorithm. */
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
}sns_oem_1_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Request Message; This command disables a OEM_1 algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */
}sns_oem_1_disable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Response Message; This command disables a OEM_1 algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */
}sns_oem_1_disable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Indication Message; Report containing OEM_1 algorithm output */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */

  /* Mandatory */
  uint32_t timestamp;
  /**<   Timestamp of input used to generate the algorithm output  */

  /* Mandatory */
  sns_oem_1_output_s_v01 output;
  /**<   output of OEM_1 algorithm instance.  */
}sns_oem_1_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Request Message; This command fetches latest report output of OEM_1 algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */
}sns_oem_1_get_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Response Message; This command fetches latest report output of OEM_1 algorithm. */
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
  /**<   Timestamp of input used to generate the algorithm output  */

  /* Optional */
  uint8_t output_valid;  /**< Must be set to true if output is being passed */
  sns_oem_1_output_s_v01 output;
  /**<    output of OEM_1 algorithm instance.  */
}sns_oem_1_get_report_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Indication Message; Asynchronous error indication for a OEM_1 algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t error;
  /**<   Sensor1 error code.  */

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance.  */
}sns_oem_1_error_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Request Message; This command handles batch mode for the oem_1 algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Mandatory */
  int32_t batch_period;
  /**<   Specifies the interval over which reports are to be batched, in seconds;
       Q16 format. If AP is in suspend and notify_suspend is FALSE, undelivered
       reports will be batched in circular FIFO and delivered upon wakeup.
       - P = 0 -- Disable batching
       - P > 0 -- Enable batching
       - P < 0 -- See sns_sam_batch_config_e
    */
}sns_sam_oem_1_batch_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Response Message; This command handles batch mode for the oem_1 algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
  /**<   Common response message. */

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Optional */
  uint8_t max_batch_size_valid;  /**< Must be set to true if max_batch_size is being passed */
  uint32_t max_batch_size;
  /**<   Maximum supported batch size. */

  /* Optional */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint32_t timestamp;
  /**<   Timestamp when the batch request was processed, in SSC ticks. */
}sns_sam_oem_1_batch_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_OEM_1_SVC_qmi_messages
    @{
  */
/** Indication Message; Report containing a batch of algorithm outputs. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Identifies the algorithm instance.  */

  /* Mandatory */
  uint32_t first_report_timestamp;
  /**<   Timestamp of the input used to generate the first algorithm output in the
       batch, in SSC ticks. */

  /* Mandatory */
  uint32_t timestamp_offsets_len;  /**< Must be set to # of elements in timestamp_offsets */
  uint16_t timestamp_offsets[SNS_SAM_OEM_1_MAX_ITEMS_IN_BATCH_V01];
  /**<   Offsets from the timestamp of the previous report in the batch. */

  /* Mandatory */
  uint32_t reports_len;  /**< Must be set to # of elements in reports */
  sns_oem_1_output_s_v01 reports[SNS_SAM_OEM_1_MAX_ITEMS_IN_BATCH_V01];
  /**<   OEM1 algorithm output reports. */

  /* Optional */
  uint8_t ind_type_valid;  /**< Must be set to true if ind_type is being passed */
  uint8_t ind_type;
  /**<   Optional batch indication type:
       - SNS_BATCH_ONLY_IND -- Standalone batch indication, not part of a back-to-back indication stream
       - SNS_BATCH_FIRST_IND -- First indication in a stream of back-to-back indications
       - SNS_BATCH_INTERMEDIATE_IND -- Intermediate indication in a stream of back-to-back indications
       - SNS_BATCH_LAST_IND -- Last indication in a stream of back-to-back indications
    */
}sns_sam_oem_1_batch_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_SNS_COMMON_CANCEL_V01
//#define REMOVE_SNS_COMMON_VERSION_V01
//#define REMOVE_SNS_OEM_1_DISABLE_V01
//#define REMOVE_SNS_OEM_1_ENABLE_V01
//#define REMOVE_SNS_OEM_1_ERROR_V01
//#define REMOVE_SNS_OEM_1_GET_REPORT_V01
//#define REMOVE_SNS_OEM_1_REPORT_V01
//#define REMOVE_SNS_SAM_OEM_1_BATCH_V01
//#define REMOVE_SNS_SAM_OEM_1_BATCH_REPORT_V01

/*Service Message Definition*/
/** @addtogroup SNS_OEM_1_SVC_qmi_msg_ids
    @{
  */
#define SNS_OEM_1_CANCEL_REQ_V01 0x0000
#define SNS_OEM_1_CANCEL_RESP_V01 0x0000
#define SNS_OEM_1_VERSION_REQ_V01 0x0001
#define SNS_OEM_1_VERSION_RESP_V01 0x0001
#define SNS_OEM_1_ENABLE_REQ_V01 0x0002
#define SNS_OEM_1_ENABLE_RESP_V01 0x0002
#define SNS_OEM_1_DISABLE_REQ_V01 0x0003
#define SNS_OEM_1_DISABLE_RESP_V01 0x0003
#define SNS_OEM_1_GET_REPORT_REQ_V01 0x0004
#define SNS_OEM_1_GET_REPORT_RESP_V01 0x0004
#define SNS_OEM_1_REPORT_IND_V01 0x0005
#define SNS_OEM_1_ERROR_IND_V01 0x0006
#define SNS_SAM_OEM_1_BATCH_REQ_V01 0x0021
#define SNS_SAM_OEM_1_BATCH_RESP_V01 0x0021
#define SNS_SAM_OEM_1_BATCH_IND_V01 0x0022
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_OEM_1_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_OEM_1_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define SNS_OEM_1_SVC_get_service_object_v01( ) \
          SNS_OEM_1_SVC_get_service_object_internal_v01( \
            SNS_OEM_1_SVC_V01_IDL_MAJOR_VERS, SNS_OEM_1_SVC_V01_IDL_MINOR_VERS, \
            SNS_OEM_1_SVC_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

