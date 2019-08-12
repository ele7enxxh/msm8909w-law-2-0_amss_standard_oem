/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q U A L C O M M _ M O B I L E _ A C C E S S _ P O I N T _ M S G R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the qcmap_msgr service Data structures.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Fri Jun 24 2016 (Spin 0)
   From IDL File: qualcomm_mobile_access_point_msgr_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t qcmap_msgr_snat_entry_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_snat_entry_config_v01, private_ip_addr),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_snat_entry_config_v01, private_port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_snat_entry_config_v01, global_port),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_snat_entry_config_v01, protocol),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_net_policy_info_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, tech_pref),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, ip_family),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, v4_profile_id_3gpp2),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, v4_profile_id_3gpp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, v6_profile_id_3gpp2),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, v6_profile_id_3gpp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_wwan_statistics_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, bytes_rx),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, bytes_tx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, pkts_rx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, pkts_tx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, pkts_dropped_rx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, pkts_dropped_tx),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_ip4_addr_subnet_mask_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip4_addr_subnet_mask_v01, addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip4_addr_subnet_mask_v01, subnet_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_ip6_addr_prefix_len_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip6_addr_prefix_len_v01, addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip6_addr_prefix_len_v01, prefix_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_tcp_udp_port_range_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_tcp_udp_port_range_v01, port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_tcp_udp_port_range_v01, range),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_ip4_tos_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip4_tos_v01, value),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip4_tos_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_ip6_traffic_class_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip6_traffic_class_v01, value),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip6_traffic_class_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_dhcp_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_config_v01, dhcp_start_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_config_v01, dhcp_end_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_config_v01, lease_time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_ip_passthrough_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ip_passthrough_config_v01, device_type),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ip_passthrough_config_v01, mac_addr),
  QCMAP_MSGR_MAC_ADDR_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ip_passthrough_config_v01, client_device_name),
  QCMAP_MSGR_DEVICE_NAME_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_lan_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_lan_config_v01, gw_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_lan_config_v01, netmask),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_lan_config_v01, enable_dhcp),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_lan_config_v01, dhcp_config),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_wlan_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_config_v01, gw_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_config_v01, netmask),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_config_v01, dhcp_config),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_usb_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_config_v01, gw_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_config_v01, netmask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_config_v01, usb_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_config_v01, lease_time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_ap_mode_wlan_usb_ip_config_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_ap_mode_wlan_usb_ip_config_v01, wlan_config),
  QMI_IDL_TYPE88(0, 11),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_ap_mode_wlan_usb_ip_config_v01, usb_config),
  QMI_IDL_TYPE88(0, 12),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_sta_static_ip_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sta_static_ip_config_v01, ip_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sta_static_ip_config_v01, gw_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sta_static_ip_config_v01, netmask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sta_static_ip_config_v01, dns_addr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_station_mode_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_config_v01, conn_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_config_v01, static_ip_config),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_config_v01, ap_sta_bridge_mode),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_wwan_call_end_reason_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_call_end_reason_v01, wwan_call_end_reason_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_call_end_reason_v01, wwan_call_end_reason_code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_data_bitrate_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_data_bitrate_v01, tx_rate),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_data_bitrate_v01, rx_rate),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_data_bitrate_v01, max_tx_rate),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_data_bitrate_v01, max_rx_rate),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_dhcp_reservation_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_reservation_v01, client_mac_addr),
  QCMAP_MSGR_MAC_ADDR_LEN_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_reservation_v01, client_reserved_ip),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_reservation_v01, client_device_name),
  QCMAP_MSGR_DEVICE_NAME_MAX_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_reservation_v01, enable_reservation),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_sip_server_info_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sip_server_info_v01, pcscf_info_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sip_server_info_v01, pcscf_ip_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sip_server_info_v01, pcscf_fqdn),
  QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_connected_device_info_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, client_mac_addr),
  QCMAP_MSGR_MAC_ADDR_LEN_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, device_type),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, host_name),
  QCMAP_MSGR_DEVICE_NAME_MAX_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, lease_expiry_time),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, ipv4_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, ll_ipv6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, ipv6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_ipv6_sip_server_info_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ipv6_sip_server_info_v01, pcscf_info_type),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ipv6_sip_server_info_v01, pcscf_ipv6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ipv6_sip_server_info_v01, pcscf_fqdn),
  QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_ddns_server_config_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ddns_server_config_v01, server_url),
  QCMAP_MSGR_DDNS_URL_LENGTH_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_wlan_iface_state_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, wlan_iface_state),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, wlan_iface_name),
  QCMAP_MSGR_DEVICE_NAME_MAX_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, wlan_iface_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, ip4_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, ip6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, ip_type),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * qcmap_msgr_mobile_ap_enable_req_msg is empty
 * static const uint8_t qcmap_msgr_mobile_ap_enable_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_mobile_ap_enable_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_enable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_enable_resp_msg_v01, mobile_ap_handle) - QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_enable_resp_msg_v01, mobile_ap_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_enable_resp_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_mobile_ap_disable_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_disable_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_mobile_ap_disable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_disable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_enable_ipv4_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_ipv4_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_ipv4_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_ipv4_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_ipv4_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_ipv4_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_ipv4_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_ipv4_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_ipv4_state_req_msg is empty
 * static const uint8_t qcmap_msgr_get_ipv4_state_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_ipv4_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv4_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ipv4_state_resp_msg_v01, ipv4_state) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipv4_state_resp_msg_v01, ipv4_state_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv4_state_resp_msg_v01, ipv4_state)
};

static const uint8_t qcmap_msgr_enable_ipv6_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_ipv6_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_ipv6_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_ipv6_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_ipv6_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_ipv6_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_ipv6_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_ipv6_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_ipv6_state_req_msg is empty
 * static const uint8_t qcmap_msgr_get_ipv6_state_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_ipv6_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_state_resp_msg_v01, ipv6_state) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_state_resp_msg_v01, ipv6_state_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_state_resp_msg_v01, ipv6_state)
};

static const uint8_t qcmap_msgr_bring_up_wwan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, call_type) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, call_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, call_type)
};

static const uint8_t qcmap_msgr_bring_up_wwan_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_resp_msg_v01, conn_status) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_resp_msg_v01, conn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_resp_msg_v01, conn_status)
};

static const uint8_t qcmap_msgr_bring_up_wwan_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, conn_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, wwan_call_end_reason) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, wwan_call_end_reason_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, wwan_call_end_reason),
  QMI_IDL_TYPE88(0, 16)
};

static const uint8_t qcmap_msgr_tear_down_wwan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, call_type) - QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, call_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, call_type)
};

static const uint8_t qcmap_msgr_tear_down_wwan_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_resp_msg_v01, conn_status) - QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_resp_msg_v01, conn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_resp_msg_v01, conn_status)
};

static const uint8_t qcmap_msgr_tear_down_wwan_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, conn_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, wwan_call_end_reason) - QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, wwan_call_end_reason_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, wwan_call_end_reason),
  QMI_IDL_TYPE88(0, 16)
};

static const uint8_t qcmap_msgr_wwan_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_wwan_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_wwan_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_call_end_reason) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_call_end_reason_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_call_end_reason),
  QMI_IDL_TYPE88(0, 16)
};

static const uint8_t qcmap_msgr_mobile_ap_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_mobile_ap_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_mobile_ap_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_ind_msg_v01, mobile_ap_status)
};

static const uint8_t qcmap_msgr_station_mode_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_station_mode_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_station_mode_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_status_ind_msg_v01, station_mode_status)
};

static const uint8_t qcmap_msgr_enable_wlan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_req_msg_v01, privileged_client) - QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_req_msg_v01, privileged_client_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_req_msg_v01, privileged_client)
};

static const uint8_t qcmap_msgr_enable_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_wlan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_req_msg_v01, privileged_client) - QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_req_msg_v01, privileged_client_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_req_msg_v01, privileged_client)
};

static const uint8_t qcmap_msgr_disable_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_add_static_nat_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_static_nat_entry_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_static_nat_entry_req_msg_v01, snat_entry_config),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t qcmap_msgr_add_static_nat_entry_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_static_nat_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_delete_static_nat_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_static_nat_entry_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_static_nat_entry_req_msg_v01, snat_entry_config),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t qcmap_msgr_delete_static_nat_entry_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_static_nat_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_static_nat_entries_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_static_nat_entries_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config),
  QCMAP_MSGR_MAX_SNAT_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t qcmap_msgr_set_dmz_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dmz_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dmz_req_msg_v01, dmz_ip_addr)
};

static const uint8_t qcmap_msgr_set_dmz_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dmz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_dmz_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_dmz_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_resp_msg_v01, dmz_ip_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_resp_msg_v01, dmz_ip_addr_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_resp_msg_v01, dmz_ip_addr)
};

static const uint8_t qcmap_msgr_delete_dmz_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dmz_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_delete_dmz_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dmz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_wwan_stats_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_req_msg_v01, ip_family)
};

static const uint8_t qcmap_msgr_get_wwan_stats_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_resp_msg_v01, wwan_stats) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_resp_msg_v01, wwan_stats_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_resp_msg_v01, wwan_stats),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t qcmap_msgr_reset_wwan_stats_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_reset_wwan_stats_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_reset_wwan_stats_req_msg_v01, ip_family)
};

static const uint8_t qcmap_msgr_reset_wwan_stats_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_reset_wwan_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01, vpn_pass_through_value)
};

static const uint8_t qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01, vpn_pass_through_value) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01, vpn_pass_through_value_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01, vpn_pass_through_value)
};

static const uint8_t qcmap_msgr_set_pptp_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01, vpn_pass_through_value)
};

static const uint8_t qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_pptp_vpn_pass_through_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value) - QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value)
};

static const uint8_t qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01, vpn_pass_through_value)
};

static const uint8_t qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value) - QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value)
};

static const uint8_t qcmap_msgr_set_nat_type_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_type_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_type_req_msg_v01, nat_type)
};

static const uint8_t qcmap_msgr_set_nat_type_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_nat_type_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_nat_type_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_resp_msg_v01, nat_type) - QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_resp_msg_v01, nat_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_resp_msg_v01, nat_type)
};

static const uint8_t qcmap_msgr_set_auto_connect_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_auto_connect_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_auto_connect_req_msg_v01, enable)
};

static const uint8_t qcmap_msgr_set_auto_connect_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_auto_connect_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_auto_connect_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_auto_connect_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_resp_msg_v01, auto_conn_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_resp_msg_v01, auto_conn_flag_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_resp_msg_v01, auto_conn_flag)
};

static const uint8_t qcmap_msgr_set_roaming_pref_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_roaming_pref_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_roaming_pref_req_msg_v01, allow_wwan_calls_while_roaming)
};

static const uint8_t qcmap_msgr_set_roaming_pref_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_roaming_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_roaming_pref_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_roaming_pref_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_resp_msg_v01, allow_wwan_calls_while_roaming) - QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_resp_msg_v01, allow_wwan_calls_while_roaming_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_resp_msg_v01, allow_wwan_calls_while_roaming)
};

static const uint8_t qcmap_msgr_set_loopback_flag_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_loopback_flag_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_loopback_flag_req_msg_v01, loopback_flag)
};

static const uint8_t qcmap_msgr_set_loopback_flag_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_loopback_flag_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_loopback_flag_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_loopback_flag_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_resp_msg_v01, loopback_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_resp_msg_v01, loopback_flag_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_resp_msg_v01, loopback_flag)
};

