/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              V O I C E _ S E R V I C E _ I M P L _ V 0 2  . C

GENERAL DESCRIPTION
  This is the file which defines the voice service Data structures.

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved. 
 QUALCOMM Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/voice_service_impl_v02.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.0
   It was generated on: Thu Feb  2 2012
   From IDL File: voice_service_v02.idl */

#include "qmi_si.h"
#include "voice_service_v02.h"

static const uint32_t voice_indication_register_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* reg_dtmf_events */
    QMI_SI_TLV_SET(0x11)   |  /* reg_voice_privacy_events */
    QMI_SI_TLV_SET(0x12)   |   /* supps_notification_events */
    QMI_SI_TLV_SET(0x13)   |   /* call_events */
    QMI_SI_TLV_SET(0x14)   |    /* handover_notification_events */
    QMI_SI_TLV_SET(0x15)   |    /* speech_events */
    QMI_SI_TLV_SET(0x16)   |    /* ussd_events */
    QMI_SI_TLV_SET(0x17)   |    /* other_sups_events */
    QMI_SI_TLV_SET(0x18)   |    /* modify_events */
    QMI_SI_TLV_SET(0x19)        /* uus_events */
};

static const qmi_implemented_optionals voice_indication_register_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_indication_register_req_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voiceication_register_reqresp_msg_impl_tuple_v02 = 
{
  &voice_indication_register_req_msg_impl_v02,
  NULL,
  NULL
};

/******* <QMI_VOICE_DIAL_CALL> *********/
static const uint32_t voice_dial_call_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_type */
    QMI_SI_TLV_SET(0x11)   |  /* clir_type */
    QMI_SI_TLV_SET(0x12)   |  /* uus */
    QMI_SI_TLV_SET(0x13)   |  /* cug */
    QMI_SI_TLV_SET(0x14)   |  /* emer_cat */
    QMI_SI_TLV_SET(0x15)   |  /* called_party_subaddress */
    QMI_SI_TLV_SET(0x16)   |  /* service_type */
    QMI_SI_TLV_SET(0x17)   |  /* sip_uri_overflow */
    QMI_SI_TLV_SET(0x18)   |  /* audio_attribute */
    QMI_SI_TLV_SET(0x19)      /* video_attribute */
    
};

static const qmi_implemented_optionals voice_dial_call_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_dial_call_req_msg_impl_array_v02
};
/******* </QMI_VOICE_DIAL_CALL> *********/

static const uint32_t voice_dial_call_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |   /* call_id */
    QMI_SI_TLV_SET(0x11)   |   /* alpha_ident */
    QMI_SI_TLV_SET(0x12)   |   /* cc_result_type */
    QMI_SI_TLV_SET(0x13)   |   /* cc_sups_result */
    QMI_SI_TLV_SET(0x14)       /* end_reason */
};

static const qmi_implemented_optionals voice_dial_call_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_dial_call_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_dial_call_reqresp_msg_impl_tuple_v02 = 
{
  &voice_dial_call_req_msg_impl_v02,
  &voice_dial_call_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_end_call_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_id */
};

static const qmi_implemented_optionals voice_end_call_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_end_call_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_end_call_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_end_call_resp_msg_impl_v02,
  NULL
};

/******* <QMI_VOICE_ANSWER_CALL> *********/
static const uint32_t voice_answer_call_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_type */
    QMI_SI_TLV_SET(0x11)   |  /* audio_attrib */
    QMI_SI_TLV_SET(0x12)      /* video_attrib */
};

static const qmi_implemented_optionals voice_answer_call_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_answer_call_req_msg_impl_array_v02
};

static const uint32_t voice_answer_call_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_id */
};

static const qmi_implemented_optionals voice_answer_call_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_answer_call_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_answer_call_reqresp_msg_impl_tuple_v02 = 
{
  &voice_answer_call_req_msg_impl_v02,
  &voice_answer_call_resp_msg_impl_v02,
  NULL
};
/******* </QMI_VOICE_ANSWER_CALL> *********/

