/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              F L A S H _ D R I V E R _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the fds service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fds/src/flash_driver_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2
   It was generated on: Mon Sep 30 2013 (Spin 0)
   From IDL File: flash_driver_service_v01.idl */

#include "qmi_si.h"
#include "flash_driver_service_v01.h"

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_fds_get_supported_msgs_reqresp_impl_tuple_v01 = 
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
static const qmi_implemented_optionals_tuple qmi_fds_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const uint32_t fds_scrub_start_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* scrub_token */
};

static const qmi_implemented_optionals fds_scrub_start_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  fds_scrub_start_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_fds_scrub_start_reqresp_impl_tuple_v01 = 
{
  NULL,
  &fds_scrub_start_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals fds_scrub_get_status_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t fds_scrub_get_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* scrub_status */
};

static const qmi_implemented_optionals fds_scrub_get_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  fds_scrub_get_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_fds_scrub_get_status_reqresp_impl_tuple_v01 = 
{
  &fds_scrub_get_status_req_msg_impl_v01,
  &fds_scrub_get_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t fds_scrub_get_sys_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* fds_internal_scrub_timer_secs */
    QMI_SI_TLV_SET(0x11)   |  /* fds_minimum_secs_between_scrub */
    QMI_SI_TLV_SET(0x12)      /* fds_previous_scrub_time_posix */
};

static const qmi_implemented_optionals fds_scrub_get_sys_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  fds_scrub_get_sys_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_fds_scrub_get_sys_info_reqresp_impl_tuple_v01 = 
{
  NULL,
  &fds_scrub_get_sys_info_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals fds_handle_fota_update_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_fds_handle_fota_update_reqresp_impl_tuple_v01 = 
{
  NULL,
  &fds_handle_fota_update_resp_msg_impl_v01,
  NULL
};

static const uint32_t fds_indication_register_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* need_apps_start_scrub_indication */
};

static const qmi_implemented_optionals fds_indication_register_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  fds_indication_register_req_msg_impl_array_v01
};
static const qmi_implemented_optionals fds_indication_register_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_fdsication_register_reqresp_impl_tuple_v01 = 
{
  &fds_indication_register_req_msg_impl_v01,
  &fds_indication_register_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals fds_scrub_apps_start_scrub_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_fds_scrub_apps_start_scrub_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &fds_scrub_apps_start_scrub_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const fds_service_implemented_array_v01[] =
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
    &qmi_fds_get_supported_msgs_reqresp_impl_tuple_v01,  /* 0x001E */
    &qmi_fds_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_fds_scrub_start_reqresp_impl_tuple_v01,         /* 0x0020 */
    &qmi_fds_scrub_get_status_reqresp_impl_tuple_v01,    /* 0x0021 */
    &qmi_fds_scrub_get_sys_info_reqresp_impl_tuple_v01,  /* 0x0022 */
    &qmi_fds_handle_fota_update_reqresp_impl_tuple_v01,  /* 0x0023 */
    &qmi_fdsication_register_reqresp_impl_tuple_v01,     /* 0x0024 */
    &qmi_fds_scrub_apps_start_scrub_impl_tuple_v01       /* 0x0025 */
};

static const qmi_implemented_messages fds_service_implemented_v01 =
{
  0x0025,  /* maximum message ID in this service */
  fds_service_implemented_array_v01
};

const qmi_implemented_messages *fds_get_service_impl_v01 (void)
{
  return &fds_service_implemented_v01;
}
