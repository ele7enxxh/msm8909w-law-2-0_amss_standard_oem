/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    L P A   M A I N   T A S K


GENERAL DESCRIPTION

  This file contains the main LPA task and supporting functions.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/14/16   ar      Introduce DS http post retry timer logic
05/17/16   bcho    Deregister qmi uim http service on task stop
05/16/16   av      Route HTTP request to QMI HTTP
05/10/16   av      Clear LPD related global when sending out error response
04/28/16   av      Move LPA task init to same group as qmiuim
04/13/16   ks      Task init changes for QMI UIM HTTP
04/11/16   av      Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_msg.h"
#include "dog_hb_rex.h"
#include "rex.h"
#include "rex_types.h"
#include "rcinit.h"
#include "rcinit_rex.h"
#include "lpa.h"
#include "lpa_platform.h"
#include "lpa_timer.h"
#include "lpa_util.h"
#include "lpa_mmgsdi.h"
#include "lpa_state.h"
#include "lpa_cmd.h"
#include "lpa_http.h"
#include "lpa_efs.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/* Flag indicating when LPA task has stopped */
boolean                      lpa_task_stopped                     = FALSE;

/*  LPA dog variabls */
dog_report_type              lpa_dog_rpt_var = 0;

/* Pointer to the LPA task tcb. Initialized with the task. */
rex_tcb_type                *lpa_tcb_ptr                          = NULL;
uint8                        lpa_ready_tasks                      = 0;

static lpa_client_data_type  lpa_client_data[LPA_NUM_SLOTS];

/* Global for storing chip family info that can be leveraged by lpa http
   to decide whether to route the http request to QMI HTTP or to modem
   DS */
DalChipInfoIdType            lpa_chip_id                          = DALCHIPINFO_ID_UNKNOWN;

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION LPA_SEND_RESPONSE

DESCRIPTION
  This function sends a response to the client by invoking the client
  callback and clears all data corresponding to the current command.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_send_response (
  lpa_result_enum_type                 status,
  lpa_response_data_type              *response_ptr,
  lpa_slot_id_enum_type                slot_id,
  boolean                              reset_all
)
{
  if(slot_id >= LPA_NUM_SLOTS)
  {
    return;
  }

  if(lpa_client_data[slot_id].callback)
  {
    lpa_client_data[slot_id].callback(status,
                                      response_ptr,
                                      lpa_client_data[slot_id].user_data_ptr);
  }

  /* lpa_reset also takes care of resetting the state back to IDLE so that
     next command can be processed */
  if(reset_all)
  {
    lpa_reset(slot_id);
    lpa_timer_clear(slot_id, LPA_TIMER_TYPE_SANITY_TIMER);
  }

  /* Set the signal if there are one or more commands still in queue
     and LPA is currently in IDLE state. */
  lpa_cmd_set_sig();
} /* lpa_send_response */


/*===========================================================================
FUNCTION LPA_SEND_EVENT

DESCRIPTION
  This function sends an event to the client by invoking the client event
  callback.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_send_event (
  lpa_result_enum_type                 status,
  lpa_event_data_type                 *event_ptr,
  lpa_slot_id_enum_type                slot_id,
  boolean                              reset_all
)
{
  if(slot_id >= LPA_NUM_SLOTS)
  {
    return;
  }

  if(lpa_client_data[slot_id].event_callback)
  {
    lpa_client_data[slot_id].event_callback(status,
                                            event_ptr);
  }

  if(reset_all)
  {
    lpa_reset(slot_id);
    lpa_timer_clear(slot_id, LPA_TIMER_TYPE_SANITY_TIMER);
  }
} /* lpa_send_event */


/*===========================================================================
FUNCTION LPA_SEND_PROFILE_DOWNLOAD_EVENT

DESCRIPTION
  This function sends a profile download event to the client by invoking
  the client event callback.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_send_profile_download_event (
  lpa_result_enum_type                 status,
  lpa_slot_id_enum_type                slot_id,
  uint8                                percent_downloaded,
  boolean                              installation_complete,
  boolean                              reset_all
)
{
  lpa_event_data_type              download_progress_event;

  memset(&download_progress_event, 0x00, sizeof(download_progress_event));
  download_progress_event.evt_type = LPA_PROFILE_DOWNLOAD_PROGRESS_EVENT;
  download_progress_event.evt_data.profile_download_progress.slot_id =
    slot_id;
  download_progress_event.evt_data.profile_download_progress.percent_downloaded =
    percent_downloaded;
  download_progress_event.evt_data.profile_download_progress.installation_complete =
    installation_complete;

  lpa_send_event(status,
                 &download_progress_event,
                 slot_id,
                 reset_all);
} /* lpa_send_profile_download_event */


