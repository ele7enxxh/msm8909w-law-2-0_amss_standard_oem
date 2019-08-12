#ifndef WL1IDLETAMGR_H
#define WL1IDLETAMGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               WL1 IDLE MODE TUNEAWAY MANAGER

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2007 by Qualcomm Technologies, Inc. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1idletamgr.h#1 $ 
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/24/15   ap      Initial check-in for WL1 QSH framework 
09/15/15   bj      Split ACQ changes for W+1x.
02/23/15   gm      To avoid the race condition between wrm_release() and prio_inv_timer expiry.
02/20/15   skk     Trigger DL WEAK IND if L1 doesnt get lock for 2 sec during cell selectoin.
11/25/14   skk     Set sleep time for BCCH in no lock cases following a geometric progression.
12/01/14   gm      Modify the return type for lock req API
11/19/14   rs      Code changes for requesting lock with new proc during BCH decode
10/21/14   kr/skk  DSDS:call rf tune to multichain in gts fail case
09/10/14   skk     If sleep time is invalid in inter f resel, make it 4.5ms.
08/20/14   skk     Ignore pended cmd if UE is paged when G is in data call.
07/28/14    skk     Handle pended ext cmd by setting ext_cmd_sig again.
04/07/14    rs     API to clear wl1idle_ta_mgr_wrm_rf_tune_needed flag 
05/06/14   as      Moving WRM to WL1TRM.
03/06/14   gsk     Ignore PICH wakeups during interf reselection
02/28/14   skk     Handle the lock_not_rel timer expiry in WL1 task context.
12/03/13   skk     Dont pend any ext cmd in G2W_TA_ENTER state.
12/16/13   as      Porting G2W TA from Triton to Dime. 
09/30/13   pv      Enable all G2W TA components
09/15/13   pv      G2W TA idle mode framework     
=============================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "wl1drxmanager.h"
#include "wl1drxentity.h"
#include "wl1multisim.h"
#if defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC)
#include "wl1_qsh_ext.h"
#endif /* defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC) */

#ifdef FEATURE_DUAL_SIM

/*===========================================================================
                     ENUMS AND MACROS
===========================================================================*/
typedef enum
{
  SRCH_W,
  SRCH_GSM,
  SRCH_LTE
} srch_type;

typedef struct
{
  uint16 freq;    /* Will have valid value only for non-IRAT searches.*/
  srch_type srch_due_next_gap; 
} drx_next_srch_info_type;

/* This will be called by DRX for deciding on the next gap.*/
boolean srchzz_any_srches_due(drx_next_srch_info_type*);


/* DRX sleep modes
** DRX_SLEEP_REGULAR_MODE - normal mode, true sleep, FW/RF sleep 
                            MCPM call and sleepctl programming.
** DRX_SLEEP_G2W_TA_MODE  - G2W TA mode, no true sleep, no RF control.
*/
typedef enum
{
  DRX_SLEEP_REGULAR_MODE,
  DRX_SLEEP_TA_MODE,
  DRX_NUM_SLEEP_MODES
} drx_sleep_mode_type;

/* DRX G2W TA states
** G2W_TA_INACTIVE 				    - G2W TA inactive.
** G2W_TA_ASLEEP   				    - G2W TA mode active but no activity.
** G2W_TA_WAITING_ON_RESOURCE_READY - Awake but waiting on resources to be ready. 
                                      WL1TRM did not inform resource ready yet.
									  We can enter this only after we close a gap.
** G2W_TA_CONFIGURED                - Awake and all resources ready. 
*/
typedef enum
{
  DRX_G2W_TA_INACTIVE,
  DRX_G2W_TA_ENTER,
  DRX_G2W_TA_WAITING_ON_RESOURCE_READY,
  DRX_G2W_TA_CONFIGURED,
  DRX_G2W_TA_LOCK_REL_OFFLINE_ACTIVITY_PENDING,
  DRX_G2W_TA_EXIT,
  DRX_G2W_TA_NUM_STATES
} drx_g2w_ta_state_type;