/******* <QMI_VOICE_GET_CALL_INFO> *********/
static const uint32_t voice_get_call_info_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_info */
    QMI_SI_TLV_SET(0x11)   |  /* remote_party_number */
    QMI_SI_TLV_SET(0x12)   |  /* srv_opt */
    QMI_SI_TLV_SET(0x13)   |  /* voice_privacy */
    QMI_SI_TLV_SET(0x14)   |  /* otasp_status */
    QMI_SI_TLV_SET(0x15)   |  /* remote_party_name */
    QMI_SI_TLV_SET(0x16)   |  /* uus */
    QMI_SI_TLV_SET(0x17)   |  /* alerting_type */
    QMI_SI_TLV_SET(0x18)   |  /* alpha_ident */
    QMI_SI_TLV_SET(0x19)   |  /* conn_num_info */
    QMI_SI_TLV_SET(0x1A)   |  /* diagnostic_info */
    QMI_SI_TLV_SET(0x1B)   |  /* alerting_pattern */
    QMI_SI_TLV_SET(0x1C)   |  /* audio_attrib */
    QMI_SI_TLV_SET(0x1D)      /* video_attrib */
};

static const qmi_implemented_optionals voice_get_call_info_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_call_info_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_call_info_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_get_call_info_resp_msg_impl_v02,
  NULL
};
/******* </QMI_VOICE_GET_CALL_INFO> *********/

static const qmi_implemented_optionals_tuple voice_otasp_status_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t voice_info_rec_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* signal_info */
    QMI_SI_TLV_SET(0x11)   |  /* caller_id_info */
    QMI_SI_TLV_SET(0x12)   |  /* display_buffer */
    QMI_SI_TLV_SET(0x13)   |  /* ext_display_buffer */
    QMI_SI_TLV_SET(0x14)   |  /* caller_name */
    QMI_SI_TLV_SET(0x15)   |  /* call_waiting */
    QMI_SI_TLV_SET(0x16)   |  /* conn_num_info */
    QMI_SI_TLV_SET(0x17)   |  /* calling_party_info */
    QMI_SI_TLV_SET(0x18)   |  /* called_party_info */
    QMI_SI_TLV_SET(0x19)   |  /* redirecting_num_info */
    QMI_SI_TLV_SET(0x1A)   |  /* clir_cause */
    QMI_SI_TLV_SET(0x1B)   |  /* audio_control */
    QMI_SI_TLV_SET(0x1C)   |  /* nss_release */
    QMI_SI_TLV_SET(0x1D)   |  /* line_control */
    QMI_SI_TLV_SET(0x1E)      /* ext_display_record */
};

static const qmi_implemented_optionals voice_info_rec_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_info_rec_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_info_rec_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &voice_info_rec_ind_msg_impl_v02
};

static const uint32_t voice_send_flash_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* flash_payload */
    QMI_SI_TLV_SET(0x11)      /* flash_type */
};

static const qmi_implemented_optionals voice_send_flash_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_send_flash_req_msg_impl_array_v02
};
static const uint32_t voice_send_flash_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_id */
};

static const qmi_implemented_optionals voice_send_flash_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_send_flash_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_send_flash_reqresp_msg_impl_tuple_v02 = 
{
  &voice_send_flash_req_msg_impl_v02,
  &voice_send_flash_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_burst_dtmf_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* dtmf_lengths */
};

static const qmi_implemented_optionals voice_burst_dtmf_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_burst_dtmf_req_msg_impl_array_v02
};
static const uint32_t voice_burst_dtmf_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_id */
};

static const qmi_implemented_optionals voice_burst_dtmf_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_burst_dtmf_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_burst_dtmf_reqresp_msg_impl_tuple_v02 = 
{
  &voice_burst_dtmf_req_msg_impl_v02,
  &voice_burst_dtmf_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_start_cont_dtmf_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_id */
};

static const qmi_implemented_optionals voice_start_cont_dtmf_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_start_cont_dtmf_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_start_cont_dtmf_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_start_cont_dtmf_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_stop_cont_dtmf_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_id */
};

static const qmi_implemented_optionals voice_stop_cont_dtmf_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_stop_cont_dtmf_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_stop_cont_dtmf_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_stop_cont_dtmf_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_dtmf_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* on_length */
    QMI_SI_TLV_SET(0x11)      /* off_length */
};

