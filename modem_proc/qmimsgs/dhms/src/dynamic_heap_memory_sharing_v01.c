/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D Y N A M I C _ H E A P _ M E M O R Y _ S H A R I N G _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the dhms service Data structures.

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/dhms/src/dynamic_heap_memory_sharing_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.7 
   It was generated on: Mon May 19 2014 (Spin 0)
   From IDL File: dynamic_heap_memory_sharing_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "dynamic_heap_memory_sharing_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t dhms_mem_alloc_addr_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_addr_info_type_v01, phy_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_addr_info_type_v01, num_bytes),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t dhms_mem_alloc_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_req_msg_v01, num_bytes),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dhms_mem_alloc_req_msg_v01, block_alignment) - QMI_IDL_OFFSET8(dhms_mem_alloc_req_msg_v01, block_alignment_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_req_msg_v01, block_alignment)
};

static const uint8_t dhms_mem_alloc_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(dhms_mem_alloc_resp_msg_v01, resp),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dhms_mem_alloc_resp_msg_v01, handle) - QMI_IDL_OFFSET8(dhms_mem_alloc_resp_msg_v01, handle_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_resp_msg_v01, handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dhms_mem_alloc_resp_msg_v01, num_bytes) - QMI_IDL_OFFSET8(dhms_mem_alloc_resp_msg_v01, num_bytes_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_resp_msg_v01, num_bytes)
};

static const uint8_t dhms_mem_free_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_free_req_msg_v01, handle)
};

static const uint8_t dhms_mem_free_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(dhms_mem_free_resp_msg_v01, resp)
};

static const uint8_t dhms_mem_alloc_generic_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, num_bytes),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, client_id),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, proc_id),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, sequence_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, alloc_contiguous) - QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, alloc_contiguous_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, alloc_contiguous),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, block_alignment) - QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, block_alignment_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_req_msg_v01, block_alignment)
};

static const uint8_t dhms_mem_alloc_generic_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dhms_mem_alloc_generic_resp_msg_v01, sequence_id) - QMI_IDL_OFFSET8(dhms_mem_alloc_generic_resp_msg_v01, sequence_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_resp_msg_v01, sequence_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dhms_mem_alloc_generic_resp_msg_v01, dhms_mem_alloc_addr_info) - QMI_IDL_OFFSET8(dhms_mem_alloc_generic_resp_msg_v01, dhms_mem_alloc_addr_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_resp_msg_v01, dhms_mem_alloc_addr_info),
  MAX_ARR_CNT_V01,
  QMI_IDL_OFFSET8(dhms_mem_alloc_generic_resp_msg_v01, dhms_mem_alloc_addr_info) - QMI_IDL_OFFSET8(dhms_mem_alloc_generic_resp_msg_v01, dhms_mem_alloc_addr_info_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t dhms_mem_free_generic_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dhms_mem_free_generic_req_msg_v01, dhms_mem_alloc_addr_info),
  MAX_ARR_CNT_V01,
  QMI_IDL_OFFSET8(dhms_mem_free_generic_req_msg_v01, dhms_mem_alloc_addr_info) - QMI_IDL_OFFSET8(dhms_mem_free_generic_req_msg_v01, dhms_mem_alloc_addr_info_len),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dhms_mem_free_generic_req_msg_v01, client_id) - QMI_IDL_OFFSET16RELATIVE(dhms_mem_free_generic_req_msg_v01, client_id_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(dhms_mem_free_generic_req_msg_v01, client_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(dhms_mem_free_generic_req_msg_v01, proc_id) - QMI_IDL_OFFSET16RELATIVE(dhms_mem_free_generic_req_msg_v01, proc_id_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(dhms_mem_free_generic_req_msg_v01, proc_id)
};

static const uint8_t dhms_mem_free_generic_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dhms_mem_free_generic_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t dhms_mem_query_size_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_query_size_req_msg_v01, client_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dhms_mem_query_size_req_msg_v01, proc_id) - QMI_IDL_OFFSET8(dhms_mem_query_size_req_msg_v01, proc_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_query_size_req_msg_v01, proc_id)
};

