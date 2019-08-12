#ifndef CMMSC_SGLTE_H
#define CMMSC_SGLTE_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
             C M    M U L T I - S T A C K    C O N T R O L L E R

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file implements the CM multi-stack architecture to support multi-stacks
  requirements such as SVLTE, DualSim, SGLTE, etc.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmsc_sglte.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/13/13   cl      CM supports explicit PS_DETACH when capability from PM changed
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
#include "cmss.h"     /* Interface to CM Serving system object. */
#include "cmdbg.h"    /* Interface to CM debug macros.  */

/*===========================================================================

            LOCAL DEFINITIONS OF CONSTANTS AND ENUMS FOR MODULE

This section contains local definitions for constants and enums used in
this module and

===========================================================================*/

#define CMMSC_SGLTE_STACK_NUM   2         /* Max num of stacks, ie. 2 for SGLTE  */

/*===========================================================================

                      LOCAL DEFINITIONS OF DATA STRUCTURE

This section contains local definitions for constants and enums used in
this module and

===========================================================================*/


/* SGLTE ROUTING TABLE DATA STRUCTURE

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

typedef struct cmmsc_routing_table_sglte_entry_s
{
  /* event */
  cmmsc_event_e_type                 event;

  /* current state-machine operation mode */
  cmmsc_state_machine_op_mode_e_type mode;

  /* current states of each stack */
  cmmsc_stack_state_e_type           state[CMMSC_SGLTE_STACK_NUM];

  /* action index */
  cmmsc_action_s_type                action[CMMSC_SGLTE_STACK_NUM];

  /* New states */
  cmmsc_stack_state_e_type           new_state[CMMSC_SGLTE_STACK_NUM];

}cmmsc_routing_table_sglte_entry_s_type;


/*===========================================================================
                      LOCAL DEFINITIONS OF FUNCTIONS

            This section contains local functions used in CM
===========================================================================*/



/*===========================================================================

FUNCTION cmmsc_get_sglte_routes

DESCRIPTION
  This function return the pointer to sglte_routes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
cmmsc_routing_table_sglte_entry_s_type *cmmsc_get_sglte_routes( void );


/*===========================================================================

FUNCTION cmmsc_get_sglte_routes_size

DESCRIPTION
  This function return the size of the sglte_routes

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int cmmsc_get_sglte_routes_size( void );


/*===========================================================================

FUNCTION cmmsc_sglte_routing_table_init

DESCRIPTION
  This function initializes SGLTE Routing Table

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_sglte_routing_table_init( cmmsc_routing_table_s_type *sglte_route_table );


/*===========================================================================

FUNCTION cmmsc_set_stack_capability_sglte

DESCRIPTION
  This function set stacks capability per SGLTE requirements

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability_sglte( cmmsc_state_machine_s_type *state_machine );

/*===========================================================================

FUNCTION cmmsc_set_stack_capability_sglte_emergency

DESCRIPTION
  This function set stacks capability during emergency call per SGLTE
  requirements

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_set_stack_capability_sglte_emergency(
   cmmsc_state_machine_s_type *state_machine  
);

/*===========================================================================

FUNCTION cmmsc_sglte_retrieve_op_mode

DESCRIPTION
  This function gets the current value of cmmsc op mode.

DEPENDENCIES
  None.

RETURN VALUE
  NONE
===========================================================================*/
cmmsc_state_machine_op_mode_e_type cmmsc_sglte_retrieve_op_mode(
   cmmsc_state_machine_s_type * state_machine
);

/*===========================================================================

FUNCTION cmmsc_sglte_update_op_mode

DESCRIPTION
  This function updates the op mode upon subscription change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsc_sglte_update_op_mode ( 
  cmmsc_state_machine_s_type  *state_machine,
  boolean                     is_gwl_sub_avail
);

/*===========================================================================

FUNCTION cmmsc_special_mode_update_for_main_stack

DESCRIPTION
  This function updates main stack with special handling in case MAIN has
  GSM only. This is SGLTE specific requirements.

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_special_mode_update_for_main_stack (
  cmmsc_state_machine_s_type  *state_machine,
  int                     stack_idx,
  sd_ss_mode_pref_e_type  *mode
);


/*===========================================================================

FUNCTION cmmsc_sglte_get_ss_for_domain

DESCRIPTION
  This function return the stack per domain input based on SGLTE requirement.
  This is SGLTE specific.

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
cm_ss_e_type cmmsc_sglte_get_ss_for_domain (
    sys_modem_as_id_e_type       as_id,
    sd_ss_srv_domain_pref_e_type domain
);

/* SGLTE specific functions */
#if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)
/*===========================================================================

FUNCTION cmmsc_proc_stack_deact_ind

DESCRIPTION
  This function should be called when NAS wants to deactivate a stack.
  In SC SGLTE, it could be due to L->G Reselection or PS Domain Transfer

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_proc_stack_deact_ind( cm_ss_e_type ss );


/*===========================================================================

FUNCTION cmmsc_proc_stack_act_ind

DESCRIPTION
  This function should be called when CM receives stack activation indication from NAS.
  In SC SGLTE, it could be due to G->L Reselection or PS Domain Transfer

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_proc_stack_act_ind( cm_ss_e_type ss );
#endif /* defined CM_GW_SUPPORTED ||defined(FEATURE_CM_LTE) || defined(FEATURE_TDSCDMA)*/


