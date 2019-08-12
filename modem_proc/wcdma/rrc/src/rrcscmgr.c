/*===========================================================================
             R R C     S T A T E    C H A N G E    M A N A G E R

DESCRIPTION

  This module manages State Transitions of the RRC state machine.
  Since RRC has concurrent procedures, when one procedure wants
  to change RRC's state, other active procedures might be affected
  by the state change. This module provides a mechanism to notify
  procedures when a state transition occurs.

  Procedures can register with this module indicating that they
  need to be notified about a particular state transition. When the
  desired state transition occurs, this module sends an internal
  command to that procedure to notify it of the state change.

EXTERNALIZED FUNCTIONS

  rrcscmgr_register_for_scn   Called by procedures to register to be
                              notified of specific state changes.

  rrcscmgr_event_handler      Main function that handles commands sent
                              by the dispatcher to this module.

INITIALIZATION AND SEQUENCING REQUIREMENTS



Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcscmgr.c_v   1.5   18 May 2002 16:10:22   krishnan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcscmgr.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/14   sp      Made changes for Mav debug snapshot
10/06/14   sr      Made changes to not start ASF timer if CSG is not supported
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
01/30/14   bc      Made changes to not handle OOS when waiting for channel config cnf
                   during connection establishment and handle it when channel config cnf is received.
12/26/13   bc      Made changes to not to initiate new state indication from disconnected 
                   to connecting state when RCE is active.
08/13/13   sr      Made changes to not set TRM extension flag when in FACH state
08/13/13   gv      Added support for FEATURE_TRIPLE_SIM
07/31/13   vi      Mainlining the MODEM_STATISTICS feature.
04/04/13   sn      Changes to restart ASF timer when PLMN search in progress and upon
                   moving to PCH/disconnected states
01/03/13   ad      Made changes to reduce the redundant F3s
12/05/11   rl      Removed the A2 API calls
11/17/11   ad      Added featurization for A2 power optimization
11/15/11   sks     Made the changes for A2 power optimization.
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/04/11   sks     Fixed compilation warnings.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/18/10   rm      Fixed compiler warnings
05/20/10   gkg     Made changes to log RRC state in SMEM area.
05/11/10   rm      Fixed compiler warnings when FEATURE_MODEM_STATISTICS is defined
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
11/02/09   rm      Added support for FEATURE_MODEM_STATISTICS
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
11/04/08   rm      Added support for rove-in to GAN in RRC Connected mode.
                   Code changes are under FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE.
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
06/18/07   kp      Bug fix to take care of Buffer Overrun Vulnerability in check
                   to see free space available to register another state change.
03/09/07   vm      Added support for FEATURE_RRC_HEAP.
04/11/06   sgk     merged RVDS changes to mainline.
09/07/05   da      Initilizing struct hzi and setting flag loc_is_invalid to true when
                   cell loss is detected or when failing to get LAC id. Func affected :
                   rrc_trigger_home_zone_ind().
07/15/05   da      Calling rrc_get_lac_id instead of rrcbmc_get_lac_id in func
                   rrc_trigger_home_zone_ind()
06/24/05   svk     Added a field new_rate and set that to 0 in the rrc_state event
06/06/05   da      Changed FEATURE_HOME_ZONE_SERVICE implementation.  Added a
                   func rrc_register_home_zone_ind_cb() to register callback
                   to be called when cell id or rrc stat changes.  Added
                   rrc_deregister_home_zone_ind_cb() for deregistering cb func.
                   rrc_trigger_home_zone_ind() will call the cb func with providing
                   appropriate and state information.
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
12/03/04   da      Changed code for HomeZone feature.  Changed the typedef of
                   CB func used and changed the parameters passed accordingly.
12/02/04   da      Added support for HomeZone service under feature name
                   FEATURE_HOME_ZONE_SERVICE.  New functions added are
                   rrcscmrg_register_rrc_state_change_ind_cb, and
                   rrcscmgr_deregister_rrc_state_change_ind_cb.
08/30/04   sgk     Removed included file 'rrctask.h' to fix lint error Header
                   file 'rrctask.h' not used in module 'rrcscmgr.c'.
                   Removed return after ERR_FATAL in function
                   rrcscmgr_register_for_scn to fixl int error 'unreachable'.
                   Typecast old_state and new_state in function
                   rrcscmgr_notify_state_change_to_procs and
                   rrc_curr_state in function rrcscmgr_log_state_change to uint8
                   to fix lint error Converting enum to int.
06/19/04   kc      Changed ERR to ERR_FATAL if SCMGR cannot accept any more
                   state-change_req's.
04/30/03   vk      Support for EVENT_WCDMA_RRC_STATE.
12/20/02   sk      Added duplicate registration check in rrcscmgr_register_for_scn()
08/30/02   vn      Always enqueue State Change Indication even when number of
                   procedures is 0. This is to ensure that LLC always gets a
                   State Change Indication - needed for CR 24233 fix.
05/18/02   ram     Additional check for procedure name while sending state
                   change notifications.
05/13/02   ram     Modified the State Change Notification Mechanism. Since
                   many procedures have registered for state change notification
                   a large number of buffers are allocated at the same
                   time causing memory issues. Changed it to just send the
                   command once and add multiple procedure names in the
                   same command.
08/31/01   bu      Added function rrcscmgr_log_state_change to log the
                   current rrc state to QXDM
04/23/01   ram     Moved the functions rrc_init_state() and rrc_get_state() from
                   rrctask.c to here since all functions related to state change
                   will be performed by the state change manager. Created function
                   rrc_initiate_state_change() which replaces rrc_set_sate() since
                   we don't set the state immedietly but only after commands
                   pending in the queue have been processed. Updated the event
                   handler to support this change.
01/09/01   ram     Corrected loop counter error in function rrcscmgr_notify_
                   state_change_to_procs.
01/08/01   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "rrcscmgr.h"
#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "event.h"
#include "rrclogging.h"
#include "rrcccm.h"

#ifdef FEATURE_GAN
#error code not present
#endif


#include "rrcmmif.h"
#include "cmwll.h"


#ifdef FEATURE_DUAL_SIM
#include "rrccspi.h"
#endif

#ifdef FEATURE_WCDMA_LOG_RRC_STATES
#include "rrclogging.h"
#endif /* FEATURE_WCDMA_LOG_RRC_STATES */

