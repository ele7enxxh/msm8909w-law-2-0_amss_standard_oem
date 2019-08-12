/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              W I R E L E S S _ M E S S A G I N G _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the wms service Data structures.

    Copyright (c) 2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/qmi_wms/src/wireless_messaging_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.2
   It was generated on: Thu Apr 26 2012
   From IDL File: wireless_messaging_service_v01.idl */

#include "qmi_si.h"
#include "wireless_messaging_service_v01.h"

static const qmi_implemented_optionals_tuple wms_reset_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_set_event_report_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* report_mt_message */
};

static const qmi_implemented_optionals wms_set_event_report_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_set_event_report_req_msg_impl_array_v01
};
static const uint32_t wms_event_report_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* mt_message */
    QMI_SI_TLV_SET(0x11)   |  /* transfer_route_mt_message */
    QMI_SI_TLV_SET(0x12)   |  /* message_mode */
    QMI_SI_TLV_SET(0x13)   |  /* etws_message */
    QMI_SI_TLV_SET(0x14)   |  /* etws_plmn_info */
    QMI_SI_TLV_SET(0x15)   |  /* mt_message_smsc_address */
    QMI_SI_TLV_SET(0x16)      /* sms_on_ims */
};

static const qmi_implemented_optionals wms_event_report_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_event_report_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_set_event_report_reqresp_msg_impl_tuple_v01 = 
{
  &wms_set_event_report_req_msg_impl_v01,
  NULL,
  &wms_event_report_ind_msg_impl_v01
};

static const uint32_t wms_raw_send_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* force_on_dc */
    QMI_SI_TLV_SET(0x11)   |  /* follow_on_dc */
    QMI_SI_TLV_SET(0x12)   |  /* link_timer */
    QMI_SI_TLV_SET(0x13)   |  /* sms_on_ims */
    QMI_SI_TLV_SET(0x14)   |  /* retry_message */
    QMI_SI_TLV_SET(0x15)      /* retry_message_id */
};

static const qmi_implemented_optionals wms_raw_send_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_raw_send_req_msg_impl_array_v01
};
static const uint32_t wms_raw_send_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cause_code */
    QMI_SI_TLV_SET(0x11)   |  /* error_class */
    QMI_SI_TLV_SET(0x12)   |  /* gw_cause_info */
    QMI_SI_TLV_SET(0x13)   |  /* message_delivery_failure_type */
    QMI_SI_TLV_SET(0x14)   |  /* message_delivery_failure_cause */
    QMI_SI_TLV_SET(0x15)      /* call_control_modified_info */
};

static const qmi_implemented_optionals wms_raw_send_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_raw_send_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_raw_send_reqresp_msg_impl_tuple_v01 = 
{
  &wms_raw_send_req_msg_impl_v01,
  &wms_raw_send_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple wms_raw_write_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_raw_read_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* message_mode */
    QMI_SI_TLV_SET(0x11)      /* sms_on_ims */
};

static const qmi_implemented_optionals wms_raw_read_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_raw_read_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_raw_read_reqresp_msg_impl_tuple_v01 = 
{
  &wms_raw_read_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wms_modify_tag_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* message_mode */
};

static const qmi_implemented_optionals wms_modify_tag_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_modify_tag_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_modify_tag_reqresp_msg_impl_tuple_v01 = 
{
  &wms_modify_tag_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wms_delete_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* index */
    QMI_SI_TLV_SET(0x11)   |  /* tag_type */
    QMI_SI_TLV_SET(0x12)      /* message_mode */
};

static const qmi_implemented_optionals wms_delete_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_delete_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_delete_reqresp_msg_impl_tuple_v01 = 
{
  &wms_delete_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_get_message_protocol_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_list_messages_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* tag_type */
    QMI_SI_TLV_SET(0x11)      /* message_mode */
};

