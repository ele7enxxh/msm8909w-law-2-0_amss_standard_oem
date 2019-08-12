/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S I G N A L   H A N D L E R

                  U S E R   I D E N T I T Y   M O D U L E

GENERAL DESCRIPTION
  This module contains all of the functions that handle signals to the UIM
  task.  In general there is one signal handling function per signal.

EXTERNALIZED FUNCTIONS

  uim_signal_handler
    Calls the signal handling functions for each UIM task signal that needs to be
    handled.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  uim_signal_handler shall be called after rex_wait returns a mask of signals that
  are set.

Copyright (c) 2001-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_sigs.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/16   na      Initializing status as UIM_UNINITIALIZED_S
06/20/16   ks      Resetting the command transacted flag if the 
                   response is not received from card
05/23/16   lm      Do not clear voltage class upon receipt of RESET_F
05/10/16   gm      Handling BTSAP connection unavailable signal
05/10/16   sam     Remove the logic to store timeout info in EFS
05/10/16   ks      Not clearing the card parameters during UIM_POWER_UP_UIM_PASSIVE_F
05/09/16   sam     reject open channel command if it failed during powerup
04/28/16   na      Restricting EFS APDU Logging with Feature
02/21/16   sam     Remove check for channel ID of previous command when  
                   get resposne is received in stream command
02/15/16   ks      Fix to make hotswap work after passive power up
02/02/16   vdc     Migrated to new MCFG Refresh Done API
11/17/15   sam     Stop recovery process only if card removal debounce is succesfull
11/05/15   ks      Rel12 polling enhancement
10/12/15   ks      Allowing streaming APDU to card in passive mode.
10/06/15   ks      Allowing streaming GET RESPONSE apdu if there is 
                   no interleaving command
07/14/15   ks      Sending err report for the on-going cmd during 
                   task stop signal handling
06/04/15   ks      UIM handling task stop signal when interface not used
05/13/15   ll      Splitting the slot specific NVs from MCFG refreshed NVs
04/27/15   ssr     Fix to stop recovery process on card removal 
04/27/15   vr      Support to check the pending proactive command on card
02/12/15   sam     Clearing internal_command_used flag appropriately
12/12/14   ks      UIM to vote for sleep during AP mode OFF and card removed
11/17/14   nmb     Remove dead directory maintenance code
11/17/14   ssr     Fixed to ensure that UIM is not overwriting the ICC\UICC
                   protocol in Reset request.
11/12/14   js      Fixed compiler warning
11/10/14   akv     Support for user to customize LDO settings
10/31/14   ks      Reset protocol and Voltage for RESET_F command
10/30/14   ks      Fix issue with BTSAP compilation
10/13/14   akv     Using 3.8Mhz clk while toggling INST class during timeouts
10/01/14   ll      KW errors
09/18/14   akv     UIMDRV UT enhancements
08/27/14   sam     Adding support to trigger recovery on reciept of bad status word
08/14/14   ks      Hotswap enhancement for flaky mechanical switches
08/14/14   sam     Support for extended recovery
07/25/14   sam     retry powerup when card timeouts on RESET_F command when
                   Hotswap disabled
07/18/14   ssr     Fix to clear the busy and transaction timer after getting
                   response for 0x88 INS of stream APDU request when NULL
                   simulation feature flag is disabled
07/15/14   ll      NV refresh upon hotswap feature
07/08/14   ssr     Fixed to send Card Removed notification when card is not
                   present on Reset request
06/23/14   ssr     Removed tmc.h header, no longer required  due to RCINIT framework
06/11/14   ll      Add interruptible time delay and return value for update params
05/22/14   sam     Correcting Fix to notify card error when timeout occurs
                   on RESET_F command in the case of Hotswap off scenario.
05/08/14   ks      Initialising the max attempts value for every command
04/23/14   lxu     Need to send response for the command that was de-queued from the
                   queue and was currently in progress before doing iso fallback.
04/16/14   lxu     Add usb init timer for waiting connection available event
04/14/14   lxu     Fix hot-swap failed in slot2 when enable USB UICC for all slots
04/14/14   lxu     Set me_powerup to TRUE before fallback to ISO mode
04/08/14   ssr     Support of CLA change on interface error
03/31/14   ll      Change stop at infinite NULL flag name for readability
03/31/14   lxu     Fix USB UICC recovery, hotswap and iso card issue
03/27/14   lxu     USB UICC modem implement
03/27/14   ll      SIM BUSY log reduction and remove dog rpt print
03/21/14   ll      Remove return_ptr and remove/change uim_free to UIM_FREE
03/18/14   sam     Fix to notify CARD_ERROR if timeout occurs on RESET_F command
03/07/14   ks      Command re-queue count not to be cleared off during
                   time out on INTERNAL FETCH
02/25/14   na      Support of select next api for non-prov session
02/07/14   akv     Recover within recovery for timeouts after ATR and PPS
01/29/14   akv     RX ISR refactoring changes
01/14/14   ak      Created for uim task re-factoring/cleanup

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rex.h"
#include "uimdrv_msg.h"
#include "uim_p.h"
#include "uimglobals.h"
#include "uim_p.h"
#include "uimi.h"
#include "uimdrv_clk.h"
#include "uimdrv_main.h"
#include "uimdrv_uartdm.h"
#include "dog_hb_rex.h"
#include "uim_hotswap.h"
#include "uimsub_manager.h"
#include "uim_logging.h"

#ifdef FEATURE_UIM_BTSAP_CLIENT
#include "uim_remote.h"
#endif /* FEATURE_UIM_BTSAP_CLIENT */

/* Function prototype for the table based signal handlers */
typedef boolean (*uim_sig_handler_type)(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);

/* This struct is used to create the table which maps signals to their handler functions */
typedef struct
{
  rex_sigs_type sigMask;
  uim_sig_handler_type handler;
}uim_sig_map_type;

extern void uim_efslog_process_purge_sig(uim_instance_global_type *uim_ptr);
/* Forward declarations */
static void uim_checkq_and_turn_on_clk(uim_instance_global_type *uim_ptr);
static boolean uim_handle_dog_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_task_stop_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_poll_timer_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_card_removed_suspicious_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_card_removed_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_card_inserted_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
#ifdef FEATURE_UIM_USB_UICC
static boolean uim_handle_usb_fallback_to_iso_sig(rex_sigs_type *sig_mask_ptr,rex_sigs_type *imask_ptr,uim_instance_global_type *uim_ptr);
static boolean uim_handle_usb_remote_wakeup_sig(rex_sigs_type *sig_mask_ptr,rex_sigs_type *imask_ptr,uim_instance_global_type *uim_ptr);
#endif /* FEATURE_UIM_USB_UICC */
static boolean uim_handle_state_toggle_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_cmd_q_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_cmd_rsp_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_efslog_purge_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_cmd_rsp_timeout_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_simulate_null_timer_exp_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_busy_ind_timer_exp_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_trans_timer_exp_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_mcgf_nv_refresh_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_handle_extended_recovery_timer_exp_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
static boolean uim_is_reset_cmd_not_allowed(uim_instance_global_type *uim_ptr);
static boolean uim_can_ignore_timeout(rex_sigs_type sigs, uim_instance_global_type *uim_ptr);
static boolean uim_is_streaming_cmd_allowed(uim_instance_global_type *uim_ptr);
static boolean uim_handle_fetch_pending_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
#ifdef FEATURE_UIM_BTSAP_CLIENT
static boolean uim_handle_remote_connection_unavailable_sig(rex_sigs_type *sig_mask_ptr, rex_sigs_type *imask_ptr, uim_instance_global_type *uim_ptr);
#endif /* FEATURE_UIM_BTSAP_CLIENT */

/*  This table associates each UIM signal to its appropriate handler function */
static uim_sig_map_type uim_sig_map[] =
{
  {UIM_DOG_HB_RPT_SIG, uim_handle_dog_sig},
  {UIM_TASK_STOP_SIG, uim_handle_task_stop_sig},
  {UIM_SUSPICIOUS_CARD_REM_SIG,uim_handle_card_removed_suspicious_sig},
  {UIM_POLL_TIMER_SIG, uim_handle_poll_timer_sig},
  {UIM_HOTSWAP_CMD_CARD_REM_SIG, uim_handle_card_removed_sig},
  {UIM_HOTSWAP_CMD_CARD_INS_SIG, uim_handle_card_inserted_sig},
#ifdef FEATURE_UIM_USB_UICC
  {UIM_USB_FALLBACK_TO_ISO_SIG, uim_handle_usb_fallback_to_iso_sig},
  {UIM_USB_REMOTE_WAKEUP_SIG, uim_handle_usb_remote_wakeup_sig},
#endif /* FEATURE_UIM_USB_UICC */
  {UIM_STATE_TOGGLE_SIG, uim_handle_state_toggle_sig},
  {UIM_FETCH_PENDING_SIG, uim_handle_fetch_pending_sig},
#ifdef FEATURE_UIM_BTSAP_CLIENT
  {UIM_REMOTE_CONNECTION_UNAVAILABLE_SIG, uim_handle_remote_connection_unavailable_sig},
#endif /* FEATURE_UIM_BTSAP_CLIENT */
  {UIM_CMD_Q_SIG, uim_handle_cmd_q_sig},
  {UIM_CMD_RSP_SIG, uim_handle_cmd_rsp_sig},
  {UIM_EFSLOG_PURGE_SIG, uim_handle_efslog_purge_sig},
  {UIM_CMD_RSP_TIMEOUT_SIG, uim_handle_cmd_rsp_timeout_sig},
  {UIM_TRANSACTION_SIG, uim_handle_cmd_rsp_timeout_sig},
  {UIM_SIMULATE_NULL_TIMER_EXP_SIG, uim_handle_simulate_null_timer_exp_sig},
  {UIM_BUSY_IND_TIMER_EXP_SIG, uim_handle_busy_ind_timer_exp_sig},
  {UIM_TRANS_TIMER_EXP_SIG, uim_handle_trans_timer_exp_sig},
  {UIM_MCGF_NV_REFRESH_SIG, uim_handle_mcgf_nv_refresh_sig},
  {UIM_EXT_RECOVERY_TIMER_EXP_SIG, uim_handle_extended_recovery_timer_exp_sig},
};


