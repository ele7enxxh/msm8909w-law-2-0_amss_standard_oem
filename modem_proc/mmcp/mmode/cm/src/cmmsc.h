#ifndef CMMSC_H
#define CMMSC_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
             C M    M U L T I - S T A C K    C O N T R O L L E R

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file implements the CM multi-stack architecture to support multi-stacks
  requirements such as SVLTE, DualSim, SGLTE, etc.

EXTERNALIZED FUNCTIONS
  Functional Interface:



Copyright (c) 2003 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmsc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/02/14   ss      Check for on-going voice or emergency calls before forcing 
                   user prefs down to MMoC in SGLTE 
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
#include "sd_v.h"
#include "mmoc.h"     /* MMEXTNal interface definitions for MMoC */

#include "cm_dualsim.h"
#include "cmi.h"
#include "cmph.h"     /* Interface to CM phone object. */
#include "cmss.h"     /* Interface to CM Serving system object. */


/*===========================================================================

            LOCAL DEFINITIONS OF CONSTANTS AND ENUMS FOR MODULE

This section contains local definitions for constants and enums used in
this module and

===========================================================================*/
#define CMMSC_STACK_NUM   3               /* Max num of stacks, ie. 3 */
#define CMMSC_SGLTE_STACK_NUM   2         /* Max num of stacks, ie. 2 for SGLTE  */
#define CMMSC_SVLTE_STACK_NUM   3         /* Max num of stacks, ie. 3 for SVLTE  */

#define CMMSC_STACK_ID_ALL 100
#define CMMSC_STACK_ID_NONE  (int)(-1)

/* Default timer values in secs */
#define GSM_DATA_SERVICE_TRANSFER_TIMER_DEFAULT_VALUE      150
#define SGLTE_TO_FULL_RAT_SEARCH_TIMER_DEFAULT_VALUE       300
#define NON_SGLTE_TO_FULL_RAT_SEARCH_TIMER_DEFAULT_VALUE   300

/* Reserved value; to indicate timer not running */
#define CMMSC_UPTIME_NONE              (dword)(-1)

/* "Acq_hdr_timer", set to 7 days.
*/
#define CMMSC_ACQ_HDR_TIMER           (dword)(604800)

/* "Avoid_hybr_hdr_timer", set to 60 seconds.
*/
#define CMMSC_AVOID_HYBR_HDR_TIMER    (dword) (60)

#if defined(FEATURE_JCDMA)
/* "Lost_main_timer", set to 2 hrs
*/
#define CMMSC_LOST_MAIN_TIMER         (dword) (7200)
#else
/* "Lost_main_timer", set to 120 seconds.
*/
#define CMMSC_LOST_MAIN_TIMER         (dword) (120)
#endif /* defined(FEATURE_JCDMA) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Structure of mini set of hybr preference .
** This is used for any hybr stacks other than main.
*/
typedef struct {
  sd_ss_orig_mode_e_type    hybr_orig_mode;
  cm_mode_pref_e_type       hybr_mode_pref;
} cm_hybr_ss_preference_s_type;


/* Structure to maintain stack kick information
*/
typedef struct
{
  /* is hybr1 hdr kick pending - only apply to svlte/auto */
  boolean  is_kick_hdr_pending;

  /* is to restore 3gpp - hicps */
  boolean  is_kick_gw;
  
  /* is hybr2 kick pending - only apply to svlte */
  boolean  is_hybr_2_kick_pending;

  /* is stack sync up bsr pending  - only apply to svlte */
  boolean  is_stack_sync_bsr_pending;
} cmmsc_stack_kick_s_type;

/* Structure to maintain timers
*/
typedef struct
{
  /* is hybr1 hdr kick pending - only apply to svlte/auto */
  dword  lost_main_uptime;

  /* is to restore 3gpp - hicps */
  dword  acq_hybr_1_uptime;
  
  /* is hybr2 kick pending - only apply to svlte */
  dword  avoid_hybr_hdr_uptime;
} cmmsc_timer_s_type;

/* Structure for additional pref info for sending pref_sys_chgd cmd to mmoc
*/
typedef struct
{
  sd_ss_pref_update_reason_e_type  *pref_update_reas;
             /**< Preference change reason
             */
} cmmsc_addl_pref_sys_chgd_mmoc_cmd_s_type;

