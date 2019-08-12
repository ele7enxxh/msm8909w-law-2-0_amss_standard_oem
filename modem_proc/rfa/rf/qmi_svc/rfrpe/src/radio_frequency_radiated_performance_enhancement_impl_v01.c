/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              R A D I O _ F R E Q U E N C Y _ R A D I A T E D _ P E R F O R M A N C E _ E N H A N C E M E N T _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the rfrpe service Data structures.

  Copyright (c) 2012 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/qmi_svc/rfrpe/src/radio_frequency_radiated_performance_enhancement_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.6
   It was generated on: Wed Dec  5 2012
   From IDL File: radio_frequency_radiated_performance_enhancement_v01.idl */

#include "qmi_si.h"
#include "radio_frequency_radiated_performance_enhancement_v01.h"

static const qmi_implemented_optionals rfrpe_set_scenario_req_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals rfrpe_set_scenario_resp_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_rfrpe_set_rfm_scenario_reqresp_impl_tuple_v01 = 
{
  &rfrpe_set_scenario_req_impl_v01,
  &rfrpe_set_scenario_resp_impl_v01,
  NULL
};

static const uint32_t rfrpe_get_rfm_scenarios_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* active_scenarios */
};

static const qmi_implemented_optionals rfrpe_get_rfm_scenarios_resp_impl_v01 =
{
  1, /* number of elements in array */
  rfrpe_get_rfm_scenarios_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_rfrpe_get_rfm_scenario_reqresp_impl_tuple_v01 = 
{
  NULL,
  &rfrpe_get_rfm_scenarios_resp_impl_v01,
  NULL
};

static const uint32_t rfrpe_get_provisioned_table_revision_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* provisioned_table_revision */
    QMI_SI_TLV_SET(0x11)      /* provisioned_table_OEM */
};

static const qmi_implemented_optionals rfrpe_get_provisioned_table_revision_resp_impl_v01 =
{
  1, /* number of elements in array */
  rfrpe_get_provisioned_table_revision_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_rfrpe_get_provisioned_table_revision_reqresp_impl_tuple_v01 = 
{
  NULL,
  &rfrpe_get_provisioned_table_revision_resp_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const rfrpe_service_implemented_array_v01[] =
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
    &qmi_rfrpe_set_rfm_scenario_reqresp_impl_tuple_v01,  /* 0x0020 */
    &qmi_rfrpe_get_rfm_scenario_reqresp_impl_tuple_v01,  /* 0x0021 */
    &qmi_rfrpe_get_provisioned_table_revision_reqresp_impl_tuple_v01  /* 0x0022 */
};

static const qmi_implemented_messages rfrpe_service_implemented_v01 =
{
  0x0022,  /* maximum message ID in this service */
  rfrpe_service_implemented_array_v01
};

const qmi_implemented_messages *rfrpe_get_service_impl_v01 (void)
{
  return &rfrpe_service_implemented_v01;
}