static const qmi_implemented_optionals voice_dtmf_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_dtmf_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_dtmf_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &voice_dtmf_ind_msg_impl_v02
};

static const qmi_implemented_optionals_tuple voice_set_preferred_privacy_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple voice_privacy_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

/******* <QMI_VOICE_ALL_CALL_STATUS_IND> *********/
static const uint32_t voice_all_call_status_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* remote_party_number */
    QMI_SI_TLV_SET(0x11)   |  /* remote_party_name */
    QMI_SI_TLV_SET(0x12)   |  /* alerting_type */
    QMI_SI_TLV_SET(0x13)   |  /* srv_opt */
    QMI_SI_TLV_SET(0x14)   |  /* call_end_reason */
    QMI_SI_TLV_SET(0x15)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x16)   |  /* conn_party_num */
    QMI_SI_TLV_SET(0x17)   |  /* diagnostic_info */
    QMI_SI_TLV_SET(0x18)   |  /* called_party_num */
    QMI_SI_TLV_SET(0x19)   |  /* redirecting_party_num */
    QMI_SI_TLV_SET(0x1A)   |  /* alerting_pattern */
    QMI_SI_TLV_SET(0x1B)   |  /* audio_attrib */
    QMI_SI_TLV_SET(0x1C)      /* video_attrib */
};

static const qmi_implemented_optionals voice_all_call_status_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_all_call_status_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_all_call_status_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &voice_all_call_status_ind_msg_impl_v02
};
/******* </QMI_VOICE_ALL_CALL_STATUS_IND> *********/

/******* <QMI_VOICE_GET_ALL_CALL_INFO> *********/
static const uint32_t voice_get_all_call_info_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_info */
    QMI_SI_TLV_SET(0x11)   |  /* remote_party_number */
    QMI_SI_TLV_SET(0x12)   |  /* remote_party_name */
    QMI_SI_TLV_SET(0x13)   |  /* alerting_type */
    QMI_SI_TLV_SET(0x14)   |  /* uus_info */
    QMI_SI_TLV_SET(0x15)   |  /* srv_opt */
    QMI_SI_TLV_SET(0x16)   |  /* otasp_status */
    QMI_SI_TLV_SET(0x17)   |  /* voice_privacy */
    QMI_SI_TLV_SET(0x18)   |  /* call_end_reason */
    QMI_SI_TLV_SET(0x19)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x1A)   |  /* conn_party_num */
    QMI_SI_TLV_SET(0x1B)   |  /* diagnostic_info */
    QMI_SI_TLV_SET(0x1C)   |  /* called_party_num */
    QMI_SI_TLV_SET(0x1D)   |  /* redirecting_party_num */
    QMI_SI_TLV_SET(0x1E)   |  /* alerting_pattern */
    QMI_SI_TLV_SET(0x1F)   |  /* audio_attrib */
    QMI_SI_TLV_SET(0x20)      /* video_attrib */
};

static const qmi_implemented_optionals voice_get_all_call_info_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_all_call_info_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_all_call_info_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_get_all_call_info_resp_msg_impl_v02,
  NULL
};
/******* </QMI_VOICE_GET_ALL_CALL_INFO> *********/

static const uint32_t voice_manage_calls_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_id */
};

static const qmi_implemented_optionals voice_manage_calls_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_manage_calls_req_msg_impl_array_v02
};
static const uint32_t voice_manage_calls_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* failure_cause */
};

static const qmi_implemented_optionals voice_manage_calls_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_manage_calls_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_manage_calls_reqresp_msg_impl_tuple_v02 = 
{
  &voice_manage_calls_req_msg_impl_v02,
  &voice_manage_calls_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_sups_notification_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* index */
    QMI_SI_TLV_SET(0x11)      /* ect_number */
};

static const qmi_implemented_optionals voice_sups_notification_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_sups_notification_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_sups_notification_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &voice_sups_notification_ind_msg_impl_v02
};

static const uint32_t voice_set_sups_service_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* service_class */
    QMI_SI_TLV_SET(0x11)   |  /* password */
    QMI_SI_TLV_SET(0x12)   |  /* number */
    QMI_SI_TLV_SET(0x13)   |  /* timer_value */
    QMI_SI_TLV_SET(0x14)      /* num_type_plan */
};

