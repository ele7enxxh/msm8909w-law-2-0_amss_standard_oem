#ifndef CMMSC_AUTO_H
#define CMMSC_AUTO_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
             C M    M U L T I - S T A C K    C O N T R O L L E R

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file implements the CM multi-stack architecture to support multi-stacks
  requirements such as feature mode SVLTE and normal per subscription.

EXTERNALIZED FUNCTIONS
  Functional Interface:



Copyright (c) 2003 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmsc_auto.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/31/14   cl      CM supports explicit PS_DETACH when capability from PM changed
02/25/13   cl      Initial release.

===========================================================================*/

#include "mmcp_variation.h"
#include "target.h"
#include "customer.h"
#include "cmll.h"
#include "cmmmgsdi.h"
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "rex.h"      /* Rex related services */

#include "sys.h"      /* Common types. */
#include "sd.h"       /* Interface for System Determination services */
#include "mmoc.h"     /* MMEXTNal interface definitions for MMoC */

#include "cm_dualsim.h"
#include "cmi.h"
#include "cmph.h"     /* Interface to CM phone object. */
#include "cmmsc.h"
#include "cmss.h"     /* Interface to CM Serving system object. */
#include "cmdbg.h"    /* Interface to CM debug macros.  */

/*===========================================================================

            LOCAL DEFINITIONS OF CONSTANTS AND ENUMS FOR MODULE

This section contains local definitions for constants and enums used in
this module and

===========================================================================*/

#define CMMSC_AUTO_STACK_NUM   3         /* Max num of stacks needed */

/*===========================================================================

                      LOCAL DEFINITIONS OF DATA STRUCTURE

This section contains local definitions for constants and enums used in
this module and

===========================================================================*/


/* AUTO ROUTING TABLE DATA STRUCTURE

   Routing table is a data structure that hold a guideline for CM generic
   algorithm. It contains a set of:

   [event] [stack_states] [actions] [new states]

   where:
   event:         is the event which we are handling.
   mode:          is the current mode/setting which UE is function in.
   stack_states:  is the states of the current stacks.
   action:        is the action to take based on given events and current states
   new states:    is the states that becomes after processing.

*/

typedef struct cmmsc_routing_table_auto_entry_s
{
  /* event */
  cmmsc_event_e_type                   event;

  /* current state-machine operation mode */
  cmmsc_state_machine_op_mode_e_type   mode;

  /* current states of each stack */
  cmmsc_stack_state_e_type             state[CMMSC_AUTO_STACK_NUM];

  /* action index */
  cmmsc_action_s_type                  action[CMMSC_AUTO_STACK_NUM];

  /* New states */
  cmmsc_stack_state_e_type             new_state[CMMSC_AUTO_STACK_NUM];

}cmmsc_routing_table_auto_entry_s_type;



/*===========================================================================
                      LOCAL DEFINITIONS OF FUNCTIONS

            This section contains local functions used in CM
===========================================================================*/