/* ENUMs for various notifications that can be sent to WL1IDLE_TA_MGR.*/
typedef enum
{
  IDLE_TA_MGR_NO_RESOURCE_NOTF,
  IDLE_TA_MGR_EXT_CMD_PENDING_NOTF,
  IDLE_TA_MGR_STEP1_DONE_OFFLINE_NASTT_PENDING_NOTF,
  IDLE_TA_MGR_NUM_NOTF
} wl1idle_ta_mgr_notf_type;

typedef struct
{
   boolean wrm_g2w_ta_prio_inversion_in_progress;
   boolean wrm_g2w_ta_block_request_and_notify;
   boolean wrm_prio_inv_tmr_set_for_split_acq;
   boolean wrm_idle_oos_prio_inv_in_progress;
   uint32 wrm_g2w_ta_l1_trm_resrv_sclk;
   trm_duration_t wrm_split_acq_saved_duration;

}wl1idle_ta_mgr_wrm_db_type;

/* G2W TA database structure.*/
typedef struct
{
   boolean lock_status;
   boolean ext_cmd_pended;
   uint32  num_g2w_ta_gaps;
   uint32  total_g2w_ta_gaps;
   wl1idle_ta_mgr_wrm_db_type wrm_db;

} wl1idle_ta_mgr_db_type;

typedef enum
{
  WL1IDLE_TA_MGR_EXT_CMD_NOT_APPLICABLE,
  WL1IDLE_TA_MGR_EXT_CMD_LOCK_WITH_GPRS_LTA_G2W_TA_DRX_END, // DRX abort condition in PCH_SLEEP
  WL1IDLE_TA_MGR_EXT_CMD_LOCK_NOT_REQD_G2W_TA_CONT, // Do not abort in PCH_SLEEP condition
  WL1IDLE_TA_MGR_EXT_CMD_LOCK_WITH_GPRS_LTA,
  WL1IDLE_TA_MGR_EXT_CMD_LOCK,
  WL1IDLE_TA_MGR_EXT_CMD_LOCK_NOT_REQD,
  WL1IDLE_TA_MGR_EXT_CMD_LOCK_NOT_REQD_G2W_TA_DRX_END, // DRX abort condition in PCH_SLEEP
  WL1IDLE_TA_MGR_EXT_CMD_LOCK_NOT_REQD_G2W_TA_END,
  WL1IDLE_TA_MGR_EXT_CMD_PROC_DEFAULT,
  NUM_MAX_WL1IDLE_TA_MGR_EXT_CMD_CATEGORY
} wl1idle_ta_mgr_ext_cmd_category_enum_type;

extern wl1idle_ta_mgr_db_type wl1_g2w_ta_db;

/* Variable to track G2W TA state.*/
extern drx_g2w_ta_state_type drx_g2w_ta_state;

/* Variable to track dRX sleep mode*/
extern drx_sleep_mode_type drx_sleep_mode;

/* Variable used to indicate to WRM that RF tune is needed when
** handling external cmd that can open a new gap.*/
extern boolean wl1idle_ta_mgr_wrm_rf_tune_needed;

/* Timer to track if we kept lock for long time.*/
extern rex_timer_type wl1idle_ta_mgr_lock_no_rel_tmr;

extern boolean wl1idle_ta_mgr_determine_g2w_ta_mode(void);

/* Priority inversion time prior to attempting search or frequency scan */
#define WRM_G2W_TA_PRIORITY_INVERSION_TIME_MS 750

/* Priority inversion time employed in the middle of split search */
#define WRM_G2W_TA_PRIORITY_INVERSION_TIME_ACQUISTION_MS 160

/* Macro to check if WL1 is operating in G2W TA mode.*/
#define WL1_IN_G2W_TA_MODE() ((WL1_IN_DS_MODE()) && \
                              (drx_sleep_mode == DRX_SLEEP_TA_MODE) && \
							  (drx_g2w_ta_state != DRX_G2W_TA_INACTIVE)\
							 )

