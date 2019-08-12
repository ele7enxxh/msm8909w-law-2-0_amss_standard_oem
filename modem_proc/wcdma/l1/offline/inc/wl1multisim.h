#ifndef WL1MULTISIM_H
#define WL1MULTISIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            WCDMA L1 MULTISIM

GENERAL DESCRIPTION:
********************
  This file contains all the source code to support multisim. This file
  contains the code to support QTA, LTA, G2W TA and WRM.

EXTERNALIZED FUNCTIONS:
***********************
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1multisim.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what
--------   ---     ----------------------------------------------------------
02/24/15   ap      Initial check-in for WL1 QSH framework 
04/18/16    pg      To save the DS status variables while moving to FTM Mode
                    and restore later while going to Online.
09/20/15   wz/ar   Add prototypes to query if W2G/W2L CM/FMO searches are ongoing
09/15/15   bj      Split ACQ changes for W+1x.
06/30/15   vs      Moving the macro RRC_UNLOCK_FACH_QTA_OVERHEAD_TIMER_IN_MS from wl1multisim.c file for extern purpose.
05/11/15   sks     Do not do a sleep abort for timetag request in any L1 state.
05/11/15   rs      Triggering FW sleep before handling ext cmd in early RF shutdown cases
04/10/15   skk     Decrease lock req timeout time for ACQ steps during split acq to 3sec
04/09/15   gsk     Added API to check if QTA is force reserve
04/08/15   skk     Increase the no-lock timer during Cell Selection to 3sec. 
02/20/15   skk     Trigger DL WEAK IND if L1 doesnt get lock for 2 sec during cell selectoin.
01/30/15   gv      Fix medium compiler warnings.
01/21/15   ar      In CELL_FACH state, send resume failure immediately if TA duration
                    is larger than OOS duration.
12/12/14   skk     Change TRM priority if BCCH wakesup before sleep after PICH wakeup. 
12/4/14    pg      Added a new API to check whether QTA is on going.
11/20/14   gm      Fix for handling Empty QTA gaps by delaying qta stop to allow
                   preloading.
11/19/14   rs      Code changes for requesting lock with new proc during BCH decode
10/10/14   gm      Mods to request for first QTA with force_reserve after moving to FACH state. 
09/23/14   pv      Support for TRM swapping of chains for call setup in DR mode.
                   Added API to handle pending call setup.
09/23/14   gm      Cleanup QTA related timers in case of winning client changes 
                   with UNLOCK_REQUIRED event.
09/18/14   gm      mods to remove global variables which are never used.
09/17/14   ac      Changed WSRCH_NBREJ and QTA_ROTA NVs to use WNV API
09/15/14   pg      FR 21174 : QTA allowed during re-config in DCH changes
09/10/14   dm      Set default QBTA+ROTA NV to 5.
08/20/14   sd      Fixed DSDS Medium and Low compiler warnings in WCDMA.
08/18/14   skk     Request for lock for CELL_SELECTION_REQ in G2W TA mode as lock can be released as a part of SIB sleep.
08/12/14   sd      Fixed DSDS Medium and Low compiler warnings in WCDMA.
07/24/14   skk     Handle pending LOCK_ACQ_SIG if go to sleep didnt get triggered because of not having enough time.
07/18/14   vs      Added support for HS multisim support.
07/18/14   gm      Changes for ROTA feature
07/18/14   gm      added a function wl1_query_qta_start to be used by RxD module.
07/02/14   gm      Fixes related to DR DSDS bring up from multisim side.
06/19/14   dm      DR-DSDS first cut.
05/06/14   as      Moving WRM to WL1TRM.
04/27/14   jkb     Add check for Feature dual sim with feature qta
04/24/14   as      Moving wrm code to wl1trm.
04/18/14   as      Initial version created
===========================================================================*/

/*===========================================================================

                INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "rfm.h"

#ifdef FEATURE_DUAL_SIM
#include "task.h"
#include "msg.h"
#include "trm.h"
#include "l1task_v.h"
#include "rrccmd_v.h"
#include "wl1trm.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)

/* 30 ms are sufficient to freeze all activities in FACH state . Also, as RRC 
** files are zipped by qzip, we may face some delay in processing the commands 
** and releasing the chain for QTA which may lead to missed page on other sub. 
** So assuming a maximum delay of 10 ms, overhead time will be 30+10= 40 ms*/
#define RRC_UNLOCK_FACH_QTA_OVERHEAD_TIMER_IN_MS 40

/* Different Quick Tuneaway states in WL1:
**   WL1_QTA_STATE_NOT_ACTIVE  - Default state, no QTA active.
**   WL1_QTA_STATE_CM_FROZEN   - QTA started by first freezing compressed mode
**                               state machine to block any CM activity.
**   WL1_QTA_STATE_WFW_START_QTA_SENT - Start QTA sent to WFW.  Waiting for response.
**   WL1_QTA_STATE_ACTIVE      - QTA in progress.  GSM is active.
**   WL1_QTA_STATE_WFW_STOP_QTA_SENT    -  QTA stopped, Informed to firmware and L1 will wait
                                           for the command Done Ind. from WFW 
**   WL1_QTA_STATE_STOP_DELAYED - Added to handle Empty QTA gap. This is to handle preload by GSM 
                                  in case of empty qta gaps. Consider QTA is not finish yet.
**   WL1_QTA_STATE_QTA_STOPPED  - Intermediate state for acting on Back to back Qta
                                  if present or restoring L1 in state (just before qta)
                                  like unfreezing CM etc. 
**  WL1_QTA_STATE_QTA_CLEANUP   - Intermediate State for handling back to back qta
                                  in case triggered or not (expiry of tuneaway timer) 
*/
typedef enum
{
  WL1_QTA_STATE_NOT_ACTIVE,
  WL1_QTA_STATE_CM_FROZEN,
  WL1_QTA_STATE_WFW_START_QTA_SENT,
  WL1_QTA_STATE_ACTIVE,
  WL1_QTA_STATE_WFW_STOP_QTA_SENT,
  WL1_QTA_STATE_STOP_DELAYED,
  WL1_QTA_STATE_QTA_STOPPED,
  WL1_QTA_STATE_QTA_CLEANUP,
  WL1_QTA_STATE_MAX
} wl1_qta_state_enum_type;
#endif

/* standby mode enums*/
typedef enum {
WL1_SINGLE_STANDBY = 0,
WL1_DUAL_STANDBY
#ifdef FEATURE_TRIPLE_SIM
, WL1_TRIPLE_STANDBY
#endif
} wl1_standby_enum_type;

/* device mode enums*/
typedef enum {
WL1_SINGLE_ACTIVE = 0,
WL1_DUAL_ACTIVE
} wl1_device_mode_enum_type;

/*WL1 dual sim procedures*/
typedef enum {
  WL1_DS_PROC_START=0,
  WL1_DS_PROC_SUSPEND=1,
  WL1_DS_PROC_RESUME=2,
  WL1_DS_PROC_IDLE=3,
  WL1_DS_PROC_STOP=4,
  WL1_DS_PROC_DEACTIVATE=5,
  WL1_DS_PROC_FS=6,
  WL1_DS_PROC_ACQ=7,
  WL1_DS_PROC_SETUP_SBCH=8,
  WL1_DS_PROC_SETUP_NBCH=9,
  WL1_DS_PROC_SETUP_PCH=10,
  WL1_DS_PROC_SETUP_CTCH_FACH=11,
  WL1_DS_PROC_SETUP_DTCH_FACH=12,
  WL1_DS_PROC_SETUP_DCH=13,
  WL1_DS_PROC_PAGING=14,
  WL1_DS_PROC_CELL_RESEL_START=15,
  WL1_DS_PROC_CELL_TRANS_REQ=16,
  WL1_DS_PROC_DROP_CTCH=17,
  WL1_DS_PROC_BPLMN=18,
  WL1_DS_PROC_BPLMN_BCCH_DROP=19,
  WL1_DS_PROC_DRX=20,
  WL1_DS_PROC_GEN_CPHY_SETUP_REQ =21,
  WL1_DS_PROC_G2W_TA_IDLE=22,
  WL1_DS_PROC_G2W_TA_CELL_SELECTION_REQ=23,
  WL1_DS_PROC_BCCH =24,
  WL1_DS_PROC_MAX=25
}wl1_ds_proc_enum_type;

/*WL1 dual sim paging specific information structure*/
typedef struct {
  boolean lock_succ;

  uint16 declare_oos_thresh;

  uint32 trm_lock_consecutive_fail_cnt;

  /*counts no of lock failures after TRM priority is changed 
    from TRM_PAGEDEMOD to TRM_CHANNEL_MAINTANENCE for BCCH decode.*/
  uint32 trm_lock_consecuive_fail_cnt_for_bch;

  boolean cell_trans_req_stat;

  boolean wl1_in_paging_wakeup;
} wl1_ds_paging_spec_info_struct_type;

