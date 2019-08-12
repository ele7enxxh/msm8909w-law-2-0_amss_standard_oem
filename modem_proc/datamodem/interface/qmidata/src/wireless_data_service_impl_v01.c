/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              W I R E L E S S _ D A T A _ S E R V I C E _ I M P L _ V 0 1   . C

GENERAL DESCRIPTION
  This is the file which defines the wds service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.  All Rights Reserved. 
 QUALCOMM Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/wireless_data_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.6
   It was generated on: Mon Dec 10 2012
   From IDL File: wireless_data_service_v01.idl */

#include "qmi_si.h"
#include "wireless_data_service_v01.h"

static const qmi_implemented_optionals_tuple qmi_wds_reset_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_set_event_report_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* report_channel_rate */
    QMI_SI_TLV_SET(0x11)   |  /* report_stats */
    QMI_SI_TLV_SET(0x12)   |  /* report_data_bearer_tech */
    QMI_SI_TLV_SET(0x13)   |  /* report_dormancy_status */
    QMI_SI_TLV_SET(0x14)   |  /* report_mip_status */
    QMI_SI_TLV_SET(0x15)   |  /* report_current_data_bearer_tech */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_SET(0x17)   |  /* report_data_call_status_change */
    QMI_SI_TLV_SET(0x18)   |  /* report_preferred_data_system */
    QMI_SI_TLV_SET(0x19)   |  /* report_evdo_page_monitor_period_change */
    QMI_SI_TLV_SET(0x1A)   |  /* report_data_system_status */
    QMI_SI_TLV_SET(0x1B)   |  /* report_uplink_flow_control */
    QMI_SI_TLV_SET(0x1C)   |  /* limited_data_system_status */
    QMI_SI_TLV_SET(0x1D)   |  /* report_additional_pdn_filters_removal */
#ifdef FEATURE_DATA_WLAN_MAPCON
    QMI_SI_TLV_SET(0x1E)      /* report_data_bearer_tech_ex */
#else
    QMI_SI_TLV_CLEAR(0x1E)    /* report_data_bearer_tech_ex */
#endif /* FEATURE_DATA_WLAN_MAPCON */
};

static const qmi_implemented_optionals wds_set_event_report_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_set_event_report_req_msg_impl_array_v01
};
static const uint32_t wds_event_report_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tx_ok_count */
    QMI_SI_TLV_SET(0x11)   |  /* rx_ok_count */
    QMI_SI_TLV_SET(0x12)   |  /* tx_err_count */
    QMI_SI_TLV_SET(0x13)   |  /* rx_err_count */
    QMI_SI_TLV_SET(0x14)   |  /* tx_ofl_count */
    QMI_SI_TLV_SET(0x15)   |  /* rx_ofl_count */
    QMI_SI_TLV_SET(0x16)   |  /* channel_rate */
    QMI_SI_TLV_SET(0x17)   |  /* data_bearer_tech */
    QMI_SI_TLV_SET(0x18)   |  /* dormancy_status */
    QMI_SI_TLV_SET(0x19)   |  /* tx_ok_bytes_count */
    QMI_SI_TLV_SET(0x1A)   |  /* rx_ok_bytes_count */
    QMI_SI_TLV_SET(0x1B)   |  /* mip_status */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_SET(0x1D)   |  /* current_bearer_tech */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_SET(0x1F)   |  /* data_call_status */
    QMI_SI_TLV_SET(0x20)   |  /* current_sys */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_SET(0x22)   |  /* data_call_type */
    QMI_SI_TLV_SET(0x23)   |  /* evdo_page_monitor_period_change */
    QMI_SI_TLV_SET(0x24)   |  /* system_status */
    QMI_SI_TLV_SET(0x25)   |  /* tx_dropped_count */
    QMI_SI_TLV_SET(0x26)   |  /* rx_dropped_count */
    QMI_SI_TLV_SET(0x27)   |  /* uplink_flow_control */
    QMI_SI_TLV_CLEAR(0x28)   |  /* data_call_addr_family */
    QMI_SI_TLV_SET(0x29)     | /* removed_filter_handles */
#ifdef FEATURE_DATA_WLAN_MAPCON
    QMI_SI_TLV_SET(0x2A)      /* bearer_tech_ex */
#else
    QMI_SI_TLV_CLEAR(0x2A)    /* bearer_tech_ex */
#endif /* FEATURE_DATA_WLAN_MAPCON */
};

static const qmi_implemented_optionals wds_event_report_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_event_report_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_set_event_report_reqresp_impl_tuple_v01 = 
{
  &wds_set_event_report_req_msg_impl_v01,
  NULL,
  &wds_event_report_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_wds_abort_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_indication_register_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* report_embms_tmgi_list */
    QMI_SI_TLV_SET(0x11)   |  /* suppress_pkt_srvc_ind */
    QMI_SI_TLV_SET(0x12)   |  /* report_extended_ip_config_change */
#ifdef FEATURE_DATA_LTE
    QMI_SI_TLV_SET(0x13)   |  /* report_lte_attach_pdn_list_change */
#else
    QMI_SI_TLV_CLEAR(0x13) |  /* report_lte_attach_pdn_list_change */
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_EPC_HANDOFF
    QMI_SI_TLV_SET(0x15)   |   /* report_handoff_information */
#else
    QMI_SI_TLV_CLEAR(0x15) |   /* report_handoff_information */
#endif /* FEATURE_EPC_HANDOFF */
    QMI_SI_TLV_SET(0x18)      /* report_throughput */
};

static const qmi_implemented_optionals wds_indication_register_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_indication_register_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wdsication_register_reqresp_impl_tuple_v01 = 
{
  &wds_indication_register_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wds_start_network_interface_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* primary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x11)   |  /* secondary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x12)   |  /* primary_nbns_address_pref */
    QMI_SI_TLV_SET(0x13)   |  /* secondary_nbns_address_pref */
    QMI_SI_TLV_SET(0x14)   |  /* apn_name */
    QMI_SI_TLV_SET(0x15)   |  /* ipv4_address_pref */
    QMI_SI_TLV_SET(0x16)   |  /* authentication_preference */
    QMI_SI_TLV_SET(0x17)   |  /* username */
    QMI_SI_TLV_SET(0x18)   |  /* password */
    QMI_SI_TLV_SET(0x19)   |  /* ip_family_preference */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x20) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x22) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x23) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x24) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x25) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x26) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x27) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x28) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x29) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2F)    /* Undefined */
  ,

    QMI_SI_TLV_SET(0x30)   |  /* technology_preference */
    QMI_SI_TLV_SET(0x31)   |  /* profile_index */
    QMI_SI_TLV_SET(0x32)   |  /* profile_index_3gpp2 */
    QMI_SI_TLV_SET(0x33)   |  /* enable_autoconnect */
    QMI_SI_TLV_SET(0x34)   |  /* ext_technology_preference */
    QMI_SI_TLV_SET(0x35)   |  /* call_type */
#ifdef FEATURE_DATA_LTE
    QMI_SI_TLV_SET(0x36)   |   /* handoff_context */
#else
    QMI_SI_TLV_CLEAR(0x36) |   /* handoff_context */
#endif /* FEATURE_DATA_LTE */
    QMI_SI_TLV_SET(0x37)      /* ips_id */
};

static const qmi_implemented_optionals wds_start_network_interface_req_msg_impl_v01 =
{
  2, /* number of elements in array */
  wds_start_network_interface_req_msg_impl_array_v01
};
static const uint32_t wds_start_network_interface_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_end_reason */
    QMI_SI_TLV_SET(0x11)      /* verbose_call_end_reason */
};

static const qmi_implemented_optionals wds_start_network_interface_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_start_network_interface_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_start_network_interface_reqresp_impl_tuple_v01 = 
{
  &wds_start_network_interface_req_msg_impl_v01,
  &wds_start_network_interface_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_stop_network_interface_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* disable_autoconnect */
};

static const qmi_implemented_optionals wds_stop_network_interface_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_stop_network_interface_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_stop_network_interface_reqresp_impl_tuple_v01 = 
{
  &wds_stop_network_interface_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wds_pkt_srvc_status_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_end_reason */
    QMI_SI_TLV_SET(0x11)   |  /* verbose_call_end_reason */
    QMI_SI_TLV_SET(0x12)   |  /* ip_family */
    QMI_SI_TLV_SET(0x13)      /* tech_name */
};

static const qmi_implemented_optionals wds_pkt_srvc_status_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_pkt_srvc_status_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_pkt_srvc_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &wds_pkt_srvc_status_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_wds_get_current_channel_rate_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_get_pkt_statistics_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tx_ok_count */
    QMI_SI_TLV_SET(0x11)   |  /* rx_ok_count */
    QMI_SI_TLV_SET(0x12)   |  /* tx_err_count */
    QMI_SI_TLV_SET(0x13)   |  /* rx_err_count */
    QMI_SI_TLV_SET(0x14)   |  /* tx_ofl_count */
    QMI_SI_TLV_SET(0x15)   |  /* rx_ofl_count */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_SET(0x19)   |  /* tx_ok_bytes_count */
    QMI_SI_TLV_SET(0x1A)   |  /* rx_ok_bytes_count */
    QMI_SI_TLV_SET(0x1B)   |  /* last_call_tx_ok_bytes_count */
    QMI_SI_TLV_SET(0x1C)   |  /* last_call_rx_ok_bytes_count */
    QMI_SI_TLV_SET(0x1D)   |  /* tx_dropped_count */
    QMI_SI_TLV_SET(0x1E)      /* rx_dropped_count */
};

