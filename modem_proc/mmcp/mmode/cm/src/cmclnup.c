/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   C L E A N U P   M O D U L E

GENERAL DESCRIPTION
  This module handles the IMS deregistration and DS cleanup needed for
  graceful SIM refresh, operating mode change operations

  This module is responsible for:
  1. Maintain a State machine independent of CM command procesing for IMS
     deregistration and DS cleanup.
  2. Maintain a seperate Buffer queue to hold the CM commands which need the
     above mentioned operations to conclude successfully before proceeding.

      ALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cm_clnup_init() must be called before invoking any other function in this
  module.

Copyright (c) 2014 by QUALCOMM TECHNOLOGIES INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmclnup.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/14   ak      Create file.

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"       /* Customer configuration file */
#include "comdef.h"         /* Definition for basic types and macros */
#include "cmidbg.h"
#include "cmph.h"
#include "cmipappi.h"
#include "cmtaski.h"
#include "cmclnup.h"
#include "cmtaski.h"
#include "cmsds.h"

/* CM Cleanup state machine.
*/
struct cm_clnup_state_s
{
  cm_clnup_stage_e_type   curr_clnup_stage;
      /* Stage for the current cleanup activity. IMS/DS.
      */
  uint8                   curr_clnup_oper;
      /* Operation responsible for the current cleanup activity. SUBS/OPRT
      */
  dword                   uptime;
      /* An uptime upper bound for how long we should wait
      ** for the reply to come */
  cm_clnup_curr_oper_payload_s_type clnup_payload;
      /* Store the payload used for callbacks during the cleanup.
      */
};

static cm_cmd_q_type      cm_buff_cmd_q;
    /* Queue to store clients commands. Clients command are temporarily
    ** being queued in this queue and then serviced in a FIFO order after
    ** cleanup is completed.
    */

static cm_clnup_state_s_type cmclnup_state_machine;
    /* Cleanup state machine */

static uint8        thpy_sub_clnup_complete;
    /* Third party IMS will return response one sub at a time instead of a
    ** consolodated bitmask. Aggregate the bitmask in this variable to
    ** determine if all subscriptions have cleaned up.
    */

/*===========================================================================

FUNCTION cm_buff_cmd_q_init

DESCRIPTION
  Initialize the client command queue.

  This function must be called before the buffer command queue is used in
  any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void                   cm_buff_cmd_q_init( void )
{
  (void) q_init( &cm_buff_cmd_q );
}

/*===========================================================================

FUNCTION cm_buff_cmd_q_cnt

DESCRIPTION
  Determine if there are any commands in the buffer command queue.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  TRUE: Buffer Queue is empty
  FALSE: Buffer Queue contains commands.

SIDE EFFECTS
  none

===========================================================================*/
       boolean                cmclnup_is_buff_q_empty( )
{
  return (q_cnt( &cm_buff_cmd_q ) == 0 );
}

/*===========================================================================

FUNCTION cm_buff_cmd_q_get

DESCRIPTION
  Dequeue a command buffer from the client command queue.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
       cm_cmd_type*           cm_buff_cmd_q_get(  )
/*lint -esym(765,cm_buff_cmd_q_get)
** Can't be static, CM unit test uses it
*/
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If the CM client command queue is empty, return without doing anything.
  */
  if( cmclnup_is_buff_q_empty() )
  {
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get next command from the CM client command queue.
  */
  return (cm_cmd_type*)q_get( &cm_buff_cmd_q );

}

/*===========================================================================

FUNCTION cm_buff_cmd_q_put

DESCRIPTION
  Enqueue a command buffer onto the client command queue.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void                   cm_buff_cmd_q_put(

       cm_cmd_type    *cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{

  CM_MSG_LOW_1("START cm_buff_cmd_q_put(), cmd_type=%d",cmd_ptr->cmd_type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer into the specified queue.
  */
  (void) q_link( cmd_ptr, &cmd_ptr->link);
  q_put( &cm_buff_cmd_q, &cmd_ptr->link );

}

/*===========================================================================

FUNCTION cmclnup_ptr

DESCRIPTION
  Return a pointer to the CM cleanup state machine.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_clnup_state_s_type*        cmclnup_ptr( void )
{
  return &cmclnup_state_machine;
} /* cmclnup_ptr() */

