/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              W I R E L E S S _ D A T A _ A D M I N I S T R A T I V E _ S E R V I C E _ I M P L _ V 0 1   . C

GENERAL DESCRIPTION
  This is the file which defines the wda service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.  All Rights Reserved. 
 QUALCOMM Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/wireless_data_administrative_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.4
   It was generated on: Thu Jul 26 2012
   From IDL File: wireless_data_administrative_service_v01#10.idl */

#include "qmi_si.h"
#include "wireless_data_administrative_service_v01.h"

static const uint32_t wda_set_data_format_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* qos_format */
    QMI_SI_TLV_SET(0x11)   |  /* link_prot */
    QMI_SI_TLV_SET(0x12)   |  /* ul_data_aggregation_protocol */
    QMI_SI_TLV_SET(0x13)   |  /* dl_data_aggregation_protocol */
    QMI_SI_TLV_SET(0x14)   |  /* ndp_signature */
    QMI_SI_TLV_SET(0x15)   |  /* dl_data_aggregation_max_datagrams */
    QMI_SI_TLV_SET(0x16)      /* dl_data_aggregation_max_size */
};

static const qmi_implemented_optionals wda_set_data_format_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_set_data_format_req_msg_impl_array_v01
};
static const uint32_t wda_set_data_format_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* qos_format */
    QMI_SI_TLV_SET(0x11)   |  /* link_prot */
    QMI_SI_TLV_SET(0x12)   |  /* ul_data_aggregation_protocol */
    QMI_SI_TLV_SET(0x13)   |  /* dl_data_aggregation_protocol */
    QMI_SI_TLV_SET(0x14)   |  /* ndp_signature */
    QMI_SI_TLV_SET(0x15)   |  /* dl_data_aggregation_max_datagrams */
    QMI_SI_TLV_SET(0x16)   |  /* dl_data_aggregation_max_size */
    QMI_SI_TLV_SET(0x17)   |  /* ul_data_aggregation_max_datagrams */
    QMI_SI_TLV_SET(0x18)      /* ul_data_aggregation_max_size */
};

static const qmi_implemented_optionals wda_set_data_format_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_set_data_format_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wda_set_data_format_reqresp_impl_tuple_v01 = 
{
  &wda_set_data_format_req_msg_impl_v01,
  &wda_set_data_format_resp_msg_impl_v01,
  NULL
};

static const uint32_t wda_get_data_format_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* qos_format */
    QMI_SI_TLV_SET(0x11)   |  /* link_prot */
    QMI_SI_TLV_SET(0x12)   |  /* ul_data_aggregation_protocol */
    QMI_SI_TLV_SET(0x13)   |  /* dl_data_aggregation_protocol */
    QMI_SI_TLV_SET(0x14)   |  /* ndp_signature */
    QMI_SI_TLV_SET(0x15)   |  /* dl_data_aggregation_max_datagrams */
    QMI_SI_TLV_SET(0x16)   |  /* dl_data_aggregation_max_size */
    QMI_SI_TLV_SET(0x17)   |  /* ul_data_aggregation_max_datagrams */
    QMI_SI_TLV_SET(0x18)      /* ul_data_aggregation_max_size */
};

static const qmi_implemented_optionals wda_get_data_format_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_get_data_format_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wda_get_data_format_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wda_get_data_format_resp_msg_impl_v01,
  NULL
};

static const uint32_t wda_packet_filter_enable_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ips_id */
};

static const qmi_implemented_optionals wda_packet_filter_enable_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_enable_req_msg_impl_array_v01
};
static const qmi_implemented_optionals wda_packet_filter_enable_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wda_packet_filter_enable_reqresp_impl_tuple_v01 = 
{
  &wda_packet_filter_enable_req_msg_impl_v01,
  &wda_packet_filter_enable_resp_msg_impl_v01,
  NULL
};

static const uint32_t wda_packet_filter_disable_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ips_id */
};

static const qmi_implemented_optionals wda_packet_filter_disable_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_disable_req_msg_impl_array_v01
};
static const qmi_implemented_optionals wda_packet_filter_disable_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wda_packet_filter_disable_reqresp_impl_tuple_v01 = 
{
  &wda_packet_filter_disable_req_msg_impl_v01,
  &wda_packet_filter_disable_resp_msg_impl_v01,
  NULL
};

static const uint32_t wda_packet_filter_get_state_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ips_id */
};

static const qmi_implemented_optionals wda_packet_filter_get_state_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_get_state_req_msg_impl_array_v01
};
static const uint32_t wda_packet_filter_get_state_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* filtering_is_enabled */
    QMI_SI_TLV_SET(0x11)      /* filter_is_restrictive */
};

static const qmi_implemented_optionals wda_packet_filter_get_state_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_get_state_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wda_packet_filter_get_state_reqresp_impl_tuple_v01 = 
{
  &wda_packet_filter_get_state_req_msg_impl_v01,
  &wda_packet_filter_get_state_resp_msg_impl_v01,
  NULL
};