/*Structure to back up the DS Status during online->FTM->Online mode transitions*/
typedef struct {

  /*Flag to indicate if WL1 has enterend into FTM mode*/
  boolean wl1_is_reset_done_during_ftm_mode; 

  /*True: UE is in dual sim mode; False: UE is in single sim mode*/
  boolean wl1_in_ds_mode;

  /* standby mode: TRUE for Dual standby, FALSE for single standby */
  wl1_standby_enum_type wl1_standby_mode;

  /* device mode: TRUE for dual active , FALSE for single active*/
  wl1_device_mode_enum_type wl1_device_mode;
}wl1_ds_status_struct_type;

/*Structure to hold WL1 dual sim information */
typedef struct {

  /*True: UE is in dual sim mode; False: UE is in single sim mode*/
  boolean wl1_in_ds_mode;

  /* standby mode: TRUE for Dual standby, FALSE for single standby */
  wl1_standby_enum_type wl1_standby_mode;

#ifdef FEATURE_TRIPLE_SIM
  /* Tells whether it is */
  rrc_multi_sim_mode_type wl1_multi_sim_mode;
#endif

  /* device mode: TRUE for dual active , FALSE for single active*/
  wl1_device_mode_enum_type wl1_device_mode;

  /*WL1 procedures that are holding TRM lock*/
  uint32 wl1_ds_proc_mask;

  /*Last WL1 procedure that requested for TRM lock*/
  wl1_ds_proc_enum_type ds_curr_proc;

  /*Last WL1 procedure that requested for TRM lock*/
  wl1_ds_proc_enum_type ds_pend_proc;

  /*Last cphy_setup_cmd procedure that requested for TRM lock*/
  wl1_ds_proc_enum_type last_cphy_setup_proc;

  /*Indicate whether cmd is pending waiting for the sleep proc to be completed*/
  boolean trm_lock_sig_pending_on_sleep;

  /* Used to store the TRM lock signal (success/failure) in
    case TRM signal is received in the middle of paging wakeup;
    only applicable if trm_lock_sig_pending_on_sleep = TRUE */
  rex_sigs_type trm_lock_sig;

  /* Flag to indicate Ext cmd processing with no TRM lock */
  boolean process_ext_cmd_without_lock;
}wl1_ds_db_struct_type;

/*Structure to hold WL1 dual sim statistics*/
typedef struct {
  uint32 lock_req_and_not_cnt;

  uint32 lock_req_and_not_succ_cnt;

  uint32 lock_req_and_not_fail_cnt;

  uint32 lock_req_cnt;

  uint32 lock_req_succ_cnt;

  uint32 lock_req_fail_cnt;

  uint32 lock_res_cnt;
} wl1_ds_overall_stat_struct_type;

typedef void WL1_DS_PROC_CB_PTR(void);

typedef void (* wrm_srch_lock_grant_cb_type) (boolean);

/*Structure to hold WL1 dual sim information on a per procedure basis*/
typedef struct {
  /*Process the RRC command even if lock failed,
  but without mdsp and RF*/
  boolean can_lock_fail;

  wrm_activity_enum_type activity;

  /* How long the resource will be needed for (in milli secs) */
  uint32 lock_req_dur_in_ms;

  /* Callback to notify client when resource is granted */
  WL1_DS_PROC_CB_PTR *lock_grant_callback;

  uint32 lock_req_timeout_in_ms;

  uint32 trm_lock_proc_fail_cnt;

  uint32 trm_lock_proc_succ_cnt;

  l1_ext_cmd_type *ext_cmd_ptr;

  l1_cmd_enum_type rrc_to_l1_cmd;

  rrc_cmd_e_type l1_to_rrc_cnf;
}wl1_ds_proc_info_struct_type;

/* =========================================================== */
/* Log packet definitions that reports the Tuneaway properties  */
/* =========================================================== */

/* Stores the types of Tune-Aways */
typedef enum
{
  /* Quick Tuneaway flag */
  WL1TRM_TUNEAWAY_LOG_QTA,

  /* Long Tuneaway flag */
  WL1TRM_TUNEAWAY_LOG_LTA
} wl1trm_tuneaway_log_type;

/* Flags to the wl1trm_tuneaway_log_set_time function to record
** specific timeticks for Tuneaways */
typedef enum
{
  /* Flag that records the timetick when CPHY_SUSPEND_WCDMA_MODE_REQ is received */
  SET_TUNEAWAY_SUSPEND_START_TIMETICK,

  /* Flag that clears the timetick of suspend, used in case the FACH QTA is cancelled. */
  CLEAR_TUNEAWAY_SUSPEND_START_TIMETICK,

  /* Flag that records the timetic when RRC_CPHY_SUSPEND_WCDMA_MODE_CNF is sent */
  SET_TUNEAWAY_SUSPEND_END_TIMETICK,

  /* Flag that records the timetick when RESUME_WCDMA_MODE_REQ is received */
  SET_TUNEAWAY_RESUME_START_TIMETICK,

  /* Flag that records the timetick when CPHY_SETUP_CNF is sent */
  SET_TUNEAWAY_SETUP_END_TIMETICK
}wl1trm_tuneaway_log_set_time_type;

/* Structure that contains the data for every timeslice type. Contains information regarding the starting
   time and the ending time of the timeslice. */
typedef PACKED struct PACKED_POST
{
  /* Timetick when the timeslice begins */
  timetick_type start_timetick;

  /* Timetick when the timeslice ends */
  timetick_type end_timetick;

  /* The duration of the timeslice in timeticks */
  timetick_type duration_timetick;

  /* The duration of the timeslice in milli-seconds */
  uint16 duration_msecs;

  /* The CFN when timeslice begins */
  uint16 start_cfn;

  /* The CFN when timeslice ends */
  uint16 end_cfn;

  /* The Slot when timeslice begins */
  uint16 start_slot;

  /* The Slot when timeslice ends */
  uint16 end_slot;
} wl1trm_tuneaway_log_timeslice_struct_type;

/* Structure that contains the data for the log packet WL1TRM_TUNEAWAY_LOG_PKT */
typedef PACKED struct PACKED_POST
{
  /* Contains the flag for the type of tuneaway (LTA or QTA) */
  uint8 tuneaway_type;

  /* State of L1 when the tuneaway begins */
  uint16 tuneaway_l1m_state;

  /* Is Harq flush required. Only valid for QTAs */
  boolean tuneaway_harq_flush;

  /* Data structure that contains data about the Tuneaway duration */
  wl1trm_tuneaway_log_timeslice_struct_type tuneaway_timeslice;

  /* Data structure that contains data about the Suspend duration */
  wl1trm_tuneaway_log_timeslice_struct_type suspend_timeslice;

  /* Data structure that contains data about the Resume and Setup duration */
  wl1trm_tuneaway_log_timeslice_struct_type resume_and_setup_timeslice;

  /* Flag that shows if FACH QTA is cancelled or not */
  boolean fach_qta_cancelled;

  /* Contains the flag is FACH QTA is force reserved */
  boolean force_reserve;

  /* This bitmask contains the type of search which was blocked to
     allow the QTA or due to which the QTA was cancelled */
  uint8 fach_qta_reason_bitmask;

} wl1trm_tuneaway_log_data_struct_type;

/* Define the WL1TRM_TUNEAWAY_LOG_PKT log packet */
LOG_RECORD_DEFINE(WL1TRM_TUNEAWAY_LOG_PKT)

  /* Version number of this log packet */
  uint8 version;

  /* The data of the log packet */
  wl1trm_tuneaway_log_data_struct_type log_data;
LOG_RECORD_END

#define WL1TRM_TUNEAWAY_LOG_PKT_SIZE() (sizeof(WL1TRM_TUNEAWAY_LOG_PKT_type))

/* WL1TRM_TUNEAWAY_LOG_SUBMIT command type that contains the WL1TRM_TUNEAWAY_LOG_PKT
** log packet data */
typedef struct
{
  /* Command header for l1m command queue */
  l1_local_cmd_hdr_type   hdr;

  /* Log packet data associated with the command */
  wl1trm_tuneaway_log_data_struct_type log_data;

} wl1trm_tuneaway_log_submit_cmd_type;

