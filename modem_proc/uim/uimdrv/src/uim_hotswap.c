/*==============================================================================
  FILE:         uim_hotswap.c

  OVERVIEW:     FIle conatins the functions for UIM HOTSWAP feature.

  DEPENDENCIES: N/A

                Copyright (c) 2014-2015 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
==============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_hotswap.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       ---------------------------------------------------------
06/20/16    ks         Resetting the command transacted flag if the 
                       response is not received from card
06/17/16    gm         F3 message optimization
05/06/16    gm         Clear BTSAP cmd response timer upon remote card removal
12/23/15    sam        Fix for KW errors
07/17/15    ks         When flaky hotswap feature is enabled then mask 
                       hotswap interrupt while handling card_suspicious signal
06/24/15    ssr        Fix for KW errors
05/13/15    ks         Clearing total recovery count upon new card insertion
04/27/15    ssr        Fix to stop recovery process on card removal 
01/19/15    kr         Fixed Compilation warnings
01/08/15    xj         Add DISABLE_CARD_STATUS_CHECK_AT_POWER_UP in features NV
11/18/14    ks         BTSAP connection establishment even when local SIM 
                       is absent
10/15/14    ssr        Not unmasking the Hotswap interrupt when 
                       IgnoreHotSwapSwitchTlv Tag is used in QMI_UIM_POWER_UP req
09/18/14    akv        UIMDRV UT enhancements
09/05/14    sam        check for command_requeued flag while processing hotswap
08/26/14    ks         Clearing path position during card insertion
08/14/14    ks         Hotswap enhancement for flaky mechanical switches
08/14/14    sam        Support for extended recovery
08/04/14    ll         Fix compile warnings
07/16/14    lm         Fix for offtarget related to hotswap
06/16/14    ll         Switch to new string based TLMM APIs
06/11/14    ll         KW errors
04/25/14    sam        Fix for KW issue: possible out of bound array.
02/08/14    sam        Fix for Klocwork issues
01/31/14    ak         Safety checks for disabled UIM instances
01/25/14    yk         The first revision
==============================================================================*/
#include "queue.h"
#include "nv_items.h"
#include "timer.h"
#include "DDITlmm.h"
#include "uimdrv_msg.h"
#include "uim_p.h"
#include "uimi.h"
#include "uimglobals.h"
#include "uim_logging.h"
#include "uim_nvcfg.h"
#include "uimdrv.h"
#include "uimdrv_main.h"
#include "uimdrv_uartdm.h"
#include "uimdrv_gpio.h"
#include "uimdrv_clk.h"
#include "uim_hotswap.h"
#ifdef FEATURE_UIM_BTSAP_CLIENT
#include "uim_remote.h"
#endif /* FEATURE_UIM_BTSAP_CLIENT */

/**
 * DECLARATIONS OF INTERNAL FUNCTIONS
 */
static boolean uim_hotswap_send_card_inserted_cmd(
  uim_instance_global_type *uim_ptr);
static void uim_hotswap_define_debounce_timer(uim_instance_global_type *uim_ptr);
static void uim_hotswap_reset_debounce_timer(
  uim_card_swap_enum_type    hos_new_card_status,
  uim_instance_global_type  *uim_ptr);
static void uim_hotswap_start_debounce_timer(
  uim_card_swap_enum_type        hos_new_card_status,
  uim_instance_global_type      *uim_ptr);
static void uim_hotswap_clear_debounce_timer(uim_instance_global_type *uim_ptr);
static void uim_hotswap_debounce_timer_expiry_cb(unsigned long instance_ptr);
static uim_hotswap_slot_capability uim_get_hotswap_enabled_slot(
  uim_instance_global_type *uim_ptr);

/**
 * DEFINITIONS OF EXTERNAL FUNTIONS
 */

/**
 * This procedure initializes the  queues and timers for UIM
 * HOTSWAP. It should be called only once, at powerup time.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_init(uim_instance_global_type *uim_ptr)
{
  int i = 0;

  boolean hotswap_enabled = FALSE;
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  if(TRUE == m_HWConfig.uim_slot_config[uim_ptr->id].disableUim)
  {
    /* Slot is disabled */
    return;
  }


  /* Ini all hs queue */
  (void) q_init( &uim_ptr->hotswap.hotswap_free_q );
  (void) q_init( &uim_ptr->hotswap.hotswap_ins_cmd_q );
  (void) q_init( &uim_ptr->hotswap.hotswap_rem_cmd_q );

  /* Function uim_uartdm_uim_controller_config is called twice as the first time we set the
        bits in the UIM CFG register that enable Battery alarm trigger, uimremoved trigger, mode18,
        pmic alarm.
        If hotswap is enabled then we set the uim_card_events _enable bit which if cleared results in
        the uim controller not reacting to any card event (insertion/removal)
    */
  uim_uartdm_uim_controller_config(uim_ptr, hotswap_enabled);

  for (i = 0; i < UIM_HOTSWAP_NUM_CMD_BUFS; i++)
  {
    /* Init the buffer with SLOT AUTOMATIC */
    uim_ptr->hotswap.hotswap_cmd_pool[i].hdr.slot = UIM_SLOT_AUTOMATIC;
    uim_ptr->hotswap.hotswap_cmd_pool[i].hdr.cmd_hdr.done_q_ptr = &uim_ptr->hotswap.hotswap_free_q;
    q_put( &uim_ptr->hotswap.hotswap_free_q, q_link( &uim_ptr->hotswap.hotswap_cmd_pool[i],
           &uim_ptr->hotswap.hotswap_cmd_pool[i].hdr.cmd_hdr.link));
  }

  uim_ptr->hotswap.hotswap_first_ins_class = uim_ptr->state.first_inst_class;

  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
  {
    hotswap_enabled = TRUE;
    uim_ptr->hotswap.hotswap_capability   =  uim_get_hotswap_enabled_slot(uim_ptr);
    /* Configure hotswap gpios */
    uim_config_hotswap_gpios(uim_ptr);
    uim_uartdm_uim_controller_config(uim_ptr, hotswap_enabled);

	if(uim_nv_is_feature_enabled(UIMDRV_FEATURE_DISABLE_CARD_STATUS_CHECK_AT_POWER_UP,
								 uim_ptr) == TRUE)
	{
	  /* set card status as inserted */
      uim_ptr->hotswap.card_status = UIM_CARD_INSERTED;
	}
    else
    {
      for(i = 0; i < UIM_HOTSWAP_MAX_CHECK_FOR_CARD_INSERTION_ON_POWER_UP; i++)
      {
        uim_ptr->hotswap.card_status  = uim_hotswap_query_card_status(uim_ptr);
        if((UIM_CARD_INSERTED == uim_ptr->hotswap.card_status) ||
           (UIM_CARD_UNCHANGED == uim_ptr->hotswap.card_status))
        {
          break;
        }
        /* Waiting 200us after write of UART_DM_UIM_CFG does not ensure the status
           of the card will be updated immediately, but it ensures the command
           has reached the UIM controller internal logic and new command might
           be executed . After a configuration of ‘UIM_CARD_EVENTS_ENABLE’
           bit  UIM_controller will start counting for EVENT_DEBOUNCE_TIME and
           only then it will update the status to card present.
  
           That is why polling on UART_DM_UIM_IO_STATUS is required. A single read
           of IO_STATUS register after 100,200us will not ensure the status will
           be CARD_INSERTED.
  
           Here in worse case the delay would be 2 ms.
         */
        uim_clk_busy_wait(200);
      }
    }

    uim_ptr->hotswap.hotswap_card_status = uim_ptr->hotswap.card_status;
    uim_hotswap_define_debounce_timer(uim_ptr);
    uim_hotswap_unmask_interrupt(uim_ptr);
  }
} /* uim_hotswap_init */