#ifdef FEATURE_FEMTO_CSG
#include "rrctmr.h"
#include "rrccspi.h"
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Structure to hold each state change notification that has been
registered with this module */
typedef struct
{
  rrc_proc_e_type  procedure;   /* Procedure to be notified */
  rrc_state_e_type from_state;  /* State from which transition occurs */
  rrc_state_e_type to_state;    /* state to which transition occurs */
}rrc_state_change_notification_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* Array to store all the registered state change notifications */
rrc_state_change_notification_type rrcscn[RRC_TOT_SCN];


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Counter to keep track of the number of registrations */
uint32 rrcscn_reg_count = 0;

RRC_HOME_ZONE_IND_CB_TYPE *rrc_home_zone_ind_cb_func;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* The Current RRC State */
LOCAL  rrc_state_e_type  rrc_curr_state;
LOCAL  rrc_state_e_type rrc_prev_state;  /* To save previous state to be sent to
                                state change manager */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */



/*===========================================================================

FUNCTION RRC_GET_STATE

DESCRIPTION
  This function returns the current state of RRC.

DEPENDENCIES
  None

RETURN VALUE
  Current state of RRC.

SIDE EFFECTS
  None

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
rrc_state_e_type  rrc_get_state( void )
{
  return( rrc_curr_state );
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/



/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION RRC_INIT_STATE

DESCRIPTION

  This function initializes the default state of RRC at powerup.
  This function should be called only once, at powerup.

DEPENDENCIES
  None

RETURN VALUE
  Current state of RRC.

SIDE EFFECTS
  None

===========================================================================*/
void  rrc_init_state( void )
{
  /* Se the default state of RRC at power-up as the disconnected state */
  rrc_curr_state = RRC_STATE_DISCONNECTED;


  /* Reset callback func pointer to NULL */
  rrc_home_zone_ind_cb_func = NULL;


#ifdef FEATURE_WCDMA_LOG_RRC_STATES
  /* Open the circular power log buffer   */
  smem_log_open(SMEM_LOG_AREA_CIRCULAR_POWER);
#endif /*FEATURE_WCDMA_LOG_RRC_STATES*/

}



