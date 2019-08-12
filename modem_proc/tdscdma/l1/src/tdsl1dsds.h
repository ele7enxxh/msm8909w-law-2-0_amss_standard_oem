
/*===========================================================================

                    L 1   D S D S    M A N A G E R

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1dsds.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/14   sy      DR DSDS changes
02/27/14   yufu    SGTDS +G DSDA mode settings
05/21/12   srk     Added code to to disable OLPC in QTA gap
===========================================================================*/
#ifndef TDSTL1MDSDS_H
#define TDSTL1MDSDS_H
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "custtdscdma.h"
//#ifdef FEATURE_TDSCDMA_DSDS  /*FEATURE_DUAL_SIM*/

#include "tdsl1trm.h"
#include "tdsl1m.h"
//#include "tdsl1msetup.h"
//#include "tdsl1mcmd.h"
#include "tdsdrx.h"
#include "tdsl1rrcif.h"

//#endif /* FEATURE_TDSCDMA_DSDS */


#ifdef FEATURE_TDSCDMA_DSDS /*FEATURE_DUAL_SIM*/
#define TDSL1_IN_DS_MODE() (tdsl1_ds_db.tdsl1_in_ds_mode)
#else
#define TDSL1_IN_DS_MODE() (FALSE)
#endif

#ifdef FEATURE_TDSCDMA_DR_DSDS
#define TDSL1_IN_DR_MODE() (tdsl1_ds_db.tdsl1_in_dr_dsds)
#endif

#define DS_SLEEP_CLK_NOMINAL_mHZ      (32765442)
#define M_BY_N_MS_IN_SC(m,n)  (1 + ((m) * (DS_SLEEP_CLK_NOMINAL_mHZ/10000)/((n) * 100))) 
#define TDSL1_PAGING_REQ_DURATION 20


#ifdef FEATURE_TDSCDMA_DSDS  /*FEATURE_DUAL_SIM*/

typedef void TDSL1_DS_PROC_CB_PTR(void);


/*Structure to hold TDSL1 dual sim information on a per procedure basis*/
typedef struct {
  /*Process the RRC command even if lock failed,
  but without mdsp and RF*/
  boolean can_lock_fail;

  /* The RF resource which is being requested */
  trm_resource_enum_t resource_needed;

  /* How long the resource will be needed for (in milli secs) */
  uint32 lock_req_dur_in_ms;

  /* Why the resource is needed (used for priority decisions) */
  trm_reason_enum_t lock_reason;

  /* Callback to notify client when resource is granted */
  TDSL1_DS_PROC_CB_PTR *lock_grant_callback;

  uint32 lock_req_timeout_in_ms;

  uint32 trm_lock_proc_fail_cnt;

  uint32 trm_lock_proc_succ_cnt;

  tdsl1_ext_cmd_type *ext_cmd_ptr;

  tdsl1_cmd_enum_type rrc_to_l1_cmd;

  tdsrrc_l1_cmd_e_type l1_to_rrc_cnf;
}tdsl1_ds_proc_info_struct_type;

/*Structure to hold TDSL1 dual sim statistics*/
typedef struct {
  uint32 lock_req_and_not_cnt;

  uint32 lock_req_and_not_succ_cnt;

  uint32 lock_req_and_not_fail_cnt;

  uint32 lock_req_cnt;

  uint32 lock_req_succ_cnt;

  uint32 lock_req_fail_cnt;

  uint32 lock_res_cnt;
} tdsl1_ds_overall_stat_struct_type;

/*****************************************************************************************************************/
/*   Global Variable defination    */
/*****************************************************************************************************************/

/*Paging specific Information when in Dual SIM mode*/
extern tdsl1_ds_paging_spec_info_struct_type tdsl1_ds_paging_spec_info;

/*Run time db storing the various params pertaining to TDSL1 procedure*/
extern tdsl1_ds_proc_info_struct_type tdsl1_ds_proc_info[TDSL1_DS_PROC_MAX];

/*TRM lock acquisition success/failure statistics when in Dual SIM mode*/
extern tdsl1_ds_overall_stat_struct_type tdsl1_ds_overall_stat;

extern boolean tdsl1_ds_hold_trm_lock_in_sib_gap;

#ifdef FEATURE_TDSCDMA_LOW_SIB_PRIORITY_DUR_CELL_SEL
extern rex_timer_type tdsl1_ds_extend_lock_timer;
extern rex_timer_type tdsl1_ds_1st_6sec_sib_timer;
extern boolean tdsl1_ds_extend_lock_for_bch_fail;
extern boolean tdsl1_ds_1st_6sec_sib_reading;
extern boolean tdsl1_ds_already_sleep_dur_sib_reading;

