/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    S I M L O C K   M A I N   T A S K


GENERAL DESCRIPTION

  This file contains the main SIMLOCK task and supporting functions.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_modem.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/07/16   stv     Move variables to heap to reduce stack usage
01/06/16   stv     Remote simlock support
12/31/15   stv     Send events to client from engine msg handling
10/06/15   stv     Fix for compilation issue
08/19/15   stv     Restrict native simlock locking when rsu is enabled
08/12/15   stv     Clear temp unlock data when temp unlock expires
07/09/15   bcho    Support for whitelist and blacklist codes in same category
07/09/15   bcho    Redundant checks for SIMLOCK
07/08/15   stv     Perform efs sync before calling client call back
07/07/15   stv     Fix to perform efs sync operation for critical file updates
07/06/15   stv     Clear temp unlock data when a permanent unlock/reset simlock req
06/29/15   stv     Start time in temp unlock req as current time
                   Graceful handling of PMIC reset during temp unlock
06/17/15   stv     send event to mmgsdi in case of relock msg
06/04/15   bcho    Run algorithm after a lock operation
05/23/15   stv     Check for NULL if client is registered before sending event
05/17/15   stv     Fix compiler errors
05/15/15   vv      Support for RSU set configuration in simlock
05/15/15   vv      Support for RSU in simlock
05/15/15   stv     SIMLOCK time module related changes
03/19/15   at      Introducing new relock feature in SIMLock
12/17/14   vv      Set emergency mode if remote SFS is enabled, but not ready
11/10/14   vv      Add support for setting emergency mode for all the RATs
11/05/14   am      Changes to eliminate memory leak
10/21/14   vv      Add support for late simlock config registration
10/10/14   vv      Add support for remote simlock configuration
09/29/14   vv      Added support for the remote SFS
09/29/14   vv      Added support for emergency only mode
08/29/14   vv      Added support for temporary unlock
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
06/17/14   tl      Featurize tmc.h
06/03/14   vv      Added support for OTA unlock
05/21/14   vv      Fix a possible stack overflow
05/12/14   vv      Fix compiler errors
04/24/14   tl      Fix callback NULL check
04/15/14   vv      Added the support for FEATURE_SEC_SFS
03/25/14   tl      Secondary revisions
03/18/14   tl      Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_msg.h"
#include "simlock_modem.h"
#include "simlocklib.h"
#include "simlock_platform.h"
#include "simlock_modem_ota.h"
#include "simlock_timer.h"
#include "simlock_sfs_remote.h"
#include "simlock_switch_sfs.h"
#include "simlock_cm.h"
#include "simlock_rsu.h"
#include "simlock_temp_unlock.h"
#include "simlock_time.h"
#include "simlock_file.h"
#include "simlock_efs.h"
#include "simlock_rsu_config.h"
#ifndef FEATURE_UIM_TEST_FRAMEWORK
#include "rex_types.h"
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
#include "rcinit_rex.h"
#include "simlock_validity.h"

#ifdef FEATURE_SIMLOCK
/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
static q_type                    simlock_cmd_q;

boolean                          simlock_task_stopped;

rex_crit_sect_type               simlock_task_state_crit_sect;
rex_crit_sect_type             * simlock_task_state_crit_sect_ptr = &simlock_task_state_crit_sect;

static simlock_evt_cb_type       simlock_event_callback = NULL;

static RCEVT_SIGEX_SIGREX        simlock_rcevt_siginfo_cm;
static boolean                   simlock_cm_task_ready_received = FALSE;

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/


/*===========================================================================
FUNCTION SIMLOCK_WAIT

DESCRIPTION
  Main wait routine for the SIMLOCK task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals). The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type simlock_wait
(
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
    rex_signals_mask = rex_wait(requested_mask);

  /* do while mask requested does not match current mask */
  } while ((rex_signals_mask & requested_mask) == 0);

  return (rex_signals_mask);
} /* simlock_wait */