/**
*  This function handles all the signals that are set for the UIM task by
*  calling the associated handler function for each signal that
*  is set.
*
*  SIDE EFFECTS
*  Can potentially change the value of the imask_ptr(set of
*  signals to that UIM task is interested in) depending on logic
*  in the handlers.
*
*  @param sig_mask_ptr Pointer to the OR'd set of signals to
*                      handle.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   The mask may be changed by this function.
*  @param uim_ptr Pointer to the current instance of the UIM
*                 global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*
*/
boolean uim_signal_handler(rex_sigs_type *sig_mask_ptr,
                           rex_sigs_type *imask_ptr,
                           uim_instance_global_type *uim_ptr)
{
  int i = 0;
  boolean b_goto_top = FALSE;

  if(uim_ptr->id >= UIM_INSTANCE_INVALID)
  {
    UIM_MSG_ERR_1("Invalid input param instance id 0x%x", uim_ptr->id);
    return FALSE;
  }
  /* Turn on clock if it is stopped and there is a cmd in the Q */
  uim_checkq_and_turn_on_clk(uim_ptr);

  for (i=0;i<ARR_SIZE(uim_sig_map);i++)
  {
    if (*sig_mask_ptr & uim_sig_map[i].sigMask)
    {
      /* Dog report sig print msg is not useful, skip printing to reduece log */
      if ( UIM_DOG_HB_RPT_SIG != uim_sig_map[i].sigMask )
      {
        UIMDRV_MSG_LOW_1(uim_ptr->id, "uim_sigs: calling handler number %d", i);
      }
      /* The handler may change the value of *sig_mask_ptr */
      b_goto_top = uim_sig_map[i].handler(sig_mask_ptr, imask_ptr, uim_ptr);

      if (TRUE == b_goto_top)
      {
        /* Goto the top of the main loop to execute rex_wait() */
        return TRUE;
      }
    }
  }
  return FALSE;
} /* uim_signal_handler */


/**
*  This function handles the UIM_DOG_HB_RPT_SIG signal.
*
*  @param sig_mask_ptr  Pointer to the OR's set of signals that UIM task needs tohandle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_dog_sig(rex_sigs_type *sig_mask_ptr,
                                  rex_sigs_type *imask_ptr,
                                  uim_instance_global_type *uim_ptr)
{
  /* DOG HeartBeat
  1. A single shared timer resource among all clients is used to register for dog.
  2. Dog will send the signals to the registered tasks (“ping”) instead of going through timer,
     through dog’s own heartbeat, and the tasks will respond to wdog (“pong”)
     Voluntary dog reporting is supported (pongs without pings).
  3. Hence we use dog_hb_report() to report to the dog and in turn pet it.
  */
  (void)sig_mask_ptr;
  (void)imask_ptr;
  (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_DOG_HB_RPT_SIG );
  dog_hb_report(uim_ptr->task.dog_rpt_var);
  return FALSE;
}/* uim_handle_dog_sig */


/**
*  This function handles the UIM_TASK_STOP_SIG signal.
*
*
*  @param sig_mask_ptr  Pointer to the OR's set of signals that UIM task needs tohandle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_task_stop_sig(rex_sigs_type *sig_mask_ptr,
                                        rex_sigs_type *imask_ptr,
                                        uim_instance_global_type *uim_ptr)
{
  UIMDRV_MSG_MED_0(uim_ptr->id, "UIM_TASK_STOP_SIG received" );
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TASK_STOP_SIG );
  (void)sig_mask_ptr;
  (void)imask_ptr;

  /* Make sure to power down the interface when not in use. */
  uim_ptr->flag.powering_down_task = TRUE;
  (void)uim_power_control_ds((uim_slot_type)(uim_ptr->id + 1),
                             (uim_voter_type)((int)UIM_AUTH | (int)UIM_MC |
                             (int)UIM_PROACTIVE_UIM | (int)UIM_UI |
                             (int)UIM_UICC_SESSION | (int)UIM_SESSION |
                              UIM_TRAF_CHAN),
                              FALSE,UIM_CALL_TYPE_NOT_APPLICABLE);

  /* change the status to ERROR so that we will send back any commands that
   * might come our way after the task_stop signal
   */
  uim_ptr->state.status = UIM_ERR_S;

  /* RESET all the timers */
  rex_clr_timer( &uim_ptr->command.cmd_rsp_timer);
  rex_clr_timer(&uim_ptr->atr.atr_40k_timer);
  rex_clr_timer(&uim_ptr->poll.wake_up_poll_timer);
  rex_clr_timer(&uim_ptr->poll.no_wake_up_poll_timer);

  if((TRUE == uim_ptr->extended_recovery.extended_recovery_enabled) &&
     (TRUE == uim_ptr->extended_recovery.extended_recovery_in_progress))
  {
    /*task stop signal has come. abandon the extended recovery and clear the relavent globals*/
    uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_TERMINATE);
  }

   /* UIM_STATE_TOGGLE_SIG will take care of powering down the
      UIM interface and flushing all queues */
  (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_STATE_TOGGLE_SIG);

  return FALSE;

} /* uim_handle_task_stop_sig */


/**
*  This function handles the UIM_POLL_TIMER_SIG signal.
*
*
*  @param sig_mask_ptr  Pointer to the OR's set of signals that UIM task needs to handle.
*                       Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_poll_timer_sig(rex_sigs_type *sig_mask_ptr,
                                         rex_sigs_type *imask_ptr,
                                         uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  
  /* clear the timer since for dual timer solution, one more timer
   * might still be running.  And for single timer solution, this
   * would not affect any functionality.
   */
  /* clear the timer since for dual timer solution, one more timer
   * might still be running.  And for single timer solution, this
   * would not affect any functionality.
   */
  uim_clear_poll_timer(uim_ptr);

  /* clear the signal */
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_POLL_TIMER_SIG);

  /* Set the internal flag as poll pending only if GSTK has
   * not disabled Polling
   */
  uim_ptr->flag.poll_pending = TRUE;

  return FALSE;
} /* uim_handle_poll_timer_sig */

/**
*  This function handles the UIM_SUSPICIOUS_CARD_REM_SIG signal.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
*                      handle.  Can be updated in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   The mask may be changed by this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_card_removed_suspicious_sig(rex_sigs_type *sig_mask_ptr,
                                            rex_sigs_type *imask_ptr,
                                            uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_SUSPICIOUS_CARD_REM_SIG);

  /* If already the card removed suspicious flag is TRUE, then simply return */
  if(uim_ptr->hotswap.hotswap_card_removed_suspicious == FALSE)
  {
    /* If already in error state or passive mode, we can not send POLL command
       So treat this as genuine card removal */
    if((uim_ptr->state.status != UIM_ERR_S)&&(uim_ptr->command.mode != UIM_PASSIVE_MODE))
    {
      uim_ptr->hotswap.hotswap_card_removed_suspicious = TRUE;
      /* Set the POLL pending flag to confirm the card status
         when there is no command in progress  */
      if (!uim_ptr->flag.command_in_progress)
      {
        uim_ptr->flag.poll_pending = TRUE;
      }
    }
    else
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Sending card rem sig as UIM is already in state UIM_ERR_S " );
      (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
      (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
      uim_ptr->hotswap.card_status         = UIM_CARD_REMOVED;
      uim_ptr->hotswap.hotswap_card_status = UIM_CARD_REMOVED;
      uim_hotswap_send_card_removed_cmd(uim_ptr);
      /* Unmask the card insert interrupt */
      uim_hotswap_unmask_interrupt(uim_ptr);
    }
  }
  return FALSE;
}

/**
*  This function handles the UIM_HOTSWAP_CMD_CARD_REM_SIG signal.
*
*  SIDE EFFECTS
*    This function may change the value of *imask_ptr or *sig_mask_ptr.
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
*                      handle.  Can be updated in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   The mask may be changed by this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_card_removed_sig(rex_sigs_type *sig_mask_ptr,
                                           rex_sigs_type *imask_ptr,
                                           uim_instance_global_type *uim_ptr)
{
  (void)imask_ptr;
  (void)sig_mask_ptr;
  
  if((TRUE == uim_hw_if.intctrl[uim_ptr->id].m_hs_enable)
#ifdef FEATURE_UIM_BTSAP_CLIENT
       || ( IS_BTSAP_HANDLE_ACTIVE(uim_ptr) )
#endif
     )
  {
    (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_HOTSWAP_CMD_CARD_REM_SIG);
    if (q_cnt(& uim_ptr->hotswap.hotswap_rem_cmd_q) != 0)
    {
      if (TRUE == uim_hotswap_process_card_removed_sig(uim_ptr))
      {
        uim_process_command(imask_ptr, uim_ptr);
        *imask_ptr &= ~UIM_CMD_Q_SIG;
        *sig_mask_ptr  &= ~UIM_CMD_Q_SIG;
        /* Update the UIM command pointer to current Hot Swap command,
           so that old cmd_ptr is not re-used*/
        uim_ptr->command.cmd_ptr = &uim_ptr->command.static_cmd_buf;
        (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_CMD_Q_SIG );
      }
    }
  }

  return FALSE;
} /* uim_handle_card_removed_sig */


/**
*  This function handles the UIM_HOTSWAP_CMD_CARD_INS_SIG signal.
*
*  SIDE EFFECTS
*    This function may change the value of *imask_ptr or *sig_mask_ptr.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
*                      handle.  Can be updated in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   The mask may be changed by this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_card_inserted_sig(rex_sigs_type *sig_mask_ptr,
                                            rex_sigs_type *imask_ptr,
                                            uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  
  /* Card removed signal has a higher priority, so do nothing if */
  /* the removed signal is set */
  if (0 == (*sig_mask_ptr & UIM_HOTSWAP_CMD_CARD_REM_SIG))
  {
    if((TRUE == uim_hw_if.intctrl[uim_ptr->id].m_hs_enable)
#ifdef FEATURE_UIM_BTSAP_CLIENT
       || ( IS_BTSAP_HANDLE_ACTIVE(uim_ptr) )
#endif
       )
    {
      (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_HOTSWAP_CMD_CARD_INS_SIG );
      if (q_cnt(& uim_ptr->hotswap.hotswap_ins_cmd_q) != 0)
      {
        /* command pointer need to end the last pending request if needed */
        if (TRUE == uim_hotswap_process_card_inserted_sig(uim_ptr))
        {
          uim_process_command(imask_ptr, uim_ptr);
          *imask_ptr &= ~UIM_CMD_Q_SIG;
          *sig_mask_ptr  &= ~UIM_CMD_Q_SIG;
          /* Update the UIM command pointer to current Hot Swap command,
             so that old cmd_ptr is not re-used  */
          uim_ptr->command.cmd_ptr = &uim_ptr->command.static_cmd_buf;
          (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_CMD_Q_SIG );
        }
      }
    }/* if(TRUE == uim_hw_if.intctrl[uim_ptr->id].m_hs_enable) */
  }

  return FALSE;
} /* uim_handle_card_inserted_sig */

