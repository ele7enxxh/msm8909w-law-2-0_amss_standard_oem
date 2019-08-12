/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              C A R D _ A P P L I C A T I O N _ T O O L K I T _ I M P L _ V 0 2  . C

GENERAL DESCRIPTION
  This is the file which defines the cat service Data structures.

  Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/card_application_toolkit_impl_v02.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2
   It was generated on: Thu Apr 18 2013 (Spin 0)
   From IDL File: card_application_toolkit_v02.idl */

#include "qmi_si.h"
#include "card_application_toolkit_v02.h"

static const qmi_implemented_optionals cat_reset_resp_msg_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_cat_reset_reqresp_impl_tuple_v02 = 
{
  NULL,
  &cat_reset_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_set_event_report_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* pc_evt_report_req_mask */
    QMI_SI_TLV_SET(0x11)   |  /* pc_dec_evt_report_req_mask */
    QMI_SI_TLV_SET(0x12)   |  /* slot_mask */
    QMI_SI_TLV_SET(0x13)      /* pc_full_func_evt_report_req_mask */
};

static const qmi_implemented_optionals cat_set_event_report_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_set_event_report_req_msg_impl_array_v02
};
static const uint32_t cat_set_event_report_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* pc_evt_report_req_err_mask */
    QMI_SI_TLV_SET(0x11)   |  /* pc_dec_evt_report_req_err_mask */
    QMI_SI_TLV_SET(0x12)      /* pc_full_func_evt_report_err_mask */
};