static const uint8_t dhms_mem_query_size_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(dhms_mem_query_size_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(dhms_mem_query_size_resp_msg_v01, size) - QMI_IDL_OFFSET8(dhms_mem_query_size_resp_msg_v01, size_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(dhms_mem_query_size_resp_msg_v01, size)
};

/* Type Table */
static const qmi_idl_type_table_entry  dhms_type_table_v01[] = {
  {sizeof(dhms_mem_alloc_addr_info_type_v01), dhms_mem_alloc_addr_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry dhms_message_table_v01[] = {
  {sizeof(dhms_mem_alloc_req_msg_v01), dhms_mem_alloc_req_msg_data_v01},
  {sizeof(dhms_mem_alloc_resp_msg_v01), dhms_mem_alloc_resp_msg_data_v01},
  {sizeof(dhms_mem_free_req_msg_v01), dhms_mem_free_req_msg_data_v01},
  {sizeof(dhms_mem_free_resp_msg_v01), dhms_mem_free_resp_msg_data_v01},
  {sizeof(dhms_mem_alloc_generic_req_msg_v01), dhms_mem_alloc_generic_req_msg_data_v01},
  {sizeof(dhms_mem_alloc_generic_resp_msg_v01), dhms_mem_alloc_generic_resp_msg_data_v01},
  {sizeof(dhms_mem_free_generic_req_msg_v01), dhms_mem_free_generic_req_msg_data_v01},
  {sizeof(dhms_mem_free_generic_resp_msg_v01), dhms_mem_free_generic_resp_msg_data_v01},
  {sizeof(dhms_mem_query_size_req_msg_v01), dhms_mem_query_size_req_msg_data_v01},
  {sizeof(dhms_mem_query_size_resp_msg_v01), dhms_mem_query_size_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object dhms_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *dhms_qmi_idl_type_table_object_referenced_tables_v01[] =
{&dhms_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object dhms_qmi_idl_type_table_object_v01 = {
  sizeof(dhms_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(dhms_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  dhms_type_table_v01,
  dhms_message_table_v01,
  dhms_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry dhms_service_command_messages_v01[] = {
  {QMI_DHMS_MEM_ALLOC_REQ_V01, QMI_IDL_TYPE16(0, 0), 14},
  {QMI_DHMS_MEM_FREE_REQ_V01, QMI_IDL_TYPE16(0, 2), 11},
  {QMI_DHMS_MEM_ALLOC_GENERIC_REQ_V01, QMI_IDL_TYPE16(0, 4), 39},
  {QMI_DHMS_MEM_FREE_GENERIC_REQ_V01, QMI_IDL_TYPE16(0, 6), 786},
  {QMI_DHMS_MEM_QUERY_SIZE_REQ_V01, QMI_IDL_TYPE16(0, 8), 14}
};

static const qmi_idl_service_message_table_entry dhms_service_response_messages_v01[] = {
  {QMI_DHMS_MEM_ALLOC_RESP_V01, QMI_IDL_TYPE16(0, 1), 23},
  {QMI_DHMS_MEM_FREE_RESP_V01, QMI_IDL_TYPE16(0, 3), 5},
  {QMI_DHMS_MEM_ALLOC_GENERIC_RESP_V01, QMI_IDL_TYPE16(0, 5), 786},
  {QMI_DHMS_MEM_FREE_GENERIC_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_DHMS_MEM_QUERY_SIZE_RESP_V01, QMI_IDL_TYPE16(0, 9), 14}
};

/*Service Object*/
struct qmi_idl_service_object dhms_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x34,
  786,
  { sizeof(dhms_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(dhms_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { dhms_service_command_messages_v01, dhms_service_response_messages_v01, NULL},
  &dhms_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type dhms_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( DHMS_V01_IDL_MAJOR_VERS != idl_maj_version || DHMS_V01_IDL_MINOR_VERS != idl_min_version 
       || DHMS_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&dhms_qmi_idl_service_object_v01;
}

