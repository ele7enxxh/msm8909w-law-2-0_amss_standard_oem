/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              D E V I C E _ M A N A G E M E N T _ S E R V I C E _ I M P L _ V 0 1 # 1 0  . C

GENERAL DESCRIPTION
  This is the file which defines the dms service Data structures.

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved. 
 QUALCOMM Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/device_management_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.4
   It was generated on: Thu Jul 26 2012
   From IDL File: device_management_service_v01.idl */

#include "qmi_si.h"
#include "device_management_service_v01.h"

static const qmi_implemented_optionals_tuple qmi_dms_reset_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_set_event_report_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* report_power_state */
    QMI_SI_TLV_SET(0x11)   |  /* lvl_limits */
    QMI_SI_TLV_SET(0x12)   |  /* report_pin_state */
    QMI_SI_TLV_SET(0x13)   |  /* report_activation_state */
    QMI_SI_TLV_SET(0x14)   |  /* report_oprt_mode_state */
    QMI_SI_TLV_SET(0x15)   |  /* report_uim_state */
    QMI_SI_TLV_SET(0x16)   |  /* report_wireless_disable_state */
    QMI_SI_TLV_SET(0x17)      /* report_prl_init */
};

static const qmi_implemented_optionals dms_set_event_report_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_set_event_report_req_msg_impl_array_v01
};
static const uint32_t dms_event_report_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* power_state */
    QMI_SI_TLV_SET(0x11)   |  /* pin1_status */
    QMI_SI_TLV_SET(0x12)   |  /* pin2_status */
    QMI_SI_TLV_SET(0x13)   |  /* activation_state */
    QMI_SI_TLV_SET(0x14)   |  /* operating_mode */
    QMI_SI_TLV_SET(0x15)   |  /* uim_state */
    QMI_SI_TLV_SET(0x16)   |  /* wireless_disable_state */
    QMI_SI_TLV_SET(0x17)      /* prl_init */
};

static const qmi_implemented_optionals dms_event_report_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_event_report_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_set_event_report_reqresp_impl_tuple_v01 = 
{
  &dms_set_event_report_req_msg_impl_v01,
  NULL,
  &dms_event_report_ind_msg_impl_v01
};

static const uint32_t dms_get_device_cap_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* device_service_capability */
    QMI_SI_TLV_SET(0x11)   |  /* voice_support_capability */
    QMI_SI_TLV_SET(0x12)      /* simul_voice_and_data_capability */
};

static const qmi_implemented_optionals dms_get_device_cap_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_get_device_cap_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_get_device_cap_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_get_device_cap_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_get_device_mfr_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_get_device_model_id_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_get_device_rev_id_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* boot_code_rev */
    QMI_SI_TLV_SET(0x11)      /* pri_rev */
};

static const qmi_implemented_optionals dms_get_device_rev_id_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_get_device_rev_id_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_get_device_rev_id_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_get_device_rev_id_resp_msg_impl_v01,
  NULL
};

static const uint32_t dms_get_msisdn_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* mobile_id_number */
    QMI_SI_TLV_SET(0x11)      /* imsi */
};

static const qmi_implemented_optionals dms_get_msisdn_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_get_msisdn_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_get_msisdn_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_get_msisdn_resp_msg_impl_v01,
  NULL
};

static const uint32_t dms_get_device_serial_numbers_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* esn */
    QMI_SI_TLV_SET(0x11)   |  /* imei */
    QMI_SI_TLV_SET(0x12)   |  /* meid */
    QMI_SI_TLV_SET(0x13)      /* imeisv_svn */
};

static const qmi_implemented_optionals dms_get_device_serial_numbers_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_get_device_serial_numbers_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_get_device_serial_numbers_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_get_device_serial_numbers_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_get_power_state_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_uim_set_pin_protection_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* pin_retries_status */
};

static const qmi_implemented_optionals dms_uim_set_pin_protection_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_uim_set_pin_protection_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_uim_set_pin_protection_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_uim_set_pin_protection_resp_msg_impl_v01,
  NULL
};

static const uint32_t dms_uim_verify_pin_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* pin_retries_status */
};

static const qmi_implemented_optionals dms_uim_verify_pin_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_uim_verify_pin_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_uim_verify_pin_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_uim_verify_pin_resp_msg_impl_v01,
  NULL
};

static const uint32_t dms_uim_unblock_pin_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* pin_retries_status */
};

static const qmi_implemented_optionals dms_uim_unblock_pin_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_uim_unblock_pin_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_uim_unblock_pin_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_uim_unblock_pin_resp_msg_impl_v01,
  NULL
};

static const uint32_t dms_uim_change_pin_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* pin_retries_status */
};

