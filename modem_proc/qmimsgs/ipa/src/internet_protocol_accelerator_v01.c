/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        I N T E R N E T _ P R O T O C O L _ A C C E L E R A T O R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the ipa service Data structures.

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/ipa/src/internet_protocol_accelerator_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2 
   It was generated on: Thu Jan  8 2015 (Spin 0)
   From IDL File: internet_protocol_accelerator_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "internet_protocol_accelerator_v01.h"
#include "common_v01.h"
#include "data_common_v01.h"


/*Type Definitions*/
static const uint8_t ipa_hdr_tbl_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_hdr_tbl_info_type_v01, modem_offset_start),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_hdr_tbl_info_type_v01, modem_offset_end),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_hdr_proc_ctx_tbl_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_hdr_proc_ctx_tbl_info_type_v01, modem_offset_start),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_hdr_proc_ctx_tbl_info_type_v01, modem_offset_end),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_zip_tbl_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_zip_tbl_info_type_v01, modem_offset_start),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_zip_tbl_info_type_v01, modem_offset_end),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_route_tbl_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_route_tbl_info_type_v01, route_tbl_start_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_route_tbl_info_type_v01, num_indices),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_modem_mem_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_modem_mem_info_type_v01, block_start_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_modem_mem_info_type_v01, size),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_range_eq_16_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_range_eq_16_type_v01, offset),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_range_eq_16_type_v01, range_low),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_range_eq_16_type_v01, range_high),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_mask_eq_32_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_32_type_v01, offset),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_32_type_v01, mask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_32_type_v01, value),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_eq_16_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_eq_16_type_v01, offset),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_eq_16_type_v01, value),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_eq_32_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_eq_32_type_v01, offset),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_eq_32_type_v01, value),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_ipfltr_mask_eq_128_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_128_type_v01, offset),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_128_type_v01, mask),
  16,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_ipfltr_mask_eq_128_type_v01, value),
  16,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_rule_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, rule_eq_bitmap),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, tos_eq_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, tos_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, protocol_eq_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, protocol_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, num_ihl_offset_range_16),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_range_16),
  QMI_IPA_IPFLTR_NUM_IHL_RANGE_16_EQNS_V01,
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, num_offset_meq_32),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, offset_meq_32),
  QMI_IPA_IPFLTR_NUM_MEQ_32_EQNS_V01,
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, tc_eq_present),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, tc_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, flow_eq_present),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, flow_eq),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_eq_16_present),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_eq_16),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_eq_32_present),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_eq_32),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, num_ihl_offset_meq_32),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ihl_offset_meq_32),
  QMI_IPA_IPFLTR_NUM_IHL_MEQ_32_EQNS_V01,
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, num_offset_meq_128),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, offset_meq_128),
  QMI_IPA_IPFLTR_NUM_MEQ_128_EQNS_V01,
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, metadata_meq32_present),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, metadata_meq32),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_type_v01, ipv4_frag_eq_present),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_spec_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, filter_spec_identifier),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, ip_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, filter_rule),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, filter_action),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, is_routing_table_index_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_spec_type_v01, route_table_index),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_type_v01, is_mux_id_valid),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_filter_spec_type_v01, mux_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_rule_identifier_to_handle_map_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_identifier_to_handle_map_v01, filter_spec_identifier),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_rule_identifier_to_handle_map_v01, filter_handle),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ipa_filter_handle_to_index_map_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_handle_to_index_map_v01, filter_handle),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_filter_handle_to_index_map_v01, filter_index),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t ipa_indication_reg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, master_driver_init_complete) - QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, master_driver_init_complete_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_indication_reg_req_msg_v01, master_driver_init_complete)
};

