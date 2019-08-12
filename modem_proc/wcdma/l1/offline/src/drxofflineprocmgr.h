#ifndef DRXOFFLINEPROCMGR_H
#define DRXOFFLINEPROCMGR_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         L 1    D R X   OFFLINE PROCESSING  R E L A T E D    D E F I N I T I O N S

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


REFERENCES
  CDMA Mobile Station ASIC Specification sheet.

Copyright (c) 2001-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when        who      what, where, why
--------    ---      ----------------------------------------------------------------
02/19/15    ap       Initial check-in for WL1 QSH framework 
12/16/14    kr       Dont trigger FW sleep fro cell decfg opts if early RF shutdown is blocked
12/12/14    gsk      Get the last search that is performed
11/11/14    rs       Replaced SECOND_REACQ notf with UPD_MEAS_RULES_DONE notf to prevent early RF shutdown evaluation 
                     happening prematurely when RLP kicks in.
09/15/14    skk      Compensate FWCPICH eng in ||el reacq based on diff b/w srchr pos and strongest fing.
08/25/14    skk      Updated FW CPICH valid bpgs table.
07/21/14    kr       removed unwanted variable 
06/20/14    kr/skk   FR19218: ReacqLessPICH(RLP) initial checkin.
05/08/14    rs/pv    FR19940 FCDC code changes
04/24/14    kr/skk   Change the early wakeup cutoff for Seq reacq case.
04/03/14    sks      Adjust outer loop for QXT change.
03/06/14    gsk      Enhancing freeze status query API
03/04/14    sks      Bump up W priority with co-ex for reacq searches for non-true sleep.
01/23/14    rs       Simple opts: increasing early wakeup threshold by 18 bpgs and reducing the sample size 
                     by 2 bps. Also added support to disable these optimization by setting NV 5458 to 0x18.
01/22/14     sks      Set/reset the srch_wakeup status for DSDA priority at first_reacq_failed and waiting_on_srch_Awake respectively.
01/22/14    sks      Raise the priority of PICH decodes and Reacq searches with co-ex manager for DSDA
12/19/13    gsk      Creating a bmask for second reacq to trach pich entity drop
12/12/13    sks      Reset the offline_pich_in_progress flag at PICH drop.
04/04/13    pv       Mutex lock around the check/setting of pich entity srch delay event.  
02/14/13    pv       Extern for SCCPCH start cell time.
02/11/13    pv       API for getting RF div chain status.
02/11/13    pv       Changes for QXT fixes.
01/21/13    pv       Offline PICH changes.
11/26/12    gsk      Externing a function.
09/12/12    pv       Change the early wakeup threshold for seq/prll reacq.
08/29/12    pv       Timeline opts - added new notf events and externs.
08/27/12    pv       Increased the samples beyond PI bits by 2.5 BPGs for 
                     Offline PICH.
08/24/12    pv       Added extern declaration of sampservmgrdb
08/01/12    pv       Externs and new mutexlock.
07/27/12    pv       Backing out previous checkin.
07/22/12    pv       New ENUMs and externs.
07/16/12    sr       Fixed compiler warnings 
07/17/12    pv       Warning fixes.
07/16/12    pv       Externed a function.
07/13/12    saik/pv  Enhance DRX notf mech and arbitration functionality
07/06/12    saik/pv  DRX offline proc mgr first version

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "srchzz.h"

#include "l1rrcif.h"
#include "drxcmd.h"
#include "tlm.h"

#include "wl1api.h"

#include "wl1drxmanager.h"
#include "rxdiv_v.h"


#include "mdspsync.h"
#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
#include "wl1trm.h"
#include "wl1cxm.h"
#endif
#if defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC)
#include "wl1_qsh_ext.h"
#endif /* defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC) */

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
/* globals */
extern uint32 dec_pich_samples_by;
extern uint32 inc_early_wkup_thresh_by; 
#ifdef FEATURE_WCDMA_REACQLESS_PICH
/*Decrease no of samples captured after PI bits in RLP cycle.*/
extern uint32 dec_pich_sample_rlp_by;
#endif

