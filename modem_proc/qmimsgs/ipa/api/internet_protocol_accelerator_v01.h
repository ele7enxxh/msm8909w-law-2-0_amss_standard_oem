#ifndef IPA_SERVICE_01_H
#define IPA_SERVICE_01_H
/**
  @file internet_protocol_accelerator_v01.h

  @brief This is the public header file which defines the ipa service Data structures.

  This header file defines the types and structures that were defined in
  ipa. It contains the constant values defined, enums, structures,
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



  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/ipa/api/internet_protocol_accelerator_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2 
   It was generated on: Thu Jan  8 2015 (Spin 0)
   From IDL File: internet_protocol_accelerator_v01.idl */

/** @defgroup ipa_qmi_consts Constant values defined in the IDL */
/** @defgroup ipa_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup ipa_qmi_enums Enumerated types used in QMI messages */
/** @defgroup ipa_qmi_messages Structures sent as QMI messages */
/** @defgroup ipa_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup ipa_qmi_accessor Accessor for QMI service object */
/** @defgroup ipa_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"
#include "data_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ipa_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define IPA_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define IPA_V01_IDL_MINOR_VERS 0x07
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define IPA_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define IPA_V01_MAX_MESSAGE_ID 0x0029
/**
    @}
  */


/** @addtogroup ipa_qmi_consts 
    @{ 
  */