/* Structure for stack preferences
*/
typedef struct
{
  sd_ss_pref_reas_e_type         pref_reas;
      /* Reason for changing the SS-Preference.
      */

  sd_ss_pref_update_reason_e_type pref_update_reason;

  sd_ss_orig_mode_e_type         orig_mode;
      /* The origination mode (normal, OTASP, emergency, other, etc.)
      ** that is associated with the selected NAM.
      */

  sd_band_e_type                 otasp_band;
     /* The band-class that is associated with OTASP origination mode.
     ** Note that this parameter is ignored if orig_mode != OTASP.
     */

  sd_blksys_e_type               otasp_blksys;
    /* The PCS frequency block/Cellular system that is associated
    ** with OTASP origination mode. Note that this parameter is ignored
    ** if orig_mode != OTASP.
    */

  sd_ss_avoid_sys_e_type         avoid_type;
    /* Type of the system to be avoided, valid only
    ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
    */

  dword                          avoid_time;
    /* Time in seconds for which the system is to be avoided, valid
    ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
    */
  const cm_acq_pri_order_pref_s_type *rat_acq_order_ptr;
     /* Rat acquisition priority order
     */

  sd_ss_pref_camp_mode_e_type     camp_mode_pref;

  cm_mode_pref_e_type               mode_pref;
    /**< Switch to one of the following mode preferences. */

  cm_band_pref_e_type               band_pref;
    /**< Switch to this band preference. */

  sys_lte_band_mask_e_type          lte_band_pref;
    /**< Indicate current LTE band preference */

  cm_band_pref_e_type               tds_band_pref;
    /**< Indicate current TDS band preference */

  cm_hybr_pref_e_type               hybr_pref;
    /**< Indicate the hybrid preference setting. */

  cm_prl_pref_e_type                prl_pref;
    /**< The current PRL preferred setting. */

  cm_roam_pref_e_type               roam_pref;
    /**< Indicate the current roaming preference. */

  cm_pref_term_e_type               pref_term;
    /**< Indicate the current mode preference term -
         one call, power cycle, etc. */

  cm_gw_acq_order_pref_e_type       acq_order_pref;
    /**< GW acquisition order preference. */

  cm_srv_domain_pref_e_type         srv_domain_pref;
    /**< Switch to this service domain (e.g., Circuit Switch).\
         Set to CM_SRV_DOMAIN_PREF_NO_CHANGE if a service
         domain is not being changed. */

  cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
    /**< Defines whether the network should be selected automatically
         or manually. */

  sys_manual_sys_info_s_type        sys_id;
  /* When the net_sel_pref is manual, this field gives the Id
  ** of the system to acquire.
  */

  cm_user_net_sel_mode_pref_e_type  user_net_sel_mode;
    /**< User selected network selection mode pref.
         Can be AUTOMATIC/MANUAL/DEFAULT. */

  dword                             mode_uptime;
    /**< Indicate uptime for temporary mode terms - i.e. the uptime
    ** when the temporary mode is expired and phone should restore
    ** the persistent mode preference */

  sys_csg_id_type                   csg_id;
    /**< CSG identifier */

  sys_radio_access_tech_e_type      csg_rat;
    /**< RAT specified for CSG */

  cm_hybr_ss_preference_s_type      hybr1_pref;

  cm_act_update_reas_e_type         act_update_reas;
    /** Reason for the update */
} cm_preference_info_s_type;

typedef struct
{

  cm_mode_pref_e_type               mode_pref;
    /**< Switch to one of the following mode preferences. */
  
  cm_band_pref_e_type               band_pref;
    /**< Switch to this band preference. */
  
  sys_lte_band_mask_e_type          lte_band_pref;
    /**< Indicate current LTE band preference */
  
  cm_band_pref_e_type               tds_band_pref;
    /**< Indicate current TDS band preference */
} cm_mode_band_pref_s_type;

#define CMMSC_PROC_HDR                \
  cm_ss_e_type  cm_ss; 

typedef struct
{
  CMMSC_PROC_HDR
  cm_preference_info_s_type  *pref_info;
  boolean                     is_new_policy_tobe_read;
} cmmsc_proc_cmd_pref_sys_chgd_s_type;


typedef enum
{
  CMMSC_STACK_STATE_NONE = -1,

  CMMSC_STACK_STATE_NULL,

  CMMSC_STACK_STATE_INACTIVE,

  CMMSC_STACK_STATE_ACTIVE,

  /* State for 2nd stack when ue mode is single stack SGLTE */
  CMMSC_STACK_STATE_BARRED,

  /* Only applies to auto on main and hybr hdr */
  CMMSC_STACK_STATE_SRV,

  /* Only applyies to hybr hdr - enter this state when 1x has service, but HDR has not acquired
  ** for very long duration (acq_hybr_1_uptime - 7 days)
  ** exit the state when 1x is lost for lost_main_uptime.
  */
  CMMSC_STACK_STATE_HDR_EXT_NO_SRV,

  CMMSC_STACK_STATE_SUSPENDED,
  
  CMMSC_STACK_STATE_ANY,

  CMMSC_STACK_STATE_NO_CHG,

}cmmsc_stack_state_e_type;

