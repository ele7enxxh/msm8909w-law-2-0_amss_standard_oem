
/*===========================================================================

                    L 1   T R M    M A N A G E R

DESCRIPTION
  This file contains global declarations and external references
  for the L1 TRM Manager.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2006 by Qualcomm Technologies, Incorporated. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1trm.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/14   sy      DR DSDS changes
05/21/12   srk     Added code to to disable OLPC in QTA gap
===========================================================================*/
#ifndef TDSTL1TRM_H
#define TDSTL1TRM_H

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "tdsl1const.h"
//#include "customer.h"
//#include "rex.h"
//#include "queue.h"
//#include "tdsl1def.h"
#include "tdsl1mtask.h"
#include "tdsl1rrcif.h"
#include "tdsl1def.h"

#include "trm.h"
#include "wwan_coex_mgr.h"
#include "subs_prio.h"
#include "tdsl1msgr.h"


/*Enum to identify L1 and RRC to TDRM*/
typedef enum {
TDRM_CLIENT_L1,
TDRM_CLIENT_RRC,
TDRM_CLIENT_DIV,
TDRM_CLIENT_MAX
} tdrm_client_enum_type;

/*Enum to identify TDRM state*/
typedef enum {
TDRM_NO_LOCK_STATE,
TDRM_LOCK_REQ_STATE,
TDRM_LOCK_RES_STATE,
TDRM_IN_LOCK_STATE,
TDRM_STATE_MAX
} tdrm_state_enum_type;

/*Enum to identify L1 and RRC state in TDRM*/
typedef enum {
TDRM_CLI_NO_LOCK_STATE,
TDRM_CLI_LOCK_REQ_STATE,
TDRM_CLI_LOCK_RES_STATE,
TDRM_CLI_IN_LOCK_STATE,
TDRM_CLI_STATE_MAX
} tdrm_client_state_enum_type;

/* band change request return type */
typedef enum {
TDRM_BAND_CHANGE_FAIL,
TDRM_BAND_CHANGE_SUCCESS,
TDRM_BAND_CHANGE_WAITING,
TDRM_BAND_CHANGE_NOT_USED
} tdrm_band_change_return_enum_type;

typedef enum {
TDRM_SECONDARY_CLIENT_RXD,
TDRM_SECONDARY_CLIENT_T2L,
TDRM_SECONDARY_CLIENT_TDRM,
TDRM_SECONDARY_CLIENT_MAX
} tdrm_secondary_client_enum_type;

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
typedef void (* tdrm_grant_callback_type)
(
  /* The client which is being informed of an event */
  trm_client_enum_t               client,

  /* The event being sent to the client */
  trm_chain_grant_return_type     grant,

  /* Anonymous payload echoed from trm_request_and_notify() */
  trm_request_tag_t               tag
);

typedef void (* tdrm_change_priority_callback_type)
(
	/* The client which is being informed of the modifiy info result */ 
	trm_client_enum_t client,
	
	/* The status of modify info result */ 
	trm_modify_return_enum_type result
);

typedef void (* tdrm_band_grant_callback_type)
(
  trm_client_enum_t client,    /* Client requested for the band change */
  trm_band_type band,          /* Band class */
  trm_band_grant_return_type granted,   /* Status of band grant */
  trm_request_tag_t tag
);

typedef void (* tdrm_unlock_callback_advanced_t)
(
  trm_unlock_cb_advanced_data_t* unlock_data
);
#endif
/*Structure to hold L1 and RRC specific information in TDRM*/
typedef struct {
tdrm_client_state_enum_type cli_state;

trm_duration_t req_dur;

trm_time_t res_starts_at;

trm_duration_t res_dur;

trm_resource_enum_t req_resource;

trm_reason_enum_t req_reason;

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
tdrm_grant_callback_type grant_cb;
#else
trm_grant_callback_type grant_cb;
#endif

trm_request_tag_t tag;

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
tdrm_band_grant_callback_type band_change_grant_cb;
tdrm_change_priority_callback_type change_priority_cb;
#else
trm_band_grant_callback_type band_change_grant_cb;
#endif

trm_frequency_type_t client_cur_band_info;
trm_frequency_type_t client_new_band_info;
boolean              band_change_waiting_flag;
boolean              tdrm_block_wait_grant_callback;
boolean              trm_lock_grant_callback_result;
} tdrm_client_info_struct_type;

