#ifndef SNS_QFP_SVC_SERVICE_01_H
#define SNS_QFP_SVC_SERVICE_01_H
/**
  @file sns_qfp_v01.h

  @brief This is the public header file which defines the SNS_QFP_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_QFP_SVC. It contains the constant values defined, enums, structures,
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

/* This file was generated with Tool version 6.14.7
   It was generated on: Wed Nov 25 2015 (Spin 0)
   From IDL File: sns_qfp_v01.idl */

/** @defgroup SNS_QFP_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_QFP_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_QFP_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_QFP_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_QFP_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_QFP_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_QFP_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_QFP_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_QFP_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_QFP_SVC_V01_IDL_MINOR_VERS 0x03
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_QFP_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_QFP_SVC_V01_MAX_MESSAGE_ID 0x002C
/**
    @}
  */


/** @addtogroup SNS_QFP_SVC_qmi_consts
    @{
  */

/**  maximum size of a simple configuration item (float/integer/...)  */
#define SNS_QFP_SIMPLE_ITEM_MAX_SIZE_V01 8

/**  maximum size of a single chunk of uploaded basis data  */
#define SNS_QFP_FFD_BASIS_DATA_MAX_CHUNK_SIZE_V01 4000

/**  maximum size of a debug request and response

 Flag bits for SNS_QFP_FINGER_DETECT_IND */
#define SNS_QFP_DEBUG_MSG_MAX_SIZE_V01 4000

/**  Configuration item IDs
 Configuration items for fast finger detection algorithm

 threshold for S1 phase, ON (float32)  */
#define SNS_QFP_FFD_ITEM_THRESHOLD_S1_V01 100

/**  threshold for S1 phase, OFF (float32)  */
#define SNS_QFP_FFD_ITEM_THRESHOLD_S1_OFF_V01 101

/**  threshold for S2 phase, ON (float32)  */
#define SNS_QFP_FFD_ITEM_THRESHOLD_S2_V01 102

/**  threshold for S2 phase, OFF (float32)  */
#define SNS_QFP_FFD_ITEM_THRESHOLD_S2_OFF_V01 103

/**  number of image runs in S1 phase (uint32)  */
#define SNS_QFP_FFD_ITEM_NUM_IMAGE_RUNS_S1_V01 104

/**  threshold for S1 phase, rubbing (uint32)  */
#define SNS_QFP_FFD_ITEM_THRESHOLD_S1_RUBBING_V01 105

/**  threshold for home-button, finger down (uint32)  */
#define SNS_QFP_FFD_ITEM_THRESHOLD_S2_HOME_TOUCH_V01 106

/**  threshold for home-button, finger up (uint32)  */
#define SNS_QFP_FFD_ITEM_THRESHOLD_S2_HOME_LIFT_V01 107

/**  log buffer size (uint32)   */
#define SNS_QFP_FFD_ITEM_LOG_BUFFER_SIZE_V01 108

/**  basis data for S1 phase, ON (binary data)  */
#define SNS_QFP_FFD_ITEM_BASIS_S1_V01 150

/**  basis data for S1 phase, OFF (binary data)  */
#define SNS_QFP_FFD_ITEM_BASIS_S1_OFF_V01 151

/**  basis data for S2 phase, ON (binary data)  */
#define SNS_QFP_FFD_ITEM_BASIS_S2_V01 152

/**  basis data for S2 phase, OFF (binary data)  */
#define SNS_QFP_FFD_ITEM_BASIS_S2_OFF_V01 153

/**  disable S1 phase (uint32, bool)  */
#define SNS_QFP_FFD_ITEM_S1_DISABLE_V01 154

/**  S1 phase FGOFF diluation (uint32)  */
#define SNS_QFP_FFD_ITEM_FGOFF_DILUATION_S1_V01 155

/**  FFD enable island mode (bool)  */
#define SNS_QFP_FFD_ITEM_ENABLE_ISLAND_MODE_V01 156

