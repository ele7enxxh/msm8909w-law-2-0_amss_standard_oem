/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              C O M M O N _ F L O W _ C O N T R O L _ M A N A G E M E N T _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the cfcm service Data structures.

  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/common_flow_control_management_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.7
   It was generated on: Fri Jun 20 2014 (Spin 0)
   From IDL File: common_flow_control_management_v01.idl */

#include "qmi_si.h"
#include "common_flow_control_management_v01.h"

static const qmi_implemented_optionals cfcm_set_bus_bw_throttling_level_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals cfcm_set_bus_bw_throttling_level_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_cfcm_bus_bw_throttling_level_reqresp_msg_impl_tuple_v01 = 
{
  &cfcm_set_bus_bw_throttling_level_req_msg_impl_v01,
  &cfcm_set_bus_bw_throttling_level_resp_msg_impl_v01,
  NULL
};

static const uint32_t cfcm_get_bus_bw_throttling_level_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* bus_bw_throttling_level */
};

static const qmi_implemented_optionals cfcm_get_bus_bw_throttling_level_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  cfcm_get_bus_bw_throttling_level_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_cfcm_get_bw_throttling_level_reqresp_msg_impl_tuple_v01 = 
{
  NULL,
  &cfcm_get_bus_bw_throttling_level_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const cfcm_service_implemented_array_v01[] =
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
    &qmi_cfcm_bus_bw_throttling_level_reqresp_msg_impl_tuple_v01, /* 0x0020 */
    &qmi_cfcm_get_bw_throttling_level_reqresp_msg_impl_tuple_v01  /* 0x0021 */
};

static const qmi_implemented_messages cfcm_service_implemented_v01 =
{
  0x0021,  /* maximum message ID in this service */
  cfcm_service_implemented_array_v01
};

const qmi_implemented_messages *cfcm_get_service_impl_v01 (void)
{
  return &cfcm_service_implemented_v01;
}