static const qmi_implemented_optionals dms_uim_change_pin_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_uim_change_pin_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_uim_change_pin_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_uim_change_pin_resp_msg_impl_v01,
  NULL
};

static const uint32_t dms_uim_get_pin_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_SET(0x11)   |  /* pin1_status */
    QMI_SI_TLV_SET(0x12)      /* pin2_status */
};

static const qmi_implemented_optionals dms_uim_get_pin_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_uim_get_pin_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_uim_get_pin_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_uim_get_pin_status_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_get_device_hardware_rev_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_get_operating_mode_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* offline_reason */
    QMI_SI_TLV_SET(0x11)      /* hardware_controlled_mode */
};

static const qmi_implemented_optionals dms_get_operating_mode_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_get_operating_mode_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_get_operating_mode_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_get_operating_mode_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_set_operating_mode_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_get_time_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* sys_time_in_ms */
    QMI_SI_TLV_SET(0x11)      /* user_time_in_ms */
};

static const qmi_implemented_optionals dms_get_time_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_get_time_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_get_time_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_get_time_resp_msg_impl_v01,
  NULL
};

static const uint32_t dms_get_prl_ver_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* prl_only */
};

static const qmi_implemented_optionals dms_get_prl_ver_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_get_prl_ver_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_get_prl_ver_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_get_prl_ver_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_get_activation_state_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_activate_automatic_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_activate_manual_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_CLEAR(0x10) |  /* Undefined */
    QMI_SI_TLV_SET(0x11)   |  /* mn_ha_key */
    QMI_SI_TLV_SET(0x12)   |  /* mn_aaa_key */
    QMI_SI_TLV_SET(0x13)      /* pref_roaming_list */
};

static const qmi_implemented_optionals dms_activate_manual_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_activate_manual_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_activate_manual_reqresp_impl_tuple_v01 = 
{
  &dms_activate_manual_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_get_user_lock_state_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_set_user_lock_state_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_set_user_lock_code_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_read_user_data_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_write_user_data_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_read_eri_file_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_restore_factory_defaults_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_validate_service_programming_code_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_uim_get_iccid_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_uim_get_ck_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* operation_blocking */
};

static const qmi_implemented_optionals dms_uim_get_ck_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_uim_get_ck_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_uim_get_ck_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_uim_get_ck_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t dms_uim_set_ck_protection_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* verify_retries_left */
};

static const qmi_implemented_optionals dms_uim_set_ck_protection_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_uim_set_ck_protection_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_uim_set_ck_protection_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_uim_set_ck_protection_resp_msg_impl_v01,
  NULL
};

static const uint32_t dms_uim_unblock_ck_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* unblock_retries_left */
};

static const qmi_implemented_optionals dms_uim_unblock_ck_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_uim_unblock_ck_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_uim_unblock_ck_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_uim_unblock_ck_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_uim_get_imsi_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_uim_get_state_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_get_band_capability_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* lte_band_capability */
    QMI_SI_TLV_SET(0x11)      /* tds_band_capability */
};

static const qmi_implemented_optionals dms_get_band_capability_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_get_band_capability_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_get_band_capability_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_get_band_capability_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_get_factory_sku_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_set_time_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* time_reference_type */
};

static const qmi_implemented_optionals dms_set_time_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_set_time_req_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_set_time_reqresp_impl_tuple_v01 = 
{
  &dms_set_time_req_msg_impl_v01,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_get_alt_net_config_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_set_alt_net_config_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_get_sw_version_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple qmi_dms_set_spc_reqresp_impl_tuple_v01 = 
{
  NULL,
  NULL,
  NULL
};

static const uint32_t dms_get_current_prl_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* prl_version */
    QMI_SI_TLV_SET(0x11)      /* prl_only */
};