#ifdef FEATURE_WCDMA_REACQLESS_PICH
/*No of samples to be captured after PI bits. */
#define OFFLINE_PICH_SAMPLES_NEEDED_PAST_PI          (5 -dec_pich_sample_rlp_by)
#define OFFLINE_PICH_SAMPLES_NEEDED_PAST_PI_IN_CHIPS (OFFLINE_PICH_SAMPLES_NEEDED_PAST_PI * CHIP_PER_256_CHIP_UNITS)
#else
/* Leave 5 BPGs beyond PI bits.*/
#define OFFLINE_PICH_SAMPLES_NEEDED_PAST_PI_IN_CHIPS (5 * CHIP_PER_256_CHIP_UNITS)
#endif
/* For short QXT, the samples are worth 18.75 bpgs. Leaving a buffer of 0.25 BPG.*/
#define OFFLINE_PICH_SAMPLES_SHORT_QXT_IN_BPGS (21 - dec_pich_samples_by)

/* For long QXT, the samples are worth 31.75 bpgs. Leaving a buffer of 0.25 BPG.*/
#define OFFLINE_PICH_SAMPLES_LONG_QXT_IN_BPGS  (35 - dec_pich_samples_by)

#define OFFLINE_PICH_LONG_TO_SHORT_QXT_DIFF (OFFLINE_PICH_SAMPLES_LONG_QXT_IN_BPGS - OFFLINE_PICH_SAMPLES_SHORT_QXT_IN_BPGS)

/* 45 - 30 = 15 BPG difference between short and long QXT for online PICH*/
#define ONLINE_PICH_LONG_TO_SHORT_QXT_DIFF 15

#define OFFLINE_PICH_SS_ACT_TIME_IMMEDIATE 0xFFFF

#define OFFLINE_PICH_MDSP_MAX_PI_BIT_END_PN 144

#ifdef FEATURE_WCDMA_REACQLESS_PICH
#define OFFLINE_PICH_EARLY_WKUP_THRESH_BPG_PRLL_REACQ (diag_masks_enabled() && diag_f3_trace_enabled_check()) ? 136 : 142
#define OFFLINE_PICH_EARLY_WKUP_THRESH_BPG_SEQ_REACQ  (diag_masks_enabled() && diag_f3_trace_enabled_check()) ? 88 : 94
#define OFFLINE_PICH_EARLY_WKUP_THRESH_BPG_PRLL_REACQ_RLP (diag_masks_enabled() && diag_f3_trace_enabled_check()) ? 138 : 144
#else
#define OFFLINE_PICH_EARLY_WKUP_THRESH_BPG_PRLL_REACQ (107 + inc_early_wkup_thresh_by)
#define OFFLINE_PICH_EARLY_WKUP_THRESH_BPG_SEQ_REACQ  (65 + inc_early_wkup_thresh_by)
#endif

#define OFFLINE_PICH_WAKEUP_ADJ_FOR_PI_END_OF_FRAME  20

#define DRX_NUM_CYCL_OFFL_PICH_TRIG_THRESH 3

#define SAMPSERV_DRX_NOTIFICATIONS_PENDING_NONE  0x0
#define SAMPSERV_DRX_SS_FREEZE_PENDING_BMSK      0x1
#define SAMPSERV_DRX_QFT_DONE_PENDING_BMSK       0x2
#define SAMPSERV_DRX_PICH_DONE_PENDING_BMSK      0x4
#define SAMPSERV_DRX_FIRST_REACQ_DONE_PENDING_BMSK 0x8
#define SAMPSERV_DRX_SECOND_REACQ_DONE_PENDING_BMSK 0x10

extern rex_crit_sect_type   sampserv_mgr_lock;
extern rex_crit_sect_type   sampserv_mgr_mdsp_ops_lock;
extern rex_crit_sect_type   sampserv_mgr_client_notf_lock;
extern rex_crit_sect_type rf_slp_wkup_lock;
extern rex_crit_sect_type rf_div_sleep_lock;


rex_crit_sect_type   sampserv_mgr_client_lock;

#define SAMPSERV_MGR_MUTEXLOCK() REX_ISR_LOCK(&sampserv_mgr_lock)
#define SAMPSERV_MGR_MUTEXFREE() REX_ISR_UNLOCK(&sampserv_mgr_lock)