#define TDSL1DS_PERIOD_OF_EXTEND_LOCK_FOR_CELL_SEL_BCH (40) // 2 BCH TTIs
#define TDSL1DS_DURATION_OF_LOCK_FOR_CELL_SEL_BCH (50) // 2 BCH TTIs + 10 msec overhead for channel setup/drop
#endif

/*****************************************************************************************************************/
/*   MACRO defination    */
/*****************************************************************************************************************/
#define TDSL1_DS_INVALID_CMD_IDX 0xFF
/*If TRM lock request fails consecutively for this threshold count, then OOS
is declared to RRC*/
#define TDSL1_DS_PAG_TRM_LOCK_OOS_THRESH (8)

/*Macros to check whethe a procedure is valid and of particular type*/
#define TDSL1_DS_IS_PROC_VALID(ds_proc) (ds_proc < TDSL1_DS_PROC_MAX)

#define TDSL1_DS_IS_PROC_VALID_FOR_RESERVE(ds_proc) (ds_proc == TDSL1_DS_PROC_PAGING)

#define TDSL1_DS_IS_PROC_FOR_SETUP(ds_proc) ((ds_proc == TDSL1_DS_PROC_SETUP_SBCH) || \
  (ds_proc == TDSL1_DS_PROC_SETUP_NBCH_IN_DRX) || (ds_proc == TDSL1_DS_PROC_SETUP_PCH) || \
  (ds_proc == TDSL1_DS_PROC_SETUP_FACH) || (ds_proc == TDSL1_DS_PROC_SETUP_DCH) || \
  (ds_proc == TDSL1_DS_PROC_SETUP_NBCH_IN_FACH))


/*Macros to handle statistics corresponding to a procedure*/
#define TDSL1_DS_INC_FAIL_CNT_FOR_PROC(ds_proc) (tdsl1_ds_proc_info[ds_proc].trm_lock_proc_fail_cnt++)

#define TDSL1_DS_INC_SUCC_CNT_FOR_PROC(ds_proc) (tdsl1_ds_proc_info[ds_proc].trm_lock_proc_succ_cnt++)

#define TDSL1_DS_INC_OVERALL_FAIL_CNT() (tdsl1_ds_overall_stat.lock_req_fail_cnt++)

#define TDSL1_DS_INC_OVERALL_SUCC_CNT() (tdsl1_ds_overall_stat.lock_req_succ_cnt++)

#define TDSL1_DS_INC_PAGING_CONSEC_FAIL_CNT() (tdsl1_ds_paging_spec_info.trm_lock_consecutive_fail_cnt++)

#define TDSL1_DS_IS_CMD_PTR_VALID(ds_proc) (tdsl1_ds_proc_info[ds_proc].ext_cmd_ptr != NULL)

/*Macros to get various fields of a procedure*/
#define TDSL1_DS_CAN_LOCK_FAIL_FOR_PROC(ds_proc) (tdsl1_ds_proc_info[ds_proc].can_lock_fail)

#define TDSL1_DS_GET_RESOURCE_FOR_PROC(ds_proc) (tdsl1_ds_proc_info[ds_proc].resource_needed)

#define TDSL1_DS_GET_DUR_IN_MS(ds_proc) (tdsl1_ds_proc_info[ds_proc].lock_req_dur_in_ms)

#define TDSL1_DS_GET_DUR_IN_SCLK(ds_proc) (M_BY_N_MS_IN_SC(TDSL1_DS_GET_DUR_IN_MS(ds_proc),1))

#define TDSL1_DS_GET_LOCK_RSN_FOR_PROC(ds_proc) (tdsl1_ds_proc_info[ds_proc].lock_reason)

#define TDSL1_DS_GET_CB_PTR_FOR_PROC(ds_proc) (tdsl1_ds_proc_info[ds_proc].lock_grant_callback)

#define TDSL1_DS_GET_LOCK_REQ_TIMEOUT_FOR_PROC(ds_proc) (tdsl1_ds_proc_info[ds_proc].lock_req_timeout_in_ms)

#define TDSL1_DS_IS_TDSL1_PROCESSING_EXT_CMD() (tdsl1_ds_db.process_ext_cmd_without_lock == TRUE)

/*Macros to handle TDSL1 dual sim procedure bit mask*/
#define TDSL1_DS_PROC_BMSK(ds_proc) ((uint32)(1 << ((uint32)ds_proc)))

#define TDSL1_DS_RESET_DS_PROC_BMASK() \
(tdsl1_ds_db.tdsl1_ds_proc_mask = 0)

#define TDSL1_DS_ADD_PROC_TO_DS_BMASK(ds_proc) \
  (tdsl1_ds_db.tdsl1_ds_proc_mask |= TDSL1_DS_PROC_BMSK(ds_proc))