static const uint8_t qcmap_msgr_add_firewall_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip_version),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, next_hdr_prot) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, next_hdr_prot_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, next_hdr_prot),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_src) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_src_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_src),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_dst) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_dst_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_dst),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_type) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_type_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_code) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_code_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, esp_spi) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, esp_spi_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, esp_spi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_src_addr) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_src_addr_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_src_addr),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_dst_addr) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_dst_addr_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_dst_addr),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_tos) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_tos_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_tos),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_src_addr) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_src_addr_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_src_addr),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_dst_addr) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_dst_addr_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_dst_addr),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_trf_cls) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_trf_cls_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_trf_cls),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, upnp_pinhole) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, upnp_pinhole_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, upnp_pinhole)
};

static const uint8_t qcmap_msgr_add_firewall_entry_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_resp_msg_v01, firewall_handle) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_resp_msg_v01, firewall_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_resp_msg_v01, firewall_handle)
};

static const uint8_t qcmap_msgr_get_firewall_entries_handle_list_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01, ip_version)
};

static const uint8_t qcmap_msgr_get_firewall_entries_handle_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list),
  QCMAP_MSGR_MAX_FIREWALL_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list_len)
};

static const uint8_t qcmap_msgr_get_firewall_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_req_msg_v01, firewall_handle)
};

static const uint8_t qcmap_msgr_get_firewall_entry_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip_version) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip_version_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip_version),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, next_hdr_prot) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, next_hdr_prot_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, next_hdr_prot),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_src) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_src_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_src),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_dst) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_dst_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_dst),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_type) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_type_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_code) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_code_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, esp_spi) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, esp_spi_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, esp_spi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_src_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_src_addr_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_src_addr),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_dst_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_dst_addr_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_dst_addr),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_tos) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_tos_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_tos),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_src_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_src_addr_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_src_addr),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_dst_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_dst_addr_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_dst_addr),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_trf_cls) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_trf_cls_valid)),
  0x1C,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_trf_cls),
  QMI_IDL_TYPE88(0, 7)
};

static const uint8_t qcmap_msgr_delete_firewall_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_firewall_entry_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_firewall_entry_req_msg_v01, firewall_handle)
};

static const uint8_t qcmap_msgr_delete_firewall_entry_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_firewall_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_wwan_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_req_msg_v01, addr_type_op)
};

static const uint8_t qcmap_msgr_get_wwan_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_addr_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_addr_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_prim_dns_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_prim_dns_addr_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_prim_dns_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_sec_dns_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_sec_dns_addr_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_sec_dns_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_prim_dns_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_prim_dns_addr_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_prim_dns_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_sec_dns_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_sec_dns_addr_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_sec_dns_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01
};

static const uint8_t qcmap_msgr_mobile_ap_status_req_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_req_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_mobile_ap_status_resp_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_resp_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_resp_v01, mobile_ap_status) - QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_resp_v01, mobile_ap_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_resp_v01, mobile_ap_status)
};

static const uint8_t qcmap_msgr_wwan_status_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, call_type) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, call_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, call_type)
};

static const uint8_t qcmap_msgr_wwan_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_resp_msg_v01, conn_status) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_resp_msg_v01, conn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_resp_msg_v01, conn_status)
};