typedef struct
{
  #if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
  wl1_qta_state_enum_type  wl1_qta_state;

  /*Variable to track ciphering key restoring status after QTA */
  boolean restore_ul_ciph_keys_after_qta;

  boolean wl1_back_to_back_qta;

  boolean wl1_is_empty_qta_gap;

  /* Time in sclk stored when a winning client tries WRM unlock */
  uint32  wl1_unlock_by_time_in_sclk;

  #ifdef FEATURE_TRIPLE_SIM
  trm_client_enum_t wl1_back_to_back_qta_client_id;
  #endif
  #endif

  #ifdef FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
  #if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
  trm_client_info_t winning_ta_client;
  #endif  /* #ifdef FEATURE_QTA      */
  #endif

  trm_time_t trm_reserve_time_sclk_val;
  
  /* Variables to hold the last trm band req grant info*/
  l1_freq_scan_band_enum_type wl1_last_granted_band;
  trm_frequency_type_t wl1_last_granted_trm_freq_info;

  /*TRM lock acquisition success/failure statistics when in Dual SIM mode*/
  wl1_ds_overall_stat_struct_type wl1_ds_overall_stat;

  uint32 wl1_already_has_tcxo;
} wl1_multisim_extern_db_type;

extern wl1_multisim_extern_db_type wl1_multisim_extern_db;

typedef struct
{
  uint32 wrm_g2w_ta_l1_trm_resrv_sclk;
  trm_duration_t wrm_split_acq_saved_duration;
  boolean wrm_force_req_and_notify;
  boolean wrm_g2w_ta_prio_inversion_in_progress;
  boolean wrm_g2w_ta_block_request_and_notify;
  boolean wrm_prio_inv_tmr_set_for_split_acq;
  boolean wrm_idle_oos_prio_inv_in_progress;
} wl1_multisim_g2w_ta_extern_db_type;


/* Multi-Sim Configuration of UE: SS/DSDA/DSDS/TSTS*/
typedef enum
{
  /* Single SIM */
  WL1_MULTI_SIM_SS,
    /* Dual Sim Dual active */
  WL1_MULTI_SIM_DSDA,
  /* Dual Sim Dual standby */
  WL1_MULTI_SIM_DSDS,
  /* Triple Sim Triple standby */
  WL1_MULTI_SIM_TSTS,
  /* Invalid Sim Config */
  INVALID_CONFIG
}wl1_ds_multi_sim_config_enum_type;


extern wl1_multisim_g2w_ta_extern_db_type wl1_multisim_g2w_ta_extern_db;

typedef struct
{
  /* WRM RRC prio inv cb store */
  wrm_grant_callback_type  wrm_rrc_prio_inv_cb;

  /* G2W TA variables */
  wrm_srch_lock_grant_cb_type wrm_g2w_ta_srch_lock_grant_cb;
}wl1_multisim_extern_cb_db_type;

extern wl1_multisim_extern_cb_db_type wl1_multisim_extern_cb_db;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/* mutex for QTA module*/
extern rex_crit_sect_type wl1_qta_crit_sect;

#endif

#ifdef FEATURE_WCDMA_TRM
extern rex_timer_type wrm_g2w_ta_priority_inversion_timer;
#endif /* FEATURE_WCDMA_TRM */

extern wl1_ds_db_struct_type wl1_ds_db;
extern wl1_ds_paging_spec_info_struct_type wl1_ds_paging_spec_info;
extern wl1_ds_proc_info_struct_type wl1_ds_proc_info[WL1_DS_PROC_MAX];
extern wl1_ds_status_struct_type wl1_ds_status_backup_db;


#define WL1_IN_DS_MODE() (wl1_ds_db.wl1_in_ds_mode)
#define WL1_IN_DUAL_ACTIVE_MODE() \
        ( \
         ( (wl1_ds_db.wl1_device_mode == WL1_DUAL_ACTIVE) && (wl1_ds_db.wl1_standby_mode == WL1_DUAL_STANDBY) ) \
         ? TRUE : FALSE \
        )
#define WL1_NOT_ACTIVE() ((l1m_state == L1M_DEACTIVE) || (l1m_state == L1M_DEEP_SLEEP) || (l1m_state == L1M_STOPPED))


#define WL1_SAVE_DS_MODE_STATUS_FOR_FTM() \
       do{                    \
        wl1_ds_status_backup_db.wl1_in_ds_mode =  wl1_ds_db.wl1_in_ds_mode;   \
        wl1_ds_status_backup_db.wl1_device_mode = wl1_ds_db.wl1_device_mode;  \
        wl1_ds_status_backup_db.wl1_standby_mode = wl1_ds_db.wl1_standby_mode; \
       }while(0)

#define WL1_RESTORE_DS_MODE_STATUS_FOR_ONLINE() \
       do{                    \
        wl1_ds_db.wl1_in_ds_mode = wl1_ds_status_backup_db.wl1_in_ds_mode;    \
        wl1_ds_db.wl1_device_mode= wl1_ds_status_backup_db.wl1_device_mode;   \
        wl1_ds_db.wl1_standby_mode = wl1_ds_status_backup_db.wl1_standby_mode;\
       }while(0)

#define WL1_FORCE_RESET_DS_MODE() \
       do{                    \
        wl1_ds_db.wl1_in_ds_mode = 0;  \
        wl1_ds_db.wl1_device_mode = WL1_SINGLE_ACTIVE; \
        wl1_ds_db.wl1_standby_mode = WL1_SINGLE_STANDBY; \
       }while(0)


#define WRM_WL1_RESERVATION_TIME_SCLK_INVALID 0xFFFFFFFF

#define WL1_INTRA_F_SRCH_DUR (M_BY_N_MS_IN_SC(60,1))
#define WL1_INTER_F_SRCH_DUR (M_BY_N_MS_IN_SC(80,1))
#define WL1_INTER_RAT_SRCH_DUR (M_BY_N_MS_IN_SC(100,1))
#define WL1_LTE_SRCH_DUR (M_BY_N_MS_IN_SC(40,1))


#define WL1_DS_NUM_PEND_EXT_CMD 3

#define WL1_DS_INVALID_CMD_IDX 0xFF

/*Timeout to send DL_WEAK_IND for no lock to RRC during Cell selection in G2W 
  TA mode. It should be > (2 * max_sib_rep_rate + 200ms). As max rep_rate seen
  generally is 1.28sec, timeout is set to 3sec.*/
#define WL1_DS_CELL_SELECTION_NO_LOCK_TIMEOUT    3000

/*Macro to check whether GL1 is allowing Tuneaway.*/
#define WRM_G2W_TA_MODE_ACTIVE() wl1idle_ta_mgr_determine_g2w_ta_mode()

/*Unit test definitions*/
#if 0
#define TRM_LAST_RESOURCE 1
#define TRM_CHANNEL_MAINTENANCE 1
#define TRM_NUM_REASONS 1
#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
#define WL1_QTA_MUTEXLOCK()   REX_ISR_LOCK(&wl1_qta_crit_sect)
#define WL1_QTA_MUTEXFREE()   REX_ISR_UNLOCK(&wl1_qta_crit_sect)
#endif

#define WL1_IS_QTA_CONFIGURED() (wl1_preparing_for_qta() || wl1_is_wfw_in_qta_gap())

/*If TRM lock request fails consecutively for this threshold count, then OOS
is declared to RRC*/
#define WL1_DS_PAG_TRM_LOCK_OOS_THRESH (8)

/*Macros to check whethe a procedure is valid and of particular type*/
#define WL1_DS_IS_PROC_VALID(ds_proc) (ds_proc < WL1_DS_PROC_MAX)

/*Macros to handle statistics corresponding to a procedure*/
#define WL1_DS_INC_FAIL_CNT_FOR_PROC(ds_proc) (wl1_ds_proc_info[ds_proc].trm_lock_proc_fail_cnt++)

#define WL1_DS_INC_SUCC_CNT_FOR_PROC(ds_proc) (wl1_ds_proc_info[ds_proc].trm_lock_proc_succ_cnt++)

#define WL1_DS_INC_OVERALL_FAIL_CNT() (wl1_multisim_extern_db.wl1_ds_overall_stat.lock_req_fail_cnt++)

#define WL1_DS_INC_OVERALL_SUCC_CNT() (wl1_multisim_extern_db.wl1_ds_overall_stat.lock_req_succ_cnt++)

#define WL1_DS_INC_PAGING_CONSEC_FAIL_CNT() (wl1_ds_paging_spec_info.trm_lock_consecutive_fail_cnt++)

#define WL1_DS_IS_CMD_PTR_VALID(ds_proc) (wl1_ds_proc_info[ds_proc].ext_cmd_ptr != NULL)

/*Macros to get various fields of a procedure*/
#define WL1_DS_CAN_LOCK_FAIL_FOR_PROC(ds_proc) (wl1_ds_proc_info[ds_proc].can_lock_fail)

#define WL1_DS_GET_RESOURCE_FOR_PROC(ds_proc) (wl1_ds_proc_info[ds_proc].resource_needed)