/*===========================================================================
   FUNCTION:      SIMLOCK_QUEUE_CMD

   DESCRIPTION:
     This function allows client to put command into the SIMLOCK command queue
     specific to a SIMLOCK task.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     simlock_result_enum_type

     SIMLOCK_SUCCESS:          The command structure was properly generated
                              and queued onto the SIMLOCK Command Queue.

   SIDE EFFECTS:
     None
===========================================================================*/
simlock_result_enum_type  simlock_queue_cmd(
  simlock_task_cmd_type * task_cmd_ptr
)
{
  simlock_result_enum_type  result = SIMLOCK_SUCCESS;

  if (task_cmd_ptr == NULL )
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_ENTER_TASK_STATE_CRIT_SECT;

  /* Do not queue command if SIMLOCK task has been stopped */
  if(simlock_task_stopped)
  {
    UIM_MSG_ERR_0("SIMLOCK task stopped, cannot queue command");
    result = SIMLOCK_GENERIC_ERROR;
  }

  if (result == SIMLOCK_SUCCESS &&
      task_cmd_ptr->cmd_type == SIMLOCK_ENGINE_CMD)
  {
#if (defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)) && !defined(FEATURE_UIM_TEST_FRAMEWORK)
    if(simlock_rsu_config_is_lock_allowed(&task_cmd_ptr->data.engine) == FALSE)
    {
      result = SIMLOCK_INVALID_LOCK_DATA;
    }
#endif /* FEATURE_SIMLOCK_RSU && !FEATURE_UIM_TEST_FRAMEWORK */

    switch (task_cmd_ptr->data.engine.msg_type)
    {
      case SIMLOCK_SET_SIMLOCK_POLICY_MSG:
      case SIMLOCK_SET_LOCK_HCK_MSG:
      case SIMLOCK_SET_LOCK_CK_MSG:
      case SIMLOCK_UNLOCK_DEVICE_MSG:
      case SIMLOCK_RUN_ALGORITHM_MSG:
      case SIMLOCK_GET_STATUS_MSG:
      case SIMLOCK_GET_CATEGORY_DATA_MSG:
      case SIMLOCK_TEMPORARY_UNLOCK_MSG:
      case SIMLOCK_SET_DEVICE_MODE_MSG:
      case SIMLOCK_ADD_LOCK_CODES_CK_MSG:
      case SIMLOCK_ADD_LOCK_CODES_HCK_MSG:
      case SIMLOCK_RELOCK_DEVICE_MSG:
        break;

      case SIMLOCK_INIT_MSG:
      default:
        UIM_MSG_ERR_1("Invalid SIMLOCK command: 0x%x", task_cmd_ptr->data.engine.msg_type);
        result = SIMLOCK_UNSUPPORTED;
        break;
    }
  }

  if(result == SIMLOCK_SUCCESS)
  {
    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

    /* Put the message on the queue */
    q_put(&simlock_cmd_q, &task_cmd_ptr->link);

    /* Set the command queue signal */
    (void) rex_set_sigs(UIM_SIMLOCK_TCB, SIMLOCK_CMD_Q_SIG);
  }

  SIMLOCK_LEAVE_TASK_STATE_CRIT_SECT;

  return result;
} /* simlock_queue_cmd */


/*===========================================================================
FUNCTION SIMLOCK_MODEM_REGISTRATION

DESCRIPTION
  This function processes the registration request for the event or the
  remote SFS operations

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_modem_registration
(
  simlock_register_msg_req_type * req_msg_ptr
)
{
  if(req_msg_ptr == NULL)
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_MSG_MED_2("simlock_process_registration, operation: 0x%x, req: 0x%x",
                    req_msg_ptr->operation, req_msg_ptr->request_type);

  switch(req_msg_ptr->operation)
  {
    case SIMLOCK_REGISTER:
      if(req_msg_ptr->request_type == SIMLOCK_REGISTRATION_EVENT)
      {
        if(simlock_event_callback != NULL)
        {
          SIMLOCK_MSG_ERR_0("Client already registered for SIM Lock events");
          return SIMLOCK_INVALID_STATE;
        }

        simlock_event_callback = req_msg_ptr->cb_type.simlock_evt_cb;
      }
      else if(req_msg_ptr->request_type == SIMLOCK_REGISTRATION_REMOTE_SFS)
      {
        return simlock_sfs_remote_register_remote_cb(req_msg_ptr->cb_type.simlock_remote_req_cb);
      }
      break;

    case SIMLOCK_DEREGISTER:
      if(req_msg_ptr->request_type == SIMLOCK_REGISTRATION_EVENT)
      {
        simlock_event_callback = NULL;
      }
      else if(req_msg_ptr->request_type == SIMLOCK_REGISTRATION_REMOTE_SFS)
      {
        return simlock_sfs_remote_deregister_remote_cb();
      }
      break;

    default:
      SIMLOCK_MSG_ERR_0("Invalid event reg operation");
      return SIMLOCK_INCORRECT_PARAMS;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_modem_registration */


#ifndef FEATURE_SEC_SFS
/*===========================================================================
FUNCTION SIMLOCK_MODEM_PROCESS_MESSAGE_NO_SFS

DESCRIPTION
  This function processes messages when the SFS feature is unavailable. All
  the message types, except the run algorithm is returned as unsupported.
  In case the message type is run algorithm it is processed successfully

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_modem_process_message_no_sfs(
  const simlock_message_request_data_type * msg_req_ptr,
  simlock_message_response_data_type      * msg_resp_ptr
)
{
  uint8            slot_index     = 0;
  uint8            sub_cnt        = 0;

  if((msg_req_ptr == NULL) ||
     (msg_resp_ptr == NULL))
  {
    SIMLOCK_MSG_ERR_0("input parameters error");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  UIM_MSG_MED_0("simlock_modem_process_message_no_sfs");

  msg_resp_ptr->msg_type = msg_req_ptr->msg_type;

  /* for all the requests other than run algo,
     return not supported */
  if(msg_req_ptr->msg_type != SIMLOCK_RUN_ALGORITHM_MSG)
  {
    return SIMLOCK_UNSUPPORTED;
  }

  /* update all the subscriptions on a slot as valid */
  for(slot_index = 0; slot_index < SIMLOCK_SLOT_COUNT_MAX; slot_index++)
  {
    for(sub_cnt = 0;
        (sub_cnt < msg_req_ptr->message.algorithm_msg.card_data[slot_index].num_subscriptions &&
         sub_cnt < SIMLOCK_SUBS_COUNT_MAX);
        sub_cnt++)
    {
      msg_resp_ptr->message.run_algo_resp.card_validity_data[slot_index].num_subscriptions++;
      msg_resp_ptr->message.run_algo_resp.card_validity_data[slot_index].subscription_data[sub_cnt].is_subscription_valid =
              TRUE;
    }
  }

  return SIMLOCK_SUCCESS;
} /* simlock_modem_process_message_no_sfs */
#endif /* !FEATURE_SEC_SFS */