static const qmi_implemented_optionals wms_list_messages_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_list_messages_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_list_messages_reqresp_msg_impl_tuple_v01 = 
{
  &wms_list_messages_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wms_set_routes_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* transfer_ind */
};

static const qmi_implemented_optionals wms_set_routes_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_set_routes_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_set_routes_reqresp_msg_impl_tuple_v01 = 
{
  &wms_set_routes_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wms_get_routes_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* transfer_ind */
};

static const qmi_implemented_optionals wms_get_routes_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_routes_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_routes_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_routes_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple wms_get_smsc_address_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_set_smsc_address_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* smsc_address_type */
};

static const qmi_implemented_optionals wms_set_smsc_address_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_set_smsc_address_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_set_smsc_address_reqresp_msg_impl_tuple_v01 = 
{
  &wms_set_smsc_address_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wms_get_store_max_size_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* message_mode */
};

static const qmi_implemented_optionals wms_get_store_max_size_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_store_max_size_req_msg_impl_array_v01
};
static const uint32_t wms_get_store_max_size_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* free_slots */
};

static const qmi_implemented_optionals wms_get_store_max_size_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_store_max_size_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_store_max_size_reqresp_msg_impl_tuple_v01 = 
{
  &wms_get_store_max_size_req_msg_impl_v01,
  &wms_get_store_max_size_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_send_ack_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* wms_3gpp2_failure_information */
    QMI_SI_TLV_SET(0x11)   |  /* wms_3gpp_failure_information */
    QMI_SI_TLV_SET(0x12)      /* sms_on_ims */
};

static const qmi_implemented_optionals wms_send_ack_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_send_ack_req_msg_impl_array_v01
};
static const uint32_t wms_send_ack_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* failure_cause */
};

static const qmi_implemented_optionals wms_send_ack_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_send_ack_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_send_ack_reqresp_msg_impl_tuple_v01 = 
{
  &wms_send_ack_req_msg_impl_v01,
  &wms_send_ack_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple wms_set_retry_period_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_set_retry_interval_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_set_dc_disconnect_timer_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_set_memory_status_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_set_broadcast_activation_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_set_broadcast_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* wms_3gpp_broadcast_config_info */
    QMI_SI_TLV_SET(0x11)      /* wms_3gpp2_broadcast_config_info */
};

static const qmi_implemented_optionals wms_set_broadcast_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_set_broadcast_config_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_set_broadcast_config_reqresp_msg_impl_tuple_v01 = 
{
  &wms_set_broadcast_config_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wms_get_broadcast_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* wms_3gpp_broadcast_info */
    QMI_SI_TLV_SET(0x11)      /* wms_3gpp2_broadcast_info */
};

static const qmi_implemented_optionals wms_get_broadcast_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_broadcast_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_broadcast_config_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_broadcast_config_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple wms_memory_full_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_get_domain_pref_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_set_domain_pref_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_send_from_mem_store_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* sms_on_ims */
};

static const qmi_implemented_optionals wms_send_from_mem_store_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_send_from_mem_store_req_msg_impl_array_v01
};
static const uint32_t wms_send_from_mem_store_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* message_id */
    QMI_SI_TLV_SET(0x11)   |  /* cause_code */
    QMI_SI_TLV_SET(0x12)   |  /* error_class */
    QMI_SI_TLV_SET(0x13)   |  /* gw_cause_info */
    QMI_SI_TLV_SET(0x14)      /* message_delivery_failure_type */
};

static const qmi_implemented_optionals wms_send_from_mem_store_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_send_from_mem_store_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_send_from_mem_store_reqresp_msg_impl_tuple_v01 = 
{
  &wms_send_from_mem_store_req_msg_impl_v01,
  &wms_send_from_mem_store_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple wms_get_message_waiting_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_message_waiting_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_set_primary_client_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_smsc_address_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_indication_register_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* reg_transport_layer_info_events */
    QMI_SI_TLV_SET(0x11)   |  /* reg_transport_nw_reg_info_events */
    QMI_SI_TLV_SET(0x12)   |  /* reg_call_status_info_events */
    QMI_SI_TLV_SET(0x13)   |  /* reg_service_ready_events */
    QMI_SI_TLV_SET(0x14)      /* reg_broadcast_config_events */
};

