#ifndef QMI_CSI_TARGET_H
#define QMI_CSI_TARGET_H
/******************************************************************************
  @file    qmi_csi_target.h
  @brief   Rex OS Specific routines internal to QCSI.

  DESCRIPTION
  This header provides an OS (Rex) abstraction to QCSI.

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "rex.h"
#include "qurt.h"
#include "smem_log.h"
#include "err.h"
#include "assert.h"
#include "qmi_common.h"
#include "qmi_csi_target_ext.h"
#include "qmi_idl_lib_internal.h"

typedef  qurt_mutex_t qmi_csi_lock_type;

#define LOCK(ptr)        qurt_pimutex_lock(ptr)
#define UNLOCK(ptr)      qurt_pimutex_unlock(ptr)
#define LOCK_INIT(ptr)   qurt_pimutex_init(ptr)
#define LOCK_DEINIT(ptr) qurt_pimutex_destroy(ptr)

#define MALLOC(size)      malloc(size)
#define CALLOC(num, size) calloc(num, size)
#define FREE(ptr)         free(ptr)
#define REALLOC(ptr,size) realloc(ptr, size)

extern void qmi_csi_os_signal_set(qmi_csi_os_params *sig);

extern void qcsi_log_error(char *filename, unsigned int line);

#define QMI_CSI_OS_SIGNAL_SET(s) qmi_csi_os_signal_set(s)

#define QMI_CSI_LOG_EVENT_TX            (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x00)
#define QMI_CSI_LOG_EVENT_TX_EXT        (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x04)
#define QMI_CSI_LOG_EVENT_RX            (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x01)
#define QMI_CSI_LOG_EVENT_RX_EXT        (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x05)
#define QMI_CSI_LOG_EVENT_ERROR         (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x03)

#ifdef FEATURE_QMI_SMEM_LOG
#define QMI_CSI_OS_LOG_TX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do {  \
    uint32_t __addr[MAX_ADDR_LEN/4] = {0};\
    QMI_MEM_COPY(__addr, MAX_ADDR_LEN, addr, addr_len); \
    SMEM_LOG_EVENT6(QMI_CSI_LOG_EVENT_TX_EXT, (cntl_flag) << 16 | (txn_id), (msg_id) << 16 | (msg_len),    (svc_obj)->service_id, __addr[0], __addr[1], __addr[2]); \
  } while(0)

#define QMI_CSI_OS_LOG_RX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do {  \
    uint32_t __addr[MAX_ADDR_LEN/4] = {0};\
    QMI_MEM_COPY(__addr, MAX_ADDR_LEN, addr, addr_len); \
    SMEM_LOG_EVENT6(QMI_CSI_LOG_EVENT_RX_EXT, (cntl_flag) << 16 | (txn_id), (msg_id) << 16 | (msg_len),    (svc_obj)->service_id, __addr[0], __addr[1], __addr[2]); \
  } while(0)
#else

#define QMI_CSI_OS_LOG_TX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len)
#define QMI_CSI_OS_LOG_RX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len)

#endif

#define QMI_CSI_OS_LOG_ERROR() qcsi_log_error(__FILENAME__, __LINE__)

#endif
