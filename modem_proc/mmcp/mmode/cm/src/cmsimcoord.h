#ifndef CMSIMCOORD_H
#define CMSIMCOORD_H
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmsimcoord.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/13/13   cl      CM supports explicit PS_DETACH when capability from PM changed
02/25/13   cl      Initial release.

===========================================================================*/

#include "mmcp_variation.h"
#include "target.h"
#include "customer.h"
#include "sd.h"
#include "cmll.h"
#include "mmoc.h"
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

//This is the correlation table size
#define CMMSIMC_SIM_MAX   3

#ifdef FEATURE_MMODE_TRIPLE_SIM
#define CMMSIMC_SIM_NUM   3
#elif defined FEATURE_MMODE_DUAL_SIM
#define CMMSIMC_SIM_NUM   2
#else
#define CMMSIMC_SIM_NUM   1
#endif


#define CMMSIMC_CORRELATION_NONE -1

typedef enum
{
  CMMSIMC_COORDINATION_NONE = -1,
  CMMSIMC_COORDINATION_SINGLE_SIM,
  CMMSIMC_COORDINATION_XSIM_SINGLE_SIM1,
  CMMSIMC_COORDINATION_XSIM_SINGLE_SIM2,
  CMMSIMC_COORDINATION_XSIM_SINGLE_SIM3,
  CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM2,
  CMMSIMC_COORDINATION_XSIM_DUAL_SIM1_SIM3,
  CMMSIMC_COORDINATION_XSIM_DUAL_SIM2_SIM3,
  CMMSIMC_COORDINATION_XSIM_TRIPLE_STANDBY,
  /*----------------- CM Internal Use --------------------*/
  CMMSIMC_COORDINATION_ANY,
  CMMSIMC_COORDINATION_MAX

}cmmsimc_coordination_e_type;


typedef enum
{
  CMMSIMC_SIM_STATE_NONE = -1,
  CMMSIMC_SIM_STATE_NULL,
  CMMSIMC_SIM_STATE_INACTIVE,
  CMMSIMC_SIM_STATE_ACTIVE,

  /* internal correlation table use */
  CMMSIMC_SIM_STATE_NO_CHG,
  CMMSIMC_SIM_STATE_ANY,
  CMMSIMC_SIM_STATE_MAX
}cmmsimc_sim_state_e_type;


typedef enum
{
  CMMSIMC_SIM_ACT_NONE = -1,
  CMMSIMC_SIM_ACT_ROUTE_REQUEST,
  CMMSIMC_SIM_ACT_POWER_DOWN,
  CMMSIMC_SIM_ACT_WAKE_UP,
  CMMSIMC_ACT_SUBS_CHANGE,
  CMMSIMC_SIM_ACT_ON,
  CMMSIMC_SIM_ACT_OFF,
  CMMSIMC_SIM_ACT_PEER_SUBS_CALL,
  CMMSIMC_SIM_ACT_MAX
}cmmsimc_action_e_type;

/*===========================================================================

                      LOCAL DEFINITIONS OF DATA STRUCTURE

This section contains local definitions for constants and enums used in
this module and

===========================================================================*/

/* action type for each sim */
typedef struct cmmsimc_action_s
{
  /* index of the sim to take action on */
  int sim_idx;
  /* the action enum, mapped to actual action */
  cmmsimc_action_e_type action;

}cmmsimc_action_s_type;


typedef struct cmmsimc_capability_s
{
  /* The restricted mode of SIM capabilities */
  sd_ss_mode_pref_e_type mode;

}cmmsimc_capability_s_type;

/* Information hold in each SIM */
typedef struct cmmsimc_sim_s
{
  /* current sim state (active/inactive) */
  cmmsimc_sim_state_e_type     state;

  /* Capability of SIM */
  cmmsimc_capability_s_type    capability;

  /* Pointer to the multi-stack controller state machines */
  cmmsc_state_machine_s_type  *msc_machine;

}cmmsimc_sim_s_type;


