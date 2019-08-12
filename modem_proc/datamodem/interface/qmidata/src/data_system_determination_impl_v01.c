/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              D A T A _ S Y S T E M _ D E T E R M I N A T I O N _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the dsd service Data structures.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/src/data_system_determination_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.0
   It was generated on: Thu Feb 21 2013 (Spin 0)
   From IDL File: data_system_determination_v01.idl */

#include "qmi_si.h"

#include "data_system_determination_v01.h"

static const uint32_t dsd_wlan_available_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* wlan_ipv4_address */
    QMI_SI_TLV_SET(0x11)   |  /* wlan_ipv6_address */
    QMI_SI_TLV_SET(0x12)   |  /* wqe_status */
	QMI_SI_TLV_SET(0x13)   |  /* dns_ipv4_address_1 */
    QMI_SI_TLV_SET(0x14)   |  /* dns_ipv4_address_2 */
    QMI_SI_TLV_SET(0x15)   |  /* dns_ipv6_address_1 */
    QMI_SI_TLV_SET(0x16)   |  /* dns_ipv6_address_2 */
    QMI_SI_TLV_SET(0x17)   |  /* epdg_ipv4_address_1 */
    QMI_SI_TLV_SET(0x18)   |  /* epdg_ipv4_address_2 */
    QMI_SI_TLV_SET(0x19)   |  /* epdg_ipv6_address_1 */
    QMI_SI_TLV_SET(0x1A)   |  /* epdg_ipv6_address_2 */
    QMI_SI_TLV_SET(0x1B)      /* ssid */
};

static const qmi_implemented_optionals dsd_wlan_available_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_wlan_available_req_msg_impl_array_v01
};
static const qmi_implemented_optionals dsd_wlan_available_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_wlan_available_reqresp_impl_tuple_v01 = 
{
  &dsd_wlan_available_req_msg_impl_v01,
  &dsd_wlan_available_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_wlan_not_available_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* wqe_status */
};

static const qmi_implemented_optionals dsd_wlan_not_available_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_wlan_not_available_req_msg_impl_array_v01
};
static const qmi_implemented_optionals dsd_wlan_not_available_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_wlan_not_available_reqresp_impl_tuple_v01 = 
{
  &dsd_wlan_not_available_req_msg_impl_v01,
  &dsd_wlan_not_available_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals dsd_set_wlan_preference_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals dsd_set_wlan_preference_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_set_wlan_preference_reqresp_impl_tuple_v01 = 
{
  &dsd_set_wlan_preference_req_msg_impl_v01,
  &dsd_set_wlan_preference_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_get_wlan_preference_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* wlan_preference */
};

static const qmi_implemented_optionals dsd_get_wlan_preference_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_get_wlan_preference_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dsd_get_wlan_preference_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dsd_get_wlan_preference_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_get_system_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* avail_sys */
    QMI_SI_TLV_SET(0x11)      /* apn_avail_sys_info */
};

static const qmi_implemented_optionals dsd_get_system_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_get_system_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dsd_get_system_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dsd_get_system_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_system_status_change_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* limit_so_mask_change_ind */
};

static const qmi_implemented_optionals dsd_system_status_change_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_system_status_change_req_msg_impl_array_v01
};
static const qmi_implemented_optionals dsd_system_status_change_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_system_status_change_reqresp_impl_tuple_v01 = 
{
  &dsd_system_status_change_req_msg_impl_v01,
  &dsd_system_status_change_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_system_status_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* avail_sys */
    QMI_SI_TLV_SET(0x11)      /* apn_avail_sys_info */
};

static const qmi_implemented_optionals dsd_system_status_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_system_status_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dsd_system_status_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &dsd_system_status_ind_msg_impl_v01
};

static const qmi_implemented_optionals dsd_bind_subscription_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals dsd_bind_subscription_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_bind_subscription_reqresp_impl_tuple_v01 = 
{
  &dsd_bind_subscription_req_msg_impl_v01,
  &dsd_bind_subscription_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_get_bind_subscription_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* bind_subscription */
};

static const qmi_implemented_optionals dsd_get_bind_subscription_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_get_bind_subscription_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dsd_get_bind_subscription_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dsd_get_bind_subscription_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals dsd_set_apn_preferred_system_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals dsd_set_apn_preferred_system_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_set_apn_preferred_system_reqresp_impl_tuple_v01 = 
{
  &dsd_set_apn_preferred_system_req_msg_impl_v01,
  &dsd_set_apn_preferred_system_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_get_modem_power_cost_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* power_cost */
};

static const qmi_implemented_optionals dsd_get_modem_power_cost_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_get_modem_power_cost_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dsd_get_modem_power_cost_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dsd_get_modem_power_cost_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_pdn_policy_start_txn_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* txn_id */
};

static const qmi_implemented_optionals dsd_pdn_policy_start_txn_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_pdn_policy_start_txn_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dsd_pdn_policy_start_txn_reqresp_impl_tuple_v01 = 
{
  NULL,
  &dsd_pdn_policy_start_txn_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_add_pdn_policy_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* is_default */
    QMI_SI_TLV_SET(0x11)      /* override_type */
};

static const qmi_implemented_optionals dsd_add_pdn_policy_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_add_pdn_policy_req_msg_impl_array_v01
};
static const qmi_implemented_optionals dsd_add_pdn_policy_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_add_pdn_policy_reqresp_impl_tuple_v01 = 
{
  &dsd_add_pdn_policy_req_msg_impl_v01,
  &dsd_add_pdn_policy_resp_msg_impl_v01,
  NULL
};