/*===========================================================================

FUNCTION cmmsc_get_auto_routes

DESCRIPTION
  This function return the pointer to auto_routes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
cmmsc_routing_table_auto_entry_s_type *cmmsc_get_auto_routes( void );


/*===========================================================================

FUNCTION cmmsc_get_auto_routes_size

DESCRIPTION
  This function return the size of the auto_routes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int cmmsc_get_auto_routes_size( void );


/*===========================================================================

FUNCTION cmmsc_auto_routing_table_init

DESCRIPTION
  This function initializes AUTO Routing Table

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_auto_routing_table_init( cmmsc_routing_table_s_type *auto_route_table );

/*===========================================================================

FUNCTION cmmsc_auto_proc_rpt_lte_do_irat

DESCRIPTION
  This function should be called to process the reselection reports
  from SD20/MMOC. currently used only for CM_LTE_DO_IRAT_F

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_proc_rpt_lte_do_irat (  
  cmmsc_param_base_s_type  *param
);

/*===========================================================================

FUNCTION cmmsc_auto_proc_rpt_redial_hdr_fail

DESCRIPTION
  This function should be called to process the faked ACQ_FAIL report
  from SD20 to trigger 1s BSR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void   cmmsc_auto_proc_rpt_redial_hdr_fail(
  cmmsc_param_base_s_type    *param
);

/*===========================================================================

FUNCTION cmmsc_auto_proc_disable_hybr_hdr

DESCRIPTION
  This function should be called to disable the HDR acquisitions while in
  hybrid operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_proc_disable_hybr_hdr (
  cmmsc_param_base_s_type    *param
);

/*===========================================================================

FUNCTION cmmsc_auto_proc_enable_hybr_hdr

DESCRIPTION
  This function should be called to enable the HDR acquisitions while in
  hybrid operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void     cmmsc_auto_proc_enable_hybr_hdr (
  cmmsc_param_proc_enable_hybr_hdr_s_type  *param
);

/*===========================================================================

FUNCTION cmmsc_auto_proc_call_end

DESCRIPTION
  This function should be called to process the call end event from CMCALL.
  This function would reinitiate SS-HDR acquisition if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_proc_call_end (  
  cmmsc_param_base_s_type    *param
);

/*===========================================================================

FUNCTION cmmsc_auto_proc_timer

DESCRIPTION
  This function should be called to process the CM's 1 second timer tick.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_proc_timer (  
  cmmsc_param_base_s_type    *param
);

/*===========================================================================

FUNCTION cmpref_proc_rpt_hybr_bsr_to_hdr

DESCRIPTION
  This function should be called to process CM_HYBR_BSR_TO_HDR_F report
  from SD to trigger BSR.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsc_auto_proc_rpt_hybr_bsr_to_hdr(
  cmmsc_param_proc_rpt_hybr_bsr_to_hdr_s_type  *param
);

/*===========================================================================

FUNCTION cmmsc_auto_proc_rpt_kick_hybr2

DESCRIPTION
  This function should be called to process CM_KICK_HYBR2_F report
  from SD indicates get net on main is done kick hybr2.

DEPENDENCIES
  cmpref_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsc_auto_proc_rpt_kick_hybr2(
  cmmsc_param_base_s_type  *param
);

/*===========================================================================

FUNCTION cmmsc_auto_update_gwl_operation

DESCRIPTION
  This function should be called to update the GWL operation on MAIN stack.
  Since HDR and GWL cant operate simiultaneosly, whenever MSC is kicking HDR (or)
  starting HDR (or) knows that HDR is in power_save. it should set the GWL
  operation accordingly

  allowed = TRUE will enable GWL operation of main stack if GWL is supported
                 by the phone+user and current preferences dont include GWL
            FALSE will disable GWL operation on main stack when CDMA (any-gwl)
                  is allowed on the main stack.

  reason = USER           will trigger SD to restart the OOS scripts.
           STACK_SYNC_UP  will trigger SD to add GWL but continue with previous
                          script
           STACk_SYNC_UP_BSR will trigger SD to add GWL and start BSR
                             algorithm.
  is_hdr_acq_fail = TRUE will restore GWL on main and shorten LTE BSR to 1 sec if 1x has
                 service and HDR acq failed.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean    cmmsc_auto_update_gwl_operation            (

       boolean                  allowed,
        /* is GWL operation allowed or not */

       sd_ss_pref_reas_e_type   reason,
         /* Reason for change in GWL operation */

       cm_mode_pref_e_type      cm_mode_pref,
         /* 3gpp mode that needs to be updated */

       boolean                  is_hdr_acq_fail,
         /* is HDR acq fail happened on SD_SS_HYBR_HDR stack */
         
       sys_modem_as_id_e_type   asubs_id
);

