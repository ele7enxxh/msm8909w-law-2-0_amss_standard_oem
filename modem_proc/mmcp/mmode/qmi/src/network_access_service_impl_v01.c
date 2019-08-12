/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              N E T W O R K _ A C C E S S _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the nas service Data structures.

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved. 
 QUALCOMM Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/network_access_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.0
   It was generated on: Thu Feb  2 2012
   From IDL File: network_access_service_v01.idl */

#include "qmi_si.h"
#include "network_access_service_v01.h"

static const qmi_implemented_optionals_tuple nas_reset_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_abort_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_set_event_report_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* signal_strength */
    QMI_SI_TLV_SET(0x11)   |  /* report_rf_band_info */
    QMI_SI_TLV_SET(0x12)   |  /* report_reg_reject */
    QMI_SI_TLV_SET(0x13)   |  /* rssi_indicator */
    QMI_SI_TLV_SET(0x14)   |  /* ecio_indicator */
    QMI_SI_TLV_SET(0x15)   |  /* io_indicator */
    QMI_SI_TLV_SET(0x16)   |  /* sinr_indicator */
    QMI_SI_TLV_SET(0x17)   |  /* report_error_rate */
    QMI_SI_TLV_SET(0x18)   |  /* rsrq_indicator */
    QMI_SI_TLV_SET(0x19)   |  /* ecio_threshold_indicator */
    QMI_SI_TLV_SET(0x1A)   |  /* sinr_threshold_indicator */
    QMI_SI_TLV_SET(0x1B)   |  /* lte_snr_delta_indicator */
    QMI_SI_TLV_SET(0x1C)      /* lte_rsrp_delta_indicator */
};

static const qmi_implemented_optionals nas_set_event_report_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_set_event_report_req_msg_impl_array_v01
};
static const uint32_t nas_event_report_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* signal_strength */
    QMI_SI_TLV_SET(0x11)   |  /* rf_band_info_list */
    QMI_SI_TLV_SET(0x12)   |  /* registration_reject_reason */
    QMI_SI_TLV_SET(0x13)   |  /* rssi */
    QMI_SI_TLV_SET(0x14)   |  /* ecio */
    QMI_SI_TLV_SET(0x15)   |  /* io */
    QMI_SI_TLV_SET(0x16)   |  /* sinr */
    QMI_SI_TLV_SET(0x17)   |  /* error_rate */
    QMI_SI_TLV_SET(0x18)   |  /* rsrq */
    QMI_SI_TLV_SET(0x19)   |  /* snr */
    QMI_SI_TLV_SET(0x1A)      /* rsrp */
};

static const qmi_implemented_optionals nas_event_report_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_event_report_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_set_event_report_reqresp_msg_impl_tuple_v01 = 
{
  &nas_set_event_report_req_msg_impl_v01,
  NULL,
  &nas_event_report_ind_msg_impl_v01
};

static const uint32_t nas_indication_register_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* reg_sys_sel_pref */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_SET(0x12)   |  /* reg_ddtm_events */
    QMI_SI_TLV_SET(0x13)   |  /* req_serving_system */
    QMI_SI_TLV_SET(0x14)   |  /* dual_standby_pref */
    QMI_SI_TLV_SET(0x15)   |  /* subscription_info */
    QMI_SI_TLV_CLEAR(0x16) |  /* Undefined */
    QMI_SI_TLV_SET(0x17)   |  /* reg_network_time */
    QMI_SI_TLV_SET(0x18)   |  /* sys_info */
    QMI_SI_TLV_SET(0x19)   |  /* sig_info */
    QMI_SI_TLV_SET(0x1A)   |  /* err_rate */
    QMI_SI_TLV_SET(0x1B)   |  /* reg_hdr_uati */
    QMI_SI_TLV_SET(0x1C)   |  /* reg_hdr_session_close */
    QMI_SI_TLV_SET(0x1D)   |  /* reg_managed_roaming */
    QMI_SI_TLV_SET(0x1E)   |  /* reg_current_plmn_name */
    QMI_SI_TLV_SET(0x1F)   |  /* reg_embms_status */
    QMI_SI_TLV_SET(0x20)   |  /* reg_rf_band_info */
    QMI_SI_TLV_SET(0x21)   |  /* network_reject */
    QMI_SI_TLV_SET(0x22) |  /* Operator name data */
    QMI_SI_TLV_SET(0x23) |  /* CSP PLMN mode bit */
    QMI_SI_TLV_SET(0x24)      /* RTRE configuration */
};

static const qmi_implemented_optionals nas_indication_register_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_indication_register_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nasication_register_reqresp_msg_impl_tuple_v01 = 
{
  &nas_indication_register_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t nas_get_signal_strength_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* request_mask */
};

static const qmi_implemented_optionals nas_get_signal_strength_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_signal_strength_req_msg_impl_array_v01
};
static const uint32_t nas_get_signal_strength_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* signal_strength_list */
    QMI_SI_TLV_SET(0x11)   |  /* rssi */
    QMI_SI_TLV_SET(0x12)   |  /* ecio */
    QMI_SI_TLV_SET(0x13)   |  /* io */
    QMI_SI_TLV_SET(0x14)   |  /* sinr */
    QMI_SI_TLV_SET(0x15)   |  /* error_rate */
    QMI_SI_TLV_SET(0x16)   |  /* rsrq */
    QMI_SI_TLV_SET(0x17)   |  /* snr */
    QMI_SI_TLV_SET(0x18)      /* lte_rsrp */
};

static const qmi_implemented_optionals nas_get_signal_strength_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_signal_strength_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_signal_strength_reqresp_msg_impl_tuple_v01 = 
{
  &nas_get_signal_strength_req_msg_impl_v01,
  &nas_get_signal_strength_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_perform_network_scan_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* network_type */
};

static const qmi_implemented_optionals nas_perform_network_scan_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_perform_network_scan_req_msg_impl_array_v01
};
static const uint32_t nas_perform_network_scan_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* nas_3gpp_network_info */
    QMI_SI_TLV_SET(0x11)   |  /* nas_network_radio_access_technology */
    QMI_SI_TLV_SET(0x12)   |  /* mnc_includes_pcs_digit */
    QMI_SI_TLV_SET(0x13)      /* nas_network_scan_result */    
};

