/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            U I M Q M I    S U B S Y S T E M    T A S K


GENERAL DESCRIPTION

  This source file contains the main UIMQMI task along with supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2014, 2016 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/uimqmi_task.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/16   bcho    Deregister qmi uim remote service on task stop
06/25/15   ar      Fix compilation warnings
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
03/17/14   kb      Add offtarget support to check mutex is owned or not
01/10/14   df      Use v2.0 diag macros
12/11/13   vs      Addition of UIMQMI task and QMI UIM REMOTE

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "rex.h"
#include "task.h"
#include "dog_hb_rex.h"
#include "modem_mem.h"
#include "amssassert.h"
#include "uimqmi_task.h"
#include "qmi_uim_remote.h"
#include "uim_msg.h"
#include "rcevt_rex.h"
#include "rcinit.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* Global structures for UIMQMI task */
uimqmi_task_generic_data_type  * uimqmi_task_generic_data_ptr  = NULL;
rex_crit_sect_type               uimqmi_crit_sect;

/*===========================================================================
FUNCTION UIMQMI_TASK_WAIT

DESCRIPTION
  Main wait routine for the UIMQMI task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals). The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
static rex_sigs_type uimqmi_task_wait (
  rex_sigs_type    requested_mask
)
{
  rex_sigs_type rex_signals_mask = 0x00;  /* Mask of signals returned by REX */

  ASSERT(uimqmi_task_generic_data_ptr != NULL);

  /* Stop listening to signals if UIMQMI task has been stopped */
  if(uimqmi_task_generic_data_ptr->task_stopped)
  {
     requested_mask &= ~(UIMQMI_TASK_QMI_UIM_REMOTE_UIM_CMD_Q_SIG  |
                         UIMQMI_TASK_QMI_UIM_REMOTE_QCSI_SIG);
  }

  /* Loop until the current signals from REX match with one or more of
     the requested signals */
  do
  {
    (void) rex_clr_sigs( rex_self(), UIMQMI_TASK_DOG_RPT_TIMER_SIG );
    dog_hb_report(uimqmi_task_generic_data_ptr->dog_rpt_var);

    /* Perform the real wait to wait for a command...or to force a
       context switch to allow higher priority tasks to run */
    rex_signals_mask = rex_wait(requested_mask | UIMQMI_TASK_DOG_RPT_TIMER_SIG);

  /* do while mask requested does not match current mask */
  } while ((rex_signals_mask & requested_mask) == 0);

  return (rex_signals_mask);
} /* uimqmi_task_wait */


/*===========================================================================
FUNCTION UIMQMI_TASK_HANDLE_OFFLINE_SIG

DESCRIPTION
  This function is called to process an offline signal received by the UIMQMI
  task.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void uimqmi_task_handle_offline_sig (
  void
)
{
  UIM_MSG_HIGH_0("UIMQMI: GOT TASK_OFFLINE_SIG");

  /* Clear signal */
  (void) rex_clr_sigs(rex_self(), TASK_OFFLINE_SIG);
} /* uimqmi_task_handle_offline_sig */


/*===========================================================================
FUNCTION UIMQMI_TASK_HANDLE_STOP_SIG

DESCRIPTION
  This function is called to process a stop signal received by the UIMQMI
  task.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void uimqmi_task_handle_stop_sig (
  void
)
{
  UIM_MSG_HIGH_0("UIMQMI: GOT TASK_STOP_SIG");

   /* Clear signal */
  (void) rex_clr_sigs(rex_self(), TASK_STOP_SIG);

  /* Mark global as task stop to prevent new commands from
     being processed */
  if(uimqmi_task_generic_data_ptr != NULL)
  {
#ifdef FEATURE_UIM_REMOTE_UIM
    qmi_uim_remote_dereg();
#endif /* FEATURE_UIM_REMOTE_UIM */

    rex_enter_crit_sect(&uimqmi_crit_sect);
    uimqmi_task_generic_data_ptr->task_stopped = TRUE;
    rex_leave_crit_sect(&uimqmi_crit_sect);
  }
} /* uimqmi_task_handle_stop_sig */


