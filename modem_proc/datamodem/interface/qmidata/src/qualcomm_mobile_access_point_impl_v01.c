/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Q U A L C O M M _ M O B I L E _ A C C E S S _ P O I N T _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the qcmap service Data structures.

  Copyright (c) 2012 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/qualcomm_mobile_access_point_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.1
   It was generated on: Thu Feb 21 2013 (Spin 0)
   From IDL File: qualcomm_mobile_access_point_v01.idl */

#include "qmi_si.h"
#include "qualcomm_mobile_access_point_v01.h"

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_supported_msgs_reqresp_impl_tuple_v01 = 
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
static const qmi_implemented_optionals_tuple qmi_qcmap_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const uint32_t qcmap_mobile_ap_enable_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ip_addr_info */
    QMI_SI_TLV_SET(0x11)   |  /* net_policy_info */
    QMI_SI_TLV_SET(0x12)   |  /* ssid2_ip_addr_info */
    QMI_SI_TLV_SET(0x13)      /* qcmap_nat_type_info */
};

static const qmi_implemented_optionals qcmap_mobile_ap_enable_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_mobile_ap_enable_req_msg_impl_array_v01
};
static const uint32_t qcmap_mobile_ap_enable_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* mobile_ap_handle */
};

static const qmi_implemented_optionals qcmap_mobile_ap_enable_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_mobile_ap_enable_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_mobile_ap_enable_reqresp_impl_tuple_v01 = 
{
  &qcmap_mobile_ap_enable_req_msg_impl_v01,
  &qcmap_mobile_ap_enable_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_mobile_ap_disable_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_mobile_ap_disable_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_mobile_ap_disable_reqresp_impl_tuple_v01 = 
{
  &qcmap_mobile_ap_disable_req_msg_impl_v01,
  &qcmap_mobile_ap_disable_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_bring_up_wwan_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_bring_up_wwan_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_bring_up_wwan_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_bring_up_wwan_reqresp_impl_tuple_v01 = 
{
  &qcmap_bring_up_wwan_req_msg_impl_v01,
  &qcmap_bring_up_wwan_resp_msg_impl_v01,
  &qcmap_bring_up_wwan_ind_msg_impl_v01
};

static const qmi_implemented_optionals qcmap_tear_down_wwan_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_tear_down_wwan_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_tear_down_wwan_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_tear_down_wwan_reqresp_impl_tuple_v01 = 
{
  &qcmap_tear_down_wwan_req_msg_impl_v01,
  &qcmap_tear_down_wwan_resp_msg_impl_v01,
  &qcmap_tear_down_wwan_ind_msg_impl_v01
};

static const qmi_implemented_optionals qcmap_get_wwan_status_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_wwan_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_end_reason */
    QMI_SI_TLV_SET(0x11)   |  /* verbose_call_end_reason */
    QMI_SI_TLV_SET(0x12)      /* wwan_status */
};

static const qmi_implemented_optionals qcmap_get_wwan_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_wwan_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_wwan_status_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_wwan_status_req_msg_impl_v01,
  &qcmap_get_wwan_status_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_ipsec_vpn_pass_through_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_ipsec_vpn_pass_through_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* vpn_pass_through_value */
};

static const qmi_implemented_optionals qcmap_get_ipsec_vpn_pass_through_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_ipsec_vpn_pass_through_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_ipsec_vpn_pass_through_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_ipsec_vpn_pass_through_req_msg_impl_v01,
  &qcmap_get_ipsec_vpn_pass_through_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_set_ipsec_vpn_pass_through_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_set_ipsec_vpn_pass_through_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_set_ipsec_vpn_pass_through_reqresp_impl_tuple_v01 = 
{
  &qcmap_set_ipsec_vpn_pass_through_req_msg_impl_v01,
  &qcmap_set_ipsec_vpn_pass_through_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_pptp_vpn_pass_through_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_pptp_vpn_pass_through_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* vpn_pass_through_value */
};