static const qmi_implemented_optionals wms_indication_register_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_indication_register_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wmsication_register_reqresp_msg_impl_tuple_v01 = 
{
  &wms_indication_register_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t wms_get_transport_layer_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* registered_ind */
    QMI_SI_TLV_SET(0x11)      /* transport_layer_info */
};

static const qmi_implemented_optionals wms_get_transport_layer_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_transport_layer_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_transport_layer_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_transport_layer_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_transport_layer_info_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* transport_layer_info */
};

static const qmi_implemented_optionals wms_transport_layer_info_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_transport_layer_info_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_transport_layer_info_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &wms_transport_layer_info_ind_msg_impl_v01
};

static const uint32_t wms_get_transport_nw_reg_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* transport_nw_reg_status */
};

static const qmi_implemented_optionals wms_get_transport_nw_reg_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_transport_nw_reg_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_transport_nw_reg_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_transport_nw_reg_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple wms_transport_nw_reg_info_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_bind_subscription_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_get_indication_register_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* reg_transport_layer_info_events */
    QMI_SI_TLV_SET(0x11)   |  /* reg_transport_nw_reg_info_events */
    QMI_SI_TLV_SET(0x12)   |  /* reg_call_status_info_events */
    QMI_SI_TLV_SET(0x13)   |  /* reg_service_ready_events */
    QMI_SI_TLV_SET(0x14)      /* reg_broadcast_config_events */
};

static const qmi_implemented_optionals wms_get_indication_register_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_indication_register_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_getication_register_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_indication_register_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_get_sms_parameters_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* dest_addr */
    QMI_SI_TLV_SET(0x11)   |  /* pid */
    QMI_SI_TLV_SET(0x12)   |  /* dcs */
    QMI_SI_TLV_SET(0x13)      /* validity */
};

static const qmi_implemented_optionals wms_get_sms_parameters_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_sms_parameters_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_sms_parameters_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_sms_parameters_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_set_sms_parameters_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* dest_addr */
    QMI_SI_TLV_SET(0x11)   |  /* pid */
    QMI_SI_TLV_SET(0x12)   |  /* dcs */
    QMI_SI_TLV_SET(0x13)      /* validity */
};

static const qmi_implemented_optionals wms_set_sms_parameters_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_set_sms_parameters_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_set_sms_parameters_reqresp_msg_impl_tuple_v01 = 
{
  &wms_set_sms_parameters_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple wms_call_status_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_get_domain_pref_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* lte_domain_pref */
    QMI_SI_TLV_SET(0x11)      /* gw_domain_pref */
};

static const qmi_implemented_optionals wms_get_domain_pref_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_domain_pref_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_domain_pref_config_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_domain_pref_config_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_set_domain_pref_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* lte_domain_pref */
    QMI_SI_TLV_SET(0x11)      /* gw_domain_pref */
};

static const qmi_implemented_optionals wms_set_domain_pref_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_set_domain_pref_config_req_msg_impl_array_v01
};
static const uint32_t wms_set_domain_pref_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* lte_domain_pref_outcome */
    QMI_SI_TLV_SET(0x11)      /* gw_domain_pref_outcome */
};

static const qmi_implemented_optionals wms_set_domain_pref_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_set_domain_pref_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_set_domain_pref_config_reqresp_msg_impl_tuple_v01 = 
{
  &wms_set_domain_pref_config_req_msg_impl_v01,
  &wms_set_domain_pref_config_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_get_retry_period_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* retry_period */
};

static const qmi_implemented_optionals wms_get_retry_period_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_retry_period_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_retry_period_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_retry_period_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_get_retry_interval_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* retry_interval */
};

