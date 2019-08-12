/*==========================================================================

    C A L L   M A N A G E R  C M  O P E R A T I O N  S W I T C H  F I L E

DESCRIPTION
  This header file contains all the definitions necessary for
  clients to interface with the Call Manager module.

Copyright (c) 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmsoa.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/14   xs      hVOLTE SSAC support
01/21/14   sk      Depend on cmss internal srv status for prediction allowance
08/06/13   xs      Add function cmlog_csfb_svlte_switch() to log switch between
                   SVLTE and CSFB
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB Official Check-in
01/28/13   cl      Redesign the messages handler approach & adding activate-flag
01/22/13   cl      Fixing PLMN switching, prediction algorithm and enabled-flag
01/14/13   cl      Adding last configuration & VOLTE-12 support
11/30/12   cl      Adding CM UTs and small bug fixes
11/24/12   cl      Fixing CM UTs
11/17/12   cl      Adding EFS item for SVLTE-MCC list
11/16/12   cl      Adding Delay timer to prevent switch() during voice call
11/16/12   cl      Adding delay timer to avoid ping-pong scenarios.
11/14/12   cl      Adding SD to report PLMN-ID along with MCC.
11/12/12   cl      Adding EFS item for SVLTE PLMN-ID lists with dynamic list structure in cmsoa module.
11/10/12   cl      Adding EFS item for CMSOA configurations
11/06/12   cl      Adding new callback (generic type) for future uses.
11/04/12   cl      Connecting SD & CM with MCC messages sending from SD -> CM.
10/26/12   cl      Adding cmregprx and cmsoa handshakes before GWL SRV_REQ
10/23/12   cl      Adding switch() functions switching between CSFB and SVLTE
10/19/12   cl      Adding service indication messages handler.
10/17/12   cl      Adding MCC messages handlers.
10/10/12   cl      Initial release
===========================================================================*/

/*---------------------------------------------------------------------------
** Includes
**---------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"      /* Customer configuration file */
#include "cm_v.h"
#include "cm_i.h"
#include "cmtaski.h"
#include "cmidbg.h"        /* Call Manager Debug internal header file */
#include "cmefs.h"
#include "cmregprxi.h"
#include "cmph.h"
#include "cmsds.h"
#include "cmll.h"
#include "cmlog_v.h"

#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)
#include "mm.h"
#endif

#include "cmsoa.h"  /* Interface to CM SV Operation Abitration file */

#ifdef CM_DEBUG
#error code not present
#endif

#include "cmmsc.h"
#include "cmmsc_auto.h"
#include "cmsimcoord.h"
#include "policyman.h"
#include "mm.h"
#include "cmpmprx.h"
#include "cmclnup.h"


#if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)) || defined(FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH)

/*--------------------------------------------------------------------------
** DEFINES, CONSTANTS and FORWARD DECLARATIONS
**--------------------------------------------------------------------------
*/

/* Forward declaration */
static cmsoa_svlte_plmn_id_node_s_type* cmsoa_svlte_plmn_id_node_buf_else_err_fatal(void);
static int cmsoa_compare_plmn_func(
  void * item_ptr, void * compare_val
);
static void cmsoa_sv_switch_csfb_svlte( void );
static void cmsoa_sv_switch_svlte_csfb( void );
static boolean cmsoa_is_prediction_allowed(
   sd_ss_e_type          ss
  /* Stores the stack of found system.
  */
);
static boolean cmsoa_is_stack_state_valid(void);