/**  Home button FGOFF diluation (uint32)  */
#define SNS_QFP_FFD_ITEM_FGOFF_DILUATION_HOME_V01 157

/**  BG-E normalization vector for S1 phase, ON (binary data)  */
#define SNS_QFP_FFD_ITEM_BASIS_S1_NORM_V01 158

/**  BG-E normalization vector for S1 phase, OFF (binary data)  */
#define SNS_QFP_FFD_ITEM_BASIS_S1_NORM_OFF_V01 159

/**  BG-E normalization vector for S2 phase, ON (binary data)  */
#define SNS_QFP_FFD_ITEM_BASIS_S2_NORM_V01 160

/**  BG-E normalization vector for S2 phase, OFF (binary data)  */
#define SNS_QFP_FFD_ITEM_BASIS_S2_NORM_OFF_V01 161
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_enums
    @{
  */
typedef enum {
  SNS_QFP_FINGER_DETECT_MODE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_QFP_FINGER_DETECT_MODE_ONE_SHOT_V01 = 1, /**<  detector is disabled once finger is detected  */
  SNS_QFP_FINGER_DETECT_MODE_CONTINUOUS_V01 = 2, /**<  detector keeps sending finger state change indications  */
  SNS_QFP_FINGER_DETECT_MODE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sns_qfp_finger_detect_mode_e_v01;
/**
    @}
  */

typedef uint64_t sns_qfp_finger_detect_ind_flags_m_v01;
#define SNS_QFP_FINGER_DETECT_IND_FLAG_DETECTED_V01 ((sns_qfp_finger_detect_ind_flags_m_v01)0x001ull) /**<  Bit will be set if finger was detected, clear if not  */
/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Allocate resources, opens SPI port and configures clocks. */
typedef struct {

  /* Mandatory */
  uint8_t port_id;
  /**<
     SPI port ID. Values 1-3 for SSC SPI ports 1 to 3.
        */

  /* Mandatory */
  uint8_t slave_index;
  /**<
     SPI slave index to identify FP sensor when multiple devices are
     connected to the SPI bus, otherwise set to 0
        */

  /* Mandatory */
  uint32_t freq;
  /**<
     Requested clock frequency to use in SPI transactions, in Hz.
        */
}sns_qfp_open_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Allocate resources, opens SPI port and configures clocks. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
}sns_qfp_open_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Close SPI port, free resources used by sensor */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}sns_qfp_close_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Close SPI port, free resources used by sensor */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
}sns_qfp_close_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Enable or disable keep alive - prevent SLPI from shutting down
           SPI port. */
