/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
             C M    M U L T I - S T A C K    C O N T R O L L E R

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file implements the CM multi-stack architecture to support multi-stacks
  requirements such as SVLTE, DualSim, SGLTE, etc.

EXTERNALIZED FUNCTIONS
  Functional Interface:



Copyright (c) 2013 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmsc.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/18/14   ss      Added function to check is SS valid for PS detach 
01/30/14   ss      Conditional Reset of ue_mode_forced flag 
01/02/14   ss      Check for on-going voice or emergency calls before forcing 
                   user prefs down to MMoC in SGLTE 
11/14/13   ss      Add check to not send UE MODE seicth addl action for pref
                   reas restore
10/07/13   jvo     Replace FEATURE_UICC_RAT_INDICATOR_SUPPORT with
                   is_rat_bal flag
08/13/13   cl      CM supports explicit PS_DETACH when capability from PM changed
07/22/13   fj      Add support for DSDX L+G
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
07/03/13   dk      KW error fixed in cmmsc_get_policy_mode_band_cap,
                   cmmsc_action_local_deact, cmmsc_send_sys_sel_pref_chgd_cmd
06/19/13   gm      Send pref to SD-ALL stacks before ONLINE/PWRUP ind
02/26/13   cl      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */

#include "comdef.h"   /* Definition for basic types and macros */
#include "cmph.h"     /* Interface to CM ph functions */
#include "cmmsc.h"
#include "cmmsc_sglte.h"
#include "cmmsc_auto.h"
#include "cmsimcoord.h"
#include "cmsoa.h"
#include "cmtaski.h"
#include "cmsds.h"
#include "mmoc_v.h"
#include "modem_mem.h"
#include "sys.h"      /* Common types. */
#include "rfm_device_config_type.h"
#include "sd_v.h"

#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)
#include "cmregprxi.h"
#include "cmss.h"
#endif

#include "cmpmprx.h"
#include "cmemgext.h"
#include "cmaccessctrl.h"
#include "cmdbg.h"

#ifdef CM_DEBUG
#error code not present
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


//static cmmsc_state_machine_s_type cmmsc_state_machine;

static cmmsc_routing_table_s_type sglte_route_table;
static cmmsc_routing_table_s_type auto_route_table;

/*===========================================================================

            FUNCTION DECLARATIONS AND IMPLEMENTATION

This section contains function declaration and implementation for this
feature.

===========================================================================*/

/* Static functions
*/
static void cmmsc_action_power_down(int stack_id,
                                    cmmsc_state_machine_s_type *state_machine);
static void cmmsc_action_sub(
       const cmmsc_event_sub_chg_s_type *evt_pkg
);
static void cmmsc_init_mode_and_capability (
       cmmsc_state_machine_s_type  *state_machine
);


static void cmmsc_get_stack_pref_per_cap (
       cmmsc_state_machine_s_type    *state_machine,
       int                            stack_idx,
       cm_preference_info_s_type     *pref_info,
       cmmsc_stack_capability_s_type *common_cap_ptr
);
static void cmmsc_get_user_mode_pref_per_cap (
       cmmsc_state_machine_s_type    *state_machine,
       int                            stack_idx,
       sd_ss_mode_pref_e_type        *user_mode_pref
);
static void  cmmsc_print_route_table_info(
    cmmsc_state_machine_s_type *state_machine,
    cmmsc_event_e_type  event,
    int                 route_tbl_idx
);
static boolean cmmsc_is_op_mode_multi_stacks (
  cmmsc_state_machine_op_mode_e_type op_mode
);
static boolean cmmsc_force_all_stack_required (
  cmmsc_state_machine_op_mode_e_type prev_op_mode,
  cmmsc_state_machine_op_mode_e_type new_op_mode
);
static cm_mode_band_pref_s_type  cmmsc_current_stack_preference(
  cmmsc_state_machine_s_type  *state_machine
);
static cmmsc_event_e_type cmmsc_get_route_tbl_event(
       cmmsc_routing_table_s_type *route_table,
       int route_idx
);
static cmmsc_state_machine_op_mode_e_type cmmsc_get_route_tbl_mode(
       cmmsc_routing_table_s_type *route_table,
       int route_idx
);
static cmmsc_stack_state_e_type cmmsc_get_route_tbl_stack_state(
       cmmsc_routing_table_s_type *route_table,
       int stack_idx,
       int route_idx
);
static cmmsc_stack_state_e_type cmmsc_get_route_tbl_stack_new_state(
       cmmsc_routing_table_s_type *route_table,
       int stack_idx,
       int route_idx
);
static cmmsc_action_s_type *cmmsc_get_route_tbl_action(
       cmmsc_routing_table_s_type *route_table,
       int stack_idx,
       int route_idx
);
static boolean cmmsc_is_volte_active_on_hybr2 (
  cmmsc_state_machine_s_type  *state_machine  
);

static sys_lte_band_mask_e_type cmmsc_read_svlte_capable_lte_bands(void);

static void cmmsc_action_suspend_stack(int stack_id,
                                    cmmsc_state_machine_s_type *state_machine);

static void cmmsc_action_resume_stack(int stack_id,
                                    cmmsc_state_machine_s_type *state_machine);

/****************************************************************************
**    Function definitions
****************************************************************************/
/*===========================================================================

FUNCTION cmmsc_state_machine_ptr

DESCRIPTION
  This function returns the CMMSC State Machine Pointer which is a static
  variable.

DEPENDENCIES
  None.

RETURN VALUE
  cmmsc_state_machine_s_type*

===========================================================================*/
//cmmsc_state_machine_s_type *cmmsc_state_machine_ptr( void )
//{
//  return &cmmsc_state_machine;
//}

/*===========================================================================

FUNCTION cmmsc_route_table_ptr

DESCRIPTION
  This function returns the a pointer to the entry of the Routing Table

DEPENDENCIES
  None.

RETURN VALUE
  cmmsc_routing_table_s_type*

===========================================================================*/
cmmsc_routing_table_s_type *cmmsc_route_table_ptr( cmmsc_routing_table_e_type table_id )
{
  sys_overall_feature_t  feature_mode = cmph_get_feature_mode();

  if (table_id != CMMSC_ROUTE_TABLE_NONE)
  {
    switch (table_id)
    {
      case CMMSC_ROUTE_TABLE_AUTO:
        return &auto_route_table;

      case CMMSC_ROUTE_TABLE_SGLTE:
        return &sglte_route_table;

      default:
        sys_err_fatal_invalid_value_exception((uint8)table_id);
        return NULL;
    }
  }

  /* if Route-ID is NONE, we fallback using feature set. This fallback could be removed
  ** in the future. */
  switch(feature_mode)
  {
    case SYS_OVERALL_FEATURE_MODE_SGLTE:
      return &sglte_route_table;

    case SYS_OVERALL_FEATURE_MODE_NORMAL:
    case SYS_OVERALL_FEATURE_MODE_SVLTE:
    case SYS_OVERALL_FEATURE_MODE_SRLTE:
      return &auto_route_table;

    /* if it's SGLTE+G, we might need to use route_id */
    default:
      sys_err_fatal_invalid_value_exception((uint8)feature_mode);
      return NULL;
  }
}

/*===========================================================================

FUNCTION cmmsc_get_curr_msc_ue_mode

DESCRIPTION
  This function gets the current ue mode stored in msc.

DEPENDENCIES
  None.

RETURN VALUE
  sys_ue_mode_e_type.

===========================================================================*/
sys_ue_mode_e_type  cmmsc_get_curr_msc_ue_mode (
    cmmsc_state_machine_s_type  *state_machine
)
{
  return state_machine->stack_common_info.ue_mode;
}

/*===========================================================================

FUNCTION cmmsc_get_curr_msc_max_cap

DESCRIPTION
  This function gets the current max capability stored in msc.

DEPENDENCIES
  None.

RETURN VALUE
  sys_sys_mode_mask_e_type.

===========================================================================*/
sys_sys_mode_mask_e_type  cmmsc_get_curr_msc_max_cap (
  sys_modem_as_id_e_type as_id
)
{
  if (!(as_id > SYS_MODEM_AS_ID_NONE && as_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    CM_MSG_HIGH_1("Warning: Invalid as_id %d", as_id);
    return SYS_SYS_MODE_MASK_NONE;
  }
  else
  {
    cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(as_id);
    return state_machine->stack_common_info.ue_max_mode_capability;
  }
}

/*===========================================================================

FUNCTION cmmsc_state_machine_op_mode_update

DESCRIPTION
  This function update the state machine operation mode (SG,non-SG,CSFB, etc.)

DEPENDENCIES
  None.

RETURN VALUE
  NONE
===========================================================================*/
void cmmsc_state_machine_op_mode_update(

   cmmsc_state_machine_s_type          *state_machine,
   /* Pointer to the state machines that needs to be updated */

   cmmsc_state_machine_op_mode_e_type   new_mode
   /* Update the current operating mode to new mode */
)
{
  if(state_machine == NULL)
    sys_err_fatal_null_ptr_exception();

  if(state_machine->op_mode != new_mode)
  {
    #ifdef FEATURE_CM_DEBUG_BUFFER
    cm_op_mode_change_entry_s_type op_mode_changed_entry;
    op_mode_changed_entry.as_id          = state_machine->asubs_id;
    op_mode_changed_entry.old_op_mode    = state_machine->op_mode;
    op_mode_changed_entry.new_op_mode    = new_mode;
    cmdbg_add_to_dbg_buffer(CM_BUFF_OP_MODE_CHANGE, 0, ((void*)&op_mode_changed_entry));
    #endif

    CM_MSG_HIGH_3("MSC: op_mode_chgd: updating op_mode new %d, old %d for as_id %d",
                new_mode, state_machine->op_mode, state_machine->asubs_id);
    state_machine->op_mode = new_mode;

    /* OP mode change, if SXLTE and sub feature mode is SVLTE / SGLTE inform
       ** MCS of new simul_cap */

        cmph_sp_subs_info_update();

      
  }
}


/*===========================================================================

FUNCTION cmmsc_compare_state_machine_op_mode

DESCRIPTION
  This function compares two op_modes. Op_Mode are enums and is not always
  'the same' to be equal.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if equals
===========================================================================*/
boolean cmmsc_compare_state_machine_op_mode(
   cmmsc_state_machine_op_mode_e_type   mode1,

   cmmsc_state_machine_op_mode_e_type   mode2
)
{
  if(mode1 == mode2)
  {
    return TRUE;
  }

  if(mode1 == CMMSC_OP_MODE_ANY ||
     mode2 == CMMSC_OP_MODE_ANY )
  {
    return TRUE;
  }

  if(mode2 == CMMSC_OP_MODE_AUTO_SVLTE)
  {
    return ((mode1 == CMMSC_OP_MODE_AUTO_MAIN_HYBR1) ||
           (mode1 == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2));
  }

  if(mode1 == CMMSC_OP_MODE_AUTO_SVLTE)
  {
    return ((mode2 == CMMSC_OP_MODE_AUTO_MAIN_HYBR1) ||
           (mode2 == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2));
  }

  /* If either of mode says "ANY", then they're matched */

  return FALSE;

}



/*===========================================================================

FUNCTION cmmsc_compare_stack_states

DESCRIPTION
  This function compares two stack_states. Stack States are enums and
  not always the same to be 'equal'.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if equals
===========================================================================*/
boolean cmmsc_compare_stack_states(
   cmmsc_stack_state_e_type   state1,

   cmmsc_stack_state_e_type   state2
)
{
  if(state1 == state2)
  {
    return TRUE;
  }

  /* If either of mode says "ANY", then they're matched */
  if(state1 == CMMSC_STACK_STATE_ANY ||
     state2 == CMMSC_STACK_STATE_ANY )
  {
    return TRUE;
  }

  return FALSE;

}


/*===========================================================================

FUNCTION cmmsc_init

DESCRIPTION
  This function initialized the available routing tables with proper data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_init( void )
{

  cmmsc_sglte_routing_table_init(&sglte_route_table );

  /* Initialize auto routing table */
  cmmsc_auto_routing_table_init(&auto_route_table );
}

/*===========================================================================

FUNCTION cmmsc_state_machine_init

DESCRIPTION
  This function should be called to initialize the whole state machine

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_state_machine_init(cmmsc_state_machine_s_type *state_machine)
{
  if(state_machine == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  //now, this should be feature-mode per sub, not total feature mode
  if(cmph_is_subs_feature_mode_sglte(state_machine->asubs_id))
  {
    state_machine->route_tbl = CMMSC_ROUTE_TABLE_SGLTE;
    state_machine->op_mode = CMMSC_OP_MODE_NORMAL;
  }
  else if (cmph_is_subs_feature_mask_auto(state_machine->asubs_id))
  {
    state_machine->route_tbl = CMMSC_ROUTE_TABLE_AUTO;
    state_machine->op_mode = CMMSC_OP_MODE_AUTO_MAIN;
  }
  else
  {
    state_machine->route_tbl = CMMSC_ROUTE_TABLE_AUTO;
    state_machine->op_mode = CMMSC_OP_MODE_AUTO_MAIN;
  }

  cmmsc_init_multi_stacks(state_machine);
}

/*===========================================================================

FUNCTION cmmsc_state_machine_process

DESCRIPTION
  This function should be called to process with state machine after we
  generated the EVENT

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_state_machine_process(
   cmmsc_state_machine_s_type *state_machine,
   cmmsc_event_base_s_type    *event_package)
{
  int route_tbl_idx = -1;

  if (event_package->event != CMMSC_EVENT_NONE)
  {
    /* Get the next action from the route table */
    route_tbl_idx
      = cmmsc_state_machine_find_route_idx(state_machine, event_package->event );
  
    //cmmsc_print_route_table_info(state_machine, event_package->event, route_tbl_idx);
  
    #ifdef FEATURE_CM_DEBUG_BUFFER
    {
      #if 0
      // dissabling as of now, when needed please remove #if 0 
      cm_msc_routing_entry_s_type msc_entry;
      msc_entry.as_id  = state_machine->asubs_id;
      msc_entry.tbl_id = state_machine->route_tbl;
      msc_entry.idx    = route_tbl_idx;
      cmdbg_add_to_dbg_buffer(CM_BUFF_MSC_ROUTING_ENTRY_TYPE, 0, ((void*)&msc_entry));
      #endif
    }
    #endif
    
    if (route_tbl_idx != -1)
    {
      /* Perform the action */
      cmmsc_execute_actions(route_tbl_idx,
                           (cmmsc_event_base_s_type*)event_package );
  
      /* Update the state */
      cmmsc_update_state(state_machine, route_tbl_idx);
    }
  }
}

/*===========================================================================

FUNCTION cmmsc_hicps_sxlte_remove_main_gw_needed

DESCRIPTION
  This function determines whether removing GW from main is allowed or not .
  GWL should be removed from main stack , to avoid GW acq on main stack ,
  when UE decided to switch to SRLTE .
  Otherwise GW acq can delay the LTE acq on hybr2 , which is not desirbale .
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean cmmsc_hicps_sxlte_remove_main_gw_needed(
  cmmsc_state_machine_s_type                 *state_machine,
  int                                         stack_idx,
  cm_preference_info_s_type                  *pref_info
)
{
  sd_si_info_s_type   *si_info_ptr = cmss_intl_srv_info_ptr(CM_SS_MAIN);
  boolean ret = FALSE;
  sd_ss_mode_pref_e_type        ph_obj_mode;
  boolean is_ph_obj_gw_mode   = FALSE;
  cm_ss_e_type                    ss                  = CM_SS_NONE;

  ss = cmmsc_mapping_stack_id_to_ss(stack_idx, state_machine);
  if (ss == CM_SS_NONE || ss ==  CM_SS_MAX)
  {
    CM_MSG_HIGH_3("Incorrect SS %d, as_id %d, stack_idx %d", ss, state_machine->asubs_id, stack_idx);
    return FALSE;
  }

  ph_obj_mode =
      SD_GET_COMMON_MODE(
           cmph_map_cm_mode_pref_to_sd_mode_pref(pref_info->mode_pref),
           cmmsc_get_cm_ss_mode_cap(ss));

  /* Check if the phone has GW mode */
  is_ph_obj_gw_mode = SD_MODE_CONTAIN(cmph_map_cm_mode_pref_to_sd_mode_pref(CM_MODE_PREF_GWL), 
                                      ph_obj_mode);


  if (ss == CM_SS_MAIN && is_ph_obj_gw_mode
      &&
      (si_info_ptr->srv_status != SYS_SRV_STATUS_NO_SRV ||
       si_info_ptr->srv_status != SYS_SRV_STATUS_PWR_SAVE ) 
      &&
      si_info_ptr->mode == SYS_SYS_MODE_LTE &&
      pref_info->pref_update_reason == SD_SS_PREF_UPDATE_REASON_SWITCH_SVLTE)
  {
    CM_MSG_HIGH_6("HICPS: ss %d ph_obj_mode: %d, is_ph_obj_gw_mode: %d srv_status %d sys_mode %d pref_update_reason: %d",
                     ss, ph_obj_mode, is_ph_obj_gw_mode,
                     si_info_ptr->srv_status, si_info_ptr->mode, pref_info->pref_update_reason);

    ret = TRUE;
  }
  return ret;
}