static const qmi_implemented_optionals cat_set_event_report_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_set_event_report_resp_msg_impl_array_v02
};
static const uint32_t cat_event_report_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* display_text */
    QMI_SI_TLV_SET(0x11)   |  /* get_inkey */
    QMI_SI_TLV_SET(0x12)   |  /* get_input */
    QMI_SI_TLV_SET(0x13)   |  /* setup_menu */
    QMI_SI_TLV_SET(0x14)   |  /* select_item */
    QMI_SI_TLV_SET(0x15)   |  /* pc_alpha_id_available */
    QMI_SI_TLV_SET(0x16)   |  /* pc_setup_evt_list */
    QMI_SI_TLV_SET(0x17)   |  /* idle_mode_text */
    QMI_SI_TLV_SET(0x18)   |  /* lang_notification */
    QMI_SI_TLV_SET(0x19)   |  /* refresh */
    QMI_SI_TLV_SET(0x1A)   |  /* proactive_session_end_type */
    QMI_SI_TLV_SET(0x1B)   |  /* decoded_header */
    QMI_SI_TLV_SET(0x1C)   |  /* text_string */
    QMI_SI_TLV_SET(0x1D)   |  /* high_priority */
    QMI_SI_TLV_SET(0x1E)   |  /* user_control */
    QMI_SI_TLV_SET(0x1F)   |  /* icon */
    QMI_SI_TLV_SET(0x20)   |  /* duration */
    QMI_SI_TLV_SET(0x21)   |  /* response_format */
    QMI_SI_TLV_SET(0x22)   |  /* help_available */
    QMI_SI_TLV_SET(0x23)   |  /* response_packing_format */
    QMI_SI_TLV_SET(0x24)   |  /* response_length */
    QMI_SI_TLV_SET(0x25)   |  /* show_user_input */
    QMI_SI_TLV_SET(0x26)   |  /* tone */
    QMI_SI_TLV_SET(0x27)   |  /* softkey_selection */
    QMI_SI_TLV_SET(0x28)   |  /* items */
    QMI_SI_TLV_SET(0x29)   |  /* default_item */
    QMI_SI_TLV_SET(0x2A)   |  /* next_action_list */
    QMI_SI_TLV_SET(0x2B)   |  /* icon_id_list */
    QMI_SI_TLV_SET(0x2C)   |  /* presentation */
    QMI_SI_TLV_SET(0x2D)   |  /* packing_required */
    QMI_SI_TLV_SET(0x2E)   |  /* sms_tpdu */
    QMI_SI_TLV_SET(0x2F)      /* is_cdma_sms */
  ,

    QMI_SI_TLV_SET(0x30)   |  /* address */
    QMI_SI_TLV_SET(0x31)   |  /* call_setup_requirement */
    QMI_SI_TLV_SET(0x32)   |  /* redial */
    QMI_SI_TLV_SET(0x33)   |  /* subaddress */
    QMI_SI_TLV_SET(0x34)   |  /* capability_config_data */
    QMI_SI_TLV_SET(0x35)   |  /* dtmf_data */
    QMI_SI_TLV_SET(0x36)   |  /* spec_lang_notify */
    QMI_SI_TLV_SET(0x37)   |  /* language */
    QMI_SI_TLV_SET(0x38)   |  /* launch_mode */
    QMI_SI_TLV_SET(0x39)   |  /* url */
    QMI_SI_TLV_SET(0x3A)   |  /* browswer_id */
    QMI_SI_TLV_SET(0x3B)   |  /* bearer_list */
    QMI_SI_TLV_SET(0x3C)   |  /* prov_files */
    QMI_SI_TLV_SET(0x3D)   |  /* ussd_string */
    QMI_SI_TLV_SET(0x3E)   |  /* default_text */
    QMI_SI_TLV_SET(0x3F)   |  /* immediate_resp */
    QMI_SI_TLV_SET(0x40)   |  /* user_conf_alpha */
    QMI_SI_TLV_SET(0x41)   |  /* setup_call_disp_alpha */
    QMI_SI_TLV_SET(0x42)   |  /* user_conf_icon */
    QMI_SI_TLV_SET(0x43)   |  /* setup_call_disp_icon */
    QMI_SI_TLV_SET(0x44)   |  /* gateway_proxy */
    QMI_SI_TLV_SET(0x45)   |  /* alpha */
    QMI_SI_TLV_SET(0x46)   |  /* notification_required */
    QMI_SI_TLV_SET(0x47)   |  /* play_tone */
    QMI_SI_TLV_SET(0x48)   |  /* setup_call */
    QMI_SI_TLV_SET(0x49)   |  /* send_dtmf */
    QMI_SI_TLV_SET(0x4A)   |  /* launch_browser */
    QMI_SI_TLV_SET(0x4B)   |  /* send_sms */
    QMI_SI_TLV_SET(0x4C)   |  /* send_ss */
    QMI_SI_TLV_SET(0x4D)   |  /* send_ussd */
    QMI_SI_TLV_SET(0x4E)   |  /* provide_local_info */
    QMI_SI_TLV_SET(0x4F)      /* setup_event_list_raw */
  ,

    QMI_SI_TLV_SET(0x50)   |  /* slot */
    QMI_SI_TLV_SET(0x51)   |  /* open_channel */
    QMI_SI_TLV_SET(0x52)   |  /* close_channel */
    QMI_SI_TLV_SET(0x53)   |  /* send_data */
    QMI_SI_TLV_SET(0x54)   |  /* receive_data */
    QMI_SI_TLV_SET(0x55)   |  /* on_demand_link */
    QMI_SI_TLV_SET(0x56)   |  /* csd_bearer_description */
    QMI_SI_TLV_SET(0x57)   |  /* gprs_bearer_description */
    QMI_SI_TLV_SET(0x58)   |  /* eutran_ext_param_bearer_description */
    QMI_SI_TLV_SET(0x59)   |  /* eutran_ext_mapped_bearer_description */
    QMI_SI_TLV_SET(0x5A)   |  /* buffer_size */
    QMI_SI_TLV_SET(0x5B)   |  /* network_access_name */
    QMI_SI_TLV_SET(0x5C)   |  /* other_address */
    QMI_SI_TLV_SET(0x5D)   |  /* user_login */
    QMI_SI_TLV_SET(0x5E)   |  /* user_password */
    QMI_SI_TLV_SET(0x5F)   |  /* transport_level */
    QMI_SI_TLV_SET(0x60)   |  /* data_destination_address */
    QMI_SI_TLV_SET(0x61)   |  /* channel_data_length */
    QMI_SI_TLV_SET(0x62)   |  /* send_data_immediately */
    QMI_SI_TLV_SET(0x63)   |  /* channel_data */
    QMI_SI_TLV_SET(0x64)   |  /* channel_id */
    QMI_SI_TLV_SET(0x65)   |  /* items_with_dcs */
    QMI_SI_TLV_SET(0x66)   |  /* activate */
    QMI_SI_TLV_SET(0x67)   |  /* activate_target */
    QMI_SI_TLV_SET(0x68)   |  /* rsp_type */
    QMI_SI_TLV_SET(0x69)   |  /* bip_status */
    QMI_SI_TLV_SET(0x6A)      /* refresh_alpha */
};

