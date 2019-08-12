/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S N S _ S A M _ C A L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the sns_sam_cal service Data structures.


  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Thu Jul 14 2016 (Spin 0)
   From IDL File: sns_sam_cal_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "sns_sam_cal_v01.h"
#include "sns_sam_common_v01.h"
#include "sns_common_v01.h"


/*Type Definitions*/
/*Message Definitions*/
/*
 * sns_sam_cal_enable_req_msg is empty
 * static const uint8_t sns_sam_cal_enable_req_msg_data_v01[] = {
 * };
 */

static const uint8_t sns_sam_cal_enable_resp_msg_data_v01[] = {
  2,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_cal_enable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_cal_enable_resp_msg_v01, instance_id) - QMI_IDL_OFFSET8(sns_sam_cal_enable_resp_msg_v01, instance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_enable_resp_msg_v01, instance_id)
};

static const uint8_t sns_sam_cal_disable_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_disable_req_msg_v01, instance_id)
};

static const uint8_t sns_sam_cal_disable_resp_msg_data_v01[] = {
  2,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_cal_disable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_cal_disable_resp_msg_v01, instance_id) - QMI_IDL_OFFSET8(sns_sam_cal_disable_resp_msg_v01, instance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_disable_resp_msg_v01, instance_id)
};

static const uint8_t sns_sam_cal_report_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_report_ind_msg_v01, instance_id),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_report_ind_msg_v01, timestamp),

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_report_ind_msg_v01, bias),
  3,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_report_ind_msg_v01, accuracy)
};

static const uint8_t sns_sam_cal_error_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_error_ind_msg_v01, error),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_error_ind_msg_v01, instance_id)
};

static const uint8_t sns_sam_cal_update_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_update_req_msg_v01, instance_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_update_req_msg_v01, update_type)
};

static const uint8_t sns_sam_cal_update_resp_msg_data_v01[] = {
  2,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(sns_sam_cal_update_resp_msg_v01, resp),
  QMI_IDL_TYPE88(2, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(sns_sam_cal_update_resp_msg_v01, instance_id) - QMI_IDL_OFFSET8(sns_sam_cal_update_resp_msg_v01, instance_id_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(sns_sam_cal_update_resp_msg_v01, instance_id)
};

/* Type Table */
/* No Types Defined in IDL */

/* Message Table */
static const qmi_idl_message_table_entry sns_sam_cal_message_table_v01[] = {
  {sizeof(sns_sam_cal_enable_req_msg_v01), 0},
  {sizeof(sns_sam_cal_enable_resp_msg_v01), sns_sam_cal_enable_resp_msg_data_v01},
  {sizeof(sns_sam_cal_disable_req_msg_v01), sns_sam_cal_disable_req_msg_data_v01},
  {sizeof(sns_sam_cal_disable_resp_msg_v01), sns_sam_cal_disable_resp_msg_data_v01},
  {sizeof(sns_sam_cal_report_ind_msg_v01), sns_sam_cal_report_ind_msg_data_v01},
  {sizeof(sns_sam_cal_error_ind_msg_v01), sns_sam_cal_error_ind_msg_data_v01},
  {sizeof(sns_sam_cal_update_req_msg_v01), sns_sam_cal_update_req_msg_data_v01},
  {sizeof(sns_sam_cal_update_resp_msg_v01), sns_sam_cal_update_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
const qmi_idl_type_table_object sns_sam_cal_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *sns_sam_cal_qmi_idl_type_table_object_referenced_tables_v01[] =
{&sns_sam_cal_qmi_idl_type_table_object_v01, &sns_sam_common_qmi_idl_type_table_object_v01, &sns_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object sns_sam_cal_qmi_idl_type_table_object_v01 = {
  0,
  sizeof(sns_sam_cal_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  NULL,
  sns_sam_cal_message_table_v01,
  sns_sam_cal_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