/*===========================================================================
FUNCTION UIMQMI_TASK_INIT_POST_STARTUP

DESCRIPTION
  Function called to define timers, dog registrations and other initialization
  that requires UIMQMI task to be up as a prerequisite.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
static void uimqmi_task_init_post_startup (
  void
)
{
  ASSERT(uimqmi_task_generic_data_ptr != NULL);

  /* Register for dog heartbeat mechanism */
  uimqmi_task_generic_data_ptr->dog_rpt_var =
    dog_hb_register_rex((rex_sigs_type) UIMQMI_TASK_DOG_RPT_TIMER_SIG);
} /* uimqmi_task_init_post_startup */


/*===========================================================================
FUNCTION UIMQMI_TASK_PROCESS_END

DESCRIPTION
  Function called to clean up the task globals and unregister from DOG as
  the task is about to end

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
static void uimqmi_task_process_end (
  void
)
{
  ASSERT(uimqmi_task_generic_data_ptr != NULL);

  dog_hb_deregister(uimqmi_task_generic_data_ptr->dog_rpt_var);

  rex_enter_crit_sect(&uimqmi_crit_sect);
  modem_mem_free(uimqmi_task_generic_data_ptr, MODEM_MEM_CLIENT_UIM);
  rex_leave_crit_sect(&uimqmi_crit_sect);
} /* uimqmi_task_process_end */


/*===========================================================================
FUNCTION UIMQMI_TASK_INIT

DESCRIPTION
  Function called to initialize UIMQMI task queues and global variables.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit enabled in place of TMC, this function is called
  in RCInit context. Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void uimqmi_task_init (
  void
)
{
  /* If for whatever reasons this init funtion is called more than
     once, we put this safeguard in place to avoid initializing
     multiple times */
  if(uimqmi_task_generic_data_ptr != NULL)
  {
    return;
  }

  rex_init_crit_sect(&uimqmi_crit_sect);

  /* Initialize queues and globals */
#ifdef FEATURE_UIM_REMOTE_UIM
  qmi_uim_remote_globals_init();
#endif /* FEATURE_UIM_REMOTE_UIM */

  uimqmi_task_generic_data_ptr =
    modem_mem_calloc(1, sizeof(uimqmi_task_generic_data_type), MODEM_MEM_CLIENT_UIM);

  if (uimqmi_task_generic_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("UIMQMI_TASK_INIT: Error allocating memory");
    return;
  }

  uimqmi_task_generic_data_ptr->task_stopped = FALSE;
} /* uimqmi_task_init */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================
FUNCTION UIMQMI_TASK_UNREGISTER_FOR_QMI_MODEM_SIG

DESCRIPTION
  Function called to unregister from RCEVT for QMI MODEM task.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
static void uimqmi_task_unregister_for_qmi_modem_sig (
  RCEVT_SIGEX_SIGREX * qmi_modem_sig_ptr
)
{
  /* Unregister from RCEVT for QMIMODEM-task-ready notification
     and clear the corresponding signal. */

  if (qmi_modem_sig_ptr)
  {
    (void)rcevt_unregister_sigex_name("QMI_MODEM: READY",
                                      RCEVT_SIGEX_TYPE_SIGREX,
                                      qmi_modem_sig_ptr);
  }
} /* uimqmi_task_unregister_for_qmi_modem_sig */


/*===========================================================================
FUNCTION UIMQMI_TASK_HANDLE_QMI_FRAMEWORK_READY

DESCRIPTION
  When the QMI Framework is ready the QMI UIM Remote service can register
  with it and that will allow clients to send messages across.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
static void uimqmi_task_handle_qmi_framework_ready (
  void
)
{
  UIM_MSG_HIGH_0("QMI Framewor is ready");

  rex_clr_sigs(UIM_UIMQMI_TASK_TCB,
               UIMQMI_TASK_QMI_MODEM_TASK_READY_SIG);

  /* Make sure we don't re-do the initialization incase the signal is
     erroneously received more than once */
  if((uimqmi_task_generic_data_ptr->ready_tasks &
       (0x00000001 << (uint8) UIMQMI_TASK_TYPE_QMI_MODEM)) == FALSE)
  {
    /* QMI UIM REMOTE service init */
#ifdef FEATURE_UIM_REMOTE_UIM
    qmi_uim_remote_service_init(UIM_UIMQMI_TASK_TCB,
                               (rex_sigs_type)UIMQMI_TASK_QMI_UIM_REMOTE_QCSI_SIG);
#endif /* FEATURE_UIM_REMOTE_UIM */

    /* Mark that the QMI MODEM task is ready */
    uimqmi_task_generic_data_ptr->ready_tasks |=
      (0x00000001 << (uint8)UIMQMI_TASK_TYPE_QMI_MODEM);
  }
} /* uimqmi_task_handle_qmi_framework_ready */