static const qmi_implemented_optionals cat_event_report_ind_msg_impl_v02 =
{
  3, /* number of elements in array */
  cat_event_report_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_set_event_report_reqresp_impl_tuple_v02 = 
{
  &cat_set_event_report_req_msg_impl_v02,
  &cat_set_event_report_resp_msg_impl_v02,
  &cat_event_report_ind_msg_impl_v02
};

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v02 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_get_supported_msgs_reqresp_impl_tuple_v02 = 
{
  NULL,
  &qmi_get_supported_msgs_resp_impl_v02,
  NULL
};

static const qmi_implemented_optionals qmi_get_supported_fields_req_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t qmi_get_supported_fields_resp_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* request_fields */
    QMI_SI_TLV_SET(0x11)   |  /* response_fields */
    QMI_SI_TLV_SET(0x12)      /* indication_fields */
};

static const qmi_implemented_optionals qmi_get_supported_fields_resp_impl_v02 =
{
  1, /* number of elements in array */
  qmi_get_supported_fields_resp_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_get_supported_fields_reqresp_impl_tuple_v02 = 
{
  &qmi_get_supported_fields_req_impl_v02,
  &qmi_get_supported_fields_resp_impl_v02,
  NULL
};

static const uint32_t cat_get_service_state_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* decoded_cat_service_state */
    QMI_SI_TLV_SET(0x11)      /* pc_full_func_evt_report_mask */
};

static const qmi_implemented_optionals cat_get_service_state_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_get_service_state_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_get_service_state_reqresp_impl_tuple_v02 = 
{
  NULL,
  &cat_get_service_state_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_send_tr_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* slot */
};

static const qmi_implemented_optionals cat_send_tr_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_send_tr_req_msg_impl_array_v02
};
static const uint32_t cat_send_tr_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* tr_response_data */
};

static const qmi_implemented_optionals cat_send_tr_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_send_tr_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_send_tr_reqresp_impl_tuple_v02 = 
{
  &cat_send_tr_req_msg_impl_v02,
  &cat_send_tr_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_send_envelope_cmd_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* slot */
};

static const qmi_implemented_optionals cat_send_envelope_cmd_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_send_envelope_cmd_req_msg_impl_array_v02
};
static const uint32_t cat_send_envelope_cmd_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* env_resp_data */
};

static const qmi_implemented_optionals cat_send_envelope_cmd_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_send_envelope_cmd_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_send_envelope_cmd_reqresp_impl_tuple_v02 = 
{
  &cat_send_envelope_cmd_req_msg_impl_v02,
  &cat_send_envelope_cmd_resp_msg_impl_v02,
  NULL
};