/*Structure to hold TDRM information*/
typedef struct {
tdrm_state_enum_type tdrm_state;
trm_frequency_type_t tdrm_pri_band_info;
trm_frequency_type_t tdrm_sec_band_info;
rfm_device_enum_type tdrm_pri_dev_info;
rfm_device_enum_type tdrm_sec_dev_info;
rfm_device_enum_type multisim_cmd_pri_dev_info;
#ifdef FEATURE_TDSCDMA_TRM_NEW_API
boolean call_init_on_dev_2;
boolean secondary_ready_to_release[TDRM_SECONDARY_CLIENT_MAX];
boolean modify_duration_grant_cb_is_called;
trm_grant_return_enum_type modify_duration_grant;
int8 pending_sec_grant_cb;
tdrm_client_enum_type change_priority_client;
uint8 wakeup_identifier;
#endif
#ifdef FEATURE_TDSCDMA_DR_DSDS
boolean in_dr_region;
#endif
tdrm_client_info_struct_type tdrm_client_info[TDRM_CLIENT_MAX];
boolean unlock_by_mode_change;
} tdrm_db_struct_type;

#ifdef FEATURE_TDSCDMA_DR_DSDS
extern rex_timer_type tdrm_unlock_timer;
#endif

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
/* TDS_L1_TDRM_GRANT_CALLBACK_CMD structure. */
typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   hdr;

  /* The client which needs the RF resource */
  trm_client_enum_t       client;

  /* The data in the grant callback */
  trm_req_n_not_return_data data;

} tdsl1_dsds_trm_grant_callback_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   hdr;

  /* The client which needs the RF resource */
  trm_client_enum_t       client;

  /* The data in the grant callback */
  trm_req_n_not_return_data data;

} tdsl1_dsds_trm_secondary_grant_callback_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   hdr;

  /* The client which needs the RF resource */
  trm_client_enum_t       client;

  /* Modify reason result */
  trm_modify_reason_return_type result;

} tdsl1_dsds_trm_modify_reason_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   hdr;

  /* The client which needs the RF resource */
  trm_client_enum_t       client;

  /* Modify reason result */
  trm_modify_band_return_type result;

  /* The tdrm_client_id that TDRM passed to TRM earlier */
  trm_request_tag_t tag;

} tdsl1_dsds_trm_modify_band_cmd_type;

typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   hdr;

  /* The client which needs the RF resource */
  trm_client_enum_t       client;

  /* Unlock callback result */
  trm_unlock_callback_data data;

} tdsl1_dsds_trm_unlock_cmd_type;

#else
/* TDS_L1_TDRM_GRANT_CALLBACK_CMD structure. */
typedef struct
{
  /* Need this to put command on l1_local_cmd_q. */
  tdsl1def_l1_local_cmd_hdr_type   hdr;

  /* The client which needs the RF resource */
  trm_client_enum_t       client;

  /*The granted Chain*/
  trm_chain_grant_return_type  event;

  /*The tag that TDRM passed to TRM in trm_request_and_notify(...)*/
  trm_request_tag_t       tag;

} tdsl1_dsds_trm_grant_callback_cmd_type;
#endif

extern tdrm_db_struct_type tdrm_db;

/*Identity of WRM/WCDMA with TRM*/
#define TDRM_TRM_CLIENT_IDENTITY TRM_TDSCDMA

/*Initialization macros*/
#define TDRM_INVALID_TIME_DURATION 0
#define TDRM_INVALID_RES_AT_TIME 0
#define TDRM_INVALID_TAG 0xFF

/*Macros to check validity of TDRM and TDRM_TRM client identities*/
#define TDRM_IS_CLIENT_ID_VALID(tdrm_client_id) (tdrm_client_id < TDRM_CLIENT_MAX)
#define TDRM_IS_TRM_CLIENT_ID_VALID(trm_client_id) (trm_client_id == TDRM_TRM_CLIENT_IDENTITY)

/*Macros to check the state of TDRM*/
#define TDRM_STATE_IS_IN_LOCK() (tdrm_db.tdrm_state == TDRM_IN_LOCK_STATE)
#define TDRM_STATE_IS_LOCK_REQ() (tdrm_db.tdrm_state == TDRM_LOCK_REQ_STATE)
#define TDRM_STATE_IS_LOCK_RES() (tdrm_db.tdrm_state == TDRM_LOCK_RES_STATE)
#define TDRM_STATE_IS_NO_LOCK() (tdrm_db.tdrm_state == TDRM_NO_LOCK_STATE)

