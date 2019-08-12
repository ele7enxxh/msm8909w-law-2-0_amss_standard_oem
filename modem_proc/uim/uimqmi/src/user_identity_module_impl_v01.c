/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              U S E R _ I D E N T I T Y _ M O D U L E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the uim service Data structures.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/user_identity_module_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Wed Apr  6 2016 (Spin 0)
   From IDL File: user_identity_module_v01.idl */

#include "qmi_si.h"
#include "user_identity_module_v01.h"

static const qmi_implemented_optionals uim_reset_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_reset_reqresp_impl_tuple_v01 = 
{
  NULL,
  &uim_reset_resp_msg_impl_v01,
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
static const qmi_implemented_optionals_tuple qmi_uim_get_supported_msgs_reqresp_impl_tuple_v01 = 
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
static const qmi_implemented_optionals_tuple qmi_uim_get_supported_fields_reqresp_impl_tuple_v01 = 
{
  &qmi_get_supported_fields_req_impl_v01,
  &qmi_get_supported_fields_resp_impl_v01,
  NULL
};

static const uint32_t uim_read_transparent_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* indication_token */
    QMI_SI_TLV_SET(0x11)   |  /* encryption */
    QMI_SI_TLV_SET(0x12)      /* encryption_ext */
};

static const qmi_implemented_optionals uim_read_transparent_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_read_transparent_req_msg_impl_array_v01
};
static const uint32_t uim_read_transparent_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)   |  /* read_result */
    QMI_SI_TLV_SET(0x12)   |  /* indication_token */
    QMI_SI_TLV_SET(0x13)   |  /* encryption */
    QMI_SI_TLV_SET(0x14)   |  /* file_length */
    QMI_SI_TLV_SET(0x15)   |  /* long_data_token */
    QMI_SI_TLV_SET(0x16)      /* read_result_ext */
};

static const qmi_implemented_optionals uim_read_transparent_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_read_transparent_resp_msg_impl_array_v01
};
static const uint32_t uim_read_transparent_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)   |  /* read_result */
    QMI_SI_TLV_SET(0x12)   |  /* encryption */
    QMI_SI_TLV_SET(0x13)   |  /* file_length */
    QMI_SI_TLV_SET(0x14)   |  /* long_data_token */
    QMI_SI_TLV_SET(0x15)      /* read_result_ext */
};

static const qmi_implemented_optionals uim_read_transparent_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_read_transparent_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_read_transparent_reqresp_impl_tuple_v01 = 
{
  &uim_read_transparent_req_msg_impl_v01,
  &uim_read_transparent_resp_msg_impl_v01,
  &uim_read_transparent_ind_msg_impl_v01
};

static const uint32_t uim_read_record_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* last_record */
    QMI_SI_TLV_SET(0x11)      /* indication_token */
};

static const qmi_implemented_optionals uim_read_record_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_read_record_req_msg_impl_array_v01
};
static const uint32_t uim_read_record_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)   |  /* read_result */
    QMI_SI_TLV_SET(0x12)   |  /* additional_read_result */
    QMI_SI_TLV_SET(0x13)      /* indication_token */
};

static const qmi_implemented_optionals uim_read_record_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_read_record_resp_msg_impl_array_v01
};
static const uint32_t uim_read_record_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)   |  /* read_result */
    QMI_SI_TLV_SET(0x12)      /* additional_read_result */
};

static const qmi_implemented_optionals uim_read_record_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_read_record_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_read_record_reqresp_impl_tuple_v01 = 
{
  &uim_read_record_req_msg_impl_v01,
  &uim_read_record_resp_msg_impl_v01,
  &uim_read_record_ind_msg_impl_v01
};

static const uint32_t uim_write_transparent_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* indication_token */
};

static const qmi_implemented_optionals uim_write_transparent_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_write_transparent_req_msg_impl_array_v01
};
static const uint32_t uim_write_transparent_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)      /* indication_token */
};

static const qmi_implemented_optionals uim_write_transparent_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_write_transparent_resp_msg_impl_array_v01
};
static const uint32_t uim_write_transparent_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* card_result */
};

static const qmi_implemented_optionals uim_write_transparent_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_write_transparent_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_write_transparent_reqresp_impl_tuple_v01 = 
{
  &uim_write_transparent_req_msg_impl_v01,
  &uim_write_transparent_resp_msg_impl_v01,
  &uim_write_transparent_ind_msg_impl_v01
};

static const uint32_t uim_write_record_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* indication_token */
};

static const qmi_implemented_optionals uim_write_record_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_write_record_req_msg_impl_array_v01
};
static const uint32_t uim_write_record_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)      /* indication_token */
};

static const qmi_implemented_optionals uim_write_record_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_write_record_resp_msg_impl_array_v01
};
static const uint32_t uim_write_record_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* card_result */
};

static const qmi_implemented_optionals uim_write_record_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_write_record_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_write_record_reqresp_impl_tuple_v01 = 
{
  &uim_write_record_req_msg_impl_v01,
  &uim_write_record_resp_msg_impl_v01,
  &uim_write_record_ind_msg_impl_v01
};

static const uint32_t uim_get_file_attributes_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* indication_token */
    QMI_SI_TLV_SET(0x11)      /* uicc_security_attributes */
};

static const qmi_implemented_optionals uim_get_file_attributes_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_file_attributes_req_msg_impl_array_v01
};
static const uint32_t uim_get_file_attributes_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)   |  /* file_attributes */
    QMI_SI_TLV_SET(0x12)   |  /* indication_token */
    QMI_SI_TLV_SET(0x13)      /* file_status */
};

static const qmi_implemented_optionals uim_get_file_attributes_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_file_attributes_resp_msg_impl_array_v01
};
static const uint32_t uim_get_file_attributes_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)   |  /* file_attributes */
    QMI_SI_TLV_SET(0x12)      /* file_status */
};