/*===========================================================================
FUNCTION LPA_SAVE_CLIENT_DATA

DESCRIPTION
  Function saves the client data that needs to be saved throughout the
  duration of the command.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_save_client_data (
  lpa_callback_type                  callback,
  lpa_event_callback_type            event_callback,
  void                              *user_data_ptr,
  lpa_slot_id_enum_type              slot_id
)
{
  if(slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_client_data[slot_id].callback = callback;
  lpa_client_data[slot_id].event_callback = event_callback;
  lpa_client_data[slot_id].user_data_ptr = user_data_ptr;

  return LPA_SUCCESS;
} /* lpa_save_client_data */


/*===========================================================================
FUNCTION LPA_CLEAR_CLIENT_DATA

DESCRIPTION
  Function clears the saved client data.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_clear_client_data (
  lpa_slot_id_enum_type              slot_id
)
{
  if(slot_id >= LPA_NUM_SLOTS)
  {
    return;
  }

  memset(&lpa_client_data[slot_id], 0x00, sizeof(lpa_client_data[slot_id]));
} /* lpa_clear_client_data */


/*==========================================================================
FUNCTION LPA_RESET

DESCRIPTION:
  Performs a reset of all sub modules within LPA.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
==========================================================================*/
void lpa_reset (
  lpa_slot_id_enum_type                  slot_id
)
{
  lpa_clear_client_data(slot_id);
  lpa_state_reset(slot_id);
  lpa_http_cleanup(slot_id);
  lpa_mmgsdi_command_cleanup(slot_id);
  lpa_lpd_reset(slot_id);
} /* lpa_reset */


/*===========================================================================
FUNCTION LPA_WAIT

DESCRIPTION
  Main wait routine for the LPA task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals that are set. The calling
  function can then check for the reason for exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
static rex_sigs_type lpa_wait (
  rex_sigs_type requested_mask
)
{
  rex_sigs_type          rex_signals_mask = 0x00;  /* Mask of signals returned by REX */

  /* --------------------------------------------------------------------
  Loop until the current signals from REX match with one or more of
  the requested signals.
  -------------------------------------------------------------------- */
  do
  {
    /* ---------------------------------------------------------------
    Perform the real wait to wait for a command...or to force a
    context switch to allow higher priority tasks to run.
    ---------------------------------------------------------------- */
    rex_signals_mask = rex_wait(requested_mask | LPA_DOG_RPT_TIMER_SIG);
    /* DOG HeartBeat
        1. A single shared timer resource among all clients is used to register for dog.
        2. Dog will send the signals to the registered tasks (“ping”) instead of going through timer,
           through dog’s own heartbeat, and the tasks will respond to wdog (“pong”)
           Voluntary dog reporting is supported (pongs without pings).
        3. Hence we use dog_hb_report() to report to the dog and in turn pet it.
     */
     (void) rex_clr_sigs( rex_self(), LPA_DOG_RPT_TIMER_SIG );
     dog_hb_report(lpa_dog_rpt_var);

    /* do while mask requested does not match current mask */
  } while ((rex_signals_mask & requested_mask) == 0);

  return rex_signals_mask;
} /* lpa_wait */


/*===========================================================================
FUNCTION LPA_HANDLE_STOP_SIG

DESCRIPTION
  This function is called to process a stop signal received by the LPA
  task.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_handle_stop_sig (
  void
)
{
  lpa_slot_id_enum_type                   slot_id = LPA_SLOT_1;

  LPA_MSG_HIGH_0("LPA: GOT TASK_STOP_SIG");

   /* Clear signal */
  (void) rex_clr_sigs(rex_self(), LPA_TASK_STOP_SIG);

  LPA_UTIL_ENTER_TASK_STATE_CRIT_SECT;
  lpa_task_stopped = TRUE;
  LPA_UTIL_LEAVE_TASK_STATE_CRIT_SECT;

  while(slot_id < LPA_NUM_SLOTS)
  {
    lpa_cmd_cleanup(slot_id);
    lpa_reset(slot_id);
    lpa_timer_cleanup(slot_id);
    slot_id++;
  }
  /* Close MMGSDI sessions to the ISDR as well */
  lpa_mmgsdi_reset();

  qmi_uim_http_dereg();
} /* lpa_handle_stop_sig */


