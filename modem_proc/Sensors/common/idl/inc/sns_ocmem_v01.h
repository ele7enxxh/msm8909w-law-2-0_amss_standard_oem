#ifndef SNS_OCMEM_SVC_SERVICE_H
#define SNS_OCMEM_SVC_SERVICE_H
/**
  @file sns_ocmem_v01.h
  
  @brief This is the public header file which defines the SNS_OCMEM_SVC service Data structures.

  This header file defines the types and structures that were defined in 
  SNS_OCMEM_SVC. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved. 
 QUALCOMM Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/idl/inc/sns_ocmem_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.5 
   It was generated on: Mon Aug 27 2012
   From IDL File: sns_ocmem_v01.idl */

/** @defgroup SNS_OCMEM_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_OCMEM_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_OCMEM_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_OCMEM_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_OCMEM_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_OCMEM_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_OCMEM_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
//#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_OCMEM_SVC_qmi_version 
    @{ 
  */ 
/** Major Version Number of the IDL used to generate this file */
#define SNS_OCMEM_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_OCMEM_SVC_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_OCMEM_SVC_V01_IDL_TOOL_VERS 0x05
/** Maximum Defined Message ID */
#define SNS_OCMEM_SVC_V01_MAX_MESSAGE_ID 0x0003;
/** 
    @} 
  */


/** @addtogroup SNS_OCMEM_SVC_qmi_consts 
    @{ 
  */

/** 

 Maximum number of segments that may be mapped from DDR to OCMEM  */
#define SNS_OCMEM_MAX_NUM_SEG_V01 16

/**  Maximum size of the ocmem_vectors structure  */
#define SNS_OCMEM_MAX_VECTORS_SIZE_V01 512

/** ============================================================================
 Service IDs
============================================================================

 Sensor OCMEM Service  */
#define SNS_OCMEM_SVC_ID_V01 0

/** ============================================================================
 Structures
============================================================================ */
#define SNS_RESULT_OCMEM_SUCCESS_V01 0
#define SNS_RESULT_OCMEM_FAILURE_V01 1
/**
    @}
  */

/** @addtogroup SNS_OCMEM_SVC_qmi_aggregates
    @{
  */

typedef struct {

  /*  This structure shall be the first element of every response message  */
  uint8_t sns_result_t;
  /**<   0 == SUCCESS; 1 == FAILURE
     A result of FAILURE indicates that that any data contained in the response
     should not be used (other than sns_err_t, to determine the type of error */

  uint8_t sns_err_t;
  /**<   See sns_ocmem_error_e in ocmem_sensors.h */
}sns_ocmem_common_resp_s_v01;  /* Type */


/**
    @}
  */

/** @addtogroup SNS_OCMEM_SVC_qmi_aggregates
    @{
  */


typedef struct {
  uint64_t start_address;
  /**<   Physical start address of this segment */
  /*  This structure represents a single memory region that must be mapped from
      DDR to OCMEM.  */
   uint32_t size;
  /**<   Size (in bytes) of this segment */
  
  uint16_t type;
  /**<   1 == Read only; 2 == Read/Write Data */

} __attribute__((packed,aligned (1))) sns_mem_segment_s_v01;  /* Type */


/**
    @}
  */

/*
 * sns_ocmem_common_cancel_req_msg is empty
 * typedef struct {
 * }sns_ocmem_common_cancel_req_msg_v01;
 */

/** @addtogroup SNS_OCMEM_SVC_qmi_messages
    @{
  */
/** Response Message; This command cancel all requests from a client to any sensor service

    This shall be the first message of any sensor service. */


typedef struct {

  /* Mandatory */
  sns_ocmem_common_resp_s_v01 resp;
} sns_ocmem_common_cancel_resp_msg_v01;  /* Message */

/**
    @}
  */

/*
 * sns_ocmem_common_version_req_msg is empty
 * typedef struct {
 * }sns_ocmem_common_version_req_msg_v01;
 */

/** @addtogroup SNS_OCMEM_SVC_qmi_messages
    @{
  */
/** Request Message; This command requests the version of the sensor service

    This shall be the second message of any sensor service. */

typedef struct {

  /* Mandatory */
  sns_ocmem_common_resp_s_v01 resp;

  uint32_t interface_version_number;
  /**<   The version number of the interface supported by the service */
  
  uint16_t max_message_id;
  /**<   The maximum message ID supported by this service */
  
   /* Optional */
  uint8_t interface_version_number_valid;  /**< Must be set to true if interface_version_number is being passed */
 /* Optional */
  uint8_t max_message_id_valid;  /**< Must be set to true if max_message_id is being passed */ 
  
} sns_ocmem_common_version_resp_msg_v01 ;  /* Message */

