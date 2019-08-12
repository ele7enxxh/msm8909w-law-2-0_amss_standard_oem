/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              P E R S I S T E N T _ D E V I C E _ C O N F I G U R A T I O N _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the pdc service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_qmi/src/persistent_device_configuration_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Thu Nov  5 2015 (Spin 0)
   From IDL File: persistent_device_configuration_v01.idl */

#include "qmi_si.h"
#include "persistent_device_configuration_v01.h"

static const qmi_implemented_optionals pdc_reset_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_pdc_reset_reqresp_impl_tuple_v01 = 
{
  NULL,
  &pdc_reset_resp_msg_impl_v01,
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
static const qmi_implemented_optionals_tuple qmi_pdc_get_supported_msgs_reqresp_impl_tuple_v01 = 
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
static const qmi_implemented_optionals_tuple qmi_pdc_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const uint32_t pdc_indication_register_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* reg_config_change */
};

static const qmi_implemented_optionals pdc_indication_register_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_indication_register_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_indication_register_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_pdcication_register_reqresp_impl_tuple_v01 = 
{
  &pdc_indication_register_req_msg_impl_v01,
  &pdc_indication_register_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals pdc_config_change_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_pdc_config_change_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &pdc_config_change_ind_msg_impl_v01
};

static const uint32_t pdc_get_selected_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* subscription_id */
    QMI_SI_TLV_SET(0x12)      /* slot_id */
};

static const qmi_implemented_optionals pdc_get_selected_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_selected_config_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_get_selected_config_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_get_selected_config_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* active_config_id */
    QMI_SI_TLV_SET(0x12)      /* pending_config_id */
};

static const qmi_implemented_optionals pdc_get_selected_config_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_selected_config_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_get_selected_config_reqresp_impl_tuple_v01 = 
{
  &pdc_get_selected_config_req_msg_impl_v01,
  &pdc_get_selected_config_resp_msg_impl_v01,
  &pdc_get_selected_config_ind_msg_impl_v01
};

static const uint32_t pdc_set_selected_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* subscription_id */
    QMI_SI_TLV_SET(0x12)      /* slot_id */
};

static const qmi_implemented_optionals pdc_set_selected_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_set_selected_config_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_set_selected_config_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_set_selected_config_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ind_token */
};

static const qmi_implemented_optionals pdc_set_selected_config_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_set_selected_config_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_set_selected_config_reqresp_impl_tuple_v01 = 
{
  &pdc_set_selected_config_req_msg_impl_v01,
  &pdc_set_selected_config_resp_msg_impl_v01,
  &pdc_set_selected_config_ind_msg_impl_v01
};

static const uint32_t pdc_list_configs_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)      /* config_type */
};

static const qmi_implemented_optionals pdc_list_configs_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_list_configs_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_list_configs_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_list_configs_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)      /* config_list */
};

static const qmi_implemented_optionals pdc_list_configs_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_list_configs_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_list_configs_reqresp_impl_tuple_v01 = 
{
  &pdc_list_configs_req_msg_impl_v01,
  &pdc_list_configs_resp_msg_impl_v01,
  &pdc_list_configs_ind_msg_impl_v01
};

static const uint32_t pdc_delete_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)      /* config_id */
};

static const qmi_implemented_optionals pdc_delete_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_delete_config_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_delete_config_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_delete_config_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ind_token */
};

static const qmi_implemented_optionals pdc_delete_config_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_delete_config_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_delete_config_reqresp_impl_tuple_v01 = 
{
  &pdc_delete_config_req_msg_impl_v01,
  &pdc_delete_config_resp_msg_impl_v01,
  &pdc_delete_config_ind_msg_impl_v01
};

static const uint32_t pdc_load_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)      /* storage */
};

static const qmi_implemented_optionals pdc_load_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_load_config_req_msg_impl_array_v01
};
static const uint32_t pdc_load_config_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* frame_data_reset */
};

static const qmi_implemented_optionals pdc_load_config_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_load_config_resp_msg_impl_array_v01
};
static const uint32_t pdc_load_config_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* received_config_size */
    QMI_SI_TLV_SET(0x12)   |  /* remaining_config_size */
    QMI_SI_TLV_SET(0x13)      /* frame_data_reset */
};

