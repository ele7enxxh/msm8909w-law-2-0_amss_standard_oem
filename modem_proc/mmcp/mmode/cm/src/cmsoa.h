#ifndef CMSOA_H
#define CMSOA_H
/*===========================================================================

  C A L L   M A N A G E R  DYNAMIC OPERATION SWITCH  H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMSOA.C

Copyright (c) 1998 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmsoa.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/14   xs      hVOLTE SSAC support
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
10/10/12   cl      Initial Release

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "sys.h"       /* System wide definitions */
#include "sys_v.h"
#include "sd.h"
#include "cmd.h"
#include "queue.h"
#include "cmll.h"
#include "cmll_v.h"


#if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)) || defined(FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH)

/*--------------------------------------------------------------------------
** DEFINITIONS
** - Section 1: defines / constants
** - Section 2: enumerations
** - Section 3: typedef
**--------------------------------------------------------------------------
*/


/***************************************************************************
** DEFINITIONS - SECTION 1: defines / constants
** This section contains all the #define and constants
***************************************************************************/

/* timer for waiting for voice call end for both dynamic switch and srlte - non configurable */
#define CMSOA_SWITCH_DELAY_REASON_VOICE_CALL_TIMER 10

/* pingpong timer value for dynamic switch - non configurable */
#define CMSOA_SWITCH_DELAY_REASON_PINGPONG_TIMER 20

/* default value for srlte Treturntocsfb and Twaitrrcidle - configurable */
#define CMSOA_SRLTE_SWITCH_DELAY_REASON_RRC_IDLE_DEFAULT_TIMER 60
#define CMSOA_SRLTE_SWITCH_DELAY_REASON_PINGPONG_TIMER 600

/* ATTACH COMPLETE timer value in ms */
#define CMSOA_ATTACH_COMPLETE_DEFAULT_TIMER 600
#define CMSOA_SVLTE_MCC_LIST_MAX 20


/***************************************************************************
** DEFINITIONS - SECTION 2: enumerations
** This section contains all the enumerations
***************************************************************************/

typedef enum cmsoa_sv_oprt_mode_e
{
  CMSOA_SV_OPRT_MODE_NONE = -1,

  CMSOA_SV_OPRT_MODE_SVLTE,

  CMSOA_SV_OPRT_MODE_CSFB
}cmsoa_sv_oprt_mode_e_type;

/* Enum of the action which UE needs to take after delay timer expires.
*/
typedef enum cmsoa_sv_switch_action_e
{
  /* FOR INTERNAL USE ONLY!                 */
  CMSOA_SV_SWITCH_ACTION_NONE = -1,

  /* Action switching to SVLTE */
  CMSOA_SV_SWITCH_ACTION_SWITCH_SVLTE = 1,

  /* Action switching to CSFB */
  CMSOA_SV_SWITCH_ACTION_SWITCH_CSFB  = 2,

  /* FOR INTERNAL USE ONLY! */
  CMSOA_SV_SWITCH_ACTION_MAX
}cmsoa_sv_switch_action_e_type;


/* Enum of the action which UE needs to take after delay timer expires.
*/
typedef enum cmsoa_sv_switch_delay_reas_e
{
  /* FOR INTERNAL USE ONLY!                 */
  CMSOA_SV_SWITCH_DELAY_REAS_NONE = -1,

  /* We're delaying to due to voice call  */
  CMSOA_SV_SWITCH_DELAY_REAS_VOICE_CALL = 1,

  /* We're delaying to avoid ping-pong */
  CMSOA_SV_SWITCH_DELAY_REAS_PINGPONG  = 2,

  /* We're delaying to when LTE in connected state*/
  CMSOA_SV_SWITCH_DELAY_LTE_CONNECTED  = 3,

  /* We're delaying to when LTE in connected state*/
  CMSOA_SV_SWITCH_DELAY_ATTACH_COMPLETE  = 4,

  /* FOR INTERNAL USE ONLY! */
  CMSOA_SV_SWITCH_DELAY_REAS_MAX

}cmsoa_sv_switch_delay_reas_e_type;

/***************************************************************************
** DEFINITIONS - SECTION 2: typedef
** This section contains all the typedef
***************************************************************************/
typedef struct cmsoa_sxlte_timers_s
{
 uint16 switch_pingpong_guard_timer;
 uint16 lte_idle_timer;
}sxlte_timers_s_type;