/*Macros to check the state of TDRM clients*/
#define TDRM_CLI_STATE_IS_IN_LOCK(tdrm_client_id) (tdrm_db.tdrm_client_info[tdrm_client_id].cli_state == TDRM_CLI_IN_LOCK_STATE)
#define TDRM_CLI_STATE_IS_LOCK_REQ(tdrm_client_id) (tdrm_db.tdrm_client_info[tdrm_client_id].cli_state == TDRM_CLI_LOCK_REQ_STATE)
#define TDRM_CLI_STATE_IS_LOCK_RES(tdrm_client_id) (tdrm_db.tdrm_client_info[tdrm_client_id].cli_state == TDRM_CLI_LOCK_RES_STATE)
#define TDRM_CLI_STATE_IS_NO_LOCK(tdrm_client_id) (tdrm_db.tdrm_client_info[tdrm_client_id].cli_state == TDRM_CLI_NO_LOCK_STATE)

/*Macros to get and set the state of TDRM*/
#define TDRM_SET_TDRM_STATE(state_of_tdrm) (tdrm_db.tdrm_state = state_of_tdrm)
#define TDRM_GET_TDRM_STATE() (tdrm_db.tdrm_state)

#ifdef FEATURE_TDSCDMA_DR_DSDS
/*Macros to get device ID info*/
#define TDRM_GET_PRI_DEVICE_ID() (tdrm_db.tdrm_pri_dev_info)
#define TDRM_GET_SEC_DEVICE_ID() (tdrm_db.tdrm_sec_dev_info)
#endif

/*Macros to get and set the state of TDRM client*/
#define TDRM_SET_TDRM_CLI_STATE(tdrm_client_id, tdrm_cli_state) (tdrm_db.tdrm_client_info[tdrm_client_id].cli_state = tdrm_cli_state)
#define TDRM_GET_TDRM_CLI_STATE(tdrm_client_id) (tdrm_db.tdrm_client_info[tdrm_client_id].cli_state)

/*Macros to get alternate TDRM client id*/
#define TDRM_GET_ALTERNATE_CLIENT_ID(tdrm_client_id) (tdrm_client_enum_type)((tdrm_client_id + (tdrm_client_enum_type)1) % TDRM_CLIENT_MAX)

#define TDRM_GET_TAG_FROM_CLI_ID(tdrm_client_id) (tdrm_db.tdrm_client_info[tdrm_client_id].tag)


/* TRM change priority timer */
#define TDRM_BAND_CHANGE_TIMEOUT_MSEC 1000

/* Delay 33 * 5 sclk (32.768 sclk = 1ms) */
#define TDRM_RXD_SECONDARY_CHAIN_REQUEST_DELAY 33*5

/* Maximum time for TRM to grant access to TD-SCDMA for the primary antenna */
#define TDRM_REQUEST_TIMEOUT_MSEC 5000
/* Initial duration of trm request for primary antenna */
#define TDRM_RXTX_MINIMUM_DURATION  TIMETICK_SCLK_FROM_MS(150)

#define TDSL1_DEFAULT_GRANT_CB (tdsl1_ds_grant_cb)

#define T2L_TO_RELEASE_TDRM_SEC_CHAIN_THRESHOLD_CM 20 //5ms
#define T2L_TO_RELEASE_TDRM_SEC_CHAIN_THRESHOLD_IDLE 40 //40ms
#define ONE_TDS_SUBFM_DURATION_IN_MS   5

#define TAG_RRC_REQUEST_LOCK_FOR_FS_ACQ 1
#define TAG_RRC_REQUEST_LOCK_NOT_FOR_FS_ACQ 0

/*TDSL1 dual sim procedures*/
typedef enum {
  TDSL1_DS_PROC_START=0,
  TDSL1_DS_PROC_SUSPEND=1,
  TDSL1_DS_PROC_RESUME=2,
  TDSL1_DS_PROC_IDLE=3,
  TDSL1_DS_PROC_STOP=4,
  TDSL1_DS_PROC_DEACTIVATE=5,
  TDSL1_DS_PROC_FS=6,
  TDSL1_DS_PROC_ACQ=7,
  TDSL1_DS_PROC_SETUP_SBCH=8,
  TDSL1_DS_PROC_SETUP_NBCH_IN_DRX=9,
  TDSL1_DS_PROC_SETUP_PCH=10,
  TDSL1_DS_PROC_SETUP_FACH=11,
  TDSL1_DS_PROC_SETUP_NBCH_IN_FACH=12,
  TDSL1_DS_PROC_SETUP_DCH=13,
  TDSL1_DS_PROC_PAGING=14,
  TDSL1_DS_PROC_CELL_RESEL_START=15,
  TDSL1_DS_PROC_CELL_TRANS_REQ=16,
  TDSL1_DS_PROC_CHANNEL_ACTIVE=17,
  TDSL1_DS_PROC_DROP_ALL=18,
  TDSL1_DS_PROC_BPLMN=19,
  TDSL1_DS_PROC_MAX=20
}tdsl1_ds_proc_enum_type;

