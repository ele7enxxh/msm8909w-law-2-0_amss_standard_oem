/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S P E C I F I C _ A B S O R P T I O N _ R A T E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the sar service Data structures.

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/sar/src/specific_absorption_rate_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2
   It was generated on: Fri May 24 2013 (Spin 0)
   From IDL File: specific_absorption_rate_v01.idl */

#include "qmi_si.h"
#include "specific_absorption_rate_v01.h"

static const uint32_t sar_rf_set_state_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* compatibility_key */
};

static const qmi_implemented_optionals sar_rf_set_state_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  sar_rf_set_state_req_msg_impl_array_v01
};
static const qmi_implemented_optionals sar_rf_set_state_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_sar_rf_set_state_reqresp_msg_impl_tuple_v01 = 
{
  &sar_rf_set_state_req_msg_impl_v01,
  &sar_rf_set_state_resp_msg_impl_v01,
  NULL
};

static const uint32_t sar_rf_get_state_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* sar_rf_state */
};

static const qmi_implemented_optionals sar_rf_get_state_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  sar_rf_get_state_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_sar_rf_get_state_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &sar_rf_get_state_resp_msg_impl_v01,
  NULL
};

static const uint32_t sar_rf_get_compatibility_key_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* compatibility_key */
};

static const qmi_implemented_optionals sar_rf_get_compatibility_key_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  sar_rf_get_compatibility_key_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_sar_get_compatibility_key_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &sar_rf_get_compatibility_key_resp_msg_impl_v01,
  NULL
};

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_sar_get_supported_msgs_reqresp_impl_tuple_v01 = 
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
static const qmi_implemented_optionals_tuple qmi_sar_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const sar_service_implemented_array_v01[] =
{
    NULL,                                                /* 0x0000 */
    &qmi_sar_rf_set_state_reqresp_msg_impl_tuple_v01,    /* 0x0001 */
    &qmi_sar_rf_get_state_reqresp_msg_impl_tuple_v01,    /* 0x0002 */
    &qmi_sar_get_compatibility_key_reqresp_msg_impl_tuple_v01, /* 0x0003 */
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
    &qmi_sar_get_supported_msgs_reqresp_impl_tuple_v01,  /* 0x001E */
    &qmi_sar_get_supported_fields_reqresp_impl_tuple_v01  /* 0x001F */
};

static const qmi_implemented_messages sar_service_implemented_v01 =
{
  0x001F,  /* maximum message ID in this service */
  sar_service_implemented_array_v01
};

const qmi_implemented_messages *sar_get_service_impl_v01 (void)
{
  return &sar_service_implemented_v01;
}
