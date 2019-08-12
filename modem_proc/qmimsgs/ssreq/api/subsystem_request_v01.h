#ifndef SSREQ_SERVICE_01_H
#define SSREQ_SERVICE_01_H
/**
  @file subsystem_request_v01.h

  @brief This is the public header file which defines the ssreq service Data structures.

  This header file defines the types and structures that were defined in
  ssreq. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/ssreq/api/subsystem_request_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.3 
   It was generated on: Fri Jan 30 2015 (Spin 1)
   From IDL File: subsystem_request_v01.idl */

/** @defgroup ssreq_qmi_consts Constant values defined in the IDL */
/** @defgroup ssreq_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ssreq_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ssreq_qmi_messages Structures sent as QMI messages */
/** @defgroup ssreq_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ssreq_qmi_accessor Accessor for QMI service object */
/** @defgroup ssreq_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ssreq_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SSREQ_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SSREQ_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SSREQ_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SSREQ_V01_MAX_MESSAGE_ID 0x0022
/**
    @}
  */


/** @addtogroup ssreq_qmi_consts 
    @{ 
  */
#define QMI_SSREQ_SERVICE_INSTANCE_OFFSET_V01 16
/**
    @}
  */

/** @addtogroup ssreq_qmi_enums
    @{
  */
typedef enum {
  SSREQ_QMI_SERVICE_INSTANCE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SSREQ_QMI_SERVICE_INSTANCE_APSS_V01 = 0x10, /**<  APSS service instance  */
  SSREQ_QMI_SERVICE_INSTANCE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ssreq_qmi_service_instance_enum_type_v01;
/**
    @}
  */

/** @addtogroup ssreq_qmi_enums
    @{
  */
typedef enum {
  SSREQ_QMI_SS_IDENTIFIER_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SSREQ_QMI_CLIENT_INSTANCE_APSS_V01 = 0, /**<  Client is APSS  */
  SSREQ_QMI_CLIENT_INSTANCE_MPSS_V01 = 1, /**<  Client is MPSS  */
  SSREQ_QMI_CLIENT_INSTANCE_WCNSS_V01 = 2, /**<  Client is WCNSS  */
  SSREQ_QMI_CLIENT_INSTANCE_ADSP_V01 = 3, /**<  Client is ADSP  */
  SSREQ_QMI_CLIENT_INSTANCE_MDM_V01 = 4, /**<  Client is MDM  */
  SSREQ_QMI_CLIENT_INSTANCE_QSC_V01 = 5, /**<  Client is QSC  */
  SSREQ_QMI_SS_IDENTIFIER_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ssreq_qmi_ss_identifier_enum_type_v01;
/**
    @}
  */

/** @addtogroup ssreq_qmi_enums
    @{
  */
typedef enum {
  SSREQ_QMI_REQUEST_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SSREQ_QMI_REQUEST_SERVICED_V01 = 0, /**<  Request was serviced  */
  SSREQ_QMI_REQUEST_NOT_SERVICED_V01 = 1, /**<  Request was not serviced  */
  SSREQ_QMI_REQUEST_STATUS_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ssreq_qmi_request_status_enum_type_v01;
/**
    @}
  */

/** @addtogroup ssreq_qmi_enums
    @{
  */
typedef enum {
  SSREQ_QMI_REQUEST_RES_CODE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SSREQ_QMI_RES_MODEM_CONF_CHANGE_V01 = 0x10, /**<  Modem configuration change  */
  SSREQ_QMI_RES_SEGMENT_LOADING_V01 = 0x11, /**<  Segmented loading  */
  SSREQ_QMI_REQUEST_RES_CODE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ssreq_qmi_request_res_code_enum_type_v01;
/**
    @}
  */

/** @addtogroup ssreq_qmi_messages
    @{
  */
/** Request Message; Shuts down the entire system. */
typedef struct {

  /* Mandatory */
  /*  Client ID */
  ssreq_qmi_ss_identifier_enum_type_v01 ss_client_id;
  /**<   Client ID. Values:\n
      - SSREQ_QMI_CLIENT_INSTANCE_APSS (0) --  Client is APSS 
      - SSREQ_QMI_CLIENT_INSTANCE_MPSS (1) --  Client is MPSS 
      - SSREQ_QMI_CLIENT_INSTANCE_WCNSS (2) --  Client is WCNSS 
      - SSREQ_QMI_CLIENT_INSTANCE_ADSP (3) --  Client is ADSP 
      - SSREQ_QMI_CLIENT_INSTANCE_MDM (4) --  Client is MDM 
      - SSREQ_QMI_CLIENT_INSTANCE_QSC (5) --  Client is QSC 
 */

  /* Optional */
  /*  Reason Code */
  uint8_t res_code_valid;  /**< Must be set to true if res_code is being passed */
  ssreq_qmi_request_res_code_enum_type_v01 res_code;
  /**<   Reason for initiating the request. Values: \n
      - SSREQ_QMI_RES_MODEM_CONF_CHANGE (0x10) --  Modem configuration change 
      - SSREQ_QMI_RES_SEGMENT_LOADING (0x11) --  Segmented loading 
 */
}qmi_ssreq_system_shutdown_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssreq_qmi_messages
    @{
  */
/** Response Message; Shuts down the entire system. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qmi_ssreq_system_shutdown_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssreq_qmi_messages
    @{
  */
/** Indication Message; Indicates whether the system shutdown request was serviced. */
typedef struct {

  /* Mandatory */
  /*  Request Status */
  ssreq_qmi_request_status_enum_type_v01 status;
  /**<   Status of the shutdown request. Values: \n
      - SSREQ_QMI_REQUEST_SERVICED (0) --  Request was serviced 
      - SSREQ_QMI_REQUEST_NOT_SERVICED (1) --  Request was not serviced 
 */
}qmi_ssreq_system_shutdown_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssreq_qmi_messages
    @{
  */
/** Request Message; Restarts the system. */
typedef struct {

  /* Mandatory */
  /*  Client ID */
  ssreq_qmi_ss_identifier_enum_type_v01 ss_client_id;
  /**<   Client ID. Values:\n
      - SSREQ_QMI_CLIENT_INSTANCE_APSS (0) --  Client is APSS 
      - SSREQ_QMI_CLIENT_INSTANCE_MPSS (1) --  Client is MPSS 
      - SSREQ_QMI_CLIENT_INSTANCE_WCNSS (2) --  Client is WCNSS 
      - SSREQ_QMI_CLIENT_INSTANCE_ADSP (3) --  Client is ADSP 
      - SSREQ_QMI_CLIENT_INSTANCE_MDM (4) --  Client is MDM 
      - SSREQ_QMI_CLIENT_INSTANCE_QSC (5) --  Client is QSC 
 */

  /* Optional */
  /*  Reason Code */
  uint8_t res_code_valid;  /**< Must be set to true if res_code is being passed */
  ssreq_qmi_request_res_code_enum_type_v01 res_code;
  /**<   Reason for initiating the request. Values: \n
      - SSREQ_QMI_RES_MODEM_CONF_CHANGE (0x10) --  Modem configuration change 
      - SSREQ_QMI_RES_SEGMENT_LOADING (0x11) --  Segmented loading 
 */
}qmi_ssreq_system_restart_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssreq_qmi_messages
    @{
  */
/** Response Message; Restarts the system. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qmi_ssreq_system_restart_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssreq_qmi_messages
    @{
  */
/** Indication Message; Indicates whether the system restart request was serviced. */
typedef struct {

  /* Mandatory */
  /*  Request Status */
  ssreq_qmi_request_status_enum_type_v01 status;
  /**<   Status of the restart request. Values:\n
      - SSREQ_QMI_REQUEST_SERVICED (0) --  Request was serviced 
      - SSREQ_QMI_REQUEST_NOT_SERVICED (1) --  Request was not serviced 
 */
}qmi_ssreq_system_restart_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssreq_qmi_messages
    @{
  */
/** Request Message; Restarts a subsystem (peripheral). */
typedef struct {

  /* Mandatory */
  /*  Client ID */
  ssreq_qmi_ss_identifier_enum_type_v01 ss_client_id;
  /**<   Client ID. Values: \n
      - SSREQ_QMI_CLIENT_INSTANCE_APSS (0) --  Client is APSS 
      - SSREQ_QMI_CLIENT_INSTANCE_MPSS (1) --  Client is MPSS 
      - SSREQ_QMI_CLIENT_INSTANCE_WCNSS (2) --  Client is WCNSS 
      - SSREQ_QMI_CLIENT_INSTANCE_ADSP (3) --  Client is ADSP 
      - SSREQ_QMI_CLIENT_INSTANCE_MDM (4) --  Client is MDM 
      - SSREQ_QMI_CLIENT_INSTANCE_QSC (5) --  Client is QSC 
 */

  /* Optional */
  /*  Reason Code */
  uint8_t res_code_valid;  /**< Must be set to true if res_code is being passed */
  ssreq_qmi_request_res_code_enum_type_v01 res_code;
  /**<   Reason for initiating the request. Values: \n
      - SSREQ_QMI_RES_MODEM_CONF_CHANGE (0x10) --  Modem configuration change 
      - SSREQ_QMI_RES_SEGMENT_LOADING (0x11) --  Segmented loading 
 */
}qmi_ssreq_peripheral_restart_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssreq_qmi_messages
    @{
  */
/** Response Message; Restarts a subsystem (peripheral). */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qmi_ssreq_peripheral_restart_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssreq_qmi_messages
    @{
  */
/** Indication Message; Indicates whether the subsystem restart request was serviced. */
typedef struct {

  /* Mandatory */
  /*  Request Status */
  ssreq_qmi_request_status_enum_type_v01 status;
  /**<   Status of the restart request. Values: \n
      - SSREQ_QMI_REQUEST_SERVICED (0) --  Request was serviced 
      - SSREQ_QMI_REQUEST_NOT_SERVICED (1) --  Request was not serviced 
 */
}qmi_ssreq_peripheral_restart_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_SSREQ_PERIPHERAL_RESTART_V01 
//#define REMOVE_QMI_SSREQ_PERIPHERAL_RESTART_IND_V01 
//#define REMOVE_QMI_SSREQ_SYSTEM_RESTART_V01 
//#define REMOVE_QMI_SSREQ_SYSTEM_RESTART_IND_V01 
//#define REMOVE_QMI_SSREQ_SYSTEM_SHUTDOWN_V01 
//#define REMOVE_QMI_SSREQ_SYSTEM_SHUTDOWN_IND_V01 

/*Service Message Definition*/
/** @addtogroup ssreq_qmi_msg_ids
    @{
  */
#define QMI_SSREQ_SYSTEM_SHUTDOWN_REQ_V01 0x0020
#define QMI_SSREQ_SYSTEM_SHUTDOWN_RESP_V01 0x0020
#define QMI_SSREQ_SYSTEM_SHUTDOWN_IND_V01 0x0020
#define QMI_SSREQ_SYSTEM_RESTART_REQ_V01 0x0021
#define QMI_SSREQ_SYSTEM_RESTART_RESP_V01 0x0021
#define QMI_SSREQ_SYSTEM_RESTART_IND_V01 0x0021
#define QMI_SSREQ_PERIPHERAL_RESTART_REQ_V01 0x0022
#define QMI_SSREQ_PERIPHERAL_RESTART_RESP_V01 0x0022
#define QMI_SSREQ_PERIPHERAL_RESTART_IND_V01 0x0022
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro ssreq_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type ssreq_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define ssreq_get_service_object_v01( ) \
          ssreq_get_service_object_internal_v01( \
            SSREQ_V01_IDL_MAJOR_VERS, SSREQ_V01_IDL_MINOR_VERS, \
            SSREQ_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

