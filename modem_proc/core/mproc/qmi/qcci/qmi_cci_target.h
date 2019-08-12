#ifndef QMI_CCI_TARGET_H
#define QMI_CCI_TARGET_H
/******************************************************************************
  @file    qmi_cci_target.h
  @brief   OS Specific routines internal to QCCI.

  DESCRIPTION
  This header provides an OS abstraction to QCCI.

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
#include "msg.h"
#include "qmi_common.h"
#include "qmi_cci_target_ext.h"
#include "qmi_idl_lib_internal.h"

typedef qurt_mutex_t qmi_cci_lock_type;
#define LOCK(ptr)        qurt_pimutex_lock(ptr)
#define UNLOCK(ptr)      qurt_pimutex_unlock(ptr)
#define LOCK_INIT(ptr)   qurt_pimutex_init(ptr)
#define LOCK_DEINIT(ptr) qurt_pimutex_destroy(ptr)

#define MALLOC(size)      malloc(size)
#define CALLOC(num, size) calloc(num, size)
#define FREE(ptr)         free(ptr)
#define REALLOC(ptr,size) realloc(ptr, size)

#define QMI_CCI_LOG_EVENT_TX            (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x00)
#define QMI_CCI_LOG_EVENT_TX_EXT        (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x04)
#define QMI_CCI_LOG_EVENT_RX            (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x01)
#define QMI_CCI_LOG_EVENT_RX_EXT        (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x05)
#define QMI_CCI_LOG_EVENT_ERROR         (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x03)

extern void qmi_cci_os_ext_signal_init
(
  qmi_cci_os_signal_type *sig
);

extern void qmi_cci_os_signal_init_self
(
  qmi_cci_os_signal_type *sig,
  qmi_cci_os_signal_type *os_params
);

extern void qmi_cci_os_copy_os_params
(
  qmi_cci_os_signal_type *sig,
  qmi_cci_os_signal_type *os_params
);

extern void qcci_log_error
(
  char *filename, 
  unsigned int line
);

#define QMI_CCI_OS_SIGNAL_VALID(ptr) ((ptr) ? ((ptr)->inited == TRUE ? 1 : 0) : 0)
#define QMI_CCI_OS_EXT_SIGNAL_VALID(ptr) QMI_CCI_OS_SIGNAL_VALID(ptr)

#define QMI_CCI_OS_EXT_SIGNAL_INIT(ptr, os_params) \
  do {  \
    ptr = os_params;  \
    qmi_cci_os_ext_signal_init(ptr);  \
  } while(0)

#define QMI_CCI_OS_SIGNAL_INIT_SELF(ptr, os_params) qmi_cci_os_signal_init_self(ptr, os_params)

#define QMI_CCI_COPY_OS_PARAMS(ptr, src) qmi_cci_os_copy_os_params(ptr, src)

#define QMI_CCI_OS_SIGNAL_START_TIMER(ptr, timeout_ms) \
do { \
     if(timeout_ms && (ptr)->timer_inited) { \
            timer_set_64(&((ptr)->timer), (timeout_ms), 0, T_MSEC); \
              } \
} while (0)

#ifdef FEATURE_QMI_MSG
#define QMI_CCI_LOG_ERR(fmt, val1, val2, val3) MSG_HIGH(fmt, val1, val2, val3)
#else
#define QMI_CCI_LOG_ERR(fmt, val1, val2, val3)
#endif

#ifdef FEATURE_QMI_SMEM_LOG
/* Assumes addr_len is MAX_ADDR_LEN. Change needed if qmi_cci_common.c changes
 * to invalidate this assumption */
#define QMI_CCI_OS_LOG_TX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do {  \
    uint32_t *int_addr = (uint32_t *)(addr);  \
    SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_TX_EXT, (cntl_flag) << 16 | (txn_id), (msg_id) << 16 | (msg_len),    (svc_obj)->service_id, int_addr[0], int_addr[1], int_addr[2]); \
  } while(0)

/* Assumes addr_len is MAX_ADDR_LEN. Change needed if qmi_cci_common.c changes
 * to invalidate this assumption */
#define QMI_CCI_OS_LOG_RX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do {  \
    uint32_t *int_addr = (uint32_t *)(addr);  \
    SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_RX_EXT, (cntl_flag) << 16 | (txn_id), (msg_id) << 16 | (msg_len),    (svc_obj)->service_id, int_addr[0], int_addr[1], int_addr[2]); \
  } while(0)

#else

#define QMI_CCI_OS_LOG_TX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len)
#define QMI_CCI_OS_LOG_RX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len)

#endif

#define QMI_CCI_OS_LOG_ERROR() qcci_log_error(__FILENAME__, __LINE__)


#endif
