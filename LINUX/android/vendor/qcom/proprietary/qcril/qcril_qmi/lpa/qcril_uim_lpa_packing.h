/******************************************************************************
#  @file    lpa_packing.h
#  @brief   Local profile assistant packing header file
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/


#ifndef QCRIL_UIM_LPA_PACKING_H
#define QCRIL_UIM_LPA_PACKING_H

#include "lpa.pb.h"
#include "qcrili.h"

size_t qcril_uim_lpa_pack_msg_and_tag(const void *msg,
                                                   bool has_token,
                                                   uint32_t token,
                                                   lpa_MessageType type,
                                                   lpa_MessageId message_id,
                                                   bool has_error, lpa_Error error,
                                                   uint8_t *buf, size_t buf_size);

lpa_MessageTag* qcril_uim_lpa_unpack_msg_tag(const uint8_t *buf,
                                                          size_t buf_size);

void qcril_uim_lpa_parse_packed_msg(lpa_MessageType msg_type,
                                                 lpa_MessageId msg_id,
                                                 const pb_callback_t packed_msg,
                                                 size_t msg_size,
                                                 void** unpacked_msg,
                                                 size_t*unpacked_msg_size_ptr,
                                                 qcril_evt_e_type* event_ptr);

#endif /* QCRIL_UIM_LPA_PACKING_H */