#ifdef FEATURE_UIM_REMOTE_UIM
/*===========================================================================
FUNCTION UIMQMI_TASK_HANDLE_QMI_UIM_REMOTE_CMD_Q_SIG

DESCRIPTION
  Function called to handle a command posted to the QMI UIM REMOTE CMD Q

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
static void uimqmi_task_handle_qmi_uim_remote_cmd_q_sig (
  void
)
{
  (void)rex_clr_sigs(UIM_UIMQMI_TASK_TCB,
                     UIMQMI_TASK_QMI_UIM_REMOTE_UIM_CMD_Q_SIG);
  if(qmi_uim_remote_cmd_q_sig_handler() == FALSE)
  {
    /* If QMI UIM REMOTE CMD Q is not empty, reset signal to
       continue processing commands */
    (void)rex_set_sigs(UIM_UIMQMI_TASK_TCB,
                       UIMQMI_TASK_QMI_UIM_REMOTE_UIM_CMD_Q_SIG);
  }
} /* uimqmi_task_handle_qmi_uim_remote_cmd_q_sig */


/*===========================================================================
FUNCTION UIMQMI_TASK_HANDLE_QMI_UIM_REMOTE_QCSI_SIG

DESCRIPTION
  UIMQMI task calls this function when it receives a QCSI sig for QMI UIM REMOTE
  service

DEPENDENCIES
  UIMQMI task should be properly initialized prior to receiving this signal

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void uimqmi_task_handle_qmi_uim_remote_qcsi_sig (
  void
)
{
  /* Clear signal */
  (void) rex_clr_sigs(UIM_UIMQMI_TASK_TCB,
                      (rex_sigs_type)UIMQMI_TASK_QMI_UIM_REMOTE_QCSI_SIG);
  UIM_MSG_LOW_0("Got UIMQMI_TASK_QMI_UIM_REMOTE_QCSI_SIG");

  qmi_uim_remote_handle_qcsi_sig(UIM_UIMQMI_TASK_TCB,
                                (rex_sigs_type)UIMQMI_TASK_QMI_UIM_REMOTE_QCSI_SIG);
} /* uimqmi_task_handle_qmi_uim_remote_qcsi_sig */
#endif /* FEATURE_UIM_REMOTE_UIM */