/*===========================================================================

FUNCTION cmclnup_empty_buff_q

DESCRIPTION
  Move all the commands on the buffer command queue to client command queue.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void                   cmclnup_empty_buff_q()
{
  cm_cmd_type*           cmd_ptr;

  /* Move all commands from buff_q to cmd_q*/
  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  while( (cmd_ptr = cm_buff_cmd_q_get()) != NULL )
  {
    cm_client_cmd_q_put(cmd_ptr);
  }
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  cmclnup_ptr()->curr_clnup_stage = CM_CLNUP_STAGE_DONE;

  /* Put a new command on client cmd q which will reset the cleanup state
  ** machine completely.
  */
  if( !cm_ph_cmd_reset_clnup_st_mc() )
  {
    CM_MSG_HIGH_0("CLNUP: Unable to queue reset cmd");
  }
}

/*===========================================================================

FUNCTION cm_buff_cmd_queue

DESCRIPTION
  Queue a CM command in its appropriate CM command queue.

DEPENDENCIES
  All CM command queues must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void                          cm_buff_cmd_queue(

       cm_ph_cmd_s_type const *      cmd_ptr,

       boolean                       is_cb_tobe_clrd
)
{


  CM_ASSERT(cmd_ptr != NULL);

  if( is_cb_tobe_clrd == TRUE )
  {
    /* This is the command initiating the clean up operation. For this command
    ** the calling module has already been notified with callback after the
    ** para_check. Need to clear the callback function pointer to avoid
    ** duplicate calls to the calling module.
    */
    cm_ph_cmd_s_type *ph_buff_cmd_ptr = cm_cmd_alloc_ph_init();

    *ph_buff_cmd_ptr = *cmd_ptr;

    ph_buff_cmd_ptr->cmd_cb_func     = NULL;
    ph_buff_cmd_ptr->data_block_ptr  = NULL;
    ph_buff_cmd_ptr->client_id       = CM_CLIENT_ID_ANONYMOUS;
    cm_buff_cmd_q_put( (cm_cmd_type *) ph_buff_cmd_ptr );
  }
  else
  {
    /* This command was just dequeued from the client_cmd_q. It has not been
    ** processed and thus its callback has not been invoked yet. Preserve the
    ** callback function pointer passed by the calling module.
    */
    ((cm_cmd_type*)cmd_ptr)->is_reused = TRUE;
    cm_buff_cmd_q_put( (cm_cmd_type *) cmd_ptr );
  }
}


/*===========================================================================

FUNCTION cmclnup_init

DESCRIPTION
  Initialize the Clean up state machine.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void                   cmclnup_init(

       cm_clnup_state_s_type* clnup_ptr )
{

  if( clnup_ptr == NULL )
  {
    return;
  }
  clnup_ptr->curr_clnup_stage = CM_CLNUP_STAGE_NONE;
  clnup_ptr->curr_clnup_oper  = CM_CLNUP_OPER_NONE;
  clnup_ptr->uptime           = 0;
  memset(&clnup_ptr->clnup_payload,0,sizeof(cm_clnup_curr_oper_payload_s_type));
}

/*===========================================================================

FUNCTION cmclnup_init

DESCRIPTION
  Initialize the Clean up state machine.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void                          cmclnup_st_mc_init( )
{
  cmclnup_init(cmclnup_ptr());
  cm_buff_cmd_q_init();
}

/*===========================================================================

FUNCTION cmclnup_is_ongoing_oprt_mode

DESCRIPTION
  Let the calling entity know if cleanup is currently in progress for
  operating mode change.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: Clean up currently in progress.
  FALSE: Cleanup operation not currently underway.

SIDE EFFECTS
  none

===========================================================================*/
boolean                       cmclnup_is_ongoing_oprt_mode( void )
{
  cm_clnup_state_s_type* clnup_ptr = cmclnup_ptr();

  if( clnup_ptr == NULL )
  {
    CM_ERR_0("CLNUP: Error getting cleanup state machine.");
  }

  return (( clnup_ptr->curr_clnup_oper & BM(CM_CLNUP_OPER_OPRT_MODE) )
          &&
          ( clnup_ptr->curr_clnup_stage != CM_CLNUP_STAGE_DONE) );
}