/**
    @}
  */

/*
 * sns_ocmem_phys_addr_req_msg is empty
 * typedef struct {
 * }sns_ocmem_phys_addr_req_msg_v01;
 */

/** @addtogroup SNS_OCMEM_SVC_qmi_messages
    @{
  */
/** Response Message; This command returns the indices into DDR which will be mapped to OCMEM */


typedef struct {

  /* Mandatory */
  sns_ocmem_common_resp_s_v01 resp;

  /* Optional */
  uint32_t segments_len;  /**< Must be set to # of elements in segments */
  
  sns_mem_segment_s_v01 segments[SNS_OCMEM_MAX_NUM_SEG_V01];
  
  
  uint8_t segments_valid;  /**< Must be set to true if segments is being passed */

  /**<   Maximum number of segments that may be mapped from DDR to OCMEM */
} __attribute__((packed,aligned (1))) sns_ocmem_phys_addr_resp_msg_v01;  /* Message */


/**
    @}
  */

/** @addtogroup SNS_OCMEM_SVC_qmi_messages
    @{
  */
/** Indication Message; This command returns the indices into DDR which will be mapped to OCMEM */
typedef struct {

  /* Mandatory */
  uint16_t num_clients;
  /**<   Number of participating active clients on the ADSP */
} __attribute__((packed,aligned (1))) sns_ocmem_has_client_ind_msg_v01;  /* Message */

/**
    @}
  */

/** @addtogroup SNS_OCMEM_SVC_qmi_messages
    @{
  */
/** Request Message; This command indicates to the DSPS that OCMEM mapping or unmapping
      has just concluded, and that it should peform bandwidth voting. */
	  

typedef struct {
    /* Mandatory */
  uint8_t is_map;
    /* Optional */
  uint8_t vectors_valid;  /**< Must be set to true if vectors is being passed */
 
  uint32_t vectors_len;  /**< Must be set to # of elements in vectors */
  uint8_t vectors[SNS_OCMEM_MAX_VECTORS_SIZE_V01];
  /**<   True if Mapping has just completed; false if unmapping just completed */ 
} __attribute__((packed,aligned (1))) sns_ocmem_bw_vote_req_msg_v01;  /* Message */

/**
    @}
  */

/** @addtogroup SNS_OCMEM_SVC_qmi_messages
    @{
  */
/** Response Message; This command indicates to the DSPS that OCMEM mapping or unmapping
      has just concluded, and that it should peform bandwidth voting. */
  
typedef struct {

  /* Mandatory */
  sns_ocmem_common_resp_s_v01 resp;
} sns_ocmem_bw_vote_resp_msg_v01;  /* Message */


/**
    @}
  */

/** @addtogroup SNS_OCMEM_SVC_qmi_messages
    @{
  */
/** Indication Message; This command indicates to the DSPS that OCMEM mapping or unmapping
      has just concluded, and that it should peform bandwidth voting. */
  
typedef struct {

  /* Mandatory */
  uint8_t is_vote_on;
  /**<   If the ADSP just voted for, or took away its vote for OCMEM Bandwidth */
} __attribute__((packed,aligned (1))) sns_ocmem_bw_vote_ind_msg_v01;  /* Message */

/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup SNS_OCMEM_SVC_qmi_msg_ids
    @{
  */
#define SNS_OCMEM_CANCEL_REQ_V01 0x0000
#define SNS_OCMEM_CANCEL_RESP_V01 0x0000
#define SNS_OCMEM_VERSION_REQ_V01 0x0001
#define SNS_OCMEM_VERSION_RESP_V01 0x0001
#define SNS_OCMEM_PHYS_ADDR_REQ_V01 0x0002
#define SNS_OCMEM_PHYS_ADDR_RESP_V01 0x0002
#define SNS_OCMEM_HAS_CLIENT_IND_V01 0x0002
#define SNS_OCMEM_BW_VOTE_REQ_V01 0x0003
#define SNS_OCMEM_BW_VOTE_RESP_V01 0x0003
#define SNS_OCMEM_BW_VOTE_IND_V01 0x0003
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_OCMEM_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_OCMEM_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define SNS_OCMEM_SVC_get_service_object_v01( ) \
          SNS_OCMEM_SVC_get_service_object_internal_v01( \
            SNS_OCMEM_SVC_V01_IDL_MAJOR_VERS, SNS_OCMEM_SVC_V01_IDL_MINOR_VERS, \
            SNS_OCMEM_SVC_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