/*===========================================================================

FUNCTION RRC_INITIATE_STATE_CHANGE

DESCRIPTION

  This function initiates a state change in RRC by posting a
  RRC_NEW_STATE_IND command with the given RRC state. When this
  command is processed, the RRC state will be changed to the
  new state and state change notifications will be sent if needed.

  The reason the new state is not set righ away is that we
  want to avoid any potential race conditions with commands
  that are already queued. When commands that are already
  queued are processed before the RRC_NEW_STATE_IND command they
  may be calling rrc_get_state to get the current state which
  may lead to some race conditions.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void  rrc_initiate_state_change
(
  rrc_state_e_type  state  /* New state */
)
{

  rrc_cmd_type *cmd_ptr;        /* To send a command to state change manager */

  /* First make sure the new state is a valid state. */
  if( !(VALID_RRC_STATE(state)) )
  {
    WRRC_MSG1_ERROR("New RRC state is invalid - %d", state);
    return;
  }

  /* Send a RRC_NEW_STATE_IND command through the internal command queue. */
  cmd_ptr = rrc_get_int_cmd_buf();
  MSG_LOW("Initiating RRC State Change", 0, 0, 0);
  cmd_ptr->cmd_hdr.cmd_id = RRC_NEW_STATE_IND;
  cmd_ptr->cmd.new_state_ind.new_state = state;
  rrc_put_int_cmd(cmd_ptr);

#ifdef FEATURE_TRIPLE_SIM
  /* This flag will be set in FACH state. Depending on this flag
     G SUB uses 940 DRX   */
  if(((state == RRC_STATE_CELL_PCH) || (RRC_STATE_DISCONNECTED)) &&
     ((WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())))
  {
    rrcwrm_set_trm_extension_flag_for_alternate_page(TRUE);
  }
  else
  {
    rrcwrm_set_trm_extension_flag_for_alternate_page(FALSE);
  }
#endif
  }

/*===========================================================================

FUNCTION RRC_GET_PREV_STATE

DESCRIPTION
  This function returns the previous state of RRC.

DEPENDENCIES
  None

RETURN VALUE
  Current state of RRC.

SIDE EFFECTS
  None

===========================================================================*/
rrc_state_e_type  rrc_get_prev_state( void )
{
  return( rrc_prev_state );
}