static const uint8_t ipa_indication_reg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_indication_reg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_init_modem_driver_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, platform_type) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, platform_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, platform_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_tbl_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_tbl_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_route_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_route_tbl_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_route_tbl_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_route_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_route_tbl_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_route_tbl_info),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_filter_tbl_start_addr) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_filter_tbl_start_addr_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v4_filter_tbl_start_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_filter_tbl_start_addr) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_filter_tbl_start_addr_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, v6_filter_tbl_start_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, modem_mem_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, modem_mem_info_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, modem_mem_info),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, ctrl_comm_dest_end_pt) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, ctrl_comm_dest_end_pt_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, ctrl_comm_dest_end_pt),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, is_ssr_bootup) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, is_ssr_bootup_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, is_ssr_bootup),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_proc_ctx_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_proc_ctx_tbl_info_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, hdr_proc_ctx_tbl_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, zip_tbl_info) - QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, zip_tbl_info_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_req_msg_v01, zip_tbl_info),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t ipa_init_modem_driver_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, ctrl_comm_dest_end_pt) - QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, ctrl_comm_dest_end_pt_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, ctrl_comm_dest_end_pt),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, default_end_pt) - QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, default_end_pt_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_init_modem_driver_resp_msg_v01, default_end_pt)
};

static const uint8_t ipa_master_driver_init_complt_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_master_driver_init_complt_ind_msg_v01, master_driver_init_status),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_install_fltr_rule_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_req_msg_v01, filter_spec_list_len),
  QMI_IDL_TYPE88(0, 11),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, source_pipe_index) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, source_pipe_index_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, source_pipe_index),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, num_ipv4_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, num_ipv4_filters_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, num_ipv4_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, num_ipv6_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, num_ipv6_filters_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, num_ipv6_filters),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list) - QMI_IDL_OFFSET16RELATIVE(ipa_install_fltr_rule_req_msg_v01, xlat_filter_indices_list_len)
};

static const uint8_t ipa_install_fltr_rule_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list) - QMI_IDL_OFFSET8(ipa_install_fltr_rule_resp_msg_v01, filter_handle_list_len),
  QMI_IDL_TYPE88(0, 12)
};

static const uint8_t ipa_fltr_installed_notif_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, source_pipe_index),

  0x02,
   QMI_IDL_2_BYTE_ENUM,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, install_status),

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, filter_index_list),
  QMI_IPA_MAX_FILTERS_V01,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, filter_index_list) - QMI_IDL_OFFSET8(ipa_fltr_installed_notif_req_msg_v01, filter_index_list_len),
  QMI_IDL_TYPE88(0, 13),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, embedded_pipe_index) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, embedded_pipe_index_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, embedded_pipe_index),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, retain_header) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, retain_header_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, retain_header),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, embedded_call_mux_id) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, embedded_call_mux_id_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, embedded_call_mux_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, num_ipv4_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, num_ipv4_filters_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, num_ipv4_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, num_ipv6_filters) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, num_ipv6_filters_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, num_ipv6_filters),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, start_ipv4_filter_idx) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, start_ipv4_filter_idx_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, start_ipv4_filter_idx),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, start_ipv6_filter_idx) - QMI_IDL_OFFSET16RELATIVE(ipa_fltr_installed_notif_req_msg_v01, start_ipv6_filter_idx_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(ipa_fltr_installed_notif_req_msg_v01, start_ipv6_filter_idx)
};

static const uint8_t ipa_fltr_installed_notif_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_fltr_installed_notif_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_enable_force_clear_datapath_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, source_pipe_bitmask),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, request_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, throttle_source) - QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, throttle_source_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_req_msg_v01, throttle_source)
};

static const uint8_t ipa_enable_force_clear_datapath_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_enable_force_clear_datapath_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_disable_force_clear_datapath_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_disable_force_clear_datapath_req_msg_v01, request_id)
};

static const uint8_t ipa_disable_force_clear_datapath_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_disable_force_clear_datapath_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_type) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_deaggr_supported) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_deaggr_supported_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_deaggr_supported),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, max_aggr_frame_size) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, max_aggr_frame_size_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, max_aggr_frame_size),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ipa_ingress_pipe_mode) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ipa_ingress_pipe_mode_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ipa_ingress_pipe_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_speed_info) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_speed_info_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, peripheral_speed_info),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_time_limit) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_time_limit_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_time_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_pkt_limit) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_pkt_limit_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_pkt_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_byte_limit) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_byte_limit_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_accumulation_byte_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_accumulation_time_limit) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_accumulation_time_limit_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_accumulation_time_limit),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_control_flags) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_control_flags_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, hw_control_flags),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_msi_event_threshold) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_msi_event_threshold_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, ul_msi_event_threshold),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_msi_event_threshold) - QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_msi_event_threshold_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_config_req_msg_v01, dl_msi_event_threshold)
};

