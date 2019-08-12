/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S U B S C R I B E R _ I D E N T I T Y _ M O D U L E _ I N T E R N A L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the sim_internal service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/sim_internal/src/subscriber_identity_module_internal_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2 
   It was generated on: Wed Jul 24 2013 (Spin 1)
   From IDL File: subscriber_identity_module_internal_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "subscriber_identity_module_internal_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t sim_internal_mmgsdi_request_data_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_request_data_type_v01, response_handle),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_request_data_type_v01, client_ref),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_aid_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_aid_type_v01, app_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_aid_type_v01, aid),
  QMI_SIM_INTERNAL_MMGSDI_MAX_AID_LEN_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_aid_type_v01, aid) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_aid_type_v01, aid_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_aid_type_v01, label),
  QMI_SIM_INTERNAL_MMGSDI_MAX_AID_LEN_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_aid_type_v01, label) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_aid_type_v01, label_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_pin_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_info_type_v01, pin_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_info_type_v01, status),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_info_type_v01, pin_replacement),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_info_type_v01, num_retries),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_info_type_v01, num_unblock_retries),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_info_type_v01, valid_num_retries),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_info_type_v01, valid_num_unblock_retries),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_app_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_app_info_type_v01, cached),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_app_info_type_v01, slot),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_app_info_type_v01, pin1),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_app_info_type_v01, pin2),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_app_info_type_v01, universal_pin),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_app_info_type_v01, prov_app),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_app_info_type_v01, app_data),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_app_info_type_v01, hiddenkey),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_app_info_type_v01, dir_index),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_refresh_file_list_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_refresh_file_list_type_v01, file_list),
  QMI_SIM_INTERNAL_MMGSDI_REFRESH_FILE_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_refresh_file_list_type_v01, file_list) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_refresh_file_list_type_v01, file_list_len),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_refresh_file_list_type_v01, file_path),
  QMI_SIM_INTERNAL_MMGSDI_REFRESH_PATH_MAX_LENGTH_V01,
  QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_refresh_file_list_type_v01, file_path) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_refresh_file_list_type_v01, file_path_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_data_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_data_type_v01, data),
  QMI_SIM_INTERNAL_MMGSDI_DATA_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_data_type_v01, data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_data_type_v01, data_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_large_data_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_large_data_type_v01, data),
  ((QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01) & 0xFF), ((QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_large_data_type_v01, data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_large_data_type_v01, data_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_sw_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_sw_type_v01, valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_sw_type_v01, sw1),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_sw_type_v01, sw2),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_response_header_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_response_header_type_v01, mmgsdi_status),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_response_header_type_v01, client_ref),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_response_header_type_v01, client_id),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_response_header_type_v01, session_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_response_header_type_v01, response_len),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_response_header_type_v01, payload_len),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_response_header_type_v01, slot_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_response_header_type_v01, status_word),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_response_header_type_v01, response_handle),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_file_security_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_security_type_v01, protection_method),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_security_type_v01, protection_pin),
  QMI_SIM_INTERNAL_MMGSDI_PIN_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_security_type_v01, protection_pin) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_security_type_v01, protection_pin_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_file_security_access_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_security_access_type_v01, read),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_security_access_type_v01, write),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_security_access_type_v01, increase),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_security_access_type_v01, invalidate_deactivate),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_file_security_access_type_v01, rehabilitate_activate),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_file_status_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_status_type_v01, file_invalidated),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_status_type_v01, read_write_when_invalidated),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_path_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_path_type_v01, path_buf),
  QMI_SIM_INTERNAL_MMGSDI_MAX_PATH_LEN_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_path_type_v01, path_buf) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_path_type_v01, path_buf_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_read_data_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_data_type_v01, accessed_rec_num),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_data_type_v01, accessed_offset),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_data_type_v01, read_data),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_write_data_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_data_type_v01, accessed_rec_num),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_data_type_v01, accessed_offset),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_data_type_v01, write_data_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_cache_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_cache_req_type_v01, session_id),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_cache_req_type_v01, file_num),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_auth_request_data_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_auth_request_data_type_v01, session_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_auth_request_data_type_v01, auth_context),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_auth_request_data_type_v01, auth_req),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_register_for_refresh_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_req_type_v01, session_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_req_type_v01, refresh_files),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_register_for_refresh_req_type_v01, vote_for_init),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_open_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_req_type_v01, client_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_req_type_v01, session_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_req_type_v01, event_handle),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_req_type_v01, set_notify),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_non_prov_app_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_non_prov_app_info_type_v01, app_info),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_non_prov_app_info_type_v01, exclusive_channel),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_open_cnf_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_cnf_type_v01, session_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_cnf_type_v01, channel_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_cnf_type_v01, slot_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_cnf_type_v01, app_info),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_close_cnf_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_cnf_type_v01, session_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_cnf_type_v01, app_info),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_file_attributes_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_attributes_type_v01, file_size),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_attributes_type_v01, file_id),
  QMI_SIM_INTERNAL_MMGSDI_FILE_ID_SIZE_V01,

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_attributes_type_v01, file_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_transparent_file_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_transparent_file_info_type_v01, file_security),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_transparent_file_info_type_v01, file_status),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_linear_fixed_file_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_linear_fixed_file_info_type_v01, file_security),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_linear_fixed_file_info_type_v01, file_status),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_linear_fixed_file_info_type_v01, num_of_rec),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_linear_fixed_file_info_type_v01, rec_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_cyclic_file_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_cyclic_file_info_type_v01, file_security),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_cyclic_file_info_type_v01, file_status),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_cyclic_file_info_type_v01, num_of_rec),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_cyclic_file_info_type_v01, rec_len),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_cyclic_file_info_type_v01, increase_allowed),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_read_transparent_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_type_v01, session_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_type_v01, offset),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_type_v01, req_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_read_record_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_type_v01, session_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_type_v01, record_num),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_type_v01, req_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_write_transparent_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_req_type_v01, session_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_req_type_v01, offset),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_req_type_v01, write_data),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_write_record_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_req_type_v01, session_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_req_type_v01, record_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_req_type_v01, record_num),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_req_type_v01, write_data),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_service_avail_req_data_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_service_avail_req_data_type_v01, session_id),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_service_avail_req_data_type_v01, srvc_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_app_capabilities_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_app_capabilities_type_v01, fdn_enabled),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_app_capabilities_type_v01, bdn_enabled),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_app_capabilities_type_v01, acl_enabled),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_app_capabilities_type_v01, imsi_invalidated),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_increase_request_data_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_request_data_type_v01, session_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_request_data_type_v01, increase_data),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_all_pin_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_all_pin_info_type_v01, pin1),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_all_pin_info_type_v01, pin2),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_all_pin_info_type_v01, universal_pin),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_all_pin_info_type_v01, hiddenkey),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_all_pin_info_type_v01, dir_index),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_toggle_uim_power_control_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_toggle_uim_power_control_req_type_v01, session_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_toggle_uim_power_control_req_type_v01, mode),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_get_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_type_v01, slot_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_type_v01, channel_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_type_v01, app_data),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_type_v01, app_state),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_run_gsm_algo_response_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_run_gsm_algo_response_type_v01, sres_resp),
  QMI_SIM_INTERNAL_MMGSDI_GSM_ALGO_SRES_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_run_gsm_algo_response_type_v01, kc_resp),
  QMI_SIM_INTERNAL_MMGSDI_GSM_ALGO_KC_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_deperso_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_deperso_req_type_v01, session_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_deperso_req_type_v01, feature),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_deperso_req_type_v01, deperso_data),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_proceed_with_refresh_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_proceed_with_refresh_req_type_v01, session_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_proceed_with_refresh_req_type_v01, ok_to_proceed),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_ok_to_refresh_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_req_type_v01, session_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_req_type_v01, ok_to_refresh),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_refresh_complete_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_req_type_v01, session_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_req_type_v01, pass_fail),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_search_offset_data_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_search_offset_data_type_v01, offset_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_search_offset_data_type_v01, offset_data),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_search_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_req_type_v01, session_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_req_type_v01, search_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_req_type_v01, search_rec_num),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_req_type_v01, search_direction),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_req_type_v01, enhanced_search_offset_data),
  QMI_IDL_TYPE88(0, 41),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_req_type_v01, search_pattern),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_seek_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_req_type_v01, session_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_req_type_v01, seek_direction),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_req_type_v01, seek_pattern),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_event_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_event_info_type_v01, client_id),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_event_info_type_v01, session_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_event_info_type_v01, event_handle),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_pin_evt_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_evt_info_type_v01, pin_event_enum),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_evt_info_type_v01, slot),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_evt_info_type_v01, pin_info),
  QMI_IDL_TYPE88(0, 2),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_evt_info_type_v01, aid_type),
  QMI_SIM_INTERNAL_MMGSDI_MAX_APP_INFO_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_evt_info_type_v01, aid_type) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_pin_evt_info_type_v01, aid_type_len),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_pin_evt_info_type_v01, dir_index),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_tp_dl_evt_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_tp_dl_evt_info_type_v01, slot),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_tp_dl_evt_info_type_v01, protocol),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_tp_dl_evt_info_type_v01, ok_dl),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_refresh_evt_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_refresh_evt_info_type_v01, slot),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_refresh_evt_info_type_v01, stage),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_refresh_evt_info_type_v01, mode),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_refresh_evt_info_type_v01, aid),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_refresh_evt_info_type_v01, app_info),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_refresh_evt_info_type_v01, refresh_files),
  QMI_IDL_TYPE88(0, 4),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_refresh_evt_info_type_v01, orig_mode),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_refresh_evt_info_type_v01, plmn_list),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_fdn_evt_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_fdn_evt_info_type_v01, slot),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_fdn_evt_info_type_v01, enabled),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_fdn_evt_info_type_v01, rec_num),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_fdn_evt_info_type_v01, rec_data),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_card_inserted_evt_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_card_inserted_evt_info_type_v01, slot),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_card_inserted_evt_info_type_v01, aid_info),
  QMI_SIM_INTERNAL_MMGSDI_MAX_APP_INFO_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_card_inserted_evt_info_type_v01, aid_info) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_card_inserted_evt_info_type_v01, aid_info_len),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_changed_evt_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_changed_evt_info_type_v01, activated),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_changed_evt_info_type_v01, app_info),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_subscription_ready_evt_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_subscription_ready_evt_info_type_v01, app_info),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_card_err_evt_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_card_err_evt_info_type_v01, slot),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_card_err_evt_info_type_v01, info),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_close_evt_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_evt_info_type_v01, session_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_evt_info_type_v01, slot),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_evt_info_type_v01, app_id),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_session_illegal_subscription_evt_info_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_evt_info_type_v01, session_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_perso_evt_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_perso_evt_info_type_v01, feature),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_perso_evt_info_type_v01, status),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_perso_evt_info_type_v01, num_retries),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_perso_evt_info_type_v01, num_unblock_retries),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_file_update_evt_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, file_enum_file_isvalid),

  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, file_enum_file),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, path_type_file_isvalid),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, file_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, rec_num_isvalid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, rec_num),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, offset_isvalid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, offset),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_file_update_evt_info_type_v01, file_data),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mmgsdi_record_ef_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mmgsdi_record_ef_info_type_v01, num_of_rec),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mmgsdi_record_ef_info_type_v01, rec_len),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mmgsdi_record_ef_info_type_v01, valid_record_list),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_eons_opl_pnn_cache_ready_evt_info_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_eons_opl_pnn_cache_ready_evt_info_type_v01, opl_info),
  QMI_IDL_TYPE88(0, 57),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_eons_opl_pnn_cache_ready_evt_info_type_v01, pnn_info),
  QMI_IDL_TYPE88(0, 57),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_gstk_tal_uim_cmd_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_cmd_type_v01, cmd_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_cmd_type_v01, user_data),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_cmd_type_v01, slot_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_cmd_type_v01, data),
  ((QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01) & 0xFF), ((QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_cmd_type_v01, data) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_cmd_type_v01, data_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_gstk_tal_uim_report_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_report_type_v01, user_data),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_report_type_v01, sw1),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_report_type_v01, sw2),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_report_type_v01, rpt_status),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_report_type_v01, data),
  ((QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01) & 0xFF), ((QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_report_type_v01, data) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_uim_report_type_v01, data_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_gstk_tal_send_pro_cmd_arg_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_pro_cmd_arg_type_v01, slot),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_pro_cmd_arg_type_v01, data),
  ((QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01) & 0xFF), ((QMI_SIM_INTERNAL_MMGSDI_LARGE_DATA_MAX_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_pro_cmd_arg_type_v01, data) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_pro_cmd_arg_type_v01, data_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_gstk_sw_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_sw_type_v01, sw1),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_sw_type_v01, sw2),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t sim_internal_mmgsdi_naa_refresh_req_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_req_type_v01, client_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_req_type_v01, slot),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_req_type_v01, aid),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_req_type_v01, mode),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_req_type_v01, file_list),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_naa_refresh_req_type_v01, num_files),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_v01, event_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_v01, client_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_open_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_req_msg_v01, session_open),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_req_msg_v01, non_prov_app_data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_req_msg_v01, non_prov_app_data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_req_msg_v01, non_prov_app_data),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_open_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_open_req_msg_v01, token_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_open_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_open_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_open_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_ind_msg_v01, session_open_cnf),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_ind_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_open_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_close_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_req_msg_v01, session_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_close_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_close_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_ind_msg_v01, session_close_cnf),
  QMI_IDL_TYPE88(0, 21),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_ind_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_close_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_get_all_available_apps_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_req_msg_v01, client_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_get_all_available_apps_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_get_all_available_apps_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_1_app_info) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_1_app_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_1_app_info),
  QMI_SIM_INTERNAL_MMGSDI_MAX_APP_INFO_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_1_app_info) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_1_app_info_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_2_app_info) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_2_app_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_2_app_info),
  QMI_SIM_INTERNAL_MMGSDI_MAX_APP_INFO_V01,
  QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_2_app_info) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, card_2_app_info_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, token_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_get_all_available_apps_sync_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_req_msg_v01, client_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_1_app_info) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_1_app_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_1_app_info),
  QMI_SIM_INTERNAL_MMGSDI_MAX_APP_INFO_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_1_app_info) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_1_app_info_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_2_app_info) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_2_app_info_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_2_app_info),
  QMI_SIM_INTERNAL_MMGSDI_MAX_APP_INFO_V01,
  QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_2_app_info) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, card_2_app_info_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, token_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_file_attr_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, session_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, file_enum_file_valid)),
  0x10,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, path_type_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, path_type_file_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_file_attr_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_file_attr_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, file_path),
  QMI_SIM_INTERNAL_MMGSDI_MAX_FILE_PATHS_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, file_path) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, file_path_len),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, file_attr) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, file_attr_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, file_attr),
  QMI_IDL_TYPE88(0, 22),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, raw_file_attr_data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, raw_file_attr_data_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, raw_file_attr_data),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, file_enum_file_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, path_type_file_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, transparent_file_info) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, transparent_file_info_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, transparent_file_info),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, linear_fixed_file_info) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, linear_fixed_file_info_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, linear_fixed_file_info),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, cyclic_file_info) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, cyclic_file_info_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, cyclic_file_info),
  QMI_IDL_TYPE88(0, 25),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, token_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_transparent_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, read_req),
  QMI_IDL_TYPE88(0, 26),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, file_enum_file_valid)),
  0x10,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, path_type_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, path_type_file_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_transparent_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_transparent_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, read_resp_data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, read_resp_data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, read_resp_data),
  QMI_IDL_TYPE88(0, 13),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, file_enum_file_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, path_type_file_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, token_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_record_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, read_req),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, file_enum_file_valid)),
  0x10,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, path_type_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, path_type_file_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_record_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_record_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_ind_msg_v01, read_resp_data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_ind_msg_v01, read_resp_data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_record_ind_msg_v01, read_resp_data),
  QMI_IDL_TYPE88(0, 13),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_record_ind_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_record_ind_msg_v01, file_enum_file_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_read_record_ind_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_record_ind_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_record_ind_msg_v01, path_type_file_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_read_record_ind_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_record_ind_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_record_ind_msg_v01, token_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_read_record_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_write_transparent_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, write_req),
  QMI_IDL_TYPE88(0, 28),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, file_enum_file_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, path_type_file_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_write_transparent_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_write_transparent_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_write_transparent_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, write_resp_data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, write_resp_data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, write_resp_data),
  QMI_IDL_TYPE88(0, 14),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, file_enum_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, file_enum_file_valid)),
  0x11,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, path_type_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, path_type_file_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, token_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_write_record_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_req_msg_v01, write_req),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_record_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_record_req_msg_v01, file_enum_file_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_write_record_req_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_record_req_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_record_req_msg_v01, path_type_file_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_write_record_req_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_record_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_record_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_write_record_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_write_record_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_write_record_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, write_resp_data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, write_resp_data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, write_resp_data),
  QMI_IDL_TYPE88(0, 14),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, file_enum_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, file_enum_file_valid)),
  0x11,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, path_type_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, path_type_file_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, token_valid)),
  0x13,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_record_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_cache_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_req_msg_v01, read_req),
  QMI_IDL_TYPE88(0, 15),

  0x02,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_req_msg_v01, read_data_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_cache_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, read_data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, read_data_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, read_data),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, token_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_read_cache_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_cache_file_size_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_req_msg_v01, read_req),
  QMI_IDL_TYPE88(0, 15),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, file_size) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, file_size_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, file_size),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, token_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_write_cache_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_cache_req_msg_v01, write_req),
  QMI_IDL_TYPE88(0, 15),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_cache_req_msg_v01, write_data),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_cache_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_write_cache_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_write_cache_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_write_cache_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_cache_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_cache_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_cache_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_cache_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_cache_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_cache_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_write_cache_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_is_service_available_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_req_msg_v01, srvc_req),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_is_service_available_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_is_service_available_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01, srvc_available) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01, srvc_available_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01, srvc_available),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_app_capabilities_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_req_msg_v01, session_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01, app_capabilities) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01, app_capabilities_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01, app_capabilities),
  QMI_IDL_TYPE88(0, 31),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_app_capabilities_sync_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_req_msg_v01, session_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, app_capabilities) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, app_capabilities_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, app_capabilities),
  QMI_IDL_TYPE88(0, 31),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, token_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_increase_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_req_msg_v01, increase_req_data),
  QMI_IDL_TYPE88(0, 32),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_req_msg_v01, file_enum_file_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_increase_req_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_req_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_req_msg_v01, path_type_file_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_increase_req_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_increase_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_increase_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_increase_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_ind_msg_v01, increase_data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_ind_msg_v01, increase_data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_increase_ind_msg_v01, increase_data),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_ind_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_ind_msg_v01, file_enum_file_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_increase_ind_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_ind_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_ind_msg_v01, path_type_file_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_increase_ind_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_ind_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_increase_ind_msg_v01, token_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_increase_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_all_pin_status_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_req_msg_v01, session_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01, all_pin_info) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01, all_pin_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01, all_pin_info),
  QMI_IDL_TYPE88(0, 33),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_toggle_uim_power_control_sync_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 34),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, in_TC) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, in_TC_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, in_TC),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, token_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_info_sync_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, query_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, session_id) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, session_id_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, session_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, session_type) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, session_type_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, session_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_get_info_sync_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, get_info) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, get_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, get_info),
  QMI_IDL_TYPE88(0, 35),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, token_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_illegal_subscription_sync_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_req_msg_v01, session_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_uicc_authenticate_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_req_msg_v01, auth_req_data),
  QMI_IDL_TYPE88(0, 16),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_uicc_authenticate_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_uicc_authenticate_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_uicc_authenticate_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01, data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01, data_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01, data),
  ((QMI_SIM_INTERNAL_MMGSDI_AUTH_RESP_MAX_LENGTH_V01) & 0xFF), ((QMI_SIM_INTERNAL_MMGSDI_AUTH_RESP_MAX_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01, data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01, data_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_usim_authenticate_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_req_msg_v01, auth_req_data),
  QMI_IDL_TYPE88(0, 16),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_usim_authenticate_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_usim_authenticate_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_usim_authenticate_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_usim_authenticate_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_usim_authenticate_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01, data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01, data_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01, data),
  ((QMI_SIM_INTERNAL_MMGSDI_AUTH_RESP_MAX_LENGTH_V01) & 0xFF), ((QMI_SIM_INTERNAL_MMGSDI_AUTH_RESP_MAX_LENGTH_V01) >> 8),
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01, data) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01, data_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_run_gsm_algo_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_req_msg_v01, session_id),

  0x03,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_req_msg_v01, rand_data),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_run_gsm_algo_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_run_gsm_algo_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_run_gsm_algo_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01, run_gsm_algo_resp) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01, run_gsm_algo_resp_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01, run_gsm_algo_resp),
  QMI_IDL_TYPE88(0, 36),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_v01, ota_deperso_req),
  QMI_IDL_TYPE88(0, 37),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01, ota_deperso_resp) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01, ota_deperso_resp_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01, ota_deperso_resp),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_register_for_refresh_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_req_msg_v01, refresh_req),
  QMI_IDL_TYPE88(0, 17),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_refresh_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_refresh_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_register_for_refresh_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_register_for_refresh_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_register_for_refresh_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01, orig_refresh_req) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01, orig_refresh_req_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01, orig_refresh_req),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_v01, refresh_req),
  QMI_IDL_TYPE88(0, 17),

  0x03,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_v01, proceed_with_refresh),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_v01, refresh_req),
  QMI_IDL_TYPE88(0, 38),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01, orig_refresh_req) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01, orig_refresh_req_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01, orig_refresh_req),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_ok_to_refresh_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_req_msg_v01, refresh_req),
  QMI_IDL_TYPE88(0, 39),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01, orig_refresh_req) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01, orig_refresh_req_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01, orig_refresh_req),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_refresh_complete_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_req_msg_v01, refresh_req),
  QMI_IDL_TYPE88(0, 40),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_refresh_complete_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_refresh_complete_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01, orig_refresh_req) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01, orig_refresh_req_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01, orig_refresh_req),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_v01, deregister_req),
  QMI_IDL_TYPE88(0, 17),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_search_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_req_msg_v01, search_req),
  QMI_IDL_TYPE88(0, 42),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_req_msg_v01, file_enum_file_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_search_req_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_req_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_req_msg_v01, path_type_file_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_search_req_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_search_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_search_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_search_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_ind_msg_v01, searched_record_nums) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_ind_msg_v01, searched_record_nums_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_search_ind_msg_v01, searched_record_nums),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_ind_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_ind_msg_v01, file_enum_file_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_search_ind_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_ind_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_ind_msg_v01, path_type_file_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_search_ind_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_ind_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_search_ind_msg_v01, token_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_search_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_seek_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_req_msg_v01, seek_req),
  QMI_IDL_TYPE88(0, 43),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_seek_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_seek_req_msg_v01, file_enum_file_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_seek_req_msg_v01, file_enum_file),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_seek_req_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_seek_req_msg_v01, path_type_file_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_seek_req_msg_v01, path_type_file),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_seek_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_seek_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_seek_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_seek_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_seek_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_register_for_file_update_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, session_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, file_enum_file_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, file_enum_file),
  QMI_SIM_INTERNAL_MMGSDI_MAX_FILE_LIST_V01,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, file_enum_file) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, file_enum_file_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, path_type_file_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, path_type_file),
  QMI_SIM_INTERNAL_MMGSDI_MAX_FILE_LIST_V01,
  QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, path_type_file) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, path_type_file_len),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, token_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_register_for_file_update_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_register_for_file_update_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_ind_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_file_update_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_event_report_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_info),
  QMI_IDL_TYPE88(0, 44),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_pin) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_pin_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_pin),
  QMI_IDL_TYPE88(0, 45),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_terminal_profile) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_terminal_profile_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_terminal_profile),
  QMI_IDL_TYPE88(0, 46),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_refresh) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_refresh_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_refresh),
  QMI_IDL_TYPE88(0, 47),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_fdn) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_fdn_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_fdn),
  QMI_IDL_TYPE88(0, 48),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_card_inserted) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_card_inserted_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_card_inserted),
  QMI_IDL_TYPE88(0, 49),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_session_changed) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_session_changed_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_session_changed),
  QMI_IDL_TYPE88(0, 50),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_subscription_ready) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_subscription_ready_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_subscription_ready),
  QMI_IDL_TYPE88(0, 51),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_card_error) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_card_error_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_card_error),
  QMI_IDL_TYPE88(0, 52),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_session_close) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_session_close_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_session_close),
  QMI_IDL_TYPE88(0, 53),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_perso) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_perso_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_perso),
  QMI_IDL_TYPE88(0, 55),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_illegal_session) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_illegal_session_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_illegal_session),
  QMI_IDL_TYPE88(0, 54),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_update) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_update_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_update),
  QMI_IDL_TYPE88(0, 56),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_eons_opl_pnn_cache_ready) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_eons_opl_pnn_cache_ready_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_eons_opl_pnn_cache_ready),
  QMI_IDL_TYPE88(0, 58),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_test_mcc_status) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_test_mcc_status_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_event_report_ind_msg_v01, evt_test_mcc_status)
};