static const qmi_implemented_optionals nas_perform_network_scan_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_perform_network_scan_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_perform_network_scan_reqresp_msg_impl_tuple_v01 = 
{
  &nas_perform_network_scan_req_msg_impl_v01,
  &nas_perform_network_scan_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_initiate_network_register_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* manual_network_register_info */
    QMI_SI_TLV_SET(0x11)   |  /* change_duration */
    QMI_SI_TLV_SET(0x12)      /* mnc_includes_pcs_digit */
};

static const qmi_implemented_optionals nas_initiate_network_register_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_initiate_network_register_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_initiate_network_register_reqresp_msg_impl_tuple_v01 = 
{
  &nas_initiate_network_register_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t nas_initiate_attach_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ps_attach_action */
};

static const qmi_implemented_optionals nas_initiate_attach_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_initiate_attach_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_initiate_attach_reqresp_msg_impl_tuple_v01 = 
{
  &nas_initiate_attach_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t nas_get_serving_system_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* roaming_indicator */
    QMI_SI_TLV_SET(0x11)   |  /* data_capabilities */
    QMI_SI_TLV_SET(0x12)   |  /* current_plmn */
    QMI_SI_TLV_SET(0x13)   |  /* cdma_system_id */
    QMI_SI_TLV_SET(0x14)   |  /* cdma_base_station_info */
    QMI_SI_TLV_SET(0x15)   |  /* roaming_indicator_list */
    QMI_SI_TLV_SET(0x16)   |  /* def_roam_ind */
    QMI_SI_TLV_SET(0x17)   |  /* nas_3gpp_time_zone */
    QMI_SI_TLV_SET(0x18)   |  /* p_rev_in_use */
    QMI_SI_TLV_CLEAR(0x19) |  /* Undefined */
    QMI_SI_TLV_SET(0x1A)   |  /* time_zone */
    QMI_SI_TLV_SET(0x1B)   |  /* adj */
    QMI_SI_TLV_SET(0x1C)   |  /* lac */
    QMI_SI_TLV_SET(0x1D)   |  /* cell_id */
    QMI_SI_TLV_SET(0x1E)   |  /* ccs */
    QMI_SI_TLV_SET(0x1F)   |  /* prl_ind */
    QMI_SI_TLV_SET(0x20)   |  /* dtm_ind */
    QMI_SI_TLV_SET(0x21)   |  /* detailed_service_info */
    QMI_SI_TLV_SET(0x22)   |  /* cdma_system_id_ext */
    QMI_SI_TLV_SET(0x23)   |  /* hdr_personality */
    QMI_SI_TLV_SET(0x24)   |  /* tac */
    QMI_SI_TLV_SET(0x25)   |  /* call_barring_status */
    QMI_SI_TLV_SET(0x26)   |  /* umts_psc */
    QMI_SI_TLV_SET(0x27)   |  /* mnc_includes_pcs_digit */
    QMI_SI_TLV_SET(0x28)      /* hs_call_status */
};

static const qmi_implemented_optionals nas_get_serving_system_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_serving_system_resp_msg_impl_array_v01
};
static const uint32_t nas_serving_system_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* roaming_indicator */
    QMI_SI_TLV_SET(0x11)   |  /* data_capabilities */
    QMI_SI_TLV_SET(0x12)   |  /* current_plmn */
    QMI_SI_TLV_SET(0x13)   |  /* cdma_system_id */
    QMI_SI_TLV_SET(0x14)   |  /* cdma_base_station_info */
    QMI_SI_TLV_SET(0x15)   |  /* roaming_indicator_list */
    QMI_SI_TLV_SET(0x16)   |  /* def_roam_ind */
    QMI_SI_TLV_SET(0x17)   |  /* nas_3gpp_time_zone */
    QMI_SI_TLV_SET(0x18)   |  /* p_rev_in_use */
    QMI_SI_TLV_SET(0x19)   |  /* plmn_description_changed */
    QMI_SI_TLV_SET(0x1A)   |  /* time_zone */
    QMI_SI_TLV_SET(0x1B)   |  /* adj */
    QMI_SI_TLV_SET(0x1C)   |  /* universal_time_and_local_time_3gpp_zone */
    QMI_SI_TLV_SET(0x1D)   |  /* lac */
    QMI_SI_TLV_SET(0x1E)   |  /* cell_id */
    QMI_SI_TLV_SET(0x1F)   |  /* ccs */
    QMI_SI_TLV_SET(0x20)   |  /* prl_ind */
    QMI_SI_TLV_SET(0x21)   |  /* dtm_ind */
    QMI_SI_TLV_SET(0x22)   |  /* detailed_service_info */
    QMI_SI_TLV_SET(0x23)   |  /* cdma_system_id_ext */
    QMI_SI_TLV_SET(0x24)   |  /* hdr_personality */
    QMI_SI_TLV_SET(0x25)   |  /* tac */
    QMI_SI_TLV_SET(0x26)   |  /* call_barring_status */
    QMI_SI_TLV_SET(0x27)   |  /* srv_sys_no_change */
    QMI_SI_TLV_SET(0x28)   |  /* umts_psc */
    QMI_SI_TLV_SET(0x29)   |  /* mnc_includes_pcs_digit */
    QMI_SI_TLV_SET(0x2A)      /* hs_call_status */
};

static const qmi_implemented_optionals nas_serving_system_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_serving_system_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_serving_system_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_serving_system_resp_msg_impl_v01,
  &nas_serving_system_ind_msg_impl_v01
};

static const uint32_t nas_get_home_network_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* home_system_id */
    QMI_SI_TLV_SET(0x11)      /* nas_3gpp2_home_network_ext */
};

static const qmi_implemented_optionals nas_get_home_network_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_home_network_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_home_network_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_home_network_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_get_preferred_networks_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* nas_3gpp_preferred_networks */
    QMI_SI_TLV_SET(0x11)      /* static_3gpp_preferred_networks */
};

static const qmi_implemented_optionals nas_get_preferred_networks_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_preferred_networks_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_preferred_networks_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_preferred_networks_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_set_preferred_networks_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* nas_3gpp_preferred_networks */
};

