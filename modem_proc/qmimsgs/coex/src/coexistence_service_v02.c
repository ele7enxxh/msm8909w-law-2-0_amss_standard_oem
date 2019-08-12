/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C O E X I S T E N C E _ S E R V I C E _ V 0 2  . C

GENERAL DESCRIPTION
  This is the file which defines the coex service Data structures.

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qmimsgs/coex/src/coexistence_service_v02.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.10 
   It was generated on: Tue Aug  5 2014 (Spin 1)
   From IDL File: coexistence_service_v02.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "coexistence_service_v02.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t coex_tech_band_type_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_tech_band_type_v02, frequency),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_tech_band_type_v02, bandwidth_and_direction),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_antenna_state_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_antenna_state_v02, adv_notice),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_antenna_state_v02, duration),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_tech_sync_state_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_tech_sync_state_v02, id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_tech_sync_state_v02, sync_state_token),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_tech_state_update_req_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_tech_state_update_req_v02, id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_lte_state_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_lte_state_v02, band_info),
  COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(coex_wwan_lte_state_v02, band_info) - QMI_IDL_OFFSET8(coex_wwan_lte_state_v02, band_info_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_lte_state_v02, sync_state_token),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_lte_state_v02, data),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_lte_state_v02, tdd_info),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_tdscdma_state_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_tdscdma_state_v02, band_info),
  COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(coex_wwan_tdscdma_state_v02, band_info) - QMI_IDL_OFFSET8(coex_wwan_tdscdma_state_v02, band_info_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_tdscdma_state_v02, sync_state_token),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_gsm_state_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_gsm_state_v02, band_info),
  COEX_WWAN_GSM_MAX_BANDS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(coex_wwan_gsm_state_v02, band_info) - QMI_IDL_OFFSET8(coex_wwan_gsm_state_v02, band_info_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_gsm_state_v02, sync_state_token),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_onex_state_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_onex_state_v02, band_info),
  COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(coex_wwan_onex_state_v02, band_info) - QMI_IDL_OFFSET8(coex_wwan_onex_state_v02, band_info_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_onex_state_v02, sync_state_token),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_hdr_state_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_hdr_state_v02, band_info),
  COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(coex_wwan_hdr_state_v02, band_info) - QMI_IDL_OFFSET8(coex_wwan_hdr_state_v02, band_info_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_hdr_state_v02, sync_state_token),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_wcdma_state_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wwan_wcdma_state_v02, band_info),
  COEX_WWAN_TECH_MAX_BANDS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(coex_wwan_wcdma_state_v02, band_info) - QMI_IDL_OFFSET8(coex_wwan_wcdma_state_v02, band_info_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_wcdma_state_v02, sync_state_token),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wlan_wifi_connection_info_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wlan_wifi_connection_info_v02, band_info),
  COEX_WLAN_WIFI_MAX_BANDS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(coex_wlan_wifi_connection_info_v02, band_info) - QMI_IDL_OFFSET8(coex_wlan_wifi_connection_info_v02, band_info_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wlan_wifi_state_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wlan_wifi_state_v02, high_priority_events_list),
  COEX_WLAN_WIFI_MAX_EVENTS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(coex_wlan_wifi_state_v02, high_priority_events_list) - QMI_IDL_OFFSET8(coex_wlan_wifi_state_v02, high_priority_events_list_len),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_wlan_wifi_state_v02, connections_list),
  COEX_WLAN_WIFI_MAX_CONNS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(coex_wlan_wifi_state_v02, connections_list) - QMI_IDL_OFFSET8(coex_wlan_wifi_state_v02, connections_list_len),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_wifi_state_v02, sync_state_token),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_diversity_antenna_state_data_v02[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_diversity_antenna_state_v02, data),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_diversity_antenna_state_v02, sync_state_token),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_metrics_lte_sinr_params_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_params_v02, filter_alpha),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_metrics_lte_sinr_stats_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stats_v02, sinr_system),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stats_v02, sinr_bt_only),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stats_v02, sinr_wifi_only),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stats_v02, sinr_bt_and_wifi),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_stats_v02, sinr_lte_only),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_metrics_lte_sinr_report_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_report_v02, estatus),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_report_v02, collection_duration),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_metrics_lte_sinr_report_v02, stats),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wwan_conflict_band_range_type_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_conflict_band_range_type_v02, tech_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_conflict_band_range_type_v02, frequency_range_start),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wwan_conflict_band_range_type_v02, frequency_range_stop),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_wlan_conflict_band_range_type_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_conflict_band_range_type_v02, tech_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_conflict_band_range_type_v02, frequency_range_start),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_wlan_conflict_band_range_type_v02, frequency_range_stop),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_conflict_definition_type_data_v02[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v02, wwan),
  QMI_IDL_TYPE88(0, 16),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v02, wlan),
  QMI_IDL_TYPE88(0, 17),
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v02, wlan_coex_policy),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_conflict_definition_type_v02, wlan_policy_coex_wwan_rx_imd_protection_params),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t coex_conflict_params_report_type_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_conflict_params_report_type_v02, request_estatus),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_conflict_params_report_type_v02, victim_tbl_offset),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(coex_conflict_params_report_type_v02, victim_tbl_complete_size),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(coex_conflict_params_report_type_v02, partial_victim_table),
  COEX_NV_MSG_VICTIM_TABLE_MAX_ENTRIES_V02,
  QMI_IDL_OFFSET8(coex_conflict_params_report_type_v02, partial_victim_table) - QMI_IDL_OFFSET8(coex_conflict_params_report_type_v02, partial_victim_table_len),
  QMI_IDL_TYPE88(0, 18),
  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qmi_coex_tech_sync_req_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_tech_sync_req_msg_v02, tech) - QMI_IDL_OFFSET8(qmi_coex_tech_sync_req_msg_v02, tech_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_tech_sync_req_msg_v02, tech),
  COEX_MAX_TECHS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(qmi_coex_tech_sync_req_msg_v02, tech) - QMI_IDL_OFFSET8(qmi_coex_tech_sync_req_msg_v02, tech_len),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t qmi_coex_tech_sync_resp_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_tech_sync_resp_msg_v02, tech) - QMI_IDL_OFFSET8(qmi_coex_tech_sync_resp_msg_v02, tech_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_tech_sync_resp_msg_v02, tech),
  COEX_MAX_TECHS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(qmi_coex_tech_sync_resp_msg_v02, tech) - QMI_IDL_OFFSET8(qmi_coex_tech_sync_resp_msg_v02, tech_len),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t qmi_coex_tech_sync_update_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_tech_sync_update_ind_msg_v02, tech) - QMI_IDL_OFFSET8(qmi_coex_tech_sync_update_ind_msg_v02, tech_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_tech_sync_update_ind_msg_v02, tech),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t qmi_coex_tech_state_update_req_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_tech_state_update_req_msg_v02, tech_ids) - QMI_IDL_OFFSET8(qmi_coex_tech_state_update_req_msg_v02, tech_ids_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_tech_state_update_req_msg_v02, tech_ids),
  COEX_MAX_TECHS_SUPPORTED_V02,
  QMI_IDL_OFFSET8(qmi_coex_tech_state_update_req_msg_v02, tech_ids) - QMI_IDL_OFFSET8(qmi_coex_tech_state_update_req_msg_v02, tech_ids_len),
  QMI_IDL_TYPE88(0, 3)
};