static const uint8_t sim_internal_gstk_tal_client_registration_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_req_msg_v01, client_type),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_req_msg_v01, gstk_tal_handle),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_req_msg_v01, tech_support_mask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_req_msg_v01, token)
};

static const uint8_t sim_internal_gstk_tal_client_registration_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_resp_msg_v01, gstk_status) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_resp_msg_v01, gstk_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_resp_msg_v01, gstk_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_client_registration_resp_msg_v01, token)
};

static const uint8_t sim_internal_gstk_tal_send_cmd_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_cmd_req_msg_v01, client_type),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_cmd_req_msg_v01, tal_cmd),
  QMI_IDL_TYPE88(0, 59),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_gstk_tal_send_cmd_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_gstk_tal_send_cmd_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_gstk_tal_send_cmd_req_msg_v01, token)
};

static const uint8_t sim_internal_gstk_tal_send_cmd_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_cmd_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_cmd_resp_msg_v01, gstk_status) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_cmd_resp_msg_v01, gstk_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_cmd_resp_msg_v01, gstk_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_cmd_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_cmd_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_send_cmd_resp_msg_v01, token)
};

static const uint8_t sim_internal_gstk_tal_notify_rat_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_req_msg_v01, client_type),

  0x02,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_req_msg_v01, rat),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_req_msg_v01, token)
};

