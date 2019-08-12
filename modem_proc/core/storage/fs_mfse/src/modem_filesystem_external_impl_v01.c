/***********************************************************************
 * modem_filesystem_external_impl_v01.c
 *
 * This is the file which defines the MFSE service Data structures.
 * Copyright (C) 2013 Qualcomm Technologies, Inc.
 *
 * Implementation of logging/query messages for MFSE service.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_mfse/src/modem_filesystem_external_impl_v01.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2013-04-03   dks   Create.

===========================================================================*/


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    M O D E M _ F I L E S Y S T E M _ E X T E R N A L _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mfse service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/core.mpss/3.5.c12.3/storage/fs_mfse/src/modem_filesystem_external_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.1
   It was generated on: Thu Apr  4 2013 (Spin 0)
   From IDL File: modem_filesystem_external_v01.idl */

#include "qmi_si.h"
#include "modem_filesystem_external_v01.h"

static const uint32_t qmi_get_supported_msgs_resp_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* supported_msgs */
};

static const qmi_implemented_optionals qmi_get_supported_msgs_resp_impl_v01 =
{
  1, /* number of elements in array */
  qmi_get_supported_msgs_resp_impl_array_v01
};
static const qmi_implemented_optionals_tuple
  qmi_mfse_get_supported_msgs_reqresp_impl_tuple_v01 =
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
static const qmi_implemented_optionals_tuple
  qmi_mfse_get_supported_fields_reqresp_impl_tuple_v01 =
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const qmi_implemented_optionals mfse_sync_no_wait_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t mfse_sync_no_wait_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* sync_token */
    QMI_SI_TLV_SET(0x11)      /* efs_err_num */
};

static const qmi_implemented_optionals mfse_sync_no_wait_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  mfse_sync_no_wait_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple
  qmi_mfse_sync_no_wait_reqresp_impl_tuple_v01 =
{
  &mfse_sync_no_wait_req_msg_impl_v01,
  &mfse_sync_no_wait_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals mfse_sync_get_status_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t mfse_sync_get_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* sync_status */
    QMI_SI_TLV_SET(0x11)      /* efs_err_num */
};

static const qmi_implemented_optionals mfse_sync_get_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  mfse_sync_get_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple
  qmi_mfse_sync_get_status_reqresp_impl_tuple_v01 =
{
  &mfse_sync_get_status_req_msg_impl_v01,
  &mfse_sync_get_status_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple
  *const mfse_service_implemented_array_v01[] =
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
    &qmi_mfse_get_supported_msgs_reqresp_impl_tuple_v01, /* 0x001E */
    &qmi_mfse_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_mfse_sync_no_wait_reqresp_impl_tuple_v01,       /* 0x0020 */
    &qmi_mfse_sync_get_status_reqresp_impl_tuple_v01     /* 0x0021 */
};

static const qmi_implemented_messages mfse_service_implemented_v01 =
{
  0x0021,  /* maximum message ID in this service */
  mfse_service_implemented_array_v01
};

const qmi_implemented_messages *mfse_get_service_impl_v01 (void)
{
  return &mfse_service_implemented_v01;
}