/**
 * Set hotswap signal to UIM task.
 *
 * @param hos_new_card_status New HOTSWAP status.
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean  uim_hotswap_set_signal(
  uim_card_swap_enum_type   hos_new_card_status,
  uim_instance_global_type *uim_ptr)
{
  if( uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE
#ifdef FEATURE_UIM_BTSAP_CLIENT
      || IS_BTSAP_HANDLE_ACTIVE(uim_ptr)
#endif /* FEATURE_UIM_BTSAP_CLIENT */
    )
  {
    if (hos_new_card_status == UIM_CARD_INSERTED)
    { 
      uim_ptr->hotswap.card_status = hos_new_card_status;
      uim_ptr->hotswap.hotswap_card_status = hos_new_card_status;
      return uim_hotswap_send_card_inserted_cmd(uim_ptr);
    }
    else
    {
      if (uim_ptr->hotswap.hotswap_flaky_switch_support)
      {
        /* Set card removed suspicious signal instead of card rem signal */
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"Sending suspicious card removed signal");
        (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_SUSPICIOUS_CARD_REM_SIG);
        return TRUE;
      }
      else
      { 
        uim_ptr->hotswap.card_status = hos_new_card_status;
        uim_ptr->hotswap.hotswap_card_status = hos_new_card_status;
        return uim_hotswap_send_card_removed_cmd(uim_ptr);
      }
    }
  }
  else
  {
    return FALSE;
  }
} /* uim_hotswap_set_signal */


/**
 * Reset the UIM global to its intial value caused by HOTSWAP
 * and re-configure the UARTDM for HOTSWAP.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_reset_slot(uim_instance_global_type *uim_ptr)
{
  /* Reset the necessary UIM globals */
  uim_reset_globals(uim_ptr);

  /* Re-configure the UARTDM */
  uim_reconfig_uartdm(uim_ptr);
}/* uim_hotswap_reset_slot */