static const qmi_implemented_optionals dms_get_current_prl_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dms_get_current_prl_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dms_get_current_prl_info_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dms_get_current_prl_info_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const dms_service_implemented_array_v01[] =
{
    &qmi_dms_reset_reqresp_impl_tuple_v01,               /* 0x0000 */
    &qmi_dms_set_event_report_reqresp_impl_tuple_v01,    /* 0x0001 */
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
    &qmi_dms_get_device_cap_reqresp_impl_tuple_v01,      /* 0x0020 */
    &qmi_dms_get_device_mfr_reqresp_impl_tuple_v01,      /* 0x0021 */
    &qmi_dms_get_device_model_id_reqresp_impl_tuple_v01, /* 0x0022 */
    &qmi_dms_get_device_rev_id_reqresp_impl_tuple_v01,   /* 0x0023 */
    &qmi_dms_get_msisdn_reqresp_impl_tuple_v01,          /* 0x0024 */
    &qmi_dms_get_device_serial_numbers_reqresp_impl_tuple_v01, /* 0x0025 */
    &qmi_dms_get_power_state_reqresp_impl_tuple_v01,     /* 0x0026 */
    &qmi_dms_uim_set_pin_protection_reqresp_impl_tuple_v01, /* 0x0027 */
    &qmi_dms_uim_verify_pin_reqresp_impl_tuple_v01,      /* 0x0028 */
    &qmi_dms_uim_unblock_pin_reqresp_impl_tuple_v01,     /* 0x0029 */
    &qmi_dms_uim_change_pin_reqresp_impl_tuple_v01,      /* 0x002A */
    &qmi_dms_uim_get_pin_status_reqresp_impl_tuple_v01,  /* 0x002B */
    &qmi_dms_get_device_hardware_rev_reqresp_impl_tuple_v01, /* 0x002C */
    &qmi_dms_get_operating_mode_reqresp_impl_tuple_v01,  /* 0x002D */
    &qmi_dms_set_operating_mode_reqresp_impl_tuple_v01,  /* 0x002E */
    &qmi_dms_get_time_reqresp_impl_tuple_v01,            /* 0x002F */
    &qmi_dms_get_prl_ver_reqresp_impl_tuple_v01,         /* 0x0030 */
    &qmi_dms_get_activation_state_reqresp_impl_tuple_v01, /* 0x0031 */
    &qmi_dms_activate_automatic_reqresp_impl_tuple_v01,  /* 0x0032 */
    &qmi_dms_activate_manual_reqresp_impl_tuple_v01,     /* 0x0033 */
    &qmi_dms_get_user_lock_state_reqresp_impl_tuple_v01, /* 0x0034 */
    &qmi_dms_set_user_lock_state_reqresp_impl_tuple_v01, /* 0x0035 */
    &qmi_dms_set_user_lock_code_reqresp_impl_tuple_v01,  /* 0x0036 */
    &qmi_dms_read_user_data_reqresp_impl_tuple_v01,      /* 0x0037 */
    &qmi_dms_write_user_data_reqresp_impl_tuple_v01,     /* 0x0038 */
    &qmi_dms_read_eri_file_reqresp_impl_tuple_v01,       /* 0x0039 */
    &qmi_dms_restore_factory_defaults_reqresp_impl_tuple_v01, /* 0x003A */
    &qmi_dms_validate_service_programming_code_reqresp_impl_tuple_v01, /* 0x003B */
    &qmi_dms_uim_get_iccid_reqresp_impl_tuple_v01,       /* 0x003C */
    NULL,                                                /* 0x003D */
    NULL,                                                /* 0x003E */
    NULL,                                                /* 0x003F */
    &qmi_dms_uim_get_ck_status_reqresp_impl_tuple_v01,   /* 0x0040 */
    &qmi_dms_uim_set_ck_protection_reqresp_impl_tuple_v01, /* 0x0041 */
    &qmi_dms_uim_unblock_ck_reqresp_impl_tuple_v01,      /* 0x0042 */
    &qmi_dms_uim_get_imsi_reqresp_impl_tuple_v01,        /* 0x0043 */
    &qmi_dms_uim_get_state_reqresp_impl_tuple_v01,       /* 0x0044 */
    &qmi_dms_get_band_capability_reqresp_impl_tuple_v01, /* 0x0045 */
    &qmi_dms_get_factory_sku_reqresp_impl_tuple_v01,     /* 0x0046 */
    NULL,                                                /* 0x0047 */
    NULL,                                                /* 0x0048 */
    NULL,                                                /* 0x0049 */
    NULL,                                                /* 0x004A */
    &qmi_dms_set_time_reqresp_impl_tuple_v01,            /* 0x004B */
    NULL,                                                /* 0x004C */
    &qmi_dms_get_alt_net_config_reqresp_impl_tuple_v01,  /* 0x004D */
    &qmi_dms_set_alt_net_config_reqresp_impl_tuple_v01,  /* 0x004E */
    NULL,                                                /* 0x004F */
    NULL,                                                /* 0x0050 */
    &qmi_dms_get_sw_version_reqresp_impl_tuple_v01,      /* 0x0051 */
    &qmi_dms_set_spc_reqresp_impl_tuple_v01,             /* 0x0052 */
    &qmi_dms_get_current_prl_info_reqresp_impl_tuple_v01  /* 0x0053 */
};

static const qmi_implemented_messages dms_service_implemented_v01 =
{
  0x0053,  /* maximum message ID in this service */
  dms_service_implemented_array_v01
};

const qmi_implemented_messages *dms_get_service_impl_v01 (void)
{
  return &dms_service_implemented_v01;
}