typedef enum
{
  CMMSC_EVENT_NONE = 0,

  CMMSC_EVENT_SUB = 1,

  CMMSC_EVENT_ONLINE =2,

  /* To trigger diable hybr1 along with peodo online */
  CMMSC_EVENT_ONLINE_DISABLE_HYBR1 = 3,

  CMMSC_EVENT_POWER_DOWN = 4,

  CMMSC_EVENT_SYS_SEL_PREF_MAIN      = 10,

  CMMSC_EVENT_SYS_SEL_PREF_HYBR1 = 11,

  CMMSC_EVENT_SYS_SEL_PREF_HYBR2 = 12,

  /* Used when ss = max but feature mode is neither svlte nor sglte */
  CMMSC_EVENT_SYS_SEL_PREF_MAIN_HYBR1 = 13,

  CMMSC_EVENT_SYS_SEL_PREF_MAX = 14,

  CMMSC_EVENT_GSM_PS_DOMAIN_TRANSFER = 20,

  CMMSC_EVENT_DUAL_STK_EMERG_CALL = 21,

  CMMSC_EVENT_KICK_HYBY1     = 30,

  /* feature mask auto specific event */
  CMMSC_EVENT_LTE_DO_RESEL     = 31,

  CMMSC_EVENT_RPT_REDIAL_HDR_FAIL = 32,

  CMMSC_EVENT_FORCE_COLLOC = 33,

  CMMSC_EVENT_RPT_HYBR_BSR_TO_HDR_START = 34,
  
  CMMSC_EVENT_RPT_HYBR_BSR_TO_HDR_END = 35,

  CMMSC_EVENT_RPT_HYBR_BSR_TO_HDR_OTHER = 36,

  /* Event when lost_main_uptime expires */
  CMMSC_EVENT_TIMER_EXP_MAIN_LOST = 37,

  /* Event when acq_hybr_1_uptime expires */
  CMMSC_EVENT_TIMER_EXP_ACQ_HYBR1 = 38,


  /* ACQ FAIL related events */
  CMMSC_EVENT_ACQ_FAIL_MAIN     = 40,

  CMMSC_EVENT_ACQ_FAIL_HYBR1 = 41,

  CMMSC_EVENT_ACQ_FAIL_HYBR2 = 42,

  CMMSC_EVENT_RESTORE_DEFERRED_3GPP = 43,

  CMMSC_EVENT_MAIN_3GPP_STOPPED = 44,

  CMMSC_EVENT_HYBR_3GPP_STOPPED = 45,

  /* Event for hicp process per policy change */
  CMMSC_EVENT_HICPS_POLICY_CHGD = 46,

  /* SRV IND events */
  CMMSC_EVENT_1X_SRV    = 50,

  CMMSC_EVENT_MAIN_HDR_SRV = 51,

  CMMSC_EVENT_HYBR_HDR_SRV = 52,

  CMMSC_EVENT_MAIN_3GPP_SRV = 53,

  CMMSC_EVENT_HYBR_3GPP_SRV = 54,

  CMMSC_EVENT_MAIN_SRV_LOST = 55,

  CMMSC_EVENT_HYBR1_SRV_LOST = 56,

  CMMSC_EVENT_HYBR2_SRV_LOST = 57,
  
  CMMSC_EVENT_KICK_HYBR2 = 58,

  CMMSC_EVENT_ON = 70,
  CMMSC_EVENT_OFF = 71,

  CMMSC_EVENT_CALL_END_OTHER_SUB = 72,

  CMMSC_EVENT_SUSPEND = 73,
  CMMSC_EVENT_RESUME  = 74,

  CMMSC_EVENT_MAX

} cmmsc_event_e_type;

