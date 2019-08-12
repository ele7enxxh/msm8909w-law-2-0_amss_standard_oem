#ifndef CFCM_SERVICE_01_H
#define CFCM_SERVICE_01_H
/**
  @file common_flow_control_management_v01.h

  @brief This is the public header file which defines the cfcm service Data structures.

  This header file defines the types and structures that were defined in
  cfcm. It contains the constant values defined, enums, structures,
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


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/cfcm/api/common_flow_control_management_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2 
   It was generated on: Thu Jan 22 2015 (Spin 1)
   From IDL File: common_flow_control_management_v01.idl */

/** @defgroup cfcm_qmi_consts Constant values defined in the IDL */
/** @defgroup cfcm_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup cfcm_qmi_enums Enumerated types used in QMI messages */
/** @defgroup cfcm_qmi_messages Structures sent as QMI messages */
/** @defgroup cfcm_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup cfcm_qmi_accessor Accessor for QMI service object */
/** @defgroup cfcm_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup cfcm_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define CFCM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define CFCM_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define CFCM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define CFCM_V01_MAX_MESSAGE_ID 0x0021
/**
    @}
  */


/** @addtogroup cfcm_qmi_consts 
    @{ 
  */
/**
    @}
  */

/** @addtogroup cfcm_qmi_enums
    @{
  */
typedef enum {
  QMI_CFCM_BUS_BW_THROTTLE_LEVEL_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_CFCM_BUS_BW_NO_THROTTLE_V01 = 0, /**<  No throttling level \n  */
  QMI_CFCM_BUS_BW_MED_THROTTLE_V01 = 1, /**<  Medium throttling level \n  */
  QMI_CFCM_BUS_BW_HIGH_THROTTLE_V01 = 2, /**<  High throttling level \n  */
  QMI_CFCM_BUS_BW_CRITICAL_THROTTLE_V01 = 3, /**<  Critical throttling level  */
  QMI_CFCM_BUS_BW_THROTTLE_LEVEL_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_cfcm_bus_bw_throttle_level_enum_type_v01;
/**
    @}
  */

/** @addtogroup cfcm_qmi_messages
    @{
  */
/** Request Message; Sets the specified bus bandwidth throttling level. */
typedef struct {

  /* Mandatory */
  /*  BW Throttling Level */
  qmi_cfcm_bus_bw_throttle_level_enum_type_v01 bus_bw_throttling_level;
  /**<   Bandwidth throttling level. \n
 Values: \n 
      - QMI_CFCM_BUS_BW_NO_THROTTLE (0) --  No throttling level \n 
      - QMI_CFCM_BUS_BW_MED_THROTTLE (1) --  Medium throttling level \n 
      - QMI_CFCM_BUS_BW_HIGH_THROTTLE (2) --  High throttling level \n 
      - QMI_CFCM_BUS_BW_CRITICAL_THROTTLE (3) --  Critical throttling level 
 */
}cfcm_set_bus_bw_throttling_level_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cfcm_qmi_messages
    @{
  */
/** Response Message; Sets the specified bus bandwidth throttling level. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
     qmi_error_type  - Error code. Possible error code values are described in 
                       the error codes section of each message definition.
    */
}cfcm_set_bus_bw_throttling_level_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup cfcm_qmi_messages
    @{
  */
/** Request Message; Gets the specified bus bandwidth throttling level. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}cfcm_get_bus_bw_throttling_level_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup cfcm_qmi_messages
    @{
  */
/** Response Message; Gets the specified bus bandwidth throttling level. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */

  /* Optional */
  /*  Throttling Level  */
  uint8_t bus_bw_throttling_level_valid;  /**< Must be set to true if bus_bw_throttling_level is being passed */
  qmi_cfcm_bus_bw_throttle_level_enum_type_v01 bus_bw_throttling_level;
  /**<   Bus bandwidth throttling level. \n
 Values: \n
      - QMI_CFCM_BUS_BW_NO_THROTTLE (0) --  No throttling level \n 
      - QMI_CFCM_BUS_BW_MED_THROTTLE (1) --  Medium throttling level \n 
      - QMI_CFCM_BUS_BW_HIGH_THROTTLE (2) --  High throttling level \n 
      - QMI_CFCM_BUS_BW_CRITICAL_THROTTLE (3) --  Critical throttling level 
 */
}cfcm_get_bus_bw_throttling_level_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_CFCM_BUS_BW_THROTTLING_LEVEL_V01 
//#define REMOVE_QMI_CFCM_GET_BUS_BW_THROTTLING_LEVEL_V01 

/*Service Message Definition*/
/** @addtogroup cfcm_qmi_msg_ids
    @{
  */
#define QMI_CFCM_BUS_BW_THROTTLING_LEVEL_REQ_MSG_V01 0x0020
#define QMI_CFCM_BUS_BW_THROTTLING_LEVEL_RESP_MSG_V01 0x0020
#define QMI_CFCM_GET_BW_THROTTLING_LEVEL_REQ_MSG_V01 0x0021
#define QMI_CFCM_GET_BW_THROTTLING_LEVEL_RESP_MSG_V01 0x0021
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro cfcm_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type cfcm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define cfcm_get_service_object_v01( ) \
          cfcm_get_service_object_internal_v01( \
            CFCM_V01_IDL_MAJOR_VERS, CFCM_V01_IDL_MINOR_VERS, \
            CFCM_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