static const qmi_implemented_optionals wds_get_pkt_statistics_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_pkt_statistics_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_pkt_statistics_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_pkt_statistics_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_go_dormant_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_go_active_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_create_profile_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* profile_name */
    QMI_SI_TLV_SET(0x11)   |  /* pdp_type */
    QMI_SI_TLV_SET(0x12)   |  /* pdp_hdr_compression_type */
    QMI_SI_TLV_SET(0x13)   |  /* pdp_data_compression_type */
    QMI_SI_TLV_SET(0x14)   |  /* apn_name */
    QMI_SI_TLV_SET(0x15)   |  /* primary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x16)   |  /* secondary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x17)   |  /* umts_requested_qos */
    QMI_SI_TLV_SET(0x18)   |  /* umts_minimum_qos */
    QMI_SI_TLV_SET(0x19)   |  /* gprs_requested_qos */
    QMI_SI_TLV_SET(0x1A)   |  /* gprs_minimum_qos */
    QMI_SI_TLV_SET(0x1B)   |  /* username */
    QMI_SI_TLV_SET(0x1C)   |  /* password */
    QMI_SI_TLV_SET(0x1D)   |  /* authentication_preference */
    QMI_SI_TLV_SET(0x1E)   |  /* ipv4_address_preference */
    QMI_SI_TLV_SET(0x1F)   |  /* pcscf_addr_using_pco */
    QMI_SI_TLV_SET(0x20)   |  /* pdp_access_control_flag */
    QMI_SI_TLV_SET(0x21)   |  /* pcscf_addr_using_dhcp */
    QMI_SI_TLV_SET(0x22)   |  /* im_cn_flag */
    QMI_SI_TLV_SET(0x23)   |  /* tft_id1_params */
    QMI_SI_TLV_SET(0x24)   |  /* tft_id2_params */
    QMI_SI_TLV_SET(0x25)   |  /* pdp_context */
    QMI_SI_TLV_SET(0x26)   |  /* secondary_flag */
    QMI_SI_TLV_SET(0x27)   |  /* primary_id */
    QMI_SI_TLV_SET(0x28)   |  /* ipv6_address_preference */
    QMI_SI_TLV_SET(0x29)   |  /* umts_requested_qos_with_sig_ind */
    QMI_SI_TLV_SET(0x2A)   |  /* umts_minimum_qos_with_sig_ind */
    QMI_SI_TLV_SET(0x2B)   |  /* primary_dns_ipv6_address_preference */
    QMI_SI_TLV_SET(0x2C)   |  /* secodnary_dns_ipv6_address_preference */
    QMI_SI_TLV_SET(0x2D)   |  /* addr_allocation_preference */
    QMI_SI_TLV_SET(0x2E)   |  /* threegpp_lte_qos_params */
    QMI_SI_TLV_SET(0x2F)      /* apn_disabled_flag */
  ,

    QMI_SI_TLV_SET(0x30)   |  /* pdn_inactivity_timeout */
    QMI_SI_TLV_SET(0x31)   |  /* apn_class */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_SET(0x35)   |  /* apn_bearer */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_SET(0x8F)      /* persistent */
  ,

    QMI_SI_TLV_SET(0x90)   |  /* negotiate_dns_server_preference */
    QMI_SI_TLV_SET(0x91)   |  /* ppp_session_close_timer_DO */
    QMI_SI_TLV_SET(0x92)   |  /* ppp_session_close_timer_1x */
    QMI_SI_TLV_SET(0x93)   |  /* allow_linger */
    QMI_SI_TLV_SET(0x94)   |  /* lcp_ack_timeout */
    QMI_SI_TLV_SET(0x95)   |  /* ipcp_ack_timeout */
    QMI_SI_TLV_SET(0x96)   |  /* auth_timeout */
    QMI_SI_TLV_SET(0x97)   |  /* lcp_creq_retry_count */
    QMI_SI_TLV_SET(0x98)   |  /* ipcp_creq_retry_count */
    QMI_SI_TLV_SET(0x99)   |  /* auth_retry_count */
    QMI_SI_TLV_SET(0x9A)   |  /* auth_protocol */
    QMI_SI_TLV_SET(0x9B)   |  /* user_id */
    QMI_SI_TLV_SET(0x9C)   |  /* auth_password */
    QMI_SI_TLV_SET(0x9D)   |  /* data_rate */
    QMI_SI_TLV_SET(0x9E)   |  /* app_type */
    QMI_SI_TLV_SET(0x9F)   |  /* data_mode */
    QMI_SI_TLV_SET(0xA0)   |  /* app_priority */
    QMI_SI_TLV_SET(0xA1)   |  /* apn_string */
    QMI_SI_TLV_SET(0xA2)   |  /* pdn_type */
    QMI_SI_TLV_SET(0xA3)   |  /* is_pcscf_address_needed */
    QMI_SI_TLV_SET(0xA4)   |  /* primary_v4_dns_address */
    QMI_SI_TLV_SET(0xA5)   |  /* secondary_v4_dns_address */
    QMI_SI_TLV_SET(0xA6)   |  /* primary_v6_dns_address */
    QMI_SI_TLV_SET(0xA7)   |  /* secondary_v6_dns_address */
    QMI_SI_TLV_SET(0xA8)   |  /* rat_type */
    QMI_SI_TLV_SET(0xA9)   |  /* apn_enabled_3gpp2 */
    QMI_SI_TLV_SET(0xAA)   |  /* pdn_inactivity_timeout_3gpp2 */
    QMI_SI_TLV_SET(0xAB)   |  /* apn_class_3gpp2 */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_SET(0xAD)   |  /* pdn_level_auth_protocol */
    QMI_SI_TLV_SET(0xAE)   |  /* pdn_level_user_id */
    QMI_SI_TLV_SET(0xAF)      /* pdn_level_auth_password */
  ,

    QMI_SI_TLV_SET(0xB0)      /* pdn_label */
};

static const qmi_implemented_optionals wds_create_profile_req_msg_impl_v01 =
{
  6, /* number of elements in array */
  wds_create_profile_req_msg_impl_array_v01
};
static const uint32_t wds_create_profile_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x14) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x15) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x20) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x22) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x23) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x24) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x25) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x26) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x27) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x28) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x29) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x30) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x31) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x35) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x90) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x91) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x92) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x93) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x94) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x95) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x96) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x97) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x98) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x99) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xB0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_create_profile_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_create_profile_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_create_profile_reqresp_impl_tuple_v01 = 
{
  &wds_create_profile_req_msg_impl_v01,
  &wds_create_profile_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_modify_profile_settings_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* profile_name */
    QMI_SI_TLV_SET(0x11)   |  /* pdp_type */
    QMI_SI_TLV_SET(0x12)   |  /* pdp_hdr_compression_type */
    QMI_SI_TLV_SET(0x13)   |  /* pdp_data_compression_type */
    QMI_SI_TLV_SET(0x14)   |  /* apn_name */
    QMI_SI_TLV_SET(0x15)   |  /* primary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x16)   |  /* secondary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x17)   |  /* umts_requested_qos */
    QMI_SI_TLV_SET(0x18)   |  /* umts_minimum_qos */
    QMI_SI_TLV_SET(0x19)   |  /* gprs_requested_qos */
    QMI_SI_TLV_SET(0x1A)   |  /* gprs_minimum_qos */
    QMI_SI_TLV_SET(0x1B)   |  /* username */
    QMI_SI_TLV_SET(0x1C)   |  /* password */
    QMI_SI_TLV_SET(0x1D)   |  /* authentication_preference */
    QMI_SI_TLV_SET(0x1E)   |  /* ipv4_address_preference */
    QMI_SI_TLV_SET(0x1F)   |  /* pcscf_addr_using_pco */
    QMI_SI_TLV_SET(0x20)   |  /* pdp_access_control_flag */
    QMI_SI_TLV_SET(0x21)   |  /* pcscf_addr_using_dhcp */
    QMI_SI_TLV_SET(0x22)   |  /* im_cn_flag */
    QMI_SI_TLV_SET(0x23)   |  /* tft_id1_params */
    QMI_SI_TLV_SET(0x24)   |  /* tft_id2_params */
    QMI_SI_TLV_SET(0x25)   |  /* pdp_context */
    QMI_SI_TLV_SET(0x26)   |  /* secondary_flag */
    QMI_SI_TLV_SET(0x27)   |  /* primary_id */
    QMI_SI_TLV_SET(0x28)   |  /* ipv6_address_preference */
    QMI_SI_TLV_SET(0x29)   |  /* umts_requested_qos_with_sig_ind */
    QMI_SI_TLV_SET(0x2A)   |  /* umts_minimum_qos_with_sig_ind */
    QMI_SI_TLV_SET(0x2B)   |  /* primary_dns_ipv6_address_preference */
    QMI_SI_TLV_SET(0x2C)   |  /* secodnary_dns_ipv6_address_preference */
    QMI_SI_TLV_SET(0x2D)   |  /* addr_allocation_preference */
    QMI_SI_TLV_SET(0x2E)   |  /* threegpp_lte_qos_params */
    QMI_SI_TLV_SET(0x2F)      /* apn_disabled_flag */
  ,

    QMI_SI_TLV_SET(0x30)   |  /* pdn_inactivity_timeout */
    QMI_SI_TLV_SET(0x31)   |  /* apn_class */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_SET(0x35)   |  /* apn_bearer */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_SET(0x90)   |  /* negotiate_dns_server_preference */
    QMI_SI_TLV_SET(0x91)   |  /* ppp_session_close_timer_DO */
    QMI_SI_TLV_SET(0x92)   |  /* ppp_session_close_timer_1x */
    QMI_SI_TLV_SET(0x93)   |  /* allow_linger */
    QMI_SI_TLV_SET(0x94)   |  /* lcp_ack_timeout */
    QMI_SI_TLV_SET(0x95)   |  /* ipcp_ack_timeout */
    QMI_SI_TLV_SET(0x96)   |  /* auth_timeout */
    QMI_SI_TLV_SET(0x97)   |  /* lcp_creq_retry_count */
    QMI_SI_TLV_SET(0x98)   |  /* ipcp_creq_retry_count */
    QMI_SI_TLV_SET(0x99)   |  /* auth_retry_count */
    QMI_SI_TLV_SET(0x9A)   |  /* auth_protocol */
    QMI_SI_TLV_SET(0x9B)   |  /* user_id */
    QMI_SI_TLV_SET(0x9C)   |  /* auth_password */
    QMI_SI_TLV_SET(0x9D)   |  /* data_rate */
    QMI_SI_TLV_SET(0x9E)   |  /* app_type */
    QMI_SI_TLV_SET(0x9F)   |  /* data_mode */
    QMI_SI_TLV_SET(0xA0)   |  /* app_priority */
    QMI_SI_TLV_SET(0xA1)   |  /* apn_string */
    QMI_SI_TLV_SET(0xA2)   |  /* pdn_type */
    QMI_SI_TLV_SET(0xA3)   |  /* is_pcscf_address_needed */
    QMI_SI_TLV_SET(0xA4)   |  /* primary_v4_dns_address */
    QMI_SI_TLV_SET(0xA5)   |  /* secondary_v4_dns_address */
    QMI_SI_TLV_SET(0xA6)   |  /* primary_v6_dns_address */
    QMI_SI_TLV_SET(0xA7)   |  /* secondary_v6_dns_address */
    QMI_SI_TLV_SET(0xA8)   |  /* rat_type */
    QMI_SI_TLV_SET(0xA9)   |  /* apn_enabled_3gpp2 */
    QMI_SI_TLV_SET(0xAA)   |  /* pdn_inactivity_timeout_3gpp2 */
    QMI_SI_TLV_SET(0xAB)   |  /* apn_class_3gpp2 */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_SET(0xAD)   |  /* pdn_level_auth_protocol */
    QMI_SI_TLV_SET(0xAE)   |  /* pdn_level_user_id */
    QMI_SI_TLV_SET(0xAF)      /* pdn_level_auth_password */
  ,

    QMI_SI_TLV_SET(0xB0)      /* pdn_label */
};

