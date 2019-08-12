/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              P H O N E B O O K _ M A N A G E R _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the pbm service Data structures.

  Copyright (c) 2011QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
  QUALCOMM Technologies Proprietary and Confidential.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/phonebook_manager_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.0
   It was generated on: Thu Feb  2 2012
   From IDL File: phonebook_manager_service_v01.idl */

#include "qmi_si.h"
#include "phonebook_manager_service_v01.h"

static const uint32_t pbm_indication_register_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* reg_mask */
};

static const qmi_implemented_optionals pbm_indication_register_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_indication_register_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbmication_register_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_indication_register_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_get_pb_capabilities_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* capability_basic_info */
    QMI_SI_TLV_SET(0x11)   |  /* group_capability */
    QMI_SI_TLV_SET(0x12)   |  /* ad_num_capability */
    QMI_SI_TLV_SET(0x13)   |  /* email_capability */
    QMI_SI_TLV_SET(0x14)   |  /* max_second_name_len */
    QMI_SI_TLV_SET(0x15)   |  /* is_hidden_entry_supported */
    QMI_SI_TLV_SET(0x16)   |  /* gas_capability */
    QMI_SI_TLV_SET(0x17)   |  /* aas_capability */
    QMI_SI_TLV_SET(0x18)      /* write_access_info */
};

static const qmi_implemented_optionals pbm_get_pb_capabilities_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_get_pb_capabilities_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_get_pb_capabilities_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_get_pb_capabilities_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_get_all_pb_capabilities_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* capability_basic_info_array */
    QMI_SI_TLV_SET(0x11)   |  /* group_capability_array */
    QMI_SI_TLV_SET(0x12)   |  /* ad_num_capability_array */
    QMI_SI_TLV_SET(0x13)   |  /* email_capability_array */
    QMI_SI_TLV_SET(0x14)   |  /* second_name_capability_array */
    QMI_SI_TLV_SET(0x15)   |  /* hidden_records_capability_array */
    QMI_SI_TLV_SET(0x16)   |  /* gas_capability_array */
    QMI_SI_TLV_SET(0x17)      /* aas_capability_array */
};

static const qmi_implemented_optionals pbm_get_all_pb_capabilities_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_get_all_pb_capabilities_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_get_all_pb_capabilities_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_get_all_pb_capabilities_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_read_records_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* num_of_recs */
};

static const qmi_implemented_optionals pbm_read_records_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_read_records_resp_msg_impl_array_v01
};
static const uint32_t pbm_record_read_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* sname_info_array */
    QMI_SI_TLV_SET(0x11)   |  /* ad_num_info_array */
    QMI_SI_TLV_SET(0x12)   |  /* grp_id_info_array */
    QMI_SI_TLV_SET(0x13)   |  /* email_info_array */
    QMI_SI_TLV_SET(0x14)      /* hidden_record_info_array */
};

static const qmi_implemented_optionals pbm_record_read_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_record_read_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_read_records_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_read_records_resp_msg_impl_v01,
  &pbm_record_read_ind_msg_impl_v01
};

static const uint32_t pbm_write_record_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* sname */
    QMI_SI_TLV_SET(0x11)   |  /* ad_num_info */
    QMI_SI_TLV_SET(0x12)   |  /* grp_id */
    QMI_SI_TLV_SET(0x13)   |  /* email_info */
    QMI_SI_TLV_SET(0x14)      /* is_hidden */
};

static const qmi_implemented_optionals pbm_write_record_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_write_record_req_msg_impl_array_v01
};
static const uint32_t pbm_write_record_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* record_id */
};

static const qmi_implemented_optionals pbm_write_record_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_write_record_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_write_record_reqresp_msg_impl_tuple_v01 =
{
  &pbm_write_record_req_msg_impl_v01,
  &pbm_write_record_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_delete_record_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* record_id */
};

static const qmi_implemented_optionals pbm_delete_record_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_delete_record_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_delete_record_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_delete_record_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple pbm_delete_all_pb_records_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  NULL
};

static const uint32_t pbm_search_records_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* number */
    QMI_SI_TLV_SET(0x11)      /* name */
};

static const qmi_implemented_optionals pbm_search_records_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_search_records_req_msg_impl_array_v01
};
static const uint32_t pbm_search_records_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* record_id */
};

static const qmi_implemented_optionals pbm_search_records_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_search_records_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_search_records_reqresp_msg_impl_tuple_v01 =
{
  &pbm_search_records_req_msg_impl_v01,
  &pbm_search_records_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple pbm_record_update_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple pbm_refresh_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple pbm_pb_ready_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  NULL
};

static const uint32_t pbm_emergency_list_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* nv_emer_nums */
    QMI_SI_TLV_SET(0x11)   |  /* card_emer_nums */
    QMI_SI_TLV_SET(0x12)      /* network_emer_nums */
};

static const qmi_implemented_optionals pbm_emergency_list_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_emergency_list_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_emergency_list_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  &pbm_emergency_list_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple pbm_all_pb_init_done_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  NULL
};

static const uint32_t pbm_get_emergency_list_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* pbm_emer_nums */
    QMI_SI_TLV_SET(0x11)   |  /* pbm_nv_emer_nums */
    QMI_SI_TLV_SET(0x12)   |  /* card_emer_nums */
    QMI_SI_TLV_SET(0x13)      /* network_emer_nums */
};