/* Macro to check if Split search mode is allowed if we are in G2W TA mode.*/
#define WL1IDLE_TA_MGR_SPLIT_SRCH_ALLOWED() (!wl1trm_is_ue_in_dr_mode() && \
                                             WL1_IN_G2W_TA_MODE()      && \
                                            ((get_g2w_ta_state() == DRX_G2W_TA_CONFIGURED) || \
                                             (get_g2w_ta_state() == DRX_G2W_TA_LOCK_REL_OFFLINE_ACTIVITY_PENDING) || \
                                              (get_g2w_ta_state() == DRX_G2W_TA_ENTER)\
                                             )\
                                            )

/* Macro that updates if we got lock in G2W TA mode.*/
#define WL1_G2W_TA_SET_LOCK_STATUS()	\
do { 									\
  wl1_g2w_ta_db.lock_status = TRUE; 	\
} while(0)   


/* Macro that updates if we did not get lock in G2W TA mode.*/
#define WL1_G2W_TA_SET_NO_LOCK_STATUS()	\
do { 									\
  wl1_g2w_ta_db.lock_status = FALSE; 	\
} while(0)

/* Macro to track if an external cmd was pended in G2W TA mode.*/
#define WL1_IDLE_TA_MGR_IS_EXT_CMD_PENDED() 	   (wl1_g2w_ta_db.ext_cmd_pended == TRUE)

/* Macro to clear the external cmd pended status.*/
#define WLIDLE_TA_MGR_CLR_EXT_CMD_PENDING_STATUS() \
do{\
   wl1_g2w_ta_db.ext_cmd_pended = FALSE;\
}while(0)


/* Macro to check if we are in G2W TA mode with no lock.*/
#define WL1_IN_G2W_TA_MODE_WITH_NO_LOCK() (WL1_IN_G2W_TA_MODE() && (wl1_g2w_ta_db.lock_status == FALSE))

/* Macro that returns num of G2W TA gaps.*/
#define WL1_IDLE_TA_MGR_INC_NUM_TA_GAPS() \
do {\
	wl1_g2w_ta_db.num_g2w_ta_gaps ++;\
} while (0)

/* Macro to reset no.of G2W TA gaps.*/
#define WL1_IDLE_TA_MGR_RESET_NUM_TA_GAPS() \
do {\
	wl1_g2w_ta_db.num_g2w_ta_gaps = 0;\
} while (0)

/* Macro to increment total no.of G2W TA gaps.*/
#define WL1_IDLE_TA_MGR_INC_TOTAL_TA_GAPS() \
do {\
	wl1_g2w_ta_db.total_g2w_ta_gaps ++;\
} while (0)

/* Macro to get the no.of G2W TA gaps.*/
#define WL1_IDLE_TA_MGR_GET_NUM_TA_GAPS() (wl1_g2w_ta_db.num_g2w_ta_gaps)

/* Macro to check if lock was released and Offline activity happening.*/
#define WL1_IDLE_TA_MGR_ALLOW_FW_SLEEP_AFTER_OFFLINE_SRCH() (drx_g2w_ta_state == DRX_G2W_TA_LOCK_REL_OFFLINE_ACTIVITY_PENDING)

/*To Clear all priority inversion variables.*/
#define WL1_IDLE_TA_MGR_CLEAR_PRIORITY_INV_VARS() \
do { \
     (void) rex_clr_timer(&wrm_g2w_ta_priority_inversion_timer);         \
     wl1_g2w_ta_db.wrm_db.wrm_g2w_ta_prio_inversion_in_progress = FALSE; \
     wl1_g2w_ta_db.wrm_db.wrm_g2w_ta_block_request_and_notify = FALSE;   \
     wl1_g2w_ta_db.wrm_db.wrm_idle_oos_prio_inv_in_progress = FALSE;     \
     wl1_g2w_ta_db.wrm_db.wrm_prio_inv_tmr_set_for_split_acq = FALSE;    \
}while (0)

#define WL1IDLE_TA_CLR_NO_LOCK_TIMER() \
do{ \
    (void) rex_clr_timer(&l1_no_lock_timer); \
    (void) rex_clr_sigs(wcdma_l1_get_tcb(), L1_NO_LOCK_TIMER_SIG); \
    wl1_ds_paging_spec_info.trm_lock_consecutive_fail_cnt = 0; \
  }while(0)
/*=========================================================================== 
                              FUNCTION EXTERNS 
===========================================================================*/

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_CHK_G2W_TA_MODE_ENTRY