static const uint8_t qmi_coex_wwan_lte_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_wwan_lte_state_ind_msg_v02, lte_band_info) - QMI_IDL_OFFSET8(qmi_coex_wwan_lte_state_ind_msg_v02, lte_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_wwan_lte_state_ind_msg_v02, lte_band_info),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t qmi_coex_wwan_tdscdma_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_wwan_tdscdma_state_ind_msg_v02, tdscdma_band_info) - QMI_IDL_OFFSET8(qmi_coex_wwan_tdscdma_state_ind_msg_v02, tdscdma_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_wwan_tdscdma_state_ind_msg_v02, tdscdma_band_info),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t qmi_coex_wwan_gsm_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_wwan_gsm_state_ind_msg_v02, gsm_band_info) - QMI_IDL_OFFSET8(qmi_coex_wwan_gsm_state_ind_msg_v02, gsm_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_wwan_gsm_state_ind_msg_v02, gsm_band_info),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t qmi_coex_wwan_onex_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_wwan_onex_state_ind_msg_v02, onex_band_info) - QMI_IDL_OFFSET8(qmi_coex_wwan_onex_state_ind_msg_v02, onex_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_wwan_onex_state_ind_msg_v02, onex_band_info),
  QMI_IDL_TYPE88(0, 7)
};

static const uint8_t qmi_coex_wwan_hdr_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_wwan_hdr_state_ind_msg_v02, hdr_band_info) - QMI_IDL_OFFSET8(qmi_coex_wwan_hdr_state_ind_msg_v02, hdr_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_wwan_hdr_state_ind_msg_v02, hdr_band_info),
  QMI_IDL_TYPE88(0, 8)
};