#define TDSL1_DS_REM_PROC_FROM_DS_BMASK(ds_proc) \
  (tdsl1_ds_db.tdsl1_ds_proc_mask &= (~(TDSL1_DS_PROC_BMSK(ds_proc))))

/*Macros to check whether any dual sim procedure is active in TDSL1*/
/*#define TDSL1_IN_DS_WITH_ANY_LOCK() (TDSL1_IN_DS_MODE() && (tdsl1_ds_db.tdsl1_ds_proc_mask != 0))

#define TDSL1_IN_DS_WITH_NO_LOCK() (TDSL1_IN_DS_MODE() && (tdsl1_ds_db.tdsl1_ds_proc_mask == 0))*/
#define TDSL1_IN_DS_WITH_ANY_LOCK() (TDSL1_IN_DS_MODE() && TDRM_CLI_STATE_IS_IN_LOCK(TDRM_CLIENT_L1))

#define TDSL1_IN_DS_WITH_NO_LOCK() (TDSL1_IN_DS_MODE() && (!TDRM_CLI_STATE_IS_IN_LOCK(TDRM_CLIENT_L1)))

/*in dsds mode but with failed wakeup lock*/
#define TDSL1_IN_DS_WITH_LOCK_FAIL_WKUP() (TDSL1_IN_DS_MODE() && tdsl1_ds_paging_spec_info.lock_fail_at_wakeup)

#define TDSL1_DS_IS_ANY_PROC_ACTIVE() (tdsl1_ds_db.tdsl1_ds_proc_mask != 0)

#define TDSL1_DS_IS_NO_PROC_ACTIVE() (tdsl1_ds_db.tdsl1_ds_proc_mask == 0)

#define TDSL1_DS_IS_PROC_ACTIVE(ds_proc) \
  ((tdsl1_ds_db.tdsl1_ds_proc_mask & TDSL1_DS_PROC_BMSK(ds_proc)) != 0)
 
#define TDSL1_IS_ANY_OTHER_PROC_ACTIVE(ds_proc) \
  ((tdsl1_ds_db.tdsl1_ds_proc_mask & (~(TDSL1_DS_PROC_BMSK(ds_proc)))) != 0)

#define TDSL1_IN_DS_WITH_PAG_LOCK() (TDSL1_IN_DS_MODE() && (tdsl1_ds_paging_spec_info.lock_succ))

#define TDSL1_IN_DS_WITH_NO_PAG_LOCK() (TDSL1_IN_DS_MODE() && (!tdsl1_ds_paging_spec_info.lock_succ))

#define TDSL1_IS_CELL_TRANS_REQ_SUCC() (tdsl1_ds_paging_spec_info.cell_trans_req_stat)

/*#define TDSL1_IS_IN_PCH_SLEEP_DS_NO_LOCK() ((TDSL1_IN_DS_MODE()) && \
  (tdsl1m_global.l1_state == TDSL1M_PCH_SLEEP) && \
  (TDSL1_IN_DS_WITH_NO_LOCK()))

#define TDSL1_IS_IN_PCH_SLEEP_DS_LOCK() ((TDSL1_IN_DS_MODE()) && \
  (tdsl1m_global.l1_state == TDSL1M_PCH_SLEEP) && \
  (TDSL1_IN_DS_WITH_ANY_LOCK()))*/

#define TDSL1_DS_IS_CMD_IDX_INVALID(cmd_idx) \
  (cmd_idx == TDSL1_DS_INVALID_CMD_IDX)

#define TDSL1_DS_IS_TRM_SIG_PENDING_ON_SLEEP() \
  (tdsl1_ds_db.trm_lock_sig_pending_on_sleep == TRUE)

#define TDSL1_DS_IS_TDSL1_IN_PAGING_WAKEUP() \
  (tdsl1_ds_paging_spec_info.tdsl1_in_paging_wakeup == TRUE)

//#define TDSL1_DEFAULT_GRANT_CB (tdsl1_ds_grant_cb)

/*macro to check whether any hwch is active in downlink*/
#define TDSL1_DS_IS_ANY_DL_CH_ACTIVE() (FALSE)   /* ????? --xiaoyuli */

#define TDSL1_DR_DSDS_FALLBACK_CRC_THRESHOLD 50  //5%*1000
#define TDSL1_DR_DSDS_FALLBACK_GLITACH_DESENSE_THRESHOLD 400  //40%*1000
#define TDSL1_DR_DSDS_FALLBACK_LACK_OF_RXD_THRESHOLD     100 //10%*1000
#define TDSL1_DRDS_UNLOCK_BY_MS_URGENT 0
#define TDSL1_DRDS_UNLOCK_BY_MS_INVALID 0xFFFF