/*===========================================================================

FUNCTION cmmsc_auto_restore_3gpp_policy_chgd

DESCRIPTION
  This function restores 3GPP back to phone object if all the conditions meet.
  Conditions:
  1. 3GPP is removed due to HICPS.
  2. and current max mode pref capability is 3GPP only.
  3. and top of the priority queue is phone object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_restore_3gpp_policy_chgd (
  cmmsc_state_machine_s_type  *state_machine,
  cm_ss_e_type                 cm_ss,
  cm_mode_pref_e_type         *mode_pref
);

/*===========================================================================

FUNCTION cmmsc_action_force_ph_ext

DESCRIPTION
  Force the phone preference on stack_idx with additional params carried by evt_package.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void    cmmsc_action_force_ph_ext (
    cmmsc_state_machine_s_type        *state_machine,
    cmmsc_event_force_pref_s_type     *evt_package,
    int                                stack_idx
);

/*===========================================================================

FUNCTION cmmsc_set_stack_capability_auto

DESCRIPTION
  This function set stacks capability per AUTO requirements

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability_auto( 
    cmmsc_state_machine_s_type  *state_machine
);

/*===========================================================================

FUNCTION cmmsc_set_stack_capability_auto_emergency

DESCRIPTION
  This function set stack capability for auto in emergency call scenarios.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability_auto_emergency( 
    cmmsc_state_machine_s_type  *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_is_ue_mode_csfb

DESCRIPTION
  This function returns if the current ue mode is csfb.

DEPENDENCIES
  None.

RETURN VALUE
  sys_ue_mode_e_type.

===========================================================================*/
boolean  cmmsc_auto_is_ue_mode_csfb (
    cmmsc_state_machine_s_type  *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_retrieve_pm_ue_mode

DESCRIPTION
  This function retrieves the policy manager ue mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
===========================================================================*/
void    cmmsc_auto_retrieve_pm_ue_mode (
    cmmsc_state_machine_s_type  *state_machine
);

#if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
/*===========================================================================

FUNCTION cmmsc_auto_unblock_1x_sxlte_plmn

DESCRIPTION
  For all PLMNs in svlte plmn list, send unblocking cmd to NAS.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void cmmsc_auto_unblock_1x_sxlte_plmn(
  sys_modem_as_id_e_type              asubs_id
);
#endif

/*===========================================================================

FUNCTION cmmsc_auto_update_op_mode

DESCRIPTION
  The function updates op mode base on various conditions.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void   cmmsc_auto_update_op_mode (
    cmmsc_state_machine_s_type  *state_machine,
    boolean                      is_new_policy_tobe_read,
    boolean                      is_oprt_sub_change,
    sd_ss_mode_pref_e_type       mode_pref,
    sd_ss_hybr_pref_e_type       hybr_pref,
    boolean                      is_gwl_sub_avail,
    boolean                      is_cdma_sub_avail,
    cm_preference_info_s_type           *pref_info
);

/*===========================================================================

FUNCTION cmmsc_auto_set_ue_mode_forced

DESCRIPTION
  The function processes op mode and ue_mode change, and sets ue_mode_forced flag.
  CMMSC will check the flag before sending command to mmoc to decide if additional action
  ue mode change needs to be set.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void   cmmsc_auto_set_ue_mode_forced (
    cmmsc_state_machine_s_type         *state_machine,
    cmmsc_state_machine_op_mode_e_type  old_op_mode,
    cmmsc_state_machine_op_mode_e_type  new_op_mode,
    sys_ue_mode_e_type                  old_ue_mode,
    sys_ue_mode_e_type                  new_ue_mode
);

/*===========================================================================

FUNCTION  cmmsc_auto_is_valid_stack_idx

DESCRIPTION
 Return if the stack_idx is valid per the state machine.

DEPENDENCIES
 None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_valid_stack_idx (
  cmmsc_state_machine_s_type  *state_machine,
  int                          stack_idx
);

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr_gwl_allowed

DESCRIPTION
  This function returns whether hybrid HDR is allowed or not based on the
  hybrid_pref passed.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean    cmmsc_auto_is_hybr_gwl_allowed
(
   cmmsc_state_machine_s_type     *state_machine,
   /* State machine ptr.
   */
   
   sd_ss_hybr_pref_e_type          hybr_pref,
   /* Hybrid preference to use.
   */

   boolean                         is_cdma_subsc_avail
   /* The  current value of CDMA subscription
   */

);

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr_allowed

DESCRIPTION
  This function returns whether hybrid is allowed or not based on the
  mode_pref and hybrid_pref passed.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean    cmmsc_auto_is_hybr_allowed
(

       sd_ss_mode_pref_e_type          mode_pref,
          /* Mode preference to use.
          */

       sd_ss_hybr_pref_e_type          hybr_pref,
          /* Hybrid preference to use.
          */

       sys_modem_as_id_e_type          asubs_id
);

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr_hdr_call_allowed