typedef enum
{
  CMMSC_ACTION_NONE = 0,

  /* Send sub chg cmd to mmoc - the action is not per stack */
  CMMSC_ACTION_SUB = 1,

  CMMSC_ACTION_POWER_DOWN = 2,

  /* Force the top preference with pref reason user with additinal action as Resume */
  CMMSC_ACTION_FORCE_USER_WITH_RESUME = 9,

  /* Force the top preference with pref reason user */
  CMMSC_ACTION_FORCE_USER = 10,

  /* Force the top preference with additional params */
  CMMSC_ACTION_FORCE_TOP_EXT = 11,

  /* Force phone preference with additional params, ie. orig_mode, hybr_pref */
  CMMSC_ACTION_FORCE_PH_EXT = 12,

  /* Force preference with reason = restore.  Only to update sd with new preference but
  ** not to trigger any sd action
  */
  CMMSC_ACTION_FORCE_RESTORE = 13,
  
  /* Force preference with reason = stack sync up */
  CMMSC_ACTION_FORCE_STACK_SYNC_UP = 14,
  
  /* Force preference with reason = stack sync up BSR */
  CMMSC_ACTION_FORCE_STACK_SYNC_UP_BSR = 15,

  /* Force preference with reason = stack sync up BSR and additional info */
  CMMSC_ACTION_FORCE_STACK_SYNC_UP_BSR_END = 16,

  /* Force preference with various reason, ie. call, user, etc */
  CMMSC_ACTION_SYS_SEL_PREF = 17,

  /* Force preference for the case of ps domain transfer */
  CMMSC_ACTION_G_PS_DOMAIN_TRNSFR = 18,

  /* Activate the specified stack locally in cm, mmoc, and sd - only applies to hybr2 */
  CMMSC_ACTION_LOCAL_ACT = 19,

  /* Deactivate the specified stack locally in cm, mmoc, and sd - applies only hybr2 */
  CMMSC_ACTION_LOCAL_DEACT = 20,

  /* acq fail related actions */
  CMMSC_ACTION_HICPS_REMOVE_3GPP = 40,

  CMMSC_ACTION_SVLTE_REMOVE_3GPP = 41,

  CMMSC_ACTION_RESTORE_3GPP = 42,

  CMMSC_ACTION_DEFER_3GPP = 43,

  CMMSC_ACTION_UPDATE_NO_SRV = 44,

  CMMSC_ACTION_SHORTEN_BSR = 45,

  CMMSC_ACTION_HICPS_COND_KICK = 46,

  CMMSC_ACTION_SVLTE_STOP_STACK = 47,
  
  CMMSC_ACTION_SVLTE_COND_KICK = 48,

  /* Conditionaly kick hybr2 for sub or oprt mode change */
  CMMSC_ACTION_SUB_CHG_COND_KICK = 49,

  CMMSC_ACTION_PENDING_KICK = 50,

  /* srv ind related actions  */
  CMMSC_ACTION_UPDATE_SRV = 51,

  /* they're different than remove/restore 3gpp, due to pre-condition */
  CMMSC_ACTION_ENABLE_3GPP = 52,
  
  CMMSC_ACTION_DISABLE_3GPP = 53,

  CMMSC_ACTION_SYNC_UP = 54,

  CMMSC_ACTION_RESUME_SRV_ACQ = 55,

  CMMSC_ACTION_KICK_HYBR2 = 56,


  CMMSC_ACTION_SUSPEND = 57,

  CMMSC_ACTION_RESUME = 58,
  
  CMMSC_ACTION_SXLTE_KICK_NOT_AUTO = 59,

  CMMSC_ACTION_MAX
}
cmmsc_action_e_type;


typedef enum
{
  /* Initial invalid state */
  CMMSC_OP_MODE_NONE = 0,

  /* UE operates in SGLTE dual-stack. Dual-Stack operation contains:
  ** LTE/TD(PS) on MAIN
  ** GSM(CS) on HYBR-2. GSM is restricted to only SGLTE bands.
  ** This is consider operating in HOME.
  */
  CMMSC_OP_MODE_SGLTE_DUAL_STACK = 1,

  /* UE operates in SGLTE single-stack. Single-Stack operation contains:
  ** LTE/TD/GSM(CS+PS) on MAIN
  ** GSM is restricted to only SGLTE bands.
  ** This is consider operating in HOME.
  */
  CMMSC_OP_MODE_SGLTE_SINGLE_STACK = 2,

  /* UE operates in SGLTE Roaming. Roaming operation contains:
  ** G/W/L on MAIN.
  ** No restriction on Bands. This is operating in Roaming country.
  */
  CMMSC_OP_MODE_SGLTE_ROAM = 3,

  /* UE operates in normal mode in case of CSFB only op mode, legacy mode, power up without
   ** sim card, and user pref does not contain 2G+4G.  In this mode, UE operates on single stack.
   */
  CMMSC_OP_MODE_NORMAL = 4,

  /* UE operates in Full Rat Search mode searching for limited services */
  CMMSC_OP_MODE_FULL_RAT_SEARCH = 5,

  /* --------------------------- */
  /* -------- Automatic --------- */
  /* --------------------------- */

  /* UE operates in automatic 1 stack mode - only main stack is on */
  CMMSC_OP_MODE_AUTO_MAIN = 10,

  /* UE operates in automatic 2 stack mode - hybr1 is active but not hybr2 stack */
  CMMSC_OP_MODE_AUTO_MAIN_HYBR1 = 11,

  /* UE operates in automatic 2 stack mode - hybr2 is active but not hybr1 stack */
  CMMSC_OP_MODE_AUTO_MAIN_HYBR2 = 12,

  /* UE operates automatic 3 stack mode - hybr2 stack is active */
  CMMSC_OP_MODE_AUTO_MAIN_HYBR1_HYBR2 = 13,

  CMMSC_OP_MODE_AUTO_SVLTE = 20,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Internal Usage */
  /* Any mode will match this mode (used in comparision, not real mode) */
  CMMSC_OP_MODE_ANY = 21,

  /* Max enum value */
  CMMSC_OP_MODE_MAX

}cmmsc_state_machine_op_mode_e_type;

typedef enum
{
  CMMSC_ROUTE_TABLE_NONE=-1,
  CMMSC_ROUTE_TABLE_AUTO,
  CMMSC_ROUTE_TABLE_SGLTE,
}cmmsc_routing_table_e_type;