static const qmi_implemented_optionals nas_set_preferred_networks_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_set_preferred_networks_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_set_preferred_networks_reqresp_msg_impl_tuple_v01 = 
{
  &nas_set_preferred_networks_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t nas_get_forbidden_networks_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* nas_3gpp_forbidden_networks */
};

static const qmi_implemented_optionals nas_get_forbidden_networks_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_forbidden_networks_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_forbidden_networks_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_forbidden_networks_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_set_forbidden_networks_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* nas_3gpp_forbidden_networks */
};

static const qmi_implemented_optionals nas_set_forbidden_networks_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_set_forbidden_networks_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_set_forbidden_networks_reqresp_msg_impl_tuple_v01 = 
{
  &nas_set_forbidden_networks_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_set_technology_preference_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_get_technology_preference_resp_type_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* persistent_technology_pref */
};

static const qmi_implemented_optionals nas_get_technology_preference_resp_type_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_technology_preference_resp_type_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_technology_preference_reqresp_type_impl_tuple_v01 = 
{
  NULL,
  &nas_get_technology_preference_resp_type_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple nas_get_accolc_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_set_accolc_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_get_network_system_preference_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_get_device_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_SET(0x11)   |  /* sci */
    QMI_SI_TLV_SET(0x12)   |  /* scm */
    QMI_SI_TLV_SET(0x13)   |  /* registration_parameters */
    QMI_SI_TLV_SET(0x14)   |  /* force_rev0 */
    QMI_SI_TLV_SET(0x15)   |  /* hdr_scp_config */
    QMI_SI_TLV_SET(0x16)   |  /* roam_pref */
    QMI_SI_TLV_SET(0x17)      /* force_hdrscp_config_at */
};

static const qmi_implemented_optionals nas_get_device_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_device_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_device_config_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_device_config_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_set_device_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* spc */
    QMI_SI_TLV_CLEAR(0x11) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x12) |  /* Undefined */
    QMI_SI_TLV_CLEAR(0x13) |  /* Undefined */
    QMI_SI_TLV_SET(0x14)   |  /* force_hdr_rev0 */
    QMI_SI_TLV_SET(0x15)   |  /* hdr_scp_config */
    QMI_SI_TLV_SET(0x16)   |   /* roam_pref */
    QMI_SI_TLV_SET(0x17)       /* force_hdrscp */    
};

static const qmi_implemented_optionals nas_set_device_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_set_device_config_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_set_device_config_reqresp_msg_impl_tuple_v01 = 
{
  &nas_set_device_config_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_get_rf_band_info_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_get_an_aaa_status_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_set_system_selection_preference_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* emergency_mode */
    QMI_SI_TLV_SET(0x11)   |  /* mode_pref */
    QMI_SI_TLV_SET(0x12)   |  /* band_pref */
    QMI_SI_TLV_SET(0x13)   |  /* prl_pref */
    QMI_SI_TLV_SET(0x14)   |  /* roam_pref */
    QMI_SI_TLV_SET(0x15)   |  /* lte_band_pref */
    QMI_SI_TLV_SET(0x16)   |  /* net_sel_pref */
    QMI_SI_TLV_SET(0x17)   |  /* change_duration */
    QMI_SI_TLV_SET(0x18)   |  /* srv_domain_pref */
    QMI_SI_TLV_SET(0x19)   |  /* gw_acq_order_pref */
    QMI_SI_TLV_SET(0x1A)   |  /* mnc_includes_pcs_digit */
    QMI_SI_TLV_SET(0x1B)   |  /* __DUP__0x18 */
    QMI_SI_TLV_SET(0x1C)   |  /* __DUP__0x19 */
    QMI_SI_TLV_SET(0x1D)   |  /* tdscdma_band_pref */
    QMI_SI_TLV_SET(0x1E)      /* acq_order */
};

static const qmi_implemented_optionals nas_set_system_selection_preference_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_set_system_selection_preference_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_set_system_selection_preference_reqresp_msg_impl_tuple_v01 = 
{
  &nas_set_system_selection_preference_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t nas_get_system_selection_preference_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* emergency_mode */
    QMI_SI_TLV_SET(0x11)   |  /* mode_pref */
    QMI_SI_TLV_SET(0x12)   |  /* band_pref */
    QMI_SI_TLV_SET(0x13)   |  /* prl_pref */
    QMI_SI_TLV_SET(0x14)   |  /* roam_pref */
    QMI_SI_TLV_SET(0x15)   |  /* band_pref_ext */
    QMI_SI_TLV_SET(0x16)   |  /* net_sel_pref */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_SET(0x18)   |  /* srv_domain_pref */
    QMI_SI_TLV_SET(0x19)   |  /* gw_acq_order_pref */
    QMI_SI_TLV_SET(0x1A)   |  /* tdscdma_band_pref */
    QMI_SI_TLV_SET(0x1B)   |  /* manual_net_sel_plmn */
    QMI_SI_TLV_SET(0x1C)      /* acq_order */
};

static const qmi_implemented_optionals nas_get_system_selection_preference_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_system_selection_preference_resp_msg_impl_array_v01
};
static const uint32_t nas_system_selection_preference_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* emergency_mode */
    QMI_SI_TLV_SET(0x11)   |  /* mode_pref */
    QMI_SI_TLV_SET(0x12)   |  /* band_pref */
    QMI_SI_TLV_SET(0x13)   |  /* prl_pref */
    QMI_SI_TLV_SET(0x14)   |  /* roam_pref */
    QMI_SI_TLV_SET(0x15)   |  /* lte_band_pref */
    QMI_SI_TLV_SET(0x16)   |  /* net_sel_pref */
    QMI_SI_TLV_CLEAR(0x17) |  /* Undefined */
    QMI_SI_TLV_SET(0x18)   |  /* srv_domain_pref */
    QMI_SI_TLV_SET(0x19)   |  /* gw_acq_order_pref */
    QMI_SI_TLV_SET(0x1A)   |  /* tdscdma_band_pref */
    QMI_SI_TLV_SET(0x1B)   |  /* manual_net_sel_plmn */
    QMI_SI_TLV_SET(0x1C)      /* acq_order */
};

