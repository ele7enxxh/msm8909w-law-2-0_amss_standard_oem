/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              A U T H E N T I C A T I O N _ S E R V I C E _ I M P L _ V 0 1   . C

GENERAL DESCRIPTION
  This is the file which defines the auth service Data structures.

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved. 
 QUALCOMM Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/authentication_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.4
   It was generated on: Thu Jul 26 2012
   From IDL File: authentication_service_v01#3.idl */

#include "qmi_si.h"
#include "authentication_service_v01.h"

static const qmi_implemented_optionals_tuple qmi_auth_reset_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t auth_start_eap_session_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* eap_method_mask */
    QMI_SI_TLV_SET(0x11)   |  /* user_id */
    QMI_SI_TLV_SET(0x12)   |  /* eap_meta_identity */
    QMI_SI_TLV_SET(0x13)      /* eap_sim_aka_algo */
};

static const qmi_implemented_optionals auth_start_eap_session_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  auth_start_eap_session_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_auth_start_eap_session_reqresp_impl_tuple_v01 = 
{
  &auth_start_eap_session_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_auth_send_eap_packet_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_auth_eap_session_result_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_auth_get_eap_session_keys_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_auth_end_eap_session_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t auth_run_aka_algo_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* aka_params */
};

static const qmi_implemented_optionals auth_run_aka_algo_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  auth_run_aka_algo_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_auth_run_aka_algo_reqresp_impl_tuple_v01 = 
{
  &auth_run_aka_algo_req_msg_impl_v01,
  NULL,
  NULL
};

static const uint32_t auth_aka_algo_result_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* aka_response_data */
};

static const qmi_implemented_optionals auth_aka_algo_result_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  auth_aka_algo_result_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_auth_aka_algo_result_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &auth_aka_algo_result_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const auth_service_implemented_array_v01[] =
{
    &qmi_auth_reset_reqresp_impl_tuple_v01,              /* 0x0000 */
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
    &qmi_auth_start_eap_session_reqresp_impl_tuple_v01,  /* 0x0020 */
    &qmi_auth_send_eap_packet_reqresp_impl_tuple_v01,    /* 0x0021 */
    &qmi_auth_eap_session_result_impl_tuple_v01,         /* 0x0022 */
    &qmi_auth_get_eap_session_keys_reqresp_impl_tuple_v01, /* 0x0023 */
    &qmi_auth_end_eap_session_reqresp_impl_tuple_v01,    /* 0x0024 */
    &qmi_auth_run_aka_algo_reqresp_impl_tuple_v01,       /* 0x0025 */
    &qmi_auth_aka_algo_result_impl_tuple_v01             /* 0x0026 */
};

static const qmi_implemented_messages auth_service_implemented_v01 =
{
  0x0026,  /* maximum message ID in this service */
  auth_service_implemented_array_v01
};

const qmi_implemented_messages *auth_get_service_impl_v01 (void)
{
  return &auth_service_implemented_v01;
}
