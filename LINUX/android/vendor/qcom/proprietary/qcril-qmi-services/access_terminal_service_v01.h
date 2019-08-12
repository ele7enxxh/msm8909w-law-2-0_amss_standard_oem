#ifndef AT_SERVICE_01_H
#define AT_SERVICE_01_H
/**
  @file access_terminal_service_v01.h

  @brief This is the public header file which defines the at service Data structures.

  This header file defines the types and structures that were defined in
  at. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.5
   It was generated on: Thu Jun 18 2015 (Spin 0)
   From IDL File: access_terminal_service_v01.idl */

/** @defgroup at_qmi_consts Constant values defined in the IDL */
/** @defgroup at_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup at_qmi_enums Enumerated types used in QMI messages */
/** @defgroup at_qmi_messages Structures sent as QMI messages */
/** @defgroup at_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup at_qmi_accessor Accessor for QMI service object */
/** @defgroup at_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup at_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define AT_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define AT_V01_IDL_MINOR_VERS 0x03
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define AT_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define AT_V01_MAX_MESSAGE_ID 0x00FF
/**
    @}
  */


/** @addtogroup at_qmi_consts
    @{
  */
#define QMI_AT_DUMMY_BUFFER_LEN_MAX_V01 4096
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Request Message; Resets the AT state variables of the requesting control point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}at_reset_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Response Message; Resets the AT state variables of the requesting control point. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}at_reset_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Request Message; This command is used by the control point to register any AT
    commands that are to be forwarded to the control point from the modem. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}at_req_at_cmd_fwd_req_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Response Message; This command is used by the control point to register any AT
    commands that are to be forwarded to the control point from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}at_req_at_cmd_fwd_resp_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Indication Message; This indication is used to forward an AT command to the control
    point. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}at_fwd_at_cmd_ind_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Request Message; This command is used by the control point to send the response to an
    AT command previously forwarded to the control point from the modem. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}at_fwd_resp_at_cmd_req_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Response Message; This command is used by the control point to send the response to an
    AT command previously forwarded to the control point from the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}at_fwd_resp_at_cmd_resp_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Indication Message; This notification is used to notify the control point that a
    particular AT command that was forwarded earlier has been aborted. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}at_abort_at_cmd_ind_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Request Message; This command is used by the control point to send the Unsolicited
    Result Code (URC) to the modem. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}at_send_at_urc_req_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Response Message; This command is used by the control point to send the Unsolicited
    Result Code (URC) to the modem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}at_send_at_urc_resp_v01;  /* Message */
/**
    @}
  */

/** @addtogroup at_qmi_messages
    @{
  */
/** Indication Message; This is an internal dummy indication message. Not going to be
    used by external clients. */
typedef struct {

  /* Optional */
  uint8_t qmi_at_dummy_buffer_valid;  /**< Must be set to true if qmi_at_dummy_buffer is being passed */
  uint32_t qmi_at_dummy_buffer_len;  /**< Must be set to # of elements in qmi_at_dummy_buffer */
  char qmi_at_dummy_buffer[QMI_AT_DUMMY_BUFFER_LEN_MAX_V01];
  /**<   Dummy buffer. Used only to define a large buffer for IDL.
    */
}at_dummy_at_cmd_ind_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_AT_ABORT_AT_CMD_IND_V01
//#define REMOVE_QMI_AT_DUMMY_AT_CMD_IND_V01
//#define REMOVE_QMI_AT_FWD_AT_CMD_IND_V01
//#define REMOVE_QMI_AT_FWD_RESP_AT_CMD_V01
//#define REMOVE_QMI_AT_REG_AT_CMD_FWD_V01
//#define REMOVE_QMI_AT_RESET_V01
//#define REMOVE_QMI_AT_SEND_AT_URC_V01

/*Service Message Definition*/
/** @addtogroup at_qmi_msg_ids
    @{
  */
#define QMI_AT_RESET_REQ_V01 0x0000
#define QMI_AT_RESET_RESP_V01 0x0000
#define QMI_AT_REG_AT_CMD_FWD_REQ_V01 0x0020
#define QMI_AT_REG_AT_CMD_FWD_RESP_V01 0x0020
#define QMI_AT_FWD_AT_CMD_IND_V01 0x0021
#define QMI_AT_FWD_RESP_AT_CMD_REQ_V01 0x0022
#define QMI_AT_FWD_RESP_AT_CMD_RESP_V01 0x0022
#define QMI_AT_ABORT_AT_CMD_IND_V01 0x0023
#define QMI_AT_SEND_AT_URC_REQ_V01 0x0024
#define QMI_AT_SEND_AT_URC_RESP_V01 0x0024
#define QMI_AT_DUMMY_AT_CMD_IND_V01 0x00FF
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro at_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type at_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define at_get_service_object_v01( ) \
          at_get_service_object_internal_v01( \
            AT_V01_IDL_MAJOR_VERS, AT_V01_IDL_MINOR_VERS, \
            AT_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