static const qmi_implemented_optionals uim_get_file_attributes_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_file_attributes_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_file_attributes_reqresp_impl_tuple_v01 = 
{
  &uim_get_file_attributes_req_msg_impl_v01,
  &uim_get_file_attributes_resp_msg_impl_v01,
  &uim_get_file_attributes_ind_msg_impl_v01
};

static const uint32_t uim_set_pin_protection_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* pin_key_ref_id */
    QMI_SI_TLV_SET(0x11)      /* indication_token */
};

static const qmi_implemented_optionals uim_set_pin_protection_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_set_pin_protection_req_msg_impl_array_v01
};
static const uint32_t uim_set_pin_protection_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* retries_left */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_pin1 */
    QMI_SI_TLV_SET(0x12)   |  /* indication_token */
    QMI_SI_TLV_SET(0x13)      /* card_result */
};

static const qmi_implemented_optionals uim_set_pin_protection_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_set_pin_protection_resp_msg_impl_array_v01
};
static const uint32_t uim_set_pin_protection_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* retries_left */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_pin1 */
    QMI_SI_TLV_SET(0x12)      /* card_result */
};

static const qmi_implemented_optionals uim_set_pin_protection_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_set_pin_protection_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_set_pin_protection_reqresp_impl_tuple_v01 = 
{
  &uim_set_pin_protection_req_msg_impl_v01,
  &uim_set_pin_protection_resp_msg_impl_v01,
  &uim_set_pin_protection_ind_msg_impl_v01
};

static const uint32_t uim_verify_pin_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* encrypted_pin1 */
    QMI_SI_TLV_SET(0x11)   |  /* pin_key_ref_id */
    QMI_SI_TLV_SET(0x12)      /* indication_token */
};

static const qmi_implemented_optionals uim_verify_pin_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_verify_pin_req_msg_impl_array_v01
};
static const uint32_t uim_verify_pin_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* retries_left */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_pin1 */
    QMI_SI_TLV_SET(0x12)   |  /* indication_token */
    QMI_SI_TLV_SET(0x13)      /* card_result */
};

static const qmi_implemented_optionals uim_verify_pin_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_verify_pin_resp_msg_impl_array_v01
};
static const uint32_t uim_verify_pin_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* retries_left */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_pin1 */
    QMI_SI_TLV_SET(0x12)      /* card_result */
};

static const qmi_implemented_optionals uim_verify_pin_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_verify_pin_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_verify_pin_reqresp_impl_tuple_v01 = 
{
  &uim_verify_pin_req_msg_impl_v01,
  &uim_verify_pin_resp_msg_impl_v01,
  &uim_verify_pin_ind_msg_impl_v01
};

static const uint32_t uim_unblock_pin_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* pin_key_ref_id */
    QMI_SI_TLV_SET(0x11)      /* indication_token */
};

static const qmi_implemented_optionals uim_unblock_pin_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_unblock_pin_req_msg_impl_array_v01
};
static const uint32_t uim_unblock_pin_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* retries_left */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_pin1 */
    QMI_SI_TLV_SET(0x12)   |  /* indication_token */
    QMI_SI_TLV_SET(0x13)      /* card_result */
};

static const qmi_implemented_optionals uim_unblock_pin_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_unblock_pin_resp_msg_impl_array_v01
};
static const uint32_t uim_unblock_pin_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* retries_left */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_pin1 */
    QMI_SI_TLV_SET(0x12)      /* card_result */
};

static const qmi_implemented_optionals uim_unblock_pin_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_unblock_pin_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_unblock_pin_reqresp_impl_tuple_v01 = 
{
  &uim_unblock_pin_req_msg_impl_v01,
  &uim_unblock_pin_resp_msg_impl_v01,
  &uim_unblock_pin_ind_msg_impl_v01
};

static const uint32_t uim_change_pin_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* pin_key_ref_id */
    QMI_SI_TLV_SET(0x11)      /* indication_token */
};

static const qmi_implemented_optionals uim_change_pin_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_change_pin_req_msg_impl_array_v01
};
static const uint32_t uim_change_pin_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* retries_left */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_pin1 */
    QMI_SI_TLV_SET(0x12)   |  /* indication_token */
    QMI_SI_TLV_SET(0x13)      /* card_result */
};

static const qmi_implemented_optionals uim_change_pin_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_change_pin_resp_msg_impl_array_v01
};
static const uint32_t uim_change_pin_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* retries_left */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_pin1 */
    QMI_SI_TLV_SET(0x12)      /* card_result */
};

static const qmi_implemented_optionals uim_change_pin_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_change_pin_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_change_pin_reqresp_impl_tuple_v01 = 
{
  &uim_change_pin_req_msg_impl_v01,
  &uim_change_pin_resp_msg_impl_v01,
  &uim_change_pin_ind_msg_impl_v01
};

static const uint32_t uim_depersonalization_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* slot */
};

static const qmi_implemented_optionals uim_depersonalization_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_depersonalization_req_msg_impl_array_v01
};
static const uint32_t uim_depersonalization_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* retries_left */
};

