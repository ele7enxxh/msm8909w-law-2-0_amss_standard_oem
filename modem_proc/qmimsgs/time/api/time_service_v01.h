#ifndef TIME_SERVICE_01_H
#define TIME_SERVICE_01_H
/**
  @file time_service_v01.h
  
  @brief This is the public header file which defines the time service Data structures.

  This header file defines the types and structures that were defined in 
  time. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/time/api/time_service_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2 
   It was generated on: Wed Jul 24 2013 (Spin 1)
   From IDL File: time_service_v01.idl */

/** @defgroup time_qmi_consts Constant values defined in the IDL */
/** @defgroup time_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup time_qmi_enums Enumerated types used in QMI messages */
/** @defgroup time_qmi_messages Structures sent as QMI messages */
/** @defgroup time_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup time_qmi_accessor Accessor for QMI service object */
/** @defgroup time_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup time_qmi_version 
    @{ 
  */ 
/** Major Version Number of the IDL used to generate this file */
#define TIME_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define TIME_V01_IDL_MINOR_VERS 0x04
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define TIME_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define TIME_V01_MAX_MESSAGE_ID 0x0031;
/** 
    @} 
  */


/** @addtogroup time_qmi_consts 
    @{ 
  */
/**
    @}
  */

/** @addtogroup time_qmi_enums
    @{
  */
typedef enum {
  TIME_SERVICE_BASE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  TIME_SERVICE_BASE_RTC_V01 = 0, /**<  Real time clock time base. */
  TIME_SERVICE_BASE_TOD_V01 = 1, /**<  Proxy base for the number of bases. */
  TIME_SERVICE_BASE_USER_V01 = 2, /**<  User time base.  */
  TIME_SERVICE_BASE_SECURE_V01 = 3, /**<  Secure time base.  */
  TIME_SERVICE_BASE_DRM_V01 = 4, /**<  Digital rights management time base.  */
  TIME_SERVICE_BASE_USER_UTC_V01 = 5, /**<  Universal time coordinated user time base.     */
  TIME_SERVICE_BASE_USER_TZ_DL_V01 = 6, /**<  Global time zone user time base.  */
  TIME_SERVICE_BASE_GPS_V01 = 7, /**<  Base for GPS time. \n
                   Note: When TIME_SERVICE_BASE_GPS is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.  */
  TIME_SERVICE_BASE_1X_V01 = 8, /**<  Base for 1X time. \n
                   Note: When TIME_SERVICE_BASE_1X is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.  */
  TIME_SERVICE_BASE_HDR_V01 = 9, /**<  Base for HDR time. \n
                   Note: When TIME_SERVICE_BASE_HDR is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.  */
  TIME_SERVICE_BASE_WCDMA_V01 = 10, /**<  Base for WCDMA time. \n
                   Note: When TIME_SERVICE_BASE_WCDMA is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.  */
  TIME_SERVICE_BASE_MFLO_V01 = 11, /**<  Base for MediaFLO time. \n
                   Note: When TIME_SERVICE_BASE_MFLO is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.    */
  TIME_SERVICE_BASE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}time_service_base_enum_v01;
/**
    @}
  */

/** @addtogroup time_qmi_enums
    @{
  */