/*===========================================================================

                      LOCAL DEFINITIONS OF DATA STRUCTURE

This section contains local definitions for constants and enums used in
this module and

===========================================================================*/

typedef struct cmmsc_stack_common_info_s
{
  sys_ue_mode_e_type            ue_mode;
  boolean                       ue_mode_forced;
  sys_sys_mode_mask_e_type      ue_mode_capability;
  sys_band_mask_e_type          ue_band_capability;
  sys_band_mask_e_type          ue_tds_band_capability;
  sys_lte_band_mask_e_type      ue_lte_band_capability;
  sys_sys_mode_mask_e_type      ue_max_mode_capability;

  /* Indicate if hybr1 is disabled */
  boolean                       is_hybr1_diabled;

  /* Structure to maintain stack kick information */
  cmmsc_stack_kick_s_type       stack_kick;

  /* Structure to maintain timers */
  cmmsc_timer_s_type            timer;
     
  /* This variable indicates if the CMMSC is currently function in emergency
  ** call.
  ** For SGLTE, emergency call required user to suspend MAIN, and hence the
  ** capability is changed for main and hybr-2 stacks.
  */
  boolean                       is_in_emergency_call;

  /* B39 forbid/restore. NAS needs to be told to force registration with n/w*/
  boolean                       is_force_reg_pending;

  boolean                       is_ue_moved_to_srlte;
  /* LTE bands which are capable of concurrent operation for SVLTE mode. */
  sys_lte_band_mask_e_type        svlte_capable_lte_bands;
  
  /* Indicates if ue_mode_change is in progress */
  boolean                       is_ue_mode_chg_in_progress;

  /* mode/band/lte_band/tds_band */
  cm_mode_band_pref_s_type         preference;

  sys_call_mode_t                  call_mode;
}
cmmsc_stack_common_info_s_type;

typedef struct cmmsc_stack_capability_s
{
  sd_ss_mode_pref_e_type        mode;
  sd_ss_band_pref_e_type        band;
  sd_ss_band_pref_e_type        tds_band;
  sys_lte_band_mask_e_type      lte_band;
  sd_ss_srv_domain_pref_e_type  domain;
}
cmmsc_stack_capability_s_type;

/* First, implement smallest unit which is a stack
** name should be: cm multi-stack controller
*/
typedef struct cmmsc_stack_s
{
  /* id is in "int" because when we scale the stack, for loop makes sense. */
  int                              stack_id;

  /* Having service / no service. In a sense, state
     is like a sub-status */
  cmmsc_stack_state_e_type         state;

  /* Then it should have its capability. Capability contains mode/band/etc. */
  cmmsc_stack_capability_s_type    capablity;

  /* stack previous preference that were used to attached to network
  ** If the preference is changed, CM will triggers PS_DETACH.
  ** For SGLTE, during FullRAT Mode, we're not expect to do any registration and
  ** hence for FullRAT Mode (CAMP_ONLY), this prev_registration_capability will not
  ** be saved.
  **
  ** Implemented per NAS team requirement:
  **    Expectation of Local/OTA detach or no detach in SGLTE context.
  ** 1. SGLTE -> FULL RAT (in OOS) -> SGLTE ( NO Detach as start ue_mode and end ue_mode are same)
  ** 2. SGLTE -> FULL RAT (in OOS) -> CSFB ( Local Detach)
  ** 3. CSFB -> FULL RAT (in OOS) -> CSFB ( No detach)
  ** 4. CSFB -> FULL RAT (in OOS) -> SGLTE (Local detach)
  ** 5. CSFB -> SGLTE (OTA detach)
  ** 6. SGLTE -> CSFB (OTA detach)
  */
  cmmsc_stack_capability_s_type    prev_registration_capability;

  sys_addtl_action_s_type           addl_action;

}cmmsc_stack_s_type;

/* Action type */
typedef struct cmmsc_action_s
{
  int stack_id;

  cmmsc_action_e_type action;

}cmmsc_action_s_type;


/* State machine : contains multiple stack */
typedef struct cmmsc_state_machine_s
{
  /* Hold multiple stacks */
  cmmsc_stack_s_type                  stack[CMMSC_STACK_NUM];

  sys_modem_as_id_e_type              asubs_id;

  /* Routing Table which they preferred to */
  cmmsc_routing_table_e_type          route_tbl;

  /* current state-machine operation mode */
  cmmsc_state_machine_op_mode_e_type  op_mode;

  /* Hold the common info cross stacks */
  cmmsc_stack_common_info_s_type      stack_common_info;

  /* Infomration related to the state machine itself */
}cmmsc_state_machine_s_type;


#define MSC_EVENT_PACKAGE_HDR                \
  cmmsc_event_e_type  event;                 \
  cmmsc_state_machine_s_type *state_machine

typedef struct cmmsc_event_base_s
{
  MSC_EVENT_PACKAGE_HDR;
}cmmsc_event_base_s_type;