#define TDSL1_DRDS_METRICS_WINDOW  50 //50*DRX_cycles
#define TDSL1_DRDS_FALLBACK_EXIT_DRX_CYCLES (2 * TDSL1_DRDS_METRICS_WINDOW) //100 DRX cycles

#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
#define TDSL1_DS_G2T_TA_MODE_ACTIVE() tdsl1_ds_determine_g2t_ta_mode_active()

/* Priority inversion time employed in the middle of split search */
#define TDSL1_DS_G2T_FS_PRIORITY_INVERSION_TIME_MS 750
#define TDSL1_DS_G2T_ACQ_PRIORITY_INVERSION_TIME_MS 160
#define TDSL1_DS_G2T_TA_LOCK_TIMEOUT_MS 5000

#endif
/*****************************************************************************************************************/
/*   Global Variable declaration    */
/*****************************************************************************************************************/
//extern tdsl1_mdsp_image_enum_type tdsl1_last_mdsp_image;
extern tdsdrx_state_type tdsdrx_state;


/*****************************************************************************************************************/
/*   Function declaration    */
/*****************************************************************************************************************/
extern void tdsl1_ds_hdle_sig(rex_sigs_type ds_sig);
extern void tdsl1_ds_hdle_lock_acq_succ(tdsl1_ds_proc_enum_type ds_proc);
extern void tdsl1_ds_hdle_lock_acq_fail(tdsl1_ds_proc_enum_type ds_proc);
extern void tdsl1_ds_prep_after_lock_acq(tdsl1_ds_proc_enum_type ds_proc);
extern void tdsl1_ds_request_resources(void);
extern tdsl1_ds_proc_enum_type tdsl1_ds_is_lock_needed_for_ext_cmd(tdsl1_ext_cmd_type *ext_cmd_ptr);
extern tdsl1_ds_proc_enum_type tdsl1_ds_find_proc_from_ext_cmd(tdsl1_ext_cmd_type *ext_cmd_ptr);
extern tdsl1_ds_proc_enum_type tdsl1_ds_find_proc_from_setup_cmd(tdsl1_ext_cmd_type *ext_cmd_ptr);
extern void tdsl1_ds_req_and_ntfy_lock_for_proc(tdsl1_ds_proc_enum_type ds_proc, tdsl1_ext_cmd_type* cmd_ptr);
//extern void tdsl1_ds_grant_cb(trm_client_enum_t client, trm_grant_event_enum_t event, trm_request_tag_t tag);
extern boolean tdsl1_ds_ok_to_proc_ext_cmd_in_sleep(tdsl1_ext_cmd_type *ext_cmd_ptr);
extern void tdsl1_ds_pre_cell_trans_req_cb(boolean status);
extern boolean tdsl1_ds_hdle_cnf_sent_to_rrc(tdsrrc_l1_cmd_type *cnf_to_rrc_ptr);
extern tdsl1_ds_proc_enum_type tdsl1_ds_find_proc_from_ext_cnf(tdsrrc_l1_cmd_type *cnf_to_rrc_ptr);
extern void tdsl1_ds_hdle_cell_resel_start(void);
extern void tdsl1_ds_post_cell_trans_cnf_cb(boolean status);
extern void tdsl1_ds_hdle_status_chg_ind(tdsl1_ds_status_change_ind_type *ds_cmd_ptr);
extern void tdsl1_ds_l1m_woken_up(void);
extern void tdsl1_ds_drx_cmd(void);
extern void tdsl1_ds_hdle_bplmn_start(void);
extern void tdsl1_ds_hdle_bplmn_stop(void);
extern boolean tdsl1_ds_exit_sleep(void);
extern void tdsl1_ds_enter_sleep(void);
#ifdef FEATURE_TDSCDMA_LOW_SIB_PRIORITY_DUR_CELL_SEL
extern void tdsl1_ds_1st_6sec_sib_timer_cb(uint32 unused);
extern void tdsl1_ds_extend_lock_for_bch_timer_cb(uint32 unused);
extern void tdsl1_ds_extend_lock_for_bch_handler(void);

#endif
/*===========================================================================
FUNCTION     TDSL1_DS_SGTDS_DEVICE_MODE_HDLE

DESCRIPTION
  This function processes the device mode combination of SGTDS and status_ind from RRC. If UE is in
  SGTDS, TL1 will run as the DA mode. 

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

extern void tdsl1_ds_sgtds_device_mode_hdle(tdsl1_meas_mode_enum_type meas_mode);

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
/*===========================================================================
FUNCTION     tdsl1dsds_modify_info_cb()

DESCRIPTION
  This function is to reply the secondary chain release request from TDRM for both T2L and DS fallback

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE

===========================================================================*/