static const qmi_implemented_optionals voice_set_sups_service_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_set_sups_service_req_msg_impl_array_v02
};
static const uint32_t voice_set_sups_service_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x11)   |  /* alpha_ident */
    QMI_SI_TLV_SET(0x12)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x13)   |  /* call_id */
    QMI_SI_TLV_SET(0x14)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_set_sups_service_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_set_sups_service_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_set_sups_service_reqresp_msg_impl_tuple_v02 = 
{
  &voice_set_sups_service_req_msg_impl_v02,
  &voice_set_sups_service_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_get_call_waiting_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* service_class */
};

static const qmi_implemented_optionals voice_get_call_waiting_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_call_waiting_req_msg_impl_array_v02
};
static const uint32_t voice_get_call_waiting_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* service_class */
    QMI_SI_TLV_SET(0x11)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x12)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x13)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x14)   |  /* call_id */
    QMI_SI_TLV_SET(0x15)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_get_call_waiting_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_call_waiting_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_call_waiting_reqresp_msg_impl_tuple_v02 = 
{
  &voice_get_call_waiting_req_msg_impl_v02,
  &voice_get_call_waiting_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_get_call_barring_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* service_class */
};

static const qmi_implemented_optionals voice_get_call_barring_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_call_barring_req_msg_impl_array_v02
};
static const uint32_t voice_get_call_barring_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* service_class */
    QMI_SI_TLV_SET(0x11)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x12)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x13)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x14)   |  /* call_id */
    QMI_SI_TLV_SET(0x15)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_get_call_barring_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_call_barring_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_call_barring_reqresp_msg_impl_tuple_v02 = 
{
  &voice_get_call_barring_req_msg_impl_v02,
  &voice_get_call_barring_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_get_clip_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* clip_response */
    QMI_SI_TLV_SET(0x11)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x12)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x13)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x14)   |  /* call_id */
    QMI_SI_TLV_SET(0x15)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_get_clip_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_clip_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_clip_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_get_clip_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_get_clir_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* clir_response */
    QMI_SI_TLV_SET(0x11)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x12)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x13)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x14)   |  /* call_id */
    QMI_SI_TLV_SET(0x15)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_get_clir_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_clir_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_clir_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_get_clir_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_get_call_forwarding_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* service_class */
};

static const qmi_implemented_optionals voice_get_call_forwarding_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_call_forwarding_req_msg_impl_array_v02
};
static const uint32_t voice_get_call_forwarding_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* get_call_forwarding_info */
    QMI_SI_TLV_SET(0x11)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x12)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x13)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x14)   |  /* call_id */
    QMI_SI_TLV_SET(0x15)   |  /* cc_sups_result */
    QMI_SI_TLV_SET(0x16)      /* get_call_forwarding_exten_info */
};

static const qmi_implemented_optionals voice_get_call_forwarding_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_call_forwarding_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_call_forwarding_reqresp_msg_impl_tuple_v02 = 
{
  &voice_get_call_forwarding_req_msg_impl_v02,
  &voice_get_call_forwarding_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_set_call_barring_password_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x11)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x12)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x13)   |  /* call_id */
    QMI_SI_TLV_SET(0x14)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_set_call_barring_password_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_set_call_barring_password_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_set_call_barring_password_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_set_call_barring_password_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_orig_ussd_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x11)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x12)   |  /* uss_info */
    QMI_SI_TLV_SET(0x13)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x14)   |  /* call_id */
    QMI_SI_TLV_SET(0x15)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_orig_ussd_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_orig_ussd_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_orig_ussd_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_orig_ussd_resp_msg_impl_v02,
  NULL
};

static const qmi_implemented_optionals_tuple voice_answer_ussd_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple voice_cancel_ussd_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple voice_ussd_release_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t voice_ussd_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* uss_info */
};

static const qmi_implemented_optionals voice_ussd_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_ussd_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_ussd_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &voice_ussd_ind_msg_impl_v02
};

static const qmi_implemented_optionals_tuple voice_uus_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t voice_set_config_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* auto_answer */
    QMI_SI_TLV_SET(0x11)   |  /* air_timer */
    QMI_SI_TLV_SET(0x12)   |  /* roam_timer */
    QMI_SI_TLV_SET(0x13)   |  /* tty_mode */
    QMI_SI_TLV_SET(0x14)   |  /* preferred_voice_so */
    QMI_SI_TLV_SET(0x15)      /* voice_domain */
};