/*TDSL1 dual sim paging specific information structure*/
typedef struct {
  boolean lock_succ;

  uint16 declare_oos_thresh;
  uint16 declare_oos_thresh_once_s_failure;
  uint16 dwpts_reacq_thresh;

  uint32 trm_lock_consecutive_fail_cnt;

  boolean cell_trans_req_stat;

  boolean tdsl1_in_paging_wakeup;

  boolean lock_fail_at_wakeup;
  boolean declare_oos_once_s_failure;
  boolean dwpts_reacq_after_long_gap;
} tdsl1_ds_paging_spec_info_struct_type;


typedef enum
{
  TDSL1_QTA_STATE_NOT_ACTIVE,
  TDSL1_QTA_STATE_WAIT_TO_START_QTA,
  TDSL1_QTA_STATE_DIS_MEAS_UPA,
  TDSL1_QTA_STATE_INFORM_GL1,
  TDSL1_QTA_STATE_ACTIVE,
  TDSL1_QTA_STATE_QTA_STOPPED,
  TDSL1_QTA_STATE_MAX
} tdsl1_qta_state_enum_type;

typedef enum
{
  TDSL1_QTA_TSN_0 = 0,
  TDSL1_QTA_TSN_SPECIAL,  
  TDSL1_QTA_TSN_1,
  TDSL1_QTA_TSN_2,
  TDSL1_QTA_TSN_3,
  TDSL1_QTA_TSN_4,
  TDSL1_QTA_TSN_5,
  TDSL1_QTA_TSN_6,
  TDSL1_QTA_TSN_INVALID
}tdsl1_qta_tsn_enum_type;

typedef enum
{
  ACQ_INV_TIMER_DEACT = 0,
  ACQ_INV_TIMER_SET_FOR_G2T,
  ACQ_INV_TIMER_SET_FOR_NON_G2T
}tdsl1_acq_inv_timer_state_enum_type;


typedef struct {
/*Range is 0-8191*/
  uint32 sub_fn;

/*Range is 0-51199*/
  uint32 cx8_offset;
  
  tdsl1_qta_tsn_enum_type tsn; /* 0~7 */
  
} tdsl1_qta_time_info_struct_type;


/*Structure to hold QTA Log Packet info */

typedef struct
{
  /* Word 0&1*/
  /*! \brief sleep clock */    
  int64 cur_timetick;
  /* Word 2*/
  /*! \brief rx_sfn */
  uint16 rx_sfn;
  /*! \brief rx_cx8 */
  uint16 rx_cx8;
}tdsl1_ds_qta_time_struct_type;

typedef struct {
  /* word 1~3 */
  /*! \brief QTA start time */
  tdsl1_ds_qta_time_struct_type qta_start_time;
  /* word 4~6 */
  /*! \brief QTA end time */
  tdsl1_ds_qta_time_struct_type qta_end_time;  

  /* word 7~8 */
  /*! \brief QTA duration(QTA end time ¡§C QTA start time) in sleep clock, 32kHz */
  int64 qta_l1_duration_in_slk;
  /* word 9 */
  /*! \brief QTA duration(QTA end time ¡§C QTA start time) in ms */
  uint16 qta_l1_duration_in_ms;
  /*! \brief QTA duration(cxmLockLostTime ¡§C cxmLockAcqTime) in ms */
  uint16 qta_fw_duration_in_ms;
  
  /* word 11 */
  /*! \brief L1M state when QTA happen(DCH:7 or FACH:6) */
  uint8 l1m_state_when_qta;
  /*! \brief HSDPA HARQ flush after QTA (TRUE or FALSE) */
  uint8 dpa_harq_flash_flag;
  /*! \brief Re-ACQ complete indicator after QTA (TRUE or FALSE) */
  uint8 reacq_complete_ind;
  uint8 pad2;
}tdsl1_ds_qta_log_packet_struct;