static const qmi_implemented_optionals nas_system_selection_preference_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_system_selection_preference_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_system_selection_preference_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_system_selection_preference_resp_msg_impl_v01,
  &nas_system_selection_preference_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple nas_set_ddtm_preference_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_ddtm_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_get_operator_name_data_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* service_provider_name */
    QMI_SI_TLV_SET(0x11)   |  /* operator_plmn_list */
    QMI_SI_TLV_SET(0x12)   |  /* plmn_network_name */
    QMI_SI_TLV_SET(0x13)   |  /* plmn_name */
    QMI_SI_TLV_SET(0x14)      /* nitz_information */
};

static const qmi_implemented_optionals nas_get_operator_name_data_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_operator_name_data_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_operator_name_data_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_operator_name_data_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_operator_name_data_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* service_provider_name */
    QMI_SI_TLV_SET(0x11)   |  /* operator_plmn_list */
    QMI_SI_TLV_SET(0x12)   |  /* plmn_network_name */
    QMI_SI_TLV_SET(0x13)   |  /* plmn_name */
    QMI_SI_TLV_SET(0x14)      /* nitz_information */
};

static const qmi_implemented_optionals nas_operator_name_data_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_operator_name_data_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_operator_name_data_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_operator_name_data_ind_msg_impl_v01
};

static const uint32_t nas_get_csp_plmn_mode_bit_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* plmn_mode */
};

static const qmi_implemented_optionals nas_get_csp_plmn_mode_bit_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_csp_plmn_mode_bit_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_csp_plmn_mode_bit_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_csp_plmn_mode_bit_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_csp_plmn_mode_bit_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* plmn_mode */
};

static const qmi_implemented_optionals nas_csp_plmn_mode_bit_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_csp_plmn_mode_bit_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_csp_plmn_mode_bit_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_csp_plmn_mode_bit_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple nas_update_akey_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_get_3gpp2_subscription_info_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* get_3gpp2_info_mask */
};

static const qmi_implemented_optionals nas_get_3gpp2_subscription_info_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_3gpp2_subscription_info_req_msg_impl_array_v01
};
static const uint32_t nas_get_3gpp2_subscription_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* nam_name */
    QMI_SI_TLV_SET(0x11)   |  /* dir_num */
    QMI_SI_TLV_SET(0x12)   |  /* cdma_sys_id */
    QMI_SI_TLV_SET(0x13)   |  /* min_based_info */
    QMI_SI_TLV_SET(0x14)   |  /* true_imsi */
    QMI_SI_TLV_SET(0x15)   |  /* cdma_channel_info */
    QMI_SI_TLV_SET(0x16)      /* mdn */
};

static const qmi_implemented_optionals nas_get_3gpp2_subscription_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_3gpp2_subscription_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_3gpp2_subscription_info_reqresp_msg_impl_tuple_v01 = 
{
  &nas_get_3gpp2_subscription_info_req_msg_impl_v01,
  &nas_get_3gpp2_subscription_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_set_3gpp2_subscription_info_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* dir_num */
    QMI_SI_TLV_SET(0x11)   |  /* cdma_sys_id */
    QMI_SI_TLV_SET(0x12)   |  /* min_based_info */
    QMI_SI_TLV_SET(0x13)   |  /* true_imsi */
    QMI_SI_TLV_SET(0x14)   |  /* cdma_channel_info */
    QMI_SI_TLV_SET(0x15)   |  /* nam_name */
    QMI_SI_TLV_SET(0x16)      /* mdn */
};

static const qmi_implemented_optionals nas_set_3gpp2_subscription_info_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_set_3gpp2_subscription_info_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_set_3gpp2_subscription_info_reqresp_msg_impl_tuple_v01 = 
{
  &nas_set_3gpp2_subscription_info_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t nas_get_mob_cai_rev_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* cai_rev */
};

static const qmi_implemented_optionals nas_get_mob_cai_rev_resp_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_mob_cai_rev_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_mob_cai_rev_reqresp_impl_tuple_v01 = 
{
  NULL,
  &nas_get_mob_cai_rev_resp_impl_v01,
  NULL
};

static const uint32_t nas_get_rtre_config_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* rtre_cfg */
    QMI_SI_TLV_SET(0x11)      /* rtre_cfg_pref */
};

static const qmi_implemented_optionals nas_get_rtre_config_resp_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_rtre_config_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_rtre_config_reqresp_impl_tuple_v01 = 
{
  NULL,
  &nas_get_rtre_config_resp_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple nas_set_rtre_config_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_get_cell_location_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* geran_info */
    QMI_SI_TLV_SET(0x11)   |  /* umts_info */
    QMI_SI_TLV_SET(0x12)   |  /* cdma_info */
    QMI_SI_TLV_SET(0x13)   |  /* lte_intra */
    QMI_SI_TLV_SET(0x14)   |  /* lte_inter */
    QMI_SI_TLV_SET(0x15)   |  /* lte_gsm */
    QMI_SI_TLV_SET(0x16)   |  /* lte_wcdma */
    QMI_SI_TLV_SET(0x17)      /* umts_cell_id */
};

static const qmi_implemented_optionals nas_get_cell_location_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_cell_location_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_cell_location_info_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_cell_location_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_get_plmn_name_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* suppress_sim_error */
    QMI_SI_TLV_SET(0x11)      /* mnc_includes_pcs_digit */			
};

static const qmi_implemented_optionals nas_get_plmn_name_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_plmn_name_req_msg_impl_array_v01
};

static const uint32_t nas_get_plmn_name_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* eons_plmn_name_3gpp */
};

static const qmi_implemented_optionals nas_get_plmn_name_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_plmn_name_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_plmn_name_reqresp_msg_impl_tuple_v01 = 
{
  &nas_get_plmn_name_req_msg_impl_v01,
  &nas_get_plmn_name_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple nas_bind_subscription_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_managed_roaming_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* radio_if */
};

static const qmi_implemented_optionals nas_managed_roaming_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_managed_roaming_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_managed_roaming_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_managed_roaming_ind_msg_impl_v01
};

static const uint32_t nas_dual_standby_pref_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* standby_pref */
};

static const qmi_implemented_optionals nas_dual_standby_pref_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_dual_standby_pref_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_dual_standby_pref_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_dual_standby_pref_ind_msg_impl_v01
};