/*===========================================================================
FUNCTION SIMLOCK_SEND_UNLOCK_EVENT

DESCRIPTION
  This function create an unlock event message and call the event callback
  to send the information back to the client.

DEPENDENCIES
  Client must have registered for simlock event cb

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_send_unlock_event(
  simlock_result_enum_type                     simlock_status,
  const simlock_unlock_device_msg_req_type   * unlock_req_ptr,
  const simlock_unlock_device_msg_resp_type  * unlock_resp_ptr
)
{
  simlock_event_data_type   event_data;

  if (unlock_req_ptr == NULL ||
      unlock_resp_ptr == NULL ||
      simlock_event_callback == NULL)
  {
    return;
  }

  memset(&event_data, 0x00, sizeof(event_data));
  event_data.evt_type = SIMLOCK_UNLOCK_EVENT;
  event_data.message.unlock.num_retries_max =
    unlock_resp_ptr->num_retries_max;
  event_data.message.unlock.curr_retries =
    unlock_resp_ptr->curr_retries;
  event_data.message.unlock.category =
    unlock_req_ptr->category;
  event_data.message.unlock.slot =
    unlock_req_ptr->slot;

  simlock_event_callback(simlock_status, event_data);
} /* simlock_send_unlock_event */


/*===========================================================================
FUNCTION SIMLOCK_SEND_TEMPORARY_UNLOCK_EVENT

DESCRIPTION
  This function creates a temporary unlock event message and call
  the event callback to send the information back to the client.

DEPENDENCIES
  Client must have registered for simlock event cb

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_send_temporary_unlock_event
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category
)
{
  simlock_event_data_type   event_data;

  if(simlock_event_callback == NULL)
  {
    return;
  }

  memset(&event_data, 0x00, sizeof(event_data));

  event_data.evt_type = SIMLOCK_TEMPORARY_UNLOCK_EVENT;
  event_data.message.temporary_unlock.slot     = slot;
  event_data.message.temporary_unlock.category = category;

  SIMLOCK_MSG_LOW_2("simlock_send_temporary_unlock_event slot 0x%x category 0x%x", slot, category);

  simlock_event_callback(SIMLOCK_SUCCESS, event_data);
} /* simlock_send_temporary_unlock_event */


/*===========================================================================
FUNCTION SIMLOCK_SEND_DEVICE_MODE_EVENT

DESCRIPTION
  This function creates a device mode event, whenever the operating mode of
  the device changes and calls the event callback to send the information
  back to the client.

DEPENDENCIES
  Client must have registered for simlock event cb

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_send_device_mode_event
(
  simlock_device_mode_enum_type   device_mode
)
{
  simlock_event_data_type   event_data;

  if(simlock_event_callback == NULL)
  {
    return;
  }

  memset(&event_data, 0x00, sizeof(event_data));

  event_data.evt_type = SIMLOCK_DEVICE_MODE_EVENT;

  event_data.message.device_mode.mode = device_mode;

  simlock_event_callback(SIMLOCK_SUCCESS, event_data);
} /* simlock_send_device_mode_event */


/*===========================================================================
FUNCTION SIMLOCK_SEND_REMOTE_REGISTER_EVENT

DESCRIPTION
  This function creates a register remote SFS event, whenever the client
  (QMI UIM) registers for remote simlock configuration.

DEPENDENCIES
  Client must have registered for simlock event cb

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_send_remote_sfs_register_event
(
  void
)
{
  simlock_event_data_type   event_data;

  if(simlock_event_callback == NULL)
  {
    return;
  }

  memset(&event_data, 0x00, sizeof(event_data));

  event_data.evt_type = SIMLOCK_REGISTER_REMOTE_SFS_EVENT;

  simlock_event_callback(SIMLOCK_SUCCESS, event_data);
} /* simlock_send_remote_sfs_register_event */