static const uint32_t dsd_modify_pdn_policy_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* supported_system_priority_list */
    QMI_SI_TLV_SET(0x11)   |  /* is_default */
    QMI_SI_TLV_SET(0x12)      /* override_type */
};

static const qmi_implemented_optionals dsd_modify_pdn_policy_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_modify_pdn_policy_req_msg_impl_array_v01
};
static const qmi_implemented_optionals dsd_modify_pdn_policy_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_modify_pdn_policy_reqresp_impl_tuple_v01 = 
{
  &dsd_modify_pdn_policy_req_msg_impl_v01,
  &dsd_modify_pdn_policy_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals dsd_delete_pdn_policy_by_apn_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals dsd_delete_pdn_policy_by_apn_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_delete_pdn_policy_by_apn_reqresp_impl_tuple_v01 = 
{
  &dsd_delete_pdn_policy_by_apn_req_msg_impl_v01,
  &dsd_delete_pdn_policy_by_apn_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals dsd_get_pdn_policy_apn_list_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t dsd_get_pdn_policy_apn_list_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* apn_list */
};

static const qmi_implemented_optionals dsd_get_pdn_policy_apn_list_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_get_pdn_policy_apn_list_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dsd_get_pdn_policy_apn_list_reqresp_impl_tuple_v01 = 
{
  &dsd_get_pdn_policy_apn_list_req_msg_impl_v01,
  &dsd_get_pdn_policy_apn_list_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals dsd_get_pdn_policy_settings_for_apn_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t dsd_get_pdn_policy_settings_for_apn_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* apn_name */
    QMI_SI_TLV_SET(0x11)   |  /* supported_system_priority_list */
    QMI_SI_TLV_SET(0x12)   |  /* is_default */
    QMI_SI_TLV_SET(0x13)      /* override_type */
};

static const qmi_implemented_optionals dsd_get_pdn_policy_settings_for_apn_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  dsd_get_pdn_policy_settings_for_apn_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_dsd_get_pdn_policy_settings_for_apn_reqresp_impl_tuple_v01 = 
{
  &dsd_get_pdn_policy_settings_for_apn_req_msg_impl_v01,
  &dsd_get_pdn_policy_settings_for_apn_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals dsd_pdn_policy_end_txn_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals dsd_pdn_policy_end_txn_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_dsd_pdn_policy_end_txn_reqresp_impl_tuple_v01 = 
{
  &dsd_pdn_policy_end_txn_req_msg_impl_v01,
  &dsd_pdn_policy_end_txn_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const dsd_service_implemented_array_v01[] =
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
#ifdef FEATURE_DATA_WLAN_MAPCON
    &qmi_dsd_wlan_available_reqresp_impl_tuple_v01,      /* 0x0020 */
    &qmi_dsd_wlan_not_available_reqresp_impl_tuple_v01,  /* 0x0021 */
    &qmi_dsd_set_wlan_preference_reqresp_impl_tuple_v01, /* 0x0022 */
    &qmi_dsd_get_wlan_preference_reqresp_impl_tuple_v01, /* 0x0023 */
    &qmi_dsd_get_system_status_reqresp_impl_tuple_v01,   /* 0x0024 */
    &qmi_dsd_system_status_change_reqresp_impl_tuple_v01, /* 0x0025 */
    &qmi_dsd_system_status_impl_tuple_v01,               /* 0x0026 */
#else
    NULL,                                                /* 0x0020 */
    NULL,                                                /* 0x0021 */
    NULL,                                                /* 0x0022 */
    NULL,                                                /* 0x0023 */
    NULL,                                                /* 0x0024 */
    NULL,                                                /* 0x0025 */
    NULL,                                                /* 0x0026 */
#endif /* FEATURE_DATA_WLAN_MAPCON */
    NULL,                                                /* 0x0027 */
    NULL,                                                /* 0x0028 */
#ifdef FEATURE_DATA_WLAN_MAPCON
    &qmi_dsd_set_apn_preferred_system_reqresp_impl_tuple_v01, /* 0x0029 */
#else
    NULL, 
#endif /* FEATURE_DATA_WLAN_MAPCON */
    &qmi_dsd_get_modem_power_cost_reqresp_impl_tuple_v01,  /* 0x002A */   
#ifdef FEATURE_DATA_WLAN_MAPCON
    &qmi_dsd_pdn_policy_start_txn_reqresp_impl_tuple_v01, /* 0x002B */
    &qmi_dsd_add_pdn_policy_reqresp_impl_tuple_v01,      /* 0x002C */
    &qmi_dsd_modify_pdn_policy_reqresp_impl_tuple_v01,   /* 0x002D */
    &qmi_dsd_delete_pdn_policy_by_apn_reqresp_impl_tuple_v01, /* 0x002E */
    &qmi_dsd_get_pdn_policy_apn_list_reqresp_impl_tuple_v01, /* 0x002F */
    &qmi_dsd_get_pdn_policy_settings_for_apn_reqresp_impl_tuple_v01, /* 0x0030 */
    &qmi_dsd_pdn_policy_end_txn_reqresp_impl_tuple_v01   /* 0x0031 */
#else
    NULL,      /* 0x002B */
    NULL,      /* 0x002C */
    NULL,      /* 0x002D */
    NULL,      /* 0x002E */
    NULL,      /* 0x002F */
    NULL,      /* 0x0030 */
    NULL       /* 0x0031 */
#endif /* FEATURE_DATA_WLAN_MAPCON */
};

static const qmi_implemented_messages dsd_service_implemented_v01 =
{
  0x0031,  /* maximum message ID in this service */
  dsd_service_implemented_array_v01
};

const qmi_implemented_messages *dsd_get_service_impl_v01 (void)
{
  return &dsd_service_implemented_v01;
}