/*===========================================================================
FUNCTION UIMQMI_TASK

DESCRIPTION
  Entry point for the UIMQMI task.  This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uimqmi_task (
  dword  dummy
)
{
  rex_sigs_type             uimqmi_sigs       =
                                  UIMQMI_TASK_QMI_MODEM_TASK_READY_SIG        |
#ifdef FEATURE_UIM_REMOTE_UIM
                                  UIMQMI_TASK_QMI_UIM_REMOTE_UIM_CMD_Q_SIG    |
                                  UIMQMI_TASK_QMI_UIM_REMOTE_QCSI_SIG         |
#endif /* FEATURE_UIM_REMOTE_UIM */
                                  TASK_STOP_SIG                               |
                                  TASK_OFFLINE_SIG;

  rex_sigs_type             uimqmi_sigs_rcvd  =  0x00;
  RCEVT_SIGEX_SIGREX        uimqmi_stop_sig;
  RCEVT_SIGEX_SIGREX        uimqmi_qmi_modem_ready_sig;
  RCINIT_GROUP              uimqmi_task_group;

  (void)dummy;

  /* Clear all signals */
  (void) rex_clr_sigs(rex_self(), (rex_sigs_type)~0);

  uimqmi_stop_sig.signal   = UIM_UIMQMI_TASK_TCB;
  uimqmi_stop_sig.mask     = TASK_STOP_SIG;
  uimqmi_task_group = rcinit_lookup_group_rextask(UIM_UIMQMI_TASK_TCB);

  rcinit_register_term_group(uimqmi_task_group,
                             RCEVT_SIGEX_TYPE_SIGREX, &uimqmi_stop_sig);

  uimqmi_qmi_modem_ready_sig.signal = UIM_UIMQMI_TASK_TCB;
  uimqmi_qmi_modem_ready_sig.mask   = UIMQMI_TASK_QMI_MODEM_TASK_READY_SIG;

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  (void)rcevt_register_sigex_name("QMI_MODEM: READY",
                                  RCEVT_SIGEX_TYPE_SIGREX,
                                  &uimqmi_qmi_modem_ready_sig);
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  /* Handshake with RCInit framework */
  rcinit_handshake_startup();

  uimqmi_task_init_post_startup();

  /* Main Task Loop */
  for (;;)
  {
    uimqmi_sigs_rcvd = uimqmi_task_wait( uimqmi_sigs );

    /*-----------------------------------------------------------------------------
      Process Signal - TASK_OFFLINE_SIG
    -----------------------------------------------------------------------------*/

    if (uimqmi_sigs_rcvd & TASK_OFFLINE_SIG)
    {
      uimqmi_task_handle_offline_sig();
    } /* end if TASK_OFFLINE_SIG */

    /*-----------------------------------------------------------------------------
      Process Signal - TASK_STOP_SIG
    -----------------------------------------------------------------------------*/
    else if (uimqmi_sigs_rcvd & TASK_STOP_SIG)
    {
      uimqmi_task_handle_stop_sig();
      rcinit_unregister_term_group(uimqmi_task_group, RCEVT_SIGEX_TYPE_SIGREX, &uimqmi_stop_sig);
      rcinit_handshake_term();
      break;
    } /* end if TASK_STOP_SIG */

    /*-----------------------------------------------------------------------------
      Process Signal - UIMQMI_TASK_QMI_MODEM_TASK_READY_SIG
    -----------------------------------------------------------------------------*/
    else if (uimqmi_sigs_rcvd & UIMQMI_TASK_QMI_MODEM_TASK_READY_SIG)
    {
      uimqmi_task_handle_qmi_framework_ready();
#ifndef FEATURE_UIM_TEST_FRAMEWORK
      uimqmi_task_unregister_for_qmi_modem_sig(&uimqmi_qmi_modem_ready_sig);
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
    }

#ifdef FEATURE_UIM_REMOTE_UIM
    /*-----------------------------------------------------------------------------
      Process Signal - UIMQMI_TASK_QMI_UIM_REMOTE_QCSI_SIG
    -----------------------------------------------------------------------------*/
    else if (uimqmi_sigs_rcvd & UIMQMI_TASK_QMI_UIM_REMOTE_QCSI_SIG)
    {
      uimqmi_task_handle_qmi_uim_remote_qcsi_sig();
    }

    /*-----------------------------------------------------------------------------
      Process Signal - UIMQMI_TASK_QMI_UIM_REMOTE_UIM_CMD_Q_SIG
    -----------------------------------------------------------------------------*/
    else if (uimqmi_sigs_rcvd & UIMQMI_TASK_QMI_UIM_REMOTE_UIM_CMD_Q_SIG)
    {
      uimqmi_task_handle_qmi_uim_remote_cmd_q_sig();
    }
#endif /* FEATURE_UIM_REMOTE_UIM */

    else
    {
      UIM_MSG_ERR_1("UIMQMI TASK INVALID SIGS: %x", uimqmi_sigs_rcvd);
      /* Reset sigs by masking out defined sigs and clearing out bad sigs */
      (void)rex_clr_sigs(UIM_UIMQMI_TASK_TCB, (uimqmi_sigs_rcvd & ~uimqmi_sigs) );
    } /* end else - Invalid signal in UIMQMI task */
  } /* end for(;;) */

  uimqmi_task_process_end();

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* uimqmi_task */


/*===========================================================================
FUNCTION UIMQMI_TASK_IS_TASK_OPERATIONAL

DESCRIPTION
  Function to determine if UIMQMI task is operational

DEPENDENCIES
  None

RETURN VALUE
  Boolean:
  TRUE if task is running
  FALSE if task is stopped or global state pointer is not init

SIDE EFFECTS
  None
===========================================================================*/
boolean uimqmi_task_is_task_operational (
  void
)
{
  rex_enter_crit_sect(&uimqmi_crit_sect);
  if (uimqmi_task_generic_data_ptr == NULL ||
      (uimqmi_task_generic_data_ptr != NULL &&
       uimqmi_task_generic_data_ptr->task_stopped))
  {
    rex_leave_crit_sect(&uimqmi_crit_sect);
    return FALSE;
  }
  rex_leave_crit_sect(&uimqmi_crit_sect);

  return TRUE;
} /* uimqmi_task_is_task_operational */