/*===========================================================================
FUNCTION SIMLOCK_SEND_LOCK_EVENT

DESCRIPTION
  This function creates a lock event message and calls the event
  callback to send the information back to the client.

DEPENDENCIES
  Client must have registered for simlock event cb

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_send_lock_event
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category
)
{
  simlock_event_data_type   event_data;

  if(simlock_event_callback == NULL)
  {
    return;
  }

  memset(&event_data, 0x00, sizeof(event_data));

  event_data.evt_type = SIMLOCK_LOCK_EVENT;
  event_data.message.lock.slot     = slot;
  event_data.message.lock.category = category;

  simlock_event_callback(SIMLOCK_SUCCESS, event_data);
} /* simlock_send_lock_event */


/*===========================================================================
FUNCTION SIMLOCK_SEND_EVENT

DESCRIPTION
  This function creates a event message based on the message type
  in the engine command and calls the event callback to send the
  information back to the client.

DEPENDENCIES
  Client must have registered for simlock event cb

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_send_event(
  simlock_result_enum_type                  status,
  const simlock_message_request_data_type  *req_msg_ptr,
  const simlock_message_response_data_type *resp_data_ptr)
{
  /* Return if any of input parameters is NULL
     or if there is no client registered for events */
  if(req_msg_ptr == NULL)
  {
    return;
  }

  switch(req_msg_ptr->msg_type)
  {
    case SIMLOCK_SET_LOCK_CK_MSG:
      if(status == SIMLOCK_SUCCESS)
      {
        simlock_send_lock_event(req_msg_ptr->message.lock_ck_msg.slot,
                                req_msg_ptr->message.lock_ck_msg.category.category_type);
      }
      break;

    case SIMLOCK_SET_LOCK_HCK_MSG:
      if(status == SIMLOCK_SUCCESS)
      {
        simlock_send_lock_event(req_msg_ptr->message.lock_hck_msg.slot,
                                req_msg_ptr->message.lock_hck_msg.category.category_type);
      }
      break;

    case SIMLOCK_ADD_LOCK_CODES_HCK_MSG:
      if(status == SIMLOCK_SUCCESS)
      {
        simlock_send_lock_event(req_msg_ptr->message.add_lock_codes_hck_msg.slot,
                                req_msg_ptr->message.add_lock_codes_hck_msg.category.category_type);
      }
      break;

    case SIMLOCK_ADD_LOCK_CODES_CK_MSG:
      if(status == SIMLOCK_SUCCESS)
      {
        simlock_send_lock_event(req_msg_ptr->message.add_lock_codes_ck_msg.slot,
                                req_msg_ptr->message.add_lock_codes_ck_msg.category.category_type);
      }
      break;

    case SIMLOCK_RELOCK_DEVICE_MSG:
      if(status == SIMLOCK_SUCCESS)
      {
        simlock_send_lock_event(req_msg_ptr->message.relock_msg.slot,
                                req_msg_ptr->message.relock_msg.category_type);
      }
      break;

    case SIMLOCK_SET_DEVICE_MODE_MSG:
      if(status == SIMLOCK_SUCCESS)
      {
        simlock_send_device_mode_event(req_msg_ptr->message.device_mode_msg.mode);
      }
      break;

    case SIMLOCK_TEMPORARY_UNLOCK_MSG:
      if(status == SIMLOCK_SUCCESS)
      {
        simlock_send_temporary_unlock_event(req_msg_ptr->message.temporary_unlock_msg.slot,
                                            req_msg_ptr->message.temporary_unlock_msg.category);
      }
      break;

    case SIMLOCK_UNLOCK_DEVICE_MSG:
      if(resp_data_ptr != NULL)
      {
        simlock_send_unlock_event(status,
                                  &req_msg_ptr->message.unlock_msg,
                                  &resp_data_ptr->message.unlock_resp);
      }
      break;

    case SIMLOCK_INIT_MSG:
    default:
      break;
  }
} /* simlock_send_event */


/*===========================================================================
FUNCTION SIMLOCK_HANDLE_CM_TASK_READY_SIG

DESCRIPTION
  Function called to handle the task-ready notification from RCEVT for
  CM task. This notification is received when CM task calls
  rcevt_signal_name().
  Upon receiving the notification, the SFS type and device mode are
  checked. If the SFS type is local and device mode is emergency only,
  then it calls the CM API to reset the emergency mode and enters a
  synchronous wait.
  This function should only be called once during task initialization.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
static void simlock_handle_cm_task_ready_sig
(
  void
)
{
  /* if the CM task ready event is already received,
     then return */
  if(simlock_cm_task_ready_received)
  {
    return;
  }

  UIM_MSG_HIGH_0("CM task is up...");

  /* Unregister from RCEVT for CM-task-ready notification
     and clear the corresponding signal. */
  (void)rcevt_unregister_sigex_name("cm:ready",
                                    RCEVT_SIGEX_TYPE_SIGREX,
                                    &simlock_rcevt_siginfo_cm);

  rex_clr_sigs(rex_self(), SIMLOCK_RCEVT_CM_TASK_READY_SIG);

  /* if the device is in emergency only mode, this function issues
     a call to CM API and enters a synchronous wait */
  if (simlock_cm_is_device_in_emergency_mode())
  {
    (void)simlock_set_device_mode_sync(SIMLOCK_DEVICE_EMERGENCY_ONLY_MODE);
  }
  simlock_cm_task_ready_received = TRUE;
} /* simlock_handle_cm_task_ready_sig */


