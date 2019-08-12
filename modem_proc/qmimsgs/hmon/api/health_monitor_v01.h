#ifndef HMON_SERVICE_01_H
#define HMON_SERVICE_01_H
/**
  @file health_monitor_v01.h

  @brief This is the public header file which defines the hmon service Data structures.

  This header file defines the types and structures that were defined in
  hmon. It contains the constant values defined, enums, structures,
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
  


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/hmon/api/health_monitor_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.3 
   It was generated on: Tue Mar  3 2015 (Spin 1)
   From IDL File: health_monitor_v01.idl */

/** @defgroup hmon_qmi_consts Constant values defined in the IDL */
/** @defgroup hmon_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup hmon_qmi_enums Enumerated types used in QMI messages */
/** @defgroup hmon_qmi_messages Structures sent as QMI messages */
/** @defgroup hmon_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup hmon_qmi_accessor Accessor for QMI service object */
/** @defgroup hmon_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hmon_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define HMON_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define HMON_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define HMON_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define HMON_V01_MAX_MESSAGE_ID 0x0022
/**
    @}
  */


/** @addtogroup hmon_qmi_consts 
    @{ 
  */
/**
    @}
  */

/** @addtogroup hmon_qmi_messages
    @{
  */
/** Request Message; Registers a health monitor client with the service. */
typedef struct {

  /* Optional */
  /*  HMON Agent Name */
  uint8_t name_valid;  /**< Must be set to true if name is being passed */
  char name[255 + 1];
  /**<   Name of the agent: allows the service to differentiate
       between various agents. */

  /* Optional */
  /*  Timeout */
  uint8_t timeout_valid;  /**< Must be set to true if timeout is being passed */
  uint32_t timeout;
  /**<   Timeout, in milliseconds, that QMI_HMON uses before 
       restarting this subsystem. */
}hmon_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup hmon_qmi_messages
    @{
  */
/** Response Message; Registers a health monitor client with the service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}hmon_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup hmon_qmi_messages
    @{
  */
/** Indication Message; Initiates a health check among clients. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}hmon_health_check_ind_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup hmon_qmi_enums
    @{
  */
typedef enum {
  HMON_CHECK_RESULT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  HMON_CHECK_SUCCESS_V01 = 0, 
  HMON_CHECK_FAILURE_V01 = 1, 
  HMON_CHECK_RESULT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}hmon_check_result_v01;
/**
    @}
  */

/** @addtogroup hmon_qmi_messages
    @{
  */
/** Request Message; Notifies when a requested health check is complete. */
typedef struct {

  /* Optional */
  /*  Health Check Result */
  uint8_t result_valid;  /**< Must be set to true if result is being passed */
  hmon_check_result_v01 result;
  /**<   Optional result to inform QMI_HMON whether it must
       take any action on this subsystem. If this TLV is not present, 
       QMI_HMON assumes success. */
}hmon_health_check_complete_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup hmon_qmi_messages
    @{
  */
/** Response Message; Notifies when a requested health check is complete. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. */
}hmon_health_check_complete_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_HMON_HEALTH_CHECK_V01 
//#define REMOVE_QMI_HMON_HEALTH_CHECK_COMPLETE_V01 
//#define REMOVE_QMI_HMON_REG_V01 

/*Service Message Definition*/
/** @addtogroup hmon_qmi_msg_ids
    @{
  */
#define QMI_HMON_REG_REQ_V01 0x0020
#define QMI_HMON_REG_RESP_V01 0x0020
#define QMI_HMON_HEALTH_CHECK_IND_V01 0x0021
#define QMI_HMON_HEALTH_CHECK_COMPLETE_REQ_V01 0x0022
#define QMI_HMON_HEALTH_CHECK_COMPLETE_RESP_V01 0x0022
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro hmon_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type hmon_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define hmon_get_service_object_v01( ) \
          hmon_get_service_object_internal_v01( \
            HMON_V01_IDL_MAJOR_VERS, HMON_V01_IDL_MINOR_VERS, \
            HMON_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