static const uint32_t nas_subscription_info_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* is_priority_subs */
    QMI_SI_TLV_SET(0x11)   |  /* is_active */
    QMI_SI_TLV_SET(0x12)      /* is_default_data_subs */
};

static const qmi_implemented_optionals nas_subscription_info_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_subscription_info_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_subscription_info_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_subscription_info_ind_msg_impl_v01
};

static const uint32_t nas_get_mode_pref_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* idx0_mode_pref */
    QMI_SI_TLV_SET(0x11)      /* idx1_mode_pref */
};

static const qmi_implemented_optionals nas_get_mode_pref_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_mode_pref_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_mode_pref_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_mode_pref_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple nas_thermal_emergency_state_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_set_dual_standby_pref_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* standby_pref */
    QMI_SI_TLV_SET(0x11)   |  /* priority_subs */
    QMI_SI_TLV_SET(0x12)      /* default_data_subs */
};

static const qmi_implemented_optionals nas_set_dual_standby_pref_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_set_dual_standby_pref_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_set_dual_standby_pref_reqresp_msg_impl_tuple_v01 = 
{
  &nas_set_dual_standby_pref_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t nas_network_time_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* time_zone */
    QMI_SI_TLV_SET(0x11)   |  /* daylt_sav_adj */
    QMI_SI_TLV_SET(0x12)      /* radio_if */
};

static const qmi_implemented_optionals nas_network_time_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_network_time_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_network_time_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_network_time_ind_msg_impl_v01
};

static const uint32_t nas_get_sys_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cdma_srv_status_info */
    QMI_SI_TLV_SET(0x11)   |  /* hdr_srv_status_info */
    QMI_SI_TLV_SET(0x12)   |  /* gsm_srv_status_info */
    QMI_SI_TLV_SET(0x13)   |  /* wcdma_srv_status_info */
    QMI_SI_TLV_SET(0x14)   |  /* lte_srv_status_info */
    QMI_SI_TLV_SET(0x15)   |  /* cdma_sys_info */
    QMI_SI_TLV_SET(0x16)   |  /* hdr_sys_info */
    QMI_SI_TLV_SET(0x17)   |  /* gsm_sys_info */
    QMI_SI_TLV_SET(0x18)   |  /* wcdma_sys_info */
    QMI_SI_TLV_SET(0x19)   |  /* lte_sys_info */
    QMI_SI_TLV_SET(0x1A)   |  /* cdma_sys_info2 */
    QMI_SI_TLV_SET(0x1B)   |  /* hdr_sys_info2 */
    QMI_SI_TLV_SET(0x1C)   |  /* gsm_sys_info2 */
    QMI_SI_TLV_SET(0x1D)   |  /* wcdma_sys_info2 */
    QMI_SI_TLV_SET(0x1E)   |  /* lte_sys_info2 */
    QMI_SI_TLV_SET(0x1F)   |  /* gsm_sys_info3 */
    QMI_SI_TLV_SET(0x20)   |  /* wcdma_sys_info3 */
    QMI_SI_TLV_SET(0x21)   |  /* voice_support_on_lte */
    QMI_SI_TLV_SET(0x22)   |  /* gsm_cipher_domain */
    QMI_SI_TLV_SET(0x23)   |  /* wcdma_cipher_domain */
    QMI_SI_TLV_CLEAR(0x24) |  /* tdscdma_srv_status_info */
    QMI_SI_TLV_CLEAR(0x25) |  /* tdscdma_sys_info */
    QMI_SI_TLV_CLEAR(0x26) |  /* lte_embms_coverage */
    QMI_SI_TLV_SET(0x27)   |  /* sim_rej_info */
    QMI_SI_TLV_CLEAR(0x28) |  /* wcdma_eutra_status */
    QMI_SI_TLV_CLEAR(0x29) |   /* lte_ims_voice_avail */
    QMI_SI_TLV_CLEAR(0x2A) |  /* lte_voice_domain */
    QMI_SI_TLV_SET(0x2B)   |  /* cdma_reg_zone */
    QMI_SI_TLV_SET(0x2C)   |  /* gsm_rac */
    QMI_SI_TLV_SET(0x2D)   |  /* wcdma_rac */
    QMI_SI_TLV_SET(0x34)      /* lte_embms_coverage_trace_id */
};

static const qmi_implemented_optionals nas_get_sys_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_sys_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_sys_info_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_sys_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_sys_info_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cdma_srv_status_info */
    QMI_SI_TLV_SET(0x11)   |  /* hdr_srv_status_info */
    QMI_SI_TLV_SET(0x12)   |  /* gsm_srv_status_info */
    QMI_SI_TLV_SET(0x13)   |  /* wcdma_srv_status_info */
    QMI_SI_TLV_SET(0x14)   |  /* lte_srv_status_info */
    QMI_SI_TLV_SET(0x15)   |  /* cdma_sys_info */
    QMI_SI_TLV_SET(0x16)   |  /* hdr_sys_info */
    QMI_SI_TLV_SET(0x17)   |  /* gsm_sys_info */
    QMI_SI_TLV_SET(0x18)   |  /* wcdma_sys_info */
    QMI_SI_TLV_SET(0x19)   |  /* lte_sys_info */
    QMI_SI_TLV_SET(0x1A)   |  /* cdma_sys_info2 */
    QMI_SI_TLV_SET(0x1B)   |  /* hdr_sys_info2 */
    QMI_SI_TLV_SET(0x1C)   |  /* gsm_sys_info2 */
    QMI_SI_TLV_SET(0x1D)   |  /* wcdma_sys_info2 */
    QMI_SI_TLV_SET(0x1E)   |  /* lte_sys_info2 */
    QMI_SI_TLV_SET(0x1F)   |  /* gsm_sys_info3 */
    QMI_SI_TLV_SET(0x20)   |  /* wcdma_sys_info3 */
    QMI_SI_TLV_SET(0x21)   |  /* voice_support_on_lte */
    QMI_SI_TLV_SET(0x22)   |  /* gsm_cipher_domain */
    QMI_SI_TLV_SET(0x23)   |  /* wcdma_cipher_domain */
    QMI_SI_TLV_SET(0x24)   |  /* sys_info_no_change */
    QMI_SI_TLV_CLEAR(0x25) |  /* tdscdma_srv_status_info */
    QMI_SI_TLV_CLEAR(0x26) |  /* tdscdma_sys_info */
    QMI_SI_TLV_CLEAR(0x27) |  /* lte_embms_coverage */
    QMI_SI_TLV_SET(0x28)   |  /* sim_rej_info */
    QMI_SI_TLV_SET(0x29)   |  /* wcdma_eutra_status */
    QMI_SI_TLV_CLEAR(0x2A) |   /* lte_ims_voice_avail */
    QMI_SI_TLV_CLEAR(0x2B) |  /* lte_voice_domain */    
    QMI_SI_TLV_SET(0x2C)   |  /* cdma_reg_zone */
    QMI_SI_TLV_SET(0x2D)   |  /* gsm_rac */
    QMI_SI_TLV_SET(0x2E)   |  /* wcdma_rac */
    QMI_SI_TLV_SET(0x35)      /* lte_embms_coverage_trace_id */
};