#ifdef FEATURE_UIM_USB_UICC
/**
*  This function handles the UIM_USB_FALLBACK_TO_ISO_SIG signal.
*
*  SIDE EFFECTS
*    This function may change the value of *imask_ptr.
*
*  @param sig_mask_ptr  Pointer to the OR's set of signals that UIM task needs to
*                       handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   The mask may be changed by this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_usb_fallback_to_iso_sig(rex_sigs_type *sig_mask_ptr,
                                                  rex_sigs_type *imask_ptr,
                                                  uim_instance_global_type *uim_ptr)
{
  if (uim_is_usb_enabled(uim_ptr))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Falling back to legacy ISO from IC USB interface");

    /* Prepare IC USB interface and set static variables for ISO fallback */
    (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_USB_FALLBACK_TO_ISO_SIG);

    /* Following checks are needed to send error report for current command in progress
     1. Command Pointer is not null
     2. Command is currently in progress
     3. Slot mentioned in the command is same as the one for which we fallback to ISO mode
     4. External commands. In the case of internal SELECT command it will be handle by flushing.
     5. check uim_command_requeued for similiar purpose as 4*/

    /* Reporting error of current command, if any */
    if ( (uim_ptr->command.cmd_ptr != NULL) &&  (uim_ptr->flag.command_in_progress == TRUE) &&
         (!uim_ptr->flag.static_buffer_used) && (uim_ptr->flag.command_requeued == FALSE))
    {
      UIMDRV_MSG_HIGH_3(uim_ptr->id,"Reporting err for current cmd: 0x%x slot: 0x%x status: 0x%x",
                        uim_ptr->command.cmd_ptr->hdr.command,
                        uim_ptr->command.cmd_ptr->hdr.slot,
                        uim_ptr->command.cmd_ptr->hdr.status);

      if (((int)uim_ptr->command.cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0)
      {
        /* Set the status words to default before sending the rpt */
        uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
        uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;
        uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
      }
    }

    /* Flush all the commands in the command queue */
    uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

    /* Reset the counters */
    uim_ptr->command.static_cmd_buf.hdr.cmd_count = 1;
    uim_ptr->debug.convention_change_count = 1;
    uim_ptr->card_cmd.instrn_toggle_count = 1;

    uim_ptr->command.static_cmd_buf.hdr.slot =
        (uim_slot_type)(uim_ptr->id + 1);

    /* ME power up flag set to true because we want to power up
       fresh if we fallback as part of previous failed recovery in USB mode */
    uim_ptr->flag.me_powerup = TRUE;

    /* The power up mode will be set after parsing the historical
       characters from the ATR */
    uim_reset_uim(imask_ptr, uim_ptr->flag.me_powerup, uim_ptr);
  }

  return FALSE;
} /* uim_handle_usb_fallback_to_iso_sig */

/**
*  This function handles the UIM_USB_REMOTE_WAKEUP_SIG signal.
*
*  SIDE EFFECTS
*    This function may change the value of *imask_ptr.
*
*  @param sig_mask_ptr  Pointer to the OR's set of signals that UIM task needs to
*                       handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   The mask may be changed by this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_usb_remote_wakeup_sig(rex_sigs_type *sig_mask_ptr,
                                                rex_sigs_type *imask_ptr,
                                                uim_instance_global_type *uim_ptr)
{
  if (uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"Received remote wake up event from USB UICC");

    /* clear the timer since for dual timer solution, one more timer
     * might still be running.  And for single timer solution, this
     * would not affect any functionality.
     */
    uim_clear_poll_timer(uim_ptr);

    /* clear the signal */
    (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_POLL_TIMER_SIG);
    (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_USB_REMOTE_WAKEUP_SIG);

    uim_ptr->flag.poll_pending = TRUE;

  }

  return FALSE;
} /* uim_handle_usb_remote_wakeup_sig */
#endif /* FEATURE_UIM_USB_UICC */

/**
*  This function checks the UIM command queue and turns the UIM clock on
*  if there is a command in the queue.
*
*
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*/
static void uim_checkq_and_turn_on_clk(uim_instance_global_type *uim_ptr)
{
  if (uim_ptr->state.status == UIM_CLK_OFF_S)
  {
    /* we have a command in the q */
    if (q_cnt(& uim_ptr->task.cmd_q) != 0)
    {
      uim_stop_power_save(uim_ptr);
      /* Clock started */
      UIMDRV_MSG_MED_0(uim_ptr->id,"Clock started");

      /* Set the state back to Initialized state */
      uim_ptr->state.status = UIM_INITIALIZED_S;
    } /* end if - Check if clock should be turned back on */
  } /* end if - Check if clock is stopped */
} /* uim_checkq_and_turn_on_clk */


/**
*  This function handles the UIM_STATE_TOGGLE_SIG signal.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
*                      handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.  Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_state_toggle_sig(rex_sigs_type *sig_mask_ptr,
                                           rex_sigs_type *imask_ptr,
                                           uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  
  /* clear the signal */
  (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_STATE_TOGGLE_SIG);
  return FALSE;
} /* uim_handle_state_toggle_sig */


/**
*  This function handles the UIM_CMD_Q_SIG signal.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
*                      handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.  The value may be updated in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_cmd_q_sig(rex_sigs_type *sig_mask_ptr,
                                    rex_sigs_type *imask_ptr,
                                    uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_Q_SIG );
  if( q_cnt(&uim_ptr->task.cmd_q) == 0)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"q_cnt is 0 before q_get");
  }
  if ((uim_ptr->command.cmd_ptr = (uim_cmd_type *)q_get( &uim_ptr->task.cmd_q )) != NULL)
  {
    /* Defensive code to turn on clocks */
    if(uim_ptr->state.status == UIM_CLK_OFF_S)
    {
      UIMDRV_MSG_ERR_0(uim_ptr->id,"CLOCK OFF when processing command");
      uim_stop_power_save(uim_ptr);
      /* Set the state back to Initialized state */
      uim_ptr->state.status = UIM_INITIALIZED_S;
    }
    /* Command has not been attempted yet */
    uim_ptr->command.rpt_buf.cmd_transacted = FALSE;
    /* If a command is queued when UIM is in an erroneous state allow the
       command to be processed only if it is a reset, power down or
       passive power up command type */
    if ( ( (uim_ptr->state.status == UIM_ERR_S) &&
           (uim_ptr->command.cmd_ptr->hdr.command  != UIM_POWER_UP_UIM_PASSIVE_F) &&
           (uim_ptr->command.cmd_ptr->hdr.command  != UIM_POWER_DOWN_F) &&
           (uim_ptr->command.cmd_ptr->hdr.command  != UIM_RESET_F) ) ||
           (! UIM_IS_CMD_VALID_IN_MODE(uim_ptr->command.cmd_ptr, uim_ptr)) ||
           (uim_is_reset_cmd_not_allowed(uim_ptr)) ||
           /* check whether logical channel is opened or not */
           (FALSE == uim_is_streaming_cmd_allowed(uim_ptr))
       )
    {
      if (uim_ptr->state.status == UIM_ERR_S)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "SIM in power down state");
      }
      else if(FALSE == uim_is_streaming_cmd_allowed(uim_ptr))
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id, "STREAMING COMMAND is not allowed for channel 0x%x",
                          uim_ptr->command.cmd_ptr->hdr.channel);
        /* Set the status words to logical channel not supported */
        uim_ptr->command.rpt_buf.sw1 = SW1_CLA_NOT_SUPPORTED;
        uim_ptr->command.rpt_buf.sw2 = SW2_LOGICAL_CHAN_NOT_SUPPORTED;
      }
      else
      {
        UIM_ERROR_MSG_0("Invalid cmd UIM in passive mode ret error");
      }
      uim_ptr->command.cmd_ptr->hdr.status = UIM_ERR_S;

      if (uim_ptr->command.cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
      {
        (void) rex_set_sigs( uim_ptr->command.cmd_ptr->hdr.cmd_hdr.task_ptr,
                             uim_ptr->command.cmd_ptr->hdr.cmd_hdr.sigs );
      }

      /* Get the mode of the command */
      uim_ptr->state.cmd_mode = (uim_command_mode_type)
                     UIM_GET_CMD_MODE((int)uim_ptr->command.cmd_ptr->hdr.command);

      /* If always reporting & have a rpt function, report failure
         uim_send_err_rpt internally releases the buffer */
      if (((int)uim_ptr->command.cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0)
      {
        /* Set the status words to default before sending the rpt */
        uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
        uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;
        uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
      }
      else
      {  /* return buffer to done queue */
        if ( uim_ptr->command.cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
        {
          /* place command on requested queue */
          q_put( uim_ptr->command.cmd_ptr->hdr.cmd_hdr.done_q_ptr,
                 &uim_ptr->command.cmd_ptr->hdr.cmd_hdr.link );
        }
      }
    }
    else
    {
      if  ( (uim_ptr->state.status == UIM_ERR_S) &&
           ((uim_ptr->command.cmd_ptr->hdr.command  == UIM_POWER_UP_UIM_PASSIVE_F) ||
           (uim_ptr->command.cmd_ptr->hdr.command  == UIM_POWER_DOWN_F) ||
           (uim_ptr->command.cmd_ptr->hdr.command  == UIM_RESET_F)))
      {
        if((TRUE == uim_ptr->extended_recovery.extended_recovery_enabled) && (TRUE == uim_ptr->extended_recovery.extended_recovery_in_progress))
        {
          /*abandon the extended recovery and clear the relavent globals*/
          uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_TERMINATE);
        }
      }
      /* We do not want to look into the command queue until we are
      done processing this command */

      (*imask_ptr) &= ~UIM_CMD_Q_SIG;
      uim_ptr->flag.static_buffer_used = FALSE;

      if (uim_ptr->command.cmd_ptr->hdr.cmd_count == 0)
      {
        /* Set Count to indicate that this is the first attempt.. once
         * set to 1, it would not be reset to 0
         */
        uim_ptr->command.cmd_ptr->hdr.cmd_count = 1;
        /* Intialising here. In uim_process_generic_cmd this takes proper value */
        uim_ptr->setting.max_num_cmd_attempts = UIM_MAX_NUM_CMD_ATTEMPTS(uim_ptr);
      }
      /* Re-intatialize internal command used since we have popped a
       * fresh command out of the command queue.  If an internal
       * command has to be used, it would be set again
       */
      uim_ptr->flag.internal_command_used = FALSE;
      uim_ptr->card_cmd.work_waiting_time =
            uim_ptr->card_cmd.original_work_waiting_time;

      uim_ptr->card_cmd.work_waiting_time_etus =
            uim_ptr->card_cmd.original_work_waiting_time_etus;

      if ( (uim_ptr->command.cmd_ptr->hdr.cmd_count > uim_ptr->setting.max_num_cmd_attempts) ||
           (uim_ptr->command.cmd_ptr->hdr.cmd_recovery_count > uim_ptr->setting.max_num_cmd_attempts))
      {
        /* include the command Q sig to the mask as we are going
           to process the command again */
        (*imask_ptr) |= UIM_CMD_Q_SIG;

        /* Get the mode of the command */
        uim_ptr->state.cmd_mode = (uim_command_mode_type)
                         UIM_GET_CMD_MODE((int)uim_ptr->command.cmd_ptr->hdr.command);
        /* reset the static buffer used flag */
        uim_ptr->flag.static_buffer_used = FALSE;

        /* reset the internal command used flag */
        uim_ptr->flag.internal_command_used = FALSE;

        /* This command has already got enough chances but did not go
         * through so send an appropriate error report
         */
        uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
      }
      else
      {
        uim_process_command(imask_ptr, uim_ptr);
      }
    } /* uim status is error */
  }  /* (uim_ptr->command.cmd_ptr != NULL) */

  return FALSE;
} /* uim_handle_cmd_q_sig */