/*===========================================================================

FUNCTION cmclnup_is_ongoing_subsc

DESCRIPTION
  Let the calling entity know if cleanup is currently in progress.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: Clean up currently in progress.
  FALSE: Cleanup operation not currently underway.

SIDE EFFECTS
  none

===========================================================================*/
boolean                       cmclnup_is_ongoing_subsc(

       uint8                  subs_bm

)
{

  if( cmclnup_is_ongoing_oprt_mode() )
  {
    return TRUE;
  }
  else
  {
    cm_clnup_state_s_type* clnup_ptr = cmclnup_ptr();

    if( clnup_ptr == NULL )
    {
      CM_ERR_0("CLNUP: Error getting cleanup state machine.");
    }

    return ( ( clnup_ptr->curr_clnup_oper & subs_bm )
             &&
             ( clnup_ptr->curr_clnup_stage != CM_CLNUP_STAGE_DONE) );
  }
}

/*===========================================================================

FUNCTION cmclnup_is_clnup_needed

DESCRIPTION
  If the device is currently in Online operating mode and has subscription
  available, cleanup operation might be needed.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: Clean up might be needed.
  FALSE: Cleanup operation wont be needed.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmclnup_is_clnup_needed
( 
  cm_subscription_not_avail_cause_e_type sub_not_avail_cause, 
  sys_oprt_mode_e_type cmd_oprt_mode
)
{

  cmph_s_type *ph_ptr = cmph_ptr();

  /* Cleanup already completed.*/
  if( cmclnup_ptr()->curr_clnup_stage == CM_CLNUP_STAGE_DONE )
  {
    return FALSE;
  }

#if defined(FEATURE_IP_CALL)
  /* If CM is processing subscription not available and IWLAN registered 
  *  cleanup is needed. 
  */
  if(sub_not_avail_cause == CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_OTHERS)
  {
    return TRUE;
  }
#endif

  /* Always process LPM Oprt Mode if state is not in CM_CLNUP_STAGE_DONE 
   * to inform IMS and DS for cleanup incase of IMS over WiFi is active
  */
  if( ph_ptr->oprt_mode == SYS_OPRT_MODE_LPM &&
      cmd_oprt_mode     == SYS_OPRT_MODE_LPM)
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If operating mode is not Online, and above condition not met,
  *  no cleanup is needed. 
  */
  if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If no subscription is available, no cleanup will be needed. */
  if ( ph_ptr->is_gwl_subscription_available == FALSE
       && ph_ptr->is_hybr_gw_subscription_available == FALSE
       #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
       && ph_ptr->is_hybr_gw3_subscription_available == FALSE
       #endif
       && ph_ptr->is_cdma_subscription_available == FALSE
      )
  {
    return FALSE;
  }

  /*When subscirpiton loss is due to ILLEGAL SIM event , no cleanup will be needed
   */
  if (sub_not_avail_cause == CM_SUBSCRIPTION_NOT_AVAIL_CAUSE_ILLEGAL_SUB)
  {
    CM_MSG_HIGH_1("CLNUP: cleanup not needed as sub not avail cause is %d", sub_not_avail_cause);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return TRUE;
}