/*===========================================================================
FUNCTION SIMLOCK_HANDLE_CMD_SIG

DESCRIPTION
  This function, called in simlock_main, is called to process a signals of
  command.

DEPENDENCIES
  SIMLOCK task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_handle_cmd_sig(void)
{
  simlock_task_cmd_type *                simlock_cmd_ptr  = NULL;
  simlock_result_enum_type               simlock_status   = SIMLOCK_GENERIC_ERROR;
  simlock_message_response_data_type   * resp_data_ptr    = NULL;
  simlock_ota_unlock_resp_type           ota_resp_data;
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  rex_priority_type                      priority_cache   = 0;
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
#if defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
  simlock_data_type                      rsu_resp_data    = {0, NULL};
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

  memset(ota_resp_data, 0x00, sizeof(simlock_ota_unlock_resp_type));

  simlock_cmd_ptr = (simlock_task_cmd_type*)(q_get(&simlock_cmd_q));

  /* If there is a command on either of the queues */
  if(simlock_cmd_ptr != NULL)
  {
    UIM_MSG_HIGH_0("SIMLOCK_CMD_Q_SIG received");

    switch(simlock_cmd_ptr->cmd_type)
    {
      case SIMLOCK_ENGINE_CMD:
        SIMLOCK_CHECK_AND_MEM_MALLOC(resp_data_ptr,
                                     sizeof(simlock_message_response_data_type));
        if (resp_data_ptr == NULL)
        {
          UIM_MSG_ERR_0("failed to allocate response structure");

          /* invoke the call back */
          if (simlock_cmd_ptr->simlock_cb_ptr != NULL)
          {
            simlock_cmd_ptr->simlock_cb_ptr(SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED,
                                            NULL,
                                            simlock_cmd_ptr->user_data_ptr);
          }
          break;
        }

#ifndef FEATURE_UIM_TEST_FRAMEWORK
        /* Reduce the priority of the SIM Lock task when performing potentially
           time intensive commands to reduce risk of staving lower priority tasks. */
        if (simlock_cmd_ptr->data.engine.msg_type == SIMLOCK_SET_LOCK_CK_MSG   ||
            simlock_cmd_ptr->data.engine.msg_type == SIMLOCK_RELOCK_DEVICE_MSG ||
            simlock_cmd_ptr->data.engine.msg_type == SIMLOCK_UNLOCK_DEVICE_MSG ||
            simlock_cmd_ptr->data.engine.msg_type == SIMLOCK_ADD_LOCK_CODES_CK_MSG)
        {
          priority_cache = rex_get_pri();
          rex_set_task_pri(rex_self(), (rex_priority_type)SIMLOCK_TASK_REDUCED_PRIORITY);
        }
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_SEC_SFS
        simlock_status = simlock_process_message(&simlock_cmd_ptr->data.engine,
                                                 resp_data_ptr);
#else
        UIM_MSG_HIGH_0("SFS not available");

        /* if the SFS is not available then it is assumed
           that the SIMLock engine is not available either.
           In this case, we let all the cards to initialize */
        simlock_status = simlock_modem_process_message_no_sfs(&simlock_cmd_ptr->data.engine,
                                                              resp_data_ptr);
#endif /* FEATURE_SEC_SFS */

#ifndef FEATURE_UIM_TEST_FRAMEWORK
        /* Restore the task priority after time intensive command has completed */
        if ((simlock_cmd_ptr->data.engine.msg_type == SIMLOCK_SET_LOCK_CK_MSG   ||
             simlock_cmd_ptr->data.engine.msg_type == SIMLOCK_RELOCK_DEVICE_MSG ||
             simlock_cmd_ptr->data.engine.msg_type == SIMLOCK_UNLOCK_DEVICE_MSG ||
             simlock_cmd_ptr->data.engine.msg_type == SIMLOCK_ADD_LOCK_CODES_CK_MSG) &&
            (priority_cache != 0))
        {
          rex_set_task_pri(rex_self(), priority_cache);
        }
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_SIMLOCK_RSU
        if(simlock_cmd_ptr->data.engine.msg_type == SIMLOCK_UNLOCK_DEVICE_MSG)
        {
          simlock_temp_unlock_reset_config(SIMLOCK_TEMP_UNLOCK_RESET_CONFIG_IF_UNLOCKED);
        }
#endif /* FEATURE_SIMLOCK_RSU */

        /* Check if a file system commit is required.
           This is to be done before calling client call back */
        simlock_file_commit_to_fs_if_needed();


        if (simlock_cmd_ptr->simlock_cb_ptr != NULL)
        {
          simlock_cmd_ptr->simlock_cb_ptr(simlock_status,
                                          resp_data_ptr,
                                          simlock_cmd_ptr->user_data_ptr);
        }

        if(resp_data_ptr->msg_type == SIMLOCK_GET_CATEGORY_DATA_MSG)
        {
          SIMLOCK_MEM_FREE(resp_data_ptr->message.get_category_data_resp.category_data_whitelist.whitelist_data_ptr);
          SIMLOCK_MEM_FREE(resp_data_ptr->message.get_category_data_resp.category_data_blacklist.blacklist_data_ptr);
        }

        SIMLOCK_MEM_FREE(resp_data_ptr);
        break;

      case SIMLOCK_REGISTER_CMD:
        /* No callback for SIM Lock event registration as there is not much MMGSDI
           can do even if registration fails. */
        simlock_status = simlock_modem_registration(&simlock_cmd_ptr->data.register_msg);

        /* When the registration for remote SFS operation is successfully
           processed, we inform the registered client (i.e. MMGSDI). This
           causes the client to trigger simlock_run_algo request */
        if(simlock_event_callback != NULL &&
           simlock_status == SIMLOCK_SUCCESS &&
           simlock_cmd_ptr->data.register_msg.operation == SIMLOCK_REGISTER &&
           simlock_cmd_ptr->data.register_msg.request_type == SIMLOCK_REGISTRATION_REMOTE_SFS)
        {
          /* if the device is in emergency only mode, this function issues
             a call to CM API and enters a synchronous wait */
          if (simlock_cm_is_device_in_emergency_mode())
          {
            /* if the CM task is not ready, then do not set
               the emergency mode now. It will be set, when we receive
               the CM task ready signal */
            if (simlock_cm_task_ready_received)
            {
              (void)simlock_set_device_mode_sync(SIMLOCK_DEVICE_EMERGENCY_ONLY_MODE);
            }
          }
          else
          {
            /* in case we set the device in emergency mode, when CM task
               ready was received because remote SFS was not ready, need to
               reset the device to normal operating mode, since the device
               is not in emergency mode. Even if the device was not set in
               emergency mode at at the CM task ready, no harm in invoking it */
            if (simlock_cm_task_ready_received)
            {
              (void)simlock_set_device_mode_sync(SIMLOCK_DEVICE_NORMAL_OPERATION_MODE);
            }
          }

          /* Check if a file system commit is required.
             This is to be done before calling client call back */
          simlock_file_commit_to_fs_if_needed();

          /* Send the event to the registered client */
          simlock_send_remote_sfs_register_event();
        }
        break;

      case SIMLOCK_OTA_UNLOCK_CMD:
        simlock_status = simlock_modem_ota_process_unlock(&simlock_cmd_ptr->data.ota_unlock_msg.unlock_req,
                                                          simlock_cmd_ptr->data.ota_unlock_msg.slot,
                                                          ota_resp_data);

        /* Check if a file system commit is required.
           This is to be done before calling client call back */
        simlock_file_commit_to_fs_if_needed();

        if(simlock_cmd_ptr->data.ota_unlock_msg.simlock_ota_cb_ptr != NULL)
        {
          simlock_cmd_ptr->data.ota_unlock_msg.simlock_ota_cb_ptr(simlock_status,
                                                                  ota_resp_data,
                                                                  simlock_cmd_ptr->user_data_ptr);
        }
        break;

      case SIMLOCK_TEMPORARY_UNLOCK_TIMER_EXPIRY_CMD:
        simlock_status = simlock_timer_process_expiry_request(&simlock_cmd_ptr->data.timer_expiry_msg);
        break;

      case SIMLOCK_SET_REMOTE_SFS_CMD:
        simlock_status = simlock_set_remote_sfs_request(&simlock_cmd_ptr->data.remote_sfs_msg);

        /* Check if a file system commit is required.
           This is to be done before calling client call back */
        simlock_file_commit_to_fs_if_needed();

        if(simlock_cmd_ptr->data.remote_sfs_msg.remote_sfs_cb_ptr != NULL)
        {
          simlock_cmd_ptr->data.remote_sfs_msg.remote_sfs_cb_ptr(simlock_status,
                                                                 simlock_cmd_ptr->user_data_ptr);
        }
        break;

      case SIMLOCK_TIME_HANDLE_GPS_TIMER_EXIPIRY_CMD:
#if defined(FEATURE_SIMLOCK_RSU) && defined(FEATURE_SIMLOCK_QMI_LOC)
        simlock_time_handle_gps_timer_expiry_cmd();
#endif /* FEATURE_SIMLOCK_RSU && FEATURE_SIMLOCK_QMI_LOC */
        break;

      case SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD:
#ifdef FEATURE_SIMLOCK_RSU
        simlock_time_handle_source_time_info_cmd(&simlock_cmd_ptr->data.source_time_info_msg);
#endif /* FEATURE_SIMLOCK_RSU */
        break;

      case SIMLOCK_RSU_CMD:
#if defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
        simlock_status = simlock_rsu_process_message(&simlock_cmd_ptr->data.rsu_msg,
                                                     &rsu_resp_data);

        /* Check if a file system commit is required.
           This is to be done before calling client call back */
        simlock_file_commit_to_fs_if_needed();

        if(simlock_cmd_ptr->data.rsu_msg.rsu_cb_ptr != NULL)
        {
          simlock_cmd_ptr->data.rsu_msg.rsu_cb_ptr(simlock_status,
                                                   rsu_resp_data,
                                                   simlock_cmd_ptr->user_data_ptr);
        }
        SIMLOCK_MEM_FREE(simlock_cmd_ptr->data.rsu_msg.message.rsu_data.data_ptr);
        SIMLOCK_MEM_FREE(rsu_resp_data.data_ptr);
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */
        break;

      case SIMLOCK_TEMPORARY_UNLOCK_START_CMD:
#ifdef FEATURE_SIMLOCK_RSU
        simlock_status = simlock_temp_unlock_process_pending_timer_expiry();
#endif /* FEATURE_SIMLOCK_RSU */
        break;

      case SIMLOCK_TEMPORARY_UNLOCK_BACKUP_TIME_UPDATE_CMD:
#ifdef FEATURE_SIMLOCK_RSU
        simlock_time_process_backup_time_update();
#endif /* FEATURE_SIMLOCK_RSU */
        break;
    }

    /* Check if a file system commit is required.
       If the efs sync is already performed above, this would
       just return without performing efs sync again for
       the same command */
    simlock_file_commit_to_fs_if_needed();

    SIMLOCK_MEM_FREE(simlock_cmd_ptr);
  }

  (void) rex_clr_sigs(rex_self(), SIMLOCK_CMD_Q_SIG);

  /* Set the signal if there are one or more commands to look at */
  if (q_cnt(&simlock_cmd_q) > 0)
  {
    /*  Items on the queue. Set the signal */
    (void) rex_set_sigs(rex_self(), SIMLOCK_CMD_Q_SIG);
  }
} /* simlock_handle_cmd_sig */