static const uint8_t sim_internal_gstk_tal_notify_rat_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_resp_msg_v01, gstk_status) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_resp_msg_v01, gstk_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_resp_msg_v01, gstk_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_rat_resp_msg_v01, token)
};

static const uint8_t sim_internal_gstk_tal_notify_ls_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_req_msg_v01, client_type),

  0x02,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_req_msg_v01, ls),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_req_msg_v01, token)
};

static const uint8_t sim_internal_gstk_tal_notify_ls_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_resp_msg_v01, gstk_status) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_resp_msg_v01, gstk_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_resp_msg_v01, gstk_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_notify_ls_resp_msg_v01, token)
};

static const uint8_t sim_internal_gstk_tal_event_report_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_event_report_ind_msg_v01, event_handle),

  0x02,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_event_report_ind_msg_v01, cmd_group),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_gstk_tal_event_report_ind_msg_v01, tal_uim_report) - QMI_IDL_OFFSET8(sim_internal_gstk_tal_event_report_ind_msg_v01, tal_uim_report_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_gstk_tal_event_report_ind_msg_v01, tal_uim_report),
  QMI_IDL_TYPE88(0, 60),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_gstk_tal_event_report_ind_msg_v01, tal_pro_cmd) - QMI_IDL_OFFSET16RELATIVE(sim_internal_gstk_tal_event_report_ind_msg_v01, tal_pro_cmd_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_gstk_tal_event_report_ind_msg_v01, tal_pro_cmd),
  QMI_IDL_TYPE88(0, 61),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_gstk_tal_event_report_ind_msg_v01, tal_intermediate_sw) - QMI_IDL_OFFSET16RELATIVE(sim_internal_gstk_tal_event_report_ind_msg_v01, tal_intermediate_sw_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_gstk_tal_event_report_ind_msg_v01, tal_intermediate_sw),
  QMI_IDL_TYPE88(0, 62),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_gstk_tal_event_report_ind_msg_v01, proactive_cmd_pending) - QMI_IDL_OFFSET16RELATIVE(sim_internal_gstk_tal_event_report_ind_msg_v01, proactive_cmd_pending_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_gstk_tal_event_report_ind_msg_v01, proactive_cmd_pending)
};