static const qmi_implemented_optionals wms_get_retry_interval_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_retry_interval_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_retry_interval_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_retry_interval_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_get_dc_disconnect_timer_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* dc_auto_disconn_timer */
};

static const qmi_implemented_optionals wms_get_dc_disconnect_timer_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_dc_disconnect_timer_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_dc_disconnect_timer_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_dc_disconnect_timer_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_get_memory_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* memory_available */
};

static const qmi_implemented_optionals wms_get_memory_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_memory_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_memory_status_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_memory_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_get_primary_client_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* primary_client */
};

static const qmi_implemented_optionals wms_get_primary_client_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_primary_client_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_primary_client_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_primary_client_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_get_subscription_binding_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* subs_type */
};

static const qmi_implemented_optionals wms_get_subscription_binding_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_subscription_binding_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_subscription_binding_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_subscription_binding_resp_msg_impl_v01,
  NULL
};

static const uint32_t wms_async_raw_send_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* force_on_dc */
    QMI_SI_TLV_SET(0x11)   |  /* follow_on_dc */
    QMI_SI_TLV_SET(0x12)   |  /* link_timer */
    QMI_SI_TLV_SET(0x13)   |  /* sms_on_ims */
    QMI_SI_TLV_SET(0x14)   |  /* retry_message */
    QMI_SI_TLV_SET(0x15)   |  /* retry_message_id */
    QMI_SI_TLV_SET(0x16)      /* user_data */
};

static const qmi_implemented_optionals wms_async_raw_send_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_async_raw_send_req_msg_impl_array_v01
};
static const uint32_t wms_async_raw_send_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* message_id */
    QMI_SI_TLV_SET(0x11)   |  /* cause_code */
    QMI_SI_TLV_SET(0x12)   |  /* error_class */
    QMI_SI_TLV_SET(0x13)   |  /* gw_cause_info */
    QMI_SI_TLV_SET(0x14)   |  /* message_delivery_failure_type */
    QMI_SI_TLV_SET(0x15)   |  /* message_delivery_failure_cause */
    QMI_SI_TLV_SET(0x16)   |  /* call_control_modified_info */
    QMI_SI_TLV_SET(0x17)      /* user_data */
};

static const qmi_implemented_optionals wms_async_raw_send_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_async_raw_send_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_async_raw_send_reqresp_msg_impl_tuple_v01 = 
{
  &wms_async_raw_send_req_msg_impl_v01,
  NULL,
  &wms_async_raw_send_ind_msg_impl_v01
};

static const uint32_t wms_async_send_ack_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* wms_3gpp2_failure_information */
    QMI_SI_TLV_SET(0x11)   |  /* wms_3gpp_failure_information */
    QMI_SI_TLV_SET(0x12)   |  /* sms_on_ims */
    QMI_SI_TLV_SET(0x13)      /* user_data */
};

static const qmi_implemented_optionals wms_async_send_ack_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_async_send_ack_req_msg_impl_array_v01
};
static const uint32_t wms_async_send_ack_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x11)      /* user_data */
};

static const qmi_implemented_optionals wms_async_send_ack_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_async_send_ack_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_async_send_ack_reqresp_msg_impl_tuple_v01 = 
{
  &wms_async_send_ack_req_msg_impl_v01,
  NULL,
  &wms_async_send_ack_ind_msg_impl_v01
};

static const uint32_t wms_async_send_from_mem_store_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* sms_on_ims */
    QMI_SI_TLV_SET(0x11)      /* user_data */
};