/*===========================================================================

FUNCTION RRCSCMGR_REGISTER_FOR_SCN

DESCRIPTION

  This function registers State change notification requests from
  other procedures. When a procedure needs to be notified when a
  particular RRC state transition occurs, that procedure must register
  using this function. The from_state is the state from which the
  transition occurs and the to_state is the state to which the transition
  occurs. Wildcard may be used for both the from_state and to_state.
  The wildcard to be used is RRC_STATE_WILDCARD. When a wildcard is
  used for both from and to states, transitions from any state to
  any other state will be notified.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcscmgr_register_for_scn
(
  rrc_proc_e_type  procedure,   /* Procedure that's doing this registration */
  rrc_state_e_type from_state,  /* State from which the transition occurs. This
                                can be "RRC_STATE_WILDCARD" to indicate that the
                                from state can be any RRC state */
  rrc_state_e_type to_state     /* State to which the transition occurs. This
                                can be "RRC_STATE_WILDCARD" to indicate that the
                                to state can be any RRC state */
)
{

  uint32 localcount;

  /* Check if the given "from" state is either a valid state or a wildcard. */
  if( (!(VALID_RRC_STATE(from_state))) && (from_state != RRC_STATE_WILDCARD) )
  {
    WRRC_MSG1_HIGH("Invalid state registered %d", from_state);
    return;
  }

  /* Check if the given "to" state is either a valid state or a wildcard. */
  if( (!(VALID_RRC_STATE(to_state))) && (to_state != RRC_STATE_WILDCARD) )
  {
    WRRC_MSG1_HIGH("Invalid state registered %d", to_state);
    return;
  }

  /* Check if there is space available to register another state change */
  if( rrcscn_reg_count >= (RRC_TOT_SCN - 1) )
  {
    ERR_FATAL("No space for state change registration!", 0, 0, 0);
  }

  /* check for the duplicate registration */
  for (localcount=0; localcount<rrcscn_reg_count; localcount++)
  {
    if (procedure == rrcscn[localcount].procedure)
    {
      if ((rrcscn[localcount].from_state == RRC_STATE_WILDCARD)
         || (rrcscn[localcount].from_state == from_state))
      {
        if ((rrcscn[localcount].to_state == RRC_STATE_WILDCARD)
            || (rrcscn[localcount].to_state == to_state))
        {
          ERR("Duplicate SCM reg! proc:rrc_proc_e_type_value%d,from:%d,to:%d",
                      procedure, from_state, to_state);
          return;
        }
      }
    }
  }      /*end of for loop */

  /* Ok to register this. */
  rrcscn[rrcscn_reg_count].procedure = procedure;   /* Copy procedure */
  rrcscn[rrcscn_reg_count].from_state = from_state; /* Copy from state */
  rrcscn[rrcscn_reg_count].to_state = to_state;     /* Copy to_state */
  rrcscn_reg_count++; /* Increment counter */

}




/*===========================================================================

FUNCTION rrc_update_rrc_state_wcdma_UI_event_info

DESCRIPTION

  This function updates the RRC state WCDMA UI event info and notifies CM

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrc_update_rrc_state_wcdma_UI_event_info
(
  rrc_state_e_type new_state
)
{
  sys_wcdma_rrc_state_e_type new_sys_rrc_state = SYS_WCDMA_RRC_STATE_DISCONNECTED;

  if(RRC_IS_WCDMA_UI_EVENT_ENABLED(SYS_WCDMA_RRC_STATE))
  {
    switch(new_state)
    {
      case RRC_STATE_DISCONNECTED:
      case RRC_STATE_CONNECTING:
        new_sys_rrc_state = SYS_WCDMA_RRC_STATE_DISCONNECTED;
        break;
      case RRC_STATE_CELL_DCH:
        new_sys_rrc_state = SYS_WCDMA_RRC_STATE_CELL_DCH;
        break;
      case RRC_STATE_CELL_FACH:
        new_sys_rrc_state = SYS_WCDMA_RRC_STATE_CELL_FACH;
        break;
      case RRC_STATE_CELL_PCH:
        new_sys_rrc_state = SYS_WCDMA_RRC_STATE_CELL_PCH;
        break;
      case RRC_STATE_URA_PCH:
        new_sys_rrc_state = SYS_WCDMA_RRC_STATE_URA_PCH;
        break;
      default:
        WRRC_MSG1_ERROR("Invalid state %d",new_state);
    }
    wcdma_UI_event_info.wcdma_event_info.wcdma_rrc_state = new_sys_rrc_state;
    wcdma_UI_event_info.bitmask |= SYS_WCDMA_RRC_STATE;
    /*Call the CM API to set the Signal*/
#ifdef FEATURE_DUAL_SIM
    cm_per_subs_stats_set_modem_info_signal(SYS_MODEM_MODULE_WCDMA, rrc_get_as_id());
#else
    cm_stats_set_modem_info_signal(SYS_MODEM_MODULE_WCDMA);
#endif
    WRRC_MSG1_HIGH("FTD_DBG: RRC State change bit mask set, bit mask %x",wcdma_UI_event_info.bitmask);
  }
}