#define SAMPSERV_MGR_CLIENT_LOCK()   REX_ISR_LOCK(&sampserv_mgr_client_lock)
#define SAMPSERV_MGR_CLIENT_UNLOCK() REX_ISR_UNLOCK(&sampserv_mgr_client_lock)
#define SAMPSERV_MGR_RESET_OFFLINE_PICH_IN_PROGRESS() sampservmgrdb.drx_offline_pich_in_progress = FALSE
#define SAMPSERV_MGR_SET_OFFLINE_PICH_IN_PROGRESS() sampservmgrdb.drx_offline_pich_in_progress = TRUE
#define RF_DIV_SLEEP_MUTEXLOCK() REX_ISR_LOCK(&rf_div_sleep_lock)
#define RF_DIV_SLEEP_MUTEXFREE() REX_ISR_UNLOCK(&rf_div_sleep_lock)

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
#define SAMPSERV_MGR_RESET_SRCH_WAKEUP_FAILURE() sampservmgrdb.wkup_srch_failed = FALSE
#define SAMPSERV_MGR_SET_SRCH_WAKEUP_FAILURE() sampservmgrdb.wkup_srch_failed = TRUE
#endif

#define RF_SLP_WKUP_MUTEXLOCK() REX_ISR_LOCK(&rf_slp_wkup_lock)
#define RF_SLP_WKUP_MUTEXFREE() REX_ISR_UNLOCK(&rf_slp_wkup_lock)

extern rex_timer_type ssmgr_mdsp_rsp_timeout_timer;
/* Timer that expires 3.7 ms before the next entity waking up after
   go_to_sleep fails*/
extern rex_timer_type drx_entity_wbump_up_timer;
extern tlm_tfn_struct_type sccpch_start, sccpch_start_from_pich_st;

/*===========================================================================
                        DATA STRUCTURES
===========================================================================*/
typedef enum
{
  SS_FREEZE_CMD,
  SS_UNFREEZE_CMD,
  SS_CMD_NONE,
  NUM_SS_CMDS
} drx_offline_pich_ss_cmd_type;

typedef enum
{
  SRCH_SSMGR_CLIENT,
  DRX_SSMGR_CLIENT,
  TRIAGE_SSMGR_CLIENT,
  NUM_SSMGR_CLIENTS
} sampserv_mgr_client_type;


typedef enum
{
  SAMPSERV_PENDING_FROZEN,
  SAMPSERV_FROZEN,
  SAMPSERV_PENDING_UNFROZEN,
  SAMPSERV_UNFROZEN,
  SAMPSERV_INIT,
  NUM_SAMPSERV_STATES  
}sampserv_resource_state;

typedef struct
{
  /* 0 = SS frozen, 1 = SS unfrozen */
  boolean ss_status;
  /* 0= RF in LPM, 1 = RF out of LPM */
  boolean  rf_status;
} sampserv_mgr_resource_status_type;

typedef enum
{
  DRX_OUTER_LOOP_ADJ_OFFLINE_PICH_MODE,
  DRX_OUTER_LOOP_ADJ_ONLINE_PICH_MODE,
  DRX_OUTER_LOOP_ADJ_OFFLINE_RLP_PICH_MODE,
  DRX_OUTER_LOOP_ADJ_NUM_MODES
} drx_outer_loop_adj_mode_type;

typedef enum
{
  DRX_ONLINE_PICH_MODE,
  DRX_OFFLINE_PICH_MODE,
  DRX_OFFLINE_RLP_MODE,
  NUM_DRX_MODES
} drx_pich_processing_mode_type;

typedef void (*sampserv_mgr_local_cmd_post_cb_fn) (void);
typedef void (*drx_ss_ops_handler_type) (void);

/*------------------------------------------------------------------------------------------- 
                    VARIABLE DECLARATIONS
--------------------------------------------------------------------------------------------*/

extern boolean drx_offline_pich_ss_freeze_cmd_this_cycle;