static const qmi_implemented_optionals wms_async_send_from_mem_store_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_async_send_from_mem_store_req_msg_impl_array_v01
};
static const uint32_t wms_async_send_from_mem_store_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* message_id */
    QMI_SI_TLV_SET(0x11)   |  /* cause_code */
    QMI_SI_TLV_SET(0x12)   |  /* error_class */
    QMI_SI_TLV_SET(0x13)   |  /* gw_cause_info */
    QMI_SI_TLV_SET(0x14)   |  /* message_delivery_failure_type */
    QMI_SI_TLV_SET(0x15)   |  /* message_delivery_failure_cause */
    QMI_SI_TLV_SET(0x16)   |  /* call_control_modified_info */
    QMI_SI_TLV_SET(0x17)      /* user_data */
};

static const qmi_implemented_optionals wms_async_send_from_mem_store_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_async_send_from_mem_store_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_async_send_from_mem_store_reqresp_msg_impl_tuple_v01 = 
{
  &wms_async_send_from_mem_store_req_msg_impl_v01,
  NULL,
  &wms_async_send_from_mem_store_ind_msg_impl_v01
};

static const uint32_t wms_get_service_ready_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* registered_ind */
    QMI_SI_TLV_SET(0x11)      /* ready_status */
};

static const qmi_implemented_optionals wms_get_service_ready_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_get_service_ready_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_get_service_ready_status_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &wms_get_service_ready_status_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple wms_service_ready_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t wms_broadcast_config_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* wms_3gpp_broadcast_info */
    QMI_SI_TLV_SET(0x11)      /* wms_3gpp2_broadcast_info */
};