static const qmi_implemented_optionals voice_set_config_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_set_config_req_msg_impl_array_v02
};
static const uint32_t voice_set_config_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* auto_answer_outcome */
    QMI_SI_TLV_SET(0x11)   |  /* air_timer_outcome */
    QMI_SI_TLV_SET(0x12)   |  /* roam_timer_outcome */
    QMI_SI_TLV_SET(0x13)   |  /* tty_mode_outcome */
    QMI_SI_TLV_SET(0x14)   |  /* pref_voice_so_outcome */
    QMI_SI_TLV_SET(0x15)      /* voice_domain_pref_outcome */
};

static const qmi_implemented_optionals voice_set_config_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_set_config_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_set_config_reqresp_msg_impl_tuple_v02 = 
{
  &voice_set_config_req_msg_impl_v02,
  &voice_set_config_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_get_config_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* auto_answer */
    QMI_SI_TLV_SET(0x11)   |  /* air_timer */
    QMI_SI_TLV_SET(0x12)   |  /* roam_timer */
    QMI_SI_TLV_SET(0x13)   |  /* tty_mode */
    QMI_SI_TLV_SET(0x14)   |  /* pref_voice_so */
    QMI_SI_TLV_SET(0x15)   |  /* amr_status */
    QMI_SI_TLV_SET(0x16)   |  /* voice_privacy */
    QMI_SI_TLV_SET(0x17)   |  /* nam_id */
    QMI_SI_TLV_SET(0x18)      /* voice_domain_pref */
};

static const qmi_implemented_optionals voice_get_config_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_config_req_msg_impl_array_v02
};
static const uint32_t voice_get_config_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* auto_answer_status */
    QMI_SI_TLV_SET(0x11)   |  /* air_timer_count */
    QMI_SI_TLV_SET(0x12)   |  /* roam_timer_count */
    QMI_SI_TLV_SET(0x13)   |  /* current_tty_mode */
    QMI_SI_TLV_SET(0x14)   |  /* current_preferred_voice_so */
    QMI_SI_TLV_SET(0x15)   |  /* current_arm_config */
    QMI_SI_TLV_SET(0x16)   |  /* current_voice_privacy_pref */
    QMI_SI_TLV_SET(0x17)      /* voice_domain */
};

static const qmi_implemented_optionals voice_get_config_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_config_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_config_reqresp_msg_impl_tuple_v02 = 
{
  &voice_get_config_req_msg_impl_v02,
  &voice_get_config_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_sups_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* service_class */
    QMI_SI_TLV_SET(0x11)   |  /* reason */
    QMI_SI_TLV_SET(0x12)   |  /* number */
    QMI_SI_TLV_SET(0x13)   |  /* timer_value */
    QMI_SI_TLV_SET(0x14)   |  /* uss_info */
    QMI_SI_TLV_SET(0x15)   |  /* call_id */
    QMI_SI_TLV_SET(0x16)   |  /* alpha_ident */
    QMI_SI_TLV_SET(0x17)   |  /* password */
    QMI_SI_TLV_SET(0x18)   |  /* new_password */
    QMI_SI_TLV_SET(0x19)   |  /* data_source */
    QMI_SI_TLV_SET(0x1A)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x1B)   |  /* call_forwarding_info */
    QMI_SI_TLV_SET(0x1C)   |  /* clir_status */
    QMI_SI_TLV_SET(0x1D)   |  /* clip_status */
    QMI_SI_TLV_SET(0x1E)   |  /* colp_status */
    QMI_SI_TLV_SET(0x1F)   |  /* colr_status */
    QMI_SI_TLV_SET(0x20)      /* cnap_status */
};

static const qmi_implemented_optionals voice_sups_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_sups_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_sups_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &voice_sups_ind_msg_impl_v02
};

static const uint32_t voice_orig_ussd_no_wait_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* error */
    QMI_SI_TLV_SET(0x11)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x12)   |  /* uss_info */
    QMI_SI_TLV_SET(0x13)      /* alpha_id */
};