static const qmi_implemented_optionals pbm_get_emergency_list_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_get_emergency_list_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_get_emergency_list_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_get_emergency_list_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_get_all_groups_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* groups_data */
};

static const qmi_implemented_optionals pbm_get_all_groups_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_get_all_groups_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_get_all_groups_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_get_all_groups_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_set_group_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* grp_id */
};

static const qmi_implemented_optionals pbm_set_group_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_set_group_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_set_group_info_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_set_group_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_get_pb_state_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* phonebook_state */
};

static const qmi_implemented_optionals pbm_get_pb_state_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_get_pb_state_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_get_pb_state_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_get_pb_state_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_read_all_hidden_records_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* num_of_recs */
};

static const qmi_implemented_optionals pbm_read_all_hidden_records_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_read_all_hidden_records_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_read_all_hidden_records_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_read_all_hidden_records_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple pbm_hidden_record_status_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  NULL
};

static const uint32_t pbm_get_next_empty_record_id_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* record_id */
};

static const qmi_implemented_optionals pbm_get_next_empty_record_id_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_get_next_empty_record_id_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_get_next_empty_record_id_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_get_next_empty_record_id_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_get_next_non_empty_record_id_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* record_id */
};

static const qmi_implemented_optionals pbm_get_next_non_empty_record_id_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_get_next_non_empty_record_id_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_get_next_non_empty_record_id_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_get_next_non_empty_record_id_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_get_all_aas_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* aas_data */
};

static const qmi_implemented_optionals pbm_get_all_aas_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_get_all_aas_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_get_all_aas_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_get_all_aas_resp_msg_impl_v01,
  NULL
};

static const uint32_t pbm_set_aas_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* aas_id */
};

static const qmi_implemented_optionals pbm_set_aas_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_set_aas_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_set_aas_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_set_aas_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple pbm_aas_update_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple pbm_gas_update_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  NULL
};

static const qmi_implemented_optionals_tuple pbm_bind_subscription_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  NULL,
  NULL
};

static const uint32_t pbm_get_subscription_binding_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* subs_type */
};

static const qmi_implemented_optionals pbm_get_subscription_binding_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  pbm_get_subscription_binding_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple pbm_get_subscription_binding_reqresp_msg_impl_tuple_v01 =
{
  NULL,
  &pbm_get_subscription_binding_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const pbm_service_implemented_array_v01[] =
{
    NULL,                                                /* 0x0000 */
    &pbmication_register_reqresp_msg_impl_tuple_v01,     /* 0x0001 */
    &pbm_get_pb_capabilities_reqresp_msg_impl_tuple_v01, /* 0x0002 */
    &pbm_get_all_pb_capabilities_reqresp_msg_impl_tuple_v01, /* 0x0003 */
    &pbm_read_records_reqresp_msg_impl_tuple_v01,        /* 0x0004 */
    &pbm_write_record_reqresp_msg_impl_tuple_v01,        /* 0x0005 */
    &pbm_delete_record_reqresp_msg_impl_tuple_v01,       /* 0x0006 */
    &pbm_delete_all_pb_records_reqresp_msg_impl_tuple_v01, /* 0x0007 */
    &pbm_search_records_reqresp_msg_impl_tuple_v01,      /* 0x0008 */
    &pbm_record_update_msg_impl_tuple_v01,               /* 0x0009 */
    &pbm_refresh_msg_impl_tuple_v01,                     /* 0x000A */
    &pbm_pb_ready_msg_impl_tuple_v01,                    /* 0x000B */
    &pbm_emergency_list_msg_impl_tuple_v01,              /* 0x000C */
    &pbm_all_pb_init_done_msg_impl_tuple_v01,            /* 0x000D */
    &pbm_get_emergency_list_reqresp_msg_impl_tuple_v01,  /* 0x000E */
    &pbm_get_all_groups_reqresp_msg_impl_tuple_v01,      /* 0x000F */
    &pbm_set_group_info_reqresp_msg_impl_tuple_v01,      /* 0x0010 */
    &pbm_get_pb_state_reqresp_msg_impl_tuple_v01,        /* 0x0011 */
    &pbm_read_all_hidden_records_reqresp_msg_impl_tuple_v01, /* 0x0012 */
    &pbm_hidden_record_status_msg_impl_tuple_v01,        /* 0x0013 */
    &pbm_get_next_empty_record_id_reqresp_msg_impl_tuple_v01, /* 0x0014 */
    &pbm_get_next_non_empty_record_id_reqresp_msg_impl_tuple_v01, /* 0x0015 */
    &pbm_get_all_aas_reqresp_msg_impl_tuple_v01,         /* 0x0016 */
    &pbm_set_aas_reqresp_msg_impl_tuple_v01,             /* 0x0017 */
    &pbm_aas_update_msg_impl_tuple_v01,                  /* 0x0018 */
    &pbm_gas_update_msg_impl_tuple_v01,                  /* 0x0019 */
    &pbm_bind_subscription_reqresp_msg_impl_tuple_v01,   /* 0x001A */
    &pbm_get_subscription_binding_reqresp_msg_impl_tuple_v01  /* 0x001B */
};

static const qmi_implemented_messages pbm_service_implemented_v01 =
{
  0x001B,  /* maximum message ID in this service */
  pbm_service_implemented_array_v01
};

const qmi_implemented_messages *pbm_get_service_impl_v01 (void)
{
  return &pbm_service_implemented_v01;
}