typedef enum
{
  /* No communication established with DRX or DRX-SSMGR link reset - 00*/
  DRX_CLEAR_ALL_NOTIFICATION,
  /* DRX cmd received from RRC - 01*/
  DRX_NOTIFY_STANDBY_MODE_ENTRY_EVENT,
  /* DRX aborted, abort procedure completed and ext cmd processing initiated -02*/
  DRX_NOTIFY_STANDBY_MODE_EXIT_EVENT,
  /* SW has sent the SS freeze cmd to MDSP  - SS state goes to pending FROZEN , RF is still UP - 03*/
  DRX_NOTIFY_OFFLINE_PICH_START_EVENT,
  /* AGC tuning is complete. - 04*/
  DRX_NOTIFY_AGC_DONE_EVENT,
  /* SS freeze has occurred in FW  - only for setting SS state to FROZEN, RF goes to LPM -05*/
  DRX_NOTIFY_SS_FREEZE_EVENT,
  /* SS state goest to UNFROZEN, RF out of LPM - all searches can be allowed following this event-6 */
  DRX_NOTIFY_PCH_SCCPCH_START_EVENT,
  /* PICH decode done generic event -07*/ 
  DRX_NOTIFY_PICH_DONE_EVENT,
  /* PICH decode done, no PICH ack, so all searches can be allowed -08*/
  DRX_NOTIFY_PICH_NACK_EVENT,
  /* QXT done event, triggered by QXT done interrupt from FW -09*/
  DRX_NOTIFY_QXT_DONE,
  /* DRX SW triggered SS unfreeze, and RF out of LPM -10*/
  DRX_NOTIFY_SS_UNFREEZE_RF_OUT_OF_LPM_CMD,
  /* Notify that sleep will be delayed. -11*/
  DRX_NOTIFY_DELAY_PICH_TO_SLEEP_EVENT,
  /* This is the case when SS freeze cmd is not yet done when
  ** we want to drop PICH entity and deconfigure cells after
  ** PICH done isr. -12*/
  DRX_NOTIFY_DELAY_PICH_ENTITY_SRCH_TO_SLEEP,
  /* FW/RF sleep complete -13 */
  DRX_NOTIFY_GO_TO_SLEEP_EVENT,
  /* Update meas rules is done -14*/
  DRX_UPD_MEAS_RULES_DONE,
  /* First reacq srch done -15*/
  DRX_NOTIFY_FIRST_REACQ_DONE,
  /* DRX DL to sleep start -16*/
  DRX_NOTIFY_READY_TO_START_DRX_DL_SLEEP_EVENT,
  DRX_NUM_NOTIFY_EVENTS
}drx_ssmgr_notify_evt_type;

typedef void (*drx_notification_post_cb_type)(void);

typedef struct
{
  drx_ssmgr_notify_evt_type drx_notification_event;
  drx_notification_post_cb_type drx_notification_post_cb;

  uint32 drx_offl_pich_st_ss_frz_frc_act_time;
  boolean drx_offl_pich_st_rf_in_lpm;
}sampserv_mgr_drx_intf_type;

typedef struct
{
  boolean status;
}sampserv_mgr_triage_intf_type;

typedef enum
{
  SRCH_SSMGR_START,
  SRCH_SSMGR_COMPLETE,
  SRCH_SSMGR_ABORT,
  NUM_SRCH_SSMGR_OPS
}ssmgr_srch_op_type;

typedef struct
{
/* Searcher should set this flag at every samp serv mgr communication */
boolean srch_intf_valid;
ssmgr_srch_op_type  ssmgr_srch_op;

/* Operation type:  SRCH_SSMGR_START */
/* The flags below should mirror the values that search intends to send
** to FW for the search operation
*/
boolean srchusefreezeflag;
uint16 srchssfreezeflag;
boolean srchstoprfflag;

/* Handlers */
void (*ss_ops_done_handler) (void);
void (*cg_agc_done_handler) (void);

/* This flag needs to be set if srch ensures that some other mechanism
** such as rfm_tune_to_chan(), will take care of bringing rf out
** of LPM. If flag is set, SS mgr will avoid bringing RF out of LPM
** although that is the pre-search operation requirement
*/
boolean override_rf_out_lpm_op; 

srchzz_substate_enum_type srch_op;

}sampserv_mgr_srch_intf_type;