/**
*  This function handles the UIM_CMD_RSP_SIG signal.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
*                      handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.  The value may be updated in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_cmd_rsp_sig(rex_sigs_type *sig_mask_ptr,
                                      rex_sigs_type *imask_ptr,
                                      uim_instance_global_type *uim_ptr)
{
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG );

  if ((uim_ptr->hotswap.hotswap_flaky_switch_support)
      && (TRUE == uim_ptr->hotswap.hotswap_card_removed_suspicious)
      && (*uim_ptr->command.generic_state_ptr != UIM_POWER_UP_ST))
  {
    /* It got confirmed that card is inserted as we received response signal */
    uim_ptr->hotswap.card_status = UIM_CARD_INSERTED;
    uim_ptr->hotswap.hotswap_card_status = UIM_CARD_INSERTED;
    /* Unmasking card remove interrupt,
       Clearing the suscipious flag happens in uim_hotswap_unmask_interrupt()*/
    uim_hotswap_unmask_interrupt(uim_ptr);
  }

  /* Defensive check, in case simulate timer set the CMD_RSP_SIG without having a cmd_ptr */
  if( uim_ptr->command.cmd_ptr != NULL )
  {
    /* Check for simulate NULL enable*/
    if( (uim_ptr->command.cmd_ptr->hdr.command == UIM_STREAM_ISO7816_APDU_F) &&
        (uim_ptr->command.cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_INSTRN_OFFSET] ==
         uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_simulate_null_ins) &&
        (uim_ptr->debug.uim_feature_busy_response_simulate_null.uim_disable_simulate_null == FALSE)
      )
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Simulating null busy and not clearing timers");
    }
    else
    {
      /* UIM BUSY RESPONSE FEATURE: Call mmgsdi API to clear uim busy */
      uim_busy_ind_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG);
      uim_trans_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_CMD_RSP_SIG);
    }
  }
  /* Check to see if we have already flushed out all the commands and
     are not in a state to process commands.
     If a command response is available when UIM is in an erroneous state
     allow the response to be processed only if it is a power down command */
  if (((uim_ptr->state.status == UIM_ERR_S)  &&
       ((uim_ptr->command.cmd_ptr != NULL) && (uim_ptr->command.cmd_ptr->hdr.command  != UIM_POWER_DOWN_F))) ||
      (!uim_ptr->flag.static_buffer_used && uim_ptr->command.cmd_ptr == NULL) )
  {
    UIMDRV_MSG_ERR_3(uim_ptr->id,
                      "CMD_RSP Sig Rcvd uim_status=0x%x, uim_st_bf_us=0x%x, cmd_ptr=0x%x",
                      uim_ptr->state.status,
                      uim_ptr->flag.static_buffer_used,
                      uim_ptr->command.cmd_ptr);
    /* we have received the response */
    uim_ptr->flag.command_in_progress = FALSE;
    if (uim_ptr->flag.powering_down_task)
    {
      /* If always reporting & have a rpt function, report failure
          uim_send_err_rpt internally releases the buffer */
      if ( (uim_ptr->command.cmd_ptr != NULL) && 
         (((int)uim_ptr->command.cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0) )
      {
        uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
      }
      /* Need not set the cmd_q_sig in i_mask as task stop is called */
      (void) rex_set_sigs(uim_ptr->tcb_ptr, UIM_STATE_TOGGLE_SIG);
      return TRUE;
    }
    /* Go back to the beginning of the main task loop */
    (*imask_ptr) |= UIM_CMD_Q_SIG;
    return TRUE;
  }

#ifdef FEATURE_UIM_REMOTE_UIM
#ifdef FEATURE_UIM_BTSAP_CLIENT
  if ((uim_ptr->command.rsp_buf.cmd_status == UIM_REMOTE_STATUS) &&
      IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
  {
    /* uim receives only sw1 sw2 from QMI remote client
       This func gets the command status for the status words*/
    uim_remote_get_command_status(uim_ptr);
    if(uim_ptr->command.rsp_buf.cmd_status == UIM_REMOTE_GET_RESPONSE_STATUS)
    {
      /*Based on intermediate status word, resend apdu */
      if(uim_remote_get_entire_response(uim_ptr) )
      {
        /* uim drv didnot receive entire response*/
        /* Go back to the beginning of the main task loop */
        return TRUE;
      }
    }
  }
#endif /* FEATURE_UIM_BTSAP_CLIENT */

#ifdef FEATURE_UIM_USB_UICC
  if ((uim_ptr->command.rsp_buf.cmd_status == UIM_REMOTE_STATUS) &&
      uim_usb_is_usb_interface_selected(uim_ptr))
  {
    /* uim receives only sw1 sw2 from QMI remote client
       This func gets the command status for the status words*/
    uim_remote_get_command_status(uim_ptr);
    if(uim_ptr->command.rsp_buf.cmd_status == UIM_REMOTE_GET_RESPONSE_STATUS)
    {
      /*Based on intermediate status word, resend apdu */
      if(uim_remote_get_entire_response(uim_ptr) )
      {
        /* uim drv didnot receive entire response*/
        /* Go back to the beginning of the main task loop */
        return TRUE;
      }
    }
  }
#endif /* FEATURE_UIM_USB_UICC */
#endif /* FEATURE_UIM_REMOTE_UIM */

  #if defined( FEATURE_UIM_T_1_SUPPORT )
  if (UIM_T_1_PROTOCOL == uim_ptr->state.current_protocol)
  {
    /* Process the card response for the T=1 protocol */
    uim_process_t_1_block( &uim_ptr->command.rsp_buf, imask_ptr, uim_ptr);
  }
  else
  #endif /* FEATURE_UIM_T_1_SUPPORT */
  {
    uim_process_card_response(imask_ptr, uim_ptr);

    if( uim_ptr->setting.pdown_on_tech_prob.to_pdown &&
        uim_ptr->card_cmd.inc_tech_prob_count >= uim_ptr->setting.pdown_on_tech_prob.cnt_tech_problem &&
        uim_ptr->state.status == UIM_ERR_S )
    {
      uim_ptr->card_cmd.inc_tech_prob_count = 0;

      /* Power down the interface */
      uim_power_down(uim_ptr);

      /* Notify the higher layers that UIM is declared faulty */
      uim_notify_error(UIM_TECHNICAL_PROBLEM_S, uim_ptr);

      /* Go back to the beginning of the main task loop */
      return TRUE;
    }
  } /* end if - the protocol is T=1 */

  /* Check to see if we have a timeout signal and clear it since
   * we already got the response and processed it
   * unless the signal is explicitly set.
   */
  if ( uim_ptr->flag.clear_timeout_on_response )
  {
    (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
    (*sig_mask_ptr) &= ~UIM_CMD_RSP_TIMEOUT_SIG;
  }
  else
  {
    /* reset the flag for next use */
    uim_ptr->flag.clear_timeout_on_response = TRUE;
  }

  return FALSE;
} /* uim_handle_cmd_rsp_sig */


/**
*   This function handles the UIM_EFSLOG_PURGE_SIG signal.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
                       handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
                    in handling.  Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_efslog_purge_sig(rex_sigs_type *sig_mask_ptr,
                                        rex_sigs_type *imask_ptr,
                                        uim_instance_global_type *uim_ptr)
{
  /* Suppress the compiler warnings */
  (void)imask_ptr;
  (void)sig_mask_ptr;
  /* Turn on clock if it is stopped and there is a cmd in the Q */
  uim_checkq_and_turn_on_clk(uim_ptr);
  (void)uim_efslog_process_purge_sig(uim_ptr);

  return FALSE;
} /* handle_efslog_purge_sig */


/**
*  This function handles the UIM_TRANSACTION_SIG signal.
*
*  SIDE EFFECTS
*    This function may change the value of *imask_ptr or *sig_mask_ptr.
*
*
*  @param sig_mask_ptr  Pointer to the OR's set of signals that UIM task needs to
*                       handle.  The value may be updated by this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.  Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_cmd_rsp_timeout_sig(rex_sigs_type *sig_mask_ptr,
                                              rex_sigs_type *imask_ptr,
                                              uim_instance_global_type *uim_ptr)
{
  rex_sigs_type orig_sig_mask = *sig_mask_ptr;
  uim_status_type notification_reason = UIM_ERR_S;

  /* Turn on clock if it is stopped and there is a cmd in the Q */
  uim_checkq_and_turn_on_clk(uim_ptr);

  /* clear UIM_TRANSACTION_SIG from the current signal to ensure
     handle_cmd_rsp_timeout_sig() does not get called again */
  (*sig_mask_ptr)  &= ~UIM_TRANSACTION_SIG;

#if defined FEATURE_UIM_REMOTE_UIM && defined FEATURE_UIM_BTSAP_CLIENT
    if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Received timeout signal when remote client is active");
      uim_ptr->remote.state = WAIT_FOR_NOTHING;
    }
#endif /* FEATURE_UIM_REMOTE_UIM && FEATURE_UIM_BTSAP_CLIENT */

#if defined FEATURE_UIM_REMOTE_UIM && defined FEATURE_UIM_USB_UICC
    if(uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Received timeout signal when remote client is active");
      uim_ptr->remote.state = WAIT_FOR_NOTHING;
    }