/**
 * Process card inserted signal in UIM task.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_hotswap_process_card_inserted_sig(
  uim_instance_global_type *uim_ptr)
{
  uim_cmd_type   *hos_cmd_ptr      = NULL; /* Pointer to received command */
  uim_slot_type   hos_curr_slot    = UIM_SLOT_NONE;

  /* Received card inserted request , take the requested command pointer */
  hos_cmd_ptr = (uim_cmd_type *)q_check(&uim_ptr->hotswap.hotswap_ins_cmd_q);
  if (hos_cmd_ptr == NULL)
  {
    return FALSE;
  }

  /* Check the slot */
  if ((hos_cmd_ptr->hdr.slot != UIM_SLOT_1) &&
      (hos_cmd_ptr->hdr.slot != UIM_SLOT_2) &&
      (hos_cmd_ptr->hdr.slot != UIM_SLOT_3))
  {
    hos_cmd_ptr = (uim_cmd_type *)q_get(&uim_ptr->hotswap.hotswap_ins_cmd_q);
    if (hos_cmd_ptr != NULL)
    {
      /* Free the UIM buffer for next request */
      hos_cmd_ptr ->hdr.slot = UIM_SLOT_AUTOMATIC;
      q_put( &uim_ptr->hotswap.hotswap_free_q, q_link(hos_cmd_ptr,&hos_cmd_ptr->hdr.cmd_hdr.link));
    }
    return FALSE;
  }

  /* Check the current card status before processing the command */
  if (uim_ptr->hotswap.card_status == UIM_CARD_REMOVED || uim_ptr->hotswap.hotswap_me_power_down == TRUE)
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"HOTSWAP: Drop Card inserted signal as card is already removed for slot 0x%x or me is in power down state",
                      hos_cmd_ptr->hdr.slot);
    /* Detect the card insert */
    hos_cmd_ptr = (uim_cmd_type *)q_get(&uim_ptr->hotswap.hotswap_ins_cmd_q);
    if (hos_cmd_ptr != NULL)
    {
      /* Free the UIM buffer for next request */
      hos_cmd_ptr ->hdr.slot = UIM_SLOT_AUTOMATIC;
      q_put( &uim_ptr->hotswap.hotswap_free_q, q_link(hos_cmd_ptr,&hos_cmd_ptr->hdr.cmd_hdr.link));
    }
    return FALSE;
  }

  /* check command is in progress */
  if (uim_ptr->flag.command_in_progress == TRUE ||  uim_ptr->flag.command_requeued == TRUE)
  {
  }

  /* Now UIM is good to process the card inserted , fetch the card inserted command */
  hos_cmd_ptr = (uim_cmd_type *)q_get(&uim_ptr->hotswap.hotswap_ins_cmd_q);
  if (hos_cmd_ptr == NULL)
  {
     return FALSE;
  }

  /* Read the slot form card inserted data */
  hos_curr_slot = hos_cmd_ptr ->hdr.slot;

  /* Free the UIM buffer for next request */
  hos_cmd_ptr ->hdr.slot = UIM_SLOT_AUTOMATIC;
  q_put( &uim_ptr->hotswap.hotswap_free_q, q_link(hos_cmd_ptr,&hos_cmd_ptr->hdr.cmd_hdr.link));

  if ((hos_curr_slot != UIM_SLOT_1) &&
      (hos_curr_slot != UIM_SLOT_2) &&
      (hos_curr_slot != UIM_SLOT_3))
  {
    return FALSE;
  }

  /* Check the Command pointer which is running and notify error for that command */
  if ((uim_ptr->command.cmd_ptr != NULL) && (uim_ptr->flag.command_in_progress == TRUE) && (uim_ptr->command.cmd_ptr->hdr.slot ==hos_curr_slot) &&
      (!uim_ptr->flag.static_buffer_used)&& (uim_ptr->flag.command_requeued == FALSE))
  {
    UIMDRV_MSG_HIGH_3(uim_ptr->id,"HOTSWAP: Reporting err for current cmd: 0x%x slot: 0x%x status: 0x%x",
                      uim_ptr->command.cmd_ptr->hdr.command,
                      uim_ptr->command.cmd_ptr->hdr.slot,
                      uim_ptr->command.cmd_ptr->hdr.status);

    if (((int)uim_ptr->command.cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0)
    {
      /*reset the command transacted flag as response 
      is not succesfully received from card*/
      uim_ptr->command.rpt_buf.cmd_transacted = FALSE;

      /* Set the status words to default before sending the rpt */
      uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
      uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;
      uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
    }
    /* return buffer to done queue */
    else if (uim_ptr->command.cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL)
    {
      /* place command on requested queue */
      q_put( uim_ptr->command.cmd_ptr->hdr.cmd_hdr.done_q_ptr, &uim_ptr->command.cmd_ptr->hdr.cmd_hdr.link);
    }
  }

  uim_hotswap_reset_slot(uim_ptr);

  if (uim_hw_if.clkreg[uim_ptr->id].m_simMaxFreqSupported == UIMDRV_CLK_FREQ_4_8_MHZ)
  {
    UIM_RESET_CLK_FREQ_TO_DEFAULT_4_8_MHZ(uim_ptr->id);
    SET_UIM_BAUD_RATE_SLOT ( uim_ptr->id, UIM_CRCF_372_1, UIM_BRAF_1 );
  }

  uim_ptr->flag.static_buffer_used             = TRUE;
  uim_ptr->command.static_cmd_buf.hdr.slot   = hos_curr_slot;

  /* command to power up the UIM due to task start up */
  /* Command has not been attempted yet */
  uim_ptr->command.rpt_buf.cmd_transacted                   = FALSE;
  uim_ptr->command.static_cmd_buf.hdr.command               = UIM_HOTSWAP_CARD_INS_F;

  if ( uim_ptr->hotswap.hotswap_first_ins_class == NV_UIM_FIRST_INST_CLASS_GSM_SIM)
  {
    uim_ptr->command.static_cmd_buf.hdr.protocol              = UIM_ICC;
  }
  else
  {
    uim_ptr->command.static_cmd_buf.hdr.protocol              = UIM_UICC;
  }
  uim_ptr->command.static_cmd_buf.hdr.channel               = UIM_CHANNEL0;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.task_ptr      = uim_ptr->tcb_ptr;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.done_q_ptr    = NULL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.sigs          = (rex_sigs_type)NULL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.link.next_ptr = NULL;
  uim_ptr->command.static_cmd_buf.hdr.rpt_function          = NULL;
  uim_ptr->command.static_cmd_buf.select.path_position      = 0;
  /* This is the first attempt for this command  */
  uim_ptr->command.static_cmd_buf.hdr.cmd_count             = 1;
  uim_ptr->hotswap.hotswap_notify_card_removed              = FALSE;
  uim_ptr->flag.internal_command_used                       = FALSE;
  uim_ptr->flag.command_in_progress                         = TRUE;

  if((TRUE == uim_ptr->extended_recovery.extended_recovery_enabled) && 
     (TRUE == uim_ptr->extended_recovery.extended_recovery_in_progress))
  {
    /*abandon the extended recovery and clear the relevent globals*/
    uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_TERMINATE);
  }

  // flush all pending commands in queue
  uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

  uim_ptr->debug.tot_recovery_count = 0;

#ifdef FEATURE_UIM_USB_UICC
  if(uim_is_usb_enabled(uim_ptr))
  {
    /* Reset usb related globals when card inserted */
    uim_usb_reset_globals(uim_ptr, FALSE);
#ifdef FEATURE_UIM_REMOTE_UIM
    (void) rex_clr_timer( &uim_ptr->remote.remote_cmd_rsp_timer);
#endif /* FEATURE_UIM_REMOTE_UIM */
    (void) rex_clr_timer( &uim_ptr->usb.usb_fallback_to_iso_timer);
  }
#endif /* FEATURE_UIM_USB_UICC */

  UIMDRV_MSG_HIGH_1(uim_ptr->id,"HOTSWAP:executing UIM_HOTSWAP_CARD_INS_F for slot 0x%x",
                    hos_curr_slot);

  /* Make command pointer point to static command buffer */
  uim_ptr->command.cmd_ptr = &uim_ptr->command.static_cmd_buf;

  return TRUE;
} /* uim_hotswap_process_card_inserted_sig */