static const qmi_implemented_optionals uim_depersonalization_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_depersonalization_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_depersonalization_reqresp_impl_tuple_v01 = 
{
  &uim_depersonalization_req_msg_impl_v01,
  &uim_depersonalization_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_refresh_register_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_refresh_register_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_refresh_register_reqresp_impl_tuple_v01 = 
{
  &uim_refresh_register_req_msg_impl_v01,
  &uim_refresh_register_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_refresh_ok_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_refresh_ok_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_refresh_ok_reqresp_impl_tuple_v01 = 
{
  &uim_refresh_ok_req_msg_impl_v01,
  &uim_refresh_ok_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_refresh_complete_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_refresh_complete_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_refresh_complete_reqresp_impl_tuple_v01 = 
{
  &uim_refresh_complete_req_msg_impl_v01,
  &uim_refresh_complete_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_refresh_get_last_event_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_refresh_get_last_event_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* refresh_event */
};

static const qmi_implemented_optionals uim_refresh_get_last_event_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_refresh_get_last_event_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_refresh_get_last_event_reqresp_impl_tuple_v01 = 
{
  &uim_refresh_get_last_event_req_msg_impl_v01,
  &uim_refresh_get_last_event_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_event_reg_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_event_reg_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* event_mask */
};

static const qmi_implemented_optionals uim_event_reg_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_event_reg_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_event_reg_reqresp_impl_tuple_v01 = 
{
  &uim_event_reg_req_msg_impl_v01,
  &uim_event_reg_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_get_card_status_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* extended_card_status */
};

static const qmi_implemented_optionals uim_get_card_status_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_card_status_req_msg_impl_array_v01
};
static const uint32_t uim_get_card_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_status */
    QMI_SI_TLV_SET(0x11)   |  /* hot_swap */
    QMI_SI_TLV_SET(0x12)   |  /* card_status_validity */
    QMI_SI_TLV_SET(0x13)   |  /* extended_card_status */
    QMI_SI_TLV_SET(0x14)   |  /* extended_hot_swap */
    QMI_SI_TLV_SET(0x15)      /* sim_busy_status */
};

static const qmi_implemented_optionals uim_get_card_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_card_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_card_status_reqresp_impl_tuple_v01 = 
{
  &uim_get_card_status_req_msg_impl_v01,
  &uim_get_card_status_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_power_down_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_power_down_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_power_down_reqresp_impl_tuple_v01 = 
{
  &uim_power_down_req_msg_impl_v01,
  &uim_power_down_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_power_up_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* ignore_hotswap_switch */
};

static const qmi_implemented_optionals uim_power_up_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_power_up_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_power_up_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_power_up_reqresp_impl_tuple_v01 = 
{
  &uim_power_up_req_msg_impl_v01,
  &uim_power_up_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_status_change_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_status */
    QMI_SI_TLV_SET(0x11)   |  /* card_status_validity */
    QMI_SI_TLV_SET(0x12)      /* extended_card_status */
};

static const qmi_implemented_optionals uim_status_change_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_status_change_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_status_change_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_status_change_ind_msg_impl_v01
};

static const uint32_t uim_refresh_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* refresh_event */
};

static const qmi_implemented_optionals uim_refresh_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_refresh_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_refresh_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_refresh_ind_msg_impl_v01
};

static const uint32_t uim_authenticate_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* indication_token */
};

static const qmi_implemented_optionals uim_authenticate_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_authenticate_req_msg_impl_array_v01
};
static const uint32_t uim_authenticate_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)   |  /* content */
    QMI_SI_TLV_SET(0x12)      /* indication_token */
};

static const qmi_implemented_optionals uim_authenticate_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_authenticate_resp_msg_impl_array_v01
};
static const uint32_t uim_authenticate_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)      /* content */
};

static const qmi_implemented_optionals uim_authenticate_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_authenticate_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_authenticate_reqresp_impl_tuple_v01 = 
{
  &uim_authenticate_req_msg_impl_v01,
  &uim_authenticate_resp_msg_impl_v01,
  &uim_authenticate_ind_msg_impl_v01
};

static const uint32_t uim_close_session_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* terminate_application */
};

static const qmi_implemented_optionals uim_close_session_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_close_session_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_close_session_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_close_session_reqresp_impl_tuple_v01 = 
{
  &uim_close_session_req_msg_impl_v01,
  &uim_close_session_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_get_service_status_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* service_number */
};

static const qmi_implemented_optionals uim_get_service_status_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_service_status_req_msg_impl_array_v01
};
static const uint32_t uim_get_service_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* fdn_status */
    QMI_SI_TLV_SET(0x11)   |  /* hidden_key_status */
    QMI_SI_TLV_SET(0x12)   |  /* index */
    QMI_SI_TLV_SET(0x13)   |  /* esn_status */
    QMI_SI_TLV_SET(0x14)   |  /* acl_status */
    QMI_SI_TLV_SET(0x15)      /* service_status */
};

static const qmi_implemented_optionals uim_get_service_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_service_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_service_status_reqresp_impl_tuple_v01 = 
{
  &uim_get_service_status_req_msg_impl_v01,
  &uim_get_service_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_set_service_status_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* fdn_status */
    QMI_SI_TLV_SET(0x11)      /* enable_acl */
};

static const qmi_implemented_optionals uim_set_service_status_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_set_service_status_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_set_service_status_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_set_service_status_reqresp_impl_tuple_v01 = 
{
  &uim_set_service_status_req_msg_impl_v01,
  &uim_set_service_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_change_provisioning_session_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* application_information */
};

static const qmi_implemented_optionals uim_change_provisioning_session_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_change_provisioning_session_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_change_provisioning_session_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_change_provisioning_session_reqresp_impl_tuple_v01 = 
{
  &uim_change_provisioning_session_req_msg_impl_v01,
  &uim_change_provisioning_session_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_get_label_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_get_label_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* label */
};

static const qmi_implemented_optionals uim_get_label_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_label_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_label_reqresp_impl_tuple_v01 = 
{
  &uim_get_label_req_msg_impl_v01,
  &uim_get_label_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_get_configuration_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* configuration_mask */
};

static const qmi_implemented_optionals uim_get_configuration_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_configuration_req_msg_impl_array_v01
};
static const uint32_t uim_get_configuration_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* automatic_selection */
    QMI_SI_TLV_SET(0x11)   |  /* personalization_status */
    QMI_SI_TLV_SET(0x12)   |  /* halt_subscription */
    QMI_SI_TLV_SET(0x13)   |  /* extended_personalization_status */
    QMI_SI_TLV_SET(0x14)   |  /* usb_uicc_supported */
    QMI_SI_TLV_SET(0x15)   |  /* sap_client_supported */
    QMI_SI_TLV_SET(0x16)   |  /* temporary_depersonalization_status */
    QMI_SI_TLV_SET(0x17)   |  /* perso_reactivation_status */
    QMI_SI_TLV_SET(0x18)      /* perso_control_key_present */
};