/*
 * qcmap_msgr_get_station_mode_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_station_mode_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_station_mode_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_station_mode_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_station_mode_status_resp_msg_v01, conn_status) - QMI_IDL_OFFSET8(qcmap_msgr_get_station_mode_status_resp_msg_v01, conn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_station_mode_status_resp_msg_v01, conn_status)
};

static const uint8_t qcmap_msgr_set_nat_timeout_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_timeout_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_timeout_req_msg_v01, nat_timeout_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_timeout_req_msg_v01, timeout_value)
};

static const uint8_t qcmap_msgr_set_nat_timeout_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_timeout_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_nat_timeout_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_req_msg_v01, nat_timeout_type)
};

static const uint8_t qcmap_msgr_get_nat_timeout_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_resp_msg_v01, timeout_value) - QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_resp_msg_v01, timeout_value_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_resp_msg_v01, timeout_value)
};

static const uint8_t qcmap_msgr_set_lan_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_lan_config_req_msg_v01, lan_config) - QMI_IDL_OFFSET8(qcmap_msgr_set_lan_config_req_msg_v01, lan_config_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_lan_config_req_msg_v01, lan_config),
  QMI_IDL_TYPE88(0, 10)
};

static const uint8_t qcmap_msgr_set_lan_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_lan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_wlan_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, wlan_mode) - QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, wlan_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, wlan_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_acess_profile) - QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_acess_profile_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_acess_profile),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, station_config) - QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, station_config_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, station_config),
  QMI_IDL_TYPE88(0, 15)
};

static const uint8_t qcmap_msgr_set_wlan_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_wlan_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_wlan_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_resp_msg_v01, wlan_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_resp_msg_v01, wlan_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_resp_msg_v01, wlan_mode)
};

static const uint8_t qcmap_msgr_activate_wlan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_wlan_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_activate_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_activate_lan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_lan_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_activate_lan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_lan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_lan_config_req_msg is empty
 * static const uint8_t qcmap_msgr_get_lan_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_lan_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, lan_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, lan_config_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, lan_config),
  QMI_IDL_TYPE88(0, 10),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, ap_wlan_usb_ip_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, ap_wlan_usb_ip_config_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, ap_wlan_usb_ip_config),
  QMI_IDL_TYPE88(0, 13)
};

/*
 * qcmap_msgr_get_wlan_config_req_msg is empty
 * static const uint8_t qcmap_msgr_get_wlan_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_wlan_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, wlan_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, wlan_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, wlan_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, station_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, station_config_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, station_config),
  QMI_IDL_TYPE88(0, 15),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_access_profile) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_access_profile_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_access_profile)
};

static const uint8_t qcmap_msgr_activate_hostapd_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, ap_type) - QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, ap_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, ap_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, action_type) - QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, action_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, action_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, privileged_client) - QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, privileged_client_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, privileged_client)
};

static const uint8_t qcmap_msgr_activate_hostapd_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_activate_supplicant_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_supplicant_config_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_activate_supplicant_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_supplicant_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_usb_link_up_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_up_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_up_req_msg_v01, usb_link)
};

static const uint8_t qcmap_msgr_usb_link_up_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_up_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_usb_link_down_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_down_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_down_req_msg_v01, usb_link)
};

static const uint8_t qcmap_msgr_usb_link_down_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_down_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_wwan_policy_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_policy_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_policy_req_msg_v01, wwan_policy),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t qcmap_msgr_set_wwan_policy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_wwan_policy_req_msg is empty
 * static const uint8_t qcmap_msgr_get_wwan_policy_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_wwan_policy_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_resp_msg_v01, wwan_policy) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_resp_msg_v01, wwan_policy_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_resp_msg_v01, wwan_policy),
  QMI_IDL_TYPE88(0, 1)
};

/*
 * qcmap_msgr_enable_upnp_req_msg is empty
 * static const uint8_t qcmap_msgr_enable_upnp_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_enable_upnp_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_upnp_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_disable_upnp_req_msg is empty
 * static const uint8_t qcmap_msgr_disable_upnp_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_disable_upnp_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_upnp_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_enable_dlna_req_msg is empty
 * static const uint8_t qcmap_msgr_enable_dlna_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_enable_dlna_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_dlna_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_disable_dlna_req_msg is empty
 * static const uint8_t qcmap_msgr_disable_dlna_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_disable_dlna_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_dlna_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_firewall_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, firewall_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, pkts_allowed) - QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, pkts_allowed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, pkts_allowed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, upnp_pinhole_flag) - QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, upnp_pinhole_flag_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, upnp_pinhole_flag)
};

static const uint8_t qcmap_msgr_set_firewall_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_firewall_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_firewall_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, firewall_enabled) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, firewall_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, firewall_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, pkts_allowed) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, pkts_allowed_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, pkts_allowed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, upnp_pinhole_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, upnp_pinhole_flag_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, upnp_pinhole_flag)
};

/*
 * qcmap_msgr_enable_multicast_dns_req_msg is empty
 * static const uint8_t qcmap_msgr_enable_multicast_dns_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_enable_multicast_dns_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_multicast_dns_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_disable_multicast_dns_req_msg is empty
 * static const uint8_t qcmap_msgr_disable_multicast_dns_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_disable_multicast_dns_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_multicast_dns_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_upnp_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_upnp_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_upnp_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_status_resp_msg_v01, upnp_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_status_resp_msg_v01, upnp_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_status_resp_msg_v01, upnp_mode)
};

/*
 * qcmap_msgr_get_dlna_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_dlna_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_dlna_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_status_resp_msg_v01, dlna_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_status_resp_msg_v01, dlna_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_status_resp_msg_v01, dlna_mode)
};

/*
 * qcmap_msgr_get_multicast_dns_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_multicast_dns_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_multicast_dns_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_multicast_dns_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_multicast_dns_status_resp_msg_v01, mdns_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_multicast_dns_status_resp_msg_v01, mdns_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_multicast_dns_status_resp_msg_v01, mdns_mode)
};

static const uint8_t qcmap_msgr_set_qcmap_bootup_cfg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, mobileap_bootup_flag) - QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, mobileap_bootup_flag_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, mobileap_bootup_flag),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, wlan_bootup_flag) - QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, wlan_bootup_flag_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, wlan_bootup_flag)
};

static const uint8_t qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_qcmap_bootup_cfg_req_msg is empty
 * static const uint8_t qcmap_msgr_get_qcmap_bootup_cfg_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, mobileap_bootup_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, mobileap_bootup_flag_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, mobileap_bootup_flag),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, wlan_bootup_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, wlan_bootup_flag_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, wlan_bootup_flag)
};

static const uint8_t qcmap_msgr_set_dlna_media_dir_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_media_dir_req_msg_v01, media_dir),
  ((QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01) & 0xFF), ((QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01) >> 8),
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_media_dir_req_msg_v01, media_dir) - QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_media_dir_req_msg_v01, media_dir_len)
};

static const uint8_t qcmap_msgr_set_dlna_media_dir_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_media_dir_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_dlna_media_dir_req_msg is empty
 * static const uint8_t qcmap_msgr_get_dlna_media_dir_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_dlna_media_dir_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir),
  ((QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01) & 0xFF), ((QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01) >> 8),
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir_len)
};

static const uint8_t qcmap_msgr_get_data_bitrate_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_data_bitrate_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_resp_msg_v01, data_rate) - QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_resp_msg_v01, data_rate_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_resp_msg_v01, data_rate),
  QMI_IDL_TYPE88(0, 17)
};

/*
 * qcmap_msgr_get_upnp_notify_interval_req_msg is empty
 * static const uint8_t qcmap_msgr_get_upnp_notify_interval_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_upnp_notify_interval_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01, notify_interval) - QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01, notify_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01, notify_interval)
};

static const uint8_t qcmap_msgr_set_upnp_notify_interval_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_upnp_notify_interval_req_msg_v01, notify_interval) - QMI_IDL_OFFSET8(qcmap_msgr_set_upnp_notify_interval_req_msg_v01, notify_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_upnp_notify_interval_req_msg_v01, notify_interval)
};

static const uint8_t qcmap_msgr_set_upnp_notify_interval_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_upnp_notify_interval_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_dlna_notify_interval_req_msg is empty
 * static const uint8_t qcmap_msgr_get_dlna_notify_interval_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_dlna_notify_interval_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01, notify_interval) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01, notify_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01, notify_interval)
};

static const uint8_t qcmap_msgr_set_dlna_notify_interval_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_notify_interval_req_msg_v01, notify_interval) - QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_notify_interval_req_msg_v01, notify_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_notify_interval_req_msg_v01, notify_interval)
};

static const uint8_t qcmap_msgr_set_dlna_notify_interval_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_notify_interval_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_add_dhcp_reservation_record_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dhcp_reservation_record_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dhcp_reservation_record_req_msg_v01, dhcp_reservation_record),
  QMI_IDL_TYPE88(0, 18)
};

static const uint8_t qcmap_msgr_add_dhcp_reservation_record_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_dhcp_reservation_records_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_dhcp_reservation_records_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records) - QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records),
  QCMAP_MSGR_MAX_DHCP_RESERVATION_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records) - QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records_len),
  QMI_IDL_TYPE88(0, 18)
};

static const uint8_t qcmap_msgr_edit_dhcp_reservation_record_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, client_reserved_ip) - QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, client_reserved_ip_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, client_reserved_ip),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, dhcp_reservation_record) - QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, dhcp_reservation_record_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, dhcp_reservation_record),
  QMI_IDL_TYPE88(0, 18)
};

static const uint8_t qcmap_msgr_edit_dhcp_reservation_record_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_delete_dhcp_reservation_record_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01, client_reserved_ip) - QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01, client_reserved_ip_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01, client_reserved_ip)
};

static const uint8_t qcmap_msgr_delete_dhcp_reservation_record_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_enable_alg_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_alg_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_alg_req_msg_v01, alg_type_op)
};

static const uint8_t qcmap_msgr_enable_alg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_alg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_alg_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_alg_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_alg_req_msg_v01, alg_type_op)
};

static const uint8_t qcmap_msgr_disable_alg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_alg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_webserver_wwan_access_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_webserver_wwan_access_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_webserver_wwan_access_req_msg_v01, webserver_wwan_access)
};

static const uint8_t qcmap_msgr_set_webserver_wwan_access_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_webserver_wwan_access_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_webserver_wwan_access_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_webserver_wwan_access_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01, webserver_wwan_access) - QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01, webserver_wwan_access_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01, webserver_wwan_access)
};

static const uint8_t qcmap_msgr_set_sip_server_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_sip_server_info_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_sip_server_info_req_msg_v01, default_sip_server_info),
  QMI_IDL_TYPE88(0, 19)
};

static const uint8_t qcmap_msgr_set_sip_server_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_sip_server_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_sip_server_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_sip_server_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, default_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, default_sip_server_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, default_sip_server_info),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info),
  QCMAP_MSGR_MAX_SIP_SERVER_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info_len),
  QMI_IDL_TYPE88(0, 19)
};

static const uint8_t qcmap_msgr_restore_factory_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_restore_factory_config_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_restore_factory_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_restore_factory_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_odu_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_odu_mode_req_msg_v01, mode) - QMI_IDL_OFFSET8(qcmap_msgr_set_odu_mode_req_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_odu_mode_req_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_odu_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_odu_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_odu_mode_req_msg is empty
 * static const uint8_t qcmap_msgr_get_odu_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_odu_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_odu_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_odu_mode_resp_msg_v01, mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_odu_mode_resp_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_odu_mode_resp_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_odu_pswd_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_odu_pswd_req_msg_v01, password) - QMI_IDL_OFFSET8(qcmap_msgr_set_odu_pswd_req_msg_v01, password_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qcmap_msgr_set_odu_pswd_req_msg_v01, password),
  15
};

static const uint8_t qcmap_msgr_set_odu_pswd_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_odu_pswd_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_connected_devices_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_connected_devices_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info),
  QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info_len),
  QMI_IDL_TYPE88(0, 20)
};

static const uint8_t qcmap_msgr_get_ipv6_sip_server_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_ipv6_sip_server_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info),
  QCMAP_MSGR_MAX_SIP_SERVER_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info_len),
  QMI_IDL_TYPE88(0, 21)
};

static const uint8_t qcmap_msgr_set_supplicant_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_supplicant_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_supplicant_config_req_msg_v01, supplicant_config_status)
};

static const uint8_t qcmap_msgr_set_supplicant_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_supplicant_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_cradle_mode_req_msg is empty
 * static const uint8_t qcmap_msgr_get_cradle_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_cradle_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_cradle_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_cradle_mode_resp_msg_v01, mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_cradle_mode_resp_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_cradle_mode_resp_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_cradle_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_cradle_mode_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_cradle_mode_req_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_cradle_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_cradle_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_cradle_mode_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_cradle_mode_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_cradle_mode_status_ind_msg_v01, cradle_status)
};

/*
 * qcmap_msgr_get_prefix_delegation_config_req_msg is empty
 * static const uint8_t qcmap_msgr_get_prefix_delegation_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_prefix_delegation_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01, prefix_delegation) - QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01, prefix_delegation_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01, prefix_delegation)
};

static const uint8_t qcmap_msgr_set_prefix_delegation_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_prefix_delegation_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_prefix_delegation_config_req_msg_v01, prefix_delegation)
};

static const uint8_t qcmap_msgr_set_prefix_delegation_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_prefix_delegation_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_prefix_delegation_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_prefix_delegation_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01, prefix_delegation) - QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01, prefix_delegation_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01, prefix_delegation)
};

static const uint8_t qcmap_msgr_set_gateway_url_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_req_msg_v01, gateway_url),
  ((QCMAP_MSGR_MAX_GATEWAY_URL_V01) & 0xFF), ((QCMAP_MSGR_MAX_GATEWAY_URL_V01) >> 8),
  QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_req_msg_v01, gateway_url) - QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_req_msg_v01, gateway_url_len)
};

static const uint8_t qcmap_msgr_set_gateway_url_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_gateway_url_req_msg is empty
 * static const uint8_t qcmap_msgr_get_gateway_url_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_gateway_url_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url) - QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url),
  ((QCMAP_MSGR_MAX_GATEWAY_URL_V01) & 0xFF), ((QCMAP_MSGR_MAX_GATEWAY_URL_V01) >> 8),
  QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url) - QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url_len)
};

static const uint8_t qcmap_msgr_enable_dynamic_dns_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_dynamic_dns_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_dynamic_dns_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_dynamic_dns_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_dynamic_dns_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_dynamic_dns_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_dynamic_dns_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_dynamic_dns_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_dynamic_dns_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, mobile_ap_handle),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, ddns_server),
  QCMAP_MSGR_DDNS_URL_LENGTH_V01,

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, hostname),
  QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01,

  0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, login),
  QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01,

  0x05,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, password),
  QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, timeout) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, timeout_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, timeout)
};

static const uint8_t qcmap_msgr_set_dynamic_dns_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_dynamic_dns_config_req_msg is empty
 * static const uint8_t qcmap_msgr_get_dynamic_dns_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_dynamic_dns_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, ddns_config),
  QCMAP_MSGR_MAX_DDNS_SERVER_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, ddns_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, ddns_config_len),
  QMI_IDL_TYPE88(0, 22),

  0x04,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, hostname) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, hostname_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, hostname),
  QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, timeout) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, timeout_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, timeout)
};

/*
 * qcmap_msgr_get_tiny_proxy_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_tiny_proxy_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_tiny_proxy_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01, tiny_proxy_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01, tiny_proxy_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01, tiny_proxy_mode)
};

static const uint8_t qcmap_msgr_enable_tiny_proxy_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_tiny_proxy_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_tiny_proxy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_tiny_proxy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_tiny_proxy_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_tiny_proxy_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_tiny_proxy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_tiny_proxy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_dlna_whitelisting_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_whitelisting_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_whitelisting_req_msg_v01, dlna_whitelist_allow)
};

static const uint8_t qcmap_msgr_set_dlna_whitelisting_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_whitelisting_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_dlna_whitelisting_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_dlna_whitelisting_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01, dlna_whitelist_allow) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01, dlna_whitelist_allow_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01, dlna_whitelist_allow)
};

static const uint8_t qcmap_msgr_add_dlna_whitelistip_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dlna_whitelistip_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dlna_whitelistip_req_msg_v01, dlna_whitelist_ip_addr)
};

static const uint8_t qcmap_msgr_add_dlna_whitelistip_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dlna_whitelistip_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_delete_dlna_whitelist_ip_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01, dlna_whitelist_ip_addr)
};

static const uint8_t qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_enable_sta_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_sta_mode_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_sta_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_sta_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_sta_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_sta_mode_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_sta_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_sta_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_wlan_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_wlan_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_wlan_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_msg_v01, wlan_status),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_msg_v01, wlan_state),
  QCMAP_MSGR_MAX_WLAN_IFACE_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_msg_v01, wlan_state) - QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_msg_v01, wlan_state_len),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_wlan_status_ind_msg_v01, wlan_mode)
};

static const uint8_t qcmap_msgr_set_backhaul_pref_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, first_priority_backhaul),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, second_priority_backhaul),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, third_priority_backhaul),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x05,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, fourth_priority_backhaul)
};

static const uint8_t qcmap_msgr_set_backhaul_pref_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_backhaul_pref_req_msg is empty
 * static const uint8_t qcmap_msgr_get_backhaul_pref_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_backhaul_pref_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, first_priority_backhaul) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, first_priority_backhaul_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, first_priority_backhaul),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, second_priority_backhaul) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, second_priority_backhaul_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, second_priority_backhaul),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, third_priority_backhaul) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, third_priority_backhaul_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, third_priority_backhaul),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, fourth_priority_backhaul) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, fourth_priority_backhaul_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, fourth_priority_backhaul)
};

static const uint8_t qcmap_msgr_cradle_mode_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_cradle_mode_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_cradle_mode_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_eth_backhaul_mode_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_eth_backhaul_mode_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_eth_backhaul_mode_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_eth_backhaul_mode_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_eth_backhaul_mode_req_msg is empty
 * static const uint8_t qcmap_msgr_get_eth_backhaul_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_eth_backhaul_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_eth_backhaul_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_eth_backhaul_mode_resp_msg_v01, mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_eth_backhaul_mode_resp_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_eth_backhaul_mode_resp_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_eth_backhaul_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_eth_backhaul_mode_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_eth_backhaul_mode_req_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_eth_backhaul_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_eth_backhaul_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_eth_backhaul_mode_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_eth_backhaul_mode_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_eth_backhaul_mode_status_ind_msg_v01, eth_backhaul_status)
};

static const uint8_t qcmap_msgr_set_ip_passthrough_flag_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, enable_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, passthrough_config) - QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, passthrough_config_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, passthrough_config),
  QMI_IDL_TYPE88(0, 9)
};

static const uint8_t qcmap_msgr_set_ip_passthrough_flag_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_ip_passthrough_flag_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_flag_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_ip_passthrough_flag_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01, enable_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01, passthrough_config),
  QMI_IDL_TYPE88(0, 9)
};

static const uint8_t qcmap_msgr_get_ip_passthrough_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_state_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_ip_passthrough_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_state_resp_msg_v01, state)
};

static const uint8_t qcmap_msgr_bring_up_bt_tethering_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_bt_tethering_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_bring_up_bt_tethering_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_bring_down_bt_tethering_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_down_bt_tethering_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_bring_down_bt_tethering_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_bt_tethering_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_bt_tethering_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, bt_tethering_status) - QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, bt_tethering_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, bt_tethering_status)
};

static const uint8_t qcmap_msgr_bt_tethering_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_bt_tethering_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_status_ind_msg_v01, bt_tethering_status)
};

/* Type Table */
static const qmi_idl_type_table_entry  qcmap_msgr_type_table_v01[] = {
  {sizeof(qcmap_msgr_snat_entry_config_v01), qcmap_msgr_snat_entry_config_data_v01},
  {sizeof(qcmap_msgr_net_policy_info_v01), qcmap_msgr_net_policy_info_data_v01},
  {sizeof(qcmap_msgr_wwan_statistics_type_v01), qcmap_msgr_wwan_statistics_type_data_v01},
  {sizeof(qcmap_ip4_addr_subnet_mask_v01), qcmap_ip4_addr_subnet_mask_data_v01},
  {sizeof(qcmap_ip6_addr_prefix_len_v01), qcmap_ip6_addr_prefix_len_data_v01},
  {sizeof(qcmap_tcp_udp_port_range_v01), qcmap_tcp_udp_port_range_data_v01},
  {sizeof(qcmap_ip4_tos_v01), qcmap_ip4_tos_data_v01},
  {sizeof(qcmap_ip6_traffic_class_v01), qcmap_ip6_traffic_class_data_v01},
  {sizeof(qcmap_msgr_dhcp_config_v01), qcmap_msgr_dhcp_config_data_v01},
  {sizeof(qcmap_msgr_ip_passthrough_config_v01), qcmap_msgr_ip_passthrough_config_data_v01},
  {sizeof(qcmap_msgr_lan_config_v01), qcmap_msgr_lan_config_data_v01},
  {sizeof(qcmap_msgr_wlan_config_v01), qcmap_msgr_wlan_config_data_v01},
  {sizeof(qcmap_msgr_usb_config_v01), qcmap_msgr_usb_config_data_v01},
  {sizeof(qcmap_msgr_ap_mode_wlan_usb_ip_config_v01), qcmap_msgr_ap_mode_wlan_usb_ip_config_data_v01},
  {sizeof(qcmap_msgr_sta_static_ip_config_v01), qcmap_msgr_sta_static_ip_config_data_v01},
  {sizeof(qcmap_msgr_station_mode_config_v01), qcmap_msgr_station_mode_config_data_v01},
  {sizeof(qcmap_msgr_wwan_call_end_reason_v01), qcmap_msgr_wwan_call_end_reason_data_v01},
  {sizeof(qcmap_msgr_data_bitrate_v01), qcmap_msgr_data_bitrate_data_v01},
  {sizeof(qcmap_msgr_dhcp_reservation_v01), qcmap_msgr_dhcp_reservation_data_v01},
  {sizeof(qcmap_msgr_sip_server_info_v01), qcmap_msgr_sip_server_info_data_v01},
  {sizeof(qcmap_msgr_connected_device_info_v01), qcmap_msgr_connected_device_info_data_v01},
  {sizeof(qcmap_msgr_ipv6_sip_server_info_v01), qcmap_msgr_ipv6_sip_server_info_data_v01},
  {sizeof(qcmap_msgr_ddns_server_config_v01), qcmap_msgr_ddns_server_config_data_v01},
  {sizeof(qcmap_msgr_wlan_iface_state_v01), qcmap_msgr_wlan_iface_state_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry qcmap_msgr_message_table_v01[] = {
  {sizeof(qcmap_msgr_mobile_ap_enable_req_msg_v01), 0},
  {sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01), qcmap_msgr_mobile_ap_enable_resp_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_disable_req_msg_v01), qcmap_msgr_mobile_ap_disable_req_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01), qcmap_msgr_mobile_ap_disable_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_ipv4_req_msg_v01), qcmap_msgr_enable_ipv4_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_ipv4_resp_msg_v01), qcmap_msgr_enable_ipv4_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_ipv4_req_msg_v01), qcmap_msgr_disable_ipv4_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_ipv4_resp_msg_v01), qcmap_msgr_disable_ipv4_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipv4_state_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_ipv4_state_resp_msg_v01), qcmap_msgr_get_ipv4_state_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_ipv6_req_msg_v01), qcmap_msgr_enable_ipv6_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_ipv6_resp_msg_v01), qcmap_msgr_enable_ipv6_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_ipv6_req_msg_v01), qcmap_msgr_disable_ipv6_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_ipv6_resp_msg_v01), qcmap_msgr_disable_ipv6_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipv6_state_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_ipv6_state_resp_msg_v01), qcmap_msgr_get_ipv6_state_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_wwan_req_msg_v01), qcmap_msgr_bring_up_wwan_req_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01), qcmap_msgr_bring_up_wwan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_wwan_ind_msg_v01), qcmap_msgr_bring_up_wwan_ind_msg_data_v01},
  {sizeof(qcmap_msgr_tear_down_wwan_req_msg_v01), qcmap_msgr_tear_down_wwan_req_msg_data_v01},
  {sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01), qcmap_msgr_tear_down_wwan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_tear_down_wwan_ind_msg_v01), qcmap_msgr_tear_down_wwan_ind_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_status_ind_register_req_msg_v01), qcmap_msgr_wwan_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_status_ind_register_resp_msg_v01), qcmap_msgr_wwan_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_status_ind_msg_v01), qcmap_msgr_wwan_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_ind_register_req_msg_v01), qcmap_msgr_mobile_ap_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01), qcmap_msgr_mobile_ap_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_ind_msg_v01), qcmap_msgr_mobile_ap_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_station_mode_status_ind_register_req_msg_v01), qcmap_msgr_station_mode_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_station_mode_status_ind_register_resp_msg_v01), qcmap_msgr_station_mode_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_station_mode_status_ind_msg_v01), qcmap_msgr_station_mode_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_enable_wlan_req_msg_v01), qcmap_msgr_enable_wlan_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_wlan_resp_msg_v01), qcmap_msgr_enable_wlan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_wlan_req_msg_v01), qcmap_msgr_disable_wlan_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_wlan_resp_msg_v01), qcmap_msgr_disable_wlan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_add_static_nat_entry_req_msg_v01), qcmap_msgr_add_static_nat_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_add_static_nat_entry_resp_msg_v01), qcmap_msgr_add_static_nat_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_static_nat_entry_req_msg_v01), qcmap_msgr_delete_static_nat_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_static_nat_entry_resp_msg_v01), qcmap_msgr_delete_static_nat_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_static_nat_entries_req_msg_v01), qcmap_msgr_get_static_nat_entries_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_static_nat_entries_resp_msg_v01), qcmap_msgr_get_static_nat_entries_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dmz_req_msg_v01), qcmap_msgr_set_dmz_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dmz_resp_msg_v01), qcmap_msgr_set_dmz_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dmz_req_msg_v01), qcmap_msgr_get_dmz_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_dmz_resp_msg_v01), qcmap_msgr_get_dmz_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dmz_req_msg_v01), qcmap_msgr_delete_dmz_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dmz_resp_msg_v01), qcmap_msgr_delete_dmz_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_stats_req_msg_v01), qcmap_msgr_get_wwan_stats_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_stats_resp_msg_v01), qcmap_msgr_get_wwan_stats_resp_msg_data_v01},
  {sizeof(qcmap_msgr_reset_wwan_stats_req_msg_v01), qcmap_msgr_reset_wwan_stats_req_msg_data_v01},
  {sizeof(qcmap_msgr_reset_wwan_stats_resp_msg_v01), qcmap_msgr_reset_wwan_stats_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01), qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01), qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01), qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01), qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01), qcmap_msgr_set_pptp_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01), qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01), qcmap_msgr_get_pptp_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01), qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01), qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01), qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01), qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01), qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_nat_type_req_msg_v01), qcmap_msgr_set_nat_type_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_nat_type_resp_msg_v01), qcmap_msgr_set_nat_type_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_nat_type_req_msg_v01), qcmap_msgr_get_nat_type_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_nat_type_resp_msg_v01), qcmap_msgr_get_nat_type_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_auto_connect_req_msg_v01), qcmap_msgr_set_auto_connect_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_auto_connect_resp_msg_v01), qcmap_msgr_set_auto_connect_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_auto_connect_req_msg_v01), qcmap_msgr_get_auto_connect_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_auto_connect_resp_msg_v01), qcmap_msgr_get_auto_connect_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_roaming_pref_req_msg_v01), qcmap_msgr_set_roaming_pref_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_roaming_pref_resp_msg_v01), qcmap_msgr_set_roaming_pref_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_roaming_pref_req_msg_v01), qcmap_msgr_get_roaming_pref_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_roaming_pref_resp_msg_v01), qcmap_msgr_get_roaming_pref_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_loopback_flag_req_msg_v01), qcmap_msgr_set_loopback_flag_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_loopback_flag_resp_msg_v01), qcmap_msgr_set_loopback_flag_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_loopback_flag_req_msg_v01), qcmap_msgr_get_loopback_flag_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_loopback_flag_resp_msg_v01), qcmap_msgr_get_loopback_flag_resp_msg_data_v01},
  {sizeof(qcmap_msgr_add_firewall_entry_req_msg_v01), qcmap_msgr_add_firewall_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_add_firewall_entry_resp_msg_v01), qcmap_msgr_add_firewall_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01), qcmap_msgr_get_firewall_entries_handle_list_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01), qcmap_msgr_get_firewall_entries_handle_list_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_entry_req_msg_v01), qcmap_msgr_get_firewall_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_entry_resp_msg_v01), qcmap_msgr_get_firewall_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_firewall_entry_req_msg_v01), qcmap_msgr_delete_firewall_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_firewall_entry_resp_msg_v01), qcmap_msgr_delete_firewall_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_config_req_msg_v01), qcmap_msgr_get_wwan_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01), qcmap_msgr_get_wwan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_req_v01), qcmap_msgr_mobile_ap_status_req_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_resp_v01), qcmap_msgr_mobile_ap_status_resp_data_v01},
  {sizeof(qcmap_msgr_wwan_status_req_msg_v01), qcmap_msgr_wwan_status_req_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_status_resp_msg_v01), qcmap_msgr_wwan_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_station_mode_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_station_mode_status_resp_msg_v01), qcmap_msgr_get_station_mode_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_nat_timeout_req_msg_v01), qcmap_msgr_set_nat_timeout_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_nat_timeout_resp_msg_v01), qcmap_msgr_set_nat_timeout_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_nat_timeout_req_msg_v01), qcmap_msgr_get_nat_timeout_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_nat_timeout_resp_msg_v01), qcmap_msgr_get_nat_timeout_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_lan_config_req_msg_v01), qcmap_msgr_set_lan_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_lan_config_resp_msg_v01), qcmap_msgr_set_lan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_wlan_config_req_msg_v01), qcmap_msgr_set_wlan_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_wlan_config_resp_msg_v01), qcmap_msgr_set_wlan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wlan_status_req_msg_v01), qcmap_msgr_get_wlan_status_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_wlan_status_resp_msg_v01), qcmap_msgr_get_wlan_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_activate_wlan_req_msg_v01), qcmap_msgr_activate_wlan_req_msg_data_v01},
  {sizeof(qcmap_msgr_activate_wlan_resp_msg_v01), qcmap_msgr_activate_wlan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_activate_lan_req_msg_v01), qcmap_msgr_activate_lan_req_msg_data_v01},
  {sizeof(qcmap_msgr_activate_lan_resp_msg_v01), qcmap_msgr_activate_lan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_lan_config_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_lan_config_resp_msg_v01), qcmap_msgr_get_lan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wlan_config_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_wlan_config_resp_msg_v01), qcmap_msgr_get_wlan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_activate_hostapd_config_req_msg_v01), qcmap_msgr_activate_hostapd_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_activate_hostapd_config_resp_msg_v01), qcmap_msgr_activate_hostapd_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_activate_supplicant_config_req_msg_v01), qcmap_msgr_activate_supplicant_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_activate_supplicant_config_resp_msg_v01), qcmap_msgr_activate_supplicant_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_usb_link_up_req_msg_v01), qcmap_msgr_usb_link_up_req_msg_data_v01},
  {sizeof(qcmap_msgr_usb_link_up_resp_msg_v01), qcmap_msgr_usb_link_up_resp_msg_data_v01},
  {sizeof(qcmap_msgr_usb_link_down_req_msg_v01), qcmap_msgr_usb_link_down_req_msg_data_v01},
  {sizeof(qcmap_msgr_usb_link_down_resp_msg_v01), qcmap_msgr_usb_link_down_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_wwan_policy_req_msg_v01), qcmap_msgr_set_wwan_policy_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_wwan_policy_resp_msg_v01), qcmap_msgr_set_wwan_policy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_policy_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_wwan_policy_resp_msg_v01), qcmap_msgr_get_wwan_policy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_upnp_req_msg_v01), 0},
  {sizeof(qcmap_msgr_enable_upnp_resp_msg_v01), qcmap_msgr_enable_upnp_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_upnp_req_msg_v01), 0},
  {sizeof(qcmap_msgr_disable_upnp_resp_msg_v01), qcmap_msgr_disable_upnp_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_dlna_req_msg_v01), 0},
  {sizeof(qcmap_msgr_enable_dlna_resp_msg_v01), qcmap_msgr_enable_dlna_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_dlna_req_msg_v01), 0},
  {sizeof(qcmap_msgr_disable_dlna_resp_msg_v01), qcmap_msgr_disable_dlna_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_firewall_config_req_msg_v01), qcmap_msgr_set_firewall_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_firewall_config_resp_msg_v01), qcmap_msgr_set_firewall_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_config_req_msg_v01), qcmap_msgr_get_firewall_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_config_resp_msg_v01), qcmap_msgr_get_firewall_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_multicast_dns_req_msg_v01), 0},
  {sizeof(qcmap_msgr_enable_multicast_dns_resp_msg_v01), qcmap_msgr_enable_multicast_dns_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_multicast_dns_req_msg_v01), 0},
  {sizeof(qcmap_msgr_disable_multicast_dns_resp_msg_v01), qcmap_msgr_disable_multicast_dns_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_upnp_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_upnp_status_resp_msg_v01), qcmap_msgr_get_upnp_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_dlna_status_resp_msg_v01), qcmap_msgr_get_dlna_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_multicast_dns_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_multicast_dns_status_resp_msg_v01), qcmap_msgr_get_multicast_dns_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01), qcmap_msgr_set_qcmap_bootup_cfg_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01), qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_qcmap_bootup_cfg_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01), qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_media_dir_req_msg_v01), qcmap_msgr_set_dlna_media_dir_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_media_dir_resp_msg_v01), qcmap_msgr_set_dlna_media_dir_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_media_dir_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_dlna_media_dir_resp_msg_v01), qcmap_msgr_get_dlna_media_dir_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_data_bitrate_req_msg_v01), qcmap_msgr_get_data_bitrate_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_data_bitrate_resp_msg_v01), qcmap_msgr_get_data_bitrate_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_upnp_notify_interval_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01), qcmap_msgr_get_upnp_notify_interval_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_upnp_notify_interval_req_msg_v01), qcmap_msgr_set_upnp_notify_interval_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_upnp_notify_interval_resp_msg_v01), qcmap_msgr_set_upnp_notify_interval_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_notify_interval_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01), qcmap_msgr_get_dlna_notify_interval_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_notify_interval_req_msg_v01), qcmap_msgr_set_dlna_notify_interval_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_notify_interval_resp_msg_v01), qcmap_msgr_set_dlna_notify_interval_resp_msg_data_v01},
  {sizeof(qcmap_msgr_add_dhcp_reservation_record_req_msg_v01), qcmap_msgr_add_dhcp_reservation_record_req_msg_data_v01},
  {sizeof(qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01), qcmap_msgr_add_dhcp_reservation_record_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dhcp_reservation_records_req_msg_v01), qcmap_msgr_get_dhcp_reservation_records_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01), qcmap_msgr_get_dhcp_reservation_records_resp_msg_data_v01},
  {sizeof(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01), qcmap_msgr_edit_dhcp_reservation_record_req_msg_data_v01},
  {sizeof(qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01), qcmap_msgr_edit_dhcp_reservation_record_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01), qcmap_msgr_delete_dhcp_reservation_record_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01), qcmap_msgr_delete_dhcp_reservation_record_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_alg_req_msg_v01), qcmap_msgr_enable_alg_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_alg_resp_msg_v01), qcmap_msgr_enable_alg_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_alg_req_msg_v01), qcmap_msgr_disable_alg_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_alg_resp_msg_v01), qcmap_msgr_disable_alg_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_webserver_wwan_access_req_msg_v01), qcmap_msgr_set_webserver_wwan_access_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_webserver_wwan_access_resp_msg_v01), qcmap_msgr_set_webserver_wwan_access_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_webserver_wwan_access_req_msg_v01), qcmap_msgr_get_webserver_wwan_access_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01), qcmap_msgr_get_webserver_wwan_access_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_sip_server_info_req_msg_v01), qcmap_msgr_set_sip_server_info_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_sip_server_info_resp_msg_v01), qcmap_msgr_set_sip_server_info_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_sip_server_info_req_msg_v01), qcmap_msgr_get_sip_server_info_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_sip_server_info_resp_msg_v01), qcmap_msgr_get_sip_server_info_resp_msg_data_v01},
  {sizeof(qcmap_msgr_restore_factory_config_req_msg_v01), qcmap_msgr_restore_factory_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_restore_factory_config_resp_msg_v01), qcmap_msgr_restore_factory_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_odu_mode_req_msg_v01), qcmap_msgr_set_odu_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_odu_mode_resp_msg_v01), qcmap_msgr_set_odu_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_odu_mode_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_odu_mode_resp_msg_v01), qcmap_msgr_get_odu_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_odu_pswd_req_msg_v01), qcmap_msgr_set_odu_pswd_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_odu_pswd_resp_msg_v01), qcmap_msgr_set_odu_pswd_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_connected_devices_info_req_msg_v01), qcmap_msgr_get_connected_devices_info_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_connected_devices_info_resp_msg_v01), qcmap_msgr_get_connected_devices_info_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipv6_sip_server_info_req_msg_v01), qcmap_msgr_get_ipv6_sip_server_info_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01), qcmap_msgr_get_ipv6_sip_server_info_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_supplicant_config_req_msg_v01), qcmap_msgr_set_supplicant_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_supplicant_config_resp_msg_v01), qcmap_msgr_set_supplicant_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_cradle_mode_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_cradle_mode_resp_msg_v01), qcmap_msgr_get_cradle_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_cradle_mode_req_msg_v01), qcmap_msgr_set_cradle_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_cradle_mode_resp_msg_v01), qcmap_msgr_set_cradle_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_cradle_mode_status_ind_msg_v01), qcmap_msgr_cradle_mode_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_get_prefix_delegation_config_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01), qcmap_msgr_get_prefix_delegation_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_prefix_delegation_config_req_msg_v01), qcmap_msgr_set_prefix_delegation_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01), qcmap_msgr_set_prefix_delegation_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_prefix_delegation_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01), qcmap_msgr_get_prefix_delegation_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_gateway_url_req_msg_v01), qcmap_msgr_set_gateway_url_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_gateway_url_resp_msg_v01), qcmap_msgr_set_gateway_url_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_gateway_url_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_gateway_url_resp_msg_v01), qcmap_msgr_get_gateway_url_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_dynamic_dns_req_msg_v01), qcmap_msgr_enable_dynamic_dns_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_dynamic_dns_resp_msg_v01), qcmap_msgr_enable_dynamic_dns_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_dynamic_dns_req_msg_v01), qcmap_msgr_disable_dynamic_dns_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_dynamic_dns_resp_msg_v01), qcmap_msgr_disable_dynamic_dns_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dynamic_dns_config_req_msg_v01), qcmap_msgr_set_dynamic_dns_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01), qcmap_msgr_set_dynamic_dns_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dynamic_dns_config_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01), qcmap_msgr_get_dynamic_dns_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_tiny_proxy_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01), qcmap_msgr_get_tiny_proxy_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_tiny_proxy_req_msg_v01), qcmap_msgr_enable_tiny_proxy_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_tiny_proxy_resp_msg_v01), qcmap_msgr_enable_tiny_proxy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_tiny_proxy_req_msg_v01), qcmap_msgr_disable_tiny_proxy_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_tiny_proxy_resp_msg_v01), qcmap_msgr_disable_tiny_proxy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_whitelisting_req_msg_v01), qcmap_msgr_set_dlna_whitelisting_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_whitelisting_resp_msg_v01), qcmap_msgr_set_dlna_whitelisting_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_whitelisting_req_msg_v01), qcmap_msgr_get_dlna_whitelisting_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01), qcmap_msgr_get_dlna_whitelisting_resp_msg_data_v01},
  {sizeof(qcmap_msgr_add_dlna_whitelistip_req_msg_v01), qcmap_msgr_add_dlna_whitelistip_req_msg_data_v01},
  {sizeof(qcmap_msgr_add_dlna_whitelistip_resp_msg_v01), qcmap_msgr_add_dlna_whitelistip_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01), qcmap_msgr_delete_dlna_whitelist_ip_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_v01), qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_sta_mode_req_msg_v01), qcmap_msgr_enable_sta_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_sta_mode_resp_msg_v01), qcmap_msgr_enable_sta_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_sta_mode_req_msg_v01), qcmap_msgr_disable_sta_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_sta_mode_resp_msg_v01), qcmap_msgr_disable_sta_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_wlan_status_ind_register_req_msg_v01), qcmap_msgr_wlan_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_wlan_status_ind_register_resp_msg_v01), qcmap_msgr_wlan_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_wlan_status_ind_msg_v01), qcmap_msgr_wlan_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_set_backhaul_pref_req_msg_v01), qcmap_msgr_set_backhaul_pref_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_backhaul_pref_resp_msg_v01), qcmap_msgr_set_backhaul_pref_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_backhaul_pref_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_backhaul_pref_resp_msg_v01), qcmap_msgr_get_backhaul_pref_resp_msg_data_v01},
  {sizeof(qcmap_msgr_cradle_mode_status_ind_register_req_msg_v01), qcmap_msgr_cradle_mode_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01), qcmap_msgr_cradle_mode_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_eth_backhaul_mode_status_ind_register_req_msg_v01), qcmap_msgr_eth_backhaul_mode_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_eth_backhaul_mode_status_ind_register_resp_msg_v01), qcmap_msgr_eth_backhaul_mode_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_eth_backhaul_mode_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_eth_backhaul_mode_resp_msg_v01), qcmap_msgr_get_eth_backhaul_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_eth_backhaul_mode_req_msg_v01), qcmap_msgr_set_eth_backhaul_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_eth_backhaul_mode_resp_msg_v01), qcmap_msgr_set_eth_backhaul_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_eth_backhaul_mode_status_ind_msg_v01), qcmap_msgr_eth_backhaul_mode_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01), qcmap_msgr_set_ip_passthrough_flag_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_ip_passthrough_flag_resp_msg_v01), qcmap_msgr_set_ip_passthrough_flag_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ip_passthrough_flag_req_msg_v01), qcmap_msgr_get_ip_passthrough_flag_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01), qcmap_msgr_get_ip_passthrough_flag_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ip_passthrough_state_req_msg_v01), qcmap_msgr_get_ip_passthrough_state_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_ip_passthrough_state_resp_msg_v01), qcmap_msgr_get_ip_passthrough_state_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_bt_tethering_req_msg_v01), qcmap_msgr_bring_up_bt_tethering_req_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01), qcmap_msgr_bring_up_bt_tethering_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bring_down_bt_tethering_req_msg_v01), qcmap_msgr_bring_down_bt_tethering_req_msg_data_v01},
  {sizeof(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01), qcmap_msgr_bring_down_bt_tethering_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_bt_tethering_status_req_msg_v01), qcmap_msgr_get_bt_tethering_status_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_bt_tethering_status_resp_msg_v01), qcmap_msgr_get_bt_tethering_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bt_tethering_status_ind_register_req_msg_v01), qcmap_msgr_bt_tethering_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_bt_tethering_status_ind_msg_v01), qcmap_msgr_bt_tethering_status_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object qcmap_msgr_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *qcmap_msgr_qmi_idl_type_table_object_referenced_tables_v01[] =
{&qcmap_msgr_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object qcmap_msgr_qmi_idl_type_table_object_v01 = {
  sizeof(qcmap_msgr_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(qcmap_msgr_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  qcmap_msgr_type_table_v01,
  qcmap_msgr_message_table_v01,
  qcmap_msgr_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry qcmap_msgr_service_command_messages_v01[] = {
  {QMI_QCMAP_MSGR_MOBILE_AP_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_QCMAP_MSGR_MOBILE_AP_DISABLE_REQ_V01, QMI_IDL_TYPE16(0, 2), 7},
  {QMI_QCMAP_MSGR_BRING_UP_WWAN_REQ_V01, QMI_IDL_TYPE16(0, 16), 14},
  {QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_REQ_V01, QMI_IDL_TYPE16(0, 19), 14},
  {QMI_QCMAP_MSGR_WWAN_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 22), 4},
  {QMI_QCMAP_MSGR_ENABLE_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 31), 11},
  {QMI_QCMAP_MSGR_DISABLE_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 33), 11},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 25), 4},
  {QMI_QCMAP_MSGR_ADD_STATIC_NAT_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 35), 19},
  {QMI_QCMAP_MSGR_DELETE_STATIC_NAT_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 37), 19},
  {QMI_QCMAP_MSGR_GET_STATIC_NAT_ENTRIES_REQ_V01, QMI_IDL_TYPE16(0, 39), 7},
  {QMI_QCMAP_MSGR_SET_DMZ_REQ_V01, QMI_IDL_TYPE16(0, 41), 14},
  {QMI_QCMAP_MSGR_DELETE_DMZ_REQ_V01, QMI_IDL_TYPE16(0, 45), 7},
  {QMI_QCMAP_MSGR_GET_DMZ_REQ_V01, QMI_IDL_TYPE16(0, 43), 7},
  {QMI_QCMAP_MSGR_GET_WWAN_STATS_REQ_V01, QMI_IDL_TYPE16(0, 47), 14},
  {QMI_QCMAP_MSGR_RESET_WWAN_STATS_REQ_V01, QMI_IDL_TYPE16(0, 49), 14},
  {QMI_QCMAP_MSGR_GET_IPSEC_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 53), 7},
  {QMI_QCMAP_MSGR_SET_IPSEC_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 51), 11},
  {QMI_QCMAP_MSGR_GET_PPTP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 57), 7},
  {QMI_QCMAP_MSGR_SET_PPTP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 55), 11},
  {QMI_QCMAP_MSGR_GET_L2TP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 61), 7},
  {QMI_QCMAP_MSGR_SET_L2TP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 59), 11},
  {QMI_QCMAP_MSGR_SET_NAT_TYPE_REQ_V01, QMI_IDL_TYPE16(0, 63), 14},
  {QMI_QCMAP_MSGR_GET_NAT_TYPE_REQ_V01, QMI_IDL_TYPE16(0, 65), 7},
  {QMI_QCMAP_MSGR_SET_AUTO_CONNECT_REQ_V01, QMI_IDL_TYPE16(0, 67), 11},
  {QMI_QCMAP_MSGR_GET_AUTO_CONNECT_REQ_V01, QMI_IDL_TYPE16(0, 69), 7},
  {QMI_QCMAP_MSGR_ADD_FIREWALL_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 79), 123},
  {QMI_QCMAP_MSGR_GET_FIREWALL_ENTRIES_HANDLE_LIST_REQ_V01, QMI_IDL_TYPE16(0, 81), 14},
  {QMI_QCMAP_MSGR_GET_FIREWALL_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 83), 14},
  {QMI_QCMAP_MSGR_DELETE_FIREWALL_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 85), 14},
  {QMI_QCMAP_MSGR_GET_WWAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 87), 18},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 89), 7},
  {QMI_QCMAP_MSGR_WWAN_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 91), 14},
  {QMI_QCMAP_MSGR_SET_NAT_TIMEOUT_REQ_V01, QMI_IDL_TYPE16(0, 95), 21},
  {QMI_QCMAP_MSGR_GET_NAT_TIMEOUT_REQ_V01, QMI_IDL_TYPE16(0, 97), 14},
  {QMI_QCMAP_MSGR_SET_LAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 99), 24},
  {QMI_QCMAP_MSGR_GET_WLAN_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 103), 7},
  {QMI_QCMAP_MSGR_ACTIVATE_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 105), 7},
  {QMI_QCMAP_MSGR_GET_LAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 109), 0},
  {QMI_QCMAP_MSGR_USB_LINK_UP_REQ_V01, QMI_IDL_TYPE16(0, 117), 14},
  {QMI_QCMAP_MSGR_USB_LINK_DOWN_REQ_V01, QMI_IDL_TYPE16(0, 119), 14},
  {QMI_QCMAP_MSGR_ENABLE_IPV6_REQ_V01, QMI_IDL_TYPE16(0, 10), 7},
  {QMI_QCMAP_MSGR_DISABLE_IPV6_REQ_V01, QMI_IDL_TYPE16(0, 12), 7},
  {QMI_QCMAP_MSGR_SET_ROAMING_PREF_REQ_V01, QMI_IDL_TYPE16(0, 71), 11},
  {QMI_QCMAP_MSGR_GET_ROAMING_PREF_REQ_V01, QMI_IDL_TYPE16(0, 73), 7},
  {QMI_QCMAP_MSGR_SET_WWAN_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 121), 26},
  {QMI_QCMAP_MSGR_GET_WWAN_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 123), 0},
  {QMI_QCMAP_MSGR_GET_IPV6_STATE_REQ_V01, QMI_IDL_TYPE16(0, 14), 0},
  {QMI_QCMAP_MSGR_ENABLE_UPNP_REQ_V01, QMI_IDL_TYPE16(0, 125), 0},
  {QMI_QCMAP_MSGR_DISABLE_UPNP_REQ_V01, QMI_IDL_TYPE16(0, 127), 0},
  {QMI_QCMAP_MSGR_ENABLE_DLNA_REQ_V01, QMI_IDL_TYPE16(0, 129), 0},
  {QMI_QCMAP_MSGR_DISABLE_DLNA_REQ_V01, QMI_IDL_TYPE16(0, 131), 0},
  {QMI_QCMAP_MSGR_SET_FIREWALL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 133), 19},
  {QMI_QCMAP_MSGR_GET_FIREWALL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 135), 7},
  {QMI_QCMAP_MSGR_ENABLE_MULTICAST_DNS_RESPONDER_REQ_V01, QMI_IDL_TYPE16(0, 137), 0},
  {QMI_QCMAP_MSGR_DISABLE_MULTICAST_DNS_RESPONDER_REQ_V01, QMI_IDL_TYPE16(0, 139), 0},
  {QMI_QCMAP_MSGR_GET_UPNP_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 141), 0},
  {QMI_QCMAP_MSGR_GET_DLNA_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 143), 0},
  {QMI_QCMAP_MSGR_GET_MULTICAST_DNS_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 145), 0},
  {QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 28), 4},
  {QMI_QCMAP_MSGR_GET_STATION_MODE_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 93), 0},
  {QMI_QCMAP_MSGR_SET_QCMAP_BOOTUP_CFG_REQ_V01, QMI_IDL_TYPE16(0, 147), 14},
  {QMI_QCMAP_MSGR_GET_QCMAP_BOOTUP_CFG_REQ_V01, QMI_IDL_TYPE16(0, 149), 0},
  {QMI_QCMAP_MSGR_SET_DLNA_MEDIA_DIR_REQ_V01, QMI_IDL_TYPE16(0, 151), 505},
  {QMI_QCMAP_MSGR_GET_DLNA_MEDIA_DIR_REQ_V01, QMI_IDL_TYPE16(0, 153), 0},
  {QMI_QCMAP_MSGR_SET_WLAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 101), 45},
  {QMI_QCMAP_MSGR_ACTIVATE_LAN_REQ_V01, QMI_IDL_TYPE16(0, 107), 7},
  {QMI_QCMAP_MSGR_GET_WLAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 111), 0},
  {QMI_QCMAP_MSGR_ENABLE_IPV4_REQ_V01, QMI_IDL_TYPE16(0, 4), 7},
  {QMI_QCMAP_MSGR_DISABLE_IPV4_REQ_V01, QMI_IDL_TYPE16(0, 6), 7},
  {QMI_QCMAP_MSGR_GET_IPV4_STATE_REQ_V01, QMI_IDL_TYPE16(0, 8), 0},
  {QMI_QCMAP_MSGR_GET_DATA_BITRATE_REQ_V01, QMI_IDL_TYPE16(0, 155), 7},
  {QMI_QCMAP_MSGR_GET_UPNP_NOTIFY_INTERVAL_REQ_V01, QMI_IDL_TYPE16(0, 157), 0},
  {QMI_QCMAP_MSGR_SET_UPNP_NOTIFY_INTERVAL_REQ_V01, QMI_IDL_TYPE16(0, 159), 7},
  {QMI_QCMAP_MSGR_GET_DLNA_NOTIFY_INTERVAL_REQ_V01, QMI_IDL_TYPE16(0, 161), 0},
  {QMI_QCMAP_MSGR_SET_DLNA_NOTIFY_INTERVAL_REQ_V01, QMI_IDL_TYPE16(0, 163), 7},
  {QMI_QCMAP_MSGR_ADD_DHCP_RESERVATION_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 165), 121},
  {QMI_QCMAP_MSGR_GET_DHCP_RESERVATION_RECORDS_REQ_V01, QMI_IDL_TYPE16(0, 167), 7},
  {QMI_QCMAP_MSGR_EDIT_DHCP_RESERVATION_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 169), 128},
  {QMI_QCMAP_MSGR_DELETE_DHCP_RESERVATION_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 171), 14},
  {QMI_QCMAP_MSGR_ACTIVATE_HOSTAPD_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 113), 25},
  {QMI_QCMAP_MSGR_ACTIVATE_SUPPLICANT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 115), 7},
  {QMI_QCMAP_MSGR_ENABLE_ALG_REQ_V01, QMI_IDL_TYPE16(0, 173), 18},
  {QMI_QCMAP_MSGR_DISABLE_ALG_REQ_V01, QMI_IDL_TYPE16(0, 175), 18},
  {QMI_QCMAP_MSGR_GET_WEBSERVER_WWAN_ACCESS_REQ_V01, QMI_IDL_TYPE16(0, 179), 7},
  {QMI_QCMAP_MSGR_SET_WEBSERVER_WWAN_ACCESS_REQ_V01, QMI_IDL_TYPE16(0, 177), 11},
  {QMI_QCMAP_MSGR_SET_SIP_SERVER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 181), 118},
  {QMI_QCMAP_MSGR_GET_SIP_SERVER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 183), 7},
  {QMI_QCMAP_MSGR_RESTORE_FACTORY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 185), 7},
  {QMI_QCMAP_MSGR_SET_ODU_MODE_REQ_V01, QMI_IDL_TYPE16(0, 187), 7},
  {QMI_QCMAP_MSGR_GET_ODU_MODE_REQ_V01, QMI_IDL_TYPE16(0, 189), 0},
  {QMI_QCMAP_MSGR_SET_ODU_PSWD_REQ_V01, QMI_IDL_TYPE16(0, 191), 18},
  {QMI_QCMAP_MSGR_GET_CONNECTED_DEVICES_INFO_REQ_V01, QMI_IDL_TYPE16(0, 193), 7},
  {QMI_QCMAP_MSGR_SET_LOOPBACK_FLAG_REQ_V01, QMI_IDL_TYPE16(0, 75), 11},
  {QMI_QCMAP_MSGR_GET_LOOPBACK_FLAG_REQ_V01, QMI_IDL_TYPE16(0, 77), 7},
  {QMI_QCMAP_MSGR_GET_IPV6_SIP_SERVER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 195), 7},
  {QMI_QCMAP_MSGR_SET_SUPPLICANT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 197), 11},
  {QMI_QCMAP_MSGR_GET_CRADLE_MODE_REQ_V01, QMI_IDL_TYPE16(0, 199), 0},
  {QMI_QCMAP_MSGR_SET_CRADLE_MODE_REQ_V01, QMI_IDL_TYPE16(0, 201), 14},
  {QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 204), 0},
  {QMI_QCMAP_MSGR_SET_PREFIX_DELEGATION_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 206), 11},
  {QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 208), 0},
  {QMI_QCMAP_MSGR_SET_GATEWAY_URL_REQ_V01, QMI_IDL_TYPE16(0, 210), 312},
  {QMI_QCMAP_MSGR_GET_GATEWAY_URL_REQ_V01, QMI_IDL_TYPE16(0, 212), 0},
  {QMI_QCMAP_MSGR_ENABLE_DYNAMIC_DNS_REQ_V01, QMI_IDL_TYPE16(0, 214), 7},
  {QMI_QCMAP_MSGR_DISABLE_DYNAMIC_DNS_REQ_V01, QMI_IDL_TYPE16(0, 216), 7},
  {QMI_QCMAP_MSGR_SET_DYNAMIC_DNS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 218), 366},
  {QMI_QCMAP_MSGR_GET_DYNAMIC_DNS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 220), 0},
  {QMI_QCMAP_MSGR_GET_TINY_PROXY_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 222), 0},
  {QMI_QCMAP_MSGR_ENABLE_TINY_PROXY_REQ_V01, QMI_IDL_TYPE16(0, 224), 7},
  {QMI_QCMAP_MSGR_DISABLE_TINY_PROXY_REQ_V01, QMI_IDL_TYPE16(0, 226), 7},
  {QMI_QCMAP_MSGR_SET_DLNA_WHITELISTING_REQ_V01, QMI_IDL_TYPE16(0, 228), 11},
  {QMI_QCMAP_MSGR_GET_DLNA_WHITELISTING_REQ_V01, QMI_IDL_TYPE16(0, 230), 7},
  {QMI_QCMAP_MSGR_ADD_DLNA_WHITELISTIP_REQ_V01, QMI_IDL_TYPE16(0, 232), 14},
  {QMI_QCMAP_MSGR_DELETE_DLNA_WHITELIST_IP_REQ_V01, QMI_IDL_TYPE16(0, 234), 14},
  {QMI_QCMAP_MSGR_ENABLE_STA_MODE_REQ_V01, QMI_IDL_TYPE16(0, 236), 7},
  {QMI_QCMAP_MSGR_WLAN_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 240), 4},
  {QMI_QCMAP_MSGR_SET_BACKHAUL_PREF_REQ_V01, QMI_IDL_TYPE16(0, 243), 35},
  {QMI_QCMAP_MSGR_GET_BACKHAUL_PREF_REQ_V01, QMI_IDL_TYPE16(0, 245), 0},
  {QMI_QCMAP_MSGR_DISABLE_STA_MODE_REQ_V01, QMI_IDL_TYPE16(0, 238), 7},
  {QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 247), 7},
  {QMI_QCMAP_MSGR_GET_ETH_BACKHAUL_MODE_REQ_V01, QMI_IDL_TYPE16(0, 251), 0},
  {QMI_QCMAP_MSGR_SET_ETH_BACKHAUL_MODE_REQ_V01, QMI_IDL_TYPE16(0, 253), 14},
  {QMI_QCMAP_MSGR_ETH_BACKHAUL_MODE_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 249), 7},
  {QMI_QCMAP_MSGR_SET_IP_PASSTHROUGH_FLAG_REQ_V01, QMI_IDL_TYPE16(0, 256), 127},
  {QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_FLAG_REQ_V01, QMI_IDL_TYPE16(0, 258), 7},
  {QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_STATE_REQ_V01, QMI_IDL_TYPE16(0, 260), 7},
  {QMI_QCMAP_MSGR_BRING_UP_BT_TETHERING_REQ_V01, QMI_IDL_TYPE16(0, 262), 7},
  {QMI_QCMAP_MSGR_BRING_DOWN_BT_TETHERING_REQ_V01, QMI_IDL_TYPE16(0, 264), 7},
  {QMI_QCMAP_MSGR_GET_BT_TETHERING_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 266), 7},
  {QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 268), 4}
};