static const qmi_implemented_optionals wds_modify_profile_settings_req_msg_impl_v01 =
{
  6, /* number of elements in array */
  wds_modify_profile_settings_req_msg_impl_array_v01
};
static const uint32_t wds_modify_profile_settings_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x14) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x15) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x20) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x22) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x23) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x24) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x25) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x26) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x27) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x28) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x29) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x30) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x31) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x35) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x90) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x91) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x92) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x93) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x94) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x95) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x96) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x97) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x98) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x99) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xB0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_modify_profile_settings_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_modify_profile_settings_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_modify_profile_settings_reqresp_impl_tuple_v01 = 
{
  &wds_modify_profile_settings_req_msg_impl_v01,
  &wds_modify_profile_settings_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_delete_profile_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x14) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x15) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x20) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x22) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x23) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x24) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x25) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x26) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x27) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x28) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x29) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x30) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x31) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x35) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x90) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x91) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x92) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x93) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x94) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x95) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x96) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x97) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x98) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x99) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xB0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_delete_profile_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_delete_profile_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_delete_profile_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_delete_profile_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_profile_list_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* profile_type */
};

static const qmi_implemented_optionals wds_get_profile_list_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_profile_list_req_msg_impl_array_v01
};
static const uint32_t wds_get_profile_list_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x14) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x15) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x20) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x22) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x23) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x24) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x25) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x26) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x27) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x28) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x29) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x30) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x31) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x35) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x90) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x91) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x92) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x93) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x94) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x95) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x96) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x97) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x98) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x99) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xB0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_get_profile_list_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_get_profile_list_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_profile_list_reqresp_impl_tuple_v01 = 
{
  &wds_get_profile_list_req_msg_impl_v01,
  &wds_get_profile_list_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_profile_settings_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* profile_name */
    QMI_SI_TLV_SET(0x11)   |  /* pdp_type */
    QMI_SI_TLV_SET(0x12)   |  /* pdp_hdr_compression_type */
    QMI_SI_TLV_SET(0x13)   |  /* pdp_data_compression_type */
    QMI_SI_TLV_SET(0x14)   |  /* apn_name */
    QMI_SI_TLV_SET(0x15)   |  /* primary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x16)   |  /* secondary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x17)   |  /* umts_requested_qos */
    QMI_SI_TLV_SET(0x18)   |  /* umts_minimum_qos */
    QMI_SI_TLV_SET(0x19)   |  /* gprs_requested_qos */
    QMI_SI_TLV_SET(0x1A)   |  /* gprs_minimum_qos */
    QMI_SI_TLV_SET(0x1B)   |  /* username */
    QMI_SI_TLV_SET(0x1C)   |  /* password */
    QMI_SI_TLV_SET(0x1D)   |  /* authentication_preference */
    QMI_SI_TLV_SET(0x1E)   |  /* ipv4_address_preference */
    QMI_SI_TLV_SET(0x1F)   |  /* pcscf_addr_using_pco */
    QMI_SI_TLV_SET(0x20)   |  /* pdp_access_control_flag */
    QMI_SI_TLV_SET(0x21)   |  /* pcscf_addr_using_dhcp */
    QMI_SI_TLV_SET(0x22)   |  /* im_cn_flag */
    QMI_SI_TLV_SET(0x23)   |  /* tft_id1_params */
    QMI_SI_TLV_SET(0x24)   |  /* tft_id2_params */
    QMI_SI_TLV_SET(0x25)   |  /* pdp_context */
    QMI_SI_TLV_SET(0x26)   |  /* secondary_flag */
    QMI_SI_TLV_SET(0x27)   |  /* primary_id */
    QMI_SI_TLV_SET(0x28)   |  /* ipv6_address_preference */
    QMI_SI_TLV_SET(0x29)   |  /* umts_requested_qos_with_sig_ind */
    QMI_SI_TLV_SET(0x2A)   |  /* umts_minimum_qos_with_sig_ind */
    QMI_SI_TLV_SET(0x2B)   |  /* primary_dns_ipv6_address_preference */
    QMI_SI_TLV_SET(0x2C)   |  /* secodnary_dns_ipv6_address_preference */
    QMI_SI_TLV_SET(0x2D)   |  /* addr_allocation_preference */
    QMI_SI_TLV_SET(0x2E)   |  /* threegpp_lte_qos_params */
    QMI_SI_TLV_SET(0x2F)      /* apn_disabled_flag */
  ,

    QMI_SI_TLV_SET(0x30)   |  /* pdn_inactivity_timeout */
    QMI_SI_TLV_SET(0x31)   |  /* apn_class */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_SET(0x35)   |  /* apn_bearer */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_SET(0x90)   |  /* negotiate_dns_server_preference */
    QMI_SI_TLV_SET(0x91)   |  /* ppp_session_close_timer_DO */
    QMI_SI_TLV_SET(0x92)   |  /* ppp_session_close_timer_1x */
    QMI_SI_TLV_SET(0x93)   |  /* allow_linger */
    QMI_SI_TLV_SET(0x94)   |  /* lcp_ack_timeout */
    QMI_SI_TLV_SET(0x95)   |  /* ipcp_ack_timeout */
    QMI_SI_TLV_SET(0x96)   |  /* auth_timeout */
    QMI_SI_TLV_SET(0x97)   |  /* lcp_creq_retry_count */
    QMI_SI_TLV_SET(0x98)   |  /* ipcp_creq_retry_count */
    QMI_SI_TLV_SET(0x99)   |  /* auth_retry_count */
    QMI_SI_TLV_SET(0x9A)   |  /* auth_protocol */
    QMI_SI_TLV_SET(0x9B)   |  /* user_id */
    QMI_SI_TLV_SET(0x9C)   |  /* auth_password */
    QMI_SI_TLV_SET(0x9D)   |  /* data_rate */
    QMI_SI_TLV_SET(0x9E)   |  /* app_type */
    QMI_SI_TLV_SET(0x9F)   |  /* data_mode */
    QMI_SI_TLV_SET(0xA0)   |  /* app_priority */
    QMI_SI_TLV_SET(0xA1)   |  /* apn_string */
    QMI_SI_TLV_SET(0xA2)   |  /* pdn_type */
    QMI_SI_TLV_SET(0xA3)   |  /* is_pcscf_address_needed */
    QMI_SI_TLV_SET(0xA4)   |  /* primary_v4_dns_address */
    QMI_SI_TLV_SET(0xA5)   |  /* secondary_v4_dns_address */
    QMI_SI_TLV_SET(0xA6)   |  /* primary_v6_dns_address */
    QMI_SI_TLV_SET(0xA7)   |  /* secondary_v6_dns_address */
    QMI_SI_TLV_SET(0xA8)   |  /* rat_type */
    QMI_SI_TLV_SET(0xA9)   |  /* apn_enabled_3gpp2 */
    QMI_SI_TLV_SET(0xAA)   |  /* pdn_inactivity_timeout_3gpp2 */
    QMI_SI_TLV_SET(0xAB)   |  /* apn_class_3gpp2 */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_SET(0xAD)   |  /* pdn_level_auth_protocol */
    QMI_SI_TLV_SET(0xAE)   |  /* pdn_level_user_id */
    QMI_SI_TLV_SET(0xAF)      /* pdn_level_auth_password */
  ,

    QMI_SI_TLV_SET(0xB0)   |  /* pdn_label */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_get_profile_settings_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_get_profile_settings_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_profile_settings_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_profile_settings_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_default_settings_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* profile_name */
    QMI_SI_TLV_SET(0x11)   |  /* pdp_type */
    QMI_SI_TLV_SET(0x12)   |  /* pdp_hdr_compression_type */
    QMI_SI_TLV_SET(0x13)   |  /* pdp_data_compression_type */
    QMI_SI_TLV_SET(0x14)   |  /* apn_name */
    QMI_SI_TLV_SET(0x15)   |  /* primary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x16)   |  /* secondary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x17)   |  /* umts_requested_qos */
    QMI_SI_TLV_SET(0x18)   |  /* umts_minimum_qos */
    QMI_SI_TLV_SET(0x19)   |  /* gprs_requested_qos */
    QMI_SI_TLV_SET(0x1A)   |  /* gprs_minimum_qos */
    QMI_SI_TLV_SET(0x1B)   |  /* username */
    QMI_SI_TLV_SET(0x1C)   |  /* password */
    QMI_SI_TLV_SET(0x1D)   |  /* authentication_preference */
    QMI_SI_TLV_SET(0x1E)   |  /* ipv4_address_preference */
    QMI_SI_TLV_SET(0x1F)   |  /* pcscf_addr_using_pco */
    QMI_SI_TLV_SET(0x20)   |  /* pdp_access_control_flag */
    QMI_SI_TLV_SET(0x21)   |  /* pcscf_addr_using_dhcp */
    QMI_SI_TLV_SET(0x22)   |  /* im_cn_flag */
    QMI_SI_TLV_SET(0x23)   |  /* tft_id1_params */
    QMI_SI_TLV_SET(0x24)   |  /* tft_id2_params */
    QMI_SI_TLV_SET(0x25)   |  /* pdp_context */
    QMI_SI_TLV_SET(0x26)   |  /* secondary_flag */
    QMI_SI_TLV_SET(0x27)   |  /* primary_id */
    QMI_SI_TLV_SET(0x28)   |  /* ipv6_address_preference */
    QMI_SI_TLV_SET(0x29)   |  /* umts_requested_qos_with_sig_ind */
    QMI_SI_TLV_SET(0x2A)   |  /* umts_minimum_qos_with_sig_ind */
    QMI_SI_TLV_SET(0x2B)   |  /* primary_dns_ipv6_address_preference */
    QMI_SI_TLV_SET(0x2C)   |  /* secodnary_dns_ipv6_address_preference */
    QMI_SI_TLV_SET(0x2D)   |  /* addr_allocation_preference */
    QMI_SI_TLV_SET(0x2E)   |  /* threegpp_lte_qos_params */
    QMI_SI_TLV_SET(0x2F)      /* apn_disabled_flag */
  ,

    QMI_SI_TLV_SET(0x30)   |  /* pdn_inactivity_timeout */
    QMI_SI_TLV_SET(0x31)   |  /* apn_class */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_SET(0x35)   |  /* apn_bearer */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_SET(0x90)   |  /* negotiate_dns_server_preference */
    QMI_SI_TLV_SET(0x91)   |  /* ppp_session_close_timer_DO */
    QMI_SI_TLV_SET(0x92)   |  /* ppp_session_close_timer_1x */
    QMI_SI_TLV_SET(0x93)   |  /* allow_linger */
    QMI_SI_TLV_SET(0x94)   |  /* lcp_ack_timeout */
    QMI_SI_TLV_SET(0x95)   |  /* ipcp_ack_timeout */
    QMI_SI_TLV_SET(0x96)   |  /* auth_timeout */
    QMI_SI_TLV_SET(0x97)   |  /* lcp_creq_retry_count */
    QMI_SI_TLV_SET(0x98)   |  /* ipcp_creq_retry_count */
    QMI_SI_TLV_SET(0x99)   |  /* auth_retry_count */
    QMI_SI_TLV_SET(0x9A)   |  /* auth_protocol */
    QMI_SI_TLV_SET(0x9B)   |  /* user_id */
    QMI_SI_TLV_SET(0x9C)   |  /* auth_password */
    QMI_SI_TLV_SET(0x9D)   |  /* data_rate */
    QMI_SI_TLV_SET(0x9E)   |  /* app_type */
    QMI_SI_TLV_SET(0x9F)   |  /* data_mode */
    QMI_SI_TLV_SET(0xA0)   |  /* app_priority */
    QMI_SI_TLV_SET(0xA1)   |  /* apn_string */
    QMI_SI_TLV_SET(0xA2)   |  /* pdn_type */
    QMI_SI_TLV_SET(0xA3)   |  /* is_pcscf_address_needed */
    QMI_SI_TLV_SET(0xA4)   |  /* primary_v4_dns_address */
    QMI_SI_TLV_SET(0xA5)   |  /* secondary_v4_dns_address */
    QMI_SI_TLV_SET(0xA6)   |  /* primary_v6_dns_address */
    QMI_SI_TLV_SET(0xA7)   |  /* secondary_v6_dns_address */
    QMI_SI_TLV_SET(0xA8)   |  /* rat_type */
    QMI_SI_TLV_SET(0xA9)   |  /* apn_enabled_3gpp2 */
    QMI_SI_TLV_SET(0xAA)   |  /* pdn_inactivity_timeout_3gpp2 */
    QMI_SI_TLV_SET(0xAB)   |  /* apn_class_3gpp2 */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_SET(0xAD)   |  /* pdn_level_auth_protocol */
    QMI_SI_TLV_SET(0xAE)   |  /* pdn_level_user_id */
    QMI_SI_TLV_SET(0xAF)      /* pdn_level_auth_password */
  ,

    QMI_SI_TLV_SET(0xB0)   |  /* pdn_label */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_get_default_settings_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_get_default_settings_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_default_settings_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_default_settings_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_runtime_settings_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* requested_settings */
};