static const qmi_implemented_optionals uim_get_configuration_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_configuration_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_configuration_reqresp_impl_tuple_v01 = 
{
  &uim_get_configuration_req_msg_impl_v01,
  &uim_get_configuration_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_send_apdu_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* channel_id */
    QMI_SI_TLV_SET(0x11)      /* procedure_bytes */
};

static const qmi_implemented_optionals uim_send_apdu_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_send_apdu_req_msg_impl_array_v01
};
static const uint32_t uim_send_apdu_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* apdu */
    QMI_SI_TLV_SET(0x11)      /* long_response */
};

static const qmi_implemented_optionals uim_send_apdu_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_send_apdu_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_send_apdu_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_send_apdu_reqresp_impl_tuple_v01 = 
{
  &uim_send_apdu_req_msg_impl_v01,
  &uim_send_apdu_resp_msg_impl_v01,
  &uim_send_apdu_ind_msg_impl_v01
};

static const uint32_t uim_sap_connection_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* disconnect_mode */
    QMI_SI_TLV_SET(0x11)   |  /* intermediate_get_response */
    QMI_SI_TLV_SET(0x12)      /* connection_condition */
};

static const qmi_implemented_optionals uim_sap_connection_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_sap_connection_req_msg_impl_array_v01
};
static const uint32_t uim_sap_connection_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* sap_state */
};

static const qmi_implemented_optionals uim_sap_connection_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_sap_connection_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_sap_connection_reqresp_impl_tuple_v01 = 
{
  &uim_sap_connection_req_msg_impl_v01,
  &uim_sap_connection_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_sap_request_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* apdu */
};

static const qmi_implemented_optionals uim_sap_request_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_sap_request_req_msg_impl_array_v01
};
static const uint32_t uim_sap_request_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ATR_value */
    QMI_SI_TLV_SET(0x11)   |  /* apdu */
    QMI_SI_TLV_SET(0x12)      /* status */
};

static const qmi_implemented_optionals uim_sap_request_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_sap_request_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_sap_request_reqresp_impl_tuple_v01 = 
{
  &uim_sap_request_req_msg_impl_v01,
  &uim_sap_request_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_sap_connection_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* sap_connection_event */
};

static const qmi_implemented_optionals uim_sap_connection_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_sap_connection_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_sap_connection_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_sap_connection_ind_msg_impl_v01
};

static const uint32_t uim_logical_channel_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* aid */
    QMI_SI_TLV_SET(0x11)   |  /* channel_id */
    QMI_SI_TLV_SET(0x12)   |  /* file_control_information */
    QMI_SI_TLV_SET(0x13)      /* terminate_application */
};

static const qmi_implemented_optionals uim_logical_channel_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_logical_channel_req_msg_impl_array_v01
};
static const uint32_t uim_logical_channel_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* channel_id */
    QMI_SI_TLV_SET(0x11)   |  /* card_result */
    QMI_SI_TLV_SET(0x12)      /* select_response */
};

static const qmi_implemented_optionals uim_logical_channel_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_logical_channel_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_logical_channel_reqresp_impl_tuple_v01 = 
{
  &uim_logical_channel_req_msg_impl_v01,
  &uim_logical_channel_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_subscription_ok_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* encrypted_imsi */
};

static const qmi_implemented_optionals uim_subscription_ok_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_subscription_ok_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_subscription_ok_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_subscription_ok_reqresp_impl_tuple_v01 = 
{
  &uim_subscription_ok_req_msg_impl_v01,
  &uim_subscription_ok_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_get_atr_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_get_atr_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* atr_value */
};

static const qmi_implemented_optionals uim_get_atr_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_atr_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_atr_reqresp_impl_tuple_v01 = 
{
  &uim_get_atr_req_msg_impl_v01,
  &uim_get_atr_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_open_logical_channel_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* aid */
    QMI_SI_TLV_SET(0x11)      /* file_control_information */
};

static const qmi_implemented_optionals uim_open_logical_channel_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_open_logical_channel_req_msg_impl_array_v01
};
static const uint32_t uim_open_logical_channel_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* channel_id */
    QMI_SI_TLV_SET(0x11)   |  /* card_result */
    QMI_SI_TLV_SET(0x12)      /* select_response */
};

static const qmi_implemented_optionals uim_open_logical_channel_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_open_logical_channel_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_open_logical_channel_reqresp_impl_tuple_v01 = 
{
  &uim_open_logical_channel_req_msg_impl_v01,
  &uim_open_logical_channel_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_session_closed_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* aid */
    QMI_SI_TLV_SET(0x11)   |  /* channel_id */
    QMI_SI_TLV_SET(0x12)   |  /* provisioning_session */
    QMI_SI_TLV_SET(0x13)   |  /* cause */
    QMI_SI_TLV_SET(0x14)      /* file_id */
};

static const qmi_implemented_optionals uim_session_closed_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_session_closed_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_session_closed_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_session_closed_ind_msg_impl_v01
};

static const uint32_t uim_refresh_register_all_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* vote_for_init */
};