#define WL1_DS_GET_DUR_IN_MS(ds_proc) (wl1_ds_proc_info[ds_proc].lock_req_dur_in_ms)

#define WL1_DS_GET_DUR_IN_SCLK(ds_proc) (M_BY_N_MS_IN_SC(WL1_DS_GET_DUR_IN_MS(ds_proc),1))

#define WL1_DS_GET_LOCK_RSN_FOR_PROC(ds_proc) (wl1_ds_proc_info[ds_proc].lock_reason)

#define WL1_DS_GET_CB_PTR_FOR_PROC(ds_proc) (wl1_ds_proc_info[ds_proc].lock_grant_callback)

#define WL1_DS_GET_LOCK_REQ_TIMEOUT_FOR_PROC(ds_proc) (wl1_ds_proc_info[ds_proc].lock_req_timeout_in_ms)

#define WL1_DS_SET_LOCK_REQ_TIMEOUT_FOR_PROC(ds_proc,msec) \
  (wl1_ds_proc_info[ds_proc].lock_req_timeout_in_ms = msec)

#define WL1_DEFAULT_GRANT_CB (wl1_ds_grant_cb)

/*Macros to handle WL1 dual sim procedure bit mask*/
#define WL1_DS_PROC_BMSK(ds_proc) ((uint32)(1 << ((uint32)ds_proc)))

#define WL1_DS_RESET_DS_PROC_BMASK() \
(wl1_ds_db.wl1_ds_proc_mask = 0)

#define WL1_DS_ADD_PROC_TO_DS_BMASK(ds_proc) \
  (wl1_ds_db.wl1_ds_proc_mask |= WL1_DS_PROC_BMSK(ds_proc))

#define WL1_DS_REM_PROC_FROM_DS_BMASK(ds_proc) \
  (wl1_ds_db.wl1_ds_proc_mask &= (~(WL1_DS_PROC_BMSK(ds_proc))))


/*Macros to check whether any dual sim procedure is active in WL1*/
#define WL1_IN_DS_WITH_ANY_LOCK() (WL1_IN_DS_MODE() && ((WRM_CLI_STATE_IS_IN_LOCK(WCDMA_TRM, WRM_PRIMARY_CLIENT_L1)) || (wl1idle_ta_mgr_lock_rel_offline_activity_pending())))

#define WL1_IN_DS_WITH_NO_LOCK() (WL1_IN_DS_MODE() && ((!WRM_CLI_STATE_IS_IN_LOCK(WCDMA_TRM, WRM_PRIMARY_CLIENT_L1)) && (!wl1idle_ta_mgr_lock_rel_offline_activity_pending())))

#define WL1_DS_IS_ANY_PROC_ACTIVE() (wl1_ds_db.wl1_ds_proc_mask != 0)

#define WL1_DS_IS_PROC_ACTIVE(ds_proc) \
  ((wl1_ds_db.wl1_ds_proc_mask & WL1_DS_PROC_BMSK(ds_proc)) != 0)
 
#define WL1_IS_ANY_OTHER_PROC_ACTIVE(ds_proc) \
  ((wl1_ds_db.wl1_ds_proc_mask & (~(WL1_DS_PROC_BMSK(ds_proc)))) != 0)

#define WL1_IN_DS_WITH_PAG_LOCK() (WL1_IN_DS_MODE() && (wl1_ds_paging_spec_info.lock_succ))

#define WL1_IN_DS_WITH_NO_PAG_LOCK() (WL1_IN_DS_MODE() && (!wl1_ds_paging_spec_info.lock_succ))

#define WL1_IS_CELL_TRANS_REQ_SUCC() (wl1_ds_paging_spec_info.cell_trans_req_stat)

#define WL1_IS_IN_PCH_SLEEP_DS_NO_LOCK() ((WL1_IN_DS_MODE()) && \
  (l1m_state == L1M_PCH_SLEEP) && \
  (WL1_IN_DS_WITH_NO_LOCK()))

#define WL1_IS_IN_PCH_SLEEP_DS_LOCK() ((WL1_IN_DS_MODE()) && \
  (l1m_state == L1M_PCH_SLEEP) && \
  (WL1_IN_DS_WITH_ANY_LOCK()))

#define WL1_DS_IS_CMD_IDX_INVALID(cmd_idx) \
  (cmd_idx == WL1_DS_INVALID_CMD_IDX)

#define WL1_DS_IS_TRM_SIG_PENDING_ON_SLEEP() \
  (wl1_ds_db.trm_lock_sig_pending_on_sleep == TRUE)

#define WL1_DS_IS_WL1_IN_PAGING_WAKEUP() \
  (wl1_ds_paging_spec_info.wl1_in_paging_wakeup == TRUE)

/*Macro to get the client that has requested for tuneaway*/
#define WL1_DS_GET_WINNING_TA_CLIENT() (wl1_multisim_extern_db.winning_ta_client.client_id)

/*Macro to get reason for which Idle tech has requested the TA gap*/
#define WL1_DS_GET_WINNING_TA_CLIENT_REASON() (wl1_multisim_extern_db.winning_ta_client.reason)

/*===========================================================================

                FUNCTION DECLARATIONS FOR MODULE

=========================================================================*/

extern boolean wl1idle_ta_mgr_lock_rel_offline_activity_pending(void);

/*===========================================================================
FUNCTION     WL1_DS_INIT

DESCRIPTION
  This function initialized WL1 dual sim related globals. This function must be
  once after power-on.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_init(void);

/*===========================================================================
FUNCTION     WL1_DS_REQ_AND_NOT_LOCK_LOCK_FOR_PROC

DESCRIPTION
  This function requests for lock from WRM and returns success/failure to the
  caller.

DEPENDENCIES
  None.

PARAMETERS
  ds_proc:.the procedure for which WL1 needs the TRM lock

RETURN VALUE
  TRUE: lock acquisition successful
  FALSE: lock acquisition failed

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_req_and_ntfy_lock_for_proc(wl1_ds_proc_enum_type ds_proc);

/*===========================================================================
FUNCTION     WL1_DS_REQ_LOCK_FOR_PROC

DESCRIPTION
  This function requests lock for WL1 paging.

DEPENDENCIES
  None.

PARAMETERS
  ds_proc:.the procedure for which WL1 needs the TRM lock

RETURN VALUE
  boolean:True - Chain is granted
  False - else

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_ds_req_lock_for_proc(wl1_ds_proc_enum_type ds_proc);

/*===========================================================================
FUNCTION     WL1_DS_GRANT_CB

DESCRIPTION
  This function is called by WRM with lock status for WL1.

DEPENDENCIES
  None.

PARAMETERS
  trm_client_enum_t: The client which needs the RF resource
  event: The RF resource which is being granted
  tag: Anonymous payload passed in wrm_request_and_notify()

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_grant_cb(boolean grant);

/*===========================================================================
FUNCTION     WL1_DS_FIND_PROC_FROM_SETUP_CMD

DESCRIPTION
  This function is called with the cphy_setup_cmd pointeto find whether any dual
  sim procedure is associated with this command.

DEPENDENCIES
  This function does not acquire lock for the associated procedure. The caller 
  must call wl1_ds_req_and_not_lock_lock_for_proc() to acquire lock based
  on the procedure returned by this function.

PARAMETERS
  ext_cmd_ptr: cphy_setup_cmd pointer sent by RRC

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern wl1_ds_proc_enum_type wl1_ds_find_proc_from_setup_cmd(l1_ext_cmd_type *ext_cmd_ptr);

/*===========================================================================
FUNCTION     WL1_DS_FIND_PROC_FROM_EXT_CMD

DESCRIPTION
  This function is called with the command pointer associated with any
  command from RRC. It finds whether any dual sim procedure is associated
  with this command.

DEPENDENCIES
  This function does not acquire lock for the associated procedure. The caller 
  must call wl1_ds_req_and_not_lock_lock_for_proc() to acquire lock based
  on the procedure returned by this function.

PARAMETERS
  ext_cmd_ptr: Command pointer associated with the command from upper layers

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern wl1_ds_proc_enum_type wl1_ds_find_proc_from_ext_cmd(l1_ext_cmd_type *ext_cmd_ptr);

/*===========================================================================
FUNCTION     WL1_DS_FIND_PROC_FROM_SETUP_CNF

DESCRIPTION
  This function is called with the command pointer associated with any confirm
  sent from L1 to RRC. It finds whether any dual sim procedure is associated
  with this command.

DEPENDENCIES
  This function does not release/acquire lock for the associated procedure. The caller 
  must call wl1_ds_req_and_not_lock_lock_for_proc() or wrm_release() to
  acquire/release the lock.

PARAMETERS
  cnf_to_rrc_ptr: Confirm message sent by L1 to RRC.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern wl1_ds_proc_enum_type wl1_ds_find_proc_from_ext_cnf(rrc_cmd_type *cnf_to_rrc_ptr);

/*===========================================================================
FUNCTION     WL1_DS_IS_LOCK_NEEDED_FOR_EXT_CMD

DESCRIPTION
  This function is called with the command pointer associated with any
  command from RRC. It finds whether any dual sim procedure is associated
  with this command.

DEPENDENCIES
  This function does not acquire lock for the associated procedure. The caller 
  must call wl1_ds_req_and_not_lock_lock_for_proc() to acquire lock based
  on the procedure returned by this function.

PARAMETERS
  ext_cmd_ptr: Command pointer associated with the command from upper layers

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern wl1_ds_proc_enum_type wl1_ds_is_lock_needed_for_ext_cmd(l1_ext_cmd_type *ext_cmd_ptr);

/*===========================================================================
FUNCTION     WL1_DS_HDLE_LOCK_ACQ_SUCC

DESCRIPTION
  This function handles TRM lock acquistion success for various procedures.
  It re-initializes mdsp/RF and increments any counters to track
  success.

DEPENDENCIES
  This function should not re-init mdsp/RF for paging procedure.

PARAMETERS
  ds_proc: Current dual sim procedure

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_hdle_lock_acq_succ(wl1_ds_proc_enum_type ds_proc);

/*===========================================================================

FUNCTION  WL1_GET_REM_TIME_IN_MS

DESCRIPTION
  This function converts the remaining Sclks to ms..

DEPENDENCIES
  None.

RETURN VALUE
  int 64, reamining Sclks time in ms

SIDE EFFECTS
  None.
===========================================================================*/
extern int64 wl1_get_rem_time_in_ms(uint32 rem_sclk);

