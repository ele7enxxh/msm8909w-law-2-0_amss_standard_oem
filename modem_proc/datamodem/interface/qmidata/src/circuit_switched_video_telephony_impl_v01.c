/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              C I R C U I T _ S W I T C H E D _ V I D E O _ T E L E P H O N Y _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the csvt service Data structures.

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved. 
 QUALCOMM Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/circuit_switched_video_telephony_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.3
   It was generated on: Thu Jul 19 2012
   From IDL File: circuit_switched_video_telephony_v01.idl */

#include "qmi_si.h"
#include "circuit_switched_video_telephony_v01.h"

static const qmi_implemented_optionals_tuple csvt_reset_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t csvt_set_event_report_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* report_call_events */
    QMI_SI_TLV_SET(0x11)      /* call_types */
};

static const qmi_implemented_optionals csvt_set_event_report_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_set_event_report_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_set_event_report_reqresp_msg_impl_tuple_v01 = 
{
  &csvt_set_event_report_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t csvt_event_report_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_type */
    QMI_SI_TLV_SET(0x11)   |  /* synchronous_call */
    QMI_SI_TLV_SET(0x12)   |  /* transparent_call */
    QMI_SI_TLV_SET(0x13)   |  /* network_type */
    QMI_SI_TLV_SET(0x14)   |  /* network_speed */
    QMI_SI_TLV_SET(0x15)   |  /* max_frame_size */
    QMI_SI_TLV_SET(0x16)   |  /* incoming_number */
    QMI_SI_TLV_SET(0x17)   |  /* uus_id */
    QMI_SI_TLV_SET(0x18)   |  /* modify_allowed */
    QMI_SI_TLV_SET(0x19)   |  /* call_end_cause */
    QMI_SI_TLV_SET(0x1A)      /* port_data */
};

static const qmi_implemented_optionals csvt_event_report_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_event_report_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_event_report_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &csvt_event_report_ind_msg_impl_v01
};

static const uint32_t csvt_originate_call_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* network_datarate */
    QMI_SI_TLV_SET(0x11)   |  /* air_interface_datarate */
    QMI_SI_TLV_SET(0x12)   |  /* synchronous_call */
    QMI_SI_TLV_SET(0x13)   |  /* transparent_call */
    QMI_SI_TLV_SET(0x14)   |  /* cli_enabled */
    QMI_SI_TLV_SET(0x15)   |  /* cug_enabled */
    QMI_SI_TLV_SET(0x16)   |  /* cug_index */
    QMI_SI_TLV_SET(0x17)   |  /* supress_preferred_cug */
    QMI_SI_TLV_SET(0x18)   |  /* supress_outgoing_access */
    QMI_SI_TLV_SET(0x19)      /* uus_id */
};

static const qmi_implemented_optionals csvt_originate_call_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_originate_call_req_msg_impl_array_v01
};
static const uint32_t csvt_originate_call_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_end_cause */
};

static const qmi_implemented_optionals csvt_originate_call_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_originate_call_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_originate_call_reqresp_msg_impl_tuple_v01 = 
{
  &csvt_originate_call_req_msg_impl_v01,
  NULL,
  &csvt_originate_call_ind_msg_impl_v01
};

static const uint32_t csvt_confirm_call_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_end_cause */
};

static const qmi_implemented_optionals csvt_confirm_call_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_confirm_call_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_confirm_call_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &csvt_confirm_call_ind_msg_impl_v01
};

static const uint32_t csvt_answer_call_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_end_cause */
};

static const qmi_implemented_optionals csvt_answer_call_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_answer_call_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_answer_call_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &csvt_answer_call_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple csvt_end_call_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t csvt_modify_call_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_end_cause */
};

static const qmi_implemented_optionals csvt_modify_call_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_modify_call_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_modify_call_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &csvt_modify_call_ind_msg_impl_v01
};

static const uint32_t csvt_ack_call_modify_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* call_end_cause */
};

static const qmi_implemented_optionals csvt_ack_call_modify_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_ack_call_modify_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_ack_call_modify_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &csvt_ack_call_modify_ind_msg_impl_v01
};