static const qmi_implemented_optionals uim_refresh_register_all_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_refresh_register_all_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_refresh_register_all_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_refresh_register_all_reqresp_impl_tuple_v01 = 
{
  &uim_refresh_register_all_req_msg_impl_v01,
  &uim_refresh_register_all_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_set_file_status_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_set_file_status_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_set_file_status_reqresp_impl_tuple_v01 = 
{
  &uim_set_file_status_req_msg_impl_v01,
  &uim_set_file_status_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_switch_slot_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_switch_slot_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_switch_slot_reqresp_impl_tuple_v01 = 
{
  &uim_switch_slot_req_msg_impl_v01,
  &uim_switch_slot_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_get_slots_status_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* physical_slot_status */
};

static const qmi_implemented_optionals uim_get_slots_status_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_slots_status_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_slots_status_reqresp_impl_tuple_v01 = 
{
  NULL,
  &uim_get_slots_status_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_slot_status_change_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* physical_slot_status */
};

static const qmi_implemented_optionals uim_slot_status_change_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_slot_status_change_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_slot_status_change_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_slot_status_change_ind_msg_impl_v01
};

static const uint32_t uim_read_transparent_long_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)      /* encryption */
};

static const qmi_implemented_optionals uim_read_transparent_long_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_read_transparent_long_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_read_transparent_long_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_read_transparent_long_ind_msg_impl_v01
};

static const uint32_t uim_sim_busy_status_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* sim_busy_status */
};

static const qmi_implemented_optionals uim_sim_busy_status_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_sim_busy_status_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_sim_busy_status_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_sim_busy_status_ind_msg_impl_v01
};

static const uint32_t uim_get_plmn_name_table_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* release_date */
    QMI_SI_TLV_SET(0x11)      /* table_source */
};

static const qmi_implemented_optionals uim_get_plmn_name_table_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_plmn_name_table_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_plmn_name_table_info_reqresp_impl_tuple_v01 = 
{
  NULL,
  &uim_get_plmn_name_table_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_personalization_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* feature_gw_network_perso */
    QMI_SI_TLV_SET(0x11)   |  /* feature_gw_network_subset_perso */
    QMI_SI_TLV_SET(0x12)   |  /* feature_gw_sp_perso */
    QMI_SI_TLV_SET(0x13)   |  /* feature_gw_corporate_perso */
    QMI_SI_TLV_SET(0x14)   |  /* feature_gw_sim_perso */
    QMI_SI_TLV_SET(0x15)   |  /* feature_1x_network1_perso */
    QMI_SI_TLV_SET(0x16)   |  /* feature_1x_network2_perso */
    QMI_SI_TLV_SET(0x17)   |  /* feature_1x_ruim_perso */
    QMI_SI_TLV_SET(0x18)   |  /* slot */
    QMI_SI_TLV_SET(0x19)   |  /* num_iterations */
    QMI_SI_TLV_SET(0x1A)   |  /* num_retries */
    QMI_SI_TLV_SET(0x1B)   |  /* reactivate_feature */
    QMI_SI_TLV_SET(0x1C)   |  /* reuse_code_for_feature */
    QMI_SI_TLV_SET(0x1D)   |  /* feature_gw_sp_ext_perso */
    QMI_SI_TLV_SET(0x1E)      /* feature_gw_corporate_ext_perso */
};

static const qmi_implemented_optionals uim_personalization_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_personalization_req_msg_impl_array_v01
};
static const uint32_t uim_personalization_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* retries_left */
};

static const qmi_implemented_optionals uim_personalization_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_personalization_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_personalization_reqresp_impl_tuple_v01 = 
{
  &uim_personalization_req_msg_impl_v01,
  &uim_personalization_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_increase_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* indication_token */
};

static const qmi_implemented_optionals uim_increase_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_increase_req_msg_impl_array_v01
};
static const uint32_t uim_increase_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)   |  /* increase_result */
    QMI_SI_TLV_SET(0x12)      /* indication_token */
};

static const qmi_implemented_optionals uim_increase_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_increase_resp_msg_impl_array_v01
};
static const uint32_t uim_increase_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)      /* increase_result */
};

static const qmi_implemented_optionals uim_increase_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_increase_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_increase_reqresp_impl_tuple_v01 = 
{
  &uim_increase_req_msg_impl_v01,
  &uim_increase_resp_msg_impl_v01,
  &uim_increase_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_recovery_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_recovery_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_recovery_reqresp_impl_tuple_v01 = 
{
  &uim_recovery_req_msg_impl_v01,
  &uim_recovery_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_reselect_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_reselect_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)      /* select_response */
};

static const qmi_implemented_optionals uim_reselect_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_reselect_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_reselect_reqresp_impl_tuple_v01 = 
{
  &uim_reselect_req_msg_impl_v01,
  &uim_reselect_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_recovery_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_recovery_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_recovery_ind_msg_impl_v01
};

static const uint32_t uim_send_status_cmd_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* status_cmd_mode */
    QMI_SI_TLV_SET(0x11)      /* status_cmd_resp */
};

static const qmi_implemented_optionals uim_send_status_cmd_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_send_status_cmd_req_msg_impl_array_v01
};
static const uint32_t uim_send_status_cmd_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* status_response */
    QMI_SI_TLV_SET(0x11)      /* card_result */
};

static const qmi_implemented_optionals uim_send_status_cmd_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_send_status_cmd_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_send_status_reqresp_impl_tuple_v01 = 
{
  &uim_send_status_cmd_req_msg_impl_v01,
  &uim_send_status_cmd_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_get_sim_profile_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_get_sim_profile_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* profile_info */
    QMI_SI_TLV_SET(0x11)      /* active_profile_info */
};

static const qmi_implemented_optionals uim_get_sim_profile_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_sim_profile_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_sim_profile_reqresp_impl_tuple_v01 = 
{
  &uim_get_sim_profile_req_msg_impl_v01,
  &uim_get_sim_profile_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_set_sim_profile_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* enable_profile */
};

