/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              T H E R M A L _ S E N S O R _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the ts service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/core.mpss/3.5.c12.3/power/thermal/src/common/thermal_sensor_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.1
   It was generated on: Sat Mar  2 2013 (Spin 0)
   From IDL File: thermal_sensor_service_v01.idl */

#include "qmi_si.h"
#include "thermal_sensor_service_v01.h"

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_ts_get_supported_msgs_reqresp_impl_tuple_v01 = 
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
static const qmi_implemented_optionals_tuple qmi_ts_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const uint32_t ts_get_sensor_list_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* sensor_list */
};

static const qmi_implemented_optionals ts_get_sensor_list_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  ts_get_sensor_list_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_ts_get_sensor_list_reqresp_impl_tuple_v01 = 
{
  NULL,
  &ts_get_sensor_list_resp_msg_impl_v01,
  NULL
};

static const uint32_t ts_register_notification_temp_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* temp_threshold_high */
    QMI_SI_TLV_SET(0x11)   |  /* temp_threshold_low */
    QMI_SI_TLV_SET(0x12)      /* seq_num */
};

static const qmi_implemented_optionals ts_register_notification_temp_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  ts_register_notification_temp_req_msg_impl_array_v01
};
static const qmi_implemented_optionals ts_register_notification_temp_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_ts_register_notification_temp_reqresp_impl_tuple_v01 = 
{
  &ts_register_notification_temp_req_msg_impl_v01,
  &ts_register_notification_temp_resp_msg_impl_v01,
  NULL
};

static const uint32_t ts_temp_report_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* temp */
    QMI_SI_TLV_SET(0x11)      /* seq_num */
};

static const qmi_implemented_optionals ts_temp_report_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  ts_temp_report_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_ts_temp_report_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &ts_temp_report_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const ts_service_implemented_array_v01[] =
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
    &qmi_ts_get_supported_msgs_reqresp_impl_tuple_v01,   /* 0x001E */
    &qmi_ts_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_ts_get_sensor_list_reqresp_impl_tuple_v01,      /* 0x0020 */
    &qmi_ts_register_notification_temp_reqresp_impl_tuple_v01, /* 0x0021 */
    &qmi_ts_temp_report_impl_tuple_v01                   /* 0x0022 */
};

static const qmi_implemented_messages ts_service_implemented_v01 =
{
  0x0022,  /* maximum message ID in this service */
  ts_service_implemented_array_v01
};

const qmi_implemented_messages *ts_get_service_impl_v01 (void)
{
  return &ts_service_implemented_v01;
}