static const qmi_implemented_optionals wds_get_runtime_settings_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_runtime_settings_req_msg_impl_array_v01
};
static const uint32_t wds_get_runtime_settings_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* profile_name */
    QMI_SI_TLV_SET(0x11)   |  /* pdp_type */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_SET(0x14)   |  /* apn_name */
    QMI_SI_TLV_SET(0x15)   |  /* primary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x16)   |  /* secondary_DNS_IPv4_address_preference */
    QMI_SI_TLV_SET(0x17)   |  /* umts_requested_qos */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_SET(0x19)   |  /* gprs_requested_qos */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_SET(0x1B)   |  /* username */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_SET(0x1D)   |  /* authentication_preference */
    QMI_SI_TLV_SET(0x1E)   |  /* ipv4_address_preference */
    QMI_SI_TLV_SET(0x1F)   |  /* profile */
    QMI_SI_TLV_SET(0x20)   |  /* ipv4_gateway_addr */
    QMI_SI_TLV_SET(0x21)   |  /* ipv4_subnet_mask */
    QMI_SI_TLV_SET(0x22)   |  /* pcscf_addr_using_pco */
    QMI_SI_TLV_SET(0x23)   |  /* pcscf_ipv4_server_addr_list */
    QMI_SI_TLV_SET(0x24)   |  /* fqdn */
    QMI_SI_TLV_SET(0x25)   |  /* ipv6_addr */
    QMI_SI_TLV_SET(0x26)   |  /* ipv6_gateway_addr */
    QMI_SI_TLV_SET(0x27)   |  /* primary_dns_IPv6_address */
    QMI_SI_TLV_SET(0x28)   |  /* secondary_dns_IPv6_address */
    QMI_SI_TLV_SET(0x29)   |  /* mtu */
    QMI_SI_TLV_SET(0x2A)   |  /* domain_name_list */
    QMI_SI_TLV_SET(0x2B)   |  /* ip_family */
    QMI_SI_TLV_SET(0x2C)   |  /* im_cn_flag */
    QMI_SI_TLV_SET(0x2D)   |  /* technology_name */
    QMI_SI_TLV_SET(0x2E)      /* pcscf_ipv6_server_addr_list */
};

static const qmi_implemented_optionals wds_get_runtime_settings_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_runtime_settings_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_runtime_settings_reqresp_impl_tuple_v01 = 
{
  &wds_get_runtime_settings_req_msg_impl_v01,
  &wds_get_runtime_settings_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_set_mip_mode_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_mip_mode_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_dormancy_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_get_autoconnect_setting_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* autoconnect_roam_setting */
};

static const qmi_implemented_optionals wds_get_autoconnect_setting_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_autoconnect_setting_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_autoconnect_setting_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_autoconnect_setting_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_call_duration_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* last_call_duration */
    QMI_SI_TLV_SET(0x11)   |  /* call_active_duration */
    QMI_SI_TLV_SET(0x12)      /* last_call_active_duration */
};

static const qmi_implemented_optionals wds_get_call_duration_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_call_duration_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_call_duration_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_call_duration_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_data_bearer_technology_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* last_call_data_bearer_tech */
};

static const qmi_implemented_optionals wds_get_data_bearer_technology_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_data_bearer_technology_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_data_bearer_technology_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_data_bearer_technology_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_dun_call_info_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* report_connection_status */
    QMI_SI_TLV_SET(0x11)   |  /* report_stats */
    QMI_SI_TLV_SET(0x12)   |  /* report_dormancy_status */
    QMI_SI_TLV_SET(0x13)   |  /* report_data_bearer_tech */
    QMI_SI_TLV_SET(0x14)      /* report_channel_rate */
};

static const qmi_implemented_optionals wds_get_dun_call_info_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_dun_call_info_req_msg_impl_array_v01
};
static const uint32_t wds_get_dun_call_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* connection_status */
    QMI_SI_TLV_SET(0x11)   |  /* call_end_reason */
    QMI_SI_TLV_SET(0x12)   |  /* tx_ok_bytes_count */
    QMI_SI_TLV_SET(0x13)   |  /* rx_ok_bytes_count */
    QMI_SI_TLV_SET(0x14)   |  /* dormancy_status */
    QMI_SI_TLV_SET(0x15)   |  /* data_bearer_tech */
    QMI_SI_TLV_SET(0x16)   |  /* channel_rate */
    QMI_SI_TLV_SET(0x17)   |  /* last_call_tx_ok_bytes_count */
    QMI_SI_TLV_SET(0x18)   |  /* last_call_rx_ok_bytes_count */
    QMI_SI_TLV_SET(0x19)   |  /* modem_call_duration_active */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_SET(0x20)      /* last_call_data_bearer_tech */
};

static const qmi_implemented_optionals wds_get_dun_call_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_dun_call_info_resp_msg_impl_array_v01
};
static const uint32_t wds_dun_call_info_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* modem_connection_status */
    QMI_SI_TLV_SET(0x11)   |  /* call_end_reason */
    QMI_SI_TLV_SET(0x12)   |  /* tx_ok_bytes_count */
    QMI_SI_TLV_SET(0x13)   |  /* rx_ok_bytes_count */
    QMI_SI_TLV_SET(0x14)   |  /* dormancy_status */
    QMI_SI_TLV_SET(0x15)   |  /* data_beare_technology */
    QMI_SI_TLV_SET(0x16)      /* channel_rate */
};

static const qmi_implemented_optionals wds_dun_call_info_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_dun_call_info_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_dun_call_info_reqresp_impl_tuple_v01 = 
{
  &wds_get_dun_call_info_req_msg_impl_v01,
  &wds_get_dun_call_info_resp_msg_impl_v01,
  &wds_dun_call_info_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_wds_get_active_mip_profile_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_set_active_mip_profile_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_read_mip_profile_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* profile_state */
    QMI_SI_TLV_SET(0x11)   |  /* home_address */
    QMI_SI_TLV_SET(0x12)   |  /* home_agent_priv */
    QMI_SI_TLV_SET(0x13)   |  /* home_agent_sec */
    QMI_SI_TLV_SET(0x14)   |  /* rev_tun_pref */
    QMI_SI_TLV_SET(0x15)   |  /* nai */
    QMI_SI_TLV_SET(0x16)   |  /* mn_ha_spi */
    QMI_SI_TLV_SET(0x17)   |  /* mn_aaa_spi */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_SET(0x1A)   |  /* mn_ha_key_state */
    QMI_SI_TLV_SET(0x1B)      /* mn_aaa_key_state */
};