/*===========================================================================
FUNCTION     WL1_DS_HDLE_CNF_SENT_TO_RRC

DESCRIPTION
  This function handles confirmations sent to RRC from dual_sim/trm_lock perspective.
  It releases the TRM lock if it is no longer needed and resets flags.

DEPENDENCIES
  None

PARAMETERS
  ds_proc: Current dual sim procedure

RETURN VALUE
  TRUE - send conf to RRC.
  FALSE - don't send the conf to RRC yet.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_ds_hdle_cnf_sent_to_rrc(rrc_cmd_type *cnf_to_rrc_ptr);

/*===========================================================================
FUNCTION     WL1_DS_HDLE_LOCK_FAILURE_FOR_PAGING

DESCRIPTION
  This function is called when lock acquisition has failed for paging procedure.
  This function sends OOS indication to RRC if the consecutive paging lock
  acquisition failure count is greater than the preset threshold.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_hdle_lock_failure_for_paging(void);


/*===========================================================================
FUNCTION     WL1_DS_CHK_N_RESET_PCH_OOS_CRIT

DESCRIPTION
  This function checks if OOS ind is about to be sent by DSDS module to RRC,
  and in that case resets the PCH OOS timer to avoid sending a second OOS
  indication.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_chk_n_reset_pch_oos_crit(void);

/*===========================================================================
FUNCTION     WL1_DS_CHECK_FOR_OOS

DESCRIPTION
  This function sends OOS indication to RRC if the consecutive paging lock
  acquisition failure count is greater than the preset threshold.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_check_for_oos(void);

/*===========================================================================
FUNCTION     WL1_DS_POST_CELL_TRANS_CNF_CB

DESCRIPTION
  This function is called at the end of cell transition. It resets any paging
  related statistics.

DEPENDENCIES
  None

PARAMETERS
  status: the status of cell transition

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_post_cell_trans_cnf_cb(boolean status);

/*===========================================================================
FUNCTION     WL1_DS_PRE_CELL_TRANS_REQ_CB

DESCRIPTION
  This function is called upon receiving the cell transition command.
  It sets the status based on the cell reselection status from RRC

DEPENDENCIES
  None

PARAMETERS
  status: the status of cell transition
  Reselection type: L1_ASET_UPD_CELL_RESELECTION_FAILURE - Treated as Failure
  Any other reselection type is treared as Success

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_pre_cell_trans_req_cb(boolean status);

/*===========================================================================
FUNCTION     WL1_DS_HDLE_LOCK_ACQ_FAIL

DESCRIPTION
  This function handles TRM lock acquistion failure for various procedures.
  It sends failure confirmation to RRC and increments any counters to track
  failure.

DEPENDENCIES
  None.

PARAMETERS
  cmd_ptr: Command pointer associated with the command from upper layers
  ds_proc: Current dual sim procedure

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_hdle_lock_acq_fail(wl1_ds_proc_enum_type ds_proc);

/*===========================================================================
FUNCTION     WL1_DS_PREP_AFTER_LOCK_ACQ

DESCRIPTION
  This function is called when lock acquisition is successful for a procedure
  and WL1 needs to be prepare mdsp/rf. This does not handle lock acquisition
  case for Paging wakeups.

DEPENDENCIES
  None

PARAMETERS
  ds_proc: the procedure for which WL1 needs to be prepared

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_prep_after_lock_acq(wl1_ds_proc_enum_type ds_proc);

/*===========================================================================
FUNCTION     WL1_DS_ENTER_SLEEP

DESCRIPTION
  This function must be called when WL1 is about to start sleeping. This function must be called
  from SLEEP_START_INT.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_enter_sleep(void);

/*===========================================================================
FUNCTION     WL1_DS_EXIT_SLEEP

DESCRIPTION
  This function conducts the dual sim procedure needed when WL1 exits sleep.
  This function must be called from micro wakeup ISR context.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  TRUE: Lock acquistion for paging is successful
  FALSE: Lock acquistion for paging failed

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_ds_exit_sleep(void);

/*===========================================================================
FUNCTION     WL1_DS_HDLE_CELL_RESEL_START

DESCRIPTION
  This function must be called at the start of cell reselection. This function will ensure that WL1 holds
  the TRM lock during the entire duration of cell reselection.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_hdle_cell_resel_start(void);

/*===========================================================================
FUNCTION     WL1_DS_PRE_MEAS_START_IND

DESCRIPTION
 This function requests for TRM lock extension for a specified duration of
 trm_duration_t slpclk units

DEPENDENCIES
  This function must be called before srchcr starts any measurements in PCH_SLEEP state.

PARAMETERS
  duration - time in slpclks for which lock is requested.

RETURN VALUE
  True - if lock extension succeeded
  False - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_ds_pre_meas_start_ind(trm_duration_t duration);

/*===========================================================================
FUNCTION     WL1_DS_SET_PAGING_LOCK_STATUS

DESCRIPTION
  This function sets the paging lock status with the passed value.

DEPENDENCIES
  This function must be called at the earliest after wakeup.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_set_paging_lock_status(boolean lock_status);

/*===========================================================================
FUNCTION     WL1_DS_HDLE_STATUS_CHG_IND

DESCRIPTION
  This function processes the Dual SIM status Indication from RRC. If UE is in
  dual sim mode, then WL1-RRC interface and sleep/wakeup procedures are
  impacted. This includes extra time needed for wakeup, mdsp/RF preparation
  for every wakeup and checking for TRM lock to process RRC request/indication.
  If UE is in single sim mode, then WL1 does not need TRM lock for every
  procedure and wakeup time can be reduced accordingly.

DEPENDENCIES
  WL1 can accept dual sim status indication only in do-nothing states.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_hdle_status_chg_ind(l1_ds_status_change_ind_type *ds_cmd_ptr);

extern void wl1_ds_hdle_sig(rex_sigs_type ds_sig);

extern void wl1_ds_proc_pich_done(void);

/*===========================================================================
FUNCTION     WL1_DS_HDLE_BPLMN_START

DESCRIPTION
  This function handles the start of BPLMN search by changing the current
  DS procedure mask from Paging to BPLMN

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_hdle_bplmn_start(void);

/*===========================================================================
FUNCTION     WL1_DS_HDLE_BPLMN_STOP

DESCRIPTION
  This function handles the stop of BPLMN search by changing the current
  DS procedure mask from BPLMN to Paging

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_hdle_bplmn_stop(void);

/*===========================================================================
FUNCTION     WL1_DS_IND_DRX_CMD

DESCRIPTION
  This function handles the sets/resets any variable in wl1_ds upon the reception
  of a drx command from RRC.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_ind_drx_cmd(void);

/*===========================================================================
FUNCTION     WL1_DS_OK_TO_PROC_EXT_CMD_WITHOUT_LOCK

DESCRIPTION
  This function says whether an external command can be processed without
  TRM lock and mdsp/RF in pch_sleep state.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  TRUE: if external command can be processed without TRM lock
  FALSE: if external command cannot be processed without TRM lock

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_ds_ok_to_proc_ext_cmd_in_sleep(l1_ext_cmd_type *ext_cmd_ptr, boolean wrm_g2w_ta_skip_sleep_abort);

/*===========================================================================
FUNCTION     WL1_DS_L1M_WOKEN_UP

DESCRIPTION
  This function clears the WL1 in paging wakeup flag as L1M is moving to PCH state.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_l1m_woken_up(void);

/*===========================================================================
FUNCTION enchs_override_cqi

DESCRIPTION
  This function sets a flag as well as the CQI override value to the MDSP. 
  mDSP reads the flag at the R99 frame boundary and if set, overrides the 
  CQI with the value written by L1.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void enchs_override_cqi(boolean flag, uint8 val);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*==========================================================================

FUNCTION     wl1_query_qta_start

DESCRIPTION
  This function is written to be used by RxD module to freeze RxD before the
  start of QTA. 

  WL1 subsystems call this function to determine the state of Quick Tuneaway
  (QTA).  QTA is a mode where WCDMA has frozen its state for a brief period,
  ~30 msec, to allow GSM to check its paging occasion.  So during QTA, WL1
  subsystems should forgo significant activities, like mDSP accesses, RF
  accesses, etc., but continue whatever processing will allow them to start
  back up again quickly in the future.

  WL1 subsystems must call this function periodically in order to determine
  current and future QTA status.  And once QTA goes active, WL1 subsystems
  must call this function periodically to determine when QTA has ended.
  At that point, WL1 subsystems should resume normal processing.

DEPENDENCIES
  None.

PARAMETERS
  uint16 *next_qta_cfn  - Return CFN of future QTA gap to caller.  Set to
                          MAX_CFN_COUNT if no known QTA gap pending.
  uint16 *next_qta_slot - Return slot of future QTA gap.  Value undefined
                          if no known QTA gap pending.
RETURN VALUE
 None.
===========================================================================*/
extern void wl1_query_qta_start(uint16 *next_qta_cfn, 
                                      uint16 *next_qta_slot);