static const qmi_implemented_optionals pdc_load_config_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_load_config_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_load_config_reqresp_impl_tuple_v01 = 
{
  &pdc_load_config_req_msg_impl_v01,
  &pdc_load_config_resp_msg_impl_v01,
  &pdc_load_config_ind_msg_impl_v01
};

static const uint32_t pdc_activate_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* activation_type */
    QMI_SI_TLV_SET(0x12)   |  /* subscription_id */
    QMI_SI_TLV_SET(0x13)      /* slot_id */
};

static const qmi_implemented_optionals pdc_activate_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_activate_config_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_activate_config_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_activate_config_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ind_token */
};

static const qmi_implemented_optionals pdc_activate_config_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_activate_config_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_activate_config_reqresp_impl_tuple_v01 = 
{
  &pdc_activate_config_req_msg_impl_v01,
  &pdc_activate_config_resp_msg_impl_v01,
  &pdc_activate_config_ind_msg_impl_v01
};

static const uint32_t pdc_get_config_info_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ind_token */
};

static const qmi_implemented_optionals pdc_get_config_info_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_config_info_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_get_config_info_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_get_config_info_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* config_size */
    QMI_SI_TLV_SET(0x12)   |  /* config_desc */
    QMI_SI_TLV_SET(0x13)   |  /* config_version */
    QMI_SI_TLV_SET(0x14)   |  /* storage */
    QMI_SI_TLV_SET(0x15)   |  /* path */
    QMI_SI_TLV_SET(0x16)      /* base_version */
};

static const qmi_implemented_optionals pdc_get_config_info_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_config_info_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_get_config_info_reqresp_impl_tuple_v01 = 
{
  &pdc_get_config_info_req_msg_impl_v01,
  &pdc_get_config_info_resp_msg_impl_v01,
  &pdc_get_config_info_ind_msg_impl_v01
};

static const uint32_t pdc_get_config_limits_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ind_token */
};

static const qmi_implemented_optionals pdc_get_config_limits_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_config_limits_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_get_config_limits_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_get_config_limits_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* max_config_size */
    QMI_SI_TLV_SET(0x12)      /* curr_config_size */
};

static const qmi_implemented_optionals pdc_get_config_limits_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_config_limits_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_get_config_limits_reqresp_impl_tuple_v01 = 
{
  &pdc_get_config_limits_req_msg_impl_v01,
  &pdc_get_config_limits_resp_msg_impl_v01,
  &pdc_get_config_limits_ind_msg_impl_v01
};

static const uint32_t pdc_get_default_config_info_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ind_token */
};

static const qmi_implemented_optionals pdc_get_default_config_info_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_default_config_info_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_get_default_config_info_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_get_default_config_info_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* config_version */
    QMI_SI_TLV_SET(0x12)   |  /* config_size */
    QMI_SI_TLV_SET(0x13)      /* config_desc */
};

static const qmi_implemented_optionals pdc_get_default_config_info_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_default_config_info_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_get_default_config_info_reqresp_impl_tuple_v01 = 
{
  &pdc_get_default_config_info_req_msg_impl_v01,
  &pdc_get_default_config_info_resp_msg_impl_v01,
  &pdc_get_default_config_info_ind_msg_impl_v01
};

static const uint32_t pdc_deactivate_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)      /* subscription_id */
};

static const qmi_implemented_optionals pdc_deactivate_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_deactivate_config_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_deactivate_config_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_deactivate_config_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ind_token */
};

static const qmi_implemented_optionals pdc_deactivate_config_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_deactivate_config_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_deactivate_config_reqresp_impl_tuple_v01 = 
{
  &pdc_deactivate_config_req_msg_impl_v01,
  &pdc_deactivate_config_resp_msg_impl_v01,
  &pdc_deactivate_config_ind_msg_impl_v01
};

static const uint32_t pdc_validate_config_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* config_id */
    QMI_SI_TLV_SET(0x12)   |  /* subscription_id */
    QMI_SI_TLV_SET(0x13)      /* path */
};

static const qmi_implemented_optionals pdc_validate_config_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_validate_config_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_validate_config_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_validate_config_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* result_format */
    QMI_SI_TLV_SET(0x12)   |  /* frame_index */
    QMI_SI_TLV_SET(0x13)      /* result_frame */
};