/*--------------------------------------------------------------------------
** STATIC FUNCTIONS
**--------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmsoa_state_info_ptr

DESCRIPTION
  This function returns the pointer to CMSOA state info.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmsoa_state_info_s_type *cmsoa_state_info_ptr(void)
{
  static cmsoa_state_info_s_type  cmsoa_state_info;

  return &cmsoa_state_info;
}

/*===========================================================================

FUNCTION cmsoa_is_stack_state_valid

DESCRIPTION
  This function process mcc_msg. It performs prediction if the MCC belongs
  to SVLTE or CSFB country based on a configuration MCC lists.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmsoa_is_stack_state_valid(void)
{
  /* stack state */
  cmmsc_stack_state_e_type main_state = CMMSC_STACK_STATE_NONE;
  cmmsc_stack_state_e_type hybr2_state = CMMSC_STACK_STATE_NONE;

  main_state = cmmsimc_get_msc_stack_state(CM_SS_MAIN);
  hybr2_state = cmmsimc_get_msc_stack_state(CM_SS_HYBR_2);

  /* If state machine is not yet initialized - stop */
  if (main_state == CMMSC_STACK_STATE_NONE ||
     main_state == CMMSC_STACK_STATE_NULL ||
     hybr2_state == CMMSC_STACK_STATE_NONE ||
     hybr2_state == CMMSC_STACK_STATE_NULL)
  {
    CM_MSG_HIGH_0("CMSOA: MCC msg received in wrong state");
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION cmsoa_svlte_plmn_id_node_buf_else_err_fatal

DESCRIPTION
  This function allocate memory for a node in SVLTE-PLMN-ID list

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static cmsoa_svlte_plmn_id_node_s_type* cmsoa_svlte_plmn_id_node_buf_else_err_fatal (
  void
)
{
  cmsoa_svlte_plmn_id_node_s_type *plmn_node_ptr;

  plmn_node_ptr =
      (cmsoa_svlte_plmn_id_node_s_type *) cm_mem_malloc( sizeof(cmsoa_svlte_plmn_id_node_s_type) );

  /* If allocation failed, error fatal.
  */
  if( plmn_node_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  return plmn_node_ptr;
}

/*===========================================================================

FUNCTION cmsoa_compare_plmn_func

DESCRIPTION
  This function compares two plmns. Function format is created to used in
  queue ultilities function.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static int cmsoa_compare_plmn_func(
  void * item_ptr, void * compare_val
)
{

  cmsoa_svlte_plmn_id_node_s_type   *q_node_ptr  = NULL;
  sys_plmn_id_s_type                *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_node_ptr  = (cmsoa_svlte_plmn_id_node_s_type *) item_ptr;
  compare_ptr = (sys_plmn_id_s_type *) compare_val;

  if (item_ptr == NULL ||
      compare_val == NULL )
  {
    CM_MSG_HIGH_0("CMSOA: item_ptr or compare_val = NULL");
    return((int) FALSE);
  }

  CM_MSG_HIGH_6("CMSOA: comparing PLMN HEX([%x][%x][%x]),against SVLTE PLMN list HEX([%x][%x][%x])",
              compare_ptr->identity[0],
              compare_ptr->identity[1],
              compare_ptr->identity[2],
              q_node_ptr->plmn.identity[0],
              q_node_ptr->plmn.identity[1],
              q_node_ptr->plmn.identity[2]);

  //Method to compare two plmns.
  if( sys_plmn_match(q_node_ptr->plmn,*compare_ptr ))
  {
    CM_MSG_HIGH_0("CMSOA: Compared values are matched");
    return((int) TRUE );
  }

  return((int) FALSE );
}/* cmtask_orig_search_func_act_type */


/*===========================================================================

FUNCTION cmsoa_sv_switch_is_delayed

DESCRIPTION
  This function checks if an SV or non-SV operation switch should happens
  or not. Delay could be caused by:
  a) Voice call is on going
  b) We're ping-ponging between SV and non SV operations.

DEPENDENCIES
 - the reason why we should delay the SV-OPERATION switch()
 - output parameters: the timers which indicates how long we should delay.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static cmsoa_sv_switch_delay_reas_e_type cmsoa_sv_switch_is_delayed(void)
{
  cmsoa_state_info_s_type * cmsoa_state = cmsoa_state_info_ptr();
  dword uptime = time_get_uptime_secs();

  /* If we're in the ping-pong hystersis timer period - wait*/
  /* no ping pong from 1xsrlte to csfb */
  if (cmsoa_state->switch_delay_timer != 0 &&
      uptime < cmsoa_state->switch_delay_timer &&
      cmsoa_state->switch_delay_reason == CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG &&
      cmsoa_state->curr_sv_oprt_mode == CMSOA_SV_OPRT_MODE_SVLTE)
  {
    return CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If mt voice call is pending, do not trigger switch */
  if (cmph_ptr()->is_1x_mt_call_pending)
  {
    return CMSOA_SV_SWITCH_DELAY_REAS_VOICE_CALL;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  if (cmsoa_state->sxlte_timers.lte_idle_timer > 0)
  {
    CM_MSG_HIGH_1("curr switch delay reas: %d", cmsoa_state->switch_delay_reason);

    /* If connection status is not IDLE and switch delay reason is not already LTE_CONNECTED
    ** (implying that Twaitrrcidle timer was already started once), then return with delay reason 
    ** LTE_CONNECTED. This will ensure that the Twaitrrcidle timer does not get extended when RRC 
    ** remains in connected state for longer than the timer duration.
    */
    if(cmph_is_oprting_in_csfb_pref_srlte(SYS_MODEM_AS_ID_1)
        && mm_per_stacks_get_connection_status(SYS_MODEM_AS_ID_1,SYS_MODEM_STACK_ID_1) != SYS_GW_CONN_STATUS_IDLE
        && cmsoa_state->switch_delay_reason != CMSOA_SV_SWITCH_DELAY_LTE_CONNECTED)
    {
      return CMSOA_SV_SWITCH_DELAY_LTE_CONNECTED;
    }
  }
  #endif

  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  if (cmsoa_state->attach_complete_timer > 0 &&
      cmsoa_state->switch_delay_reason != CMSOA_SV_SWITCH_DELAY_ATTACH_COMPLETE)
  {
    CM_MSG_HIGH_3("CMSOA ATTACH COMPLETE TIMER: sub mode %d mode %d extend_srv_info %d",\
        cmph_is_oprting_in_csfb_pref_csfb_sub_mode(SYS_MODEM_AS_ID_1), 
        cmss_intl_srv_info_ptr(CM_SS_MAIN)->mode,
        cmss_intl_srv_info_ptr(CM_SS_MAIN)->extend_srv_info);
    
    if(cmph_is_oprting_in_csfb_pref_csfb_sub_mode(SYS_MODEM_AS_ID_1) &&
       (cmss_intl_srv_info_ptr(CM_SS_MAIN)->mode == SD_MODE_LTE) &&
       (cmss_intl_srv_info_ptr(CM_SS_MAIN)->extend_srv_info == SYS_EXTEND_SRV_INFO_REG_COMP_ACK_PENDING) )
    {
      return CMSOA_SV_SWITCH_DELAY_ATTACH_COMPLETE;
    }
  }
  #endif
  
  return CMSOA_SV_SWITCH_DELAY_REAS_NONE;

}

/*===========================================================================

  This function evaulates if capabilities to enable are supported.
  This function is less restrictive than cmmsc_auto_is_hybr_gwl_allowed, which
  check if CSFB is enabled or not in order to transition to SRLTE.
  cmmsc_auto_is_hybr_gwl_allowed considers CMSOA state, where this function
  needn't check that because result of this function decides CMSOA state.
===========================================================================*/

static boolean cmsoa_is_srlte_capability(sys_modem_as_id_e_type as_id)
{
  boolean srlte_cap = FALSE, switch_allowed_per_self_activation = TRUE;
  cmph_s_type  *ph_ptr = cmph_ptr();
  
  srlte_cap =
    (cmcall_misc_is_mode_pref (CM_MODE_PREF_CDMA_ONLY,CMPH_SS_MODE_PREF(as_id, ph_ptr)) &&
    cmcall_misc_is_mode_pref (CM_MODE_PREF_LTE_ONLY,CMPH_SS_MODE_PREF(as_id, ph_ptr)) &&
    cmph_is_valid_mode_pref(CM_MODE_PREF_CDMA_ONLY, as_id) &&
    cmph_is_valid_mode_pref(CM_MODE_PREF_LTE_ONLY, as_id) &&
    cmph_ptr()->is_cdma_subscription_available);
  
  srlte_cap &= cmmmgsdi_is_switch_allowed_per_csim_self_activation(as_id);   
  CM_MSG_HIGH_1("is_srlte_cap_returns %d",srlte_cap);
  return srlte_cap;
}


/*===========================================================================

FUNCTION cmsoa_clear_active_attach_complete_timer

DESCRIPTION
This function clears the attach complete timer,.if delay reason is 
CMSOA_SV_SWITCH_DELAY_ATTACH_COMPLETE andattach complete timer
is already running.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmsoa_clear_active_attach_complete_timer()
{

  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
  if (cmph_is_oprting_in_csfb_pref_csfb_sub_mode(SYS_MODEM_AS_ID_1) &&
      (cmsoa_state->switch_delay_state == CMSOA_SV_OPRT_MODE_SVLTE) &&
      (cmsoa_state->switch_delay_reason == CMSOA_SV_SWITCH_DELAY_ATTACH_COMPLETE) &&
      (cmsoa_state->is_attach_complete_timer_active == TRUE))
  {
    cmtask_attach_complete_timer_clear();
  }
}


/*===========================================================================

FUNCTION cmsoa_check_and_switch_to_srlte

DESCRIPTION
  This function performs all neccessary checks before it decides to whether
  switching to SRLTE operation.
  If a switch() operation is required, it will call to another function that
  will only performs the switch.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsoa_check_and_switch_to_srlte(cm_ss_e_type ss, boolean is_switch_needed)
{
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);
  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(as_id);
  boolean switching_to_srlte = FALSE;
  cmss_s_type    *ss_ptr   = cmss_ptr();
  cmph_s_type  *ph_ptr = cmph_ptr();

  if (cmph_is_subs_feature_mode_1x_sxlte(as_id))
  {
    CM_MSG_HIGH_2("main mode_pref=%d,hybr2_operational=%d", 
                   CMPH_SS_MODE_PREF(as_id,ph_ptr),
                   cmss_is_hybr2_operational());
    /* Switch SRLTE mode if following conditions are met
    ** 1) ue_mode is SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED
    ** 2) PLMN supports SRLTE   */
    if(state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED
        && cmss_is_hybr2_operational() == FALSE
        && cmsoa_is_srlte_capability(as_id) && !cmsds_ptr()->ext_volte_call_active)
    {
      boolean is_1xsrlte_mcc = FALSE;
  
      CM_MSG_HIGH_3("CMSOA: plmn 0x%x 0x%x 0x%x",
                       ss_ptr->info.sys_id.id.plmn.identity[0],
                       ss_ptr->info.sys_id.id.plmn.identity[1],
                       ss_ptr->info.sys_id.id.plmn.identity[2]);
                       
      if(policyman_svd_possible_on_plmn(&ss_ptr->info.sys_id.id.plmn, 
                                         FALSE, &is_1xsrlte_mcc ) ==
                                         POLICYMAN_STATUS_SUCCESS)
      {
        CM_MSG_HIGH_2("SRLTE: SRV_IND: is_1xsrlte_plmn=%d, curr_sub_mode=%d", 
                         is_1xsrlte_mcc, 
                         cmsoa_state_info_ptr()->curr_sv_oprt_mode);
                         
        if(is_1xsrlte_mcc == TRUE)
        {
          switching_to_srlte = TRUE;
          if(is_switch_needed)
          {
            if (cmss_intl_srv_info_ptr(CM_SS_MAIN)->extend_srv_info != SYS_EXTEND_SRV_INFO_REG_COMP_ACK_PENDING)
            {
              cmsoa_clear_active_attach_complete_timer();
            }
            cmsoa_sv_switch_base(CMSOA_SV_OPRT_MODE_SVLTE, ss,FALSE);
          }
        }
      }
      else
      {
        CM_MSG_HIGH_0("SRLTE: fail to check policyman plmn - WARNING!!!");
      }
    }    
  }
  return switching_to_srlte;
}


/*===========================================================================

FUNCTION cmsoa_sv_switch_base

DESCRIPTION
  This function performs all neccessary checks before it decides to whether
  switching to SVLTE or CSFB operation.
  If a switch() operation is required, it will call to another function that
  will only performs the switch.

DEPENDENCIES
  none

RETURN VALUE
  TRUE - switch happens
  FALSE - swith did no happen

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsoa_sv_switch_base(
  cmsoa_sv_oprt_mode_e_type  new_state,
  cm_ss_e_type               cm_ss,
  boolean                    switch_on_ims_reg  
)
{
  cmsoa_state_info_s_type * cmsoa_state = cmsoa_state_info_ptr();
  cmsoa_sv_switch_delay_reas_e_type delay_reas = CMSOA_SV_SWITCH_DELAY_REAS_NONE;
  cmph_s_type                   *ph_ptr = cmph_ptr();

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_HIGH_2("CMSOA: switch_base(), new_state=%d, switch_enabled=%d",
               new_state, cmsoa_state->config.is_switch_enabled);

  if (cmsoa_state->config.is_switch_enabled == FALSE)
  {
    CM_MSG_HIGH_0("CMSOA: Dynamic switches is disabled");
    return FALSE;
  }

  if (!cmph_is_subs_feature_mode_1x_sxlte(cmph_map_cm_ss_to_subs(cm_ss)))
  {
    CM_MSG_HIGH_0("CMSOA: non svlte/srlte sub");
    return FALSE;
  }

  if (ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    CM_MSG_HIGH_0("Skip prediction in MANUAL mode since device will be 3GPP only device");
    return FALSE;
  }

  if (cmclnup_is_ongoing_subsc(BM(SYS_MODEM_AS_ID_1)) ||
      cmclnup_is_ongoing_oprt_mode() ||
      ph_ptr->is_oprt_mode_change_ongoing)
  {
    CM_MSG_HIGH_0("CMSOA: Skip switch to to ongoing subscription or oprt_mode change");
    return FALSE;
  }

  /* Dont switch to SRLTE ,  
  ** while waiting for ESR response for a WPS call .
  ** Since the switch causes NAS to drop the ESR .
  ** However the switch will be forced immediately
  ** after this WPS call ends .
  */
  if (cmcall_is_there_hvolte_call_waiting_for_esr_rsp() != CM_CALL_ID_INVALID
      && new_state == CMSOA_SV_OPRT_MODE_SVLTE)
  {
    CM_MSG_HIGH_0("Hvolte:Switch postponed to call end");
    return TRUE;
  }

  if (cmsoa_state->is_attach_complete_timer_active)
  {
    CM_MSG_HIGH_0("Switch stopped due to active attach complete timer.");
    return TRUE;
  }

  /* Bail out if current state is the same as new state */
  if (cmsoa_state->curr_sv_oprt_mode == new_state)
  {
    /* For ping-pong scenario below, make sure we clear the action and armed
    ** the guard-timer again.
    ** 1. UE switches from CSFB --> SVLTE. Guard-timer is set.
    ** 2. UE switches back SVLTE --> CSFB. Not allowed during hysterisis period.
    ** 3. UE set switch-action. It remains in SVLTE and wait for guard timer expires.
    ** 4. It found MCC belongs to SVLTE again.
    ** 5. We should clear the action in step #3, and re-set in step #1.
    */
    //might not need this condi
    if (cmsoa_state->switch_delay_timer != 0 &&
        cmsoa_state->switch_delay_reason == CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG
        #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
        && cmph_is_oprting_in_csfb_pref_srlte(SYS_MODEM_AS_ID_1) == FALSE
        #endif
        )
    {
      cmsoa_arm_timer(CMSOA_SV_OPRT_MODE_NONE,
                                  CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG);

    }
    CM_MSG_HIGH_1("CMSOA: current sv-opr state = new state (= %d)",new_state);
    return FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Ignore the ping pong & lte connected delay timers ,
  ** if the switch is due to IMS registration .
  ** However MT page pending delay timer should be considered ,
  ** to make sure no MT calls are dropped due to switch */
  /* If we don't support Voice - new state is either SVLTE (based on the list) or CSFB */
  delay_reas = cmsoa_sv_switch_is_delayed();
  
  if (delay_reas != CMSOA_SV_SWITCH_DELAY_REAS_NONE
      && 
      (switch_on_ims_reg == FALSE ||
       delay_reas == CMSOA_SV_SWITCH_DELAY_REAS_VOICE_CALL))
  {
    cmsoa_state->switch_delay_state = new_state;
    /* except for ping-pong we need to arm the timer
    ** for ping-pong, timer is already armed */
    if (delay_reas != CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG)
    {
      cmsoa_arm_timer(new_state,
                                  delay_reas);
    }
    CM_MSG_HIGH_1("CMSOA: delay_reas %d",delay_reas);
    return FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* main logic */

  switch (cmsoa_state->curr_sv_oprt_mode)
  {
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CMSOA_SV_OPRT_MODE_SVLTE:
    {
      if (new_state == CMSOA_SV_OPRT_MODE_CSFB)
      {
          /* Switching from SVLTE to CSFB */
          cmsoa_sv_switch_svlte_csfb();
        }
      break;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CMSOA_SV_OPRT_MODE_CSFB:
    {
      if (new_state == CMSOA_SV_OPRT_MODE_SVLTE)
      {
          /* Switching from CSFB to SVLTE */
          cmsoa_sv_switch_csfb_svlte();
      }
      break;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    default:
      CM_MSG_HIGH_0("Incorrect SV State");
      return FALSE;
  }
  return TRUE;
}

static cmsoa_sv_switch_action_e_type cmsoa_state_to_action(cmsoa_sv_oprt_mode_e_type state)
{
  cmsoa_sv_switch_action_e_type action;
  switch( state )
  {

    case CMSOA_SV_OPRT_MODE_SVLTE:
      action = CMSOA_SV_SWITCH_ACTION_SWITCH_SVLTE;
      break;

    case CMSOA_SV_OPRT_MODE_CSFB:
      action = CMSOA_SV_SWITCH_ACTION_SWITCH_CSFB;
      break;

   case CMSOA_SV_OPRT_MODE_NONE:
    default:
      action = CMSOA_SV_SWITCH_ACTION_NONE;
      break;
  } /* switch( ss_info_ptr->sys_id.id_type ) */
  return action;
  }


/*===========================================================================

FUNCTION cmsoa_sv_switch_svlte_csfb

DESCRIPTION
  This function performs a switch from SVLTE -> CSFB by turning off the
  hybrid preference to non-SVLTE.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsoa_sv_switch_svlte_csfb( void )
{
  /* Indicate the hybr preference setting */
  cm_hybr_pref_e_type              new_hybr_pref;

  /* Pointer to phone object */
  cmph_s_type                     *ph_ptr = cmph_ptr( );

  /* Pointer to cmsoa_state object */
  cmsoa_state_info_s_type         *cmsoa_state = cmsoa_state_info_ptr();

  cm_pref_s_type         *hybr_2_pref_ptr = NULL;

  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);

  sys_ue_mode_e_type curr_ue_mode = state_machine->stack_common_info.ue_mode;


  if (ph_ptr == NULL || cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error checking */

  /* Update the hybr-pref */
  if (ph_ptr->main_stack_info.pref_info.hybr_pref == CM_HYBR_PREF_CDMA__LTE__HDR)
  {
    if(state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED)
    {
      CM_MSG_HIGH_0("CMSOA: Switching SRLTE -> CSFB");
    }
    else
    {
    CM_MSG_HIGH_0("CMSOA: Switching SVLTE -> CSFB");
    }
    new_hybr_pref = CM_HYBR_PREF_CDMA__HDR;
  }
  else
  {
    CM_MSG_HIGH_0("CMSOA: UE is not in SVLTE mode ");
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Execution */
  cmph_adjust_hybr_pref_for_pending_pref(new_hybr_pref);

  /* Set the pref_update_reason so we could send this reason down to SD */
  cmsoa_set_pref_update_reason( SD_SS_PREF_UPDATE_REASON_SWITCH_CSFB );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Allocate buffer only if multi-sim */
  if (cmph_is_msim())
  {
      hybr_2_pref_ptr = cm_pref_ptr();
      memscpy(hybr_2_pref_ptr, sizeof(cm_pref_s_type),
                     &(CMPH_HYBR_2_PREF(ph_ptr)), sizeof(cm_pref_s_type));
  }

  /* Update the current SV-oprt mode */
  cmsoa_state->curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;

  /* Write back the new SV-OPR mode into the EFS */
  cmsoa_state->config.curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmph_remove_orig_mode_with_priority_end(CM_SS_MAIN);
  cmph_remove_orig_mode_with_priority_end(CM_SS_HYBR_2);

  (void)cmph_force_pref_on_the_fly2(
                        CM_SS_MAX,
                        ph_ptr,
                        SD_SS_PREF_REAS_USER,
                        CM_ACT_TYPE_PH_OBJ,
                        ph_ptr->main_stack_info.pref_info.pref_term,
                        ph_ptr->main_stack_info.pref_info.mode_pref,
                        ph_ptr->main_stack_info.pref_info.band_pref,
                        ph_ptr->main_stack_info.pref_info.lte_band_pref,
                        ph_ptr->main_stack_info.pref_info.tds_band_pref,
                        ph_ptr->main_stack_info.pref_info.prl_pref,
                        ph_ptr->main_stack_info.pref_info.roam_pref,
                        new_hybr_pref, /* Forcing new hybr-pref */
                        ph_ptr->main_stack_info.pref_info.plmn,
                        ph_ptr->main_stack_info.pref_info.srv_domain_pref,
                        ph_ptr->main_stack_info.pref_info.acq_order_pref,
                        ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
                        hybr_2_pref_ptr,
                        #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
                        NULL,
                        #endif
                        CM_OTASP_ACT_CODE_NONE,
                        (cm_act_id_type)ph_ptr,
                        CM_ACT_UPDATE_REAS_USER,
                        FALSE,
                        cmph_get_acq_pri_order_per_sub(CM_SS_MAX),
                        CM_DEFAULT_VALUE,
                        ph_ptr->main_stack_info.pref_info.csg_id,
                        ph_ptr->main_stack_info.pref_info.csg_rat);

  /* Free allocated buffer */
  if (hybr_2_pref_ptr)
  {
    cm_mem_free (hybr_2_pref_ptr);
  }

  /* Update phone persistent values */
  cmph_ptr()->prst_main_stack_info[cmph_ptr()->curr_nam].hybr_pref = new_hybr_pref;

  if (curr_ue_mode != SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED)
  {
    cmsoa_write_soa_config_efs(&cmsoa_state->config);
  }



  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Cleaning up */

  /* After we forced the preference, clear the reason */
  cmsoa_set_pref_update_reason( SD_SS_PREF_UPDATE_REASON_NONE );

  /* Reset variables after switching to CSFB */
  *cmph_get_is_kicked_hybr_2_ptr() = FALSE;

  /* Also reset the voice-call timer / ping-pong timer after we switched */
  cmsoa_arm_timer(CMSOA_SV_OPRT_MODE_NONE,
                              CMSOA_SV_SWITCH_DELAY_REAS_NONE);

  /* Log the dynamic switch event */
  cmlog_csfb_svlte_switch( cmsoa_state_to_action(CMSOA_SV_OPRT_MODE_CSFB));

  return;
}

/*===========================================================================

FUNCTION cmsoa_sv_switch_csfb_svlte

DESCRIPTION
  This function performs a switch from CSFB -> SVLTE by turning off the
  hybrid preference to SVLTE. It also required to check if SVLTE is possible
  before performing the switch.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsoa_sv_switch_csfb_svlte( void )
{
  /* Indicate the hybr preference setting */
  cm_hybr_pref_e_type              new_hybr_pref;

  /* Pointer to phone object */
  cmph_s_type                     *ph_ptr = cmph_ptr( );

  /* Pointer to phone system selection object */
  cmss_s_type                     *ss_ptr = cmss_ptr();

  /* Pointer to CMSOA state info */
  cmsoa_state_info_s_type         *cmsoa_state = cmsoa_state_info_ptr();

  /* Current uptime */
  cm_pref_s_type             *hybr_2_pref_ptr = NULL;

  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);

  sys_ue_mode_e_type curr_ue_mode = state_machine->stack_common_info.ue_mode;

  if (ph_ptr == NULL || ss_ptr == NULL || cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error Checking */
  if ( ph_ptr->is_svlte_supported == FALSE &&
       curr_ue_mode != SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED)
  {
    CM_MSG_HIGH_0("CMSOA: Phone is not capable of SVLTE");
    return;
  }

  /* Update the hybr-pref */
  if ((ph_ptr->main_stack_info.pref_info.hybr_pref == CM_HYBR_PREF_CDMA__HDR) ||
      ((ph_ptr->main_stack_info.pref_info.hybr_pref == CM_HYBR_PREF_CDMA__LTE__HDR) &&
       (state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED))
     )
  {
    if(state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED)
    {
      CM_MSG_HIGH_0("CMSOA: Switching CSFB -> SRLTE");
    }
    else
    {
    CM_MSG_HIGH_0("CMSOA: Switching CSFB -> SVLTE");
    }
    new_hybr_pref = CM_HYBR_PREF_CDMA__LTE__HDR;
  }
  else
  {
    CM_MSG_HIGH_0("CMSOA: UE is not in hybr-mode enabled ");
    return;
  }

  CM_MSG_HIGH_2("def-delay-timer %d %d",cmsoa_state_info_ptr()->sxlte_timers.switch_pingpong_guard_timer,
                                                 cmsoa_state_info_ptr()->sxlte_timers.lte_idle_timer);


  cmph_adjust_hybr_pref_for_pending_pref(new_hybr_pref);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Execution */

  /* Set pref_update_Reason so we could send this reason to SD */
  {
    sd_ss_pref_update_reason_e_type  pref_update_reas;
    pref_update_reas = cmsoa_get_pref_update_reason(); 
    if (pref_update_reas != SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2 &&
        pref_update_reas != SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2_SILENT ) 
    {
      pref_update_reas = SD_SS_PREF_UPDATE_REASON_SWITCH_SVLTE;
      cmsoa_set_pref_update_reason( SD_SS_PREF_UPDATE_REASON_SWITCH_SVLTE );
    }
    CM_MSG_HIGH_1("CMSOA: pref update reason %d ",pref_update_reas );
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Allocate buffer only if multi-sim */
  if (cmph_is_msim())
  {
      hybr_2_pref_ptr = cm_pref_ptr();
      memscpy(hybr_2_pref_ptr, sizeof(cm_pref_s_type),
                     &(CMPH_HYBR_2_PREF(ph_ptr)), sizeof(cm_pref_s_type));
  }

  cmph_remove_orig_mode_with_priority_end(CM_SS_MAIN);
  cmph_remove_orig_mode_with_priority_end(CM_SS_HYBR_2);

  /* After switched, update the state */
  cmsoa_state->curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_SVLTE;

  /* Write back the new SV-OPR mode into the EFS */
  cmsoa_state->config.curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_SVLTE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)cmph_force_pref_on_the_fly2(
                        CM_SS_MAX,
                        ph_ptr,
                        SD_SS_PREF_REAS_USER,
                        CM_ACT_TYPE_PH_OBJ,
                        ph_ptr->main_stack_info.pref_info.pref_term,
                        ph_ptr->main_stack_info.pref_info.mode_pref,
                        ph_ptr->main_stack_info.pref_info.band_pref,
                        ph_ptr->main_stack_info.pref_info.lte_band_pref,
                        ph_ptr->main_stack_info.pref_info.tds_band_pref,
                        ph_ptr->main_stack_info.pref_info.prl_pref,
                        ph_ptr->main_stack_info.pref_info.roam_pref,
                        new_hybr_pref, /* Forcing new hybr-pref */
                        ph_ptr->main_stack_info.pref_info.plmn,
                        ph_ptr->main_stack_info.pref_info.srv_domain_pref,
                        ph_ptr->main_stack_info.pref_info.acq_order_pref,
                        ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
                        hybr_2_pref_ptr,
                        #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
                        NULL,
                        #endif
                        CM_OTASP_ACT_CODE_NONE,
                        (cm_act_id_type)ph_ptr,
                        CM_ACT_UPDATE_REAS_USER,
                        FALSE,
                        cmph_get_acq_pri_order_per_sub(CM_SS_MAX),
                        CM_DEFAULT_VALUE,
                        ph_ptr->main_stack_info.pref_info.csg_id,
                        ph_ptr->main_stack_info.pref_info.csg_rat);

  /* Free allocated buffer */
  if (hybr_2_pref_ptr)
  {
    cm_mem_free (hybr_2_pref_ptr);
  }

  /* Update phone persistent values */
  cmph_ptr()->prst_main_stack_info[cmph_ptr()->curr_nam].hybr_pref = new_hybr_pref;

  if (curr_ue_mode != SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED)
  {
    cmsoa_write_soa_config_efs(&cmsoa_state->config);
  }


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Cleaning up */

  /* After we forced the preference, clear the reason */
  cmsoa_set_pref_update_reason( SD_SS_PREF_UPDATE_REASON_NONE );

  /* Now that we switched to SVLTE - set the ping-pong guard timer */
  if (cmsoa_state->sxlte_timers.switch_pingpong_guard_timer > 0)
  {
    cmsoa_arm_timer(CMSOA_SV_OPRT_MODE_NONE,
                                CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG);
  }
  /* Log the dynamic switch event */
  cmlog_csfb_svlte_switch( cmsoa_state_to_action(CMSOA_SV_OPRT_MODE_SVLTE) );

  return;
}


/*--------------------------------------------------------------------------
** FUNCTIONS
** - Section 1: Contains all the queues related functions.
** - Section 2: Contains all the initialization functions, read NV.
** - Section 3: Contains all the command/reports/msg processing functions
** - Section 4: Contains all the Utilities functions
**--------------------------------------------------------------------------
*/


/***************************************************************************
** FUNCTIONS - SECTION 1: Queues / linked list / sending messages
** This section contains function handles queues, tasks related to CMSOA.
***************************************************************************/

/*===========================================================================

FUNCTION cmsoa_svlte_plmn_list_add

DESCRIPTION
  This function add a node which contains a PLMN to the SVLTE-PLMN-ID list

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_svlte_plmn_list_add(

    cm_cmd_q_type                      *q_ptr,
        /* pointer to a command queue to enqueue command buffer onto */

    cmsoa_svlte_plmn_id_node_s_type    *node_ptr
        /* pointer to a command buffer to be enqueued */
)
{

  if (q_ptr == NULL || node_ptr == NULL)
  {
    CM_ERR_0("MMSOA: q_ptr | node_ptr == NULL");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer
  ** into the specified queue.
  */
  (void) q_link( node_ptr, &node_ptr->link);
  q_put( q_ptr, &node_ptr->link );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_HIGH_3( "CMSOA: cmsoa_svlte_plmn_list_add(), plmn [%x][%x][%x]",
               node_ptr->plmn.identity[0],node_ptr->plmn.identity[1],
               node_ptr->plmn.identity[2] );

  return;
}

/*===========================================================================

FUNCTION cmsoa_svlte_plmn_list_search

DESCRIPTION
  This function search a plmn in the SVLTE-PLMN-ID list.

DEPENDENCIES
  none

RETURN VALUE
  boolean - indicates if the plmn is found or not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsoa_svlte_plmn_list_search(

    cm_cmd_q_type                      *q_ptr,
        /* pointer to a command queue to enqueue command buffer onto */

    sys_plmn_id_s_type                 *plmn
        /* plmn_info of the node */
)
{
  cmsoa_svlte_plmn_id_node_s_type *node_position = NULL;

  if (q_ptr == NULL || plmn == NULL)
  {
    CM_ERR_0("MMSOA: plmn == NULL or q_ptr == NULL");
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer
  ** into the specified queue.
  */
  node_position = q_linear_search(q_ptr, cmsoa_compare_plmn_func, (void*)plmn);

  if( node_position != NULL )
  {
    CM_MSG_HIGH_3( "CMSOA: Found plmn [%x][%x][%x]",node_position->plmn.identity[0],
                node_position->plmn.identity[1],node_position->plmn.identity[2]);
    return TRUE;
  }

  return FALSE;

}

/*===========================================================================

FUNCTION cmsoa_svlte_mcc_list_search

DESCRIPTION
  This function search a MCC in the SVLTE-MCC list.

DEPENDENCIES
  none

RETURN VALUE
  boolean - indicates if the MCC is found or not.

SIDE EFFECTS
  none

===========================================================================*/
boolean       cmsoa_svlte_mcc_list_search(

   /* MCC which we want to search */
   uint32     mcc
)
{
  /* State info pointer contains MCC list */
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
  int                      i;

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  for (i = 0; i < CMSOA_SVLTE_MCC_LIST_MAX; i++)
  {
    if ( cmsoa_state->config.svlte_mcc_list[i] == mcc)
    {
      CM_MSG_HIGH_1("MMSOA: found MCC %d in SVLTE-MCC list", mcc);
      return TRUE;
    }
  }

  return FALSE;
}

/*===========================================================================

FUNCTION cmsoa_regprx_srv_req_proceed_request_msg

DESCRIPTION
  This functions sends a messages to generic report queues asking for
  permission from CMSOA (arbitration module) to send SERVICE_REQ

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_regprx_srv_req_proceed_request_msg(cm_ss_e_type ss)
{

  cm_generic_rpt_s_type   *soa_rpt_ptr = NULL;
  soa_rpt_ptr             = cm_generic_rpt_get_buf_else_err_fatal();

  if (soa_rpt_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_HIGH_0("CMSOA: cm_soa send regprx_req_msg()");

  soa_rpt_ptr->hdr.cmd     =  CM_SOA_REGPRX_SRV_REQ_PROCEED;
  /* Store the current SV OPRT mode at the moment of request */
  soa_rpt_ptr->generic_rpt.regprx_srv_req.hyrb2_on
                           = cmss_is_hybr2_operational();
  soa_rpt_ptr->generic_rpt.regprx_srv_req.ss = (uint8)ss;

  cm_generic_rpt(soa_rpt_ptr);
}

/***************************************************************************
** FUNCTIONS - SECTION 2: Initializations
** Contains all the initialization functions, read NV.
***************************************************************************/

/*===========================================================================

FUNCTION cmsoa_check_and_dealloc_svlte_plmn_list

DESCRIPTION
  It deallocate memory for cmsoa svlte plmn list.

DEPENDENCIES
  cmsoa_state->svlte_plmn_list should be already intialized
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void cmsoa_check_and_dealloc_svlte_plmn_list(void)
{

  cmsoa_svlte_plmn_id_node_s_type  *cur_ptr = NULL;
  cmsoa_svlte_plmn_id_node_s_type  *next_ptr = NULL;
  int                      q_counter = 0;
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();  
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  q_counter = q_cnt(&(cmsoa_state->svlte_plmn_list));
 
  /* If queue has some elements, delete the memory allocated for them */
  if( q_counter > 0)
  {
    // delete all the items from the queue
    cur_ptr = q_check(&(cmsoa_state->svlte_plmn_list));

    if( NULL == cur_ptr )
    {
      sys_err_fatal_null_ptr_exception();
    }
    
    while((q_counter > 0) && (cur_ptr != NULL))
    {
      next_ptr = q_next(&(cmsoa_state->svlte_plmn_list), &(cur_ptr->link));

      #ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete( &(cmsoa_state->svlte_plmn_list), &cur_ptr->link );
      #else
      q_delete( &cur_ptr->link );
      #endif

      /* Free memory for this node */
      cm_mem_free( cur_ptr);
      
      q_counter--;
      cur_ptr = next_ptr ;
    } /* end while */
  }/* end if */

}

/*===========================================================================

FUNCTION cmsoa_init

DESCRIPTION
  This functions initialized cmsoa state info.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_init(boolean is_pwr_on)
{
  int                      i;
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
  sys_plmn_id_s_type       temp_plmn;
  sys_ue_mode_e_type curr_ue_mode;
  int32 size_of_opti_flag = 0;
  mmode_1xsxlte_optimization_s_type read_opti;
  sys_subs_feature_t sfmode;

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* Initialization */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if svlte_plmn_list already has some memory, 
  ** free it before allocating new memory
  */
  if(!is_pwr_on)
  {
    cmsoa_check_and_dealloc_svlte_plmn_list();
  }

  /* Initialize the SVLTE plmn-id list */
  (void) q_init( &(cmsoa_state->svlte_plmn_list) );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize non-configurables values */
  sys_plmn_undefine_plmn_id(&temp_plmn);
  cmsoa_state->last_gwl_plmn = temp_plmn;
  cmsoa_state->pref_update_reason = SD_SS_PREF_UPDATE_REASON_NONE;

  cmsoa_state->switch_delay_timer = 0;
  cmsoa_state->switch_delay_state = CMSOA_SV_OPRT_MODE_NONE;

  cmsoa_state->is_activated = FALSE;
  cmsoa_state->is_svlte_predicted = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize the configurable items with defaults and then read */
  cmsoa_state->config.curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_NONE;
  cmsoa_state->config.is_volte12 = TRUE;
  cmsoa_state->config.is_switch_enabled = FALSE;
  cmsoa_state->is_c2k_modeswitch2srlte = FALSE;
  cmsoa_state->init_sub_mode = CMSOA_SV_OPRT_MODE_NONE;
  
  for (i = 0; i < CMSOA_SVLTE_MCC_LIST_MAX; i++)
  {
    cmsoa_state->config.svlte_mcc_list[i] = 0;
  }


  (void) cmsoa_read_soa_config_efs();

  /* read efs to decide if to power up from last sub mode */
  size_of_opti_flag = sizeof(mmode_1xsxlte_optimization_s_type);
  if(cmefs_read(CMEFS_MMODE_1XSXLTE_OPTIMIZATION, (byte*)&read_opti, &size_of_opti_flag))
  {
    cmsoa_state->is_pwrup_from_last_submode = read_opti.opti[0];
  }
  else
  {
    cmsoa_state->is_pwrup_from_last_submode = FALSE;
  }

  cmpmprx_read_ue_mode(SYS_MODEM_AS_ID_1,&curr_ue_mode);

  /*If this is MCFG refresh, update ue mode read from PM to CMMSC state machine as well */
  if(is_pwr_on == FALSE)
  {
    cmmsc_state_machine_s_type   *state_machine_ptr = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);
    state_machine_ptr->stack_common_info.ue_mode = curr_ue_mode;
  }

  if(curr_ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED)
  {
    cmsoa_state->config.is_switch_enabled = TRUE;
    /* if power up from last sub mode optimization is not enabled
    ** set default curr sub mode to CSFB
    */
    if(cmsoa_state->is_pwrup_from_last_submode == FALSE)
    {
      cmsoa_state->config.curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;
    }
    if(cmsoa_state->config.curr_sv_oprt_mode == CMSOA_SV_OPRT_MODE_CSFB)
    {
      cmsoa_state->curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;
    }
    else if(cmsoa_state->config.curr_sv_oprt_mode == CMSOA_SV_OPRT_MODE_SVLTE)
    {
      cmsoa_state->curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_SVLTE;
    }
  }

  /* When powering up with ue mode CSFB, set switch as disbaled and
   sv_oprt_mode as CSFB 
  */
  (void)cmpmprx_get_subs_feature_mode(SYS_MODEM_AS_ID_1,&sfmode);

  if((curr_ue_mode == SYS_UE_MODE_CSFB || 
     curr_ue_mode == SYS_UE_MODE_NONE || 
     curr_ue_mode == SYS_UE_MODE_NORMAL
    ) &&
    sfmode == SYS_SUBS_FEATURE_MODE_SRLTE
  )
  {
    cmsoa_state->curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;
    cmsoa_state->config.curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;
    cmsoa_state->config.is_switch_enabled = FALSE;
  }
  cmsoa_state->init_sub_mode = cmsoa_state->curr_sv_oprt_mode;

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  if(curr_ue_mode != SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED
     && curr_ue_mode != SYS_UE_MODE_1XSRLTE_ONLY)
  {
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* After initialize, need to read the value from EFS */
  if (!cmsoa_read_plmn_id_efs())
  {
    /* Dynamically creates a node */
    cmsoa_svlte_plmn_id_node_s_type    *node_ptr =
                    cmsoa_svlte_plmn_id_node_buf_else_err_fatal();

    if (node_ptr != NULL)
    {
      /* Default value only has 1 Verizon PLMN ID for SVLTE plmn list */
      sys_plmn_set_mcc_mnc2(TRUE,311,480,&node_ptr->plmn);
      cmsoa_svlte_plmn_list_add(&cmsoa_state->svlte_plmn_list, node_ptr);
    }

    #ifndef FEATURE_DISABLE_CM_INIT_NV_WRITE
    /* write back to EFS :: better design, should pass in a default list
    ** that way, we only have to set the default value at 1 place
    */
    cmsoa_write_plmn_id_efs_default();
    #endif
  }
}
#endif

}

