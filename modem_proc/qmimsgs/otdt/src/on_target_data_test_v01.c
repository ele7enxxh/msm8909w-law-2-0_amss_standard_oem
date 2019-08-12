/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        O N _ T A R G E T _ D A T A _ T E S T _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the otdt service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/otdt/src/on_target_data_test_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5 
   It was generated on: Sun Sep 29 2013 (Spin 2)
   From IDL File: on_target_data_test_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "on_target_data_test_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t otdt_start_test_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(otdt_start_test_req_msg_v01, test_group),
  TEST_GROUP_MAX_LENGTH_V01,

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(otdt_start_test_req_msg_v01, test_number),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(otdt_start_test_req_msg_v01, task_priority) - QMI_IDL_OFFSET8(otdt_start_test_req_msg_v01, task_priority_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(otdt_start_test_req_msg_v01, task_priority)
};

static const uint8_t otdt_start_test_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(otdt_start_test_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* 
 * otdt_stop_test_req_msg is empty
 * static const uint8_t otdt_stop_test_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t otdt_stop_test_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(otdt_stop_test_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t otdt_test_status_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(otdt_test_status_ind_msg_v01, test_group),
  TEST_GROUP_MAX_LENGTH_V01,

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(otdt_test_status_ind_msg_v01, test_number),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(otdt_test_status_ind_msg_v01, test_result),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(otdt_test_status_ind_msg_v01, dss_errno) - QMI_IDL_OFFSET8(otdt_test_status_ind_msg_v01, dss_errno_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(otdt_test_status_ind_msg_v01, dss_errno)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry otdt_message_table_v01[] = {
  {sizeof(otdt_start_test_req_msg_v01), otdt_start_test_req_msg_data_v01},
  {sizeof(otdt_start_test_resp_msg_v01), otdt_start_test_resp_msg_data_v01},
  {sizeof(otdt_stop_test_req_msg_v01), 0},
  {sizeof(otdt_stop_test_resp_msg_v01), otdt_stop_test_resp_msg_data_v01},
  {sizeof(otdt_test_status_ind_msg_v01), otdt_test_status_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object otdt_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *otdt_qmi_idl_type_table_object_referenced_tables_v01[] =
{&otdt_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object otdt_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(otdt_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  otdt_message_table_v01,
  otdt_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry otdt_service_command_messages_v01[] = {
  {QMI_OTDT_START_TEST_REQ_V01, QMI_IDL_TYPE16(0, 0), 49},
  {QMI_OTDT_STOP_TEST_REQ_V01, QMI_IDL_TYPE16(0, 2), 0}
};

static const qmi_idl_service_message_table_entry otdt_service_response_messages_v01[] = {
  {QMI_OTDT_START_TEST_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_OTDT_STOP_TEST_RESP_V01, QMI_IDL_TYPE16(0, 3), 7}
};

static const qmi_idl_service_message_table_entry otdt_service_indication_messages_v01[] = {
  {QMI_OTDT_TEST_STATUS_IND_V01, QMI_IDL_TYPE16(0, 4), 56}
};

/*Service Object*/
struct qmi_idl_service_object otdt_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x25,
  56,
  { sizeof(otdt_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(otdt_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(otdt_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { otdt_service_command_messages_v01, otdt_service_response_messages_v01, otdt_service_indication_messages_v01},
  &otdt_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type otdt_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( OTDT_V01_IDL_MAJOR_VERS != idl_maj_version || OTDT_V01_IDL_MINOR_VERS != idl_min_version 
       || OTDT_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&otdt_qmi_idl_service_object_v01;
}

