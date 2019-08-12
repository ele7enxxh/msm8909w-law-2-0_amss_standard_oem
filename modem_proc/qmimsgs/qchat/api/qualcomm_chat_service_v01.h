#ifndef QCHAT_SERVICE_01_H
#define QCHAT_SERVICE_01_H
/**
  @file qualcomm_chat_service_v01.h
  
  @brief This is the public header file which defines the qchat service Data structures.

  This header file defines the types and structures that were defined in 
  qchat. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All rights reserved.  Confidential and Proprietary - Qualcomm Technologies, Inc.

    QUALCOMM does not grant to you in this Agreement any rights in the OEM
    QChat Porting Kit including the accompanying associated documentation
    (the “Software”) for any purpose whatsoever, including without limitation
    any rights to copy, use or distribute the Software. Your rights to use the
    Software shall be only as set forth in any separate license between you and
    QUALCOMM which grants to you a license in the Software, and such license 
    shall be under the terms and for the limited purposes set forth in that 
    agreement.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/qchat/api/qualcomm_chat_service_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2 
   It was generated on: Wed Jul 24 2013 (Spin 1)
   From IDL File: qualcomm_chat_service_v01.idl */

/** @defgroup qchat_qmi_consts Constant values defined in the IDL */
/** @defgroup qchat_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup qchat_qmi_enums Enumerated types used in QMI messages */
/** @defgroup qchat_qmi_messages Structures sent as QMI messages */
/** @defgroup qchat_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup qchat_qmi_accessor Accessor for QMI service object */
/** @defgroup qchat_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qchat_qmi_version 
    @{ 
  */ 
/** Major Version Number of the IDL used to generate this file */
#define QCHAT_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QCHAT_V01_IDL_MINOR_VERS 0x03
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QCHAT_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define QCHAT_V01_MAX_MESSAGE_ID 0x0023;
/** 
    @} 
  */


/** @addtogroup qchat_qmi_consts 
    @{ 
  */
#define QCHAT_MAX_PAYLOAD_V01 8200
/**
    @}
  */

/** @addtogroup qchat_qmi_enums
    @{
  */