/*===========================================================================
FUNCTION SIMLOCK_FLUSH_QUEUE

DESCRIPTION
  This function is called to send error to the client so that client
  will call it's callback.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void simlock_flush_queue (
  void
)
{
  simlock_task_cmd_type  *simlock_cmd_ptr = NULL;

  while (q_cnt(&simlock_cmd_q)!= 0)
  {
    simlock_cmd_ptr = (simlock_task_cmd_type *)(q_get(&simlock_cmd_q));

    if (simlock_cmd_ptr != NULL)
    {
      /* invoke the call back */
      if (simlock_cmd_ptr->simlock_cb_ptr != NULL)
      {
        SIMLOCK_MSG_LOW_0("SIMLOCK sending error cnf to client");
        simlock_cmd_ptr->simlock_cb_ptr(SIMLOCK_GENERIC_ERROR,
                                        NULL,
                                        simlock_cmd_ptr->user_data_ptr);
      }
      SIMLOCK_MEM_FREE(simlock_cmd_ptr);
    }
  }
}/* simlock_flush_queue */


/*===========================================================================
FUNCTION SIMLOCK_HANDLE_STOP_SIG

DESCRIPTION
  This function is called to process a stop signal received by the SIMLOCK
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
static void simlock_handle_stop_sig (
  void
)
{
  UIM_MSG_HIGH_0("SIMLOCK: GOT TASK_STOP_SIG");

#ifdef FEATURE_SIMLOCK_RSU
  simlock_time_deinit(simlock_temp_unlock_is_active());
#endif /* FEATURE_SIMLOCK_RSU */

   /* Clear signal */
  (void) rex_clr_sigs(rex_self(), SIMLOCK_TASK_STOP_SIG);

  SIMLOCK_ENTER_TASK_STATE_CRIT_SECT;
  simlock_task_stopped = TRUE;
  SIMLOCK_LEAVE_TASK_STATE_CRIT_SECT;

  simlock_flush_queue();

  /* Free all memory held */
  simlock_validity_deinit();
} /* simlock_handle_stop_sig */
#endif /* FEATURE_SIMLOCK */