static const qmi_implemented_optionals cat_get_event_report_req_msg_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t cat_get_event_report_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* display_text */
    QMI_SI_TLV_SET(0x11)   |  /* get_inkey */
    QMI_SI_TLV_SET(0x12)   |  /* get_input */
    QMI_SI_TLV_SET(0x13)   |  /* setup_menu */
    QMI_SI_TLV_SET(0x14)   |  /* select_item */
    QMI_SI_TLV_SET(0x15)   |  /* pc_alpha_id_available */
    QMI_SI_TLV_SET(0x16)   |  /* pc_setup_evt_list */
    QMI_SI_TLV_SET(0x17)   |  /* idle_mode_text */
    QMI_SI_TLV_SET(0x18)   |  /* lang_notification */
    QMI_SI_TLV_SET(0x19)   |  /* refresh */
    QMI_SI_TLV_SET(0x1A)   |  /* proactive_session_end_type */
    QMI_SI_TLV_SET(0x1B)   |  /* decoded_header */
    QMI_SI_TLV_SET(0x1C)   |  /* text_string */
    QMI_SI_TLV_SET(0x1D)   |  /* high_priority */
    QMI_SI_TLV_SET(0x1E)   |  /* user_control */
    QMI_SI_TLV_SET(0x1F)   |  /* icon */
    QMI_SI_TLV_SET(0x20)   |  /* duration */
    QMI_SI_TLV_SET(0x21)   |  /* response_format */
    QMI_SI_TLV_SET(0x22)   |  /* help_available */
    QMI_SI_TLV_SET(0x23)   |  /* response_packing_format */
    QMI_SI_TLV_SET(0x24)   |  /* response_length */
    QMI_SI_TLV_SET(0x25)   |  /* show_user_input */
    QMI_SI_TLV_SET(0x26)   |  /* tone */
    QMI_SI_TLV_SET(0x27)   |  /* softkey_selection */
    QMI_SI_TLV_SET(0x28)   |  /* items */
    QMI_SI_TLV_SET(0x29)   |  /* default_item */
    QMI_SI_TLV_SET(0x2A)   |  /* next_action_list */
    QMI_SI_TLV_SET(0x2B)   |  /* icon_id_list */
    QMI_SI_TLV_SET(0x2C)   |  /* presentation */
    QMI_SI_TLV_SET(0x2D)   |  /* packing_required */
    QMI_SI_TLV_SET(0x2E)   |  /* sms_tpdu */
    QMI_SI_TLV_SET(0x2F)      /* is_cdma_sms */
  ,

    QMI_SI_TLV_SET(0x30)   |  /* address */
    QMI_SI_TLV_SET(0x31)   |  /* call_setup_requirement */
    QMI_SI_TLV_SET(0x32)   |  /* redial */
    QMI_SI_TLV_SET(0x33)   |  /* subaddress */
    QMI_SI_TLV_SET(0x34)   |  /* capability_config_data */
    QMI_SI_TLV_SET(0x35)   |  /* dtmf_data */
    QMI_SI_TLV_SET(0x36)   |  /* spec_lang_notify */
    QMI_SI_TLV_SET(0x37)   |  /* language */
    QMI_SI_TLV_SET(0x38)   |  /* launch_mode */
    QMI_SI_TLV_SET(0x39)   |  /* url */
    QMI_SI_TLV_SET(0x3A)   |  /* browswer_id */
    QMI_SI_TLV_SET(0x3B)   |  /* bearer_list */
    QMI_SI_TLV_SET(0x3C)   |  /* prov_files */
    QMI_SI_TLV_SET(0x3D)   |  /* ussd_string */
    QMI_SI_TLV_SET(0x3E)   |  /* default_text */
    QMI_SI_TLV_SET(0x3F)   |  /* immediate_resp */
    QMI_SI_TLV_SET(0x40)   |  /* user_conf_alpha */
    QMI_SI_TLV_SET(0x41)   |  /* setup_call_disp_alpha */
    QMI_SI_TLV_SET(0x42)   |  /* user_conf_icon */
    QMI_SI_TLV_SET(0x43)   |  /* setup_call_disp_icon */
    QMI_SI_TLV_SET(0x44)   |  /* gateway_proxy */
    QMI_SI_TLV_SET(0x45)   |  /* alpha */
    QMI_SI_TLV_SET(0x46)   |  /* notification_required */
    QMI_SI_TLV_SET(0x47)   |  /* play_tone */
    QMI_SI_TLV_SET(0x48)   |  /* setup_call */
    QMI_SI_TLV_SET(0x49)   |  /* send_dtmf */
    QMI_SI_TLV_SET(0x4A)   |  /* launch_browser */
    QMI_SI_TLV_SET(0x4B)   |  /* send_sms */
    QMI_SI_TLV_SET(0x4C)   |  /* send_ss */
    QMI_SI_TLV_SET(0x4D)   |  /* send_ussd */
    QMI_SI_TLV_SET(0x4E)   |  /* provide_local_info */
    QMI_SI_TLV_SET(0x4F)      /* setup_event_list_raw */
  ,

    QMI_SI_TLV_SET(0x50)   |  /* slot */
    QMI_SI_TLV_SET(0x51)   |  /* open_channel */
    QMI_SI_TLV_SET(0x52)   |  /* close_channel */
    QMI_SI_TLV_SET(0x53)   |  /* send_data */
    QMI_SI_TLV_SET(0x54)   |  /* receive_data */
    QMI_SI_TLV_SET(0x55)   |  /* on_demand_link */
    QMI_SI_TLV_SET(0x56)   |  /* csd_bearer_description */
    QMI_SI_TLV_SET(0x57)   |  /* gprs_bearer_description */
    QMI_SI_TLV_SET(0x58)   |  /* eutran_ext_param_bearer_description */
    QMI_SI_TLV_SET(0x59)   |  /* eutran_ext_mapped_bearer_description */
    QMI_SI_TLV_SET(0x5A)   |  /* buffer_size */
    QMI_SI_TLV_SET(0x5B)   |  /* network_access_name */
    QMI_SI_TLV_SET(0x5C)   |  /* other_address */
    QMI_SI_TLV_SET(0x5D)   |  /* user_login */
    QMI_SI_TLV_SET(0x5E)   |  /* user_password */
    QMI_SI_TLV_SET(0x5F)   |  /* transport_level */
    QMI_SI_TLV_SET(0x60)   |  /* data_destination_address */
    QMI_SI_TLV_SET(0x61)   |  /* channel_data_length */
    QMI_SI_TLV_SET(0x62)   |  /* send_data_immediately */
    QMI_SI_TLV_SET(0x63)   |  /* channel_data */
    QMI_SI_TLV_SET(0x64)   |  /* channel_id */
    QMI_SI_TLV_SET(0x65)   |  /* items_with_dcs */
    QMI_SI_TLV_SET(0x66)   |  /* activate */
    QMI_SI_TLV_SET(0x67)   |  /* activate_target */
    QMI_SI_TLV_SET(0x68)   |  /* rsp_type */
    QMI_SI_TLV_SET(0x69)   |  /* bip_status */
    QMI_SI_TLV_SET(0x6A)      /* refresh_alpha */
};