static const uint8_t sim_internal_mmgsdi_get_protocol_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_req_msg_v01, client_id),

  0x02,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_req_msg_v01, slot),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_req_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_get_protocol_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, protocol) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, protocol_valid)),
  0x11,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, protocol),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, token_valid)),
  0x12,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_get_protocol_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01, orig_refresh_req) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01, orig_refresh_req_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01, orig_refresh_req),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_naa_refresh_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_req_msg_v01, req_data),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_req_msg_v01, refresh_req),
  QMI_IDL_TYPE88(0, 63),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_naa_refresh_req_msg_v01, token) - QMI_IDL_OFFSET16RELATIVE(sim_internal_mmgsdi_naa_refresh_req_msg_v01, token_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(sim_internal_mmgsdi_naa_refresh_req_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_naa_refresh_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_resp_msg_v01, mmgsdi_status) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_resp_msg_v01, mmgsdi_status_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_resp_msg_v01, mmgsdi_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_resp_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_resp_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_resp_msg_v01, token)
};

static const uint8_t sim_internal_mmgsdi_naa_refresh_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_ind_msg_v01, response_header),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_ind_msg_v01, orig_refresh_req) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_ind_msg_v01, orig_refresh_req_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_ind_msg_v01, orig_refresh_req),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_ind_msg_v01, token) - QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_ind_msg_v01, token_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(sim_internal_mmgsdi_naa_refresh_ind_msg_v01, token)
};