static const uint8_t qmi_coex_wwan_wcdma_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_wwan_wcdma_state_ind_msg_v02, wcdma_band_info) - QMI_IDL_OFFSET8(qmi_coex_wwan_wcdma_state_ind_msg_v02, wcdma_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_wwan_wcdma_state_ind_msg_v02, wcdma_band_info),
  QMI_IDL_TYPE88(0, 9)
};

static const uint8_t qmi_coex_wlan_wifi_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_wlan_wifi_state_ind_msg_v02, wifi_state_info) - QMI_IDL_OFFSET8(qmi_coex_wlan_wifi_state_ind_msg_v02, wifi_state_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_wlan_wifi_state_ind_msg_v02, wifi_state_info),
  QMI_IDL_TYPE88(0, 11)
};

/* 
 * qmi_coex_wlan_bt_state_ind_msg is empty
 * static const uint8_t qmi_coex_wlan_bt_state_ind_msg_data_v02[] = {
 * };
 */
  
static const uint8_t qmi_coex_diversity_antenna_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_diversity_antenna_state_ind_msg_v02, state) - QMI_IDL_OFFSET8(qmi_coex_diversity_antenna_state_ind_msg_v02, state_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_diversity_antenna_state_ind_msg_v02, state),
  QMI_IDL_TYPE88(0, 12)
};

static const uint8_t qmi_coex_metrics_lte_sinr_start_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_metrics_lte_sinr_start_ind_msg_v02, data) - QMI_IDL_OFFSET8(qmi_coex_metrics_lte_sinr_start_ind_msg_v02, data_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_metrics_lte_sinr_start_ind_msg_v02, data),
  QMI_IDL_TYPE88(0, 13)
};

/* 
 * qmi_coex_metrics_lte_sinr_read_req_msg is empty
 * static const uint8_t qmi_coex_metrics_lte_sinr_read_req_msg_data_v02[] = {
 * };
 */
  
static const uint8_t qmi_coex_metrics_lte_sinr_read_resp_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_metrics_lte_sinr_read_resp_msg_v02, report) - QMI_IDL_OFFSET8(qmi_coex_metrics_lte_sinr_read_resp_msg_v02, report_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_metrics_lte_sinr_read_resp_msg_v02, report),
  QMI_IDL_TYPE88(0, 15)
};

/* 
 * qmi_coex_metrics_lte_sinr_stop_ind_msg is empty
 * static const uint8_t qmi_coex_metrics_lte_sinr_stop_ind_msg_data_v02[] = {
 * };
 */
  
static const uint8_t qmi_coex_wwan_gsm2_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_wwan_gsm2_state_ind_msg_v02, gsm2_band_info) - QMI_IDL_OFFSET8(qmi_coex_wwan_gsm2_state_ind_msg_v02, gsm2_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_wwan_gsm2_state_ind_msg_v02, gsm2_band_info),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t qmi_coex_wwan_gsm3_state_ind_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_wwan_gsm3_state_ind_msg_v02, gsm3_band_info) - QMI_IDL_OFFSET8(qmi_coex_wwan_gsm3_state_ind_msg_v02, gsm3_band_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_wwan_gsm3_state_ind_msg_v02, gsm3_band_info),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t qmi_coex_conflict_params_req_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_conflict_params_req_msg_v02, victim_tbl_offset) - QMI_IDL_OFFSET8(qmi_coex_conflict_params_req_msg_v02, victim_tbl_offset_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmi_coex_conflict_params_req_msg_v02, victim_tbl_offset)
};

static const uint8_t qmi_coex_conflict_params_resp_msg_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmi_coex_conflict_params_resp_msg_v02, report) - QMI_IDL_OFFSET8(qmi_coex_conflict_params_resp_msg_v02, report_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmi_coex_conflict_params_resp_msg_v02, report),
  QMI_IDL_TYPE88(0, 19)
};