/*===========================================================================
FUNCTION LPA_TASK_INIT_POST_STARTUP

DESCRIPTION
  Function called to initialize the different LPA modules after RCINIT
  handshake.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void lpa_task_init_post_startup (
)
{
  lpa_lpd_cmd_init();
  lpa_mmgsdi_init();
  lpa_timer_init();
  lpa_http_init();
  lpa_efs_init();
  /* Register for watchdog */
  lpa_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) LPA_DOG_RPT_TIMER_SIG);
} /* lpa_task_init_post_startup */


/*===========================================================================
FUNCTION LPA_TASK_INIT

DESCRIPTION
  Function called to initialize the LPA task queue and globals

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None. Function should only be called once during task initialization.
  On targets with RCInit enabled, this function is called in RCInit context.
  Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void lpa_task_init (
  void
)
{
  DalDeviceHandle             *chip_info_ptr    = NULL;

  lpa_task_stopped = FALSE;

  lpa_util_crit_sect_init();

  lpa_cmd_init();

  if(DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &chip_info_ptr) == DAL_SUCCESS)
  {
    (void)DalChipInfo_GetChipId(chip_info_ptr, &lpa_chip_id);
  }
} /* lpa_task_init */


/*===========================================================================
FUNCTION      LPA_HANDLE_QMI_MODEM_TASK_READY_SIG

DESCRIPTION
  When the QMI Framework is ready the QMI UIM LPA service can register
  with it and that will allow clients to send messages across.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
static void lpa_handle_qmi_modem_task_ready_sig (
  void
)
{
  UIM_MSG_HIGH_0("QMI Modem task is ready");

  rex_clr_sigs(UIM_LPA_TCB,
               LPA_QMI_MODEM_TASK_READY_SIG);

  /* Make sure we don't re-do the initialization incase the signal is
     erroneously received more than once */
  if((lpa_ready_tasks &
      (0x01 << (uint8)LPA_QMI_MODEM_TYPE)) == FALSE)
  {
    /* QMI UIM HTTP service init */
    qmi_uim_http_service_init(UIM_LPA_TCB,
                               (rex_sigs_type)QMI_UIM_HTTP_QCSI_SIG);

    /* Mark that the QMI MODEM task is ready */
    lpa_ready_tasks |=
      (0x01 << (uint8)LPA_QMI_MODEM_TYPE);
  }
} /* lpa_handle_qmi_modem_task_ready_sig */


/*===========================================================================
FUNCTION    LPA_UNREGISTER_FOR_QMI_MODEM_SIG

DESCRIPTION
  Function called to unregister from RCEVT for QMI MODEM task.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
static void lpa_unregister_for_qmi_modem_sig (
  RCEVT_SIGEX_SIGREX           *qmi_modem_sig_ptr
)
{
  /* Unregister from RCEVT for QMIMODEM-task-ready notification
     and clear the corresponding signal. */
  if(qmi_modem_sig_ptr)
  {
    (void)rcevt_unregister_sigex_name("QMI_MODEM: READY",
                                      RCEVT_SIGEX_TYPE_SIGREX,
                                      qmi_modem_sig_ptr);
  }
} /* lpa_unregister_for_qmi_modem_sig */

/*===========================================================================
FUNCTION LPA_HANDLE_QMI_UIM_HTTP_CMD_Q_SIG

DESCRIPTION
  Function called to handle a http request posted to the QMI UIM HTTP CMD Q

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
static void lpa_handle_qmi_uim_http_cmd_q_sig (
  void
)
{
  (void)rex_clr_sigs(UIM_LPA_TCB,
                     QMI_UIM_HTTP_CMD_Q_SIG);
  if(qmi_uim_http_cmd_q_sig_handler() == FALSE)
  {
    /* If QMI UIM HTTP CMD Q is not empty, reset signal to
       continue processing commands */
    (void)rex_set_sigs(UIM_LPA_TCB,
                       QMI_UIM_HTTP_CMD_Q_SIG);
  }
} /* lpa_handle_qmi_uim_http_cmd_q_sig */