static const qmi_implemented_optionals voice_orig_ussd_no_wait_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_orig_ussd_no_wait_ind_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_orig_ussd_no_wait_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  &voice_orig_ussd_no_wait_ind_msg_impl_v02
};

static const qmi_implemented_optionals_tuple voice_bind_subscription_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple voice_als_set_line_switching_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple voice_als_select_line_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple voice_aoc_reset_acm_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple voice_aoc_set_acmmax_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t voice_aoc_get_call_meter_info_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* acm */
    QMI_SI_TLV_SET(0x11)   |  /* acmmax */
    QMI_SI_TLV_SET(0x12)      /* ccm */
};

static const qmi_implemented_optionals voice_aoc_get_call_meter_info_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_aoc_get_call_meter_info_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_aoc_get_call_meter_info_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_aoc_get_call_meter_info_resp_msg_impl_v02,
  NULL
};

static const qmi_implemented_optionals_tuple voice_aoc_low_funds_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t voice_get_colp_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* colp_response */
    QMI_SI_TLV_SET(0x11)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x12)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x13)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x14)   |  /* call_id */
    QMI_SI_TLV_SET(0x15)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_get_colp_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_colp_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_colp_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_get_colp_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_get_colr_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* colr_response */
    QMI_SI_TLV_SET(0x11)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x12)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x13)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x14)   |  /* call_id */
    QMI_SI_TLV_SET(0x15)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_get_colr_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_colr_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_colr_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_get_colr_resp_msg_impl_v02,
  NULL
};

static const uint32_t voice_get_cnap_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* cnap_response */
    QMI_SI_TLV_SET(0x11)   |  /* failure_cause */
    QMI_SI_TLV_SET(0x12)   |  /* alpha_id */
    QMI_SI_TLV_SET(0x13)   |  /* cc_result_type */
    QMI_SI_TLV_SET(0x14)   |  /* call_id */
    QMI_SI_TLV_SET(0x15)      /* cc_sups_result */
};

static const qmi_implemented_optionals voice_get_cnap_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_cnap_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_cnap_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_get_cnap_resp_msg_impl_v02,
  NULL
};

/******* <QMI_VOICE_MANAGE_IP_CALLS> *********/
static const uint32_t voice_manage_ip_calls_req_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_id */
    QMI_SI_TLV_SET(0x11)   |  /* call_type */
    QMI_SI_TLV_SET(0x12)   |  /* audio_attrib */
    QMI_SI_TLV_SET(0x13)   |  /* video_attrib */    
    QMI_SI_TLV_SET(0x14)      /* sip_uri */
};

static const qmi_implemented_optionals voice_manage_ip_calls_req_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_manage_ip_calls_req_msg_impl_array_v02
};

static const uint32_t voice_manage_ip_calls_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |   /* call_id */
    QMI_SI_TLV_SET(0x11)       /* failure_cause */
};

static const qmi_implemented_optionals voice_manage_ip_calls_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_manage_ip_calls_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_manage_ip_calls_reqresp_msg_impl_tuple_v02 = 
{
  &voice_manage_ip_calls_req_msg_impl_v02,
  &voice_manage_ip_calls_resp_msg_impl_v02,
  NULL
};
/******* </QMI_VOICE_MANAGE_IP_CALLS> *********/

/******* <QMI_VOICE_ALS_GET_LINE_SWITCHING_STATUS> *********/
static const uint32_t voice_get_line_switching_status_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* switch_value */
};

static const qmi_implemented_optionals voice_get_line_switching_status_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_line_switching_status_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_line_switching_status_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_get_line_switching_status_resp_msg_impl_v02,
  NULL
};
/******* </QMI_VOICE_ALS_GET_LINE_SWITCHING_STATUS> *********/

/******* <QMI_VOICE_ALS_GET_SELECTED_LINE> *********/
static const uint32_t voice_get_selected_line_resp_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)      /* line_value */
};

static const qmi_implemented_optionals voice_get_selected_line_resp_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_get_selected_line_resp_msg_impl_array_v02
};
static const qmi_implemented_optionals_tuple voice_get_selected_line_reqresp_msg_impl_tuple_v02 = 
{
  NULL,
  &voice_get_selected_line_resp_msg_impl_v02,
  NULL
};
/******* </QMI_VOICE_ALS_GET_SELECTED_LINE> *********/