//For SW DA, the DA mode starts from power up. For HW DA, the DA mode only for FACH and DCH once we have tx for the possible tx bkoff.
#if defined (FEATURE_TDSCDMA_DR_DSDS) || defined (FEATURE_TDSCDMA_TX_SHARING_DSDA)
#define TDSL1_IN_DSDA_MODE()  (((tdssrchsglte.meas_mode == TDSL1_MEAS_MODE_NORMAL) && (tdsl1_ds_db.tdsl1_in_da_mode)) || \
		  (((tdsl1m_global.l1_state == TDSL1M_FACH) || (tdsl1m_global.l1_state == TDSL1M_DCH)) && \
		   (tdssrchsglte.meas_mode == TDSL1_MEAS_MODE_SGLTE) && (tdsl1_ds_db.tdsl1_in_da_mode)))
#else
#define TDSL1_IN_DSDA_MODE()  (((tdsl1m_global.l1_state == TDSL1M_FACH) || (tdsl1m_global.l1_state == TDSL1M_DCH)) && (tdsl1_ds_db.tdsl1_in_da_mode)) 
#endif

typedef struct
{  
  uint32 crc_error_counter_one_cycle;
  uint32 crc_total_counter_one_cycle;
  uint32 crc_error_counter;
  uint32 crc_total_counter;
  uint32 desense_glitch_counter;
  uint32 desense_glitch_total_counter;
  uint32 lack_of_RxD_duration;
  uint32 RxD_on_total_duration;  //in number of DRX cycles  
  uint32 lack_of_RxD_rate;
  uint32 crc_error_rate;
  uint32 desense_glitch_rate;
  uint32 eval_drx_cycle_counter;
  boolean tds_to_trigger_fb;
  boolean tds_fb_in_legacy_ds;
  boolean pch_without_pich;
} tdsl1_drds_legacy_fb_metrics_type;


typedef struct
{      
  /*the flags when secondary release request is pending*/
   boolean chain_release_request_in_t2l_meas;
   boolean chain_release_request_to_fb;
   uint16 chain_release_by_subfm;
   uint16 chain_release_by_ms;
   boolean secondary_chain_is_granted;
   boolean t2l_to_release_rightaway;
   uint32 tid;
   trm_band_t cur_lte_band;
   trm_band_t next_lte_band;
} tdsl1_drds_secondary_chain_status_type;