DESCRIPTION
  This function queries WL1TRM if G2W TA mode entry is needed.

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  boolean - TRUE if entry is needed, FALSE otherwise.
===========================================================================*/
extern boolean wl1idle_ta_mgr_chk_g2w_ta_mode_enter(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_CHK_G2W_TA_MODE_EXIT

DESCRIPTION
  This function queries WL1TRM if G2W TA mode exit is needed.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if exit is needed, FALSE otherwise.
===========================================================================*/
extern boolean wl1idle_ta_mgr_chk_g2w_ta_mode_exit(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_CONFIG_NEXT_WAKEUP

DESCRIPTION
  This function will reserve the resources needed for the next TA gap.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern boolean wl1idle_ta_mgr_config_next_wakeup(drx_entity_handle_type handle,
						boolean *regular_mode_switch);


extern void wl1idle_ta_mgr_check_schedule(drx_entity_handle_type entity_woken_up);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_G2W_TA_CONFIG_RF_FW_AT_WAKEUP

DESCRIPTION
  This function brings up FW and RF at the start of the gap in G2W TA mode.

DEPENDENCIES
  None.
 
PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_g2w_ta_config_rf_fw_at_wakeup(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_G2W_TA_CONFIG_RESOURCES

DESCRIPTION
  This function configures resources needed for a G2W TA gap.
 
DEPENDENCIES
  None.

PARAMETERS
  Pointer to a variable to declare resource availability.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_g2w_ta_config_resources(boolean *res_avail, drx_entity_handle_type handle_woken_up);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_MODIFY

DESCRIPTION
  This function is used by other modules to notify WL1IDLE_TA_MGR.
 
DEPENDENCIES
  None.

PARAMETERS
  Type of notification, DRX ENTITY handle.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_notify(wl1idle_ta_mgr_notf_type notf,drx_entity_handle_type handle);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_G2W_TA_RESOURCE_NEEDED

DESCRIPTION
  This function checks if we need resources for G2W TA gap.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Boolean - TRUE if we need resource, FALSE otherwise.
===========================================================================*/
extern boolean wl1idle_ta_mgr_g2w_ta_resource_needed(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_CHK_TA_MODE_OPERATION

DESCRIPTION
  This function checks if we can continue with TA mode operation.If we did not get
  a lock, we will have to put the entity woken up.
 
DEPENDENCIES
  None.

PARAMETERS
  DRX ENTITY handle woken up.

RETURN VALUE
  Boolean - TRUE if TA mode operation allowed, FALSE otherwise.
===========================================================================*/
extern boolean wl1idle_ta_mgr_chk_ta_mode_operation(drx_entity_handle_type handle);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_PREP_FOR_EXT_CMD_HANDLE

DESCRIPTION
  This function is called by WRM whenever G2W TA mode active and we get an
  external cmd.
 
DEPENDENCIES
  None.

PARAMETERS
  The WRM ext cmd category.

RETURN VALUE
  Boolean - TRUE if need to pend WRM to handle ext cmd, FALSE otherwise.
===========================================================================*/
extern boolean wl1idle_ta_mgr_prep_for_ext_cmd_handle(wl1idle_ta_mgr_ext_cmd_category_enum_type wrm_g2w_ta_mode_curr_ext_cmd_category);

/*===========================================================================
FUNCTION    WL1_G2W_TA_ENTER_SLEEP

DESCRIPTION
  This function is called when entering sleep and reserves TRM lock for the next
  activity at a specified time.

DEPENDENCIES
  This API will be called only when are in G2W TA mode.

PARAMETERS
  DRX ENTITY handle waking up next, next wakeup time in ms.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_g2w_ta_enter_sleep(drx_entity_handle_type handle_waking_up,int32 wkup_time_usec);

extern boolean wl1idle_ta_mgr_allow_sleep_all_entities_asleep(void);


/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_DETERMINE_MIN_SLEEP_TIME_USEC

DESCRIPTION
  This function determines the min sleep time for passing to DRX MANAGER.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  uint32 - Min sleep time for DRX MANAGER to block sleep.
===========================================================================*/
extern uint32 wl1idle_ta_mgr_determine_min_sleep_time_usec(void);

extern void wl1idle_ta_mgr_wake_other_entities(drx_entity_handle_type entity_dropped);

extern void wl1idle_ta_mgr_set_curr_rf_freq(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_SET_RF_WARMUP_STATUS

DESCRIPTION
  This function sets the RF warmup status in G2W TA mode.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Boolean - TRUE if sleep can be allowed, FALSE otherwise.
===========================================================================*/
extern void wl1idle_ta_mgr_set_rf_warmup_status(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_CLR_RF_WARMUP_STATUS

DESCRIPTION
  This function clears the RF warmup status.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Boolean - TRUE if sleep can be allowed, FALSE otherwise.
===========================================================================*/
extern void wl1idle_ta_mgr_clr_rf_warmup_status(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_ADJ_PICH_WAKEUP_TIME

DESCRIPTION
  This function adjusts PICH wakeup time if needed in G2W TA mode.
 
DEPENDENCIES
  None.

PARAMETERS
  Pointer to the target time for wakeup.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_adj_pich_wakeup_time(int32 *target_time);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_REL_RESOURCES_POST_OFFLINE_SRCH

DESCRIPTION
  This function takes all necessary actions post an Offline search.
  Puts FW to sleep and SUSPEND and resets variables.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_rel_resources_post_offline_srch(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_REL_RESOURCES_FOR_OFFLINE_SRCH

DESCRIPTION
  This function puts RF to sleep and releases the TRM lock since only Offline
  search is pending in this gap.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_rel_resources_pre_offline_srch(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_WAIT_FOR_FW_RSP

DESCRIPTION
  This function posts a local cmd when cell/fing decfg is done after ACQ success.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_wait_for_fw_rsp(void);

extern drx_g2w_ta_state_type get_g2w_ta_state(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_G2W_TA_WRM_RESERVE_PENDING

DESCRIPTION
  This function returns if a TRM reservation for lock is pending within the specified
  number of frames.(1 frame = 10ms).

DEPENDENCIES
  None.

PARAMETERS
  uint32 reserve_chk_frames

RETURN VALUE
  Boolean.
===========================================================================*/
extern boolean wl1idle_ta_mgr_g2w_ta_wrm_reserve_pending(uint32 reserve_time_chk_frames);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_LOCK_NOT_REL_TMR_EXPIRED_CB

DESCRIPTION
  This is the callback for the timer expiry when we keep lock for long time,
  specified by WL1IDLE_TA_MGR_NO_LOCK_REL_TIMER_VAL_MS.

DEPENDENCIES
  None.

PARAMETERS
  uint32 but unused.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_lock_not_rel_tmr_expired_cb(uint32 unused);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_LOCK_NOT_REL_TMR_EXPIRED_CB_HANDLER

DESCRIPTION
  This function is called in as a part of local cmd handling of
   the lock_not_rel_timer expiry

DEPENDENCIES
  None.

PARAMETERS
None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_lock_not_rel_tmr_expired_cb_handler(void);

/*===========================================================================
FUNCTION    WL1IDLE_SET_G2W_TA_OOS_TIME

DESCRIPTION
  This function set the OOS time based on the mode. If we are in G2W TA mode 
  we expect atleast one gap to be opened for inter/intra f search so we double
  the oos time as 24s, and GSM neighbours are present we might open a new gap 
  for GSM meas so it was made to 36s.

DEPENDENCIES
  None.

PARAMETERS
None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_set_g2w_ta_oos_time(void);

/*===========================================================================
FUNCTION    WL1IDLE_G2W_TA_REL

DESCRIPTION
  This function is called from drx_rel() whenever we are moving out
  of PCH_SLEEP. 

DEPENDENCIES
  None.

PARAMETERS
None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_g2w_ta_rel(void);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_RESET_G2W_TUNE_NEEDED

DESCRIPTION
  This function is called to reset RF tune needed flag. It can be called when
  transitioning from G2WTA mode to regular mode.
  Makes sure any external cmd handled in the first cycle after exiting g2wta mode
  will be handled the regular way of programming sleep cntrl.
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_reset_g2w_tune_needed(void);

/*===========================================================================
FUNCTION     WL1IDLE_TA_MGR_RELEASE_TRM_SPLIT_ACQ

DESCRIPTION
  This function is used by searcher to release TRM lock in split acq mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_release_trm_split_acq(wrm_primary_client_enum_type wrm_client_id);

/*===========================================================================
FUNCTION     WL1IDLE_TA_MGR_RELEASE_TRM

DESCRIPTION
  This function is used by WL1 idle mode scheduler to release lock for G2W TA

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_release_trm(wrm_primary_client_enum_type wrm_client_id);

/*===========================================================================
FUNCTION     WRM_G2W_TA_PRIO_INV_TMR_EXPIRED_CB

DESCRIPTION 
  This will do a req_notify with actual ACQ priority after priority
  inversion timer expires. WRM modifies the priority with which TRM
  lock is requested in G2W TA mode. After priority inversion timer
  expires WRM will do a req_notify with actual TRM priority if we
  are in good state.

DEPENDENCIES
  None.

PARAMETERS 
  uint32 unused.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_prio_inv_tmr_expired_cb(uint32 unused);

/*===========================================================================
FUNCTION    WL1IDLE_TA_MGR_REQ_AND_NOTIFY_FOR_LOCK

DESCRIPTION
  Is used to request and notify for TRM lock in G2W TA mode. 

DEPENDENCIES
  None.

PARAMETERS
  client - WRM client type which is requesting for lock
  activity - Type of activity for which lock is needed
  duration - How long the resource will be needed for (in sclks)
  grant_callback - Callback to notify client when resource is granted
  band_info -  Band info
  wrm_prio_inv_req_n_not- to indicate that the req_n_not is getting called 
                          due to priority_inversion_timer expiry

RETURN VALUE
  Boolean - FALSE - lock failed , TRUE - otherwise
===========================================================================*/
extern boolean wl1idle_ta_mgr_req_and_notify_for_lock(
  /* The client which needs the RF resource */
  wrm_primary_client_enum_type wrm_client_id,

  wrm_activity_enum_type activity,

/* How long the resource will be needed for (in sclks) */
  trm_duration_t                  duration,

  /* Callback to notify client when resource is granted */
  wrm_grant_callback_type            grant_callback,

  /* Band info */
  wrm_freq_input_type band_info,

  /* Used to indicate that the req_n_not is getting called due to priority_inversion_timer expiry */
  boolean                         wrm_prio_inv_req_n_not
);

/*===========================================================================
FUNCTION     WRM_REQUEST_AND_NOTIFY_FOR_SPLIT_ACQ

DESCRIPTION
  This function is used by WCDMA to request lock for split acq

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_req_and_notify_for_split_acq(
  /* How long the resource will be needed for (in sclks) */
  trm_duration_t                  duration, // change to milliseconds 

  /* Callback to notify client when resource is granted */
  wrm_srch_lock_grant_cb_type            grant_callback
);

/*===========================================================================
FUNCTION  WRM_G2W_LTA_PROCESS_EXT_MCD

DESCRIPTION
 Performs categorization of external cmds that are sent to WL1

DEPENDENCIES
  To be used only in G2W TA mode.

RETURN VALUE
  Category type

SIDE EFFECTS
  None.
===========================================================================*/
extern wl1idle_ta_mgr_ext_cmd_category_enum_type wl1idle_ta_mgr_process_ext_cmd(l1_ext_cmd_type *ext_cmd_ptr);

/*===========================================================================
FUNCTION  WL1IDLE_TA_MGR_RESUME_EXT_CMD_PROCESSING

DESCRIPTION
 Signals WL1 to resume external cmd processing, once DRX is in the correct
 state.

DEPENDENCIES
  To be used only in G2W TA mode.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1idle_ta_mgr_resume_ext_cmd_processing(void);
#ifdef FEATURE_QSH_DUMP
extern void wl1_idletamgr_get_debug_info(wl1_drx_sleep_mini_debug_info_struct_type* obj);
#endif /* FEATURE_QSH_DUMP */
#endif /* FEATURE_DUAL_SIM */
#endif /* WL1IDLETAMGR_H */