/******* <QMI_VOICE_MODIFIED_IND> *********/
static const uint32_t voice_call_modified_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_type */
    QMI_SI_TLV_SET(0x11)   |  /* audio_attrib */
    QMI_SI_TLV_SET(0x12)      /* video_attrib */    
};

static const qmi_implemented_optionals voice_call_modified_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_call_modified_ind_msg_impl_array_v02
};

static const qmi_implemented_optionals_tuple voice_call_modified_msg_impl_tuple_v02 = 
{
  NULL,  
  NULL,
  &voice_call_modified_ind_msg_impl_v02
};
/******* </QMI_VOICE_MODIFIED_IND> *********/


/******* <QMI_VOICE_MODIFY_ACCEPT_IND> *********/
static const uint32_t voice_call_modify_accept_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_type */
    QMI_SI_TLV_SET(0x11)   |  /* audio_attrib */
    QMI_SI_TLV_SET(0x12)      /* video_attrib */    
};

static const qmi_implemented_optionals voice_call_modify_accept_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_call_modify_accept_ind_msg_impl_array_v02
};

static const qmi_implemented_optionals_tuple voice_call_modify_accept_msg_impl_tuple_v02 = 
{
  NULL,  
  NULL,
  &voice_call_modify_accept_ind_msg_impl_v02
};
/******* </QMI_VOICE_MODIFIED_IND> *********/

/******* <QMI_VOICE_SPEECH_CODEC_INFO_IND> *********/
static const uint32_t voice_speech_codec_info_ind_msg_impl_array_v02[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* network_mode */
    QMI_SI_TLV_SET(0x11)   |  /* speech_codec */
    QMI_SI_TLV_SET(0x12)      /* sample_rate */    
};

static const qmi_implemented_optionals voice_speech_codec_info_ind_msg_impl_v02 =
{
  1, /* number of elements in array */
  voice_speech_codec_info_ind_msg_impl_array_v02
};

static const qmi_implemented_optionals_tuple voice_speech_codec_info_reqresp_msg_impl_tuple_v02 = 
{
  NULL,  
  NULL,
  &voice_speech_codec_info_ind_msg_impl_v02
};
/******* </QMI_VOICE_SPEECH_CODEC_INFO_IND> *********/

/********<QMI_VOICE_HANDOVER_IND>********************/

static const qmi_implemented_optionals_tuple voice_handover_msg_impl_tuple_v02 = 
{
  NULL,
  NULL,
  NULL
};
/********</QMI_VOICE_HANDOVER_IND>********************/