static const qmi_implemented_optionals qcmap_get_pptp_vpn_pass_through_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_pptp_vpn_pass_through_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_pptp_vpn_pass_through_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_pptp_vpn_pass_through_req_msg_impl_v01,
  &qcmap_get_pptp_vpn_pass_through_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_set_pptp_vpn_pass_through_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_set_pptp_vpn_pass_through_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_set_pptp_vpn_pass_through_reqresp_impl_tuple_v01 = 
{
  &qcmap_set_pptp_vpn_pass_through_req_msg_impl_v01,
  &qcmap_set_pptp_vpn_pass_through_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_l2tp_vpn_pass_through_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_l2tp_vpn_pass_through_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* vpn_pass_through_value */
};

static const qmi_implemented_optionals qcmap_get_l2tp_vpn_pass_through_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_l2tp_vpn_pass_through_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_l2tp_vpn_pass_through_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_l2tp_vpn_pass_through_req_msg_impl_v01,
  &qcmap_get_l2tp_vpn_pass_through_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_set_l2tp_vpn_pass_through_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_set_l2tp_vpn_pass_through_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_set_l2tp_vpn_pass_through_reqresp_impl_tuple_v01 = 
{
  &qcmap_set_l2tp_vpn_pass_through_req_msg_impl_v01,
  &qcmap_set_l2tp_vpn_pass_through_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_dynamic_nat_entry_timeout_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_dynamic_nat_entry_timeout_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* timeout */
};