/**
 * Process card removed signal in UIM task.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_hotswap_process_card_removed_sig(
  uim_instance_global_type  *uim_ptr)
{
  uim_slot_type   hos_curr_slot    = UIM_SLOT_NONE;
  uim_cmd_type   *hos_cmd_ptr      = NULL; /* Pointer to received command */
  /* Received card removed signal */
  hos_cmd_ptr = (uim_cmd_type *)q_check(&uim_ptr->hotswap.hotswap_rem_cmd_q);
  if (hos_cmd_ptr == NULL)
  {
    return FALSE;
  }

  /* Check the slot value */
  if ((hos_cmd_ptr->hdr.slot != UIM_SLOT_1) &&
      (hos_cmd_ptr->hdr.slot != UIM_SLOT_2) &&
      (hos_cmd_ptr->hdr.slot != UIM_SLOT_3))
  {
    hos_cmd_ptr = (uim_cmd_type *)q_get(&uim_ptr->hotswap.hotswap_rem_cmd_q);
    if (hos_cmd_ptr != NULL)
    {
      /* Free the UIM buffer for next request */
      hos_cmd_ptr ->hdr.slot = UIM_SLOT_AUTOMATIC;
      q_put( &uim_ptr->hotswap.hotswap_free_q,
             q_link(hos_cmd_ptr,&hos_cmd_ptr->hdr.cmd_hdr.link));
    }
    return FALSE;
  }

  /* Check the current card status before processing the command */
  if (uim_ptr->hotswap.card_status == UIM_CARD_INSERTED || uim_ptr->hotswap.hotswap_me_power_down == TRUE)
  {
    UIMDRV_MSG_HIGH_1(uim_ptr->id,"HOTSWAP: Drop Card removed signal as card is inserted for slot 0x%x or ME is in power down state",
                      hos_cmd_ptr->hdr.slot);
    hos_cmd_ptr = (uim_cmd_type *)q_get(&uim_ptr->hotswap.hotswap_rem_cmd_q);
    if (hos_cmd_ptr != NULL)
    {
      /* Free the UIM buffer for next request */
      hos_cmd_ptr ->hdr.slot = UIM_SLOT_AUTOMATIC;
      q_put( &uim_ptr->hotswap.hotswap_free_q, q_link(hos_cmd_ptr,&hos_cmd_ptr->hdr.cmd_hdr.link));
    }
    return FALSE;
  }

  /* Now UIM is good to process the card removed, get the card removed command */
  hos_cmd_ptr = (uim_cmd_type *)q_get(&uim_ptr->hotswap.hotswap_rem_cmd_q);
  if (hos_cmd_ptr == NULL)
  {
     return FALSE;
  }
  /* Get the slot information */
  hos_curr_slot = hos_cmd_ptr ->hdr.slot;

  /* Free the buffer for next request */
  hos_cmd_ptr ->hdr.slot = UIM_SLOT_AUTOMATIC;
  q_put( &uim_ptr->hotswap.hotswap_free_q, q_link(hos_cmd_ptr,&hos_cmd_ptr->hdr.cmd_hdr.link));

  /* check for valid slot */
  if ((hos_curr_slot != UIM_SLOT_1) &&
      (hos_curr_slot != UIM_SLOT_2) &&
      (hos_curr_slot != UIM_SLOT_3))
  {
    return FALSE;
  }

  /* Following checks are needed to send error report for current command in progress
     1. Command Pointer is not null
     2. Command is currently in progress
     3. Slot mentioned in the command is same as the one for which we received hotswap
        card removed indication
     4. External commands. In the case of internal SELECT command it will be handle by flushing.
     5. check uim_command_requeued for similiar purpose as 4*/

  /* Reporting error of current command, if any */
  if ( (uim_ptr->command.cmd_ptr != NULL) &&  (uim_ptr->flag.command_in_progress == TRUE) && ( uim_ptr->command.cmd_ptr->hdr.slot == hos_curr_slot )  &&
       (!uim_ptr->flag.static_buffer_used) && (uim_ptr->flag.command_requeued == FALSE))
  {
    UIMDRV_MSG_HIGH_3(uim_ptr->id,"HOTSWAP: Reporting err for current cmd: 0x%x slot: 0x%x status: 0x%x",
                      uim_ptr->command.cmd_ptr->hdr.command,
                      uim_ptr->command.cmd_ptr->hdr.slot,
                      uim_ptr->command.cmd_ptr->hdr.status);

    if (((int)uim_ptr->command.cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0)
    {
      /*reset the command transacted flag as response 
      is not succesfully received from card*/
      uim_ptr->command.rpt_buf.cmd_transacted = FALSE;

      /* Set the status words to default before sending the rpt */
      uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
      uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;
      uim_send_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);
    }
  }

  /* Flush immediately all pending command on that slot */
  uim_flush_command(UIM_FLUSH_ALL, uim_ptr);

  uim_hotswap_reset_slot(uim_ptr);
  uim_ptr->flag.static_buffer_used             = TRUE;
  uim_ptr->command.static_cmd_buf.hdr.slot   = hos_curr_slot;


  /* Command has not been attempted yet */
  uim_ptr->command.rpt_buf.cmd_transacted                   = FALSE;
  /* command to power up the UIM due to task start up */
  uim_ptr->command.static_cmd_buf.hdr.command               = UIM_HOTSWAP_CARD_REM_F;
  uim_ptr->command.static_cmd_buf.hdr.protocol              = UIM_UICC;
  uim_ptr->command.static_cmd_buf.hdr.channel               = UIM_CHANNEL0;
  uim_ptr->command.static_cmd_buf.pdown.pdown_option        = UIM_OPTION_NOTIFY_PDOWN;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.task_ptr      = uim_ptr->tcb_ptr;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.done_q_ptr    = NULL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.sigs          = (rex_sigs_type)NULL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.link.next_ptr = NULL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.done_q_ptr    = NULL;
  uim_ptr->command.static_cmd_buf.hdr.rpt_function          = NULL;
  /* The status of UIM is set to Removed state */
  uim_ptr->state.status                                        = UIM_ERR_S;
  /* !!!!! Do not clear the ATCSIM directory structure !!!!! */

  if((TRUE == uim_ptr->extended_recovery.extended_recovery_enabled) && 
     (TRUE == uim_ptr->extended_recovery.extended_recovery_in_progress))
  {
    /*abandon the extended recovery and clear the relevent globals*/
    uim_extended_recovery_event_handler(uim_ptr, UIM_EXTENDED_RECOVERY_EVT_TERMINATE);
  }

  uim_notify_error(UIM_CARD_REMOVED_S, uim_ptr);

  uim_ptr->flag.internal_command_used                         = FALSE;
  uim_ptr->flag.command_in_progress                           = TRUE;
  UIMDRV_MSG_HIGH_1(uim_ptr->id,"HOTSWAP: executing UIM_HOTSWAP_CARD_REM_F for slot 0x%x",
                    hos_curr_slot);

  /* Make command pointer point to static command buffer */
  uim_ptr->command.cmd_ptr = &uim_ptr->command.static_cmd_buf;

  return TRUE;
} /* uim_hotswap_process_card_removed_sig */


/**
 * HOTSWAP ISR for the PMIC\GPIO used to communicate with the
 * UIM.
 *
 * @param uim_ptr Pointer to the global data common.
 * @param hos_need_to_query Request the current status.
 * @param hos_new_card_status New HOTSWAP status.
 */