typedef struct cmsoa_config_s
{
  /* This item controls if the feature is enabled */
  boolean                    is_switch_enabled;

  /* Store current SV & non-SV state */
  cmsoa_sv_oprt_mode_e_type  curr_sv_oprt_mode;

  /* Store VoLTE-2012 configuration.
  ** a. VoLTE2012 = TRUE, the UE will enter single-domain camping
  **       if the LTE network supports VoLTE
  ** b. VoLTE2012 = FALSE, the UE will not enter single-domain camping
  **       even if the LTE network supports VoLTE
  **
  ** The reason is that VOLTE-2012 only supports voice between VOLTE to
  ** another VOLTE devices.
  */
  boolean                    is_volte12;


  /* Store the congifurable list of SVLTE MCC */
  uint32                     svlte_mcc_list[CMSOA_SVLTE_MCC_LIST_MAX];

}cmsoa_config_s_type;


typedef struct cmsoa_state_info_s
{
  /* Store current SV & non-SV state */
  cmsoa_sv_oprt_mode_e_type       curr_sv_oprt_mode;

  /* Store the plmn-id list read from EFS indicates which plmn supports SVLTE */
  q_type                          svlte_plmn_list;

  /* Store the configuration */
  cmsoa_config_s_type             config;

  /* This indicate if the feature should be operated.*/
  boolean                         is_activated;

  /* This variables store the last gwl plmn_id which we found */
  sys_plmn_id_s_type              last_gwl_plmn;

  /* This stores the pref_update_reason when performing SV-Switch() */
  sd_ss_pref_update_reason_e_type pref_update_reason;

  /* This stores the timer when we are decide to delay the switch action
  ** Cases which we want to delay happens such as:
  ** 1) during voice call
  ** 2) ping-pong scenarios
  */
  dword                           switch_delay_timer;

  /* This stores the future action state which we decided to deferred to future time */
  cmsoa_sv_oprt_mode_e_type   switch_delay_state;

  /* This stores the delay reason for the switch */
  cmsoa_sv_switch_delay_reas_e_type  switch_delay_reason;

  /* This stores the indication that we found MCC country, but
  ** prediction turns out wrong and we have switched to SVLTE. This flag
  ** should only be cleared when we received an ACQ_FAIL on main.
  */
  boolean                         is_svlte_predicted;

  sxlte_timers_s_type  sxlte_timers;

  /* Indicates if UE should move to SRLTE mode of operation when camped
  ** on a C2K home system ( UE mode is CSFB preferred)
  ** This will allow background BSR to LTE vs foreground ( if UE remained in CSFB mode)
  ** Default value is TRUE
  */
  boolean                         is_c2k_modeswitch2srlte;

  /* This stores the optimization flag for SRLTE to decide
  ** whether to power up from last sub mode
  */
  boolean                         is_pwrup_from_last_submode;

  /* Inidicates initial sub mode at power up */
  cmsoa_sv_oprt_mode_e_type       init_sub_mode;

  /* indicates whether ATTACH complete timer is running or not */
  boolean                         is_attach_complete_timer_active;

  unsigned long                   attach_complete_timer;

}cmsoa_state_info_s_type;


/* Type to hold all CM commands.
*/
typedef struct cmsoa_svlte_plmn_id_node_s {

  q_link_type           link;
      /* Q linking element */

  sys_plmn_id_s_type    plmn;
      /* plmn_info of the node */

}cmsoa_svlte_plmn_id_node_s_type;

/*--------------------------------------------------------------------------
** EXTERN FUNCTIONS
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
extern cmsoa_state_info_s_type *cmsoa_state_info_ptr( void );

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
void cmsoa_init( boolean is_pwr_on );

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
boolean cmsoa_read_plmn_id_efs(void);

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
void cmsoa_msg_proc(const cm_generic_rpt_s_type *msg_ptr);


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
void cmsoa_mcc_msg_process( cmsoa_mcc_s_type *mcc_msg);

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
void cmsoa_process_domain_selection_srv_ind(

   const sd_si_info_s_type   *si_info_ptr,
  /* Pointer to buffer containing new service info information */

   cm_ss_e_type               cm_ss
);


