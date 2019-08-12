/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H E A L T H _ M O N I T O R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the hmon service Data structures.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/hmon/src/health_monitor_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.3 
   It was generated on: Tue Mar  3 2015 (Spin 1)
   From IDL File: health_monitor_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "health_monitor_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t hmon_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(hmon_register_req_msg_v01, name) - QMI_IDL_OFFSET8(hmon_register_req_msg_v01, name_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(hmon_register_req_msg_v01, name),
  255,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(hmon_register_req_msg_v01, timeout) - QMI_IDL_OFFSET16RELATIVE(hmon_register_req_msg_v01, timeout_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(hmon_register_req_msg_v01, timeout)
};

static const uint8_t hmon_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(hmon_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* 
 * hmon_health_check_ind_msg is empty
 * static const uint8_t hmon_health_check_ind_msg_data_v01[] = {
 * };
 */
  
static const uint8_t hmon_health_check_complete_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(hmon_health_check_complete_req_msg_v01, result) - QMI_IDL_OFFSET8(hmon_health_check_complete_req_msg_v01, result_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(hmon_health_check_complete_req_msg_v01, result)
};

static const uint8_t hmon_health_check_complete_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(hmon_health_check_complete_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry hmon_message_table_v01[] = {
  {sizeof(hmon_register_req_msg_v01), hmon_register_req_msg_data_v01},
  {sizeof(hmon_register_resp_msg_v01), hmon_register_resp_msg_data_v01},
  {sizeof(hmon_health_check_ind_msg_v01), 0},
  {sizeof(hmon_health_check_complete_req_msg_v01), hmon_health_check_complete_req_msg_data_v01},
  {sizeof(hmon_health_check_complete_resp_msg_v01), hmon_health_check_complete_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object hmon_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *hmon_qmi_idl_type_table_object_referenced_tables_v01[] =
{&hmon_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object hmon_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(hmon_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  hmon_message_table_v01,
  hmon_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry hmon_service_command_messages_v01[] = {
  {QMI_HMON_REG_REQ_V01, QMI_IDL_TYPE16(0, 0), 265},
  {QMI_HMON_HEALTH_CHECK_COMPLETE_REQ_V01, QMI_IDL_TYPE16(0, 3), 7}
};

static const qmi_idl_service_message_table_entry hmon_service_response_messages_v01[] = {
  {QMI_HMON_REG_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_HMON_HEALTH_CHECK_COMPLETE_RESP_V01, QMI_IDL_TYPE16(0, 4), 7}
};

static const qmi_idl_service_message_table_entry hmon_service_indication_messages_v01[] = {
  {QMI_HMON_HEALTH_CHECK_IND_V01, QMI_IDL_TYPE16(0, 2), 0}
};

/*Service Object*/
struct qmi_idl_service_object hmon_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x3C,
  265,
  { sizeof(hmon_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(hmon_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(hmon_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { hmon_service_command_messages_v01, hmon_service_response_messages_v01, hmon_service_indication_messages_v01},
  &hmon_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type hmon_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( HMON_V01_IDL_MAJOR_VERS != idl_maj_version || HMON_V01_IDL_MINOR_VERS != idl_min_version 
       || HMON_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&hmon_qmi_idl_service_object_v01;
}

