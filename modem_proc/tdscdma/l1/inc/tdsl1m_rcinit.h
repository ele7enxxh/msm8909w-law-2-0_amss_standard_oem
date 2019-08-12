#ifndef TDSL1M_RCINIT_H
#define TDSL1M_RCINIT_H
/*===========================================================================

                    L 1   T A S K   D E C L A R A T I O N S

DESCRIPTION
  This file contains global declarations and external references
  for the L1 Manager task.

  The L1 Manager interfaces the L1 Sequencer unit, the Device Driver layer,
  and the Processors within the L1 with the units external to the L1. It is
  the control interface between the L1 and external subsystems and provides
  a task context for the L1. The L1 Manager dispatches external and local
  commands to Processors in the L1. The L1 Manager also provides a basic state
  machine, and acts as a switcher between states of the phone.

  The L1 Manager waits on a set of signals (watchdog timer, task offline,
  task stop, command queues, etc). Whenever there is a signal the L1 Manager
  task will ascertain the source of the signal and performs appropriate
  processing.

  If there is a command on any of the command queues, the L1 Manager performs
  the following processing based on the command.

  Protocol Command: These are sub-states within a particular protocol or
  commands within a sub-state. Upon receipt of the above command, the L1
  Manager will invoke the currently active Protocol Processor to process
  the command.

  Local Command: These commands provide local state control and
  synchronization within the L1. They are also state independent commands.
  The Local Commands are sent through the Local Queue. The local state maps
  all local commands directly to protocol sub-state processing. Upon receipt
  of a Local Command, the L1M will invoke the corresponding Local Processor
  to process the command.

EXTERNALIZED FUNCTIONS

 tdsl1m_free_cmd_buf
   This function is used by L1 to free the external command buffer once L1 is
   finished processing the command. This function is executed in the context
   of the caller.

 tdsl1m_cmd_confirm
   This function sends a confirmation cmd to the upper layer.

 tdsl1m_cphy_setup_cnf_put_status
   This function is used by the L1 modules to update the tdsl1m_global.cphy_setup_status
   buffer in the course of performing the steps of a TDSL1_CPHY_SETUP_REQ.

 l1_put_local_cmd
   This function is used by the internal units of the L1 to submit local
   commands. This function puts the command onto the L1M Local Command
   Queue and sets the Local Command Queue for the L1 task.

   This function is called in the context of the L1M thread or in the
   context of an ISR within the L1.

 tdsl1m_get_local_cmd_buf
   This function is used by the internal clients of the L1 Manager to get a
   command buffer.

 tdsl1m_get_rrc_cmd_buf
   This function gets the command buffer to send a command/confirmation
   to RRC.

INTERNALIZED FUNCTIONS

  l1_process_ext_cmd
    This function de-queues a command from the protocol command queue,
    invokes the command processor of the currently active protocol processor
    and passes the command to it.
    If there are no more protocol commands pending on the protocol command
    queue, the protocol command queue signal is cleared.
    This function is called in the context of the L1 thread.

  tdsl1m_cmd_err_ind
    This function sends a command error indication to the upper layer.

  l1_free_local_cmd_buf
    This function frees the command buffer associated with the local command.

  tdsl1m_process_local_cmd
    This function de-queues a command from the local command queue,
    invokes the command processor of the currently active local processor
    and passes the command to it.
    If there are no more local commands pending on the local command
    queue, the local command queue signal is cleared.

    This function is called in the context of the L1M thread.

  tdsl1m_init
    This function does all initializations necessary for proper operation
    of the L1.

    This function should be called before the L1M can be used.

  tdsl1m_mdsp_init_done_isr
    This ISR is called after the MDSP is initialized. The MDSP_INT4 is
    muxed with the init done and rach done interrupts. This function will
    set the MDSP_INIT_DONE_SIG signal.

  L1M Idle State Functions
  tdsl1m_idle_init: Called when L1M transitions to Idle state.
  tdsl1m_idle_cleanup: Called when L1M leaves Idle state.
  tdsl1m_idle_cmd_processor: Processes external commands received by L1 in Idle
                          state.

  L1M ACQ State Functions
  tdsl1m_acq_init: Called when L1M transitions to ACQ state.
  tdsl1m_acq_cleanup: Called when L1M leaves ACQ state.
  tdsl1m_acq_cmd_processor: Processes external commands received by L1 in ACQ
                         state.

  L1M BCH State Functions
  tdsl1m_bch_init: Called when L1M transitions to BCH state.
  tdsl1m_bch_cleanup: Called when L1M leaves BCH state.
  tdsl1m_bch_cmd_processor: Processes external commands received by L1 in ACQ
                         state.

  L1M PCH State Functions
  tdsl1m_pch_init: Called when L1M transitions to PCH state.
  tdsl1m_pch_cleanup: Called when L1M leaves PCH state.
  tdsl1m_pch_cmd_processor: Processes external commands received by L1 in PCH
                         state.

  L1M FACH State Functions
  tdsl1m_fach_init: Called when L1M transitions to FACH state.
  tdsl1m_fach_cleanup: Called when L1M leaves FACH state.
  tdsl1m_fach_cmd_processor: Processes external commands received by L1 in FACH
                         state.

  L1M DCH State Functions
  tdsl1m_dch_init: Called when L1M transitions to DCH state.
  tdsl1m_dch_cleanup: Called when L1M leaves DCH state.
  tdsl1m_dch_cmd_processor: Processes external commands received by L1 in DCH
                         state.

  L1M Deactive State Functions
  tdsl1m_deact_init: Called when L1M transitions to Deactive state.
  tdsl1m_deact_cleanup: Called when L1M leaves Deactive state.
  tdsl1m_deact_cmd_processor: Processes external commands received by L1 in
                           Deactive state.

  L1M Wait for TRM Stop Functions
  tdsl1m_wait_for_trm_stop_init: Called when L1M transitions to Wait for TRM Stop state.
  tdsl1m_wait_for_trm_stop_cleanup: Called when L1M leaves Wait for TRM Stop state.
  tdsl1m_wait_for_trm_stop_cmd_processor: Processes external commands received by L1 in
                           Wait for TRM Stop state.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  The L1_task function contains the L1 Manager task.  It must be
  specified in a call to rex_def task and started by a Main Control task.

  Copyright (c) 2000 - 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1m_rcinit.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/22/14   zl      Initialize Gap Manager critical section with LTE feature off
02/02/11   sq      Removed obsolete header file 
01/25/11   cxd     Merge MSGR queues for L1 task
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "tdsl1def.h"
#include "msg.h"

/******************************************************************************
*******************************************************************************
                      EXTERNAL VARIABLE DECLARATIONS
*******************************************************************************
******************************************************************************/