void uim_hotswap_isr(uim_instance_global_type *uim_ptr,
                     boolean                   hos_need_to_query,
                     uim_card_swap_enum_type   hos_new_card_status)
{
  if(NULL == uim_ptr)
  {
    return; 
  }

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
  {
    uim_card_swap_enum_type uim_query_card_status = UIM_CARD_UNCHANGED;

    /* Check if we are already in debounce logic then do nothing */
    if (uim_ptr->hotswap.hotswap_debounce_in_progress == TRUE)
    {
      UIMDRV_MSG_LOW_0(uim_ptr->id,"HOTSWAP: Debounce is in progress");
      return;
    }

    /* Mask hotswap isr */
    uim_hotswap_mask_interrupt(uim_ptr);
    /* Get the query status */
    if (hos_need_to_query)
    {
      uim_query_card_status = uim_hotswap_query_card_status(uim_ptr);
    }
    else
    {
      uim_query_card_status = hos_new_card_status;
    }
    if(uim_query_card_status == uim_ptr->hotswap.card_status)
    {
      if(UIM_CARD_INSERTED == uim_query_card_status)
      {
        uim_query_card_status = UIM_CARD_REMOVED;
      }
      else
      {
        uim_query_card_status = UIM_CARD_INSERTED;
      }
    }

    /* Clear the interrupt */
    uim_hotswap_clear_interrupt(uim_ptr);

    /* Check if there is any change in the card detection status */
    if(uim_query_card_status != UIM_CARD_UNCHANGED)
    {
      if (uim_query_card_status == UIM_CARD_INSERTED)
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id,"HOTSWAP: Start, For CARD INSERTED Interrupt ");
      }
      else
      {
        UIMDRV_MSG_HIGH_0(uim_ptr->id, "HOTSWAP: Start, For CARD REMOVED Interrupt");
      }
      uim_hotswap_start_debounce_timer(uim_query_card_status, uim_ptr);
    }
    else
    {
      /* The card detection status has not changed,
         enable card detect interrupt again */
      uim_hotswap_unmask_interrupt (uim_ptr);
    }
  }
} /* uim_hotswap_isr */


/**
 * HOTSWAP ISR for GPIO interrupt.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_gpio_isr(uim_instance_global_type *uim_ptr)
{
  /* The last parameter is ignored when the second parameter is TRUE  */
  uim_hotswap_isr(uim_ptr, TRUE, UIM_CARD_UNCHANGED);
} /* uim_hotswap_gpio_isr */


/**
 * Retrieves the real time card status of given slot.
 *
 * @param uim_slot Slot number.
 *
 * @return uim_rt_status_enum Real time HOTSWAP status.
 */
uim_rt_status_enum  uim_get_rt_status(uim_slot_type  uim_slot)
{
  /* Since this function gets called from QMI map uim slot
     to instance id */
  uim_instance_global_type *uim_ptr = NULL;
  uim_card_swap_enum_type   uim_rt_status;

  uim_ptr = uim_get_instance_ptr((uim_instance_enum_type) (uim_slot - 1));

  if(uim_ptr == NULL)
  {
    UIM_MSG_ERR_0("uim_get_rt_status: uim_ptr is NULL due to bad uim_slot");
    return UIM_RT_STATUS_NOT_SUPPORTED;
  }

  if(uim_ptr->id >= UIM_INSTANCE_INVALID)
  {
    UIM_MSG_ERR_1("uim_get_rt_status: Invalid uim instance id - 0x%x",
                   uim_ptr->id);
    return UIM_RT_STATUS_NOT_SUPPORTED;
  }

  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
  {
    uim_rt_status =  uim_hotswap_query_card_status (uim_ptr);
    switch (uim_rt_status)
    {
      case UIM_CARD_REMOVED:
        return UIM_RT_STATUS_CARD_ABSENT;
      case UIM_CARD_INSERTED:
        return UIM_RT_STATUS_CARD_PRESENT;
      default:
        UIM_MSG_ERR_1("Unknown status received for slot 0x%x", uim_slot);
    }
  }
  return UIM_RT_STATUS_NOT_SUPPORTED;
}/* uim_get_rt_status */


/**
 * Disable the hotswap interrupt for given slot
 *
 * @param uim_slot Slot number
 */
void uim_disable_hotswap_interrupt(uim_slot_type uim_slot)
{
  uim_instance_global_type *uim_ptr = NULL;

  uim_ptr = uim_get_instance_ptr((uim_instance_enum_type) (uim_slot - 1));

  if(uim_ptr == NULL)
  {
    UIM_MSG_ERR_0("uim_disable_hotswap_interrupt: uim_ptr is NULL due to bad uim_slot");
    return;
  }

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
  {
    uim_ptr->hotswap.hotswap_disabled_uicc_seq_off = TRUE;

    UIMDRV_MSG_HIGH_0(uim_ptr->id, "Disabled hotswap");
  }
}/* uim_disable_hotswap_interrupt */


/**
 * Query the real-time HOTSWAP status
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return uim_card_swap_enum_type
 */
uim_card_swap_enum_type uim_hotswap_query_card_status(
  uim_instance_global_type *uim_ptr)
{
  /* The default value of query card status is always set to UIM_CARD_INSERTED */
  uim_card_swap_enum_type card_status =  UIM_CARD_INSERTED;

  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE
     #ifdef FEATURE_UIM_BTSAP_CLIENT
      || IS_BTSAP_HANDLE_ACTIVE(uim_ptr)
     #endif /* FEATURE_UIM_BTSAP_CLIENT */
     )
  {
    return card_status;
  }

  /* uim_hos_disabled_uicc_seq_off flag is used to disable the hotswap
     capability at runtime, UIM will not allow to make a card status query,
     when this flag is TRUE and this function return as card inserted */
  if (uim_ptr->hotswap.hotswap_disabled_uicc_seq_off == FALSE)
  {
    card_status = uim_uartdm_uim_controller_card_presence(uim_ptr);
  }
  UIMDRV_MSG_LOW_1(uim_ptr->id,"HOTSWAP: uim_hotswap_query_card_status 0x%x",
                   card_status);
  return card_status;
} /* uim_hotswap_query_card_status */