DESCRIPTION
  This function should be called to determine if hybrid HDR call is allowed.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean    cmmsc_auto_is_hybr_hdr_call_allowed(
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION cmmsc_auto_set_hybr1_pref

DESCRIPTION
  The function sets the hybr1 specific pref to be forced to SD.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_set_hybr1_pref (
  cmmsc_state_machine_s_type           *state_machine,
  cmmsc_state_machine_op_mode_e_type   old_op_mode,  
  cm_preference_info_s_type            *pref
);

/*===========================================================================

FUNCTION cmmsc_auto_online_disable_hybr1

DESCRIPTION
  The function returns whether or not to disable hybr1 depending on oprt mode,
  mode pref, and hybr pref.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is to be disabled.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
 boolean    cmmsc_auto_online_disable_hybr1 (
    sys_oprt_mode_e_type         oprt_mode,
    cm_mode_pref_e_type          mode_pref,
    sd_ss_hybr_pref_e_type       hybr_pref
);

/*===========================================================================

FUNCTION cmmsc_auto_is_op_mode_hybr1_enabled

DESCRIPTION
  The function returns if hybr1 is enabled for the specified op mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is enabled for the specified op mode.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_op_mode_hybr1_enabled (
    cmmsc_state_machine_op_mode_e_type  op_mode
);

/*===========================================================================

FUNCTION cmmsc_auto_update_is_hybr1_disabled

DESCRIPTION
  The function updates is_hybr1_disabled in MSC state machine.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_hybr1_disabled (
  cmmsc_state_machine_s_type  *state_machine,
  boolean                     is_hybr1_disabled
);

/*===========================================================================

FUNCTION cmmsc_auto_get_is_hybr1_disabled

DESCRIPTION
  The function returns is_hybr1_disabled in MSC state machine.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is enabled for the specified op mode.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_get_is_hybr1_disabled (
  cmmsc_state_machine_s_type  *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr1_switched_off

DESCRIPTION
  The function returns if hybr1 stack is switched off.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is switched off.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_hybr1_switched_off (
  cmmsc_state_machine_op_mode_e_type  old_op_mode,
  cmmsc_state_machine_op_mode_e_type  new_op_mode
);

/*===========================================================================

FUNCTION cmmsc_auto_is_hybr1_switched_on

DESCRIPTION
  The function returns if hybr1 stack is switched on.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybr1 is switched on.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_hybr1_switched_on (
  cmmsc_state_machine_op_mode_e_type  old_op_mode,
  cmmsc_state_machine_op_mode_e_type  new_op_mode
);

/*===========================================================================

FUNCTION cmmsc_auto_update_is_kick_hybr2_pending

DESCRIPTION
  The function updates is_kick_hybr2_pending.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_kick_hybr2_pending (
  cmmsc_state_machine_s_type     *state_machine,
  boolean                         is_pending
);

/*===========================================================================

FUNCTION cmmsc_auto_update_is_kick_hdr_pending

DESCRIPTION
  The function updates is_kick_hdr_pending.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_kick_hdr_pending (
  cmmsc_state_machine_s_type     *state_machine,
  boolean                         is_kick_hdr_pending
);

/*===========================================================================

FUNCTION cmmsc_auto_update_is_stack_sync_bsr_pending

DESCRIPTION
  The function updates is_stack_sync_bsr_pending.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_stack_sync_bsr_pending (
  cmmsc_state_machine_s_type     *state_machine,
  boolean                         is_stack_sync_bsr_pending
);

/*===========================================================================

FUNCTION cmmsc_auto_update_is_kick_gw

DESCRIPTION
  The function updates is_kick_gw.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void  cmmsc_auto_update_is_kick_gw (
  cmmsc_state_machine_s_type     *state_machine,
  boolean                         is_kick_gw
);

/*===========================================================================

FUNCTION cmmsc_auto_is_kick_hdr_pending

DESCRIPTION
  The function returns is_kick_hdr_pending.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_kick_hdr_pending (
  cmmsc_state_machine_s_type     *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_is_kick_hybr2_pending

DESCRIPTION
  The function returns is_hybr_2_kick_pending.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_kick_hybr2_pending (
  cmmsc_state_machine_s_type     *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_is_is_stack_sync_bsr_pending

DESCRIPTION
  The function returns is_stack_sync_bsr_pending.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_stack_sync_bsr_pending (
  cmmsc_state_machine_s_type     *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_is_kick_gw

DESCRIPTION
  The function returns is_kick_gw.

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmmsc_auto_is_kick_gw (
  cmmsc_state_machine_s_type     *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_get_lost_main_uptime

DESCRIPTION
  This function gets the current acq_hybr_1_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  dword.

SIDE EFFECTS
  None.
===========================================================================*/
dword    cmmsc_auto_get_acq_hybr_1_uptime (
  cmmsc_state_machine_s_type     *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_get_lost_main_uptime

DESCRIPTION
  This function gets the current lost_main_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  dword.

SIDE EFFECTS
  None.
===========================================================================*/
dword    cmmsc_auto_get_lost_main_uptime (
  cmmsc_state_machine_s_type     *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_get_avoid_hybr_hdr_uptime

DESCRIPTION
  This function gets the current avoid_hybr_hdr_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  dword.

SIDE EFFECTS
  None.
===========================================================================*/
dword    cmmsc_auto_get_avoid_hybr_hdr_uptime (
  cmmsc_state_machine_s_type     *state_machine
);

/*===========================================================================

FUNCTION cmmsc_auto_set_acq_hybr_1_uptime

DESCRIPTION
  This function sets the current acq_hybr_1_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_set_acq_hybr_1_uptime (  
  cmmsc_state_machine_s_type     *state_machine,
  dword                           uptime
);

/*===========================================================================

FUNCTION cmmsc_auto_set_lost_main_uptime

DESCRIPTION
  This function sets the current lost_main_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_set_lost_main_uptime (  
  cmmsc_state_machine_s_type     *state_machine,
  dword                           uptime
);

/*===========================================================================

FUNCTION cmmsc_auto_set_avoid_hybr_hdr_uptime

DESCRIPTION
  This function sets the current avoid_hybr_hdr_uptime.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_auto_set_avoid_hybr_hdr_uptime (  
  cmmsc_state_machine_s_type     *state_machine,
  dword                           uptime
);

/*===========================================================================

FUNCTION cmmsc_auto_mapping_ss_to_stack_idx

DESCRIPTION
  The function maps cm ss to stack index.

DEPENDENCIES
  None

RETURN VALUE
  stack index

SIDE EFFECTS
  None

===========================================================================*/
int    cmmsc_auto_mapping_ss_to_stack_idx(
    cm_ss_e_type ss
);

/*===========================================================================

FUNCTION cmmsc_auto_proc_acq_fail_rpt

DESCRIPTION
  This function process acq failure rpt from SD. It provides following functions as result of acq 
  fail on a given stack/sub
    - add/remove mode capability for the stack on which acq failed
    - add/remove mode capability on peer stacks within same sub and kick


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void cmmsc_auto_proc_acq_fail_rpt(

  cmmsc_state_machine_s_type *msc,

  cm_ss_e_type ss,

  sd_ss_mode_pref_e_type   mode_pref

);

/*===========================================================================

FUNCTION cmmsc_auto_proc_srv_ind_rpt

DESCRIPTION
  This function process srv ind rpt from SD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void cmmsc_auto_proc_srv_ind_rpt
(
  cmmsc_state_machine_s_type *msc,
  
  const cm_sd_rpt_u_type *rpt_ptr,

  sys_srv_status_e_type prev_srv_status_ss
);

void cmmsc_auto_proc_3gpp_stopped_rpt(

  cmmsc_state_machine_s_type *msc,

  cm_ss_e_type ss
);


void cmmsc_action_kick_hybr2(
    cmmsc_state_machine_s_type *msc,
    int                        stack_idx
);

void cmmsc_action_hicps_remove_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_event_acq_fail_s_type *acq_fail
);

void cmmsc_action_hicps_cond_kick(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
);

void cmmsc_action_pending_kick(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    boolean                     force_top
);

void cmmsc_action_shorten_bsr(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
);

void cmmsc_action_restore_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
);

void cmmsc_action_svlte_remove_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
);

void cmmsc_action_svlte_stop_stack(
    cmmsc_state_machine_s_type  *msc,
    int                          stack_idx
);

void cmmsc_action_svlte_cond_kick(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_event_acq_fail_s_type *acq_fail_evt,
    boolean                    is_oprt_or_sub_chgd
);

void cmmsc_action_defer_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx
);

void cmmsc_action_enable_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
);

void cmmsc_action_disable_3gpp(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
);

void cmmsc_action_sync_up(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
);

void cmmsc_action_update_srv(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
);

void cmmsc_action_update_no_srv(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_srv_ind_s_type *srv
);

void cmmsc_auto_proc_peer_subs_call(
  cmmsc_state_machine_s_type *msc,
  const cmmsc_param_call_ind_s_type* call_event
);

void cmmsc_action_resume_srv_acq(
    cmmsc_state_machine_s_type *msc,
    int                         stack_idx,
    cmmsc_param_call_ind_s_type *call_evt
);

boolean  cmmsc_auto_is_hybr2_on (
    cmmsc_state_machine_op_mode_e_type op_mode
);

/*===========================================================================

FUNCTION cmmsc_auto_update_lte_data_ss_srlte

DESCRIPTION
 It will update call_ptr->ss for LTE data calls, from MAIn to HYBR_2 when switch to dual stack happens in SRLTE.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmmsc_auto_update_lte_data_ss_srlte(
cmmsc_state_machine_op_mode_e_type  prev_op_mode, 
cmmsc_state_machine_op_mode_e_type  new_op_mode
);

#endif
