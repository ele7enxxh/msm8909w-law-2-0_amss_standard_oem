#ifndef QMI_CSI_TARGET_EXT_H
#define QMI_CSI_TARGET_EXT_H
/******************************************************************************
  @file    qmi_csi_target_ext.h
  @brief   The QMI Common Service Interface (CSI) OS specific Header file
           (Hybrid Rex and QuRT support)

  DESCRIPTION
  Describes the Rex specific OS parameters to QCSI
  
  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

 *******************************************************************************/
#include "comdef.h"
#include "qurt.h"
#include "rex.h"

#ifdef __cplusplus
extern "C" {
#endif

/** OS Parameters provided by the
 * service to the QCSI framework 
 */
typedef struct
{
  /***************************
   *  REX USER SET MEMBERS   *
   ***************************/

  /** tcb of the service thread.
   * (NULL if the QuRT abstraction
   * should be used) */
  rex_tcb_type  *tcb;

  /***************************
   *  QURT USER SET MEMBERS  *
   ***************************/

  /** signal structure which the 
   * service thread shall wait on
   * using qurt_anysignal_wait() */
  qurt_anysignal_t *signal;

  /** Signal mask to be set on the 
   * provided TCB when there is an
   * event pending for the serivce.
   * User shall wait for an event
   * using rex_wait() (If REX tcb is set)
   * or qurt_anysignal_wait() (and clear the
   * signal before handling the 
   * event using qmi_csi_handle_event() */
  unsigned int  sig;
} qmi_csi_os_params;

#ifdef __cplusplus
}
#endif
#endif