typedef struct {

  /* Mandatory */
  uint8_t enable;
  /**<   enable or disable keep alive  */
}sns_qfp_keep_alive_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Enable or disable keep alive - prevent SLPI from shutting down
           SPI port. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
}sns_qfp_keep_alive_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Start the wake-up finger detect processing. */
typedef struct {

  /* Mandatory */
  uint32_t poll_time;
  /**<   polling time in ms */

  /* Optional */
  uint8_t finger_detect_mode_valid;  /**< Must be set to true if finger_detect_mode is being passed */
  sns_qfp_finger_detect_mode_e_v01 finger_detect_mode;
  /**<   finger detection mode */

  /* Optional */
  uint8_t low_poll_time_valid;  /**< Must be set to true if low_poll_time is being passed */
  uint32_t low_poll_time;
  /**<   low polling time in ms. Default is 1000*/
}sns_qfp_finger_detect_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Start the wake-up finger detect processing. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
}sns_qfp_finger_detect_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Stop the wake-up finger detect processing. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}sns_qfp_finger_detect_disable_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Stop the wake-up finger detect processing. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
}sns_qfp_finger_detect_disable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Indication Message; Indication when finger is detected */
typedef struct {

  /* Mandatory */
  uint32_t timestamp;
  /**<   timestamp of event in SSC ticks */

  /* Mandatory */
  sns_qfp_finger_detect_ind_flags_m_v01 flags;
  /**<   Defines the indication flags. This bit flags have meanings following:
    - 01 - SNS_QFP_FINGER_DETECT_IND_FLAG_DETECTED - Finger was detected.
      Note that this indication is always sent when finger detection sensor is
      disabled for any reason. If a finger was detected, the sensor will be
      disabled because it is a one-shot sensor and the indication will be sent
      with this bit set. If the sensor was explicitly disabled by a service
      command, the indication will be sent with this bit clear.
    - All other values are reserved for future use
  */
}sns_qfp_finger_detect_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Set the value of a simple configuration item */
typedef struct {

  /* Mandatory */
  uint32_t item_id;
  /**<   ID of the item to be written */

  /* Mandatory */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[SNS_QFP_SIMPLE_ITEM_MAX_SIZE_V01];
  /**<   Item data to be written to the registry. Data types varies according
       to item */
}sns_qfp_simple_item_write_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Set the value of a simple configuration item */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
}sns_qfp_simple_item_write_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Initialize upload of basis data file */
typedef struct {

  /* Mandatory */
  uint32_t item_id;
  /**<   ID of the basis data to be uploaded */

  /* Mandatory */
  uint32_t vector_size;
  /**<   size of each basis vector in units of float32 */

  /* Mandatory */
  uint32_t num_vectors;
  /**<   number of basis vectors in the data */
}sns_qfp_ffd_basis_data_create_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Initialize upload of basis data file */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t handle_valid;  /**< Must be set to true if handle is being passed */
  uint32_t handle;
  /**<   handle of basis data file. Use in write and close requests */
}sns_qfp_ffd_basis_data_create_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Write a chunk of basis data */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   handle of basis data file returned from open request */

  /* Mandatory */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[SNS_QFP_FFD_BASIS_DATA_MAX_CHUNK_SIZE_V01];
  /**<   data to write */
}sns_qfp_ffd_basis_data_write_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Write a chunk of basis data */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t written_valid;  /**< Must be set to true if written is being passed */
  uint32_t written;
  /**<   number of bytes actually written */
}sns_qfp_ffd_basis_data_write_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Finish upload of basis data file */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   handle of basis data file returned from open request */
}sns_qfp_ffd_basis_data_close_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Finish upload of basis data file */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
}sns_qfp_ffd_basis_data_close_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Debug and diagnostics command. For internal usage only,
           may be disabled in production */
typedef struct {

  /* Mandatory */
  uint32_t req_id;
  /**<   id of debug request */

  /* Optional */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[SNS_QFP_DEBUG_MSG_MAX_SIZE_V01];
  /**<   request data */
}sns_qfp_debug_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Debug and diagnostics command. For internal usage only,
           may be disabled in production */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t req_id_valid;  /**< Must be set to true if req_id is being passed */
  uint32_t req_id;
  /**<   id of debug request issued */

  /* Optional */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[SNS_QFP_DEBUG_MSG_MAX_SIZE_V01];
  /**<   response data if any */
}sns_qfp_debug_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; BG-E normalization vector */
typedef struct {

  /* Mandatory */
  uint32_t item_id;
  /**<   ID of the basis normalization vector to be uploaded */

  /* Mandatory */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[SNS_QFP_FFD_BASIS_DATA_MAX_CHUNK_SIZE_V01];
  /**<   data to write */
}sns_qfp_ffd_basis_norm_vector_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; BG-E normalization vector */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
}sns_qfp_ffd_basis_norm_vector_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Request Message; Controls SSC dynamic polling. Can be high/low */
typedef struct {

  /* Mandatory */
  uint32_t state;
  /**<   State of the polling - can be 1 for high and 0 for low */
}sns_qfp_ffd_change_polling_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_QFP_SVC_qmi_messages
    @{
  */
/** Response Message; Controls SSC dynamic polling. Can be high/low */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;
}sns_qfp_ffd_change_polling_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_SNS_COMMON_CANCEL_V01
//#define REMOVE_SNS_COMMON_VERSION_V01
//#define REMOVE_SNS_QFP_CLOSE_V01
//#define REMOVE_SNS_QFP_DEBUG_V01
//#define REMOVE_SNS_QFP_FFD_BASIS_DATA_CLOSE_V01
//#define REMOVE_SNS_QFP_FFD_BASIS_DATA_CREATE_V01
//#define REMOVE_SNS_QFP_FFD_BASIS_DATA_WRITE_V01
//#define REMOVE_SNS_QFP_FFD_BASIS_NORM_VECTOR_V01
//#define REMOVE_SNS_QFP_FFD_CHANGE_POLLING_V01
//#define REMOVE_SNS_QFP_FINGER_DETECT_DISABLE_V01
//#define REMOVE_SNS_QFP_FINGER_DETECT_ENABLE_V01
//#define REMOVE_SNS_QFP_FINGER_DETECT_IND_V01
//#define REMOVE_SNS_QFP_KEEP_ALIVE_V01
//#define REMOVE_SNS_QFP_OPEN_V01
//#define REMOVE_SNS_QFP_SIMPLE_ITEM_WRITE_V01