static const qmi_implemented_optionals nas_sys_info_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_sys_info_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_sys_info_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_sys_info_ind_msg_impl_v01
};

static const uint32_t nas_get_sig_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cdma_sig_info */
    QMI_SI_TLV_SET(0x11)   |  /* hdr_sig_info */
    QMI_SI_TLV_SET(0x12)   |  /* gsm_sig_info */
    QMI_SI_TLV_SET(0x13)   |  /* wcdma_sig_info */
    QMI_SI_TLV_SET(0x14)   |  /* lte_sig_info */
    QMI_SI_TLV_CLEAR(0x15)      /* rscp */
};

static const qmi_implemented_optionals nas_get_sig_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_sig_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_sig_info_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_sig_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_config_sig_info_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* rssi_threshold_list */
    QMI_SI_TLV_SET(0x11)   |  /* ecio_threshold_list */
    QMI_SI_TLV_SET(0x12)   |  /* hdr_sinr_threshold_list */
    QMI_SI_TLV_SET(0x13)   |  /* lte_snr_threshold_list */
    QMI_SI_TLV_SET(0x14)   |  /* io_threshold_list */
    QMI_SI_TLV_SET(0x15)   |  /* lte_rsrq_threshold_list */
    QMI_SI_TLV_SET(0x16)   |  /* lte_rsrp_threshold_list */
    QMI_SI_TLV_SET(0x17)   |  /* lte_sig_rpt_config */
    QMI_SI_TLV_CLEAR(0x18)      /* rscp_threshold_list */
};

static const qmi_implemented_optionals nas_config_sig_info_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_config_sig_info_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_config_sig_info_reqresp_msg_impl_tuple_v01 = 
{
  &nas_config_sig_info_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t nas_sig_info_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cdma_sig_info */
    QMI_SI_TLV_SET(0x11)   |  /* hdr_sig_info */
    QMI_SI_TLV_SET(0x12)   |  /* gsm_sig_info */
    QMI_SI_TLV_SET(0x13)   |  /* wcdma_sig_info */
    QMI_SI_TLV_SET(0x14)   |  /* lte_sig_info */
    QMI_SI_TLV_CLEAR(0x15)      /* rscp */
};

static const qmi_implemented_optionals nas_sig_info_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_sig_info_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_sig_info_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_sig_info_ind_msg_impl_v01
};

static const uint32_t nas_get_err_rate_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cdma_frame_err_rate */
    QMI_SI_TLV_SET(0x11)   |  /* hdr_packet_err_rate */
    QMI_SI_TLV_SET(0x12)   |  /* gsm_bit_err_rate */
    QMI_SI_TLV_SET(0x13)   |  /* wcdma_block_err_rate */
    QMI_SI_TLV_CLEAR(0x14)      /* tdscdma_block_err_rate */
};

static const qmi_implemented_optionals nas_get_err_rate_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_err_rate_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_err_rate_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_err_rate_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_err_rate_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cdma_frame_err_rate */
    QMI_SI_TLV_SET(0x11)   |  /* hdr_packet_err_rate */
    QMI_SI_TLV_SET(0x12)   |  /* gsm_bit_err_rate */
    QMI_SI_TLV_SET(0x13)   |  /* wcdma_block_err_rate */
    QMI_SI_TLV_CLEAR(0x14)      /* tdscdma_block_err_rate */
};

static const qmi_implemented_optionals nas_err_rate_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_err_rate_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_err_rate_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_err_rate_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple nas_hdr_session_close_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_hdr_uati_update_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_get_hdr_subtype_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* subtype */
};

static const qmi_implemented_optionals nas_get_hdr_subtype_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_hdr_subtype_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_hdr_subtype_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_hdr_subtype_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_get_hdr_color_code_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* color_code */
};

static const qmi_implemented_optionals nas_get_hdr_color_code_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_hdr_color_code_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_hdr_color_code_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_hdr_color_code_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple nas_set_rx_diversity_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_get_tx_rx_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* rx_chain_0 */
    QMI_SI_TLV_SET(0x11)   |  /* rx_chain_1 */
    QMI_SI_TLV_SET(0x12)      /* tx */
};

static const qmi_implemented_optionals nas_get_tx_rx_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_tx_rx_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_tx_rx_info_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_tx_rx_info_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple nas_update_akey_ext_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_get_dual_standby_pref_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* standby_pref */
    QMI_SI_TLV_SET(0x11)   |  /* priority_subs */
    QMI_SI_TLV_SET(0x12)   |  /* active_subs */
    QMI_SI_TLV_SET(0x13)      /* default_data_subs */
};

static const qmi_implemented_optionals nas_get_dual_standby_pref_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_dual_standby_pref_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_dual_standby_pref_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_dual_standby_pref_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple nas_detach_lte_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_block_lte_plmn_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* blocking_interval_abs */
    QMI_SI_TLV_SET(0x11)      /* blocking_interval_mult */
};

static const qmi_implemented_optionals nas_block_lte_plmn_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_block_lte_plmn_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_block_lte_plmn_reqresp_msg_impl_tuple_v01 = 
{
  &nas_block_lte_plmn_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_unblock_lte_plmn_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_reset_lte_plmn_blocking_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_current_plmn_name_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* plmn_id */
    QMI_SI_TLV_SET(0x11)   |  /* spn */
    QMI_SI_TLV_SET(0x12)   |  /* short_name */
    QMI_SI_TLV_SET(0x13)      /* long_name */
};