void tdsl1dsds_modify_info_cb
(
  /* The client which is being informed of the modifiy info result */
  trm_client_enum_t client,

  /* The result of modify info request */
  trm_modify_return_enum_type result
);
#endif

#ifdef FEATURE_TDSCDMA_DR_DSDS

/*===========================================================================
FUNCTION     tdsl1dsds_dual_rx_fallback_status_cb()

DESCRIPTION
  This function is to get the async_cb for the hybride mode status

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  TRUE:  legacy DSDS fallback is triggered, hold the DRx during the fallback
  FALSE: fallabck is not triggered, running as DR-DSDS
===========================================================================*/
void tdsl1dsds_dual_rx_fallback_status_cb(trm_dual_receive_enum_type dual_rec_info);

/*===========================================================================
FUNCTION     tdsl1dsds_dual_rx_chain_status

DESCRIPTION
  This is to check the dual RX chain availabilty from TDRM

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  None.
===========================================================================*/
boolean tdsl1dsds_dual_rx_chain_status(void);
/*===========================================================================
FUNCTION     tdsl1dsds_secondary_chain_status_cb

DESCRIPTION
  This is for TDRM to notify the secondary chain vailability for T2L and FB

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  None.
===========================================================================*/
void tdsl1dsds_secondary_client_status(boolean is_available);

/*===========================================================================
FUNCTION     tdsl1dsds_dual_rx_init

DESCRIPTION
  This function is to init the dual receive mode related parameters

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  TRUE:  legacy DSDS fallback is triggered, hold the DRx during the fallback
  FALSE: fallabck is not triggered, running as DR-DSDS
===========================================================================*/
void tdsl1dsds_dual_rx_init(void);
/*===========================================================================
FUNCTION     tdsl1dsds_dual_rx_legacy_fallback_metrics

DESCRIPTION
  This function is to maintain the legacy DSDS fallback metrics

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  TRUE:  legacy DSDS fallback is triggered, hold the DRx during the fallback
  FALSE: fallabck is not triggered, running as DR-DSDS
===========================================================================*/
void tdsl1dsds_dual_rx_legacy_fallback_metrics_update(void);

/*===========================================================================
FUNCTION     tdsl1dsds_dual_rx_legacy_fallback_init

DESCRIPTION
  This function is to exit the legacy DSDS fallback metrics

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  TRUE:  legacy DSDS fallback is triggered, hold the DRx during the fallback
  FALSE: fallabck is not triggered, running as DR-DSDS
===========================================================================*/
void tdsl1dsds_dual_rx_legacy_fallback_metrics_init(void);

/*===========================================================================
FUNCTION     tdsl1dsds_dual_rx_Chain_status _cleanup

DESCRIPTION
  This function is to init the dual receive mode related parameters

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  TRUE:  legacy DSDS fallback is triggered, hold the DRx during the fallback
  FALSE: fallabck is not triggered, running as DR-DSDS
===========================================================================*/
void tdsl1dsds_dual_rx_chain_status_cleanup(void);

/*===========================================================================
FUNCTION     tdsl1dsds_dual_rx_legacy_fallback_cleanup

DESCRIPTION
  This function is to exit the legacy DSDS fallback metrics

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  TRUE:  legacy DSDS fallback is triggered, hold the DRx during the fallback
  FALSE: fallabck is not triggered, running as DR-DSDS
===========================================================================*/
void tdsl1dsds_dual_rx_legacy_fallback_metrics_cleanup(void);

/*===========================================================================
FUNCTION     tdsl1dsds_dual_rx_legacy_fallback_exit

DESCRIPTION
  This function is to exit the legacy DSDS fallback metrics

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  TRUE:  legacy DSDS fallback is triggered, hold the DRx during the fallback
  FALSE: fallabck is not triggered, running as DR-DSDS
===========================================================================*/
void tdsl1dsds_dual_rx_legacy_fallback_exit(void);
/*===========================================================================
FUNCTION  tdsl1dsds_fillup_fb_set_state_input

DESCRIPTION:
  

DEPENDENCIES
  

RETURN VALUE
  Frequency index.  

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1dsds_fillup_fb_set_state_input(trm_set_client_state_input_type *input_t2l_state, boolean to_trigger_fb);

/*===========================================================================
FUNCTION     tdsl1dsds_dual_rx_legacy_fallback_metrics

DESCRIPTION
  This function is to maintain the legacy DSDS fallback metrics

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  TRUE:  legacy DSDS fallback is triggered, hold the DRx during the fallback
  FALSE: fallabck is not triggered, running as DR-DSDS
===========================================================================*/
void tdsl1dsds_dual_rx_legacy_fallback_metrics_update(void);
#endif