/*Structure to hold TDSL1 dual sim information */
typedef struct {
  /*Whether TDSL1 is initializing RF and mdsp*/
  boolean tdsl1_in_ds_prep_mode;

  /*True: UE is in DS mode*/
  boolean tdsl1_in_ds_mode;
  
  /*True: UE is in dual active mode*/
  /*False && tdsl1_in_ds_mode == FALSE : US in the single sim mode*/
  boolean tdsl1_in_da_mode;  

  /* To indicate the UE capability
    TRUE: in dual Rx DSDS, the secondary chin could be released
       FALSE: in legacy DSDS, hold the both chains*/
  boolean tdsl1_in_dr_dsds;
  
   /*the mode indication from TRM*/
  uint32 trm_mode;
  
  /*trm single tx dsda*/
  boolean tx_sharing_da_mode;
 
  tdsl1_drds_legacy_fb_metrics_type tdsl1_dr_ds_fb_metrics;
  tdsl1_drds_secondary_chain_status_type tdsl1_dr_sec_chain_status;
 
  /*which G got the LOCK*/
  trm_client_info_t winning_client;
  /*the current client for unlock request*/
  trm_client_info_t current_client;
  
  /* device mode from RRC*/
  tdsl1_ds_status_change_ind_type tdsl1_ds_status;

  /* pending device mode if it's not at the stopped mode*/
  tdsl1_ds_status_change_ind_type tdsl1_pending_ds_cmd;
  
  /*pending_mode flag
     TRUE: the device mode has been applied
     FALSE: the device mode is pending, to be applied at the stopped mode*/
  boolean device_mode_config_is_pending;

  /*debug mask to track dual sim status*/
  uint32 tdsl1_ds_debug_mask;

  /*TDSL1 procedures that are holding TRM lock*/
  uint32 tdsl1_ds_proc_mask;

  /*Last TDSL1 procedure that requested for TRM lock*/
  tdsl1_ds_proc_enum_type ds_curr_proc;

  /*Last TDSL1 procedure that requested for TRM lock*/
  tdsl1_ds_proc_enum_type ds_pend_proc;

  /*Last cphy_setup_cmd procedure that requested for TRM lock*/
  tdsl1_ds_proc_enum_type last_cphy_setup_proc;

  /*Indicate whether cmd is pending waiting for the sleep proc to be completed*/
  boolean trm_lock_sig_pending_on_sleep;

  /* Used to store the TRM lock signal (success/failure) in
    case TRM signal is received in the middle of paging wakeup;
    only applicable if trm_lock_sig_pending_on_sleep = TRUE */
  rex_sigs_type trm_lock_sig;

  /* Flag to indicate Ext cmd processing with no TRM lock */
  boolean process_ext_cmd_without_lock;
  
  tdsl1_qta_state_enum_type tdsl1_qta_sub_state;
  uint32  tdsl1_qta_start_time_in_sclk; /* QTA start timestamp */
  tdsl1_qta_time_info_struct_type tdsl1_qta_start_time;
  
  boolean qta_suspend_srch_demod_in_ho_flag;/* disable during_dl_sync_flag during QTA, if QTA happens during HO and before DL sends ESTABLISH_IND */
  
  sys_modem_as_id_e_type qta_g_asid; /*T2G QTA G sub ASID*/
  /* To time different states of inter-RAT tuneaway. */
  rex_timer_type     l1m_dsds_tuneaway_timer;
  /* CxM priority table */
  cxm_activity_table_s   tdsl1dsds_qta_cxm_priority_table[MAX_ACTIVITY_TIERS];
  boolean                tdsl1dsds_qta_cxm_priority_table_update_flag;
  boolean                qta_fw_cleanup_ind_recvd;

  /* QTA log packet */
  tdsl1_ds_qta_log_packet_struct  tdsl1dsds_qta_log_pkt;
  uint8                  qta_end_rrc_fw_cmd_rec_flag;
  uint8                  pad[3];
  
#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY  
  boolean                  l1m_g2t_ta_prio_inv_flag;/* L1M trigger TRM priority inversion when receives RRC CMD */
#endif
  tdsl1_acq_inv_timer_state_enum_type  acq_inv_timer_set_flag; //acq inverse timer set flag; 
  boolean                              acq_inv_flag;
  
  uint16     tuneaway_duration;
  boolean    need_to_skip_reacq;
  
}tdsl1_ds_db_struct_type;

/*extern tdsl1_ds_db_struct_type tdsl1_ds_db;*/

  /*Macros to handle TDSL1 dual sim procedure bit mask*/
  #define TDSL1_DS_PROC_BMSK(ds_proc) ((uint32)(1 << ((uint32)ds_proc)))
  
  #define TDSL1_DS_RESET_DS_PROC_BMASK() \
  (tdsl1_ds_db.tdsl1_ds_proc_mask = 0)
  
  #define TDSL1_DS_ADD_PROC_TO_DS_BMASK(ds_proc) \
    (tdsl1_ds_db.tdsl1_ds_proc_mask |= TDSL1_DS_PROC_BMSK(ds_proc))
  
  #define TDSL1_DS_REM_PROC_FROM_DS_BMASK(ds_proc) \
    (tdsl1_ds_db.tdsl1_ds_proc_mask &= (~(TDSL1_DS_PROC_BMSK(ds_proc))))
/*****************************************************************************************************************/
/*   Global Variable defination    */
/*****************************************************************************************************************/
/*Information about current TDSL1 Dual SIM procedure*/
extern  tdsl1_ds_db_struct_type tdsl1_ds_db;








/*****************************************************************************************************************/
/*   Function declaration    */
/*****************************************************************************************************************/
void tdrm_request_and_notify(
  /* The client which needs the RF resource */
  tdrm_client_enum_type tdrm_client_id,
  /* The RF resource which is being requested */
  trm_resource_enum_t             resource,
  /* How long the resource will be needed for (in sclks) */
  trm_duration_t                  duration,
  /* Why the resource is needed (used for priority decisions) */
  trm_reason_enum_t               reason,
#ifdef FEATURE_TDSCDMA_TRM_NEW_API
  /* Callback to notify client when resource is granted */
  tdrm_grant_callback_type		grant_callback,
#else
  /* Callback to notify client when resource is granted */
  trm_grant_callback_type		grant_callback,
#endif
  /* Anonymous payload to be echoed through grant_callback() */
  trm_request_tag_t               tag,
  trm_frequency_type_t            tdrm_cur_band);


