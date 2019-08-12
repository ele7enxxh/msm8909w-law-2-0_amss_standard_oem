/******************************************************************************
  @file    qmi_cci_target.c
  @brief   OS Specific routines internal to QCCI.

  DESCRIPTION
  This source provides an OS abstraction to QCCI.
  
  The current design is to use QuRT abstraction for all interfaces except
  when the user is involved. If the user provides NULL as the rex TCB type,
  the QuRT abstraction is automatically used.

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/qmi/qcci/qmi_cci_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================
                      INCLUDES
===========================================================================*/
#include "qmi_cci_target.h"


#define IS_POW_2(val) (((val) & ((val) - 1)) == 0 ? TRUE : FALSE)

size_t strlcpy(char *dst, const char *src, size_t siz);

/*===========================================================================
  FUNCTION  qmi_cci_os_ext_signal_init
===========================================================================*/
/*!
@brief

  Initializes an external signal object (For a notifier)

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_ext_signal_init
(
  qmi_cci_os_signal_type *ptr
)
{
  unsigned int timer_sig;
  if(!ptr || !IS_POW_2(ptr->sig))
  {
    return;
  }
  if(!IS_POW_2(ptr->timer_sig))
  {
    ptr->timer_sig = 0;
  }
  timer_sig = ptr->timer_sig;

  ptr->inited = FALSE;
  ptr->timer_inited = FALSE;
  ptr->timed_out = FALSE;

#ifdef FEATURE_QMI_CCI_REX_SUPPORTED
  /* Look at the REX TCB only if it is supported in this process domain.
   * If it is not supported, it is probably garbage, so do not even check
   * for its validity. If REX is supported, allow the users a migration
   * path where they would set tcb = NULL if they want to move to the 
   * QuRT based abstraction */
  if(ptr->tcb)
  {
    if(timer_sig)
    {
      if(TE_SUCCESS == timer_def_osal(&ptr->timer, NULL, TIMER_REX_TCB_SIGNAL_TYPE, ptr->tcb, timer_sig))
      {
        ptr->timer_inited = TRUE;
      }
    }
    ptr->inited = TRUE;
    return;
  }
#endif

  if(ptr->ext_signal)
  {
    if(timer_sig)
    {
      if(TE_SUCCESS == timer_def_osal(&ptr->timer, NULL, TIMER_NATIVE_OS_SIGNAL_TYPE, ptr->ext_signal, timer_sig))
      {
        ptr->timer_inited = TRUE;
      }
    }
    ptr->inited = TRUE;
    return;
  }
  qurt_anysignal_init(&ptr->signal);
  if(timer_sig)
  {
    if(TE_SUCCESS == timer_def_osal(&ptr->timer, NULL, TIMER_NATIVE_OS_SIGNAL_TYPE, &ptr->signal, timer_sig))
    {
      ptr->timer_inited = TRUE;
    }
  }
  ptr->inited = TRUE;
}

/*===========================================================================
  FUNCTION  qmi_cci_os_signal_init_self
===========================================================================*/
/*!
@brief

  Initialize a signal used internally by QCCI

@return
  None

@note
  Since this is used internally by QCCI we shall use QuRT signaling 
  and ignore any REX specific bits.
*/
/*=========================================================================*/
void qmi_cci_os_signal_init_self
(
  qmi_cci_os_signal_type *sig,
  qmi_cci_os_signal_type *os_params
)
{
  if(!sig)
  {
    return;
  }
  sig->inited = FALSE;
  sig->timer_inited = FALSE;
  sig->timed_out = FALSE;
  sig->sig = 1;
  sig->timer_sig = 2;
  sig->ext_signal = NULL;
  sig->tcb = NULL;
  qurt_anysignal_init(&sig->signal);

  if(TE_SUCCESS == timer_def_osal(&sig->timer, NULL, TIMER_NATIVE_OS_SIGNAL_TYPE, &sig->signal, sig->timer_sig))
  {
    sig->timer_inited = TRUE;
  }
  sig->inited = TRUE;
}

/*===========================================================================
  FUNCTION  qmi_cci_os_copy_os_params
===========================================================================*/
/*!
@brief

  Copy the OS Params provided by the user

@return
  None

@note
  Since this is also for a signal we are creating internal to QCCI, we 
  can go ahead and ignore any REX specific parameters in the OS Params
  and initiate the creation of a pure QuRT signal
*/
/*=========================================================================*/
void qmi_cci_os_copy_os_params
(
  qmi_cci_os_signal_type *sig,
  qmi_cci_os_signal_type *os_params
)
{
  if(sig)
  {
    sig->tcb = NULL;
    sig->ext_signal = NULL;
    sig->sig = 1;
    sig->timer_sig = 2;
  }
}


