/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S N S _ Q F P _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the SNS_QFP_SVC service Data structures.


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

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "sns_qfp_v01.h"
#include "sns_common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t sns_qfp_open_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_open_req_msg_v01, port_id),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_open_req_msg_v01, slave_index),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_open_req_msg_v01, freq)
};

static const uint8_t sns_qfp_open_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_open_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * sns_qfp_close_req_msg is empty
 * static const uint8_t sns_qfp_close_req_msg_data_v01[] = {
 * };
 */

static const uint8_t sns_qfp_close_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_close_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t sns_qfp_keep_alive_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_keep_alive_req_msg_v01, enable)
};

static const uint8_t sns_qfp_keep_alive_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_keep_alive_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t sns_qfp_finger_detect_enable_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_finger_detect_enable_req_msg_v01, poll_time),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_qfp_finger_detect_enable_req_msg_v01, finger_detect_mode) - QMI_IDL_OFFSET8(sns_qfp_finger_detect_enable_req_msg_v01, finger_detect_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_finger_detect_enable_req_msg_v01, finger_detect_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_qfp_finger_detect_enable_req_msg_v01, low_poll_time) - QMI_IDL_OFFSET8(sns_qfp_finger_detect_enable_req_msg_v01, low_poll_time_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_finger_detect_enable_req_msg_v01, low_poll_time)
};

static const uint8_t sns_qfp_finger_detect_enable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_finger_detect_enable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * sns_qfp_finger_detect_disable_req_msg is empty
 * static const uint8_t sns_qfp_finger_detect_disable_req_msg_data_v01[] = {
 * };
 */

static const uint8_t sns_qfp_finger_detect_disable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_finger_detect_disable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t sns_qfp_finger_detect_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_finger_detect_ind_msg_v01, timestamp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_finger_detect_ind_msg_v01, flags)
};

static const uint8_t sns_qfp_simple_item_write_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_simple_item_write_req_msg_v01, item_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_simple_item_write_req_msg_v01, data),
  SNS_QFP_SIMPLE_ITEM_MAX_SIZE_V01,
  QMI_IDL_OFFSET8(sns_qfp_simple_item_write_req_msg_v01, data) - QMI_IDL_OFFSET8(sns_qfp_simple_item_write_req_msg_v01, data_len)
};

static const uint8_t sns_qfp_simple_item_write_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_simple_item_write_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t sns_qfp_ffd_basis_data_create_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_create_req_msg_v01, item_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_create_req_msg_v01, vector_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_create_req_msg_v01, num_vectors)
};

static const uint8_t sns_qfp_ffd_basis_data_create_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_create_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_create_resp_msg_v01, handle) - QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_create_resp_msg_v01, handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_create_resp_msg_v01, handle)
};

static const uint8_t sns_qfp_ffd_basis_data_write_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_write_req_msg_v01, handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_write_req_msg_v01, data),
  ((SNS_QFP_FFD_BASIS_DATA_MAX_CHUNK_SIZE_V01) & 0xFF), ((SNS_QFP_FFD_BASIS_DATA_MAX_CHUNK_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_write_req_msg_v01, data) - QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_write_req_msg_v01, data_len)
};

static const uint8_t sns_qfp_ffd_basis_data_write_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_write_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_write_resp_msg_v01, written) - QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_write_resp_msg_v01, written_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_write_resp_msg_v01, written)
};

static const uint8_t sns_qfp_ffd_basis_data_close_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_close_req_msg_v01, handle)
};

static const uint8_t sns_qfp_ffd_basis_data_close_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_data_close_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t sns_qfp_debug_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_debug_req_msg_v01, req_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_qfp_debug_req_msg_v01, data) - QMI_IDL_OFFSET8(sns_qfp_debug_req_msg_v01, data_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_debug_req_msg_v01, data),
  ((SNS_QFP_DEBUG_MSG_MAX_SIZE_V01) & 0xFF), ((SNS_QFP_DEBUG_MSG_MAX_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(sns_qfp_debug_req_msg_v01, data) - QMI_IDL_OFFSET8(sns_qfp_debug_req_msg_v01, data_len)
};

static const uint8_t sns_qfp_debug_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_debug_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_qfp_debug_resp_msg_v01, req_id) - QMI_IDL_OFFSET8(sns_qfp_debug_resp_msg_v01, req_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_debug_resp_msg_v01, req_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_qfp_debug_resp_msg_v01, data) - QMI_IDL_OFFSET8(sns_qfp_debug_resp_msg_v01, data_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_debug_resp_msg_v01, data),
  ((SNS_QFP_DEBUG_MSG_MAX_SIZE_V01) & 0xFF), ((SNS_QFP_DEBUG_MSG_MAX_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(sns_qfp_debug_resp_msg_v01, data) - QMI_IDL_OFFSET8(sns_qfp_debug_resp_msg_v01, data_len)
};

static const uint8_t sns_qfp_ffd_basis_norm_vector_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_norm_vector_req_msg_v01, item_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_norm_vector_req_msg_v01, data),
  ((SNS_QFP_FFD_BASIS_DATA_MAX_CHUNK_SIZE_V01) & 0xFF), ((SNS_QFP_FFD_BASIS_DATA_MAX_CHUNK_SIZE_V01) >> 8),
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_norm_vector_req_msg_v01, data) - QMI_IDL_OFFSET8(sns_qfp_ffd_basis_norm_vector_req_msg_v01, data_len)
};