/*--------------------------------------------------------------------------
                 EXTERN VARIABLE:  L1_EXT_STATE_PROCESSOR
--------------------------------------------------------------------------*/

/* Critical section mutex for TSN info Q */
extern rex_crit_sect_type tdsl1dec_tsn_q_crit_sect;
/* Mutex for DOB read functionality */
extern rex_crit_sect_type mcaltds_dec_dob_read_crit_sect;
/* Critical section mutex for F8 done */
/* Critial section mutex for A2 PC/HSDPA data path sync */
extern rex_crit_sect_type mcaltds_dec_a2_pc_crit_sect;
/* Critical section mutex for Dec HS HARQ statistics */
extern rex_crit_sect_type mcaltds_dec_hs_harq_stats_crit_sect;
extern rex_crit_sect_type mcaltds_dec_f8_crit_sect;
//extern rex_crit_sect_type mcal_dl_rsv_comb_crit_sect;
extern rex_crit_sect_type tdsmcal_dl_evt_id_crit_sect;
extern rex_crit_sect_type tdshsscch_crit_sect;
/* Mutex for all HS logging */
extern rex_crit_sect_type tdsmcal_hs_logging_crit_sec;
extern rex_crit_sect_type tds_srch_crit_sect;
extern rex_crit_sect_type tdsdechs_decode_satus_logging_crit_sect;
extern rex_crit_sect_type tdsdechs_ol_cqi_crit_sect;
/* DL PHCH critical sections */
extern rex_crit_sect_type tdsdlphch_pend_cfg_crit_sect;
extern rex_crit_sect_type tdsdlphch_stmr_ctr_crit_sect;
extern rex_crit_sect_type tdsdlsccpch_pre_susp_res_crit_sect;
/* DL TRCH MEAS critical sections */
extern rex_crit_sect_type tdsdltrchmeas_bler_parms_crit_sect;
// Mutex to mutually exclude UL and EUL ISRs executions
extern rex_crit_sect_type tdsenc_eul_and_r99_ul_mutex;
/* Local Mutex for the protection of cbfunc_pending_bitmask in ISR and task context*/
extern rex_crit_sect_type tdseul_cbfunc_pending_bitmask_mutex;
/* Local Mutex for the protection of EUL cfg params in ISR and task context*/
extern rex_crit_sect_type tdsedpch_params_mutex;
/* Local Mutex for the protection of EUL state params in ISR and task context*/
extern rex_crit_sect_type tdsedpch_state_params_mutex;
/*Global mutex for EUL ISR and L2*/
extern rex_crit_sect_type tdse_ul_isr_l2_mutex;

