#ifndef QMI_IP_SERVICE_01_H
#define QMI_IP_SERVICE_01_H
/**
  @file qmi_ip_service_v01.h

  @brief This is the public header file which defines the qmi_ip service Data structures.

  This header file defines the types and structures that were defined in
  qmi_ip. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.9
   It was generated on: Wed May 21 2014 (Spin 0)
   From IDL File: qmi_ip_service_v01.idl */

/** @defgroup qmi_ip_qmi_consts Constant values defined in the IDL */

/** @defgroup qmi_ip_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup qmi_ip_qmi_enums Enumerated types used in QMI messages */
/** @defgroup qmi_ip_qmi_messages Structures sent as QMI messages */
/** @defgroup qmi_ip_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup qmi_ip_qmi_accessor Accessor for QMI service object */
/** @defgroup qmi_ip_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qmi_ip_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QMI_IP_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QMI_IP_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QMI_IP_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define QMI_IP_V01_MAX_MESSAGE_ID 0x005E
/**
    @}
  */


/** @addtogroup qmi_ip_qmi_consts
    @{
  */
#define QMI_IP_IPV6_ADDR_LEN_V01 16
/**
    @}
  */

/** @addtogroup qmi_ip_qmi_enums
    @{
  */
typedef enum {
  QMI_IP_CLIENT_ADDR_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  IPV4_V01 = 0x04, /**<  IPv4 address type
   */
  IPV6_V01 = 0x06, /**<  IPv6 address type
   */
  QMI_IP_CLIENT_ADDR_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_ip_client_addr_type_v01;
/**
    @}
  */

/** @addtogroup qmi_ip_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t major_version;
  /**<   QMI IP major version.
   */

  uint32_t minor_version;
  /**<   QMI IP minor version.
   */
}qmi_ip_client_server_version_v01;  /* Type */
/**
    @}
  */

/** @addtogroup qmi_ip_qmi_messages
    @{
  */
/** Request Message; Initializes the QMI IP client session. */
typedef struct {

  /* Mandatory */
  /*  QMI IP client version */
  qmi_ip_client_server_version_v01 client_version;
  /**<   QMI IP client version.
  */

  /* Mandatory */
  /*  Client IP type */
  qmi_ip_client_addr_type_v01 ip_type;
  /**<   QMI IP client address type.
  */

  /* Optional */
  /*  Client IPv4 address */
  uint8_t client_ipv4_addr_valid;  /**< Must be set to true if client_ipv4_addr is being passed */
  uint32_t client_ipv4_addr;
  /**<   Client IPv4 address.
  */

  /* Optional */
  /*  Client IPv6 address */
  uint8_t client_ipv6_addr_valid;  /**< Must be set to true if client_ipv6_addr is being passed */
  uint8_t client_ipv6_addr[QMI_IP_IPV6_ADDR_LEN_V01];
  /**<   Client IPv6 address.
  */
}qmi_ip_client_session_init_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qmi_ip_qmi_messages
    @{
  */
/** Response Message; Initializes the QMI IP client session. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Mandatory */
  /*  QMI IP server version */
  qmi_ip_client_server_version_v01 server_version;
  /**<   QMI IP server version.
  */

  /* Mandatory */
  /*  FPOP time out value */
  uint32_t fpop_timeout;
  /**<   QMI IP client should generate at least
       one FPOP message within an fpop_timeout period.
	   This timeout value is defined in seconds.

  */

  /* Mandatory */
  /*  FPOP threshold value */
  uint32_t fpop_timeout_threshold;
  /**<   QMI IP service module will decide that the QMI IP
       client is no more active after a total timeout of
	   fpop_timeout_threshold*fpop_timeout amount of time.
  */
}qmi_ip_client_session_init_resp_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup qmi_ip_qmi_msg_ids
    @{
  */
#define QMI_IP_CLIENT_SESSION_INIT_REQ_V01 0x005E
#define QMI_IP_CLIENT_SESSION_INIT_RESP_V01 0x005E
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro qmi_ip_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type qmi_ip_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define qmi_ip_get_service_object_v01( ) \
          qmi_ip_get_service_object_internal_v01( \
            QMI_IP_V01_IDL_MAJOR_VERS, QMI_IP_V01_IDL_MINOR_VERS, \
            QMI_IP_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