/*==========================================================================

FUNCTION     wl1_set_reserve_fail_count_for_first_page

DESCRIPTION
 As G might have missed some pages during transition to FACH state(those missed pages we cant count), 
 request first QTA with force_reserve. So setting the reserve_fail_count to 2.

DEPENDENCIES
 None.

PARAMETERS
 None.

RETURN VALUE
 None.
===========================================================================*/
extern void wl1_set_reserve_fail_count_for_first_page(void);

/*===========================================================================
FUNCTION     WRM_IS_NEXT_TUNEAWAY_PAGE_DECODE

DESCRIPTION
  This function is called by RRC to determine whether the next W2G tuneaway
  is PAGE Decode by GSM Sub or not.  It calls into TRM to check GSM's intentions
  for the next tuneaway.  If GSM's intentions are long, like cell reselection, 
  then return FALSE.  If GSM's intentions are short, like only check paging 
  occasion, then do quick tuneaway.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - Next Tuneaway is short Tuneaway.
  FALSE - Next Tuneaway could be long Tuneaway.
===========================================================================*/
extern boolean wrm_is_next_tuneaway_page_decode(void);

/*===========================================================================
FUNCTION     WRM_IS_NEXT_TUNEAWAY_QTA

DESCRIPTION
  This function is called by RRC to determine whether the next W2G tuneaway
  should be legacy variety or quick variety (QTA).  It calls into TRM to
  check GSM's intentions for the next tuneaway.  If GSM's intentions are long,
  like cell reselection, then do legacy tuneaway.  If GSM's intentions are
  short, like only check paging occasion, then do quick tuneaway.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - Use QTA for next tuneaway.
  FALSE - Use legacy for next tuneaway.
===========================================================================*/
extern boolean wrm_is_next_tuneaway_qta(void);

/*===========================================================================
FUNCTION     WRM_IS_FACH_RESERVATION_TIMER_ON

DESCRIPTION
  Returns wl1_fach_reservation_timer_on

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - Fach reservation timer is running
  FALSE - Fach reservation timer is not running
===========================================================================*/
extern boolean wl1_is_fach_reservation_timer_on(void);

/*===========================================================================
FUNCTION     WL1_QTA_TIMER_EXPIRED_CMD

DESCRIPTION
  wl1_tuneaway_timer expired during quick tuneaway procedure, which has
  different steps and different times between steps.  This is local command
  handler invoked for WL1_QTA_TIMER_EXPIRED_CMD.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_qta_timer_expired_cmd(void);

/*===========================================================================
FUNCTION     WL1_QTA_UNLOCK_CANCEL_CMD

DESCRIPTION
  This is local command handler is invoked for WL1_QTA_UNLOCK_CANCEL_CMD.


DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_qta_unlock_cancel_cmd(void);

/*===========================================================================
FUNCTION     L1M_CPHY_START_QTA_REQ_CMD

DESCRIPTION
  This function is an indication from RRC that we are to start W2G quick
  tuneaway process.

DEPENDENCIES
  None.

PARAMETERS
  l1_start_qta_cmd_type *start_qta_cmd - Unused.

RETURN VALUE
  None.
===========================================================================*/
extern void l1m_cphy_start_qta_req_cmd(l1_start_qta_cmd_type *start_qta_cmd);

/*===========================================================================
FUNCTION     L1M_CPHY_STOP_QTA_REQ_CMD

DESCRIPTION
  This function is an indication from RRC that we are to end W2G quick
  tuneaway process, meaning resume WCDMA mode.

DEPENDENCIES
  None.

PARAMETERS
  l1_stop_qta_cmd_type *stop_qta_cmd - Unused.

RETURN VALUE
  None.
===========================================================================*/
extern void l1m_cphy_stop_qta_req_cmd(l1_stop_qta_cmd_type *stop_qta_cmd);

/*===========================================================================
FUNCTION     WL1_QTA_START_QTA

DESCRIPTION
  This function is invoked by QXDM debug command to simulate getting start
  QTA command from RRC.

  This is what QXDM command looks like to simulate QTA from RRC:

  send_data 75 4 47 0 0x1E 0x00

  The last 2 values define the QTA duration in msec.  For example, the above
  sets QTA duration of 30 msec = 0x001E.  Swap the bytes of the hex value to
  use and enter on the command line separated by spaces.

DEPENDENCIES
  None.

PARAMETERS
  uint16 qta_duration_msec - How long simulated QTA should last, i.e. when
                             to send stop QTA.
RETURN VALUE
  None.
===========================================================================*/
extern void wl1_qta_start_qta(uint16 qta_duration_msec);

/*===========================================================================
FUNCTION     WL1_QTA_COMMAND_DONE_ISR

DESCRIPTION
  This function handles the QTA START and STOP done indicatores. from WFW

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_qta_command_done_isr(void);

/*=============================================================================

FUNCTION  eq_drop_eq_for_qta

DESCRIPTION:

 QTA manager calls this function 12ms before QTA enabled all the time without 
 knowledge of either EQ enabled or not If EQ has already enabled EQ controller 
 sends EQ disable command to FW,Deregister the call back with triage,demod and 
 CM.If EQ is not enabled, no need to do anything.
 
DEPENDENCIES:

None

PARAMETERS:

None

RETURN VALUES:

None
=============================================================================*/
extern void  eq_drop_eq_for_qta(void);

/*=============================================================================
FUNCTION void  eq_enable_eq_after_qta
DESCRIPTION:

 QTA manager calls this function after QTA gap completed.EQ controller validates
 weather CM and TD are on or not.If both of them are off  EQ controller sends 
 EQ enable command to FW and register the call back functions with triage,demod
 and CM.otherwise it is in EQ disable state only.

 DEPENDENCIES:
 
 CM and TD both should be off.
 
 PARAMETERS:
 
 None

 RETURN VALUES:
 
 None
=============================================================================*/
extern void  eq_enable_eq_after_qta(void);

