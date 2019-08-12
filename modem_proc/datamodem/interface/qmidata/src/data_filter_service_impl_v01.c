/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              D A T A _ F I L T E R _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the dfs service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/data_filter_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.6
   It was generated on: Tue Feb 11 2014 (Spin 0)
   From IDL File: data_filter_service_v01.idl */

#include "qmi_si.h"
#include "data_filter_service_v01.h"

static const uint32_t dfs_get_filter_capability_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* max_media_offload_filters */
    QMI_SI_TLV_SET(0x11)      /* max_pdn_sharing_filters */
};

static const qmi_implemented_optionals dfs_get_filter_capability_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dfs_get_filter_capability_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dfs_get_filter_capability_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dfs_get_filter_capability_resp_msg_impl_v01,
  NULL
};

static const uint32_t dfs_bind_client_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* data_port */
    QMI_SI_TLV_SET(0x11)   |  /* ip_preference */
    QMI_SI_TLV_SET(0x12)   |  /* bind_subs */
    QMI_SI_TLV_SET(0x13)   |  /* ep_id */
    QMI_SI_TLV_SET(0x14)      /* mux_id */
};

static const qmi_implemented_optionals dfs_bind_client_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dfs_bind_client_req_msg_impl_array_v01
};
static const qmi_implemented_optionals dfs_bind_client_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dfs_bind_client_reqresp_impl_tuple_v01 = 
{
  &dfs_bind_client_req_msg_impl_v01,
  &dfs_bind_client_resp_msg_impl_v01,
  NULL
};

static const uint32_t dfs_get_client_binding_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* data_port */
    QMI_SI_TLV_SET(0x11)   |  /* ip_preference */
    QMI_SI_TLV_SET(0x12)   |  /* bound_subs */
    QMI_SI_TLV_SET(0x13)   |  /* bound_ep_id */
    QMI_SI_TLV_SET(0x14)      /* bound_mux_id */
};

static const qmi_implemented_optionals dfs_get_client_binding_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dfs_get_client_binding_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dfs_get_client_binding_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dfs_get_client_binding_resp_msg_impl_v01,
  NULL
};

static const uint32_t dfs_add_media_offload_filter_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* filter_id */
    QMI_SI_TLV_SET(0x11)   |  /* ipv4_dest_address */
    QMI_SI_TLV_SET(0x12)   |  /* ipv6_dest_address */
    QMI_SI_TLV_SET(0x13)   |  /* xport_protocol */
    QMI_SI_TLV_SET(0x14)      /* udp_dest_port */
};

static const qmi_implemented_optionals dfs_add_media_offload_filter_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dfs_add_media_offload_filter_req_msg_impl_array_v01
};
static const uint32_t dfs_add_media_offload_filter_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* filter_handle */
};

static const qmi_implemented_optionals dfs_add_media_offload_filter_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dfs_add_media_offload_filter_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dfs_add_media_offload_filter_reqresp_impl_tuple_v01 = 
{
  &dfs_add_media_offload_filter_req_msg_impl_v01,
  &dfs_add_media_offload_filter_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals dfs_remove_media_offload_filter_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals dfs_remove_media_offload_filter_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dfs_remove_media_offload_filter_reqresp_impl_tuple_v01 = 
{
  &dfs_remove_media_offload_filter_req_msg_impl_v01,
  &dfs_remove_media_offload_filter_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals dfs_get_media_offload_statistics_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t dfs_get_media_offload_statistics_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* bytes_sent */
    QMI_SI_TLV_SET(0x11)   |  /* bytes_received */
    QMI_SI_TLV_SET(0x12)   |  /* packets_sent */
    QMI_SI_TLV_SET(0x13)      /* packets_received */
};

static const qmi_implemented_optionals dfs_get_media_offload_statistics_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dfs_get_media_offload_statistics_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dfs_get_media_offload_statistics_reqresp_impl_tuple_v01 = 
{
  &dfs_get_media_offload_statistics_req_msg_impl_v01,
  &dfs_get_media_offload_statistics_resp_msg_impl_v01,
  NULL
};

static const uint32_t dfs_add_pdn_sharing_filters_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* filter_set */
};

static const qmi_implemented_optionals dfs_add_pdn_sharing_filters_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dfs_add_pdn_sharing_filters_req_msg_impl_array_v01
};
static const uint32_t dfs_add_pdn_sharing_filters_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* filter_handle */
    QMI_SI_TLV_SET(0x11)      /* filter_rule_error */
};

static const qmi_implemented_optionals dfs_add_pdn_sharing_filters_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dfs_add_pdn_sharing_filters_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dfs_add_pdn_sharing_filters_reqresp_impl_tuple_v01 = 
{
  &dfs_add_pdn_sharing_filters_req_msg_impl_v01,
  &dfs_add_pdn_sharing_filters_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals dfs_remove_filters_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals dfs_remove_filters_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dfs_remove_filters_reqresp_impl_tuple_v01 = 
{
  &dfs_remove_filters_req_msg_impl_v01,
  &dfs_remove_filters_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const dfs_service_implemented_array_v01[] =
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
    &qmi_dfs_get_filter_capability_reqresp_impl_tuple_v01, /* 0x0020 */
    &qmi_dfs_bind_client_reqresp_impl_tuple_v01,         /* 0x0021 */
    &qmi_dfs_get_client_binding_reqresp_impl_tuple_v01,  /* 0x0022 */
    &qmi_dfs_add_media_offload_filter_reqresp_impl_tuple_v01, /* 0x0023 */
    &qmi_dfs_remove_media_offload_filter_reqresp_impl_tuple_v01, /* 0x0024 */
    &qmi_dfs_get_media_offload_statistics_reqresp_impl_tuple_v01, /* 0x0025 */
    &qmi_dfs_add_pdn_sharing_filters_reqresp_impl_tuple_v01, /* 0x0026 */
    &qmi_dfs_remove_filters_reqresp_impl_tuple_v01       /* 0x0027 */
};

static const qmi_implemented_messages dfs_service_implemented_v01 =
{
  0x0027,  /* maximum message ID in this service */
  dfs_service_implemented_array_v01
};

const qmi_implemented_messages *dfs_get_service_impl_v01 (void)
{
  return &dfs_service_implemented_v01;
}