/* This is the structure of one single entry in the correlation table */
typedef struct cmmsimc_correlation_table_entry_s
{
  /* event */
  cmmsimc_event_e_type                 event;

  /* current state-machine operation mode */
  cmmsimc_coordination_e_type          coordination;

  /* current states of each stack */
  cmmsimc_sim_state_e_type             state[CMMSIMC_SIM_MAX];

  /* action index */
  cmmsimc_action_s_type                action[CMMSIMC_SIM_MAX];

  /* New states */
  cmmsimc_sim_state_e_type             new_state[CMMSIMC_SIM_MAX];

}cmmsimc_correlation_table_entry_s_type;

/* Enum to track ESR to NAS. So that subsequent TAU/RSP are tracked */
typedef enum
{
  CMSIMC_ESR_NONE,

  CMSIMC_ESR_RSP_PENDING,
    /* ESR is sent to NAS, MM_CM_1XCSFB_CALL_RSP is awaited */

  CMSIMC_ESR_TAU_PENDING,
    /* MM_CM_1XCSFB_CALL_RSP is received. MMCM_DUAL_RX_1XCSFB_TAU_REQ is due
       after call/traffic activity is complete */

  CMSIMC_ESR_MAX
  
}cmsimc_esr_status_e_type;

/* SIM Coordinator State : contains multiple sim state machine */
typedef struct cmmsimc_state_s
{
  /* Hold multiple sim states */
  cmmsimc_sim_s_type                   sim[CMMSIMC_SIM_NUM];

  /* current state-machine operation mode */
  cmmsimc_coordination_e_type          coordination;

  /************************************************************************/
  /*********** Fields that are specific to DS - DS feature ****************/
  /************************************************************************/

  /* Effective standby preference, ph pref + # of subsc */
  sys_modem_dual_standby_pref_e_type   standby_pref;

  /* Remember standby pref change, should be used in subsequent subs update
     to SIMC */
  boolean                              standby_pref_chg;

  /* Active subs (in single-standby, there's only 1 active-sub) */
  uint8                                active_subs;

  sys_modem_device_mode_e_type         device_mode;

  /************************************************************************/
  /*********** Fields that are specific to DS - DS feature ****************/
  /************************************************************************/


  /* Remember this state, so that after call activity is done we need to
     perform TAU so that lower layers can resume LTE service */
  cmsimc_esr_status_e_type             esr_state;

}cmmsimc_state_s_type;


/*===========================================================================
                      LOCAL DEFINITIONS OF FUNCTIONS

            This section contains local functions used in CM
===========================================================================*/

void cmmsimc_state_init( void );

void cmsimcoord_reinit(void);

cmmsimc_state_s_type *cmmsimc_state_ptr( void );

void cmmsimc_execute_action(
     int                        correlation_tbl_idx,
     cmmsc_param_base_s_type *event_package
);

void cmmsimc_update_state( int corr_table_idx );

cmmsc_state_machine_s_type     *cmmsimc_state_machine_ptr(
  sys_modem_as_id_e_type        as_id
);

cmmsc_state_machine_s_type     *cmmsimc_state_machine_ptr_per_sim_idx(
  size_t                        sim_idx
);

void cmmsimc_proc_cmd_oprt_mode_chgd
(
  sys_oprt_mode_e_type                  oprt_mode
  /* New operating mode of the phone.*/
);

void cmmsimc_proc_cmd_pref_sys_chgd
(
  cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd
);

/*===========================================================================

FUNCTION cmmsimc_proc_cmd_subsc_chgd

DESCRIPTION
  This function should be called when the subscrption availability has
  changed.This function will pack the info and in turn call the function
  cmmsc_proc_cmd_subsc_chgd during processing based on which subscription.

DEPENDENCIES
  cmmsc_init_multi_stacks() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsimc_proc_cmd_subsc_chgd
(

        mmoc_subsc_chg_e_type           chg_type,
             /* Subscription change type.
             */

        byte                            nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

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

        cm_pref_s_type                 *hybr_2_pref_info_ptr,
           /* hybr2 preference info pointer.
           */

        #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
        cm_pref_s_type                  *hybr_3_pref_info_ptr,
           /* Roam preference that is associated with the selected NAM.
           */
        #endif

        boolean                         is_gwl_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available, false = subscription not available. */

        boolean                         is_hybr_gw_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available, false = subscription not available. */

        boolean                         is_hybr_3_gw_subsc_avail,
           /* Current GSM/WCDMA subscription availability status.
           ** true = subscription available, false = subscription not available. */

        boolean                         is_cdma_subsc_avail,
           /* Current CDMA/AMPS/HDR subscription availability status.
           ** true = subscription available, false = subscription not available. */

        prot_subsc_chg_e_type           prot_subsc_chg,
           /* Enumeration of protocols that have a change in
           ** subscription available status */

        prot_subsc_chg_e_type           hybr_2_prot_subsc_chg,
           /* Enumeration of protocols that have a change in
              subscription available status */

        prot_subsc_chg_e_type           hybr_3_prot_subsc_chg
);

