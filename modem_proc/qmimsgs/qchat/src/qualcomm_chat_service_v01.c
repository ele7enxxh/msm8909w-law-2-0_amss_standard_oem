/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q U A L C O M M _ C H A T _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the qchat service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All rights reserved.  Confidential and Proprietary - Qualcomm Technologies, Inc.

    QUALCOMM does not grant to you in this Agreement any rights in the OEM
    QChat Porting Kit including the accompanying associated documentation
    (the “Software”) for any purpose whatsoever, including without limitation
    any rights to copy, use or distribute the Software. Your rights to use the
    Software shall be only as set forth in any separate license between you and
    QUALCOMM which grants to you a license in the Software, and such license 
    shall be under the terms and for the limited purposes set forth in that 
    agreement.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/qchat/src/qualcomm_chat_service_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2 
   It was generated on: Wed Jul 24 2013 (Spin 1)
   From IDL File: qualcomm_chat_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "qualcomm_chat_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
/* 
 * qchat_init_req_msg is empty
 * static const uint8_t qchat_init_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t qchat_init_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qchat_init_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qchat_init_resp_msg_v01, handle) - QMI_IDL_OFFSET8(qchat_init_resp_msg_v01, handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qchat_init_resp_msg_v01, handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qchat_init_resp_msg_v01, qchat_error) - QMI_IDL_OFFSET8(qchat_init_resp_msg_v01, qchat_error_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qchat_init_resp_msg_v01, qchat_error)
};

static const uint8_t qchat_trigger_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qchat_trigger_req_msg_v01, handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qchat_trigger_req_msg_v01, trigger_category),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qchat_trigger_req_msg_v01, data),
  ((QCHAT_MAX_PAYLOAD_V01) & 0xFF), ((QCHAT_MAX_PAYLOAD_V01) >> 8),
  QMI_IDL_OFFSET8(qchat_trigger_req_msg_v01, data) - QMI_IDL_OFFSET8(qchat_trigger_req_msg_v01, data_len)
};

static const uint8_t qchat_trigger_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qchat_trigger_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qchat_trigger_resp_msg_v01, qchat_error) - QMI_IDL_OFFSET8(qchat_trigger_resp_msg_v01, qchat_error_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qchat_trigger_resp_msg_v01, qchat_error),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qchat_trigger_resp_msg_v01, data) - QMI_IDL_OFFSET8(qchat_trigger_resp_msg_v01, data_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qchat_trigger_resp_msg_v01, data),
  ((QCHAT_MAX_PAYLOAD_V01) & 0xFF), ((QCHAT_MAX_PAYLOAD_V01) >> 8),
  QMI_IDL_OFFSET8(qchat_trigger_resp_msg_v01, data) - QMI_IDL_OFFSET8(qchat_trigger_resp_msg_v01, data_len)
};

static const uint8_t qchat_event_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qchat_event_register_req_msg_v01, enable) - QMI_IDL_OFFSET8(qchat_event_register_req_msg_v01, enable_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qchat_event_register_req_msg_v01, enable)
};

static const uint8_t qchat_event_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qchat_event_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qchat_event_registered_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qchat_event_registered_ind_msg_v01, event) - QMI_IDL_OFFSET8(qchat_event_registered_ind_msg_v01, event_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qchat_event_registered_ind_msg_v01, event),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qchat_event_registered_ind_msg_v01, payload) - QMI_IDL_OFFSET8(qchat_event_registered_ind_msg_v01, payload_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qchat_event_registered_ind_msg_v01, payload),
  ((QCHAT_MAX_PAYLOAD_V01) & 0xFF), ((QCHAT_MAX_PAYLOAD_V01) >> 8),
  QMI_IDL_OFFSET8(qchat_event_registered_ind_msg_v01, payload) - QMI_IDL_OFFSET8(qchat_event_registered_ind_msg_v01, payload_len)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry qchat_message_table_v01[] = {
  {sizeof(qchat_init_req_msg_v01), 0},
  {sizeof(qchat_init_resp_msg_v01), qchat_init_resp_msg_data_v01},
  {sizeof(qchat_trigger_req_msg_v01), qchat_trigger_req_msg_data_v01},
  {sizeof(qchat_trigger_resp_msg_v01), qchat_trigger_resp_msg_data_v01},
  {sizeof(qchat_event_register_req_msg_v01), qchat_event_register_req_msg_data_v01},
  {sizeof(qchat_event_register_resp_msg_v01), qchat_event_register_resp_msg_data_v01},
  {sizeof(qchat_event_registered_ind_msg_v01), qchat_event_registered_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object qchat_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *qchat_qmi_idl_type_table_object_referenced_tables_v01[] =
{&qchat_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object qchat_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(qchat_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  qchat_message_table_v01,
  qchat_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry qchat_service_command_messages_v01[] = {
  {QMI_QCHAT_INIT_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_QCHAT_TRIGGER_REQ_V01, QMI_IDL_TYPE16(0, 2), 8219},
  {QMI_QCHAT_EVENT_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 4), 4}
};

static const qmi_idl_service_message_table_entry qchat_service_response_messages_v01[] = {
  {QMI_QCHAT_INIT_RESP_V01, QMI_IDL_TYPE16(0, 1), 21},
  {QMI_QCHAT_TRIGGER_RESP_V01, QMI_IDL_TYPE16(0, 3), 8219},
  {QMI_QCHAT_EVENT_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 5), 7}
};

static const qmi_idl_service_message_table_entry qchat_service_indication_messages_v01[] = {
  {QMI_QCHAT_EVENT_REGISTERED_IND_V01, QMI_IDL_TYPE16(0, 6), 8212}
};

/*Service Object*/
struct qmi_idl_service_object qchat_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x0D,
  8219,
  { sizeof(qchat_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qchat_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qchat_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { qchat_service_command_messages_v01, qchat_service_response_messages_v01, qchat_service_indication_messages_v01},
  &qchat_qmi_idl_type_table_object_v01,
  0x03,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type qchat_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( QCHAT_V01_IDL_MAJOR_VERS != idl_maj_version || QCHAT_V01_IDL_MINOR_VERS != idl_min_version 
       || QCHAT_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&qchat_qmi_idl_service_object_v01;
}