/* Type Table */
static const qmi_idl_type_table_entry  sim_internal_type_table_v01[] = {
  {sizeof(sim_internal_mmgsdi_request_data_type_v01), sim_internal_mmgsdi_request_data_type_data_v01},
  {sizeof(sim_internal_mmgsdi_aid_type_v01), sim_internal_mmgsdi_aid_type_data_v01},
  {sizeof(sim_internal_mmgsdi_pin_info_type_v01), sim_internal_mmgsdi_pin_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_app_info_type_v01), sim_internal_mmgsdi_app_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_refresh_file_list_type_v01), sim_internal_mmgsdi_refresh_file_list_type_data_v01},
  {sizeof(sim_internal_mmgsdi_data_type_v01), sim_internal_mmgsdi_data_type_data_v01},
  {sizeof(sim_internal_mmgsdi_large_data_type_v01), sim_internal_mmgsdi_large_data_type_data_v01},
  {sizeof(sim_internal_mmgsdi_sw_type_v01), sim_internal_mmgsdi_sw_type_data_v01},
  {sizeof(sim_internal_mmgsdi_response_header_type_v01), sim_internal_mmgsdi_response_header_type_data_v01},
  {sizeof(sim_internal_mmgsdi_file_security_type_v01), sim_internal_mmgsdi_file_security_type_data_v01},
  {sizeof(sim_internal_mmgsdi_file_security_access_type_v01), sim_internal_mmgsdi_file_security_access_type_data_v01},
  {sizeof(sim_internal_mmgsdi_file_status_type_v01), sim_internal_mmgsdi_file_status_type_data_v01},
  {sizeof(sim_internal_mmgsdi_path_type_v01), sim_internal_mmgsdi_path_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_data_type_v01), sim_internal_mmgsdi_session_read_data_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_data_type_v01), sim_internal_mmgsdi_session_write_data_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_cache_req_type_v01), sim_internal_mmgsdi_session_cache_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_auth_request_data_type_v01), sim_internal_mmgsdi_auth_request_data_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_refresh_req_type_v01), sim_internal_mmgsdi_session_register_for_refresh_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_open_req_type_v01), sim_internal_mmgsdi_session_open_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_non_prov_app_info_type_v01), sim_internal_mmgsdi_non_prov_app_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_open_cnf_type_v01), sim_internal_mmgsdi_session_open_cnf_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_close_cnf_type_v01), sim_internal_mmgsdi_session_close_cnf_type_data_v01},
  {sizeof(sim_internal_mmgsdi_file_attributes_type_v01), sim_internal_mmgsdi_file_attributes_type_data_v01},
  {sizeof(sim_internal_mmgsdi_transparent_file_info_type_v01), sim_internal_mmgsdi_transparent_file_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_linear_fixed_file_info_type_v01), sim_internal_mmgsdi_linear_fixed_file_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_cyclic_file_info_type_v01), sim_internal_mmgsdi_cyclic_file_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_transparent_req_type_v01), sim_internal_mmgsdi_session_read_transparent_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_record_req_type_v01), sim_internal_mmgsdi_session_read_record_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_transparent_req_type_v01), sim_internal_mmgsdi_session_write_transparent_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_record_req_type_v01), sim_internal_mmgsdi_session_write_record_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_service_avail_req_data_type_v01), sim_internal_mmgsdi_service_avail_req_data_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_app_capabilities_type_v01), sim_internal_mmgsdi_session_app_capabilities_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_increase_request_data_type_v01), sim_internal_mmgsdi_session_increase_request_data_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_all_pin_info_type_v01), sim_internal_mmgsdi_session_all_pin_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_toggle_uim_power_control_req_type_v01), sim_internal_mmgsdi_toggle_uim_power_control_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_info_type_v01), sim_internal_mmgsdi_session_get_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_run_gsm_algo_response_type_v01), sim_internal_mmgsdi_run_gsm_algo_response_type_data_v01},
  {sizeof(sim_internal_mmgsdi_deperso_req_type_v01), sim_internal_mmgsdi_deperso_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_proceed_with_refresh_req_type_v01), sim_internal_mmgsdi_session_proceed_with_refresh_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_ok_to_refresh_req_type_v01), sim_internal_mmgsdi_session_ok_to_refresh_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_refresh_complete_req_type_v01), sim_internal_mmgsdi_session_refresh_complete_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_search_offset_data_type_v01), sim_internal_mmgsdi_search_offset_data_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_search_req_type_v01), sim_internal_mmgsdi_session_search_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_seek_req_type_v01), sim_internal_mmgsdi_session_seek_req_type_data_v01},
  {sizeof(sim_internal_mmgsdi_event_info_type_v01), sim_internal_mmgsdi_event_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_pin_evt_info_type_v01), sim_internal_mmgsdi_pin_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_tp_dl_evt_info_type_v01), sim_internal_mmgsdi_tp_dl_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_refresh_evt_info_type_v01), sim_internal_mmgsdi_refresh_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_fdn_evt_info_type_v01), sim_internal_mmgsdi_fdn_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_card_inserted_evt_info_type_v01), sim_internal_mmgsdi_card_inserted_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_changed_evt_info_type_v01), sim_internal_mmgsdi_session_changed_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_subscription_ready_evt_info_type_v01), sim_internal_mmgsdi_subscription_ready_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_card_err_evt_info_type_v01), sim_internal_mmgsdi_card_err_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_close_evt_info_type_v01), sim_internal_mmgsdi_session_close_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_session_illegal_subscription_evt_info_type_v01), sim_internal_mmgsdi_session_illegal_subscription_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_perso_evt_info_type_v01), sim_internal_mmgsdi_perso_evt_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_file_update_evt_info_type_v01), sim_internal_mmgsdi_file_update_evt_info_type_data_v01},
  {sizeof(mmgsdi_record_ef_info_type_v01), mmgsdi_record_ef_info_type_data_v01},
  {sizeof(sim_internal_mmgsdi_eons_opl_pnn_cache_ready_evt_info_type_v01), sim_internal_mmgsdi_eons_opl_pnn_cache_ready_evt_info_type_data_v01},
  {sizeof(sim_internal_gstk_tal_uim_cmd_type_v01), sim_internal_gstk_tal_uim_cmd_type_data_v01},
  {sizeof(sim_internal_gstk_tal_uim_report_type_v01), sim_internal_gstk_tal_uim_report_type_data_v01},
  {sizeof(sim_internal_gstk_tal_send_pro_cmd_arg_type_v01), sim_internal_gstk_tal_send_pro_cmd_arg_type_data_v01},
  {sizeof(sim_internal_gstk_sw_type_v01), sim_internal_gstk_sw_type_data_v01},
  {sizeof(sim_internal_mmgsdi_naa_refresh_req_type_v01), sim_internal_mmgsdi_naa_refresh_req_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry sim_internal_message_table_v01[] = {
  {sizeof(sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_v01), sim_internal_mmgsdi_client_id_and_evt_reg_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_v01), sim_internal_mmgsdi_client_id_and_evt_reg_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_v01), sim_internal_mmgsdi_client_id_and_evt_reg_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_v01), sim_internal_mmgsdi_client_id_and_evt_dereg_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_v01), sim_internal_mmgsdi_client_id_and_evt_dereg_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_v01), sim_internal_mmgsdi_client_id_and_evt_dereg_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_open_req_msg_v01), sim_internal_mmgsdi_session_open_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_open_resp_msg_v01), sim_internal_mmgsdi_session_open_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_open_ind_msg_v01), sim_internal_mmgsdi_session_open_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_close_req_msg_v01), sim_internal_mmgsdi_session_close_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_close_resp_msg_v01), sim_internal_mmgsdi_session_close_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_close_ind_msg_v01), sim_internal_mmgsdi_session_close_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_get_all_available_apps_req_msg_v01), sim_internal_mmgsdi_get_all_available_apps_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_get_all_available_apps_resp_msg_v01), sim_internal_mmgsdi_get_all_available_apps_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_get_all_available_apps_ind_msg_v01), sim_internal_mmgsdi_get_all_available_apps_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_get_all_available_apps_sync_req_msg_v01), sim_internal_mmgsdi_get_all_available_apps_sync_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_v01), sim_internal_mmgsdi_get_all_available_apps_sync_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_file_attr_req_msg_v01), sim_internal_mmgsdi_session_get_file_attr_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_file_attr_resp_msg_v01), sim_internal_mmgsdi_session_get_file_attr_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_file_attr_ind_msg_v01), sim_internal_mmgsdi_session_get_file_attr_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_transparent_req_msg_v01), sim_internal_mmgsdi_session_read_transparent_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_transparent_resp_msg_v01), sim_internal_mmgsdi_session_read_transparent_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_transparent_ind_msg_v01), sim_internal_mmgsdi_session_read_transparent_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_record_req_msg_v01), sim_internal_mmgsdi_session_read_record_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_record_resp_msg_v01), sim_internal_mmgsdi_session_read_record_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_record_ind_msg_v01), sim_internal_mmgsdi_session_read_record_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_transparent_req_msg_v01), sim_internal_mmgsdi_session_write_transparent_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_transparent_resp_msg_v01), sim_internal_mmgsdi_session_write_transparent_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_transparent_ind_msg_v01), sim_internal_mmgsdi_session_write_transparent_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_record_req_msg_v01), sim_internal_mmgsdi_session_write_record_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_record_resp_msg_v01), sim_internal_mmgsdi_session_write_record_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_record_ind_msg_v01), sim_internal_mmgsdi_session_write_record_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_cache_req_msg_v01), sim_internal_mmgsdi_session_read_cache_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_cache_resp_msg_v01), sim_internal_mmgsdi_session_read_cache_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_cache_file_size_req_msg_v01), sim_internal_mmgsdi_session_read_cache_file_size_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_v01), sim_internal_mmgsdi_session_read_cache_file_size_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_cache_req_msg_v01), sim_internal_mmgsdi_session_write_cache_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_write_cache_resp_msg_v01), sim_internal_mmgsdi_session_write_cache_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_is_service_available_req_msg_v01), sim_internal_mmgsdi_session_is_service_available_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_is_service_available_resp_msg_v01), sim_internal_mmgsdi_session_is_service_available_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_is_service_available_ind_msg_v01), sim_internal_mmgsdi_session_is_service_available_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_app_capabilities_req_msg_v01), sim_internal_mmgsdi_session_get_app_capabilities_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_v01), sim_internal_mmgsdi_session_get_app_capabilities_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_v01), sim_internal_mmgsdi_session_get_app_capabilities_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_app_capabilities_sync_req_msg_v01), sim_internal_mmgsdi_session_get_app_capabilities_sync_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_v01), sim_internal_mmgsdi_session_get_app_capabilities_sync_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_increase_req_msg_v01), sim_internal_mmgsdi_session_increase_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_increase_resp_msg_v01), sim_internal_mmgsdi_session_increase_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_increase_ind_msg_v01), sim_internal_mmgsdi_session_increase_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_all_pin_status_req_msg_v01), sim_internal_mmgsdi_session_get_all_pin_status_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_v01), sim_internal_mmgsdi_session_get_all_pin_status_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_v01), sim_internal_mmgsdi_session_get_all_pin_status_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_req_msg_v01), sim_internal_mmgsdi_session_toggle_uim_power_control_sync_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_v01), sim_internal_mmgsdi_session_toggle_uim_power_control_sync_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_info_sync_req_msg_v01), sim_internal_mmgsdi_session_get_info_sync_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_get_info_sync_resp_msg_v01), sim_internal_mmgsdi_session_get_info_sync_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_illegal_subscription_sync_req_msg_v01), sim_internal_mmgsdi_session_illegal_subscription_sync_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_v01), sim_internal_mmgsdi_session_illegal_subscription_sync_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_uicc_authenticate_req_msg_v01), sim_internal_mmgsdi_session_uicc_authenticate_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_v01), sim_internal_mmgsdi_session_uicc_authenticate_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_v01), sim_internal_mmgsdi_session_uicc_authenticate_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_usim_authenticate_req_msg_v01), sim_internal_mmgsdi_session_usim_authenticate_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_usim_authenticate_resp_msg_v01), sim_internal_mmgsdi_session_usim_authenticate_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_usim_authenticate_ind_msg_v01), sim_internal_mmgsdi_session_usim_authenticate_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_run_gsm_algo_req_msg_v01), sim_internal_mmgsdi_session_run_gsm_algo_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_v01), sim_internal_mmgsdi_session_run_gsm_algo_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_v01), sim_internal_mmgsdi_session_run_gsm_algo_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_v01), sim_internal_mmgsdi_session_perso_ota_deperso_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_v01), sim_internal_mmgsdi_session_perso_ota_deperso_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_v01), sim_internal_mmgsdi_session_perso_ota_deperso_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_refresh_req_msg_v01), sim_internal_mmgsdi_session_register_for_refresh_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_refresh_resp_msg_v01), sim_internal_mmgsdi_session_register_for_refresh_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_refresh_ind_msg_v01), sim_internal_mmgsdi_session_register_for_refresh_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_v01), sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_v01), sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_v01), sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_v01), sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_v01), sim_internal_mmgsdi_session_ok_to_proceed_with_refresh_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_ok_to_refresh_req_msg_v01), sim_internal_mmgsdi_session_ok_to_refresh_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_v01), sim_internal_mmgsdi_session_ok_to_refresh_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_v01), sim_internal_mmgsdi_session_ok_to_refresh_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_refresh_complete_req_msg_v01), sim_internal_mmgsdi_session_refresh_complete_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_refresh_complete_resp_msg_v01), sim_internal_mmgsdi_session_refresh_complete_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_refresh_complete_ind_msg_v01), sim_internal_mmgsdi_session_refresh_complete_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_v01), sim_internal_mmgsdi_session_deregister_for_refresh_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_v01), sim_internal_mmgsdi_session_deregister_for_refresh_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_v01), sim_internal_mmgsdi_session_deregister_for_refresh_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_search_req_msg_v01), sim_internal_mmgsdi_session_search_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_search_resp_msg_v01), sim_internal_mmgsdi_session_search_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_search_ind_msg_v01), sim_internal_mmgsdi_session_search_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_seek_req_msg_v01), sim_internal_mmgsdi_session_seek_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_seek_resp_msg_v01), sim_internal_mmgsdi_session_seek_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_file_update_req_msg_v01), sim_internal_mmgsdi_session_register_for_file_update_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_file_update_resp_msg_v01), sim_internal_mmgsdi_session_register_for_file_update_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_file_update_ind_msg_v01), sim_internal_mmgsdi_session_register_for_file_update_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_event_report_ind_msg_v01), sim_internal_mmgsdi_event_report_ind_msg_data_v01},
  {sizeof(sim_internal_gstk_tal_client_registration_req_msg_v01), sim_internal_gstk_tal_client_registration_req_msg_data_v01},
  {sizeof(sim_internal_gstk_tal_client_registration_resp_msg_v01), sim_internal_gstk_tal_client_registration_resp_msg_data_v01},
  {sizeof(sim_internal_gstk_tal_send_cmd_req_msg_v01), sim_internal_gstk_tal_send_cmd_req_msg_data_v01},
  {sizeof(sim_internal_gstk_tal_send_cmd_resp_msg_v01), sim_internal_gstk_tal_send_cmd_resp_msg_data_v01},
  {sizeof(sim_internal_gstk_tal_notify_rat_req_msg_v01), sim_internal_gstk_tal_notify_rat_req_msg_data_v01},
  {sizeof(sim_internal_gstk_tal_notify_rat_resp_msg_v01), sim_internal_gstk_tal_notify_rat_resp_msg_data_v01},
  {sizeof(sim_internal_gstk_tal_notify_ls_req_msg_v01), sim_internal_gstk_tal_notify_ls_req_msg_data_v01},
  {sizeof(sim_internal_gstk_tal_notify_ls_resp_msg_v01), sim_internal_gstk_tal_notify_ls_resp_msg_data_v01},
  {sizeof(sim_internal_gstk_tal_event_report_ind_msg_v01), sim_internal_gstk_tal_event_report_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_get_protocol_req_msg_v01), sim_internal_mmgsdi_get_protocol_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_get_protocol_resp_msg_v01), sim_internal_mmgsdi_get_protocol_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_v01), sim_internal_mmgsdi_session_register_for_refresh_ok_to_proceed_voting_ind_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_naa_refresh_req_msg_v01), sim_internal_mmgsdi_naa_refresh_req_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_naa_refresh_resp_msg_v01), sim_internal_mmgsdi_naa_refresh_resp_msg_data_v01},
  {sizeof(sim_internal_mmgsdi_naa_refresh_ind_msg_v01), sim_internal_mmgsdi_naa_refresh_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object sim_internal_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *sim_internal_qmi_idl_type_table_object_referenced_tables_v01[] =
{&sim_internal_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object sim_internal_qmi_idl_type_table_object_v01 = {
  sizeof(sim_internal_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(sim_internal_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  sim_internal_type_table_v01,
  sim_internal_message_table_v01,
  sim_internal_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry sim_internal_service_command_messages_v01[] = {
  {QMI_SIM_INTERNAL_GET_SUPPORTED_MSGS_REQ_V01, QMI_IDL_TYPE16(1, 0), 0},
  {QMI_SIM_INTERNAL_GET_SUPPORTED_FIELDS_REQ_V01, QMI_IDL_TYPE16(1, 2), 5},
  {QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_REQ_V01, QMI_IDL_TYPE16(0, 0), 23},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_OPEN_REQ_V01, QMI_IDL_TYPE16(0, 6), 293},
  {QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_REQ_V01, QMI_IDL_TYPE16(0, 12), 27},
  {QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_SYNC_REQ_V01, QMI_IDL_TYPE16(0, 15), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_FILE_ATTR_REQ_V01, QMI_IDL_TYPE16(0, 17), 46},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_TRANSPARENT_REQ_V01, QMI_IDL_TYPE16(0, 20), 50},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 23), 50},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_TRANSPARENT_REQ_V01, QMI_IDL_TYPE16(0, 26), 4050},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 29), 4051},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_CACHE_REQ_V01, QMI_IDL_TYPE16(0, 32), 23},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_REQ_V01, QMI_IDL_TYPE16(0, 34), 18},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_CACHE_REQ_V01, QMI_IDL_TYPE16(0, 36), 277},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_REQ_V01, QMI_IDL_TYPE16(0, 38), 29},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ_V01, QMI_IDL_TYPE16(0, 41), 27},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_TOGGLE_UIM_POWER_CONTROL_SYNC_REQ_V01, QMI_IDL_TYPE16(0, 52), 17},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_INFO_SYNC_REQ_V01, QMI_IDL_TYPE16(0, 54), 24},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_UICC_AUTHENTICATE_REQ_V01, QMI_IDL_TYPE16(0, 58), 284},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_USIM_AUTHENTICATE_REQ_V01, QMI_IDL_TYPE16(0, 61), 284},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_RUN_GSM_ALGO_REQ_V01, QMI_IDL_TYPE16(0, 64), 286},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_REQ_V01, QMI_IDL_TYPE16(0, 70), 385},
  {QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ_V01, QMI_IDL_TYPE16(0, 3), 27},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_CLOSE_REQ_V01, QMI_IDL_TYPE16(0, 9), 27},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_REQ_V01, QMI_IDL_TYPE16(0, 44), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_INCREASE_REQ_V01, QMI_IDL_TYPE16(0, 46), 302},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_ALL_PIN_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 49), 27},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_REQ_V01, QMI_IDL_TYPE16(0, 56), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_PERSO_OTA_DEPERSO_REQ_V01, QMI_IDL_TYPE16(0, 67), 284},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_REQ_V01, QMI_IDL_TYPE16(0, 73), 389},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_REQ_V01, QMI_IDL_TYPE16(0, 75), 28},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_REFRESH_REQ_V01, QMI_IDL_TYPE16(0, 78), 28},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REFRESH_COMPLETE_REQ_V01, QMI_IDL_TYPE16(0, 81), 28},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_DEREGISTER_FOR_REFRESH_REQ_V01, QMI_IDL_TYPE16(0, 84), 385},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_SEARCH_REQ_V01, QMI_IDL_TYPE16(0, 87), 309},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_SEEK_REQ_V01, QMI_IDL_TYPE16(0, 90), 303},
  {QMI_SIM_INTERNAL_GSTK_TAL_CLIENT_REGISTRATION_REQ_V01, QMI_IDL_TYPE16(0, 96), 23},
  {QMI_SIM_INTERNAL_GSTK_TAL_SEND_CMD_REQ_V01, QMI_IDL_TYPE16(0, 98), 4020},
  {QMI_SIM_INTERNAL_GSTK_TAL_NOTIFY_RAT_REQ_V01, QMI_IDL_TYPE16(0, 100), 13},
  {QMI_SIM_INTERNAL_GSTK_TAL_NOTIFY_LS_REQ_V01, QMI_IDL_TYPE16(0, 102), 13},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE_REQ_V01, QMI_IDL_TYPE16(0, 92), 3350},
  {QMI_SIM_INTERNAL_MMGSDI_GET_PROTOCOL_REQ_V01, QMI_IDL_TYPE16(0, 105), 20},
  {QMI_SIM_INTERNAL_MMGSDI_NAA_REFRESH_REQ_V01, QMI_IDL_TYPE16(0, 108), 353}
};