static const qmi_implemented_optionals wds_read_mip_profile_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_read_mip_profile_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_read_mip_profile_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_read_mip_profile_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_modify_mip_profile_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* profile_state */
    QMI_SI_TLV_SET(0x11)   |  /* home_address */
    QMI_SI_TLV_SET(0x12)   |  /* home_agent_priv */
    QMI_SI_TLV_SET(0x13)   |  /* home_agent_sec */
    QMI_SI_TLV_SET(0x14)   |  /* rev_tun_pref */
    QMI_SI_TLV_SET(0x15)   |  /* nai */
    QMI_SI_TLV_SET(0x16)   |  /* mn_ha_spi */
    QMI_SI_TLV_SET(0x17)   |  /* mn_aaa_spi */
    QMI_SI_TLV_SET(0x18)   |  /* mn_ha_key */
    QMI_SI_TLV_SET(0x19)      /* mn_aaa_key */
};

static const qmi_implemented_optionals wds_modify_mip_profile_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_modify_mip_profile_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_modify_mip_profile_reqresp_impl_tuple_v01 = 
{
  &wds_modify_mip_profile_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wds_get_mip_settings_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* mip_mode */
    QMI_SI_TLV_SET(0x11)   |  /* mip_reg_retry_count */
    QMI_SI_TLV_SET(0x12)   |  /* mip_reg_retry_interval */
    QMI_SI_TLV_SET(0x13)   |  /* mip_re_reg_peroid */
    QMI_SI_TLV_SET(0x14)   |  /* mip_re_reg_if_traf */
    QMI_SI_TLV_SET(0x15)   |  /* mip_qc_handoff */
    QMI_SI_TLV_SET(0x16)      /* mip_rfc2002bis */
};

static const qmi_implemented_optionals wds_get_mip_settings_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_mip_settings_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_mip_settings_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_mip_settings_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_set_mip_settings_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* mip_mode */
    QMI_SI_TLV_SET(0x11)   |  /* mip_reg_retry_count */
    QMI_SI_TLV_SET(0x12)   |  /* mip_reg_retry_interval */
    QMI_SI_TLV_SET(0x13)   |  /* mip_re_reg_peroid */
    QMI_SI_TLV_SET(0x14)   |  /* mip_re_reg_if_traf */
    QMI_SI_TLV_SET(0x15)   |  /* mip_qc_handoff */
    QMI_SI_TLV_SET(0x16)      /* mip_rfc2002bis */
};

static const qmi_implemented_optionals wds_set_mip_settings_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_set_mip_settings_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_set_mip_settings_reqresp_impl_tuple_v01 = 
{
  &wds_set_mip_settings_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_last_mip_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_get_current_data_bearer_technology_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* last_bearer_tech */
};

static const qmi_implemented_optionals wds_get_current_data_bearer_technology_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_current_data_bearer_technology_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_current_data_bearer_technology_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_current_data_bearer_technology_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_call_history_list_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* full_call_history */
    QMI_SI_TLV_SET(0x11)      /* id_only_call_history */
};

static const qmi_implemented_optionals wds_call_history_list_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_call_history_list_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_call_history_list_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_call_history_list_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_call_history_read_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_call_history_delete_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_call_history_max_size_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_get_default_profile_num_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x14) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x15) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x20) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x22) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x23) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x24) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x25) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x26) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x27) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x28) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x29) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x30) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x31) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x35) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x90) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x91) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x92) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x93) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x94) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x95) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x96) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x97) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x98) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x99) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xB0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_get_default_profile_num_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_get_default_profile_num_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_default_profile_num_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_default_profile_num_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_set_default_profile_num_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x14) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x15) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x20) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x22) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x23) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x24) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x25) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x26) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x27) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x28) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x29) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x30) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x31) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x35) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x90) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x91) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x92) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x93) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x94) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x95) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x96) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x97) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x98) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x99) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xB0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_set_default_profile_num_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_set_default_profile_num_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_set_default_profile_num_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_set_default_profile_num_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_reset_profile_to_default_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x14) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x15) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x20) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x22) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x23) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x24) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x25) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x26) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x27) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x28) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x29) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x30) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x31) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x35) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x90) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x91) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x92) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x93) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x94) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x95) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x96) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x97) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x98) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x99) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xB0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_reset_profile_to_default_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_reset_profile_to_default_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_reset_profile_to_default_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_reset_profile_to_default_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_reset_profile_param_to_invalid_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x14) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x15) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x18) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x1F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x20) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x21) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x22) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x23) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x24) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x25) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x26) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x27) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x28) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x29) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x2F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x30) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x31) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x32) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x33) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x34) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x35) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x36) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x37) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x38) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x39) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x3F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x40) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x41) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x42) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x43) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x44) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x45) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x46) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x47) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x48) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x49) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x4F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x50) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x51) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x52) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x53) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x54) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x55) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x56) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x57) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x58) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x59) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x5F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x60) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x61) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x62) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x63) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x64) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x65) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x66) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x67) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x68) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x69) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x6F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x70) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x71) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x72) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x73) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x74) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x75) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x76) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x77) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x78) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x79) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x7F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x80) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x81) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x82) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x83) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x84) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x85) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x86) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x87) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x88) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x89) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x8F)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0x90) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x91) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x92) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x93) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x94) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x95) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x96) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x97) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x98) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x99) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9A) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9B) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9C) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9D) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9E) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x9F) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xA9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xAF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xB0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xB9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xBF) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xC9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xCF)    /* Undefined */
  ,

    QMI_SI_TLV_CLEAR(0xD0) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD1) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD2) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD3) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD4) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD5) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD6) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD7) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD8) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xD9) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDA) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDB) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDC) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDD) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDE) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0xDF) |  /* Undefined */
    QMI_SI_TLV_SET(0xE0)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_reset_profile_param_to_invalid_resp_msg_impl_v01 =
{
  7, /* number of elements in array */
  wds_reset_profile_param_to_invalid_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_reset_profile_param_to_invalid_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_reset_profile_param_to_invalid_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_set_client_ip_family_pref_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_fmc_set_tunnel_params_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ipv4_sock_addr */
    QMI_SI_TLV_SET(0x11)      /* ipv6_sock_addr */
};

static const qmi_implemented_optionals wds_fmc_set_tunnel_params_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_fmc_set_tunnel_params_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_fmc_set_tunnel_params_reqresp_impl_tuple_v01 = 
{
  &wds_fmc_set_tunnel_params_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_fmc_clear_tunnel_params_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_fmc_get_tunnel_params_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tunnel_params */
    QMI_SI_TLV_SET(0x11)   |  /* ipv4_sock_addr */
    QMI_SI_TLV_SET(0x12)      /* ipv6_sock_addr */
};

static const qmi_implemented_optionals wds_fmc_get_tunnel_params_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_fmc_get_tunnel_params_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_fmc_get_tunnel_params_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_fmc_get_tunnel_params_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_set_autoconnect_settings_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* autoconnect_roam_setting */
};

static const qmi_implemented_optionals wds_set_autoconnect_settings_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_set_autoconnect_settings_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_set_autoconnect_settings_reqresp_impl_tuple_v01 = 
{
  &wds_set_autoconnect_settings_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wds_get_dns_settings_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* primary_dns_ipv4_address */
    QMI_SI_TLV_SET(0x11)   |  /* secondary_dns_ipv4_address */
    QMI_SI_TLV_SET(0x12)   |  /* primary_dns_ipv6_address */
    QMI_SI_TLV_SET(0x13)      /* secondary_dns_ipv6_address */
};

static const qmi_implemented_optionals wds_get_dns_settings_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_dns_settings_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_dns_settings_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_dns_settings_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_set_dns_settings_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* primary_dns_ipv4_address */
    QMI_SI_TLV_SET(0x11)   |  /* secondary_dns_ipv4_address */
    QMI_SI_TLV_SET(0x12)   |  /* primary_dns_ipv6_address */
    QMI_SI_TLV_SET(0x13)      /* secondary_dns_ipv6_address */
};

static const qmi_implemented_optionals wds_set_dns_settings_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_set_dns_settings_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_set_dns_settings_reqresp_impl_tuple_v01 = 
{
  &wds_set_dns_settings_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_pre_dormancy_cdma_settings_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_set_cam_timer_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_cam_timer_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_set_scrm_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_scrm_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_set_rdud_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_rdud_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_sip_mip_call_type_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_set_evdo_page_monitor_period_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_set_evdo_force_long_sleep_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_evdo_page_monitor_period_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_call_throttle_info_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_get_nsapi_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_set_dun_ctrl_pref_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* dun_allow_preference */
};

static const qmi_implemented_optionals wds_set_dun_ctrl_pref_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_set_dun_ctrl_pref_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_set_dun_ctrl_pref_reqresp_impl_tuple_v01 = 
{
  &wds_set_dun_ctrl_pref_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wds_get_dun_ctrl_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* allow_preference */
    QMI_SI_TLV_SET(0x11)   |  /* current_control_point */
    QMI_SI_TLV_SET(0x12)      /* event_report_mask */
};

static const qmi_implemented_optionals wds_get_dun_ctrl_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_dun_ctrl_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_dun_ctrl_info_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_dun_ctrl_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_set_dun_ctrl_event_report_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* notify_entitlement */
    QMI_SI_TLV_SET(0x11)      /* notify_silent_redial */
};

static const qmi_implemented_optionals wds_set_dun_ctrl_event_report_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_set_dun_ctrl_event_report_req_msg_impl_array_v01
};
static const uint32_t wds_dun_ctrl_event_report_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* dun_call_notification */
    QMI_SI_TLV_SET(0x11)   |  /* dun_call_id */
    QMI_SI_TLV_SET(0x12)      /* previous_dun_failure_reason */
};