/**
 * Mask the hotswap interrupt.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_mask_interrupt(uim_instance_global_type *uim_ptr)
{
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
  {
    return;
  }

  uim_hotswap_clear_debounce_timer(uim_ptr);
  UIMDRV_MSG_LOW_0(uim_ptr->id,"Masked UIM controller hotswap interrupt");
  uim_uartdm_uim_controller_mask_carddet_int(uim_ptr);
} /* uim_hotswap_mask_interrupt */


/**
 * Clear the hotswap interrupt.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_clear_interrupt(uim_instance_global_type *uim_ptr)
{
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
  {
    return;
  }
  uim_uartdm_uim_controller_clear_carddet_int(uim_ptr);
  UIMDRV_MSG_LOW_0(uim_ptr->id,"Cleared GPIO interrupt");
} /* uim_hotswap_clear_interrupt  */


/**
 * Unmask the hotswap interrupt.
 *
 * @param uim_ptr Pointer to the global data common.
 */
void uim_hotswap_unmask_interrupt(uim_instance_global_type *uim_ptr)
{
  uim_hotswap_slot_capability  uim_hos_capability     = {FALSE, FALSE, 0, FALSE, 0, 0};
  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);
  uim_hos_capability = uim_ptr->hotswap.hotswap_capability;
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE || 
     uim_ptr->hotswap.hotswap_disabled_uicc_seq_off == TRUE 
     #ifdef FEATURE_UIM_BTSAP_CLIENT
      || IS_BTSAP_HANDLE_ACTIVE(uim_ptr)
     #endif /* FEATURE_UIM_BTSAP_CLIENT */
     )
  {
    return;
  }

  /* When card_removed_suspicious flag is TRUE, check the card_status to un-mask the interrupt */
  if((uim_ptr->hotswap.hotswap_flaky_switch_support) 
      && (uim_ptr->hotswap.hotswap_card_removed_suspicious == TRUE))
  {

    uim_ptr->hotswap.hotswap_card_removed_suspicious = FALSE;
    if(UIM_CARD_REMOVED == uim_ptr->hotswap.card_status)  
    {
      uim_uartdm_uim_controller_card_ins_int(uim_ptr->id,
                                           uim_hos_capability.nGPIOInsertedLevel);
    }
    else if (UIM_CARD_INSERTED == uim_ptr->hotswap.card_status)
    {
      uim_uartdm_uim_controller_card_rem_int(uim_ptr->id,
                                           uim_hos_capability.nGPIOInsertedLevel);
    }
    return;
  }

  if (UIM_CARD_REMOVED == uim_hotswap_query_card_status(uim_ptr))
  {
    /* Do not unmask card inserted interrupt until it is confirmed that 
       the card is really removed */
    if((!uim_ptr->hotswap.hotswap_flaky_switch_support) 
        ||((UIM_CARD_REMOVED == uim_ptr->hotswap.card_status)) ) 
    {
      uim_uartdm_uim_controller_card_ins_int(uim_ptr->id,
                                             uim_hos_capability.nGPIOInsertedLevel); 
    }
  }
  else if (UIM_CARD_INSERTED == uim_hotswap_query_card_status(uim_ptr))
  {
    uim_uartdm_uim_controller_card_rem_int(uim_ptr->id,
                                           uim_hos_capability.nGPIOInsertedLevel);
  }
} /* uim_hotswap_unmask_interrupt */


/**
 * Query the hotswap capability.
 *
 * @param uim_slot Slot number.
 *
 * @return boolean True if slot have the hotswap capability
 */
boolean uim_is_hotswap_enabled_slot( uim_slot_type uim_slot )
{
  uim_instance_global_type *uim_ptr = uim_get_instance_ptr((uim_instance_enum_type)(uim_slot - 1));

  if(uim_ptr == NULL)
  {
    UIM_MSG_ERR_1("Invalid input param uim_slot 0x%x", uim_slot);
    return FALSE;
  }

  if((uim_ptr->id < UIM_MAX_INSTANCES)
      &&(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE))
  {
    uim_hotswap_slot_capability  uim_hos_capability = {FALSE, FALSE, 0, FALSE, 0, 0};

    uim_hos_capability = uim_get_hotswap_enabled_slot(uim_ptr);

    return uim_hos_capability.hos_enabled;
  }
  else
  {
    return FALSE;
  }
} /* uim_is_hotswap_enabled_slot */


/**
 * DEFINITIONS OF INTERNAL FUNTIONS
 *
 */
/**
 * Send the card removed cmd to uim task.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
boolean uim_hotswap_send_card_removed_cmd(
  uim_instance_global_type *uim_ptr)
{
  uim_cmd_type *hos_cmd_ptr = NULL;

  if(uim_ptr->id >= UIM_MAX_INSTANCES)
  {
    return FALSE;
  }

  if( uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE
#ifdef FEATURE_UIM_BTSAP_CLIENT
      || IS_BTSAP_HANDLE_ACTIVE(uim_ptr)
#endif /* FEATURE_UIM_BTSAP_CLIENT */
    )
  {
    /* Allocate command buffer space */
    if((q_cnt(& uim_ptr->hotswap.hotswap_rem_cmd_q) * UIM_NUM_DRV_SLOTS) >= UIM_HOTSWAP_NUM_CMD_BUFS)
    {
      UIMDRV_MSG_LOW_0(uim_ptr->id,"HOTSWAP:UIM was not able to process the card removed req" );
    }
    else if ((hos_cmd_ptr = (uim_cmd_type *)q_get( &uim_ptr->hotswap.hotswap_free_q)) != NULL)
    {
      hos_cmd_ptr->hdr.slot      = (uim_slot_type)(uim_ptr->id + 1);
      hos_cmd_ptr->hdr.channel   = UIM_DEFAULT_CHANNEL;
      hos_cmd_ptr->hdr.cmd_count = 0;
      q_put(&uim_ptr->hotswap.hotswap_rem_cmd_q, q_link(hos_cmd_ptr, &hos_cmd_ptr->hdr.cmd_hdr.link));
      UIMDRV_MSG_HIGH_0(uim_ptr->id,
                        "HOTSWAP: uim_hotswap_send_card_removed_cmd is successfully send to uim");
      (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_HOTSWAP_CMD_CARD_REM_SIG);
#ifdef FEATURE_UIM_BTSAP_CLIENT
      if(IS_BTSAP_HANDLE_ACTIVE(uim_ptr))
      {
        (void) rex_clr_timer( &uim_ptr->remote.remote_cmd_rsp_timer);
      }
#endif /* FEATURE_UIM_BTSAP_CLIENT */
      return TRUE;
    }
  }

  UIMDRV_MSG_HIGH_0(uim_ptr->id,
                    "HOTSWAP: uim_hotswap_send_card_removed_cmd is failed to send the UIM");
  return FALSE;
}/* uim_hotswap_send_card_removed_cmd */