static const qmi_implemented_optionals qcmap_get_dynamic_nat_entry_timeout_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_dynamic_nat_entry_timeout_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_dynamic_nat_entry_timeout_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_dynamic_nat_entry_timeout_req_msg_impl_v01,
  &qcmap_get_dynamic_nat_entry_timeout_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_set_dynamic_nat_entry_timeout_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_set_dynamic_nat_entry_timeout_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_set_dynamic_nat_entry_timeout_reqresp_impl_tuple_v01 = 
{
  &qcmap_set_dynamic_nat_entry_timeout_req_msg_impl_v01,
  &qcmap_set_dynamic_nat_entry_timeout_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_add_static_nat_entry_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_add_static_nat_entry_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_add_static_nat_entry_reqresp_impl_tuple_v01 = 
{
  &qcmap_add_static_nat_entry_req_msg_impl_v01,
  &qcmap_add_static_nat_entry_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_delete_static_nat_entry_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_delete_static_nat_entry_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_delete_static_nat_entry_reqresp_impl_tuple_v01 = 
{
  &qcmap_delete_static_nat_entry_req_msg_impl_v01,
  &qcmap_delete_static_nat_entry_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_static_nat_entries_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_static_nat_entries_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* snat_config */
};

static const qmi_implemented_optionals qcmap_get_static_nat_entries_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_static_nat_entries_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_static_nat_entries_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_static_nat_entries_req_msg_impl_v01,
  &qcmap_get_static_nat_entries_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_set_dmz_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_set_dmz_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_set_dmz_reqresp_impl_tuple_v01 = 
{
  &qcmap_set_dmz_req_msg_impl_v01,
  &qcmap_set_dmz_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_delete_dmz_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_delete_dmz_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_delete_dmz_reqresp_impl_tuple_v01 = 
{
  &qcmap_delete_dmz_req_msg_impl_v01,
  &qcmap_delete_dmz_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_dmz_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_dmz_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* dmz_ip_addr */
};

static const qmi_implemented_optionals qcmap_get_dmz_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_dmz_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_dmz_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_dmz_req_msg_impl_v01,
  &qcmap_get_dmz_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_wwan_config_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_wwan_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* v4_addr */
    QMI_SI_TLV_SET(0x11)   |  /* v6_addr */
    QMI_SI_TLV_SET(0x12)   |  /* v4_prim_dns_addr */
    QMI_SI_TLV_SET(0x13)   |  /* v4_sec_dns_addr */
    QMI_SI_TLV_SET(0x14)   |  /* v6_prim_dns_addr */
    QMI_SI_TLV_SET(0x15)      /* v6_sec_dns_addr */
};

static const qmi_implemented_optionals qcmap_get_wwan_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_wwan_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_wwan_config_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_wwan_config_req_msg_impl_v01,
  &qcmap_get_wwan_config_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_enable_firewall_setting_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_enable_firewall_setting_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_enable_firewall_setting_reqresp_impl_tuple_v01 = 
{
  &qcmap_enable_firewall_setting_req_msg_impl_v01,
  &qcmap_enable_firewall_setting_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_firewall_setting_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_firewall_setting_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* firewall_enabled */
    QMI_SI_TLV_SET(0x11)      /* pkts_allowed */
};

static const qmi_implemented_optionals qcmap_get_firewall_setting_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_firewall_setting_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_firewall_setting_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_firewall_setting_req_msg_impl_v01,
  &qcmap_get_firewall_setting_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_disable_firewall_setting_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_disable_firewall_setting_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_disable_firewall_setting_reqresp_impl_tuple_v01 = 
{
  &qcmap_disable_firewall_setting_req_msg_impl_v01,
  &qcmap_disable_firewall_setting_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_add_firewall_config_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_add_firewall_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* firewall_handle */
};

static const qmi_implemented_optionals qcmap_add_firewall_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_add_firewall_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_add_firewall_config_reqresp_impl_tuple_v01 = 
{
  &qcmap_add_firewall_config_req_msg_impl_v01,
  &qcmap_add_firewall_config_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_firewall_config_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_firewall_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* firewall_config */
};

static const qmi_implemented_optionals qcmap_get_firewall_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_firewall_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_firewall_config_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_firewall_config_req_msg_impl_v01,
  &qcmap_get_firewall_config_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_delete_firewall_config_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_delete_firewall_config_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_delete_firewall_config_reqresp_impl_tuple_v01 = 
{
  &qcmap_delete_firewall_config_req_msg_impl_v01,
  &qcmap_delete_firewall_config_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_wwan_status_ind_register_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_wwan_status_ind_register_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_wwan_status_reg_reqresp_impl_tuple_v01 = 
{
  &qcmap_wwan_status_ind_register_req_msg_impl_v01,
  &qcmap_wwan_status_ind_register_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_station_mode_enable_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_station_mode_enable_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_station_mode_enable_reqresp_impl_tuple_v01 = 
{
  &qcmap_station_mode_enable_req_msg_impl_v01,
  &qcmap_station_mode_enable_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_station_mode_disable_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals qcmap_station_mode_disable_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_station_mode_disable_reqresp_impl_tuple_v01 = 
{
  &qcmap_station_mode_disable_req_msg_impl_v01,
  &qcmap_station_mode_disable_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_station_mode_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_station_mode_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* station_mode */
};

static const qmi_implemented_optionals qcmap_get_station_mode_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_station_mode_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_station_mode_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_station_mode_req_msg_impl_v01,
  &qcmap_get_station_mode_resp_msg_impl_v01,
  NULL
};

static const uint32_t qcmap_wwan_status_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_end_reason */
    QMI_SI_TLV_SET(0x11)      /* verbose_call_end_reason */
};

static const qmi_implemented_optionals qcmap_wwan_status_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_wwan_status_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_wwan_status_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &qcmap_wwan_status_ind_msg_impl_v01
};

static const uint32_t qcmap_add_extd_firewall_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tcp_udp_src */
    QMI_SI_TLV_SET(0x11)   |  /* tcp_udp_dst */
    QMI_SI_TLV_SET(0x12)   |  /* icmp_type */
    QMI_SI_TLV_SET(0x13)   |  /* icmp_code */
    QMI_SI_TLV_SET(0x14)   |  /* esp_spi */
    QMI_SI_TLV_SET(0x15)   |  /* ip4_src_addr */
    QMI_SI_TLV_SET(0x16)   |  /* ip4_dst_addr */
    QMI_SI_TLV_SET(0x17)   |  /* ip4_tos */
    QMI_SI_TLV_SET(0x18)   |  /* ip6_src_addr */
    QMI_SI_TLV_SET(0x19)   |  /* ip6_dst_addr */
    QMI_SI_TLV_SET(0x1A)      /* ip6_trf_cls */
};

static const qmi_implemented_optionals qcmap_add_extd_firewall_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_add_extd_firewall_config_req_msg_impl_array_v01
};
static const uint32_t qcmap_add_extd_firewall_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* firewall_handle */
};

