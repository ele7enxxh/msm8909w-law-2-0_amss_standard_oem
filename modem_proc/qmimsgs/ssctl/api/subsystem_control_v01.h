#ifndef SSCTL_SERVICE_01_H
#define SSCTL_SERVICE_01_H
/**
  @file subsystem_control_v01.h

  @brief This is the public header file which defines the ssctl service Data structures.

  This header file defines the types and structures that were defined in
  ssctl. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/ssctl/api/subsystem_control_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.7 
   It was generated on: Sat Feb  8 2014 (Spin 0)
   From IDL File: subsystem_control_v01.idl */

/** @defgroup ssctl_qmi_consts Constant values defined in the IDL */
/** @defgroup ssctl_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ssctl_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ssctl_qmi_messages Structures sent as QMI messages */
/** @defgroup ssctl_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ssctl_qmi_accessor Accessor for QMI service object */
/** @defgroup ssctl_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ssctl_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SSCTL_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SSCTL_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SSCTL_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SSCTL_V01_MAX_MESSAGE_ID 0x0023
/**
    @}
  */


/** @addtogroup ssctl_qmi_consts 
    @{ 
  */
#define QMI_SSCTL_ERROR_MSG_LENGTH_V01 90
#define QMI_SSCTL_SUB_SYS_NAME_LENGTH_V01 15
/**
    @}
  */

/** @addtogroup ssctl_qmi_enums
    @{
  */
typedef enum {
  SSCTL_QMI_SERVICE_INSTANCE_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SSCTL_QMI_SERVICE_INSTANCE_MPSS_0_V01 = 0, 
  SSCTL_QMI_SERVICE_INSTANCE_WCNSS_0_V01 = 1, 
  SSCTL_QMI_SERVICE_INSTANCE_ADSP_0_V01 = 2, 
  SSCTL_QMI_SERVICE_INSTANCE_VPU_0_V01 = 3, 
  SSCTL_QMI_SERVICE_INSTANCE_MDM_0_V01 = 4, 
  SSCTL_QMI_SERVICE_INSTANCE_MDM_1_V01 = 5, 
  SSCTL_QMI_SERVICE_INSTANCE_QSC_0_V01 = 6, 
  SSCTL_QMI_SERVICE_INSTANCE_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ssctl_qmi_service_instance_enum_type_v01;
/**
    @}
  */

/** @addtogroup ssctl_qmi_enums
    @{
  */
typedef enum {
  SSCTL_SSR_EVENT_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SSCTL_SSR_EVENT_BEFORE_POWERUP_V01 = 0, 
  SSCTL_SSR_EVENT_AFTER_POWERUP_V01 = 1, 
  SSCTL_SSR_EVENT_BEFORE_SHUTDOWN_V01 = 2, 
  SSCTL_SSR_EVENT_AFTER_SHUTDOWN_V01 = 3, 
  SSCTL_SSR_EVENT_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ssctl_ssr_event_enum_type_v01;
/**
    @}
  */

/** @addtogroup ssctl_qmi_enums
    @{
  */
typedef enum {
  SSCTL_SSR_EVENT_DRIVEN_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SSCTL_SSR_EVENT_FORCED_V01 = 0, 
  SSCTL_SSR_EVENT_GRACEFUL_V01 = 1, 
  SSCTL_SSR_EVENT_DRIVEN_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ssctl_ssr_event_driven_enum_type_v01;
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ssctl_restart_req_msg_v01;

/** @addtogroup ssctl_qmi_messages
    @{
  */
/** Response Message; Restarts the subsystem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qmi_ssctl_restart_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ssctl_restart_ind_msg_v01;

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ssctl_shutdown_req_msg_v01;

/** @addtogroup ssctl_qmi_messages
    @{
  */
/** Response Message; Shuts down the subsystem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qmi_ssctl_shutdown_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ssctl_shutdown_ind_msg_v01;

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ssctl_get_failure_reason_req_msg_v01;

/** @addtogroup ssctl_qmi_messages
    @{
  */
/** Response Message; Gets the reason for the failure from the subsystem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Error Message */
  uint8_t error_message_valid;  /**< Must be set to true if error_message is being passed */
  uint32_t error_message_len;  /**< Must be set to # of elements in error_message */
  char error_message[QMI_SSCTL_ERROR_MSG_LENGTH_V01];
  /**<   Reason for the failure.  */
}qmi_ssctl_get_failure_reason_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssctl_qmi_messages
    @{
  */
/** Request Message; Receives other subsystem's status(shutdown or powerup) message. */
typedef struct {

  /* Mandatory */
  /*  Subsystem Name */
  uint32_t subsys_name_len;  /**< Must be set to # of elements in subsys_name */
  char subsys_name[QMI_SSCTL_SUB_SYS_NAME_LENGTH_V01];

  /* Mandatory */
  /*  Event Notification of the Subsystem */
  ssctl_ssr_event_enum_type_v01 event;

  /* Optional */
  /*  Event Notification Driven Type of the Subsystem */
  uint8_t evt_driven_valid;  /**< Must be set to true if evt_driven is being passed */
  ssctl_ssr_event_driven_enum_type_v01 evt_driven;
}qmi_ssctl_subsys_event_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ssctl_qmi_messages
    @{
  */
/** Response Message; Receives other subsystem's status(shutdown or powerup) message. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}qmi_ssctl_subsys_event_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_ssctl_subsys_event_comp_ind_msg_v01;

/*Service Message Definition*/
/** @addtogroup ssctl_qmi_msg_ids
    @{
  */
#define QMI_SSCTL_RESTART_REQ_V01 0x0020
#define QMI_SSCTL_RESTART_RESP_V01 0x0020
#define QMI_SSCTL_RESTART_READY_IND_V01 0x0020
#define QMI_SSCTL_SHUTDOWN_REQ_V01 0x0021
#define QMI_SSCTL_SHUTDOWN_RESP_V01 0x0021
#define QMI_SSCTL_SHUTDOWN_READY_IND_V01 0x0021
#define QMI_SSCTL_GET_FAILURE_REASON_REQ_V01 0x0022
#define QMI_SSCTL_GET_FAILURE_REASON_RESP_V01 0x0022
#define QMI_SSCTL_SUBSYS_EVENT_REQ_V01 0x0023
#define QMI_SSCTL_SUBSYS_EVENT_RESP_V01 0x0023
#define QMI_SSCTL_SUBSYS_EVENT_COMP_IND_V01 0x0023
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro ssctl_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type ssctl_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define ssctl_get_service_object_v01( ) \
          ssctl_get_service_object_internal_v01( \
            SSCTL_V01_IDL_MAJOR_VERS, SSCTL_V01_IDL_MINOR_VERS, \
            SSCTL_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