/**
 * Send the card inserted cmd to uim task.
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return boolean Success or Fail.
 */
static boolean uim_hotswap_send_card_inserted_cmd(
  uim_instance_global_type *uim_ptr)
{
  uim_cmd_type *hos_cmd_ptr = NULL;

  if(uim_ptr->id >= UIM_MAX_INSTANCES)
  {
    return FALSE;
  }

  if( uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE
#ifdef FEATURE_UIM_BTSAP_CLIENT
      || IS_BTSAP_HANDLE_ACTIVE(uim_ptr)
#endif /* FEATURE_UIM_BTSAP_CLIENT */
    )
  {
    /* Allocate command buffer space */
    if ((q_cnt(& uim_ptr->hotswap.hotswap_ins_cmd_q) *  UIM_NUM_DRV_SLOTS) >= UIM_HOTSWAP_NUM_CMD_BUFS)
    {
      UIMDRV_MSG_ERR_0(uim_ptr->id,"HOTSWAP:Card inserted req failed" );
    }
    else if ((hos_cmd_ptr = (uim_cmd_type *)q_get( &uim_ptr->hotswap.hotswap_free_q)) != NULL)
    {
      hos_cmd_ptr->hdr.slot      = (uim_slot_type)(uim_ptr->id + 1);
      hos_cmd_ptr->hdr.channel   = UIM_DEFAULT_CHANNEL;
      hos_cmd_ptr->hdr.cmd_count = 0;
      q_put(&uim_ptr->hotswap.hotswap_ins_cmd_q, q_link(hos_cmd_ptr, &hos_cmd_ptr->hdr.cmd_hdr.link));
      (void) rex_set_sigs( uim_ptr->tcb_ptr, UIM_HOTSWAP_CMD_CARD_INS_SIG);
      return TRUE;
    }
  }
  return FALSE;
}/* uim_hotswap_send_card_inserted_cmd */


/**
 * Define the HOTSWAP debounce timer.
 *
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_hotswap_define_debounce_timer(uim_instance_global_type *uim_ptr)
{
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
  {
    timer_def(&(uim_ptr->hotswap.hotswap_slot_debounce_timer), &uim_ptr->task.always_on_timer_group, NULL, 0,
              uim_hotswap_debounce_timer_expiry_cb, (timer_cb_data_type)uim_ptr);
  }
} /* uim_hotswap_reset_debounce_timer */


/**
 * Reset the HOTSWAP debounce timer.
 *
 * @param hos_new_card_status New HOTSWAP status.
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_hotswap_reset_debounce_timer(
  uim_card_swap_enum_type    hos_new_card_status,
  uim_instance_global_type  *uim_ptr)
{
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
  {
    return;
  }

  timer_clr(&(uim_ptr->hotswap.hotswap_slot_debounce_timer), T_SCLK);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  if(hos_new_card_status == UIM_CARD_INSERTED )
  {
    timer_set( & uim_ptr->hotswap.hotswap_slot_debounce_timer,
               UIM_HOTSWAP_CARD_INSERTED_SAMPLE_PERIOD, 0, T_SCLK );
  }
  else
  {
    timer_set( &uim_ptr->hotswap.hotswap_slot_debounce_timer,
               UIM_HOTSWAP_CARD_REMOVED_SAMPLE_PERIOD, 0, T_SCLK );
  }
} /* uim_hotswap_reset_debounce_timer */


/**
 * Start the sample count and reset the debounce timer.
 *
 * @param hos_new_card_status New HOTSWAP status
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_hotswap_start_debounce_timer(
  uim_card_swap_enum_type        hos_new_card_status,
  uim_instance_global_type      *uim_ptr)
{
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
  {
   return;
  }
  uim_ptr->hotswap.hotswap_ins_sample_count       = 0x00;
  uim_ptr->hotswap.hotswap_rem_sample_count       = 0x00;
  uim_ptr->hotswap.hotswap_card_status            = hos_new_card_status;
  uim_ptr->hotswap.hotswap_debounce_retry_count   = 0x00;
  uim_ptr->hotswap.hotswap_debounce_in_progress   = TRUE;
  uim_hotswap_reset_debounce_timer(hos_new_card_status, uim_ptr);
} /* uim_hotswap_start_debounce_timer */


/**
 * Clear the uim hotswap timer
 *
 * @param uim_ptr Pointer to the global data common.
 */
static void uim_hotswap_clear_debounce_timer(uim_instance_global_type *uim_ptr)
{
  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == TRUE)
  {
    UIMDRV_MSG_HIGH_0(uim_ptr->id,"HOTSWAP: uim_hotswap_clear_debounce_timer");
    timer_clr(&(uim_ptr->hotswap.hotswap_slot_debounce_timer), T_SCLK);
    uim_ptr->hotswap.hotswap_debounce_in_progress   = FALSE;
  }
} /* uim_hotswap_clear_debounce_timer */


/**
 * This procedure is registered as a call-back associated with
 * the expiry of uim_hotswap_slot_debounce_timer, where we start
 * card status query for a fixed number of samples to get the
 * stable value of card status. This call-back is called in the
 * context of the high priority timer task. our sampling logic
 * based on the number of the allowed sample for card detection
 * and number of retrying count for this debounce. We have
 * different number for allowed sample count for card inserted
 * and card removed detection but Maximum debounce retry is same
 * for both query.
 *
 * @param instance_ptr Pointer to the global data common.
 */