/*===========================================================================

FUNCTION cmsoa_read_plmn_id_efs

DESCRIPTION
  This function reads the SVLTE PLMN ID list from EFS

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsoa_read_plmn_id_efs()
{
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();

  /* Store all the plmns read to an array - now assume 64 elements */
  sys_plmn_id_s_type plmn_arr[64];
  int32  size = sizeof(plmn_arr);
  int counter = 0;
  int plmn_num = 0;

  /* Clear the plmn_arr[] */
  memset(plmn_arr,0,size );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(cmefs_read(CMEFS_SOA_SVLTE_PLMN_ID_LIST, (byte*)plmn_arr, &size) &&
     size > 0)
  {
    /* Calculate with the read size, how much plmn we have */
    plmn_num = (int)(size / sizeof(sys_plmn_id_s_type));

    /* If we could read the file successfully, then add all the plmn
    ** into the array
    */
    for (counter = 0; counter < plmn_num; counter++)
    {
      /* Dynamically creates a node */
      cmsoa_svlte_plmn_id_node_s_type    *node_ptr =
                    cmsoa_svlte_plmn_id_node_buf_else_err_fatal();

      if (node_ptr == NULL)
      {
        continue;
      }

      /* Stop if until we have a plmn id that's not defined */
      if (sys_plmn_id_is_undefined(plmn_arr[counter]))
      {
        modem_mem_free((void*)node_ptr, MODEM_MEM_CLIENT_MMODE);
        break;
      }

      /* Update the node_ptr and adding to the list*/
      node_ptr->plmn = plmn_arr[counter];
      cmsoa_svlte_plmn_list_add(&cmsoa_state->svlte_plmn_list, node_ptr);
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION cmsoa_write_plmn_id_efs_default

DESCRIPTION
  This function write default SVLTE-PLMN (311-480) into the EFS

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_write_plmn_id_efs_default()
{

  sys_plmn_id_s_type plmn_arr[64];
  int32  size = sizeof(sys_plmn_id_s_type);


  /* Clear the plmn_arr[] */
  memset(plmn_arr,0,size );

  /* Default SVLTE PLMN-ID contains the U.S. MCC:
  ** MCC 311, MNC 480
  */
  sys_plmn_set_mcc_mnc2(TRUE,311,480,&plmn_arr[0]);

  /*set the size to be 1 items only */
  size = size*1;

  CM_MSG_MED_0 ("CMSOA: Writing default SVLTE-PLMN-ID list");
  cmefs_write( CMEFS_SOA_SVLTE_PLMN_ID_LIST,(byte*)&plmn_arr, size);

  return;
}

/*===========================================================================

FUNCTION cmsoa_read_soa_config_efs

DESCRIPTION
  This function reads SOA configuration from EFS

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsoa_read_soa_config_efs(void)
{

  boolean ret = FALSE;
  int32 size =  sizeof(cmsoa_config_s_type);
  sys_ue_mode_e_type curr_ue_mode;
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
  sys_subs_feature_t pry_sfmode = SYS_SUBS_FEATURE_MODE_NORMAL;

  cmpmprx_read_ue_mode(SYS_MODEM_AS_ID_1,&curr_ue_mode);
  cmpmprx_get_subs_feature_mode(SYS_MODEM_AS_ID_1,&pry_sfmode);

  CM_MSG_HIGH_1("CMSOA: UeMode %d",curr_ue_mode);

  if(pry_sfmode != SYS_SUBS_FEATURE_MODE_SRLTE
    && pry_sfmode != SYS_SUBS_FEATURE_MODE_SVLTE)
  {
    return TRUE;
  }
  size =  sizeof(cmsoa_config_s_type);
  ret = cmefs_read(CMEFS_SOA_CONFIG, (byte*)&cmsoa_state->config, &size);
  /* data read must be more than 0 */
  if (size <= 0)
    ret = FALSE;

  /* set default values if no EFS is found */
  if(!ret)
  {
    cmsoa_state->config.is_switch_enabled = FALSE;
    if(pry_sfmode == SYS_SUBS_FEATURE_MODE_SVLTE)
    {
      cmsoa_state->config.curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_SVLTE;
      cmsoa_state->curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_SVLTE;
      cmsoa_state->config.svlte_mcc_list[0] = 310;
      cmsoa_state->config.svlte_mcc_list[1] = 311;
      cmsoa_state->config.svlte_mcc_list[2] = 312;
    }
    else if(pry_sfmode == SYS_SUBS_FEATURE_MODE_SRLTE)
    {
      cmsoa_state->config.curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;
      cmsoa_state->curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;
    }
    

    #ifndef FEATURE_DISABLE_CM_INIT_NV_WRITE
    /* And write back the default into EFS item */
    /* Some clean up could be done here by writing the default with passed
    ** in parameters
    */
    cmsoa_write_soa_config_efs(&cmsoa_state->config);
    #endif
  }

  CM_MSG_HIGH_1("CMSOA: init svoa_mode=%d",cmsoa_state->config.curr_sv_oprt_mode);
  cmsoa_state->curr_sv_oprt_mode = cmsoa_state->config.curr_sv_oprt_mode;

  if(pry_sfmode == SYS_SUBS_FEATURE_MODE_SRLTE)
  {
    size = sizeof(sxlte_timers_s_type);
    if(!cmefs_read(CMEFS_SXLTE_TIMERS, (byte*)&cmsoa_state->sxlte_timers, &size))
    {
      cmsoa_state->sxlte_timers.switch_pingpong_guard_timer =
               CMSOA_SRLTE_SWITCH_DELAY_REASON_PINGPONG_TIMER;
      cmsoa_state->sxlte_timers.lte_idle_timer =
               CMSOA_SRLTE_SWITCH_DELAY_REASON_RRC_IDLE_DEFAULT_TIMER;
    }

    /* Read value of C2kswitch2srlte from EFS */
    if ( cmefs_get ( CMEFS_C2KSWITCH2SRLTE,
                     (byte*)&cmsoa_state->is_c2k_modeswitch2srlte, 
                     sizeof(boolean),
                     SYS_MODEM_AS_ID_1) )
    {
      CM_MSG_HIGH_1 (" CMEFS_C2KSWITCH2SRLTE = %d ", cmsoa_state->is_c2k_modeswitch2srlte);
    }
    else
    {
      CM_ERR_0("Can't read CMEFS_C2KSWITCH2SRLTE ");
      /* set default value to FALSE */
      cmsoa_state->is_c2k_modeswitch2srlte = FALSE;
    }

    if ( cmefs_get ( CMEFS_ATTACH_COMPLETE_TIMER,
                     (byte*)&cmsoa_state->attach_complete_timer, 
                     sizeof(unsigned long),
                     SYS_MODEM_AS_ID_1))
    {
      CM_MSG_HIGH_1 (" CMEFS_ATTACH_COMPLETE_TIMER = %d ", cmsoa_state->attach_complete_timer);
    }
    else
    {
      CM_ERR_0("Can't read cmphCMEFS_ATTACH_COMPLETE_TIMER ");
      /* set default value to 600 ms */
      cmsoa_state->attach_complete_timer = CMSOA_ATTACH_COMPLETE_DEFAULT_TIMER;
    }
  }
  return TRUE;

}

/*===========================================================================

FUNCTION cmsoa_write_soa_config_efs

DESCRIPTION
  This function write SOA configuration to EFS. It happens that sometimes
  as we power-down, we need to write to EFS. It also required to write back
  the SV-OPRT mode after we performed the switch. As we hit this function
  we guarantee that the arbitration is currenly enabled and activated.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsoa_write_soa_config_efs(cmsoa_config_s_type *soa_config)
{
  boolean ret = FALSE;
  int size = sizeof(cmsoa_config_s_type);

  if (soa_config == NULL)
  {
    CM_ERR_0("CMSOA: write soa_config ptr is null");
    return FALSE;
  }

  ret = cmefs_write( CMEFS_SOA_CONFIG, (byte*)soa_config, size);
  CM_MSG_HIGH_2("CMSOA: Write back SOA config = %d, size=%d", ret, size );

  return ret;
}


/***************************************************************************
** FUNCTIONS - SECTION 3: Messages Processing / Core logic & algorithm
** This section contains function handles each messages/reports and commands.
***************************************************************************/


/*===========================================================================

FUNCTION cmsoa_msg_proc

DESCRIPTION
  This function is a generic function that process messages that sent to
  SOA module.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_msg_proc(
   const cm_generic_rpt_s_type *msg_ptr
)
{
  if (msg_ptr == NULL)
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Switch to process each reports */
  switch( msg_ptr->hdr.cmd )
  {
     /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
     case CM_SOA_MSG_MCC:
       cmsoa_mcc_msg_process((cmsoa_mcc_s_type*)&msg_ptr->generic_rpt.mcc_msg);
       break;

     /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
     case CM_SOA_REGPRX_SRV_REQ_PROCEED:
       cmsoa_regprx_srv_req_proceed_process((cm_generic_rpt_u_type *)&msg_ptr->generic_rpt);
       break;

     case CM_ATTACH_COMPLETE_RPT:
        cmsoa_attach_complete_rpt_proc(FALSE);
        break;
        
     /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
     default:
       break;
  }
}