static const qmi_implemented_optionals uim_set_sim_profile_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_set_sim_profile_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_set_sim_profile_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_set_sim_profile_reqresp_impl_tuple_v01 = 
{
  &uim_set_sim_profile_req_msg_impl_v01,
  &uim_set_sim_profile_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_supply_voltage_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_supply_voltage_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_supply_voltage_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_supply_voltage_reqresp_impl_tuple_v01 = 
{
  &uim_supply_voltage_req_msg_impl_v01,
  &uim_supply_voltage_resp_msg_impl_v01,
  &uim_supply_voltage_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_card_activation_status_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_card_activation_status_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_card_activation_status_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_depersonalization_secure_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_depersonalization_secure_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* encrypted_resp */
};

static const qmi_implemented_optionals uim_depersonalization_secure_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_depersonalization_secure_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_depersonalization_secure_reqresp_impl_tuple_v01 = 
{
  &uim_depersonalization_secure_req_msg_impl_v01,
  &uim_depersonalization_secure_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_personalization_secure_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_personalization_secure_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* encrypted_resp */
};

static const qmi_implemented_optionals uim_personalization_secure_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_personalization_secure_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_personalization_secure_reqresp_impl_tuple_v01 = 
{
  &uim_personalization_secure_req_msg_impl_v01,
  &uim_personalization_secure_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_emergency_only_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_emergency_only_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* encrypted_resp */
};

static const qmi_implemented_optionals uim_emergency_only_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_emergency_only_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_emergency_only_reqresp_impl_tuple_v01 = 
{
  &uim_emergency_only_req_msg_impl_v01,
  &uim_emergency_only_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_simlock_configuration_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_simlock_configuration_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* encrypted_resp */
};

static const qmi_implemented_optionals uim_simlock_configuration_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_simlock_configuration_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_simlock_configuration_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_simlock_configuration_reqresp_impl_tuple_v01 = 
{
  &uim_simlock_configuration_req_msg_impl_v01,
  &uim_simlock_configuration_resp_msg_impl_v01,
  &uim_simlock_configuration_ind_msg_impl_v01
};

static const uint32_t uim_gba_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* force_bootstrapping */
    QMI_SI_TLV_SET(0x11)   |  /* indication_token */
    QMI_SI_TLV_SET(0x12)   |  /* apn_name */
    QMI_SI_TLV_SET(0x13)   |  /* bsf_information */
    QMI_SI_TLV_SET(0x14)      /* key_status_ind_reg */
};

static const qmi_implemented_optionals uim_gba_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_gba_req_msg_impl_array_v01
};
static const uint32_t uim_gba_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ks_naf */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_ks_naf */
    QMI_SI_TLV_SET(0x12)   |  /* btid */
    QMI_SI_TLV_SET(0x13)   |  /* lifetime */
    QMI_SI_TLV_SET(0x14)   |  /* impi */
    QMI_SI_TLV_SET(0x15)      /* indication_token */
};

static const qmi_implemented_optionals uim_gba_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_gba_resp_msg_impl_array_v01
};
static const uint32_t uim_gba_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* ks_naf */
    QMI_SI_TLV_SET(0x11)   |  /* encrypted_ks_naf */
    QMI_SI_TLV_SET(0x12)   |  /* btid */
    QMI_SI_TLV_SET(0x13)   |  /* lifetime */
    QMI_SI_TLV_SET(0x14)      /* impi */
};

static const qmi_implemented_optionals uim_gba_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_gba_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_gba_reqresp_impl_tuple_v01 = 
{
  &uim_gba_req_msg_impl_v01,
  &uim_gba_resp_msg_impl_v01,
  &uim_gba_ind_msg_impl_v01
};

static const uint32_t uim_get_gba_impi_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* encrypt_data */
};

static const qmi_implemented_optionals uim_get_gba_impi_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_gba_impi_req_msg_impl_array_v01
};
static const uint32_t uim_get_gba_impi_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* impi */
    QMI_SI_TLV_SET(0x11)      /* encrypted_impi */
};

static const qmi_implemented_optionals uim_get_gba_impi_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_gba_impi_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_gba_impi_reqresp_impl_tuple_v01 = 
{
  &uim_get_gba_impi_req_msg_impl_v01,
  &uim_get_gba_impi_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_search_record_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* search_data */
    QMI_SI_TLV_SET(0x11)      /* indication_token */
};

static const qmi_implemented_optionals uim_search_record_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_search_record_req_msg_impl_array_v01
};
static const uint32_t uim_search_record_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)   |  /* records */
    QMI_SI_TLV_SET(0x12)      /* indication_token */
};

static const qmi_implemented_optionals uim_search_record_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_search_record_resp_msg_impl_array_v01
};
static const uint32_t uim_search_record_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* card_result */
    QMI_SI_TLV_SET(0x11)      /* records */
};

static const qmi_implemented_optionals uim_search_record_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_search_record_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_search_record_reqresp_impl_tuple_v01 = 
{
  &uim_search_record_req_msg_impl_v01,
  &uim_search_record_resp_msg_impl_v01,
  &uim_search_record_ind_msg_impl_v01
};

static const uint32_t uim_remote_unlock_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* simlock_data */
};

static const qmi_implemented_optionals uim_remote_unlock_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_remote_unlock_req_msg_impl_array_v01
};
static const uint32_t uim_remote_unlock_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* encrypted_key */
};

static const qmi_implemented_optionals uim_remote_unlock_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_remote_unlock_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_remote_unlock_reqresp_impl_tuple_v01 = 
{
  &uim_remote_unlock_req_msg_impl_v01,
  &uim_remote_unlock_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_verify_imsi_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* imsi */
};

