#ifndef FDS_SERVICE_01_H
#define FDS_SERVICE_01_H
/**
  @file flash_driver_service_v01.h

  @brief This is the public header file which defines the fds service Data structures.

  This header file defines the types and structures that were defined in
  fds. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2013 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/fds/api/flash_driver_service_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.6 
   It was generated on: Fri Nov 15 2013 (Spin 1)
   From IDL File: flash_driver_service_v01.idl */

/** @defgroup fds_qmi_consts Constant values defined in the IDL */
/** @defgroup fds_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup fds_qmi_enums Enumerated types used in QMI messages */
/** @defgroup fds_qmi_messages Structures sent as QMI messages */
/** @defgroup fds_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup fds_qmi_accessor Accessor for QMI service object */
/** @defgroup fds_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup fds_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define FDS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define FDS_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define FDS_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define FDS_V01_MAX_MESSAGE_ID 0x0025
/**
    @}
  */


/** @addtogroup fds_qmi_consts 
    @{ 
  */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}fds_scrub_start_req_msg_v01;

/** @addtogroup fds_qmi_messages
    @{
  */
/** Response Message; Invokes a flash scrub operation. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Scrub Token */
  uint8_t scrub_token_valid;  /**< Must be set to true if scrub_token is being passed */
  uint32_t scrub_token;
  /**<   Token that can be used to query the status of the scrub request using the
       QMI_FDS_SCRUB_GET_STATUS command. The scrub token is only returned when
       the Result Code TLV indicates success. */
}fds_scrub_start_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup fds_qmi_enums
    @{
  */
typedef enum {
  FDS_SCRUB_STATUS_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  FDS_SCRUB_PENDING_V01 = 0, /**<  Scrub is still in progress  */
  FDS_SCRUB_COMPLETE_V01 = 1, /**<  Scrub is complete  */
  FDS_SCRUB_STATUS_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}fds_scrub_status_v01;
/**
    @}
  */

/** @addtogroup fds_qmi_messages
    @{
  */
/** Request Message; Retrieves the status of a previously issued scrub request. */
typedef struct {

  /* Mandatory */
  /*  Scrub Token */
  uint32_t scrub_token;
  /**<   Scrub token returned by the service in QMI_FDS_SCRUB_START_RESP to
       identify a scrub request. */
}fds_scrub_get_status_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup fds_qmi_messages
    @{
  */
/** Response Message; Retrieves the status of a previously issued scrub request. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Scrub Status */
  uint8_t scrub_status_valid;  /**< Must be set to true if scrub_status is being passed */
  fds_scrub_status_v01 scrub_status;
  /**<   Status of the scrub request identified by the scrub token.
 Values: \n
      - FDS_SCRUB_PENDING (0) --  Scrub is still in progress 
      - FDS_SCRUB_COMPLETE (1) --  Scrub is complete  */
}fds_scrub_get_status_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}fds_scrub_get_sys_info_req_msg_v01;

/** @addtogroup fds_qmi_messages
    @{
  */
/** Response Message; Retrieves the scrub system information. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Internal Scrub Timer */
  uint8_t fds_internal_scrub_timer_secs_valid;  /**< Must be set to true if fds_internal_scrub_timer_secs is being passed */
  uint64_t fds_internal_scrub_timer_secs;
  /**<   Default system timer, in seconds, that is set to trigger the next
       scrub cycle. */

  /* Optional */
  /*  Minimum Seconds Between Scrubs */
  uint8_t fds_minimum_secs_between_scrub_valid;  /**< Must be set to true if fds_minimum_secs_between_scrub is being passed */
  uint64_t fds_minimum_secs_between_scrub;
  /**<   Minimum number of seconds the system mandates between two consecutive
       scrubs. If a scrub request is sent before this time has elapsed
       since the previous scrub, the QMI invocation returns failure. */

  /* Optional */
  /*  Previous Scrub Time */
  uint8_t fds_previous_scrub_time_posix_valid;  /**< Must be set to true if fds_previous_scrub_time_posix is being passed */
  uint64_t fds_previous_scrub_time_posix;
  /**<   POSIX time for when the previous scrub was performed. */
}fds_scrub_get_sys_info_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}fds_handle_fota_update_req_msg_v01;

/** @addtogroup fds_qmi_messages
    @{
  */
/** Response Message; Requests appropriate action on a Firmware Over the AIR (FOTA)
                  update. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}fds_handle_fota_update_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup fds_qmi_messages
    @{
  */
/** Request Message; Registers and deregisters clients for events supported by
                  the service. */
typedef struct {

  /* Optional */
  /*  Applications Start Scrub Event */
  uint8_t need_apps_start_scrub_indication_valid;  /**< Must be set to true if need_apps_start_scrub_indication is being passed */
  uint8_t need_apps_start_scrub_indication;
  /**<   Controls the reporting of QMI_FDS_SCRUB_APPS_START_ SCRUB_IND. 
       Values:  \n
       - 0 -- Do not report \n
       - 1 -- Report
  */
}fds_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup fds_qmi_messages
    @{
  */
/** Response Message; Registers and deregisters clients for events supported by
                  the service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}fds_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup fds_qmi_messages
    @{
  */
/** Indication Message; Indicates when to start applications processor side 
                  scrubbing, including scrubbing all partitions the 
                  applications processor requires and maintains. */
typedef struct {

  /* Mandatory */
  /*  Scrub System's Current Running Scrub Token */
  uint32_t scrub_token;
  /**<   Token that uniquely identifies the current sequence of the scrub 
       operation. */
}fds_scrub_apps_start_scrub_ind_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup fds_qmi_msg_ids
    @{
  */
#define QMI_FDS_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_FDS_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_FDS_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_FDS_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_FDS_SCRUB_START_REQ_V01 0x0020
#define QMI_FDS_SCRUB_START_RESP_V01 0x0020
#define QMI_FDS_SCRUB_GET_STATUS_REQ_V01 0x0021
#define QMI_FDS_SCRUB_GET_STATUS_RESP_V01 0x0021
#define QMI_FDS_SCRUB_GET_SYS_INFO_REQ_V01 0x0022
#define QMI_FDS_SCRUB_GET_SYS_INFO_RESP_V01 0x0022
#define QMI_FDS_HANDLE_FOTA_UPDATE_REQ_V01 0x0023
#define QMI_FDS_HANDLE_FOTA_UPDATE_RESP_V01 0x0023
#define QMI_FDS_INDICATION_REGISTER_REQ_V01 0x0024
#define QMI_FDS_INDICATION_REGISTER_RESP_V01 0x0024
#define QMI_FDS_SCRUB_APPS_START_SCRUB_IND_V01 0x0025
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro fds_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type fds_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define fds_get_service_object_v01( ) \
          fds_get_service_object_internal_v01( \
            FDS_V01_IDL_MAJOR_VERS, FDS_V01_IDL_MINOR_VERS, \
            FDS_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