/*===========================================================================

FUNCTION cmsoa_mcc_msg_process

DESCRIPTION
  This function process mcc_msg. It performs prediction if the MCC belongs
  to SVLTE or CSFB country based on a configuration MCC lists.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_mcc_msg_process( cmsoa_mcc_s_type *mcc_msg)
{
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
  cmsoa_sv_oprt_mode_e_type new_sv_state = CMSOA_SV_OPRT_MODE_NONE;

  /* Get LTE_IMS_VOICE availability */
  cm_sds_ims_voice_avail_e_type lte_ims_voice_availability =
                                       CMSDS_IMS_VOICE_NONE;

  /* stack state */
  cmmsc_stack_state_e_type main_state = CMMSC_STACK_STATE_NONE;
  cmmsc_stack_state_e_type hybr2_state = CMMSC_STACK_STATE_NONE;

  boolean is_mcc_found = FALSE;

  uint32 mcc = mcc_msg->mcc;
  cm_ss_e_type cm_ss = cmph_map_sd_ss_type_to_cm_ss_type(mcc_msg->ss);

  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  sys_modem_as_id_e_type mcc_msg_asubs_id
    = cmph_map_cm_ss_to_subs(
        cmph_map_sd_ss_type_to_cm_ss_type(mcc_msg->ss));
  #endif
  
  #ifdef FEATURE_DOMAIN_SELECTION
  lte_ims_voice_availability = cmsds_get_lte_ims_voice_availability();
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Gather Information & Error Checking */
  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  if (!cmsoa_state->is_activated)
  {
    return ;
  }

  if (!cmph_is_subs_feature_mode_1x_sxlte(cmph_map_cm_ss_to_subs(cm_ss)))
  {
    CM_MSG_HIGH_0("CMSOA: non svlte/srlte sub");
    return;
  }

  /* Print plmn info */
  if ( mcc_msg->mode == SYS_SYS_MODE_GWL)
  {
    CM_MSG_HIGH_3("CMSOA: MCC_MSG:    plmn 0x%x 0x%x 0x%x",
                mcc_msg->plmn_id.identity[0],
                mcc_msg->plmn_id.identity[1],
                mcc_msg->plmn_id.identity[2]);
  }

  main_state = cmmsimc_get_msc_stack_state(CM_SS_MAIN);
  hybr2_state = cmmsimc_get_msc_stack_state(CM_SS_HYBR_2);

  /* If state machine is not yet initialized - stop */
  if (main_state == CMMSC_STACK_STATE_NONE ||
     main_state == CMMSC_STACK_STATE_NULL)
  {
    CM_MSG_HIGH_0("CMSOA: MCC msg received in wrong state");
    return;
  }

  if (hybr2_state == CMMSC_STACK_STATE_NONE ||
     hybr2_state == CMMSC_STACK_STATE_NULL)
  {
    CM_MSG_HIGH_0("CMSOA: MCC msg received in wrong state");
    return;
  }

  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  /* while operating in SRLTE mode ,
          **if we move to an MCC which doesnt supports SRLTE ,
          **then we need to move to single stack mode */

  /* FR 1xsrlte - switch to CSFB ,
  ** since current PLMN doesnt supports SRLTE
  */
  if(cmph_is_subs_feature_mode_srlte(mcc_msg_asubs_id))
  {
    if(cmph_is_oprting_in_csfb_pref_srlte(mcc_msg_asubs_id)
        && (mcc_msg->ss == SD_SS_HYBR_2)  &&
       (mcc_msg->srv_status == SYS_SRV_STATUS_SRV))
    {
      boolean is_1xsrlte_plmn = FALSE;

      if(policyman_svd_possible_on_plmn(&(mcc_msg->plmn_id), FALSE,
                                        &is_1xsrlte_plmn) ==
                                        POLICYMAN_STATUS_SUCCESS)
      {
        CM_MSG_HIGH_4("SRLTE: MCC_MSG: is_1xsrlte_plmn=%d, ss=%d, curr_sub_mode=%d srv_status=%d", 
                   is_1xsrlte_plmn, 
                   mcc_msg->ss, 
                   cmsoa_state->curr_sv_oprt_mode,
                   mcc_msg->srv_status);

        if(is_1xsrlte_plmn == FALSE)
        {
          /* reset the ping pong timer ,
          ** since it is not applicable when PLMN is not in white list
          */
          cmsoa_arm_timer(CMSOA_SV_OPRT_MODE_NONE,
                                      CMSOA_SV_SWITCH_DELAY_REAS_NONE);
        
          cmsoa_sv_switch_base(CMSOA_SV_OPRT_MODE_CSFB, 
                               cmph_map_sd_ss_type_to_cm_ss_type(mcc_msg->ss),
                               FALSE);
        }
      }
      else
      {
        CM_ERR_0("SRLTE: fail to check policyman plmn !!!");
      }
    }
    return;
  }
  #endif
  
  /* if this is GWL system, we store the PLMN id - this should be done
  ** eventhough we decided to bail out
  */
  if ( mcc_msg->mode == SYS_SYS_MODE_GWL)
  {
    CM_MSG_HIGH_0("CMSOA: Store last plmn");

    cmsoa_state->last_gwl_plmn = mcc_msg->plmn_id;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Conditions not to perform prediction */


  /* If prediction not allowed */
  if (!cmsoa_is_prediction_allowed( mcc_msg->ss))
  {
    CM_MSG_HIGH_0("CMSOA: Skipping prediction");
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Decision */

  /* Check if MCC belongs to the SVLTE-MCC list */
  is_mcc_found = cmsoa_svlte_mcc_list_search(mcc);

  if( is_mcc_found )
  {
    /* If we have already did prediction that we were in SVLTE and now we're
    ** in CSFB because of complete plmn does not belongs to SVLTE PLMN list.
    ** In this case, we don't need to perform prediction because we already
    ** performed it before. This often happens in the transition state, moving
    ** LTE from HYBR-2 stack to MAIN after SVLTE -> CSFB switch.
    */
    if (cmsoa_state->is_svlte_predicted)
    {
      CM_MSG_HIGH_0("CMSOA: already performed prediction");
      return;
    }

    /* If current LTE system has lte_support voice, stay with CSFB
    ** This is the scenario where we're in a country that supports SVLTE,
    ** However, we're acquiring LTE system which supports voice. There
    ** no need for SVLTE here. So although we keep detecting MCC supporting
    ** SVLTE, we should ignore them and sticks with CSFB.
    */
    if (lte_ims_voice_availability == CMSDS_IMS_VOICE_AVAIL &&
        cmss_ptr()->info.srv_status == SYS_SRV_STATUS_SRV &&
        cmss_ptr()->info.sys_mode == SYS_SYS_MODE_LTE )
    {
      CM_MSG_HIGH_0("CMSOA: mcc-svlte but LTE supports voice");
      new_sv_state = CMSOA_SV_OPRT_MODE_CSFB;
    }
    else
    {
      new_sv_state = CMSOA_SV_OPRT_MODE_SVLTE;
    }

  }
  else
  {
    new_sv_state = CMSOA_SV_OPRT_MODE_CSFB;
  }

  CM_MSG_HIGH_3("CMSOA: mcc_msg: mcc=%d, mcc_found=%d, new_sv_state =%d",
               mcc, is_mcc_found, new_sv_state);

  cmsoa_sv_switch_base(new_sv_state, cm_ss,FALSE);
}

#ifdef FEATURE_DOMAIN_SELECTION
/*===========================================================================

FUNCTION cmsoa_process_domain_selection_srv_ind

DESCRIPTION
  This function is called to process new domain selection information
  after we have received new service indications.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_process_domain_selection_srv_ind (

   const sd_si_info_s_type   *si_info_ptr,
   /* Pointer to buffer containing new service info information */

   cm_ss_e_type               cm_ss
)
{
  cmsoa_sv_oprt_mode_e_type new_sv_state = CMSOA_SV_OPRT_MODE_NONE;
  boolean is_lte_voice_support = FALSE;

  cmsoa_state_info_s_type        *cmsoa_state = cmsoa_state_info_ptr();

  sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(cm_ss);

  /* Get CSFB capability and LTE_IMS_VOICE availability */
  cm_call_csfb_e_type csfb_type = cmsds_get_csfb_type_allowed(asubs_id);
  cm_sds_ims_voice_avail_e_type lte_ims_voice_availability =
                              cmsds_get_lte_ims_voice_availability();
  cmph_s_type                   *ph_ptr = cmph_ptr();

  if (si_info_ptr == NULL)
  {
    CM_ERR_0("CMSOA: si_info_ptr == NULL");
    return; //wrong usage don't cause err fatal
  }

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* return if feature is not activated */
  if (!cmsoa_state->is_activated)
  {
    CM_MSG_HIGH_0("CMSOA: Arbitration is not activated ");
    return;
  }

  if (!cmsoa_is_stack_state_valid())
    return;

  CM_MSG_HIGH_3("CMSOA: Process Domain Selection, csfb_type=%d, ims=%d, volte-12=%d",
              csfb_type, lte_ims_voice_availability, cmsoa_state->config.is_volte12);

  /* We only process this service indication when we acquire LTE full service
  */
  if (!(si_info_ptr->srv_status == SYS_SRV_STATUS_SRV))
  {
    CM_MSG_HIGH_2("CMSOA: not full service. bail out (mode=%d, srv_status=%d)",
                si_info_ptr->mode, si_info_ptr->srv_status);
    return;
  }

  if (ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    CM_MSG_HIGH_0("Skip prediction in MANUAL mode since device will be 3GPP only device");
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(si_info_ptr->mode == SD_MODE_LTE)
  {  

  /* If we support VOICE or we fall to CSFB - new state is CSFB */
  if (csfb_type ==   CM_CALL_CSFB_TYPE_1XCSFB ||
      csfb_type == CM_CALL_CSFB_TYPE_PPCSFB)
  {
    is_lte_voice_support = TRUE;
    new_sv_state = CMSOA_SV_OPRT_MODE_CSFB;
    cmsoa_state->is_svlte_predicted = TRUE;
  }
  /* If we support IMS/VoLTE (full VoLTE, not VoLTE-2012) then */
  else if (lte_ims_voice_availability == CMSDS_IMS_VOICE_AVAIL &&
           cmsoa_state->config.is_volte12 == FALSE)
  {
    is_lte_voice_support = TRUE;
    new_sv_state = CMSOA_SV_OPRT_MODE_CSFB;
    cmsoa_state->is_svlte_predicted = TRUE;
  }
  else
  {
    is_lte_voice_support = FALSE;
    new_sv_state = CMSOA_SV_OPRT_MODE_SVLTE;
  }

  /*
  **   Requirement: If UE has attached on LTE in SV mode and it is learnt that
  **      LTE does not support voice (no VoPS, no SIB-8, no full CS+PS)
  **
  **   a. If LTE PLMN-ID is in SV-LTE PLMN-ID list, UE shall camp on
  **      LTE in SV-LTE mode (ignore domain selection flag)
  **
  **   b. If LTE PLMN-ID is not listed in SV-LTE PLMN-ID list,
  **      UE shall follow domain selection behavior
  **
  **      For e.g:  If UE = voice-centric, Detach from LTE and disable LTE PLMN for X minutes
  **
  **   Requirement: If LTE supports voice, switch to CSFB
  */
  if (is_lte_voice_support == FALSE)
  {
    boolean plmn_found = cmsoa_is_last_plmn_in_svlte_plmn_list();

    CM_MSG_HIGH_1("CMSOA: LTE not support voice. PLMN found=%d",plmn_found);

    if (plmn_found)
    {
      /* New state to be SVLTE regardless of what MCC. Explained below:
      **  1) If we're in Korea (MCC says CSFB),
      **     and this plmn is supporting SVLTE, so switch to SVLTE.
      **
      **  2) If we're in U.S. (MCC says SVLTE), prediction should yields
      **     SVLTE already. This is like a protection.
      **
      */
      new_sv_state = CMSOA_SV_OPRT_MODE_SVLTE;
    }
    else
    {
      /* Even though we're in SVLTE MCC area, the PLMN which we camped
      ** did not support SVLTE, so we need to switch to CSFB
      */
      new_sv_state = CMSOA_SV_OPRT_MODE_CSFB;

      /* If current mode is in SVLTE (MCC indicates SVLTE)
      ** and now we're switching to CSFB
      */
      if (cmsoa_state->curr_sv_oprt_mode == CMSOA_SV_OPRT_MODE_SVLTE)
      {
        cmsoa_state->is_svlte_predicted = TRUE;
      }
    }
  }
  /* Requirement: If LTE supports voice, switch to CSFB */
  else
  {
    new_sv_state = CMSOA_SV_OPRT_MODE_CSFB;
  }
  }
  else if(si_info_ptr->mode == SD_MODE_GSM || si_info_ptr->mode == SD_MODE_WCDMA)
  {
    /* if current mode is SVLTE, then switch back to CSFB mode, as when GW service is camped onto,
    LTE will be scanned only in CSFB mode. So no point of staying in SV mode */
    if(cmsoa_state->curr_sv_oprt_mode == CMSOA_SV_OPRT_MODE_SVLTE)
    {
      new_sv_state = CMSOA_SV_OPRT_MODE_CSFB;
      cmsoa_state->is_svlte_predicted = TRUE;
    }    
  }

  /* Call switch base to handle the switch if we have decided what is the SV-oprt */
  cmsoa_sv_switch_base(new_sv_state, cm_ss,FALSE);

}
#endif

/*===========================================================================

FUNCTION cmsoa_regprx_srv_req_proceed_process

DESCRIPTION
  This function process the service-request from CMREGPRX - it seemly check
  if the current SV-OPRT is the same at the SV-OPRT at the moment of the
  request.
  a) If it the same - allow service request to go through.
  b) If it's not the same - meaning we have decided to switch SV-OPRT in the
     meanwhile, hence, block the service request.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_regprx_srv_req_proceed_process( cm_generic_rpt_u_type *regprx_msg)
{
  boolean is_srv_req_proceed = TRUE;
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* Check if we're not transition the SVLTE <-> CSFB state, then allow the
  ** srv_req
  */
  if (cmss_is_hybr2_operational() == regprx_msg->regprx_srv_req.hyrb2_on)
  {
    CM_MSG_HIGH_2("CMSOA: Allow Service Request - curr_svlte_state=%d, req_svlte_state=%d",
                cmss_is_hybr2_operational(), 
                regprx_msg->regprx_srv_req.hyrb2_on);
    is_srv_req_proceed = TRUE;
  }
  else
  {
    CM_MSG_HIGH_2("CMSOA: Block Service Request - curr_svlte_state=%d, req_svlte_state=%d",
                cmss_is_hybr2_operational, 
                regprx_msg->regprx_srv_req.hyrb2_on);
    is_srv_req_proceed = FALSE;
  }

  /* Send this messages to CMREGPRX so it could proceed with service request */
  cmregprx_cmd_srv_req_proceed(is_srv_req_proceed, (cm_ss_e_type)regprx_msg->regprx_srv_req.ss );

}