/*===========================================================================

FUNCTION cmclnup_initiate_ds_clnup

DESCRIPTION
  Initiate the DS cleanup operation.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  TRUE: Timer started.
  FALSE: Timer not started.

SIDE EFFECTS
  none

===========================================================================*/
static boolean                   cmclnup_initiate_ds_clnup(

       cm_clnup_state_s_type*    clnup_ptr
)
{

  boolean                  is_wait_timer_needed = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( clnup_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( clnup_ptr->curr_clnup_oper & BM( CM_CLNUP_OPER_OPRT_MODE ) )
  {
    is_wait_timer_needed = cmds_is_wait_on_oprt_mode(clnup_ptr->clnup_payload.oprt_mode);
  }
  else
  {
    is_wait_timer_needed = cmds_is_wait_on_subsc_cmd(clnup_ptr->clnup_payload.subs_id_bitmask);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( is_wait_timer_needed )
  {
    clnup_ptr->uptime = time_get_uptime_secs() + CMPH_MAX_DS_TRAFFIC_DOWN_TIME;
    CM_MSG_HIGH_2("CLNUP: DS clnup timer started uptime = %d, timer_exp = %d",
                   time_get_uptime_secs(),clnup_ptr->uptime);
  }

  return is_wait_timer_needed;

}

/*===========================================================================

FUNCTION cmclnup_update_st_mc

DESCRIPTION
  Update the state machine.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static void                   cmclnup_update_st_mc(

       cm_clnup_state_s_type* clnup_local
)
{
  cm_clnup_state_s_type*   clnup_ptr       = cmclnup_ptr();

  if( clnup_local == NULL ||
      clnup_ptr->curr_clnup_oper & BM(CM_CLNUP_OPER_OPRT_MODE) ||
      (clnup_local->clnup_payload.subs_id_bitmask>>1)>=MAX_SIMS)
  {
    return;
  }

  clnup_ptr->curr_clnup_oper |= clnup_local->curr_clnup_oper;
  clnup_ptr->curr_clnup_stage = clnup_local->curr_clnup_stage;
  clnup_ptr->uptime           = clnup_local->uptime;
  clnup_ptr->clnup_payload.oprt_mode    = clnup_local->clnup_payload.oprt_mode;
  clnup_ptr->clnup_payload.subs_id_bitmask |= clnup_local->clnup_payload.subs_id_bitmask;

  /*If oprt mode LPM received after subs deactivation, should not reset sub reason mask */
  if(!(clnup_local->curr_clnup_oper & BM(CM_CLNUP_OPER_OPRT_MODE)))
  {
    clnup_ptr->clnup_payload.sub_cleanup_reason[clnup_local->clnup_payload.subs_id_bitmask>>1] = 
      clnup_local->clnup_payload.sub_cleanup_reason[clnup_local->clnup_payload.subs_id_bitmask>>1];
  }

  return;
}

/*===========================================================================

FUNCTION cmclnup_initiate_cleanup

DESCRIPTION
  Initiate the cleanup operation.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void                          cmclnup_initiate_cleanup(

       cm_ph_cmd_s_type const *      ph_cmd_ptr

)
{
  cm_clnup_state_s_type  clnup_local;
  boolean                is_wait_timer_needed = FALSE;
  cm_ph_cmd_info_s_type const *cmd_info_ptr    = NULL;
sys_modem_as_id_e_type lte_sub_id = cmph_get_sub_with_lte_cap();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If the command pointer is NULL or
  ** Command is something other than OPRT_MODE or SUBS_NOT_AVAIL.
  ** Exit the cleanup.
  */
  if( ph_cmd_ptr == NULL
      ||
      ( ph_cmd_ptr->cmd != CM_PH_CMD_OPRT_MODE &&
        ph_cmd_ptr->cmd != CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE &&
        ph_cmd_ptr->cmd != CM_PH_CMD_DEACTIVATE_SUBS&&
        ph_cmd_ptr->cmd != CM_PH_CMD_DUAL_STANDBY_PREF)
    )
  {
    CM_MSG_HIGH_0("CLNUP: Error initiating cleanup.");
    return;
  }

  cmd_info_ptr = CMD_INFO_PTR( ph_cmd_ptr );
  cmclnup_init(&clnup_local);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ph_cmd_ptr->cmd )
  {
     case CM_PH_CMD_OPRT_MODE:
       clnup_local.curr_clnup_oper |= BM( CM_CLNUP_OPER_OPRT_MODE );
       clnup_local.clnup_payload.oprt_mode = cmd_info_ptr->oprt_mode;
       break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     case CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE:
       {
         uint8 subs_bm = cmph_compute_subs_bitmask(
                                      cmd_info_ptr->cdma_sub_not_avail_status,
                                      cmd_info_ptr->gwl_sub_not_avail_status,
                                      cmd_info_ptr->gw2_sub_not_avail_status,
                                      cmd_info_ptr->gw3_sub_not_avail_status);

         if( subs_bm == SYS_MODEM_AS_ID_NO_ACTIVE_MASK )
         {
           CM_ERR_1("CLNUP: No Subs_id impacted %d",subs_bm);
           return;
         }

       /* If UE already doing cleanup for oprt_mode, it will not initiate cleanup
       ** for subscription lost.
       ** If the UE already doing cleanup for subscription lost, and the new clnup
       ** is being initiated for subscription lost on another subs_id, we need to
       ** aggregate the affected subs_id into the final bitmask. So read the
       ** current payload. Cleanup reason is set for particular sub 
	   ** on which subs become unavailable
       */
         clnup_local.clnup_payload = cmclnup_ptr()->clnup_payload;

         clnup_local.curr_clnup_oper |= subs_bm;
         clnup_local.clnup_payload.subs_id_bitmask |= subs_bm;
	 clnup_local.clnup_payload.sub_cleanup_reason[subs_bm>>1] |= BM(CM_SUB_CLNUP_REASON_SUBS_NOT_AVAIL);

       }
       break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     case CM_PH_CMD_DEACTIVATE_SUBS:
       /* If UE already doing cleanup for oprt_mode, it will not initiate cleanup
       ** for subscription lost.
       ** If the UE already doing cleanup for subscription lost, and the new clnup
       ** is being initiated for subscription lost on another subs_id, we need to
       ** aggregate the affected subs_id into the final bitmask. So read the
       ** current payload.Cleanup reason is set for particular sub on which subs gets deactivated
       */
       clnup_local.clnup_payload = cmclnup_ptr()->clnup_payload;
       clnup_local.curr_clnup_oper |= cmd_info_ptr->cmd_subs_mask;
       clnup_local.clnup_payload.subs_id_bitmask |= cmd_info_ptr->cmd_subs_mask;
       clnup_local.clnup_payload.sub_cleanup_reason[cmd_info_ptr->cmd_subs_mask>>1] |= BM(CM_SUB_CLNUP_REASON_SUB_DEACTIVATED);
       break;

     case CM_PH_CMD_DUAL_STANDBY_PREF:
       /*
       ** If the UE standby pref has changed,then deactivated subs_id will be part of  final bitmask.
	   ** Cleanup reason is set for particular sub which got deactivated throug standby preference change
	   ** from AP.
       */
	 {
        uint8 deactivated_sub_mask = 0;
		cmph_find_standby_pref(ph_cmd_ptr->info, &deactivated_sub_mask);
        deactivated_sub_mask ^=  cmph_ptr()->active_subs ;
        clnup_local.clnup_payload = cmclnup_ptr()->clnup_payload;
        clnup_local.curr_clnup_oper |= deactivated_sub_mask;
        clnup_local.clnup_payload.subs_id_bitmask |= deactivated_sub_mask;
	    clnup_local.clnup_payload.sub_cleanup_reason[deactivated_sub_mask>>1] |= BM(CM_SUB_CLNUP_REASON_STANDBY_PREF_CHANGE);
     }
     break;


     default:
       CM_ERR_1("CLNUP: Invalid phone command %d",ph_cmd_ptr->cmd);
       return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If UE is currently performing IMS dereg on 3rd Party IMS and the new
  ** command is operating mode change, just update the payload and curr_oper.
  **
  ** There is no AI on 3rd Party IMS for operating mode. Allow the Subscription
  ** related dereg to complete. The updated payload and curr_oper will ensure
  ** that the subsequent DS cleanup will be Operating mode related.
  */
  if( cmclnup_ptr()->curr_clnup_stage == CM_CLNUP_STAGE_IMS_DEREG
      && cmph_get_sub_config_ptr(lte_sub_id)->active_ims_stack == SYS_IMS_STACK_STATUS_ENABLED_THPY
      && ph_cmd_ptr->cmd == CM_PH_CMD_OPRT_MODE )
  {
    cm_buff_cmd_queue( ph_cmd_ptr, TRUE );

    clnup_local.uptime           = cmclnup_ptr()->uptime;
    clnup_local.curr_clnup_stage = cmclnup_ptr()->curr_clnup_stage;

    cmclnup_update_st_mc(&clnup_local);

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Send deregistration command to IMS. */
  #ifdef FEATURE_IP_CALL

  if( clnup_local.curr_clnup_oper & BM( CM_CLNUP_OPER_OPRT_MODE ) )
  {
    if( cmipapp_send_oprt_mode (clnup_local.clnup_payload.oprt_mode, TRUE))
    {
      is_wait_timer_needed = cmipapp_is_wait_on_oprt_mode_cnf(clnup_local.clnup_payload.oprt_mode);
    }
  }
  else
  {
    if(cmipapp_send_subsc_cmd (ph_cmd_ptr->cmd,
                               clnup_local.clnup_payload.subs_id_bitmask,
                               clnup_local.clnup_payload.sub_cleanup_reason ))
    {
      is_wait_timer_needed = cmipapp_is_wait_on_subsc_cmd_cnf( ph_cmd_ptr->cmd );
    }
  }

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If timer is needed, set the cleanup timer value.
  ** Only if CM needs to wait for confirmation, put the command on to the
  ** Buffer queue.
  */
  if( is_wait_timer_needed )
  {
    cm_buff_cmd_queue( ph_cmd_ptr, TRUE );

    clnup_local.curr_clnup_stage = CM_CLNUP_STAGE_IMS_DEREG;
    clnup_local.uptime = time_get_uptime_secs() + CMPH_MAX_IPAPP_DEREG_TIME;

    cmclnup_update_st_mc(&clnup_local);

    CM_MSG_HIGH_2("CLNUP: IMS dereg timer started uptime = %d, timer_exp = %d",
                   time_get_uptime_secs(),clnup_local.uptime);

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If we got here, IMS cleanup did require to wait for confirmation.
  ** Trigger the DS cleanup right away.
  */
  if( cmclnup_initiate_ds_clnup(&clnup_local) )
  {
    cm_buff_cmd_queue( ph_cmd_ptr, TRUE );

    clnup_local.curr_clnup_stage = CM_CLNUP_STAGE_DS_CLNUP;

    cmclnup_update_st_mc(&clnup_local);

    CM_MSG_HIGH_2("CLNUP: DS clnup timer started uptime = %d, timer_exp = %d",
                   time_get_uptime_secs(),clnup_local.uptime);

    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Even DS cleanup did not require waiting for confirmation.
  */
  return;
}

#ifdef FEATURE_IP_CALL
/*===========================================================================

FUNCTION cmclnup_ip_rpt_proc

DESCRIPTION
  Process the IMS deregistration confirmation.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void                          cmclnup_ip_rpt_proc(

       cm_name_type           rpt_name,

       uint8                  subs_bm
)
{
  cm_clnup_state_s_type* clnup_ptr = cmclnup_ptr();
  sys_modem_as_id_e_type lte_sub_id = cmph_get_sub_with_lte_cap();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (rpt_name != CM_IP_APP_ALLAPPS_OPRT_CNF &&
       rpt_name != CM_IP_APP_ALLAPSS_SUBSC_CNF)
      ||
      clnup_ptr->curr_clnup_stage != CM_CLNUP_STAGE_IMS_DEREG
    )
  {
    CM_MSG_HIGH_2("CLNUP: Incorrect rpt %d or incorrect stage %d",rpt_name,
                                                 clnup_ptr->curr_clnup_stage);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rpt_name == CM_IP_APP_ALLAPSS_SUBSC_CNF )
  {
    if( clnup_ptr->curr_clnup_oper & BM( CM_CLNUP_OPER_OPRT_MODE ) &&
        cmph_get_sub_config_ptr(lte_sub_id)->active_ims_stack != SYS_IMS_STACK_STATUS_ENABLED_THPY)
    {
      CM_MSG_HIGH_0("CLNUP: Discard subscription cnf as oprt_mode clnup underway");
      return;
    }

    /* Reset the bits of the payload for which the response is received.
    */
    thpy_sub_clnup_complete |= (subs_bm & SYS_MODEM_AS_ID_ALL_ACTIVE_MASK);

    if( clnup_ptr->clnup_payload.subs_id_bitmask & (~thpy_sub_clnup_complete ))
    {
      uint8 temp = ( clnup_ptr->clnup_payload.subs_id_bitmask & (~thpy_sub_clnup_complete));

      CM_MSG_HIGH_1("CLNUP: Dereg confirmation pending on subs BM %d",temp);
      return;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  thpy_sub_clnup_complete = 0;

  if( !cmclnup_initiate_ds_clnup(clnup_ptr))
  {
    cmclnup_empty_buff_q();
  }
  else
  {
    clnup_ptr->curr_clnup_stage = CM_CLNUP_STAGE_DS_CLNUP;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;

}
#endif

/*===========================================================================

FUNCTION cmclnup_ds_pkt_rpt_proc

DESCRIPTION
  Process the DS cleanup confirmation.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
       void                   cmclnup_ds_pkt_rpt_proc( uint8 subs_id_bitmask )
{
  cm_clnup_state_s_type* clnup_ptr = cmclnup_ptr();
  uint8 ref_bitmask = 0;
  uint8 sub=0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( clnup_ptr->curr_clnup_stage != CM_CLNUP_STAGE_DS_CLNUP )
  {
    CM_MSG_HIGH_1("CLNUP: DS Cleanup in incorrect stage %d",clnup_ptr->curr_clnup_stage);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ref_bitmask = ((clnup_ptr->curr_clnup_oper & BM( CM_CLNUP_OPER_OPRT_MODE )) ?
                 0xFF :
                 clnup_ptr->clnup_payload.subs_id_bitmask);


  if( ref_bitmask != subs_id_bitmask )
  {
    CM_MSG_HIGH_2("CLNUP: Oprt_mode DS Cleanup incomp:oper %d bm %d",
                                  clnup_ptr->curr_clnup_oper,subs_id_bitmask);
    return;
  }

  /* reset subs specific reason only if there was no successive OPRT MODE change*/
  if(ref_bitmask!= 0XFF)
  {
    for(sub=0; sub<MAX_AS_IDS;sub++)
	{
     if(ref_bitmask &  BM( sub ))
	 {
       clnup_ptr->clnup_payload.sub_cleanup_reason[sub]=SUBS_CLEANUP_REASON_MASK_NONE;   
	 }
	}
  }
  else
  {
    memset(clnup_ptr->clnup_payload.sub_cleanup_reason,0,sizeof(uint8)*MAX_AS_IDS);
  }

  CM_MSG_HIGH_1("CLNUP: DS Cleanup success, stage %d",clnup_ptr->curr_clnup_stage);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmclnup_empty_buff_q();
}

/*===========================================================================

FUNCTION cmclnup_timer_proc

DESCRIPTION
  Process the DS cleanup confirmation.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void                          cmclnup_timer_proc( void )
{
  cm_clnup_state_s_type* clnup_ptr = cmclnup_ptr();
  dword                  uptime    = time_get_uptime_secs();
      /* Get the current uptime */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If no timer is running or timer has not yet expired no action needed. */
  if( !clnup_ptr->uptime ||
      clnup_ptr->uptime > uptime )
  {
    return;
  }

  CM_MSG_HIGH_1("CLNUP: Timer expiry in stage: %d",clnup_ptr->curr_clnup_stage);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If timer has expired, move on to the next stage of the cleanup. */
  switch( clnup_ptr->curr_clnup_stage )
  {
    #ifdef FEATURE_IP_CALL
    case CM_CLNUP_STAGE_IMS_DEREG:
      cmipapp_reset_trans_id ();
      if( clnup_ptr->curr_clnup_oper & BM(CM_CLNUP_OPER_OPRT_MODE) )
      {
        cmclnup_ip_rpt_proc( CM_IP_APP_ALLAPPS_OPRT_CNF, 0xFF);
      }
      else
      {
        cmclnup_ip_rpt_proc( CM_IP_APP_ALLAPSS_SUBSC_CNF,
                             clnup_ptr->clnup_payload.subs_id_bitmask );
      }
      return;
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_CLNUP_STAGE_DS_CLNUP:
    {
      uint8 bitmask = 0;
      if( clnup_ptr->curr_clnup_oper & BM(CM_CLNUP_OPER_OPRT_MODE) )
      {
        bitmask = 0xFF;
      }
      else
      {
        bitmask = clnup_ptr->clnup_payload.subs_id_bitmask;
      }
      cmclnup_ds_pkt_rpt_proc( bitmask );
    }
    return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      CM_MSG_HIGH_0("CLNUP: Invalid timer expiry");
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  clnup_ptr->uptime = 0;
  return;
}

/*===========================================================================

FUNCTION cmclnup_is_cmd_tobe_buff

DESCRIPTION
  Put the Phone command on to the buffer queue if the cleanup related to it is
  already underway.

DEPENDENCIES
  Buffer command queue must have already been initialized with
  cm_buff_cmd_q_init().

RETURN VALUE
  TRUE: Command was placed on the buffer queue.
  False: Command was not placed on the buffer queue.

SIDE EFFECTS
  none

===========================================================================*/
boolean                       cmclnup_is_cmd_tobe_buff(

       cm_ph_cmd_s_type const *      ph_cmd_ptr

)
{
  cm_ph_cmd_info_s_type const* cmd_info_ptr = CMD_INFO_PTR(ph_cmd_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ph_cmd_ptr == NULL ||
      cmclnup_ptr()->curr_clnup_stage == CM_CLNUP_STAGE_DONE )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If cleanup is already underway,queue this command to buffer queue and end
  ** the processing for now.
  */
  switch( ph_cmd_ptr->cmd )
  {
    case CM_PH_CMD_OPRT_MODE:
      if( cmclnup_is_ongoing_oprt_mode() )
      {
        cm_buff_cmd_queue( ph_cmd_ptr, FALSE );
        return TRUE;
      }
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE:
      {
        uint8 subs_bm = cmph_compute_subs_bitmask(
                                      cmd_info_ptr->cdma_sub_not_avail_status,
                                      cmd_info_ptr->gwl_sub_not_avail_status,
                                      cmd_info_ptr->gw2_sub_not_avail_status,
                                      cmd_info_ptr->gw3_sub_not_avail_status);
        if( cmclnup_is_ongoing_subsc(subs_bm) )
        {
          cm_buff_cmd_queue( ph_cmd_ptr, FALSE );
          return TRUE;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_PH_CMD_SUBSCRIPTION_AVAILABLE:
      {
        uint8 subs_bm = cmph_compute_subs_bitmask(
                                      cmd_info_ptr->cdma_sub_avail_status,
                                      cmd_info_ptr->gwl_sub_avail_status,
                                      cmd_info_ptr->gw2_sub_avail_status,
                                      cmd_info_ptr->gw3_sub_avail_status);
        if( cmclnup_is_ongoing_subsc(subs_bm) )
        {
          cm_buff_cmd_queue( ph_cmd_ptr, FALSE );
          return TRUE;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_PH_CMD_SUBSCRIPTION_CHANGED:
      {
        uint8 subs_bm = cmph_compute_subs_bitmask(
                                      cmd_info_ptr->cdma_sub_changed_status,
                                      cmd_info_ptr->gwl_sub_changed_status,
                                      cmd_info_ptr->gw2_sub_changed_status,
                                      cmd_info_ptr->gw3_sub_changed_status);
        if( cmclnup_is_ongoing_subsc(subs_bm) )
        {
          cm_buff_cmd_queue( ph_cmd_ptr, FALSE );
          return TRUE;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     case CM_PH_CMD_DEACTIVATE_SUBS:
     case CM_PH_CMD_ACTIVATE_SUBS:
       if( cmclnup_is_ongoing_subsc(cmd_info_ptr->cmd_subs_mask) )
       {
         cm_buff_cmd_queue( ph_cmd_ptr, FALSE );
         return TRUE;
       }
       break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      break;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION cmclnup_get_sub_reason_mask

DESCRIPTION
  Get the cleanup reason mask 

DEPENDENCIES
  none

RETURN VALUE
  cm_clnup_stage_e_type

SIDE EFFECTS
  none

===========================================================================*/
uint8* cmclnup_get_sub_reason_mask( void )
{
	return (&cmclnup_ptr()->clnup_payload)->sub_cleanup_reason;
}


#ifdef CM_DEBUG
#error code not present
#endif