/*Light-weight mutex for EUL ISR and L2 Data path*/
extern rex_crit_sect_type tdse_ul_isr_l2_datapath_mutex;


extern rex_crit_sect_type tdseulsg_agch_hich_crit_section;
extern rex_crit_sect_type tdseulsg_erucch_tx_info_crit_section;
extern rex_crit_sect_type tdseulsg_sg_type_state_crit_section;

/* Mutex for all HS configuration */
extern rex_crit_sect_type tdshscfg_cfg_crit_sec;
/* Mutex shared by a lot of module, added by owen for crm build */
extern rex_crit_sect_type tds_crit_sect;
extern rex_crit_sect_type tdsl1m_ext_cmd_handle_crit_sect;
extern rex_crit_sect_type tdsl1m_local_cmd_handle_crit_sect;
extern rex_crit_sect_type tdsl1m_msgr_handle_crit_sect;
/* Critical section mutex for DOB handle */
extern rex_crit_sect_type tdsl1trace_db_idx_crit_sect;
extern rex_crit_sect_type tdsl1_gsm_clk_ctrl_crit_sect;
// Mutex definition for RF command queue
extern rex_crit_sect_type tdsrf_msgr_rf_handle_crit_sect;
extern rex_crit_sect_type   tdsseq_sfn_cfn_mutex_lock;
extern rex_crit_sect_type   tdsseq_chnl_setup_event_enabled_lock;
extern rex_crit_sect_type   tdsseq_rx_time_adjust_event_enabled_lock;
extern rex_crit_sect_type   tdsseq_cfn_update_lock;
/* Critical section used to arbitrate concurrent access to shared structure "tdssrchevt_ops_sanity_chk" */
extern rex_crit_sect_type tdssrch_ops_sanity_chk_crit_sect;

extern rex_crit_sect_type tdssrchmeas_rpt_crit_sect;
/* Critical section used to arbitrate concurrent access to the MRM prepared by L1 */
extern rex_crit_sect_type tdssrchmeas_multi_filter_sect;

/* Create SRCHGSM critical section */
extern rex_crit_sect_type tdssrchgsm_meas_crit_sect;
extern rex_crit_sect_type tdssrchgsmdrv_w2g_crit_sect;

/* Critical Section variables/calls used to protect intra freq meas database */
extern rex_crit_sect_type  tdssrch_intra_freq_db_crit_sect;
/* Critical Section variables/calls used to protect inter freq meas database */
extern rex_crit_sect_type  tdssrch_inter_freq_db_crit_sect;
/* Critical Section variables/calls used to protect freq scan substate */
extern rex_crit_sect_type  tdssrchfs_scan_substate_crit_sect;

/* Critical Section variables/calls used to protect internal  meas database */
extern rex_crit_sect_type  tdssrch_internal_meas_db_crit_sect;

/* Critical section used to arbitrate concurrent access to search module timestamp counter tdssrch_ts_cnt */
extern rex_crit_sect_type tdssrch_ts_cnt_crit_sect;

/* Critical section used to arbitrate concurrent access to tbar timer elapsed count */
extern rex_crit_sect_type tdssrch_tbar_elapsed_cnt_crit_sect;

