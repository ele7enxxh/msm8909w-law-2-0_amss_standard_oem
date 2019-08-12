#ifndef DHMS_SERVICE_01_H
#define DHMS_SERVICE_01_H
/**
  @file dynamic_heap_memory_sharing_v01.h

  @brief This is the public header file which defines the dhms service Data structures.

  This header file defines the types and structures that were defined in
  dhms. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/dhms/api/dynamic_heap_memory_sharing_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.7 
   It was generated on: Mon May 19 2014 (Spin 0)
   From IDL File: dynamic_heap_memory_sharing_v01.idl */

/** @defgroup dhms_qmi_consts Constant values defined in the IDL */
/** @defgroup dhms_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup dhms_qmi_enums Enumerated types used in QMI messages */
/** @defgroup dhms_qmi_messages Structures sent as QMI messages */
/** @defgroup dhms_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup dhms_qmi_accessor Accessor for QMI service object */
/** @defgroup dhms_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup dhms_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define DHMS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define DHMS_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define DHMS_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define DHMS_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup dhms_qmi_consts 
    @{ 
  */
#define MAX_ARR_CNT_V01 64
/**
    @}
  */

/** @addtogroup dhms_qmi_enums
    @{
  */
typedef enum {
  DHMS_MEM_BLOCK_ALIGN_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DHMS_MEM_BLOCK_ALIGN_2_V01 = 0, /**<  Align allocated memory by 2 bytes  */
  DHMS_MEM_BLOCK_ALIGN_4_V01 = 1, /**<  Align allocated memory by 4 bytes  */
  DHMS_MEM_BLOCK_ALIGN_8_V01 = 2, /**<  Align allocated memory by 8 bytes  */
  DHMS_MEM_BLOCK_ALIGN_16_V01 = 3, /**<  Align allocated memory by 16 bytes  */
  DHMS_MEM_BLOCK_ALIGN_32_V01 = 4, /**<  Align allocated memory by 32 bytes  */
  DHMS_MEM_BLOCK_ALIGN_64_V01 = 5, /**<  Align allocated memory by 64 bytes  */
  DHMS_MEM_BLOCK_ALIGN_128_V01 = 6, /**<  Align allocated memory by 128 bytes  */
  DHMS_MEM_BLOCK_ALIGN_256_V01 = 7, /**<  Align allocated memory by 256 bytes  */
  DHMS_MEM_BLOCK_ALIGN_512_V01 = 8, /**<  Align allocated memory by 512 bytes  */
  DHMS_MEM_BLOCK_ALIGN_1K_V01 = 9, /**<  Align allocated memory by 1024 bytes  */
  DHMS_MEM_BLOCK_ALIGN_2K_V01 = 10, /**<  Align allocated memory by 2048 bytes  */
  DHMS_MEM_BLOCK_ALIGN_4K_V01 = 11, /**<  Align allocated memory by 4096 bytes  */
  DHMS_MEM_BLOCK_ALIGN_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dhms_mem_block_align_enum_v01;
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Request Message; Allocates a physically continous 
           memory block from the server memory subsystem. */
typedef struct {

  /* Mandatory */
  /*  Memory Block Size */
  uint32_t num_bytes;
  /**<   The number of bytes to be allocated on the server.
  */

  /* Optional */
  /*  Block Alignment */
  uint8_t block_alignment_valid;  /**< Must be set to true if block_alignment is being passed */
  dhms_mem_block_align_enum_v01 block_alignment;
  /**<   Block alignment for the memory block to be allocated. Values: \n
      - DHMS_MEM_BLOCK_ALIGN_2 (0) --  Align allocated memory by 2 bytes 
      - DHMS_MEM_BLOCK_ALIGN_4 (1) --  Align allocated memory by 4 bytes 
      - DHMS_MEM_BLOCK_ALIGN_8 (2) --  Align allocated memory by 8 bytes 
      - DHMS_MEM_BLOCK_ALIGN_16 (3) --  Align allocated memory by 16 bytes 
      - DHMS_MEM_BLOCK_ALIGN_32 (4) --  Align allocated memory by 32 bytes 
      - DHMS_MEM_BLOCK_ALIGN_64 (5) --  Align allocated memory by 64 bytes 
      - DHMS_MEM_BLOCK_ALIGN_128 (6) --  Align allocated memory by 128 bytes 
      - DHMS_MEM_BLOCK_ALIGN_256 (7) --  Align allocated memory by 256 bytes 
      - DHMS_MEM_BLOCK_ALIGN_512 (8) --  Align allocated memory by 512 bytes 
      - DHMS_MEM_BLOCK_ALIGN_1K (9) --  Align allocated memory by 1024 bytes 
      - DHMS_MEM_BLOCK_ALIGN_2K (10) --  Align allocated memory by 2048 bytes 
      - DHMS_MEM_BLOCK_ALIGN_4K (11) --  Align allocated memory by 4096 bytes 
 */
}dhms_mem_alloc_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Response Message; Allocates a physically continous 
           memory block from the server memory subsystem. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_result_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
  */

  /* Optional */
  /*  Memory Block Handle */
  uint8_t handle_valid;  /**< Must be set to true if handle is being passed */
  uint64_t handle;
  /**<   Physical address of the memory allocated on the HLOS.
   */

  /* Optional */
  /*  Memory block size */
  uint8_t num_bytes_valid;  /**< Must be set to true if num_bytes is being passed */
  uint32_t num_bytes;
  /**<   Number of bytes actually allocated for the request. 
        This value can be smaller than the size requested in 
        QMI_DHMS_MEM_ALLOC_REQ.
   */
}dhms_mem_alloc_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dhms_qmi_enums
    @{
  */
typedef enum {
  DHMS_MEM_PROC_ID_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DHMS_MEM_PROC_MPSS_V01 = 0, /**<  Request from MPSS processor  */
  DHMS_MEM_PROC_ADSP_V01 = 1, /**<  Request from ADSP processor  */
  DHMS_MEM_PROC_WCNSS_V01 = 2, /**<  Request from WCNSS processor  */
  DHMS_MEM_PROC_ID_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dhms_mem_proc_id_v01;
/**
    @}
  */

/** @addtogroup dhms_qmi_enums
    @{
  */
typedef enum {
  DHMS_MEM_CLIENT_ID_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  DHMS_MEM_CLIENT_GPS_V01 = 0, /**<  Request from GPS Client    */
  DHMS_MEM_CLIENT_ID_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}dhms_mem_client_id_v01;
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Request Message; Allocates a physically memory block from the server memory subsystem. */
typedef struct {

  /* Mandatory */
  /*  Memory Block Size */
  uint32_t num_bytes;
  /**<   The number of bytes to be allocated on the server.
  */

  /* Mandatory */
  /*  Client ID */
  dhms_mem_client_id_v01 client_id;
  /**<   Client id is used to identify the client requesting allocation.
  */

  /* Mandatory */
  /*  Proc ID */
  dhms_mem_proc_id_v01 proc_id;
  /**<   Proc_id is used for identifying the subsystem.
  */

  /* Mandatory */
  /*  Sequence ID */
  uint32_t sequence_id;

  /* Optional */
  /*  alloc_contiguous */
  uint8_t alloc_contiguous_valid;  /**< Must be set to true if alloc_contiguous is being passed */
  uint8_t alloc_contiguous;
  /**<   alloc_contiguous is used to identify that clients are requesting 
       for contiguous or non contiguous memory, default is contiguous
	   0 = non contiguous else contiguous	   
  */

  /* Optional */
  /*  Block Alignment */
  uint8_t block_alignment_valid;  /**< Must be set to true if block_alignment is being passed */
  dhms_mem_block_align_enum_v01 block_alignment;
  /**<   Block alignment for the memory block to be allocated. Values: \n
      - DHMS_MEM_BLOCK_ALIGN_2 (0) --  Align allocated memory by 2 bytes 
      - DHMS_MEM_BLOCK_ALIGN_4 (1) --  Align allocated memory by 4 bytes 
      - DHMS_MEM_BLOCK_ALIGN_8 (2) --  Align allocated memory by 8 bytes 
      - DHMS_MEM_BLOCK_ALIGN_16 (3) --  Align allocated memory by 16 bytes 
      - DHMS_MEM_BLOCK_ALIGN_32 (4) --  Align allocated memory by 32 bytes 
      - DHMS_MEM_BLOCK_ALIGN_64 (5) --  Align allocated memory by 64 bytes 
      - DHMS_MEM_BLOCK_ALIGN_128 (6) --  Align allocated memory by 128 bytes 
      - DHMS_MEM_BLOCK_ALIGN_256 (7) --  Align allocated memory by 256 bytes 
      - DHMS_MEM_BLOCK_ALIGN_512 (8) --  Align allocated memory by 512 bytes 
      - DHMS_MEM_BLOCK_ALIGN_1K (9) --  Align allocated memory by 1024 bytes 
      - DHMS_MEM_BLOCK_ALIGN_2K (10) --  Align allocated memory by 2048 bytes 
      - DHMS_MEM_BLOCK_ALIGN_4K (11) --  Align allocated memory by 4096 bytes 
 */
}dhms_mem_alloc_generic_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dhms_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t phy_addr;

  uint32_t num_bytes;
}dhms_mem_alloc_addr_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Response Message; Allocates a physically memory block from the server memory subsystem. */
typedef struct {

  /* Mandatory */
  /*  Response Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */

  /* Optional */
  /*  Sequence ID */
  uint8_t sequence_id_valid;  /**< Must be set to true if sequence_id is being passed */
  uint32_t sequence_id;

  /* Optional */
  /*  Alloction information */
  uint8_t dhms_mem_alloc_addr_info_valid;  /**< Must be set to true if dhms_mem_alloc_addr_info is being passed */
  uint32_t dhms_mem_alloc_addr_info_len;  /**< Must be set to # of elements in dhms_mem_alloc_addr_info */
  dhms_mem_alloc_addr_info_type_v01 dhms_mem_alloc_addr_info[MAX_ARR_CNT_V01];
  /**<   Physical address and size of the memory allocated on the HLOS.
   */
}dhms_mem_alloc_generic_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Request Message; Frees the physically continous 
           memory block on the HLOS that was previously allocated. */
typedef struct {

  /* Mandatory */
  /*  Memory Block Handle */
  uint64_t handle;
  /**<   The physical address of the memory to be freed on the HLOS.
   */
}dhms_mem_free_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Response Message; Frees the physically continous 
           memory block on the HLOS that was previously allocated. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_result_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
  */
}dhms_mem_free_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Request Message; Frees the physical memory block on the HLOS that was previously allocated. */
typedef struct {

  /* Mandatory */
  /*  Alloction information */
  uint32_t dhms_mem_alloc_addr_info_len;  /**< Must be set to # of elements in dhms_mem_alloc_addr_info */
  dhms_mem_alloc_addr_info_type_v01 dhms_mem_alloc_addr_info[MAX_ARR_CNT_V01];
  /**<   Physical address and size of the memory allocated on the HLOS to be freed.
   */

  /* Optional */
  /*  Client ID */
  uint8_t client_id_valid;  /**< Must be set to true if client_id is being passed */
  dhms_mem_client_id_v01 client_id;

  /* Optional */
  /*  Proc ID */
  uint8_t proc_id_valid;  /**< Must be set to true if proc_id is being passed */
  dhms_mem_proc_id_v01 proc_id;
}dhms_mem_free_generic_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Response Message; Frees the physical memory block on the HLOS that was previously allocated. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */
}dhms_mem_free_generic_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Request Message; Queries the max size which client can allocate. */
typedef struct {

  /* Mandatory */
  /*  Client ID */
  dhms_mem_client_id_v01 client_id;

  /* Optional */
  /*  Proc ID */
  uint8_t proc_id_valid;  /**< Must be set to true if proc_id is being passed */
  dhms_mem_proc_id_v01 proc_id;
}dhms_mem_query_size_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup dhms_qmi_messages
    @{
  */
/** Response Message; Queries the max size which client can allocate. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described in
                          the error codes section of each message definition.
   */

  /* Optional */
  uint8_t size_valid;  /**< Must be set to true if size is being passed */
  uint32_t size;
}dhms_mem_query_size_resp_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup dhms_qmi_msg_ids
    @{
  */
#define QMI_DHMS_MEM_ALLOC_REQ_V01 0x0020
#define QMI_DHMS_MEM_ALLOC_RESP_V01 0x0020
#define QMI_DHMS_MEM_FREE_REQ_V01 0x0021
#define QMI_DHMS_MEM_FREE_RESP_V01 0x0021
#define QMI_DHMS_MEM_ALLOC_GENERIC_REQ_V01 0x0022
#define QMI_DHMS_MEM_ALLOC_GENERIC_RESP_V01 0x0022
#define QMI_DHMS_MEM_FREE_GENERIC_REQ_V01 0x0023
#define QMI_DHMS_MEM_FREE_GENERIC_RESP_V01 0x0023
#define QMI_DHMS_MEM_QUERY_SIZE_REQ_V01 0x0024
#define QMI_DHMS_MEM_QUERY_SIZE_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro dhms_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type dhms_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define dhms_get_service_object_v01( ) \
          dhms_get_service_object_internal_v01( \
            DHMS_V01_IDL_MAJOR_VERS, DHMS_V01_IDL_MINOR_VERS, \
            DHMS_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