static const qmi_implemented_optionals wds_dun_ctrl_event_report_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_dun_ctrl_event_report_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_set_dun_ctrl_event_report_reqresp_impl_tuple_v01 = 
{
  &wds_set_dun_ctrl_event_report_req_msg_impl_v01,
  NULL,
  &wds_dun_ctrl_event_report_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_wds_control_pending_dun_call_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_embms_tmgi_activate_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tranx_id */
    QMI_SI_TLV_SET(0x11)   |  /* preempt_priority */
    QMI_SI_TLV_SET(0x12)      /* earfcn_list */
};

static const qmi_implemented_optionals wds_embms_tmgi_activate_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_activate_req_msg_impl_array_v01
};
static const uint32_t wds_embms_tmgi_activate_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_embms_tmgi_activate_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_activate_resp_msg_impl_array_v01
};
static const uint32_t wds_embms_tmgi_activate_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* tranx_id */
};

static const qmi_implemented_optionals wds_embms_tmgi_activate_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_activate_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_embms_tmgi_activate_reqresp_impl_tuple_v01 = 
{
  &wds_embms_tmgi_activate_req_msg_impl_v01,
  &wds_embms_tmgi_activate_resp_msg_impl_v01,
  &wds_embms_tmgi_activate_ind_msg_impl_v01
};

static const uint32_t wds_embms_tmgi_deactivate_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* tranx_id */
};

static const qmi_implemented_optionals wds_embms_tmgi_deactivate_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_deactivate_req_msg_impl_array_v01
};
static const uint32_t wds_embms_tmgi_deactivate_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_embms_tmgi_deactivate_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_deactivate_resp_msg_impl_array_v01
};
static const uint32_t wds_embms_tmgi_deactivate_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* tranx_id */
};

static const qmi_implemented_optionals wds_embms_tmgi_deactivate_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_deactivate_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_embms_tmgi_deactivate_reqresp_impl_tuple_v01 = 
{
  &wds_embms_tmgi_deactivate_req_msg_impl_v01,
  &wds_embms_tmgi_deactivate_resp_msg_impl_v01,
  &wds_embms_tmgi_deactivate_ind_msg_impl_v01
};

static const uint32_t wds_embms_tmgi_list_query_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* tranx_id */
};

static const qmi_implemented_optionals wds_embms_tmgi_list_query_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_list_query_req_msg_impl_array_v01
};
static const uint32_t wds_embms_tmgi_list_query_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tmgi_list */
    QMI_SI_TLV_SET(0x11)      /* warn_reason */
};

static const qmi_implemented_optionals wds_embms_tmgi_list_query_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_list_query_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_embms_tmgi_list_query_reqresp_impl_tuple_v01 = 
{
  &wds_embms_tmgi_list_query_req_msg_impl_v01,
  &wds_embms_tmgi_list_query_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_embms_tmgi_list_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tmgi_list */
    QMI_SI_TLV_SET(0x11)   |  /* warn_reason */
    QMI_SI_TLV_SET(0x12)      /* tranx_id */
};

static const qmi_implemented_optionals wds_embms_tmgi_list_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_list_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_embms_tmgi_list_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &wds_embms_tmgi_list_ind_msg_impl_v01
};

static const uint32_t wds_get_preferred_data_system_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* current_sys */
};

static const qmi_implemented_optionals wds_get_preferred_data_system_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_preferred_data_system_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_preferred_data_system_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_preferred_data_system_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_last_data_call_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* data_call_status */
    QMI_SI_TLV_SET(0x11)      /* data_call_type */
};

static const qmi_implemented_optionals wds_get_last_data_call_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_last_data_call_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_last_data_call_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_last_data_call_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_current_data_system_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* system_status */
};

static const qmi_implemented_optionals wds_get_current_data_system_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_current_data_system_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_current_system_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_current_data_system_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_pdn_throttle_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* throttle_info */
};

static const qmi_implemented_optionals wds_get_pdn_throttle_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_pdn_throttle_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_pdn_throttle_info_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_pdn_throttle_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_lte_attach_params_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* apn_string */
    QMI_SI_TLV_SET(0x11)   |  /* ip_type */
    QMI_SI_TLV_SET(0x12)      /* ota_attach_performed */
};

static const qmi_implemented_optionals wds_get_lte_attach_params_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_lte_attach_params_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_lte_attach_params_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_lte_attach_params_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_wds_reset_pkt_statistics_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wds_get_flow_control_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* uplink_flow_control */
};

static const qmi_implemented_optionals wds_get_flow_control_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_flow_control_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_flow_control_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_flow_control_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_embms_tmgi_act_deact_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tranx_id */
    QMI_SI_TLV_SET(0x11)   |  /* preempt_priority */
    QMI_SI_TLV_SET(0x12)      /* earfcn_list */
};

static const qmi_implemented_optionals wds_embms_tmgi_act_deact_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_act_deact_req_msg_impl_array_v01
};
static const uint32_t wds_embms_tmgi_act_deact_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* extended_error_code */
};

static const qmi_implemented_optionals wds_embms_tmgi_act_deact_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_act_deact_resp_msg_impl_array_v01
};
static const uint32_t wds_embms_tmgi_act_deact_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* tranx_id */
};

static const qmi_implemented_optionals wds_embms_tmgi_act_deact_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_embms_tmgi_act_deact_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_embms_tmgi_act_deact_reqresp_impl_tuple_v01 = 
{
  &wds_embms_tmgi_act_deact_req_msg_impl_v01,
  &wds_embms_tmgi_act_deact_resp_msg_impl_v01,
  &wds_embms_tmgi_act_deact_ind_msg_impl_v01
};

static const qmi_implemented_optionals wds_bind_data_port_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals wds_bind_data_port_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_bind_data_port_reqresp_impl_tuple_v01 = 
{
  &wds_bind_data_port_req_msg_impl_v01,
  &wds_bind_data_port_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_set_additional_pdn_filter_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* next_hdr_prot */
    QMI_SI_TLV_SET(0x11)      /* tcp_udp_src */
};

static const qmi_implemented_optionals wds_set_additional_pdn_filter_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_set_additional_pdn_filter_req_msg_impl_array_v01
};
static const uint32_t wds_set_additional_pdn_filter_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* filter_handle */
};

static const qmi_implemented_optionals wds_set_additional_pdn_filter_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_set_additional_pdn_filter_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_set_additional_pdn_filter_reqresp_impl_tuple_v01 = 
{
  &wds_set_additional_pdn_filter_req_msg_impl_v01,
  &wds_set_additional_pdn_filter_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals wds_remove_additional_pdn_filter_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals wds_remove_additional_pdn_filter_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_remove_additional_pdn_filter_reqresp_impl_tuple_v01 = 
{
  &wds_remove_additional_pdn_filter_req_msg_impl_v01,
  &wds_remove_additional_pdn_filter_resp_msg_impl_v01,
  NULL
};
static const uint32_t wds_extended_ip_config_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* changed_ip_config */
};

static const qmi_implemented_optionals wds_extended_ip_config_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_extended_ip_config_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_extended_ip_config_ind_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &wds_extended_ip_config_ind_msg_impl_v01
};

static const qmi_implemented_optionals wds_reverse_ip_transport_connection_ind_registration_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals wds_reverse_ip_transport_connection_ind_registration_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_reverse_ip_transport_connection_registration_reqresp_impl_tuple_v01 = 
{
  &wds_reverse_ip_transport_connection_ind_registration_req_msg_impl_v01,
  &wds_reverse_ip_transport_connection_ind_registration_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_reverse_ip_transport_connection_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tech_name */
    QMI_SI_TLV_SET(0x11)   |  /* is_shared */
    QMI_SI_TLV_SET(0x12)   |  /* ipv4_addr */
    QMI_SI_TLV_SET(0x13)   |  /* ipv4_subnet_mask */
    QMI_SI_TLV_SET(0x14)      /* ipv6_addr */
};

static const qmi_implemented_optionals wds_reverse_ip_transport_connection_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_reverse_ip_transport_connection_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_reverse_ip_transport_connection_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &wds_reverse_ip_transport_connection_ind_msg_impl_v01
};

static const uint32_t wds_get_ipsec_static_sa_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* spi_rx */
    QMI_SI_TLV_SET(0x11)   |  /* spi_tx */
    QMI_SI_TLV_SET(0x12)   |  /* ipsec_sa_protocol */
    QMI_SI_TLV_SET(0x13)   |  /* encapsulation_mode */
    QMI_SI_TLV_SET(0x14)   |  /* dest_addr */
    QMI_SI_TLV_SET(0x15)   |  /* local_addr */
    QMI_SI_TLV_SET(0x16)   |  /* hash_algo */
    QMI_SI_TLV_SET(0x17)   |  /* hash_key_rx */
    QMI_SI_TLV_SET(0x18)   |  /* hash_key_tx */
    QMI_SI_TLV_SET(0x19)   |  /* crypto_algo */
    QMI_SI_TLV_SET(0x1A)   |  /* crypto_key_rx */
    QMI_SI_TLV_SET(0x1B)   |  /* crypto_key_tx */
    QMI_SI_TLV_SET(0x1C)   |  /* iv */
    QMI_SI_TLV_SET(0x1D)   |  /* is_udp_encaps */
    QMI_SI_TLV_SET(0x1E)   |  /* nat_local_ip_addr */
    QMI_SI_TLV_SET(0x1F)   |  /* nat_remote_ip_addr */
    QMI_SI_TLV_SET(0x20)   |  /* cfg_attr_internal_ipv4_address */
    QMI_SI_TLV_SET(0x21)   |  /* cfg_attr_internal_ipv4_netmask */
    QMI_SI_TLV_SET(0x22)   |  /* cfg_attr_internal_ipv4_dns */
    QMI_SI_TLV_SET(0x23)   |  /* cfg_attr_internal_ipv4_nbns */
    QMI_SI_TLV_SET(0x24)   |  /* cfg_attr_internal_address_expiry */
    QMI_SI_TLV_SET(0x25)   |  /* cfg_attr_internal_ipv4_dhcp */
    QMI_SI_TLV_SET(0x26)   |  /* cfg_attr_application_version */
    QMI_SI_TLV_SET(0x27)   |  /* cfg_attr_internal_ipv6_address */
    QMI_SI_TLV_SET(0x28)   |  /* cfg_attr_internal_ipv6_dns */
    QMI_SI_TLV_SET(0x29)   |  /* cfg_attr_internal_ipv6_nbns */
    QMI_SI_TLV_SET(0x2A)   |  /* cfg_attr_internal_ipv6_dhcp */
    QMI_SI_TLV_SET(0x2B)   |  /* cfg_attr_internal_ipv4_subnet */
    QMI_SI_TLV_SET(0x2C)   |  /* cfg_attr_supported_attributes */
    QMI_SI_TLV_SET(0x2D)   |  /* cfg_attr_internal_ipv6_subnet */
    QMI_SI_TLV_SET(0x2E)   |  /* cfg_attr_internal_pcscf_ipv4_address */
    QMI_SI_TLV_SET(0x2F)      /* cfg_attr_internal_pcscf_ipv6_address */
  ,

    QMI_SI_TLV_SET(0x30)   |  /* cfg_attr_3gpp2_mip4_ha */
    QMI_SI_TLV_SET(0x31)   |  /* cfg_attr_3gpp2_mip4_hoa */
    QMI_SI_TLV_SET(0x32)   |  /* cfg_attr_3gpp2_mip6_ha */
    QMI_SI_TLV_SET(0x33)   |  /* cfg_attr_3gpp2_mip6_hoa */
    QMI_SI_TLV_SET(0x34)      /* traffic_selector_list */
};

