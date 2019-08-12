/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S N S _ O C M E M _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the SNS_OCMEM_SVC service Data structures.

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved. 
 QUALCOMM Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/common/idl/src/sns_ocmem_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.5 
   It was generated on: Mon Aug 27 2012
   From IDL File: sns_ocmem_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "sns_ocmem_v01.h"


/*Type Definitions*/
static const uint8_t sns_ocmem_common_resp_s_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_ocmem_common_resp_s_v01, sns_result_t),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_ocmem_common_resp_s_v01, sns_err_t),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sns_mem_segment_s_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sns_mem_segment_s_v01, type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_mem_segment_s_v01, size),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sns_mem_segment_s_v01, start_address),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/* 
 * sns_ocmem_common_cancel_req_msg is empty
 * static const uint8_t sns_ocmem_common_cancel_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t sns_ocmem_common_cancel_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 2,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_ocmem_common_cancel_resp_msg_v01, resp),
  0, 0
};

/* 
 * sns_ocmem_common_version_req_msg is empty
 * static const uint8_t sns_ocmem_common_version_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t sns_ocmem_common_version_resp_msg_data_v01[] = {
  2,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_ocmem_common_version_resp_msg_v01, resp),
  0, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_ocmem_common_version_resp_msg_v01, interface_version_number) - QMI_IDL_OFFSET8(sns_ocmem_common_version_resp_msg_v01, interface_version_number_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_ocmem_common_version_resp_msg_v01, interface_version_number),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_ocmem_common_version_resp_msg_v01, max_message_id) - QMI_IDL_OFFSET8(sns_ocmem_common_version_resp_msg_v01, max_message_id_valid)),
  0x11,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sns_ocmem_common_version_resp_msg_v01, max_message_id)
};

/* 
 * sns_ocmem_phys_addr_req_msg is empty
 * static const uint8_t sns_ocmem_phys_addr_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t sns_ocmem_phys_addr_resp_msg_data_v01[] = {
  2,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_ocmem_phys_addr_resp_msg_v01, resp),
  0, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_ocmem_phys_addr_resp_msg_v01, segments) - QMI_IDL_OFFSET8(sns_ocmem_phys_addr_resp_msg_v01, segments_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_ocmem_phys_addr_resp_msg_v01, segments),
  SNS_OCMEM_MAX_NUM_SEG_V01,
  QMI_IDL_OFFSET8(sns_ocmem_phys_addr_resp_msg_v01, segments) - QMI_IDL_OFFSET8(sns_ocmem_phys_addr_resp_msg_v01, segments_len),
  1, 0
};

static const uint8_t sns_ocmem_has_client_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sns_ocmem_has_client_ind_msg_v01, num_clients)
};

static const uint8_t sns_ocmem_bw_vote_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_ocmem_bw_vote_req_msg_v01, is_map),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_ocmem_bw_vote_req_msg_v01, vectors) - QMI_IDL_OFFSET8(sns_ocmem_bw_vote_req_msg_v01, vectors_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_ocmem_bw_vote_req_msg_v01, vectors),
  ((SNS_OCMEM_MAX_VECTORS_SIZE_V01) & 0xFF), ((SNS_OCMEM_MAX_VECTORS_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(sns_ocmem_bw_vote_req_msg_v01, vectors) - QMI_IDL_OFFSET8(sns_ocmem_bw_vote_req_msg_v01, vectors_len)
};

static const uint8_t sns_ocmem_bw_vote_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 2,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_ocmem_bw_vote_resp_msg_v01, resp),
  0, 0
};

static const uint8_t sns_ocmem_bw_vote_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_ocmem_bw_vote_ind_msg_v01, is_vote_on)
};

/* Type Table */
static const qmi_idl_type_table_entry  SNS_OCMEM_SVC_type_table_v01[] = {
  {sizeof(sns_ocmem_common_resp_s_v01), sns_ocmem_common_resp_s_data_v01},
  {sizeof(sns_mem_segment_s_v01), sns_mem_segment_s_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry SNS_OCMEM_SVC_message_table_v01[] = {
  {0, 0},
  {sizeof(sns_ocmem_common_cancel_resp_msg_v01), sns_ocmem_common_cancel_resp_msg_data_v01},
  {0, 0},
  {sizeof(sns_ocmem_common_version_resp_msg_v01), sns_ocmem_common_version_resp_msg_data_v01},
  {0, 0},
  {sizeof(sns_ocmem_phys_addr_resp_msg_v01), sns_ocmem_phys_addr_resp_msg_data_v01},
  {sizeof(sns_ocmem_has_client_ind_msg_v01), sns_ocmem_has_client_ind_msg_data_v01},
  {sizeof(sns_ocmem_bw_vote_req_msg_v01), sns_ocmem_bw_vote_req_msg_data_v01},
  {sizeof(sns_ocmem_bw_vote_resp_msg_v01), sns_ocmem_bw_vote_resp_msg_data_v01},
  {sizeof(sns_ocmem_bw_vote_ind_msg_v01), sns_ocmem_bw_vote_ind_msg_data_v01}
};

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object SNS_OCMEM_SVC_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *SNS_OCMEM_SVC_qmi_idl_type_table_object_referenced_tables_v01[] =
{&SNS_OCMEM_SVC_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object SNS_OCMEM_SVC_qmi_idl_type_table_object_v01 = {
  sizeof(SNS_OCMEM_SVC_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(SNS_OCMEM_SVC_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  SNS_OCMEM_SVC_type_table_v01,
  SNS_OCMEM_SVC_message_table_v01,
  SNS_OCMEM_SVC_qmi_idl_type_table_object_referenced_tables_v01
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry SNS_OCMEM_SVC_service_command_messages_v01[] = {
  {SNS_OCMEM_CANCEL_REQ_V01, TYPE16(0, 0), 0},
  {SNS_OCMEM_VERSION_REQ_V01, TYPE16(0, 2), 0},
  {SNS_OCMEM_PHYS_ADDR_REQ_V01, TYPE16(0, 4), 0},
  {SNS_OCMEM_BW_VOTE_REQ_V01, TYPE16(0, 7), 521}
};

static const qmi_idl_service_message_table_entry SNS_OCMEM_SVC_service_response_messages_v01[] = {
  {SNS_OCMEM_CANCEL_RESP_V01, TYPE16(0, 1), 5},
  {SNS_OCMEM_VERSION_RESP_V01, TYPE16(0, 3), 17},
  {SNS_OCMEM_PHYS_ADDR_RESP_V01, TYPE16(0, 5), 1801},
  {SNS_OCMEM_BW_VOTE_RESP_V01, TYPE16(0, 8), 5}
};

static const qmi_idl_service_message_table_entry SNS_OCMEM_SVC_service_indication_messages_v01[] = {
  {SNS_OCMEM_HAS_CLIENT_IND_V01, TYPE16(0, 6), 5},
  {SNS_OCMEM_BW_VOTE_IND_V01, TYPE16(0, 9), 4}
};

/*Service Object*/
struct qmi_idl_service_object SNS_OCMEM_SVC_qmi_idl_service_object_v01 = {
  0x05,
  0x01,
  SNS_OCMEM_SVC_ID_V01,
  1801,
  { sizeof(SNS_OCMEM_SVC_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(SNS_OCMEM_SVC_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(SNS_OCMEM_SVC_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { SNS_OCMEM_SVC_service_command_messages_v01, SNS_OCMEM_SVC_service_response_messages_v01, SNS_OCMEM_SVC_service_indication_messages_v01},
  &SNS_OCMEM_SVC_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type SNS_OCMEM_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SNS_OCMEM_SVC_V01_IDL_MAJOR_VERS != idl_maj_version || SNS_OCMEM_SVC_V01_IDL_MINOR_VERS != idl_min_version 
       || SNS_OCMEM_SVC_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&SNS_OCMEM_SVC_qmi_idl_service_object_v01;
}