/***************************************************************************
** FUNCTIONS - SECTION 4: Utilities function
** This section contains ultilities functions such as get/set, timer, srch.
***************************************************************************/

/*===========================================================================

FUNCTION cmsoa_timer_proc()

DESCRIPTION
  This function process the timer event for CMSOA. Timer is used when we want
  to delay a switch due to either Voice Call or Ping-pong situation.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_timer_proc()
{
  cmsoa_state_info_s_type   *cmsoa_state = cmsoa_state_info_ptr();
  dword                      uptime  = time_get_uptime_secs();

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  if (!cmsoa_state->is_activated)
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If the timer expires */
  if (cmsoa_state->switch_delay_timer != 0 &&
      uptime >= cmsoa_state->switch_delay_timer &&
      cmsoa_state->switch_delay_state != CMSOA_SV_OPRT_MODE_NONE)
  {

    #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
    if (cmph_is_oprting_in_csfb_pref_srlte(SYS_MODEM_AS_ID_1) == FALSE)
    {
      cmsoa_process_delay_action( cmsoa_state->switch_delay_state);
    }
    else
    #endif
    if (cmsoa_state->switch_delay_state == CMSOA_SV_OPRT_MODE_CSFB)
    {
      #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
      cmregprx_stack_info_s_type *cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);
      /* only process delay action if there is LTE service on hybr2 and no RLF */
      if ( (cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->srv_status == SYS_SRV_STATUS_SRV &&
            cmregprx_stack_info_ptr->lte_connected_mode == FALSE &&
            cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->mode == SD_MODE_LTE) &&
           (cmsds_ptr()->csfb_status == CM_1XCSFB_PREREG_SUCCESS ||
            cmsds_ptr()->csfb_status == CM_1XCSFB_AVAIL || 
            cmsds_ptr()->ims_reg_status_voice[SYS_SYS_MODE_LTE] == SYS_IMS_REG_STATE_REGISTERED) ) 
      {
        cmsoa_process_delay_action( cmsoa_state->switch_delay_state);
      }
      else
      {
        /* reset the switch delay state */
        CM_MSG_HIGH_2("cmsoa_timer_proc: resetting switch delay state %d %d",
                      cmregprx_stack_info_ptr->lte_connected_mode,
                      cmsds_ptr()->csfb_status);
        cmsoa_state->switch_delay_state = CMSOA_SV_OPRT_MODE_NONE;

        /* reset CSFB status */
        if(cmsds_ptr()->csfb_status != CM_1XCSFB_NONE)
        {
          CM_MSG_HIGH_1("cmsoa_timer_proc: resetting csfb_status. Current status: %d",
                        cmsds_ptr()->csfb_status);
          cmsds_ptr()->csfb_status = CM_1XCSFB_NONE;
        }
      }
      #endif
    }
    else
    if (cmsoa_state->switch_delay_state == CMSOA_SV_OPRT_MODE_SVLTE)
    {
      boolean is_1xsrlte_mcc = FALSE;
      cmss_s_type  *ss_ptr = cmss_ptr();

      CM_MSG_HIGH_3("CMSOA: TIME_PROC: plmn 0x%x 0x%x 0x%x",
            ss_ptr->info.sys_id.id.plmn.identity[0],
            ss_ptr->info.sys_id.id.plmn.identity[1],
            ss_ptr->info.sys_id.id.plmn.identity[2]);
            
      if(policyman_svd_possible_on_plmn
           (&ss_ptr->info.sys_id.id.plmn, FALSE, &is_1xsrlte_mcc)
                    == POLICYMAN_STATUS_SUCCESS)
      {
        CM_MSG_HIGH_3("SRLTE: TIME_PROC: is_1xsrlte_plmn=%d, delay_state=%d, curr_sub_mode=%d", 
             is_1xsrlte_mcc, 
             cmsoa_state->switch_delay_state, 
             cmsoa_state->curr_sv_oprt_mode);
             
        if(is_1xsrlte_mcc == TRUE)
        {
          cmsoa_process_delay_action( cmsoa_state->switch_delay_state);
        }
        else
        {
          /* reset all timers */
          cmsoa_arm_timer(CMSOA_SV_OPRT_MODE_NONE,
                                      CMSOA_SV_SWITCH_DELAY_REAS_NONE);
        }
      }
      else
      {
        CM_ERR_0("SRLTE: fail to check policyman plmn !!!");
      }
    }
    else
    {
      /* reset all timers */
      cmsoa_arm_timer(CMSOA_SV_OPRT_MODE_NONE,
                                  CMSOA_SV_SWITCH_DELAY_REAS_NONE);
    }

  }

}