static const qmi_implemented_optionals wds_get_ipsec_static_sa_config_resp_msg_impl_v01 =
{
  2, /* number of elements in array */
  wds_get_ipsec_static_sa_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_ipsec_static_sa_config_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_ipsec_static_sa_config_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals wds_reverse_ip_transport_config_complete_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals wds_reverse_ip_transport_config_complete_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_reverse_ip_transport_config_complete_reqresp_impl_tuple_v01 = 
{
  &wds_reverse_ip_transport_config_complete_req_msg_impl_v01,
  &wds_reverse_ip_transport_config_complete_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_data_bearer_technology_ex_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* bearer_tech */
    QMI_SI_TLV_SET(0x11)      /* last_bearer_tech */
};

static const qmi_implemented_optionals wds_get_data_bearer_technology_ex_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_data_bearer_technology_ex_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_data_bearer_technology_ex_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_data_bearer_technology_ex_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_lte_max_attach_pdn_num_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* max_attach_pdn_num */
};

static const qmi_implemented_optionals wds_get_lte_max_attach_pdn_num_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_lte_max_attach_pdn_num_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_lte_max_attach_pdn_num_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_lte_max_attach_pdn_num_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals wds_set_lte_attach_pdn_list_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals wds_set_lte_attach_pdn_list_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_set_lte_attach_pdn_list_reqresp_impl_tuple_v01 = 
{
  &wds_set_lte_attach_pdn_list_req_msg_impl_v01,
  &wds_set_lte_attach_pdn_list_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_lte_attach_pdn_list_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* attach_pdn_list */
};

static const qmi_implemented_optionals wds_get_lte_attach_pdn_list_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_lte_attach_pdn_list_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_lte_attach_pdn_list_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_lte_attach_pdn_list_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_lte_attach_pdn_list_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* attach_pdn_list */
};

static const qmi_implemented_optionals wds_lte_attach_pdn_list_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_lte_attach_pdn_list_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_lte_attach_pdn_list_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &wds_lte_attach_pdn_list_ind_msg_impl_v01
};

static const qmi_implemented_optionals wds_set_lte_data_retry_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals wds_set_lte_data_retry_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_set_lte_data_retry_reqresp_impl_tuple_v01 = 
{
  &wds_set_lte_data_retry_req_msg_impl_v01,
  &wds_set_lte_data_retry_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_lte_data_retry_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* lte_data_retry */
};

static const qmi_implemented_optionals wds_get_lte_data_retry_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_lte_data_retry_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_lte_data_retry_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_lte_data_retry_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals wds_set_lte_attach_type_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals wds_set_lte_attach_type_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_set_lte_attach_type_reqresp_impl_tuple_v01 = 
{
  &wds_set_lte_attach_type_req_msg_impl_v01,
  &wds_set_lte_attach_type_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_lte_attach_type_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* lte_attach_type */
};

static const qmi_implemented_optionals wds_get_lte_attach_type_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_lte_attach_type_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_lte_attach_type_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_lte_attach_type_resp_msg_impl_v01,
  NULL
};
static const qmi_implemented_optionals wds_handoff_information_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_handoff_information_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &wds_handoff_information_ind_msg_impl_v01
};

static const qmi_implemented_optionals wds_update_lte_attach_pdn_list_profiles_type_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals wds_update_lte_attach_pdn_list_profiles_type_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_update_lte_attach_pdn_list_profiles_type_reqresp_impl_tuple_v01 = 
{
  &wds_update_lte_attach_pdn_list_profiles_type_req_msg_impl_v01,
  &wds_update_lte_attach_pdn_list_profiles_type_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qmi_wds_set_data_path_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qmi_wds_set_data_path_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_set_data_path_reqresp_impl_tuple_v01 = 
{
  &qmi_wds_set_data_path_req_msg_impl_v01,
  &qmi_wds_set_data_path_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_data_path_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)  /* data_path */
};

static const qmi_implemented_optionals wds_get_data_path_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_data_path_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_data_path_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_data_path_resp_msg_impl_v01,
  NULL
};
static const qmi_implemented_optionals wds_initiate_esp_rekey_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_initiate_esp_rekey_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_initiate_esp_rekey_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_set_throughput_info_ind_freq_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* report_interval */
};

static const qmi_implemented_optionals wds_set_throughput_info_ind_freq_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_set_throughput_info_ind_freq_req_msg_impl_array_v01
};
static const qmi_implemented_optionals wds_set_throughput_info_ind_freq_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wds_set_throughput_info_freq_reqresp_impl_tuple_v01 = 
{
  &wds_set_throughput_info_ind_freq_req_msg_impl_v01,
  &wds_set_throughput_info_ind_freq_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_get_last_throughput_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* throughput_info */
};

static const qmi_implemented_optionals wds_get_last_throughput_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_get_last_throughput_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_get_last_throughput_info_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wds_get_last_throughput_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t wds_throughput_info_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* throughput_info */
};