#define QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS_V01 2
#define QMI_IPA_IPFLTR_NUM_MEQ_32_EQNS_V01 2
#define QMI_IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS_V01 2
#define QMI_IPA_IPFLTR_NUM_MEQ_128_EQNS_V01 2
#define QMI_IPA_MAX_FILTERS_V01 64
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Registers for indications from a service. */
typedef struct {

  /* Optional */
  /*  Master Driver Initialization Completion */
  uint8_t master_driver_init_complete_valid;  /**< Must be set to true if master_driver_init_complete is being passed */
  uint8_t master_driver_init_complete;
  /**<   If set to TRUE, this field indicates that the client wants to receive 
         indications about the completion of the master driver's initialization
         sequence.

         Setting this field in the request message makes sense only when the 
         QMI_IPA_INDICATION_REGISTER_REQ is being originated from the modem
         driver.
    */
}ipa_indication_reg_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Registers for indications from a service. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_indication_reg_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_PLATFORM_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_PLATFORM_TYPE_INVALID_V01 = 0, /**<  Invalid platform identifier
    */
  QMI_IPA_PLATFORM_TYPE_TN_V01 = 1, /**<  Data card device
   */
  QMI_IPA_PLATFORM_TYPE_LE_V01 = 2, /**<  Data router device
   */
  QMI_IPA_PLATFORM_TYPE_MSM_ANDROID_V01 = 3, /**<  MSM device with Android HLOS
   */
  QMI_IPA_PLATFORM_TYPE_MSM_WINDOWS_V01 = 4, /**<  MSM device with Windows HLOS
   */
  QMI_IPA_PLATFORM_TYPE_MSM_QNX_V01 = 5, /**<  MSM device with QNX HLOS
   */
  IPA_PLATFORM_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_platform_type_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t modem_offset_start;
  /**<   Offset from the start of IPA shared memory from which the modem driver may 
       insert header table entries.
  */

  uint32_t modem_offset_end;
  /**<   Offset from the start of IPA shared memory beyond which the modem driver
       may not insert header table entries. The space available for the modem
       driver includes modem_offset_start and modem_offset_end.
  */
}ipa_hdr_tbl_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t modem_offset_start;
  /**<   Offset from the start of IPA shared memory from which the modem driver may
       insert header processing context table entries.
  */

  uint32_t modem_offset_end;
  /**<   Offset from the start of IPA shared memory beyond which the modem driver
       may not insert header proc table entries. The space available for the modem
       driver includes modem_offset_start and modem_offset_end.
  */
}ipa_hdr_proc_ctx_tbl_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t modem_offset_start;
  /**<   Offset from the start of IPA shared memory from which the modem driver may
       insert compression/decompression command entries.
  */

  uint32_t modem_offset_end;
  /**<   Offset from the start of IPA shared memory beyond which the modem driver
       may not insert compression/decompression command entries. The space available
       for the modem driver includes modem_offset_start and modem_offset_end.
  */
}ipa_zip_tbl_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t route_tbl_start_addr;
  /**<   Identifies the start of the routing table. Denotes the offset from the 
       start of the IPA shared memory.
  */

  uint32_t num_indices;
  /**<   Number of indices (starting from 0) that are allocated to the modem.
       The number indicated here is also included in the allocation. The value
       of num_indices must not exceed 31 (5 bits used to specify the routing
       table index), unless there is a change in the hardware.
  */
}ipa_route_tbl_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t block_start_addr;
  /**<   Identifies the start of the memory block allocated for the modem.
       Denotes the offset from the start of the IPA shared memory.
  */

  uint32_t size;
  /**<   Size of the block allocated for the modem driver.
  */
}ipa_modem_mem_info_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests the modem IPA driver to perform initialization. */
typedef struct {

  /* Optional */
  /*  Platform Information */
  uint8_t platform_type_valid;  /**< Must be set to true if platform_type is being passed */
  ipa_platform_type_enum_v01 platform_type;
  /**<   Provides information about the platform. Values: \n
      - QMI_IPA_PLATFORM_TYPE_INVALID (0) --  Invalid platform identifier
   
      - QMI_IPA_PLATFORM_TYPE_TN (1) --  Data card device
  
      - QMI_IPA_PLATFORM_TYPE_LE (2) --  Data router device
  
      - QMI_IPA_PLATFORM_TYPE_MSM_ANDROID (3) --  MSM device with Android HLOS
  
      - QMI_IPA_PLATFORM_TYPE_MSM_WINDOWS (4) --  MSM device with Windows HLOS
  
      - QMI_IPA_PLATFORM_TYPE_MSM_QNX (5) --  MSM device with QNX HLOS
  
 */

  /* Optional */
  /*  Header Table Information */
  uint8_t hdr_tbl_info_valid;  /**< Must be set to true if hdr_tbl_info is being passed */
  ipa_hdr_tbl_info_type_v01 hdr_tbl_info;

  /* Optional */
  /*  IPv4 Routing Table Information */
  uint8_t v4_route_tbl_info_valid;  /**< Must be set to true if v4_route_tbl_info is being passed */
  ipa_route_tbl_info_type_v01 v4_route_tbl_info;

  /* Optional */
  /*  IPv6 Routing Table Information */
  uint8_t v6_route_tbl_info_valid;  /**< Must be set to true if v6_route_tbl_info is being passed */
  ipa_route_tbl_info_type_v01 v6_route_tbl_info;

  /* Optional */
  /*  IPv4 Filter Table Start Address */
  uint8_t v4_filter_tbl_start_addr_valid;  /**< Must be set to true if v4_filter_tbl_start_addr is being passed */
  uint32_t v4_filter_tbl_start_addr;
  /**<   Identifies the starting address of the IPv4 filter table.
       Denotes the offset from the start of the IPA shared memory.
  */

  /* Optional */
  /*  IPv6 Filter Table Start Address */
  uint8_t v6_filter_tbl_start_addr_valid;  /**< Must be set to true if v6_filter_tbl_start_addr is being passed */
  uint32_t v6_filter_tbl_start_addr;
  /**<   Identifies the starting address of the IPv6 filter table.
       Denotes the offset from the start of the IPA shared memory.
  */

  /* Optional */
  /*  Modem Memory Block */
  uint8_t modem_mem_info_valid;  /**< Must be set to true if modem_mem_info is being passed */
  ipa_modem_mem_info_type_v01 modem_mem_info;
  /**<   \n Provides information about the start address (physical address) and the
       size of the memory block that is allocated to the modem driver.
  */

  /* Optional */
  /*  Destination Endpoint for Control Commands from the Modem */
  uint8_t ctrl_comm_dest_end_pt_valid;  /**< Must be set to true if ctrl_comm_dest_end_pt is being passed */
  uint32_t ctrl_comm_dest_end_pt;
  /**<   Provides information about the destination endpoint on the application
       processor to which the modem driver can send control commands. \n
       The value of this parameter cannot exceed 19, since the IPA only supports 20
       endpoints.
  */

  /* Optional */
  /*  Modem Bootup Information */
  uint8_t is_ssr_bootup_valid;  /**< Must be set to true if is_ssr_bootup is being passed */
  uint8_t is_ssr_bootup;
  /**<   Specifies whether the modem is booting up after a modem only subsystem 
       restart. This lets the modem driver know that it is not required
       to re-initialize some of the hardware blocks because IPA has not been reset 
       since the previous initialization.
  */

  /* Optional */
  /*  Header Processing Context Table Information */
  uint8_t hdr_proc_ctx_tbl_info_valid;  /**< Must be set to true if hdr_proc_ctx_tbl_info is being passed */
  ipa_hdr_proc_ctx_tbl_info_type_v01 hdr_proc_ctx_tbl_info;
  /**<   Provides information about the header processing context table.
  */

  /* Optional */
  /*  Compression Decompression Table Information */
  uint8_t zip_tbl_info_valid;  /**< Must be set to true if zip_tbl_info is being passed */
  ipa_zip_tbl_info_type_v01 zip_tbl_info;
  /**<   Provides information about the header processing context table.
  */
}ipa_init_modem_driver_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests the modem IPA driver to perform initialization. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Destination Endpoint for Control Commands from the Master Driver */
  uint8_t ctrl_comm_dest_end_pt_valid;  /**< Must be set to true if ctrl_comm_dest_end_pt is being passed */
  uint32_t ctrl_comm_dest_end_pt;
  /**<   Provides information about the destination endpoint on the modem
       processor to which the master driver can send control commands.

       The value of this parameter cannot exceed 19, since IPA only supports 20
       endpoints.

       The server does not include this field if the response indicates a failure.
  */

  /* Optional */
  /*  Default Endpoint */
  uint8_t default_end_pt_valid;  /**< Must be set to true if default_end_pt is being passed */
  uint32_t default_end_pt;
  /**<   Provides information about the default endpoint. The master driver may 
       or may not set the register in the hardware with this value.

       The value of this parameter cannot exceed 19, since the IPA only supports 20
       endpoints.

       The server does not include this field if the response indicates a failure.
  */
}ipa_init_modem_driver_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Indication Message; Indication sent to the modem IPA driver from the master IPA driver
           regarding the completion of the initialization. */