/*===========================================================================

FUNCTION RRCSCMGR_NOTIFY_STATE_CHANGE_TO_PROCS

DESCRIPTION

  This function looks through the stored registrations to see
  if the given state transition should be notified to any of
  the registered procedures. This is called by the event_handler
  when RRC moves to a new state.

  If it finds procedures that need to be notified, it sends
  an internal command RRC_STATE_CHANGE_IND to the internal command
  queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcscmgr_notify_state_change_to_procs
(
  rrc_state_e_type old_state,  /* RRC's previous state */
  rrc_state_e_type new_state   /* RRC's new state      */
)
{
  uint32 count;
  rrc_cmd_type *cmd_ptr;
  uint32 proc_count=0; /* Number of procedures that would
                       get the notification */
  rrc_event_state_change_info_type  state_change_event;

  MSG_LOW("Sending notification about state change", 0, 0, 0);

  /* Get a command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill up the command type, the old and new states  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_STATE_CHANGE_IND;
  cmd_ptr->cmd.state_change_ind.new_state = new_state;
  cmd_ptr->cmd.state_change_ind.previous_state = old_state;

  /* Look at each of the registered entries to see if this
  state change should be notified to any of the procedures.
  if so fill up the procedure name in the command */
  for( count = 0; count < rrcscn_reg_count; count++ )
  {
    /* The from state should either match the old state or be wildcard
    AND the to state should either match the new state or be wildcard */
    if( ((rrcscn[count].from_state == old_state) ||
        (rrcscn[count].from_state == RRC_STATE_WILDCARD)) &&
        ((rrcscn[count].to_state == new_state) ||
        (rrcscn[count].to_state == RRC_STATE_WILDCARD)) &&
        (rrcscn[count].procedure != RRC_PROCEDURE_NONE) )
    {
      /* We have a match. Add this procedure. */
      cmd_ptr->cmd.state_change_ind.procedure[proc_count]=
        rrcscn[count].procedure;
      proc_count++;
    }
  }/* end for */

  /* Always enqueue command even when count =0, since LLC has to get this ALWAYS */
  cmd_ptr->cmd.state_change_ind.num_procedures = proc_count;

  state_change_event.current_rrc_state = (uint8)old_state;
  state_change_event.next_rrc_state = (uint8)new_state;
  state_change_event.new_rate = 0;

  event_report_payload(EVENT_WCDMA_RRC_STATE, /* event toindicate that RRC has changed the state */
                       sizeof(rrc_event_state_change_info_type), /* size of the payload */
                       (void *)&state_change_event); /* The state change structure contains the
                                                         current state and the next state after transition*/
  MSG_HIGH("Sending STATE_CHANGE_IND to %d procs, Changed state from %d to %d ",proc_count,state_change_event.current_rrc_state,
            state_change_event.next_rrc_state);

  /* Now, send the command. */
  rrc_put_int_cmd( cmd_ptr );


  /*Notify the state change indication to the registered callback */
  if (rrc_home_zone_ind_cb_func)
  {
    rrc_trigger_home_zone_ind();
  }


#ifdef FEATURE_GAN
  #error code not present
#endif

#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
  #error code not present
#endif


  rrc_update_rrc_state_wcdma_UI_event_info(new_state);


#ifdef FEATURE_DUAL_SIM
  if((new_state == RRC_STATE_CELL_PCH) ||
     (new_state == RRC_STATE_URA_PCH))
  {
    WRRC_MSG1_HIGH("DSIM: Release the lock, new state %d",new_state);
    rrc_release_lock();
  }
#endif

#ifdef FEATURE_FEMTO_CSG
  /* Start the ASF timer for smaller duration when moving from traffic to non-traffic states and when the ASF timer has already expired. */
  if((TRUE == is_whitelist_valid) &&
      ((new_state == RRC_STATE_CELL_PCH) ||
       (new_state == RRC_STATE_URA_PCH) ||
       (new_state == RRC_STATE_DISCONNECTED))
        && (rrctmr_get_remaining_time(RRCTMR_CSG_ASF_SRCH_TIMER) == 0)
    )
  {
    MSG_HIGH("CSG: Start ASF timer of %d ms on state change %d -> %d", RRC_CSG_DISCONN_TRANS_ASF_SRCH_TIMER_IN_MS, old_state, new_state);
    rrctmr_start_timer(RRCTMR_CSG_ASF_SRCH_TIMER, RRC_CSG_DISCONN_TRANS_ASF_SRCH_TIMER_IN_MS);
  }
#endif
#ifdef FEATURE_QSH_EVENT_METRIC
  wrrc_qsh_log_rrc_state_stats(new_state);
#endif
}/* end function - rrcscmgr_notify_state_change_to_procs */


