/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              C O E X I S T E N C E _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the coex service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/coexistence_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.1
   It was generated on: Wed Mar 13 2013 (Spin 1)
   From IDL File: coexistence_service_v01.idl */

#include "qmi_si.h"
#include "coexistence_service_v01.h"

static const qmi_implemented_optionals coex_reset_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_reset_reqresp_impl_tuple_v01 = 
{
  NULL,
  &coex_reset_resp_msg_impl_v01,
  NULL
};

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_get_supported_msgs_reqresp_impl_tuple_v01 = 
{
  NULL,
  &qmi_get_supported_msgs_resp_impl_v01,
  NULL
};

static const qmi_implemented_optionals qmi_get_supported_fields_req_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmi_get_supported_fields_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* request_fields */
    QMI_SI_TLV_SET(0x11)   |  /* response_fields */
    QMI_SI_TLV_SET(0x12)      /* indication_fields */
};

static const qmi_implemented_optionals qmi_get_supported_fields_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_fields_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const uint32_t coex_indication_register_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* report_coex_wwan_state */
    QMI_SI_TLV_SET(0x11)   |  /* report_coex_metrics_lte_bler */
    QMI_SI_TLV_SET(0x12)   |  /* report_coex_fail_condition */
    QMI_SI_TLV_SET(0x13)   |  /* report_coex_success_condition */
    QMI_SI_TLV_SET(0x14)   |  /* report_coex_sleep */
    QMI_SI_TLV_SET(0x15)   |  /* report_coex_wakeup */
    QMI_SI_TLV_SET(0x16)      /* report_coex_page_sync */
};

static const qmi_implemented_optionals coex_indication_register_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_indication_register_req_msg_impl_array_v01
};
static const qmi_implemented_optionals coex_indication_register_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coexication_register_reqresp_impl_tuple_v01 = 
{
  &coex_indication_register_req_msg_impl_v01,
  &coex_indication_register_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_wwan_state_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* lte_band_info */
    QMI_SI_TLV_SET(0x11)   |  /* lte_tdd_info */
    QMI_SI_TLV_SET(0x12)   |  /* lte_off_period */
    QMI_SI_TLV_SET(0x13)   |  /* lte_band_info_set */
    QMI_SI_TLV_SET(0x14)   |  /* tdscdma_band_info_set */
    QMI_SI_TLV_SET(0x15)   |  /* gsm_band_info_set */
    QMI_SI_TLV_SET(0x16)   |  /* onex_band_info_set */
    QMI_SI_TLV_SET(0x17)   |  /* hdr_band_info_set */
    QMI_SI_TLV_SET(0x18)      /* wcdma_band_info_set */
};

static const qmi_implemented_optionals coex_wwan_state_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_wwan_state_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_wwan_state_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &coex_wwan_state_ind_msg_impl_v01
};

static const uint32_t coex_get_wwan_state_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* lte_band_info */
    QMI_SI_TLV_SET(0x11)   |  /* lte_tdd_info */
    QMI_SI_TLV_SET(0x12)   |  /* lte_off_period */
    QMI_SI_TLV_SET(0x13)   |  /* lte_band_info_set */
    QMI_SI_TLV_SET(0x14)   |  /* tdscdma_band_info_set */
    QMI_SI_TLV_SET(0x15)   |  /* gsm_band_info_set */
    QMI_SI_TLV_SET(0x16)   |  /* onex_band_info_set */
    QMI_SI_TLV_SET(0x17)   |  /* hdr_band_info_set */
    QMI_SI_TLV_SET(0x18)      /* wcdma_band_info_set */
};

static const qmi_implemented_optionals coex_get_wwan_state_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_get_wwan_state_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_get_wwan_state_reqresp_impl_tuple_v01 = 
{
  NULL,
  &coex_get_wwan_state_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_set_wlan_state_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* scan_info */
    QMI_SI_TLV_SET(0x11)      /* conn_info */
};

static const qmi_implemented_optionals coex_set_wlan_state_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_set_wlan_state_req_msg_impl_array_v01
};
static const qmi_implemented_optionals coex_set_wlan_state_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_set_wlan_state_reqresp_impl_tuple_v01 = 
{
  &coex_set_wlan_state_req_msg_impl_v01,
  &coex_set_wlan_state_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals coex_get_wlan_scan_state_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t coex_get_wlan_scan_state_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* scan_info */
};