/*===========================================================================

FUNCTION cmmsc_perform_action

DESCRIPTION
  This function takes action based on the matched index action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_execute_actions(
     int                      route_table_idx,
     cmmsc_event_base_s_type *event_package )
{

  int stack_idx = 0;
  cmmsc_action_e_type act = CMMSC_ACTION_NONE;
  int act_idx;
  cmmsc_routing_table_e_type  route_tbl_id = event_package->state_machine->route_tbl;
  cmmsc_routing_table_s_type  *route_table = cmmsc_route_table_ptr(route_tbl_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(act_idx=0; act_idx < route_table->n_stack; act_idx++)
  {
    cmmsc_action_s_type *action = cmmsc_get_route_tbl_action(route_table,act_idx,route_table_idx);
    if(action != NULL)
    {
      stack_idx = action->stack_id;
      act = action->action;
    }

    /* Skip the action if the stack_idx is invalid - this ideally should not happend */
    if (act != CMMSC_ACTION_NONE &&
      !cmmsc_auto_is_valid_stack_idx(event_package->state_machine, stack_idx)
    )
    {
      CM_MSG_HIGH_1("Ignoring stack index %d",stack_idx);
      continue;
    }

    if(act != CMMSC_ACTION_NONE)
    {
      CM_MSG_HIGH_3("MSC: execute action %d on stack_idx %d from tbl_idx %d",
                    act, stack_idx, route_table_idx);
    }

    switch( act )
    {

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CMMSC_ACTION_POWER_DOWN:
      {
        cmmsc_event_oprt_mode_s_type* evt_pkg = (cmmsc_event_oprt_mode_s_type*)event_package;
        cmmsc_action_power_down(stack_idx,evt_pkg->state_machine);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CMMSC_ACTION_SUB:
      {
        cmmsc_event_sub_chg_s_type* evt_pkg = (cmmsc_event_sub_chg_s_type*)event_package;
        cmmsc_action_sub(evt_pkg);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CMMSC_ACTION_SYS_SEL_PREF:
      {
        cmmsc_event_ss_pref_s_type       * ss_pref;
        boolean gwl_removed = FALSE;

        /* casting to correct type */
        ss_pref = (cmmsc_event_ss_pref_s_type*)event_package;
        if (ss_pref->cmd && ss_pref->cmd->pref_info)
        {
          if (cmmsc_hicps_sxlte_remove_main_gw_needed(ss_pref->state_machine,
                                             stack_idx, ss_pref->cmd->pref_info))
          {
            /* Remove GWL from main stack
            ** Since it interuppts LTE acq on HYBR2 .
            */          
            gwl_removed = cmmsc_auto_update_gwl_operation (FALSE,
                                                   SD_SS_PREF_REAS_USER,
                                                   CM_MODE_PREF_GWL, 
                                                   TRUE,
                                                   SYS_MODEM_AS_ID_1);   
          }
          if(gwl_removed == FALSE)
          {
            cmmsc_send_sys_sel_pref_chgd_cmd(ss_pref->state_machine,
                                             stack_idx, ss_pref->cmd->pref_info, NULL);
          }
        }
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Force preference with reason user */
      case CMMSC_ACTION_FORCE_USER:
      {
        cmmsc_action_force_top(event_package->state_machine,
                               stack_idx, SD_SS_PREF_REAS_USER);
        break;
      }

      /* Force preference with reason user with additional action as Resume*/
      case CMMSC_ACTION_FORCE_USER_WITH_RESUME:
      {
        if (stack_idx != 0)
        {
          event_package->state_machine->stack[stack_idx].addl_action.action |= 
            (uint8)MMOC_PREF_SYS_ADDL_ACTION_RESUME;
        }
        cmmsc_action_force_top(event_package->state_machine,
                               stack_idx, SD_SS_PREF_REAS_USER);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Force preference with reason user */
      case CMMSC_ACTION_FORCE_TOP_EXT:
      {
        cmmsc_action_force_top_ext (event_package->state_machine,
                                (cmmsc_event_force_pref_s_type *)event_package,
                                stack_idx);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Force phone preference with additional params ie. orig_mode */
      case CMMSC_ACTION_FORCE_PH_EXT:
      {
        cmmsc_action_force_ph_ext (event_package->state_machine,
                                (cmmsc_event_force_pref_s_type *)event_package,
                                stack_idx);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Force preference with reason restore */
      case CMMSC_ACTION_FORCE_RESTORE:
      {
        cmmsc_action_force_top(event_package->state_machine,
                               stack_idx, SD_SS_PREF_REAS_RESTORE);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Force preference with reason stack_sync_up_bsr */
      case CMMSC_ACTION_FORCE_STACK_SYNC_UP:
      {
        cmmsc_action_force_top(event_package->state_machine,
                               stack_idx, SD_SS_PREF_REAS_STACK_SYNC_UP);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Force preference with reason stack_sync_up_bsr */
      case CMMSC_ACTION_FORCE_STACK_SYNC_UP_BSR:
      {
        cmmsc_action_force_top(event_package->state_machine,
                               stack_idx, SD_SS_PREF_REAS_STACK_SYNC_UP_BSR);
        break;
      }
      
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Force preference with reason stack_sync_up_bsr 
         and update_reason = SD_SS_PREF_UPDATE_REASON_HYBR_BSR_TO_HDR_END_SUCCESS
      */
      case CMMSC_ACTION_FORCE_STACK_SYNC_UP_BSR_END:
      {
        cmmsc_addl_pref_sys_chgd_mmoc_cmd_s_type   addl_info;
        sd_ss_pref_update_reason_e_type pref_update_reas 
          = SD_SS_PREF_UPDATE_REASON_HYBR_BSR_TO_HDR_END_SUCCESS;

        addl_info.pref_update_reas = &pref_update_reas;

        cmmsc_action_force_top_addl(event_package->state_machine, stack_idx,
                                     SD_SS_PREF_REAS_STACK_SYNC_UP_BSR, &addl_info);
        break;
      }
      
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CMMSC_ACTION_G_PS_DOMAIN_TRNSFR:
      {
        cmmsc_action_g_ps_domain_transfer(event_package->state_machine, stack_idx);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CMMSC_ACTION_LOCAL_DEACT:
      {
        cmmsc_action_local_deact(event_package->state_machine, stack_idx);
        break;
      }


      case CMMSC_ACTION_LOCAL_ACT:
      {
        cmmsc_action_local_act(event_package->state_machine, stack_idx);
        break;
      }

      case CMMSC_ACTION_HICPS_REMOVE_3GPP:
      {
        cmmsc_action_hicps_remove_3gpp(event_package->state_machine, stack_idx,
               (cmmsc_event_acq_fail_s_type*)event_package);
        break;
      }

      case CMMSC_ACTION_HICPS_COND_KICK:
      {
        cmmsc_action_hicps_cond_kick(event_package->state_machine, stack_idx);
        break;
      }

      case CMMSC_ACTION_PENDING_KICK:
      {
        cmmsc_action_pending_kick(event_package->state_machine,
                                  stack_idx, FALSE);
        break;
      }

      case CMMSC_ACTION_SHORTEN_BSR:
      {
        cmmsc_action_shorten_bsr(event_package->state_machine, stack_idx);
        break;
      }

      case CMMSC_ACTION_RESTORE_3GPP:
      {
        cmmsc_action_restore_3gpp(event_package->state_machine, stack_idx);
        break;
      }

      case CMMSC_ACTION_SVLTE_REMOVE_3GPP:
      {
        cmmsc_action_svlte_remove_3gpp(event_package->state_machine, stack_idx);
        break;
      }
      
      case CMMSC_ACTION_SVLTE_STOP_STACK:
      {
        cmmsc_action_svlte_stop_stack(event_package->state_machine, stack_idx);
      }

      case CMMSC_ACTION_SVLTE_COND_KICK:
      {
        cmmsc_action_svlte_cond_kick(event_package->state_machine, stack_idx,
          (cmmsc_event_acq_fail_s_type*)event_package, FALSE);
        break;
      }

      case CMMSC_ACTION_SUB_CHG_COND_KICK:
      {
        cmmsc_action_svlte_cond_kick(event_package->state_machine, stack_idx,
          (cmmsc_event_acq_fail_s_type*)event_package, TRUE);
        break;
      }

      case CMMSC_ACTION_DEFER_3GPP:
      {
        cmmsc_action_defer_3gpp(event_package->state_machine, stack_idx);
        break;
      }

      case CMMSC_ACTION_ENABLE_3GPP:
      {
        cmmsc_action_enable_3gpp(event_package->state_machine, stack_idx,
                                 (cmmsc_param_srv_ind_s_type*)event_package);
        break;
      }

      case CMMSC_ACTION_DISABLE_3GPP:
      {
        cmmsc_action_disable_3gpp(event_package->state_machine, stack_idx,
                                 (cmmsc_param_srv_ind_s_type*)event_package);
        break;
      }

      case CMMSC_ACTION_SYNC_UP:
      {
        cmmsc_action_sync_up(event_package->state_machine, stack_idx,
                                 (cmmsc_param_srv_ind_s_type*)event_package);
        break;
      }

      case CMMSC_ACTION_UPDATE_SRV:
      {
        cmmsc_action_update_srv(event_package->state_machine, stack_idx,
                                 (cmmsc_param_srv_ind_s_type*)event_package);
        break;
      }

      case CMMSC_ACTION_UPDATE_NO_SRV:
      {
        cmmsc_action_update_no_srv(event_package->state_machine, stack_idx,
                                 (cmmsc_param_srv_ind_s_type*)event_package);
        break;
      }

      case CMMSC_ACTION_RESUME_SRV_ACQ:
      {
        cmmsc_action_resume_srv_acq(event_package->state_machine, stack_idx,
                                 (cmmsc_param_call_ind_s_type*)event_package);
        break;
      }

      case CMMSC_ACTION_KICK_HYBR2:
      {
        
        cmmsc_action_kick_hybr2(event_package->state_machine, stack_idx);
        break;
      }
      case CMMSC_ACTION_SUSPEND:
      {
        cmmsc_event_suspend_resume_s_type* evt_pkg = (cmmsc_event_suspend_resume_s_type*)event_package;
        cmmsc_action_suspend_stack(stack_idx,evt_pkg->state_machine);
        break;
      }

      case CMMSC_ACTION_RESUME:
      {
        cmmsc_event_suspend_resume_s_type* evt_pkg = (cmmsc_event_suspend_resume_s_type*)event_package;
        cmmsc_action_resume_stack(stack_idx,evt_pkg->state_machine);
        break;
      }

      case CMMSC_ACTION_SXLTE_KICK_NOT_AUTO:
      {
        cmmsc_action_sxlte_kick_not_auto(event_package->state_machine, stack_idx);
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CMMSC_ACTION_MAX:
      default:
        break;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}

/*===========================================================================

FUNCTION cmmsc_update_state

DESCRIPTION
  This function update the current states to a new states in given
  routing table index.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_update_state(
  cmmsc_state_machine_s_type       *state_machine,
  int                               route_table_idx
)
{
  int                               stack_idx;
  cmmsc_routing_table_e_type  route_tbl_id = state_machine->route_tbl;
  cmmsc_routing_table_s_type  *route_table = cmmsc_route_table_ptr(route_tbl_id);

  for(stack_idx = 0; stack_idx < route_table->n_stack; stack_idx++)
  {
    cmmsc_stack_state_e_type new_state =
         cmmsc_get_route_tbl_stack_new_state(route_table,stack_idx,route_table_idx);

    if( new_state != CMMSC_STACK_STATE_NO_CHG )
    {
      if(state_machine->stack[stack_idx].state != new_state)
      {
        CM_MSG_HIGH_3("MSC: updating stack %d, from state %d to %d",stack_idx,
                  state_machine->stack[stack_idx].state,
                  new_state);
      }

      /* Update stack state */
      state_machine->stack[stack_idx].state = new_state;

      /* Update is_operational flag base on the new state */
      if (new_state == CMMSC_STACK_STATE_ACTIVE)
      {
        cmmsc_update_cmss_is_operational(state_machine, stack_idx, TRUE);
      }
      else if (new_state == CMMSC_STACK_STATE_INACTIVE
               || new_state == CMMSC_STACK_STATE_BARRED
               || new_state == CMMSC_STACK_STATE_NULL)
      {
        cmmsc_update_cmss_is_operational(state_machine, stack_idx, FALSE);
      }
    }
  }
}

/*===========================================================================

FUNCTION cmmsc_state_machine_find_route_idx

DESCRIPTION
  This function search for the Route Table index based on the current
  state and event.

DEPENDENCIES
  None.

RETURN VALUE
  Route table index.

SIDE EFFECTS
  None.
===========================================================================*/
int cmmsc_state_machine_find_route_idx(
    cmmsc_state_machine_s_type       *state_machine,
    cmmsc_event_e_type event
)
{
  int                         col_idx, route_tbl_idx;
  cmmsc_routing_table_e_type  route_tbl_id = state_machine->route_tbl;
  cmmsc_routing_table_s_type  *routing_table = cmmsc_route_table_ptr(route_tbl_id);
  boolean                     is_match = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate input parameter */
  if(event == CMMSC_EVENT_NONE)
  {
    return -1;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First- take the event and check for action */
  for(route_tbl_idx = 0; route_tbl_idx < routing_table->n_row; route_tbl_idx++)
  {
    /* Check for event && current mode. */
    if(cmmsc_get_route_tbl_event(routing_table,route_tbl_idx) == event &&
       cmmsc_compare_state_machine_op_mode(state_machine->op_mode,
                                           cmmsc_get_route_tbl_mode(routing_table, route_tbl_idx))
      )
    {
      /* If event matched then check for each individual states */
      for(col_idx = 0; col_idx < routing_table->n_stack; col_idx++)
      {
        if(!cmmsc_compare_stack_states(state_machine->stack[col_idx].state,
                                      cmmsc_get_route_tbl_stack_state(routing_table,col_idx,route_tbl_idx)))
        {
          break;
        }
      }

      if(col_idx == routing_table->n_stack)
      {
        is_match = TRUE;
        break;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If match, take action for each stack (this gonna be a different function) */
  if(is_match)
  {
    return route_tbl_idx;
  }

  return -1;
}

/*===========================================================================

FUNCTION cmmsc_init_mode_and_capability

DESCRIPTION
  Initialize the phone capability to default values.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void cmmsc_init_mode_and_capability (cmmsc_state_machine_s_type  *state_machine)
{
  state_machine->stack_common_info.ue_mode                = SYS_UE_MODE_NORMAL;
  state_machine->stack_common_info.ue_mode_forced         = TRUE;
  state_machine->stack_common_info.ue_band_capability     = SD_SS_BAND_PREF_ANY;
  state_machine->stack_common_info.ue_lte_band_capability = SYS_LTE_BAND_MASK_CONST_ANY;
  state_machine->stack_common_info.ue_tds_band_capability = SD_SS_BAND_PREF_ANY;
  state_machine->stack_common_info.ue_mode_capability     = cmph_map_sd_mode_pref_to_sys_mode_mask(SD_SS_MODE_PREF_ALL_MODES);
  state_machine->stack_common_info.is_force_reg_pending   = FALSE;
  state_machine->stack_common_info.call_mode              = SYS_CALL_MODE_NORMAL;
  sd_ss_update_call_mode(state_machine->asubs_id, SYS_CALL_MODE_NORMAL);

  if (cmph_is_msim())
  {
    (void)cmpmprx_fetch_max_mode_capability(state_machine->asubs_id);
  }
  else
  {
    state_machine->stack_common_info.ue_max_mode_capability 
      = cmph_map_sd_mode_pref_to_sys_mode_mask(SD_SS_MODE_PREF_ANY);
  }
}

/*===========================================================================

FUNCTION cmmsc_init_max_mode_capability

DESCRIPTION
  Get the phone capability from cmpmprx.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_init_max_mode_capability (void)
{
  if (cmph_is_msim())
  {
    size_t i = 0;
    for(i = 0; i < cmpmprx_get_num_of_sims(); i++)
    {
      cmmsc_state_machine_s_type * state_machine = cmmsimc_state_machine_ptr((sys_modem_as_id_e_type)i);
      (void)cmpmprx_fetch_max_mode_capability(state_machine->asubs_id);
    }
  }
  else
  {
    cmmsc_state_machine_s_type * state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);
    state_machine->stack_common_info.ue_max_mode_capability 
      = cmph_map_sd_mode_pref_to_sys_mode_mask(SD_SS_MODE_PREF_ANY);
  }
}

/*===========================================================================

FUNCTION cmmsc_get_policy_mode_band_cap

DESCRIPTION
  Retrieve the Mode and Band capability from PMPRX module.
  fetch the call mode capability per subs and store that in stack_common_info as well.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_get_policy_mode_band_cap (
     cmmsc_state_machine_s_type  *state_machine,
     sys_sys_mode_mask_e_type    *mode,
     sys_band_mask_e_type        *band,
     sys_band_mask_e_type        *tds_band,
     sys_lte_band_mask_e_type    *lte_band
)
{
  boolean is_force_reg = FALSE;
  sys_call_mode_t          call_mode_buffer;

  CM_ASSERT(mode != NULL);
  CM_ASSERT(band != NULL);
  CM_ASSERT(tds_band != NULL);
  CM_ASSERT(lte_band != NULL);

  if (mode == NULL || band == NULL || tds_band == NULL || lte_band == NULL ||
      state_machine == NULL)
  {
    CM_ERR_3("MSC: band null ptr, band %d, tds_band %d, lte_band %d",
            band,tds_band,lte_band);
    return;
  }

  if(!cmpmprx_read_cm_policy_consumption_eligibility(state_machine->asubs_id))
  {
    CM_MSG_HIGH_0("MSC: Not eligible to read the new policy so send default value ");
    state_machine->stack_common_info.ue_mode_capability 
      = *mode = cmmsc_get_curr_msc_max_cap(state_machine->asubs_id);
    state_machine->stack_common_info.ue_band_capability = *band = SYS_BAND_MASK_ANY;
    state_machine->stack_common_info.ue_lte_band_capability = *lte_band = SYS_LTE_BAND_MASK_CONST_ANY;
    state_machine->stack_common_info.ue_tds_band_capability = *tds_band = SD_SS_BAND_PREF_ANY;
	state_machine->stack_common_info.call_mode = SYS_CALL_MODE_NORMAL;
	
    return;
  }

  if( cmpmprx_fetch_mode_capability(state_machine->asubs_id,mode) == TRUE )
  {
    state_machine->stack_common_info.ue_mode_capability = *mode;
  }
  else
  {
    *mode = state_machine->stack_common_info.ue_mode_capability;
  }

  if( cmpmprx_get_band_capability( state_machine->asubs_id,
                                   band,
                                   lte_band,
                                   tds_band,
                                   &is_force_reg) == TRUE )
  {
    state_machine->stack_common_info.ue_band_capability = *band;
    state_machine->stack_common_info.ue_lte_band_capability = *lte_band;
    state_machine->stack_common_info.ue_tds_band_capability = *tds_band;
    if( is_force_reg == TRUE )
    {
      state_machine->stack_common_info.is_force_reg_pending = TRUE;
    }
  }
  else
  {
    *band     = state_machine->stack_common_info.ue_band_capability;
    *lte_band = state_machine->stack_common_info.ue_lte_band_capability;
    *tds_band = state_machine->stack_common_info.ue_tds_band_capability;
  }

  if(cmpmprx_get_call_mode_capability(state_machine->asubs_id, &call_mode_buffer))
  {
      state_machine->stack_common_info.call_mode = call_mode_buffer;
    sd_ss_update_call_mode(cmph_map_subs_to_sd_ss(state_machine->asubs_id), call_mode_buffer);
  }
}


/*===========================================================================
FUNCTION cmmsc_apply_SIM_capability

DESCRIPTION
  This function set stack capability based on its SIM capability
===========================================================================*/
void cmmsc_apply_SIM_capability(cmmsc_state_machine_s_type   *state_machine )
{
  int s,sim_idx = 0;
  cmmsimc_state_s_type *cmmsimc_ptr = cmmsimc_state_ptr();

  sd_ss_mode_pref_e_type old_mode_cap = SD_SS_MODE_PREF_NONE;

  if(state_machine->asubs_id >= CMMSIMC_SIM_NUM)
  {
    return;
  }
  sim_idx = (int)state_machine->asubs_id;

  for(s = 0; s < CMMSC_STACK_NUM; s++)
  {
	#ifdef FEATURE_MMODE_TRIPLE_SIM
    if(sim_idx > SYS_MODEM_AS_ID_NONE && sim_idx < SYS_MODEM_AS_ID_NO_CHANGE)
  #elif defined FEATURE_MMODE_DUAL_SIM
    if(sim_idx > SYS_MODEM_AS_ID_NONE && sim_idx < SYS_MODEM_AS_ID_3)
  #else
	  if(sim_idx > SYS_MODEM_AS_ID_NONE && sim_idx < SYS_MODEM_AS_ID_2)
  #endif
    {
      old_mode_cap = state_machine->stack[s].capablity.mode;
      state_machine->stack[s].capablity.mode &= cmmsimc_ptr->sim[sim_idx].capability.mode;

      if (old_mode_cap != state_machine->stack[s].capablity.mode)
      {      
        CM_MSG_HIGH_5("cap_chgd: sim_idx=%d, sim_cap=0x%x stack_id=%d, old stack_cap=0x%x, updated stack_cap=0x%x",
                         sim_idx, cmmsimc_ptr->sim[sim_idx].capability.mode,
                        s,
                        old_mode_cap,
                        state_machine->stack[s].capablity.mode);
      }
    }
  }
}

/*===========================================================================

FUNCTION cmmsc_set_stack_capability

DESCRIPTION
  This function set stack capability. This function is called when we
  power-up or send subscriptions. Depending on which harware , we set
  different capability.
  For SGLTE, the total capability needs to come from PM, however, this
  function will split the capabilities to individual stacks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability( cmmsc_state_machine_s_type   *state_machine )
{
  sys_subs_feature_t  feature_mode = SYS_SUBS_FEATURE_MODE_NORMAL ;
  boolean                       is_emergency;

  if(state_machine == NULL)
    sys_err_fatal_null_ptr_exception();

  feature_mode = cmph_get_subs_feature_mode(state_machine->asubs_id);

  is_emergency = state_machine->stack_common_info.is_in_emergency_call;

  /* Apply Feature Level Filtering per Each Sub */
  switch(feature_mode)
  {
    case SYS_SUBS_FEATURE_MODE_SGLTE:
      if( is_emergency )
      {
        cmmsc_set_stack_capability_sglte_emergency(state_machine);
      }
      else
      {
        cmmsc_set_stack_capability_sglte(state_machine);
      }
      break;

    case SYS_SUBS_FEATURE_MODE_SVLTE:
    case SYS_SUBS_FEATURE_MODE_SRLTE:
    case SYS_SUBS_FEATURE_MODE_NORMAL:
      if( is_emergency )
      {
        cmmsc_set_stack_capability_auto_emergency(state_machine);
      }
      else
      {
        cmmsc_set_stack_capability_auto(state_machine);
      }
      break;

    default:
      CM_ERR_1("Invalid feature_mode",feature_mode);
  }

  /* Apply SIM Level Filtering Per Each Sub */
  cmmsc_apply_SIM_capability(state_machine);
}

/*===========================================================================

FUNCTION cmmsc_map_ue_mode_pm2msc

DESCRIPTION
  This function mapping the Policy Manager UE_MODE to CMMSC's OP MODE.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
cmmsc_state_machine_op_mode_e_type  cmmsc_map_ue_mode_pm2msc (

    sys_ue_mode_e_type    pm_ue_mode
)
{
  switch (pm_ue_mode)
  {
    case SYS_UE_MODE_NORMAL:
    case SYS_UE_MODE_CSFB_ONLY:
      return CMMSC_OP_MODE_NORMAL;

    case SYS_UE_MODE_SGLTE_HOME:
    case SYS_UE_MODE_SGLTE_TEST_ONLY:
      return CMMSC_OP_MODE_SGLTE_DUAL_STACK;

    case SYS_UE_MODE_SGLTE_ROAM:
      return CMMSC_OP_MODE_SGLTE_ROAM;

    case SYS_UE_MODE_NONE:
    default:
      CM_ERR_1("SC_SGLTE: invalid pm ue_mode %d",pm_ue_mode);
      return CMMSC_OP_MODE_NONE;
  }
}

/*===========================================================================

FUNCTION cmmsc_pack_pref_info

DESCRIPTION
  This function packs pref info.

DEPENDENCIES
  cmpref2_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_pack_pref_info
(

        cm_network_sel_mode_pref_e_type network_sel_mode_pref,
            /* The network sel mode associated with the selected NAM */

        cm_mode_pref_e_type             mode_pref,
           /* The mode preference that is associated with the selected NAM.
           */

        cm_band_pref_e_type             band_pref,
           /* Band preference that is associated with the selected NAM.
           */

        sys_lte_band_mask_e_type        lte_band_pref,
           /* LTE band preference that is associated with the selected NAM.
           */

        cm_band_pref_e_type             tds_band_pref,
           /* TD-SCDMA band preference that is associated with the selected NAM.
           */

        cm_prl_pref_e_type              prl_pref,
           /* PRL preference that is associated with the selected NAM.
           */

        cm_roam_pref_e_type             roam_pref,
           /* Roam preference that is associated with the selected NAM.
           */

        cm_hybr_pref_e_type             hybr_pref,
           /* New hybrid preference (none, CDMA/HDR).
           */

        sys_plmn_id_s_type              plmn,
           /* The PLMN id to be acquired, valid when the net_sel_mode_pref
           ** is MANUAL.
           ** Applies only for GSM/WCDMA modes.
           */

        cm_srv_domain_pref_e_type       srv_domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        cm_gw_acq_order_pref_e_type     acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        cm_pref_s_type                 *pref_info
        /* Pointer to preference info stuct
            */
)
{
  /* Validate params */
  CM_ASSERT(pref_info != NULL);

  if (pref_info == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* Pack the pref info */
  pref_info->mode_pref = mode_pref;
  pref_info->network_sel_mode_pref = network_sel_mode_pref;
  pref_info->band_pref = band_pref;
  pref_info->lte_band_pref = lte_band_pref;
  pref_info->tds_band_pref = tds_band_pref;
  pref_info->prl_pref = prl_pref;
  pref_info->roam_pref = roam_pref;
  pref_info->hybr_pref = hybr_pref;
  pref_info->plmn = plmn;
  pref_info->srv_domain_pref = srv_domain_pref;
  pref_info->acq_order_pref = acq_order_pref;

}

/*===========================================================================

FUNCTION  cmmsc_pack_cmd_for_pref_sys_chgd_per_top_queue

DESCRIPTION
  Pack the param for cmd_pref_sys_chgd base on the preference of the top priority
  queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_pack_cmd_for_pref_sys_chgd_per_top_queue(

        cm_ss_e_type                          ss,
            /* Indicates the ss this pref_sys_chgd command is intended for.
            */

        sd_ss_pref_reas_e_type                pref_reas,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type                orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        boolean                               is_new_policy_tobe_read,
            /* Indicate if to update the policy */

        const cm_orig_q_s_type               *ph_orig_top_ptr,
           /* preference of the top priority queue.
           */

        cm_act_update_reas_e_type             act_update_reas,
           /* Reason for the update
           */

        cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd
)
{
  cm_preference_info_s_type  *pref_info = NULL;
  sys_modem_as_id_e_type      asubs_id = cmph_map_cm_ss_to_subs(ss);

  if (cmd == NULL)
  {
    CM_ERR_0("cmd ptr null");
    return;
  }

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_0("queue_top_ptr null");
    return;
  }

  cmd->cm_ss = ss;
  cmd->is_new_policy_tobe_read = is_new_policy_tobe_read;

  /* Allocate memory for preference info */
  pref_info = (cm_preference_info_s_type*) cm_mem_malloc(
                          sizeof(cm_preference_info_s_type));
  cmd->pref_info = pref_info;

  /* Set preference base on the input params */
  pref_info->pref_reas = pref_reas;
  pref_info->orig_mode = orig_mode;
  pref_info->act_update_reas = act_update_reas;

  /* Set preference base on default values */
  pref_info->otasp_band = cmph_map_otasp_act_code_to_sd_band(CM_OTASP_ACT_CODE_NONE);
  pref_info->otasp_blksys = cmph_map_otasp_act_code_to_sd_blksys(CM_OTASP_ACT_CODE_NONE);;
  pref_info->avoid_type = SD_SS_AVOID_SYS_IDLE;
  pref_info->avoid_time = 0;

  /* Set preference base on top priority queue */
  pref_info->mode_pref = ph_orig_top_ptr->orig->orig_mode_pref;
  pref_info->band_pref = ph_orig_top_ptr->orig->orig_band_pref;
  pref_info->lte_band_pref = ph_orig_top_ptr->orig->orig_lte_band_pref;
  pref_info->tds_band_pref = ph_orig_top_ptr->orig->orig_tds_band_pref;
  pref_info->hybr_pref = ph_orig_top_ptr->orig->orig_hybr_pref;
  pref_info->prl_pref = ph_orig_top_ptr->orig->orig_prl_pref;
  pref_info->roam_pref = ph_orig_top_ptr->orig->orig_roam_pref;
  pref_info->srv_domain_pref = ph_orig_top_ptr->orig->orig_srv_domain_pref;
  pref_info->csg_id = ph_orig_top_ptr->orig->csg_id;
  pref_info->csg_rat = ph_orig_top_ptr->orig->csg_rat;

  /* Set preference base on ph preference - those preference are not part of priority queue */
  pref_info->network_sel_mode_pref = CMPH_SS_NETWORK_SEL_MODE_PREF(asubs_id, cmph_ptr());
  pref_info->camp_mode_pref
    = cmph_map_cm_net_sel_mode_pref_to_camp_pref(
       pref_info->network_sel_mode_pref);
  pref_info->rat_acq_order_ptr
    = cmph_get_acq_pri_order_per_sub(cmph_map_subs_to_ss(asubs_id));
  pref_info->acq_order_pref = CMPH_SS_ACQ_ORDER_PREF(asubs_id, cmph_ptr());
  pref_info->sys_id.sys_id.id_type = SYS_SYS_ID_TYPE_UMTS;
  pref_info->sys_id.sys_id.id.plmn  = CMPH_SS_PLMN_PREF(asubs_id, cmph_ptr());
  pref_info->sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;

  pref_info->hybr1_pref.hybr_mode_pref = ph_orig_top_ptr->orig->orig_mode_pref;
  pref_info->hybr1_pref.hybr_orig_mode = orig_mode;

  /* retrieve if there is any CMSOA pref_update_reason  */
  pref_info->pref_update_reason = ph_orig_top_ptr->orig->pref_update_reason;
  CM_MSG_HIGH_1("phobj pref_update_reason %d",ph_orig_top_ptr->orig->pref_update_reason);  
}

/*===========================================================================

FUNCTION  cmmsc_pack_cmd_for_pref_sys_chgd_per_ph_pref

DESCRIPTION
  Pack the param for cmd_pref_sys_chgd base on the phone preference.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_pack_cmd_for_pref_sys_chgd_per_ph_pref(

        cm_ss_e_type                          ss,
            /* Indicates the ss this pref_sys_chgd command is intended for.
            */

        sd_ss_pref_reas_e_type                pref_reas,
            /* Reason for changing the SS-Preference.
            */

        boolean                               is_new_policy_tobe_read,
            /* Indicate if to update the policy */

        cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd
)
{
  cm_preference_info_s_type  *pref_info = NULL;
  sys_modem_as_id_e_type      asubs_id = cmph_map_cm_ss_to_subs(ss);
  cmph_s_type                 *ph_ptr = cmph_ptr();
  cm_orig_q_s_type            *ph_obj 
    = cmtask_orig_para_search_act_id( ss, (cm_act_id_type)cmph_ptr());

  if (cmd == NULL)
  {
    CM_ERR_0("cmd ptr null");
    return;
  }

  cmd->cm_ss = ss;
  cmd->is_new_policy_tobe_read = is_new_policy_tobe_read;

  /* Allocate memory for preference info */
  pref_info = (cm_preference_info_s_type*) cm_mem_malloc(
                          sizeof(cm_preference_info_s_type));
  cmd->pref_info = pref_info;

  /* Set preference base on the input params */
  pref_info->pref_reas = pref_reas;

  /* Set preference base on phone pref */
  pref_info->network_sel_mode_pref = CMPH_SS_NETWORK_SEL_MODE_PREF(asubs_id, ph_ptr);
  pref_info->orig_mode = cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                      pref_info->network_sel_mode_pref);
  pref_info->mode_pref = CMPH_SS_MODE_PREF(asubs_id, ph_ptr);

  pref_info->hybr1_pref.hybr_mode_pref = pref_info->mode_pref;
  pref_info->hybr1_pref.hybr_orig_mode = pref_info->orig_mode;

  pref_info->hybr_pref = CMPH_SS_HYBR_PREF(asubs_id, ph_ptr);
  pref_info->band_pref = CMPH_SS_BAND_PREF(asubs_id, ph_ptr);
  pref_info->lte_band_pref = CMPH_SS_LTE_BAND_PREF(asubs_id, ph_ptr);
  pref_info->tds_band_pref = CMPH_SS_TDS_BAND_PREF(asubs_id, ph_ptr);
  pref_info->prl_pref = CMPH_SS_PRL_PREF(asubs_id, ph_ptr);
  pref_info->roam_pref = CMPH_SS_ROAM_PREF(asubs_id, ph_ptr);
  pref_info->srv_domain_pref = CMPH_SS_SRV_DOMAIN_PREF(asubs_id, ph_ptr);
  pref_info->acq_order_pref = CMPH_SS_ACQ_ORDER_PREF(asubs_id, ph_ptr);
  pref_info->rat_acq_order_ptr
    = cmph_get_acq_pri_order_per_sub(cmph_map_subs_to_ss(asubs_id));
  pref_info->network_sel_mode_pref = CMPH_SS_NETWORK_SEL_MODE_PREF(asubs_id, ph_ptr);
  pref_info->camp_mode_pref = cmph_map_cm_net_sel_mode_pref_to_camp_pref(
       pref_info->network_sel_mode_pref);
  pref_info->sys_id.sys_id.id.plmn = CMPH_SS_PLMN_PREF(asubs_id, ph_ptr);
  pref_info->sys_id.sys_id.id_type = SYS_SYS_ID_TYPE_UMTS;
  pref_info->sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;

  /* Set preference base on default values */
  pref_info->otasp_band = cmph_map_otasp_act_code_to_sd_band(CM_OTASP_ACT_CODE_NONE);
  pref_info->otasp_blksys = cmph_map_otasp_act_code_to_sd_blksys(CM_OTASP_ACT_CODE_NONE);;
  pref_info->avoid_type = SD_SS_AVOID_SYS_IDLE;
  pref_info->avoid_time = 0;
  if (ph_obj != NULL)
  {
    pref_info->pref_update_reason = ph_obj->orig->pref_update_reason;
  }
  else
  {
    pref_info->pref_update_reason = SD_SS_PREF_UPDATE_REASON_NONE;
  }
}
/*===========================================================================

FUNCTION  cmmsc_pack_cmd_for_pref_sys_chgd_per_pref

DESCRIPTION
  Pack the param for cmd_pref_sys_chgd base on the given preference .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_pack_cmd_for_pref_sys_chgd_per_pref(

    cm_ss_e_type                          ss,
        /* Indicates the ss this pref_sys_chgd command is intended for.
        */

    sd_ss_pref_reas_e_type                pref_reas,
        /* Reason for changing the SS-Preference.
        */

    sd_ss_orig_mode_e_type                orig_mode,
        /* The origination mode (normal, OTASP, emergency, other, etc.)
        ** that is associated with the selected NAM.
        */

    const cm_pref_s_type                 *ss_pref,
       /* Stack preference that is associated with the selected NAM.
       */

    sd_band_e_type                        otasp_band,
       /* The band-class that is associated with OTASP origination mode.
       ** Note that this parameter is ignored if orig_mode != OTASP.
       */

    sd_blksys_e_type                      otasp_blksys,
      /* The PCS frequency block/Cellular system that is associated
      ** with OTASP origination mode. Note that this parameter is ignored
      ** if orig_mode != OTASP.
      */

    sd_ss_avoid_sys_e_type                avoid_type,
      /* Type of the system to be avoided, valid only
      ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
      */

    dword                                 avoid_time,
      /* Time in seconds for which the system is to be avoided, valid
      ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
      */
    const cm_acq_pri_order_pref_s_type   *rat_acq_order_ptr,
       /* Rat acquisition priority order
       */

    sd_ss_pref_camp_mode_e_type           camp_mode_pref,
       /* camp only pref
       */

    boolean                               is_new_policy_tobe_read,

    cm_act_update_reas_e_type             act_update_reas,
    /* Reason for the update
    */

    cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd
)
{
  cm_preference_info_s_type  *pref_info = NULL;

  if (cmd == NULL)
  {
    CM_ERR_0("cmd ptr null");
    return;
  }

  cmd->cm_ss = ss;
  cmd->is_new_policy_tobe_read = is_new_policy_tobe_read;

  /* Allocate memory for preference info */
  pref_info = (cm_preference_info_s_type*) cm_mem_malloc(
                          sizeof(cm_preference_info_s_type));
  cmd->pref_info = pref_info;

  pref_info->pref_reas  = pref_reas;
  pref_info->orig_mode  = orig_mode;
  pref_info->otasp_band = otasp_band;
  pref_info->otasp_blksys = otasp_blksys;
  pref_info->avoid_type   = avoid_type;
  pref_info->avoid_time   = avoid_time;
  pref_info->rat_acq_order_ptr = rat_acq_order_ptr;
  pref_info->camp_mode_pref    = camp_mode_pref;
  pref_info->act_update_reas   = act_update_reas;

  if (ss_pref)
  {
    pref_info->pref_update_reason = ss_pref->pref_update_reason;
    pref_info->mode_pref = ss_pref->mode_pref;
    pref_info->band_pref = ss_pref->band_pref;
    pref_info->lte_band_pref = ss_pref->lte_band_pref;
    pref_info->tds_band_pref = ss_pref->tds_band_pref;
    pref_info->hybr_pref = ss_pref->hybr_pref;
    pref_info->prl_pref = ss_pref->prl_pref;
    pref_info->roam_pref = ss_pref->roam_pref;
    pref_info->acq_order_pref = ss_pref->acq_order_pref;
    pref_info->srv_domain_pref = ss_pref->srv_domain_pref;
    pref_info->network_sel_mode_pref = ss_pref->network_sel_mode_pref;
    pref_info->sys_id.sys_id.id_type = SYS_SYS_ID_TYPE_UMTS;
    pref_info->sys_id.sys_id.id.plmn  = ss_pref->plmn;
    pref_info->sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;
    pref_info->hybr1_pref.hybr_mode_pref = ss_pref->mode_pref;
  }

  pref_info->hybr1_pref.hybr_orig_mode = orig_mode;

}


/*===========================================================================

FUNCTION cmmsc_proc_cmd_oprt_mode_chgd

DESCRIPTION

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_proc_cmd_oprt_mode_chgd
(
   cmmsc_param_oprt_mode_s_type      *oprt_mode_event
   /* New operating mode of the phone. */
)
{

  sys_oprt_mode_e_type          oprt_mode;
  cmmsc_state_machine_s_type   *state_machine;
  cmmsc_event_e_type            event = CMMSC_EVENT_NONE;
  cmmsc_event_oprt_mode_s_type  event_oprt_mode;
  cmph_s_type                   *ph_ptr = cmph_ptr();
  cm_policy_config_s_type       *pm_ptr = cmpm_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set parameters */
  if(oprt_mode_event == NULL)
    sys_err_fatal_null_ptr_exception();

  oprt_mode = oprt_mode_event->oprt_mode;
  state_machine = oprt_mode_event->state_machine;

  CM_MSG_HIGH_2("MSC: oprt_chgd, asubs_id=%d, sub_fmode=%d",
                 state_machine->asubs_id,
                 cmph_get_subs_feature_mode(state_machine->asubs_id));

  #if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)) || defined(FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH)
  if(cmph_is_subs_feature_mode_srlte(state_machine->asubs_id))
  {
    cmsoa_state_info_s_type *cmsoa_state = cmsoa_state_info_ptr();
    CM_MSG_HIGH_2("MSC: curr sub mode=%d, is_pwrup_from_last_submode = %d",
                   cmsoa_state->config.curr_sv_oprt_mode, 
                   cmsoa_state->is_pwrup_from_last_submode);
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Terminate any pending get network request. */
  cmph_term_pending_get_networks_req_client_only(CM_SS_MAX);

  /* Reset is_kicked_hybr_2 */
  if (cmph_is_subs_feature_mask_auto(state_machine->asubs_id))
  {
    *cmph_get_is_kicked_hybr_2_ptr() = FALSE;
    cmmsc_auto_update_is_kick_hdr_pending(state_machine, FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (oprt_mode)
  {
    case SYS_OPRT_MODE_ONLINE:
    case SYS_OPRT_MODE_PSEUDO_ONLINE:
    {
      event = CMMSC_EVENT_ONLINE;

	  /* if oprt mode is online, send call mode ind to clients */
      if(oprt_mode == SYS_OPRT_MODE_ONLINE)
	  {
	    uint8 subs_id = SYS_MODEM_AS_ID_1;
		
        for(subs_id=0; subs_id < pm_ptr->device_mode.nSims;subs_id++)
        {
          sys_call_mode_t temp_call_mode_buf = SYS_CALL_MODE_INVALID;
          if(cmpmprx_fetch_call_mode_capability(subs_id, &temp_call_mode_buf))
          {
		    cmemg_send_call_mode_ind(subs_id, temp_call_mode_buf, TRUE);
			sd_ss_update_call_mode(cmph_map_subs_to_sd_ss(subs_id), temp_call_mode_buf);
            cmmsc_set_stack_common_call_mode_per_subs(temp_call_mode_buf, subs_id);
          }
        }
	  }

      /* Update op mode per feature mode */
      switch(cmph_get_subs_feature_mode(state_machine->asubs_id))
      {

        case SYS_SUBS_FEATURE_MODE_SGLTE:
          cmmsc_sglte_update_op_mode(state_machine,
                                        ph_ptr->is_gwl_subscription_available);
          break;

        case SYS_SUBS_FEATURE_MODE_NORMAL:
        case SYS_SUBS_FEATURE_MODE_SRLTE:
        case SYS_SUBS_FEATURE_MODE_SVLTE:
        {
          /* Set special event to disable hybr1 */
          if (cmph_msim_is_3gpp2_allowed(state_machine->asubs_id) &&
             cmmsc_auto_online_disable_hybr1(
                                               oprt_mode,
                                               ph_ptr->main_stack_info.pref_info.mode_pref,
                                               ph_ptr->int_hybr_pref)
          )
          {
            cmmsc_auto_update_is_hybr1_disabled(state_machine, TRUE);
            event = CMMSC_EVENT_ONLINE_DISABLE_HYBR1;
          }
          break;
        }

        default:
          break;
      }
      break;
    }

    default:
    {
      if (cmph_is_subs_feature_mode_sglte(state_machine->asubs_id))
      {
        cmmsc_state_machine_op_mode_e_type op_mode = cmmsc_sglte_retrieve_op_mode(state_machine);

        CM_MSG_HIGH_2("MSC: op_mode updated =%d, at operating mode change =%d", 
                   op_mode, oprt_mode );

        cmmsc_state_machine_op_mode_update(state_machine, op_mode );      
      }

      state_machine->stack_common_info.is_in_emergency_call = FALSE;
      event = CMMSC_EVENT_POWER_DOWN;
      state_machine->stack_common_info.ue_mode_forced = FALSE;
      break;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Setting stacks capability */
  cmmsc_set_stack_capability(state_machine);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Package the event with parameters */
  event_oprt_mode.event = event;
  event_oprt_mode.state_machine = state_machine;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process in Routing Table */
  cmmsc_state_machine_process(state_machine,
                              (cmmsc_event_base_s_type*)&event_oprt_mode);

}


/*===========================================================================

FUNCTION cmmsc_proc_cmd_suspend_resume

DESCRIPTION

DEPENDENCIES
  cmmsc_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_proc_cmd_suspend_resume
(
   cmmsc_param_suspend_resume_subs_s_type      *suspend_resume_event
)
{

  cmmsc_state_machine_s_type           *state_machine;
  cmmsc_event_e_type                   event           = CMMSC_EVENT_NONE;
  cmmsc_event_suspend_resume_s_type    event_suspend_resume;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(suspend_resume_event == NULL)
    sys_err_fatal_null_ptr_exception();

  state_machine = suspend_resume_event->state_machine;
  
  CM_MSG_HIGH_3("MSC: suspend_resume stat_machine asubs_id %d, event asubs_id=%d, resume=%d",
         state_machine->asubs_id,
         suspend_resume_event->asubs_id,
         suspend_resume_event->suspend);

  if(suspend_resume_event->asubs_id != state_machine->asubs_id)
  {
    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (suspend_resume_event->suspend)
  {
    event = CMMSC_EVENT_SUSPEND;
  }
  else
  {
    event = CMMSC_EVENT_RESUME;
  }
  /* Package the event with parameters */
  event_suspend_resume.event         = event;
  event_suspend_resume.state_machine = state_machine;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process in Routing Table */
  cmmsc_state_machine_process(state_machine,
                              (cmmsc_event_base_s_type*)&event_suspend_resume);

}

/*===========================================================================

FUNCTION cmmsc_is_ok_to_force_prefs

DESCRIPTION
  Check if there are any on-going VOICE or EMERGENCY calls in SGLTE. If yes,
  do not force down the preferences yet. 

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE: If NO on-going VOICE or EMERGENCY Calls in SGLTE
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static dword cmmsc_is_ok_to_force_prefs(
   sd_ss_pref_reas_e_type           force_reason,
        /* Reason for forcing the SS-preference */

   cm_act_update_reas_e_type        act_update_reas,
        /* Reason for the update */

   cmmsc_state_machine_s_type*      state_machine
        /* State machine for the subs being checked */

)
{
  /* Do not force down user preferences if a call is in progress. This could
  ** cause the call to drop. At this point, priority queues have been updated 
  ** and the new preferences will be forced down once the call ends 
  */

  cm_call_id_type  active_call_id;
  cmcall_s_type   *active_call_ptr = NULL;

  cmph_s_type     *ph_ptr = cmph_ptr();

  active_call_id = cmcall_find_active_cs_call_per_subs(state_machine->asubs_id);


  /* 1. force reason is SD_SS_PREF_REAS_USER --> User changed the SS-Preference
  ** 2. act_update_reas is CM_ACT_UPDATE_REAS_USER --> User is forcing the pref. Force Pref reas 
  ** can be USER sometime even when USER is not actively changing prefs like during redial 
  ** 3. There is an active call 
  ** 4. ps_detach_forced flag is not Set. If set, this flag indicates that PS detach is required 
  ** during call. So even if act reas is user, prefs need to be sent out 
  */

  if( ( force_reason == SD_SS_PREF_REAS_USER ) &&
      act_update_reas == CM_ACT_UPDATE_REAS_USER &&
      active_call_id != CM_CALL_ID_INVALID &&
      !ph_ptr->is_srv_domain_ps_detach_forced &&
      cmph_is_subs_feature_mode_sglte(state_machine->asubs_id) &&
      ph_ptr->oprt_mode == SYS_OPRT_MODE_ONLINE
    )
  {
    CM_MSG_HIGH_1( "MSC: On-Going call ID: %d", active_call_id);
    active_call_ptr = cmcall_ptr( active_call_id );
    if( active_call_ptr != NULL )
    {
      CM_MSG_HIGH_3( "MSC: Call Type: %d, Call Subs: %d, Call State: %d",
                   active_call_ptr->call_type,
                   active_call_ptr->asubs_id,
                   active_call_ptr->call_state );
    }

    return FALSE;
  }

  return TRUE;
}

cm_mode_band_pref_s_type  cmmsc_current_stack_preference(
  cmmsc_state_machine_s_type  *state_machine
)
{
  
  cm_mode_band_pref_s_type               stack_pref;
  
  if(state_machine == NULL)
  {
    CM_ERR_FATAL_0("state machine NULL");
  }
  else
  {
  
  /* Update preference for that stack */
    stack_pref.mode_pref      =  state_machine->stack_common_info.preference.mode_pref;
    stack_pref.band_pref      =  state_machine->stack_common_info.preference.band_pref;
    stack_pref.lte_band_pref  =  state_machine->stack_common_info.preference.lte_band_pref;
    stack_pref.tds_band_pref  =  state_machine->stack_common_info.preference.tds_band_pref;

    /*
    CM_MSG_HIGH_5("cmmsc_current_stack_preference(): mode_pref  %d, band_pref 0x%08x %08x tds_band_pref  0x%08x %08x",
      state_machine->stack_common_info.preference.mode_pref,
      QWORD_HIGH(state_machine->stack_common_info.preference.band_pref),
      QWORD_LOW(state_machine->stack_common_info.preference.band_pref),
      QWORD_HIGH(state_machine->stack_common_info.preference.tds_band_pref),
      QWORD_LOW(state_machine->stack_common_info.preference.tds_band_pref));
    cm_print_lte_band_mask(state_machine->stack_common_info.preference.lte_band_pref); 
     */

  }
 
  return stack_pref;
  
}

/*===========================================================================

FUNCTION cmmsc_proc_cmd_pref_sys_chgd

DESCRIPTION
  This function should be called when the phone preferences have changed.
  This module will be responsible for queing pref_sys_chgd command to MMOC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void   cmmsc_proc_cmd_pref_sys_chgd
(
  cmmsc_param_pref_sys_s_type * pref_sys_param
)
{
  cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd = NULL;
  cmmsc_state_machine_s_type           *state_machine = NULL;

  cmmsc_event_e_type                  event = CMMSC_EVENT_SYS_SEL_PREF_MAX;
  cmmsc_event_ss_pref_s_type          event_ss_pref;
  cmmsc_state_machine_op_mode_e_type  old_op_mode;
  boolean                             is_new_policy_tobe_read = 0;
  boolean                             is_emerg_call_actv = FALSE;
  sd_ss_pref_camp_mode_e_type         camp_mode_pref = SD_SS_PREF_CAMP_MODE_NONE;
  cm_preference_info_s_type           *pref_info = NULL;
  cm_ss_e_type                        cm_ss;
  int                                 cm_stack_id = 0;
  sys_subs_feature_t                  subs_feature_mode;
  cm_hybr_pref_e_type                 hybr_pref = CM_HYBR_PREF_NONE;
  sd_ss_orig_mode_e_type              orig_mode = SD_SS_ORIG_MODE_NONE;
  cm_mode_band_pref_s_type            old_stack_pref;
  cm_orig_q_s_type                    *ph_orig_top_ptr     = NULL;
  /* Stack preference that is associated with the selected NAM.
  */
  cmph_s_type                        *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Validate parameters */
  if(pref_sys_param == NULL)
    sys_err_fatal_null_ptr_exception();

  cmd = pref_sys_param->cmd;
  state_machine = pref_sys_param->state_machine;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmd == NULL || state_machine == NULL)
    sys_err_fatal_null_ptr_exception();

  /* Get the params from the cmd */
  is_new_policy_tobe_read = cmd->is_new_policy_tobe_read;
  cm_ss = cmd->cm_ss;

  /* Validate cm_ss */
  if(cm_ss != CM_SS_MAX)
  {
    cm_stack_id = cmph_ptr()->ss_subs_map[cmd->cm_ss].stack_id;
  }

  pref_info = cmd->pref_info;
  if (pref_info == NULL)
  {
    CM_ERR_0("pref info null ptr");
    return;
  }

  CM_MSG_HIGH_3("pref_chgd: MSC->MMOC cm_ss=%d, pref mode_pref=%d, is_new_policy_tobe_read=%d", 
                   cm_ss, pref_info->mode_pref, is_new_policy_tobe_read);

  if ( !cmmsc_is_ok_to_force_prefs( pref_info->pref_reas,
                                    pref_info->act_update_reas,
                                    state_machine ) )
  {
    CM_MSG_HIGH_0( "MSC: Voice Call in progress. Don't force user prefs");
    return;
  }

  /* Check if ss is valid per op mode and hybr pref */
  hybr_pref = pref_info->hybr_pref;
  if (!cmmsc_is_valid_ss_per_op_mode(state_machine, cm_stack_id,hybr_pref))
    return;

  camp_mode_pref = pref_info->camp_mode_pref;
  orig_mode = pref_info->orig_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* For SGLTE, Policy Manager will set the UE-Mode
  ** For SVLTE, we might have to check and determine the UE by CM.
  */
  /* Save the previous stack prefernce. This will be used later to determine PS_DETACH */
  old_stack_pref = cmmsc_current_stack_preference(state_machine);

  /* Save the previous ue mode.  This will be used later to determine */
  old_op_mode = state_machine->op_mode;

  /****************************************************
  ** SGLTE: Read UE mode if PM indicates UE changed
  ** SVLTE: Determine UE mode
  *****************************************************/

  if( is_new_policy_tobe_read )
  {
    if (cmph_is_subs_feature_mode_sglte(state_machine->asubs_id))
    {
      cmmsc_state_machine_op_mode_update(state_machine, cmmsc_sglte_retrieve_op_mode(state_machine));

      /* For SGLTE, if we're in CAMP_ONLY mode, we will be doing FULL_RAT SEARCH
         If CAMP_MODE_OFF, we go back to SGLTE_ROAM*/
      if( state_machine->stack_common_info.ue_mode == SYS_UE_MODE_SGLTE_ROAM )
      {
        if (camp_mode_pref == SD_SS_PREF_CAMP_MODE_ON )
        {
          cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_FULL_RAT_SEARCH );
        }
        else if (camp_mode_pref == SD_SS_PREF_CAMP_MODE_OFF)
        {
          cmmsc_state_machine_op_mode_update(state_machine, CMMSC_OP_MODE_SGLTE_ROAM );
        }
      }
      cmmsc_set_stack_capability(state_machine);
    }
  }

  if (cmph_is_subs_feature_mask_auto(state_machine->asubs_id))
  {
    if( is_new_policy_tobe_read || cm_ss == CM_SS_MAX)
    {
      /* Print ph obj top information for debug purpose */
      cm_orig_q_s_type *ph_orig_top_ptr 
        = cmtask_orig_para_get_top(cm_ss == CM_SS_MAX?CM_SS_MAIN:cm_ss);

      /* Update op_mode per latest preference and ue_mode */
      cmmsc_auto_update_op_mode(
               state_machine,
               is_new_policy_tobe_read,
               FALSE,
               cmph_map_cm_mode_pref_to_sd_mode_pref(pref_info->mode_pref),
               cmph_map_cm_hybr_pref_to_sd_hybr_pref(hybr_pref),
               cmph_ptr()->is_gwl_subscription_available,
               cmph_ptr()->is_cdma_subscription_available,
               pref_info);
               
      cmmsc_set_stack_capability_auto(state_machine);
      
      /* Apply SIM Level Filtering Per Each Sub */
      cmmsc_apply_SIM_capability(state_machine);
    }

    /* Set capability during entire duration of of emerg call */
    is_emerg_call_actv = cmcall_emergency_call_active();
	
    if(is_emerg_call_actv || 
		(state_machine->stack_common_info.is_in_emergency_call != is_emerg_call_actv))
    {
      state_machine->stack_common_info.is_in_emergency_call = is_emerg_call_actv;

      /* If emerg orig req is on a specific SS, then don't do special mode setting */
      if(! (is_emerg_call_actv && cmmsc_is_msc_multi_stacks(state_machine->asubs_id)))
      {
        cmmsc_set_stack_capability(state_machine);
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  subs_feature_mode = cmph_get_subs_feature_mode(state_machine->asubs_id);


  /* For operation mode LPM/FTM, we also allow sys_sel_pref to go through
     Override pref_reas to update SD pref but not invoke SD's engine */
  if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE ||
      ph_ptr->oprt_mode_send_time != CMPH_MAX_UPTIME)
  {
    pref_info->pref_reas = SD_SS_PREF_REAS_RESTORE;

    /* For legacy mode, map ss max to main+hybr1 */
    if(subs_feature_mode == SYS_SUBS_FEATURE_MODE_NORMAL &&
      cm_ss == CM_SS_MAX)
    {
      if((!cmmsc_force_all_stack_required(old_op_mode, state_machine->op_mode)))
      {
        /* if HYBR-1 stack is available, also force on HYBR-1 */
        if (cmph_ptr()->subs_stack_map[state_machine->asubs_id][1] == CM_SS_NONE)
        {
          event = CMMSC_EVENT_SYS_SEL_PREF_MAIN;
        }
        else
        {
          event = CMMSC_EVENT_SYS_SEL_PREF_MAIN_HYBR1;
        }
      }
      else
      {
        event = CMMSC_EVENT_SYS_SEL_PREF_MAIN_HYBR1;
      }
    }
    else if (subs_feature_mode == SYS_SUBS_FEATURE_MODE_SGLTE ||
           subs_feature_mode == SYS_SUBS_FEATURE_MODE_SVLTE ||
           subs_feature_mode == SYS_SUBS_FEATURE_MODE_SRLTE)
    {
      event = CMMSC_EVENT_SYS_SEL_PREF_MAX;
    }
    /*
    else if (cm_stack_id == 1 && subs_feature_mode == SYS_SUBS_FEATURE_MODE_SGLTE)
    {
      event = CMMSC_EVENT_SYS_SEL_PREF_HYBR2;
    }*/
    else if (cm_ss == CM_SS_HYBR_1) /* NORMAL / SVLTE */
    {
      event = CMMSC_EVENT_SYS_SEL_PREF_HYBR1;
    }
    else if (cm_stack_id == 2) /* NORMAL /SVLTE */
    {
      event = CMMSC_EVENT_SYS_SEL_PREF_HYBR2;
    }
    else if(cm_stack_id == 0)
    {
      event = CMMSC_EVENT_SYS_SEL_PREF_MAIN;
    }
  }
  else if (cm_ss == CM_SS_MAX)
  {
    if (!cmmsc_force_all_stack_required(old_op_mode, state_machine->op_mode))
    {
      event = CMMSC_EVENT_SYS_SEL_PREF_MAIN;
    }
    /* If volte call is active, not force user pref on hybr2 */
    else if (cmmsc_is_volte_active_on_hybr2(state_machine) && 
           orig_mode == SD_SS_ORIG_MODE_NONE)
    {
      event = CMMSC_EVENT_SYS_SEL_PREF_MAIN_HYBR1;
    }
    else
    {
      event = CMMSC_EVENT_SYS_SEL_PREF_MAX;
    }
  }
  else if ((cm_stack_id == 1 && subs_feature_mode == SYS_SUBS_FEATURE_MODE_SGLTE) ||
            cm_stack_id == 2)
  {
    event = CMMSC_EVENT_SYS_SEL_PREF_HYBR2;
  }
  else if (cm_ss == CM_SS_HYBR_1)
  {
    event = CMMSC_EVENT_SYS_SEL_PREF_HYBR1;
  }
  else if(cm_stack_id == 0)
  {
    event = CMMSC_EVENT_SYS_SEL_PREF_MAIN;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If ss is not active, force pref_reason to restore */
  if (cmph_is_msim()
    && !(ph_ptr->active_subs & BM(state_machine->asubs_id))
    && pref_info->pref_reas != SD_SS_PREF_REAS_ORIG_END)
  {
    pref_info->pref_reas = SD_SS_PREF_REAS_RESTORE;
  }
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* If preference changed on HYBR-2 because of emergency call and CMMSC was
  ** not in emergency call before then
  ** - Update the state to be in emergency
  ** If we're in Dual-stack then:
  ** - Force preference on both stacks to shutdown main.
  */
  if(subs_feature_mode == SYS_SUBS_FEATURE_MODE_SGLTE)
  {
    if (cmcall_emergency_call_active() &&
        state_machine->stack_common_info.is_in_emergency_call == FALSE)
    {
      state_machine->stack_common_info.is_in_emergency_call = TRUE;
      if(state_machine->op_mode == CMMSC_OP_MODE_SGLTE_DUAL_STACK)
      {
        event = CMMSC_EVENT_DUAL_STK_EMERG_CALL;
      }
      /* Set stack capability also if emergency call is stopped */
      cmmsc_set_stack_capability(state_machine);
    }
    /* Else, if CMMSC is no longer in emergency call but CMMSC still in emergency
    ** capability, then:
    ** - Update the state to non-emergency
    ** - Force preference on both stacks to shut down hybr-2
    */
    else if(!cmcall_emergency_call_active() &&
        state_machine->stack_common_info.is_in_emergency_call == TRUE)
    {
      state_machine->stack_common_info.is_in_emergency_call = FALSE;
      if(state_machine->op_mode == CMMSC_OP_MODE_SGLTE_DUAL_STACK)
      {
        event = CMMSC_EVENT_DUAL_STK_EMERG_CALL;
      }
      /* Set stack capability also if emergency call is stopped */
      cmmsc_set_stack_capability(state_machine);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* If UE moving from CSFB --> SGLTE, and UE was doing Manual Selection on
  ** MAIN, then we have to make sure UE doing Manual Selection on Hybr-2 also.
  */
  if(state_machine->op_mode != old_op_mode)
  {
    CM_MSG_HIGH_3("MSC: pref_chgd: net_sel_mode=%d, old_op_mode=%d, new_op_mode=%d",
              ph_ptr->main_stack_info.pref_info.network_sel_mode_pref,
              old_op_mode,state_machine->op_mode);
  }

  if ( (ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL &&
        ph_ptr->csg_id == SYS_CSG_ID_INVALID) &&
       !sys_plmn_id_is_undefined (ph_ptr->main_stack_info.pref_info.plmn) &&
        pref_info->pref_reas == SD_SS_PREF_REAS_USER )
  {
    /* Moving from CSFB --> SGLTE */
    if( old_op_mode != CMMSC_OP_MODE_SGLTE_SINGLE_STACK &&
        (cmmsc_is_op_mode_multi_stacks(old_op_mode) &&
        !cmmsc_is_op_mode_multi_stacks(state_machine->op_mode)))
    {
      CM_MSG_HIGH_0("SC_SGLTE: Changing pref_reas to USER_RESEL");
      pref_info->pref_reas = SD_SS_PREF_REAS_USER_RESEL;
    }
  }

  /* Get the top of the priority queue */
  ph_orig_top_ptr = cmtask_orig_para_get_top(cm_ss);

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_0("MSC: ph_orig_top_ptr null ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* For SGLTE, if UE_MODE has been changed which triggers capability changed,
  ** then CM has to perform ps_detach.
  ** For SVLTE, preference is constant and capability remains the same unless
  ** preference changed, hence CM don't need to do explicit detach.
  */
  /* PS_DETACH request is sent to MMOC if top of queue is either CM_ACT_TYPE_PH_OBJ 
     or CM_ACT_TYPE_DATA_CALL. So evaluation also restricted to that 
   */
  if( is_new_policy_tobe_read && ph_ptr->is_ps_detach_required == FALSE &&
    (ph_orig_top_ptr->act_type == CM_ACT_TYPE_PH_OBJ || 
     ph_orig_top_ptr->act_type == CM_ACT_TYPE_DATA_CALL))
  {
    cmmsc_evaluate_ps_detach_required_sglte(state_machine,cmmsc_mapping_ss_to_stack_idx(cm_ss),old_stack_pref);
  }
  
  cmmsc_update_stack_preference(
    state_machine,
    pref_info->mode_pref,
    pref_info->band_pref,
    pref_info->lte_band_pref,
    pref_info->tds_band_pref);
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* if all the conditions meet, restore 3GPP back to phone object 
  */
  if( is_new_policy_tobe_read )
  {
    cmmsc_auto_restore_3gpp_policy_chgd(state_machine, cm_ss,
                                          &(pref_info->mode_pref));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmmsc_auto_set_hybr1_pref(state_machine, old_op_mode, pref_info);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Package the event with parameters */
  event_ss_pref.event = event;
  event_ss_pref.state_machine = state_machine;
  event_ss_pref.cmd = cmd;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Consult Routing Table and return action */
  cmmsc_state_machine_process(state_machine,
                              (cmmsc_event_base_s_type*)&event_ss_pref);

}

/*===========================================================================

FUNCTION cmmsc_mapping_stack_id_to_ss

DESCRIPTION
  The function maps stack index to cm ss.

DEPENDENCIES
  None

RETURN VALUE
  stack index

SIDE EFFECTS
  None

===========================================================================*/
cm_ss_e_type  cmmsc_mapping_stack_id_to_ss(
  int                       stack_idx,
  cmmsc_state_machine_s_type *state_machine
)
{
  #if defined(FEATURE_MMODE_TRIPLE_SIM)
  if(state_machine->asubs_id >= SYS_MODEM_AS_ID_1 && state_machine->asubs_id <= SYS_MODEM_AS_ID_3)
  #else  
  if(state_machine->asubs_id >= SYS_MODEM_AS_ID_1 && state_machine->asubs_id <= SYS_MODEM_AS_ID_2)
  #endif
  {
  return (cmph_ptr()->subs_stack_map[state_machine->asubs_id][stack_idx]);
}
  else
  {
    CM_MSG_HIGH_2("Invalid as_id %d and stack_id %d",state_machine->asubs_id, stack_idx);
    return CM_SS_NONE;
  }
}

/*===========================================================================

FUNCTION cmmsc_mapping_ss_to_stack_idx

DESCRIPTION
  The function maps cm ss to stack index.

DEPENDENCIES
  None

RETURN VALUE
  stack index

SIDE EFFECTS
  None

===========================================================================*/
int cmmsc_mapping_ss_to_stack_idx(cm_ss_e_type ss)
{
  if(ss == CM_SS_MAX || ss == CM_SS_NONE)
    ss = CM_SS_MAIN;
    
  return CMPH_SS_STACK_IDX(ss,cmph_ptr());
}

/*===========================================================================

FUNCTION cmmsc_is_device_oprt_in_LTE

DESCRIPTION
   Determine if the device will be working in LTE.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmmsc_is_device_oprt_in_LTE(

   cmmsc_state_machine_s_type     *state_machine
)
{

  return( SD_MODE_CONTAIN( cmph_map_cm_mode_pref_to_sd_mode_pref(
                       state_machine->stack_common_info.preference.mode_pref),
                           SD_SS_MODE_PREF_LTE)
          &&
          (state_machine->stack_common_info.ue_mode_capability & SYS_SYS_MODE_MASK_LTE)
          &&
          cmph_ptr()->true_oprt_mode == SYS_OPRT_MODE_ONLINE );
}
/*===========================================================================

FUNCTION cmmsc_is_oper_flag_buff

DESCRIPTION
   Determine if the is_operational flag on Hybrid2 stack should be buffered.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmmsc_is_oper_flag_buff(

   cmmsc_state_machine_s_type     *state_machine
)
{
  cmss_s_type  *ss_ptr  =  cmss_ptr();

  /*
  ** 1. Hybrid 2 is currently in LTE service.
  ** 2. Hybrid 2 is currently in Sub1 SXLTE operation                          .
  ** 3. LTE still part of the device capability.                                                                          .
  ** 4. The feature is enabled.
  ** 5. Device is not exiting ONLINE mode.
  */
  return( sys_srv_status_is_srv(ss_ptr->hybr_gw_srv_status)           &&
          ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_LTE                &&
          cmph_is_subs_feature_mode_1x_sxlte(state_machine->asubs_id) &&
          ss_ptr->hold_hy2_oper_flag_timer_value > 0                  &&
          cmmsc_is_device_oprt_in_LTE(state_machine)                  &&
          !cmtask_orig_para_is_obj_on_any_q(CM_ACT_TYPE_EMERG_CALL)
        );
}
/*===========================================================================

FUNCTION cmmsc_update_cmss_is_operational

DESCRIPTION
   Update cmss is_operational for the specified stack.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmmsc_update_cmss_is_operational(
  cmmsc_state_machine_s_type     *state_machine,
     int      stack_idx,
     boolean  is_operational
)
{

  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx,state_machine);
  boolean is_operational_flag_chg=FALSE;

  switch(ss)
  {
    case CM_SS_MAIN:
       if(cmss_ptr()->info.is_main_operational != is_operational)
       {
         is_operational_flag_chg = TRUE;
       }
    cmss_ptr()->info.is_main_operational = is_operational;
      break;

  case CM_SS_HYBR_2:
       /* If Hybrid 2 stack is being deactivated, check if the flag value
       ** needs to be buffered and start the timer accordingly.
       */
       if ( cmss_ptr()->info.is_hybr_gw_operational == TRUE &&
            is_operational == FALSE                         &&
            cmmsc_is_oper_flag_buff(state_machine) )
       {
         cmss_start_hy2_oper_flag_timer();
       }
       /* If Hybrid 2 stack is being activated, reset the buffer timer.*/
       else if ( cmss_ptr()->info.is_hybr_gw_operational == FALSE &&
                 is_operational == TRUE )
       {
         cmss_ptr()->hold_hy2_oper_flag_uptime = CMSS_NO_SRV_UPTIME_NONE;
       }
       if(cmss_ptr()->info.is_hybr_gw_operational != is_operational)
       {
         is_operational_flag_chg = TRUE; 
       }
    cmss_ptr()->info.is_hybr_gw_operational = is_operational;
    cmss_reset_cell_info_upon_hybr_gw_change(is_operational,state_machine->asubs_id);
    break;

  case CM_SS_HYBR_1:
       if(cmss_ptr()->info.hdr_hybrid != is_operational)
       {
         is_operational_flag_chg = TRUE;
       }
    cmss_ptr()->info.hdr_hybrid = is_operational;
    break;

  case CM_SS_HYBR_3:
       if(cmss_ptr()->info.is_gw3_operational != is_operational)
       {
         is_operational_flag_chg = TRUE;
       }
    cmss_ptr()->info.is_gw3_operational = is_operational;
    break;

  default:
    break;
  }

  if(is_operational_flag_chg)
  {
    CM_MSG_HIGH_2("Change reporting operational_radio_stacks to PM for subs_id = %d, is_operational=%d",state_machine->asubs_id,is_operational);
    cmpmprx_report_operational_radio_stacks(state_machine->asubs_id);
  }
}

/*===========================================================================

  This function returns a boolean value:
  TRUE : If stack pointed by ss is active and being attempted
  FALSE: If stack is not active and no service acquisition is taking place
  
===========================================================================*/
static boolean cmmsc_cmss_is_operational(

  cm_ss_e_type ss

)
{
  switch(ss)
  {
    case CM_SS_MAIN:
      return cmss_ptr()->info.is_main_operational;
    case CM_SS_HYBR_2:
      return cmss_ptr()->info.is_hybr_gw_operational;
    case CM_SS_HYBR_1:
      return cmss_ptr()->info.hdr_hybrid;
    case CM_SS_HYBR_3:
      return cmss_ptr()->info.is_gw3_operational;
    default:
      return FALSE;
  }
}

/*===========================================================================

FUNCTION cmmsc_action_force_top

DESCRIPTION
  Force the top priority queue preference on the specified stack.

DEPENDENCIES
  Stacks must have already been initialized with cmmsc_init_multi_stacks().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_action_force_top(
    cmmsc_state_machine_s_type *state_machine,
    int                         stack_idx,
    sd_ss_pref_reas_e_type      pref_reas
)
{
  cmmsc_action_force_top_addl(state_machine, stack_idx, pref_reas, NULL);
}

/*===========================================================================

FUNCTION cmmsc_action_force_top_ext

DESCRIPTION
  Force the top priority queue preference on the specified stack.

DEPENDENCIES
  Stacks must have already been initialized with cmmsc_init_multi_stacks().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_action_force_top_ext(
    cmmsc_state_machine_s_type        *state_machine,
    cmmsc_event_force_pref_s_type     *evt_package,
    int                                stack_idx
)
{
  cmph_s_type                *ph_ptr = cmph_ptr();
  cm_ss_e_type               cm_ss = CM_SS_MAX;
  cm_orig_q_s_type           *ph_orig_top_ptr = NULL;
  cm_preference_info_s_type  pref_info;

  if (state_machine == NULL || evt_package == NULL)
  {
    CM_ERR_2("MSC: null ptr %d %d", state_machine, evt_package);
    return;
  }


  cm_ss = cmmsc_mapping_stack_id_to_ss(stack_idx, state_machine);
  ph_orig_top_ptr = cmtask_orig_para_get_top(cm_ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure top orig ptr is not null
  */
  if (ph_orig_top_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update base on evt package data */
  pref_info.pref_reas = evt_package->pref_reas;
  pref_info.hybr_pref = evt_package->hybr_pref;
  pref_info.orig_mode = evt_package->orig_mode;

  /* Populate the preference base on top pri queue */
  pref_info.mode_pref = ph_orig_top_ptr->orig->orig_mode_pref;
  pref_info.band_pref = ph_orig_top_ptr->orig->orig_band_pref;
  pref_info.lte_band_pref = ph_orig_top_ptr->orig->orig_lte_band_pref;
  pref_info.tds_band_pref = ph_orig_top_ptr->orig->orig_tds_band_pref;
  pref_info.prl_pref = ph_orig_top_ptr->orig->orig_prl_pref;
  pref_info.roam_pref = ph_orig_top_ptr->orig->orig_roam_pref;
  pref_info.srv_domain_pref = ph_orig_top_ptr->orig->orig_srv_domain_pref;
  pref_info.acq_order_pref = ph_ptr->main_stack_info.pref_info.acq_order_pref;
  pref_info.rat_acq_order_ptr
    = cmph_get_acq_pri_order_per_sub(cm_ss);
  pref_info.camp_mode_pref =
    cmph_map_cm_net_sel_mode_pref_to_camp_pref(
    ph_ptr->main_stack_info.pref_info.network_sel_mode_pref);
  pref_info.sys_id.sys_id.id.plmn  = ph_ptr->main_stack_info.pref_info.plmn;
  pref_info.sys_id.sys_id.id_type = SYS_SYS_ID_TYPE_UMTS;
  pref_info.sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;
  pref_info.otasp_band = cmph_map_otasp_act_code_to_sd_band(CM_OTASP_ACT_CODE_NONE);
  pref_info.otasp_blksys = cmph_map_otasp_act_code_to_sd_blksys(CM_OTASP_ACT_CODE_NONE);
  pref_info.avoid_type = SD_SS_AVOID_SYS_IDLE;
  pref_info.avoid_time = 0;

  /* Update hybr1 specific data */
  pref_info.hybr1_pref.hybr_orig_mode = evt_package->orig_mode;
  pref_info.hybr1_pref.hybr_mode_pref = ph_orig_top_ptr->orig->orig_mode_pref;
  pref_info.pref_update_reason = ph_orig_top_ptr->orig->pref_update_reason;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Force preference to the stack */
  switch(stack_idx)
  {
    case 0:
    case 1:
    case 2:
      cmmsc_send_sys_sel_pref_chgd_cmd(state_machine, stack_idx, &pref_info, NULL);
      break;

    default:
      CM_ERR_1("MSC: Invalid stack_idx %d",stack_idx);
  }

}

/*===========================================================================

FUNCTION cmmsc_action_force_top_addl

DESCRIPTION
  Force the top priority queue preference on the specified stack.

DEPENDENCIES
  Stacks must have already been initialized with cmmsc_init_multi_stacks().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_action_force_top_addl(
    cmmsc_state_machine_s_type *state_machine,
    int                         stack_idx,
    sd_ss_pref_reas_e_type      pref_reas,
    const cmmsc_addl_pref_sys_chgd_mmoc_cmd_s_type   *addl_ptr
)
{
  cmph_s_type                   *ph_ptr          = cmph_ptr();
  cm_ss_e_type                   cm_ss = CM_SS_MAX ;
  cm_orig_q_s_type              *ph_orig_top_ptr = NULL ;
  cm_preference_info_s_type   pref_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (state_machine == NULL)
     return;

  cm_ss = cmmsc_mapping_stack_id_to_ss(stack_idx, state_machine);

  

  /* Make sure top orig ptr is not null
  */ 
  ph_orig_top_ptr = cmtask_orig_para_get_top(cm_ss);

  if (ph_orig_top_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Populate the preference base on top pri queue */
  pref_info.pref_reas = pref_reas;
  pref_info.mode_pref = ph_orig_top_ptr->orig->orig_mode_pref;

  if(CMPH_SS_NETWORK_SEL_MODE_PREF(state_machine->asubs_id, ph_ptr ) == CM_NETWORK_SEL_MODE_PREF_MANUAL && 
	  ph_orig_top_ptr->orig->orig_mode == SD_SS_ORIG_MODE_NONE )

  {
    pref_info.orig_mode = SD_SS_ORIG_MODE_MANUAL_IDLE;
  }
  else
  {
  pref_info.orig_mode = ph_orig_top_ptr->orig->orig_mode;
  }

  pref_info.band_pref = ph_orig_top_ptr->orig->orig_band_pref;
  pref_info.lte_band_pref = ph_orig_top_ptr->orig->orig_lte_band_pref;
  pref_info.tds_band_pref = ph_orig_top_ptr->orig->orig_tds_band_pref;
  pref_info.hybr_pref = ph_orig_top_ptr->orig->orig_hybr_pref;
  pref_info.prl_pref = ph_orig_top_ptr->orig->orig_prl_pref;
  pref_info.roam_pref = ph_orig_top_ptr->orig->orig_roam_pref;
  pref_info.srv_domain_pref = ph_orig_top_ptr->orig->orig_srv_domain_pref;
  pref_info.acq_order_pref = ph_ptr->main_stack_info.pref_info.acq_order_pref;
  pref_info.rat_acq_order_ptr
    = cmph_get_acq_pri_order_per_sub(cm_ss);
  pref_info.camp_mode_pref =
    cmph_map_cm_net_sel_mode_pref_to_camp_pref(ph_ptr->main_stack_info.pref_info.network_sel_mode_pref);
  pref_info.sys_id.sys_id.id.plmn  = ph_ptr->main_stack_info.pref_info.plmn;
  pref_info.sys_id.sys_id.id_type = SYS_SYS_ID_TYPE_UMTS;
  pref_info.sys_id.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;
  pref_info.otasp_band = cmph_map_otasp_act_code_to_sd_band(CM_OTASP_ACT_CODE_NONE);
  pref_info.otasp_blksys = cmph_map_otasp_act_code_to_sd_blksys(CM_OTASP_ACT_CODE_NONE);
  pref_info.avoid_type = SD_SS_AVOID_SYS_IDLE;
  pref_info.avoid_time = 0;

  pref_info.hybr1_pref.hybr_mode_pref = ph_orig_top_ptr->orig->orig_mode_pref;
  //pref_info.hybr1_pref.hybr_orig_mode = ph_orig_top_ptr->orig->orig_mode;
  pref_info.pref_update_reason = ph_orig_top_ptr->orig->pref_update_reason;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Force preference to the stack */
  switch(stack_idx)
  {
    case 0:
    case 1:
    case 2:
      cmmsc_send_sys_sel_pref_chgd_cmd(state_machine,stack_idx, &pref_info, addl_ptr);
      break;

    default:
      CM_ERR_1("MSC: Invalid stack_idx %d",stack_idx);
  }

}

static void cmmsc_action_sub(
       const cmmsc_event_sub_chg_s_type *evt_pkg
)
{
  /*
  boolean                 hybr2_prot_sub_chg = FALSE;
  mmoc_subs_data_s_type  *hybr2_sub_data     = NULL;

  boolean               hybr3_prot_sub_chg = FALSE;
  mmoc_subs_data_s_type  *hybr3_sub_data     = NULL;
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(evt_pkg != NULL);
  if (evt_pkg == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
}


/*===========================================================================

FUNCTION cmmsc_action_kick_hybr2

DESCRIPTION
  This function to kick hybr2 stack when get net is completed on main

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void cmmsc_action_kick_hybr2(
    cmmsc_state_machine_s_type *msc,
    int                        stack_idx
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc );
  
  CM_MSG_MED_1("cmmsc_action_kick_hybr2(), ss=%d", ss);

  switch(ss)
  {

    case CM_SS_HYBR_2:
    {
      if(!cmmsc_is_hybr2_kick_allowed(msc))
      {
        cmmsc_auto_update_is_kick_hybr2_pending(msc, TRUE);
      }
      else
      {
        cmmsc_action_pending_kick(msc, stack_idx, TRUE);
      }

      break;
    }

   default:
      break;
  }
}

/*===========================================================================

FUNCTION cmsoa_read_svlte_capable_lte_bands

DESCRIPTION
  This function reads RF configuration and determines the LTE bands which are
  capable of SVLTE operation.

DEPENDENCIES
  none

RETURN VALUE
  sd_ss_band_pref_e_type : the bands which are capable of SVLTE operation.

SIDE EFFECTS
  none

===========================================================================*/
static sys_lte_band_mask_e_type cmmsc_read_svlte_capable_lte_bands()
{
  sys_lte_band_mask_e_type         ret_bands = SYS_LTE_BAND_MASK_CONST_ANY;

  #if !defined CM_DEBUG
  rfm_devices_configuration_type rf_device_config;
  uint16                         prx_devices=0;
  int                            i=0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Read restrictions from RF API. If any failure return LTE_ANY */
  if (!rfm_get_devices_configuration(&rf_device_config))
  {
    CM_MSG_HIGH_0("Unable to read RF device config");
    return ret_bands;
  }

  CM_MSG_HIGH_1("num_restrictions %d",rf_device_config.concurrency_restrictions.num_restrictions);
  if ( rf_device_config.concurrency_restrictions.num_restrictions == 0 ||
       rf_device_config.concurrency_restrictions.restriction_table == NULL )
  {
    return ret_bands;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Restrictions successfully read. Parse the data for determining the LTE
  ** bands supporting SVLTE operation.
  */
  /* Determine all the PRX devices. */
  for (i=0;i< RFM_MAX_DEVICES;i++)
  {
    if (rf_device_config.device_support[i].associated_device != RFM_INVALID_DEVICE)
    {
      prx_devices |= (1<<i);
    }
  }

  CM_MSG_HIGH_1("PRX Device bitmask 0x%x",prx_devices);

  /* If in restriction table, mode=LTE and other mode=1x and other device=PRX.
  ** the lte band listed cannot be supported in SVLTE.
  */
  for (i=0;i<rf_device_config.concurrency_restrictions.num_restrictions;i++)
  {
    /* If device 1 has LTE band restriction and device 2 has 1x band restriction
    ** and Device 2 is PRX device, remove the LTE band from the SVLTE supported
    ** bands.
    */
    if (rf_device_config.concurrency_restrictions.restriction_table[i].mode1 == RFM_LTE_MODE &&
        rf_device_config.concurrency_restrictions.restriction_table[i].mode2 == RFM_1X_MODE &&
        (prx_devices & (1<< rf_device_config.concurrency_restrictions.restriction_table[i].device2)))
    {
      sys_sband_lte_e_type lte_sband = sys_map_lte_band_to_sband(rf_device_config.concurrency_restrictions.restriction_table[i].band1);

      if (lte_sband != SYS_SBAND_LTE_EUTRAN_BAND_MAX)
      {
        SYS_LTE_BAND_MASK_REMOVE_BAND(&ret_bands,lte_sband );
      }
    }

    /* If device 2 has LTE band restriction and device 1 has 1x band restriction
    ** and Device 1 is PRX device, remove the LTE band from the SVLTE supported
    ** bands.
    */
    if (rf_device_config.concurrency_restrictions.restriction_table[i].mode2 == RFM_LTE_MODE &&
        rf_device_config.concurrency_restrictions.restriction_table[i].mode1 == RFM_1X_MODE &&
        (prx_devices & (1<< rf_device_config.concurrency_restrictions.restriction_table[i].device1)))
    {
      sys_sband_lte_e_type lte_sband = sys_map_lte_band_to_sband(rf_device_config.concurrency_restrictions.restriction_table[i].band2);
      if (lte_sband != SYS_SBAND_LTE_EUTRAN_BAND_MAX)
      {
        SYS_LTE_BAND_MASK_REMOVE_BAND(&ret_bands,lte_sband );
      }
    }
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #else
  #error code not present
#endif

  return ret_bands;
}

/*===========================================================================

FUNCTION cmmsc_init_stack_common

DESCRIPTION
  This function should be called to initialize stack common info.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_init_stack_common(cmmsc_state_machine_s_type  *state_machine)
{
  if(state_machine == NULL)
    return;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  state_machine->stack_common_info.is_in_emergency_call = FALSE;
  state_machine->stack_common_info.is_hybr1_diabled = FALSE;
  state_machine->stack_common_info.stack_kick.is_kick_hdr_pending = FALSE;
  state_machine->stack_common_info.stack_kick.is_kick_gw = FALSE;
  state_machine->stack_common_info.stack_kick.is_hybr_2_kick_pending = FALSE;
  state_machine->stack_common_info.stack_kick.is_stack_sync_bsr_pending = FALSE;
  state_machine->stack_common_info.svlte_capable_lte_bands = cmmsc_read_svlte_capable_lte_bands();

  state_machine->stack_common_info.timer.acq_hybr_1_uptime = CMMSC_UPTIME_NONE;
  state_machine->stack_common_info.timer.lost_main_uptime= CMMSC_UPTIME_NONE;
  state_machine->stack_common_info.timer.avoid_hybr_hdr_uptime= CMMSC_UPTIME_NONE;


  state_machine->stack_common_info.preference.mode_pref = CMPH_PRST_SS_MODE_PREF(state_machine->asubs_id, cmph_ptr());
  state_machine->stack_common_info.preference.band_pref = CMPH_PRST_SS_BAND_PREF(state_machine->asubs_id, cmph_ptr());
  state_machine->stack_common_info.preference.tds_band_pref = CMPH_PRST_SS_TDS_BAND_PREF(state_machine->asubs_id, cmph_ptr());
  state_machine->stack_common_info.preference.lte_band_pref = CMPH_PRST_SS_LTE_BAND_PREF(state_machine->asubs_id, cmph_ptr());
}

/*===========================================================================

FUNCTION cmmsc_init_multi_stacks

DESCRIPTION
  This function should be called to initialize all stacks.

DEPENDENCIES
  cmph_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_init_multi_stacks(cmmsc_state_machine_s_type  *state_machine)
{
  int i;
  if(state_machine == NULL)
    return;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the stack common info */
  cmmsc_init_stack_common(state_machine);

  /* Initialize the phone capability to default values */
  cmmsc_init_mode_and_capability(state_machine);

  /* Initialize stack 0 */
  cmmsc_init_stack_0(state_machine);

  /* Initialized all the stacks - although SGLTE has 2 stacks, SVLTE has 3 stacks
  ** the cmmsc_state_machine has a contanst number of stacks states. For SGLTE, 3rd one
  ** is un-used. (Memory impacts are small).
  */
  for(i = 1; i < CMMSC_STACK_NUM; i++)
  {
    /* Initialize stack 1 */
    cmmsc_init_stack_other(state_machine,i);
  }
}

/*===========================================================================

FUNCTION cmmsc_init_stack_0

DESCRIPTION
  This function should be called to initialize stack 0.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_init_stack_0 (cmmsc_state_machine_s_type *state_machine)
{
  int                 stack_idx         = 0;
  cmmsc_stack_s_type         *stack_pointer;

  if(state_machine == NULL)
    return;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  stack_pointer = &(state_machine->stack[stack_idx]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize state machine for the stack
   */
  stack_pointer->stack_id = stack_idx;
  stack_pointer->state  = CMMSC_STACK_STATE_NULL;
  cmmsc_set_stack_capability(state_machine);
  state_machine->stack[0].prev_registration_capability =
                                            state_machine->stack[0].capablity;
  stack_pointer->addl_action.action = 0;
  stack_pointer->addl_action.ue_mode = SYS_UE_MODE_NONE;

}

/*===========================================================================

FUNCTION cmmsc_init_stack_other

DESCRIPTION
  This function should be called to initialize the specified stack other than 0.  Stack 0 is
  initialized in cmmsc_init_stack_0.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_init_stack_other (cmmsc_state_machine_s_type *state_machine,
                             int stack_idx)
{

  cmmsc_stack_s_type         *stack_pointer;

  if (state_machine == NULL)
     return;

  stack_pointer = &(state_machine->stack[stack_idx]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ?Need to update mode pref mask
  ** *cmph_get_is_kicked_hybr_2_ptr() = FALSE;
  */


  /* Initialize cmss is_operational
   */
  cmmsc_update_cmss_is_operational(state_machine, stack_idx, FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize state machine for the stack
   */

  stack_pointer->stack_id = stack_idx;
  stack_pointer->state  = CMMSC_STACK_STATE_NULL;
  cmmsc_set_stack_capability(state_machine);
  state_machine->stack[stack_idx].prev_registration_capability =
                                            state_machine->stack[0].capablity;
  
  stack_pointer->addl_action.action = 0;
  stack_pointer->addl_action.ue_mode = SYS_UE_MODE_NONE;
}


static void cmmsc_action_power_down(int stack_id,
                                    cmmsc_state_machine_s_type *state_machine)
{
  if (stack_id == 0)
  {
    /* Initialize the stack common info */
    cmmsc_init_stack_common(state_machine);
  }

  switch(stack_id)
  {
    case 0:
      /* Reinit stack 0 */
      cmmsc_init_stack_0(state_machine);
      break;

    case 1:
    case 2:
      /* Reinit stack 1 */
      cmmsc_init_stack_other(state_machine,stack_id);
      break;

    default:
      break;
  }
}

/*===========================================================================
FUNCTION cmmsc_is_valid_ss_for_ps_detach

DESCRIPTION
  This function checks if the SS is valid to send a PS detach on based on
  UE feature mode.

DEPENDENCIES
  SUBS FEATURE mode must be initialized properly.

RETURN VALUE
  TRUE:  If SS is valid
  FALSE: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
static boolean cmmsc_is_valid_ss_for_ps_detach( 
  sys_modem_as_id_e_type  as_id,
    /* AS_ID of the FEATURE MODE */

  cm_ss_e_type            ss
    /* SS Value to validate */
)
{
   sys_subs_feature_t subs_feature_mode = cmph_get_subs_feature_mode( as_id );

   boolean is_valid = FALSE;

   CM_MSG_HIGH_3( "MSC: Validate ss = %d on asubs_id = %d for PS Detach. subs_f_mode %d", ss, as_id, subs_feature_mode );

   switch ( subs_feature_mode )
   {
     case SYS_SUBS_FEATURE_MODE_SGLTE:
     {
       if ( ss == CM_SS_MAIN )
       {
         is_valid = TRUE;
       }
       break;
     }

     case SYS_SUBS_FEATURE_MODE_SVLTE:
     case SYS_SUBS_FEATURE_MODE_SRLTE:
     {
       if(cmmsc_cmss_is_operational(CM_SS_HYBR_2))
       {
          CM_MSG_HIGH_1("is ps detach , ss=%d",ss);
#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)
          CM_MSG_HIGH_2( "cmregprx_state[0]=%d,cmregprx_state[1]=%d,",cmregprx_get_info_ptr()->stack_info[0].state,
              cmregprx_get_info_ptr()->stack_info[1].state);
#endif
          if((ss == CM_SS_HYBR_2
#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)
            && cmregprx_get_info_ptr()->stack_info[0].state != CMREGPRX_STATE_ACTIVE
#endif
             ) ||
             (ss == CM_SS_MAIN
#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)
            && cmregprx_get_info_ptr()->stack_info[1].state != CMREGPRX_STATE_ACTIVE
#endif
             )
            )
         {
           is_valid = TRUE;
            CM_MSG_HIGH_1("ps detach to be sent, ss=%d",ss);
         }
       }

       else if(ss != CM_SS_HYBR_1)
       {
         is_valid = TRUE;
       }
       break;
     }

     case SYS_SUBS_FEATURE_MODE_NORMAL:
     {
       if ( ss != CM_SS_HYBR_1 )
       {
         is_valid = TRUE;
       }
       break;
     }
     default:
     {
       CM_MSG_HIGH_1( "Invalid SUBS FEATURE MODE %d", subs_feature_mode );
     }
   }

   return is_valid;
}



/*===========================================================================
FUNCTION cmmsc_update_addl_action_ue_mode_switch

DESCRIPTION
  This function checks and updates addl_action with UE_MODE_SWITCH if needed

DEPENDENCIES
  

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_update_addl_action_ue_mode_switch(
  cmmsc_state_machine_s_type      *state_machine,
  cm_ss_e_type                    ss,
  sys_addtl_action_s_type         *addl_action,
    cm_preference_info_s_type     *pref_info)
{
  if (addl_action == NULL || state_machine == NULL)
  {
    CM_ERR_2("null ptr addl_action %x state_machine %x",addl_action,state_machine);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* SRLTE handling */

  /* Send the ue mode indication to NAS for srlte */
  if (cmph_is_subs_feature_mode_srlte(state_machine->asubs_id) &&
     state_machine->stack_common_info.ue_mode_forced == FALSE)
  {
    if (ss == CM_SS_MAIN)
      {
        addl_action->action |= (uint8)MMOC_PREF_SYS_ADDL_ACTION_UE_MODE_SWITCH;
        addl_action->ue_mode = state_machine->stack_common_info.ue_mode;
        addl_action->is_ue_mode_substate_srlte = cmmsc_auto_is_hybr2_on(state_machine->op_mode);
      CM_MSG_HIGH_3 ("SRLTE: ue_mode_chgd: ss=%d, addl_action %d, ue_mode %d", 
                           ss, addl_action->action, addl_action->ue_mode);
        state_machine->stack_common_info.ue_mode_forced = TRUE;
      state_machine->stack_common_info.is_ue_mode_chg_in_progress = TRUE;
      }
    }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* SVLTE + G handling */

  /* Only set addtional action on main */
  if(cmph_is_sxlte() && 
    cmph_is_subs_feature_mode_svlte(state_machine->asubs_id) &&
    state_machine->stack_common_info.ue_mode_forced == FALSE
  )
  {
    /* Reset ue_mode_forced if ss == main*/
    if (ss == CM_SS_MAIN)
    {
      addl_action->action |= (uint8)MMOC_PREF_SYS_ADDL_ACTION_UE_MODE_SWITCH;
      addl_action->ue_mode = state_machine->stack_common_info.ue_mode;
      addl_action->is_ue_mode_substate_srlte = FALSE;
      CM_MSG_HIGH_3 ("SVLTE_G: ue_mode_chgd:   ss=%d, addl_action %d, ue_mode %d", 
                       ss, addl_action->action, addl_action->ue_mode);
      state_machine->stack_common_info.ue_mode_forced = TRUE;
      state_machine->stack_common_info.is_ue_mode_chg_in_progress = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* SGLTE handling */

#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)
  if( cmph_is_subs_feature_mode_sglte(state_machine->asubs_id) && ss == CM_SS_MAIN &&
      state_machine->stack_common_info.ue_mode_forced == FALSE && cmph_ptr()->true_oprt_mode == SYS_OPRT_MODE_ONLINE)
  {
    addl_action->action |= (uint8)MMOC_PREF_SYS_ADDL_ACTION_UE_MODE_SWITCH;
    addl_action->ue_mode = state_machine->stack_common_info.ue_mode;
    addl_action->is_ue_mode_substate_srlte = FALSE;
    CM_MSG_HIGH_3 ("SC_SGLTE: ue_mode_chgd: ss=%d, addl_action %d, ue_mode %d", ss, addl_action->action, addl_action->ue_mode);
    state_machine->stack_common_info.ue_mode_forced = TRUE;
  }

  //moving from dual-stack --> single-stack, we need to send STOP_MODE on both stacks.
  //sendin "NO_SYS" on hybrid-2 and UE_MODE_SWITCH reason on MAIN
  if(  cmph_is_subs_feature_mode_sglte(state_machine->asubs_id) && ss == CM_SS_HYBR_2 &&
      state_machine->stack_common_info.ue_mode_forced == FALSE &&
      pref_info->pref_reas != SD_SS_PREF_REAS_RESTORE &&
      cmmsc_is_op_mode_multi_stacks(
         cmmsc_map_ue_mode_pm2msc(cmregprx_get_stack_info_ptr(cmph_map_cm_ss_type_to_sd_ss_type(CM_SS_MAIN))->ue_mode))  &&
      !cmmsc_is_op_mode_multi_stacks(state_machine->op_mode)
    )
  {

    addl_action->action |= (uint8)MMOC_PREF_SYS_ADDL_ACTION_UE_MODE_SWITCH;
    addl_action->ue_mode = state_machine->stack_common_info.ue_mode;
    addl_action->is_ue_mode_substate_srlte = FALSE;
    CM_MSG_HIGH_3 ("SC_SGLTE: ue_mode_chgd:   ss=%d, addl_action %d, ue_mode %d",
                   ss, addl_action->action, addl_action->ue_mode);
  }
#endif /* defined CM_GW_SUPPORTED ||defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)*/
}

static void cmmsc_action_suspend_stack(int stack_id,
                                    cmmsc_state_machine_s_type *state_machine)
{

  cm_ss_e_type  ss = cmph_ptr()->subs_stack_map[state_machine->asubs_id][stack_id];
  
  if (ss > CM_SS_NONE && ss < CM_SS_MAX)
  {
    cmph_ptr()->ss_susp_info[ss].susp_due_to_emerg = TRUE;
    mmoc_cmd_suspend_ss_new(cmph_map_cm_ss_type_to_sd_ss_type(ss),TRUE
                            ,FALSE, PROT_DEACT_DSDX_SUSP);
    cmph_update_suspend_status(ss, TRUE, TRUE);
  }
  else
  {
    CM_MSG_HIGH_1("Invalid ss %d",ss);
  }
}

/*===========================================================================
  After emergency call is ended, all suspended stacks have to resumed.
  When resume is sent to all suspended stacks in a given subscription:
  if ss=MAIN, clear flag in mmoc & then a pwrup ind is sent from MMOC to SD
  if ss=HYBR1, clear flag in mmoc & wait for 1x acq result to start
  if ss=HYBR2, clear flag in MMOC & start acq by sending pref, if its SVLTE
===========================================================================*/

static void cmmsc_action_resume_stack(int stack_id,
                                    cmmsc_state_machine_s_type *state_machine)
{
  cm_ss_e_type  ss = cmph_ptr()->subs_stack_map[state_machine->asubs_id][stack_id];
  
  if (ss > CM_SS_NONE && ss < CM_SS_MAX)
  {
    cmph_ptr()->ss_susp_info[ss].susp_due_to_emerg = FALSE;
    mmoc_cmd_suspend_ss_new(cmph_map_cm_ss_type_to_sd_ss_type(ss),FALSE
                            ,FALSE, PROT_DEACT_DSDX_SUSP);
    cmph_update_suspend_status(ss, TRUE, FALSE);
  }
  else
  {
    CM_MSG_HIGH_1("Invalid ss %d",ss);
  }

  if(ss == CM_SS_HYBR_2 &&
     cmph_is_subs_feature_mode_1x_sxlte(state_machine->asubs_id))
  {
    /* After resuming, HYBR2 in SVLTE needs to start acquisition along
       with MAIN stack, so kickstart fresh HYBR2 acq */
    cmmsc_action_force_top(state_machine, stack_id, SD_SS_PREF_REAS_USER);
  }
}


void    cmmsc_send_sys_sel_pref_chgd_cmd(
  cmmsc_state_machine_s_type                 *state_machine,
  int                                         stack_idx,
  cm_preference_info_s_type                  *pref_info,
  const cmmsc_addl_pref_sys_chgd_mmoc_cmd_s_type   *addl_ptr
)
{
  cmmsc_stack_s_type              *stack_pointer;
  sd_ss_pref_update_reason_e_type  pref_update_reason  =
                                  SD_SS_PREF_UPDATE_REASON_NONE;
  sys_addtl_action_s_type         addl_action;
  uint16                          sys_sel_pref_req_id = 0;
  sys_csg_id_type                 csg_id              = SYS_CSG_ID_INVALID;
  sys_radio_access_tech_e_type    csg_rat             = SYS_RAT_NONE;
  cm_ss_e_type                    ss                  = CM_SS_NONE;
  cm_orig_q_s_type               *ph_orig_top_ptr     = NULL;
  cmph_s_type                    *ph_ptr              = cmph_ptr();
  cmmsc_stack_capability_s_type   common_pref;
  sd_ss_srv_domain_pref_e_type    srv_domain_to_be_forced = SD_SS_SRV_DOMAIN_PREF_NONE;
  sd_ss_mode_pref_e_type          user_mode_pref      = SD_SS_MODE_PREF_ANY;
  sd_rat_acq_order_s_type         sd_rat_acq_order;
  sd_ss_hybr_pref_e_type          internal_hybr_pref  = SD_SS_HYBR_PREF_NONE;

  sd_ss_pref_reas_e_type          pref_reason;
  sd_ss_pref_camp_mode_e_type     camp_mode;
  sd_ss_orig_mode_e_type          orig_mode = SD_SS_ORIG_MODE_NONE;

  addl_action.action = (uint8)MMOC_PREF_SYS_ADDL_ACTION_NONE;
  addl_action.ue_mode = SYS_UE_MODE_NONE;
  addl_action.is_ue_mode_substate_srlte = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pref_info == NULL)
  {
    CM_ERR_0("MSC: pref info null ptr");
    return;
  }

  if (!((state_machine->asubs_id > SYS_MODEM_AS_ID_NONE && state_machine->asubs_id < MAX_AS_IDS) &&
      (stack_idx >= 0 && stack_idx < MAX_STACK_PER_SUB)))
  {
    CM_ERR_0("MSC: Invalid as_id, stack_id");
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get the correct preference pointer to the correct stack */

  stack_pointer = &state_machine->stack[stack_idx];
  ss = cmmsc_mapping_stack_id_to_ss(stack_idx, state_machine);
  if (ss == CM_SS_NONE)
  {
    CM_MSG_HIGH_0("MSC: ss is NONE");
    return;
  }

  /* Get the top of the priority queue */
  ph_orig_top_ptr = cmtask_orig_para_get_top(ss);

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_0("MSC: ph_orig_top_ptr null ptr");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get pref info */

  pref_reason = pref_info->pref_reas;
  camp_mode = pref_info->camp_mode_pref;
  sd_rat_acq_order =
     cmph_map_cm_rat_acq_order_pref_to_sd (pref_info->rat_acq_order_ptr);

  if (ss == CM_SS_HYBR_1)
  {
    orig_mode = pref_info->hybr1_pref.hybr_orig_mode;
  }
  else
  {
    orig_mode = pref_info->orig_mode;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Terminate the pending get networks request. */

  if( pref_reason == SD_SS_PREF_REAS_ORIG_START_CS ||
      pref_reason == SD_SS_PREF_REAS_ORIG_START_PS ||
      pref_reason == SD_SS_PREF_REAS_ORIG_RESUME   ||
      pref_reason == SD_SS_PREF_REAS_REDIAL_NORM   ||
      pref_reason == SD_SS_PREF_REAS_REDIAL_OPTI   ||
      pref_reason == SD_SS_PREF_REAS_USER          ||
      pref_reason == SD_SS_PREF_REAS_USER_RESEL
    )
  {
    if (cmph_is_subs_feature_mode_sglte(state_machine->asubs_id))
    {
      /* For sglte, get network is only performed on main */
      cmph_term_pending_get_networks_req(CM_SS_MAIN);
    }
    else
    {
      cmph_term_pending_get_networks_req(ss);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* if any network scan is ongoing forcing STACK_SYNC_UP_BSR may cause 
        a new service req which can abort ongoing network scan.
        Considering user triggered network scan being higher priority pend STACK_SYNC_UP_BSR
        Once network_scan is completed pending STACK_SYNC_UP_BSR is forced to SD */
  if(stack_idx == 2 &&
    pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP_BSR &&
    (cmemg_get_reason_per_sub(state_machine->asubs_id) == CM_AC_REASON_THIRD_PARTY_IMS_E911 ||
    cmph_is_get_networks_ongoing(state_machine->asubs_id) == TRUE) &&
    cmph_is_subs_feature_mode_1x_sxlte(state_machine->asubs_id))
  {
    CM_MSG_HIGH_1("MSC: stack synup bsr buffered - no action taken get_networks_ongoing=%d",
		cmph_is_get_networks_ongoing(state_machine->asubs_id));
    cmmsc_auto_update_is_stack_sync_bsr_pending(state_machine, TRUE);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update csg_id & csg_rat */

  if( ph_orig_top_ptr->orig != NULL )
  {
    sys_sel_pref_req_id = ph_orig_top_ptr->orig->sys_sel_pref_req_id;
    csg_id = ph_orig_top_ptr->orig->csg_id;
    csg_rat = ph_orig_top_ptr->orig->csg_rat;

    if(ss == CM_SS_HYBR_2 && ph_ptr->is_sys_sel_pref_rsp_pending &&
       cmph_is_subs_feature_mode_sglte(state_machine->asubs_id))
    {
      cmph_set_sys_sel_pref_rsp_pending2(sys_sel_pref_req_id);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update additional action */

  if( (cmph_ptr()->is_ps_detach_required == TRUE)
       &&
      (ph_orig_top_ptr->act_type == CM_ACT_TYPE_PH_OBJ ||
       ph_orig_top_ptr->act_type == CM_ACT_TYPE_DATA_CALL || 
       ph_orig_top_ptr->act_type == CM_ACT_TYPE_POLICYMAN_RESTRICT)
       &&
       cmmsc_is_valid_ss_for_ps_detach( state_machine->asubs_id, ss )
    )
  {
    addl_action.action |= (uint8)MMOC_PREF_SYS_ADDL_ACTION_PS_DETACH;
    ph_ptr->is_ps_detach_required = FALSE;
    ph_ptr->is_user_ps_detach_required = FALSE;
    CM_MSG_HIGH_1 ("MSC: addl_action %d", addl_action.action);
  }


  if (state_machine->stack[stack_idx].addl_action.action == MMOC_PREF_SYS_ADDL_ACTION_RESUME)
  {
    cm_ss_e_type ss                 = CM_SS_NONE;
    sys_modem_as_id_e_type asubs_id = state_machine->asubs_id;

    #if defined(FEATURE_MMODE_TRIPLE_SIM)
    if( asubs_id == SYS_MODEM_AS_ID_3 )
    {
      ss = cmph_ptr()->subs_stack_map[asubs_id][stack_idx];
    }
    else
    #endif
    if( (asubs_id == SYS_MODEM_AS_ID_1) || (asubs_id == SYS_MODEM_AS_ID_2))
    {
      ss = ph_ptr->subs_stack_map[asubs_id][stack_idx];
    }
    else
    {
      ss = CM_SS_NONE;
    }
    CM_MSG_HIGH_1( "MSC: pref_chgd, subs_stack_map ss=%d", ss );
    
    if (ss > CM_SS_NONE && ss < CM_SS_MAX )
    {
      CM_MSG_HIGH_4("Before: susp_state %d %d %d %d",
                    ph_ptr->ss_susp_info[CM_SS_MAIN].is_suspend,
                    ph_ptr->ss_susp_info[CM_SS_HYBR_1].is_suspend,
                    ph_ptr->ss_susp_info[CM_SS_HYBR_2].is_suspend,
                    ph_ptr->ss_susp_info[CM_SS_HYBR_3].is_suspend);

     

      addl_action.action |= (uint8)MMOC_PREF_SYS_ADDL_ACTION_RESUME;
      CM_MSG_HIGH_1("setting addl_action to resume %d", addl_action.action);
      state_machine->stack[stack_idx].addl_action.action &= ~(MMOC_PREF_SYS_ADDL_ACTION_RESUME);
      cmph_ptr()->ss_susp_info[ss].susp_due_to_emerg = FALSE;
      cmph_update_suspend_status(ss, FALSE, FALSE);

      CM_MSG_HIGH_4("After: susp_state %d %d %d %d",
                    ph_ptr->ss_susp_info[CM_SS_MAIN].is_suspend,
                    ph_ptr->ss_susp_info[CM_SS_HYBR_1].is_suspend,
                    ph_ptr->ss_susp_info[CM_SS_HYBR_2].is_suspend,   
                    ph_ptr->ss_susp_info[CM_SS_HYBR_3].is_suspend);
    }
  }

  /* Update add_action - ue_mode if conditions are met */
  cmmsc_update_addl_action_ue_mode_switch(state_machine,ss,&addl_action,pref_info);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set pref_update reason */

  if(cmac_is_1x_ppp_dereg_end())
  {
	pref_update_reason =  SD_SS_PREF_UPDATE_REASON_1X_DEREG_END;
	CM_MSG_MED_0 ("MSC: pref update reas 1x_DEREG_END ");
  }

  #ifdef CM_GW_SUPPORTED
  /* Set EF_RAT variables */
  else if (ph_ptr->is_rat_bal == TRUE &&
            ph_ptr->is_mode_change_due_to_ef_rat == TRUE && 
            ss != CM_SS_HYBR_1 )
  {
    pref_update_reason =  SD_SS_PREF_UPDATE_REASON_EF_RAT_MODE_CHG;
    /* Reset the flag */
    ph_ptr->is_mode_change_due_to_ef_rat = FALSE;
    CM_MSG_MED_0 ("MSC: hybr2/3:EF_RAT_MODE_CHG set FALSE");
  }
  #endif /* CM_GW_SUPPORTED */

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  else if(pref_info->pref_update_reason != SD_SS_PREF_UPDATE_REASON_NONE)
  {
    /* After we set this - we're not clearing the variable
    ** pref_update_reason here because we will continue to force on MAIN.
    ** again, so if we clear it here, MAIN will see this value as _NONE value when
    ** MAIN stack is forced.
    */
    pref_update_reason = pref_info->pref_update_reason;
    CM_MSG_HIGH_1("CMSOA: pref_update_reason=%d ", pref_update_reason);

    /* Skip forcing preference on HDR for srlte<->csfb sub mode switch */
    if(cmph_is_subs_feature_mode_srlte(state_machine->asubs_id) &&
      stack_idx == 1 &&
      (pref_update_reason == SD_SS_PREF_UPDATE_REASON_SWITCH_SVLTE ||
       pref_update_reason == SD_SS_PREF_UPDATE_REASON_SWITCH_CSFB    ||
       pref_update_reason == SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2 ||
       pref_update_reason == SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2_SILENT )
    )
    {
      CM_MSG_HIGH_0("MSC: cmsoa switch - hybr1 no action taken");
      *cmph_hdr_kick_pending() = TRUE;
      cmmsc_auto_update_is_kick_hdr_pending(state_machine, TRUE);
      return;
    }
  }
  else if (cmmsc_auto_is_hybr2_on(state_machine->op_mode) &&
           (ph_ptr->is_dds_change_on_svlte_sub == TRUE) &&
           (stack_idx == 1) && (pref_reason == SD_SS_PREF_REAS_USER))
  {
    CM_MSG_HIGH_0("MSC:Skip forceing preferences to HYBR_1 due to DDS change to SV/SR mode");
    ph_ptr->is_dds_change_on_svlte_sub = FALSE;
    *cmph_hdr_kick_pending() = TRUE;
    cmmsc_auto_update_is_kick_hdr_pending(state_machine, TRUE);
    return;
  }
  #endif

  /* Update pref reason as AC_EMERG when device in AC EMERG state
  ** to overide checks in SD for emergency preferences*/

  if(pref_update_reason == SD_SS_PREF_UPDATE_REASON_NONE &&
    cmac_is_in_emergency_only())
  {
    pref_update_reason = SD_SS_PREF_UPDATE_REASON_AC_EMERG;
  }

  pref_info->srv_domain_pref = cmtask_compute_srv_domain_pref(ss);
  
  /* For emergency call make user mode preference as EMERGENCY so that SD
  ** can look for service on all possible RATs
  */
  if (( pref_info->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG ||
       pref_info->orig_mode == SD_SS_ORIG_MODE_EMERG_CB    ||
       pref_info->orig_mode == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG ||
       pref_info->orig_mode == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG)
 )
  { 
    /* For emergency call if domain pref is CS only, CM need not to add LTE in mode pref to SD */
    if( pref_info->srv_domain_pref == CM_SRV_DOMAIN_PREF_CS_ONLY )
    {
       CM_MSG_HIGH_0("MSC: Remove LTE as pref_info->srv_domain_pref is CS only ");
       user_mode_pref = SD_SS_MODE_PREF_ANY_BUT_LTE;
    }
    /* If LTE emergency call is not possible and user mode pref did not already include LTE,
       don't add LTE to the user mode pref */ 
    #if ( defined(FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X) || defined (FEATURE_IP_CALL) ) && defined FEATURE_LTE
    else if( !cmsds_is_LTE_emerg_call_possible(state_machine->asubs_id) && 
             !cmcall_misc_is_mode_pref( cmph_get_ss_mode_pref(ss), CM_MODE_PREF_LTE_ONLY ) 
        )
    {      
      CM_MSG_HIGH_0("MSC: Remove LTE as pref_info->srv_domain_pref is CS only ");
      if (cmph_is_msim())
      {
        user_mode_pref = SD_SS_MODE_PREF_ANY_BUT_LTE & \
                        cmph_map_sys_mode_mask_to_sd_mode_pref(
                        cmmsc_get_curr_msc_max_cap(state_machine->asubs_id));
      }
      else
      {
        user_mode_pref = SD_SS_MODE_PREF_ANY_BUT_LTE;
      }
    }
    #endif
    else 
    {
      if (cmph_is_msim())
      {
        user_mode_pref = SD_SS_MODE_PREF_ANY & \
                        cmph_map_sys_mode_mask_to_sd_mode_pref(
                        cmmsc_get_curr_msc_max_cap(state_machine->asubs_id));
      }
      else
      {
        user_mode_pref = SD_SS_MODE_PREF_ANY;
      }
    }
  }
  else
  {
    if (ph_orig_top_ptr->orig !=NULL)

    {
      user_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(
                       ph_orig_top_ptr->orig->user_mode_pref);
    }
    else
    {
      CM_ERR_1("top element in the priority queue of stack %d is NULL",ss);
    }
  }

  /*
  CM_MSG_HIGH_3 ("MSC->MMOC: pref_chgd, ss %d, stack rat_cap=0x%x, mode_pref=%d",
               ss,
               stack_pointer->capablity.mode,
               pref_info->mode_pref);
  CM_MSG_MED_5 ("MSC->MMOC: pref_chgd, sub max_cap=0x%x, pm rat_cap=%d user_mode_pref=0x%x, hybr_pref=%d, orig_mode=%d",
               state_machine->stack_common_info.ue_max_mode_capability,
               state_machine->stack_common_info.ue_mode_capability,
               user_mode_pref,
               pref_info->hybr_pref,
               pref_info->orig_mode);
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter mode, band, and domain */
  cmmsc_get_stack_pref_per_cap(state_machine, stack_idx, pref_info, &common_pref);
  cmmsc_get_user_mode_pref_per_cap(state_machine, stack_idx, &user_mode_pref);
  if (ss == CM_SS_HYBR_2  &&
     cmph_is_subs_feature_mode_srlte(state_machine->asubs_id)&&
       cmmsc_auto_is_hybr2_on(state_machine->op_mode))
  {
    user_mode_pref = SD_MODE_ADD(user_mode_pref,SD_SS_MODE_PREF_CDMA);
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filtering only applies to svlte not srlte for now */
  if( stack_idx == 2 &&
      cmph_is_subs_feature_mode_svlte(state_machine->asubs_id))
  {
    CM_MSG_HIGH_0("Filtering Hybrid 2 LTE bands on filter");
    cm_print_lte_band_mask(state_machine->stack_common_info.svlte_capable_lte_bands);
    common_pref.lte_band = SYS_LTE_BAND_MASK_COMMON( &common_pref.lte_band,
                                               &(state_machine->stack_common_info.svlte_capable_lte_bands));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Filter hybrid preference on MAIN. MAX shouldn't be the case here */
  if ((ss == CM_SS_MAX || ss == CM_SS_MAIN) &&
     (!cmmsc_auto_is_hybr_allowed(user_mode_pref,
             cmph_map_cm_hybr_pref_to_sd_hybr_pref(pref_info->hybr_pref),
             state_machine->asubs_id)
      ||
      !cmmsc_is_msc_multi_stacks(state_machine->asubs_id))
  )
  {
    internal_hybr_pref = SD_SS_HYBR_PREF_NONE;
  }
  else
  {
    internal_hybr_pref = cmph_map_cm_hybr_pref_to_sd_hybr_pref(pref_info->hybr_pref);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get the addl info from addl_ptr */

  if (addl_ptr && addl_ptr->pref_update_reas)
  {
    pref_update_reason = *addl_ptr->pref_update_reas;
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmph_is_subs_feature_mode_1x_sxlte(state_machine->asubs_id) && 
     ss == CM_SS_HYBR_2 && 
     pref_reason != SD_SS_PREF_REAS_RESTORE)
  {
    *cmph_get_is_kicked_hybr_2_ptr() = TRUE;
    cmmsc_auto_update_is_kick_hybr2_pending(state_machine, FALSE);
  }
  if (ss == CM_SS_HYBR_1)
  {
    cmmsc_auto_update_is_kick_hdr_pending(state_machine, FALSE);
    *cmph_get_is_kicked_hybr_ptr() = TRUE;
    pref_update_reason = SD_SS_PREF_UPDATE_REASON_NONE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Store LTE stack capability,
  ** used later to determine if ps_detach should be triggered
  */
  cmmsc_store_lte_stack_capability(
    state_machine,
    ss,
    pref_reason,
    user_mode_pref,
    common_pref
  );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If DDS switch is permanent and was preceded by a temporary switch,
  ** indicate the same to MMOC/SD */
  if(cmph_is_msim() && ss != CM_SS_HYBR_1 &&
   (pref_info->srv_domain_pref == CM_SRV_DOMAIN_PREF_CS_PS || 
    pref_info->srv_domain_pref == CM_SRV_DOMAIN_PREF_PS_ONLY) && 
    ph_ptr->is_dds_switchback_from_temp && pref_reason == SD_SS_PREF_REAS_USER
    )
  {
    if(!(SD_MODE_CONTAIN(common_pref.mode,~(int)SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE)))
    {
      CM_MSG_HIGH_0("Sending pref update reason as dds_switchback_from_temp to MMOC");
      pref_update_reason = SD_SS_PREF_UPDATE_REASON_DDS_SWITCHBACK_FROM_TEMP;
    }
    ph_ptr->is_dds_switchback_from_temp = FALSE;
  }

  srv_domain_to_be_forced = common_pref.domain;

  // For nonDDS Sub, if 'IMS for non DDS' is set, change CS+PS to CS+VoLTE 
  if(cmpm_ptr()->ims_for_nondds && (SD_SS_MODE_PREF_LTE & user_mode_pref) && 
    (cmph_map_subs_to_ss(cmph_map_cm_ss_to_as_id(ss)) == ss))
  {
    if((ph_ptr->dds_change_in_progress && (cmph_map_cm_ss_to_as_id(ss) == ph_ptr->default_data_subs))
        ||(!ph_ptr->dds_change_in_progress && (cmph_map_cm_ss_to_as_id(ss) != ph_ptr->default_data_subs)))
    {
      if(common_pref.domain == SD_SS_SRV_DOMAIN_PREF_CS_PS)
      {
        srv_domain_to_be_forced = SD_SS_SRV_DOMAIN_PREF_CS_VOLTE;
      }
      else if(common_pref.domain == SD_SS_SRV_DOMAIN_PREF_PS_ONLY)
      {
        srv_domain_to_be_forced = SD_SS_SRV_DOMAIN_PREF_VOLTE;
      }

      CM_MSG_HIGH_3("MSC: Due to ims_for_nondds, on ss %d: srv_domain change to %d: dds_change_in_progress %d",
        ss, srv_domain_to_be_forced,ph_ptr->dds_change_in_progress);

      cmpm_ptr()->change_in_ims_for_nondds = FALSE;
    }
  }

  /* Send command to MMOC */
  cmmsc_send_mmoc_pref_sys_chgd_cmd(cmph_map_cm_ss_type_to_sd_ss_type(ss),
                                      pref_reason,
                                      orig_mode,
                                      common_pref.mode,
                                      common_pref.band,
                                      common_pref.lte_band,
                                      common_pref.tds_band,
                                      cmph_map_cm_prl_pref_to_sd_prl_pref(pref_info->prl_pref),
                                      cmph_map_cm_roam_pref_to_sd_roam_pref(pref_info->roam_pref),
                                      internal_hybr_pref,
                                      pref_info->otasp_band,
                                      pref_info->otasp_blksys,
                                      pref_info->avoid_type,
                                      pref_info->avoid_time,
                                      (const sys_manual_sys_info_s_type *)&(pref_info->sys_id),
                                      srv_domain_to_be_forced,
                                      cmph_map_cm_acq_order_pref_to_sd(pref_info->acq_order_pref),
                                      pref_update_reason,
                                      &addl_action,
                                      user_mode_pref,
                                      (const sd_rat_acq_order_s_type *)&sd_rat_acq_order,
                                      sys_sel_pref_req_id,
                                      camp_mode,
                                      csg_id,
                                      csg_rat,
                                      CMPH_SS_VOICE_DOMAIN_PREF(state_machine->asubs_id, ph_ptr)
                                    );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update last preferences which were sent */
  if (ss != CM_SS_HYBR_1)
  {
    cm_act_orig_s_type *pref_sent_ptr = cmph_get_last_pref_sent(ss);
  
    if (pref_sent_ptr)
    {
      pref_sent_ptr->act_priority = CM_ACT_PRIORITY_PH;
      pref_sent_ptr->orig_band_pref = cmph_map_sd_band_pref_to_cm_band_pref(common_pref.band);
      pref_sent_ptr->orig_lte_band_pref = common_pref.lte_band;
      pref_sent_ptr->orig_tds_band_pref = cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(common_pref.tds_band);
      pref_sent_ptr->orig_hybr_pref = cmph_map_sd_hybr_pref_to_cm_hybr_pref(internal_hybr_pref);
      pref_sent_ptr->orig_mode = orig_mode;
      pref_sent_ptr->orig_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(common_pref.mode);
      pref_sent_ptr->orig_pref_term = CM_PREF_TERM_NONE;
      pref_sent_ptr->orig_prl_pref = pref_info->prl_pref;
      pref_sent_ptr->orig_roam_pref = pref_info->roam_pref;
  
      pref_sent_ptr->orig_srv_domain_pref =
        cmph_map_sd_srv_domain_pref_to_cm_domain_pref(common_pref.domain);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* For L+G DSDS */
  #ifdef FEATURE_MMODE_DUAL_SIM
  if (cmph_is_msim()&&
     (ph_ptr->dds_status == CMPH_DDS_STATUS_PS_DETACH_PENDING))
  {
    /* If there's LTE call(s), change the dds_status to REMOVE_DATA_OBJ
    ** Else, reset the dds_status.
    */
    if (cmcall_is_no_call_in_lte_ps() == FALSE)
    {
      ph_ptr->dds_status = CMPH_DDS_STATUS_REMOVE_DATA_OBJ;
    }
    else
    {
      ph_ptr->dds_status = CMPH_DDS_STATUS_NONE;
    }
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For svlte, if is_stack_sync_bsr_pending and call end, force preference */
  if(cmmsc_auto_is_stack_sync_bsr_pending(state_machine) &&
    ss == CM_SS_HYBR_2 &&
    pref_reason == SD_SS_PREF_REAS_ORIG_END)
  {
    cmmsc_send_mmoc_pref_sys_chgd_cmd(cmph_map_cm_ss_type_to_sd_ss_type(ss),
                                      SD_SS_PREF_REAS_STACK_SYNC_UP_BSR,
                                      orig_mode,
                                      common_pref.mode,
                                      common_pref.band,
                                      common_pref.lte_band,
                                      common_pref.tds_band,
                                      cmph_map_cm_prl_pref_to_sd_prl_pref(pref_info->prl_pref),
                                      cmph_map_cm_roam_pref_to_sd_roam_pref(pref_info->roam_pref),
                                      internal_hybr_pref,
                                      pref_info->otasp_band,
                                      pref_info->otasp_blksys,
                                      pref_info->avoid_type,
                                      pref_info->avoid_time,
                                      (const sys_manual_sys_info_s_type *)&(pref_info->sys_id),
                                      srv_domain_to_be_forced,
                                      cmph_map_cm_acq_order_pref_to_sd(pref_info->acq_order_pref),
                                      pref_update_reason,
                                      &addl_action,
                                      user_mode_pref,
                                      NULL,
                                      sys_sel_pref_req_id,
                                      camp_mode,
                                      csg_id,
                                      csg_rat,
                                      CMPH_SS_VOICE_DOMAIN_PREF(state_machine->asubs_id, ph_ptr)
                                    );
    cmmsc_auto_update_is_stack_sync_bsr_pending(state_machine, FALSE);
  }  

  
  if(ph_orig_top_ptr->orig != NULL)
  {
    ph_orig_top_ptr->orig->pref_update_reason = SD_SS_PREF_UPDATE_REASON_NONE;
  }
}


/*===========================================================================

FUNCTION cmmsc_store_lte_stack_capability

DESCRIPTION
  This function is used to store the capability of the lte stack.
  The stored capability is used later to determine if a PS detach
  should be triggered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_store_lte_stack_capability(
  cmmsc_state_machine_s_type *state_machine,
  cm_ss_e_type ss,
  sd_ss_pref_reas_e_type pref_reason,
  sd_ss_mode_pref_e_type user_mode_pref,
  cmmsc_stack_capability_s_type common_pref
)
{
 
  cm_orig_q_s_type* ph_orig_top_ptr = cmtask_orig_para_get_top(ss);
  int                         st_id = cmmsc_mapping_ss_to_stack_idx(ss);

  if(state_machine == NULL)
  {
    CM_ERR_0("cmmsc_store_lte_stack_capability: state_machine NULL");
    return;
  }

    if(ph_orig_top_ptr == NULL)
    {
      CM_ERR_0("cmmsc_store_lte_stack_capability: ph_orig_top_ptr NULL");
      return;
    }

    /* Save the capability only if the top of the priority queue
    ** is the phone object
    */
    if(ph_orig_top_ptr->act_type != CM_ACT_TYPE_PH_OBJ)
    {
      return;
    }

  /* LTE stack can be the main stack or the hybr2 stack/hybr3 stack
    */
  if(pref_reason == SD_SS_PREF_REAS_USER || pref_reason == SD_SS_PREF_REAS_RESTORE ||
	pref_reason == SD_SS_PREF_REAS_USER_RESEL)
    {
    state_machine->stack[st_id].prev_registration_capability.mode = user_mode_pref;
    state_machine->stack[st_id].prev_registration_capability.band = common_pref.band;
    state_machine->stack[st_id].prev_registration_capability.tds_band= common_pref.tds_band;
    state_machine->stack[st_id].prev_registration_capability.lte_band= common_pref.lte_band;
    state_machine->stack[st_id].prev_registration_capability.domain= common_pref.domain;

  }

}

/*===========================================================================

FUNCTION cmmsc_send_mmoc_pref_sys_chgd_cmd

DESCRIPTION
  This function is used to command the MMoC to do Preferred system selection
  change. This function will queue the preferred system selection changed
  command to the MMoC Task.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void    cmmsc_send_mmoc_pref_sys_chgd_cmd
(

        sd_ss_e_type                   ss,
            /**< Indicates the SS-Instance ( SS_MAIN or SS_HDR) this
            ** pref_sys_chgd command is intended for.
            */


        sd_ss_pref_reas_e_type         pref_reas,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type         orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type         mode_pref,
           /**< The mode preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         band_pref,
           /**< Band preference that is associated with the selected NAM.
           */

        sys_lte_band_mask_e_type       lte_band_pref,
           /* LTE band preference that is associated with the selected NAM.
           */

        sd_ss_band_pref_e_type         tds_band_pref,
           /* TDS band preference that is associated with the selected NAM.
           */

        sd_ss_prl_pref_e_type          prl_pref,
           /**< PRL preference that is associated with the selected NAM.
           */

        sd_ss_roam_pref_e_type         roam_pref,
           /**< Roam preference that is associated with the selected NAM.
           */

        sd_ss_hybr_pref_e_type         hybr_pref,
           /**< New hybrid preference (none, CDMA/HDR).
           */

        sd_band_e_type                 otasp_band,
           /**< The band-class that is associated with OTASP origination mode.
           ** Note that this parameter is ignored if orig_mode != OTASP.
           */

        sd_blksys_e_type               otasp_blksys,
          /**< The PCS frequency block/Cellular system that is associated
          ** with OTASP origination mode. Note that this parameter is ignored
          ** if orig_mode != OTASP.
          */

        sd_ss_avoid_sys_e_type         avoid_type,
          /**< Type of the system to be avoided, valid only
          ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        dword                          avoid_time,
          /**< Time in seconds for which the system is to be avoided, valid
          ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
          */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
          */

        sd_ss_srv_domain_pref_e_type   srv_domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_pref_update_reason_e_type  pref_update_reas,
           /**< Preference change reason
           */

        sys_addtl_action_s_type        *addl_action,
        /*
        To indicate sub action to be performed by MMOC during pref sys chgd cmd
        */

        sd_ss_mode_pref_e_type         user_mode_pref,
        /**< The user/phone mode preference.
        */

        const sd_rat_acq_order_s_type  *rat_acq_order_ptr,
        /**< Rat acquisition order including LTE.
        */

        uint16                         req_id,
        /**< System Selection Preferences request id.
        */

        sd_ss_pref_camp_mode_e_type    camp_only_pref,

        sys_csg_id_type                csg_id,
         /**< CSG identifier */

        sys_radio_access_tech_e_type   csg_rat,
        /**< RAT specified for CSG */

        sys_voice_domain_pref_e_type   voice_domain_pref
        /**< voice domain pref */
)
{
  #ifdef FEATURE_CM_DEBUG_BUFFER
  {
    CMDBG_MEM_ALLOC(cm_mmoc_cmd_buff, cm_mmoc_prot_gen_cmd_entry_s_type)

    cm_mmoc_cmd_buff->ss                = ss;
    cm_mmoc_cmd_buff->pref_reas         = pref_reas;
    cm_mmoc_cmd_buff->orig_mode         = orig_mode;
    cm_mmoc_cmd_buff->mode_pref         = mode_pref;
    cm_mmoc_cmd_buff->band_pref         = band_pref;
    cm_mmoc_cmd_buff->lte_band_pref     = lte_band_pref;
    cm_mmoc_cmd_buff->tds_band_pref     = tds_band_pref;
    cm_mmoc_cmd_buff->roam_pref         = roam_pref;
    cm_mmoc_cmd_buff->hybr_pref         = hybr_pref;
    cm_mmoc_cmd_buff->avoid_type        = avoid_type;
    cm_mmoc_cmd_buff->avoid_time        = avoid_time;
    cm_mmoc_cmd_buff->domain_pref       = srv_domain_pref;
    cm_mmoc_cmd_buff->pref_update_reas  = pref_update_reas;
    cm_mmoc_cmd_buff->addl_action       = *addl_action;
    cm_mmoc_cmd_buff->user_mode_pref    = user_mode_pref;
    cm_mmoc_cmd_buff->camp_mode_pref    = camp_only_pref;
    cm_mmoc_cmd_buff->voice_domain_pref = voice_domain_pref;
    cmdbg_add_to_dbg_buffer(CM_BUFF_MMOC_CMD_TYPE, MMOC_CMD_PROT_GEN_CMD, ((void*)cm_mmoc_cmd_buff));
  }
  #endif
  mmoc_cmd_pref_sys_chgd6( ss,
                           pref_reas,
                           orig_mode,
                           mode_pref,
                           band_pref,
                           lte_band_pref,
                           tds_band_pref,
                           prl_pref,
                           roam_pref,
                           hybr_pref,
                           otasp_band,
                           otasp_blksys,
                           avoid_type,
                           avoid_time,
                           manual_sys_info_ptr,
                           srv_domain_pref,
                           acq_order_pref,
                           pref_update_reas,
                           addl_action,
                           user_mode_pref,
                           rat_acq_order_ptr,
                           req_id,
                           camp_only_pref,
                           csg_id,
                           csg_rat,
                           voice_domain_pref
                        );

}

/*===========================================================================

FUNCTION cmmsc_form_sub_data_sim0

DESCRIPTION
  This function forms subscription info for sim0 to send to mmoc.

DEPENDENCIES
  cmpref2_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void   cmmsc_form_sub_data_sim0(

  boolean                 sub_avail[2],
  /* An array containing the info of subs_avail for each sub.
   ** 0 for gw primary, 1 for cdma primary ...
   */

  const cm_pref_s_type         *pref_info,
  /* pref_info pointer for sim0
   */

  mmoc_subs_data_s_type  *sub_data[2]
  /* An array of pointers to point to sub_data for sim0
   */
)
{
  sd_ss_mode_pref_e_type           mode_pref0;
  sd_ss_hybr_pref_e_type           hybr_pref0;
  cm_network_sel_mode_pref_e_type  network_sel_mode_pref0;
  sd_ss_orig_mode_e_type           orig_mode0;
  sys_manual_sys_info_s_type       manual_sys_info;
  sd_ss_band_pref_e_type           band0;
  sd_ss_band_pref_e_type           tds_band0;
  sys_lte_band_mask_e_type         lte_band0;
  sd_ss_srv_domain_pref_e_type     domain0;
  mmgsdi_session_type_enum_type    session_type;
  cmmsc_state_machine_s_type   *state_machine;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);
  
  /* Validate params
   */
  CM_ASSERT(sub_avail != NULL);
  CM_ASSERT(pref_info != NULL);
  CM_ASSERT(sub_data != NULL);

  if (sub_avail == NULL || pref_info == NULL || sub_data == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  if (sub_data[0] == NULL || sub_data[1] == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  if (state_machine == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  mode_pref0 = cmph_map_cm_mode_pref_to_sd_mode_pref(pref_info->mode_pref);
  band0 = cmph_map_cm_band_pref_to_sd_band_pref(pref_info->band_pref);
  tds_band0
    = cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(pref_info->tds_band_pref);
  lte_band0 = pref_info->lte_band_pref;
  domain0 = cmph_map_cm_srv_domain_pref_to_sd(pref_info->srv_domain_pref);

  hybr_pref0 = cmph_map_cm_hybr_pref_to_sd_hybr_pref(pref_info->hybr_pref);
  hybr_pref0 = cmmsc_auto_is_hybr_allowed(mode_pref0, hybr_pref0, state_machine->asubs_id) ?
                              hybr_pref0: SD_SS_HYBR_PREF_NONE;

  network_sel_mode_pref0 = pref_info->network_sel_mode_pref;
  orig_mode0 = cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                                network_sel_mode_pref0);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Fill Subscription Information for SIM1 - gw pri + cdma pri */

  /* GW pri specific info */
  sub_data[0]->is_subs_avail          = sub_avail[0];
  sub_data[0]->is_perso_locked        = FALSE;
  if(cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw(SYS_MODEM_AS_ID_1,&session_type) != CM_MMGSDI_SUCCESS)
  {
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }
  sub_data[0]->session_type           = session_type;
  sub_data[0]->prm.gw.acq_order_pref  =
              cmph_map_cm_acq_order_pref_to_sd(pref_info->acq_order_pref);
  sub_data[0]->prm.gw.srv_domain_pref = domain0;

  manual_sys_info.sys_id.id_type  = SYS_SYS_ID_TYPE_UMTS;
  manual_sys_info.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;
  manual_sys_info.sys_id.id.plmn    = pref_info->plmn;
  sub_data[0]->prm.gw.manual_sys_info = manual_sys_info;
  sub_data[0]->curr_ue_mode =   state_machine->stack_common_info.ue_mode;

  if (cmph_is_subs_feature_mode_srlte(SYS_MODEM_AS_ID_1))
  {
    sub_data[0]->is_ue_mode_substate_srlte = 
                (state_machine->op_mode == CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2);
  }
  else
  {
    sub_data[0]->is_ue_mode_substate_srlte = FALSE;
  }

  sub_data[0]->subs_capability = cmpmprx_fetch_max_mode_capability(SYS_MODEM_AS_ID_1);

  sub_data[1]->subs_capability = cmpmprx_fetch_max_mode_capability(SYS_MODEM_AS_ID_1);
  sub_data[1]->curr_ue_mode         = sub_data[0]->curr_ue_mode;
  sub_data[1]->is_ue_mode_substate_srlte = 
                                      sub_data[0]->is_ue_mode_substate_srlte;

  /* 1x pri specific info */
  sub_data[1]->is_subs_avail        = sub_avail[1];
  sub_data[1]->is_perso_locked        = FALSE;
  sub_data[1]->session_type         = MMGSDI_1X_PROV_PRI_SESSION;
  sub_data[1]->prm.cdma.prl_pref    = cmph_map_cm_prl_pref_to_sd_prl_pref(
                                        pref_info->prl_pref);

  /* Common info */
  sub_data[1]->as_id                = sub_data[0]->as_id
                                    = SYS_MODEM_AS_ID_1;
  sub_data[1]->ss                   = sub_data[0]->ss = SD_SS_MAIN;
  sub_data[1]->orig_mode            = sub_data[0]->orig_mode
                                    = orig_mode0;
  sub_data[1]->mode_pref            = sub_data[0]->mode_pref
                                    = mode_pref0;
  sub_data[1]->band_pref            = sub_data[0]->band_pref
                                    = band0;
  sub_data[1]->tds_band_pref        = sub_data[0]->tds_band_pref
                                    = tds_band0;
  sub_data[1]->lte_band_pref        = sub_data[0]->lte_band_pref
                                    = lte_band0;
  sub_data[1]->hybr_pref            = sub_data[0]->hybr_pref
                                    = hybr_pref0;
  sub_data[1]->roam_pref            = sub_data[0]->roam_pref
                                    = cmph_map_cm_roam_pref_to_sd_roam_pref(
                                       pref_info->roam_pref);
  sub_data[1]->nv_context           = sub_data[0]->nv_context
                                    = cmph_ptr()->main_stack_info.nv_context;
  
  CM_MSG_HIGH_6("CM_SUB_DATA: sim_idx=0, mode_pref=0x%x, orig_mode=%d, pm rat_cap=0x%x,curr_ue_mode=%d, srv_domain_pref=%d, sub max_cap=0x%x",
                mode_pref0,
                orig_mode0, 
                state_machine->stack_common_info.ue_mode_capability,
                sub_data[0]->curr_ue_mode,
                domain0,
                sub_data[0]->subs_capability);

}

/*===========================================================================

FUNCTION cmmsc_form_subs_data

DESCRIPTION
  This function forms the subscription data to be send to mmoc.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void    cmmsc_form_subs_data(

        boolean                         sub_avail[],
        /* An array containing the info of subs_avail for each sub.
             ** 0 for gw primary, 1 for cdma primary, 2 for gw secondary
             */

        const cm_pref_s_type                 *sim_pref_ptr[],
        /* An array containing pref_info for sim0, sim1, ...
            */

        mmoc_subs_data_s_type                *sub_data[],
        /* An array of pointers to sub_data
            */

        size_t                                size_of_sub_avail,
        /* Array size of sub_avail
        */

        size_t                                size_of_sim_pref_ptr,
        /* Array size of pref_info, 1 set of pref info for each sim
        */

        size_t                                size_of_sub_data_ptr
        /* Array size of sub_data,
        ** primary sub has 2 sets of sub data, and all other subs have 1 set
        */
)
{
  /* Index for sim. 0-> sim1, 1->sim2 ... */
  size_t sim_idx = 0;

  /* For sim2, 3, ... , the sub data storage is started at index 2
  ** Index 0 - sub data for gw primary
  ** Index 1 - sub data for cdma primary
  ** Index 2 - sub data for gw secondary
  */
  size_t index_for_sub = 2;
  size_t index_inc = 1;

  /* Validate params */
  CM_ASSERT(sub_avail != NULL);
  CM_ASSERT(sim_pref_ptr != NULL);
  CM_ASSERT(sub_data != NULL);

  if (sub_avail == NULL || sim_pref_ptr == NULL || sub_data == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Form Subscription Information for SIM1 - gw pri and cdma pri */
  cmmsc_form_sub_data_sim0(sub_avail,
                            sim_pref_ptr[0],
                            sub_data);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* No need to form sub data for sim2 if there is only 1 sim */
  if (cmph_is_ssim())
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If reach here, it is dsdx/tstx/qsqx target ....
   ** !!! Note: additional handling needs to be added for more than 2 sims
   */
  for (sim_idx = 1; sim_idx < size_of_sim_pref_ptr; sim_idx ++)
  {
    if (size_of_sub_data_ptr > index_for_sub &&
        size_of_sub_avail > index_for_sub &&
        sub_data[index_for_sub])
    {
       /* Update pref info and form subscription info for sim != 0  */
       cmmsc_update_sim_other(sim_pref_ptr, sub_data[index_for_sub], sim_idx);
       sub_data[index_for_sub]->is_subs_avail = sub_avail[index_for_sub];
       sub_data[index_for_sub]->curr_ue_mode  = SYS_UE_MODE_NONE;
       sub_data[index_for_sub]->is_ue_mode_substate_srlte  = FALSE;
       index_for_sub = index_for_sub + index_inc;
    }
  }
}

void    cmmsc_update_sim_other (

        const cm_pref_s_type              *sim_pref_ptr[],
        /* An array containing pref_info for sim0, sim1, ...
        ** each sim has 1 set of pref info
        */

        mmoc_subs_data_s_type          *sub_other_data,
        /* Subscription data for sim != 0
            */

        size_t                                     sim_idx
        /* Index of sim for which to update sub data
        */
)
{
  sys_manual_sys_info_s_type manual_sys_info;
  mmgsdi_session_type_enum_type session_type;
  cmmsc_state_machine_s_type *state_machine 
    = cmmsimc_state_machine_ptr_per_sim_idx(sim_idx);

  manual_sys_info.sys_id.id_type  = SYS_SYS_ID_TYPE_UMTS;
  manual_sys_info.prm.gw.sel_type = SYS_MANUAL_SEL_TYPE_USER_SPECIFIED;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Validate the params
   */
  CM_ASSERT(sim_pref_ptr != NULL);
  CM_ASSERT(sub_other_data != NULL);

  if(sim_pref_ptr == NULL || sub_other_data == NULL)
  {
    CM_ERR_0("MSC: pref_info/sub_other_data null ptr");
    return;
  }

  if (sim_idx == 0)
  {
    CM_ERR_0("MSC: Invalid sim_idx");
    return;
  }

  if (state_machine == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update pref info for stack other than 0
   ** Stack 0 pref update is taken care in cmpref.
   */
  if  (sim_idx < cmpmprx_get_num_of_sims() && sim_pref_ptr[sim_idx])
  {
    sub_other_data->mode_pref
            = cmph_map_cm_mode_pref_to_sd_mode_pref(sim_pref_ptr[sim_idx]->mode_pref);
    sub_other_data->band_pref
            = cmph_map_cm_band_pref_to_sd_band_pref(sim_pref_ptr[sim_idx]->band_pref);
    sub_other_data->tds_band_pref
            = cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(sim_pref_ptr[sim_idx]->tds_band_pref);
    sub_other_data->lte_band_pref = sim_pref_ptr[sim_idx]->lte_band_pref;
    sub_other_data->prm.gw.srv_domain_pref
            = cmph_map_cm_srv_domain_pref_to_sd(sim_pref_ptr[sim_idx]->srv_domain_pref);

    sub_other_data->prm.gw.acq_order_pref
            = cmph_map_cm_acq_order_pref_to_sd(sim_pref_ptr[sim_idx]->acq_order_pref);

    sub_other_data->hybr_pref
            = cmph_map_cm_hybr_pref_to_sd_hybr_pref(sim_pref_ptr[sim_idx]->hybr_pref);
    sub_other_data->hybr_pref
            = cmmsc_auto_is_hybr_allowed(sub_other_data->mode_pref,
                                          sub_other_data->hybr_pref,
                                          sim_idx) ?
              sub_other_data->hybr_pref: SD_SS_HYBR_PREF_NONE;

    sub_other_data->orig_mode
            = cmph_map_cm_network_sel_mode_pref_to_orig_mode(
                                        sim_pref_ptr[sim_idx]->network_sel_mode_pref);
    sub_other_data->roam_pref
            = cmph_map_cm_roam_pref_to_sd_roam_pref(sim_pref_ptr[sim_idx]->roam_pref);

    #if defined( FEATURE_MMODE_SXLTE_G )
    /* Update srv_domain_pref of ph_ptr, with srv_domain_pref CM is sending to MMOC.
      ** This is to keep domain_pref in CM in sync with domain pref in MMOC.
      */
    if(sim_idx == 1 && cmph_is_sxlte())
    {
      sub_other_data->ss                      = SD_SS_HYBR_3;
	  if(cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw(SYS_MODEM_AS_ID_2,&session_type) != CM_MMGSDI_SUCCESS)
      {
        session_type = MMGSDI_GW_PROV_SEC_SESSION;
      }
      sub_other_data->session_type            = session_type;
      sub_other_data->as_id = SYS_MODEM_AS_ID_2;
      sub_other_data->nv_context
            = cmph_ptr()->hybr_3_stack_info.nv_context;
      manual_sys_info.sys_id.id.plmn
            = cmph_ptr()->hybr_3_stack_info.pref_info.plmn;
      sub_other_data->prm.gw.manual_sys_info  = manual_sys_info;

	  sub_other_data->subs_capability = cmpmprx_fetch_max_mode_capability(SYS_MODEM_AS_ID_2);
    }
	else 
   #endif
    if (sim_idx == 1 )
    {
      sub_other_data->ss                      = SD_SS_HYBR_2;
	  if(cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw(SYS_MODEM_AS_ID_2,&session_type) != CM_MMGSDI_SUCCESS)
      {
        session_type = MMGSDI_GW_PROV_SEC_SESSION;
      }
      sub_other_data->session_type            = session_type;
      sub_other_data->as_id = SYS_MODEM_AS_ID_2;
      sub_other_data->nv_context
            = cmph_ptr()->hybr_2_stack_info.nv_context;
      manual_sys_info.sys_id.id.plmn
            = cmph_ptr()->hybr_2_stack_info.pref_info.plmn;
      sub_other_data->prm.gw.manual_sys_info  = manual_sys_info;

      cmph_ptr()->hybr_2_stack_info.pref_info.srv_domain_pref
            = sim_pref_ptr[sim_idx]->srv_domain_pref;
	  sub_other_data->subs_capability = cmpmprx_fetch_max_mode_capability(SYS_MODEM_AS_ID_2);	  
    }

    #if defined(FEATURE_MMODE_TRIPLE_SIM) 
    if (sim_idx == 2)
    {
      sub_other_data->ss                      = SD_SS_HYBR_3;
      sub_other_data->session_type            = MMGSDI_GW_PROV_TER_SESSION;
      sub_other_data->as_id
            =SYS_MODEM_AS_ID_3;
      sub_other_data->nv_context
            = cmph_ptr()->hybr_3_stack_info.nv_context;
      manual_sys_info.sys_id.id.plmn
            = cmph_ptr()->hybr_3_stack_info.pref_info.plmn;
      sub_other_data->prm.gw.manual_sys_info  = manual_sys_info;

      cmph_ptr()->hybr_3_stack_info.pref_info.srv_domain_pref
            = sim_pref_ptr[sim_idx]->srv_domain_pref;
	  sub_other_data->subs_capability = cmpmprx_fetch_max_mode_capability(SYS_MODEM_AS_ID_3);
    }
    #endif

    CM_MSG_HIGH_6("CM_SUB_DATA: sim_idx=%d, mode_pref=0x%x, orig_mode=%d, pm rat_cap=0x%x, srv_domain %d, subs_cap %d",
                    sim_idx,
                    sub_other_data->mode_pref,
                    sub_other_data->orig_mode,
                   state_machine->stack_common_info.ue_mode_capability,
                  sub_other_data->prm.gw.srv_domain_pref,
                  sub_other_data->subs_capability);

  }
}


/*===========================================================================

FUNCTION cmmsc_proc_cmd_subsc_chgd_set_capability

DESCRIPTION
  This function will set the capability of each stacks, based on given subsc
  scription. We want to split the subscription process as subscription is sent
  on Main, but capability is set from stack (cmmsc).
  - CMMSIMC: 1. set subscription capability
  - CMMSIMC: 2. send subscription on MAIN
  - CMMSIMC: 3. Trigger CMMSC.

  Key note: we don't want to trigger CMMSC Routing Table before CMMSIMC because
  we might take action on CMMSC, which we don't want to take before subscription
  are sent.

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_proc_cmd_subsc_chgd_set_capability(
     cmmsc_param_subs_chgd_s_type  * subs_event
)
{
  uint8                        sim_idx = 0;
  cmmsc_state_machine_s_type  *state_machine = NULL;
  uint8                        sub_data_idx  = 0;

  if(subs_event == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  for(sim_idx = 0; sim_idx< cmpmprx_get_num_of_sims() && sim_idx < MAX_SIMS; sim_idx++)
  {
    if (subs_event->prot_sub_chg[sim_idx])
    {
      /* Update op mode per feature mode */
      if (sim_idx != 0)
      {
       sub_data_idx = sim_idx + 1;
      }
      if((state_machine = cmmsimc_state_machine_ptr_per_sim_idx(sim_idx)) == NULL)
      {
        CM_MSG_HIGH_1("Could not get state_machine for sim %d", sim_idx);
        continue;
      }
      //If subscription is available just after emergency call end(<4 secs)
        //Update this boolean with latest status
      state_machine->stack_common_info.is_in_emergency_call = cmcall_emergency_call_active();

      if (sub_data_idx < MAX_SUB_SESSION_TYPES)
      {
        sys_modem_as_id_e_type asubs_id = (sys_modem_as_id_e_type)sim_idx;

        /* Update op mode per feature mode */
        switch(cmph_get_subs_feature_mode(asubs_id))
        {
          case SYS_SUBS_FEATURE_MODE_SGLTE:
                  cmmsc_sglte_update_op_mode(state_machine, subs_event->sub_data[sub_data_idx]->is_subs_avail);
            break;
      
          case SYS_SUBS_FEATURE_MODE_SVLTE:
                case SYS_SUBS_FEATURE_MODE_SRLTE:
          case SYS_SUBS_FEATURE_MODE_NORMAL:
          {
            cmmsc_auto_update_op_mode(
                                            state_machine,
                                      FALSE,
                                      TRUE,
                                            subs_event->sub_data[sub_data_idx]->mode_pref,
                                            subs_event->sub_data[sub_data_idx]->hybr_pref,
                                            subs_event->sub_data[sub_data_idx]->is_subs_avail, //gw primary 
                                            subs_event->sub_data[1]->is_subs_avail, //cdma primary
                                            NULL);
            break;
          }
      
          default:
            break;
        }
      }

      /* Setting stacks capability */
      cmmsc_set_stack_capability(state_machine);
    }
    else if(sim_idx == 0)
    {
      cmmsc_set_stack_capability(subs_event->state_machine);
    }
  }

}



/*===========================================================================

FUNCTION cmmsc_proc_cmd_subsc_chgd

DESCRIPTION
  This function should be called when the subscrption availability has
  changed.This function will pack the info and in turn call the function
  cmmsc_proc_cmd_subsc_chgd_.

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_proc_cmd_subsc_chgd
(
  cmmsc_param_subs_chgd_s_type  * subs_event
)
{
  cmmsc_event_e_type            event = CMMSC_EVENT_NONE;
  cmmsc_event_sub_chg_s_type    event_sub_chg;
  cmmsc_state_machine_s_type   *state_machine 
    = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);
  size_t                        sim_idx = 0;

  event = CMMSC_EVENT_SUB;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  for (sim_idx = 0; sim_idx < MIN(MAX_SIMS, (cmpmprx_get_num_of_sims()) ); sim_idx++)
  {
  
    if (( subs_event->prot_sub_chg[sim_idx] != PROT_SUBSC_CHG_NONE ||
          subs_event->apply_standby_pref_chg)
          &&
         ( subs_event->active_subs & BM(sim_idx))
    )
    {
      state_machine = cmmsimc_state_machine_ptr((sys_modem_as_id_e_type)sim_idx);
       /* Setting stacks capability */
      cmmsc_set_stack_capability(state_machine);

      if(cmph_is_subs_feature_mode_1x_sxlte(state_machine->asubs_id))
      {
        CM_MSG_HIGH_0("MSC: reset kick");
        *cmph_get_is_kicked_hybr_ptr() = FALSE;
        *cmph_get_is_kicked_hybr_2_ptr() = FALSE;
      }
      
      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      /* Package the event with parameters */
      event_sub_chg.event = event;
      event_sub_chg.state_machine = state_machine;
      event_sub_chg.nam = subs_event->nam;
      event_sub_chg.chg_type = subs_event->chg_type;
      event_sub_chg.size_of_sub_data = subs_event->size_of_sub_data;
      memscpy( event_sub_chg.prot_sub_chg,
               sizeof(prot_subsc_chg_e_type)*MAX_SIMS,
               subs_event->prot_sub_chg,
               sizeof(prot_subsc_chg_e_type)*MAX_SIMS );

      memscpy( event_sub_chg.sub_data,
               sizeof(mmoc_subs_data_s_type*)*MAX_SUB_SESSION_TYPES,
               subs_event->sub_data,
               sizeof(mmoc_subs_data_s_type*)*MAX_SUB_SESSION_TYPES );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if ( subs_event->prot_sub_chg[sim_idx] != PROT_SUBSC_CHG_NONE )
      {
        state_machine->stack_common_info.ue_mode_forced = TRUE;
      }

      /* Consult & Process in Routing Table */
      cmmsc_state_machine_process(state_machine,
                                  (cmmsc_event_base_s_type*)&event_sub_chg);
    }
  }

}

void cmmsc_proc_cmd_on_off(
     boolean                   is_stack_deactivated,
     cmmsc_param_base_s_type * event_package
)
{
  cmmsc_event_e_type            event = CMMSC_EVENT_NONE;
  cmmsc_event_base_s_type       event_on_off;

  if(is_stack_deactivated == FALSE)
  {
    event = CMMSC_EVENT_ON;
  }
  else
  {
    event = CMMSC_EVENT_OFF;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  event_on_off.event = event;
  event_on_off.state_machine = event_package->state_machine;

  /* Consult & Process in Routing Table */
  cmmsc_state_machine_process(event_on_off.state_machine, &event_on_off);
}


void cmmsc_update_sub_data_per_cap (
        mmoc_subs_data_s_type          *sub_other_data,
        /* Subscription data for sim != 0
        */

        size_t                          sim_idx
)
{
  cmmsc_state_machine_s_type  *state_machine = cmmsimc_state_machine_ptr_per_sim_idx(sim_idx);
  /* Always take the capability of first stack because subscription is sent only on ss=MAIN */
  cmmsc_stack_s_type          *stack_pointer = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(sub_other_data == NULL || state_machine == NULL)
  {
    return;
  }

  stack_pointer = &state_machine->stack[0];

  /* Update stack preference based on capability */
  sub_other_data->mode_pref =
      SD_GET_COMMON_MODE( sub_other_data->mode_pref,
      stack_pointer->capablity.mode);

  sub_other_data->band_pref =
      SD_GET_COMMON_BAND( sub_other_data->band_pref,
      stack_pointer->capablity.band);

  sub_other_data->tds_band_pref =
      SD_GET_COMMON_BAND( sub_other_data->tds_band_pref,
      stack_pointer->capablity.tds_band);

  sub_other_data->lte_band_pref =
      SYS_LTE_BAND_MASK_COMMON( &(sub_other_data->lte_band_pref),
                                &(stack_pointer->capablity.lte_band));

  sub_other_data->prm.gw.srv_domain_pref
    = cmmsc_get_common_domain( sub_other_data->prm.gw.srv_domain_pref,
                               stack_pointer->capablity.domain);

}


static void cmmsc_get_stack_pref_per_cap (
       cmmsc_state_machine_s_type    *state_machine,
       int                            stack_idx,
       cm_preference_info_s_type     *pref_info,
       cmmsc_stack_capability_s_type *common_cap_ptr
)
{

  cmmsc_stack_s_type            *stack_pointer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pref_info == NULL)
  {
    CM_ERR_0("MSC: pref info null ptr");
    return;
  }

  if(common_cap_ptr == NULL)
  {
    CM_ERR_0("MSC: stack_cap_ptr null");
    return;
  }

  if (state_machine == NULL)
    return;

  stack_pointer = &state_machine->stack[stack_idx];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get mode pref from hybr1 specific pref */
  if (cmmsc_mapping_stack_id_to_ss(stack_idx, state_machine) == CM_SS_HYBR_1)
  {
    common_cap_ptr->mode =
        SD_GET_COMMON_MODE(
           cmph_map_cm_mode_pref_to_sd_mode_pref(pref_info->hybr1_pref.hybr_mode_pref),
           stack_pointer->capablity.mode);

  }
  else
  {                  
    common_cap_ptr->mode =
        SD_GET_COMMON_MODE(
           cmph_map_cm_mode_pref_to_sd_mode_pref(pref_info->mode_pref),
           stack_pointer->capablity.mode);

  }

  /* For sglte dual stack mode, if main stack has GSM only, then set mode to none.
   ** This is done due to NAS limiation - NAS is unable to handle G + G
   */
  cmmsc_special_mode_update_for_main_stack(state_machine,
                                           stack_idx, &(common_cap_ptr->mode));

  common_cap_ptr->band =
      SD_GET_COMMON_BAND(
         cmph_map_cm_band_pref_to_sd_band_pref(pref_info->band_pref),
         stack_pointer->capablity.band);

  common_cap_ptr->tds_band =
      SD_GET_COMMON_BAND(
         cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(pref_info->tds_band_pref),
         stack_pointer->capablity.tds_band);

  common_cap_ptr->lte_band =
      SYS_LTE_BAND_MASK_COMMON(
         &(pref_info->lte_band_pref),
         &(stack_pointer->capablity.lte_band));

  common_cap_ptr->domain
    = cmmsc_get_common_domain(
          cmph_map_cm_srv_domain_pref_to_sd(pref_info->srv_domain_pref),
          stack_pointer->capablity.domain );

  if(common_cap_ptr->band != cmph_map_cm_band_pref_to_sd_band_pref(pref_info->band_pref)
    || ((common_cap_ptr->mode & SD_SS_MODE_PREF_TDS) 
         && common_cap_ptr->tds_band != cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(pref_info->tds_band_pref))
    || ((common_cap_ptr->mode & SD_SS_MODE_PREF_LTE) 
         && !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&(common_cap_ptr->lte_band), &(pref_info->lte_band_pref)))
     )
  {
    CM_MSG_HIGH_4("MSC->MMOC band_pref = 0x%08x %08x. tds_band_pref = 0x%08x %08x",
                                                         QWORD_HIGH(pref_info->band_pref),
                                                         QWORD_LOW(pref_info->band_pref),
                                                         QWORD_HIGH(pref_info->tds_band_pref),
                                                         QWORD_LOW(pref_info->tds_band_pref));
    cm_print_lte_band_mask(pref_info->lte_band_pref);
  }
  
}

static void cmmsc_get_user_mode_pref_per_cap (
       cmmsc_state_machine_s_type    *state_machine,
       int                            stack_idx,
       sd_ss_mode_pref_e_type         *user_mode_pref
)
{
  cmmsc_stack_s_type            *stack_pointer = &state_machine->stack[stack_idx];

  if(user_mode_pref == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  *user_mode_pref =
      SD_GET_COMMON_MODE( *user_mode_pref, stack_pointer->capablity.mode);

  /* For sglte dual stack mode, if main stack has GSM only, then set mode to none.
   ** This is done due to NAS limiation - NAS is unable to handle G + G
   */
  cmmsc_special_mode_update_for_main_stack(state_machine,
                                           stack_idx, user_mode_pref);

}

sd_ss_srv_domain_pref_e_type cmmsc_get_common_domain (
  sd_ss_srv_domain_pref_e_type  domain1,
  sd_ss_srv_domain_pref_e_type  domain2
)
{
  /* Get the common domain pref of 2 domain_pref */
  if (domain1 == domain2)
  {
    return domain1;
  }
  else if ((domain1 == SD_SS_SRV_DOMAIN_PREF_CS_ONLY
            && domain2 == SD_SS_SRV_DOMAIN_PREF_PS_ONLY) ||
           (domain1 == SD_SS_SRV_DOMAIN_PREF_PS_ONLY
            && domain2 == SD_SS_SRV_DOMAIN_PREF_CS_ONLY))
  {
    CM_MSG_HIGH_2("MSC: Common domain none - domain1 %d domain2 %d",domain1,domain2);
    return SD_SS_SRV_DOMAIN_PREF_NONE;
  }
  else if(domain1 == SD_SS_SRV_DOMAIN_PREF_CS_PS)
  {
    return domain2;
  }
  else if (domain2 == SD_SS_SRV_DOMAIN_PREF_CS_PS)
  {
    return domain1;
  }
  else
  {
    CM_MSG_HIGH_2("MSC: Common domain none - domain1 %d domain2 %d",domain1,domain2);
    return SD_SS_SRV_DOMAIN_PREF_NONE;
  }
}



static void  cmmsc_print_route_table_info(
    cmmsc_state_machine_s_type *state_machine,
    cmmsc_event_e_type  event,
    int                 route_tbl_idx
)
{
  cmmsc_routing_table_s_type *route_table =
                              cmmsc_route_table_ptr(state_machine->route_tbl);
  cmmsc_routing_table_auto_entry_s_type *auto_entry =
                    (cmmsc_routing_table_auto_entry_s_type *)route_table->route_tbl;

  /* Print event and op mode UE will operate on */
  if (route_tbl_idx != -1)
  {
    CM_MSG_HIGH_6("MSC: sub=%d, ue_mode=%d, tbl_id=%d, tbl event=%d, op_mode=%d, route_tbl_idx=%d",
                   state_machine->asubs_id,
                   state_machine->stack_common_info.ue_mode,
                   state_machine->route_tbl,
                   event,
                   state_machine->op_mode,
                   route_tbl_idx);
  }
  else
  {
    CM_MSG_HIGH_5("MSC: sub=%d, ue_mode=%d, tbl_id=%d,  tbl event=%d, op_mode=%d, No entry found",
                   state_machine->asubs_id,
                   state_machine->stack_common_info.ue_mode,
                   state_machine->route_tbl,
                   event,
                   state_machine->op_mode);
  }
                   
  /* Print current stack state */
  if(route_table->n_stack == CMMSC_AUTO_STACK_NUM && ARR_SIZE(state_machine->stack) >= 3)
  {
    CM_MSG_HIGH_6("MSC: stk0 curr state %d tbl_state %d, stk1 curr state %d tbl_state %d, stk2 curr state %d tbl_state %d",
                  state_machine->stack[0].state,
                  auto_entry[route_tbl_idx].state[0],
                  state_machine->stack[1].state,
                  auto_entry[route_tbl_idx].state[1],
                  state_machine->stack[2].state,
                  auto_entry[route_tbl_idx].state[2]);
  }
}

/*===========================================================================

FUNCTION cmmsc_is_op_mode_multi_stacks

DESCRIPTION
  This function returns if the op_mode is multi-stack operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean cmmsc_is_op_mode_multi_stacks (
  cmmsc_state_machine_op_mode_e_type op_mode
)
{
  switch(op_mode)
  {
    case CMMSC_OP_MODE_SGLTE_DUAL_STACK:
      return TRUE;

    case CMMSC_OP_MODE_SGLTE_SINGLE_STACK:
    case CMMSC_OP_MODE_SGLTE_ROAM:
    case CMMSC_OP_MODE_NORMAL:
    case CMMSC_OP_MODE_FULL_RAT_SEARCH:
      return FALSE;

    /* feature mask auto specific op mode */
    case CMMSC_OP_MODE_AUTO_MAIN_HYBR1:
    case CMMSC_OP_MODE_AUTO_MAIN_HYBR2:
    case CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2:
      return TRUE;

    /* feature mask auto specific op mode */
    case CMMSC_OP_MODE_AUTO_MAIN:
      return FALSE;

    /* Invalid op_mode for this function */
    case CMMSC_OP_MODE_NONE:
    case CMMSC_OP_MODE_ANY:
    case CMMSC_OP_MODE_MAX:
    default:
      CM_ERR_1("MSC: invalid op_mode %d",op_mode);
      return FALSE;
  }
}

/*===========================================================================

FUNCTION cmmsc_is_msc_multi_stacks

DESCRIPTION
  This function returns if the curr op_mode is multi-stack operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmmsc_is_msc_multi_stacks (sys_modem_as_id_e_type as_id)
{

  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(as_id);

  cmmsc_state_machine_op_mode_e_type op_mode = state_machine->op_mode;

  return (cmmsc_is_op_mode_multi_stacks(op_mode));
}

/*===========================================================================

FUNCTION cmmsc_is_valid_ss_per_op_mode

DESCRIPTION
  This function returns if the cm_ss is valid base on the current op mode.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmmsc_is_valid_ss_per_op_mode (
  cmmsc_state_machine_s_type  *state_machine, 
  int                         cm_stack_id,
  cm_hybr_pref_e_type         hybr_pref
)
{
  cmmsc_state_machine_op_mode_e_type op_mode = state_machine->op_mode;

  if ( cm_stack_id == 2 &&
     state_machine->route_tbl == CMMSC_ROUTE_TABLE_AUTO &&
     op_mode != CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2 &&
     op_mode != CMMSC_OP_MODE_AUTO_MAIN_HYBR2 &&
     hybr_pref != CM_HYBR_PREF_CDMA__LTE__HDR
     )
  {
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION cmmsc_is_volte_active_on_hybr2

DESCRIPTION
  This function returns if volte call is active on hybr2.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean cmmsc_is_volte_active_on_hybr2 (
  cmmsc_state_machine_s_type  *state_machine  
)
{
  cm_orig_q_s_type *ph_orig_top_ptr = cmtask_orig_para_get_top(CM_SS_HYBR_2);

  if(ph_orig_top_ptr == NULL)
  {
    CM_MSG_HIGH_0("Top_ptr is NULL for HYBR2 stack");
    return FALSE;
  }

  /* Check if svlte hybr2 is in a volte call
  */
  if (cmph_is_subs_feature_mode_1x_sxlte(state_machine->asubs_id) &&
     ph_orig_top_ptr->orig->orig_mode == SD_SS_ORIG_MODE_VOLTE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================

FUNCTION cmmsc_force_all_stack_required

DESCRIPTION
  This function returns if msc needs to force the preference to all stacks.
  Pre-condition: CMPH indicates ss = CM_SS_MAX.
  Conditions:
  1. If previous op_mode is multi_stack opeation, and new op_mode is single stack operation.
  2. If previous op_mode is single_stack opeation, and new op_mode is multi stack operation.
  3. If the new op_mode is multi-stack operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean cmmsc_force_all_stack_required (
  cmmsc_state_machine_op_mode_e_type prev_op_mode,
  cmmsc_state_machine_op_mode_e_type new_op_mode
)
{
  CM_ASSERT_ENUM_IS_INRANGE(prev_op_mode,CMMSC_OP_MODE_MAX);
  CM_ASSERT_ENUM_IS_INRANGE(new_op_mode,CMMSC_OP_MODE_MAX);

  if (( cmmsc_is_op_mode_multi_stacks(prev_op_mode) &&
        !cmmsc_is_op_mode_multi_stacks(new_op_mode))
                          ||
      ( !cmmsc_is_op_mode_multi_stacks(prev_op_mode) &&
        cmmsc_is_op_mode_multi_stacks(new_op_mode))
                          ||
       (cmmsc_is_op_mode_multi_stacks(new_op_mode))
       )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


static cmmsc_event_e_type cmmsc_get_route_tbl_event(
       cmmsc_routing_table_s_type *route_table,
       int route_idx
)
{
  if(route_table == NULL)
    return CMMSC_EVENT_NONE;

  if(route_idx >= route_table->n_row)
  {
    sys_err_fatal_invalid_value_exception(route_idx);
  }

  switch (route_table->route_tbl_id)
  {
    case CMMSC_ROUTE_TABLE_SGLTE:
    {
      cmmsc_routing_table_sglte_entry_s_type *sglte_entry;
      sglte_entry = (cmmsc_routing_table_sglte_entry_s_type *)route_table->route_tbl;
      return sglte_entry[route_idx].event;
    }

    case CMMSC_ROUTE_TABLE_AUTO:
    {
      cmmsc_routing_table_auto_entry_s_type *auto_entry;
      auto_entry = (cmmsc_routing_table_auto_entry_s_type *)route_table->route_tbl;
      return auto_entry[route_idx].event;
    }

    default:
      break;
  }

  return CMMSC_EVENT_NONE;
}


static cmmsc_state_machine_op_mode_e_type cmmsc_get_route_tbl_mode(
       cmmsc_routing_table_s_type *route_table,
       int route_idx
)
{
  if(route_table == NULL)
    return CMMSC_OP_MODE_NONE;

  if(route_idx >= route_table->n_row)
  {
    sys_err_fatal_invalid_value_exception(route_idx);
  }

  switch (route_table->route_tbl_id)
  {
    case CMMSC_ROUTE_TABLE_SGLTE:
    {
      cmmsc_routing_table_sglte_entry_s_type *sglte_entry;
      sglte_entry = (cmmsc_routing_table_sglte_entry_s_type *)route_table->route_tbl;
      return sglte_entry[route_idx].mode;
    }

    case CMMSC_ROUTE_TABLE_AUTO:
    {
      cmmsc_routing_table_auto_entry_s_type *auto_entry;
      auto_entry = (cmmsc_routing_table_auto_entry_s_type *)route_table->route_tbl;
      return auto_entry[route_idx].mode;
    }

    default:
      break;
  }

  return CMMSC_OP_MODE_NONE;
}


static cmmsc_stack_state_e_type cmmsc_get_route_tbl_stack_state(
       cmmsc_routing_table_s_type *route_table,
       int stack_idx,
       int route_idx
)
{
  if(route_table == NULL)
    return CMMSC_STACK_STATE_NO_CHG;

  if(route_idx >= route_table->n_row ||
     stack_idx >= route_table->n_stack)
  {
    sys_err_fatal_invalid_value_exception(route_idx);
  }

  switch (route_table->route_tbl_id)
  {
    case CMMSC_ROUTE_TABLE_SGLTE:
    {
      cmmsc_routing_table_sglte_entry_s_type *sglte_entry;
      sglte_entry = (cmmsc_routing_table_sglte_entry_s_type *)route_table->route_tbl;
      return sglte_entry[route_idx].state[stack_idx];
    }

    case CMMSC_ROUTE_TABLE_AUTO:
    {
      cmmsc_routing_table_auto_entry_s_type *auto_entry;
      auto_entry = (cmmsc_routing_table_auto_entry_s_type *)route_table->route_tbl;
      return auto_entry[route_idx].state[stack_idx];
    }

    default:
      break;
  }

  return CMMSC_STACK_STATE_NO_CHG;
}

static cmmsc_stack_state_e_type cmmsc_get_route_tbl_stack_new_state(
       cmmsc_routing_table_s_type *route_table,
       int stack_idx,
       int route_idx
)
{
  if(route_table == NULL)
    return CMMSC_STACK_STATE_NO_CHG;

  if(route_idx >= route_table->n_row ||
     stack_idx >= route_table->n_stack)
  {
    sys_err_fatal_invalid_value_exception(route_idx);
  }

  switch (route_table->route_tbl_id)
  {
    case CMMSC_ROUTE_TABLE_SGLTE:
    {
      cmmsc_routing_table_sglte_entry_s_type *sglte_entry;
      sglte_entry = (cmmsc_routing_table_sglte_entry_s_type *)route_table->route_tbl;
      return sglte_entry[route_idx].new_state[stack_idx];
    }

    case CMMSC_ROUTE_TABLE_AUTO:
    {
      cmmsc_routing_table_auto_entry_s_type *auto_entry;
      auto_entry = (cmmsc_routing_table_auto_entry_s_type *)route_table->route_tbl;
      return auto_entry[route_idx].new_state[stack_idx];
    }

    default:
      break;
  }

  return CMMSC_STACK_STATE_NO_CHG;
}


static cmmsc_action_s_type *cmmsc_get_route_tbl_action(
       cmmsc_routing_table_s_type *route_table,
       int stack_idx,
       int route_idx
)
{
  if(route_table == NULL)
    return NULL;

  if(route_idx >= route_table->n_row ||
     stack_idx >= route_table->n_stack)
  {
    sys_err_fatal_invalid_value_exception(route_idx);
  }

  switch (route_table->route_tbl_id)
  {
    case CMMSC_ROUTE_TABLE_SGLTE:
    {
      cmmsc_routing_table_sglte_entry_s_type *sglte_entry;
      sglte_entry = (cmmsc_routing_table_sglte_entry_s_type *)route_table->route_tbl;
      return &(sglte_entry[route_idx].action[stack_idx]);
    }

    case CMMSC_ROUTE_TABLE_AUTO:
    {
      cmmsc_routing_table_auto_entry_s_type *auto_entry;
      auto_entry = (cmmsc_routing_table_auto_entry_s_type *)route_table->route_tbl;
      return &(auto_entry[route_idx].action[stack_idx]);
    }

    default:
      break;
  }

  return NULL;
}

/*===========================================================================

FUNCTION cmmsc_map_sim_id_to_stack_id

DESCRIPTION
  This function returns stack_id from sim index.

DEPENDENCIES
  None.

RETURN VALUE
  stack index.

SIDE EFFECTS
  None.
===========================================================================*/
size_t  cmmsc_map_sim_id_to_stack_id (
    size_t  sim_index
)
{
  switch (sim_index)
  {
    case 0:
    case 1:
    case 2:
      return sim_index;

    default:
     CM_ERR_1("MSC: Invalid sim_index %d, default to 0", sim_index);
     return 0;
  }
}

/*===========================================================================

FUNCTION cmmsc_get_cm_ss_mode_cap

DESCRIPTION
  This function returns the mode pref of the top priority queue base on the capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_mode_pref_e_type  cmmsc_get_cm_ss_mode_cap (
  cm_ss_e_type  cm_ss
)
{
  cmmsc_stack_s_type          *stack_pointer = NULL;
  int                         stack_idx = cmmsc_mapping_ss_to_stack_idx(cm_ss);
  cmmsc_state_machine_s_type  *state_machine
    = cmmsimc_get_msc_ptr_per_cm_ss(cm_ss);

  if (state_machine == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  stack_pointer = &state_machine->stack[stack_idx];
  return stack_pointer->capablity.mode;
}


#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)

/*===========================================================================

FUNCTION cmmsc_get_cm_ss_top_mode_pref_and_cap

DESCRIPTION
  This function returns the mode pref of the top priority queue base on the capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_mode_pref_e_type  cmmsc_get_cm_ss_top_mode_pref_and_cap (
  cm_ss_e_type  cm_ss
)
{
  cmmsc_stack_s_type          *stack_pointer = NULL;
  int                         stack_idx = cmmsc_mapping_ss_to_stack_idx(cm_ss);
  cm_orig_q_s_type            *ph_orig_top_ptr = cmtask_orig_para_get_top(cm_ss);
  sd_ss_mode_pref_e_type      mode_pref = SD_SS_MODE_PREF_NONE;
  cmmsc_state_machine_s_type  *state_machine
    = cmmsimc_get_msc_ptr_per_sd_ss(cm_ss);

  if (state_machine == NULL)
    return mode_pref;

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_1("MSC: Top element on priority queue of stack_idx=%d is NULL",
            stack_idx);
    return mode_pref;
  }

  stack_pointer = &state_machine->stack[stack_idx];
  mode_pref = SD_GET_COMMON_MODE(
                          cmph_map_cm_mode_pref_to_sd_mode_pref(
                          ph_orig_top_ptr->orig->orig_mode_pref),
                          stack_pointer->capablity.mode);
  return mode_pref;
}

/*===========================================================================

FUNCTION cmmsc_get_sd_ss_top_mode_pref_and_cap

DESCRIPTION
  This function returns the mode pref of the top priority queue base on the capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_mode_pref_e_type  cmmsc_get_sd_ss_top_mode_pref_and_cap (
  sd_ss_e_type  sd_ss
)
{
  cm_ss_e_type cm_ss = cmph_map_sd_ss_type_to_cm_ss_type(sd_ss);
  return cmmsc_get_cm_ss_top_mode_pref_and_cap(cm_ss);
}
#endif

/*===========================================================================

FUNCTION cmmsc_is_in_srv_state

DESCRIPTION
  The function returns if the stack is in service.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_is_in_srv_state (
  cmmsc_state_machine_s_type  *state_machine,
  int                         stack_idx
)
{
  if (state_machine == NULL)
  {
    CM_ERR_0("MSC: state_machine NULL ptr");
    return FALSE;
  }

  return (state_machine->stack[stack_idx].state == CMMSC_STACK_STATE_SRV);
}

/*===========================================================================

===========================================================================*/
boolean  cmmsc_is_in_ext_no_srv(
  cmmsc_state_machine_s_type  *state_machine,
  int                         stack_idx
)
{
  if (state_machine == NULL)
  {
    CM_ERR_0("MSC: state_machine NULL ptr");
    return FALSE;
  }

  return (state_machine->stack[stack_idx].state == CMMSC_STACK_STATE_HDR_EXT_NO_SRV);
}

/*===========================================================================

FUNCTION cmmsc_is_in_activated_state

DESCRIPTION
  Checks if a stack either in ACTIVE or SERVICE state. Basically if stack
  is looking for srv or found it.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_is_in_activated_state (
  cmmsc_state_machine_s_type  *state_machine,
  int                         stack_idx
)
{
  if (state_machine == NULL)
  {
    CM_ERR_0("MSC: state_machine NULL ptr");
    return FALSE;
  }

  return (state_machine->stack[stack_idx].state == CMMSC_STACK_STATE_SRV ||
          state_machine->stack[stack_idx].state == CMMSC_STACK_STATE_ACTIVE ||
          state_machine->stack[stack_idx].state == CMMSC_STACK_STATE_HDR_EXT_NO_SRV);
}

/*===========================================================================

FUNCTION cmmsc_action_sxlte_kick_not_auto

DESCRIPTION

Kick HYBR2 stack on standby preference change if the user standby
preference is not SYS_MODEM_DS_PREF_AUTO.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cmmsc_action_sxlte_kick_not_auto(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
)
{
  cm_ss_e_type ss = cmmsc_mapping_stack_id_to_ss(stack_idx, msc);

  switch(ss)
  {
    case CM_SS_HYBR_2:

      if (cmph_is_subs_feature_mode_sglte(msc->asubs_id) &&
          (cmph_ptr()->user_standby_pref != SYS_MODEM_DS_PREF_AUTO)
         )
      {
        cmmsc_action_pending_kick(msc, stack_idx, TRUE);
      }
      else if (cmph_ptr()->user_standby_pref != SYS_MODEM_DS_PREF_AUTO)
      {	  
        if(!cmmsc_is_hybr2_kick_allowed(msc))
        {
          cmmsc_auto_update_is_kick_hybr2_pending(msc, TRUE);
        }
        else
        {
          cmmsc_action_pending_kick(msc, stack_idx, TRUE);
        }
      }
      break;
	  
    default:
      break;
  }
}

void cmmsc_update_stack_preference(

        cmmsc_state_machine_s_type       *state_machine,
            /* Subscription for which preference need to be updated */
        
        cm_mode_pref_e_type              mode_pref,
            /* Mode preference which to update */
        
        cm_band_pref_e_type              band_pref,
            /* Band preference which to update */
        
        sys_lte_band_mask_e_type         lte_band_pref,
            /* LTE band preference which to update */
        
        cm_band_pref_e_type              tds_band_pref
            /* LTE band preference which to update */

)
{
 
  if(  state_machine->stack_common_info.preference.mode_pref != mode_pref ||
  state_machine->stack_common_info.preference.band_pref != band_pref ||
  !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&(state_machine->stack_common_info.preference.lte_band_pref), &lte_band_pref) ||
  state_machine->stack_common_info.preference.tds_band_pref != tds_band_pref
  )
  {
    CM_MSG_HIGH_6("update_stack_preference(): mode_pref Old %d, new %d,band_pref Old 0x%x, new 0x%x, tds_band_pref Old 0x%x, new 0x%x", 
                  state_machine->stack_common_info.preference.mode_pref, mode_pref, state_machine->stack_common_info.preference.band_pref, 
                  band_pref, state_machine->stack_common_info.preference.tds_band_pref, tds_band_pref);
    cm_print_lte_band_mask(lte_band_pref);
  }  
  state_machine->stack_common_info.preference.mode_pref = mode_pref;
  state_machine->stack_common_info.preference.band_pref = band_pref;
  state_machine->stack_common_info.preference.lte_band_pref = lte_band_pref;
  state_machine->stack_common_info.preference.tds_band_pref = tds_band_pref;

}

void cmmsc_set_stack_common_call_mode_per_subs(sys_call_mode_t call_mode, sys_modem_as_id_e_type subs_id)
{
  cmmsc_state_machine_s_type *state_machine = NULL;
  if(subs_id>SYS_MODEM_AS_ID_NONE && subs_id<SYS_MODEM_AS_ID_NO_CHANGE)
  {
    state_machine = cmmsimc_state_machine_ptr(subs_id);
	state_machine->stack_common_info.call_mode = call_mode;
	CM_MSG_LOW_2("setting call mode %d, for subs %d",call_mode, subs_id);
  }
  else
  {
    CM_ERR_FATAL_1("MSIMC: invalid subs id to set call mode as_id=%d",subs_id);
  }
}

sys_call_mode_t cmmsc_get_stack_common_call_mode_per_subs(sys_modem_as_id_e_type subs_id)
{
  cmmsc_state_machine_s_type *state_machine = NULL;
  sys_call_mode_t call_mode = SYS_CALL_MODE_NORMAL;
  
  if(subs_id > SYS_MODEM_AS_ID_NONE && subs_id < SYS_MODEM_AS_ID_NO_CHANGE)
  {
    state_machine = cmmsimc_state_machine_ptr(subs_id);
	call_mode = state_machine->stack_common_info.call_mode;
	CM_MSG_LOW_2("returning call mode %d, for subs %d", call_mode, subs_id);
  }
  else
  {
    CM_ERR_FATAL_1("MSIMC: invalid subs id to fetch call mode as_id=%d", subs_id);
  }
  return call_mode;
}

/*===========================================================================

===========================================================================*/

boolean cmmsc_is_hybr2_kick_allowed(

  cmmsc_state_machine_s_type *msc

)
{
  boolean allowed = TRUE;

  /* if in emerg mode, do not kick LTE stack with orig mode NONE or pref reas != EMERG;
  ** or if cmregprx is active for main, do not kick LTE.
  */
  #ifdef CM_GW_SUPPORTED
  allowed =( (cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state != CMREGPRX_STATE_ACTIVE &&
               cmph_is_subs_feature_mode_1x_sxlte(SYS_MODEM_AS_ID_1)) ||
	         cmph_is_subs_feature_mode_sglte(SYS_MODEM_AS_ID_1)  ||
            cmph_ptr()->ph_state == CM_PH_STATE_EMERG_CB ||
            cmcall_emergency_call_active()
          );
  CM_MSG_HIGH_3("asubs_id=%d, is_hybr2_kick_allowed = %d, main cmregprx state=%d", 
                 msc->asubs_id, allowed, 
                 cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state);
  #endif


  return allowed;
}