/*===========================================================================

FUNCTION cmmsc_action_local_act

DESCRIPTION
  The function will do the following:
  1. Activate the specified stack in CM.
  2. send local activate indication to mmoc.

DEPENDENCIES
  Stacks must have already been initialized with cmmsc_init_multi_stacks().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_action_local_act(
   cmmsc_state_machine_s_type *state_machine,
   int stack_idx);

/*===========================================================================

FUNCTION cmmsc_action_local_deact

DESCRIPTION
  The function will do the following:
  1. Deactivate stack locally
  2. Transfer data

DEPENDENCIES
  Stacks must have already been initialized with cmmsc_init_multi_stacks().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_action_local_deact(
   cmmsc_state_machine_s_type *state_machine,
   int stack_idx);

/*===========================================================================

FUNCTION cmmsc_action_g_ps_domain_transfer

DESCRIPTION
  The function will do the following:
  1. force the top preference onto SD with reason RESTORE after
     service has been transfered to Main or Hybrd

DEPENDENCIES
  Stacks must have already been initialized with cmmsc_init_multi_stacks().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmmsc_action_g_ps_domain_transfer(
   cmmsc_state_machine_s_type *state_machine,
   int stack_idx);


/*===========================================================================

FUNCTION cmmsc_is_ps_detach_required

DESCRIPTION
  Stack previous preference that were used to attached to network
  If the preference is changed, CM will triggers PS_DETACH.
  For SGLTE, during FullRAT Mode, we're not expect to do any registration and
  hence for FullRAT Mode (CAMP_ONLY), this prev_registration_capability will not
  be saved.

  Implemented per NAS team requirement:
     Expectation of Local/OTA detach or no detach in SGLTE context.
  1. SGLTE -> FULL RAT (in OOS) -> SGLTE ( NO Detach as start ue_mode and end ue_mode are same)
  2. SGLTE -> FULL RAT (in OOS) -> CSFB ( Local Detach)
  3. CSFB -> FULL RAT (in OOS) -> CSFB ( No detach)
  4. CSFB -> FULL RAT (in OOS) -> SGLTE (Local detach)
  5. CSFB -> SGLTE (OTA detach)
  6. SGLTE -> CSFB (OTA detach)

DEPENDENCIES
  None.

RETURN VALUE
  Boolean TRUE: ps_detach required
          FALSE: ps_detach not required

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_evaluate_ps_detach_required_sglte(
   cmmsc_state_machine_s_type *state_machine,
   int stack_idx,cm_mode_band_pref_s_type old_stack_pref);

/*===========================================================================

FUNCTION cmmsc_sglte_mapping_ss_to_stack_idx

DESCRIPTION
  The function maps cm ss to stack index.

DEPENDENCIES
  None

RETURN VALUE
 int

SIDE EFFECTS
  None

===========================================================================*/
int cmmsc_sglte_mapping_ss_to_stack_idx(cm_ss_e_type ss);

/*===========================================================================

FUNCTION cmmsc_sglte_mapping_stack_id_to_ss

DESCRIPTION
  The function maps stack index to cm ss.

DEPENDENCIES
  None

RETURN VALUE
  cm_ss_e_type

SIDE EFFECTS
  None

===========================================================================*/
cm_ss_e_type  cmmsc_sglte_mapping_stack_id_to_ss(int stack_idx);

/*===========================================================================

FUNCTION cmmsc_sglte_mapping_stack_id_to_sd_ss

DESCRIPTION
  The function maps stack index to sd ss.

DEPENDENCIES
  None

RETURN VALUE
  stack index

SIDE EFFECTS
  None

===========================================================================*/
sd_ss_e_type cmmsc_sglte_mapping_stack_id_to_sd_ss(int stack_idx);

#endif
