/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              T I M E _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the time service Data structures.

  Copyright (c) 2012 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/core.mpss/3.5.c12.3/services/time_qmi/src/time_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.1
   It was generated on: Thu Feb 21 2013 (Spin 0)
   From IDL File: time_service_v01.idl */

#include "qmi_si.h"
#include "time_service_v01.h"

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_time_get_supported_msgs_reqresp_impl_tuple_v01 = 
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
static const qmi_implemented_optionals_tuple qmi_time_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const qmi_implemented_optionals time_set_genoff_offset_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals time_set_genoff_offset_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_time_genoff_set_reqresp_impl_tuple_v01 = 
{
  &time_set_genoff_offset_req_msg_impl_v01,
  &time_set_genoff_offset_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals time_get_genoff_offset_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals time_get_genoff_offset_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_time_genoff_get_reqresp_impl_tuple_v01 = 
{
  &time_get_genoff_offset_req_msg_impl_v01,
  &time_get_genoff_offset_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals time_turn_off_ind_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals time_turn_off_ind_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_time_turn_off_reqresp_impl_tuple_v01 = 
{
  &time_turn_off_ind_req_msg_impl_v01,
  &time_turn_off_ind_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals time_turn_on_ind_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals time_turn_on_ind_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_time_turn_on_reqresp_impl_tuple_v01 = 
{
  &time_turn_on_ind_req_msg_impl_v01,
  &time_turn_on_ind_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals time_set_leap_sec_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals time_set_leap_sec_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_time_leap_sec_set_reqresp_impl_tuple_v01 = 
{
  &time_set_leap_sec_req_msg_impl_v01,
  &time_set_leap_sec_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals time_get_leap_sec_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_time_leap_sec_get_reqresp_impl_tuple_v01 = 
{
  NULL,
  &time_get_leap_sec_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals time_update_indication_message_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_time_ats_rtc_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_tod_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_user_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_secure_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_drm_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_user_utc_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_user_tz_dl_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_gps_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_1x_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_hdr_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_wcdma_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple qmi_time_ats_brew_update_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &time_update_indication_message_impl_v01
};

static const qmi_implemented_optionals_tuple * const time_service_implemented_array_v01[] =
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
    &qmi_time_get_supported_msgs_reqresp_impl_tuple_v01, /* 0x001E */
    &qmi_time_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_time_genoff_set_reqresp_impl_tuple_v01,         /* 0x0020 */
    &qmi_time_genoff_get_reqresp_impl_tuple_v01,         /* 0x0021 */
    &qmi_time_turn_off_reqresp_impl_tuple_v01,           /* 0x0022 */
    &qmi_time_turn_on_reqresp_impl_tuple_v01,            /* 0x0023 */
    &qmi_time_leap_sec_set_reqresp_impl_tuple_v01,       /* 0x0024 */
    &qmi_time_leap_sec_get_reqresp_impl_tuple_v01,       /* 0x0025 */
    &qmi_time_ats_rtc_update_impl_tuple_v01,             /* 0x0026 */
    &qmi_time_ats_tod_update_impl_tuple_v01,             /* 0x0027 */
    &qmi_time_ats_user_update_impl_tuple_v01,            /* 0x0028 */
    &qmi_time_ats_secure_update_impl_tuple_v01,          /* 0x0029 */
    &qmi_time_ats_drm_update_impl_tuple_v01,             /* 0x002A */
    &qmi_time_ats_user_utc_update_impl_tuple_v01,        /* 0x002B */
    &qmi_time_ats_user_tz_dl_update_impl_tuple_v01,      /* 0x002C */
    &qmi_time_ats_gps_update_impl_tuple_v01,             /* 0x002D */
    &qmi_time_ats_1x_update_impl_tuple_v01,              /* 0x002E */
    &qmi_time_ats_hdr_update_impl_tuple_v01,             /* 0x002F */
    &qmi_time_ats_wcdma_update_impl_tuple_v01,           /* 0x0030 */
    &qmi_time_ats_brew_update_impl_tuple_v01             /* 0x0031 */
};

static const qmi_implemented_messages time_service_implemented_v01 =
{
  0x0031,  /* maximum message ID in this service */
  time_service_implemented_array_v01
};

const qmi_implemented_messages *time_get_service_impl_v01 (void)
{
  return &time_service_implemented_v01;
}