/* Type Table */
static const qmi_idl_type_table_entry  coex_type_table_v02[] = {
  {sizeof(coex_tech_band_type_v02), coex_tech_band_type_data_v02},
  {sizeof(coex_antenna_state_v02), coex_antenna_state_data_v02},
  {sizeof(coex_tech_sync_state_v02), coex_tech_sync_state_data_v02},
  {sizeof(coex_tech_state_update_req_v02), coex_tech_state_update_req_data_v02},
  {sizeof(coex_wwan_lte_state_v02), coex_wwan_lte_state_data_v02},
  {sizeof(coex_wwan_tdscdma_state_v02), coex_wwan_tdscdma_state_data_v02},
  {sizeof(coex_wwan_gsm_state_v02), coex_wwan_gsm_state_data_v02},
  {sizeof(coex_wwan_onex_state_v02), coex_wwan_onex_state_data_v02},
  {sizeof(coex_wwan_hdr_state_v02), coex_wwan_hdr_state_data_v02},
  {sizeof(coex_wwan_wcdma_state_v02), coex_wwan_wcdma_state_data_v02},
  {sizeof(coex_wlan_wifi_connection_info_v02), coex_wlan_wifi_connection_info_data_v02},
  {sizeof(coex_wlan_wifi_state_v02), coex_wlan_wifi_state_data_v02},
  {sizeof(coex_diversity_antenna_state_v02), coex_diversity_antenna_state_data_v02},
  {sizeof(coex_metrics_lte_sinr_params_v02), coex_metrics_lte_sinr_params_data_v02},
  {sizeof(coex_metrics_lte_sinr_stats_v02), coex_metrics_lte_sinr_stats_data_v02},
  {sizeof(coex_metrics_lte_sinr_report_v02), coex_metrics_lte_sinr_report_data_v02},
  {sizeof(coex_wwan_conflict_band_range_type_v02), coex_wwan_conflict_band_range_type_data_v02},
  {sizeof(coex_wlan_conflict_band_range_type_v02), coex_wlan_conflict_band_range_type_data_v02},
  {sizeof(coex_conflict_definition_type_v02), coex_conflict_definition_type_data_v02},
  {sizeof(coex_conflict_params_report_type_v02), coex_conflict_params_report_type_data_v02}
};