/*===========================================================================

FUNCTION RRC_REGISTER_HOME_ZONE__IND_CB

DESCRIPTION

This function enables upper layer to register a callback function if notification
of of current rrc state and location information is required.  Once the function is
registered, it will be called whenever there is a state change or cell id change.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrc_register_home_zone_ind_cb
(
  RRC_HOME_ZONE_IND_CB_TYPE *cb_ptr
)
{

  if (cb_ptr)
  {
    if (rrc_home_zone_ind_cb_func)
    {
      WRRC_MSG0_HIGH("Overwritting the existing hzi callback func");
    }
    WRRC_MSG0_HIGH("Registering hzi cb func");
    rrc_home_zone_ind_cb_func = cb_ptr;

  }
  else
  {
    WRRC_MSG0_ERROR("Cannot Register NULL hzi Callback");
  }

}

/*===========================================================================

FUNCTION RRC_DEREGISTER_HOME_ZONE_IND_CB

DESCRIPTION

This function enables upper layer to deregister the callback function that was
registered for Home zone indication.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrc_deregister_home_zone_ind_cb
(
  void
)
{
    WRRC_MSG0_HIGH("Deregistering hzi func ptr");
    rrc_home_zone_ind_cb_func = NULL;

}


/*===========================================================================

FUNCTION RRC_TRIGGER_HOME_ZONE_IND

DESCRIPTION

This function will call the register HZI cb func to notify the rrc state and
required location information

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrc_trigger_home_zone_ind
(
  void
)
{
  /* This stores state and location info needed by upper layers*/
  home_zone_ind_type hzi;
  /* This stores the LAI received in SIB1 */
  rrc_lai_type  lai;

  memset(&hzi,0,sizeof(home_zone_ind_type));
  memset(&lai,0,sizeof(rrc_lai_type));

  /* Populate all state and location information */
  hzi.prev_state = (uint32)rrc_prev_state;
  hzi.curr_state = (uint32)rrc_curr_state;
  if (RRC_CELL_LOSS_NO_NEW_CELL == rrc_ccm_get_serv_plmn_cellid(&hzi.cell_id, &hzi.plmn_id))
  {
    hzi.loc_info_invalid = TRUE;
    WRRC_MSG0_HIGH("HZI, cell loss");
  }

  if (SUCCESS == rrc_get_lac_id(&lai))
  {
    hzi.lac[0] = lai.lac[0];
    hzi.lac[1] = lai.lac[1];
  }
  else
  {
    hzi.loc_info_invalid = TRUE;
    WRRC_MSG0_ERROR("Failed to get LAC for HZI");
  }
  
  WRRC_MSG0_HIGH("Calling HZI callback func");
  rrc_home_zone_ind_cb_func(hzi);
}


#if (!(defined(T_WINNT)))
/*===========================================================================

FUNCTION RRCSCMGR_LOG_STATE_CHANGE

DESCRIPTION

  This function sends a log indicating the new RRC state to QXDM.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcscmgr_log_state_change( void )
{
  WCDMA_RRC_MODES_AND_STATES_type *log_ptr; /* Pointer to hold log packet */
  uint32 log_record_length;                 /* Length of the log packet   */

  /* Since there is a header included in the log packet, the length is
  calculated using the FPOS macro */
  log_record_length = (FPOS( WCDMA_RRC_MODES_AND_STATES_type, rrc_state) +
                       sizeof(uint8));

  /* Allocate the packet memory */
  log_ptr = (WCDMA_RRC_MODES_AND_STATES_type *)log_alloc( WCDMA_RRC_MODES_AND_STATES, log_record_length );

  /* If the memory allocation is sucssful, fill in the value of the
  new state */
  if( log_ptr != NULL )
  {
    log_ptr->rrc_state = (uint8) rrc_curr_state;
    /*commit the log record */
    log_commit(log_ptr);
  }
  /* If the memory allocation failed, QXDM was not on, so we
  can ignore the logging of this event. */
}
#endif /* (!(defined(T_WINNT))) */




