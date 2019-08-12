/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              U S E R _ I D E N T I T Y _ M O D U L E _ R E M O T E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the uim_remote service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/user_identity_module_remote_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5
   It was generated on: Sat Nov  9 2013 (Spin 0)
   From IDL File: user_identity_module_remote_v01.idl */

#include "qmi_si.h"
#include "user_identity_module_remote_v01.h"

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_get_supported_msgs_reqresp_impl_tuple_v01 = 
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
static const qmi_implemented_optionals_tuple qmi_uim_remote_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_remote_reset_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_reset_reqresp_impl_tuple_v01 = 
{
  NULL,
  &uim_remote_reset_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_remote_event_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* atr */
    QMI_SI_TLV_SET(0x11)   |  /* wakeup_support */
    QMI_SI_TLV_SET(0x12)      /* error_cause */
};

static const qmi_implemented_optionals uim_remote_event_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_remote_event_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_remote_event_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_event_reqresp_impl_tuple_v01 = 
{
  &uim_remote_event_req_msg_impl_v01,
  &uim_remote_event_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_remote_apdu_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* response_apdu_info */
    QMI_SI_TLV_SET(0x11)      /* response_apdu_segment */
};

static const qmi_implemented_optionals uim_remote_apdu_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_remote_apdu_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_remote_apdu_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_remote_apdu_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_apdu_reqresp_impl_tuple_v01 = 
{
  &uim_remote_apdu_req_msg_impl_v01,
  &uim_remote_apdu_resp_msg_impl_v01,
  &uim_remote_apdu_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_remote_connect_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_connect_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_remote_connect_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_remote_disconnect_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_disconnect_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_remote_disconnect_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_remote_card_power_up_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_card_power_up_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_remote_card_power_up_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_remote_card_power_down_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_card_power_down_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_remote_card_power_down_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_remote_card_reset_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_card_reset_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_remote_card_reset_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const uim_remote_service_implemented_array_v01[] =
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
    &qmi_uim_remote_get_supported_msgs_reqresp_impl_tuple_v01, /* 0x001E */
    &qmi_uim_remote_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_uim_remote_reset_reqresp_impl_tuple_v01,        /* 0x0020 */
    &qmi_uim_remote_event_reqresp_impl_tuple_v01,        /* 0x0021 */
    &qmi_uim_remote_apdu_reqresp_impl_tuple_v01,         /* 0x0022 */
    &qmi_uim_remote_connect_impl_tuple_v01,              /* 0x0023 */
    &qmi_uim_remote_disconnect_impl_tuple_v01,           /* 0x0024 */
    &qmi_uim_remote_card_power_up_impl_tuple_v01,        /* 0x0025 */
    &qmi_uim_remote_card_power_down_impl_tuple_v01,      /* 0x0026 */
    &qmi_uim_remote_card_reset_impl_tuple_v01            /* 0x0027 */
};

static const qmi_implemented_messages uim_remote_service_implemented_v01 =
{
  0x0027,  /* maximum message ID in this service */
  uim_remote_service_implemented_array_v01
};

const qmi_implemented_messages *uim_remote_get_service_impl_v01 (void)
{
  return &uim_remote_service_implemented_v01;
}