static void uim_hotswap_debounce_timer_expiry_cb(unsigned long instance_ptr)
{
  uim_card_swap_enum_type   uim_new_query_status                  = UIM_CARD_UNCHANGED;
  uim_card_swap_enum_type   uim_curr_hos_card_status              = UIM_CARD_UNCHANGED;
  uint16                    *uim_hotswap_sample_couter_ptr        = NULL;
  uint16                    *uim_hotswap_debounce_retry_count_ptr = NULL;
  uint16                    uim_hotswap_max_sample_counter        = 0;
  boolean                   uim_hotswap_card_swap_debonce_end     = FALSE;
  boolean                   uim_hotswap_card_swap_debonce_success = FALSE;
  uim_instance_global_type *uim_ptr
    = (uim_instance_global_type*)instance_ptr;

  if(uim_ptr == NULL)
  {
    UIM_MSG_ERR_0("uim_hotswap_slot_debounce_timer_expiry_cb: uim_ptr is NULL");
    return;
  }

  RETURN_IF_INSTANCE_INVALID(uim_ptr->id);

  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
  {
    return;
  }

  /* Query for real time card status   */
  uim_new_query_status     = uim_hotswap_query_card_status (uim_ptr);

  /* Last ISR card status */
  uim_curr_hos_card_status =  uim_ptr->hotswap.hotswap_card_status;

  /* Get the debounce retry count */
  uim_hotswap_debounce_retry_count_ptr = &uim_ptr->hotswap.hotswap_debounce_retry_count;

  /* Get the sample count for query status */
  if (uim_curr_hos_card_status == UIM_CARD_INSERTED)
  {
    uim_hotswap_max_sample_counter = uim_ptr->hotswap.hotswap_me_conf.num_of_sample_for_insertion;
    uim_hotswap_sample_couter_ptr =  &uim_ptr->hotswap.hotswap_ins_sample_count;
  }
  else
  {
    uim_hotswap_max_sample_counter = uim_ptr->hotswap.hotswap_me_conf.num_of_sample_for_removal;
    uim_hotswap_sample_couter_ptr =  &uim_ptr->hotswap.hotswap_rem_sample_count;
  }

  /* Increment the sample counter */
  *uim_hotswap_sample_couter_ptr = *uim_hotswap_sample_couter_ptr +1;

  UIMDRV_MSG_HIGH_3(uim_ptr->id,"HOTSWAP: Debounce logic Sample Count 0x%x, Max Count 0x%x, RT Card Staus = 0x%x [1: Card Inserted]",
                    *uim_hotswap_sample_couter_ptr,
                    uim_hotswap_max_sample_counter,
                    uim_new_query_status);

  /* Check if the card detection status changes since the last ISR */
  if(uim_new_query_status == uim_curr_hos_card_status )
  {
    /* Query status and ISR status is same , continue sampling */
    if (*uim_hotswap_sample_couter_ptr >= uim_hotswap_max_sample_counter)
    {
      /* Debounce successfully completed !!!! */
      /* Store the stable new card detect status */
      /* send the hotswap signal to uim_task */
      uim_hotswap_card_swap_debonce_end = TRUE;
      uim_hotswap_card_swap_debonce_success = TRUE;
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"HOTSWAP: Debounce logic ended successfully, card_status : 0x%x", 
                        uim_curr_hos_card_status);

      if (FALSE == uim_hotswap_set_signal(uim_curr_hos_card_status, uim_ptr))
      {
        UIMDRV_MSG_ERR_0(uim_ptr->id,"HOTSWAP: Failed to set the CARD INSERTED or REMOVED signal");
      }
    }
    else
    {
      /* continue for next sampling */
      uim_hotswap_reset_debounce_timer( uim_curr_hos_card_status, uim_ptr);
    }
  }
  else
  {
    /* Query status and ISR status is not same, continue for next retry,
       It reset the sample count again */
    *uim_hotswap_debounce_retry_count_ptr = *uim_hotswap_debounce_retry_count_ptr + 1;
    if(*uim_hotswap_debounce_retry_count_ptr >= uim_ptr->hotswap.hotswap_me_conf.maximum_debounce_retry_counter)
    {
      /* Debounce is completed no change in card status */
      UIMDRV_MSG_HIGH_1(uim_ptr->id,"HOTSWAP: Debounce logic End, Unstable status: %d [0=REMOVED 1=INSERTED]",
                        uim_curr_hos_card_status);

      uim_hotswap_card_swap_debonce_end = TRUE;
    }
    else
    {
      /* Reset and start sampling again */
      *uim_hotswap_sample_couter_ptr  = 0;
      UIMDRV_MSG_HIGH_2(uim_ptr->id,"HOTSWAP: Debounce logic Maximum Retry 0x%x Current Count 0x%x",
                        uim_ptr->hotswap.hotswap_me_conf.maximum_debounce_retry_counter,
                        *uim_hotswap_debounce_retry_count_ptr);
      uim_hotswap_reset_debounce_timer(uim_curr_hos_card_status, uim_ptr);
    }
  }

  if(uim_hotswap_card_swap_debonce_end == TRUE)
  {
    *uim_hotswap_sample_couter_ptr        = 0;
    *uim_hotswap_debounce_retry_count_ptr = 0;
    uim_ptr->hotswap.hotswap_debounce_in_progress               = FALSE;
    if(TRUE == uim_hotswap_card_swap_debonce_success &&
       UIM_CARD_REMOVED == uim_curr_hos_card_status &&
       TRUE == uim_ptr->hotswap.hotswap_flaky_switch_support)
    {
      UIMDRV_MSG_HIGH_0(uim_ptr->id,
      "Hotswap flaky switch enabled,keep the hotswap interuupts masked till the card state is determined");
    }
    else
    {
      uim_hotswap_unmask_interrupt(uim_ptr);
    }
  }
} /* uim_hotswap_debounce_timer_expiry_cb */


/**
 * Get hotswap capability
 *
 * @param uim_ptr Pointer to the global data common.
 *
 * @return uim_hotswap_slot_capability Hotswap capability
 */
static uim_hotswap_slot_capability uim_get_hotswap_enabled_slot(
  uim_instance_global_type *uim_ptr)
{
  uim_hotswap_slot_capability  uim_hos_capability;
  memset(&uim_hos_capability, 0x00, sizeof(uim_hos_capability));

  if(uim_hw_if.intctrl[uim_ptr->id].m_hs_enable == FALSE)
  {
    return uim_hos_capability;
  }
  uim_hos_capability.is_uim_controller   = TRUE;
  uim_hos_capability.hos_enabled         = TRUE;
  uim_hos_capability.nGPIOInsertedLevel  = (DALGpioValueType)uim_hw_if.intctrl[uim_ptr->id].m_hs_polarity;
  uim_set_hotswap_polarity(uim_ptr->id, uim_hos_capability.nGPIOInsertedLevel);
  return uim_hos_capability;
} /* uim_util_get_hotswap_enabled_slot*/