/***********************************************************************************************************************************************************/
/*       QTA          */
/***********************************************************************************************************************************************************/

#define TDSL1_QTA_NORMAL_TS_CX8_DURATION 6912 /* 864*8 */
#define TDSL1_QTA_SPECIAL_TS_CX8_DURATION 2816 /* 352*8 */

/* How much before actual QTA gap must we begin to prepare for QTA */
extern const uint32  tdsl1_qta_prepare_start_usec;
extern const uint32  tdsl1_qta_protection_gap_before_qta; /* ms */

/*****************************************************************************************************************/
/*   QTA Function declaration    */
/*****************************************************************************************************************/
/*===========================================================================
FUNCTION     tdsl1dsds_ok_for_qta

DESCRIPTION
  RRC calls this function before QTA gap to ask TDSL1 if it is OK start QTA.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if tuneaway request approved.  FALSE if not.
===========================================================================*/
#ifdef FEATURE_TDSCDMA_DSDS_ALLOW_QTA_BEFORE_ACT_TIME
extern boolean tdsl1dsds_ok_for_qta(uint16 qta_adv_ms);
#else
extern boolean tdsl1dsds_ok_for_qta(void);
#endif
/*===========================================================================
FUNCTION     tdsl1dsds_prepare_for_tuneaway

DESCRIPTION
  This function is triggered in DSDS unlock callback. TL1 uses this function to decide if tune away can be done.

DEPENDENCIES
  None

PARAMETERS
  unlock_by_sclk  unint32 the unlock time needed by the RAT on another subscription

RETURN VALUE
  TRUE: Lock acquistion for paging is successful
  FALSE: Lock acquistion for paging failed

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsl1dsds_prepare_for_tuneaway(uint32 unlock_time_by_sclk);
/*===========================================================================

FUNCTION  tdsl1dsds_get_qta_rem_time_in_ms

DESCRIPTION
  This function converts the remaining Sclks to ms..

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern int64 tdsl1dsds_get_qta_rem_time_in_ms(uint32 rem_sclk);
/*===========================================================================
FUNCTION     tdsl1dsds_add_cx8

DESCRIPTION
  This function add a time length (in unit of cx8) to a time point, then return
  the new time point

DEPENDENCIES
  None.

PARAMETERS
  time_bef  --- based time point
  delta_cx8 --- the time length

RETURN VALUE
  new time point delta_cx8 after time_bef

SIDE EFFECTS
  None
===========================================================================*/
extern tdsl1_qta_time_info_struct_type tdsl1dsds_add_cx8
(
  tdsl1_qta_time_info_struct_type time_bef, 
  int64 delta_cx8
);
/*===========================================================================
FUNCTION     tdsl1dsds_start_qta_req_cmd

DESCRIPTION
  This function is an indication from RRC that we are to start T2G quick
  tuneaway process under DCH/FACH state.

DEPENDENCIES
  None.

PARAMETERS
  tdsl1_start_qta_cmd_type *start_qta_cmd - Unused.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsl1dsds_start_qta_req_cmd(tdsl1_start_qta_cmd_type *start_qta_cmd);
/*===========================================================================
FUNCTION     TDSL1DSDS_QTA_TIMER_EXPIRED_CMD

DESCRIPTION
  l1m_dsds_tuneaway_timer expired during quick tuneaway procedure, which has
  different steps and different times between steps.  This is local command
  handler invoked for TDSL1M_QTA_TIMER_EXPIRY.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsl1dsds_qta_timer_expired_cmd(void);
/*===========================================================================
FUNCTION     tdsl1dsds_stop_qta_req_cmd

DESCRIPTION
  This function is an indication from RRC that we are to end T2G quick
  tuneaway process, meaning resume TD-SCDMA mode.

DEPENDENCIES
  None.

PARAMETERS
  tdsl1_stop_qta_cmd_type *stop_qta_cmd - Unused.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsl1dsds_stop_qta_req_cmd(tdsl1_stop_qta_cmd_type *stop_qta_cmd);
/*===========================================================================
FUNCTION     TDSL1DSDS_QTA_DIS_MEAS_CB

DESCRIPTION
  This function would be triggerred during L1 QTA start processing, to disable MEAS and UPA

DEPENDENCIES
  None.

PARAMETERS
  subfn: input uint16 activation time to trigger this function by sequencer
  cfg_db_idx: input uint32 common parameter used by sequencer sub_frame callback

RETURN VALUE
  None.
===========================================================================*/
extern void tdsl1dsds_qta_dis_meas_cb(uint16 subfn, uint32 cfg_db_idx);
/*===========================================================================
FUNCTION     TDSL1DSDS_QTA_INFORM_GL1_CB

DESCRIPTION
  This function would be triggerred during L1 QTA start processing, to inform GL1 to build RF Script

DEPENDENCIES
  None.

PARAMETERS
  subfn: input uint16 activation time to trigger this function by sequencer
  cfg_db_idx: input uint32 common parameter used by sequencer sub_frame callback

RETURN VALUE
  None.
===========================================================================*/
extern void tdsl1dsds_qta_inform_gl1_cb(uint16 subfn, uint32 cfg_db_idx);