static const qmi_implemented_optionals nas_current_plmn_name_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_current_plmn_name_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_current_plmn_name_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_current_plmn_name_ind_msg_impl_v01
};

static const uint32_t nas_config_embms_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)     /* trace_id */
};

static const qmi_implemented_optionals nas_config_embms_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_config_embms_req_msg_impl_array_v01
};

static const uint32_t nas_config_embms_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)     /* trace_id */
};

static const qmi_implemented_optionals nas_config_embms_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_config_embms_resp_msg_impl_array_v01
};

static const qmi_implemented_optionals_tuple nas_config_embms_reqresp_msg_impl_tuple_v01 = 
{
  &nas_config_embms_req_msg_impl_v01,
  &nas_config_embms_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_get_embms_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* enabled */
    QMI_SI_TLV_SET(0x11)      /* trace_id */
};

static const qmi_implemented_optionals nas_get_embms_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_embms_status_resp_msg_impl_array_v01
};

static const qmi_implemented_optionals_tuple nas_get_embms_status_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_embms_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_embms_status_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)     /* trace_id */
};

static const qmi_implemented_optionals nas_embms_status_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_embms_status_ind_msg_impl_array_v01
};

static const qmi_implemented_optionals_tuple nas_embms_status_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_embms_status_ind_msg_impl_v01
};

static const uint32_t nas_get_embms_sig_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)     /* trace_id */
};

static const qmi_implemented_optionals nas_get_embms_sig_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_embms_sig_req_msg_impl_array_v01
};

static const uint32_t nas_get_embms_sig_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* trace_id */
    QMI_SI_TLV_SET(0x11)      /* signal_quality */
};

static const qmi_implemented_optionals nas_get_embms_sig_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_embms_sig_resp_msg_impl_array_v01
};

static const qmi_implemented_optionals_tuple nas_get_embms_sig_reqresp_msg_impl_tuple_v01 = 
{
  &nas_get_embms_sig_req_msg_impl_v01,
  &nas_get_embms_sig_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_get_cdma_position_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* info */
};

static const qmi_implemented_optionals nas_get_cdma_position_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_cdma_position_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_cdma_position_info_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_cdma_position_info_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple nas_rf_band_info_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_force_network_search_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple nas_network_reject_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t nas_get_managed_roaming_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)     /* managed_roaming_supported */
};
static const qmi_implemented_optionals nas_get_managed_roaming_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_managed_roaming_config_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_managed_roaming_config_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &nas_get_managed_roaming_config_resp_msg_impl_v01,  
  NULL
};

static const uint32_t nas_rtre_cfg_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* rtre_cfg */
    QMI_SI_TLV_SET(0x11)      /* rtre_cfg_pref */
};

static const qmi_implemented_optionals nas_rtre_cfg_ind_msg_impl_v01 = 
{
  1, /* number of elements in array */
  nas_rtre_cfg_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_rtre_cfg_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &nas_rtre_cfg_ind_msg_impl_v01
};