typedef enum {
  TIME_SERVICE_INDICATIONS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  TIME_SERVICE_ATS_RTC_IND_MSG_V01 = 0, /**<  Indication message for RTC offset update. */
  TIME_SERVICE_ATS_TOD_IND_MSG_V01 = 1, /**<  Indication message for TOD offset update. */
  TIME_SERVICE_ATS_USER_IND_MSG_V01 = 2, /**<  Indication message for USER offset update. */
  TIME_SERVICE_ATS_SECURE_IND_MSG_V01 = 3, /**<  Indication message for SECURE offset update. */
  TIME_SERVICE_ATS_DRM_IND_MSG_V01 = 4, /**<  Indication message for DRM offset update. */
  TIME_SERVICE_ATS_USER_UTC_IND_MSG_V01 = 5, /**<  Indication message for USER_UTC offset update. */
  TIME_SERVICE_ATS_USER_TZ_DL_IND_MSG_V01 = 6, /**<  Indication message for USER_TZ_DL offset update. */
  TIME_SERVICE_ATS_GPS_IND_MSG_V01 = 7, /**<  Indication message for GPS offset update. */
  TIME_SERVICE_ATS_1X_IND_MSG_V01 = 8, /**<  Indication message for 1X offset update. */
  TIME_SERVICE_ATS_HDR_IND_MSG_V01 = 9, /**<  Indication message for HDR offset update. */
  TIME_SERVICE_ATS_WCDMA_IND_MSG_V01 = 10, /**<  Indication message for WCDMA offset update. */
  TIME_SERVICE_ATS_MFLO_IND_MSG_V01 = 11, /**<  Indication message for MFLO offset update. */
  TIME_SERVICE_ATS_3GPP_IND_MSG_V01 = 12, /**<  Indication message for 3GPP offset update. */
  TIME_SERVICE_INDICATIONS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}time_service_indications_enum_v01;
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Request Message; Sets the generic offset specified by the base 
                 using the TIME service.  */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  time_service_base_enum_v01 base;
  /**<   Specifies the time base to be set on the modem. Values: \n
      - TIME_SERVICE_BASE_RTC (0) --  Real time clock time base.
      - TIME_SERVICE_BASE_TOD (1) --  Proxy base for the number of bases.
      - TIME_SERVICE_BASE_USER (2) --  User time base. 
      - TIME_SERVICE_BASE_SECURE (3) --  Secure time base. 
      - TIME_SERVICE_BASE_DRM (4) --  Digital rights management time base. 
      - TIME_SERVICE_BASE_USER_UTC (5) --  Universal time coordinated user time base.    
      - TIME_SERVICE_BASE_USER_TZ_DL (6) --  Global time zone user time base. 
      - TIME_SERVICE_BASE_GPS (7) --  Base for GPS time. \n
                   Note: When TIME_SERVICE_BASE_GPS is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_1X (8) --  Base for 1X time. \n
                   Note: When TIME_SERVICE_BASE_1X is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_HDR (9) --  Base for HDR time. \n
                   Note: When TIME_SERVICE_BASE_HDR is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_WCDMA (10) --  Base for WCDMA time. \n
                   Note: When TIME_SERVICE_BASE_WCDMA is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_MFLO (11) --  Base for MediaFLO time. \n
                   Note: When TIME_SERVICE_BASE_MFLO is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.   
 */

  /* Mandatory */
  /*  Offset Value */
  uint64_t generic_offset;
  /**<   Offset value to set on the remote procedure. Current time = RTC value at bootup + generic offset + uptime. 
      Therefore the generic offset = 
      the number of ms that have elapsed from January 06, 1980 - RTC offset at bootup - uptime          
  */
}time_set_genoff_offset_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Response Message; Sets the generic offset specified by the base 
                 using the TIME service.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. 
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */
}time_set_genoff_offset_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Request Message; Retrieves the TIME service specified generic offset value.  */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  time_service_base_enum_v01 base;
  /**<   Time base to be obtained from the modem. \n
      - TIME_SERVICE_BASE_RTC (0) --  Real time clock time base.
      - TIME_SERVICE_BASE_TOD (1) --  Proxy base for the number of bases.
      - TIME_SERVICE_BASE_USER (2) --  User time base. 
      - TIME_SERVICE_BASE_SECURE (3) --  Secure time base. 
      - TIME_SERVICE_BASE_DRM (4) --  Digital rights management time base. 
      - TIME_SERVICE_BASE_USER_UTC (5) --  Universal time coordinated user time base.    
      - TIME_SERVICE_BASE_USER_TZ_DL (6) --  Global time zone user time base. 
      - TIME_SERVICE_BASE_GPS (7) --  Base for GPS time. \n
                   Note: When TIME_SERVICE_BASE_GPS is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_1X (8) --  Base for 1X time. \n
                   Note: When TIME_SERVICE_BASE_1X is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_HDR (9) --  Base for HDR time. \n
                   Note: When TIME_SERVICE_BASE_HDR is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_WCDMA (10) --  Base for WCDMA time. \n
                   Note: When TIME_SERVICE_BASE_WCDMA is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_MFLO (11) --  Base for MediaFLO time. \n
                   Note: When TIME_SERVICE_BASE_MFLO is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.   
 */
}time_get_genoff_offset_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Response Message; Retrieves the TIME service specified generic offset value.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. 
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Mandatory */
  /*  Time Service Base */
  time_service_base_enum_v01 base;
  /**<   Time base to be obtained from the modem. \n
      - TIME_SERVICE_BASE_RTC (0) --  Real time clock time base.
      - TIME_SERVICE_BASE_TOD (1) --  Proxy base for the number of bases.
      - TIME_SERVICE_BASE_USER (2) --  User time base. 
      - TIME_SERVICE_BASE_SECURE (3) --  Secure time base. 
      - TIME_SERVICE_BASE_DRM (4) --  Digital rights management time base. 
      - TIME_SERVICE_BASE_USER_UTC (5) --  Universal time coordinated user time base.    
      - TIME_SERVICE_BASE_USER_TZ_DL (6) --  Global time zone user time base. 
      - TIME_SERVICE_BASE_GPS (7) --  Base for GPS time. \n
                   Note: When TIME_SERVICE_BASE_GPS is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_1X (8) --  Base for 1X time. \n
                   Note: When TIME_SERVICE_BASE_1X is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_HDR (9) --  Base for HDR time. \n
                   Note: When TIME_SERVICE_BASE_HDR is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_WCDMA (10) --  Base for WCDMA time. \n
                   Note: When TIME_SERVICE_BASE_WCDMA is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_MFLO (11) --  Base for MediaFLO time. \n
                   Note: When TIME_SERVICE_BASE_MFLO is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.   
 */

  /* Mandatory */
  /*  Offset Value */
  uint64_t generic_offset;
  /**<   Offset value to set on the remote procedure. Current time = RTC value at bootup + generic offset + uptime. 
      Therefore the generic offset = 
      the number of ms elapsed from January 6, 1980 - RTC offset at bootup - uptime
  */
}time_get_genoff_offset_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Request Message; Sets the leap seconds on the modem.  */
typedef struct {

  /* Mandatory */
  /*  Leap Seconds Set Value */
  uint8_t leap_sec_set_value;
  /**<   Leap seconds to be set on the modem. */
}time_set_leap_sec_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Response Message; Sets the leap seconds on the modem.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. 
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                        the error codes section of each message definition.
  */
}time_set_leap_sec_resp_msg_v01;  /* Message */
/**
    @}
  */

