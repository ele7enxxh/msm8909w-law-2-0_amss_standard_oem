/******************************************************************************
#  @file    lpa_msg_meta.h
#  @brief   Local profile assistant meta information header file
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/


#ifndef QCRIL_UIM_LPA_MSG_META_H
#define QCRIL_UIM_LPA_MSG_META_H

#include "lpa.pb.h"
#include "qcrili.h"
#include "qcril_uim_lpa_misc.h"
#include "qcril_qmi_npb_utils.h"

size_t            qcril_uim_lpa_get_msg_size      (lpa_MessageId msg_id, lpa_MessageType msg_type);
boolean           qcril_uim_lpa_is_msg_valid      (lpa_MessageId msg_id, lpa_MessageType msg_type);
const char*       qcril_uim_lpa_get_msg_log_str   (lpa_MessageId msg_id, lpa_MessageType msg_type);
const pb_field_t* qcril_uim_lpa_get_msg_pb_fields (lpa_MessageId msg_id, lpa_MessageType msg_type);
qcril_evt_e_type  qcril_uim_lpa_get_msg_event     (lpa_MessageId msg_id, lpa_MessageType msg_type);

#endif /* QCRIL_UIM_LPA_MSG_META_H */