static const qmi_idl_service_message_table_entry qcmap_msgr_service_response_messages_v01[] = {
  {QMI_QCMAP_MSGR_MOBILE_AP_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_QCMAP_MSGR_MOBILE_AP_DISABLE_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_QCMAP_MSGR_BRING_UP_WWAN_RESP_V01, QMI_IDL_TYPE16(0, 17), 14},
  {QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_RESP_V01, QMI_IDL_TYPE16(0, 20), 14},
  {QMI_QCMAP_MSGR_WWAN_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 23), 7},
  {QMI_QCMAP_MSGR_ENABLE_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 32), 7},
  {QMI_QCMAP_MSGR_DISABLE_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 34), 7},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_QCMAP_MSGR_ADD_STATIC_NAT_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 36), 7},
  {QMI_QCMAP_MSGR_DELETE_STATIC_NAT_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 38), 7},
  {QMI_QCMAP_MSGR_GET_STATIC_NAT_ENTRIES_RESP_V01, QMI_IDL_TYPE16(0, 40), 461},
  {QMI_QCMAP_MSGR_SET_DMZ_RESP_V01, QMI_IDL_TYPE16(0, 42), 7},
  {QMI_QCMAP_MSGR_DELETE_DMZ_RESP_V01, QMI_IDL_TYPE16(0, 46), 7},
  {QMI_QCMAP_MSGR_GET_DMZ_RESP_V01, QMI_IDL_TYPE16(0, 44), 14},
  {QMI_QCMAP_MSGR_GET_WWAN_STATS_RESP_V01, QMI_IDL_TYPE16(0, 48), 42},
  {QMI_QCMAP_MSGR_RESET_WWAN_STATS_RESP_V01, QMI_IDL_TYPE16(0, 50), 7},
  {QMI_QCMAP_MSGR_GET_IPSEC_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 54), 11},
  {QMI_QCMAP_MSGR_SET_IPSEC_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 52), 7},
  {QMI_QCMAP_MSGR_GET_PPTP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 58), 11},
  {QMI_QCMAP_MSGR_SET_PPTP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 56), 7},
  {QMI_QCMAP_MSGR_GET_L2TP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 62), 11},
  {QMI_QCMAP_MSGR_SET_L2TP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 60), 7},
  {QMI_QCMAP_MSGR_SET_NAT_TYPE_RESP_V01, QMI_IDL_TYPE16(0, 64), 7},
  {QMI_QCMAP_MSGR_GET_NAT_TYPE_RESP_V01, QMI_IDL_TYPE16(0, 66), 14},
  {QMI_QCMAP_MSGR_SET_AUTO_CONNECT_RESP_V01, QMI_IDL_TYPE16(0, 68), 7},
  {QMI_QCMAP_MSGR_GET_AUTO_CONNECT_RESP_V01, QMI_IDL_TYPE16(0, 70), 11},
  {QMI_QCMAP_MSGR_ADD_FIREWALL_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 80), 14},
  {QMI_QCMAP_MSGR_GET_FIREWALL_ENTRIES_HANDLE_LIST_RESP_V01, QMI_IDL_TYPE16(0, 82), 211},
  {QMI_QCMAP_MSGR_GET_FIREWALL_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 84), 119},
  {QMI_QCMAP_MSGR_DELETE_FIREWALL_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 86), 7},
  {QMI_QCMAP_MSGR_GET_WWAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 88), 85},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 90), 14},
  {QMI_QCMAP_MSGR_WWAN_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 92), 14},
  {QMI_QCMAP_MSGR_SET_NAT_TIMEOUT_RESP_V01, QMI_IDL_TYPE16(0, 96), 7},
  {QMI_QCMAP_MSGR_GET_NAT_TIMEOUT_RESP_V01, QMI_IDL_TYPE16(0, 98), 14},
  {QMI_QCMAP_MSGR_SET_LAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 100), 7},
  {QMI_QCMAP_MSGR_GET_WLAN_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 104), 14},
  {QMI_QCMAP_MSGR_ACTIVATE_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 106), 7},
  {QMI_QCMAP_MSGR_GET_LAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 110), 70},
  {QMI_QCMAP_MSGR_USB_LINK_UP_RESP_V01, QMI_IDL_TYPE16(0, 118), 7},
  {QMI_QCMAP_MSGR_USB_LINK_DOWN_RESP_V01, QMI_IDL_TYPE16(0, 120), 7},
  {QMI_QCMAP_MSGR_ENABLE_IPV6_RESP_V01, QMI_IDL_TYPE16(0, 11), 7},
  {QMI_QCMAP_MSGR_DISABLE_IPV6_RESP_V01, QMI_IDL_TYPE16(0, 13), 7},
  {QMI_QCMAP_MSGR_SET_ROAMING_PREF_RESP_V01, QMI_IDL_TYPE16(0, 72), 7},
  {QMI_QCMAP_MSGR_GET_ROAMING_PREF_RESP_V01, QMI_IDL_TYPE16(0, 74), 11},
  {QMI_QCMAP_MSGR_SET_WWAN_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 122), 7},
  {QMI_QCMAP_MSGR_GET_WWAN_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 124), 26},
  {QMI_QCMAP_MSGR_GET_IPV6_STATE_RESP_V01, QMI_IDL_TYPE16(0, 15), 11},
  {QMI_QCMAP_MSGR_ENABLE_UPNP_RESP_V01, QMI_IDL_TYPE16(0, 126), 7},
  {QMI_QCMAP_MSGR_DISABLE_UPNP_RESP_V01, QMI_IDL_TYPE16(0, 128), 7},
  {QMI_QCMAP_MSGR_ENABLE_DLNA_RESP_V01, QMI_IDL_TYPE16(0, 130), 7},
  {QMI_QCMAP_MSGR_DISABLE_DLNA_RESP_V01, QMI_IDL_TYPE16(0, 132), 7},
  {QMI_QCMAP_MSGR_SET_FIREWALL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 134), 7},
  {QMI_QCMAP_MSGR_GET_FIREWALL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 136), 19},
  {QMI_QCMAP_MSGR_ENABLE_MULTICAST_DNS_RESPONDER_RESP_V01, QMI_IDL_TYPE16(0, 138), 7},
  {QMI_QCMAP_MSGR_DISABLE_MULTICAST_DNS_RESPONDER_RESP_V01, QMI_IDL_TYPE16(0, 140), 7},
  {QMI_QCMAP_MSGR_GET_UPNP_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 142), 14},
  {QMI_QCMAP_MSGR_GET_DLNA_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 144), 14},
  {QMI_QCMAP_MSGR_GET_MULTICAST_DNS_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 146), 14},
  {QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 29), 7},
  {QMI_QCMAP_MSGR_GET_STATION_MODE_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 94), 14},
  {QMI_QCMAP_MSGR_SET_QCMAP_BOOTUP_CFG_RESP_V01, QMI_IDL_TYPE16(0, 148), 7},
  {QMI_QCMAP_MSGR_GET_QCMAP_BOOTUP_CFG_RESP_V01, QMI_IDL_TYPE16(0, 150), 21},
  {QMI_QCMAP_MSGR_SET_DLNA_MEDIA_DIR_RESP_V01, QMI_IDL_TYPE16(0, 152), 7},
  {QMI_QCMAP_MSGR_GET_DLNA_MEDIA_DIR_RESP_V01, QMI_IDL_TYPE16(0, 154), 512},
  {QMI_QCMAP_MSGR_SET_WLAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 102), 7},
  {QMI_QCMAP_MSGR_ACTIVATE_LAN_RESP_V01, QMI_IDL_TYPE16(0, 108), 7},
  {QMI_QCMAP_MSGR_GET_WLAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 112), 45},
  {QMI_QCMAP_MSGR_ENABLE_IPV4_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_QCMAP_MSGR_DISABLE_IPV4_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_QCMAP_MSGR_GET_IPV4_STATE_RESP_V01, QMI_IDL_TYPE16(0, 9), 11},
  {QMI_QCMAP_MSGR_GET_DATA_BITRATE_RESP_V01, QMI_IDL_TYPE16(0, 156), 42},
  {QMI_QCMAP_MSGR_GET_UPNP_NOTIFY_INTERVAL_RESP_V01, QMI_IDL_TYPE16(0, 158), 14},
  {QMI_QCMAP_MSGR_SET_UPNP_NOTIFY_INTERVAL_RESP_V01, QMI_IDL_TYPE16(0, 160), 7},
  {QMI_QCMAP_MSGR_GET_DLNA_NOTIFY_INTERVAL_RESP_V01, QMI_IDL_TYPE16(0, 162), 14},
  {QMI_QCMAP_MSGR_SET_DLNA_NOTIFY_INTERVAL_RESP_V01, QMI_IDL_TYPE16(0, 164), 7},
  {QMI_QCMAP_MSGR_ADD_DHCP_RESERVATION_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 166), 7},
  {QMI_QCMAP_MSGR_GET_DHCP_RESERVATION_RECORDS_RESP_V01, QMI_IDL_TYPE16(0, 168), 2231},
  {QMI_QCMAP_MSGR_EDIT_DHCP_RESERVATION_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 170), 7},
  {QMI_QCMAP_MSGR_DELETE_DHCP_RESERVATION_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 172), 7},
  {QMI_QCMAP_MSGR_ACTIVATE_HOSTAPD_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 114), 7},
  {QMI_QCMAP_MSGR_ACTIVATE_SUPPLICANT_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 116), 7},
  {QMI_QCMAP_MSGR_ENABLE_ALG_RESP_V01, QMI_IDL_TYPE16(0, 174), 7},
  {QMI_QCMAP_MSGR_DISABLE_ALG_RESP_V01, QMI_IDL_TYPE16(0, 176), 7},
  {QMI_QCMAP_MSGR_GET_WEBSERVER_WWAN_ACCESS_RESP_V01, QMI_IDL_TYPE16(0, 180), 11},
  {QMI_QCMAP_MSGR_SET_WEBSERVER_WWAN_ACCESS_RESP_V01, QMI_IDL_TYPE16(0, 178), 7},
  {QMI_QCMAP_MSGR_SET_SIP_SERVER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 182), 7},
  {QMI_QCMAP_MSGR_GET_SIP_SERVER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 184), 2822},
  {QMI_QCMAP_MSGR_RESTORE_FACTORY_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 186), 7},
  {QMI_QCMAP_MSGR_SET_ODU_MODE_RESP_V01, QMI_IDL_TYPE16(0, 188), 7},
  {QMI_QCMAP_MSGR_GET_ODU_MODE_RESP_V01, QMI_IDL_TYPE16(0, 190), 14},
  {QMI_QCMAP_MSGR_SET_ODU_PSWD_RESP_V01, QMI_IDL_TYPE16(0, 192), 7},
  {QMI_QCMAP_MSGR_GET_CONNECTED_DEVICES_INFO_RESP_V01, QMI_IDL_TYPE16(0, 194), 6011},
  {QMI_QCMAP_MSGR_SET_LOOPBACK_FLAG_RESP_V01, QMI_IDL_TYPE16(0, 76), 7},
  {QMI_QCMAP_MSGR_GET_LOOPBACK_FLAG_RESP_V01, QMI_IDL_TYPE16(0, 78), 11},
  {QMI_QCMAP_MSGR_GET_IPV6_SIP_SERVER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 196), 3011},
  {QMI_QCMAP_MSGR_SET_SUPPLICANT_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 198), 7},
  {QMI_QCMAP_MSGR_GET_CRADLE_MODE_RESP_V01, QMI_IDL_TYPE16(0, 200), 14},
  {QMI_QCMAP_MSGR_SET_CRADLE_MODE_RESP_V01, QMI_IDL_TYPE16(0, 202), 7},
  {QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 205), 11},
  {QMI_QCMAP_MSGR_SET_PREFIX_DELEGATION_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 207), 7},
  {QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 209), 11},
  {QMI_QCMAP_MSGR_SET_GATEWAY_URL_RESP_V01, QMI_IDL_TYPE16(0, 211), 7},
  {QMI_QCMAP_MSGR_GET_GATEWAY_URL_RESP_V01, QMI_IDL_TYPE16(0, 213), 312},
  {QMI_QCMAP_MSGR_ENABLE_DYNAMIC_DNS_RESP_V01, QMI_IDL_TYPE16(0, 215), 7},
  {QMI_QCMAP_MSGR_DISABLE_DYNAMIC_DNS_RESP_V01, QMI_IDL_TYPE16(0, 217), 7},
  {QMI_QCMAP_MSGR_SET_DYNAMIC_DNS_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 219), 7},
  {QMI_QCMAP_MSGR_GET_DYNAMIC_DNS_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 221), 2125},
  {QMI_QCMAP_MSGR_GET_TINY_PROXY_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 223), 14},
  {QMI_QCMAP_MSGR_ENABLE_TINY_PROXY_RESP_V01, QMI_IDL_TYPE16(0, 225), 7},
  {QMI_QCMAP_MSGR_DISABLE_TINY_PROXY_RESP_V01, QMI_IDL_TYPE16(0, 227), 7},
  {QMI_QCMAP_MSGR_SET_DLNA_WHITELISTING_RESP_V01, QMI_IDL_TYPE16(0, 229), 7},
  {QMI_QCMAP_MSGR_GET_DLNA_WHITELISTING_RESP_V01, QMI_IDL_TYPE16(0, 231), 11},
  {QMI_QCMAP_MSGR_ADD_DLNA_WHITELISTIP_RESP_V01, QMI_IDL_TYPE16(0, 233), 7},
  {QMI_QCMAP_MSGR_DELETE_DLNA_WHITELIST_IP_RESP_V01, QMI_IDL_TYPE16(0, 235), 7},
  {QMI_QCMAP_MSGR_ENABLE_STA_MODE_RESP_V01, QMI_IDL_TYPE16(0, 237), 7},
  {QMI_QCMAP_MSGR_WLAN_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 241), 7},
  {QMI_QCMAP_MSGR_SET_BACKHAUL_PREF_RESP_V01, QMI_IDL_TYPE16(0, 244), 7},
  {QMI_QCMAP_MSGR_GET_BACKHAUL_PREF_RESP_V01, QMI_IDL_TYPE16(0, 246), 35},
  {QMI_QCMAP_MSGR_DISABLE_STA_MODE_RESP_V01, QMI_IDL_TYPE16(0, 239), 7},
  {QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 248), 7},
  {QMI_QCMAP_MSGR_GET_ETH_BACKHAUL_MODE_RESP_V01, QMI_IDL_TYPE16(0, 252), 14},
  {QMI_QCMAP_MSGR_SET_ETH_BACKHAUL_MODE_RESP_V01, QMI_IDL_TYPE16(0, 254), 7},
  {QMI_QCMAP_MSGR_ETH_BACKHAUL_MODE_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 250), 7},
  {QMI_QCMAP_MSGR_SET_IP_PASSTHROUGH_FLAG_RESP_V01, QMI_IDL_TYPE16(0, 257), 7},
  {QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_FLAG_RESP_V01, QMI_IDL_TYPE16(0, 259), 127},
  {QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_STATE_RESP_V01, QMI_IDL_TYPE16(0, 261), 11},
  {QMI_QCMAP_MSGR_BRING_UP_BT_TETHERING_RESP_V01, QMI_IDL_TYPE16(0, 263), 7},
  {QMI_QCMAP_MSGR_BRING_DOWN_BT_TETHERING_RESP_V01, QMI_IDL_TYPE16(0, 265), 7},
  {QMI_QCMAP_MSGR_GET_BT_TETHERING_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 267), 11},
  {QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 26), 7}
};