void tdrm_retain_lock(
  /* The client which needs the RF resource */
  tdrm_client_enum_type tdrm_client_id,
  /* Function to call when lock must be given up. */
#ifdef FEATURE_TDSCDMA_TRM_NEW_API
  tdrm_unlock_callback_advanced_t           unlock_callback);
#else
  trm_unlock_callback_advanced_t           unlock_callback);
#endif


void tdrm_grant_callback(
  /* The client which needs the RF resource */
  trm_client_enum_t client, 
  /*The granted Chain*/
  trm_chain_grant_return_type event,
  /*The tag that TDRM passed to TRM in trm_request_and_notify(...)*/
  trm_request_tag_t tag);

void tdrm_grant_callback_cmd_handler(
  /* The client which needs the RF resource */
  trm_client_enum_t client, 

  /*The granted Chain*/
  trm_chain_grant_return_type event,

  /*The tag that TDRM passed to TRM in trm_request_and_notify(...)*/
  trm_request_tag_t tag);

void tdrm_reserve_at(
  /* The client which needs the RF resource */
  tdrm_client_enum_type               tdrm_client_id,
  /* The RF resource which is being requested */
  trm_resource_enum_t             resource,
  /* When the resource will be needed (sclks timestamp) */
  trm_time_t                      when,
  /* How long the resource will be needed for (in sclks) */
  trm_duration_t                  duration,
  /* Why the resource is needed (used for priority decisions) */
  trm_reason_enum_t               reason,
  trm_frequency_type_t            tdrm_cur_band,
  /* a boolean flag to indicate whether to increase wakeup identifier in TDRM<->TRM API */
  boolean                         increase_wakeup_identifier);

trm_grant_event_enum_t tdrm_request(
  /* The client which needs the RF resource */
  tdrm_client_enum_type tdrm_client_id,
  /* The transceiver resource which is being requested */
  trm_resource_enum_t             resource,
  /* How long the resource will be needed for (in sclks) */
  trm_duration_t                  duration,
  /* Why the resource is needed (used for priority decisions) */
  trm_reason_enum_t               reason,
  trm_frequency_type_t            tdrm_cur_band);

void tdrm_change_priority(
  /* The client whose priority is to be changed */
  tdrm_client_enum_type               tdrm_client_id,
  /* The new resource why the RF lock is held (used for priority decisions) */
  trm_reason_enum_t               reason
#ifdef FEATURE_TDSCDMA_TRM_NEW_API
, tdrm_change_priority_callback_type change_priority_cb
#endif
  );

void tdrm_release(tdrm_client_enum_type tdrm_client_id);

boolean tdrm_extend_duration
(
  /* The client which is attempting to extend the lock duration */
  tdrm_client_enum_type tdrm_client_id,
  /* The required extension, in sclks, from "now" */
  trm_duration_t                  new_duration);

void tdrm_unlock_trm_cb(
 trm_unlock_cb_advanced_data_t* unlock_data);

void tdrm_unlock_secondary_trm_cb(
  /* The client which is being informed of an event */
  trm_client_enum_t               client,

  /* The event being sent to the client */
  trm_unlock_event_enum_t         event,

  /* Sclk timestamp for TRM_UNLOCK_BY */
  uint32                          unlock_by_sclk);


void tdrm_init(void);

boolean tdrm_check_cli_state_is_in_lock(tdrm_client_enum_type tdrm_client_id);
boolean tdrm_check_cli_state_is_lock_req(tdrm_client_enum_type tdrm_client_id);

trm_band_t tdrm_get_band(uint16  freq);
boolean tdrm_get_ext_cmd_band_info(tdsl1_ext_cmd_type*  ext_cmd_ptr, trm_band_t curr_band_info, trm_band_t* new_band_info);
void tdrm_band_grant_callback(trm_client_enum_t client, 
                              trm_band_type band,          
                              trm_band_grant_return_type granted,   
                              trm_request_tag_t tag);
tdrm_band_change_return_enum_type tdrm_band_change_request(tdrm_client_enum_type          tdrm_client_id,      /* Client requesting a band change */
                                                           trm_frequency_type_t       tds_new_band_info,         /* New band that the client wants to use */
#ifdef FEATURE_TDSCDMA_TRM_NEW_API
                                                           tdrm_band_grant_callback_type	tds_band_grant_cb,	/* Band Grant Callback */
#else
                                                           trm_band_grant_callback_type  tds_band_grant_cb,  /* Band Grant Callback */
#endif
                                                           trm_request_tag_t tag);

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
void tdrm_register_async_callback(void);

void tdrm_async_callback(trm_async_callback_information_type* event_data);