#endif /* FEATURE_UIM_REMOTE_UIM && FEATURE_UIM_USB_UICC */

  if( orig_sig_mask & UIM_CMD_RSP_SIG )
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"WARNING: Command response and Command response timeout happened at the same time");
    /* Clear the command response timer */
    (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );

    /* Go back to the top of the main loop */
    return TRUE;
  }

  /* Check hotswap is enable for current slot */
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
  {
    if  (uim_ptr->hotswap.hotswap_capability.hos_enabled == TRUE)
    {
      /* Time-out handling when we suspect the card removal */
      if ( (uim_ptr->hotswap.hotswap_flaky_switch_support)
            &&  (uim_ptr->hotswap.hotswap_card_removed_suspicious == TRUE))
      {
         /* Usually when there is timeout, we can confirm it to be
            a genuine card removal case, but give a chance to try with 3v
            in case ATR_PPS is not done (power-up case, but not during recovery) */
        if(  (!uim_ptr->atr.atr_pps_done
              && (uim_ptr->state.current_voltage_class < uim_ptr->state.max_possible_voltage_class)
              && (!uim_ptr->flag.recovery_triggered)))
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id, "Attempting power up at 3V");
          uim_ptr->state.current_voltage_class++;
          uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;
          uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup, uim_ptr);
        }
        else
        {
          /* Confirmed that there is no card.
             So send card rem sig and unmasking the card inserted interrupt */
          (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
          (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
          uim_ptr->hotswap.card_status = UIM_CARD_REMOVED;
          uim_ptr->hotswap.hotswap_card_status = UIM_CARD_REMOVED;
          uim_hotswap_send_card_removed_cmd(uim_ptr);
          uim_hotswap_unmask_interrupt(uim_ptr);
        }
        return TRUE;
      }
      if(uim_ptr->hotswap.card_status == UIM_CARD_REMOVED)
      {
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, (( (*sig_mask_ptr) & UIM_TRANSACTION_SIG) |
                            ((*sig_mask_ptr) & UIM_CMD_RSP_TIMEOUT_SIG)));
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "HOTSWAP: card_status removed during cmd_rsp_timeout");

        if ((!uim_ptr->flag.static_buffer_used) && uim_ptr->command.cmd_ptr != NULL)
        {
          UIMDRV_MSG_HIGH_3(uim_ptr->id,"Reporting err for current cmd: 0x%x slot: 0x%x Status: 0x%x",
                            uim_ptr->command.cmd_ptr->hdr.command,
                            uim_ptr->command.cmd_ptr->hdr.slot,
                            uim_ptr->command.cmd_ptr->hdr.status);

          if (uim_ptr->command.cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
          {
            (void) rex_set_sigs( uim_ptr->command.cmd_ptr->hdr.cmd_hdr.task_ptr,
            uim_ptr->command.cmd_ptr->hdr.cmd_hdr.sigs );
          }
          /* Get the mode of the command */
          uim_ptr->state.cmd_mode = (uim_command_mode_type)
                          UIM_GET_CMD_MODE((int)uim_ptr->command.cmd_ptr->hdr.command);

          if (((int)uim_ptr->command.cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0)
          {
            /* Set the status words to default before sending the rpt */
            uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
            uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;
            /* It indicate that Command is not successfully send to the card */
            uim_ptr->command.rpt_buf.cmd_transacted = FALSE;
            uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
          }
          /* return buffer to done queue */
          else if (uim_ptr->command.cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL)
          {
            /* place command on requested queue */
            q_put( uim_ptr->command.cmd_ptr->hdr.cmd_hdr.done_q_ptr, &uim_ptr->command.cmd_ptr->hdr.cmd_hdr.link);
          }
        }
        else
        {
          /* This command has been completed */
          uim_ptr->flag.command_in_progress    = FALSE;
          /* Reset the command re-queued flag */
          uim_ptr->flag.command_requeued        = FALSE;
          uim_ptr->flag.internal_command_used = FALSE;
        }
       /* The status of UIM is set to Removed state */
       uim_ptr->state.status  = UIM_ERR_S;
       (*imask_ptr) |= UIM_CMD_Q_SIG;

       /* Go back to the top of the main loop */
       return TRUE;
      }
      else
      {
        /*  Check the card status, if card is not removed then the debounce will end with no change
            in status and recovery logic will continue. */
        UIMDRV_MSG_HIGH_2(uim_ptr->id,"HOTSWAP: UIM received response time out signal  0x%x 0x%x",
                           orig_sig_mask, *imask_ptr);
        UIM_DETECT_CARD_STATUS(uim_ptr);
        uim_timed_sleep(
               UIM_HOTSWAP_MAXTIME_HW_INTERRUPT,
               uim_ptr,
               UIM_HOTSWAP_CMD_CARD_REM_SIG);

        if (UIM_CARD_REMOVED == uim_ptr->hotswap.card_status)
        {
          UIMDRV_MSG_HIGH_1(uim_ptr->id,
              "Recovery is not allowed, card is either not inserted  0x%x or not detected on this slot",
              uim_ptr->hotswap.card_status);
          if (uim_ptr->hotswap.hotswap_flaky_switch_support)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id," Setting suspicious card removed signal during power-up ");
            (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_SUSPICIOUS_CARD_REM_SIG);
          }
          (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
          (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );
          return TRUE;
        }
        UIMDRV_MSG_HIGH_0(uim_ptr->id, 
            "Delaying the recovery process by 3ms to wait for Card Removed interrupt ");
      }
    }
  }

  /* clear the signal as well just in case if it was set */
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );

  if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DISABLE_RECOVERY_UPON_INFINITE_NULL,
                               uim_ptr) == TRUE)
  {
    if((orig_sig_mask & UIM_TRANSACTION_SIG)!= 0)
    {
      /* Check UIM is in middle of processing of any command */
      if (TRUE == uim_ptr->flag.command_in_progress)
      {
        /*
          1. We will need to report at this point only for an
             external command that was not re-queued.
          2. If the external command was re-queued, then in
             uim_flush_commands(), we do a q_get() and then report
             for the re-queued command.
          This check for uim_command_requeued avoids double reporting.
         */
        if ((!uim_ptr->flag.static_buffer_used)
            &&(!uim_ptr->flag.command_requeued)
            && uim_ptr->command.cmd_ptr != NULL)
        {
          uim_ptr->command.static_cmd_buf.hdr.slot = uim_ptr->command.cmd_ptr->hdr.slot;
          /*reset the command transacted flag as response 
           is not succesfully received from card*/
          uim_ptr->command.rpt_buf.cmd_transacted = FALSE;

          /* Sending error report for the current command */
          uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
        }
      }
      /* Set the command in progress flag to FALSE */
      uim_ptr->flag.command_in_progress = FALSE;

      /* Toggle the poll pending param not to proceed any poll pending */
      uim_ptr->flag.poll_pending = FALSE;
      /* set the status to ERROR */
      uim_ptr->state.status = UIM_ERR_S;
      /* set the command count to MAX */
      uim_ptr->command.static_cmd_buf.hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
      /* Flush all pending command */
      uim_flush_command(UIM_FLUSH_ALL, uim_ptr);
      /* UIM power down */
      uim_power_down(uim_ptr);
      /* Notify the higher layers that UIM is declared faulty */
      uim_notify_error(UIM_NULL_BYTE_ERR_S, uim_ptr);
      /* Reset all the flags and include the imask */
      (*imask_ptr) |= UIM_CMD_Q_SIG;
      /* Reset the internal command used flag */
      uim_ptr->flag.internal_command_used = FALSE;
      /* Reset the static_buffer */
      uim_ptr->flag.static_buffer_used = FALSE;

      UIMDRV_MSG_ERR_0(uim_ptr->id, "Shut down UIM due to the infinite NULL responses");

      /* Go back to the top of the main loop */
      return TRUE;
    }
  }

  UIMDRV_MSG_ERR_0(uim_ptr->id, "Timed out on the command response");
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_TRANSACTION_SIG );

#if defined( FEATURE_UIM_T_1_SUPPORT )
  if (UIM_T_1_PROTOCOL == uim_ptr->state.current_protocol)
  {
    /* For T=1, the handling of CWT timeout does not lead to
       power-down of the SIM interface always. */
    uim_ptr->flag.clear_timeout_on_response = TRUE;
    if ( uim_can_ignore_timeout(orig_sig_mask, uim_ptr) )
    {
      /* This is a spurious timeout signal.  We have bytes in the FIFO
         Return from this point as though nothing happened */
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Ignoring the spurious timeout signal for T=1");

      /* Go back to the top of the main loop */
      return TRUE;
    }

    /* Process the timeout for the T=1 protocol */
    if (uim_process_t_1_timeout(uim_ptr))
    {
      /* The timeout has been processed.  No need to process it
         further here */
      /* Go back to the top of the main loop */
      return TRUE;
    } /* end if - the timeout has been processed by T=1 code */
  } /* end if - operating in the T=1 protocol */