/* Event package contains the event and parameters */
typedef struct cmmsc_event_ss_pref_s
{
  MSC_EVENT_PACKAGE_HDR;
    /* event type */

  cmmsc_proc_cmd_pref_sys_chgd_s_type  *cmd;
  /* pointer to the cmd_pref_sys_chgd data structure
  */

}cmmsc_event_ss_pref_s_type;

/* Event package for oprt_mode change
*/
typedef struct cmmsc_event_oprt_mode_s
{
  MSC_EVENT_PACKAGE_HDR;
  /* event type */

}cmmsc_event_oprt_mode_s_type;

/* Event package for subsc_chgd change
*/
typedef struct cmmsc_event_sub_chg_s
{
  MSC_EVENT_PACKAGE_HDR;
  byte                     nam;
  mmoc_subsc_chg_e_type    chg_type;
  prot_subsc_chg_e_type    prot_sub_chg[MAX_SIMS];
  mmoc_subs_data_s_type   *sub_data[MAX_SUB_SESSION_TYPES];
  size_t                   size_of_sub_data;
}cmmsc_event_sub_chg_s_type;

typedef struct cmmsc_event_force_pref_s
{
  MSC_EVENT_PACKAGE_HDR;
    /* event type */

  sd_ss_orig_mode_e_type  orig_mode;
  cm_hybr_pref_e_type     hybr_pref;
  sd_ss_pref_reas_e_type  pref_reas;
}cmmsc_event_force_pref_s_type;

typedef struct
{
  MSC_EVENT_PACKAGE_HDR;
    /* event type */

  cm_ss_e_type              ss;
  cm_mode_pref_e_type       main_orig_mode_pref;
  sd_ss_mode_pref_e_type    acq_fail_mode_pref;

}cmmsc_event_acq_fail_s_type;

typedef struct cmmsc_event_srv_ind_s
{
  MSC_EVENT_PACKAGE_HDR;
  const cm_sd_rpt_u_type       *rpt_ptr;
  sys_srv_status_e_type         prev_srv_status_ss;
}cmmsc_event_srv_ind_s_type;

typedef struct
{
  MSC_EVENT_PACKAGE_HDR;
    /* event type */
}cmmsc_event_suspend_resume_s_type;

typedef struct cmmsc_routing_table_entry_base_s
{
  cmmsc_event_e_type                event;

}cmmsc_routing_table_entry_base_s_type;



/* ROUTING TABLE BASE STRUCTURE
*/
typedef struct cmmsc_routing_table_s
{
  /* A Routing Table ID */
  cmmsc_routing_table_e_type              route_tbl_id;

  /* The number of stacks */
  byte                                    n_stack; /* stacks */

  /* The number of rows */
  byte                                    n_row;

  /* A pointer to routing table */
  cmmsc_routing_table_entry_base_s_type  *route_tbl;

}cmmsc_routing_table_s_type;



/*==================================================================================*/
/* Events & Package headers for Parameters that are passing from CMMSIMC --> CMMSC  */
/*============================= SECTION START ======================================*/
#define MSIMC_EVENT_PACKAGE_HDR                  \
  cmmsimc_event_e_type  event;                   \
  cmmsc_state_machine_s_type *state_machine

/* Base event header*/
typedef struct cmmsc_param_base_s
{
  MSIMC_EVENT_PACKAGE_HDR;
}cmmsc_param_base_s_type;

typedef struct cmmsc_param_oprt_mode_s
{
  MSIMC_EVENT_PACKAGE_HDR;
  sys_oprt_mode_e_type oprt_mode;
}cmmsc_param_oprt_mode_s_type;

typedef struct cmmsc_param_pref_sys_s
{
  MSIMC_EVENT_PACKAGE_HDR;
  cmmsc_proc_cmd_pref_sys_chgd_s_type *cmd;
}cmmsc_param_pref_sys_s_type;

typedef struct cmmsc_param_subs_chgd_s
{
  MSIMC_EVENT_PACKAGE_HDR;
  byte                     nam;
  mmoc_subsc_chg_e_type    chg_type;
  prot_subsc_chg_e_type    prot_sub_chg[MAX_SIMS];
  boolean                  apply_standby_pref_chg;
  mmoc_subs_data_s_type   *sub_data[MAX_SUB_SESSION_TYPES];
  size_t                   size_of_sub_data;
  uint8                    active_subs;
}cmmsc_param_subs_chgd_s_type;

/* params for cmmsc_auto_proc_cmd_enable_hybr_hdr */
typedef struct cmmsc_param_proc_enable_hybr_hdr_s
{
  MSIMC_EVENT_PACKAGE_HDR;
  boolean  send_pref_sys_cmd;
}cmmsc_param_proc_enable_hybr_hdr_s_type;

/* params for cmmsc_auto_proc_rpt_lte_do_irat */
typedef struct cmmsc_param_proc_rpt_lte_do_irat_s
{
  MSIMC_EVENT_PACKAGE_HDR;
  cm_ss_e_type  ss;
}cmmsc_param_proc_rpt_lte_do_irat_s_type;