static const qmi_implemented_optionals wms_broadcast_config_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  wms_broadcast_config_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple wms_broadcast_config_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &wms_broadcast_config_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const wms_service_implemented_array_v01[] =
{
    &wms_reset_reqresp_msg_impl_tuple_v01,               /* 0x0000 */
    &wms_set_event_report_reqresp_msg_impl_tuple_v01,    /* 0x0001 */
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
    NULL,                                                /* 0x001E */
    NULL,                                                /* 0x001F */
    &wms_raw_send_reqresp_msg_impl_tuple_v01,            /* 0x0020 */
    &wms_raw_write_reqresp_msg_impl_tuple_v01,           /* 0x0021 */
    &wms_raw_read_reqresp_msg_impl_tuple_v01,            /* 0x0022 */
    &wms_modify_tag_reqresp_msg_impl_tuple_v01,          /* 0x0023 */
    &wms_delete_reqresp_msg_impl_tuple_v01,              /* 0x0024 */
    NULL,                                                /* 0x0025 */
    NULL,                                                /* 0x0026 */
    NULL,                                                /* 0x0027 */
    NULL,                                                /* 0x0028 */
    NULL,                                                /* 0x0029 */
    NULL,                                                /* 0x002A */
    NULL,                                                /* 0x002B */
    NULL,                                                /* 0x002C */
    NULL,                                                /* 0x002D */
    NULL,                                                /* 0x002E */
    NULL,                                                /* 0x002F */
    &wms_get_message_protocol_reqresp_msg_impl_tuple_v01, /* 0x0030 */
    &wms_list_messages_reqresp_msg_impl_tuple_v01,       /* 0x0031 */
    &wms_set_routes_reqresp_msg_impl_tuple_v01,          /* 0x0032 */
    &wms_get_routes_reqresp_msg_impl_tuple_v01,          /* 0x0033 */
    &wms_get_smsc_address_reqresp_msg_impl_tuple_v01,    /* 0x0034 */
    &wms_set_smsc_address_reqresp_msg_impl_tuple_v01,    /* 0x0035 */
    &wms_get_store_max_size_reqresp_msg_impl_tuple_v01,  /* 0x0036 */
    &wms_send_ack_reqresp_msg_impl_tuple_v01,            /* 0x0037 */
    &wms_set_retry_period_reqresp_msg_impl_tuple_v01,    /* 0x0038 */
    &wms_set_retry_interval_reqresp_msg_impl_tuple_v01,  /* 0x0039 */
    &wms_set_dc_disconnect_timer_reqresp_msg_impl_tuple_v01, /* 0x003A */
    &wms_set_memory_status_reqresp_msg_impl_tuple_v01,   /* 0x003B */
    &wms_set_broadcast_activation_reqresp_msg_impl_tuple_v01, /* 0x003C */
    &wms_set_broadcast_config_reqresp_msg_impl_tuple_v01, /* 0x003D */
    &wms_get_broadcast_config_reqresp_msg_impl_tuple_v01, /* 0x003E */
    &wms_memory_full_msg_impl_tuple_v01,                 /* 0x003F */
    &wms_get_domain_pref_reqresp_msg_impl_tuple_v01,     /* 0x0040 */
    &wms_set_domain_pref_reqresp_msg_impl_tuple_v01,     /* 0x0041 */
    &wms_send_from_mem_store_reqresp_msg_impl_tuple_v01, /* 0x0042 */
    &wms_get_message_waiting_reqresp_msg_impl_tuple_v01, /* 0x0043 */
    &wms_message_waiting_msg_impl_tuple_v01,             /* 0x0044 */
    &wms_set_primary_client_reqresp_msg_impl_tuple_v01,  /* 0x0045 */
    &wms_smsc_address_msg_impl_tuple_v01,                /* 0x0046 */
    &wmsication_register_reqresp_msg_impl_tuple_v01,     /* 0x0047 */
    &wms_get_transport_layer_reqresp_msg_impl_tuple_v01, /* 0x0048 */
    &wms_transport_layer_info_msg_impl_tuple_v01,        /* 0x0049 */
    &wms_get_transport_nw_reg_reqresp_msg_impl_tuple_v01, /* 0x004A */
    &wms_transport_nw_reg_info_msg_impl_tuple_v01,       /* 0x004B */
    &wms_bind_subscription_reqresp_msg_impl_tuple_v01,   /* 0x004C */
    &wms_getication_register_reqresp_msg_impl_tuple_v01, /* 0x004D */
    &wms_get_sms_parameters_reqresp_msg_impl_tuple_v01,  /* 0x004E */
    &wms_set_sms_parameters_reqresp_msg_impl_tuple_v01,  /* 0x004F */
    &wms_call_status_msg_impl_tuple_v01,                 /* 0x0050 */
    &wms_get_domain_pref_config_reqresp_msg_impl_tuple_v01, /* 0x0051 */
    &wms_set_domain_pref_config_reqresp_msg_impl_tuple_v01, /* 0x0052 */
    &wms_get_retry_period_reqresp_msg_impl_tuple_v01,    /* 0x0053 */
    &wms_get_retry_interval_reqresp_msg_impl_tuple_v01,  /* 0x0054 */
    &wms_get_dc_disconnect_timer_reqresp_msg_impl_tuple_v01, /* 0x0055 */
    &wms_get_memory_status_reqresp_msg_impl_tuple_v01,   /* 0x0056 */
    &wms_get_primary_client_reqresp_msg_impl_tuple_v01,  /* 0x0057 */
    &wms_get_subscription_binding_reqresp_msg_impl_tuple_v01, /* 0x0058 */
    &wms_async_raw_send_reqresp_msg_impl_tuple_v01,      /* 0x0059 */
    &wms_async_send_ack_reqresp_msg_impl_tuple_v01,      /* 0x005A */
    &wms_async_send_from_mem_store_reqresp_msg_impl_tuple_v01, /* 0x005B */
    &wms_get_service_ready_status_reqresp_msg_impl_tuple_v01, /* 0x005C */
    &wms_service_ready_msg_impl_tuple_v01,               /* 0x005D */
    &wms_broadcast_config_msg_impl_tuple_v01             /* 0x005E */
};

static const qmi_implemented_messages wms_service_implemented_v01 =
{
  0x005E,  /* maximum message ID in this service */
  wms_service_implemented_array_v01
};

const qmi_implemented_messages *wms_get_service_impl_v01 (void)
{
  return &wms_service_implemented_v01;
}
