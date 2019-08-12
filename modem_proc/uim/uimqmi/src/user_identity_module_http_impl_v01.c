/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              U S E R _ I D E N T I T Y _ M O D U L E _ H T T P _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the uim_http service Data structures.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/user_identity_module_http_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Thu Apr 14 2016 (Spin 0)
   From IDL File: user_identity_module_http_v01.idl */

#include "qmi_si.h"
#include "user_identity_module_http_v01.h"

static const qmi_implemented_optionals uim_http_reset_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_http_reset_reqresp_impl_tuple_v01 = 
{
  NULL,
  &uim_http_reset_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_http_transaction_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* headers */
    QMI_SI_TLV_SET(0x11)   |  /* segment_info */
    QMI_SI_TLV_SET(0x12)      /* payload_body */
};

static const qmi_implemented_optionals uim_http_transaction_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_http_transaction_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_http_transaction_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_http_transaction_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* url */
    QMI_SI_TLV_SET(0x11)   |  /* headers */
    QMI_SI_TLV_SET(0x12)   |  /* segment_info */
    QMI_SI_TLV_SET(0x13)      /* payload_body */
};

static const qmi_implemented_optionals uim_http_transaction_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_http_transaction_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_http_transaction_reqresp_impl_tuple_v01 = 
{
  &uim_http_transaction_req_msg_impl_v01,
  &uim_http_transaction_resp_msg_impl_v01,
  &uim_http_transaction_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const uim_http_service_implemented_array_v01[] =
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
    &qmi_uim_http_reset_reqresp_impl_tuple_v01,          /* 0x0020 */
    &qmi_uim_http_transaction_reqresp_impl_tuple_v01     /* 0x0021 */
};

static const qmi_implemented_messages uim_http_service_implemented_v01 =
{
  0x0021,  /* maximum message ID in this service */
  uim_http_service_implemented_array_v01
};

const qmi_implemented_messages *uim_http_get_service_impl_v01 (void)
{
  return &uim_http_service_implemented_v01;
}