static const qmi_implemented_optionals cat_get_event_report_resp_msg_impl_v02 =
{
  3, /* number of elements in array */
  cat_get_event_report_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_get_event_report_reqresp_impl_tuple_v02 = 
{
  &cat_get_event_report_req_msg_impl_v02,
  &cat_get_event_report_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_send_decoded_tr_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* text_string */
    QMI_SI_TLV_SET(0x11)   |  /* identifier */
    QMI_SI_TLV_SET(0x12)   |  /* get_inkey_extra_info */
    QMI_SI_TLV_SET(0x13)   |  /* language */
    QMI_SI_TLV_SET(0x14)   |  /* slot */
    QMI_SI_TLV_SET(0x15)      /* gst_inkey_yes_no_info */
};

static const qmi_implemented_optionals cat_send_decoded_tr_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_send_decoded_tr_req_msg_impl_array_v02
};
static const uint32_t cat_send_decoded_tr_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* tr_response_data */
};

static const qmi_implemented_optionals cat_send_decoded_tr_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_send_decoded_tr_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_send_decoded_tr_reqresp_impl_tuple_v02 = 
{
  &cat_send_decoded_tr_req_msg_impl_v02,
  &cat_send_decoded_tr_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_send_decoded_envelope_cmd_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* identifier */
    QMI_SI_TLV_SET(0x11)   |  /* help_request */
    QMI_SI_TLV_SET(0x12)   |  /* language */
    QMI_SI_TLV_SET(0x13)   |  /* slot */
    QMI_SI_TLV_SET(0x14)   |  /* address */
    QMI_SI_TLV_SET(0x15)   |  /* sub_address */
    QMI_SI_TLV_SET(0x16)   |  /* capability_config_param1 */
    QMI_SI_TLV_SET(0x17)   |  /* capability_config_param2 */
    QMI_SI_TLV_SET(0x18)   |  /* ussd_string */
    QMI_SI_TLV_SET(0x19)   |  /* pdp_context_act */
    QMI_SI_TLV_SET(0x1A)   |  /* eps_pdn_connect_act */
    QMI_SI_TLV_SET(0x1B)   |  /* browser_term_cause */
    QMI_SI_TLV_SET(0x1C)   |  /* sms_tpdu */
    QMI_SI_TLV_SET(0x1D)   |  /* is_cdma_sms */
    QMI_SI_TLV_SET(0x1E)   |  /* rat */
    QMI_SI_TLV_SET(0x1F)   |  /* call_type */
    QMI_SI_TLV_SET(0x20)   |  /* transaction_id */
    QMI_SI_TLV_SET(0x21)   |  /* rp_dest_address */
    QMI_SI_TLV_SET(0x22)   |  /* tp_dest_address */
    QMI_SI_TLV_SET(0x23)      /* cause */
};