#ifdef FEATURE_MMODE_DUAL_ACTIVE

/*===========================================================================

FUNCTION cmmsimc_suspend_or_resume_other_subs
DESCRIPTION
  Function to suspend or resume other SS

DEPENDENCIES

RETURN VALUE
  none
  
SIDE EFFECTS
  none

===========================================================================*/
void cmmsimc_suspend_or_resume_other_subs
(
  sys_modem_as_id_e_type asubs_id, 
  boolean                suspend
);
#endif

/*===========================================================================

FUNCTION cmmsimc_proc_rpt_srv_ind

DESCRIPTION
  This function should be called to process the CM_SRV_IND_INFO_F report
  from SD20.

DEPENDENCIES
  cmpref2_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsimc_proc_rpt_srv_ind
(
        const cm_sd_rpt_u_type         *rpt_ptr,
           /* Pointer to the CM_SRV_IND_INFO_F report.
           */
        sys_srv_status_e_type         prev_srv_status_ss

);

/*===========================================================================

FUNCTION  cmmsimc_proc_rpt_lte_do_irat

DESCRIPTION
This function process the reselection reports from SD20/MMOC at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_rpt_lte_do_irat (void);

/*===========================================================================

FUNCTION  cmmsimc_proc_rpt_redial_hdr_fail

DESCRIPTION
This function process the faked ACQ_FAIL report from SD20 to trigger 1s BSR at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_rpt_redial_hdr_fail (void);

/*===========================================================================

FUNCTION  cmmsimc_proc_disable_hybr_hdr

DESCRIPTION
This function process disable hybr hdr at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_disable_hybr_hdr (void);

/*===========================================================================

FUNCTION  cmmsimc_proc_enable_hybr_hdr

DESCRIPTION
This function process enable hybr hdr at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_enable_hybr_hdr (

        boolean                        send_pref_sys_cmd
        /* Flag to indicate if pref. sys changed command has to be sent.
        */
);

/*===========================================================================

FUNCTION  cmmsimc_proc_call_end

DESCRIPTION
This function process timer expiration at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsimc_proc_call_end (void);

/*===========================================================================
===========================================================================*/
void cmmsimc_proc_call_event(
  cm_ss_e_type         ss,
  cm_call_type_e_type  call_type,
  cm_call_event_e_type call_event
);