/*===========================================================================

FUNCTION cmsoa_process_delay_action()

DESCRIPTION
  This function process the delayed action due to either Voice Call or
  Ping-pong situation.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_process_delay_action( cmsoa_sv_oprt_mode_e_type   state )
{
  cmsoa_sv_switch_delay_reas_e_type delay_reas = CMSOA_SV_SWITCH_DELAY_REAS_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Basic checks */
  CM_MSG_HIGH_1("CMSOA: Process Delayed State =%d ",state);

  if (state == CMSOA_SV_OPRT_MODE_NONE)
  {
    CM_MSG_HIGH_0("CMSOA: No action taken!");
    return;
  }

  /* As timer expires, we check if we need to continue delaying.
  ** If yes, arm_timer again and exit.
  */
  delay_reas = cmsoa_sv_switch_is_delayed();

  /* if need to delay, arm the timer again */
  if (delay_reas == CMSOA_SV_SWITCH_DELAY_REAS_NONE)
  {
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get here: timer expired and action will be taken */
    switch (state)
  {
      case CMSOA_SV_OPRT_MODE_SVLTE:
    {
      /* Switching from CSFB --> SVLTE */
      cmsoa_sv_switch_csfb_svlte();
      break;
    }

      case CMSOA_SV_OPRT_MODE_CSFB:
    {
      /* Switching from SVLTE --> CSFB */
      cmsoa_sv_switch_svlte_csfb();
      break;
    }

    default:
    {
      CM_MSG_HIGH_0("CMSOA: invalid action");
    }
  }

  }
  /*no need to do anything for CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG ,
      timer is already running for that */
  else if(delay_reas != CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG)
  {
    cmsoa_arm_timer(state,delay_reas);
  }
  return;
}

/*===========================================================================

FUNCTION cmsoa_arm_timer()

DESCRIPTION
  This function set the timer for delay switch due to voice call.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_arm_timer(cmsoa_sv_oprt_mode_e_type state,
                          cmsoa_sv_switch_delay_reas_e_type  switch_delay_reason)
{
  dword                     uptime      = time_get_uptime_secs();
  cmsoa_state_info_s_type  *cmsoa_state = cmsoa_state_info_ptr();

  switch( switch_delay_reason )
  {
     /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
     case CMSOA_SV_SWITCH_DELAY_REAS_NONE:
       cmsoa_state->switch_delay_timer = 0;
       /* Clear attach complete timer as well since switch_delay_reas is
       ** being set to NONE
       */
       cmtask_attach_complete_timer_clear();
       break;

     /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
     case CMSOA_SV_SWITCH_DELAY_REAS_VOICE_CALL:
       cmsoa_state->switch_delay_timer = uptime +
           CMSOA_SWITCH_DELAY_REASON_VOICE_CALL_TIMER;
       break;

     /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
     case CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG:
       if(cmph_get_feature_mode() == SYS_OVERALL_FEATURE_MODE_SVLTE)
       {
         cmsoa_state->switch_delay_timer = uptime +
                CMSOA_SWITCH_DELAY_REASON_PINGPONG_TIMER;
       }
       else
       {
         cmsoa_state->switch_delay_timer = uptime +
             cmsoa_state->sxlte_timers.switch_pingpong_guard_timer;
       }
       break;
       
     #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
     /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
     case CMSOA_SV_SWITCH_DELAY_LTE_CONNECTED:
       if(cmph_is_oprting_in_csfb_pref_srlte(SYS_MODEM_AS_ID_1))
       {
         cmsoa_state->switch_delay_timer = uptime +
            cmsoa_state->sxlte_timers.lte_idle_timer;
       }
       break;

     case CMSOA_SV_SWITCH_DELAY_ATTACH_COMPLETE:
       cmtask_attach_complete_timer_start(cmsoa_state->attach_complete_timer);
       break;
     #endif
     /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
     default:
       CM_MSG_HIGH_1("CMSOA: Unknown delay reason %d ",switch_delay_reason);
       break;
  }

  if(switch_delay_reason == CMSOA_SV_SWITCH_DELAY_REAS_NONE
       && state != CMSOA_SV_OPRT_MODE_NONE)
  {
    CM_MSG_HIGH_1("CMSOA:unexpected action=%d",state);
    cmsoa_state->switch_delay_state = CMSOA_SV_OPRT_MODE_NONE;
  }
  else
  {
    cmsoa_state->switch_delay_state = state;
}

  cmsoa_state->switch_delay_reason = switch_delay_reason;


  CM_MSG_HIGH_3("CMSOA: Set delay time: switch_delay_reason=%d, timer =%d, state=%d",
                      switch_delay_reason, cmsoa_state->switch_delay_timer, state);
  return;
}