static const qmi_idl_service_message_table_entry sim_internal_service_response_messages_v01[] = {
  {QMI_SIM_INTERNAL_GET_SUPPORTED_MSGS_RESP_V01, QMI_IDL_TYPE16(1, 1), 8204},
  {QMI_SIM_INTERNAL_GET_SUPPORTED_FIELDS_RESP_V01, QMI_IDL_TYPE16(1, 3), 115},
  {QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_RESP_V01, QMI_IDL_TYPE16(0, 1), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_OPEN_RESP_V01, QMI_IDL_TYPE16(0, 7), 16},
  {QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_RESP_V01, QMI_IDL_TYPE16(0, 13), 16},
  {QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_SYNC_RESP_V01, QMI_IDL_TYPE16(0, 16), 1806},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_FILE_ATTR_RESP_V01, QMI_IDL_TYPE16(0, 18), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_TRANSPARENT_RESP_V01, QMI_IDL_TYPE16(0, 21), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 24), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_TRANSPARENT_RESP_V01, QMI_IDL_TYPE16(0, 27), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 30), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_CACHE_RESP_V01, QMI_IDL_TYPE16(0, 33), 4021},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_RESP_V01, QMI_IDL_TYPE16(0, 35), 21},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_CACHE_RESP_V01, QMI_IDL_TYPE16(0, 37), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_RESP_V01, QMI_IDL_TYPE16(0, 39), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_RESP_V01, QMI_IDL_TYPE16(0, 42), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_TOGGLE_UIM_POWER_CONTROL_SYNC_RESP_V01, QMI_IDL_TYPE16(0, 53), 20},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_INFO_SYNC_RESP_V01, QMI_IDL_TYPE16(0, 55), 90},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_UICC_AUTHENTICATE_RESP_V01, QMI_IDL_TYPE16(0, 59), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_USIM_AUTHENTICATE_RESP_V01, QMI_IDL_TYPE16(0, 62), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_RUN_GSM_ALGO_RESP_V01, QMI_IDL_TYPE16(0, 65), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_RESP_V01, QMI_IDL_TYPE16(0, 71), 16},
  {QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_DEREG_RESP_V01, QMI_IDL_TYPE16(0, 4), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_CLOSE_RESP_V01, QMI_IDL_TYPE16(0, 10), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_RESP_V01, QMI_IDL_TYPE16(0, 45), 23},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_INCREASE_RESP_V01, QMI_IDL_TYPE16(0, 47), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_ALL_PIN_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 50), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_RESP_V01, QMI_IDL_TYPE16(0, 57), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_PERSO_OTA_DEPERSO_RESP_V01, QMI_IDL_TYPE16(0, 68), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_RESP_V01, QMI_IDL_TYPE16(0, 74), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_RESP_V01, QMI_IDL_TYPE16(0, 76), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_REFRESH_RESP_V01, QMI_IDL_TYPE16(0, 79), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REFRESH_COMPLETE_RESP_V01, QMI_IDL_TYPE16(0, 82), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_DEREGISTER_FOR_REFRESH_RESP_V01, QMI_IDL_TYPE16(0, 85), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_SEARCH_RESP_V01, QMI_IDL_TYPE16(0, 88), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_SEEK_RESP_V01, QMI_IDL_TYPE16(0, 91), 16},
  {QMI_SIM_INTERNAL_GSTK_TAL_CLIENT_REGISTRATION_RESP_V01, QMI_IDL_TYPE16(0, 97), 16},
  {QMI_SIM_INTERNAL_GSTK_TAL_SEND_CMD_RESP_V01, QMI_IDL_TYPE16(0, 99), 16},
  {QMI_SIM_INTERNAL_GSTK_TAL_NOTIFY_RAT_RESP_V01, QMI_IDL_TYPE16(0, 101), 16},
  {QMI_SIM_INTERNAL_GSTK_TAL_NOTIFY_LS_RESP_V01, QMI_IDL_TYPE16(0, 103), 16},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE_RESP_V01, QMI_IDL_TYPE16(0, 93), 16},
  {QMI_SIM_INTERNAL_MMGSDI_GET_PROTOCOL_RESP_V01, QMI_IDL_TYPE16(0, 106), 20},
  {QMI_SIM_INTERNAL_MMGSDI_NAA_REFRESH_RESP_V01, QMI_IDL_TYPE16(0, 109), 16}
};

