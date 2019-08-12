/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S N S _ S Y N C _ P R O X Y _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the SNS_SYNC_PROXY_SVC service Data structures.

  
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14 
   It was generated on: Tue Oct 28 2014 (Spin 0)
   From IDL File: sns_sync_proxy_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "sns_sync_proxy_v01.h"
#include "sns_common_v01.h"


/*Type Definitions*/
static const uint8_t sns_sync_proxy_sync_schedule_s_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sns_sync_proxy_sync_schedule_s_v01, absolute_qtimer_value),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sync_proxy_sync_schedule_s_v01, qtimer_interval),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t sns_sync_proxy_motion_enable_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sync_proxy_motion_enable_req_msg_v01, sync_schedule),
  SNS_SYNC_PROXY_MAX_SCHEDS_V01,
  QMI_IDL_OFFSET8(sns_sync_proxy_motion_enable_req_msg_v01, sync_schedule) - QMI_IDL_OFFSET8(sns_sync_proxy_motion_enable_req_msg_v01, sync_schedule_len),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sync_proxy_motion_enable_req_msg_v01, stationary_to_motion_latency),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sync_proxy_motion_enable_req_msg_v01, motion_to_stationary_latency)
};

static const uint8_t sns_sync_proxy_motion_enable_resp_msg_data_v01[] = {
  2,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sync_proxy_motion_enable_resp_msg_v01, Resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sync_proxy_motion_enable_resp_msg_v01, num_sync_schedules_processed) - QMI_IDL_OFFSET8(sns_sync_proxy_motion_enable_resp_msg_v01, num_sync_schedules_processed_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sync_proxy_motion_enable_resp_msg_v01, num_sync_schedules_processed)
};

static const uint8_t sns_sync_proxy_motion_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sync_proxy_motion_ind_msg_v01, timestamp),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sync_proxy_motion_ind_msg_v01, state)
};

/* Type Table */
static const qmi_idl_type_table_entry  SNS_SYNC_PROXY_SVC_type_table_v01[] = {
  {sizeof(sns_sync_proxy_sync_schedule_s_v01), sns_sync_proxy_sync_schedule_s_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry SNS_SYNC_PROXY_SVC_message_table_v01[] = {
  {sizeof(sns_sync_proxy_motion_enable_req_msg_v01), sns_sync_proxy_motion_enable_req_msg_data_v01},
  {sizeof(sns_sync_proxy_motion_enable_resp_msg_v01), sns_sync_proxy_motion_enable_resp_msg_data_v01},
  {sizeof(sns_sync_proxy_motion_ind_msg_v01), sns_sync_proxy_motion_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object SNS_SYNC_PROXY_SVC_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *SNS_SYNC_PROXY_SVC_qmi_idl_type_table_object_referenced_tables_v01[] =
{&SNS_SYNC_PROXY_SVC_qmi_idl_type_table_object_v01, &sns_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object SNS_SYNC_PROXY_SVC_qmi_idl_type_table_object_v01 = {
  sizeof(SNS_SYNC_PROXY_SVC_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(SNS_SYNC_PROXY_SVC_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  SNS_SYNC_PROXY_SVC_type_table_v01,
  SNS_SYNC_PROXY_SVC_message_table_v01,
  SNS_SYNC_PROXY_SVC_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry SNS_SYNC_PROXY_SVC_service_command_messages_v01[] = {
  {SNS_SYNC_PROXY_CANCEL_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {SNS_SYNC_PROXY_VERSION_REQ_V01, QMI_IDL_TYPE16(1, 2), 0},
  {SNS_SYNC_PROXY_MOTION_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 0), 138}
};

static const qmi_idl_service_message_table_entry SNS_SYNC_PROXY_SVC_service_response_messages_v01[] = {
  {SNS_SYNC_PROXY_CANCEL_RESP_V01, QMI_IDL_TYPE16(1, 1), 5},
  {SNS_SYNC_PROXY_VERSION_RESP_V01, QMI_IDL_TYPE16(1, 3), 17},
  {SNS_SYNC_PROXY_MOTION_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 1), 12}
};

static const qmi_idl_service_message_table_entry SNS_SYNC_PROXY_SVC_service_indication_messages_v01[] = {
  {SNS_SYNC_PROXY_MOTION_IND_V01, QMI_IDL_TYPE16(0, 2), 14}
};

/*Service Object*/
struct qmi_idl_service_object SNS_SYNC_PROXY_SVC_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  SNS_QMI_SVC_ID_50_V01,
  138,
  { sizeof(SNS_SYNC_PROXY_SVC_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(SNS_SYNC_PROXY_SVC_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(SNS_SYNC_PROXY_SVC_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { SNS_SYNC_PROXY_SVC_service_command_messages_v01, SNS_SYNC_PROXY_SVC_service_response_messages_v01, SNS_SYNC_PROXY_SVC_service_indication_messages_v01},
  &SNS_SYNC_PROXY_SVC_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type SNS_SYNC_PROXY_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SNS_SYNC_PROXY_SVC_V01_IDL_MAJOR_VERS != idl_maj_version || SNS_SYNC_PROXY_SVC_V01_IDL_MINOR_VERS != idl_min_version 
       || SNS_SYNC_PROXY_SVC_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&SNS_SYNC_PROXY_SVC_qmi_idl_service_object_v01;
}