static const uint32_t csvt_get_rlp_params_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* rlp1_parameters */
    QMI_SI_TLV_SET(0x11)   |  /* v42_parameters */
    QMI_SI_TLV_SET(0x12)   |  /* rlp2_parameters */
    QMI_SI_TLV_SET(0x13)      /* rlp3_parameters */
};

static const qmi_implemented_optionals csvt_get_rlp_params_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_get_rlp_params_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_get_rlp_params_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &csvt_get_rlp_params_resp_msg_impl_v01,
  NULL
};

static const uint32_t csvt_set_rlp_params_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* rlp_parameters */
    QMI_SI_TLV_SET(0x11)      /* v42_parameters */
};

static const qmi_implemented_optionals csvt_set_rlp_params_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_set_rlp_params_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_set_rlp_params_reqresp_msg_impl_tuple_v01 = 
{
  &csvt_set_rlp_params_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t csvt_get_active_call_list_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* active_call_inst_id */
};

static const qmi_implemented_optionals csvt_get_active_call_list_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_get_active_call_list_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_get_active_call_list_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &csvt_get_active_call_list_resp_msg_impl_v01,
  NULL
};

static const uint32_t csvt_get_call_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_type */
    QMI_SI_TLV_SET(0x11)   |  /* synchronous_call */
    QMI_SI_TLV_SET(0x12)   |  /* transparent_call */
    QMI_SI_TLV_SET(0x13)   |  /* network_type */
    QMI_SI_TLV_SET(0x14)   |  /* network_speed */
    QMI_SI_TLV_SET(0x15)   |  /* max_frame_size */
    QMI_SI_TLV_SET(0x16)   |  /* incoming_number */
    QMI_SI_TLV_SET(0x17)   |  /* uus_id */
    QMI_SI_TLV_SET(0x18)   |  /* modify_allowed */
    QMI_SI_TLV_SET(0x19)   |  /* call_end_cause */
    QMI_SI_TLV_SET(0x1A)      /* port_data */
};

static const qmi_implemented_optionals csvt_get_call_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_get_call_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_get_call_info_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &csvt_get_call_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t csvt_get_call_stats_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* call_active */
    QMI_SI_TLV_SET(0x11)   |  /* tx_counter */
    QMI_SI_TLV_SET(0x12)      /* rx_counter */
};

static const qmi_implemented_optionals csvt_get_call_stats_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  csvt_get_call_stats_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple csvt_get_call_stats_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &csvt_get_call_stats_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const csvt_service_implemented_array_v01[] =
{
    NULL,                                                /* 0x0000 */
    &csvt_reset_reqresp_msg_impl_tuple_v01,              /* 0x0001 */
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
    &csvt_set_event_report_reqresp_msg_impl_tuple_v01,   /* 0x0020 */
    &csvt_event_report_msg_impl_tuple_v01,               /* 0x0021 */
    &csvt_originate_call_reqresp_msg_impl_tuple_v01,     /* 0x0022 */
    &csvt_confirm_call_reqresp_msg_impl_tuple_v01,       /* 0x0023 */
    &csvt_answer_call_reqresp_msg_impl_tuple_v01,        /* 0x0024 */
    &csvt_end_call_reqresp_msg_impl_tuple_v01,           /* 0x0025 */
    &csvt_modify_call_reqresp_msg_impl_tuple_v01,        /* 0x0026 */
    &csvt_ack_call_modify_reqresp_msg_impl_tuple_v01,    /* 0x0027 */
    &csvt_get_rlp_params_reqresp_msg_impl_tuple_v01,     /* 0x0028 */
    &csvt_set_rlp_params_reqresp_msg_impl_tuple_v01,     /* 0x0029 */
    &csvt_get_active_call_list_reqresp_msg_impl_tuple_v01, /* 0x002A */
    &csvt_get_call_info_reqresp_msg_impl_tuple_v01,      /* 0x002B */
    &csvt_get_call_stats_reqresp_msg_impl_tuple_v01      /* 0x002C */
};

static const qmi_implemented_messages csvt_service_implemented_v01 =
{
  0x002C,  /* maximum message ID in this service */
  csvt_service_implemented_array_v01
};

const qmi_implemented_messages *csvt_get_service_impl_v01 (void)
{
  return &csvt_service_implemented_v01;
}