static const uint32_t wda_packet_filter_add_rule_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ips_id */
};

static const qmi_implemented_optionals wda_packet_filter_add_rule_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_add_rule_req_msg_impl_array_v01
};
static const uint32_t wda_packet_filter_add_rule_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* handle */
    QMI_SI_TLV_SET(0x11)      /* rule */
};

static const qmi_implemented_optionals wda_packet_filter_add_rule_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_add_rule_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wda_packet_filter_add_rule_reqresp_impl_tuple_v01 = 
{
  &wda_packet_filter_add_rule_req_msg_impl_v01,
  &wda_packet_filter_add_rule_resp_msg_impl_v01,
  NULL
};

static const uint32_t wda_packet_filter_delete_rule_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* handle */
    QMI_SI_TLV_SET(0x11)      /* ips_id */
};

static const qmi_implemented_optionals wda_packet_filter_delete_rule_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_delete_rule_req_msg_impl_array_v01
};
static const uint32_t wda_packet_filter_delete_rule_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* handle */
};

static const qmi_implemented_optionals wda_packet_filter_delete_rule_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_delete_rule_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wda_packet_filter_delete_rule_reqresp_impl_tuple_v01 = 
{
  &wda_packet_filter_delete_rule_req_msg_impl_v01,
  &wda_packet_filter_delete_rule_resp_msg_impl_v01,
  NULL
};

static const uint32_t wda_packet_filter_get_rule_handles_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ips_id */
};

static const qmi_implemented_optionals wda_packet_filter_get_rule_handles_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_get_rule_handles_req_msg_impl_array_v01
};
static const uint32_t wda_packet_filter_get_rule_handles_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* handle */
};

static const qmi_implemented_optionals wda_packet_filter_get_rule_handles_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_get_rule_handles_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wda_packet_filter_get_rule_handles_reqresp_impl_tuple_v01 = 
{
  &wda_packet_filter_get_rule_handles_req_msg_impl_v01,
  &wda_packet_filter_get_rule_handles_resp_msg_impl_v01,
  NULL
};

static const uint32_t wda_packet_filter_get_rule_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ips_id */
};

static const qmi_implemented_optionals wda_packet_filter_get_rule_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_get_rule_req_msg_impl_array_v01
};
static const uint32_t wda_packet_filter_get_rule_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* handle */
    QMI_SI_TLV_SET(0x11)      /* rule */
};

static const qmi_implemented_optionals wda_packet_filter_get_rule_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_packet_filter_get_rule_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wda_packet_filter_get_rule_reqresp_impl_tuple_v01 = 
{
  &wda_packet_filter_get_rule_req_msg_impl_v01,
  &wda_packet_filter_get_rule_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals wda_set_loopback_state_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals wda_set_loopback_state_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_wda_set_loopback_state_reqresp_impl_tuple_v01 = 
{
  &wda_set_loopback_state_req_msg_impl_v01,
  &wda_set_loopback_state_resp_msg_impl_v01,
  NULL
};

static const uint32_t wda_get_loopback_state_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* loopback_state_is_enabled */
};

static const qmi_implemented_optionals wda_get_loopback_state_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  wda_get_loopback_state_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_wda_get_loopback_state_reqresp_impl_tuple_v01 = 
{
  NULL,
  &wda_get_loopback_state_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const wda_service_implemented_array_v01[] =
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
    NULL,                                                /* 0x001E */
    NULL,                                                /* 0x001F */
    &qmi_wda_set_data_format_reqresp_impl_tuple_v01,     /* 0x0020 */
    &qmi_wda_get_data_format_reqresp_impl_tuple_v01,     /* 0x0021 */
    &qmi_wda_packet_filter_enable_reqresp_impl_tuple_v01, /* 0x0022 */
    &qmi_wda_packet_filter_disable_reqresp_impl_tuple_v01, /* 0x0023 */
    &qmi_wda_packet_filter_get_state_reqresp_impl_tuple_v01, /* 0x0024 */
    &qmi_wda_packet_filter_add_rule_reqresp_impl_tuple_v01, /* 0x0025 */
    &qmi_wda_packet_filter_delete_rule_reqresp_impl_tuple_v01, /* 0x0026 */
    &qmi_wda_packet_filter_get_rule_handles_reqresp_impl_tuple_v01, /* 0x0027 */
    &qmi_wda_packet_filter_get_rule_reqresp_impl_tuple_v01, /* 0x0028 */
    &qmi_wda_set_loopback_state_reqresp_impl_tuple_v01,  /* 0x0029 */
    &qmi_wda_get_loopback_state_reqresp_impl_tuple_v01   /* 0x002A */
};

static const qmi_implemented_messages wda_service_implemented_v01 =
{
  0x002A,  /* maximum message ID in this service */
  wda_service_implemented_array_v01
};

const qmi_implemented_messages *wda_get_service_impl_v01 (void)
{
  return &wda_service_implemented_v01;
}