static const qmi_idl_service_message_table_entry sim_internal_service_indication_messages_v01[] = {
  {QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_REG_IND_V01, QMI_IDL_TYPE16(0, 2), 45},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_OPEN_IND_V01, QMI_IDL_TYPE16(0, 8), 125},
  {QMI_SIM_INTERNAL_MMGSDI_GET_ALL_AVAILABLE_APPS_IND_V01, QMI_IDL_TYPE16(0, 14), 1835},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_FILE_ATTR_IND_V01, QMI_IDL_TYPE16(0, 19), 563},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_TRANSPARENT_IND_V01, QMI_IDL_TYPE16(0, 22), 4072},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_READ_RECORD_IND_V01, QMI_IDL_TYPE16(0, 25), 4072},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_TRANSPARENT_IND_V01, QMI_IDL_TYPE16(0, 28), 72},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_WRITE_RECORD_IND_V01, QMI_IDL_TYPE16(0, 31), 72},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_IND_V01, QMI_IDL_TYPE16(0, 40), 49},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_APP_CAPABILITIES_IND_V01, QMI_IDL_TYPE16(0, 43), 52},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_UICC_AUTHENTICATE_IND_V01, QMI_IDL_TYPE16(0, 60), 306},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_USIM_AUTHENTICATE_IND_V01, QMI_IDL_TYPE16(0, 63), 306},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_RUN_GSM_ALGO_IND_V01, QMI_IDL_TYPE16(0, 66), 60},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_IND_V01, QMI_IDL_TYPE16(0, 72), 49},
  {QMI_SIM_INTERNAL_MMGSDI_EVENT_REPORT_IND_V01, QMI_IDL_TYPE16(0, 95), 3606},
  {QMI_SIM_INTERNAL_MMGSDI_CLIENT_ID_AND_EVT_DEREG_IND_V01, QMI_IDL_TYPE16(0, 5), 45},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_CLOSE_IND_V01, QMI_IDL_TYPE16(0, 11), 123},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_INCREASE_IND_V01, QMI_IDL_TYPE16(0, 48), 323},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_GET_ALL_PIN_STATUS_IND_V01, QMI_IDL_TYPE16(0, 51), 77},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_PERSO_OTA_DEPERSO_IND_V01, QMI_IDL_TYPE16(0, 69), 304},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_REFRESH_OK_TO_PROCEED_VOTING_IND_V01, QMI_IDL_TYPE16(0, 107), 49},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_IND_V01, QMI_IDL_TYPE16(0, 77), 49},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_OK_TO_REFRESH_IND_V01, QMI_IDL_TYPE16(0, 80), 49},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REFRESH_COMPLETE_IND_V01, QMI_IDL_TYPE16(0, 83), 49},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_DEREGISTER_FOR_REFRESH_IND_V01, QMI_IDL_TYPE16(0, 86), 45},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_SEARCH_IND_V01, QMI_IDL_TYPE16(0, 89), 323},
  {QMI_SIM_INTERNAL_GSTK_TAL_EVENT_REPORT_IND_V01, QMI_IDL_TYPE16(0, 104), 8038},
  {QMI_SIM_INTERNAL_MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE_IND_V01, QMI_IDL_TYPE16(0, 94), 45},
  {QMI_SIM_INTERNAL_MMGSDI_NAA_REFRESH_IND_V01, QMI_IDL_TYPE16(0, 110), 49}
};

/*Service Object*/
struct qmi_idl_service_object sim_internal_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x23,
  8204,
  { sizeof(sim_internal_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(sim_internal_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(sim_internal_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { sim_internal_service_command_messages_v01, sim_internal_service_response_messages_v01, sim_internal_service_indication_messages_v01},
  &sim_internal_qmi_idl_type_table_object_v01,
  0x08,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type sim_internal_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SIM_INTERNAL_V01_IDL_MAJOR_VERS != idl_maj_version || SIM_INTERNAL_V01_IDL_MINOR_VERS != idl_min_version 
       || SIM_INTERNAL_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&sim_internal_qmi_idl_service_object_v01;
}