static const qmi_implemented_optionals cat_send_decoded_envelope_cmd_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_send_decoded_envelope_cmd_req_msg_impl_array_v02
};
static const uint32_t cat_send_decoded_envelope_cmd_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cc_result */
    QMI_SI_TLV_SET(0x11)   |  /* address */
    QMI_SI_TLV_SET(0x12)   |  /* sub_address */
    QMI_SI_TLV_SET(0x13)   |  /* capability_config_param1 */
    QMI_SI_TLV_SET(0x14)   |  /* capability_config_param2 */
    QMI_SI_TLV_SET(0x15)   |  /* ussd_string */
    QMI_SI_TLV_SET(0x16)   |  /* pdp_context_act */
    QMI_SI_TLV_SET(0x17)   |  /* eps_pdn_connect_act */
    QMI_SI_TLV_SET(0x18)   |  /* alpha */
    QMI_SI_TLV_SET(0x19)   |  /* bc_repeat_ind */
    QMI_SI_TLV_SET(0x1A)   |  /* sms_pp_uicc_acknowledge */
    QMI_SI_TLV_SET(0x1B)   |  /* rp_dest_address */
    QMI_SI_TLV_SET(0x1C)      /* tp_dest_address */
};

static const qmi_implemented_optionals cat_send_decoded_envelope_cmd_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_send_decoded_envelope_cmd_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_send_decoded_envelope_cmd_reqresp_impl_tuple_v02 = 
{
  &cat_send_decoded_envelope_cmd_req_msg_impl_v02,
  &cat_send_decoded_envelope_cmd_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_event_confirmation_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* confirm */
    QMI_SI_TLV_SET(0x11)   |  /* display */
    QMI_SI_TLV_SET(0x12)      /* slot */
};

static const qmi_implemented_optionals cat_event_confirmation_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_event_confirmation_req_msg_impl_array_v02
};
static const qmi_implemented_optionals cat_event_confirmation_resp_msg_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_cat_event_confirmation_reqresp_impl_tuple_v02 = 
{
  &cat_event_confirmation_req_msg_impl_v02,
  &cat_event_confirmation_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_scws_open_channel_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* slot */
};

static const qmi_implemented_optionals cat_scws_open_channel_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_scws_open_channel_req_msg_impl_array_v02
};
static const qmi_implemented_optionals cat_scws_open_channel_resp_msg_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t cat_scws_open_channel_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* open_channel_info */
    QMI_SI_TLV_SET(0x11)   |  /* slot */
    QMI_SI_TLV_SET(0x12)      /* alpha */
};

static const qmi_implemented_optionals cat_scws_open_channel_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_scws_open_channel_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_scws_open_channel_reqresp_impl_tuple_v02 = 
{
  &cat_scws_open_channel_req_msg_impl_v02,
  &cat_scws_open_channel_resp_msg_impl_v02,
  &cat_scws_open_channel_ind_msg_impl_v02
};

static const uint32_t cat_scws_close_channel_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* slot */
};

static const qmi_implemented_optionals cat_scws_close_channel_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_scws_close_channel_req_msg_impl_array_v02
};
static const qmi_implemented_optionals cat_scws_close_channel_resp_msg_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t cat_scws_close_channel_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* close_channel_info */
    QMI_SI_TLV_SET(0x11)      /* slot */
};

static const qmi_implemented_optionals cat_scws_close_channel_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_scws_close_channel_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_scws_close_channel_reqresp_impl_tuple_v02 = 
{
  &cat_scws_close_channel_req_msg_impl_v02,
  &cat_scws_close_channel_resp_msg_impl_v02,
  &cat_scws_close_channel_ind_msg_impl_v02
};

static const uint32_t cat_scws_send_data_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* slot */
};

static const qmi_implemented_optionals cat_scws_send_data_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_scws_send_data_req_msg_impl_array_v02
};
static const qmi_implemented_optionals cat_scws_send_data_resp_msg_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t cat_scws_send_data_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* send_data_info */
    QMI_SI_TLV_SET(0x11)      /* slot */
};

static const qmi_implemented_optionals cat_scws_send_data_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_scws_send_data_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_scws_send_data_reqresp_impl_tuple_v02 = 
{
  &cat_scws_send_data_req_msg_impl_v02,
  &cat_scws_send_data_resp_msg_impl_v02,
  &cat_scws_send_data_ind_msg_impl_v02
};

static const uint32_t cat_scws_data_available_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* slot */
};

static const qmi_implemented_optionals cat_scws_data_available_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_scws_data_available_req_msg_impl_array_v02
};
static const qmi_implemented_optionals cat_scws_data_available_resp_msg_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_cat_scws_data_available_reqresp_impl_tuple_v02 = 
{
  &cat_scws_data_available_req_msg_impl_v02,
  &cat_scws_data_available_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_scws_channel_status_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* slot */
};