typedef struct {
  /* This element is a placeholder to prevent the declaration of 
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}time_get_leap_sec_req_msg_v01;

/** @addtogroup time_qmi_messages
    @{
  */
/** Response Message; Retrieves the leap seconds from the modem.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. 
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */

  /* Mandatory */
  /*  Leap Second */
  uint8_t leap_second;
  /**<   Leap second value to retrieve from the remote procedure. */
}time_get_leap_sec_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Request Message; Turns off the specified indication. */
typedef struct {

  /* Mandatory */
  /*  Indication To Turn Off */
  time_service_indications_enum_v01 msg_id;
  /**<   Specifies the indication to be turned off.\n
      - TIME_SERVICE_ATS_RTC_IND_MSG (0) --  Indication message for RTC offset update.
      - TIME_SERVICE_ATS_TOD_IND_MSG (1) --  Indication message for TOD offset update.
      - TIME_SERVICE_ATS_USER_IND_MSG (2) --  Indication message for USER offset update.
      - TIME_SERVICE_ATS_SECURE_IND_MSG (3) --  Indication message for SECURE offset update.
      - TIME_SERVICE_ATS_DRM_IND_MSG (4) --  Indication message for DRM offset update.
      - TIME_SERVICE_ATS_USER_UTC_IND_MSG (5) --  Indication message for USER_UTC offset update.
      - TIME_SERVICE_ATS_USER_TZ_DL_IND_MSG (6) --  Indication message for USER_TZ_DL offset update.
      - TIME_SERVICE_ATS_GPS_IND_MSG (7) --  Indication message for GPS offset update.
      - TIME_SERVICE_ATS_1X_IND_MSG (8) --  Indication message for 1X offset update.
      - TIME_SERVICE_ATS_HDR_IND_MSG (9) --  Indication message for HDR offset update.
      - TIME_SERVICE_ATS_WCDMA_IND_MSG (10) --  Indication message for WCDMA offset update.
      - TIME_SERVICE_ATS_MFLO_IND_MSG (11) --  Indication message for MFLO offset update.
      - TIME_SERVICE_ATS_3GPP_IND_MSG (12) --  Indication message for 3GPP offset update.
 */
}time_turn_off_ind_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Response Message; Turns off the specified indication. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. 
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                         the error codes section of each message definition.
  */
}time_turn_off_ind_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Request Message; Turns on the specified indication */
typedef struct {

  /* Mandatory */
  /*  Indication To Turn On */
  time_service_indications_enum_v01 msg_id;
  /**<   Specifies the indication to turn on. \n
      - TIME_SERVICE_ATS_RTC_IND_MSG (0) --  Indication message for RTC offset update.
      - TIME_SERVICE_ATS_TOD_IND_MSG (1) --  Indication message for TOD offset update.
      - TIME_SERVICE_ATS_USER_IND_MSG (2) --  Indication message for USER offset update.
      - TIME_SERVICE_ATS_SECURE_IND_MSG (3) --  Indication message for SECURE offset update.
      - TIME_SERVICE_ATS_DRM_IND_MSG (4) --  Indication message for DRM offset update.
      - TIME_SERVICE_ATS_USER_UTC_IND_MSG (5) --  Indication message for USER_UTC offset update.
      - TIME_SERVICE_ATS_USER_TZ_DL_IND_MSG (6) --  Indication message for USER_TZ_DL offset update.
      - TIME_SERVICE_ATS_GPS_IND_MSG (7) --  Indication message for GPS offset update.
      - TIME_SERVICE_ATS_1X_IND_MSG (8) --  Indication message for 1X offset update.
      - TIME_SERVICE_ATS_HDR_IND_MSG (9) --  Indication message for HDR offset update.
      - TIME_SERVICE_ATS_WCDMA_IND_MSG (10) --  Indication message for WCDMA offset update.
      - TIME_SERVICE_ATS_MFLO_IND_MSG (11) --  Indication message for MFLO offset update.
      - TIME_SERVICE_ATS_3GPP_IND_MSG (12) --  Indication message for 3GPP offset update.
 */
}time_turn_on_ind_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Response Message; Turns on the specified indication */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. 
 Standard response type. Contains the following data members:
       qmi_result_type - QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE
       qmi_error_type  - Error code. Possible error code values are described in
                        the error codes section of each message definition.
  */
}time_turn_on_ind_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup time_qmi_messages
    @{
  */
/** Indication Message; Generic indication definition. Notifies the client of any
                 change in time offsets on the server. */
typedef struct {

  /* Mandatory */
  /*  Time Service Base */
  time_service_base_enum_v01 base;
  /**<   Specifies the time base for which the indication is to be sent. \n
      - TIME_SERVICE_BASE_RTC (0) --  Real time clock time base.
      - TIME_SERVICE_BASE_TOD (1) --  Proxy base for the number of bases.
      - TIME_SERVICE_BASE_USER (2) --  User time base. 
      - TIME_SERVICE_BASE_SECURE (3) --  Secure time base. 
      - TIME_SERVICE_BASE_DRM (4) --  Digital rights management time base. 
      - TIME_SERVICE_BASE_USER_UTC (5) --  Universal time coordinated user time base.    
      - TIME_SERVICE_BASE_USER_TZ_DL (6) --  Global time zone user time base. 
      - TIME_SERVICE_BASE_GPS (7) --  Base for GPS time. \n
                   Note: When TIME_SERVICE_BASE_GPS is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_1X (8) --  Base for 1X time. \n
                   Note: When TIME_SERVICE_BASE_1X is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_HDR (9) --  Base for HDR time. \n
                   Note: When TIME_SERVICE_BASE_HDR is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_WCDMA (10) --  Base for WCDMA time. \n
                   Note: When TIME_SERVICE_BASE_WCDMA is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD. 
      - TIME_SERVICE_BASE_MFLO (11) --  Base for MediaFLO time. \n
                   Note: When TIME_SERVICE_BASE_MFLO is modified, changes are also 
                   reflected on TIME_SERVICE_BASE_TOD.   
 */

  /* Mandatory */
  /*  Offset */
  uint64_t offset;
  /**<   Offset value to send to the remote procedure. 
       Number of ms elapsed from January 6, 1980. */
}time_update_indication_message_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup time_qmi_msg_ids
    @{
  */
#define QMI_TIME_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_TIME_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_TIME_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_TIME_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_TIME_GENOFF_SET_REQ_V01 0x0020
#define QMI_TIME_GENOFF_SET_RESP_V01 0x0020
#define QMI_TIME_GENOFF_GET_REQ_V01 0x0021
#define QMI_TIME_GENOFF_GET_RESP_V01 0x0021
#define QMI_TIME_TURN_OFF_IND_REQ_V01 0x0022
#define QMI_TIME_TURN_OFF_IND_RESP_V01 0x0022
#define QMI_TIME_TURN_ON_IND_REQ_V01 0x0023
#define QMI_TIME_TURN_ON_IND_RESP_V01 0x0023
#define QMI_TIME_LEAP_SEC_SET_REQ_V01 0x0024
#define QMI_TIME_LEAP_SEC_SET_RESP_V01 0x0024
#define QMI_TIME_LEAP_SEC_GET_REQ_V01 0x0025
#define QMI_TIME_LEAP_SEC_GET_RESP_V01 0x0025
#define QMI_TIME_ATS_RTC_UPDATE_IND_V01 0x0026
#define QMI_TIME_ATS_TOD_UPDATE_IND_V01 0x0027
#define QMI_TIME_ATS_USER_UPDATE_IND_V01 0x0028
#define QMI_TIME_ATS_SECURE_UPDATE_IND_V01 0x0029
#define QMI_TIME_ATS_DRM_UPDATE_IND_V01 0x002A
#define QMI_TIME_ATS_USER_UTC_UPDATE_IND_V01 0x002B
#define QMI_TIME_ATS_USER_TZ_DL_UPDATE_IND_V01 0x002C
#define QMI_TIME_ATS_GPS_UPDATE_IND_V01 0x002D
#define QMI_TIME_ATS_1X_UPDATE_IND_V01 0x002E
#define QMI_TIME_ATS_HDR_UPDATE_IND_V01 0x002F
#define QMI_TIME_ATS_WCDMA_UPDATE_IND_V01 0x0030
#define QMI_TIME_ATS_BREW_UPDATE_IND_V01 0x0031
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro time_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type time_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define time_get_service_object_v01( ) \
          time_get_service_object_internal_v01( \
            TIME_V01_IDL_MAJOR_VERS, TIME_V01_IDL_MINOR_VERS, \
            TIME_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