typedef struct
{ 
  /* Client requesting operation */  
  sampserv_mgr_client_type sampserv_mgr_client;
  /* Searcher interface */
  sampserv_mgr_srch_intf_type sampserv_srch_intf;
  /* DRX interface */
  sampserv_mgr_drx_intf_type sampserv_drx_intf;
  /* Triage interface */
  sampserv_mgr_triage_intf_type sampserv_triage_intf;
} sampserv_mgr_intf_type;

extern boolean sampserv_mgr_notify(sampserv_mgr_intf_type* sampserv_mgr_notification);

extern boolean sampserv_mgr_query_freeze_status(boolean check_for_pend_frozen);

extern uint32 ss_freeze_action_time;

typedef enum
{
  SAMPSERV_SSMGR_FORCE_UNFREEZE,
  SAMPSERV_SSMGR_NO_ACTION,
  SAMPSERV_SSMGR_NO_ACTION_ONLINE_MODE,
  SAMPSERV_SSMGR_QXT_DONE_EVT_NOTIFY,
  SAMPSERV_SSMGR_QXT_DONE_EVT_NOTIFY_ONLINE_MODE,
  SAMPSERV_SSMGR_PICH_DONE_EVT_NOTIFY,
  SAMPSERV_SSMGR_PICH_DONE_EVT_NOTIFY_ONLINE_MODE,
  SAMPSERV_SSMGR_PICH_NACK_EVT_NOTIFY,
  SAMPSERV_MGR_NUM_CMD_ACTION,
  DRX_COEX_PRIORITY_LOW,
  DRX_COEX_PRIORITY_HIGH,
  SAMPSERV_SSMGR_NUM_ACTIONS
} sampserv_mgr_cmd_action_type;


typedef struct
{
 l1_local_cmd_hdr_type local_cmd_hdr;
 sampserv_mgr_client_type client;
 sampserv_mgr_cmd_action_type action;    
 sampserv_mgr_local_cmd_post_cb_fn post_cb;
}sampserv_mgr_cmd_type;


#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
typedef enum
{
  PRIORITY_LOW,
  PRIORITY_HIGH
}drx_priority_level_with_coexmgr;

#endif

extern void sampservmgr_process_local_cmd(sampserv_mgr_cmd_type* cmd_ptr);

extern boolean sampserv_mgr_is_ss_frozen(void);

extern void ssmgr_notify_rf_tune( sampserv_mgr_client_type client,uint16 freq);

extern void sampserv_mgr_set_notification_pending(sampserv_mgr_client_type,uint32 bmsk);

extern void sampserv_mgr_clear_notification_pending(sampserv_mgr_client_type,uint32 bmsk);

extern uint32 sampserv_mgr_get_notifications_pending_for_pich_drop(void);

extern void ssmgr_mdsp_rsp_timeout_handler(void);

extern boolean drx_offline_pich_ss_freeze_cmd_sent(void);

extern void ssmgr_mdsp_rsp_agc_tune_timeout_handler(void);

extern boolean sampserv_mgr_nv_enabled(void);

extern void sampserv_mgr_notify_wakeup_isr_entry(void);

extern void sampservmgr_post_local_cmd(sampserv_mgr_client_type client,sampserv_mgr_local_cmd_post_cb_fn post_cb,sampserv_mgr_cmd_action_type action);

extern void sampserv_mgr_notify_status_operational(void);

extern boolean sampserv_mgr_query_ss_unfreeze_status(void);

extern void drx_early_rf_shutdown_handler(void);

extern void drx_init_sampserv_state(void);

extern void drx_disable_other_opts(void);

void drx_clr_early_rf_shutdown_bmsk(drx_ssmgr_notify_evt_type drx_ssmgr_notf_evt);