/*===========================================================================
FUNCTION SIMLOCK_TASK_INIT

DESCRIPTION
  Function called to initialize the SIMLOCK task queue

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit enabled, this function is called in RCInit context.
  Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void simlock_task_init
(
  void
)
{
#ifdef FEATURE_SIMLOCK
  /* Initalize command queue */
  (void) q_init(&simlock_cmd_q);

  simlock_task_stopped   = FALSE;

  rex_init_crit_sect(simlock_task_state_crit_sect_ptr);

  simlock_validity_init();
#endif /* FEATURE_SIMLOCK */
} /* simlock_task_init */


/*===========================================================================
FUNCTION SIMLOCK_MAIN

DESCRIPTION
  Entry point for the SIMLOCK task.  This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void simlock_main
(
  dword dummy
)
{
#ifdef FEATURE_SIMLOCK
  rex_sigs_type                       sigs = SIMLOCK_CMD_Q_SIG                 |
                                             SIMLOCK_RCEVT_CM_TASK_READY_SIG   |
                                             SIMLOCK_TIME_QMI_LOC_READY_SIG    |
                                             SIMLOCK_TASK_STOP_SIG;
  rex_sigs_type                       sigs_rcvd    = 0x00;
  simlock_message_request_data_type  *init_req_ptr = NULL;
  RCEVT_SIGEX_SIGREX                  stop_sig;
  RCINIT_GROUP                        simlock_group;
#endif /* FEATURE_SIMLOCK */

  (void)dummy;