static const qmi_idl_service_message_table_entry qcmap_msgr_service_indication_messages_v01[] = {
  {QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01, QMI_IDL_TYPE16(0, 18), 25},
  {QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01, QMI_IDL_TYPE16(0, 21), 25},
  {QMI_QCMAP_MSGR_WWAN_STATUS_IND_V01, QMI_IDL_TYPE16(0, 24), 25},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_V01, QMI_IDL_TYPE16(0, 27), 14},
  {QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_V01, QMI_IDL_TYPE16(0, 30), 14},
  {QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND_V01, QMI_IDL_TYPE16(0, 203), 14},
  {QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01, QMI_IDL_TYPE16(0, 242), 414},
  {QMI_QCMAP_MSGR_ETH_BACKHAUL_MODE_STATUS_IND_V01, QMI_IDL_TYPE16(0, 255), 14},
  {QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_V01, QMI_IDL_TYPE16(0, 269), 14}
};

/*Service Object*/
struct qmi_idl_service_object qcmap_msgr_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x0400,
  6011,
  { sizeof(qcmap_msgr_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qcmap_msgr_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qcmap_msgr_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { qcmap_msgr_service_command_messages_v01, qcmap_msgr_service_response_messages_v01, qcmap_msgr_service_indication_messages_v01},
  &qcmap_msgr_qmi_idl_type_table_object_v01,
  0x27,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type qcmap_msgr_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( QCMAP_MSGR_V01_IDL_MAJOR_VERS != idl_maj_version || QCMAP_MSGR_V01_IDL_MINOR_VERS != idl_min_version
       || QCMAP_MSGR_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&qcmap_msgr_qmi_idl_service_object_v01;
}