/*===========================================================================

FUNCTION cmsoa_attach_complete_rpt_proc()

DESCRIPTION
  This function performs the DELAYED SWITCH due to ATTACH COMPLETE IND.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmsoa_attach_complete_rpt_proc(boolean is_timer_exp)
{
  cmsoa_state_info_s_type   *cmsoa_state = cmsoa_state_info_ptr();

  if (cmph_is_oprting_in_csfb_pref_csfb_sub_mode(SYS_MODEM_AS_ID_1) &&
      (cmsoa_state->switch_delay_state == CMSOA_SV_OPRT_MODE_SVLTE) &&
      (cmsoa_state->switch_delay_reason == CMSOA_SV_SWITCH_DELAY_ATTACH_COMPLETE))
  {
    CM_MSG_HIGH_0("Perform the delayed SR switch");

    if(cmss_intl_srv_info_ptr(CM_SS_MAIN)->extend_srv_info == SYS_EXTEND_SRV_INFO_REG_COMP_ACK_PENDING)
    {
      cmss_intl_srv_info_ptr(CM_SS_MAIN)->extend_srv_info  = SYS_EXTEND_SRV_INFO_NONE;
      
      if (!is_timer_exp)
      {
        cmtask_attach_complete_timer_clear();
      }
      else
      {
        cmsoa_state->is_attach_complete_timer_active = FALSE;
      }
    }
    
    cmsoa_process_delay_action(cmsoa_state->switch_delay_state);
  }
  else if(cmss_intl_srv_info_ptr(CM_SS_MAIN)->extend_srv_info == SYS_EXTEND_SRV_INFO_REG_COMP_ACK_PENDING)
  {
    CM_MSG_MED_0("Reset extend_srv_info");
    cmss_intl_srv_info_ptr(CM_SS_MAIN)->extend_srv_info  = SYS_EXTEND_SRV_INFO_NONE;
  }
  
}

/*===========================================================================

FUNCTION cmsoa_is_prediction_allowed_in_srv_status()

DESCRIPTION
  self descriptive.

DEPENDENCIES
  none

RETURN VALUE
  true if prediction is allowed
  false otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmsoa_is_prediction_allowed_in_srv_status(sys_srv_status_e_type srv_status)
{
  switch(srv_status)
  {
   case SYS_SRV_STATUS_NO_SRV:
   case SYS_SRV_STATUS_NO_SRV_INTERNAL:
   case SYS_SRV_STATUS_PWR_SAVE:
   case SYS_SRV_STATUS_PWR_SAVE_INTERNAL:
        return TRUE;
   default:
        return FALSE;
  }
}
/*===========================================================================

FUNCTION cmsoa_is_prediction_allowed()

DESCRIPTION
  This function checks if we allow prediction. We don't allow prediction
  when:
  a) feature not enabled
  b) states not initialized
  c) we cannot move to other mode, for example, no subscriptions means UE
     cannot move to SVLTE. So legacy code will take care.
  d) we have already done the prediction and currently having service.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmsoa_is_prediction_allowed(
   sd_ss_e_type          ss
  /* Stores the stack of found system.
  */
)
{

  sys_srv_status_e_type          main_srv_status;
  sys_srv_status_e_type          hybr2_srv_status;
  boolean                        is_main_allow_prediction = FALSE;
  boolean                        is_hybr2_allow_prediction = FALSE;
  cmph_s_type                   *ph_ptr = cmph_ptr();

  /* stack state */
  cmmsc_stack_state_e_type main_state = cmmsimc_get_msc_stack_state(CM_SS_MAIN);
  cmmsc_stack_state_e_type hybr2_state = cmmsimc_get_msc_stack_state(CM_SS_HYBR_2);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */



  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If prediction happens on HYBR-2 stack while HYBR-2 is not operational,
  ** Skip this prediction.
  */
  if (!cmss_is_hybr2_operational() &&
      ss == SD_SS_HYBR_2)
  {
    return FALSE;
  }

  if (ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    CM_MSG_HIGH_0("Skip prediction in MANUAL mode since device will be 3GPP only device");
    return FALSE;
  }
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If MCC is found after we have got service, we should not act upon it
  ** since we only use it for prediction before having service (in OOS)
  */
  main_srv_status = cmss_intl_srv_info_ptr(CM_SS_MAIN)->srv_status;

  CM_MSG_HIGH_2("MMSOA: check prediction: main_srv=%d, main stack_state=%d",
              main_srv_status, main_state);

  /* Check main stack first -*/
  if ( cmsoa_is_prediction_allowed_in_srv_status(main_srv_status))
  {
    is_main_allow_prediction = TRUE;
  }

  /* In case main stack acquire 3GPP service first, then we need to also
  ** consider the pref_state machine
  */
  if ( main_srv_status == SYS_SRV_STATUS_LIMITED &&
       /* could possibly contains more state than below listed states */
       (!cmmsc_is_msc_multi_stacks(SYS_MODEM_AS_ID_1) ||
        !cmmsimc_is_in_srv(CM_SS_MAIN))
     )
  {
    is_main_allow_prediction = TRUE;
  }

  /* If main does not allow prediction, then we don't need to continue to check
  ** for hybr-2. Just return here.
  */
  if (is_main_allow_prediction == FALSE ||
      cmss_is_hybr2_operational() == FALSE)
  {
    return is_main_allow_prediction;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for hybr-2 */
  hybr2_srv_status = cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->srv_status;

  CM_MSG_HIGH_2("CMSOA: checking prediction hybr2_srv=%d, state=%d",
              hybr2_srv_status, hybr2_state);

  if ( cmsoa_is_prediction_allowed_in_srv_status(hybr2_srv_status))
  {
    is_hybr2_allow_prediction = TRUE;
  }

  /* If we have LIMITED service, and state indicating that we
  ** not yet having service.
  */
  if ( hybr2_srv_status == SYS_SRV_STATUS_LIMITED &&
     hybr2_state == CMMSC_STACK_STATE_ACTIVE)
  {
     is_hybr2_allow_prediction = TRUE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Return */

  /* Allow prediction to happens if both stacks are ok with it */
  if (is_main_allow_prediction &&
      is_hybr2_allow_prediction)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmsoa_get_current_sv_oprt_state()

DESCRIPTION
  This function return current sv_oprt state variable.

RETURN VALUE
  cmsoa_sv_oprt_mode_e_type

===========================================================================*/
cmsoa_sv_oprt_mode_e_type cmsoa_get_current_sv_oprt_state()
{
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  return cmsoa_state->curr_sv_oprt_mode;
}

/*===========================================================================
FUNCTION  cmsoa_is_oprting_in_csfb_pref_srlte

DESCRIPTION
Returns TRUE if UE is operating in 1XSRLTE-Dual mode .



DEPENDENCIES
 cmmsc_init should already be called


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsoa_is_oprting_in_csfb_pref_srlte(
    sys_modem_as_id_e_type         asubs_id
)
{
  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(asubs_id);
  boolean ret = FALSE;

  if(cmph_is_subs_feature_mode_1x_sxlte(asubs_id) &&
     state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED &&
     cmsoa_get_current_sv_oprt_state() == CMSOA_SV_OPRT_MODE_SVLTE)
  {
    ret = TRUE;
  }

  return ret;
}

/*===========================================================================

FUNCTION cmsoa_get_pref_update_reason()

DESCRIPTION
  This function returns pref_update_reason variable

RETURN VALUE
  sd_ss_pref_update_reason_e_type

===========================================================================*/
sd_ss_pref_update_reason_e_type cmsoa_get_pref_update_reason()
{
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();

  if (cmsoa_state == NULL)
  {
    return SD_SS_PREF_UPDATE_REASON_NONE;
  }

  return cmsoa_state->pref_update_reason;
}

/*===========================================================================

FUNCTION cmsoa_set_pref_update_reason()

DESCRIPTION
  This function set pref_update_reason variable

RETURN VALUE
  none

===========================================================================*/
void cmsoa_set_pref_update_reason
(
     sd_ss_pref_update_reason_e_type  reason
)
{
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();

  if (cmsoa_state == NULL)
  {
    return;
  }

  cmsoa_state->pref_update_reason = reason;
  return;
}

/*===========================================================================

FUNCTION cmsoa_is_last_plmn_in_svlte_plmn_list()

DESCRIPTION
  This function check if the last plmn which we have acquired belongs to the
  SVLTE-PLMN-ID list. It often used when we received service indication,
  we want to check if the FULL plmn is in the lists or not.

  Use case:
  1) Power-up and acquire MCC in SVLTE MCC list
  2) Acquire full-plmn that does not belongs to SVLTE-PLMN-ID lists.
  3) Decision: Switch() to CSFB even though we're in MCC belongs to SVLTE-MCC.

  Reason: It's possible that this plmn has not been fully tested with
  SVLTE capabailities.

RETURN VALUE
  none

===========================================================================*/
boolean cmsoa_is_last_plmn_in_svlte_plmn_list()
{
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  return cmsoa_svlte_plmn_list_search(&cmsoa_state->svlte_plmn_list,
                                      &cmsoa_state->last_gwl_plmn);
}

/*===========================================================================

FUNCTION cmsoa_activate_arbitration()

DESCRIPTION
  This function checks and activate the SV-Operation Arbitration module.

RETURN VALUE
  none

===========================================================================*/
void cmsoa_activate_arbitration()
{

  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();

  cmph_s_type             *ph_ptr      = cmph_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( cmsoa_state == NULL || ph_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If feature mode is not svlte, then don't activate */
  if (!cmph_is_subs_feature_mode_1x_sxlte(SYS_MODEM_AS_ID_1))
  {
    CM_MSG_HIGH_0("CMSOA: no svlte - deactivate");
    cmsoa_state->is_activated = FALSE;
    return;
  }

  /* If switch is disabled, then don't activate */
  if (!cmsoa_state->config.is_switch_enabled)
  {
    CM_MSG_HIGH_0("CMSOA: Switch is not enabled - deactivate");
    cmsoa_state->is_activated = FALSE;
    return;
  }

  /* If we are not ONLINE - then don't activate arbitration */
  if ( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
  {
    CM_MSG_HIGH_0("CMSOA: Phone not online - deactivate");
    cmsoa_state->is_activated = FALSE;
    return;
  }

  /* If we don't have 1X / GW subscription, then MMSS will not be enabled
  ** Hence we will stay in NON-SVLTE anyways. So don't activate the
  ** feature.
  */
  if (!( ph_ptr->is_cdma_subscription_available &&
         ph_ptr->is_gwl_subscription_available ))
  {
    CM_MSG_HIGH_0("CMSOA: SVLTE operation is not possible - deactivate ");
    cmsoa_state->is_activated = FALSE;
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If all the checks passed, activate arbitration */
  CM_MSG_HIGH_0("CMSOA: Activate SV-Operation Arbitration");
  cmsoa_state->is_activated = TRUE;

}

/*===========================================================================

FUNCTION cmsoa_is_arbitration_activate()

DESCRIPTION
  This function returns if arbitration is activated.

RETURN VALUE
  none

===========================================================================*/
boolean cmsoa_is_arbitration_activate()
{
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();

  if (cmsoa_state == NULL)
  {
    return FALSE;
  }

  return (cmsoa_state->is_activated);
}

/*===========================================================================

FUNCTION cmsoa_proc_acq_fail()

DESCRIPTION
  This function process acq_fail messages.

RETURN VALUE
  none

===========================================================================*/
void cmsoa_proc_acq_fail(sd_ss_e_type ss)
{

  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (ss)
  {
    case SD_SS_MAIN:
    {
      CM_MSG_HIGH_0("CMSOA: acq_fail, clr svlte_predicted flag");
      cmsoa_state->is_svlte_predicted = FALSE;
      break;
    }

    default:
       break;
  }
}


/*===========================================================================

FUNCTION cmsoa_msgr_rpt_proc

DESCRIPTION
  Process reports from the LTE lower layers.

DEPENDENCIES
  Serving System object must have already been initialized with
  cmss_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsoa_msgr_rpt_proc(

  const void          *rpt_ptr
    /* Pointer to the report sent by Message Router */
)
{
  cmsoa_state_info_s_type   *cmsoa_state = cmsoa_state_info_ptr();

  if (cmsoa_state == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  if (!cmsoa_state->is_activated)
  {
    return;
  }

#if defined( FEATURE_LTE_TO_1X )
  /* If the timer expires */
  if (cmsoa_state->switch_delay_timer != 0 &&
      cmsoa_state->switch_delay_state != CMSOA_SV_OPRT_MODE_NONE &&
      cmsoa_state->switch_delay_reason == CMSOA_SV_SWITCH_DELAY_LTE_CONNECTED)
  {
    if (cmsoa_state->switch_delay_state == CMSOA_SV_OPRT_MODE_CSFB)
    {
      cmregprx_stack_info_s_type *cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);
      /* only process delay action if there is LTE service on hybr2 and no RLF */
      if (cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->srv_status == SYS_SRV_STATUS_SRV &&
          cmregprx_stack_info_ptr->lte_connected_mode == FALSE &&
          cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->mode == SD_MODE_LTE &&
           (cmsds_ptr()->csfb_status == CM_1XCSFB_PREREG_SUCCESS ||
            cmsds_ptr()->csfb_status == CM_1XCSFB_AVAIL || 
            cmsds_ptr()->ims_reg_status_voice[SYS_SYS_MODE_LTE] == SYS_IMS_REG_STATE_REGISTERED) )
      {
        cmsoa_process_delay_action( cmsoa_state->switch_delay_state);
      }
      else
      {
        /* reset the switch delay state  */
        CM_MSG_HIGH_2("cmsoa_msgr_rpt_proc: resetting switch delay state %d %d",
                      cmregprx_stack_info_ptr->lte_connected_mode,
                      cmsds_ptr()->csfb_status);
        cmsoa_state->switch_delay_state = CMSOA_SV_OPRT_MODE_NONE;

        /* reset CSFB status */
        if(cmsds_ptr()->csfb_status != CM_1XCSFB_NONE)
        {
          CM_MSG_HIGH_1("cmsoa_msgr_rpt_proc: resetting csfb_status. Current status: %d",
                        cmsds_ptr()->csfb_status);
          cmsds_ptr()->csfb_status = CM_1XCSFB_NONE;
        }
      }
    }
  }
#endif
}
/*===========================================================================

FUNCTION cmsoa_check_and_switch_fromc2k_to_srlte

DESCRIPTION
  This function performs all neccessary checks before it decides to whether
  switching to SRLTE operation.
  If a switch() operation is required, it will call to another function that
  will only performs the switch.
 
  conditions to be met are the following 
       If a)is_c2k_modeswitch2srlte = TRUE && UE is capable of SRLTE operation
      ** b) SD reports stable 1x service on MAIN 
      ** c) UE mode on MAIN is CSFB preferred 
      ** d) UE is currently operating in single stack -CSFB mode
      ** e) UE is not in a 1xcsfb call and is not 1xcsfb capable 
      ** and f)1x MCC is part of Home PLMN list
      

DEPENDENCIES  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmsoa_check_and_switch_fromc2k_to_srlte(cm_ss_e_type ss)
{
  sys_modem_as_id_e_type     as_id              = cmph_map_cm_ss_to_subs(ss);
  cmss_s_type                *ss_ptr            = cmss_ptr();
  cmph_s_type                *ph_ptr            = cmph_ptr();
  cmsoa_state_info_s_type    *cmsoa_state       = cmsoa_state_info_ptr();
  boolean                    is_home_mcc        = FALSE;
  sys_plmn_id_s_type         plmn;
  boolean                    ret                = FALSE;
  uint8                      count              = 0;
  sd_si_info_s_type         *si_info_ptr        = NULL;
  
  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(as_id);
  sys_ue_mode_e_type curr_ue_mode = state_machine->stack_common_info.ue_mode;

  if (!cmph_is_subs_feature_mode_srlte(as_id)) 
  {
     CM_MSG_MED_0("c2kswitch: Not SRLTE mode");
     return FALSE ;
  }
  if (!cmsoa_state->is_c2k_modeswitch2srlte) 
  {
     CM_MSG_MED_0("is_c2k_modeswitch2srlte disabled");
     return FALSE ;
  }

  if (cmsds_get_config_ptr(as_id)->ue_usage_setting == SYS_UE_USAGE_SETTING_DATA_CENTRIC )
  {
     CM_MSG_HIGH_0("c2kswitch:Do not perform switch in data centric device");
     return FALSE ;
  }

  /* Check if SRLTE operation is possible and if UE 
  ** is currently in CSFB submode of CSFB preferred UE mode 
  */
  if (!(curr_ue_mode                   == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED && 
        cmsoa_state->curr_sv_oprt_mode == CMSOA_SV_OPRT_MODE_CSFB &&
        cmss_is_hybr2_operational()    == FALSE &&
        cmsoa_is_srlte_capability(as_id)
        )
      ) 
  {
    CM_MSG_MED_3("c2kswitch:Not in single stack csfb preferred mode: main mode_pref=%d,hybr2_op=%d, ue_mode=%d", 
                 CMPH_SS_MODE_PREF(as_id,ph_ptr),
                 cmss_is_hybr2_operational(),
                 curr_ue_mode);

     
     return FALSE ;
  }

  /* In non-MMSS configuration, we do not need to switch to dual stack mode
  ** since LTE will be in OOS timeline and  might unnecessarily stop HDR stack 
  */
  if(sd_misc_is_mmss_operation(ss) == FALSE)
  {
    CM_MSG_MED_0("c2kswitch:Not MMSS config");
    return FALSE ;
  }

  if (cmph_is_in_emergency_cb())
  {
    CM_MSG_MED_0("c2kswitch:skip switch due to ECBM mode");
    return FALSE ;
  }
  
  si_info_ptr = cmss_intl_srv_info_ptr(CM_SS_MAIN);

  if (si_info_ptr == NULL)
  {
    CM_MSG_MED_0("c2kswitch:si_ptr NULL");
    return FALSE ;
  }
  /* Check if UE is on 1x full service and immediate BSR is complete */
  if (!( sys_srv_status_is_srv(si_info_ptr->srv_status) &&
         si_info_ptr->mode            == SYS_SYS_MODE_CDMA &&
         si_info_ptr->is_pref_srv_acq == FALSE 
       )
     )
  {
    CM_MSG_MED_0("c2kswitch:No stable 1x service on MAIN");
    return FALSE ;
  }

 /* During the DDS change from SUB2 to SUB1, PS_DETACH will happoen on SUB2 and on SUB1, UE mode will be set to CSFB_PREF.
 If DETACH takes lojng time on SUB2 and if there is a 1X servcie indication, it will cause silent SRLTE switch as top of the queue is phone object and
 UE MODE is already CSFB_PREF. So skip the c2k switch till PS_DETACH is completed and 1X + L preferences are forced to SD.Switch to SRLTE should be
 performed only after the DDS preferences are forced on to SUB1(1X + L) and after IBSR */
 if ((cmregprx_get_stack_info_ptr(SD_SS_HYBR_3)->substate == CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF) &&
     (ph_ptr->default_data_subs == SYS_MODEM_AS_ID_1))
 {
   CM_MSG_MED_0("c2kswitch:PS DETACH is in progress on SUB2. So skip the c2k based switch on SUB1");
   return FALSE ;
 }


  /* Check if UE is 1xcsfb capable or is in a 1xcsfb call
  */
  #ifdef FEATURE_LTE_TO_1X
  if (cmsds_is_in_1xcsfb_mode())
  {
    CM_MSG_HIGH_0("c2kswitch:1xcsfb activated");
    return FALSE ;
  }
  /* check for a 1xcsfb call in the call obj array
  */
  {
    cm_call_id_type  call_id = CM_CALL_ID_INVALID;
    call_id =  cmcall_is_there_csfb_call(CM_SS_MAIN,
                                         CM_CALL_CSFB_TYPE_1XCSFB,
                                         CM_CALL_ID_INVALID);
    if (call_id != CM_CALL_ID_INVALID)
    {
      CM_MSG_HIGH_1("c2kswitch:1xcsfb call active %d",call_id);
      return FALSE ;
    }
  }
  /* check for a 1xcsfb call in the priority Q
  */
  {
    cm_orig_q_s_type *ph_orig_ptr = NULL;
    if ((cmtask_orig_para_search_orig_mode(CM_SS_MAIN, SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG) != NULL) ||
        (cmtask_orig_para_search_orig_mode(CM_SS_MAIN, SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG) != NULL)
       )
    {
      CM_MSG_HIGH_0("c2kswitch:1xcsfb obj in pri Q");
      return FALSE ;
    }
  }
  #endif //FEATURE_LTE_TO_1X


 /* If we got here conditions a-e are met. Check for HOME mcc with PM.
  ** If for some reason MCC check cannot be done, assume home mcc
  */

  /* Get 1x MCC value */
  if (ss_ptr->info.prl_3gpp2_mcc.count == 0) 
  {
    CM_MSG_MED_0("c2kswitch:No valid MCC value");
    is_home_mcc = TRUE;
  }
  else
  {
    /* set plmn =  mcc + wildcard mnc 
    */
    for (count = 0; count < ss_ptr->info.prl_3gpp2_mcc.count; count++)
    {
      CM_MSG_MED_1("c2kswitch:MCC %d",ss_ptr->info.prl_3gpp2_mcc.mcc_list[count] );
      ret = sys_plmn_set_mcc_mnc2(TRUE,ss_ptr->info.prl_3gpp2_mcc.mcc_list[count] ,SYS_WILDCARD_MNC,&plmn);
      if(ret == FALSE)
      {
        CM_MSG_MED_0("c2kswitch:plmn conversion failed");
        continue;
      }
      else
      {
        CM_MSG_HIGH_3("c2kswitch: plmn 0x%x 0x%x 0x%x",
                       plmn.identity[0],
                       plmn.identity[1],
                       plmn.identity[2]);
        if(policyman_svd_possible_on_plmn(&plmn, TRUE,&is_home_mcc) !=
                                      POLICYMAN_STATUS_SUCCESS)
        {
          CM_MSG_HIGH_0("c2kswitch:PM check incomplete");
          continue;       
        }
        if (is_home_mcc) 
        {
          CM_MSG_MED_0("c2kswitch:PM check -home MCC");
          break;
        }
      }
    }
  }
  if (is_home_mcc) 
  {
    /* Check if the top of the MAIN priority Q is the ph object
    */
    cm_orig_q_s_type    *q_top_ptr          = NULL;

    q_top_ptr = cmtask_orig_para_get_top( CM_SS_MAIN );
   
    CM_MSG_HIGH_1("c2kswitch:is_lte_Scanned value :%d", si_info_ptr->is_lte_scanned );
    /* If the top is not the phone object or SD indicates that lte has not been 
    ** scanned even once, use reason ACTIVATE_HYBR2 for the switch.
    ** This will force LTE scan to happen. If the top is the Phone object, assume that 
    ** full scan of LTE has already happened on MAIN and activate HYBR2 with reason 
    ** ACTIVATE_HYBR2_SILENT 
    */
    if( (q_top_ptr != NULL &&
         q_top_ptr->orig->act_priority != CM_ACT_PRIORITY_PH) ||
        (cmtask_orig_para_search_act_type( CM_SS_MAIN,CM_ACT_TYPE_DATA_CALL) != NULL) ||
        (cmtask_orig_para_search_act_type( CM_SS_HYBR_1,CM_ACT_TYPE_DATA_CALL) != NULL) ||
        (si_info_ptr->is_lte_scanned == FALSE)
      )
    {
      cmsoa_set_pref_update_reason( SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2 );
    }
    else
    {
      cmsoa_set_pref_update_reason( SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2_SILENT );
    }
    CM_MSG_HIGH_0("c2kswitch:Switch to SRLTE");
    cmsoa_sv_switch_base(CMSOA_SV_OPRT_MODE_SVLTE, CM_SS_MAIN,FALSE );
    #if defined(FEATURE_LTE_TO_1X)
      if(cmsds_ptr()->csfb_status != CM_1XCSFB_NONE)
      {
        CM_MSG_HIGH_1("Resetting csfb_status, Current status: %d", cmsds_ptr()->csfb_status);
        cmsds_ptr()->csfb_status = CM_1XCSFB_NONE;
      }
    #endif
  }

  return TRUE;
}

/*===========================================================================
  This function sets CMSOA to perform only in CSFB mode. Under no network
  conditions will phone switch to SRLTE mode.
===========================================================================*/

static void cmsoa_srlte_ue_mode_csfb(void)
{
  cmsoa_state_info_s_type   *cmsoa_state = cmsoa_state_info_ptr();
  
  cmsoa_state->curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;
  cmsoa_state->config.curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_CSFB;
  cmsoa_state->config.is_switch_enabled = FALSE;
  
  cmph_ptr()->main_stack_info.pref_info.hybr_pref =
    cmph_ptr()->prst_main_stack_info[cmph_ptr()->curr_nam].hybr_pref =
                               CM_HYBR_PREF_CDMA__HDR;
}

/*===========================================================================
  This function sets CMSOA to be in CSFB and if no 1xCSFB is supported on LTE
  then modem will try to switch to SRLTE to have 1x and LTE in standby state.
  Privided if that PLMN is configured to support SRLTE in XML.
===========================================================================*/

static void cmsoa_srlte_ue_mode_csfb_pref(void)
{
  cmsoa_state_info_s_type   *cmsoa_state = cmsoa_state_info_ptr();
  /* the switch will only be true first time when ue powers up
  ** in CSFB preferred mode. curr sub mode and hybr pref
  ** would have been set in cmsoa_init()
  */
  if(cmsoa_state->config.is_switch_enabled == FALSE)
  {
    cmsoa_srlte_ue_mode_csfb();
    cmsoa_state->config.is_switch_enabled = TRUE;
    cmsoa_check_and_switch_fromc2k_to_srlte(CM_SS_MAIN);
  }
}

/*===========================================================================
  This function sets in hybr capability to enable SRLTE operation. Since
  modem operates always in SRLTE, CMSOA (dynamic SRLTE<->CSFB) is disabled.
===========================================================================*/

static void cmsoa_srlte_ue_mode_srlte_only(void)
{
  cmsoa_state_info_s_type   *cmsoa_state = cmsoa_state_info_ptr();

  cmsoa_state->curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_SVLTE;
  cmsoa_state->config.curr_sv_oprt_mode = CMSOA_SV_OPRT_MODE_SVLTE;
  cmsoa_state->config.is_switch_enabled = FALSE;
  
  cmph_ptr()->main_stack_info.pref_info.hybr_pref =
    cmph_ptr()->prst_main_stack_info[cmph_ptr()->curr_nam].hybr_pref =
                               CM_HYBR_PREF_CDMA__LTE__HDR;
}

/*===========================================================================
  This function helps MSC in changing the UE-mode. When UE-mode changes
  CMSOA may be in between its switch procedures, may be running timers or
  have deferred the switch procedure. In any CMSOA state, if UE-mode changes
  then whether modem is in SRLTE or CSFB is decided by UE-mode. So Dynamic-
  switch module is initialized to support that UE-mode as its done in
  cmph_config_hybr_pref & cmsoa_init.
===========================================================================*/

void cmsoa_srlte_ue_mode_update(sys_ue_mode_e_type ue_mode)
{
  cmsoa_state_info_s_type   *cmsoa_state = cmsoa_state_info_ptr();

  CM_MSG_HIGH_1("proc ue_mode change to %d", ue_mode);

  switch(ue_mode)
  {
    case SYS_UE_MODE_CSFB:
      cmsoa_srlte_ue_mode_csfb();
      break;
    case SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED:
      cmsoa_srlte_ue_mode_csfb_pref();
      break;
    case SYS_UE_MODE_1XSRLTE_ONLY:
      cmsoa_srlte_ue_mode_srlte_only();
      break;
    default:
      CM_MSG_HIGH_1("ue_mode %d have no impact on cmsoa state", ue_mode);
      return;
  }

  /* Reset, similar to cmsoa_init values */
  cmsoa_state->switch_delay_timer = 0;
  cmsoa_state->switch_delay_reason = CMSOA_SV_SWITCH_DELAY_REAS_NONE;
  cmsoa_state->switch_delay_state = CMSOA_SV_OPRT_MODE_NONE;
  /* Clear attach complete timer as well since switch_delay_reas is
  ** being set to NONE
  */
  cmtask_attach_complete_timer_clear();  
  
  cmsoa_state->pref_update_reason = SD_SS_PREF_UPDATE_REASON_NONE;
  cmsoa_state->is_svlte_predicted = FALSE;

  cmsoa_activate_arbitration();
}

/*===========================================================================

FUNCTION cmsoa_skip_dom_sel_due_to_pending_ue_mode()

DESCRIPTION
  This function tells if Domain selection logis of disablign LTE
  should be skipped, as SOA can take care of it.
SRLTE -
  1. Hybr-2 is on
  2. Hybr-2 is off, but PLMN belongs to PLMN white list, LTE full service has been reported.
RETURN VALUE
  none

===========================================================================*/
boolean cmsoa_skip_dom_sel_due_to_pending_ue_mode(void)
{

  boolean ret_val = FALSE;
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
  //const sd_si_info_s_type    *si_info_ptr = sd_si_info_ptr_get();
  sd_si_info_s_type  *intl_srv_info_ptr =  cmss_intl_srv_info_ptr(CM_SS_MAIN);
  sys_plmn_id_s_type plmn = intl_srv_info_ptr->sid.plmn.plmn_id;
  cmmsc_state_machine_s_type  *state_machine      = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);
  cmss_s_type    *ss_ptr   = cmss_ptr();

  CM_MSG_MED_3( "CMSOA: plmn to be checked [%x][%x][%x]",plmn.identity[0],
              plmn.identity[1],plmn.identity[2]);
  if(cmph_is_subs_feature_mode_srlte(SYS_MODEM_AS_ID_1) &&
     cmsoa_state->curr_sv_oprt_mode == CMSOA_SV_OPRT_MODE_CSFB &&
     intl_srv_info_ptr->mode == SYS_SYS_MODE_LTE  && 
     intl_srv_info_ptr->srv_status == SYS_SRV_STATUS_SRV &&
     state_machine->stack_common_info.ue_mode == SYS_UE_MODE_CSFB &&
     cmsoa_is_srlte_capability(SYS_MODEM_AS_ID_1))
  {
        if (policyman_svd_possible_on_plmn(&ss_ptr->info.sys_id.id.plmn, 
                                       FALSE, &ret_val ) !=
                                       POLICYMAN_STATUS_SUCCESS)
        {
          CM_MSG_HIGH_0("SRLTE: fail to check policyman plmn - WARNING!!!");
        }
  }
  return ret_val;  
}


/*===========================================================================

FUNCTION cmsoa_skip_dom_sel_due_to_soa()

DESCRIPTION
  This function tells if Domain selection logis of disablign LTE
  should be skipped, as SOA can take care of it.
  1. If in SV mode, Hybr-2 is on
  2. if in Non-SV mode, but PLMN belongs to SV-PLMN list, LTE full service has been reported.

RETURN VALUE
  none

===========================================================================*/
boolean cmsoa_skip_dom_sel_due_to_soa(void)
{

  boolean ret_val = FALSE;
  cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
  //const sd_si_info_s_type    *si_info_ptr = sd_si_info_ptr_get();
  sd_si_info_s_type  *intl_srv_info_ptr =  cmss_intl_srv_info_ptr(CM_SS_MAIN);
  sys_plmn_id_s_type plmn = intl_srv_info_ptr->sid.plmn.plmn_id;

  if(cmsoa_state->is_activated)
  {
    CM_MSG_MED_3( "CMSOA: plmn to be checked [%x][%x][%x]",plmn.identity[0],
                plmn.identity[1],plmn.identity[2]);
    if (cmss_is_hybr2_operational())
    {
      ret_val = TRUE;
    }
    else if( (cmsoa_state->curr_sv_oprt_mode == CMSOA_SV_OPRT_MODE_CSFB) &&
             ( intl_srv_info_ptr->mode == SYS_SYS_MODE_LTE ) && 
             ( intl_srv_info_ptr->srv_status == SYS_SRV_STATUS_SRV )&&
             ( cmsoa_svlte_plmn_list_search(&cmsoa_state->svlte_plmn_list,
                                          &plmn))
           )
      {
        ret_val = TRUE;
      }
    }
  return ret_val;  
}


/*===========================================================================

FUNCTION cmsoa_sd_rpt_proc

DESCRIPTION
  Process System Determination reports


DEPENDENCIES
  CMSOA object must have already been initialized with
  cmsoa_init().
 
  cmss_sd_rpt_proc() should be invoked before calling cmsoa_sd_rpt_proc()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsoa_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD reports */
)
{

  /*lint -e{826} */
  const cm_sd_rpt_u_type         *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
    /* Pointer to SD reports */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sd_rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( sd_rpt_ptr->hdr.cmd )
  {

    /* SRV indication
    */
    case CM_SRV_IND_INFO_F:

      /* If a)is_c2k_modeswitch2srlte = TRUE
      ** b) SD reports stable 1x service on MAIN 
      ** c) UE mode on MAIN is CSFB preferred 
      ** d) UE is currently operating in single stack -CSFB mode 
      ** and e)1x MCC is part of Home PLMN list
      */
      if ( sd_rpt_ptr->srv_ind_info.ss == SD_SS_MAIN ) 
      {
        cmsoa_check_and_switch_fromc2k_to_srlte(CM_SS_MAIN);
      }
      break;
 

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } /* switch( sd_rpt_ptr->hdr.cmd ) */

} //cmsoa_sd_rpt_proc

#endif /* defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE) */