typedef struct {

  /* Mandatory */
  /*  Master Driver Initialization Completion Status */
  qmi_response_type_v01 master_driver_init_status;
  /**<   Indicates the status of the initialization. If everything went as expected,
       this field is set to SUCCESS. ERROR is set otherwise.
  */
}ipa_master_driver_init_complt_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset from the Internet Header Length (IHL).
  */

  uint16_t range_low;
  /**<   Lower bound of the range.
  */

  uint16_t range_high;
  /**<   Upper bound of the range.
  */
}ipa_ipfltr_range_eq_16_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset either from the IHL or from the start of the IP packet.
       This depends on the equation in which this structure is used.
  */

  uint32_t mask;
  /**<   Mask that must be used in the comparison. The field is
       ANDed with the mask and compared against the value.
  */

  uint32_t value;
  /**<   32-bit value that is to be used in the comparison.
  */
}ipa_ipfltr_mask_eq_32_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset in the packet.
  */

  uint16_t value;
  /**<   16-bit value that is to be used in the comparison.
  */
}ipa_ipfltr_eq_16_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset in the packet.
  */

  uint32_t value;
  /**<   32-bit value that is to be used in the comparison.
  */
}ipa_ipfltr_eq_32_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t offset;
  /**<   Offset in the packet.
  */

  uint8_t mask[16];
  /**<   Mask that must be used in the comparison. The field is
       ANDed with the mask and compared against the value.
  */

  uint8_t value[16];
  /**<   128-bit value that is to be used in the comparison.
  */
}ipa_ipfltr_mask_eq_128_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t rule_eq_bitmap;
  /**<   16-bit bitmask to indicate how many equations are valid in this rule.
         Equations are the building blocks of a filter rule. The receiver uses
         this value to check if it has contructed the rule correctly out of the
         TLVs in the message. This is a way to build in redundancy.
    */

  uint8_t tos_eq_present;
  /**<   Specifies whether a type of service check rule is present.
    */

  uint8_t tos_eq;
  /**<   Value to check against the type of service (IPv4) field.
    */

  uint8_t protocol_eq_present;
  /**<   Specifies whether a protocol check rule is present.
    */

  uint8_t protocol_eq;
  /**<   Value to check against the protocol field.
    */

  uint8_t num_ihl_offset_range_16;
  /**<   Number of 16-bit range check rules at the location determined by
         the IP header length plus a specified offset in this rule.
         See the definition of the ipa_ipfltr_range_eq_16_type for better 
         understanding.

         The value of this field cannot exceed QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS,
         which is set as 2.
    */

  ipa_ipfltr_range_eq_16_type_v01 ihl_offset_range_16[QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS_V01];
  /**<   Array of the registered IP header length offset 16-bit range check 
         rules. 
    */

  uint8_t num_offset_meq_32;
  /**<   Number of 32-bit masked comparison rules present in this rule.
    */

  ipa_ipfltr_mask_eq_32_type_v01 offset_meq_32[QMI_IPA_IPFLTR_NUM_MEQ_32_EQNS_V01];
  /**<   Array of all the possible 32-bit masked comparison rules in this rule.
    */

  uint8_t tc_eq_present;
  /**<   Specifies whether the traffic class rule is present in this rule.
    */

  uint8_t tc_eq;
  /**<   Value against which the IPv4 traffic class field must be checked.
    */

  uint8_t flow_eq_present;
  /**<   Specifies if the "flow equals" rule is present in this rule.
    */

  uint32_t flow_eq;
  /**<   Value against which the IPv6 flow field must be checked.
    */

  uint8_t ihl_offset_eq_16_present;
  /**<   Specifies whether a 16-bit comparison is required at the location in
         the packet determined by (IHL + specified offset).
    */

  ipa_ipfltr_eq_16_type_v01 ihl_offset_eq_16;
  /**<   16-bit comparison equation.
    */

  uint8_t ihl_offset_eq_32_present;
  /**<   Specifies whether a 32-bit comparison is required at the location in
         the packet determined by (IHL + specified offset).
    */

  ipa_ipfltr_eq_32_type_v01 ihl_offset_eq_32;
  /**<   32-bit comparison equation.
    */

  uint8_t num_ihl_offset_meq_32;
  /**<   Number of 32-bit masked comparison equations in this rule. The
         location of the packet to be compared is determined by (IHL +
         the specified offset).
    */

  ipa_ipfltr_mask_eq_32_type_v01 ihl_offset_meq_32[QMI_IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS_V01];
  /**<   Array of 32-bit masked comparison equations.
    */

  uint8_t num_offset_meq_128;
  /**<   Number of 128-bit comparison equations in this rule. 
    */

  ipa_ipfltr_mask_eq_128_type_v01 offset_meq_128[QMI_IPA_IPFLTR_NUM_MEQ_128_EQNS_V01];
  /**<   Array of 128-bit comparison equations. The location in the packet is
         determined by the specified offset.
    */

  uint8_t metadata_meq32_present;
  /**<   Indicates whether the 32-bit masked comparison equation
         is present. The comparison is done against the metadata in the IPA.
         Metadata can either be extracted from the packet header or from the
         metadata register.
    */

  ipa_ipfltr_mask_eq_32_type_v01 metadata_meq32;
  /**<   Metadata 32-bit masked comparison equation.
    */

  uint8_t ipv4_frag_eq_present;
  /**<   Specifies whether the IPv4 fragment equation is present in this rule.
    */
}ipa_filter_rule_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_IP_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_IP_TYPE_INVALID_V01 = 0, /**<  Invalid IP type identifier
   */
  QMI_IPA_IP_TYPE_V4_V01 = 1, /**<  IPv4 type
   */
  QMI_IPA_IP_TYPE_V6_V01 = 2, /**<  IPv6 type
   */
  QMI_IPA_IP_TYPE_V4V6_V01 = 3, /**<  Applies to both IP types
   */
  IPA_IP_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_ip_type_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_FILTER_ACTION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_FILTER_ACTION_INVALID_V01 = 0, /**<  Invalid action on a filter hit
   */
  QMI_IPA_FILTER_ACTION_SRC_NAT_V01 = 1, /**<  Pass a packet to the NAT block for the source NAT
   */
  QMI_IPA_FILTER_ACTION_DST_NAT_V01 = 2, /**<  Pass a packet to the NAT block for the destination NAT
   */
  QMI_IPA_FILTER_ACTION_ROUTING_V01 = 3, /**<  Pass a packet to the routing block
   */
  QMI_IPA_FILTER_ACTION_EXCEPTION_V01 = 4, /**<  Treat a packet as an exception and send it to the exception pipe
   */
  IPA_FILTER_ACTION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_filter_action_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t filter_spec_identifier;
  /**<   Filter specification in the list of filter 
         specifications sent from the client. This field is applicable only in
         the filter installation request and response.
    */

  ipa_ip_type_enum_v01 ip_type;
  /**<   IP type for which this rule is applicable.
 The driver must identify the filter table (v6 or v4), and this
 field is essential for that. Values: \n
      - QMI_IPA_IP_TYPE_INVALID (0) --  Invalid IP type identifier
  
      - QMI_IPA_IP_TYPE_V4 (1) --  IPv4 type
  
      - QMI_IPA_IP_TYPE_V6 (2) --  IPv6 type
  
      - QMI_IPA_IP_TYPE_V4V6 (3) --  Applies to both IP types
  
 */

  ipa_filter_rule_type_v01 filter_rule;
  /**<   Rules in the filter specification. These rules are the
         ones that are matched against fields in the packet.
    */

  ipa_filter_action_enum_v01 filter_action;
  /**<   Action to be taken when a filter match occurs.
 The remote side must install this information into the hardware along
 with the filter equations. Values:
      - QMI_IPA_FILTER_ACTION_INVALID (0) --  Invalid action on a filter hit
  
      - QMI_IPA_FILTER_ACTION_SRC_NAT (1) --  Pass a packet to the NAT block for the source NAT
  
      - QMI_IPA_FILTER_ACTION_DST_NAT (2) --  Pass a packet to the NAT block for the destination NAT
  
      - QMI_IPA_FILTER_ACTION_ROUTING (3) --  Pass a packet to the routing block
  
      - QMI_IPA_FILTER_ACTION_EXCEPTION (4) --  Treat a packet as an exception and send it to the exception pipe
  
 */

  uint8_t is_routing_table_index_valid;
  /**<   Specifies whether the routing table index is present. If the
         action is QMI_IPA_FILTER_ACTION_EXCEPTION, this parameter need not be
         provided.
    */

  uint32_t route_table_index;
  /**<   Routing table index used to route the packets if the filter rule is hit.
    */

  uint8_t is_mux_id_valid;
  /**<   Specifies whether the multiplexer ID is valid.
    */

  uint32_t mux_id;
  /**<   QMAP Mux ID. As a part of the QMAP protocol,
         several data calls may be multiplexed over the same physical transport
         channel. This identifier is used to identify one such data call.
         The maximum value for this identifier is 255.
    */
}ipa_filter_spec_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Requests installation of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Optional */
  /*  Filter Specification  */
  uint8_t filter_spec_list_valid;  /**< Must be set to true if filter_spec_list is being passed */
  uint32_t filter_spec_list_len;  /**< Must be set to # of elements in filter_spec_list */
  ipa_filter_spec_type_v01 filter_spec_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of filters that must be installed in
         the hardware.

         The driver installing these rules must do so in the same order as
         specified in this list.
    */

  /* Optional */
  /*  Pipe Index to Install Rule */
  uint8_t source_pipe_index_valid;  /**< Must be set to true if source_pipe_index is being passed */
  uint32_t source_pipe_index;
  /**<   Pipe index to install the filter rule.
         The requester may not always know the pipe indices. If not specified,
         the receiver must install this rule on all pipes that it controls,
         through which data may be fed into the IPA.
    */

  /* Optional */
  /*  Total number of IPv4 filters in the filter spec list */
  uint8_t num_ipv4_filters_valid;  /**< Must be set to true if num_ipv4_filters is being passed */
  uint32_t num_ipv4_filters;
  /**<   Number of IPv4 rules included in filter spec list.
    */

  /* Optional */
  /*  Total number of IPv6 filters in the filter spec list */
  uint8_t num_ipv6_filters_valid;  /**< Must be set to true if num_ipv6_filters is being passed */
  uint32_t num_ipv6_filters;
  /**<   Number of IPv6 rules included in filter spec list.
    */

  /* Optional */
  /*  List of XLAT filter indices in the filter spec list */
  uint8_t xlat_filter_indices_list_valid;  /**< Must be set to true if xlat_filter_indices_list is being passed */
  uint32_t xlat_filter_indices_list_len;  /**< Must be set to # of elements in xlat_filter_indices_list */
  uint32_t xlat_filter_indices_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   List of XLAT filter indices.
         Filter rules at specified indices will need to be modified by the 
         receiver if the PDN is XLAT before installing them on the associated
         IPA consumer pipe.
    */
}ipa_install_fltr_rule_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t filter_spec_identifier;
  /**<   Specification in the list of filter 
         specifications sent from the client. This field is applicable only in
         the filter installation request and response.
    */

  uint32_t filter_handle;
  /**<   Identifies a rule in any subsequent message. This
         is a value that is provided by the server to the control point.
    */
}ipa_filter_rule_identifier_to_handle_map_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Requests installation of filtering rules in the hardware block on the remote side. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.
 Standard response type. Contains the following data members: \n
       - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
       - qmi_error_type  -- Error code. Possible error code values are described
                            in the error codes section of each message definition.
    */

  /* Optional */
  /*  Filter Handle List */
  uint8_t filter_handle_list_valid;  /**< Must be set to true if filter_handle_list is being passed */
  uint32_t filter_handle_list_len;  /**< Must be set to # of elements in filter_handle_list */
  ipa_filter_rule_identifier_to_handle_map_v01 filter_handle_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n List of handles returned to the control point. Each handle is
         mapped to the rule identifier that was specified in the request message.
         Any further reference to the rule is done using the filter handle.
    */
}ipa_install_fltr_rule_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t filter_handle;
  /**<   Handle to the remote client that requested the rule addition.
    */

  uint32_t filter_index;
  /**<   Denotes the location in a filter table where the filter 
         rule has been installed.
         The maximum value of this field is 64.
    */
}ipa_filter_handle_to_index_map_v01;  /* Type */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Notifies the remote driver of the installation of the filter rule
           supplied earlier by the remote driver. */