#endif /* FEATURE_UIM_T_1_SUPPORT */

  if ( uim_ptr->flag.max_parity_error   ||
       uim_ptr->flag.max_rx_break_error ||
       uim_ptr->flag.max_overrun_error )
  {
    /* We need to reset the interface so clear the values
     * for these error conditions since they would no longer
     * be right and could interfere in the sub-sequent time-out
     */
    uim_ptr->flag.overrun_error          = FALSE;
    uim_ptr->flag.bad_status_words_error = FALSE;
  }
  /* If an overrun error is encountered or bad status words
   * are received
   */
  if ( uim_ptr->flag.overrun_error ||
      (TRUE == uim_ptr->flag.bad_status_words_error &&
       FALSE == uim_nv_is_feature_enabled(UIMDRV_FEATURE_RECOVERY_ON_BAD_STATUS_WORD, uim_ptr)))
  {
    if ( uim_ptr->flag.overrun_error )
    {
      UIMDRV_MSG_ERR_0(uim_ptr->id,"Processing UART Overrun Error in UIM task");
    }
    if ( uim_ptr->flag.bad_status_words_error )
    {
      UIMDRV_MSG_ERR_0(uim_ptr->id,"Processing reception of bad status words. not triggering recovery");
    }

    /* Process the status as a command error */
    uim_process_cmd_err(imask_ptr, uim_ptr);

    /* Set the flag to FALSE */
    uim_ptr->flag.overrun_error          = FALSE;
    uim_ptr->flag.bad_status_words_error = FALSE;
  }
  else
  {
    /* Reset and try to recover from these error cases */
    if ( uim_ptr->flag.max_parity_error   ||
         uim_ptr->flag.max_rx_break_error ||
         uim_ptr->flag.max_overrun_error ||
         uim_ptr->flag.bad_status_words_error)
    {
      if (uim_ptr->flag.max_parity_error)
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id,"Reset due to Parity error");
      }
      else if (uim_ptr->flag.max_rx_break_error)
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id,"Reset due to rx break errors");
      }
      else if (uim_ptr->flag.max_overrun_error)
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id,"Reset due to overrun errors");
      }
      else if( uim_ptr->flag.bad_status_words_error )
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id,"Reset due to bad status words");
      }

      /* Clear all the flags irrespective of which one
       * got us here
       */
      uim_ptr->flag.max_parity_error   = FALSE;
      uim_ptr->flag.max_rx_break_error = FALSE;
      uim_ptr->flag.max_overrun_error  = FALSE;
      uim_ptr->flag.bad_status_words_error = FALSE;
      uim_ptr->flag.interface_gone_bad = TRUE;
    }
    else if ( uim_can_ignore_timeout(orig_sig_mask, uim_ptr) )
    {
      /* This is a spurious timeout signal.  We have bytes in the FIFO
         Return from this point as though nothing happened */
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Ignoring the spurious timeout signal for T=0");
      /* Go back to the top of the main loop */
      return TRUE;
    }

    /* Any timeout while in passive mode would be reported to the client
     * both internal or external commands
     */
    if (uim_ptr->command.mode == UIM_PASSIVE_MODE)
    {
      /* Set the flag to false since the command is completed */
      uim_ptr->flag.command_in_progress = FALSE;

      /* Setting the status to ERROR prior to invoking power down.
         While shutting the LDO down we evaluate the uim status to
         cater to the user customized LDO settings if needed. */
      uim_ptr->state.status = UIM_ERR_S;

      uim_power_down(uim_ptr);

      /* Clear out any votes to keep UIM powered on */
      uim_ptr->state.ctl = UIM_NONE;

      uim_ptr->flag.powerdown = TRUE;

      /* internal command timed-out for max attempts */

      /*Send the Error Report*/
      if(!uim_ptr->flag.static_buffer_used && uim_ptr->command.cmd_ptr != NULL)
      {
        /*reset the command transacted flag as response 
        is not succesfully received from card*/
        uim_ptr->command.rpt_buf.cmd_transacted = FALSE;

        uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
      }

      /* flush all the commands in the command queue */
      uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

      /* Notify the higher layers that UIM is declared faulty */
      uim_notify_error(UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S, uim_ptr);

      /* Whenver we receive a command, send an error report as the
      SIM is to be powered down */
      (*imask_ptr) |= UIM_CMD_Q_SIG;

      /* reset the directory struct and power down to UIM */
      uim_reset_dir(uim_ptr);

      /* Reset the static buffer used flag */
      uim_ptr->flag.static_buffer_used = FALSE;

      /* Reset the static command count */
      uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;
    }
    else if ((uim_ptr->flag.static_buffer_used) && uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_FETCH_F)
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Timeout on an internal fetch");
      /* Set the flag to false since the command is completed */
      uim_ptr->flag.command_in_progress = FALSE;
      /* Reset the internal command used flag */
      uim_ptr->flag.internal_command_used = FALSE;
      /* Reset static buffer used*/
      uim_ptr->flag.static_buffer_used = FALSE;
      uim_ptr->flag.command_requeued = FALSE;
      uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup,uim_ptr);
    }
    /* If we are in the middle of processing an external command */
    else if (!uim_ptr->flag.static_buffer_used)
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "UIM timeout in external command");

      /* Save GPIO and clock state in globals */
      uim_save_gpio_info(uim_ptr, FALSE);

      if (uim_ptr->command.cmd_ptr != NULL)
      {
        /* This makes sure that we do not send the status again to
           the task that sent the command */
        uim_ptr->command.cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;

        /* Reset requeue counter to 1 as UIM is entering recovery */
        uim_ptr->command.command_requeue_counter = 1;

        /* When recovery is triggered during UIM_VERIFY_CHV_F command and it
           should not be retried as it drops the number of incorrect pin attempts
           if the pin is entered wrong.
           In general we should not retry for a VERIFY CHV command and hence we
           send an error report to MMGSDI and we prompt the user again.
           All the pin related commands such as CHANGE, ENABLE, VERIFY, UNBLOCK,
           DISABLE are not retried and an error report is sent to MMGSDI. */

        if ( uim_ptr->command.cmd_ptr->hdr.command != UIM_RESET_F &&
             uim_ptr->command.cmd_ptr->hdr.command != UIM_VERIFY_CHV_F  &&
             uim_ptr->command.cmd_ptr->hdr.command != UIM_CHANGE_CHV_F  &&
             uim_ptr->command.cmd_ptr->hdr.command != UIM_DISABLE_CHV_F &&
             uim_ptr->command.cmd_ptr->hdr.command != UIM_ENABLE_CHV_F  &&
             uim_ptr->command.cmd_ptr->hdr.command != UIM_AUTHENTICATE_F  &&
             uim_ptr->command.cmd_ptr->hdr.command != UIM_UNBLOCK_CHV_F &&
             /* Do not re-attempt this comamnd if it so end-up infinite loop */
             uim_ptr->command.cmd_ptr->hdr.command != UIM_SILENT_RECOVER_UICC_F)
        {
          uim_ptr->flag.command_requeued = TRUE;
          /* Put the command back in the head of the queue so that it can be
            processed again */
          if ( q_cnt( &uim_ptr->task.cmd_q ) == 0 )
          {
            q_put( &uim_ptr->task.cmd_q, &uim_ptr->command.cmd_ptr->hdr.cmd_hdr.link );
          }
          else
          {
            q_insert( & uim_ptr->task.cmd_q,
                      & uim_ptr->command.cmd_ptr->hdr.cmd_hdr.link,
                      & ((uim_cmd_type*)q_check( &uim_ptr->task.cmd_q ))->hdr.cmd_hdr.link);
          }

          /* Increment the number of times the command is executed
           * instead of flushing right away, it would be flushed when it is
           * taken out of the que for processing to avoid giving an error
           * response while in recovery mode
           */
          ++uim_ptr->command.cmd_ptr->hdr.cmd_recovery_count;
        }
        else
        {
          UIMDRV_MSG_HIGH_1(uim_ptr->id,"UIM Command Re-queue did not happen for cmd = 0x%x",
                            uim_ptr->command.cmd_ptr->hdr.command);
          if(uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"Processing response for the reset command");

            if (uim_ptr->state.max_possible_voltage_class == uim_ptr->state.current_voltage_class
#ifdef FEATURE_UIM_BTSAP_CLIENT
                ||(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
#endif /* FEATURE_UIM_BTSAP_CLIENT */
               )
            {
              /* Set UIM signal mask to start accepting commands*/
              (*imask_ptr) |= UIM_CMD_Q_SIG;
            }

            if (uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE &&
                uim_ptr->hotswap.card_status == UIM_CARD_INSERTED )
            {
             /* If RESET_F times out when hotswap is enabled and if card is inserted then
                  we need to try powering up on the next votage class by calling uim_reset_uim */
              UIMDRV_MSG_HIGH_0(uim_ptr->id,"Reset_F timeout-Hotswap-Enabled,Card-Inserted,Apply 3V");
              uim_process_generic_response(&uim_ptr->command.rsp_buf, uim_ptr);
              uim_ptr->flag.command_in_progress = FALSE;
              uim_ptr->state.status = UIM_UNINITIALIZED_S;
            }
            else
            {
              uim_process_generic_response(&uim_ptr->command.rsp_buf, uim_ptr);
              UIMDRV_MSG_HIGH_0(uim_ptr->id,"Reset_F timeout out when hotswap disabled. report to client and try a fresh powerup");
              uim_ptr->flag.command_in_progress = FALSE;
              uim_ptr->state.status = UIM_UNINITIALIZED_S;
              /* Go back to the top of the main loop */
            }
          }
          else if(uim_ptr->command.cmd_ptr->hdr.command == UIM_VERIFY_CHV_F  ||
                  uim_ptr->command.cmd_ptr->hdr.command == UIM_CHANGE_CHV_F  ||
                  uim_ptr->command.cmd_ptr->hdr.command == UIM_DISABLE_CHV_F ||
                  uim_ptr->command.cmd_ptr->hdr.command == UIM_ENABLE_CHV_F  ||
                  uim_ptr->command.cmd_ptr->hdr.command == UIM_AUTHENTICATE_F  ||
                  uim_ptr->command.cmd_ptr->hdr.command == UIM_UNBLOCK_CHV_F)
          {
            /* When recovery is triggered during UIM_VERIFY_CHV_F command and it
               should not be retried as it drops the number of incorrect pin attempts
               if the pin is entered wrong.
               In general we should not retry for a VERIFY CHV command and hence we
               send an error report to MMGSDI and we prompt the user again.
               All the pin related commands such as CHANGE, ENABLE, VERIFY, UNBLOCK,
               DISABLE are not retried and an error report is sent to MMGSDI. */
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM Recovery during AUTHENTICATION Command or VERIFY/CHANGE/DISABLE/ENABLE/UNBLOCK Pin command");

            /*reset the command transacted flag as response 
            is not succesfully received from card*/
            uim_ptr->command.rpt_buf.cmd_transacted = FALSE;

            uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
          }
          /* Added for SILENT RECOVERY */
          else if (UIM_SILENT_RECOVER_UICC_F == uim_ptr->command.cmd_ptr->hdr.command)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"UIM SILENT Recovery ");
            /*Copying command buffer to report buffer to have it freed */
            uim_ptr->command.rpt_buf.user_data = uim_ptr->command.cmd_ptr->hdr.user_data;
            uim_ptr->command.cmd_ptr->hdr.rpt_function(&uim_ptr->command.rpt_buf);
            /*Start Processing the next command */
            (*imask_ptr) |= UIM_CMD_Q_SIG;
          }
        }
      }  /* uim_ptr->command.cmd_ptr != NULL */
      else
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"Command Pointer is Null");
      }

      /* reset the card and we do not want to include the command q sig */
      UIMDRV_MSG_HIGH_2(uim_ptr->id,"Reset after timeout Rx-state 0x%x Tx-state 0x%x",
                        uim_ptr->rxtx_state_machine.rx_state,
                        uim_ptr->rxtx_state_machine.tx_state);

      uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup,uim_ptr);
    }  /* if it was an internal command */
    else
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "UIM timeout in internal command");

      /* Save GPIO and clock state in globals */
      uim_save_gpio_info(uim_ptr, TRUE);

#ifdef FEATURE_UIM_USB_UICC
      if(uim_is_usb_enabled(uim_ptr) && uim_usb_is_usb_interface_selected(uim_ptr))
      {
        UIMDRV_MSG_HIGH_1(uim_ptr->id, "USB UICC: Timeout handling for cmd 0x%x",
                     uim_ptr->command.static_cmd_buf.hdr.command);
        /* Check if it is recovery command */
        if (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_WAKE_UP_F)
        {
          /* Set the flag to false since the command is completed */
          uim_ptr->flag.command_in_progress = FALSE;

          /* set the status to ERROR */
          uim_ptr->state.status = UIM_ERR_S;

          /* Clear out any votes to keep UIM powered on */
          uim_ptr->state.ctl = UIM_NONE;
          uim_ptr->flag.powerdown = TRUE;

          /* flush all the commands in the command queue */
          uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

          /* Notify the higher layers that UIM is declared faulty */
          uim_notify_error(UIM_ERR_S, uim_ptr);

          /* Whenver we receive a command, send an error report as the
          SIM is to be powered down */
          (*imask_ptr) |= UIM_CMD_Q_SIG;

          uim_ptr->flag.me_powerup = FALSE;
          /* reset the directory struct and power down to UIM */
          uim_reset_dir(uim_ptr);

          /* Reset the static buffer used flag */
          uim_ptr->flag.static_buffer_used = FALSE;

          /* Reset the static command count */
          uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;
          /* Reset recovery triggered flag */
          uim_ptr->flag.recovery_triggered = FALSE;
          if(uim_ptr->command.mode == UIM_RECOVERY_MODE)
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"Switching to NORMAL_MODE");
            uim_ptr->command.mode = UIM_NORMAL_MODE;
          }
          /* Now attempt fallback to ISO mode */
          uim_usb_fallback_to_iso(uim_ptr);

          /* Go back to the top of the main loop */
          return TRUE;
        }
        else
        {
          /* This is internal select/poll/fetch etc. Trigger Recovery */
          uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup,uim_ptr);

          /* Go back to the top of the main loop */
          return TRUE;
        }
      }

      /* If we have received an ATR and are timing out for any of the
         commands in the power up sequence, check if USIM instruction
         class */
      else