static const uint8_t sns_qfp_ffd_basis_norm_vector_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_basis_norm_vector_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t sns_qfp_ffd_change_polling_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_change_polling_req_msg_v01, state)
};

static const uint8_t sns_qfp_ffd_change_polling_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_qfp_ffd_change_polling_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry SNS_QFP_SVC_message_table_v01[] = {
  {sizeof(sns_qfp_open_req_msg_v01), sns_qfp_open_req_msg_data_v01},
  {sizeof(sns_qfp_open_resp_msg_v01), sns_qfp_open_resp_msg_data_v01},
  {sizeof(sns_qfp_close_req_msg_v01), 0},
  {sizeof(sns_qfp_close_resp_msg_v01), sns_qfp_close_resp_msg_data_v01},
  {sizeof(sns_qfp_keep_alive_req_msg_v01), sns_qfp_keep_alive_req_msg_data_v01},
  {sizeof(sns_qfp_keep_alive_resp_msg_v01), sns_qfp_keep_alive_resp_msg_data_v01},
  {sizeof(sns_qfp_finger_detect_enable_req_msg_v01), sns_qfp_finger_detect_enable_req_msg_data_v01},
  {sizeof(sns_qfp_finger_detect_enable_resp_msg_v01), sns_qfp_finger_detect_enable_resp_msg_data_v01},
  {sizeof(sns_qfp_finger_detect_disable_req_msg_v01), 0},
  {sizeof(sns_qfp_finger_detect_disable_resp_msg_v01), sns_qfp_finger_detect_disable_resp_msg_data_v01},
  {sizeof(sns_qfp_finger_detect_ind_msg_v01), sns_qfp_finger_detect_ind_msg_data_v01},
  {sizeof(sns_qfp_simple_item_write_req_msg_v01), sns_qfp_simple_item_write_req_msg_data_v01},
  {sizeof(sns_qfp_simple_item_write_resp_msg_v01), sns_qfp_simple_item_write_resp_msg_data_v01},
  {sizeof(sns_qfp_ffd_basis_data_create_req_msg_v01), sns_qfp_ffd_basis_data_create_req_msg_data_v01},
  {sizeof(sns_qfp_ffd_basis_data_create_resp_msg_v01), sns_qfp_ffd_basis_data_create_resp_msg_data_v01},
  {sizeof(sns_qfp_ffd_basis_data_write_req_msg_v01), sns_qfp_ffd_basis_data_write_req_msg_data_v01},
  {sizeof(sns_qfp_ffd_basis_data_write_resp_msg_v01), sns_qfp_ffd_basis_data_write_resp_msg_data_v01},
  {sizeof(sns_qfp_ffd_basis_data_close_req_msg_v01), sns_qfp_ffd_basis_data_close_req_msg_data_v01},
  {sizeof(sns_qfp_ffd_basis_data_close_resp_msg_v01), sns_qfp_ffd_basis_data_close_resp_msg_data_v01},
  {sizeof(sns_qfp_debug_req_msg_v01), sns_qfp_debug_req_msg_data_v01},
  {sizeof(sns_qfp_debug_resp_msg_v01), sns_qfp_debug_resp_msg_data_v01},
  {sizeof(sns_qfp_ffd_basis_norm_vector_req_msg_v01), sns_qfp_ffd_basis_norm_vector_req_msg_data_v01},
  {sizeof(sns_qfp_ffd_basis_norm_vector_resp_msg_v01), sns_qfp_ffd_basis_norm_vector_resp_msg_data_v01},
  {sizeof(sns_qfp_ffd_change_polling_req_msg_v01), sns_qfp_ffd_change_polling_req_msg_data_v01},
  {sizeof(sns_qfp_ffd_change_polling_resp_msg_v01), sns_qfp_ffd_change_polling_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object SNS_QFP_SVC_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *SNS_QFP_SVC_qmi_idl_type_table_object_referenced_tables_v01[] =
{&SNS_QFP_SVC_qmi_idl_type_table_object_v01, &sns_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object SNS_QFP_SVC_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(SNS_QFP_SVC_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  SNS_QFP_SVC_message_table_v01,
  SNS_QFP_SVC_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry SNS_QFP_SVC_service_command_messages_v01[] = {
  {SNS_QFP_CANCEL_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {SNS_QFP_VERSION_REQ_V01, QMI_IDL_TYPE16(1, 2), 0},
  {SNS_QFP_OPEN_REQ_V01, QMI_IDL_TYPE16(0, 0), 15},
  {SNS_QFP_CLOSE_REQ_V01, QMI_IDL_TYPE16(0, 2), 0},
  {SNS_QFP_KEEP_ALIVE_REQ_V01, QMI_IDL_TYPE16(0, 4), 4},
  {SNS_QFP_FINGER_DETECT_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 6), 21},
  {SNS_QFP_FINGER_DETECT_DISABLE_REQ_V01, QMI_IDL_TYPE16(0, 8), 0},
  {SNS_QFP_SIMPLE_ITEM_WRITE_REQ_V01, QMI_IDL_TYPE16(0, 11), 19},
  {SNS_QFP_FFD_BASIS_DATA_CREATE_REQ_V01, QMI_IDL_TYPE16(0, 13), 21},
  {SNS_QFP_FFD_BASIS_DATA_WRITE_REQ_V01, QMI_IDL_TYPE16(0, 15), 4012},
  {SNS_QFP_FFD_BASIS_DATA_CLOSE_REQ_V01, QMI_IDL_TYPE16(0, 17), 7},
  {SNS_QFP_DEBUG_REQ_V01, QMI_IDL_TYPE16(0, 19), 4012},
  {SNS_QFP_FFD_BASIS_NORM_VECTOR_REQ_V01, QMI_IDL_TYPE16(0, 21), 4012},
  {SNS_QFP_FFD_CHANGE_POLLING_REQ_V01, QMI_IDL_TYPE16(0, 23), 7}
};

static const qmi_idl_service_message_table_entry SNS_QFP_SVC_service_response_messages_v01[] = {
  {SNS_QFP_CANCEL_RESP_V01, QMI_IDL_TYPE16(1, 1), 5},
  {SNS_QFP_VERSION_RESP_V01, QMI_IDL_TYPE16(1, 3), 17},
  {SNS_QFP_OPEN_RESP_V01, QMI_IDL_TYPE16(0, 1), 5},
  {SNS_QFP_CLOSE_RESP_V01, QMI_IDL_TYPE16(0, 3), 5},
  {SNS_QFP_KEEP_ALIVE_RESP_V01, QMI_IDL_TYPE16(0, 5), 5},
  {SNS_QFP_FINGER_DETECT_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 7), 5},
  {SNS_QFP_FINGER_DETECT_DISABLE_RESP_V01, QMI_IDL_TYPE16(0, 9), 5},
  {SNS_QFP_SIMPLE_ITEM_WRITE_RESP_V01, QMI_IDL_TYPE16(0, 12), 5},
  {SNS_QFP_FFD_BASIS_DATA_CREATE_RESP_V01, QMI_IDL_TYPE16(0, 14), 12},
  {SNS_QFP_FFD_BASIS_DATA_WRITE_RESP_V01, QMI_IDL_TYPE16(0, 16), 12},
  {SNS_QFP_FFD_BASIS_DATA_CLOSE_RESP_V01, QMI_IDL_TYPE16(0, 18), 5},
  {SNS_QFP_DEBUG_RESP_V01, QMI_IDL_TYPE16(0, 20), 4017},
  {SNS_QFP_FFD_BASIS_NORM_VECTOR_RESP_V01, QMI_IDL_TYPE16(0, 22), 5},
  {SNS_QFP_FFD_CHANGE_POLLING_RESP_V01, QMI_IDL_TYPE16(0, 24), 5}
};

static const qmi_idl_service_message_table_entry SNS_QFP_SVC_service_indication_messages_v01[] = {
  {SNS_QFP_FINGER_DETECT_IND_V01, QMI_IDL_TYPE16(0, 10), 18}
};

/*Service Object*/
struct qmi_idl_service_object SNS_QFP_SVC_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  SNS_QMI_SVC_ID_56_V01,
  4017,
  { sizeof(SNS_QFP_SVC_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(SNS_QFP_SVC_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(SNS_QFP_SVC_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { SNS_QFP_SVC_service_command_messages_v01, SNS_QFP_SVC_service_response_messages_v01, SNS_QFP_SVC_service_indication_messages_v01},
  &SNS_QFP_SVC_qmi_idl_type_table_object_v01,
  0x03,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type SNS_QFP_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SNS_QFP_SVC_V01_IDL_MAJOR_VERS != idl_maj_version || SNS_QFP_SVC_V01_IDL_MINOR_VERS != idl_min_version
       || SNS_QFP_SVC_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&SNS_QFP_SVC_qmi_idl_service_object_v01;
}

