/******************************************************************************
#  @file    lpa_socket.h
#  @brief   Local profile assistant socket header file
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef QCRIL_UIM_LPA_SOCKET_H
#define QCRIL_UIM_LPA_SOCKET_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "qcrili.h"
#include "lpa.pb.h"

#define QCRIL_UIM_LPA_SOCKET_MAX_BUF_SIZE          (1024*150)
#define QCRIL_UIM_LPA_SOCKET_MAX_THREAD_NAME_SIZE  (50)

void qcril_uim_lpa_socket_init();

void qcril_uim_lpa_socket_send(bool has_token,
                                          RIL_Token token,
                                          lpa_MessageType type,
                                          lpa_MessageId message_id,
                                          bool has_error,
                                          lpa_Error error,
                                          const void* msg, int msg_len);

void qcril_uim_lpa_socket_send_empty_payload_unsol_resp(lpa_MessageId msg_id);

#ifdef  __cplusplus
}
#endif

#endif /* QCRIL_UIM_LPA_SOCKET_H */