#if 0 /* changed to FACH QTA API */
/*===========================================================================
FUNCTION:  tdsl1dsds_ul_query_qta_gaps

DESCRIPTION:
  This function queries the timeslot mapping of QTA(Quick Tune Away) gap for
  DSDS UE, radio frames from start_sfn to start_sfn+len-1.
 
  For each radio frame, one uint16 element in the array ts_mapping[] will
  show whether each timeslot is configured as a QTA gap or not.
  | X | X | X | X | X | X | X | X |   | X | X | X | X | X | X | X | X |
   ts6 ts5 ts4 ts3 ts2 ts1 spt ts0     ts6 ts5 ts4 ts3 ts2 ts1 spt ts0
      subframe1                           subframe0
 
      spt:   special slot between ts0 and ts1.
      X:0:   not a QTA timeslot
      X:1:   a QTA timeslot
 
  Assumptions: 1) start_sfn should be no smaller than current SFN;
               2) [start_sfn, (start_sfn+len-1)] should fall into
                  [current SFN, current SFN+9];
               3) len should be between 1 and 8;
               4) ts_mapping points to an array which at least is size len;
               5) ts_mapping[0] indicates radio frame start_sfn;
               6) ts_mapping[len-1] indicates radio frame (start_sfn+len-1);
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1dsds_ul_query_qta_gaps
(
  /* start radio frame */
  uint16 start_sfn,   
  /* length of radio frames (1 -- 8) */
  uint8 len,
  /* the pointer to an array to store the timeslot mapping  */
  /* (size should be the same as len)  */    
  uint16 *ts_mapping  
  );
#endif
/*===========================================================================
FUNCTION:  tdsl1dsds_ul_query_qta_gaps

DESCRIPTION:
  This function queries the timeslot mapping of QTA(Quick Tune Away) gap for
  DSDS UE, radio frames from start_sub_sfn to start_sub_sfn+len-1.
 
  For each sub frame, one uint8 element in the array ts_mapping[] will
  show whether each timeslot is configured as a QTA gap or not.
  | X | X | X | X | X | X | X | X | 
   ts6 ts5 ts4 ts3 ts2 ts1 spt ts0   
      subframe0                
 
      spt:   special slot between ts0 and ts1.
      X:0:   not a QTA timeslot
      X:1:   a QTA timeslot
 
  Assumptions: 1) start_sub_sfn should be no smaller than current sub SFN;
               2) [start_sub_sfn, (start_sub_sfn+len-1)] should fall into
                  [current SFN, current SFN+9];
               3) len should be between 1 and 16;
               4) ts_mapping points to an array which at least is size len;
               5) ts_mapping[0] indicates sub frame start_sub_sfn;
               6) ts_mapping[len-1] indicates sub frame (start_sub_sfn+len-1);
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1dsds_ul_query_qta_gaps
(
  /* start sub frame */
  uint16 start_sub_sfn,   
  /* length of sub frames (1 -- 16) */
  uint8 len,
  /* the pointer to an array to store the timeslot mapping  */
  /* (size should be the same as len)  */    
  uint8 *ts_mapping  
);
/*===========================================================================
FUNCTION     tdsl1dsds_qta_fw_cleanup_ind

DESCRIPTION
  This function indicates FW if QTA start or abort.

DEPENDENCIES
  None.

PARAMETERS
  tfw_multi_sim_settings_t *stop_qta_cmd - Unused.

RETURN VALUE
  None.
===========================================================================*/
extern void tdsl1dsds_qta_fw_cleanup_ind(tfw_multi_sim_cxm_lock_lost_timings_t *qta_cleanup_ind);

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
/*===========================================================================

FUNCTION    tdsl1dsds_qta_log_packet_output

DESCRIPTION
          output DSDS QTA log packet buff

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1dsds_qta_log_packet_output(void);
/*===========================================================================
FUNCTION     tdsl1dsds_qta_end_time_for_trm_lock

DESCRIPTION
  record TDSRRC gets TRM lock time (in both sleep clock & Rx time) when QTA ends.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
===========================================================================*/
void tdsl1dsds_qta_end_time_for_trm_lock(void);
/*===========================================================================

FUNCTION    tdsl1dsds_qta_log_packet_init

DESCRIPTION
          init DSDS QTA log packet buff

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1dsds_qta_log_packet_init(void);
#endif /* FEATURE_TDSCDMA_L1_LOGGING_ENABLED */