typedef struct cmmsc_param_proc_rpt_hybr_bsr_to_hdr_s
{
  MSIMC_EVENT_PACKAGE_HDR;
  sd_hybr_bsr_to_hdr_s_type  hybr_bsr_to_hdr_status;
}cmmsc_param_proc_rpt_hybr_bsr_to_hdr_s_type;

typedef struct cmmsc_param_srv_ind_s
{
  MSIMC_EVENT_PACKAGE_HDR;
  const cm_sd_rpt_u_type       *rpt_ptr;
  sys_srv_status_e_type         prev_srv_status_ss;
}cmmsc_param_srv_ind_s_type;

typedef struct
{
  MSIMC_EVENT_PACKAGE_HDR;

  cm_call_type_e_type call_type;

  cm_call_event_e_type call_event;

}cmmsc_param_call_ind_s_type;

typedef struct cmmsc_param_suspend_resume_subs_s
{
  MSIMC_EVENT_PACKAGE_HDR;
  uint8                  asubs_id; 
  boolean                suspend;
}cmmsc_param_suspend_resume_subs_s_type;
/*==================================================================================*/
/* Events & Package headers for Parameters that are passing from CMMSIMC --> CMMSC  */
/*=============================== SECTION END ======================================*/




/*===========================================================================
                      LOCAL DEFINITIONS OF FUNCTIONS

            This section contains local functions used in CM
===========================================================================*/


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
void cmmsc_init( void );

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
);

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
);

/*===========================================================================

FUNCTION cmmsc_proc_cmd_oprt_mode_chgd

DESCRIPTION
  This function processes the operation mode change in MSC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void    cmmsc_proc_cmd_oprt_mode_chgd
(
   cmmsc_param_oprt_mode_s_type      *oprt_mode_event
   /* New operating mode of the phone. */
);

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
);

/*===========================================================================

FUNCTION cmmsc_pack_pref_info

DESCRIPTION
  This function packs pref info.

DEPENDENCIES
  None.

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
);

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
);

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
);

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
);

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
);

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
);

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
void    cmmsc_proc_cmd_pref_sys_chgd
(
  cmmsc_param_pref_sys_s_type * pref_sys_param
);

void    cmmsc_send_sys_sel_pref_chgd_cmd(
  cmmsc_state_machine_s_type                 *state_machine,
  int                                         stack_idx,
  cm_preference_info_s_type                  *pref_info,
  const cmmsc_addl_pref_sys_chgd_mmoc_cmd_s_type   *addl_ptr
);

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
  cmmsc_state_machine_s_type     *state_machine,
  cm_ss_e_type                   ss,
  sd_ss_pref_reas_e_type         pref_reason,
  sd_ss_mode_pref_e_type         user_mode_pref,
  cmmsc_stack_capability_s_type  common_pref
);

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
);

cmmsc_state_machine_op_mode_e_type  cmmsc_map_ue_mode_pm2msc (
    sys_ue_mode_e_type    pm_ue_mode
);

sd_ss_srv_domain_pref_e_type cmmsc_get_common_domain (
  sd_ss_srv_domain_pref_e_type  domain1,
  sd_ss_srv_domain_pref_e_type  domain2
);

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
);

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
int cmmsc_mapping_ss_to_stack_idx(cm_ss_e_type ss);

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
void cmmsc_init_stack_common(cmmsc_state_machine_s_type  *state_machine);

/*===========================================================================

FUNCTION cmmsc_init_multi_stacks

DESCRIPTION
  This function should be called to initialize all stacks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmmsc_init_multi_stacks(cmmsc_state_machine_s_type *state_machine);

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
void cmmsc_state_machine_init(cmmsc_state_machine_s_type *state_machine);

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
void cmmsc_init_stack_0 (cmmsc_state_machine_s_type *state_machine);

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
void cmmsc_init_stack_other (
   cmmsc_state_machine_s_type *state_machine,
   int                         stack_idx);

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
void    cmmsc_update_cmss_is_operational(
      cmmsc_state_machine_s_type     *state_machine,
        int                          stack_idx,
        boolean                      is_operational
);


void    cmmsc_update_sub_data_per_cap (
        mmoc_subs_data_s_type          *sub_other_data,
        /* Subscription data for sim != 0
        */

        size_t                                     sim_idx
);