#endif /* FEATURE_UIM_USB_UICC */
      {
        if (uim_ptr->atr.atr_pps_done)
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id, "ATR_PPS Done");

          if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_NO_SWITCH_INST_ON_WWT_EXPIRY,
                                       uim_ptr) == TRUE)
          {
            if (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_ME_PUP_F ||
                uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_WAKE_UP_F)
            {
              uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup,uim_ptr);
              uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                                  uim_ptr->setting.wakeup_poll_offset,
                                  uim_ptr);
            }
          }
          else if( TRUE == uim_ptr->flag.interface_gone_bad )
          {
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"Timeout happened on interface error"
                                           "interface_gone_bad ");
            uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup,uim_ptr);
            uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                                uim_ptr->setting.wakeup_poll_offset,
                                uim_ptr);
          }
          else /* ! UIMDRV_FEATURE_NO_SWITCH_INST_ON_WWT_EXPIRY */
          {
            if(((uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE
                 && (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_ME_PUP_F
                     || uim_ptr->command.static_cmd_buf.hdr.command == UIM_HOTSWAP_CARD_INS_F))
                || (uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE
                    && uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_ME_PUP_F))
               && (uim_ptr->command.static_cmd_buf.hdr.cmd_count == 1)
               && (uim_ptr->state.first_inst_class == NV_UIM_FIRST_INST_CLASS_UMTS_SIM)
  #if defined( FEATURE_UIM_T_1_SUPPORT )
                && (UIM_T_1_PROTOCOL != uim_ptr->state.current_protocol)
  #endif /* FEATURE_UIM_T_1_SUPPORT */
               )
            {
              /* Power down the interface to force a cold reset */
              uim_power_down(uim_ptr);
              if (!uim_ptr->flag.invalid_pps_received)
              {
                /* Change the instruction class to GSM. */
                uim_ptr->flag.toggle_instrn_class = TRUE;
                if(UIM_IS_CLK_FREQ_AT_4_8_MHZ(uim_ptr->id))
                {
                  UIMDRV_MSG_HIGH_0(uim_ptr->id, "Toggling INST CLA and dropping freq to 3.8MHz");
                  UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ(uim_ptr->id);
                  SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, UIM_CRCF_372_1, UIM_BRAF_1 );
                }
              }
            }
            else
            {
              uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup,uim_ptr);
              uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                                  uim_ptr->setting.wakeup_poll_offset,
                                  uim_ptr);
            }
          }/* !UIMDRV_FEATURE_NO_SWITCH_INST_ON_WWT_EXPIRY */
        } /*atr pps done */
       else if (!uim_ptr->atr.atr_received)/* No ATR received */
       {
          /* Check if there is a higher voltage class */
          if(uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_WAKE_UP_F)
          {
            uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup,uim_ptr);
            uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                                uim_ptr->setting.wakeup_poll_offset,
                                uim_ptr);
          }
          else
          {
            if (uim_ptr->state.max_possible_voltage_class == uim_ptr->state.current_voltage_class)
            {
              if(UIM_IS_CLK_FREQ_AT_4_8_MHZ(uim_ptr->id))
              {
                UIMDRV_MSG_HIGH_0(uim_ptr->id, "Trying 3.84MHz CLK freq as a safety measure for old cards");
                uim_power_down(uim_ptr);
                uim_hotswap_reset_slot(uim_ptr);
                UIM_SWITCH_CLK_FREQ_TO_3_8_MHZ(uim_ptr->id);
                SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, UIM_CRCF_372_1, UIM_BRAF_1 );
                uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup,uim_ptr);
                /* Go back to the top of the main loop */
                return TRUE;
              }
              else
              {
                /* There are no more voltages to try */
                /* Clk Freq is at 3.84MHz, the lowest possible freq */
                /* Stop interfacing with the card */
                uim_ptr->command.static_cmd_buf.hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
              }
            }
            else if (uim_ptr->state.current_voltage_class < uim_ptr->state.max_possible_voltage_class )
            {
              /* There are more voltages to try */
              /* Power down the interface */
              uim_power_down(uim_ptr);
              UIMDRV_MSG_HIGH_0(uim_ptr->id, "Attempting power up at 3V");
              uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY,
                              uim_ptr,
                              UIM_ALL_ZERO_SIG);
              /* Try the next voltage class */
              uim_ptr->state.current_voltage_class++;
              /* Reset the error count */
              uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;
            } /* end if - there are no more voltages to try */
          }  /* uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_WAKE_UP_F */
        } /* No ATR received */
        else if(uim_ptr->atr.atr_received &&
                !uim_ptr->atr.atr_pps_done)
        {
          if((uim_ptr->flag.interface_gone_bad == TRUE) ||
              (uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_WAKE_UP_F))
          {
            uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup, uim_ptr);
            UIMDRV_MSG_HIGH_0(uim_ptr->id,"Bad PPS in WAKE_UP_F -> Resetting + UIM POLL timer reset" );
            uim_set_poll_timer( uim_ptr->setting.poll_time_delay,
                                uim_ptr->setting.wakeup_poll_offset,
                                uim_ptr);
          }
        }
      }
      if (++uim_ptr->command.static_cmd_buf.hdr.cmd_count > UIM_MAX_NUM_ATTEMPTS)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"Entering UIM_MAX_NUM_ATTEMPTS" );
        (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_SIG);
        if (uim_ptr->atr.atr_pps_done)
        {
          /* internal command failed after the PPS procedure */
          notification_reason = UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S;
        }
        else if ( !uim_ptr->atr.atr_received )
        {
          /* We have not yet received the ATR */
          if ( uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_ME_PUP_F )
          {
            notification_reason = UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S;
          }
          else
          {
            notification_reason = UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S;
          }
        }
        else
        {
          /* Atr is received but PPS procedure is not done and we are done
           * with all the re-tries
           */
          notification_reason = UIM_PPS_TIMED_OUT_MAX_TIMES_S;
        }

        /* Set the flag to false since the command is completed */
        uim_ptr->flag.command_in_progress = FALSE;

      /* Setting the status to ERROR prior to invoking power down.
         While shutting the LDO down we evaluate the uim status to
         cater to the user customized LDO settings if needed. */
        uim_ptr->state.status = UIM_ERR_S;

        uim_power_down(uim_ptr);

        /* Clear out any votes to keep UIM powered on */
        uim_ptr->state.ctl = UIM_NONE;

        uim_ptr->flag.powerdown = TRUE;

        /* flush all the commands in the command queue */
        uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

        if((uim_ptr->extended_recovery.extended_recovery_enabled == TRUE &&
               uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_WAKE_UP_F)  ||
           (uim_ptr->extended_recovery.extended_recovery_enabled == TRUE &&
               uim_ptr->extended_recovery.extended_recovery_in_progress == TRUE &&
               uim_ptr->command.static_cmd_buf.hdr.command == UIM_INTERNAL_ME_PUP_F)   )
        {
           /*1. all recoveries exhausted kick start the extended recovery
             2. extended powerup retry failed, check if further attempts are left and start the timer
             3. Donot kick start for internal_me_powerup failure*/
          uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_TIMER_START);
        }

        /* Notify the higher layers that UIM is declared faulty */
        uim_notify_error(notification_reason, uim_ptr);

        /* Whenver we receive a command, send an error report as the
        SIM is to be powered down */
        (*imask_ptr) |= UIM_CMD_Q_SIG;

        uim_ptr->flag.me_powerup = FALSE;
        /* reset the directory struct and power down to UIM */
        uim_reset_dir(uim_ptr);

        /* Reset the static buffer used flag */
        uim_ptr->flag.static_buffer_used = FALSE;

        /* Reset the static command count */
        uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;
        /* Reset recovery triggered flag */
        uim_ptr->flag.recovery_triggered = FALSE;
        if(uim_ptr->command.mode == UIM_RECOVERY_MODE)
        {
          UIMDRV_MSG_HIGH_0(uim_ptr->id,"Switching to NORMAL_MODE");
          uim_ptr->command.mode = UIM_NORMAL_MODE;
        }
      }
      else
      {
        /* One slot only */
        if(!uim_ptr->flag.recovery_triggered)
        {
          /* Power down the interface to force a cold reset */
          uim_power_down(uim_ptr);
          uim_timed_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY,
                          uim_ptr,
                          UIM_ALL_ZERO_SIG);
          UIMDRV_MSG_HIGH_2(uim_ptr->id,"Cold Reset after timeout Rx-state 0x%x Tx-state 0x%x",
                            uim_ptr->rxtx_state_machine.rx_state,
                            uim_ptr->rxtx_state_machine.tx_state);
          uim_reset_uim (imask_ptr, uim_ptr->flag.me_powerup, uim_ptr);
        }
      }
    }  /* internal or external command */
  } /* uim_overrun_error */
  /* Resetting uim_interface_gone_bad to FALSE
     after processing cmd rsp timeout sig */
  uim_ptr->flag.interface_gone_bad = FALSE;

  return FALSE;
} /* uim_handle_cmd_rsp_timeout_sig */


/**
*  This function handles the UIM_SIMULATE_NULL_TIMER_EXP_SIG signal.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
                       handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
                    in handling.  Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_simulate_null_timer_exp_sig(rex_sigs_type *sig_mask_ptr,
                                                      rex_sigs_type *imask_ptr,
                                                      uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  
  /* Simulate receiving response after uim_simulate_null_timer_val_s sec of NULL bytes */
  uim_simulate_null_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_SIMULATE_NULL_TIMER_EXP_SIG);
  return FALSE;
} /* uim_handle_simulate_null_timer_exp_sig */


/**
*  This function handles the UIM_BUSY_IND_TIMER_EXP_SIG signal.
*
*  Process busy_ind_timer before trans_timer to prevent the
*  possibility of notify in twice of the uim_busy_ind_timer_val.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
*                      handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.  Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_busy_ind_timer_exp_sig(rex_sigs_type *sig_mask_ptr,
                                                 rex_sigs_type *imask_ptr,
                                                 uim_instance_global_type *uim_ptr)
{
  /* Notify uim busy to mmgsdi */
  uim_busy_ind_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_BUSY_IND_TIMER_EXP_SIG);
  return FALSE;
} /* uim_handle_busy_ind_timer_exp_sig */


/**
*  This function handles the UIM_TRANS_TIMER_EXP_SIG signal.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
*                      handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.  Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_trans_timer_exp_sig(rex_sigs_type *sig_mask_ptr,
                                              rex_sigs_type *imask_ptr,
                                              uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  
  /* Trigger force recovery */
  uim_trans_timer_state_handler(uim_ptr, UIM_BUSY_RESPONSE_EVT_TRANS_TIMER_EXP_SIG);
  return FALSE;
} /* uim_handle_trans_timer_exp_sig */


/**
*  This function handles the UIM_MCGF_NV_REFRESH_SIG signal.
*
*  Refresh the NV and EFS items that are carrier specific.
*
*
*  @param sig_mask_ptr Pointer to the OR's set of signals that UIM task needs to
*                      handle.  Unused in this function.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.  Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_mcgf_nv_refresh_sig(rex_sigs_type *sig_mask_ptr,
                                              rex_sigs_type *imask_ptr,
                                              uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  
  /* Clear the sig */
  (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_MCGF_NV_REFRESH_SIG );

  /* Invoke NV refresh procedures here */
  uimdrv_read_mcfg_refreshed_nv(uim_ptr);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* Call MCFG call back that the refresh is done */
  (void)mcfg_refresh_done_w_status(uim_ptr->state.nv_refresh_reference_id,
                                   MCFG_REFRESH_SUCCESS);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
  /* Do not need to return to the top immediately */
  return FALSE;
} /* uim_handle_mcgf_nv_refresh_sig */

static boolean uim_handle_extended_recovery_timer_exp_sig(rex_sigs_type *sig_mask_ptr,
                                              rex_sigs_type *imask_ptr,
                                              uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  
  (void) rex_clr_sigs( uim_ptr->tcb_ptr, UIM_EXT_RECOVERY_TIMER_EXP_SIG);
  /*extended recovery timer expierd. post a fresh powerup command*/
  UIMDRV_MSG_HIGH_0(uim_ptr->id, "uim_handle_enhanced_recovery_timer_exp_sig");
  uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_TIMER_EXP);
  uim_reset_uim(imask_ptr, uim_ptr->flag.me_powerup, uim_ptr);
  return FALSE;
} /* uim_handle_extended_recovery_timer_exp_sig */