typedef enum {
  QCHAT_ERROR_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCHAT_ERROR_TYPE_SUCCESS_V01 = 0, 
  QCHAT_ERROR_TYPE_FAILED_V01 = 1, 
  QCHAT_ERROR_TYPE_NO_MEMORY_V01 = 2, 
  QCHAT_ERROR_TYPE_TIMEOUT_V01 = 3, 
  QCHAT_ERROR_TYPE_NOT_FOUND_V01 = 4, 
  QCHAT_ERROR_TYPE_BAD_PARAMETER_V01 = 5, 
  QCHAT_ERROR_TYPE_WOULDBLOCK_V01 = 6, 
  QCHAT_ERROR_TYPE_NOT_SUPPORTED_V01 = 7, 
  QCHAT_ERROR_TYPE_STATE_VIOLATION_V01 = 8, 
  QCHAT_ERROR_TYPE_DUPLICATE_V01 = 9, 
  QCHAT_ERROR_TYPE_NET_OPEN_PENDING_V01 = 101, 
  QCHAT_ERROR_TYPE_NET_OPEN_FAILURE_V01 = 102, 
  QCHAT_ERROR_TYPE_NET_WOULDBLOCK_V01 = 103, 
  QCHAT_ERROR_TYPE_NET_EINVAL_V01 = 104, 
  QCHAT_ERROR_TYPE_NET_ETIMEDOUT_V01 = 105, 
  QCHAT_ERROR_TYPE_SOCKET_SEND_FAILURE_V01 = 151, 
  QCHAT_ERROR_TYPE_SOCKET_SEND_SKIPPED_V01 = 152, 
  QCHAT_ERROR_TYPE_SOCKET_ALREADY_OPEN_V01 = 153, 
  QCHAT_ERROR_TYPE_SOCKET_NET_WOULDBLOCK_V01 = 154, 
  QCHAT_ERROR_TYPE_SOCKET_NET_ERROR_V01 = 155, 
  QCHAT_ERROR_TYPE_SOCKET_NOT_SUPPORTED_V01 = 156, 
  QCHAT_ERROR_TYPE_MEDIA_INIT_FAILED_V01 = 201, 
  QCHAT_ERROR_TYPE_MEDIA_VOC_CONFIG_FAILED_V01 = 202, 
  QCHAT_ERROR_TYPE_MEDIA_VOC_NOT_AVAILABLE_V01 = 203, 
  QCHAT_ERROR_TYPE_MEDIA_VOC_NOT_STARTED_V01 = 204, 
  QCHAT_ERROR_TYPE_MEDIA_VOC_IN_USE_V01 = 205, 
  QCHAT_ERROR_TYPE_MEDIA_VOC_ERATE_OUTOFRANGE_V01 = 206, 
  QCHAT_ERROR_TYPE_MEDIA_VOC_EBAD_TXREDUCTION_V01 = 207, 
  QCHAT_ERROR_TYPE_MEDIA_VOC_BUFFER_EMPTY_V01 = 208, 
  QCHAT_ERROR_TYPE_MEDIA_VOC_BUFFER_FULL_V01 = 209, 
  QCHAT_ERROR_TYPE_MEDIA_OEM_NOT_SUPPORTED_V01 = 210, 
  QCHAT_ERROR_TYPE_QOS_UNAWARE_NET_V01 = 301, 
  QCHAT_ERROR_TYPE_QOS_BAD_STATE_V01 = 302, 
  QCHAT_ERROR_TYPE_DB_FILE_OPEN_ERROR_V01 = 351, 
  QCHAT_ERROR_TYPE_DB_FILE_READ_ERROR_V01 = 352, 
  QCHAT_ERROR_TYPE_DB_FILE_WRITE_ERROR_V01 = 353, 
  QCHAT_ERROR_TYPE_DB_KEY_NOT_FOUND_V01 = 354, 
  QCHAT_ERROR_TYPE_DB_NOT_OPEN_V01 = 355, 
  QCHAT_ERROR_TYPE_DB_ALREADY_OPEN_V01 = 356, 
  QCHAT_ERROR_TYPE_DB_INSUFFICIENT_BUFFER_V01 = 357, 
  QCHAT_ERROR_TYPE_DB_CHECKSUM_FAILED_V01 = 358, 
  QCHAT_ERROR_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qchat_error_type_enum_v01;
/**
    @}
  */

/** @addtogroup qchat_qmi_enums
    @{
  */
typedef enum {
  QCHAT_TRIGGER_CATEGORY_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCHAT_TRIGGER_CATEGORY_CMDA_1X_REL0_V01 = 0, /**<  QChat air interface optimization
                                                ** for CDMA 1X REL-0 network.
                                                 */
  QCHAT_TRIGGER_CATEGORY_CDMA_1X_REVA_V01 = 1, /**<  QChat air interface optimization
                                               ** for CDMA 1X REV-A network.
                                                */
  QCHAT_TRIGGER_CATEGORY_CDMA_DO_REL0_V01 = 2, /**<  QChat air interface optimization
                                               ** for EVDO REL-0 network.
                                                */
  QCHAT_TRIGGER_CATEGORY_CDMA_DO_REVA_V01 = 3, /**<  QChat air interface optimization
                                               ** for EVDO REV-A network.
                                                */
  QCHAT_TRIGGER_CATEGORY_WCDMA_REL6_V01 = 4, /**<  QChat air interface optimization
                                               ** for WCDMA REL-6 network.
                                                */
  QCHAT_TRIGGER_CATEGORY_WCDMA_REL7_V01 = 5, /**<  QChat air interface optimization
                                               ** for WCDMA REL-7 network.
                                                */
  QCHAT_TRIGGER_CATEGORY_LTE_V01 = 6, /**<  QChat air interface optimization
                                               ** for LTE network.
                                                */
  QCHAT_TRIGGER_CATEGORY_AIR_INTERFACE_ANY_V01 = 7, /**<  QChat air interface agnostic optimization. 
                                                */
  QCHAT_TRIGGER_CATEGORY_APPID_QCAM_V01 = 8, /**<  QChat CAM Module.
                                                   */
  QCHAT_TRIGGER_CATEGORY_APPID_QCEN_V01 = 9, /**<  QChat CEN Module.
                                                   */
  QCHAT_TRIGGER_CATEGORY_APPID_QMED_V01 = 10, /**<  QChat MED Module.
                                                   */
  QCHAT_TRIGGER_CATEGORY_APPID_QREG_V01 = 11, /**<  QChat REG Module.
                                                  */
  QCHAT_TRIGGER_CATEGORY_APPID_QAUTH_V01 = 12, /**<  QChat AUTH Module.
                                                  */
  QCHAT_TRIGGER_CATEGORY_OEM_CUSTOMIZATION_V01 = 13, /**<  QChat OEM Customization Module.
												  */
  QCHAT_TRIGGER_CATEGORY_UTILITY_V01 = 14, /**<  QChat Utility Module.
											      */
  QCHAT_TRIGGER_CATEGORY_TRIGGER_MAX_V01 = 15, 
  QCHAT_TRIGGER_CATEGORY_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qchat_trigger_category_type_enum_v01;
/**
    @}
  */

/** @addtogroup qchat_qmi_enums
    @{
  */
typedef enum {
  QCHAT_EVENT_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCHAT_EVENT_TYPE_QOS_V01 = 0x00000001, 
  QCHAT_EVENT_TYPE_FA_CHANGE_V01 = 0x00000002, 
  QCHAT_EVENT_TYPE_LOCATION_CHANGE_V01 = 0x00000003, 
  QCHAT_EVENT_TYPE_QCAM_QMI_EVENT_V01 = 0x00001000, 
  QCHAT_EVENT_TYPE_QCEN_QMI_EVENT_V01 = 0x00001001, 
  QCHAT_EVENT_TYPE_QMED_QMI_EVENT_V01 = 0x00001002, 
  QCHAT_EVENT_TYPE_QREG_QMI_EVENT_V01 = 0x00001003, 
  QCHAT_EVENT_TYPE_QAUTH_QMI_EVENT_V01 = 0x00001004, 
  QCHAT_EVENT_TYPE_OEM_EVENT_V01 = 0x00010000, 
  QCHAT_EVENT_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qchat_event_type_enum_v01;
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qchat_init_req_msg_v01;

/** @addtogroup qchat_qmi_messages
    @{
  */
/** Response Message; Initializes QChat Interface. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   QChat client handle*/

  /* Optional */
  uint8_t handle_valid;  /**< Must be set to true if handle is being passed */
  uint32_t handle;
  /**<   API execution status*/

  /* Optional */
  uint8_t qchat_error_valid;  /**< Must be set to true if qchat_error is being passed */
  qchat_error_type_enum_v01 qchat_error;
}qchat_init_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qchat_qmi_messages
    @{
  */
/** Request Message; Executes the QChat optimization for particular interface. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   QChat handle
 QChat Component*/

  /* Mandatory */
  qchat_trigger_category_type_enum_v01 trigger_category;
  /**<   QChat data payload*/

  /* Mandatory */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QCHAT_MAX_PAYLOAD_V01];
}qchat_trigger_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qchat_qmi_messages
    @{
  */
/** Request Message; Executes the QChat optimization for particular interface. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   API execution status*/

  /* Optional */
  uint8_t qchat_error_valid;  /**< Must be set to true if qchat_error is being passed */
  qchat_error_type_enum_v01 qchat_error;
  /**<  QChat data payload*/

  /* Optional */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[QCHAT_MAX_PAYLOAD_V01];
}qchat_trigger_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qchat_qmi_messages
    @{
  */
/** Request Message; Enables/Disables the QChat Service to send events to the client. */
typedef struct {

  /* Optional */
  uint8_t enable_valid;  /**< Must be set to true if enable is being passed */
  uint8_t enable;
}qchat_event_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qchat_qmi_messages
    @{
  */
/** Request Message; Enables/Disables the QChat Service to send events to the client. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qchat_event_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup qchat_qmi_messages
    @{
  */
/** Indication Message; Asynchronous events of QMI_QCHAT_EVENT_REGISTER. */
typedef struct {

  /* Optional */
  uint8_t event_valid;  /**< Must be set to true if event is being passed */
  qchat_event_type_enum_v01 event;
  /**<   event Id
QChat data payload*/

  /* Optional */
  uint8_t payload_valid;  /**< Must be set to true if payload is being passed */
  uint32_t payload_len;  /**< Must be set to # of elements in payload */
  uint8_t payload[QCHAT_MAX_PAYLOAD_V01];
}qchat_event_registered_ind_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup qchat_qmi_msg_ids
    @{
  */
#define QMI_QCHAT_INIT_REQ_V01 0x0020
#define QMI_QCHAT_INIT_RESP_V01 0x0020
#define QMI_QCHAT_TRIGGER_REQ_V01 0x0021
#define QMI_QCHAT_TRIGGER_RESP_V01 0x0021
#define QMI_QCHAT_EVENT_REGISTER_REQ_V01 0x0022
#define QMI_QCHAT_EVENT_REGISTER_RESP_V01 0x0022
#define QMI_QCHAT_EVENT_REGISTERED_IND_V01 0x0023
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro qchat_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type qchat_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define qchat_get_service_object_v01( ) \
          qchat_get_service_object_internal_v01( \
            QCHAT_V01_IDL_MAJOR_VERS, QCHAT_V01_IDL_MINOR_VERS, \
            QCHAT_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