/* Message Table */
static const qmi_idl_message_table_entry coex_message_table_v02[] = {
  {sizeof(qmi_coex_tech_sync_req_msg_v02), qmi_coex_tech_sync_req_msg_data_v02},
  {sizeof(qmi_coex_tech_sync_resp_msg_v02), qmi_coex_tech_sync_resp_msg_data_v02},
  {sizeof(qmi_coex_tech_sync_update_ind_msg_v02), qmi_coex_tech_sync_update_ind_msg_data_v02},
  {sizeof(qmi_coex_tech_state_update_req_msg_v02), qmi_coex_tech_state_update_req_msg_data_v02},
  {sizeof(qmi_coex_wwan_lte_state_ind_msg_v02), qmi_coex_wwan_lte_state_ind_msg_data_v02},
  {sizeof(qmi_coex_wwan_tdscdma_state_ind_msg_v02), qmi_coex_wwan_tdscdma_state_ind_msg_data_v02},
  {sizeof(qmi_coex_wwan_gsm_state_ind_msg_v02), qmi_coex_wwan_gsm_state_ind_msg_data_v02},
  {sizeof(qmi_coex_wwan_onex_state_ind_msg_v02), qmi_coex_wwan_onex_state_ind_msg_data_v02},
  {sizeof(qmi_coex_wwan_hdr_state_ind_msg_v02), qmi_coex_wwan_hdr_state_ind_msg_data_v02},
  {sizeof(qmi_coex_wwan_wcdma_state_ind_msg_v02), qmi_coex_wwan_wcdma_state_ind_msg_data_v02},
  {sizeof(qmi_coex_wlan_wifi_state_ind_msg_v02), qmi_coex_wlan_wifi_state_ind_msg_data_v02},
  {sizeof(qmi_coex_wlan_bt_state_ind_msg_v02), 0},
  {sizeof(qmi_coex_diversity_antenna_state_ind_msg_v02), qmi_coex_diversity_antenna_state_ind_msg_data_v02},
  {sizeof(qmi_coex_metrics_lte_sinr_start_ind_msg_v02), qmi_coex_metrics_lte_sinr_start_ind_msg_data_v02},
  {sizeof(qmi_coex_metrics_lte_sinr_read_req_msg_v02), 0},
  {sizeof(qmi_coex_metrics_lte_sinr_read_resp_msg_v02), qmi_coex_metrics_lte_sinr_read_resp_msg_data_v02},
  {sizeof(qmi_coex_metrics_lte_sinr_stop_ind_msg_v02), 0},
  {sizeof(qmi_coex_wwan_gsm2_state_ind_msg_v02), qmi_coex_wwan_gsm2_state_ind_msg_data_v02},
  {sizeof(qmi_coex_wwan_gsm3_state_ind_msg_v02), qmi_coex_wwan_gsm3_state_ind_msg_data_v02},
  {sizeof(qmi_coex_conflict_params_req_msg_v02), qmi_coex_conflict_params_req_msg_data_v02},
  {sizeof(qmi_coex_conflict_params_resp_msg_v02), qmi_coex_conflict_params_resp_msg_data_v02}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object coex_qmi_idl_type_table_object_v02;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *coex_qmi_idl_type_table_object_referenced_tables_v02[] =
{&coex_qmi_idl_type_table_object_v02, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object coex_qmi_idl_type_table_object_v02 = {
  sizeof(coex_type_table_v02)/sizeof(qmi_idl_type_table_entry ),
  sizeof(coex_message_table_v02)/sizeof(qmi_idl_message_table_entry),
  1,
  coex_type_table_v02,
  coex_message_table_v02,
  coex_qmi_idl_type_table_object_referenced_tables_v02,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry coex_service_indication_messages_v02[] = {
  {QMI_COEX_TECH_SYNC_REQ_V02, QMI_IDL_TYPE16(0, 0), 52},
  {QMI_COEX_TECH_SYNC_RESP_V02, QMI_IDL_TYPE16(0, 1), 52},
  {QMI_COEX_TECH_SYNC_UPDATE_IND_V02, QMI_IDL_TYPE16(0, 2), 5},
  {QMI_COEX_TECH_STATE_UPDATE_REQ_V02, QMI_IDL_TYPE16(0, 3), 28},
  {QMI_COEX_WWAN_LTE_STATE_IND_V02, QMI_IDL_TYPE16(0, 4), 41},
  {QMI_COEX_WWAN_TDSCDMA_STATE_IND_V02, QMI_IDL_TYPE16(0, 5), 37},
  {QMI_COEX_WWAN_GSM_STATE_IND_V02, QMI_IDL_TYPE16(0, 6), 69},
  {QMI_COEX_WWAN_ONEX_STATE_IND_V02, QMI_IDL_TYPE16(0, 7), 37},
  {QMI_COEX_WWAN_HDR_STATE_IND_V02, QMI_IDL_TYPE16(0, 8), 37},
  {QMI_COEX_WWAN_WCDMA_STATE_IND_V02, QMI_IDL_TYPE16(0, 9), 37},
  {QMI_COEX_WLAN_WIFI_STATE_IND_V02, QMI_IDL_TYPE16(0, 10), 106},
  {QMI_COEX_WLAN_BT_STATE_IND_V02, QMI_IDL_TYPE16(0, 11), 0},
  {QMI_COEX_DIVERSITY_ANTENNA_STATE_IND_V02, QMI_IDL_TYPE16(0, 12), 8},
  {QMI_COEX_METRICS_LTE_SINR_START_IND_V02, QMI_IDL_TYPE16(0, 13), 4},
  {QMI_COEX_METRICS_LTE_SINR_READ_REQ_V02, QMI_IDL_TYPE16(0, 14), 0},
  {QMI_COEX_METRICS_LTE_SINR_READ_RESP_V02, QMI_IDL_TYPE16(0, 15), 10},
  {QMI_COEX_METRICS_LTE_SINR_STOP_IND_V02, QMI_IDL_TYPE16(0, 16), 0},
  {QMI_COEX_WWAN_GSM2_STATE_IND_V02, QMI_IDL_TYPE16(0, 17), 69},
  {QMI_COEX_WWAN_GSM3_STATE_IND_V02, QMI_IDL_TYPE16(0, 18), 69},
  {QMI_COEX_CONFLICT_PARAMS_REQ_V02, QMI_IDL_TYPE16(0, 19), 4},
  {QMI_COEX_CONFLICT_PARAMS_RESP_V02, QMI_IDL_TYPE16(0, 20), 37}
};

/*Service Object*/
struct qmi_idl_service_object coex_qmi_idl_service_object_v02 = {
  0x06,
  0x02,
  0x22,
  106,
  { 0,
    0,
    sizeof(coex_service_indication_messages_v02)/sizeof(qmi_idl_service_message_table_entry) },
  { NULL, NULL, coex_service_indication_messages_v02},
  &coex_qmi_idl_type_table_object_v02,
  0x03,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type coex_get_service_object_internal_v02
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( COEX_V02_IDL_MAJOR_VERS != idl_maj_version || COEX_V02_IDL_MINOR_VERS != idl_min_version 
       || COEX_V02_IDL_TOOL_VERS != library_version) 
  {
    return NULL;
  } 
  return (qmi_idl_service_object_type)&coex_qmi_idl_service_object_v02;
}