static const uint32_t nas_get_centralized_eons_support_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)     /* centralized_eons_supported */
};
static const qmi_implemented_optionals nas_get_centralized_eons_support_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  nas_get_centralized_eons_support_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_centralized_eons_support_status_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &nas_get_centralized_eons_support_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t nas_get_tds_cell_and_location_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)|     /* TDS cell information*/
    QMI_SI_TLV_SET(0x11)     /* TDS nbr cell information*/
};
static const qmi_implemented_optionals nas_get_tds_cell_and_location_info_resp_msg_impl_v01=
{
  2, /* number of elements in array */
  nas_get_tds_cell_and_location_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple nas_get_tds_cell_and_location_info_reqresp_msg_impl_tuple_v01=
{
  NULL,
  &nas_get_tds_cell_and_location_info_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple nas_set_hplmn_irat_search_timer_reqresp_msg_impl_tuple_v01 = 
{ 
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple * const nas_service_implemented_array_v01[] =
{
    &nas_reset_reqresp_msg_impl_tuple_v01,               /* 0x0000 */
    &nas_abort_reqresp_msg_impl_tuple_v01,               /* 0x0001 */
    &nas_set_event_report_reqresp_msg_impl_tuple_v01,    /* 0x0002 */
    &nasication_register_reqresp_msg_impl_tuple_v01,     /* 0x0003 */
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
    &nas_get_signal_strength_reqresp_msg_impl_tuple_v01, /* 0x0020 */
    &nas_perform_network_scan_reqresp_msg_impl_tuple_v01, /* 0x0021 */
    &nas_initiate_network_register_reqresp_msg_impl_tuple_v01, /* 0x0022 */
    &nas_initiate_attach_reqresp_msg_impl_tuple_v01,     /* 0x0023 */
    &nas_get_serving_system_reqresp_msg_impl_tuple_v01,  /* 0x0024 */
    &nas_get_home_network_reqresp_msg_impl_tuple_v01,    /* 0x0025 */
    &nas_get_preferred_networks_reqresp_msg_impl_tuple_v01, /* 0x0026 */
    &nas_set_preferred_networks_reqresp_msg_impl_tuple_v01, /* 0x0027 */
    &nas_get_forbidden_networks_reqresp_msg_impl_tuple_v01, /* 0x0028 */
    &nas_set_forbidden_networks_reqresp_msg_impl_tuple_v01, /* 0x0029 */
    &nas_set_technology_preference_reqresp_msg_impl_tuple_v01, /* 0x002A */
    &nas_get_technology_preference_reqresp_type_impl_tuple_v01, /* 0x002B */
    &nas_get_accolc_reqresp_msg_impl_tuple_v01,          /* 0x002C */
    &nas_set_accolc_reqresp_msg_impl_tuple_v01,          /* 0x002D */
    &nas_get_network_system_preference_reqresp_impl_tuple_v01, /* 0x002E */
    &nas_get_device_config_reqresp_msg_impl_tuple_v01,   /* 0x002F */
    &nas_set_device_config_reqresp_msg_impl_tuple_v01,   /* 0x0030 */
    &nas_get_rf_band_info_reqresp_msg_impl_tuple_v01,    /* 0x0031 */
    &nas_get_an_aaa_status_reqresp_msg_impl_tuple_v01,   /* 0x0032 */
    &nas_set_system_selection_preference_reqresp_msg_impl_tuple_v01, /* 0x0033 */
    &nas_get_system_selection_preference_reqresp_msg_impl_tuple_v01, /* 0x0034 */
    NULL,                                                /* 0x0035 */
    NULL,                                                /* 0x0036 */
    &nas_set_ddtm_preference_reqresp_msg_impl_tuple_v01, /* 0x0037 */
    &nas_ddtm_msg_impl_tuple_v01,                        /* 0x0038 */
    &nas_get_operator_name_data_reqresp_msg_impl_tuple_v01, /* 0x0039 */
    &nas_operator_name_data_msg_impl_tuple_v01,          /* 0x003A */
    &nas_get_csp_plmn_mode_bit_reqresp_msg_impl_tuple_v01, /* 0x003B */
    &nas_csp_plmn_mode_bit_msg_impl_tuple_v01,           /* 0x003C */
    NULL,                                               /* 0x003D */
    &nas_get_3gpp2_subscription_info_reqresp_msg_impl_tuple_v01, /* 0x003E */
    &nas_set_3gpp2_subscription_info_reqresp_msg_impl_tuple_v01, /* 0x003F */
    &nas_get_mob_cai_rev_reqresp_impl_tuple_v01,         /* 0x0040 */
    &nas_get_rtre_config_reqresp_impl_tuple_v01,         /* 0x0041 */
    &nas_set_rtre_config_reqresp_impl_tuple_v01,         /* 0x0042 */
    &nas_get_cell_location_info_reqresp_msg_impl_tuple_v01, /* 0x0043 */
    &nas_get_plmn_name_reqresp_msg_impl_tuple_v01,       /* 0x0044 */
    &nas_bind_subscription_reqresp_msg_impl_tuple_v01,   /* 0x0045 */
    &nas_managed_roaming_msg_impl_tuple_v01,             /* 0x0046 */
    &nas_dual_standby_pref_msg_impl_tuple_v01,           /* 0x0047 */
    &nas_subscription_info_msg_impl_tuple_v01,           /* 0x0048 */
    &nas_get_mode_pref_reqresp_msg_impl_tuple_v01,       /* 0x0049 */
    &nas_thermal_emergency_state_msg_impl_tuple_v01,     /* 0x004A */
    &nas_set_dual_standby_pref_reqresp_msg_impl_tuple_v01, /* 0x004B */
    &nas_network_time_msg_impl_tuple_v01,                /* 0x004C */
    &nas_get_sys_info_reqresp_msg_impl_tuple_v01,        /* 0x004D */
    &nas_sys_info_msg_impl_tuple_v01,                    /* 0x004E */
    &nas_get_sig_info_reqresp_msg_impl_tuple_v01,        /* 0x004F */
    &nas_config_sig_info_reqresp_msg_impl_tuple_v01,     /* 0x0050 */
    &nas_sig_info_msg_impl_tuple_v01,                    /* 0x0051 */
    &nas_get_err_rate_reqresp_msg_impl_tuple_v01,        /* 0x0052 */
    &nas_err_rate_msg_impl_tuple_v01,                    /* 0x0053 */
    &nas_hdr_session_close_msg_impl_tuple_v01,           /* 0x0054 */
    &nas_hdr_uati_update_msg_impl_tuple_v01,             /* 0x0055 */
    &nas_get_hdr_subtype_reqresp_msg_impl_tuple_v01,     /* 0x0056 */
    &nas_get_hdr_color_code_reqresp_msg_impl_tuple_v01,  /* 0x0057 */
    NULL,                                                /* 0x0058 */
    &nas_set_rx_diversity_reqresp_msg_impl_tuple_v01,    /* 0x0059 */
    &nas_get_tx_rx_info_reqresp_msg_impl_tuple_v01,      /* 0x005A */
    &nas_update_akey_ext_reqresp_msg_impl_tuple_v01,     /* 0x005B */
    &nas_get_dual_standby_pref_reqresp_msg_impl_tuple_v01, /* 0x005C */
    &nas_detach_lte_reqresp_msg_impl_tuple_v01,          /* 0x005D */
    &nas_block_lte_plmn_reqresp_msg_impl_tuple_v01,      /* 0x005E */
    &nas_unblock_lte_plmn_reqresp_msg_impl_tuple_v01,    /* 0x005F */
    &nas_reset_lte_plmn_blocking_reqresp_msg_impl_tuple_v01, /* 0x0060 */
    &nas_current_plmn_name_msg_impl_tuple_v01,           /* 0x0061 */
    &nas_config_embms_reqresp_msg_impl_tuple_v01,        /* 0x0062 */
    &nas_get_embms_status_reqresp_msg_impl_tuple_v01,    /* 0x0063 */
    &nas_embms_status_msg_impl_tuple_v01,                /* 0x0064 */
    &nas_get_cdma_position_info_reqresp_msg_impl_tuple_v01, /* 0x0065 */
    &nas_rf_band_info_msg_impl_tuple_v01,                /* 0x0066 */
    NULL,                                                /* 0x0067 */
    &nas_network_reject_msg_impl_tuple_v01,              /* 0x0068 */
    &nas_get_managed_roaming_config_reqresp_msg_impl_tuple_v01, /* 0x0069 */
    &nas_rtre_cfg_msg_impl_tuple_v01,                    /* 0x006A*/
    &nas_get_centralized_eons_support_status_reqresp_msg_impl_tuple_v01, /* 0x006B*/
    NULL,
    &nas_get_tds_cell_and_location_info_reqresp_msg_impl_tuple_v01, /* 0x006D */
    &nas_set_hplmn_irat_search_timer_reqresp_msg_impl_tuple_v01,    /* 0x006E */
    &nas_get_embms_sig_reqresp_msg_impl_tuple_v01                   /* 0x006F */

};

static const qmi_implemented_messages nas_service_implemented_v01 =
{
  0x006F,  /* maximum message ID in this service */
  nas_service_implemented_array_v01
};

const qmi_implemented_messages *nas_get_service_impl_v01 (void)
{
  return &nas_service_implemented_v01;
}