static const qmi_implemented_optionals_tuple * const voice_service_implemented_array_v02[] =
{
    NULL,                                                /* 0x0000 */
    NULL,                                                /* 0x0001 */
    NULL,                                                /* 0x0002 */
    &voiceication_register_reqresp_msg_impl_tuple_v02,   /* 0x0003 */
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
    &voice_dial_call_reqresp_msg_impl_tuple_v02,         /* 0x0020 */
    &voice_end_call_reqresp_msg_impl_tuple_v02,          /* 0x0021 */
    &voice_answer_call_reqresp_msg_impl_tuple_v02,       /* 0x0022 */
    NULL,                                                /* 0x0023 */
    &voice_get_call_info_reqresp_msg_impl_tuple_v02,     /* 0x0024 */
    &voice_otasp_status_msg_impl_tuple_v02,              /* 0x0025 */
    &voice_info_rec_msg_impl_tuple_v02,                  /* 0x0026 */
    &voice_send_flash_reqresp_msg_impl_tuple_v02,        /* 0x0027 */
    &voice_burst_dtmf_reqresp_msg_impl_tuple_v02,        /* 0x0028 */
    &voice_start_cont_dtmf_reqresp_msg_impl_tuple_v02,   /* 0x0029 */
    &voice_stop_cont_dtmf_reqresp_msg_impl_tuple_v02,    /* 0x002A */
    &voice_dtmf_msg_impl_tuple_v02,                      /* 0x002B */
    &voice_set_preferred_privacy_reqresp_msg_impl_tuple_v02, /* 0x002C */
    &voice_privacy_msg_impl_tuple_v02,                   /* 0x002D */
    &voice_all_call_status_msg_impl_tuple_v02,           /* 0x002E */
    &voice_get_all_call_info_reqresp_msg_impl_tuple_v02, /* 0x002F */
    NULL,                                                /* 0x0030 */
    &voice_manage_calls_reqresp_msg_impl_tuple_v02,      /* 0x0031 */
    &voice_sups_notification_msg_impl_tuple_v02,         /* 0x0032 */
    &voice_set_sups_service_reqresp_msg_impl_tuple_v02,  /* 0x0033 */
    &voice_get_call_waiting_reqresp_msg_impl_tuple_v02,  /* 0x0034 */
    &voice_get_call_barring_reqresp_msg_impl_tuple_v02,  /* 0x0035 */
    &voice_get_clip_reqresp_msg_impl_tuple_v02,          /* 0x0036 */
    &voice_get_clir_reqresp_msg_impl_tuple_v02,          /* 0x0037 */
    &voice_get_call_forwarding_reqresp_msg_impl_tuple_v02, /* 0x0038 */
    &voice_set_call_barring_password_reqresp_msg_impl_tuple_v02, /* 0x0039 */
    &voice_orig_ussd_reqresp_msg_impl_tuple_v02,         /* 0x003A */
    &voice_answer_ussd_reqresp_msg_impl_tuple_v02,       /* 0x003B */
    &voice_cancel_ussd_reqresp_msg_impl_tuple_v02,       /* 0x003C */
    &voice_ussd_release_msg_impl_tuple_v02,              /* 0x003D */
    &voice_ussd_msg_impl_tuple_v02,                      /* 0x003E */
    &voice_uus_msg_impl_tuple_v02,                       /* 0x003F */
    &voice_set_config_reqresp_msg_impl_tuple_v02,        /* 0x0040 */
    &voice_get_config_reqresp_msg_impl_tuple_v02,        /* 0x0041 */
    &voice_sups_msg_impl_tuple_v02,                      /* 0x0042 */
    &voice_orig_ussd_no_wait_reqresp_msg_impl_tuple_v02, /* 0x0043 */
    &voice_bind_subscription_reqresp_msg_impl_tuple_v02, /* 0x0044 */
    &voice_als_set_line_switching_reqresp_msg_impl_tuple_v02, /* 0x0045 */
    &voice_als_select_line_reqresp_msg_impl_tuple_v02,   /* 0x0046 */
    &voice_aoc_reset_acm_reqresp_msg_impl_tuple_v02,     /* 0x0047 */
    &voice_aoc_set_acmmax_reqresp_msg_impl_tuple_v02,    /* 0x0048 */
    &voice_aoc_get_call_meter_info_reqresp_msg_impl_tuple_v02, /* 0x0049 */
    &voice_aoc_low_funds_msg_impl_tuple_v02,             /* 0x004A */
    &voice_get_colp_reqresp_msg_impl_tuple_v02,          /* 0x004B */
    &voice_get_colr_reqresp_msg_impl_tuple_v02,          /* 0x004C */
    &voice_get_cnap_reqresp_msg_impl_tuple_v02,          /* 0x004D */
    &voice_manage_ip_calls_reqresp_msg_impl_tuple_v02,    /* 0x004E */
    &voice_get_line_switching_status_reqresp_msg_impl_tuple_v02,  /* 0x004F */
    &voice_get_selected_line_reqresp_msg_impl_tuple_v02,  /* 0x0050 */
    &voice_call_modified_msg_impl_tuple_v02,               /* 0x0051 */
    &voice_call_modify_accept_msg_impl_tuple_v02,          /* 0x0052 */
    &voice_speech_codec_info_reqresp_msg_impl_tuple_v02,   /* 0x0053 */
    &voice_handover_msg_impl_tuple_v02                     /* 0x0054 */
};

static const qmi_implemented_messages voice_service_implemented_v02 =
{
  0x0054,  /* maximum message ID in this service */
  voice_service_implemented_array_v02
};

const qmi_implemented_messages *voice_get_service_impl_v02 (void)
{
  return &voice_service_implemented_v02;
}