static const qmi_implemented_optionals wds_throughput_info_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wds_throughput_info_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wds_throughput_info_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &wds_throughput_info_ind_msg_impl_v01
};
static const qmi_implemented_optionals_tuple * const wds_service_implemented_array_v01[] =
{
    &qmi_wds_reset_reqresp_impl_tuple_v01,               /* 0x0000 */
    &qmi_wds_set_event_report_reqresp_impl_tuple_v01,    /* 0x0001 */
    &qmi_wds_abort_reqresp_impl_tuple_v01,               /* 0x0002 */
    &qmi_wdsication_register_reqresp_impl_tuple_v01,     /* 0x0003 */
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
    NULL,                                                /* 0x001E */
    NULL,                                                /* 0x001F */
    &qmi_wds_start_network_interface_reqresp_impl_tuple_v01, /* 0x0020 */
    &qmi_wds_stop_network_interface_reqresp_impl_tuple_v01, /* 0x0021 */
    &qmi_wds_get_pkt_srvc_status_reqresp_impl_tuple_v01, /* 0x0022 */
    &qmi_wds_get_current_channel_rate_reqresp_impl_tuple_v01, /* 0x0023 */
    &qmi_wds_get_pkt_statistics_reqresp_impl_tuple_v01,  /* 0x0024 */
    &qmi_wds_go_dormant_reqresp_impl_tuple_v01,          /* 0x0025 */
    &qmi_wds_go_active_reqresp_impl_tuple_v01,           /* 0x0026 */
    &qmi_wds_create_profile_reqresp_impl_tuple_v01,      /* 0x0027 */
    &qmi_wds_modify_profile_settings_reqresp_impl_tuple_v01, /* 0x0028 */
    &qmi_wds_delete_profile_reqresp_impl_tuple_v01,      /* 0x0029 */
    &qmi_wds_get_profile_list_reqresp_impl_tuple_v01,    /* 0x002A */
    &qmi_wds_get_profile_settings_reqresp_impl_tuple_v01, /* 0x002B */
    &qmi_wds_get_default_settings_reqresp_impl_tuple_v01, /* 0x002C */
    &qmi_wds_get_runtime_settings_reqresp_impl_tuple_v01, /* 0x002D */
    &qmi_wds_set_mip_mode_reqresp_impl_tuple_v01,        /* 0x002E */
    &qmi_wds_get_mip_mode_reqresp_impl_tuple_v01,        /* 0x002F */
    &qmi_wds_get_dormancy_status_reqresp_impl_tuple_v01, /* 0x0030 */
    NULL,                                                /* 0x0031 */
    NULL,                                                /* 0x0032 */
    NULL,                                                /* 0x0033 */
    &qmi_wds_get_autoconnect_setting_reqresp_impl_tuple_v01, /* 0x0034 */
    &qmi_wds_get_call_duration_reqresp_impl_tuple_v01,   /* 0x0035 */
    NULL,                                                /* 0x0036 */
    &qmi_wds_get_data_bearer_technology_reqresp_impl_tuple_v01, /* 0x0037 */
    &qmi_wds_get_dun_call_info_reqresp_impl_tuple_v01,   /* 0x0038 */
    NULL,                                                /* 0x0039 */
    NULL,                                                /* 0x003A */
    NULL,                                                /* 0x003B */
    &qmi_wds_get_active_mip_profile_reqresp_impl_tuple_v01, /* 0x003C */
    &qmi_wds_set_active_mip_profile_reqresp_impl_tuple_v01, /* 0x003D */
    &qmi_wds_read_mip_profile_reqresp_impl_tuple_v01,    /* 0x003E */
    &qmi_wds_modify_mip_profile_reqresp_impl_tuple_v01,  /* 0x003F */
    &qmi_wds_get_mip_settings_reqresp_impl_tuple_v01,    /* 0x0040 */
    &qmi_wds_set_mip_settings_reqresp_impl_tuple_v01,    /* 0x0041 */
    &qmi_wds_get_last_mip_status_reqresp_impl_tuple_v01, /* 0x0042 */
    NULL,                                                /* 0x0043 */
    &qmi_wds_get_current_data_bearer_technology_reqresp_impl_tuple_v01, /* 0x0044 */
    &qmi_wds_call_history_list_reqresp_impl_tuple_v01,   /* 0x0045 */
    &qmi_wds_call_history_read_reqresp_impl_tuple_v01,   /* 0x0046 */
    &qmi_wds_call_history_delete_reqresp_impl_tuple_v01, /* 0x0047 */
    &qmi_wds_call_history_max_size_reqresp_impl_tuple_v01, /* 0x0048 */
    &qmi_wds_get_default_profile_num_reqresp_impl_tuple_v01, /* 0x0049 */
    &qmi_wds_set_default_profile_num_reqresp_impl_tuple_v01, /* 0x004A */
    &qmi_wds_reset_profile_to_default_reqresp_impl_tuple_v01, /* 0x004B */
    &qmi_wds_reset_profile_param_to_invalid_reqresp_impl_tuple_v01, /* 0x004C */
    &qmi_wds_set_client_ip_family_pref_reqresp_impl_tuple_v01, /* 0x004D */
    &qmi_wds_fmc_set_tunnel_params_reqresp_impl_tuple_v01, /* 0x004E */
    &qmi_wds_fmc_clear_tunnel_params_reqresp_impl_tuple_v01, /* 0x004F */
    &qmi_wds_fmc_get_tunnel_params_reqresp_impl_tuple_v01, /* 0x0050 */
    &qmi_wds_set_autoconnect_settings_reqresp_impl_tuple_v01, /* 0x0051 */
    &qmi_wds_get_dns_settings_reqresp_impl_tuple_v01,    /* 0x0052 */
    &qmi_wds_set_dns_settings_reqresp_impl_tuple_v01,    /* 0x0053 */
    &qmi_wds_get_pre_dormancy_cdma_settings_reqresp_impl_tuple_v01, /* 0x0054 */
    &qmi_wds_set_cam_timer_reqresp_impl_tuple_v01,       /* 0x0055 */
    &qmi_wds_get_cam_timer_reqresp_impl_tuple_v01,       /* 0x0056 */
    &qmi_wds_set_scrm_reqresp_impl_tuple_v01,            /* 0x0057 */
    &qmi_wds_get_scrm_reqresp_impl_tuple_v01,            /* 0x0058 */
    &qmi_wds_set_rdud_reqresp_impl_tuple_v01,            /* 0x0059 */
    &qmi_wds_get_rdud_reqresp_impl_tuple_v01,            /* 0x005A */
    &qmi_wds_get_sip_mip_call_type_reqresp_impl_tuple_v01, /* 0x005B */
    &qmi_wds_set_evdo_page_monitor_period_reqresp_impl_tuple_v01, /* 0x005C */
    &qmi_wds_set_evdo_force_long_sleep_reqresp_impl_tuple_v01, /* 0x005D */
    &qmi_wds_get_evdo_page_monitor_period_reqresp_impl_tuple_v01, /* 0x005E */
    &qmi_wds_get_call_throttle_info_reqresp_impl_tuple_v01, /* 0x005F */
    &qmi_wds_get_nsapi_reqresp_impl_tuple_v01,           /* 0x0060 */
    &qmi_wds_set_dun_ctrl_pref_reqresp_impl_tuple_v01,   /* 0x0061 */
    &qmi_wds_get_dun_ctrl_info_reqresp_impl_tuple_v01,   /* 0x0062 */
    &qmi_wds_set_dun_ctrl_event_report_reqresp_impl_tuple_v01, /* 0x0063 */
    &qmi_wds_control_pending_dun_call_reqresp_impl_tuple_v01, /* 0x0064 */
    &qmi_wds_embms_tmgi_activate_reqresp_impl_tuple_v01, /* 0x0065 */
    &qmi_wds_embms_tmgi_deactivate_reqresp_impl_tuple_v01, /* 0x0066 */
    &qmi_wds_embms_tmgi_list_query_reqresp_impl_tuple_v01, /* 0x0067 */
    &qmi_wds_embms_tmgi_list_impl_tuple_v01,             /* 0x0068 */
    &qmi_wds_get_preferred_data_system_reqresp_impl_tuple_v01, /* 0x0069 */
    &qmi_wds_get_last_data_call_status_reqresp_impl_tuple_v01, /* 0x006A */
    &qmi_wds_get_current_system_status_reqresp_impl_tuple_v01, /* 0x006B */
    &qmi_wds_get_pdn_throttle_info_reqresp_impl_tuple_v01, /* 0x006C */
    NULL,                                                /* 0x006D */
    NULL,                                                /* 0x006E */
    NULL,                                                /* 0x006F */
    NULL,                                                /* 0x0070 */
    NULL,                                                /* 0x0071 */
    NULL,                                                /* 0x0072 */
    NULL,                                                /* 0x0073 */
    NULL,                                                /* 0x0074 */
    NULL,                                                /* 0x0075 */
    NULL,                                                /* 0x0076 */
    NULL,                                                /* 0x0077 */
    NULL,                                                /* 0x0078 */
    NULL,                                                /* 0x0079 */
    NULL,                                                /* 0x007A */
    NULL,                                                /* 0x007B */
    NULL,                                                /* 0x007C */
    NULL,                                                /* 0x007D */
    NULL,                                                /* 0x007E */
    NULL,                                                /* 0x007F */
    NULL,                                                /* 0x0080 */
    NULL,                                                /* 0x0081 */
    NULL,                                                /* 0x0082 */
    NULL,                                                /* 0x0083 */
    NULL,                                                /* 0x0084 */
    &qmi_wds_get_lte_attach_params_reqresp_impl_tuple_v01, /* 0x0085 */
    &qmi_wds_reset_pkt_statistics_reqresp_impl_tuple_v01, /* 0x0086 */
    &qmi_wds_get_flow_control_status_reqresp_impl_tuple_v01, /* 0x0087 */
    &qmi_wds_embms_tmgi_act_deact_reqresp_impl_tuple_v01, /* 0x0088 */
    &qmi_wds_bind_data_port_reqresp_impl_tuple_v01,      /* 0x0089 */
    &qmi_wds_set_additional_pdn_filter_reqresp_impl_tuple_v01, /* 0x008A */
    &qmi_wds_remove_additional_pdn_filter_reqresp_impl_tuple_v01, /* 0x008B */
    &qmi_wds_extended_ip_config_ind_reqresp_impl_tuple_v01,  /* 0x008C */
#ifdef FEATURE_DATA_WLAN_MAPCON
    &qmi_wds_reverse_ip_transport_connection_registration_reqresp_impl_tuple_v01, /* 0x008D */
#else
    NULL,
#endif /* FEATURE_DATA_WLAN_MAPCON */
#ifdef FEATURE_DATA_WLAN_MAPCON
    &qmi_wds_reverse_ip_transport_connection_impl_tuple_v01, /* 0x008E */
#else
    NULL,
#endif /* FEATURE_DATA_WLAN_MAPCON */
#ifdef FEATURE_DATA_WLAN_MAPCON
    &qmi_wds_get_ipsec_static_sa_config_reqresp_impl_tuple_v01, /* 0x008F */
#else
    NULL,
#endif /* FEATURE_DATA_WLAN_MAPCON */
#ifdef FEATURE_DATA_WLAN_MAPCON
    &qmi_wds_reverse_ip_transport_config_complete_reqresp_impl_tuple_v01, /* 0x0090 */
#else
    NULL,
#endif /* FEATURE_DATA_WLAN_MAPCON */
#ifdef FEATURE_DATA_WLAN_MAPCON
    &qmi_wds_get_data_bearer_technology_ex_reqresp_impl_tuple_v01,  /* 0x0091 */
#else
    NULL,
#endif /* FEATURE_DATA_WLAN_MAPCON */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_get_lte_max_attach_pdn_num_reqresp_impl_tuple_v01, /* 0x0092 */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_set_lte_attach_pdn_list_reqresp_impl_tuple_v01, /* 0x0093 */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_get_lte_attach_pdn_list_reqresp_impl_tuple_v01, /* 0x0094 */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_lte_attach_pdn_list_impl_tuple_v01,         /* 0x0095 */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_set_lte_data_retry_reqresp_impl_tuple_v01,  /* 0x0096 */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_get_lte_data_retry_reqresp_impl_tuple_v01,  /* 0x0097 */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_set_lte_attach_type_reqresp_impl_tuple_v01, /* 0x0098 */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_get_lte_attach_type_reqresp_impl_tuple_v01,  /* 0x0099 */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
    NULL,                                                                         /* 0x009A */
#ifdef FEATURE_EPC_HANDOFF
    &qmi_wds_handoff_information_impl_tuple_v01,        /* 0x009B */
#else
    NULL,
#endif /* FEATURE_EPC_HANDOFF */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_set_data_path_reqresp_impl_tuple_v01,   /* 0x009C */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_get_data_path_reqresp_impl_tuple_v01,  /* 0x009D */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
    NULL,                                          /* 0x009E */
#ifdef FEATURE_DATA_LTE
    &qmi_wds_update_lte_attach_pdn_list_profiles_type_reqresp_impl_tuple_v01,   /* 0x009F */
#else
    NULL,
#endif /* FEATURE_DATA_LTE */
    NULL, /* 0x00A0 */
    NULL, /* 0x00A1 */
    NULL, /* 0x00A2 */
    &qmi_wds_set_throughput_info_freq_reqresp_impl_tuple_v01, /* 0x00A3 */
    &qmi_wds_get_last_throughput_info_reqresp_impl_tuple_v01, /* 0x00A4 */
    NULL, /* 0x00A5 */
#ifdef FEATURE_DATA_WLAN_MAPCON
    &qmi_wds_initiate_esp_rekey_reqresp_impl_tuple_v01  /* 0x00A6 */
#else
    NULL
#endif /* FEATURE_DATA_WLAN_MAPCON */
};

static const qmi_implemented_messages wds_service_implemented_v01 =
{
  0x00A6,  /* maximum message ID in this service */
  wds_service_implemented_array_v01
};

const qmi_implemented_messages *wds_get_service_impl_v01 (void)
{
  return &wds_service_implemented_v01;
}