/***********************************************************************************************************************************************************/
/*       QTA  end        */
/***********************************************************************************************************************************************************/
/***********************************************************************************************************************************************************/
/*       Tune Away Start       */
/***********************************************************************************************************************************************************/
/*160ms checking threshold after LTA resume*/
#define TDSL1DSDS_LTA_RESUME_CHECK_THRESHOLD 16
#define TDSL1DSDS_TUNEAWAY_INVALID_VALUE 0xFFFF

/*===========================================================================
FUNCTION     tdsl1dsds_ok_for_tuneaway

DESCRIPTION
  RRC calls this function at inter-RAT tuneaway time to ask TDSL1 if it is OK
  to SUSPEND TDSL1 and start the tuneaway process.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if tuneaway request approved.  FALSE if not.
===========================================================================*/
extern boolean tdsl1dsds_ok_for_tuneaway(void);

/*===========================================================================
FUNCTION     tdsl1dsds_fill_tuneaway_duration

DESCRIPTION
  This is called by other modules to fill tuneaway duration (in frames)
  into our global variable.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.
===========================================================================*/
extern void tdsl1dsds_fill_tuneaway_duration(uint16 duration);
/*===========================================================================
FUNCTION     tdsl1dsds_return_tuneaway_duration

DESCRIPTION
  This function returns duration of tuneaway, in frames to the caller.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
Tuneaway duration, in frames.
===========================================================================*/
extern uint16 tdsl1dsds_return_tuneaway_duration(void);


/***********************************************************************************************************************************************************/
/*      Tune Away  end        */
/***********************************************************************************************************************************************************/

#ifdef FEATURE_WWAN_WLAN_ANTENNA_SHARING
void tdsl1_non_ds_enter_sleep(void);
void tdsl1_non_ds_exit_sleep(void);
#endif

#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
/*===========================================================================
FUNCTION     TDRM_DETERMINE_G2T_TA_MODE_ACTIVE

DESCRIPTION
  This function is used by TDSCDMA to determine if GERAN currently supports
  G2W TA

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsl1_ds_determine_g2t_ta_mode_active(void);

/*===========================================================================
FUNCTION     tdsl1dsds_g2t_ta_prio_inv_timer_expired_cmd

DESCRIPTION
  

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE

===========================================================================*/
extern void tdsl1dsds_g2t_ta_prio_inv_timer_expired_cmd(void);

/*===========================================================================
FUNCTION     TDSL1_DS_REQUEST_AND_NOTIFY_FOR_SPLIT_ACQ

DESCRIPTION
  This function is used by TDSCDMA to request lock for split fs/acq

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsl1_ds_request_and_notify_for_split(trm_reason_enum_t reason, trm_duration_t duration);

#endif /* FEATURE_TDSCDMA_G2T_TUNE_AWAY */

#ifdef FEATURE_TDSCDMA_DR_DSDS
void tdsl1dsds_secondary_client_status(boolean is_available);
void tdsl1dsds_dual_rx_fallback_status_cb(trm_dual_receive_enum_type dual_rec_info);
#endif

trm_client_enum_t tds_get_current_unlock_client_id(void);

/*===========================================================================
FUNCTION     tdsl1dsds_acq_inv_timer_expired_cmd

DESCRIPTION
  

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE

===========================================================================*/
extern void tdsl1dsds_acq_inv_timer_expired_cmd(void);


#endif /*FEATURE_TDSCDMA_DSDS */

#if defined (FEATURE_TDSCDMA_DSDS) || defined(FEATURE_TDSCDMA_DSDA)
/*===========================================================================
FUNCTION     tdsl1dsds_cxm_priority_table_cb

DESCRIPTION
  When TDSCDMA mode starts, TL1 would call CxM to get priority table for QTA

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
Tuneaway duration, in frames.
===========================================================================*/
void tdsl1dsds_cxm_priority_table_cb
(
  cxm_tech_type  tech_id,
  cxm_activity_table_s activity_tbl[MAX_ACTIVITY_TIERS]
);

#endif

extern void tdsl1_ds_init(void);

extern void tdsl1_ds_release_resources(void);

extern boolean tdsl1m_f_mdsp_and_rf_awake;

#endif /* #ifndef TDSTL1MDSDS_H */