/*===========================================================================
FUNCTION drx_offline_pich_send_sample_server_freeze_cmd

DESCRIPTION:
  This function sends sample server freeze command to MDSP sync i\f.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void drx_offline_pich_send_sample_server_freeze_cmd
(
  /* Indicates whether to apply freeze/unfreeze action to chain 0 */
  boolean chain0,
  /* Indicates whether to apply freeze/unfreeze action to chain 1 */
  boolean chain1,
  /* Indicates the action to be taken with RTR after sample server cmd action time.*/
  boolean rfPwrUpdateFlag,
  /* Indicates the STMR Cx1 value for the action time for the freeze/unfreeze.*/
  uint32 actionTime
);

typedef void  (*srch_ss_ops_handler_type) (void);
typedef void  (*srch_rf_tune_done_handler_type) (void);

typedef struct
{
  /* Common data */
  sampserv_mgr_client_type curr_ssmgr_client;
  sampserv_mgr_client_type prev_ssmgr_client;
  boolean pending_rf_state_update;
  boolean pending_rf_state;
  boolean pending_ss_state_update;
  sampserv_resource_state pending_ss_state;
  sampserv_resource_state current_state;
  sampserv_resource_state prev_state;
  sampserv_resource_state pending_next_state;
  boolean ssmgr_operational;
  boolean ssmgr_deact_pending;
  boolean ssmgr_force_unfreeze_pending;
  boolean ssmgr_wait_for_agc_tune_done;
  uint32 ssmgr_mdsp_ops_pending_bmsk;

  /* DRX data */
  drx_ssmgr_notify_evt_type current_notify_event;
  drx_ssmgr_notify_evt_type prev_notify_event;
  drx_notification_post_cb_type drx_pich_entity_to_sleep_cb;
  drx_notification_post_cb_type drx_pich_entity_srch_to_sleep_cb;  
  boolean drx_pich_entity_to_sleep_cb_pending;
  boolean drx_pich_entity_srch_to_sleep_cb_pending;

  boolean drx_rf_mgr_ss_freeeze_evt_notify;

  uint32  drx_notifications_pending_for_pich_drop_bmsk;

  /* Flag gets set to TRUE at DRX_NOTIFY_OFFLINE_PICH_START_EVENT
  ** Flag get cleared (set to FALSE) at either DRX_NOTIFY_PICH_NACK_EVENT 
  ** or at DRX_NOTIFY_SCCPCH_START_EVENT
  */
  boolean drx_offline_pich_in_progress;
  /* Search data */
  boolean rfstopflag;
  boolean rf_state_upd_at_srch_done_reqd;
  boolean rf_state;
  sampserv_resource_state final_state;
  boolean ssrv_in_transition;
  boolean srch_in_progress;
  boolean override_rf_out_lpm_op;
  srchzz_substate_enum_type completed_srch_type;
  srchzz_substate_enum_type started_srch_type;
  srch_ss_ops_handler_type srch_ss_ops_handler_cb;
  srch_rf_tune_done_handler_type srch_rf_tune_done_handler_cb;

  sampserv_mgr_srch_intf_type prev_srch_intf_notf;
  #ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
  boolean wkup_srch_failed;
  #endif
}ssmgr_db_type;
  

extern ssmgr_db_type sampservmgrdb;

extern rex_crit_sect_type drx_rf_mgr_lock;

extern boolean wl1_is_rf_awake(void);

extern void drx_set_early_rf_shutdown_bmsk(drx_pich_processing_mode_type drx_pich_proc_mode);

extern void drx_perform_offline_pich_eval
(
  /* Indicates whether to apply freeze/unfreeze action to chain 0 */
  boolean chain0,
  /* Indicates whether to apply freeze/unfreeze action to chain 1 */
  boolean chain1,
  /* Indicates the action to be taken with RTR after sample server cmd action time.*/
  boolean rfPwrUpdateFlag,
  /* Indicates the STMR Cx1 value for the action time for the freeze/unfreeze.*/
  uint32 actionTime
);

extern void drx_disable_early_rf_shutdown(void);

extern void drx_disable_cell_fing_decfg_opt(void);

