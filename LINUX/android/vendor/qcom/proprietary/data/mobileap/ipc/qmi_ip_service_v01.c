/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q M I _ I P _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the qmi_ip service Data structures.

  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.9
   It was generated on: Wed May 21 2014 (Spin 0)
   From IDL File: qmi_ip_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_ip_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t qmi_ip_client_server_version_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ip_client_server_version_v01, major_version),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ip_client_server_version_v01, minor_version),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qmi_ip_client_session_init_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ip_client_session_init_req_msg_v01, client_version),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ip_client_session_init_req_msg_v01, ip_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ip_client_session_init_req_msg_v01, client_ipv4_addr) - QMI_IDL_OFFSET8(qmi_ip_client_session_init_req_msg_v01, client_ipv4_addr_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ip_client_session_init_req_msg_v01, client_ipv4_addr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_ip_client_session_init_req_msg_v01, client_ipv6_addr) - QMI_IDL_OFFSET8(qmi_ip_client_session_init_req_msg_v01, client_ipv6_addr_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_ip_client_session_init_req_msg_v01, client_ipv6_addr),
  QMI_IP_IPV6_ADDR_LEN_V01
};

static const uint8_t qmi_ip_client_session_init_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ip_client_session_init_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_ip_client_session_init_resp_msg_v01, server_version),
  QMI_IDL_TYPE88(0, 0),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ip_client_session_init_resp_msg_v01, fpop_timeout),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x05,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmi_ip_client_session_init_resp_msg_v01, fpop_timeout_threshold)
};

/* Type Table */
static const qmi_idl_type_table_entry  qmi_ip_type_table_v01[] = {
  {sizeof(qmi_ip_client_server_version_v01), qmi_ip_client_server_version_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry qmi_ip_message_table_v01[] = {
  {sizeof(qmi_ip_client_session_init_req_msg_v01), qmi_ip_client_session_init_req_msg_data_v01},
  {sizeof(qmi_ip_client_session_init_resp_msg_v01), qmi_ip_client_session_init_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object qmi_ip_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *qmi_ip_qmi_idl_type_table_object_referenced_tables_v01[] =
{&qmi_ip_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object qmi_ip_qmi_idl_type_table_object_v01 = {
  sizeof(qmi_ip_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(qmi_ip_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  qmi_ip_type_table_v01,
  qmi_ip_message_table_v01,
  qmi_ip_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry qmi_ip_service_command_messages_v01[] = {
  {QMI_IP_CLIENT_SESSION_INIT_REQ_V01, QMI_IDL_TYPE16(0, 0), 44}
};

static const qmi_idl_service_message_table_entry qmi_ip_service_response_messages_v01[] = {
  {QMI_IP_CLIENT_SESSION_INIT_RESP_V01, QMI_IDL_TYPE16(0, 1), 32}
};

/*Service Object*/
struct qmi_idl_service_object qmi_ip_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x00,
  44,
  { sizeof(qmi_ip_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qmi_ip_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    0 },
  { qmi_ip_service_command_messages_v01, qmi_ip_service_response_messages_v01, NULL},
  &qmi_ip_qmi_idl_type_table_object_v01,
  0x00,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type qmi_ip_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( QMI_IP_V01_IDL_MAJOR_VERS != idl_maj_version || QMI_IP_V01_IDL_MINOR_VERS != idl_min_version
       || QMI_IP_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&qmi_ip_qmi_idl_service_object_v01;
}