static const qmi_implemented_optionals cat_scws_channel_status_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_scws_channel_status_req_msg_impl_array_v02
};
static const qmi_implemented_optionals cat_scws_channel_status_resp_msg_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_cat_scws_channel_status_reqresp_impl_tuple_v02 = 
{
  &cat_scws_channel_status_req_msg_impl_v02,
  &cat_scws_channel_status_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_get_terminal_profile_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* slot */
};

static const qmi_implemented_optionals cat_get_terminal_profile_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_get_terminal_profile_req_msg_impl_array_v02
};
static const uint32_t cat_get_terminal_profile_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* terminal_profile_data */
};

static const qmi_implemented_optionals cat_get_terminal_profile_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_get_terminal_profile_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_get_terminal_profile_reqresp_impl_tuple_v02 = 
{
  &cat_get_terminal_profile_req_msg_impl_v02,
  &cat_get_terminal_profile_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_set_configuration_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* custom_tp */
};

static const qmi_implemented_optionals cat_set_configuration_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_set_configuration_req_msg_impl_array_v02
};
static const qmi_implemented_optionals cat_set_configuration_resp_msg_impl_v02 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_cat_set_configuration_reqresp_impl_tuple_v02 = 
{
  &cat_set_configuration_req_msg_impl_v02,
  &cat_set_configuration_resp_msg_impl_v02,
  NULL
};

static const uint32_t cat_get_configuration_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cat_config_mode */
    QMI_SI_TLV_SET(0x11)      /* custom_tp */
};

static const qmi_implemented_optionals cat_get_configuration_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  cat_get_configuration_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple qmi_cat_get_configuration_reqresp_impl_tuple_v02 = 
{
  NULL,
  &cat_get_configuration_resp_msg_impl_v02,
  NULL
};

static const qmi_implemented_optionals_tuple * const cat_service_implemented_array_v02[] =
{
    &qmi_cat_reset_reqresp_impl_tuple_v02,               /* 0x0000 */
    &qmi_cat_set_event_report_reqresp_impl_tuple_v02,    /* 0x0001 */
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
    &qmi_cat_get_supported_msgs_reqresp_impl_tuple_v02,  /* 0x001E */
    &qmi_cat_get_supported_fields_reqresp_impl_tuple_v02, /* 0x001F */
    &qmi_cat_get_service_state_reqresp_impl_tuple_v02,   /* 0x0020 */
    &qmi_cat_send_tr_reqresp_impl_tuple_v02,             /* 0x0021 */
    &qmi_cat_send_envelope_cmd_reqresp_impl_tuple_v02,   /* 0x0022 */
    &qmi_cat_get_event_report_reqresp_impl_tuple_v02,    /* 0x0023 */
    &qmi_cat_send_decoded_tr_reqresp_impl_tuple_v02,     /* 0x0024 */
    &qmi_cat_send_decoded_envelope_cmd_reqresp_impl_tuple_v02, /* 0x0025 */
    &qmi_cat_event_confirmation_reqresp_impl_tuple_v02,  /* 0x0026 */
    &qmi_cat_scws_open_channel_reqresp_impl_tuple_v02,   /* 0x0027 */
    &qmi_cat_scws_close_channel_reqresp_impl_tuple_v02,  /* 0x0028 */
    &qmi_cat_scws_send_data_reqresp_impl_tuple_v02,      /* 0x0029 */
    &qmi_cat_scws_data_available_reqresp_impl_tuple_v02, /* 0x002A */
    &qmi_cat_scws_channel_status_reqresp_impl_tuple_v02, /* 0x002B */
    &qmi_cat_get_terminal_profile_reqresp_impl_tuple_v02, /* 0x002C */
    &qmi_cat_set_configuration_reqresp_impl_tuple_v02,   /* 0x002D */
    &qmi_cat_get_configuration_reqresp_impl_tuple_v02    /* 0x002E */
};

static const qmi_implemented_messages cat_service_implemented_v02 =
{
  0x002E,  /* maximum message ID in this service */
  cat_service_implemented_array_v02
};

const qmi_implemented_messages *cat_get_service_impl_v02 (void)
{
  return &cat_service_implemented_v02;
}