/*Service Message Definition*/
/** @addtogroup SNS_QFP_SVC_qmi_msg_ids
    @{
  */
#define SNS_QFP_CANCEL_REQ_V01 0x0000
#define SNS_QFP_CANCEL_RESP_V01 0x0000
#define SNS_QFP_VERSION_REQ_V01 0x0001
#define SNS_QFP_VERSION_RESP_V01 0x0001
#define SNS_QFP_OPEN_REQ_V01 0x0020
#define SNS_QFP_OPEN_RESP_V01 0x0020
#define SNS_QFP_CLOSE_REQ_V01 0x0021
#define SNS_QFP_CLOSE_RESP_V01 0x0021
#define SNS_QFP_KEEP_ALIVE_REQ_V01 0x0022
#define SNS_QFP_KEEP_ALIVE_RESP_V01 0x0022
#define SNS_QFP_FINGER_DETECT_ENABLE_REQ_V01 0x0023
#define SNS_QFP_FINGER_DETECT_ENABLE_RESP_V01 0x0023
#define SNS_QFP_FINGER_DETECT_DISABLE_REQ_V01 0x0024
#define SNS_QFP_FINGER_DETECT_DISABLE_RESP_V01 0x0024
#define SNS_QFP_FINGER_DETECT_IND_V01 0x0025
#define SNS_QFP_SIMPLE_ITEM_WRITE_REQ_V01 0x0026
#define SNS_QFP_SIMPLE_ITEM_WRITE_RESP_V01 0x0026
#define SNS_QFP_FFD_BASIS_DATA_CREATE_REQ_V01 0x0027
#define SNS_QFP_FFD_BASIS_DATA_CREATE_RESP_V01 0x0027
#define SNS_QFP_FFD_BASIS_DATA_WRITE_REQ_V01 0x0028
#define SNS_QFP_FFD_BASIS_DATA_WRITE_RESP_V01 0x0028
#define SNS_QFP_FFD_BASIS_DATA_CLOSE_REQ_V01 0x0029
#define SNS_QFP_FFD_BASIS_DATA_CLOSE_RESP_V01 0x0029
#define SNS_QFP_DEBUG_REQ_V01 0x002A
#define SNS_QFP_DEBUG_RESP_V01 0x002A
#define SNS_QFP_FFD_BASIS_NORM_VECTOR_REQ_V01 0x002B
#define SNS_QFP_FFD_BASIS_NORM_VECTOR_RESP_V01 0x002B
#define SNS_QFP_FFD_CHANGE_POLLING_REQ_V01 0x002C
#define SNS_QFP_FFD_CHANGE_POLLING_RESP_V01 0x002C
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_QFP_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_QFP_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define SNS_QFP_SVC_get_service_object_v01( ) \
          SNS_QFP_SVC_get_service_object_internal_v01( \
            SNS_QFP_SVC_V01_IDL_MAJOR_VERS, SNS_QFP_SVC_V01_IDL_MINOR_VERS, \
            SNS_QFP_SVC_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