/*===========================================================================

FUNCTION cmsoa_get_current_sv_oprt_state()

DESCRIPTION
  This function return current sv_oprt state variable.

RETURN VALUE
  cmsoa_sv_oprt_mode_e_type

===========================================================================*/
cmsoa_sv_oprt_mode_e_type cmsoa_get_current_sv_oprt_state( void );

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
);

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
extern void cmsoa_regprx_srv_req_proceed_request_msg( cm_ss_e_type ss );


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
void cmsoa_regprx_srv_req_proceed_process( cm_generic_rpt_u_type *regprx_msg);

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

void cmsoa_attach_complete_rpt_proc(boolean is_timer_exp);


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
void cmsoa_write_plmn_id_efs_default( void );

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
boolean cmsoa_read_soa_config_efs(void);

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
boolean cmsoa_write_soa_config_efs(cmsoa_config_s_type *soa_config);


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
void cmsoa_timer_proc(void);

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
void cmsoa_process_delay_action( cmsoa_sv_oprt_mode_e_type   state );


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
                            cmsoa_sv_switch_delay_reas_e_type  switch_delay_reason);


/*===========================================================================

FUNCTION cmsoa_get_pref_update_reason()

DESCRIPTION
  This function returns pref_update_reason variable

RETURN VALUE
  sd_ss_pref_update_reason_e_type

===========================================================================*/
sd_ss_pref_update_reason_e_type cmsoa_get_pref_update_reason( void );


/*===========================================================================

FUNCTION cmsoa_set_pref_update_reason()

DESCRIPTION
  This function set pref_update_reason variable

RETURN VALUE
  none

===========================================================================*/
void cmsoa_set_pref_update_reason ( sd_ss_pref_update_reason_e_type  reason );

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
boolean cmsoa_is_last_plmn_in_svlte_plmn_list( void );

/*===========================================================================

FUNCTION cmsoa_activate_arbitration()

DESCRIPTION
  This function checks and activate the SV-Operation Arbitration module.

RETURN VALUE
  none

===========================================================================*/
void cmsoa_activate_arbitration( void );


/*===========================================================================

FUNCTION cmsoa_is_arbitration_activate()

DESCRIPTION
  This function returns if arbitration is activated.

RETURN VALUE
  none

===========================================================================*/
boolean cmsoa_is_arbitration_activate( void );


/*===========================================================================

FUNCTION cmsoa_proc_acq_fail()

DESCRIPTION
  This function process acq_fail messages.

RETURN VALUE
  none

===========================================================================*/
void cmsoa_proc_acq_fail(sd_ss_e_type ss);

boolean cmsoa_sv_switch_base(
  cmsoa_sv_oprt_mode_e_type  new_state,
  cm_ss_e_type               cm_ss,
  boolean                    switch_on_ims_reg
);

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
);


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

extern void cmsoa_clear_active_attach_complete_timer();

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
boolean cmsoa_check_and_switch_to_srlte(cm_ss_e_type ss, boolean is_switch_needed);


void cmsoa_srlte_ue_mode_update(sys_ue_mode_e_type ue_mode);

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
boolean cmsoa_skip_dom_sel_due_to_pending_ue_mode(void);

/*===========================================================================

FUNCTION cmsoa_skip_dom_sel_due_to_soa()

DESCRIPTION
  This function tells if Domain selection logis of disablign LTE
  should be skipped, as SOA can take care of it.
  1. If in SV mode, Hybr-2 is on
  2. if in Non-SV mode, but PLMN belongs to SV-PLMN list

RETURN VALUE
  none

===========================================================================*/
boolean cmsoa_skip_dom_sel_due_to_soa(void);

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
boolean cmsoa_skip_dom_sel_due_to_pending_ue_mode(void);


/*===========================================================================

FUNCTION cmsoa_sd_rpt_proc

DESCRIPTION
  Process System Determination reports


DEPENDENCIES
  CMSOA object must have already been initialized with
  cmsoa_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmsoa_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD reports */
);

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
      ** and e)1x MCC is part of Home PLMN list
      

DEPENDENCIES  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmsoa_check_and_switch_fromc2k_to_srlte(cm_ss_e_type ss);

#endif /* FEATURE_CM_LTE */


#endif /* CMSOA_H */