typedef struct {

  /* Mandatory */
  /*  Pipe Index */
  uint32_t source_pipe_index;
  /**<   Source pipe on which the filter rule has been installed
         or was attempted to be installed.
    */

  /* Mandatory */
  /*  Installation Status */
  qmi_result_type_v01 install_status;
  /**<   Installation status. If this indicates SUCCESS, other
         optional fields carry additional information. The driver installing
         the rules never installs a subset of the rules. The install status
         indicates the status of the installation of all the rules.
    */

  /* Mandatory */
  /*  List of Filter Indices */
  uint32_t filter_index_list_len;  /**< Must be set to # of elements in filter_index_list */
  ipa_filter_handle_to_index_map_v01 filter_index_list[QMI_IPA_MAX_FILTERS_V01];
  /**<   \n Provides the list of filter indices and the corresponding 
         filter handle. If the installation_status indicates a failure, the 
         filter indices must be set to a reserve index (255).
    */

  /* Optional */
  /*  Embedded Pipe Index */
  uint8_t embedded_pipe_index_valid;  /**< Must be set to true if embedded_pipe_index is being passed */
  uint32_t embedded_pipe_index;
  /**<   Denotes the embedded pipe number on which a call to the same Public
         Data Network (PDN) has been made. If this field is set, it denotes
         that this is a use case where PDN sharing is occuring. The embedded
         pipe is used to send data from the embedded client in the device.
    */

  /* Optional */
  /*  Retain Header Configuration */
  uint8_t retain_header_valid;  /**< Must be set to true if retain_header is being passed */
  uint8_t retain_header;
  /**<   Indicates whether the driver installing the rule has turned on 
         the retain_header bit. If this is TRUE, the header that is removed
         by the IPA is reinserted after the packet processing is completed.
    */

  /* Optional */
  /*  Embedded Call Mux ID */
  uint8_t embedded_call_mux_id_valid;  /**< Must be set to true if embedded_call_mux_id is being passed */
  uint32_t embedded_call_mux_id;
  /**<   Identifies one of the many calls that have been originated on the
         embedded pipe. This is how the PDN gateway to which traffic
         from the source pipe must flow is identified.
    */

  /* Optional */
  /*  Total number of IPv4 filters in the filter index list */
  uint8_t num_ipv4_filters_valid;  /**< Must be set to true if num_ipv4_filters is being passed */
  uint32_t num_ipv4_filters;
  /**<   Number of IPv4 rules included in filter index list.
    */

  /* Optional */
  /*  Total number of IPv6 filters in the filter index list */
  uint8_t num_ipv6_filters_valid;  /**< Must be set to true if num_ipv6_filters is being passed */
  uint32_t num_ipv6_filters;
  /**<   Number of IPv6 rules included in filter index list.
    */

  /* Optional */
  /*  Start index on IPv4 filters installed on source pipe */
  uint8_t start_ipv4_filter_idx_valid;  /**< Must be set to true if start_ipv4_filter_idx is being passed */
  uint32_t start_ipv4_filter_idx;
  /**<   Start index of IPv4 rules in filter index list.
    */

  /* Optional */
  /*  Start index on IPv6 filters installed on source pipe */
  uint8_t start_ipv6_filter_idx_valid;  /**< Must be set to true if start_ipv6_filter_idx is being passed */
  uint32_t start_ipv6_filter_idx;
  /**<   Start index of IPv6 rules in filter index list.
    */
}ipa_fltr_installed_notif_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Notifies the remote driver of the installation of the filter rule
           supplied earlier by the remote driver. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_fltr_installed_notif_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Notifies the remote driver of the need to clear the data path to
           prevent the IPA from being blocked at the head of the processing
           pipeline. */