/*===========================================================================
FUNCTION LPA_HANDLE_QMI_UIM_HTTP_QCSI_SIG

DESCRIPTION
  LPA task calls this function when it receives a QCSI sig for QMI UIM HTTP
  service

DEPENDENCIES
  LPA task should be properly initialized prior to receiving this signal

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void lpa_handle_qmi_uim_http_qcsi_sig (
  void
)
{
  /* Clear signal */
  (void) rex_clr_sigs(UIM_LPA_TCB,
                      (rex_sigs_type)QMI_UIM_HTTP_QCSI_SIG);
  UIM_MSG_LOW_0("Got QMI_UIM_HTTP_QCSI_SIG");

  qmi_uim_http_handle_qcsi_sig(UIM_LPA_TCB,
                                (rex_sigs_type)QMI_UIM_HTTP_QCSI_SIG);
} /* lpa_handle_qmi_uim_http_qcsi_sig */


/*===========================================================================
FUNCTION LPA_MAIN

DESCRIPTION
  Entry point for the LPA task. This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_main (
  dword dummy
)
{
  rex_sigs_type                sigs       = LPA_CMD_SIG                        |
                                            LPA_LPD_CMD_SIG                    |
                                            LPA_MMGSDI_CMD_SIG                 |
                                            LPA_HTTP_RSP_SIG                   |
                                            QMI_UIM_HTTP_CMD_Q_SIG             |
                                            QMI_UIM_HTTP_QCSI_SIG              |
                                            LPA_SANITY_TIMER_EXPIRE_SIG_SLOT_1 |
                                            LPA_SANITY_TIMER_EXPIRE_SIG_SLOT_2 |
                                            LPA_HTTP_RETRY_TIMER_EXPIRE_SIG_SLOT_1 |
                                            LPA_HTTP_RETRY_TIMER_EXPIRE_SIG_SLOT_2 |
                                            LPA_TASK_STOP_SIG;
  rex_sigs_type                sigs_rcvd  = 0;
  RCEVT_SIGEX_SIGREX           stop_sig;
  RCINIT_GROUP                 lpa_group;
  RCEVT_SIGEX_SIGREX           lpa_qmi_modem_ready_sig;

  (void)dummy;

  lpa_tcb_ptr = rcinit_lookup_rextask("lpa");

  /* Clear all signals */
  (void) rex_clr_sigs(UIM_LPA_TCB, (rex_sigs_type)~0);

  stop_sig.signal   = UIM_LPA_TCB;
  stop_sig.mask     = LPA_TASK_STOP_SIG;
  lpa_group         = rcinit_lookup_group_rextask(UIM_LPA_TCB);
  rcinit_register_term_group(lpa_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);

  /* Register appropriately with RCINIT in order to get notified when the
     QMI MODEM task is ready so that we can then initiatlize QMI UIM LPA */
  lpa_qmi_modem_ready_sig.signal = UIM_LPA_TCB;
  lpa_qmi_modem_ready_sig.mask   = LPA_QMI_MODEM_TASK_READY_SIG;
  (void)rcevt_register_sigex_name("QMI_MODEM: READY",
                                  RCEVT_SIGEX_TYPE_SIGREX,
                                  &lpa_qmi_modem_ready_sig);
  rcinit_handshake_startup();

  (void)rcevt_signal_name("LPA:READY");

  lpa_task_init_post_startup();

  /* Set the LPA cmd sig on the LPA tcb so that it is able to process any
     command that was queued to it before we reached here (because LPA
     task init function is in GROUP4, it is possible that qmiuim might have
     called a LPA API and as a result we would have queued the request to
     LPA queue, but would not have set the LPA cmd sig as the LPA tcb was
     not valid (was null) at that point) */
  rex_set_sigs(UIM_LPA_TCB, LPA_CMD_SIG);

  /* Main Task Loop */
  for (;;)
  {
    /* Wait on signal */
    sigs_rcvd = lpa_wait(sigs);

    /*-----------------------------------------------------------------------------
                          Process Signal - LPA_TASK_STOP_SIG
    -----------------------------------------------------------------------------*/
    if (sigs_rcvd & LPA_TASK_STOP_SIG)
    {
      lpa_handle_stop_sig();
      break;
    } /* end if LPA_TASK_STOP_SIG */

    /*-----------------------------------------------------------------------------
      Process Signal - LPA_QMI_MODEM_TASK_READY_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & LPA_QMI_MODEM_TASK_READY_SIG)
    {
      lpa_handle_qmi_modem_task_ready_sig();
      lpa_unregister_for_qmi_modem_sig(&lpa_qmi_modem_ready_sig);
    }

    /*-----------------------------------------------------------------------------
      Process Signal - QMI_UIM_HTTP_QCSI_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & QMI_UIM_HTTP_QCSI_SIG)
    {
      lpa_handle_qmi_uim_http_qcsi_sig();
    }

    /*-----------------------------------------------------------------------------
      Process Signal - QMI_UIM_HTTP_CMD_Q_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & QMI_UIM_HTTP_CMD_Q_SIG)
    {
      lpa_handle_qmi_uim_http_cmd_q_sig();
    }

    LPA_UTIL_ENTER_DATA_CRIT_SECT;
    /*-----------------------------------------------------------------------------
                          Process Signal - LPA_SANITY_TIMER_EXPIRE_SIG_SLOT_1
    -----------------------------------------------------------------------------*/
    if (sigs_rcvd & LPA_SANITY_TIMER_EXPIRE_SIG_SLOT_1)
    {
      lpa_timer_handle_sanity_timer_expire_sig(LPA_SLOT_1);
    } /* end if LPA_SANITY_TIMER_EXPIRE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - LPA_SANITY_TIMER_EXPIRE_SIG_SLOT_2
    -----------------------------------------------------------------------------*/
    if (sigs_rcvd & LPA_SANITY_TIMER_EXPIRE_SIG_SLOT_2)
    {
      lpa_timer_handle_sanity_timer_expire_sig(LPA_SLOT_2);
    } /* end if LPA_SANITY_TIMER_EXPIRE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - LPA_HTTP_RETRY_TIMER_EXPIRE_SIG_SLOT_1
    -----------------------------------------------------------------------------*/
    if (sigs_rcvd & LPA_HTTP_RETRY_TIMER_EXPIRE_SIG_SLOT_1)
    {
      lpa_http_handle_http_retry_timer_expire_sig(LPA_SLOT_1);
    } /* end if LPA_HTTP_RETRY_TIMER_EXPIRE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - LPA_HTTP_RETRY_TIMER_EXPIRE_SIG_SLOT_2
    -----------------------------------------------------------------------------*/
    if (sigs_rcvd & LPA_HTTP_RETRY_TIMER_EXPIRE_SIG_SLOT_2)
    {
      lpa_http_handle_http_retry_timer_expire_sig(LPA_SLOT_2);
    } /* end if LPA_HTTP_RETRY_TIMER_EXPIRE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - LPA_MMGSDI_CMD_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & LPA_MMGSDI_CMD_SIG)
    {
      lpa_mmgsdi_handle_cmd_sig();
    } /* end if LPA_MMGSDI_CMD_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - LPA_HTTP_RSP_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & LPA_HTTP_RSP_SIG)
    {
      lpa_http_handle_cmd_sig();
    } /* end if LPA_HTTP_CMD_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - LPA_LPD_CMD_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & LPA_LPD_CMD_SIG)
    {
      lpa_lpd_handle_cmd_sig();
    } /* end if LPA_LPD_CMD_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - LPA_CMD_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & LPA_CMD_SIG)
    {
      lpa_cmd_handle_cmd_sig();
    } /* end if LPA_CMD_SIG */

    else
    {
      /* Reset sigs by masking out defined sigs and clearing out bad sigs */
      (void)rex_clr_sigs(UIM_LPA_TCB, (sigs_rcvd & ~sigs));
    } /* Invalid signal in LPA task */
    LPA_UTIL_LEAVE_DATA_CRIT_SECT;
  } /* end for(;;) */

  rcinit_unregister_term_group(lpa_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);
  rcinit_handshake_term();
  /* Deregister dog if leaving the task so that we don't ERR_FATAL */
  dog_hb_deregister(lpa_dog_rpt_var);
} /* lpa_main */

