/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C O M M O N _ F L O W _ C O N T R O L _ M A N A G E M E N T _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the cfcm service Data structures.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/cfcm/src/common_flow_control_management_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2 
   It was generated on: Thu Jan 22 2015 (Spin 1)
   From IDL File: common_flow_control_management_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "common_flow_control_management_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
static const uint8_t cfcm_set_bus_bw_throttling_level_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cfcm_set_bus_bw_throttling_level_req_msg_v01, bus_bw_throttling_level)
};

static const uint8_t cfcm_set_bus_bw_throttling_level_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(cfcm_set_bus_bw_throttling_level_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* 
 * cfcm_get_bus_bw_throttling_level_req_msg is empty
 * static const uint8_t cfcm_get_bus_bw_throttling_level_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t cfcm_get_bus_bw_throttling_level_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(cfcm_get_bus_bw_throttling_level_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(cfcm_get_bus_bw_throttling_level_resp_msg_v01, bus_bw_throttling_level) - QMI_IDL_OFFSET8(cfcm_get_bus_bw_throttling_level_resp_msg_v01, bus_bw_throttling_level_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cfcm_get_bus_bw_throttling_level_resp_msg_v01, bus_bw_throttling_level)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry cfcm_message_table_v01[] = {
  {sizeof(cfcm_set_bus_bw_throttling_level_req_msg_v01), cfcm_set_bus_bw_throttling_level_req_msg_data_v01},
  {sizeof(cfcm_set_bus_bw_throttling_level_resp_msg_v01), cfcm_set_bus_bw_throttling_level_resp_msg_data_v01},
  {sizeof(cfcm_get_bus_bw_throttling_level_req_msg_v01), 0},
  {sizeof(cfcm_get_bus_bw_throttling_level_resp_msg_v01), cfcm_get_bus_bw_throttling_level_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object cfcm_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *cfcm_qmi_idl_type_table_object_referenced_tables_v01[] =
{&cfcm_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object cfcm_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(cfcm_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  cfcm_message_table_v01,
  cfcm_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry cfcm_service_command_messages_v01[] = {
  {QMI_CFCM_BUS_BW_THROTTLING_LEVEL_REQ_MSG_V01, QMI_IDL_TYPE16(0, 0), 7},
  {QMI_CFCM_GET_BW_THROTTLING_LEVEL_REQ_MSG_V01, QMI_IDL_TYPE16(0, 2), 0}
};

static const qmi_idl_service_message_table_entry cfcm_service_response_messages_v01[] = {
  {QMI_CFCM_BUS_BW_THROTTLING_LEVEL_RESP_MSG_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_CFCM_GET_BW_THROTTLING_LEVEL_RESP_MSG_V01, QMI_IDL_TYPE16(0, 3), 14}
};

/*Service Object*/
struct qmi_idl_service_object cfcm_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x36,
  14,
  { sizeof(cfcm_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(cfcm_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { cfcm_service_command_messages_v01, cfcm_service_response_messages_v01, NULL},
  &cfcm_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type cfcm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( CFCM_V01_IDL_MAJOR_VERS != idl_maj_version || CFCM_V01_IDL_MINOR_VERS != idl_min_version 
       || CFCM_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&cfcm_qmi_idl_service_object_v01;
}