static const qmi_implemented_optionals pdc_validate_config_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_validate_config_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_validate_config_reqresp_impl_tuple_v01 = 
{
  &pdc_validate_config_req_msg_impl_v01,
  &pdc_validate_config_resp_msg_impl_v01,
  &pdc_validate_config_ind_msg_impl_v01
};

static const uint32_t pdc_get_feature_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ind_token */
};

static const qmi_implemented_optionals pdc_get_feature_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_feature_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_get_feature_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_get_feature_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* selection_mode */
    QMI_SI_TLV_SET(0x12)   |  /* carrier */
    QMI_SI_TLV_SET(0x13)   |  /* flex_mapping */
    QMI_SI_TLV_SET(0x14)      /* refresh_mode */
};

static const qmi_implemented_optionals pdc_get_feature_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_get_feature_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_get_feature_reqresp_impl_tuple_v01 = 
{
  &pdc_get_feature_req_msg_impl_v01,
  &pdc_get_feature_resp_msg_impl_v01,
  &pdc_get_feature_ind_msg_impl_v01
};

static const uint32_t pdc_set_feature_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ind_token */
    QMI_SI_TLV_SET(0x11)   |  /* selection_mode */
    QMI_SI_TLV_SET(0x12)   |  /* carrier */
    QMI_SI_TLV_SET(0x13)   |  /* flex_mapping */
    QMI_SI_TLV_SET(0x14)      /* refresh_mode */
};

static const qmi_implemented_optionals pdc_set_feature_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_set_feature_req_msg_impl_array_v01
};
static const qmi_implemented_optionals pdc_set_feature_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t pdc_set_feature_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ind_token */
};

static const qmi_implemented_optionals pdc_set_feature_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pdc_set_feature_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_pdc_set_feature_reqresp_impl_tuple_v01 = 
{
  &pdc_set_feature_req_msg_impl_v01,
  &pdc_set_feature_resp_msg_impl_v01,
  &pdc_set_feature_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const pdc_service_implemented_array_v01[] =
{
    &qmi_pdc_reset_reqresp_impl_tuple_v01,               /* 0x0000 */
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
    &qmi_pdc_get_supported_msgs_reqresp_impl_tuple_v01,  /* 0x001E */
    &qmi_pdc_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_pdcication_register_reqresp_impl_tuple_v01,     /* 0x0020 */
    &qmi_pdc_config_change_impl_tuple_v01,               /* 0x0021 */
    &qmi_pdc_get_selected_config_reqresp_impl_tuple_v01, /* 0x0022 */
    &qmi_pdc_set_selected_config_reqresp_impl_tuple_v01, /* 0x0023 */
    &qmi_pdc_list_configs_reqresp_impl_tuple_v01,        /* 0x0024 */
    &qmi_pdc_delete_config_reqresp_impl_tuple_v01,       /* 0x0025 */
    &qmi_pdc_load_config_reqresp_impl_tuple_v01,         /* 0x0026 */
    &qmi_pdc_activate_config_reqresp_impl_tuple_v01,     /* 0x0027 */
    &qmi_pdc_get_config_info_reqresp_impl_tuple_v01,     /* 0x0028 */
    &qmi_pdc_get_config_limits_reqresp_impl_tuple_v01,   /* 0x0029 */
    &qmi_pdc_get_default_config_info_reqresp_impl_tuple_v01, /* 0x002A */
    &qmi_pdc_deactivate_config_reqresp_impl_tuple_v01,   /* 0x002B */
    &qmi_pdc_validate_config_reqresp_impl_tuple_v01,     /* 0x002C */
    &qmi_pdc_get_feature_reqresp_impl_tuple_v01,         /* 0x002D */
    &qmi_pdc_set_feature_reqresp_impl_tuple_v01          /* 0x002E */
};

static const qmi_implemented_messages pdc_service_implemented_v01 =
{
  0x002E,  /* maximum message ID in this service */
  pdc_service_implemented_array_v01
};

const qmi_implemented_messages *pdc_get_service_impl_v01 (void)
{
  return &pdc_service_implemented_v01;
}