static const qmi_implemented_optionals coex_get_wlan_scan_state_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_get_wlan_scan_state_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_get_wlan_scan_state_reqresp_impl_tuple_v01 = 
{
  &coex_get_wlan_scan_state_req_msg_impl_v01,
  &coex_get_wlan_scan_state_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals coex_get_wlan_conn_state_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t coex_get_wlan_conn_state_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* conn_info */
};

static const qmi_implemented_optionals coex_get_wlan_conn_state_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_get_wlan_conn_state_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_get_wlan_conn_state_reqresp_impl_tuple_v01 = 
{
  &coex_get_wlan_conn_state_req_msg_impl_v01,
  &coex_get_wlan_conn_state_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_set_policy_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* policy */
    QMI_SI_TLV_SET(0x11)   |  /* power_threshold */
    QMI_SI_TLV_SET(0x12)   |  /* rb_threshold */
    QMI_SI_TLV_SET(0x13)   |  /* lte_tx_continuous_subframe_denials_threshold */
    QMI_SI_TLV_SET(0x14)   |  /* lte_tx_subrame_denial_params */
    QMI_SI_TLV_SET(0x15)   |  /* apt_table */
    QMI_SI_TLV_SET(0x16)   |  /* controller_tx_power_limit */
    QMI_SI_TLV_SET(0x17)   |  /* wci2_power_limit */
    QMI_SI_TLV_SET(0x18)   |  /* link_path_loss_threshold */
    QMI_SI_TLV_SET(0x19)   |  /* rb_filter_alpha */
    QMI_SI_TLV_SET(0x1A)   |  /* filtered_rb_threshold */
    QMI_SI_TLV_SET(0x1B)   |  /* wci2_tx_pwrlmt_timeout */
    QMI_SI_TLV_SET(0x1C)      /* controller_tx_pwrlmt_timeout */
};

static const qmi_implemented_optionals coex_set_policy_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_set_policy_req_msg_impl_array_v01
};
static const qmi_implemented_optionals coex_set_policy_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_set_policy_reqresp_impl_tuple_v01 = 
{
  &coex_set_policy_req_msg_impl_v01,
  &coex_set_policy_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_get_policy_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* policy */
    QMI_SI_TLV_SET(0x11)   |  /* power_threshold */
    QMI_SI_TLV_SET(0x12)   |  /* rb_threshold */
    QMI_SI_TLV_SET(0x13)   |  /* lte_tx_continuous_subframe_denials_threshold */
    QMI_SI_TLV_SET(0x14)   |  /* lte_tx_subrame_denial_params */
    QMI_SI_TLV_SET(0x15)   |  /* apt_table */
    QMI_SI_TLV_SET(0x16)   |  /* controller_tx_power_limit */
    QMI_SI_TLV_SET(0x17)   |  /* wci2_power_limit */
    QMI_SI_TLV_SET(0x18)   |  /* link_path_loss_threshold */
    QMI_SI_TLV_SET(0x19)   |  /* rb_filter_alpha */
    QMI_SI_TLV_SET(0x1A)   |  /* filtered_rb_threshold */
    QMI_SI_TLV_SET(0x1B)   |  /* wci2_tx_pwrlmt_timeout */
    QMI_SI_TLV_SET(0x1C)      /* controller_tx_pwrlmt_timeout */
};

static const qmi_implemented_optionals coex_get_policy_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_get_policy_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_get_policy_reqresp_impl_tuple_v01 = 
{
  NULL,
  &coex_get_policy_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals coex_metrics_lte_bler_start_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals coex_metrics_lte_bler_start_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_metrics_lte_bler_start_reqresp_impl_tuple_v01 = 
{
  &coex_metrics_lte_bler_start_req_msg_impl_v01,
  &coex_metrics_lte_bler_start_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_metrics_lte_bler_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tb_cnt */
    QMI_SI_TLV_SET(0x11)      /* errored_tb_cnt */
};

static const qmi_implemented_optionals coex_metrics_lte_bler_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_metrics_lte_bler_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_metrics_lte_bler_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &coex_metrics_lte_bler_ind_msg_impl_v01
};