void tdrm_primary_grant_callback(trm_client_enum_t client, trm_req_n_not_return_data data);

void tdrm_primary_grant_callback_cmd_handler(trm_client_enum_t client, trm_req_n_not_return_data data);

void tdrm_secondary_grant_callback(trm_client_enum_t client, trm_req_n_not_return_data data);

void tdrm_secondary_grant_callback_cmd_handler(trm_client_enum_t client, trm_req_n_not_return_data data);

void tdrm_unlock_secondary_cb_for_retain_lock(trm_unlock_cb_advanced_data_t* unlock_data);

void tdrm_unlock_secondary_cb(trm_client_enum_t client, trm_unlock_callback_data data);

void tdrm_unlock_secondary_cmd_handler(trm_client_enum_t client, trm_unlock_callback_data data);

void tdrm_secondary_client_is_ready_to_release(tdrm_secondary_client_enum_type client);

void tdrm_get_info(trm_get_info_input_type* get_info_input, trm_get_info_return_type* get_info_return);

void tdrm_register_feature(trm_register_feature_input_info* register_info);

void tdrm_deregister_feature(trm_deregister_feature_input_info* deregister_info);

void tdrm_set_client_state(trm_set_client_state_input_type* input_client_state, trm_set_client_state_output_data* output_client_state);

void tdrm_change_priority_callback(trm_client_enum_t client, trm_modify_return_enum_type result);

void tdrm_send_reserve_at_to_trm(trm_client_enum_t client, trm_resource_enum_t resource, trm_time_t when, trm_duration_t duration, trm_reason_enum_t reason, trm_band_t band);

void tdrm_send_request_and_notify_enhanced_to_trm(trm_client_enum_t client, trm_resource_enum_t resource, trm_duration_t duration, trm_reason_enum_t reason, trm_band_t band);

trm_grant_event_enum_t tdrm_send_request_to_trm(trm_client_enum_t client, trm_resource_enum_t resource, trm_duration_t duration, trm_reason_enum_t reason, trm_band_t band);

#ifdef FEATURE_TDSCDMA_DR_DSDS
void tdrm_dr_region_update(trm_reason_enum_t new_reason);
void tdrm_unlock_timer_expire(uint32 unused);
#endif

#endif

#ifdef  FEATURE_PBR_MARKOV
/*===========================================================================
FUNCTION     tdrm_set_drx_cycle

DESCRIPTION
  This function set the pbr modes to TRM

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdrm_set_pbr_modes(trm_reason_enum_t req_reason);

/*===========================================================================
FUNCTION     tdrm_set_drx_cycle

DESCRIPTION
  This function send the drx cycle to TRM.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdrm_set_drx_cycle(uint32 drx_cycle_len);
#endif

/*===========================================================================
FUNCTION     tdrm_exchange

DESCRIPTION
  This function is called to exchange lock between TD and other IRAT .

DEPENDENCIES
  None.

PARAMETERS
  BAND information.

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
trm_grant_event_enum_t tdrm_exchange(
  /* The client which needs the RF resource */
  tdrm_client_enum_type  tdrm_client_id,
  /* The client which TD will exchange */
  trm_client_enum_t    client_id);


extern void tdsrrc_start_unlock_by_timer_for_tune_away
(
  /* Sclk timestamp for TRM_UNLOCK_BY */
  uint32                          unlock_by_sclk
);

extern void tdsrrc_event_log_dsds_ta_block
(
  void
);
extern void tdsrrc_set_ta_block_reason
(
  tdsrrc_tuneaway_block_reason_type block_reason
);

extern void tdsrrcwrm_release_lock(void);

extern boolean tdrm_is_next_tuneaway_qta(void);

extern trm_reason_enum_t tdrm_get_reason(void);
  
extern void tdrm_sync_reason(trm_reason_enum_t reason);

extern void tdrm_unsync_reason(void);

extern trm_reason_enum_t tdrm_get_client_req_reason(tdrm_client_enum_type tdrm_client_id);

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
extern void tdrm_modify_reason_cmd_handler(trm_client_enum_t client, trm_modify_reason_return_type result);

extern void tdrm_modify_band_cmd_handler(trm_client_enum_t client, trm_modify_band_return_type result);
extern boolean tdrm_check_trm_client_is_in_lock(trm_client_enum_t trm_client);
#endif

extern boolean tdrm_is_next_tuneaway_page_decode(void);

#endif /* #ifndef TDSTL1TRM_H */