static const qmi_implemented_optionals qcmap_add_extd_firewall_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_add_extd_firewall_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_add_extd_firewall_config_reqresp_impl_tuple_v01 = 
{
  &qcmap_add_extd_firewall_config_req_msg_impl_v01,
  &qcmap_add_extd_firewall_config_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_extd_firewall_config_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_extd_firewall_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* next_hdr_prot */
    QMI_SI_TLV_SET(0x11)   |  /* tcp_udp_src */
    QMI_SI_TLV_SET(0x12)   |  /* tcp_udp_dst */
    QMI_SI_TLV_SET(0x13)   |  /* icmp_type */
    QMI_SI_TLV_SET(0x14)   |  /* icmp_code */
    QMI_SI_TLV_SET(0x15)   |  /* esp_spi */
    QMI_SI_TLV_SET(0x16)   |  /* ip4_src_addr */
    QMI_SI_TLV_SET(0x17)   |  /* ip4_dst_addr */
    QMI_SI_TLV_SET(0x18)   |  /* ip4_tos */
    QMI_SI_TLV_SET(0x19)   |  /* ip6_src_addr */
    QMI_SI_TLV_SET(0x1A)   |  /* ip6_dst_addr */
    QMI_SI_TLV_SET(0x1B)      /* ip6_trf_cls */
};

static const qmi_implemented_optionals qcmap_get_extd_firewall_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_extd_firewall_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_extd_firewall_config_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_extd_firewall_config_req_msg_impl_v01,
  &qcmap_get_extd_firewall_config_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_firewall_config_handle_list_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_firewall_config_handle_list_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* firewall_handle_list */
};

static const qmi_implemented_optionals qcmap_get_firewall_config_handle_list_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_firewall_config_handle_list_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_firewall_config_handle_list_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_firewall_config_handle_list_req_msg_impl_v01,
  &qcmap_get_firewall_config_handle_list_resp_msg_impl_v01,
  NULL
};

static const uint32_t qcmap_change_nat_type_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* nat_type_option */
};

static const qmi_implemented_optionals qcmap_change_nat_type_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_change_nat_type_req_msg_impl_array_v01
};
static const qmi_implemented_optionals qcmap_change_nat_type_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_qcmap_change_nat_type_reqresp_impl_tuple_v01 = 
{
  &qcmap_change_nat_type_req_msg_impl_v01,
  &qcmap_change_nat_type_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals qcmap_get_nat_type_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qcmap_get_nat_type_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* nat_type_option */
};