static const qmi_implemented_optionals coex_metrics_lte_bler_stop_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_metrics_lte_bler_stop_reqresp_impl_tuple_v01 = 
{
  NULL,
  &coex_metrics_lte_bler_stop_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals coex_metrics_lte_sinr_start_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals coex_metrics_lte_sinr_start_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_metrics_lte_sinr_start_reqresp_impl_tuple_v01 = 
{
  &coex_metrics_lte_sinr_start_req_msg_impl_v01,
  &coex_metrics_lte_sinr_start_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_metrics_lte_sinr_read_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* sinr */
};

static const qmi_implemented_optionals coex_metrics_lte_sinr_read_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_metrics_lte_sinr_read_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_metrics_lte_sinr_read_reqresp_impl_tuple_v01 = 
{
  NULL,
  &coex_metrics_lte_sinr_read_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals coex_metrics_lte_sinr_stop_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_metrics_lte_sinr_stop_reqresp_impl_tuple_v01 = 
{
  NULL,
  &coex_metrics_lte_sinr_stop_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_set_band_filter_info_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* bands */
};

static const qmi_implemented_optionals coex_set_band_filter_info_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_set_band_filter_info_req_msg_impl_array_v01
};
static const qmi_implemented_optionals coex_set_band_filter_info_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_set_band_filter_info_reqresp_impl_tuple_v01 = 
{
  &coex_set_band_filter_info_req_msg_impl_v01,
  &coex_set_band_filter_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_get_band_filter_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* bands */
};

static const qmi_implemented_optionals coex_get_band_filter_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_get_band_filter_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_get_band_filter_info_reqresp_impl_tuple_v01 = 
{
  NULL,
  &coex_get_band_filter_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_condition_fail_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tx_subframe_denials_status */
    QMI_SI_TLV_SET(0x11)   |  /* controller_tx_pwrlmt_fail_cond */
    QMI_SI_TLV_SET(0x12)      /* wci2_tx_pwrlmt_fail_cond */
};

static const qmi_implemented_optionals coex_condition_fail_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_condition_fail_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_condition_fail_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &coex_condition_fail_ind_msg_impl_v01
};

static const uint32_t coex_condition_success_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* tx_pwrlmt_success_case */
};

static const qmi_implemented_optionals coex_condition_success_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_condition_success_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_condition_success_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &coex_condition_success_ind_msg_impl_v01
};

static const uint32_t coex_get_wci2_mws_params_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* mws_frame_sync_assert_offset */
    QMI_SI_TLV_SET(0x11)   |  /* mws_frame_sync_assert_jitter */
    QMI_SI_TLV_SET(0x12)   |  /* mws_rx_assert_offset */
    QMI_SI_TLV_SET(0x13)   |  /* mws_rx_assert_jitter */
    QMI_SI_TLV_SET(0x14)   |  /* mws_rx_deassert_offset */
    QMI_SI_TLV_SET(0x15)   |  /* mws_rx_deassert_jitter */
    QMI_SI_TLV_SET(0x16)   |  /* mws_tx_assert_offset */
    QMI_SI_TLV_SET(0x17)   |  /* mws_tx_assert_jitter */
    QMI_SI_TLV_SET(0x18)   |  /* mws_tx_deassert_offset */
    QMI_SI_TLV_SET(0x19)      /* mws_tx_deassert_jitter */
};

static const qmi_implemented_optionals coex_get_wci2_mws_params_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_get_wci2_mws_params_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_get_wci2_mws_params_reqresp_impl_tuple_v01 = 
{
  NULL,
  &coex_get_wci2_mws_params_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals coex_get_sleep_notification_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t coex_get_sleep_notification_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tech */
    QMI_SI_TLV_SET(0x11)      /* off_period_threshold */
};

static const qmi_implemented_optionals coex_get_sleep_notification_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_get_sleep_notification_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_get_sleep_notification_reqresp_impl_tuple_v01 = 
{
  &coex_get_sleep_notification_req_msg_impl_v01,
  &coex_get_sleep_notification_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals coex_set_sleep_notification_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals coex_set_sleep_notification_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_set_sleep_notification_reqresp_impl_tuple_v01 = 
{
  &coex_set_sleep_notification_req_msg_impl_v01,
  &coex_set_sleep_notification_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals coex_sleep_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_sleep_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &coex_sleep_ind_msg_impl_v01
};

static const qmi_implemented_optionals coex_wakeup_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_wakeup_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &coex_wakeup_ind_msg_impl_v01
};

static const uint32_t coex_wcn_wake_sync_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* scan_interval */
};

static const qmi_implemented_optionals coex_wcn_wake_sync_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_wcn_wake_sync_req_msg_impl_array_v01
};
static const qmi_implemented_optionals coex_wcn_wake_sync_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_coex_wcn_wake_sync_reqresp_impl_tuple_v01 = 
{
  &coex_wcn_wake_sync_req_msg_impl_v01,
  &coex_wcn_wake_sync_resp_msg_impl_v01,
  NULL
};

static const uint32_t coex_wcn_wake_sync_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* timestamp */
};