/*===========================================================================
  FUNCTION  qmi_cci_os_signal_deinit
===========================================================================*/
/*!
@brief

  De-initialize a previously initialized signal

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_signal_deinit
(
  qmi_cci_os_signal_type *ptr
)
{
  if(!ptr || !ptr->inited)
  {
    return;
  }

  if(ptr->timer_inited)
  {
    timer_clr_64(&ptr->timer, T_MSEC);
    timer_undef(&ptr->timer);
    ptr->timer_inited = FALSE;
  }
  ptr->inited = FALSE;
#ifdef FEATURE_QMI_CCI_REX_SUPPORTED
  if(ptr->tcb)
  {
    return;
  }
#endif
  if(!ptr->ext_signal)
  {
    qurt_anysignal_destroy(&ptr->signal);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_os_signal_clear
===========================================================================*/
/*!
@brief

  Clears a set event

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_signal_clear
(
  qmi_cci_os_signal_type *ptr
)
{
  unsigned int sig_mask;
  if(!ptr || !ptr->inited)
  {
    return;
  }
  sig_mask = ptr->sig | ptr->timer_sig;

#ifdef FEATURE_QMI_CCI_REX_SUPPORTED
  if(ptr->tcb)
  {
    rex_clr_sigs(ptr->tcb, (rex_sigs_type)sig_mask);
    return;
  }
#endif
  if(ptr->ext_signal)
  {
    qurt_anysignal_clear(ptr->ext_signal, sig_mask);
  }
  else
  {
    qurt_anysignal_clear(&ptr->signal, sig_mask);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_os_signal_set
===========================================================================*/
/*!
@brief

  Sets an event on a signal

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_signal_set
(
  qmi_cci_os_signal_type *ptr
)
{
  if(!ptr || !ptr->inited)
  {
    return;
  }

#ifdef FEATURE_QMI_CCI_REX_SUPPORTED
  if(ptr->tcb)
  {
    rex_set_sigs(ptr->tcb, (rex_sigs_type)ptr->sig);
    return;
  }
#endif
  if(ptr->ext_signal)
  {
    qurt_anysignal_set(ptr->ext_signal, ptr->sig);
  }
  else
  {
    qurt_anysignal_set(&ptr->signal, ptr->sig);
  }
}


/*===========================================================================
  FUNCTION  qmi_cci_os_signal_wait
===========================================================================*/
/*!
@brief

  Waits for a signal for the specified amount of time

@return
  None

@note
  REX will be used only if supported
*/
/*=========================================================================*/
void qmi_cci_os_signal_wait
(
  qmi_cci_os_signal_type *ptr,
  unsigned int timeout_ms
)
{
  if(!ptr || !ptr->inited)
  {
    return;
  }
#ifdef FEATURE_QMI_CCI_REX_SUPPORTED
  if(ptr->tcb)
  {
    ptr->timed_out = FALSE;
    if(timeout_ms && ptr->timer_inited)
    {
      rex_wait(ptr->sig | ptr->timer_sig);
      if(timer_get_64(&ptr->timer, T_MSEC) == 0)
      {
        ptr->timed_out = TRUE;
      }
    }
    else
    {
      rex_wait(ptr->sig);
    }
    return;
  }
#endif
  if(timeout_ms && ptr->timer_inited)
  {
    if(ptr->ext_signal)
    {
      timer_set_64(&ptr->timer, timeout_ms, 0, T_MSEC);
      qurt_anysignal_wait(ptr->ext_signal, ptr->sig | ptr->timer_sig);
      if(timer_clr_64(&ptr->timer, T_MSEC) == 0)
      {
        ptr->timed_out = TRUE;
      }
    }
    else
    {
      /* Timer is already set by QCCI by the call to
       * QMI_CCI_OS_SIGNAL_START_TIMER */
      qurt_anysignal_wait(&ptr->signal, ptr->sig | ptr->timer_sig);
      if(timer_get_64(&ptr->timer, T_MSEC) == 0)
      {
        ptr->timed_out = TRUE;
      }
    }
  }
  else
  {
    qurt_anysignal_wait(ptr->ext_signal ? ptr->ext_signal : &ptr->signal, ptr->sig | ptr->timer_sig);
  }
}


/*===========================================================================
  FUNCTION  qcci_log_error
===========================================================================*/
/*!
@brief

  Logs an error

@return
  None

@note
  None
*/
/*=========================================================================*/
void qcci_log_error
(
  char *filename, 
  unsigned int line
)
{
  uint32 name[5];
  strlcpy((char *)name, filename, sizeof(name));

#ifdef FEATURE_QMI_SMEM_LOG
  SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_ERROR, name[0], name[1], name[2], name[3], 
      name[4], line);
#endif
#ifdef FEATURE_QMI_MSG
  MSG_2(MSG_SSID_ONCRPC, MSG_LEGACY_ERROR,
        "Runtime error. File 0x%s, Line: %d", filename, line);
#endif
}