/*===========================================================================

FUNCTION RRCSCMGR_EVENT_HANDLER

DESCRIPTION

  This is the event handler for the state change manager module.
  Only one event is expected here and that is the internal command
  RRC_NEW_STATE_IND indicating that RRC has moved to a new state.
  This function is called by the dispatcher.

  This function initiates the process of notifying procedures, if any,
  that need to be notified of the state change.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcscmgr_event_handler
(
  rrc_cmd_type *cmd_ptr    /* Pointer to received command */
)
{
  WRRC_MSG0_HIGH_OPT("function rrcscmgr_event_handler is called");

  /* We handle only one type of command - RRC_NEW_STATE_IND to indicate
  that we have a new RRC state. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_NEW_STATE_IND:

      /* First save the previous state and set the new RRC state */
      rrc_prev_state = rrc_curr_state;
      rrc_curr_state = cmd_ptr->cmd.new_state_ind.new_state;

      MSG_MED( "RRC State set to %d", rrc_curr_state, 0, 0 );

      /* Make sure the previous and current states are not the same. */
      if( rrc_prev_state == rrc_curr_state )
      {
        /* Bogus state change. No need to inform state change manager,
        just return */
        MSG_MED("Prev state and current state are same!", 0, 0, 0);
        return;
      }

#ifdef FEATURE_WCDMA_LOG_RRC_STATES
      switch(rrc_curr_state)
      {
        case RRC_STATE_CELL_DCH:
          if(rrc_get_hsdpa_status() == HSDPA_ACTIVE)
          {
            SMEM_LOG_EVENT_EX(SMEM_LOG_AREA_CIRCULAR_POWER, SMEM_LOG_RRC_DCH_STATE_HS_ON, 0, 0, 0, 0);
          }
          else
          {
            SMEM_LOG_EVENT_EX(SMEM_LOG_AREA_CIRCULAR_POWER, SMEM_LOG_RRC_DCH_STATE, 0, 0, 0, 0);         
          }
          break;

        case RRC_STATE_CELL_FACH:
          SMEM_LOG_EVENT_EX(SMEM_LOG_AREA_CIRCULAR_POWER, SMEM_LOG_RRC_FACH_STATE, 0, 0, 0, 0);
          break;

        case RRC_STATE_CELL_PCH:
        case RRC_STATE_URA_PCH:
          SMEM_LOG_EVENT_EX(SMEM_LOG_AREA_CIRCULAR_POWER, SMEM_LOG_RRC_PCH_STATE, 0, 0, 0, 0);
          break;

        case RRC_STATE_DISCONNECTED:
          SMEM_LOG_EVENT_EX(SMEM_LOG_AREA_CIRCULAR_POWER, SMEM_LOG_RRC_IDLE_STATE, 0, 0, 0, 0);
          break;

        default:
          MSG_LOW("Not logging RRC state",0,0,0);
      }
#endif /*FEATURE_WCDMA_LOG_RRC_STATES*/

      /* Notify all the procedures that registered to be notified
      about this state change */
      rrcscmgr_notify_state_change_to_procs( rrc_prev_state, rrc_curr_state );

#if (!(defined(T_WINNT)))
      /* Log the state change with QXDM */
      rrcscmgr_log_state_change();
#endif /* (!(defined(T_WINNT))) */
      break;

    default:
      WRRC_MSG1_HIGH("Invalid cmd %d Rx'ed by SCMGR", cmd_ptr->cmd_hdr.cmd_id);
      break;
  }
}