static const qmi_implemented_optionals coex_wcn_wake_sync_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  coex_wcn_wake_sync_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_coex_wcn_wake_sync_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &coex_wcn_wake_sync_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const coex_service_implemented_array_v01[] =
{
    &qmi_coex_reset_reqresp_impl_tuple_v01,              /* 0x0000 */
    NULL,                                                /* 0x0001 */
    NULL,                                                /* 0x0002 */
    NULL,                                                /* 0x0003 */
    NULL,                                                /* 0x0004 */
    NULL,                                                /* 0x0005 */
    NULL,                                                /* 0x0006 */
    NULL,                                                /* 0x0007 */
    NULL,                                                /* 0x0008 */
    NULL,                                                /* 0x0009 */
    NULL,                                                /* 0x000A */
    NULL,                                                /* 0x000B */
    NULL,                                                /* 0x000C */
    NULL,                                                /* 0x000D */
    NULL,                                                /* 0x000E */
    NULL,                                                /* 0x000F */
    NULL,                                                /* 0x0010 */
    NULL,                                                /* 0x0011 */
    NULL,                                                /* 0x0012 */
    NULL,                                                /* 0x0013 */
    NULL,                                                /* 0x0014 */
    NULL,                                                /* 0x0015 */
    NULL,                                                /* 0x0016 */
    NULL,                                                /* 0x0017 */
    NULL,                                                /* 0x0018 */
    NULL,                                                /* 0x0019 */
    NULL,                                                /* 0x001A */
    NULL,                                                /* 0x001B */
    NULL,                                                /* 0x001C */
    NULL,                                                /* 0x001D */
    &qmi_coex_get_supported_msgs_reqresp_impl_tuple_v01, /* 0x001E */
    &qmi_coex_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_coexication_register_reqresp_impl_tuple_v01,    /* 0x0020 */
    &qmi_coex_wwan_state_impl_tuple_v01,                 /* 0x0021 */
    &qmi_coex_get_wwan_state_reqresp_impl_tuple_v01,     /* 0x0022 */
    &qmi_coex_set_wlan_state_reqresp_impl_tuple_v01,     /* 0x0023 */
    &qmi_coex_get_wlan_scan_state_reqresp_impl_tuple_v01, /* 0x0024 */
    &qmi_coex_get_wlan_conn_state_reqresp_impl_tuple_v01, /* 0x0025 */
    &qmi_coex_set_policy_reqresp_impl_tuple_v01,         /* 0x0026 */
    &qmi_coex_get_policy_reqresp_impl_tuple_v01,         /* 0x0027 */
    &qmi_coex_metrics_lte_bler_start_reqresp_impl_tuple_v01, /* 0x0028 */
    &qmi_coex_metrics_lte_bler_impl_tuple_v01,           /* 0x0029 */
    &qmi_coex_metrics_lte_bler_stop_reqresp_impl_tuple_v01, /* 0x002A */
    &qmi_coex_metrics_lte_sinr_start_reqresp_impl_tuple_v01, /* 0x002B */
    &qmi_coex_metrics_lte_sinr_read_reqresp_impl_tuple_v01, /* 0x002C */
    &qmi_coex_metrics_lte_sinr_stop_reqresp_impl_tuple_v01, /* 0x002D */
    &qmi_coex_set_band_filter_info_reqresp_impl_tuple_v01, /* 0x002E */
    &qmi_coex_get_band_filter_info_reqresp_impl_tuple_v01, /* 0x002F */
    &qmi_coex_condition_fail_impl_tuple_v01,             /* 0x0030 */
    &qmi_coex_condition_success_impl_tuple_v01,          /* 0x0031 */
    &qmi_coex_get_wci2_mws_params_reqresp_impl_tuple_v01, /* 0x0032 */
    &qmi_coex_get_sleep_notification_reqresp_impl_tuple_v01, /* 0x0033 */
    &qmi_coex_set_sleep_notification_reqresp_impl_tuple_v01, /* 0x0034 */
    &qmi_coex_sleep_impl_tuple_v01,                      /* 0x0035 */
    &qmi_coex_wakeup_impl_tuple_v01,                     /* 0x0036 */
    &qmi_coex_wcn_wake_sync_reqresp_impl_tuple_v01,      /* 0x0037 */
    &qmi_coex_wcn_wake_sync_impl_tuple_v01               /* 0x0038 */
};

static const qmi_implemented_messages coex_service_implemented_v01 =
{
  0x0038,  /* maximum message ID in this service */
  coex_service_implemented_array_v01
};

const qmi_implemented_messages *coex_get_service_impl_v01 (void)
{
  return &coex_service_implemented_v01;
}
