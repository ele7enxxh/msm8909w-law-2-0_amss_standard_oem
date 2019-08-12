/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S U B S Y S T E M _ C O N T R O L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the ssctl service Data structures.

  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/ssctl/src/subsystem_control_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.7 
   It was generated on: Sat Feb  8 2014 (Spin 0)
   From IDL File: subsystem_control_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "subsystem_control_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
/* 
 * qmi_ssctl_restart_req_msg is empty
 * static const uint8_t qmi_ssctl_restart_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t qmi_ssctl_restart_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ssctl_restart_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* 
 * qmi_ssctl_restart_ind_msg is empty
 * static const uint8_t qmi_ssctl_restart_ind_msg_data_v01[] = {
 * };
 */
  
/* 
 * qmi_ssctl_shutdown_req_msg is empty
 * static const uint8_t qmi_ssctl_shutdown_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t qmi_ssctl_shutdown_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ssctl_shutdown_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* 
 * qmi_ssctl_shutdown_ind_msg is empty
 * static const uint8_t qmi_ssctl_shutdown_ind_msg_data_v01[] = {
 * };
 */
  
/* 
 * qmi_ssctl_get_failure_reason_req_msg is empty
 * static const uint8_t qmi_ssctl_get_failure_reason_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t qmi_ssctl_get_failure_reason_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ssctl_get_failure_reason_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ssctl_get_failure_reason_resp_msg_v01, error_message) - QMI_IDL_OFFSET8(qmi_ssctl_get_failure_reason_resp_msg_v01, error_message_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ssctl_get_failure_reason_resp_msg_v01, error_message),
  QMI_SSCTL_ERROR_MSG_LENGTH_V01,
  QMI_IDL_OFFSET8(qmi_ssctl_get_failure_reason_resp_msg_v01, error_message) - QMI_IDL_OFFSET8(qmi_ssctl_get_failure_reason_resp_msg_v01, error_message_len)
};

static const uint8_t qmi_ssctl_subsys_event_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ssctl_subsys_event_req_msg_v01, subsys_name),
  QMI_SSCTL_SUB_SYS_NAME_LENGTH_V01,
  QMI_IDL_OFFSET8(qmi_ssctl_subsys_event_req_msg_v01, subsys_name) - QMI_IDL_OFFSET8(qmi_ssctl_subsys_event_req_msg_v01, subsys_name_len),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ssctl_subsys_event_req_msg_v01, event),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ssctl_subsys_event_req_msg_v01, evt_driven) - QMI_IDL_OFFSET8(qmi_ssctl_subsys_event_req_msg_v01, evt_driven_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ssctl_subsys_event_req_msg_v01, evt_driven)
};

static const uint8_t qmi_ssctl_subsys_event_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ssctl_subsys_event_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* 
 * qmi_ssctl_subsys_event_comp_ind_msg is empty
 * static const uint8_t qmi_ssctl_subsys_event_comp_ind_msg_data_v01[] = {
 * };
 */
  
/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry ssctl_message_table_v01[] = {
  {sizeof(qmi_ssctl_restart_req_msg_v01), 0},
  {sizeof(qmi_ssctl_restart_resp_msg_v01), qmi_ssctl_restart_resp_msg_data_v01},
  {sizeof(qmi_ssctl_restart_ind_msg_v01), 0},
  {sizeof(qmi_ssctl_shutdown_req_msg_v01), 0},
  {sizeof(qmi_ssctl_shutdown_resp_msg_v01), qmi_ssctl_shutdown_resp_msg_data_v01},
  {sizeof(qmi_ssctl_shutdown_ind_msg_v01), 0},
  {sizeof(qmi_ssctl_get_failure_reason_req_msg_v01), 0},
  {sizeof(qmi_ssctl_get_failure_reason_resp_msg_v01), qmi_ssctl_get_failure_reason_resp_msg_data_v01},
  {sizeof(qmi_ssctl_subsys_event_req_msg_v01), qmi_ssctl_subsys_event_req_msg_data_v01},
  {sizeof(qmi_ssctl_subsys_event_resp_msg_v01), qmi_ssctl_subsys_event_resp_msg_data_v01},
  {sizeof(qmi_ssctl_subsys_event_comp_ind_msg_v01), 0}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object ssctl_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *ssctl_qmi_idl_type_table_object_referenced_tables_v01[] =
{&ssctl_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object ssctl_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(ssctl_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  ssctl_message_table_v01,
  ssctl_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry ssctl_service_command_messages_v01[] = {
  {QMI_SSCTL_RESTART_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_SSCTL_SHUTDOWN_REQ_V01, QMI_IDL_TYPE16(0, 3), 0},
  {QMI_SSCTL_GET_FAILURE_REASON_REQ_V01, QMI_IDL_TYPE16(0, 6), 0},
  {QMI_SSCTL_SUBSYS_EVENT_REQ_V01, QMI_IDL_TYPE16(0, 8), 33}
};

static const qmi_idl_service_message_table_entry ssctl_service_response_messages_v01[] = {
  {QMI_SSCTL_RESTART_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_SSCTL_SHUTDOWN_RESP_V01, QMI_IDL_TYPE16(0, 4), 7},
  {QMI_SSCTL_GET_FAILURE_REASON_RESP_V01, QMI_IDL_TYPE16(0, 7), 101},
  {QMI_SSCTL_SUBSYS_EVENT_RESP_V01, QMI_IDL_TYPE16(0, 9), 7}
};

static const qmi_idl_service_message_table_entry ssctl_service_indication_messages_v01[] = {
  {QMI_SSCTL_RESTART_READY_IND_V01, QMI_IDL_TYPE16(0, 2), 0},
  {QMI_SSCTL_SHUTDOWN_READY_IND_V01, QMI_IDL_TYPE16(0, 5), 0},
  {QMI_SSCTL_SUBSYS_EVENT_COMP_IND_V01, QMI_IDL_TYPE16(0, 10), 0}
};

/*Service Object*/
struct qmi_idl_service_object ssctl_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x2B,
  101,
  { sizeof(ssctl_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ssctl_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ssctl_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { ssctl_service_command_messages_v01, ssctl_service_response_messages_v01, ssctl_service_indication_messages_v01},
  &ssctl_qmi_idl_type_table_object_v01,
  0x02,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type ssctl_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SSCTL_V01_IDL_MAJOR_VERS != idl_maj_version || SSCTL_V01_IDL_MINOR_VERS != idl_min_version 
       || SSCTL_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&ssctl_qmi_idl_service_object_v01;
}