static const qmi_implemented_optionals qcmap_get_nat_type_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  qcmap_get_nat_type_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_qcmap_get_nat_type_reqresp_impl_tuple_v01 = 
{
  &qcmap_get_nat_type_req_msg_impl_v01,
  &qcmap_get_nat_type_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const qcmap_service_implemented_array_v01[] =
{
    NULL,                                                /* 0x0000 */
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
    &qmi_qcmap_get_supported_msgs_reqresp_impl_tuple_v01, /* 0x001E */
    &qmi_qcmap_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_qcmap_mobile_ap_enable_reqresp_impl_tuple_v01,  /* 0x0020 */
    &qmi_qcmap_mobile_ap_disable_reqresp_impl_tuple_v01, /* 0x0021 */
    &qmi_qcmap_bring_up_wwan_reqresp_impl_tuple_v01,     /* 0x0022 */
    &qmi_qcmap_tear_down_wwan_reqresp_impl_tuple_v01,    /* 0x0023 */
    &qmi_qcmap_get_wwan_status_reqresp_impl_tuple_v01,   /* 0x0024 */
    &qmi_qcmap_get_ipsec_vpn_pass_through_reqresp_impl_tuple_v01, /* 0x0025 */
    &qmi_qcmap_set_ipsec_vpn_pass_through_reqresp_impl_tuple_v01, /* 0x0026 */
    &qmi_qcmap_get_pptp_vpn_pass_through_reqresp_impl_tuple_v01, /* 0x0027 */
    &qmi_qcmap_set_pptp_vpn_pass_through_reqresp_impl_tuple_v01, /* 0x0028 */
    &qmi_qcmap_get_l2tp_vpn_pass_through_reqresp_impl_tuple_v01, /* 0x0029 */
    &qmi_qcmap_set_l2tp_vpn_pass_through_reqresp_impl_tuple_v01, /* 0x002A */
    &qmi_qcmap_get_dynamic_nat_entry_timeout_reqresp_impl_tuple_v01, /* 0x002B */
    &qmi_qcmap_set_dynamic_nat_entry_timeout_reqresp_impl_tuple_v01, /* 0x002C */
    &qmi_qcmap_add_static_nat_entry_reqresp_impl_tuple_v01, /* 0x002D */
    &qmi_qcmap_delete_static_nat_entry_reqresp_impl_tuple_v01, /* 0x002E */
    &qmi_qcmap_get_static_nat_entries_reqresp_impl_tuple_v01, /* 0x002F */
    &qmi_qcmap_set_dmz_reqresp_impl_tuple_v01,           /* 0x0030 */
    &qmi_qcmap_delete_dmz_reqresp_impl_tuple_v01,        /* 0x0031 */
    &qmi_qcmap_get_dmz_reqresp_impl_tuple_v01,           /* 0x0032 */
    &qmi_qcmap_get_wwan_config_reqresp_impl_tuple_v01,   /* 0x0033 */
    &qmi_qcmap_enable_firewall_setting_reqresp_impl_tuple_v01, /* 0x0034 */
    &qmi_qcmap_get_firewall_setting_reqresp_impl_tuple_v01, /* 0x0035 */
    &qmi_qcmap_disable_firewall_setting_reqresp_impl_tuple_v01, /* 0x0036 */
    &qmi_qcmap_add_firewall_config_reqresp_impl_tuple_v01, /* 0x0037 */
    &qmi_qcmap_get_firewall_config_reqresp_impl_tuple_v01, /* 0x0038 */
    &qmi_qcmap_delete_firewall_config_reqresp_impl_tuple_v01, /* 0x0039 */
    &qmi_qcmap_wwan_status_reg_reqresp_impl_tuple_v01,   /* 0x003A */
    &qmi_qcmap_station_mode_enable_reqresp_impl_tuple_v01, /* 0x003B */
    &qmi_qcmap_station_mode_disable_reqresp_impl_tuple_v01, /* 0x003C */
    &qmi_qcmap_get_station_mode_reqresp_impl_tuple_v01,  /* 0x003D */
    &qmi_qcmap_wwan_status_impl_tuple_v01,               /* 0x003E */
    &qmi_qcmap_add_extd_firewall_config_reqresp_impl_tuple_v01, /* 0x003F */
    &qmi_qcmap_get_extd_firewall_config_reqresp_impl_tuple_v01, /* 0x0040 */
    &qmi_qcmap_get_firewall_config_handle_list_reqresp_impl_tuple_v01, /* 0x0041 */
    &qmi_qcmap_change_nat_type_reqresp_impl_tuple_v01,   /* 0x0042 */
    &qmi_qcmap_get_nat_type_reqresp_impl_tuple_v01       /* 0x0043 */
};

static const qmi_implemented_messages qcmap_service_implemented_v01 =
{
  0x0043,  /* maximum message ID in this service */
  qcmap_service_implemented_array_v01
};

const qmi_implemented_messages *qcmap_get_service_impl_v01 (void)
{
  return &qcmap_service_implemented_v01;
}