static const qmi_implemented_optionals uim_verify_imsi_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_verify_imsi_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_verify_imsi_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_verify_imsi_reqresp_impl_tuple_v01 = 
{
  &uim_verify_imsi_req_msg_impl_v01,
  &uim_verify_imsi_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_temporary_unlock_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_temporary_unlock_status_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_temporary_unlock_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_gba_key_status_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_gba_key_status_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &uim_gba_key_status_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_purge_temporary_identity_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_purge_temporary_identity_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_purge_temporary_identity_ind_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_purge_temporary_identity_reqresp_impl_tuple_v01 = 
{
  &uim_purge_temporary_identity_req_msg_impl_v01,
  &uim_purge_temporary_identity_resp_msg_impl_v01,
  &uim_purge_temporary_identity_ind_msg_impl_v01
};

static const qmi_implemented_optionals uim_switch_slot_ext_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_switch_slot_ext_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_switch_slot_ext_reqresp_impl_tuple_v01 = 
{
  &uim_switch_slot_ext_req_msg_impl_v01,
  &uim_switch_slot_ext_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_get_physical_slot_mapping_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* logical_slot */
};

static const qmi_implemented_optionals uim_get_physical_slot_mapping_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_physical_slot_mapping_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_physical_slot_mapping_reqresp_impl_tuple_v01 = 
{
  NULL,
  &uim_get_physical_slot_mapping_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_get_profile_info_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_get_profile_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* profile_iccid */
    QMI_SI_TLV_SET(0x11)   |  /* profile_state */
    QMI_SI_TLV_SET(0x12)   |  /* profile_nickname */
    QMI_SI_TLV_SET(0x13)   |  /* profile_spn */
    QMI_SI_TLV_SET(0x14)   |  /* profile_name */
    QMI_SI_TLV_SET(0x15)   |  /* profile_icon_type */
    QMI_SI_TLV_SET(0x16)      /* profile_icon_data */
};

static const qmi_implemented_optionals uim_get_profile_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_profile_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_profile_info_reqresp_impl_tuple_v01 = 
{
  &uim_get_profile_info_req_msg_impl_v01,
  &uim_get_profile_info_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_get_eid_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_get_eid_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* eid_value */
};

static const qmi_implemented_optionals uim_get_eid_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_get_eid_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_get_eid_reqresp_impl_tuple_v01 = 
{
  &uim_get_eid_req_msg_impl_v01,
  &uim_get_eid_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_delete_profile_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_delete_profile_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_delete_profile_reqresp_impl_tuple_v01 = 
{
  &uim_delete_profile_req_msg_impl_v01,
  &uim_delete_profile_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals uim_euicc_memory_reset_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals uim_euicc_memory_reset_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_euicc_memory_reset_reqresp_impl_tuple_v01 = 
{
  &uim_euicc_memory_reset_req_msg_impl_v01,
  &uim_euicc_memory_reset_resp_msg_impl_v01,
  NULL
};

static const uint32_t uim_add_profile_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* confirmation_code */
};

static const qmi_implemented_optionals uim_add_profile_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_add_profile_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_add_profile_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t uim_add_profile_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* error_cause */
    QMI_SI_TLV_SET(0x11)      /* percentage */
};

static const qmi_implemented_optionals uim_add_profile_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_add_profile_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_uim_add_profile_reqresp_impl_tuple_v01 = 
{
  &uim_add_profile_req_msg_impl_v01,
  &uim_add_profile_resp_msg_impl_v01,
  &uim_add_profile_ind_msg_impl_v01
};

static const uint32_t uim_update_profile_nickname_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* nickname */
};