#ifdef FEATURE_SIMLOCK
  /* Clear all signals */
  (void) rex_clr_sigs(UIM_SIMLOCK_TCB, (rex_sigs_type)~0);

  stop_sig.signal   = UIM_SIMLOCK_TCB;
  stop_sig.mask     = SIMLOCK_TASK_STOP_SIG;
  simlock_group     = rcinit_lookup_group_rextask(UIM_SIMLOCK_TCB);

  rcinit_register_term_group(simlock_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);

  /* Register for CM ready signals.
     Perform task-ready sig registrations before doing the rcinit_handshake
     to avoid race-conditions where the notifying tasks might start early,
     before SIMLOCK is even able to register for task-ready sigs for those
     tasks */
  simlock_rcevt_siginfo_cm.signal = rex_self();
  simlock_rcevt_siginfo_cm.mask   = SIMLOCK_RCEVT_CM_TASK_READY_SIG;
  (void)rcevt_register_sigex_name("cm:ready",
                                  RCEVT_SIGEX_TYPE_SIGREX,
                                  &simlock_rcevt_siginfo_cm);
#endif /* FEATURE_SIMLOCK */

  rcinit_handshake_startup();

#ifdef FEATURE_SIMLOCK

#ifdef FEATURE_SEC_SFS
  SIMLOCK_CHECK_AND_MEM_MALLOC(init_req_ptr, sizeof(simlock_message_request_data_type));
  if(init_req_ptr == NULL)
  {
    ERR_FATAL("Could not get the memory to perform simlock init", 0, 0, 0);
  }
  init_req_ptr->msg_type = SIMLOCK_INIT_MSG;
  (void)simlock_process_message(init_req_ptr, NULL);
  SIMLOCK_FREE(init_req_ptr);
#endif /* FEATURE_SEC_SFS */

  /* Main Task Loop */
  for (;;)
  {
    /* Wait on signal */
    sigs_rcvd = simlock_wait( sigs );

    /*-----------------------------------------------------------------------------
                          Process Signal - SIMLOCK_TASK_STOP_SIG
    -----------------------------------------------------------------------------*/
    if (sigs_rcvd & SIMLOCK_TASK_STOP_SIG)
    {
      simlock_handle_stop_sig();
      break;
    } /* end if SIMLOCK_TASK_STOP_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - SIMLOCK_CMD_Q_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & SIMLOCK_CMD_Q_SIG)
    {
      /* if SIMLOCK task has been stopped, we break the loop. So no need to
         check if the task has been stopped */
      simlock_handle_cmd_sig();
    } /* end if SIMLOCK_CMD_Q_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - SIMLOCK_RCEVT_CM_TASK_READY_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & SIMLOCK_RCEVT_CM_TASK_READY_SIG)
    {
      /* if SIMLOCK task has been stopped, we break the loop. So no need to
         check if the task has been stopped */
      simlock_handle_cm_task_ready_sig();
    } /* end if SIMLOCK_RCEVT_CM_TASK_READY_SIG */

#if defined(FEATURE_SIMLOCK_RSU) && defined(FEATURE_SIMLOCK_QMI_LOC)
    /*-----------------------------------------------------------------------------
                          Process Signal - SIMLOCK_TIME_QMI_LOC_READY_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & SIMLOCK_TIME_QMI_LOC_READY_SIG)
    {
      simlock_time_handle_qmi_loc_ready_sig();
    } /* end if SIMLOCK_TIME_QMI_LOC_READY_SIG */
#endif /* FEATURE_SIMLOCK_RSU && FEATURE_SIMLOCK_QMI_LOC */

    else
    {
      UIM_MSG_ERR_1("SIMLOCK TASK INVALID SIGS: %x", sigs_rcvd);
      /* Reset sigs by masking out defined sigs and clearing out bad sigs */
      (void)rex_clr_sigs(UIM_SIMLOCK_TCB, (sigs_rcvd & ~sigs) );
    } /* Invalid signal in SIM Lock task */
  } /* end for(;;) */

  if (!simlock_cm_task_ready_received)
  {
    /* We didn't receive the CM task ready notification,
       so unregister here from RCEVT for CM-task-ready notification
       and clear the corresponding signal. */
    (void)rcevt_unregister_sigex_name("cm:ready",
                                       RCEVT_SIGEX_TYPE_SIGREX,
                                       &simlock_rcevt_siginfo_cm);
  }

  rcinit_unregister_term_group(simlock_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);
  rcinit_handshake_term();
#endif /* FEATURE_SIMLOCK */
} /* simlock_main */