extern rex_crit_sect_type tdssrchset_get_cell_crit_sect;
/* stmr mutex for protecting register writes*/
extern rex_crit_sect_type tdsstmr_mutex;
/* Mutex between GET_DATA ISR and FPACH_DEC_DONE INT */
extern rex_crit_sect_type tdsulra_isr_mutex;

 /* Mutex to protect state tranisitions of MTPL Backoff Algorithm 
    * This mutex also ensures the TIMER Task is not run in parallel with state transitions */
extern rex_crit_sect_type state_mutex; 
  /*Mutex between UL DCH ISR and UL reconfig in WL1 task*/
extern rex_crit_sect_type                    tdsulcmd_dch_reconfig_isr_mutex;
extern rex_crit_sect_type tdsl1dec_dob_handle_crit_sect;
extern rex_crit_sect_type tdsseq_crit_sect;
extern rex_crit_sect_type tdsdlchmgr_drop_resume_crit_sect;
extern rex_crit_sect_type  tdsrxdiv_cond_mutex ;
extern rex_crit_sect_type  tdsrxdiv_scch_update_mutex;
extern rex_crit_sect_type  tdsrxdiv_ard_ind_mutex;
extern rex_crit_sect_type tdssrchlte_crit_sect;
extern rex_crit_sect_type tdsuldpchctrl_isr_mutex;

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT) || defined(FEATURE_TDSCDMA_60MS_GAP_FOR_BSIC_OPT)
extern rex_crit_sect_type tdsgapmgr_crit_sect;
#endif

extern rex_crit_sect_type mcaltds_mcvs_req_crit_sect;
extern rex_crit_sect_type tdsdlpccpch_slam_res_crit_sect;
//extern rex_crit_sect_type tdsgapmgr_crit_sect;
extern rex_crit_sect_type tdsl1_tfc_excess_pwr_states_mutex;

// mutex to protect the drx rf state
extern rex_crit_sect_type   tdsrf_drx_rf_state_mutex;

// mutex to protect the drx rf state
extern rex_crit_sect_type   tdsl1sleep_sleep_abort_mutex;

//mutex to protest rf_sleep function
extern rex_crit_sect_type   tdsl1sleep_rf_sleep_mutex;

extern rex_crit_sect_type tdsl1sleep_abort_mutex;
#ifdef FEATURE_TDSCDMA_DSDS
extern rex_crit_sect_type tdsl1sleep_wakeup_cancel_mutex;
#endif

#ifdef FEATURE_TDSCDMA_DSDS_QTA
extern rex_crit_sect_type mcaltds_hs_qta_hsscch_olpc_mutex;
extern rex_crit_sect_type tdseulsg_qta_eagch_olpc_mutex;
#endif


//mutex to protest DSDA MCS freq id
extern rex_crit_sect_type   tdsl1m_freq_id_mutex;

#ifdef FEATURE_TDSCDMA_WLAN_COEX
extern rex_crit_sect_type  tdsl1wlancoex_slot_adv_ntc_mutex;
#endif
//mutext to protect fw_rsp timer set id 
extern rex_crit_sect_type tdsl1m_fw_rsp_timer_mutex;

extern rex_crit_sect_type tdsstmr_st_event_prog_mutex;

#ifdef FEATURE_TDSCDMA_VSTMR
extern rex_crit_sect_type tdsstmr_vstmr_rxtime_view_mutex;
#endif

extern rex_crit_sect_type tdrm_secondary_grant_mutex;

#ifdef FEATURE_TDSCDMA_L1M_PROFILING_LOG_PACKET
extern rex_crit_sect_type tds_tdrm_profiling_mutex;
#endif
extern rex_crit_sect_type tdssrch_reacq_mutex;

//mutext to protect aset_update_cmd_buffer 
extern rex_crit_sect_type tdssrch_aset_update_cmd_buffer_crit_sect;

extern rex_crit_sect_type tdsrxd_freeze_state_mutex;
/*=============================================================================================

FUNCTION  tds_l1_task_init

DESCRIPTION
  This task initializes layer 1 queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/

void tds_l1_init_crit_sect(void);

#endif /* TDSL1M_RCINIT_H */