static const qmi_implemented_optionals uim_update_profile_nickname_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  uim_update_profile_nickname_req_msg_impl_array_v01
};
static const qmi_implemented_optionals uim_update_profile_nickname_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_uim_update_profile_nickname_reqresp_impl_tuple_v01 = 
{
  &uim_update_profile_nickname_req_msg_impl_v01,
  &uim_update_profile_nickname_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals_tuple * const uim_service_implemented_array_v01[] =
{
    &qmi_uim_reset_reqresp_impl_tuple_v01,               /* 0x0000 */
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
    &qmi_uim_get_supported_msgs_reqresp_impl_tuple_v01,  /* 0x001E */
    &qmi_uim_get_supported_fields_reqresp_impl_tuple_v01, /* 0x001F */
    &qmi_uim_read_transparent_reqresp_impl_tuple_v01,    /* 0x0020 */
    &qmi_uim_read_record_reqresp_impl_tuple_v01,         /* 0x0021 */
    &qmi_uim_write_transparent_reqresp_impl_tuple_v01,   /* 0x0022 */
    &qmi_uim_write_record_reqresp_impl_tuple_v01,        /* 0x0023 */
    &qmi_uim_get_file_attributes_reqresp_impl_tuple_v01, /* 0x0024 */
    &qmi_uim_set_pin_protection_reqresp_impl_tuple_v01,  /* 0x0025 */
    &qmi_uim_verify_pin_reqresp_impl_tuple_v01,          /* 0x0026 */
    &qmi_uim_unblock_pin_reqresp_impl_tuple_v01,         /* 0x0027 */
    &qmi_uim_change_pin_reqresp_impl_tuple_v01,          /* 0x0028 */
    &qmi_uim_depersonalization_reqresp_impl_tuple_v01,   /* 0x0029 */
    &qmi_uim_refresh_register_reqresp_impl_tuple_v01,    /* 0x002A */
    &qmi_uim_refresh_ok_reqresp_impl_tuple_v01,          /* 0x002B */
    &qmi_uim_refresh_complete_reqresp_impl_tuple_v01,    /* 0x002C */
    &qmi_uim_refresh_get_last_event_reqresp_impl_tuple_v01, /* 0x002D */
    &qmi_uim_event_reg_reqresp_impl_tuple_v01,           /* 0x002E */
    &qmi_uim_get_card_status_reqresp_impl_tuple_v01,     /* 0x002F */
    &qmi_uim_power_down_reqresp_impl_tuple_v01,          /* 0x0030 */
    &qmi_uim_power_up_reqresp_impl_tuple_v01,            /* 0x0031 */
    &qmi_uim_status_change_impl_tuple_v01,               /* 0x0032 */
    &qmi_uim_refresh_impl_tuple_v01,                     /* 0x0033 */
    &qmi_uim_authenticate_reqresp_impl_tuple_v01,        /* 0x0034 */
    &qmi_uim_close_session_reqresp_impl_tuple_v01,       /* 0x0035 */
    &qmi_uim_get_service_status_reqresp_impl_tuple_v01,  /* 0x0036 */
    &qmi_uim_set_service_status_reqresp_impl_tuple_v01,  /* 0x0037 */
    &qmi_uim_change_provisioning_session_reqresp_impl_tuple_v01, /* 0x0038 */
    &qmi_uim_get_label_reqresp_impl_tuple_v01,           /* 0x0039 */
    &qmi_uim_get_configuration_reqresp_impl_tuple_v01,   /* 0x003A */
    &qmi_uim_send_apdu_reqresp_impl_tuple_v01,           /* 0x003B */
    &qmi_uim_sap_connection_reqresp_impl_tuple_v01,      /* 0x003C */
    &qmi_uim_sap_request_reqresp_impl_tuple_v01,         /* 0x003D */
    &qmi_uim_sap_connection_impl_tuple_v01,              /* 0x003E */
    &qmi_uim_logical_channel_reqresp_impl_tuple_v01,     /* 0x003F */
    &qmi_uim_subscription_ok_reqresp_impl_tuple_v01,     /* 0x0040 */
    &qmi_uim_get_atr_reqresp_impl_tuple_v01,             /* 0x0041 */
    &qmi_uim_open_logical_channel_reqresp_impl_tuple_v01, /* 0x0042 */
    &qmi_uim_session_closed_impl_tuple_v01,              /* 0x0043 */
    &qmi_uim_refresh_register_all_reqresp_impl_tuple_v01, /* 0x0044 */
    &qmi_uim_set_file_status_reqresp_impl_tuple_v01,     /* 0x0045 */
    &qmi_uim_switch_slot_reqresp_impl_tuple_v01,         /* 0x0046 */
    &qmi_uim_get_slots_status_reqresp_impl_tuple_v01,    /* 0x0047 */
    &qmi_uim_slot_status_change_impl_tuple_v01,          /* 0x0048 */
    &qmi_uim_read_transparent_long_impl_tuple_v01,       /* 0x0049 */
    &qmi_uim_sim_busy_status_impl_tuple_v01,             /* 0x004A */
    &qmi_uim_get_plmn_name_table_info_reqresp_impl_tuple_v01, /* 0x004B */
    &qmi_uim_personalization_reqresp_impl_tuple_v01,     /* 0x004C */
    &qmi_uim_increase_reqresp_impl_tuple_v01,            /* 0x004D */
    &qmi_uim_recovery_reqresp_impl_tuple_v01,            /* 0x004E */
    &qmi_uim_reselect_reqresp_impl_tuple_v01,            /* 0x004F */
    &qmi_uim_recovery_impl_tuple_v01,                    /* 0x0050 */
    &qmi_uim_send_status_reqresp_impl_tuple_v01,         /* 0x0051 */
    &qmi_uim_get_sim_profile_reqresp_impl_tuple_v01,     /* 0x0052 */
    &qmi_uim_set_sim_profile_reqresp_impl_tuple_v01,     /* 0x0053 */
    &qmi_uim_supply_voltage_reqresp_impl_tuple_v01,      /* 0x0054 */
    &qmi_uim_card_activation_status_impl_tuple_v01,      /* 0x0055 */
    &qmi_uim_depersonalization_secure_reqresp_impl_tuple_v01, /* 0x0056 */
    &qmi_uim_personalization_secure_reqresp_impl_tuple_v01, /* 0x0057 */
    &qmi_uim_emergency_only_reqresp_impl_tuple_v01,      /* 0x0058 */
    &qmi_uim_simlock_configuration_reqresp_impl_tuple_v01, /* 0x0059 */
    &qmi_uim_gba_reqresp_impl_tuple_v01,                 /* 0x005A */
    &qmi_uim_get_gba_impi_reqresp_impl_tuple_v01,        /* 0x005B */
    &qmi_uim_search_record_reqresp_impl_tuple_v01,       /* 0x005C */
    &qmi_uim_remote_unlock_reqresp_impl_tuple_v01,       /* 0x005D */
    &qmi_uim_verify_imsi_reqresp_impl_tuple_v01,         /* 0x005E */
    &qmi_uim_temporary_unlock_status_impl_tuple_v01,     /* 0x005F */
    &qmi_uim_gba_key_status_impl_tuple_v01,              /* 0x0060 */
    &qmi_uim_purge_temporary_identity_reqresp_impl_tuple_v01, /* 0x0061 */
    &qmi_uim_switch_slot_ext_reqresp_impl_tuple_v01,     /* 0x0062 */
    &qmi_uim_get_physical_slot_mapping_reqresp_impl_tuple_v01, /* 0x0063 */
    &qmi_uim_get_profile_info_reqresp_impl_tuple_v01,    /* 0x0064 */
    &qmi_uim_get_eid_reqresp_impl_tuple_v01,             /* 0x0065 */
    &qmi_uim_delete_profile_reqresp_impl_tuple_v01,      /* 0x0066 */
    &qmi_uim_euicc_memory_reset_reqresp_impl_tuple_v01,  /* 0x0067 */
    &qmi_uim_add_profile_reqresp_impl_tuple_v01,         /* 0x0068 */
    &qmi_uim_update_profile_nickname_reqresp_impl_tuple_v01  /* 0x0069 */
};

static const qmi_implemented_messages uim_service_implemented_v01 =
{
  0x0069,  /* maximum message ID in this service */
  uim_service_implemented_array_v01
};

const qmi_implemented_messages *uim_get_service_impl_v01 (void)
{
  return &uim_service_implemented_v01;
}