/*===========================================================================

FUNCTION     WL1_IS_QTA_ACTIVE

DESCRIPTION  This function checks if QTA is active or not...

DEPENDENCIES
  None.

RETURN VALUE  Boolean.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_is_qta_active(void);

/*===========================================================================
FUNCTION wl1_trm_query_trm_multisim_config_info

DESCRIPTION
  This function is to query trm about the multi-sim configuration. To print UE is configured for SS/DSDS/TSTS/DSDA

DEPENDENCIES
  Assumed to be called after HS has indicated to start logging to DECHS

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern wl1_ds_multi_sim_config_enum_type wl1_ds_query_trm_multisim_config_info(void);


/*===========================================================================

FUNCTION  WL1_IN_QTA_GAP

DESCRIPTION
  This function checks if QTA is active or not...

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_in_qta_gap(void);

/*===========================================================================

FUNCTION  WL1_IS_WFW_IN_QTA_GAP

DESCRIPTION
  This function checks if WFW is in QTA or  not...

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_is_wfw_in_qta_gap(void);

/*===========================================================================

FUNCTION  WL1_IN_QTA_CLEANUP

DESCRIPTION
  This function checks if QTA just ended and CRCs reported are valid.

DEPENDENCIES
  None.

RETURN VALUE
  True - if QTA just ended and CRCs reported are not valid.
  else False.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_in_qta_cleanup(void);
/*===========================================================================

FUNCTION  WL1_IN_QTA_CLEANUP_BCH

DESCRIPTION
  This function checks if QTA just ended and BCH CRCs can be processed.

DEPENDENCIES
  None.

RETURN VALUE
  True - if QTA ended in the last 20 ms and BCH CRC reports are not to be 
  processed.
  else False.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_in_qta_cleanup_for_bch(void);

/*===========================================================================

FUNCTION  WL1_IN_FRAME_AFTER_QTA

DESCRIPTION
  This function checks if this is the first frame after QTA.

DEPENDENCIES
  None.

RETURN VALUE
  True - if QTA ended in the previous frame.
  else False.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_in_frame_after_qta(void);
/*===========================================================================

FUNCTION  WL1_PREPARING_FOR_QTA

DESCRIPTION
  This function checks if there is a QTA preparation going or not..

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_preparing_for_qta(void);

/*===========================================================================

FUNCTION  WL1_QTA_CLEANUP

DESCRIPTION
  This function cleanup QTA related timers and variables if set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_qta_cleanup(void);

/*===========================================================================
FUNCTION     WL1_QTA_UNLOCK_CANCEL_CB

DESCRIPTION
  Post a local command to handle QTA unlock cancel event.  
  Local command should help mitigate any concurrency
  problems with timer and task running in parallel.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_qta_unlock_cancel_cb(void);

/*===========================================================================

FUNCTION  l1m_gsm_rf_nv_init_wait_handler

DESCRIPTION
  This function is callbacek function used for GSM RF NV init to wait
  on the NV init done signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1m_gsm_rf_nv_init_wait_handler(rex_sigs_type sig);

/*===========================================================================

FUNCTION SRCHDCH_FREEZE_SRCH

DESCRIPTION 
    This function is used to freeze the search HB event and make note of the 
    time stamp which is used later to evaluate the elapsed time while resuming.

DEPENDENCIES
  
RETURN VALUE
    None.

SIDE EFFECTS
    Ignore the HB event, so srch_ops_ctl_parms count will not be updated 
    until resumed.
===========================================================================*/
extern void srchdch_freeze_srch(void);

/*===========================================================================

FUNCTION SRCHDCH_UNFREEZE_SRCH

DESCRIPTION 
    This function is used to unfreeze the suspended HB event,calculate the elasped 
    time and increment the srch_ops_ctl_parms like RSSI,ASET SEARCH etc. 
    accordingly.

DEPENDENCIES
  
RETURN VALUE
    None.

SIDE EFFECTS
   The value of srch_ops_ctl_parms will be update according to the elapsed time.
===========================================================================*/
extern void srchdch_unfreeze_srch(void);

/*===========================================================================

FUNCTION     wl1_waitfor_mdsp_download_complete

DESCRIPTION
  This function will wait for mdsp download completion on a TIMEOUT sig.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wl1_waitfor_mdsp_download_complete(void);

/*===========================================================================
FUNCTION     wl1_query_qta_activity

DESCRIPTION
  WL1 subsystems call this function to determine the state of Quick Tuneaway
  (QTA).  QTA is a mode where WCDMA has frozen its state for a brief period,
  ~30 msec, to allow GSM to check its paging occasion.  So during QTA, WL1
  subsystems should forgo significant activities, like mDSP accesses, RF
  accesses, etc., but continue whatever processing will allow them to start
  back up again quickly in the future.

  WL1 subsystems must call this function periodically in order to determine
  current and future QTA status.  And once QTA goes active, WL1 subsystems
  must call this function periodically to determine when QTA has ended.
  At that point, WL1 subsystems should resume normal processing.

DEPENDENCIES
  None.

PARAMETERS
  uint16 *next_qta_cfn  - Return CFN of future QTA gap to caller.  Set to
                          MAX_CFN_COUNT if no known QTA gap pending.
  uint8  *next_qta_slot - Return slot of future QTA gap.  Value undefined
                          if no known QTA gap pending.
RETURN VALUE
  TRUE  - WL1 is currently in the middle of a QTA gap.
  FALSE - WL1 is currently NOT in the middle of a QTA gap.
===========================================================================*/
extern boolean wl1_query_qta_activity(uint16 *, uint16 *);

/*===========================================================================
FUNCTION     wl1_is_next_subfn_in_rota

DESCRIPTION
  WL1 subsystems call this function to determine the state of Quick Burst Tuneaway

DEPENDENCIES
  None.

PARAMETERS
  uint8  next_sbufn - next sub-frame number to check if falls under RoTA Gap

RETURN VALUE
  TRUE  - SubFn's TX fall inside RoTA Gap
  FALSE  - SubFn's Tx will not fall inside RoTA Gap
===========================================================================*/
/*New API for making the qta smaller and dont transmit during rota gap*/
extern boolean wl1_is_next_subfn_in_rota(uint16 eul_next_subfn) ;
                             
/*===========================================================================

FUNCTION  WL1_QTA_CLEANUP_INIT

DESCRIPTION
  This function initializes the variables needed for QTA cleanup.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_qta_cleanup_init(void);

/*===========================================================================

FUNCTION  WL1_QTA_CLEANUP_RESET

DESCRIPTION
  This function resets the variables used for QTA cleanup.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_qta_cleanup_reset(void);
/*===========================================================================
FUNCTION     WL1_QTA_SKIP_OLPC

DESCRIPTION

  This fucntion is called every frame to check if OLPC need to skipped in QTA gap

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Boolean value
     TRUE: SKIP olpc in current frame
     FALSE: Don't skip olpc in current frame
===========================================================================*/
extern boolean wl1_qta_skip_olpc(void);

/*===========================================================================
FUNCTION     WL1_QTA_UPDATE_FRAME_CNT_FOR_CLEANUP

DESCRIPTION
  This function updates frame count variables after QTA stop 
  to know if we are in max TTI gap after QTA

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_qta_update_frame_cnt_for_cleanup(void);

/*===========================================================================
FUNCTION     WL1_QTA_SET_GL1_RF_INFO

DESCRIPTION
  WL1_QTA_WFW_START_QTA
  Gets the free rf/rxlm buffer info from IRAT before start QTA and passes it GERAN

  WL1_QTA_WFW_STOP_QTA
  Call GERAN api to cleanup the buffer

DEPENDENCIES
  None.

PARAMETERS
  qta_flag

RETURN VALUE
  None.
===========================================================================*/

extern void wl1_qta_set_gl1_rf_info(uint16 qta_flag);

#endif  /* #ifdef FEATURE_QTA */

