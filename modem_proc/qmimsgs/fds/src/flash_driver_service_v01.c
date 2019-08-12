/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        F L A S H _ D R I V E R _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the fds service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/fds/src/flash_driver_service_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.6 
   It was generated on: Fri Nov 15 2013 (Spin 1)
   From IDL File: flash_driver_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "flash_driver_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
/* 
 * fds_scrub_start_req_msg is empty
 * static const uint8_t fds_scrub_start_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t fds_scrub_start_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(fds_scrub_start_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(fds_scrub_start_resp_msg_v01, scrub_token) - QMI_IDL_OFFSET8(fds_scrub_start_resp_msg_v01, scrub_token_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(fds_scrub_start_resp_msg_v01, scrub_token)
};

static const uint8_t fds_scrub_get_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(fds_scrub_get_status_req_msg_v01, scrub_token)
};

static const uint8_t fds_scrub_get_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(fds_scrub_get_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(fds_scrub_get_status_resp_msg_v01, scrub_status) - QMI_IDL_OFFSET8(fds_scrub_get_status_resp_msg_v01, scrub_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(fds_scrub_get_status_resp_msg_v01, scrub_status)
};

/* 
 * fds_scrub_get_sys_info_req_msg is empty
 * static const uint8_t fds_scrub_get_sys_info_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t fds_scrub_get_sys_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, fds_internal_scrub_timer_secs) - QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, fds_internal_scrub_timer_secs_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, fds_internal_scrub_timer_secs),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, fds_minimum_secs_between_scrub) - QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, fds_minimum_secs_between_scrub_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, fds_minimum_secs_between_scrub),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, fds_previous_scrub_time_posix) - QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, fds_previous_scrub_time_posix_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(fds_scrub_get_sys_info_resp_msg_v01, fds_previous_scrub_time_posix)
};

/* 
 * fds_handle_fota_update_req_msg is empty
 * static const uint8_t fds_handle_fota_update_req_msg_data_v01[] = {
 * };
 */
  
static const uint8_t fds_handle_fota_update_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(fds_handle_fota_update_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t fds_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(fds_indication_register_req_msg_v01, need_apps_start_scrub_indication) - QMI_IDL_OFFSET8(fds_indication_register_req_msg_v01, need_apps_start_scrub_indication_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(fds_indication_register_req_msg_v01, need_apps_start_scrub_indication)
};

static const uint8_t fds_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(fds_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t fds_scrub_apps_start_scrub_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(fds_scrub_apps_start_scrub_ind_msg_v01, scrub_token)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry fds_message_table_v01[] = {
  {sizeof(fds_scrub_start_req_msg_v01), 0},
  {sizeof(fds_scrub_start_resp_msg_v01), fds_scrub_start_resp_msg_data_v01},
  {sizeof(fds_scrub_get_status_req_msg_v01), fds_scrub_get_status_req_msg_data_v01},
  {sizeof(fds_scrub_get_status_resp_msg_v01), fds_scrub_get_status_resp_msg_data_v01},
  {sizeof(fds_scrub_get_sys_info_req_msg_v01), 0},
  {sizeof(fds_scrub_get_sys_info_resp_msg_v01), fds_scrub_get_sys_info_resp_msg_data_v01},
  {sizeof(fds_handle_fota_update_req_msg_v01), 0},
  {sizeof(fds_handle_fota_update_resp_msg_v01), fds_handle_fota_update_resp_msg_data_v01},
  {sizeof(fds_indication_register_req_msg_v01), fds_indication_register_req_msg_data_v01},
  {sizeof(fds_indication_register_resp_msg_v01), fds_indication_register_resp_msg_data_v01},
  {sizeof(fds_scrub_apps_start_scrub_ind_msg_v01), fds_scrub_apps_start_scrub_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object fds_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *fds_qmi_idl_type_table_object_referenced_tables_v01[] =
{&fds_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object fds_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(fds_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  fds_message_table_v01,
  fds_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry fds_service_command_messages_v01[] = {
  {QMI_FDS_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_FDS_GET_SUPPORTED_FIELDS_REQ_V01, QMI_IDL_TYPE16(1, 2), 5},
  {QMI_FDS_SCRUB_START_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_FDS_SCRUB_GET_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 2), 7},
  {QMI_FDS_SCRUB_GET_SYS_INFO_REQ_V01, QMI_IDL_TYPE16(0, 4), 0},
  {QMI_FDS_HANDLE_FOTA_UPDATE_REQ_V01, QMI_IDL_TYPE16(0, 6), 0},
  {QMI_FDS_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 8), 4}
};

static const qmi_idl_service_message_table_entry fds_service_response_messages_v01[] = {
  {QMI_FDS_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_FDS_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_FDS_SCRUB_START_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_FDS_SCRUB_GET_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 3), 14},
  {QMI_FDS_SCRUB_GET_SYS_INFO_RESP_V01, QMI_IDL_TYPE16(0, 5), 40},
  {QMI_FDS_HANDLE_FOTA_UPDATE_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_FDS_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 9), 7}
};

static const qmi_idl_service_message_table_entry fds_service_indication_messages_v01[] = {
  {QMI_FDS_SCRUB_APPS_START_SCRUB_IND_V01, QMI_IDL_TYPE16(0, 10), 7}
};

/*Service Object*/
struct qmi_idl_service_object fds_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x2D,
  8204,
  { sizeof(fds_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(fds_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(fds_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { fds_service_command_messages_v01, fds_service_response_messages_v01, fds_service_indication_messages_v01},
  &fds_qmi_idl_type_table_object_v01,
  0x02,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type fds_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( FDS_V01_IDL_MAJOR_VERS != idl_maj_version || FDS_V01_IDL_MINOR_VERS != idl_min_version 
       || FDS_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&fds_qmi_idl_service_object_v01;
}