extern void drx_offline_pich_logging(void);
/*===========================================================================
FUNCTION     DRX_IS_OFFLINE_PICH_IN_PROGRESS

DESCRIPTION
  Returns the status of the offline pich
  
PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE - When Offline PICH in progress
  FALSE - When PICH ACK/NACK is recevied

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean drx_is_offline_pich_in_progress(void);

/*===========================================================================
FUNCTION     WL1_IS_RF_DIV_AWAKE

DESCRIPTION
  This is the wrapper function to get the RF state for div chain.
  
PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_is_rf_div_awake(void);

/*===========================================================================
FUNCTION     DRX_SAMPSERV_LAST_SRCH_PERFORMED

DESCRIPTION
  Returns the last search that is performed through drxofflineprocmgr 
  
PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Type of search that is performed

SIDE EFFECTS
  None.
===========================================================================*/
srchzz_substate_enum_type drx_sampserv_last_srch_performed(void);

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
/*===========================================================================
FUNCTION     WL1_DSDA_TOGGLE_PRIORITY

DESCRIPTION
  This is the wrapper function to inform co-ex manager about the change in W's priority level for RF usage.
   Event gets triggered to higher the priority of WFW_COEX_ACTIVITY_SEARCHER in order to make sure G blanks during entire duration of PICH decode 
   this will ensure that the samples captured by WFW are clean during the PICH decode.
   Event is triggered to lower the priority of WFW_COEX_ACTIVITY_SEARCHER after PICH has been decoded, in order to make sure G does not have to blank
   unnecessarily while W is in sleep.
  
PARAMETERS
  wl1_cxm_evt_type evt_type

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_coexmgr_toggle_priority(drx_priority_level_with_coexmgr level, boolean drx_priority_called_from_second_reacq_done);

/*===========================================================================
FUNCTION     DRX_SET_TIMER_FOR_W_BUMPUP

DESCRIPTION
  Function called only in case go_to_sleep fails. A new timer is set that expires 3.7 ms
  before the next entity waking up. This gives UE enough time to Bump up W priority.
  
  3.5 slots for coex manager delay + 2 slots for reacq search samples =  5.5 slots
  (3.7 ms) before reacq search command is sent to FW
 
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None
===========================================================================*/
void drx_set_timer_for_w_bumpup(void);

/*===========================================================================
FUNCTION     WL1_BUMP_UP_PRIORITY_ON_TIMER_EX

DESCRIPTION
  Wrapper function that sets the W priority on timer expiry
 
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None
===========================================================================*/
extern void wl1_bump_up_priority_on_timer_ex(uint32 unused);

#endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */
/*===========================================================================
FUNCTION     DRX_DISABLE_SIMPLE_OPTS_THRU_NV5458

DESCRIPTION
  Function used to disable simple optimizations if NV 5458 is set to 0x18
  Opts to be disabled are:
  1. decrease the samples by 2 bgps
  2. Increase the early wakeup threshold by 18 bpgs
 
PARAMETERS
  

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
extern void drx_disable_simple_opts_thru_nv5458(void);
#ifdef FEATURE_QSH_DUMP
extern void wl1_drxofflineproc_get_debug_info(wl1_drx_sleep_mini_debug_info_struct_type* obj);
#endif

#ifdef FEATURE_WCDMA_REACQLESS_PICH
/*===========================================================================
FUNCTION     drx_adjust_fw_rssi_for_valid_pi_bits

DESCRIPTION
   This is a wrapper function to call SS unfreeze and RF out of LPM.
 
PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 drx_adjust_fw_rssi_for_valid_pi_bits(uint8 drx_num_valid_bpgs);

/*===========================================================================
FUNCTION     DRX_ADJUST_FING_SRCH_POS_BIAS

DESCRIPTION
   When RLP is enabled, in all Parallel reacq cycles FW CPICH energy
   is saved as a reference to RLP cycles. As with RLP parallel reacq 
   finger assignment will be based on srch results which we got before
   4 DRX cycles, srch pos might have changed but finger are still tracking
   drifted pos so we need to adjust below bias based on diff b/w srch pos in
   current cycles and stronges fing pos.
 
PARAMETERS
  cell_db_idx - cell_db index used for getting pos of strongest finger.

DEPENDENCIES
  None

RETURN VALUE
  ret_val - energy bias in dBx4 units.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 drx_adjust_fing_srch_pos_bias(uint8 cell_db_idx);
#endif
extern uint8 drx_early_rf_srches_chk(void);
#endif 