typedef struct {

  /* Mandatory */
  /*  Pipe Mask */
  uint32_t source_pipe_bitmask;
  /**<   Set of consumer (source) pipes that must be clear of
         active data transfers.
    */

  /* Mandatory */
  /*  Request ID */
  uint32_t request_id;
  /**<   Identifies the ID of the request that is sent to
         the server. The same request ID is used in the message to remove
         the force_clear request. The server is expected to keep track of the
         request ID and the source_pipe_bitmask so that it can revert as needed
    */

  /* Optional */
  /*  Source Throttle State */
  uint8_t throttle_source_valid;  /**< Must be set to true if throttle_source is being passed */
  uint8_t throttle_source;
  /**<   Specifies whether the server is to throttle the data from
         these consumer (source) pipes after clearing the exisiting 
         data present in the IPA that were pulled from these pipes.
         The server is expected to put all the source pipes in the 
         source_pipe_bitmask in the same state.
    */
}ipa_enable_force_clear_datapath_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Notifies the remote driver of the need to clear the data path to
           prevent the IPA from being blocked at the head of the processing
           pipeline. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_enable_force_clear_datapath_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Notifies the remote driver that the forceful clearing of the data
           path can be lifted. */
typedef struct {

  /* Mandatory */
  /*  Request ID */
  uint32_t request_id;
  /**<   Identifies the request that was sent to the server to
         forcibly clear the data path. This request simply undoes the operation
         done in that request.
    */
}ipa_disable_force_clear_datapath_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Notifies the remote driver that the forceful clearing of the data
           path can be lifted. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_disable_force_clear_datapath_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_PERIPHERAL_SPEED_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_PER_USB_FS_V01 = 1, /**<  Full-speed USB connection
   */
  QMI_IPA_PER_USB_HS_V01 = 2, /**<  High-speed USB connection
   */
  QMI_IPA_PER_USB_SS_V01 = 3, /**<  Super-speed USB connection
   */
  IPA_PERIPHERAL_SPEED_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_peripheral_speed_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_PIPE_MODE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_PIPE_MODE_HW_V01 = 1, /**<  Pipe is connected with a hardware block
   */
  QMI_IPA_PIPE_MODE_SW_V01 = 2, /**<  Pipe is controlled by the software
   */
  IPA_PIPE_MODE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_pipe_mode_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_enums
    @{
  */
typedef enum {
  IPA_PERIPHERAL_TYPE_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_IPA_PERIPHERAL_USB_V01 = 1, /**<  Specifies a USB peripheral
   */
  QMI_IPA_PERIPHERAL_HSIC_V01 = 2, /**<  Specifies an HSIC peripheral
   */
  QMI_IPA_PERIPHERAL_PCIE_V01 = 3, /**<  Specifies a PCIe	peripheral
   */
  IPA_PERIPHERAL_TYPE_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}ipa_peripheral_type_enum_v01;
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Notifies the remote driver of the configuration information. */
typedef struct {

  /* Optional */
  /*  Peripheral Type */
  uint8_t peripheral_type_valid;  /**< Must be set to true if peripheral_type is being passed */
  ipa_peripheral_type_enum_v01 peripheral_type;
  /**<   Informs the remote driver about the perhipheral for
 which this configuration information is relevant. Values: \n
      - QMI_IPA_PERIPHERAL_USB (1) --  Specifies a USB peripheral
  
      - QMI_IPA_PERIPHERAL_HSIC (2) --  Specifies an HSIC peripheral
  
      - QMI_IPA_PERIPHERAL_PCIE (3) --  Specifies a PCIe	peripheral
  

 */

  /* Optional */
  /*  HW Deaggregation Support */
  uint8_t hw_deaggr_supported_valid;  /**< Must be set to true if hw_deaggr_supported is being passed */
  uint8_t hw_deaggr_supported;
  /**<   Informs the remote driver whether the local IPA driver  
         allows de-aggregation to be performed in the hardware.
    */

  /* Optional */
  /*  Maximum Aggregation Frame Size */
  uint8_t max_aggr_frame_size_valid;  /**< Must be set to true if max_aggr_frame_size is being passed */
  uint32_t max_aggr_frame_size;
  /**<   Specifies the maximum size of the aggregated frame that
         the remote driver can expect from this execution environment. \n
         - Valid range: 128 bytes to 32768 bytes.
    */

  /* Optional */
  /*  IPA Ingress Pipe Mode */
  uint8_t ipa_ingress_pipe_mode_valid;  /**< Must be set to true if ipa_ingress_pipe_mode is being passed */
  ipa_pipe_mode_enum_v01 ipa_ingress_pipe_mode;
  /**<   Indicates to the remote driver if the ingress pipe into the
 IPA is in direct connection with another hardware block or if the
 producer of data to this ingress pipe is a software module. Values: \n
      - QMI_IPA_PIPE_MODE_HW (1) --  Pipe is connected with a hardware block
  
      - QMI_IPA_PIPE_MODE_SW (2) --  Pipe is controlled by the software
  
 */

  /* Optional */
  /*  Peripheral Speed Info */
  uint8_t peripheral_speed_info_valid;  /**< Must be set to true if peripheral_speed_info is being passed */
  ipa_peripheral_speed_enum_v01 peripheral_speed_info;
  /**<   Indicates the speed that the peripheral connected to the IPA supports.
 Values: \n
      - QMI_IPA_PER_USB_FS (1) --  Full-speed USB connection
  
      - QMI_IPA_PER_USB_HS (2) --  High-speed USB connection
  
      - QMI_IPA_PER_USB_SS (3) --  Super-speed USB connection
  
 */

  /* Optional */
  /*  Downlink Accumulation Time limit */
  uint8_t dl_accumulation_time_limit_valid;  /**< Must be set to true if dl_accumulation_time_limit is being passed */
  uint32_t dl_accumulation_time_limit;
  /**<   Informs the remote driver about the time for which data
         is accumulated in the downlink direction before it is pushed into the
         IPA (downlink is with respect to the WWAN air interface). \n
         - Units: milliseconds \n
         - Maximum value: 255
    */

  /* Optional */
  /*  Downlink Accumulation Packet limit */
  uint8_t dl_accumulation_pkt_limit_valid;  /**< Must be set to true if dl_accumulation_pkt_limit is being passed */
  uint32_t dl_accumulation_pkt_limit;
  /**<   Informs the remote driver about the number of packets
         that are to be accumulated in the downlink direction before it is pushed
         into the IPA. \n
         - Maximum value: 1023
    */

  /* Optional */
  /*  Downlink Accumulation Byte Limit */
  uint8_t dl_accumulation_byte_limit_valid;  /**< Must be set to true if dl_accumulation_byte_limit is being passed */
  uint32_t dl_accumulation_byte_limit;
  /**<   Inform the remote driver about the number of bytes
         that are to be accumulated in the downlink direction before it is pushed
         into the IPA. \n
         - Maximum value: TBD
    */

  /* Optional */
  /*  Uplink Accumulation Time Limit */
  uint8_t ul_accumulation_time_limit_valid;  /**< Must be set to true if ul_accumulation_time_limit is being passed */
  uint32_t ul_accumulation_time_limit;
  /**<   Inform thes remote driver about the time for which data
         is to be accumulated in the uplink direction before it is pushed into
         the IPA (downlink is with respect to the WWAN air interface). \n
         - Units: milliseconds \n
         - Maximum value: 255
    */

  /* Optional */
  /*  HW Control Flags */
  uint8_t hw_control_flags_valid;  /**< Must be set to true if hw_control_flags is being passed */
  uint32_t hw_control_flags;
  /**<   Informs the remote driver about the hardware control flags: \n

         - Bit 0: IPA_HW_FLAG_HALT_SYSTEM_ON_NON_TERMINAL_FAILURE --
         Indicates to the hardware that it must not continue with
         any subsequent operation even if the failure is not terminal.

         - Bit 1: IPA_HW_FLAG_NO_REPORT_MHI_CHANNEL_ERORR --
         Indicates to the hardware that it is not required to report
         channel errors to the host.

         - Bit 2: IPA_HW_FLAG_NO_REPORT_MHI_CHANNEL_WAKE_UP --
         Indicates to the hardware that it is not required to generate wake-up
         events to the host.

         - Bit 4: IPA_HW_FLAG_WORK_OVER_DDR --
         Indicates to the hardware that it is accessing addresses in the DDR
         and not over PCIe.

         - Bit 5: IPA_HW_FLAG_INTERRUPT_MODE_CTRL_FLAG --
         Indicates whether the device must
         raise an event to let the host know that it is going into an interrupt
         mode (no longer polling for data/buffer availability).
    */

  /* Optional */
  /*  Uplink MSI Event Threshold */
  uint8_t ul_msi_event_threshold_valid;  /**< Must be set to true if ul_msi_event_threshold is being passed */
  uint32_t ul_msi_event_threshold;
  /**<   Informs the remote driver about the threshold that will
         cause an interrupt (MSI) to be fired to the host. This ensures
         that the remote driver does not accumulate an excesive number of
         events before firing an interrupt.

         This threshold is applicable for data that is moved in the UL direction. \n
         - Maximum value: 65535
    */

  /* Optional */
  /*  Downlink MSI Event Threshold */
  uint8_t dl_msi_event_threshold_valid;  /**< Must be set to true if dl_msi_event_threshold is being passed */
  uint32_t dl_msi_event_threshold;
  /**<   Informs the remote driver about the threshold that will
         cause an interrupt (MSI) to be fired to the host. This ensures
         that the remote driver does not accumulate an excesive number of
         events before firing an interrupt.

         This threshold is applicable for data that is moved in the DL direction. \n
         - Maximum value: 65535
    */
}ipa_config_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Notifies the remote driver of the configuration information. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_config_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Message exchanged between control point and service to notify the 
           remote driver to prevent the transport link
           from going into low power state because low latency traffic
           is in progress.  */
typedef struct {

  /* Optional */
  /*  Request ID */
  uint8_t request_id_valid;  /**< Must be set to true if request_id is being passed */
  uint32_t request_id;
  /**<   This is used to identify the ID of the request that is being sent to
         the server. The same request id will be used in the message to remove
         the enable_link_low_power_state request. 
         The server is expected to keep track of the request id and the 
         link_inactivity_threshold that it can revert as needed
    */

  /* Optional */
  /*  Link Endpoint ID */
  uint8_t link_ep_id_valid;  /**< Must be set to true if link_ep_id is being passed */
  data_ep_id_type_v01 link_ep_id;
  /**<   This is used to specify the link endpoint ID on which the requested 
         action needs to be taken. This endpoint ID is a global identfier 
         used to identify the hw accelerated channel */
}ipa_disable_link_low_pwr_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Message exchanged between control point and service to notify the 
           remote driver to prevent the transport link
           from going into low power state because low latency traffic
           is in progress.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_disable_link_low_pwr_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Request Message; Message exchanged between control point and service to notify the 
           remote driver about lifting the requirement to prevent transport
           link low power state becuase low latency traffic has ended */
typedef struct {

  /* Optional */
  /*  Request ID */
  uint8_t request_id_valid;  /**< Must be set to true if request_id is being passed */
  uint32_t request_id;
  /**<   This is used to identify the request that was sent to the server to
         disbale_link_low_power_state request. 
         This request simply undoes the operation done in that request.
    */
}ipa_enable_link_low_pwr_state_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup ipa_qmi_messages
    @{
  */
/** Response Message; Message exchanged between control point and service to notify the 
           remote driver about lifting the requirement to prevent transport
           link low power state becuase low latency traffic has ended */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/
}ipa_enable_link_low_pwr_state_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_IPA_CONFIG_V01 
//#define REMOVE_QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_V01 
//#define REMOVE_QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_V01 
//#define REMOVE_QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_V01 
//#define REMOVE_QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_V01 
//#define REMOVE_QMI_IPA_FILTER_INSTALLED_NOTIF_V01 
//#define REMOVE_QMI_IPA_INDICATION_REGISTER_V01 
//#define REMOVE_QMI_IPA_INIT_MODEM_DRIVER_V01 
//#define REMOVE_QMI_IPA_INSTALL_FILTER_RULE_V01 
//#define REMOVE_QMI_IPA_MASTER_DRIVER_INIT_COMPLETE_V01 

/*Service Message Definition*/
/** @addtogroup ipa_qmi_msg_ids
    @{
  */
#define QMI_IPA_INDICATION_REGISTER_REQ_V01 0x0020
#define QMI_IPA_INDICATION_REGISTER_RESP_V01 0x0020
#define QMI_IPA_INIT_MODEM_DRIVER_REQ_V01 0x0021
#define QMI_IPA_INIT_MODEM_DRIVER_RESP_V01 0x0021
#define QMI_IPA_MASTER_DRIVER_INIT_COMPLETE_IND_V01 0x0022
#define QMI_IPA_INSTALL_FILTER_RULE_REQ_V01 0x0023
#define QMI_IPA_INSTALL_FILTER_RULE_RESP_V01 0x0023
#define QMI_IPA_FILTER_INSTALLED_NOTIF_REQ_V01 0x0024
#define QMI_IPA_FILTER_INSTALLED_NOTIF_RESP_V01 0x0024
#define QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_REQ_V01 0x0025
#define QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_RESP_V01 0x0025
#define QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_REQ_V01 0x0026
#define QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_RESP_V01 0x0026
#define QMI_IPA_CONFIG_REQ_V01 0x0027
#define QMI_IPA_CONFIG_RESP_V01 0x0027
#define QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_REQ_V01 0x0028
#define QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_RESP_V01 0x0028
#define QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_REQ_V01 0x0029
#define QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_RESP_V01 0x0029
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro ipa_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type ipa_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define ipa_get_service_object_v01( ) \
          ipa_get_service_object_internal_v01( \
            IPA_V01_IDL_MAJOR_VERS, IPA_V01_IDL_MINOR_VERS, \
            IPA_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