static const uint8_t ipa_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_disable_link_low_pwr_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, request_id) - QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, request_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, request_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, link_ep_id) - QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, link_ep_id_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_req_msg_v01, link_ep_id),
  QMI_IDL_TYPE88(2, 0)
};

static const uint8_t ipa_disable_link_low_pwr_state_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_disable_link_low_pwr_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t ipa_enable_link_low_pwr_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ipa_enable_link_low_pwr_state_req_msg_v01, request_id) - QMI_IDL_OFFSET8(ipa_enable_link_low_pwr_state_req_msg_v01, request_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ipa_enable_link_low_pwr_state_req_msg_v01, request_id)
};

static const uint8_t ipa_enable_link_low_pwr_state_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ipa_enable_link_low_pwr_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/* Type Table */
static const qmi_idl_type_table_entry  ipa_type_table_v01[] = {
  {sizeof(ipa_hdr_tbl_info_type_v01), ipa_hdr_tbl_info_type_data_v01},
  {sizeof(ipa_hdr_proc_ctx_tbl_info_type_v01), ipa_hdr_proc_ctx_tbl_info_type_data_v01},
  {sizeof(ipa_zip_tbl_info_type_v01), ipa_zip_tbl_info_type_data_v01},
  {sizeof(ipa_route_tbl_info_type_v01), ipa_route_tbl_info_type_data_v01},
  {sizeof(ipa_modem_mem_info_type_v01), ipa_modem_mem_info_type_data_v01},
  {sizeof(ipa_ipfltr_range_eq_16_type_v01), ipa_ipfltr_range_eq_16_type_data_v01},
  {sizeof(ipa_ipfltr_mask_eq_32_type_v01), ipa_ipfltr_mask_eq_32_type_data_v01},
  {sizeof(ipa_ipfltr_eq_16_type_v01), ipa_ipfltr_eq_16_type_data_v01},
  {sizeof(ipa_ipfltr_eq_32_type_v01), ipa_ipfltr_eq_32_type_data_v01},
  {sizeof(ipa_ipfltr_mask_eq_128_type_v01), ipa_ipfltr_mask_eq_128_type_data_v01},
  {sizeof(ipa_filter_rule_type_v01), ipa_filter_rule_type_data_v01},
  {sizeof(ipa_filter_spec_type_v01), ipa_filter_spec_type_data_v01},
  {sizeof(ipa_filter_rule_identifier_to_handle_map_v01), ipa_filter_rule_identifier_to_handle_map_data_v01},
  {sizeof(ipa_filter_handle_to_index_map_v01), ipa_filter_handle_to_index_map_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry ipa_message_table_v01[] = {
  {sizeof(ipa_indication_reg_req_msg_v01), ipa_indication_reg_req_msg_data_v01},
  {sizeof(ipa_indication_reg_resp_msg_v01), ipa_indication_reg_resp_msg_data_v01},
  {sizeof(ipa_init_modem_driver_req_msg_v01), ipa_init_modem_driver_req_msg_data_v01},
  {sizeof(ipa_init_modem_driver_resp_msg_v01), ipa_init_modem_driver_resp_msg_data_v01},
  {sizeof(ipa_master_driver_init_complt_ind_msg_v01), ipa_master_driver_init_complt_ind_msg_data_v01},
  {sizeof(ipa_install_fltr_rule_req_msg_v01), ipa_install_fltr_rule_req_msg_data_v01},
  {sizeof(ipa_install_fltr_rule_resp_msg_v01), ipa_install_fltr_rule_resp_msg_data_v01},
  {sizeof(ipa_fltr_installed_notif_req_msg_v01), ipa_fltr_installed_notif_req_msg_data_v01},
  {sizeof(ipa_fltr_installed_notif_resp_msg_v01), ipa_fltr_installed_notif_resp_msg_data_v01},
  {sizeof(ipa_enable_force_clear_datapath_req_msg_v01), ipa_enable_force_clear_datapath_req_msg_data_v01},
  {sizeof(ipa_enable_force_clear_datapath_resp_msg_v01), ipa_enable_force_clear_datapath_resp_msg_data_v01},
  {sizeof(ipa_disable_force_clear_datapath_req_msg_v01), ipa_disable_force_clear_datapath_req_msg_data_v01},
  {sizeof(ipa_disable_force_clear_datapath_resp_msg_v01), ipa_disable_force_clear_datapath_resp_msg_data_v01},
  {sizeof(ipa_config_req_msg_v01), ipa_config_req_msg_data_v01},
  {sizeof(ipa_config_resp_msg_v01), ipa_config_resp_msg_data_v01},
  {sizeof(ipa_disable_link_low_pwr_state_req_msg_v01), ipa_disable_link_low_pwr_state_req_msg_data_v01},
  {sizeof(ipa_disable_link_low_pwr_state_resp_msg_v01), ipa_disable_link_low_pwr_state_resp_msg_data_v01},
  {sizeof(ipa_enable_link_low_pwr_state_req_msg_v01), ipa_enable_link_low_pwr_state_req_msg_data_v01},
  {sizeof(ipa_enable_link_low_pwr_state_resp_msg_v01), ipa_enable_link_low_pwr_state_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object ipa_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *ipa_qmi_idl_type_table_object_referenced_tables_v01[] =
{&ipa_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01, &data_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object ipa_qmi_idl_type_table_object_v01 = {
  sizeof(ipa_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(ipa_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  ipa_type_table_v01,
  ipa_message_table_v01,
  ipa_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry ipa_service_command_messages_v01[] = {
  {QMI_IPA_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 4},
  {QMI_IPA_INIT_MODEM_DRIVER_REQ_V01, QMI_IDL_TYPE16(0, 2), 98},
  {QMI_IPA_INSTALL_FILTER_RULE_REQ_V01, QMI_IDL_TYPE16(0, 5), 11293},
  {QMI_IPA_FILTER_INSTALLED_NOTIF_REQ_V01, QMI_IDL_TYPE16(0, 7), 574},
  {QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_REQ_V01, QMI_IDL_TYPE16(0, 9), 18},
  {QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_REQ_V01, QMI_IDL_TYPE16(0, 11), 7},
  {QMI_IPA_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 13), 81},
  {QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_REQ_V01, QMI_IDL_TYPE16(0, 15), 18},
  {QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_REQ_V01, QMI_IDL_TYPE16(0, 17), 7}
};

static const qmi_idl_service_message_table_entry ipa_service_response_messages_v01[] = {
  {QMI_IPA_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_IPA_INIT_MODEM_DRIVER_RESP_V01, QMI_IDL_TYPE16(0, 3), 21},
  {QMI_IPA_INSTALL_FILTER_RULE_RESP_V01, QMI_IDL_TYPE16(0, 6), 523},
  {QMI_IPA_FILTER_INSTALLED_NOTIF_RESP_V01, QMI_IDL_TYPE16(0, 8), 7},
  {QMI_IPA_ENABLE_FORCE_CLEAR_DATAPATH_RESP_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_IPA_DISABLE_FORCE_CLEAR_DATAPATH_RESP_V01, QMI_IDL_TYPE16(0, 12), 7},
  {QMI_IPA_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_IPA_DISABLE_LINK_LOW_PWR_STATE_RESP_V01, QMI_IDL_TYPE16(0, 16), 7},
  {QMI_IPA_ENABLE_LINK_LOW_PWR_STATE_RESP_V01, QMI_IDL_TYPE16(0, 18), 7}
};

static const qmi_idl_service_message_table_entry ipa_service_indication_messages_v01[] = {
  {QMI_IPA_MASTER_DRIVER_INIT_COMPLETE_IND_V01, QMI_IDL_TYPE16(0, 4), 7}
};

/*Service Object*/
struct qmi_idl_service_object ipa_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x31,
  11293,
  { sizeof(ipa_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ipa_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ipa_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { ipa_service_command_messages_v01, ipa_service_response_messages_v01, ipa_service_indication_messages_v01},
  &ipa_qmi_idl_type_table_object_v01,
  0x07,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type ipa_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( IPA_V01_IDL_MAJOR_VERS != idl_maj_version || IPA_V01_IDL_MINOR_VERS != idl_min_version 
       || IPA_V01_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&ipa_qmi_idl_service_object_v01;
}