/**
*  This procedure returns TRUE if Reset cmd is not allowed there
*  else it return false.
*  If the reset command is allowed, this function also does
*  some pre-processing like resetting the globals.
*
*
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*
*  @return boolean returns TRUE if Reset cmd is not allowed
*/
static boolean uim_is_reset_cmd_not_allowed(uim_instance_global_type *uim_ptr)
{
   /* If its switch command request then it will power down current active slot
     and update the new active slot */
  if(TRUE == uim_sm_update_active_slot_data(uim_ptr))
  {
    return TRUE;
  }

  if(uim_ptr->id >= UIM_MAX_INSTANCES)
  {
    UIM_MSG_ERR_1("uim_is_reset_cmd_not_allowed: Incorrect uim_instance 0x%x",
                   uim_ptr->id);
    return TRUE;
  }

  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
  {
    if(uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F &&
       uim_ptr->command.cmd_ptr->hdr.cmd_count == 0)
    {      
      if ( uim_ptr->hotswap.hotswap_first_ins_class == NV_UIM_FIRST_INST_CLASS_GSM_SIM ||
           uim_ptr->command.cmd_ptr->hdr.protocol   == UIM_ICC )
      {
        uim_ptr->command.cmd_ptr->hdr.protocol = UIM_ICC;
      }
      else
      {
        uim_ptr->command.cmd_ptr->hdr.protocol = UIM_UICC;
      }
    }
    return FALSE;
  }

  if (((uim_ptr->command.cmd_ptr->hdr.command == UIM_RESET_F) 
       ||(uim_ptr->command.cmd_ptr->hdr.command == UIM_POWER_UP_UIM_PASSIVE_F)) &&
      (uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE))
  {
    /* Enable the hot swap interrupt*/
    UIMDRV_MSG_HIGH_2(uim_ptr->id,"UIM received the RESET F command  for slot 0x%x with protocol",
                      uim_ptr->command.cmd_ptr->hdr.slot,
                      uim_ptr->command.cmd_ptr->hdr.protocol);
    if (uim_ptr->hotswap.hotswap_me_power_down == TRUE &&
        uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
    {
      /* UIM does not know the current card status as hotswap interrupt is masked at power down,
         it may be older card or may be a new card so UIM will always reset the protocol and voltage
         before processing the Reset command */
      UIMDRV_MSG_HIGH_0(uim_ptr->id,
                        "Reset command come after the power down, UIM does not know the current card status");
      uim_ptr->hotswap.hotswap_me_power_down = FALSE;
      uim_ptr->hotswap.card_status = uim_hotswap_query_card_status(uim_ptr);
      uim_ptr->hotswap.hotswap_card_status = uim_ptr->hotswap.card_status;
      uim_ptr->hotswap.hotswap_notify_card_removed = FALSE;
      if(uim_ptr->command.cmd_ptr->hdr.command != UIM_POWER_UP_UIM_PASSIVE_F)
      {
        uim_hotswap_reset_slot(uim_ptr);
        if ( uim_ptr->hotswap.hotswap_first_ins_class == NV_UIM_FIRST_INST_CLASS_GSM_SIM)
        {
          uim_ptr->command.cmd_ptr->hdr.protocol = UIM_ICC;
        }
        else
        {
          uim_ptr->command.cmd_ptr->hdr.protocol = UIM_UICC;
        }
      }
       /* Clear the interrupt */
       uim_hotswap_clear_interrupt(uim_ptr);
       uim_hotswap_unmask_interrupt(uim_ptr);
       UIMDRV_MSG_LOW_0(uim_ptr->id,"Successfully unmask hot swap interrupt for RESET_F command");
    }
    UIMDRV_MSG_HIGH_2(uim_ptr->id,"UIM current card status 0x%x  HS status 0x%x",
                      uim_ptr->hotswap.card_status,
                      uim_ptr->hotswap.hotswap_card_status);
    if (uim_ptr->hotswap.card_status == UIM_CARD_REMOVED)
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,"Card is already removed, UIM does not need to RESET the card");
      uim_power_down(uim_ptr);
      uim_notify_error(UIM_CARD_REMOVED_S, uim_ptr);
      uim_ptr->state.status = UIM_ERR_S;
      return TRUE;
    }
  }
  return FALSE;
}/* uim_is_reset_cmd_not_allowed */


/**
* This function check whether or not we can ignore the timeout signal by
* checking some variables and the uart status to see if an rx_isr would
* eventually fire or the possibility that the interrupt was already fired
* after the rex_wait has exited.
*
*
*  @param sigs The OR's set of signals that UIM task needs to
*              handle.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*
*  @return boolean TRUE if we can safely ignore the timeout signal
*/
static boolean uim_can_ignore_timeout(rex_sigs_type sigs,
                                      uim_instance_global_type *uim_ptr)
{
  boolean ignore_timeout = FALSE;

#ifdef FEATURE_UIM_BTSAP_CLIENT
  if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
  {
    uim_ptr->flag.clear_timeout_on_response = FALSE;
    return ignore_timeout;
  }
#endif

  if ( (UIM_T_1_PROTOCOL == uim_ptr->state.current_protocol) &&
       (sigs & UIM_TRANSACTION_SIG))
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"WARNING!!! TRANSACTION RESPONSE TIMED-OUT in T=1 protocol");
  }

  /* Start ignoring the time-outs only if we have an indication not to
   * ignore the timeout signal because of not receiving the start of
   * ATR on time.  Do not ignore the timeout even if there are
   * bytes in the FIFO if the transaction timer expired since there
   * could NULL bytes since a long time.
   */
  if ( uim_ptr->flag.clear_timeout_on_response && !( sigs & UIM_TRANSACTION_SIG ) )
  {
    /* Lock the interrupts to make sure that all the checks are done
     * atomically
     */
    if ( (uim_get_uart_status(uim_ptr) & MSMU_SR_RXRDY) ||
         TIMER_IS_ACTIVE(&uim_ptr->command.cmd_rsp_timer) ||
         (rex_get_sigs(uim_ptr->tcb_ptr) & UIM_CMD_RSP_SIG) )
    {
      /* The following conditions based on which we ignore the time out signal
       *
       * 1. There are bytes in the FIFO indicating that the rx_isr
       *    would eventually be fired setting a new timer or completing
       *    the transaction based on the contents pending in the FIFO
       *
       * 2. After the signals have been extracted, the task was pre-empted
       *    and rx_isr was scheduled which would clear out the reeive FIFO
       *    and re-set the timer.  In this case, there whould be some
       *    count pending in the timer.
       *
       * 3. After the signals have been extracted, the task was pre-empted
       *    and rx_isr was scheduled which ended the transaction.  In This
       *    case, though there is no timer set, the response signal should
       *    be pending.
       */

      ignore_timeout = TRUE;
    }
  }
  /* If we have an indication to not ignore the timeout signal,
   * prepare for the next iteration.
   */
  uim_ptr->flag.clear_timeout_on_response = TRUE;

  return ignore_timeout;
} /* uim_can_ignore_timeout */

/**
*
* This procedure checks whether streaming command for given logical
* channel is allowed or not.
* Case may be that recovery is kicked in at streaming APDU and
* during smart channel recovery, streaming session/channel is closed and
* the error for this command is sent to its client.
*
*
*   @param uim_ptr Pointer to the current instance of the UIM global structure
*
*  @return boolean TRUE if processing of command is allowed else FALSE

*/

static boolean uim_is_streaming_cmd_allowed
(
  uim_instance_global_type *uim_ptr
)
{
  if(uim_ptr->id >= UIM_MAX_INSTANCES)
  {
    UIM_ERROR_MSG_0("uim_is_streaming_cmd_allowed: Incorrect uim_instance");
    return (FALSE);
  }
  
  if ( uim_ptr->command.mode == UIM_PASSIVE_MODE )
  {
    /* Streaming APDU to be sent to card in passive mode */
    return TRUE;
  }

  /* Check for the validity of channel. If this channel is opened then process the command otherwise
     return the error.
     Use case is like this- All non-prov session is closed during smart channel recovery and
     command shall never be attempted if it's streaming APDU.
     SelectNext (Next Occurrence of partial AID) will always belong to streaming APDU.
  */
  if( (uim_ptr->command.cmd_ptr->hdr.channel > UIM_CHANNEL0) &&
      (FALSE == uim_ptr->state.is_ch_open[uim_ptr->command.cmd_ptr->hdr.channel]))
  {
    return(FALSE);
  }

  if(uim_ptr->command.cmd_ptr->hdr.command == UIM_MANAGE_CHANNEL_F &&
     uim_ptr->state.channel_info.manage_channel_allowed  == FALSE)
  {
    UIMDRV_MSG_ERR_0(uim_ptr->id,"open channel not supported");
    return(FALSE);
  }
  return(TRUE);

}/* uim_is_streaming_cmd_allowed */


/**
*  This function handles the UIM_FETCH_PENDING_SIG signal.
*
*  @param sig_mask_ptr  Pointer to the OR's set of signals that UIM task needs tohandle.
                        Unused in this function.
*  @param imask_ptr     Pointer to the mask of signals that the uim task is interested
*                       in handling.   Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_fetch_pending_sig(rex_sigs_type *sig_mask_ptr,
                                            rex_sigs_type *imask_ptr,
                                            uim_instance_global_type *uim_ptr)
{
  (void)sig_mask_ptr;
  (void)imask_ptr;
  
  uim_ptr->flag.fetch_pending  = TRUE;
  UIMDRV_MSG_LOW_0(uim_ptr->id, "Recevied Fetch pending request");
  (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_FETCH_PENDING_SIG );
  return FALSE;
}/* uim_handle_fetch_pending_sig */


#ifdef FEATURE_UIM_BTSAP_CLIENT
/**
*  This function handles the UIM_CONNECTION_UNAVAILABLE_SIG signal.
*
*  @param sig_mask_ptr  Pointer to the OR's set of signals that UIM task needs tohandle.
                        Unused in this function.
*  @param imask_ptr     Pointer to the mask of signals that the uim task is interested
*                       in handling.   Unused in this function.
*  @param uim_ptr Pointer to the current instance of the UIM global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
static boolean uim_handle_remote_connection_unavailable_sig(rex_sigs_type *sig_mask_ptr,
                                            rex_sigs_type *imask_ptr,
                                            uim_instance_global_type *uim_ptr)
{
  UIMDRV_MSG_HIGH_1(uim_ptr->id, "In uim_handle_remote_connection_unavailable_sig function with remote state 0x%x",uim_ptr->remote.state);
  (void) rex_clr_sigs(uim_ptr->tcb_ptr, UIM_REMOTE_CONNECTION_UNAVAILABLE_SIG );

  if(WAIT_FOR_RESPONSE == uim_ptr->remote.state)
  {
    (void)rex_clr_timer(&uim_ptr->remote.remote_cmd_rsp_timer);
    /* Set the command response timeout signal */
    (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_CMD_RSP_TIMEOUT_SIG );
  }
  
  return FALSE;
}/* uim_handle_remote_connection_unavailable_sig */
#endif /* FEATURE_UIM_BTSAP_CLIENT */