/*===========================================================================

FUNCTION cmmsc_form_sub_data_sim0

DESCRIPTION
  This function forms subscription info for sim0 to send to mmoc.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void   cmmsc_form_sub_data_sim0(

  boolean                           sub_avail[2],
  /* An array containing the info of subs_avail for each sub.
   ** 0 for gw primary, 1 for cdma primary ...
   */

  const cm_pref_s_type      *pref_info,
  /* pref_info pointer for sim0
   */

  mmoc_subs_data_s_type  *sub_data[2]
  /* An array of pointers to point to sub_data for sim0
   */
);

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

        boolean                               sub_avail[],
        /* An array containing the info of subs_avail for each sub.
             ** 0 for gw primary, 1 for cdma primary, 2 for gw secondary
             */

        const cm_pref_s_type         *sim_pref_ptr[],
        /* An array containing pref_info for sim0, sim1, ...
            */

        mmoc_subs_data_s_type     *sub_data[],
        /* An array of pointers to point to sub_data for sim0
            */

        size_t                                size_of_sub_avail,
        /* Array size of sub_avail
        */

        size_t                                size_of_pref_info_ptr,
        /* Array size of pref_info
        */

        size_t                                size_of_sub_data_ptr
        /* Array size of sub_data
        */
);

void   cmmsc_update_sim_other (
       const  cm_pref_s_type             *sim_pref_ptr[],
       /* An array containing pref_info for sim0, sim1, ...
           */

       mmoc_subs_data_s_type          *sub_other_data,
       /* Subscription data for sim != 0
           */

        size_t                                    index_of_pref
        /* Index of pref from which to update sub data
        */
);

/* This function takes in an event package and it:
** 1) Found what is the Routing Index to process
** 2) Process the Actions in that Route Entry
** 3) Update the current states to new states
*/
void cmmsc_state_machine_process(
   cmmsc_state_machine_s_type *state_machine,
   cmmsc_event_base_s_type    *event_package);

/* This function takes an Event and search the Routing Table to give
** the index for which actions should be taken based on current
** states.
*/
int cmmsc_state_machine_find_route_idx(
    cmmsc_state_machine_s_type       *state_machine,
    cmmsc_event_e_type event
);

/* This function takes a Route Index from which it knows what action
** shall be taken, and then it performs the action in this Entry index
** based on the parameters given in the event package.
*/
void cmmsc_execute_actions(
     int                      route_table_idx,
     cmmsc_event_base_s_type *event_package );

/* This function takes a Route Index from which the action is been taken
** and it updates the current states to the new states according to this
** Route Entry
*/
void cmmsc_update_state(
  cmmsc_state_machine_s_type       *state_machine,
  int                               route_table_idx
);

boolean cmmsc_is_msc_multi_stacks (sys_modem_as_id_e_type as_id);

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
);

void cmmsc_get_policy_mode_band_cap (
     cmmsc_state_machine_s_type  *state_machine,
     sys_sys_mode_mask_e_type    *mode,
     sys_band_mask_e_type        *band,
     sys_band_mask_e_type        *tds_band,
     sys_lte_band_mask_e_type    *lte_band
);

void cmmsc_init_max_mode_capability (void);

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
);

void cmmsc_action_force_top(
    cmmsc_state_machine_s_type *state_machine,
    int                         stack_idx,
    sd_ss_pref_reas_e_type      pref_reas
);

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
);

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
);

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
);


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
void cmmsc_set_stack_capability( cmmsc_state_machine_s_type   *state_machine );

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
);

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
);

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
);
#endif

void cmmsc_proc_cmd_on_off(
     boolean                   is_stack_deactivated,
     cmmsc_param_base_s_type * event_package
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
boolean  cmmsc_is_in_srv_state (
  cmmsc_state_machine_s_type  *state_machine,
  int                         stack_idx
);

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
);

boolean  cmmsc_is_in_ext_no_srv(
  cmmsc_state_machine_s_type  *state_machine,
  int                         stack_idx
);


/*===========================================================================
FUNCTION cmmsc_apply_SIM_capability

DESCRIPTION
  This function set stack capability based on its SIM capability
===========================================================================*/
void cmmsc_apply_SIM_capability(cmmsc_state_machine_s_type   *state_machine );

boolean cmmsc_is_hybr2_kick_allowed(

  cmmsc_state_machine_s_type *msc

);

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
);

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

);

/*===========================================================================
FUNCTION cmmsc_get_stack_common_call_mode_per_subs

DESCRIPTION
  This function fetches call mode for given subs id
===========================================================================*/
sys_call_mode_t cmmsc_get_stack_common_call_mode_per_subs(sys_modem_as_id_e_type subs_id);

/*===========================================================================
FUNCTION cmmsc_set_stack_common_call_mode_per_subs

DESCRIPTION
  This function to set the call mode for given subs id
===========================================================================*/
void cmmsc_set_stack_common_call_mode_per_subs(sys_call_mode_t call_mode, sys_modem_as_id_e_type subs_id);



/*===========================================================================

FUNCTION cmmsc_action_sxlte_kick_not_auto

DESCRIPTION

Kick HYBR2 stack on standby preference change

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
);

/*===========================================================================

FUNCTION cmmsc_is_device_oprt_in_LTE

DESCRIPTION
   Determine if the device will operate in LTE.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmmsc_is_device_oprt_in_LTE(

   cmmsc_state_machine_s_type     *state_machine
);
#endif /* CMMSC_H */