/*===========================================================================

FUNCTION  cmmsimc_proc_timer

DESCRIPTION
This function process enable hybr hdr at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsimc_proc_timer (void);

/*===========================================================================

FUNCTION  cmmsimc_proc_rpt_hybr_bsr_to_hdr

DESCRIPTION
This function process CM_HYBR_BSR_TO_HDR_F rpt at the SIM level.

DEPENDENCIES
  cmmsimc_state_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void  cmmsimc_proc_rpt_hybr_bsr_to_hdr (
  sd_hybr_bsr_to_hdr_s_type  hybr_bsr_to_hdr_status
);
 /*===========================================================================
 
 FUNCTION  cmmsimc_proc_rpt_kick_hybr2
 
 DESCRIPTION
 This function process CM_KICK_HYBR2_F rpt at the SIM level.
 
 DEPENDENCIES
   cmmsimc_state_init() should have been called previously.
 
 RETURN VALUE
   None.
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
void  cmmsimc_proc_rpt_kick_hybr2 (void
 );

void cmmsimc_act_route_request( cmmsc_param_base_s_type * event_package);
void cmmsimc_act_subs_change( cmmsc_param_base_s_type * event_package);

/*===========================================================================

FUNCTION cmmsimc_send_subsc_chgd_current_pref

DESCRIPTION
  This function should be called when the subscrptions need to be refreshed
  using the current preferences on both the stacks.This function should be
  called just before sending the subsc_chgd command to MMOC.

DEPENDENCIES
  cmpref2_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsimc_send_subsc_chgd_current_pref(

        prot_subsc_chg_e_type main_subsc_chg,
		
	prot_subsc_chg_e_type hybr_2_subsc_chg,
		
	prot_subsc_chg_e_type hybr_3_subsc_chg 
);

void cmmsimc_act_wake_up(
     int                         sim_idx,
     cmmsc_param_srv_ind_s_type *event_package);

void cmsimc_act_peer_subs_call(
  cmmsc_param_call_ind_s_type *call_ind
);

void cmmsimc_act_on_off(
     cmmsimc_action_e_type     action,
     cmmsc_param_base_s_type * event_package
);

/*===========================================================================
FUNCTION cmmsimc_proc_cmd_standby_pref_chgd

DESCRIPTION
  This function should be called when the operating mode of the phone has
  changed.This function should be called just before sending the operating
  mode changed command to MMOC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void    cmmsimc_proc_cmd_standby_pref_chgd
(
        sys_modem_dual_standby_pref_e_type    standby_pref,
           /* New standby preference of the phone.
           */

        uint8                                 active_subs_id,
           /* New active subscription when the standby preference is
           ** single
           */
        boolean                               is_cdma_subscription_available,
          /**< Subscription status of CDMA system */

        boolean                               is_gwl_subscription_available,
          /**< Subscription status of GSM/WCDMA system */

        boolean                               is_hybr_gw_subscription_available,
          /**< Subscription status of GSM/WCDMA system */

        boolean                               is_hybr_gw3_subscription_available,

        boolean                               int_dds_cmd
);

void cmmsimc_act_power_down( cmmsc_param_oprt_mode_s_type * event_package);

void cmmsimmsc_proc_rpt_acq_fail( cm_ss_e_type ss , 
                                 sd_ss_mode_pref_e_type mode_pref);
void cmmsimmsc_proc_rpt_3gpp_stopped( sd_ss_e_type sd_ss );

/*===========================================================================

FUNCTION cmmsimc_get_sim_cap

DESCRIPTION
  The function gets the sim capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_mode_pref_e_type  cmmsimc_get_sim_cap (
  cm_ss_e_type  cm_ss
);

/*===========================================================================

FUNCTION cmmsimc_get_msc_ptr_per_cm_ss

DESCRIPTION
  The function gets the MSC state machine pointer base on the specified cm_ss.

DEPENDENCIES
  None

RETURN VALUE
 cmmsc_state_machine_s_type *

SIDE EFFECTS
  None

===========================================================================*/
cmmsc_state_machine_s_type * cmmsimc_get_msc_ptr_per_cm_ss (
  cm_ss_e_type  cm_ss
);


/*===========================================================================

FUNCTION cmmsimc_get_msc_stack_state

DESCRIPTION
  Get state of a stack from MSC

===========================================================================*/
cmmsc_stack_state_e_type  cmmsimc_get_msc_stack_state(
  cm_ss_e_type  cm_ss
);

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
boolean  cmmsimc_is_in_srv (
  cm_ss_e_type  cm_ss
);

/*===========================================================================

FUNCTION cmmsimc_get_msc_ptr_per_sd_ss

DESCRIPTION
  The function gets the MSC state machine pointer base on the specified sd_ss.

DEPENDENCIES
  None

RETURN VALUE
 cmmsc_state_machine_s_type *

SIDE EFFECTS
  None

===========================================================================*/
cmmsc_state_machine_s_type * cmmsimc_get_msc_ptr_per_sd_ss (
  sd_ss_e_type  sd_ss
);


boolean cmmsimc_is_esr_on_call_required
(
  const cmcall_s_type *call_ptr
);

void cmmsimc_send_esr_req
(
  const cmcall_s_type *call_ptr
);

void cmmsimc_proc_rpt_esr_rsp
(
  uint8 context_id
);


#endif /* CMSIMCOORD_H */
