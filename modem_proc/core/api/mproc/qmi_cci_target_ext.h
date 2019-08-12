#ifndef QMI_CCI_TARGET_EXT_H
#define QMI_CCI_TARGET_EXT_H
/******************************************************************************
  @file    qmi_cci_target_ext.h
  @brief   QMI CCI OS Specific macro/types externalized to clients.

  DESCRIPTION
  QMI OS Specific types and macros required by QCCI clients.

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "comdef.h"
#include "qurt.h"
#include "timer.h"
#include "msg.h"
#include "rex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Rex signal mask used by the framework for signalling if the OS Parameters
 * are not provided. If the user thread is expecting this to conflict with 
 * other functionality performed by the thread it should provide the OS
 * Parameters with the signal mask reserved for the framework's use */
#define QMI_CCI_DEFAULT_SIGNAL_MASK   0x80000000

/* OS Specific parameters provided by User on client and 
 * Notifier initialization. Note that this is a hybrid structure
 * to accommodate both Rex and QuRT users. 
 * REX: 
 * Existing users using the Rex abstraction should be unaffected.
 * QuRT:
 * QuRT users must memset the structure to 0 before using it.
 * Note that the REX part may not always be support */
typedef struct {
  /***************************
   *   REX USER SET MEMBERS  *
   ***************************/

  /* QuRT Users must set these to NULL or 0 */

  /* Set this to the TCB of the primary client thread */
  rex_tcb_type  *tcb;

  /***************************
   *  QuRT USER SET MEMBERS  *
   ***************************/

  /* (Optional) Use a provided initialized signal structure.
   * This field is useful for notifier clients (initialized
   * using qmi_client_notifier_init() and will be ignored
   * otherwise) */
  qurt_anysignal_t  *ext_signal;

  /***************************
   * Common USER SET MEMBERS *
   ***************************/

  /* Event signal mask to be used for events.
   * Set to 0 to let QCCI pick one for you */
  unsigned int      sig;

  /* Timer signal mask to be used for timeout. 
   * Set to 0 to let QCCI pick one for you */
  unsigned int      timer_sig;

  /***************************
   *     PRIVATE MEMBERS     *
   ***************************/
  qurt_anysignal_t  signal;
  boolean        timer_inited;
  timer_type     timer;
  boolean        timed_out;
  boolean        inited;
} qmi_cci_os_signal_type;

typedef qmi_cci_os_signal_type qmi_client_os_params;

#define QMI_CCI_OS_SIGNAL qmi_cci_os_signal_type

extern void qmi_cci_os_signal_deinit
(
  qmi_cci_os_signal_type *sig
);

extern void qmi_cci_os_signal_clear
(
  qmi_cci_os_signal_type *sig
);

extern void qmi_cci_os_signal_wait
(
  qmi_cci_os_signal_type *sig,
  unsigned int timeout_ms
);

extern void qmi_cci_os_signal_set
(
  qmi_cci_os_signal_type *sig
);

/* This is no longer used */
#define QMI_CCI_OS_SIGNAL_INIT(ptr, os_params) \
  do { \
    (ptr)->inited = TRUE;  \
    qurt_anysignal_init(&(ptr)->signal);  \
    (ptr)->timer_inited = FALSE;  \
  }while(0)


#define QMI_CCI_OS_SIGNAL_DEINIT(ptr) qmi_cci_os_signal_deinit(ptr)

#define QMI_CCI_OS_SIGNAL_CLEAR(ptr) qmi_cci_os_signal_clear(ptr)

#define QMI_CCI_OS_SIGNAL_WAIT(ptr, timeout_ms) qmi_cci_os_signal_wait(ptr, timeout_ms)

#define QMI_CCI_OS_SIGNAL_TIMED_OUT(ptr) ((ptr)->timed_out)

#define QMI_CCI_OS_SIGNAL_SET(ptr)  qmi_cci_os_signal_set(ptr)

#ifdef __cplusplus
}
#endif

#endif