/*===========================================================================
FUNCTION     WL1_DS_TRM_RESERVE_OPS_GTS_IND

DESCRIPTION
  This function must be called when WL1 receives a GO TO SLEEP IND from RRC. 
  It must be called from l1m_process_ext_cmd() only.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_trm_reserve_ops_gts_ind(void);

/*===========================================================================
FUNCTION     WL1_QTA_SET_DL_TDDET_PENDING

DESCRIPTION
  This function sets the flag that indicates a call to dl_cell_check_tddet_status
  is pending, and should be completed after the QTA gap is closed.

DEPENDENCIES
  None

PARAMETERS
  boolean flag - Sets the internal flag to the value provided.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_qta_set_dl_tddet_pending(boolean flag);

/*===========================================================================
FUNCTION     WL1TRM_TUNEAWAY_LOG_PKT_SUBMIT

DESCRIPTION
  This function submits the wl1trm_tuneaway_log_pkt log packet after it is
  allocated memory.

DEPENDENCIES
  Sends the log packet only if it can successfully allocate memory.

PARAMETERS 
  wl1trm_tuneaway_log_data_struct_type *log_data - pointer to the log data
  that needs to be sent out.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1trm_tuneaway_log_pkt_submit(wl1trm_tuneaway_log_data_struct_type *log_data);

/*===========================================================================
FUNCTION     WL1TRM_TUNEAWAY_LOG_SET_TIME

DESCRIPTION
  This function provides a way to store the suspend start, suspend stop, resume
  start and the setup confirmation timeticks into the wl1trm_tuneaway_log_packet's
  data structure.

DEPENDENCIES
  None.
 
PARAMETERS 
  wl1trm_tuneaway_log_set_time_type time_type - Flag to indicate the event.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1trm_tuneaway_log_set_time(wl1trm_tuneaway_log_set_time_type time_type);

/*===========================================================================
FUNCTION     WL1TRM_TUNEAWAY_LOG_SET_L1M_STATE

DESCRIPTION
  This function stores the state of L1M in the wl1trm_tuneaway_log_packet's
  data structure.

DEPENDENCIES
  None.

PARAMETERS 
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1trm_tuneaway_log_set_l1m_state(void);

/*===========================================================================
FUNCTION     WL1TRM_TUNEAWAY_LOG_UPDATE_FACH_QTA_REASON_BITMASK

DESCRIPTION
  This function updates the search reason bitmask with the search that was cancelled
  due to impending QTA or search which cancels the QTA.

DEPENDENCIES
  None.

PARAMETERS 
  uint8 add_new_reason - The active or blocked client that needs to be added to the
  tuneaway log packet.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1trm_tuneaway_log_update_fach_qta_reason_bitmask(uint8 add_new_reason);

/*===========================================================================
FUNCTION     wl1_ds_hdle_cell_decfg_done_cb

DESCRIPTION
  This is  a callback function after the cell de registration is done.

DEPENDENCIES
  None

PARAMETERS
  None  
  
RETURN VALUE  
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_ds_hdle_cell_decfg_done_cb(void);

#ifdef FEATURE_DUAL_SIM_WCDMA_TUNEAWAY

/*===========================================================================
FUNCTION     wl1_read_w2g_tuneaway_config_from_nv

DESCRIPTION
  This function reads W2G tuneaway configuration items from EFS/NV one time
  at WCDMA start-up.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_read_w2g_tuneaway_config_from_nv(void);

/*===========================================================================
FUNCTION     wl1_cqi_zero_timeout_cmd

DESCRIPTION
  Local command handler for L1_CQI_ZERO_TIMEOUT_CMD.  If RRC never suspends
  WL1 for tuneaway for some reason, we want to reset CQI from zero back to
  normal.  In theory this timeout should never happen, but handle just in case.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_cqi_zero_timeout_cmd(void);

/*===========================================================================
FUNCTION     wl1_start_cqi_zero_cmd

DESCRIPTION
  Local command handler for L1_START_CQI_ZERO_CMD.  It is time to override
  CQI to zero in preparation for inter-RAT tuneaway in the near future.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_start_cqi_zero_cmd(void);

/*===========================================================================
FUNCTION     wl1_prepare_for_tuneaway

DESCRIPTION
  This function is called to tell WL1 to prepare for a future inter-RAT
  tuneaway, and to ask WL1 for permission to perform that future tuneaway.

DEPENDENCIES
  None.

PARAMETERS
  uint32 unlock_by_sclk - Future sleep clock timetick value when RRC intends
                          to SUSPEND WL1 and start tuneaway.
RETURN VALUE
  boolean - TRUE if tuneaway request approved.  FALSE if not.
===========================================================================*/
extern boolean wl1_prepare_for_tuneaway(uint32 unlock_by_sclk);

/*===========================================================================
FUNCTION     wl1_return_tuneaway_duration

DESCRIPTION
  This function saves the duration of tuneaway filled by RRC.
  RRC fills the duration into the global tuneaway_duration but might reset it if they start another 
  Suspend WCDMA procedure. So save the value into internal L1 variable and keep.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_read_tuneaway_duration_from_rrc(void);

/*===========================================================================
FUNCTION     wl1_return_tuneaway_duration

DESCRIPTION
  This function returns duration of tuneaway, in frames to the caller.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
Tuneaway duration, in frames.
===========================================================================*/
extern uint16 wl1_return_tuneaway_duration(void);

/*===========================================================================
FUNCTION     wl1_reset_tuneaway_duration

DESCRIPTION
  This function resets duration of tuneaway to zero.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.
===========================================================================*/
extern void wl1_reset_tuneaway_duration(void);
/*===========================================================================
FUNCTION  RRC_IMMEDIATE_PREEMPTION_CB

DESCRIPTION

  This is a call back function to be called in case of pre-emption.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.
===========================================================================*/
extern boolean rrc_immediate_preemption_cb 
(
  /* The client which is being informed of an event */
  trm_client_enum_t               client,

  /* The event being sent to the client */
  trm_unlock_event_enum_t         event,

  /* Sclk timestamp for TRM_UNLOCK_BY */
  uint32                          unlock_by_sclk
);

#endif  /* FEATURE_DUAL_SIM_WCDMA_TUNEAWAY */

/*===========================================================================
FUNCTION  WRM_DRX_EXT_CMD_PROC_RESUME

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
extern void wrm_drx_ext_cmd_proc_resume(void);

extern void wrm_drx_ext_cmd_pend_notf(void);

/*===========================================================================
FUNCTION     WRM_G2W_TA_RESERVE_TRM_LOCK

DESCRIPTION
  This function is used by WCDMA Idle scheduler to reserve TRM lock in G2W TA
  mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wrm_g2w_ta_reserve_trm_lock(uint32 trm_reserve_sclk);

/*===========================================================================
FUNCTION     WRM_SWITCH_G2W_TA_REG_MODE_PROC

DESCRIPTION
  This function is used by WCDMA to switch WRM from G2W TA mode to default
  DSDS mode

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wrm_switch_g2w_ta_reg_mode_proc(void);

extern void wrm_relinquish_alt_client_lock_for_split_acq
(
  /* The client which is attempting to gain complete control of TRM lock */
  wrm_primary_client_enum_type wrm_client_id
);

/*===========================================================================
FUNCTION     WL1_TRM_REQ_SEC_CHAIN

DESCRIPTION
  This is a wrapper function that requests for secondary antenna

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_trm_req_sec_chain(void);

/*===========================================================================
FUNCTION     WL1_DS_HANDLE_TRM_PENDING_SIG

DESCRIPTION
  This function must be called to handle the lock_acq_sig which was
  pended upon getting a TRM grant when WL1 is in paging wakeup.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wl1_ds_handle_trm_pending_sig(void);

/*===========================================================================
FUNCTION     WL1_DS_HANDLE_PENDING_CALL_SETUP

DESCRIPTION
  This function checks if call setup is pending from RRC and puts RF/FW to
  sleep and releases the TRM lock for the chain we are holding. This will 
  facilitate handling of pending RRC cmd in WRM and getting the right chain
  for TX.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_ds_handle_pending_call_setup(void);

/*===========================================================================
FUNCTION wl1_qta_rota_nv_read

DESCRIPTION
  Creates nv and reads it to find whether TX ia allowed or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_qta_rota_nv_read(void);


#else
#define WL1_IN_DS_MODE() (FALSE)
#define WL1_IN_DUAL_ACTIVE_MODE() (FALSE)
#endif

/*=========================================================================
FUNCTION    WL1_DS_FAIL_RESUME_ON_LONG_TUNEAWAY

DESCRIPTION
    This function is called upon resume to check if the tuneaway duration is 
    1. Greater than OOS duration in FACH state
    2. DCH state is not handled currently here as RRC will trigger RLF after 
       ACQ during RESUME if T313+N313 out-of-sync's occured

    For now we support only CELL_FACH state and this function will indicate 
    to fail resume if #1 above is TRUE

PARAMETERS
 
DEPENDENCIES
  None

RETURN VALUE
  Boolean
  True -- Tuneaway duration > OOS duration (CELL_FACH state)
  False -- Otherwise
=========================================================================*/
extern boolean wl1_ds_fail_resume_on_long_tuneaway(void);

/*=========================================================================
FUNCTION    WL1_IS_FACH_QTA_FORCE_RESERV

DESCRIPTION
    This function is used by FRM to know if QTA is force reserved
    and allows/blocks RACH accordingly

PARAMETERS
 
DEPENDENCIES
  None

RETURN VALUE
  Boolean
  True -- QTA is force reserved
  False -- Otherwise
=========================================================================*/
extern boolean wl1_is_fach_qta_force_reserv(void);

#ifdef FEATURE_QSH_DUMP
/*=========================================================================
FUNCTION    wl1_msim_dump_cb

DESCRIPTION
    This function will be called by QSH for dumping out global variables in case of a FATAL

PARAMETERS
   debug_dump_ptr: Pointer to memory location in which to dump the data
DEPENDENCIES
  None

RETURN VALUE
  
=========================================================================*/
void wl1_msim_dump_cb(void* );

#endif /* FEATURE_QSH_DUMP */
#endif /* WL1MULTISIM_H */

