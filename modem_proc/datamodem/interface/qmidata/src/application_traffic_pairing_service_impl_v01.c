/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              A P P L I C A T I O N _ T R A F F I C _ P A I R I N G _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the atp service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved. 
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/application_traffic_pairing_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2
   It was generated on: Wed Sep 11 2013 (Spin 0)
   From IDL File: application_traffic_pairing_service_v01.idl */

#include "qmi_si.h"
#include "application_traffic_pairing_service_impl_v01.h"

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_atp_get_supported_msgs_reqresp_impl_tuple_v01 = 
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
static const qmi_implemented_optionals_tuple qmi_atp_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const uint32_t atp_indication_register_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* report_activation_status */
    QMI_SI_TLV_SET(0x11)      /* report_policy_info */
};

static const qmi_implemented_optionals atp_indication_register_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  atp_indication_register_req_msg_impl_array_v01
};
static const uint32_t atp_indication_register_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* activation_status */
    QMI_SI_TLV_SET(0x11)      /* policy_info */
};

static const qmi_implemented_optionals atp_indication_register_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  atp_indication_register_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_atpication_register_reqresp_impl_tuple_v01 = 
{
  &atp_indication_register_req_msg_impl_v01,
  &atp_indication_register_resp_msg_impl_v01,
  NULL
};

static const uint32_t atp_activation_status_query_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* is_activated */
};

static const qmi_implemented_optionals atp_activation_status_query_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  atp_activation_status_query_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_atp_activation_status_query_reqresp_impl_tuple_v01 = 
{
  NULL,
  &atp_activation_status_query_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals atp_activation_status_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_atp_activation_status_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &atp_activation_status_ind_msg_impl_v01
};

static const qmi_implemented_optionals atp_policy_info_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_atp_policy_info_query_reqresp_impl_tuple_v01 = 
{
  NULL,
  &atp_policy_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t atp_policy_info_change_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* atp_policy_list */
};

static const qmi_implemented_optionals atp_policy_info_change_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  atp_policy_info_change_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_atp_policy_info_change_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &atp_policy_info_change_ind_msg_impl_v01
};

static const uint32_t atp_send_filter_report_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* action */
    QMI_SI_TLV_SET(0x11)   |  /* service_id */
    QMI_SI_TLV_SET(0x12)   |  /* package_name */
    QMI_SI_TLV_SET(0x13)   |  /* hash_values */
    QMI_SI_TLV_SET(0x14)   |  /* ip_type */
    QMI_SI_TLV_SET(0x15)   |  /* dest_ipv4_addr */
    QMI_SI_TLV_SET(0x16)   |  /* dest_ipv6_addr */
    QMI_SI_TLV_SET(0x17)   |  /* dest_port */
    QMI_SI_TLV_SET(0x18)   |  /* src_ipv4_addr */
    QMI_SI_TLV_SET(0x19)   |  /* src_ipv6_addr */
    QMI_SI_TLV_SET(0x1A)   |  /* src_port */
    QMI_SI_TLV_SET(0x1B)      /* protocol */
};

static const qmi_implemented_optionals atp_send_filter_report_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  atp_send_filter_report_req_msg_impl_array_v01
};
static const qmi_implemented_optionals atp_send_filter_report_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t atp_send_filter_report_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* filter_report_resp */
};

static const qmi_implemented_optionals atp_send_filter_report_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  atp_send_filter_report_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_atp_send_filter_report_reqresp_impl_tuple_v01 = 
{
  &atp_send_filter_report_req_msg_impl_v01,
  &atp_send_filter_report_resp_msg_impl_v01,
  &atp_send_filter_report_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const atp_service_implemented_array_v01[] =
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
    &qmi_atp_get_supported_msgs_reqresp_impl_tuple_v01,  /* 0x001E */
    &qmi_atp_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_atpication_register_reqresp_impl_tuple_v01,     /* 0x0020 */
    &qmi_atp_activation_status_query_reqresp_impl_tuple_v01, /* 0x0021 */
    &qmi_atp_activation_status_impl_tuple_v01,           /* 0x0022 */
    &qmi_atp_policy_info_query_reqresp_impl_tuple_v01,   /* 0x0023 */
    &qmi_atp_policy_info_change_impl_tuple_v01,          /* 0x0024 */
    &qmi_atp_send_filter_report_reqresp_impl_tuple_v01   /* 0x0025 */
};

static const qmi_implemented_messages atp_service_implemented_v01 =
{
  0x0025,  /* maximum message ID in this service */
  atp_service_implemented_array_v01
};

const qmi_implemented_messages *atp_get_service_impl_v01 (void)
{
  return &atp_service_implemented_v01;
}
