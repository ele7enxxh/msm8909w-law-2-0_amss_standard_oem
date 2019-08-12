#ifndef WL1_QSH_EXT_H
#define WL1_QSH_EXT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            WL 1 QSH EXT . H                 

GENERAL DESCRIPTION
   Contains declarations for function which are QUALCOMM proprietary 
   and may or may not be shipped as source code.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:$
  $Header: 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/03/15   ap     Addition of metrics WL1_QSH_METRIC_SERV_CELL_RF_PRIM_DIV and
                  WL1_QSH_METRIC_VOICE_BLER
03/18/15   ash    QSH metric addition to add the number of RACH Procedures attempted
02/24/15   sr/ap  Changes to support FW dump collect for QSH
02/23/14   sr     Initial version for wl1_qsh_ext.h

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include <string.h>
#include "trm.h"
#ifdef FEATURE_QSH_DUMP
#include "wfw_qsh_ext.h"
#endif
#include "qsh.h"
#include "l1rrcif.h"
#include "queue.h"

#define WL1_QSH_MAJOR_VER 0
#define WL1_QSH_MINOR_VER 1

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                  Struct type declaration                      */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                  Debug mini info Struct type declaration                      */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_QSH_DUMP

/*                                 COMMON                           */
/*                                                                         */
typedef PACKED struct PACKED_POST
{
  /* Indicates if a cell transition command is in progress in L1 */
  boolean cell_trans_in_progress;
  /* Indicates if a CPHY_SETUP_REQ command is in progress in L1 */
  boolean cphy_setup_in_progress;
  uint8 hho_status;
  /* Keeps track of the current state of the L1 State Machine. */
  uint8 l1m_state;
  uint8 l1m_next_state;

  /* From l1m_ctrl_db_type */
  /** Bitmask for all cmds that have been issued but have not been completed yet**/
  uint32 pending_cmd_bmsk;

  /** Bitmask to determine which module need clean up **/
  uint32 module_cleanup_pending_bmsk;

  /** Bitmask to determine steps to be carried out in nex cmd handler**/
  uint32 setup_actions_mask;

  /**Drop_req_mask: To evaluate all the drops that are required**/
  uint32 cphy_drop_req_mask;

  /** QXDM Event bmsk */
  uint32 setup_event_bmsk;

  /** Flag to indicate cleanup trigger from HHO */
  boolean hho_cleanup;

  /** Suspend_req_mask: to suspend all periodic ops befor CPHY processing **/
  uint16 cphy_periodic_suspend_mask;

  /* act time type to indicate if there is activation time from cphy setup */
  uint8 act_time_type;

  /* activation time from cphy setup used to inform HS module do not start before this time*/
  uint16 act_time;

  uint32 setup_req_mask;
}wl1_debug_common_l1m_info_struct_type;

#define WL1_DEBUG_MDSP_BUFF_SIZE  6

typedef PACKED struct PACKED_POST
{
 /* indicate this command is suspended */
  boolean suspended;
  /* which command is being sent */
  uint8 cmd_title;
/*frc frame num*/
  uint16 frame_num_cnt;
  /* frc cx1 count */
  uint16 frame_cx1_cnt;
}wl1_debug_common_mdsp_sync_cmd_type;

/* mDSP response command trace buffer structure */
typedef struct
{
  /* Response Type */
  uint8 cmd_id;
  /* Frame and chip counts */
  uint16 frame_num_cnt;
  uint16 frame_cx1_cnt;
} wl1_debug_common_mdsp_rsp_trace_struct_type;

typedef PACKED struct PACKED_POST
{
  /*store last 6 sync cmds*/
  wl1_debug_common_mdsp_sync_cmd_type mdsp_sync_cmd_buffers[WL1_DEBUG_MDSP_BUFF_SIZE];
  /*store last 6 cmd done responses from fw side*/
  wl1_debug_common_mdsp_rsp_trace_struct_type  mdsp_cmd_rsp_info[WL1_DEBUG_MDSP_BUFF_SIZE];
} wl1_debug_common_mdsp_sync_cmd_exchange_info_struct_type;


typedef PACKED struct PACKED_POST
{
    /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;
  /*l1m info*/
  wl1_debug_common_l1m_info_struct_type l1m_info;
  /*Sync cmd exchange info*/
  wl1_debug_common_mdsp_sync_cmd_exchange_info_struct_type mdsp_sync_cmd_exchange_info;
}wl1_common_mini_debug_info_struct_type;


/*                              DEMOD                                */
/*                                                                         */
typedef PACKED struct PACKED_POST
{
  uint32 cell_idx : 5;
  /* the PN position of the finger as of last status dump, chipX8 */
  uint32 pos : 20;
  /* the state of the finger */
  uint32 fing_state : 5;
  /* index of the ssid*/
  uint32 ssid : 2;
}wl1_debug_dl_fing_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /** CPICH primary scrambling code. Range 0..511 */
  uint16 priScrCode;
  /**uarfcn   */
  uint16 frequency;
  /** cell index   */
  uint8 cell_db_idx;
  /** cell current state   */
  uint8 cell_current_state;
  /** Carrier Idx*/
  uint8 carrierIdx;
  /* From dltri_rl_info */
  /* indicate per PSC triage active with this information */
  boolean enabled;
  /* reference finger */
  uint8  ref_fing;
  /* what are the services associated with this RL */
  uint8 assoc_services;
}wl1_debug_dl_per_cell_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /** Current state for the channel */
  uint8 dl_hwch_state;
  /** for which physical channel type this hwch is reserved */  
  uint8 rsvd_for_phch;
}wl1_debug_dl_phch_db_struct_type;

typedef PACKED struct PACKED_POST
{
  /*! Bit 7:0 Channel Type */
  uint8 channelType;
  /*! Bit 15:8 Cell index to which this channel belongs */
  uint8  cellIndex;
  /*! Bit 31:29 Action type Enable. Reslam, Disable, HICH/RGCH action type */
  uint8 actionType;
}wl1_debug_dl_mcal_per_phch_cfg_cmd_info_struct_type;

#define WL1_DL_DEBUG_MAX_NUM_FING_INFO      24
#define WL1_DL_DEBUG_MAX_NUM_CELL_INFO      8
#define WL1_DL_DEBUG_MAX_NUM_CHAN_INFO      10
#define WL1_DL_DEBUG_MAX_NUM_COMB_INFO      7

typedef PACKED struct PACKED_POST
{
    /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;
  /* FW pending bmask */
  uint32 fw_fing_cfg_pending;
  uint32 fw_cell_cfg_pending;
  uint64 fw_chan_cfg_pending;
  /* Per fing info */
  wl1_debug_dl_fing_info_struct_type per_fing_info[WL1_DL_DEBUG_MAX_NUM_FING_INFO];
  /* Per cell info */
  wl1_debug_dl_per_cell_info_struct_type dl_per_cell_info[WL1_DL_DEBUG_MAX_NUM_CELL_INFO];
  /* Active phch info */
  wl1_debug_dl_mcal_per_phch_cfg_cmd_info_struct_type active_phch_info[WL1_DL_DEBUG_MAX_NUM_CHAN_INFO];
  /* Per comb info */
  wl1_debug_dl_phch_db_struct_type dl_per_comb_db[WL1_DL_DEBUG_MAX_NUM_COMB_INFO];
  /*Controller info*/
  uint8 drxctrl_curr_state;
}wl1_dl_mini_struct_type; // 221 bytes

/*                           SEARCHER                             */
/*                                                                         */

#define NUM_DBG_CARR 2
#define SRCHSET_DBG_MAX_CELLS 10
#define SRCH_DBG_MAX_STEP1_RESULTS 5
#define SRCH_DBG_MAX_NASTT_RESULTS 5
#define WL1_DEBUG_L1_MAX_RL    6
typedef struct
{
    /* Indicates the acquisition mechanism to be used, i.e. frequency
     * only or frequency and scrambling code  */
  uint8  acq_type;
    /* Indicates which RF channel to be scanned. The value is  */
    /* the UTRA Absolute Radio Frequency Channel Number (UARFCN) */
    /* of downlink carrier frequency, as defined in TS25.101. */
  uint16                  freq;
  /* the number of individual radio links */
  uint16                  num_rl;
  /* Requested scrambling code.  Can be 0 to (16*511)+15 = 8191. */
  uint16                  scr_code[WL1_DEBUG_L1_MAX_RL];
  uint8  acq_mode;
    /* Indicates how deep the reference RF channel shall be scanned.*/
  uint8  srch;
  uint8  actual_srch_type; 
  /* Indicates the search requested; service search, manual plmn or background plmn */
    /* Indicates if AGC thresh needs to be compared before proceeding
     with acquisition path.  */
  boolean                 apply_agc_thresh;
  uint32                  pn_pos;
    /* If present, PN position of the cell in chipx1 units. */
  boolean                 sttd_indicator_present;
    /* TRUE means "sttd_indicator" value is present.  FALSE otherwise. */
  boolean                 sttd_indicator;
    /* If present, TRUE means the cell is using Space Time Transmit Diversity.
    ** FALSE means it is not.  Described in TS 25.331, 10.3.6.78. */
  boolean                 unbar_freqs;
  /*  The agc threshold value is set if the apply_agc_thresh (= TRUE)
        Otherwise the value is not set and L1 will not look into this value
        in that case, (= -1) indicates L1 shall decide the value
*/
  boolean                 do_acq_split;
}wl1_debug_l1_acq_cmd_type;

typedef PACKED struct PACKED_POST
{
  boolean sttd_nw_informed;

  /* Acquisition sequence being performed */
  uint8 proc;
  /* The number of times the current procedure has been attempted */
  uint8 attempts;
  boolean reset_xo; // see srch_reset_vctcxo_param_during_acquisition
  uint16 scr_code_acq_passed; // PSC that ACQ passed on
  /* Bitmask of remaining procedures in the sequence. Bit position is defined
     by srchacq_step_enum_type */
  uint16 bmsk;
  wl1_debug_l1_acq_cmd_type cmd;
  boolean sttd; // was srchacq_ratchange_sttd_indicator and srchacq_wcdma_hho_sttd_indicator
  uint32 pos; // was srchacq_pn_pos
  uint16 pullin_scr_code; // scr code that pullin passed on
} wl1_debug_srchacq_dbg_info_struct_type;

typedef PACKED struct PACKED_POST
{
   /* mode in whcih srch is scheduled */
   uint8         mode;  
   /* srch is in sleep state or active state */
   uint8     substate;  
   /* drx mode schduling will be freezed by drx ctrl  during  reception of any cphy_setup_req ,so this flag indicates that
   drx have freeze the srch schduling mode */
   boolean                        drx_mode_freeze;
   /* hb mode is enable step1/2 searches during CM gap and till this flag is true 
   we will be in HB mode only */
   boolean                        hb_mode_for_step12;   
}wl1_debug_srchcpcdrx_dbg_info_struct_type;


typedef PACKED struct PACKED_POST
{
  /* Flag indicating that asynchronous neighbor searches is in progress */
  boolean async_full_srch_active;
  /** This flag gets set to TRUE if Step1 search has just been processed and we need to do a NASTT for the carrier 
  * specified, otherwise it stays FALSE and gets set to FALSE after NASTT search completed */
  boolean nastt_srch_due[NUM_DBG_CARR];
  /** This flag is set to TRUE if a mode change is in progress, FALSE otherwise */
  boolean mode_change_in_progress;
  /** This variable holds the current mode of operation of the search scheduler */
  uint8 curr_mode;
  /** This variable holds the next mode of operation that the search scheduler will be put in */
  uint8 next_mode;
  /** This variable will hold the current client ID that is driving the search scheduler */
  uint8 curr_client_id;
  /** This variable holds the type of the current search that just got issued. Since
  * there can't be two searches issued at the same time, this is unique. */
  uint8 curr_srch_issued;
  /*Variable holds if scch activation search is in progress*/
  boolean scch_act_srch_in_progress;
  /* Variable holds aset cell masks of carriers which
      needs to be issued for hpq search*/
  uint8 scch_deact_bmsk;
  /** Current Freq Index sent for NASTT */
  uint8 curr_nastt_freq_idx;
  /* Starting position of PSC peaks to be sent to step_2 searcher */
  uint16 cur_psc_peak_indx[NUM_DBG_CARR];
  /* number of peaks issued for current nastt */
  uint16 num_peaks_issued_for_nastt;
  /* step1 peak count for each carrier */
  uint16 srch_step1_peak_cnt[NUM_DBG_CARR];
  /* Variable holds the bitmask of carriers which are 
     de-activated by scch order*/
  uint32 scch_act_cell_mask;
  /* Variable holds aset cell masks of carriers which
      needs to be issued for hpq search*/
  uint32 cur_scch_order;
  /** This variable holds the mask of all searches that will get pended while a mode 
  * change is in progress */
  uint32 pended_searches;
}wl1_debug_srchsched_dbg_info_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 num_cells;
  uint16 freq;
  /* WCDMA Cell Scrambling Code */
  uint16 reference_cell;  
}wl1_debug_srchset_dbg_carr_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /* *sttd of the cell*/
  boolean sttd;
  /* Current carrier index of cell */
  uint8 carr_idx;
  /* Which Set the Cell currently belongs to */
  uint8 set;
  uint8 prev_set;
   /* ecio*/  
  uint16 eng;
  /* WCDMA Cell Scrambling Code */
  uint16 scr_code;
  /* file_ecio */
  uint16 filt_ecio;
  /* Reported as a combination of 3-bit LSW and 16-bit MSW in unit of chipx8 */
  uint32 ref_pos;
}wl1_debug_srch_cell_dbg_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /* Groups of sets arranged by their carrier */
  wl1_debug_srchset_dbg_carr_info_struct_type carr[NUM_DBG_CARR];
  wl1_debug_srch_cell_dbg_info_struct_type cells[SRCHSET_DBG_MAX_CELLS];
}wl1_debug_srchset_dbg_info_struct_type;

typedef PACKED struct PACKED_POST
{
  uint16 eng;
  /* Reported in unit of chipx8. Range is 0-20476 (step1 is searching on chipx2 
   * resolution, the range of PSC position is thus from 0 to 5119, which 
   * translates into 0 to 20476 in chipx8) */
  uint16 peak_pos_cx8;
}wl1_debug_srch_step1_peak_info_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 num_peaks;
  wl1_debug_srch_step1_peak_info_struct_type peak_info[SRCH_DBG_MAX_STEP1_RESULTS];
}wl1_debug_srch_step1_dbg_result_struct_type;

/* Searcher NASTT task result type */
typedef PACKED struct PACKED_POST
{
  /* carrier index of each task, when parallel nastt is issued of 2 carriers  */
  uint8 carrier_idx;
  /* 16 bit non-coherent accumulation energy, if the energy is 0, 
     it’s an invalid peak */
  uint16 eng;
  /* Scrambling code index: 0-512 */
  uint16 sc_index;
  /* peak position returned by NAS-TT */
  uint32 peak_pos_cx8;
}wl1_debug_srch_nastt_peak_info_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 num_peaks;
  wl1_debug_srch_nastt_peak_info_struct_type peak_info[SRCH_DBG_MAX_NASTT_RESULTS];
}wl1_debug_srch_nastt_dbg_result_struct_type;

typedef PACKED struct PACKED_POST
{
    /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;
  /* Database to debug ACQ crashes*/
  wl1_debug_srchacq_dbg_info_struct_type srchacq_dbg_info;
    /* Database to debug scheduler crashes*/
  wl1_debug_srchsched_dbg_info_struct_type srchsched_dbg_info;
    /* Database to debug cpc drx issues*/
  wl1_debug_srchcpcdrx_dbg_info_struct_type srchcpcdrx_dbg_info;
    /* Database to debug srchset crashes*/
  wl1_debug_srchset_dbg_info_struct_type srchset_dbg_info;
  /* Step1 results*/
  wl1_debug_srch_step1_dbg_result_struct_type srch_step1_dbg_result_info;
  /* NASTT results*/ 
  wl1_debug_srch_nastt_dbg_result_struct_type srch_nastt_dbg_result_info;
} wl1_srcher_mini_debug_info_struct_type;


/*                               IDLE                                  */
/*                                                                         */
/* get partial info from the drx rxd db, 1 byte*/
typedef PACKED struct PACKED_POST
{
   uint8 idle_rxd_enabled : 1;
   uint8 idle_rxd_curr_mode : 7;
}wl1_debug_drx_rxd_info_struct_type;

typedef struct {
/*Range is 0-0x7FFFFFFF*/
  int32 frame_qty;
/*Range is 0-307199*/
  int32 chip_offset_cx8_qty;
} wl1_debug_tlm_extended_sfn_struct_type;

typedef PACKED struct PACKED_POST /* 10 bytes */
{
   uint8                          type : 5;
   uint8                          state : 3;
   uint8                          sleep_type : 3;
   uint8                        inactive_pending : 1;
   uint8                        entity_delayed : 1;
   uint8                        schedule_update_delayed : 3;
   wl1_debug_tlm_extended_sfn_struct_type   target_time; /* 8 bytes*/
}wl1_debug_drx_entity_info_struct_type;


typedef PACKED struct PACKED_POST /* 10 bytes */
{
  /* number of elapsed DRX Cycles */
  uint32 num_drx_cycles;
  /* SFN of PCCPCH frame that contains the (1st of the) PI bits */
  uint16 pich_sfn; 
    /* the time difference between the start of the PI bits to the start of the
    previous PCCPCH frame. In other words, 
    the sum of pi_offset_from_pich and pich_offset parameters described above, modulo 150.
    An mdsp parameter*/
  uint16 mdsp_pi_pn_count;
    /* the time difference (in units of 256 chips) from the start of the pich frame to
     when the page indicator bits start in the pich frame
   * Range (0...143).
   */
  uint8 pi_offset_from_pich;
    /* 10.3.6.41 Number of PI per frame - obtained from RRC during SCCPCH setup; 
  */
  uint8 pi_per_frame;
}wl1_debug_drx_parms_info_struct_type;

typedef PACKED struct PACKED_POST /* 8 bytes */
{
  uint8 pending_rf_state_update : 1;
  uint8 pending_rf_state : 1;
  uint8 pending_ss_state_update : 1;
  uint8 pending_ss_state : 3;
  uint8 current_state : 3;
  uint8 prev_state : 3;
  uint8 pending_next_state : 3;
  uint8 rf_state : 1;
  uint8 completed_srch_type;
  uint8 started_srch_type;
  uint32  drx_notifications_pending_for_pich_drop_bmsk;
}wl1_debug_ssmgr_db_info_struct_type;
/*---------------------------------------------------------------------------------------------------------------------------*/

/* globals needed for timeline related debugging, total = 36 bytes*/
typedef PACKED struct PACKED_POST
{
   uint8 drx_outer_loop_adj_mode : 1;
   uint8 drx_update_slp_longer_adj_Cx8 : 1;
   uint8 short_qxt : 1;
   uint8 wsleep_early_wakeup_last_cycle : 5;
   uint32 sleep_interval_Cx8;
   uint32 cc_ols_Cx8;
   uint32 sfn_ols;
   int32 ticks_until_pi;
   int32 ticks_rem_adj_sleep_longer_Cx8;
   uint32 early_wakeup_delta_bpgs;
   wl1_debug_drx_parms_info_struct_type drx_parms;
   wl1_debug_drx_rxd_info_struct_type  rxd_db;
}wl1_debug_drx_slp_timeline_info_struct_type;

#ifdef FEATURE_WCDMA_REACQLESS_PICH
typedef struct
{
/*whether RLP  conditions met for next cycle or not.*/
  boolean possible_next_cycle; 
/*RLP will happen this cycle */
  boolean this_cycle;  
}wl1_debug_drx_rlp_info_struct_type;
#endif
typedef struct
{
  boolean parallel_reacq_next_cycle;
  boolean parallel_reacq_this_cycle;
  boolean parallel_reacq_peformed;
#ifdef FEATURE_WCDMA_REACQLESS_PICH
  wl1_debug_drx_rlp_info_struct_type rlp;
#endif
}drx_parallel_reacq_info_struct_type;

/*globals needed for getting DRX state MC info = 20 bytes*/
typedef PACKED struct PACKED_POST
{
   uint8 drx_state : 4;
   uint8 drx_pich_opts_status : 1;
   uint8 wl1_fw_status : 2;
   uint8 last_int_from_sleep_ctlr : 3;
   uint8 drx_sleep_aborted : 1;
   uint8 drx_delay_sleep_abort :  1;
   uint8 drx_offline_pich_this_cycle : 1;
   uint8 drx_bch_is_on : 1;
   uint8 drx_just_paged : 1;  
   uint8 wtow_bplmn_in_progress : 1;
   boolean wtog_bplmn_in_progress;
   drx_parallel_reacq_info_struct_type drx_parallel_reacq_db; /* 5 bytes*/
   uint32 drx_delayed_timer_ms;
   uint32 drx_early_rf_shutdown_bmsk;
   uint32 drx_eval_cell_fing_decfg_bmsk;

}wl1_debug_drx_state_machine_info_struct_type;

/*globals needed for getting entity state info = 36 bytes*/
typedef PACKED struct PACKED_POST
{
   uint32 drx_handles_awake_mask;
   /* get info from the logical entity db only for these 8 entities
   PICH, BCCH_EXT, SPLIT_SRCH
   entity type(1), sleep info(9), inactive pending(1), schedule update delayed(1), state_type (1) , total = 13*8 = 91 bytes*/
   wl1_debug_drx_entity_info_struct_type logical_entity_db[3]; /* 10 bytes per entity * 3 = 30*/
   int16 drx_entity_target_handle;      
}wl1_debug_drx_entity_db_info_struct_type;

/* globals needed for samp serv info = 8 bytes*/
typedef PACKED struct PACKED_POST
{
   wl1_debug_ssmgr_db_info_struct_type sampservmgrdb;/* 8 bytes*/     
}wl1_debug_drx_sampserv_info_struct_type;

typedef struct
{
   boolean wrm_g2w_ta_prio_inversion_in_progress;
   boolean wrm_g2w_ta_block_request_and_notify;
   boolean wrm_prio_inv_tmr_set_for_split_acq;
   boolean wrm_idle_oos_prio_inv_in_progress;
   boolean wrm_split_acq_offline_nastt_in_progress;
   uint32 wrm_g2w_ta_l1_trm_resrv_sclk;
   uint32 wrm_split_acq_saved_duration;
}wl1_debug_idle_ta_mgr_wrm_info_struct_type;

/* G2W TA database structure.*/
typedef struct
{
   boolean lock_status;
   boolean ext_cmd_pended;
   uint32  num_g2w_ta_gaps;
   uint32  total_g2w_ta_gaps;
   wl1_debug_idle_ta_mgr_wrm_info_struct_type wrm_db;
} wl1_debug_idle_ta_mgr_info_struct_type;

/* globals needed for G2W TA info = 31 bytes*/
typedef PACKED struct PACKED_POST
{
   uint8 drx_sleep_mode : 2;
   uint8 drx_g2w_ta_state : 6;
   wl1_debug_idle_ta_mgr_info_struct_type wl1_g2w_ta_db; /* 28 bytes*/
   int16 wl1idle_ta_mgr_split_srch_entity_handle;
}wl1_debug_drx_g2w_ta_info_struct_type;

/* globals needed for miscellaneous info = 3 bytes*/
typedef PACKED struct PACKED_POST
{
   boolean wsleep_wakeup_srch_pending;
   int16 rxagc;   

}wl1_debug_drx_miscellaneous_info_struct_type;

typedef PACKED struct PACKED_POST
{
    /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;
   wl1_debug_drx_slp_timeline_info_struct_type drx_slp_timeline_info;
   wl1_debug_drx_state_machine_info_struct_type drx_state_machine_info;
   wl1_debug_drx_entity_db_info_struct_type drx_entity_info;
   wl1_debug_drx_sampserv_info_struct_type drx_sampserv_info;
   wl1_debug_drx_g2w_ta_info_struct_type drx_g2w_ta_info;
   wl1_debug_drx_miscellaneous_info_struct_type drx_miscellaneous_info;   

}wl1_drx_sleep_mini_debug_info_struct_type;

/*                          CELL RESEL                         */
/*                                                                    */

typedef enum
{
  WL1_DEBUG_SRCH_FACH_INTRAF_SEARCHER,
  WL1_DEBUG_SRCH_FACH_INTRAF_RACH,
  WL1_DEBUG_SRCH_FACH_INTER_MEAS,
  WL1_DEBUG_SRCH_FACH_RACH_RXD,
  WL1_DEBUG_SRCH_FACH_HSDPA, /* ID for HS-FACH. HSFACH module will reserve with this ID for HS channel config/reconfig/deconfig */
  WL1_DEBUG_SRCH_FACH_QTA,
  WL1_DEBUG_SRCH_FACH_NUM_REQ_ID
} wl1_debug_srch_fach_reserve_req_id_enum_type;

typedef PACKED struct PACKED_POST
{
  uint8   quan_type;
  /* Measurement quantity -- Ec/Io or RSCP. Note that, for Cell Selection
     and Reselection, we only support Ec/Io Measurement Quantity on FDD cells,
     as required by TS25.304.
   */
  boolean thresh_intra_f_incl;
  int16   thresh_intra_f;
  /* Intra-frequency measurement threshold. Value (in dB) ranges (-32..20)
     by step of 2. 0xFFFF indicates the parameter is not applicable.
   */
  boolean thresh_inter_f_incl;
  int16   thresh_inter_f;
  /* Inter-frequency measurement threshold. Value (in dB) ranges (-32..20)
     by step of 2. 0xFFFF indicates the parameter is not applicable.
   */
  boolean thresh_interrat_incl;
  uint16   t_resel_frames;
  /* Cell reselection timer, in 10ms unit */
  int16   q_qualmin;
  /* Minimum required quality level for Ec/Io measurement (in dB).
     Value ranges (-20..0)
   */
  int16   q_rxlevmin;
  /* Minimum required received level for RSCP measurement (in dBm).
     Value ranges (-115..-25) by step of 2.
   */
  boolean hcs_used;
  /*Flag indicating if HCS is enabled on the current cell or not */
  #ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
  boolean prior_used;
  /*Flag indicating if priority information is provided by network or not */
  #endif
}wl1_debug_srchcr_aset_parms_info_struct_type;

typedef PACKED struct PACKED_POST
{

  /* 25.133 section 4.2.2.5a
     The total number of configured higher priority E-UTRA 
    and UTRA FDD carrier frequencies and is additionally 
    increased by one if one or more groups of GSM frequencies 
    is configured as a higher priority
    Thigher_priority_search = 60 * Nlayers */

  uint8 nlayers;

  /* 25.133 section 4.2.2.3
     The total number of carriers used for FDD cells.
     The UE shall measure CPICH Ec/Io and CPICH RSCP of detected
     Inter-Frequency cells at least every (Ncarrier-1) * TmeasureFDD */

  uint8 ncarrier;

  /* Indicates if priority based reselection is to be used
     for Inter-Freq layers */
  boolean interf_prior_info_present;

 /* Indicates if priority based reselection is to be used
     for GSM layers */
  boolean gsm_prior_info_present;

#ifdef FEATURE_WCDMA_TO_LTE
  /* Indicates if LTE EARFCNs have been configured in the 
     Neighbor list */
  boolean lte_info_present;
#endif

  /*Variables to store if AbsPrio in FACH measurement are included in SIB19*/
  
  uint8 fach_abs_prio_ue_cap_sib_ind;
  
}wl1_debug_srchcr_prior_info_for_rats_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 cur_fmo_state;
  uint8  fmo_done_cb_cnt;
  uint8  cur_fmo_done_cb_idx;
  uint8  client_bmsk;
}wl1_debug_srchfach_fmo_state_mc_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /* search scheduler mode */
  uint8  current;
  /* search scheduler mode */
  uint8  next;  
  /* if search mode change is pending */
  boolean                     change_pending;
}wl1_debug_srchfachdrx_db_srch_mode_info_struct_type;

/* FACH non-intra-F measurement parameters */
typedef PACKED struct PACKED_POST
{
  boolean  c_rnti_valid;
  uint16   c_rnti;   /* C_RNTI value only if c_rnti_valid is TRUE */
  /* Bit 0  - inter-freq FMO ind 
   *  Bit 1  - GSM/LTE FMO ind
   */
  uint8  inter_f_gsm_ind; /* Inter-F FDD Meas indicator, N_FDD */
} wl1_debug_srchfach_n_parms_info_struct_type;


/* globals needed for srchcr module */
typedef PACKED struct PACKED_POST
{
   uint8 pending_cell_resel : 1;
   uint8 rf_tuned_away : 1;
   uint8 start_cell_reselection : 1;
   uint8 drx_waiting_on_srch_asleep : 1;
   uint8 resp_early_rf_slp_allowed : 1;
   uint8 pending_immdt_resel : 1;
   uint8 pending_cell_resel_bsic_verif : 1;
   uint8 out_of_service_timer_expired : 1;
   uint8 irat_meas_state;
   uint16 t_resel_frames_fdd;
   uint16 tresel_lte;
   uint16 tresel_gsm;
   wl1_debug_srchcr_aset_parms_info_struct_type  aset_parms;
   uint8 res_eval_status;
   uint8 res_eval_meas_pending;
   uint8 split_res_eval_meas_pending;
   uint8 res_eval_meas_completed;
   uint8 curr_prior_scheduling_mode;
   union
   {
      uint32 inter_f_cell_ptr;
      uint32 gsm_cell_ptr;
      uint32 cr_cell_ptr;
      uint32 lte_cell_ptr;
   }u;
   uint8 prev_meas_rules;
   rex_timer_cnt_type rem_oos_time;
   rex_timer_cnt_type rem_CR_time_ms;
   uint8 cell_select_failed_drx_cycles;
   uint8  current_meas_rules;
   wl1_debug_srchcr_prior_info_for_rats_struct_type prior_info_for_rats;
}wl1_debug_srchcr_info_struct_type;

typedef struct 
{
  /* Current information */
  uint8 current_search_mode;
  uint16 current_entity_mask;
  uint16 current_mode_duration; /* In frames */
  
  /* Previous information */
  uint8 previous_search_mode;
  uint16 previous_entity_mask;

  /* Next information */
  boolean mode_change_pending;
  uint8 next_search_mode;
  uint16 next_entity_mask;
  uint16 entity_wu_after_current_mode;
} wl1_debug_srchzz_srch_mode_info_struct_type;

typedef struct
{
  uint16 psc;
  uint16 freq;
  uint32 bmask_sent;
  uint32 bmask_done;
  boolean status;
} wl1_debug_srch_wakeup_set_info_struct_type;

/* globals needed for srchzz module */
typedef PACKED struct PACKED_POST 
{
   boolean reacq_failed;
   boolean config_decfg_in_prog;
   boolean perform_enh_ctch_algo;
   uint8 type_of_split_srch;
   uint8  substate;
   uint8 substate_type;
   wl1_debug_srchzz_srch_mode_info_struct_type mode_info;
   wl1_debug_srch_wakeup_set_info_struct_type wakeup_set;
}wl1_debug_srchzz_info_struct_type;

/* globals needed for srchfach module */
typedef PACKED struct PACKED_POST
{
   wl1_debug_srchfach_n_parms_info_struct_type  n_intra_meas_parms;
   uint8 fmo_oksn_rat_type;
   uint8 overall_reserve_pending_req_id_bmsk;
   uint8  srchfach_substate;

   /* Num frames reserved are valid only for the below 2 clients
    * SRCH_FACH_INTRAF_SEARCHER
    * SRCH_FACH_INTER_MEAS
    */
   uint16 req_id_num_fr_reserved[2];
   uint16 req_id_num_fr_remaining[2];

   uint8 req_id_reserve_start_cfn[WL1_DEBUG_SRCH_FACH_NUM_REQ_ID];
   uint32 tick_to_next_oksn;
   uint8 crnti_query_timer;
   boolean sccpch_evt_enabled;
   wl1_debug_srchfach_fmo_state_mc_info_struct_type fmo_state_info;

}wl1_debug_srchfach_info_struct_type;

/* struct for a time instance */
typedef struct
{
  int16  cfn;
  int32 ticks;
}wl1_debug_srchfachdrx_timeline_info_struct_type;

typedef struct
{
 uint8 srch_active;
 /* time when next interf/IRAT srch scheduling can be done  */
 wl1_debug_srchfachdrx_timeline_info_struct_type     time_fmo_srch_complete;
}wl1_debug_srchfachdrx_fmo_srch_timing_info_struct_type;

/* globals needed for srchfachedrx module */
typedef PACKED struct PACKED_POST
{
   wl1_debug_srchfachdrx_db_srch_mode_info_struct_type edrx_db;
   wl1_debug_srchfachdrx_fmo_srch_timing_info_struct_type fmo_srch_timing_info;
}wl1_debug_srchfachdrx_info_struct_type;


typedef PACKED struct PACKED_POST
{
    /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;
  wl1_debug_srchcr_info_struct_type srchcr;
  wl1_debug_srchzz_info_struct_type srchzz;
  wl1_debug_srchfach_info_struct_type srchfach;
  wl1_debug_srchfachdrx_info_struct_type srchfachedrx;
}wl1_cell_resel_mini_debug_info_struct_type;

/*                              UPLINK                            */
/*                                                                    */

/******************************PRACH*******************************/

typedef PACKED struct PACKED_POST {
  uint8                        avail_sig_mask;
  uint8                        sf_min;
  uint8                        sc_num; 
  uint8                        punct_lim;
  uint8                        avail_sub_chan_mask;
  uint8                        trch_id;
  int8                         pwr_offset_P0;
  uint8                        max_preamble_tx;
  
} wl1_debug_ul_prach_cfg_struct_type;
//8 bytes

typedef PACKED struct PACKED_POST  {

  uint8                      num_preambles;
  uint8  aich_status;
  uint8                      message_length;

  /*
  ** bits 0-3                last_signature;
  ** bits 4-7                last subchannel;
  */
  uint8                      last_signature_sub_ch;

  int16                      last_pmbl_power;
  int16                      message_tx_power;

} wl1_debug_ul_prach_status_struct_type; //8 bytes

/****************************HS_PRACH******************************/

typedef PACKED struct PACKED_POST
{
 uint8                  ai_status;
 uint8                 hsrach_status;
 uint8           eai_status;
 uint8  wl1_hs_rach_edch_rsrc_rel_cause;
 uint8                                      addtnl_edch_backoff_timer;
 uint8                                      maxAGCHCollResolution;
 uint8                                      avail_sig_mask;
 uint8                                      avail_subchan_mask;
 uint16                                     edchTxContBackOff;
 uint16                                     maxCCCHAllocation;
 
}wl1_debug_ul_hs_rach_config_params_struct_type; //12 Bytes


/******************************UL_DPCH*****************************/
typedef PACKED struct PACKED_POST {
  int16                                      dpcch_pwr_offset;
  uint8                                      pcp_len;
  uint8                                      srb_delay;
  uint8                        pca;             
  uint8                    scrambling_code_type;
  uint32                                     scrambling_code; 
  uint8                                      num_dpdch;
  uint8                            sf_min;
  boolean                                    tfci_present;
  uint8                    fbi_len; 
  uint8                                      punct_lim;
  uint8 dpcch_slot_format;

} wl1_debug_ul_dpch_cfg_struct_type; //16 Bytes

typedef PACKED struct PACKED_POST {
  uint16                               TFCI;
  uint8                                num_trch;
  boolean                              mac_busy;
  uint8                                asc_num;
  uint8                                num_active_trchs;
  boolean                              srb_data_present;
  uint8                     srb_data_num_tti;

}wl1_debug_ul_tx_data_struct_type; //8bytes


/******************************EDCH********************************/
typedef struct
{
  /* Can take values from 0 to 127.
  ** ETFCI_BOOST_VAL to set to 127 to disable BOOST */
  uint8 e_tfci_boost_val;

  /* takes values 0-6. If e_tfci_boost_val is set 127, 
  ** then this variable should not be looked into*/
  uint8 delta_t2tp;
} wl1_debug_ul_e_tfc_boost_info_struct_type;

typedef PACKED struct PACKED_POST
{                                   
  uint8                                         tti;
  uint8                                         pl_non_max; 
  uint8                                         max_ch_codes;
  uint8                                         harq_rv_config;
  uint8                                         num_ref_e_tfci;
  uint8                                         e_tfci_table_idx; 
  uint8                                         edpcch_pwr_offset;
  boolean                                       edpdch_pwr_interpolation; 
  uint8                                         min_beta_ed;
  wl1_debug_ul_e_tfc_boost_info_struct_type     e_tfc_boost_info;
  boolean                                       dpdch_cfged;
} wl1_debug_ul_e_ul_sig_cfg_db_struct_type;

/******************************CPC_DTX******************************/
typedef PACKED struct PACKED_POST {

  uint8                       dtx_action;
  uint8                       start_cfn;
  uint8                       dtx_drx_offset;
  uint8                       enabling_delay;
  uint8                       ue_dtx_cycle1;
  uint8                       ue_dtx_cycle2;
  uint8                       ue_dpcch_burst_1;
  uint8                       ue_dpcch_burst_2;
  uint8                       tinactive_cycle2;
  uint8                       dtx_long_preamble_length;
  uint8                       l1_mac_dtx_cycle;
  uint8                       l1_mac_inactivity_threshold;

} wl1_debug_ul_e_ul_cpc_dtx_cfg_db_struct_type;//12 bytes


/******************************EDL_CPC_DRX**************************/
typedef PACKED struct PACKED_POST
{
  
  /*
  ** bit 0                                               cpc_drx_status_active
  ** bit 1                                               ue_drx_grant_monitor_enabled
  ** bit 2-3                                             entity_id_ag_s_rg
  ** bit 4-5                                             entity_id_hich
  ** bit 6-7                                             entity_id_ns_rg
  */
  uint8                                                  cpc_drx_active_grant_monitor_en_entity_ids;

  l1_inact_thrshd_for_ue_grant_monitoring_info_enum_type inact_thresh_cfg_grant_monitor;
  uint8                                                  max_ns_rgch_dpch_offset;
  uint8                                                  max_s_rgch_dpch_offset;
  uint8                                                  max_hich_dpch_offset;

  /* 
  ** bit 0                                               subfn 0
  ** bit 1                                               subfn 1
  ** And so on, info for 8 subFns
  */
  uint8                                                  is_sched_tx_hich_rslt_fn;
  uint16                                                 curr_inact_thresh_grant_monitor;

}wl1_debug_ul_edldrx_ctrl_struct_type;
//8 bytes

/******************************UL_CM*******************************/
typedef PACKED struct PACKED_POST {
   
   /*
   ** bit0       -  next_tti_is_in_cm_frame;
   ** bit1       -  need_cm_factor_update;
   ** bit2       -  next_2ms_tti_is_cm;
   ** bit 3 to 7 -  first_cm_slot
   */
   uint8            next_cfn_cm_flags_bmsk;

   /*
   ** bit 0 - Harq 0
   ** bit 1 - Harq 1
   ** And so on
   ** bit 7 - Harq 7
   */
   uint8            first_10ms_tx_is_cm;
   
} wl1_debug_ul_edch_cm_info_struct_type;//2 Bytes


/*******ISR Timining profile information struct for EUL ***********/

typedef PACKED struct PACKED_POST{
  uint8 eul_start_bpg;
  uint8 eul_end_bpg;
  uint8 eul_async_param_start_bpg;
  uint8 eul_async_param_end_bfi_start_bpg;
  uint8 eul_bfi_end_a2_start_bpg;
  uint8 eul_a2_end_bpg;
  uint8 eul_a2_rb_start_bpg;
  uint8 eul_a2_rb_end_bpg;
  uint8  pre_or_post_fiq;
  uint8 tti;
  uint8 cfn;
  uint8 sub_fn;
} wl1_debug_ul_eul_isr_timing_struct_type; //12 Bytes


/******************************EDL*********************************/

/* Num pages per channel in EUL DL CFG */
typedef enum {
  WL1_DEBUG_E_DL_CONFIG_DB_PAGE_ONE,
  WL1_DEBUG_E_DL_CONFIG_DB_PAGE_TWO,
  WL1_DEBUG_E_DL_CONFIG_DB_MAX_PAGES,
  WL1_DEBUG_E_DL_CONFIG_DB_INVALID_PAGE = 0xFF
} wl1_debug_ul_e_dl_config_db_page_enum_type;

  #define   WL1_DEBUG_EUL_MAX_RL                        4
  #define   WL1_DEBUG_EUL_MAX_CFG_DB_COUNT              2

typedef PACKED struct PACKED_POST {
  uint8 action;
} wl1_debug_ul_e_dl_hich_info_page_struct_type;
typedef PACKED struct PACKED_POST {
  uint8 action;
} wl1_debug_ul_e_dl_rgch_info_page_struct_type; 
typedef PACKED struct PACKED_POST {
  uint8 action;
  uint8                                intf_buf_idx;
} wl1_debug_ul_e_agch_info_page_struct_type;
typedef PACKED struct PACKED_POST {
  uint8                           cur_page;
  wl1_debug_ul_e_agch_info_page_struct_type   agch_info_page[WL1_DEBUG_E_DL_CONFIG_DB_MAX_PAGES];
}wl1_debug_ul_e_agch_info_struct_type; //8 bytes

typedef PACKED struct PACKED_POST {
  uint8                              hich_cur_page;
  uint8                              rgch_cur_page;
  uint8                              cell_idx;
  wl1_debug_ul_e_dl_hich_info_page_struct_type  hich_info_page[2];
  wl1_debug_ul_e_dl_rgch_info_page_struct_type  rgch_info_page[2];
}wl1_debug_ul_e_dl_rl_info_struct_type; //8 bytes


typedef PACKED struct PACKED_POST {
  uint8                                     num_e_rl;
  uint8                           curr_eul_tti;
  uint8                           ord_eul_tti;
  uint8   e_serv_cell_changed;
  uint8            e_dl_state;
  uint8            e_dl_next_state;
  uint16                                    farthest_ch2dpch_offset;
  wl1_debug_ul_e_agch_info_struct_type      agch_info;
  wl1_debug_ul_e_dl_rl_info_struct_type     e_dl_rl_info[WL1_DEBUG_EUL_MAX_RL];

} wl1_debug_ul_e_dl_config_db_struct_type; //48 bytes


typedef PACKED struct PACKED_POST {
  /** is scheduling needed */
  uint8 scheduling_needed;
  /** entity id provided by drx ctrlr   */
  uint8 drx_ctrlr_id;
  /** entity type RACH/AICH */
  uint8 entity;
  /** entity state */
  uint8 entity_state;
}wl1_debug_edrx_ul_entity_info_struct_type;
/****************************UL Overall Struct***********************/

typedef PACKED struct PACKED_POST {

  /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;

  /* bits 0-1                             e_ul_curr_phychan_cfg_db_idx
  ** bits 2-3                             e_ul_curr_dpch_cfg_db_idx 
  ** bits 4-5                             ul_phychan_db_idx
  ** bits 6-7                             debug_eul_timing_idx
  */
  uint8                                   e_ul_debug_db_idx;
  
  uint8                                   debug_etfci_value;
 
  /*
  ** bit0                                 ul_dpcch_cfg_send;
  ** bit1                                 rach_isr_pending;
  ** bit2                                 enc_cleanup_flg;
  ** bit3                                 wenc_rf_tx_enable_done_for_rach;
  ** bit4                                 wl1_ul_rach_trigger_during_qta;
  ** bit5                                 ul_set_pa_on;
  ** bit6                                 ul_pa_on_in_pcp_wait;
  ** bit7                                 wl1_ul_eul_qta_active
  */
  uint8                                   enc_rach_dpch_qta_dbg_bmsk;

  /* State enums can be bit field sizes, depending on max value */

  /*
  ** bit 0-2                              ul_state;
  ** bit 3-4                              ul_dpch_state;
  ** bit 5-7                              wenc_rach_curr_state;
  */
  uint8                                   ul_r99_states;
  
  /*
  ** bit 0-2                              wl1_hs_rach_state;
  ** bit 3-6                              wl1_hs_rach_sub_state;
  ** bit 7                                unused;
  */
  uint8                                   wl1_hs_rach_state_sub_state;
  
  /*
  ** bit 0-1                              ul_tfc_pwr_elmn_state;
  ** bit 2-3                              wenc_thermal_backoff_curr_state;
  ** bit 4                                wl1_rf_tx_state;
  ** bit 5-6                              qta_state;
  ** bit 7                                unused;
  */
  uint8                                   wenc_rf_tx_therm_algo_tfc_pwr_elmn_qta_state;
  
  /*
  ** bit 0-1                              e_ul_state;
  ** bit 2-4                              e_ul_edpch_state;
  ** bit 5-7                              e_ul_cpc_dtx_state;
  */
  uint8                                   e_ul_state_edpch_cpc_dtx_state;

  uint8                                   available_sg_idx;
//8 Bytes

  int16                                   dpch_init_tx_pwr_agc;
  uint32                                  wenc_rach_action_bmask;
//14 Bytes

  wl1_debug_edrx_ul_entity_info_struct_type          wl1_ul_edrx_entity;
//18

  wl1_debug_ul_edldrx_ctrl_struct_type               wl1_edldrx_entity_ctrl_info;
//26 bytes
 
  wl1_debug_ul_edch_cm_info_struct_type              edch_cm_info;
//28
  
  wl1_debug_ul_eul_isr_timing_struct_type            debug_eul_isr_timing_ptrs[3];
//64 bytes

  wl1_debug_ul_e_ul_sig_cfg_db_struct_type           e_ul_phychan_cfg_db_info[WL1_DEBUG_EUL_MAX_CFG_DB_COUNT];
//88 bytes

  wl1_debug_ul_prach_cfg_struct_type                 prach_cfg;
//96 Bytes

  wl1_debug_ul_prach_status_struct_type              prach_status;
//104 Bytes

  wl1_debug_ul_dpch_cfg_struct_type                  dpch_cfg;
//120 bytes

  wl1_debug_ul_tx_data_struct_type                   l1_ul_tx_data_debug_buf;
//128 bytes

  wl1_debug_ul_e_dl_config_db_struct_type            e_dl_config_db;
//176 Bytes

  wl1_debug_ul_e_ul_cpc_dtx_cfg_db_struct_type       e_ul_cpc_dtx_cfg_db_info;
//188

  wl1_debug_ul_hs_rach_config_params_struct_type     wl1_hs_rach_cfg_stats_db;
//200 Bytes

} wl1_ul_mini_debug_info_struct_type;



/*                    Advanced Receivers                        */
/*                                                                         */

#define WL1_RXD_MAX_TRACE_BUFFER_HISTORY (20) 
#define WL1_DEBUG_RXD_NUM_OPS 6
#define WL1_CME_SRCH_RESULT_MINI_NUM_CELLS  (4)
#define WL1_DEBUG_MCALWCDMA_CME_MAX_NUM_QSET_CELLS   (3)
/** Number of channel types for which receiver selection has to be done. */
#define WL1_DEBUG_MCALWCDMA_CME_NUM_RECEIVER_CHAN_TYPES      (3)

typedef enum
{
  WL1_DEBUG_CME_CARRIER_PRI = 0,
  WL1_DEBUG_CME_CARRIER_SEC,
#ifdef FEATURE_WCDMA_3C_HSDPA
  WL1_DEBUG_CME_CARRIER_TRI,
#endif
  WL1_DEBUG_CME_NUM_CARRIERS
} wl1_debug_cme_carrier_enum_type;

/* struct to hold details regarding qta gaps */
typedef struct{
  uint8 qta_gap_status;
  boolean rxdpm_trigger_set;
  uint8  start_cfn;
  uint8  cfn_gap;
}wl1_debug_rxd_qta_info_struct_type;

/* Struct declaration for PCH mode RxD */
typedef struct
{
  uint8 wl1_rxd_pch_mode;
  uint8  wl1_rxd_pch_tune_pending : 1;
  uint8 wl1_rxd_pch_trm_req_pending : 1;
  //boolean wl1_rxd_pch_disable_clocks_pending;
  uint8 wl1_rxd_pch_teardown_started :1;
  uint8 wl1_rxd_placed_out_of_lock :1;

  uint8 : 4;
  
  uint16 wl1_rxd_pch_full_div_cycle_ctr;
  uint16 min_full_div_cycles_ctr;
  int32  pi_sum_thresh;
  uint32 wl1_rxd_pch_waiting_on_sigs;
}wl1_debug_rxd_pch_mini_info_struct_type;

typedef PACKED struct PACKED_POST
{
    /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;

    /* Variable used to indicate that QXDM cmds are being used in debug mode */

  uint16 wl1_rxd_debug_dont_request_trm : 1;

  uint16 wl1_rxd_cfg_pending : 1;   

  /* Flag indicating that a Rx chain swap is pending */
  uint16 rxd_rx_chain_recfg_pending : 1;

  /* Flag indicating that a Rx chain swap was done */
  uint16 rxd_rx_chain_swap_done : 1;

  /* Flag indicating that RxD has been requested OFF so NV must be updated */
  uint16 rxd_rx_chain_swap_rxd_nv_update : 1;

  /* Whether the state update function needs to be called again later because
      the command processor may be busy currently */
  uint16 rxd_recall_needed : 1;

  /* Whether RxD cleanup command needs to be sent at end of tear down */
  uint16 rxd_send_cleanup_done_needed : 1 ;

    /* Boolean to Track Sec. carrier Status */
  uint16  rxd_sec_carr_deactivated : 1;

  /* hold if an unlock cancel for wrm is in progress */
  uint16 wrm_cancel_in_progress : 1 ;

  /* Holds if we are denied while transitioning ON */
  uint16 rxd_denied_by_wrm : 1;

  uint16 rxd_freeze_flag : 1;

  uint16 :5;

    /* Whether a particular commands needs to be executed by processor in a state transition
     The value for this will be calculated prior to beginning any state transition */
  boolean rxd_cmd_proc[WL1_DEBUG_RXD_NUM_OPS];

    /* Index of command to be processed by processor */
  uint8 rxd_cmd_proc_idx ;

  uint8 rxd_proc_client_pending_bmsk ;

  /* Ids for the primary and diversity RF chains*/
  uint8 rxd_prim_ant_id;
  uint8 rxd_div_ant_id;

  /* The RXD operation mode as read from the NV */
  uint8 rxd_nv_value;

   uint8 wl1_rxd_dsda_state;

    /* State of the RxD state controller*/
  uint8 rxd_state ;

    /* State of the controller when all the commands have been processed */
  uint8 rxd_pend_state ;

  /* Order in which commands will be processed by RxD command processor */
  uint8 rxd_cmd_proc_mode;

  /* Each of the bits corresponds to particular cfg masks */ 
  uint16 rxd_cfg ;

  uint16 rxd_ops;

    /* holds the current rf device for RxD */
  uint16 wl1_rxd_rf_device ;
  
  /* Mask indicating the action items that need to performed for a state transition*/
  uint16 rxd_act;

  /* State of the controller when all the commands have been
     processed. Fields are same as ops variables */
  uint16 rxd_pend_ops;

  uint16 rxd_div_chain_foff_client_mask ;

  /* This timer is utilized for counting up to various hold times for current RxD state.
     Each trigger has its own associated hold time */
  uint16 rxd_state_tmr;

    /* Trigger for RxD state machine transitions.    
     Each of the bits corresponds to particular trigger masks*/
  uint32 rxd_trig;

   /* Triggers contributing to current RxD state */
  uint32 rxd_state_reason_bmsk ;

 /* holds qta gap information */
  wl1_debug_rxd_qta_info_struct_type wl1_rxd_qta;  
  wl1_debug_rxd_pch_mini_info_struct_type rxd_pch_db;
} wl1_rxd_mini_debug_info_struct_type;

typedef PACKED struct PACKED_POST
{
  uint16 scr_code;
  uint16 freq;
  uint8 txd_status;
  uint8 cell_db_idx;
} wl1_debug_mcalwcdma_cme_cell_info_struct_type;

/* CME QSET Config Info  */
typedef PACKED struct PACKED_POST
{
  /** EQ cell index used flag. */
  uint32 used : 1;

  /** Initialize flag, set when the cell_idx changes. */
  uint32 init : 1;

  /** Primary cell flag, which is set for the HS cell when one is present, or
   *  the strongest cell when one isn't.
   */
  uint32 primary_cell : 1;

  /** RxD enable flag. */
  uint32 rxd_enable : 1;

  /** QICE enable flag, set when QICE is to be used with this EQ cell, currently
   *  only during HS, but set for all cells, not just the HS cell.
   */
  uint32 qice_en : 1;

  /** PSC enable flag. */
  uint32 psc_en : 1;

  /** Reframe flag, set when RxD, TxD, or HS state changes. */
  uint32 reframe : 1;

  uint32 num_qice_iterations : 4;

  
  /** The drop counter of the cell, used to drop a cell from the QSET if it is
   *  too weak.  Initialized to 0 and used by wl1_cme_update_qset_drop_counters().
   *  If this cell is weaker than the Ec/Io drop threshold, then increment this
   *  counter.  When this counter exceeds "wl1_cme_oride_qset_drop_ctr_threshold"
   *  and this isn't teh HS cell, then drop it from the QSET.
   */
  uint32 drop_ctr : 4;

   /** Ghost cell Drop Counter */
  uint32  ghost_drop_ctr : 4;

  /** Cell db index. */
  uint32 cell_idx : 8;

  uint32 : 5;

  /** Center window position of the cell in CHIPx1. */
  uint32 pn_position;

  /* Fields used by CME. */

  /** The primary scrambling code. */
  uint16 psc;

  /** The current filtered Ec/Io of the cell.  This is set whenever a cell is
   *  added to the QSET and in wl1_cme_srch_cell_triage_info_cb().  It is used
   *  throughout the code, for example when update the drop counters,
   *  calculating PSET matrices, and when determining what the weakest QSET
   *  cell is.
   */
  uint16 ecio;

  uint8 qset_cell_type;

  /** cell information, set whenever a cell is added to the QSET and
   *  updated whenever wl1_cme_srch_cell_triage_info_cb() is
   *  called */
  wl1_debug_mcalwcdma_cme_cell_info_struct_type  cell_ptr;

} wl1_debug_mcalwcdma_cme_per_qset_cfg_params_info_struct_type;

/* CME Common Info database */
typedef struct
{
  boolean enable;
  uint8 receiver_selection_bmsk[WL1_DEBUG_MCALWCDMA_CME_NUM_RECEIVER_CHAN_TYPES];
  uint8 ic_on_receiver_bmsk;
  uint8 ic_on_cell_id;
}wl1_debug_mcalwcdma_cme_common_info_struct_type;

typedef PACKED struct PACKED_POST
{
  wl1_debug_mcalwcdma_cme_common_info_struct_type common_info;
  wl1_debug_mcalwcdma_cme_per_qset_cfg_params_info_struct_type cfg_params[WL1_DEBUG_MCALWCDMA_CME_MAX_NUM_QSET_CELLS];
} wl1_debug_mcalwcdma_cme_cfg_params_info_struct_type;

typedef PACKED struct PACKED_POST
{

  uint8 oride_qpc_control : 1;

  uint8 oride_qpc_mode : 1;

  /* Debug variables used to enable/disable 
   * 1) The dynamic QPC algorithm
   * 2) SCHIC power Algorithm
   * 3) The fall back to Rake algorithm (matters only if QPC is enabled)
   */
  uint8 dbg_enable_dyn_qpc_alg : 1;
  uint8 dbg_enable_schic_power_alg : 1;
  uint8 dbg_enable_dyn_qice_alg : 1;

  uint8 qice_power_collapse_enabled : 1;

  uint8 :2;
    
  /* Receiver and EQ cell configuration, set and used throughout the
   * code.
   */
  wl1_debug_mcalwcdma_cme_cfg_params_info_struct_type mcal_cfg_params;

  /* When hs_enabled == TRUE, this is the cell db idx of the HS cell.  Set in
   * wl1_cme_hs_state_change_cb() and used throughout the code.
   */
  uint8 hs_cell_idx;

  /* Bitmask of CME/QEST cells which need to be updated in FW.  Set throughout
   * the code and used in wl1_cme_controller().
   */
  uint16 eq_cfg_update_bmsk;

  /* Bitmask of common receiver info that needs to be updated.  Set in
   * wl1_cme_receiver_selection(), wl1_cme_schic_cell_selection(), and
   * wl1_cme_drop_qset_cell() and used in wl1_cme_controller().
   */
  uint16 cmn_cfg_update_bmsk;

  /* Number of cells currently in the QSET.  Set in wl1_cme_add_qset_cell() and
   * wl1_cme_drop_qset_cell() and used throughout the code.
   */
  uint8 num_qset_cells;

  /* Current maximum number of QSET cells. */
  uint8 oride_cur_max_num_qset_cells;
  
  uint16 nghbr_cell_cnt;
} wl1_debug_cme_per_carrier_info_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8                          num_cells[WL1_DEBUG_CME_NUM_CARRIERS];
  wl1_debug_mcalwcdma_cme_cell_info_struct_type cell[WL1_DEBUG_CME_NUM_CARRIERS][WL1_CME_SRCH_RESULT_MINI_NUM_CELLS];
  uint8                          num_cells_above_qset_thresh[WL1_DEBUG_CME_NUM_CARRIERS];
}wl1_debug_cme_stored_srch_info_struct_type;

typedef struct
{
  boolean hs_cell_txd_enabled;
  uint8 num_txd_intf;
  uint8 col_index;
}wl1_debug_cme_carrier_txd_info_struct_type;

typedef struct
{
  uint8 num_of_carrs;
  boolean rxd_enabled;
  wl1_debug_cme_carrier_txd_info_struct_type txd_info[WL1_DEBUG_CME_NUM_CARRIERS];
}wl1_debug_cme_env_params_info_struct_type;

typedef PACKED struct PACKED_POST
{
    /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;
  /* Flag that notes if the SRCH Cell info callback results are being ignored (Suspend CME triage).
   * This is used by wl1_cme_srch_cell_triage_info_cb().
   */
  uint16  ignore_srch_cell_info_cb : 1;

  /* TRUE when RxD is active, FALSE otherwise.  Set in wl1_cme_rxd_trans_cb()
   * and used by wl1_cme_reframe().
   */
  uint16 rxd_state : 1;

  /* TRUE when DRX is active, FALSE otherwise.  Set in wl1_cme_drx_status_cb()
   * and used by wl1_cme_reframe().
   */
  uint16 cpc_edrx_drx_state : 1;

  /* TRUE when LTE FMO is active, FALSE otherwise; Set in wl1_cme_lte_fmo_status_cb()
   */
  uint16  lte_fmo_state : 1;

  /* Flag that notes if HS is active.  Set in wl1_cme_hs_state_change_cb() and
   * used throughout the code to determine is QICE should be enabled on QSET
   * cells, if EQ/QICE should be set in the demod path, and to determine the
   * maximum size of the QSET.
   */
  uint16  hs_enabled : 1;

  /* Notes if a MDSP sync command needs to be sent.  Set in wl1_cme_controller()
   * and used in mcalwcdma_cme_demod_cfg_cmd_done_cb().
   */
  uint16  pending_flag : 1;

  /* Notes if an MDSP sync command has been sent but the post callback has yet
   * to be called.
   */
  uint16  config_in_progress : 1;

  /* Notes if DCH cleanup completed confirmation is to be sent */
  uint16  cleanup_cnf_needed : 1;

  /* Record the type of MCVS TxD Status sent last time */
//  wl1_cme_mcvs_struct_type mcvs_info;


  /* Flag which notes if the CFN update callback is registered with SEQ.  This
   * callback is used to send any pending config or deregister the QICE client
   * with CLKREGIME when the SRCH Traige callback is not registered.  Used in
   * wl1_cme_controller().
   */
  uint16  cfn_upd_cb_registered : 1;

    /* Stores if MCVS Update Reqd. */
  uint16  mcvs_update_reqd : 1;

  uint16 :6;

    /* Boolean to Hold Carr activate/deactivate Status */
  boolean  carr_deactivated[WL1_DEBUG_CME_NUM_CARRIERS];

  /* Flag that remembers who asked for CME to be in SUSPEND state */
  uint16 suspend_cme_controller_client_mask;

  uint16 ignore_srch_cell_info_cb_client_mask;

    /* The current CME state
   */
  uint8 cme_state;

/* The state CME is going to move to, set in wl1_cme_enable(),
   * wl1_cme_disable(), and wl1_cme_cm_act_deact_cb() and used by
   * wl1_cme_update_state().
   */
  uint8 cme_next_state;

  /* Used to select which carrier to operate on. */
  uint8 carrier;

  /* Carrier specific info. */
  wl1_debug_cme_per_carrier_info_struct_type carrier_info[WL1_DEBUG_CME_NUM_CARRIERS];
  
  /* bitmasks used to keep track of cells being dropped from qset */
  uint16 drop_ngbr_cell_bmsk;

  uint16 cfg_done_ngbr_cell_bmsk;

  /* number of active carriers*/
  uint8 num_carriers;

  /* BMSK to store cells which are Newly CFGD Ngbr cells */
  uint16 ngbr_cfgd_cell_bmsk;

  /* BMSK used to Store Cells to Be deconfigured before calling Resume Cb */
  uint16 cme_drop_cell_bmsk_for_resume;

  /* Database to hold cell pointers returned by searcher */
  wl1_debug_cme_stored_srch_info_struct_type wl1_cme_srch_info;

  /* Variable that will hold latest cfg env after query */
  wl1_debug_cme_env_params_info_struct_type cme_env_params;
} wl1_cme_mini_debug_info_struct_type;


/*                               IRAT                                  */
/*                                                                         */

/* ============support structs============= */
#define WL1_DEBUG_SRCH_GSM_MAX_MDSP_CMD_BUF_SIZE (6)
#define WL1_DEBUG_MCALWCDMA_SRCH_CM_MAX_NUM_CMQ (2)
#define WL1_DEBUG_SRCHCM_GSM_PWR_SCAN_CMD_MAX_PER_GAP (3)
#define WL1_DEBUG_SRCHCM_GSM_SCH_DEC_CMD_MAX_PER_GAP (2)
#define WL1_DEBUG_SRCHCM_GSM_ADV_INFO_COUNT (1)
#define WL1_DEBUG_MCALWCDMA_CM_IRAT_MAX_RF_BUF (2)
#define WL1_DEBUG_IRAT_NUM_CARR (3)

/* GSM advance info buffer count.
   Keep 1 extra buffer as the GSM mDSP command buffer pointers
   are held in this in the measurement info structure */
#define WL1_DEBUG_SRCHCM_GSM_ADV_INFO_BUF_COUNT (WL1_DEBUG_SRCHCM_GSM_ADV_INFO_COUNT + 1)

/* This structure defines the compressed mode state control parameters */
typedef PACKED struct PACKED_POST
{
  /* starting slot number (0..14) for transmission gap */
  uint8 tg_start_slot_num;
  /* transmission gap length in slots (1..14)
     This may not be specified. RRC must set TGL2 = TGL1 */
  uint8 tg_len_1;
  uint8 tg_len_2;
  /* transmission gap pattern length in frames (1..144)
     This may not be specified. RRC must set TGPL2 = TGPL1 */
  uint8 tgp_len_1;
  uint8 tgp_len_2;
  /* Transmission gap repetition count. Valid range 1..511
     If 0 then it is infinite */
  uint16 tgp_rep_count;
  /* transmission gap distance (0, 15..269) */
  uint16 tgd;
} wl1_debug_l1_cm_tgp_state_ctrl_info_struct_type;

/* TGPS info. 14 bytes */
typedef PACKED struct PACKED_POST
{
  uint8 status_flag;  /* b0(active): TG activate/de-activate.
                       b1(activate_action_cfn_valid): If activate action CFN is valid then TGPS is activated at
                           start_cfn on or after activate_action_cfn.
                       b2(info_valid): states if the following info is valid.
                           If this is set to FALSE it means that compressed info has
                           already been received and this is only meant for enabling or
                           disabling the compressed mode for the TGPSI for this info.
                    */

  uint8 tgp_seq_id; /* TGPSI (1..6) */
  uint8 start_cfn;  /* TG starting CFN */
  uint8 activate_action_cfn;  /* valid only if b1 of status_flag is 1*/

  /* The following members are valid only if b2 of status_flag is 1 */
  /* transmission gap measurement purpose: l1_cm_tg_meas_purpose_enum_type */
  uint8 tg_meas_purpose;
  /* compressed mode state control info (9 bytes)*/
  wl1_debug_l1_cm_tgp_state_ctrl_info_struct_type tgp_state_ctrl_info;

} wl1_debug_l1_cm_tgp_seq_info_struct_type;

/* CM action information. 3 bytes */
typedef PACKED struct PACKED_POST
{
  uint8 info_flag;  /* b0(valid): indicate if this info is valid or not.
                       b1(next_frame_occupied): indicate that next frame is a compressed frame and will be using the
                           compressed mode pattern sequence info from sequence given by Id
                           l1_cm_tgps_id_active_in_next_frame.
                       b2(cm_prep_reqd): indicate that next frame should be configured for compressed mode.
                       b3(cm_exit_reqd): indicate that some of the compressed mode pattern sequence are out of
                           compressed mode and if there is no other pattern sequence that ask for the
                           next frame to be compressed frame then normal mode parameters must be restored.
                    */
  /* next frame CFN */
  uint8 next_frame_cfn;
  /* transmission gap pattern sequence Id active in next frame. Valid only
     when next_frame_occupied is TRUE */
  uint8 tgpsi_active_in_next_frame;
  
} wl1_debug_l1_cm_action_info_struct_type;

/* CM state information. 8 bytes */
typedef PACKED struct PACKED_POST
{
  uint8 state_flag; /* b0(l1_cm_cleanup_pending): becomes TRUE as soon as someone requests a cleanup of l1cmmgr, and remains
                           TRUE until l1cmmgr finishes cleaning up.
                       b1(l1_cm_gap_in_progress): TRUE when current in gap
                       b2(l1_cm_action_frozen_state): indicate that compressed actions are frozen or not.
                    */

  /* internal notification paramaters and state for CM clients. used when patterns go
     active and deactive to see whether a client should be notified of CM's status */
  /* Whether the client is registered.  b0-bx used for client id(0..L1_CM_NUM_CLIENTS) */
  uint16 is_registered; 
  /* describes which callback (act or deact) we called last: b=0 for L1_CM_DEACTIVE,
     b=1 for L1_CM_ACTIVE; b0-bx used for client id(0..L1_CM_NUM_CLIENTS) */ 
  uint16 prev_notified;

  /* information index to be used by current coming frame */
  uint8 l1_cm_current_stat_head;
  /* Lets us know which cfn l1cmmgr previously ran state_update() in.  By 
   comparing against the current CFN, you can use it to see if 
   l1_cm_state_update() has been called yet in the current CFN.  Always intlock
   before reading or writing */
  uint8 l1_cm_last_update_cfn;

  /* prev served CFN TGMP: l1_cm_tg_meas_purpose_enum_type */
  uint8 l1_cm_prev_served_tgmp;

} wl1_debug_l1_cm_state_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /* GSM Frame that the buffer can be freed */
  uint8 gfn_to_free;
} wl1_debug_mcalwcdma_cm_gsm_rf_buf_info_struct_type;

/* RXLM buffer usage: 19 bytes */
typedef PACKED struct PACKED_POST
{
  /* b0-b6: Home RAT RxLM index. b7(buffer_in_use): 1 means buffer in use, not free */
  uint8 source_rxlm_index;

  /* TGMP of the buffer : mcalwcdma_cm_tgmp_enum_type */
  uint8 tgmp;

  struct
  {
    /* TRM request ID */
    trm_irat_identifier req_id;
    /* IRAT duration for TRM to lock RF resource */
    uint32 meas_duration;
  } wrm_meas_req_info;

  union
  {
    wl1_debug_mcalwcdma_cm_gsm_rf_buf_info_struct_type gsm_pending_rf_buf;
   /* To be included at a later stage */
   /* mcalwcdma_lte_rf_buf_struct_type lte_pending_rf_buf; */
  } irat_rf_buf;

  /* this structure also will contain the rxlm buffer index info passed to 
     other RAT's as part of searches */
  union
  {
    struct
    { 
      /* Interf neighbor RxLM buffer index */
      uint32 interf_buf_idx;
    } interf_info;

    /** (for GSM searches) buffer index for the WL1_XLM_RX_CHAIN0_IRAT_GSM_NBR
        client in addition to the WCDMA home client */ 
    struct
    {
      uint32 gsm_buf_idx;
    } gsm_info;

    /** (for LTE searches) buffer indicies for the 
        WL1_XLM_RX_CHAIN0_IRAT_LTE_NBR and WL1_XLM_RX_CHAIN1_IRAT_LTE_NBR
        clients */
    struct
    {
      uint32 lte_ch0_buf_idx;
      uint32 lte_ch1_buf_idx;
    } lte_info;

  } rxlm_buf_info;

} wl1_debug_mcalwcdma_cm_irat_rf_buf_ctrl_info_struct_type;

/* RF buffer usage information: 39 bytes */
typedef PACKED struct PACKED_POST
{
  /* Structs to control the RF buffer usage for W2G and W2L */
  wl1_debug_mcalwcdma_cm_irat_rf_buf_ctrl_info_struct_type mcalwcdma_cm_irat_rf_buf_ctrl[WL1_DEBUG_MCALWCDMA_CM_IRAT_MAX_RF_BUF];

  /* RF Buffer index last used */
  uint8 mcalwcdma_cm_rf_buffer_last_used;

} wl1_debug_irat_rf_buffer_usage_info_struct_type;

/* Struct for RF source parameters */
typedef PACKED struct PACKED_POST
{
  /* Source Device id */
  uint32 device;
  /* Source Div Device id if RxD is enabled */
  uint32 div_device;
  /* Serving W DL chan */
  uint16 serving_chan[WL1_DEBUG_IRAT_NUM_CARR];
  /* Serving W band */
  uint8 serving_band;
  /* Number of carriers: 0 - Single carrier 1- Dual Carrier 2- 3-Carrier*/
  uint8 carrier_type;
  /* Indicate if RxD is enabled */
  boolean div_en; 
} wl1_debug_wl1_rf_source_params_info_struct_type;

/* structure definition for inter frequency RF setup information: 28 bytes */
typedef PACKED struct PACKED_POST
{
  /* Number of RF source items, 1 for single band 2 for Dual Band */
  uint8 num_source_item;

  /* Last triggered search Inter-freq neighbor freq info */
  uint16 interf_freq_info;

  #ifdef FEATURE_WCDMA
  /* Source params for RF header struct. only save source_params[0]. 17 bytes*/
  wl1_debug_wl1_rf_source_params_info_struct_type source_params;
  #endif

  /* pointer to RF buffer to be used for cleanup after search */
  void *rf_buf_cleanup_ptr[WL1_DEBUG_MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];
} wl1_debug_srchcm_interf_rf_set_info_struct_type;

/* interF measurement info: 43 bytes */
typedef PACKED struct PACKED_POST
{
  uint8 state_flag; /* b0(srchcm_interf_step1_srch_in_progress): 
                       b1b2(srchcm_interf_psc_acq_list_srch_in_progress[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ]): 
                       b3b4(srchcm_interf_reconf_list_srch_in_progress[MCALWCDMA_SRCH_CM_MAX_NUM_CMQ]): indicate if step1
                              or any type of list search in progress in WFW This desn't reflect the suspended state of search.
                              Search type is still considered in progress. 
                    */

  /* This struct contains interfrequency RF setup related information: 28 bytes */
  wl1_debug_srchcm_interf_rf_set_info_struct_type srchcm_interf_rf_setup_info;

  /* Type of search last executed */
  uint8 srchcm_interf_last_search_type;
  /* WCDMA inter-frequency search state machine related variables */
  uint8 srchcm_interf_step1_srch_state;
  uint8 srchcm_interf_list_srch_state;

  /* step 1 non coherent length done upto the current task programmed */
  uint8 srchcm_step1_non_coh_int_done_len;

  /* variable to keep track of how many step1 searches we scheduled so if some
     don't get completed in WFW then we will know to reschedule the searches */
  uint8 srchcm_step1_sched_nn[WL1_DEBUG_MCALWCDMA_SRCH_CM_MAX_NUM_CMQ];

  /* this is the amount of time (in chips x8) it takes for RF to tune away to a 
     neighboring WCDMA frequency (it includes ssbi write time, pll settling time,
     and agc settling time) */
  uint32 mcalwcdma_cm_w2w_tune_away_time_cx8;
  /* this is the amount of time (in chips x8) it takes for RF to tune back to the
     home frequency at the end of a neighbor search (it includes ssbi write time,
     pll settling time, and agc settling time) */
  uint32 mcalwcdma_cm_w2w_tune_back_time_cx8;

} wl1_debug_irat_interf_info_struct_type;

/* GSM command structure definition */
typedef PACKED struct PACKED_POST
{
  /* command type */
  uint8 cmd_type;
  /* frame number at which the command to be sent */
  uint8 frame_num;

} wl1_debug_srchgsm_mdsp_cmd_info_struct_type;

/* GSM CM startup command structure definition. 25 bytes*/
typedef PACKED struct PACKED_POST
{
  /* Command send frame number */
  uint8 frame_num;
  /* Num RF header source items */
  uint8 num_source_item;
  /* CM startup QS offset */
  uint16 offset_qs_to_start;
  /* RF buffer info */
  void *rf_buf;
  #ifdef FEATURE_WCDMA
  /* RF source params: only save source_params[0].17 bytes */
  wl1_debug_wl1_rf_source_params_info_struct_type source_params;
  #endif
} wl1_debug_srchcm_gsm_startup_cmd_param_info_struct_type;

/* GSM CM cleanup command structure definition. 3 bytes */
typedef PACKED struct PACKED_POST
{
  /* Command send frame number */
  uint8 frame_num;
  /* CM cleanup QS offset */
  uint16 offset_qs_to_end;

} wl1_debug_srchcm_gsm_cleanup_cmd_param_info_struct_type;

/* GSM RSSI power meas command structure definition. 8 bytes*/
typedef PACKED struct PACKED_POST
{
  /* Command send frame number */
  uint8 frame_num;
  /* number RSSI scan burst */
  uint8 num_bursts;
  /* ARFCN of the carrier for which RSSI scan to be done. only save carrier_arfcn[0] */
  uint16 carrier_arfcn;

  /* starting offset of start of the burst: only save
     offset_to_start[0] and offset_to_start[SRCHCM_MAX_PWR_MEAS_BURST_PER_GSM_FR-1]*/
  uint16 offset_to_start[2];

} wl1_debug_srchcm_gsm_pwr_scan_cmd_param_info_struct_type;

/* 11 bytes */
typedef PACKED struct PACKED_POST
{
  /* cell channel frequency */
  uint16 arfcn;
  /* offset in frame where to start SCH decode or FCCH detect*/
  uint16 offset_qs;
  /* for SCH decode */
  /* frequency offset for this cell from the fcch tone detect */
  int16 frequency_offset;
  /* for FCCH detect */
  /* window size for FCCH detect */
  uint16 fcch_win_size;
  /* for SCH decode */
  /* number of frame available for SCH decode */
  uint8 num_frames;
  /* timing valid. If valid then SCH decode otherwise FCCH detect */
  boolean timing_valid;
  /* first GSM frame number in which to start FCCH detect or SCH decode*/
  uint8 first_frame;
} wl1_debug_srchcm_gsm_bsic_id_params_info_struct_type;

/* 7 bytes */
typedef PACKED struct PACKED_POST
{ 
  /* GSM frame number in which to do SCH decode*/
  uint8 sch_decode_fr_num;
  /* cell channel frequency */
  uint16 arfcn;
  /* offset in frame where to start SCH decode */
  uint16 sch_decode_qs_offset;
  /* frequency offset for the SCH decode */
  int16 frequency_offset;
} wl1_debug_srchcm_gsm_bsic_reconf_params_info_struct_type;

/* GSM FCCH ACQ start command structure definition: 7 bytes */
typedef PACKED struct PACKED_POST
{
  /* Command send frame number */
  uint8 frame_num;
  /* RSSI of the GSM cell */
  int16 signal_strength;
  /* ACQ start QS offset */
  uint16 start_offset;
  /* Frequency offset for this GSM cell */
  int16 frequency_offset;
} wl1_debug_srchcm_gsm_start_acq_cmd_param_info_struct_type;

/* GSM FCCH ACQ stop command structure definition. 3 bytes*/
typedef PACKED struct PACKED_POST
{
  /* Command send frame number */
  uint8 frame_num;
  /* ACQ stop QS offset */
  uint16 stop_offset;

} wl1_debug_srchcm_gsm_stop_acq_cmd_param_info_struct_type;

/* GSM SCH decode command structure definition. 11 bytes */
typedef PACKED struct PACKED_POST
{
  /* Command send frame number */
  uint8 frame_num;
  /* GSM cell ARFCN */
  uint16 arfcn;
  /* RSSI of the GSM cell */
  int16 signal_strength;
  /* SCH decode uncertaintity window */
  uint16 uncertainty_win_size;
  /* Offset of SCH burst center from frame boundary */
  uint16 offset_to_center_of_burst;
  /* Frequency offset for this GSM cell */
  int16 frequency_offset;
} wl1_debug_srchcm_gsm_sch_decode_cmd_param_info_struct_type;

/* GAP parameters in GSM time unit structure definition */
typedef PACKED struct PACKED_POST
{
  /* CM gap start frame number */
  uint8 frame_num;
  /* CM gap start QS offset */
  uint16 offset_qs;
  /* CM gap length in GSP QS */
  uint16 gap_len_qs;
} wl1_debug_schcm_cm_gap_param_gsm_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /* start frame number at which pwr scan can be started */
  uint8 start_frame_num;
  /* This is the starting offset in the start frame */
  uint16 start_offset_qs;
  /* window size in which the power scan can be done.
     Note that this can be more than 1 frame length */
  uint16 win_size_qs;
} wl1_debug_debug_srchcm_gsm_pwr_scan_params_info_struct_type;

/* GSM measurement info struct definition.
   This structure keeps the parameters for various type of measurement and
   the GSM command parameters. The meas info is the UNION of all types of
   possible measurement. The measurement type is indicated by GSM measurement
   type. 74 bytes */
typedef PACKED struct PACKED_POST
{
  uint8 state_flag; /* b0(valid): indicate if this measurement info is valid. The info is Invalid
                                  to start with. If gets updates with each GSM advance info update.
                       b1(in_use): indicate that this information is in use and shall not be over-written.
                       b2(gap_purpose_is_oride): indicate that gap is used with overridden or alternate purpose. 
                       b3(rxd_en in startup cmd): rxd enable status
                       b4(srchcm_rssi_in_sch_trigger_status): if RSSI burst is getting triggered inside SCH gap
                    */

  /* indicate the CFN at which this info is applicable */
  uint8 cfn;

  /* GSM measurement type. indicate the type of measurement info kept
     in the union below */
  uint8 meas_purpose;

  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif

  /* GSM GAP parameters. indicate GFN, QS offset etc. 5 bytes*/
  wl1_debug_schcm_cm_gap_param_gsm_info_struct_type gsm_gap_params;

  /* CM startup and cleaup command parameters. This is applicable to any type of
     GSM measurement */
  wl1_debug_srchcm_gsm_startup_cmd_param_info_struct_type cm_startup_cmd_params; /* 25 bytes */
  wl1_debug_srchcm_gsm_cleanup_cmd_param_info_struct_type cm_cleanup_cmd_params; /* 3 bytes */

  union  /* 30 bytes */
  {
    /* RSSI scan info. 30 bytes*/
    /* ------------- */
    /* RSSI scan GSM measurement and command parameters. */
    struct
    {
      /* RSSI scan measurement parameters */
      wl1_debug_debug_srchcm_gsm_pwr_scan_params_info_struct_type pwr_scan_params; /* 5 bytes */

      /* RSSI scan GSM command parameters */
      uint8 num_pwr_scan_cmd;
      /* 8*3 = 24 bytes */
      wl1_debug_srchcm_gsm_pwr_scan_cmd_param_info_struct_type cmd_params[WL1_DEBUG_SRCHCM_GSM_PWR_SCAN_CMD_MAX_PER_GAP];
    } pwr_meas;

    /* BSIC Id parameter. 24 bytes */
    /* ----------------- */
    struct
    {
        /* BSIC Id measurement parameters: 11 bytes */
        wl1_debug_srchcm_gsm_bsic_id_params_info_struct_type bsic_id_params;

        /* FCCH ACQ detect OR SCG decode command parameters. 13 bytes */
        union
        {
          struct /* 13 bytes */
          { 
            uint8 cmd_flag; /* b0(sch_decode in start_acq): indicate SCH to be decode if the tone is detected. 
                               b1(do_cleanup in stop_acq): indicate to perform cleanup for RF unwind along with stop ACQ.
                               b2():  */
            /* GSM cell ARFCN */
            uint16 arfcn;
            wl1_debug_srchcm_gsm_start_acq_cmd_param_info_struct_type acq_start;
            wl1_debug_srchcm_gsm_stop_acq_cmd_param_info_struct_type acq_stop;
          } acq_cmd;
          
          /* 11 bytes */
          wl1_debug_srchcm_gsm_sch_decode_cmd_param_info_struct_type sch_decode[WL1_DEBUG_SRCHCM_GSM_SCH_DEC_CMD_MAX_PER_GAP];
        } cmd_params;
    } bsic_id;

    /* BSIC reconf parameter. 19 bytes*/
    /* --------------------- */
    struct
    {
      /* Number of cell in the candidate measurement parameter list */
      uint8 num_cell_info;
      struct
      {
        /* SCH decode measurement parameters */
        wl1_debug_srchcm_gsm_sch_decode_cmd_param_info_struct_type cmd_params; /* 11 bytes */
      } bsic_reconf_info;
    } bsic_reconf;
  } meas_info;

} wl1_debug_srchcm_gsm_meas_per_buf_info_struct_type;

/* 76 bytes */
typedef PACKED struct PACKED_POST
{ 
  /* Current usable index for srchcm_gsm_cm_meas_info[]*/
  uint8 srchcm_gsm_cur_info_idx;
  uint8 srchcm_gsm_adv_info_idx;

  /* GSM CM measurement related processing. 74 bytes */
  wl1_debug_srchcm_gsm_meas_per_buf_info_struct_type srchcm_gsm_cm_meas_info[WL1_DEBUG_SRCHCM_GSM_ADV_INFO_BUF_COUNT];
} wl1_debug_srchcm_gsm_meas_info_struct_type;


/* 59 bytes */
typedef PACKED struct PACKED_POST
{ 
  uint8 cmd_flag; /* b0(sch_decode in start_acq): indicate SCH to be decode if the tone is detected. 
                     b1(do_cleanup in stop_acq): indicate to perform cleanup for RF unwind along with stop ACQ.
                     b2(rxd_en in startup cmd): rxd enable status.
                    */
  #ifdef FEATURE_DUAL_SIM
  /* as_id received from RRC. Passed to GSM L1 for DSDS */
  sys_modem_as_id_e_type as_id;
  #endif
  /* RSSI scan GSM command parameters */
  uint8 num_pwr_scan_cmd;
  /* Startup command before GSM measurements during cell reselection. 25 bytes */
  wl1_debug_srchcm_gsm_startup_cmd_param_info_struct_type srchcrgsm_startup_cmd;
  /* Cleanup command after GSM measurement during cell reselection. 3 bytes */
  wl1_debug_srchcm_gsm_cleanup_cmd_param_info_struct_type srchcrgsm_cleanup_cmd;
  /* RSSI Power Scan command during Idle Mode. only save the first and the last cmd. 8*2=16 */
  wl1_debug_srchcm_gsm_pwr_scan_cmd_param_info_struct_type srchcrgsm_pwr_scan_cmd[2];
  /* GSM cell ARFCN in start/stop acq */
  uint16 arfcn;
  /* Acquisition start command. 7 bytes */
  wl1_debug_srchcm_gsm_start_acq_cmd_param_info_struct_type srchcrgsm_acq_start_cmd;
  /* Acquisition stop command. 3 bytes */
  wl1_debug_srchcm_gsm_stop_acq_cmd_param_info_struct_type srchcrgsm_acq_stop_cmd;
  /* SCH decode command. 11 bytes*/
  wl1_debug_srchcm_gsm_sch_decode_cmd_param_info_struct_type srchcrgsm_sch_decode_cmd;
} wl1_debug_irat_w2g_idle_fmo_srch_cmd_info_struct_type;


/* 95 bytes */
typedef PACKED struct PACKED_POST
{ 
  /* GSM frame number that counts 51 multiframe modulo count */
  uint8 srchgsm_gsm_frame_num;

  /* GSM command buffer management related variables */
  uint8 srchgsm_mdsp_cmd_buf_head;
  uint8 srchgsm_mdsp_cmd_buf_tail;
  uint8 srchgsm_mdsp_cmd_buf_used;

  /* GSM comand buffer 2*6 = 12 bytes */
  wl1_debug_srchgsm_mdsp_cmd_info_struct_type srchgsm_mdsp_cmd_buf[WL1_DEBUG_SRCH_GSM_MAX_MDSP_CMD_BUF_SIZE];

  union /* 76 bytes */
  { 
    /* GSM CM measurement related processing. 76 bytes */
    wl1_debug_srchcm_gsm_meas_info_struct_type srchcm_gsm_cm_meas;

    /* GSM IDLE/FMO measurement related processing. 59 bytes */
    wl1_debug_irat_w2g_idle_fmo_srch_cmd_info_struct_type srchcm_gsm_idle_fmo_meas_info;
  } w2g_meas;

  /* indicate the state of the GSM RSSI measurement */
  uint8 srchcm_gsm_rssi_meas_state;
  /* indicate the state of the GSM BSIC identification */
  uint8 srchcm_gsm_bsic_id_state;
  /* indicate the state of the GSM BSIC verification */
  uint8 srchcm_gsm_bsic_reconf_state;

} wl1_debug_irat_w2g_info_struct_type;

/* Control structure for W2L search. 7 bytes */
typedef PACKED struct PACKED_POST
{
  uint8 ctrl_flag; /* b0(srch_in_progress): Indicates that a search request came and it has started.
                      b1(send_deinit_needed): Indicates the need to send DEINIT_CNF after searches are done.
                      b2(agc_tune_done_isr_rcvd): Flag to check if AGC tune done ISR is received from MDSP.
                      b3(wsrchlte_srch_init_req_sent): Indicates if lte init req has been sent.
                    */
  /* State of W to L search */
  uint8 srch_state;
  /* srch mode */
  uint8 srch_mode;
  /* Type of search in progress */
  uint8 srch_type;

  #ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  /* Indicates the earfcn_idx where CM is in progress */
  uint8 earfcn_idx;
  /* The CFN when the CM GAP starts */
  uint16 curr_cfn;
  #endif
} wl1_debug_wsrchlte_ctrl_info_struct_type;

/* RF params for W2L idle and CM search/meas. 22 bytes */
typedef PACKED struct PACKED_POST
{
  uint8 num_source_item;
  #ifdef FEATURE_WCDMA
  /* Source parameters to be passed to LTE. only save source_params[0]. 17 bytes */
  wl1_debug_wl1_rf_source_params_info_struct_type source_params;
  #endif
} wl1_debug_wsrchltedrv_rf_param_info_struct_type;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
typedef PACKED struct PACKED_POST {
/*Range is 0-4095*/
  int16 frame_qty;

/*Range is 0-307199*/
  int32 chip_offset_cx8_qty;
} wl1_debug_tlm_tfn_info_struct_type;

/* AGC parameter info struct type */
typedef PACKED struct PACKED_POST
{
  /* The AGC value for the frequency */
  int32 agc_value;
} wl1_debug_wsrchltedrv_agc_param_info_struct_type;

/* Struct for LTE Connected Mode info related to gap params and srch/meas params. 64 bytes */
typedef PACKED struct PACKED_POST
{
  uint8 info_flag; /* b0(valid): Flag to check contents are updated and valid to be used in next frame's gap.
                      b1(freq_state_included): TRUE if the agc info is populated and to be used.
                      #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
                      b2(vcell_info_present): Virtual LTE cell is the cell with the best SSS_CORR in current frequency.
                                              TRUE if the vcell info is present and the next gap is used for measurement.
                      #endif
                      b3(dl_bandwith_present): if bandwidth information(dl_bandwidth) is valid
                      b4(search_params.perform_search): indicate search & meas or only meas.
                      b5(search_params.measure_newly_detected_cells):  indicate to measure new cells; TRUE only if perform_search is TRUE
                      b6(asap_gap_start): Gap will start as soon as possible. gap_start field will be ignored but gap_end should 
                                          remain valid. used by idle srch only.
                      b7(use_auto_schedule_hint): if use default schedule hint AUTO when calling LTE API to schedule gap. used by idle srch only.
                    */

  /* Frame number for starting the gap */
  uint8 cfn;
  /* Valid EARFCN index to search/meas on */
  uint8 earfcn_idx;

  /* freq and gap info and search-meas params to be sent to LTE */
  /* The frequency on which to tune to. range 0...65535 */
  uint16 freq;

  #if defined(FEATURE_WCDMA_TO_LTE_FACH) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH)
  /* Next frequency information for gap scheduling */
  uint16 next_freq;
  #endif

  /* The gap start frame number and offset for LTE search/Meas. (in FRC time). 6 bytes */
  wl1_debug_tlm_tfn_info_struct_type gap_start_frame_offset;

  /* The gap end frame number and offset for LTE search/Meas as seen by LTE (in FRC time) 6 bytes*/
  wl1_debug_tlm_tfn_info_struct_type gap_end_frame_offset;

  /* The frame number and offset for LTE to switch off its clocks.
  Happens after the RF tune back on source RAT (in FRC time) 6 bytes*/ 
  wl1_debug_tlm_tfn_info_struct_type clock_off_frame_offset;

  /* RF parameters to be sent to LTE. 22 bytes */
  wl1_debug_wsrchltedrv_rf_param_info_struct_type rf_params;

  /* Bandwidth information. 1 byte */
  uint8 dl_bandwidth;

  /* AGC parameters. 4 bytes */
  wl1_debug_wsrchltedrv_agc_param_info_struct_type agc_info;

  /* The accumulated timing drift from the source RAT stmr, between
     the last STARTUP_REQ and this STARTUP_REQ.
     Units are in LTE RTC/OSTMR ticks, 30720 ticks/ ms which
     is equivalent to WCDMA cx8.
     used by idle srch only. */
  int32 timing_adjustment;

  /* RF tune startup time */
  uint32 startup_time_cx8;
  /* RF tune cleanup time */
  uint32 cleanup_time_cx8;

} wl1_debug_debug_wsrchlte_srch_meas_param_info_struct_type;

/* W2L debug info struct. 71 */
typedef PACKED struct PACKED_POST
{
  /* Drivers control structure. 7 bytes */
  wl1_debug_wsrchlte_ctrl_info_struct_type wsrchlte_ctrl_info;

  /* Object for sending LTE Timed Srch Meas Req for CM/IDLE/FMO.64 bytes */
  wl1_debug_debug_wsrchlte_srch_meas_param_info_struct_type wsrchlte_srch_meas_info;

} wl1_debug_irat_w2l_info_struct_type;
#endif

/* CM gap measurement info. Availabe only when L1 state is in DCH: 145 bytes */
typedef PACKED struct PACKED_POST
{
   /* CM info. 50 bytes */
   uint8 info_valid_flag; /* b1-b6(l1_cm_info_valid[L1_CM_MAX_TGP_SEQ_COUNT]): correspond to TGPSI1-6.
                            Compressed mode info valid. This is initialized to FALSE at begining.
                            It is set to TRUE when compressed mode info update function is called.
                            It is used in validation that if the compressed mode info update will
                            be called if no info in it and not previously set. 
                          */
   /* saved at every CM update frame event */
   uint8 l1_cm_prev_cfn_served;

   /* current cfn and next cfn gap info */
   wl1_debug_l1_cm_tgp_seq_info_struct_type l1_cm_tgp_seq_info[2]; /* 14*2 = 28 bytes */
   wl1_debug_l1_cm_action_info_struct_type  l1_cm_action_info[2]; /* 3*2 = 6 bytes */
   wl1_debug_l1_cm_state_info_struct_type   l1_cm_state_info; /* 8 bytes */
   
   /* the type of the ongoing measurement: 0-w2w,1-w2g,2-w2l */
   uint8 meas_type; 
   /* meas info. 95 bytes */
   union
   {
     /* Interf meas info: 43 bytes */
     wl1_debug_irat_interf_info_struct_type irat_interf_info;
   
     /* W2G meas info. 95 bytes*/
     wl1_debug_irat_w2g_info_struct_type irat_w2g_info;
#ifdef FEATURE_WCDMA_TO_LTE
     /* W2L meas info. 64 bytes */
     wl1_debug_irat_w2l_info_struct_type irat_w2l_info;
#endif
   } cur_meas;

} wl1_debug_irat_cm_meas_info_struct_type;

/* IDLE measurement info. Availabe only when L1 state is in IDLE: 95 bytes */
typedef PACKED struct PACKED_POST
{   
   /* the type of the ongoing measurement: 0-w2w,1-w2g,2-w2l */
   uint8 meas_type; 

   /* meas info. 95 bytes */
   union
   {
     /* W2G meas info. 95 bytes*/
     wl1_debug_irat_w2g_info_struct_type irat_w2g_info;
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
     /* W2L meas info. 64 bytes */
     wl1_debug_irat_w2l_info_struct_type irat_w2l_info;
#endif
   } cur_meas;

} wl1_debug_irat_idle_meas_info_struct_type;

/* FMO gap measurement info. Availabe only when L1 state is in FACH: 95 bytes */
typedef PACKED struct PACKED_POST
{   
   /* the type of the ongoing measurement: 0-no meas,1-w2w,2-w2g,3-w2l */
   uint8 meas_type; 

   /* meas info. 95 bytes */
   union
   {
     /* Interf meas info: 43 bytes */
     wl1_debug_irat_interf_info_struct_type irat_interf_info;
   
     /* W2G meas info. 95 bytes*/
     wl1_debug_irat_w2g_info_struct_type irat_w2g_info;
#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
     /* W2L meas info. 64 bytes */
     wl1_debug_irat_w2l_info_struct_type irat_w2l_info;
#endif
   } cur_meas;

} wl1_debug_irat_fmo_meas_info_struct_type;



/* this is the general struct for IRAT debug API. struct name with "debug_" means it's for this debug API only.
   other structs and constants are defined and used in working code. 184 bytes */
typedef PACKED struct PACKED_POST
{
    /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;
   /* RF buffer use info: 39 bytes */
   wl1_debug_irat_rf_buffer_usage_info_struct_type rf_buffer_usage_info;

   /* accessed based on l1m state. 145 bytes */
   union
   {
     wl1_debug_irat_cm_meas_info_struct_type cm;
     wl1_debug_irat_idle_meas_info_struct_type idle;
     wl1_debug_irat_fmo_meas_info_struct_type fmo;
   } meas_info;

} wl1_irat_mini_debug_info_struct_type;

/*                           DATAPATH                              */
/*                                                                         */

/* Number of channelisation codes for SCCH in HS PDSCH */
#define WL1_DEBUG_L1_DL_HS_SCCH_CODES_MAX 4
#define WL1_DEBUG_WFW_DEMOD_HS_CM_CTRL_TABLE_SIZE 16
/* Maximum CCTrCh Id counts that can be used by RRC or L1 manager */
#define WL1_DEBUG_L1_CCTRCH_ID_COUNT 6
#define WL1_DEBUG_WFW_DEMOD_HS_CM_CTRL_TABLE_NUM_BUF 2

/* Carrier index for MC-HSDPA */
typedef enum
{
  WL1_DEBUG_L1_HSDPA_CARRIER_0,
  WL1_DEBUG_L1_HSDPA_CARRIER_1,
 WL1_DEBUG_L1_HSDPA_CARRIER_2,
 #ifdef FEATURE_WCDMA_3C_HSDPA
 WL1_DEBUG_L1_HSDPA_MAX_CARRIERS = (WL1_DEBUG_L1_HSDPA_CARRIER_2 + 1)
 #else
  WL1_DEBUG_L1_HSDPA_MAX_CARRIERS = (WL1_DEBUG_L1_HSDPA_CARRIER_1 + 1)
 #endif
} wl1_debug_hsdpa_carrier_enum_type;

/* This structure defines the DL HS DPSCH SCCH information */
typedef struct
{
  /* indicate if secondary scrambling code exists for HS SCCH channel */
  boolean sec_scr_code_valid;
  /* secondary scrambling code value.
     Valid only if sec_scr_code_valid is set to TRUE.
     If not valid then primary CPICH scrambling code is used */
  uint8 sec_scr_code;

  /* number of channelisation codes to SCCH */
  uint8 num_ch_code;
  /* SCCH channelisation codes */
  uint8 ch_code[WL1_DEBUG_L1_DL_HS_SCCH_CODES_MAX];
} wl1_debug_hs_scch_info_info_struct_type;

/* 16 Bytes */
typedef PACKED struct PACKED_POST
{
  uint8 hsdsch_tb_size_alignment_type;   /* 1B - Indicates if the HS-DSCH TB table to be chosen is octet aligned or not */
  uint8 tx_div_mode;   /* 1B - Transmit diversity mode for HS-PDSCH */
  boolean hs_64_qam_configured;                        /* 1B - 64QAM configured */
  uint8 hs_info_table_idx;                             /* 1B - HSDPA information table index */
  uint8 action;         /* 1B - HSDPA channel configuration action used by mDSP */
  uint32 h_rnti;                                       /* 1B - Store the common/dedicated H-RNTI */
  uint32 bcch_h_rnti;                                  /* 1B - Store the BCCH H-RNTI */
  uint16 dsch_to_cpich_offset;                         /* 1B - DSCH to CPICH offset. MPO */
  boolean activation_status;                           /* 1B - Carrier ACTIVATION status, TRUE: ACTIVATE, FALSE: DEACTIVATE. NA for Primary Carrier */
  wl1_debug_hs_scch_info_info_struct_type  scch_info;           /* 7B - L1 DL HS SCCH physical channel info */
} wl1_debug_hs_dl_channel_per_carrier_info_struct_type;

/* 53 Bytes */
typedef PACKED struct PACKED_POST
{
  uint8 hs_or_ehs;      /* 1B - indicate if this configuration is for MAC-hs or for MAC-ehs */
  uint8 cctrch_id;                     /* 1B - CCTrCh Id for this configuration */
  uint8 hs_serving_rl_tau_dpch;        /* 1B - HS serving radio link DPCH tau-DPCH */
  uint8 dpch_to_pdsch_offset;          /* 1B - DPCH to HS-PDSCH offset */
  uint8 config_cfn;                    /* 1B - configuration CFN */
  uint8 start_cfn;                     /* 1B - Start CFN for Enable or Reconfig */
  uint8 dpch_hwch_num;                 /* 1B - DPCH hardware channel number */
  uint32 hs_req_mask;                  /* 4B - Request mask indicating type of HS channel */
  uint8 efach_h_rnti_bmsk;             /* 1B - store the HRNTI bit mask info */
  uint8 hs_offset;                     /* 1B - offset of enable HS-SCCH relative to CPICH frame boundary (in bpg) */
  boolean hrnti_update;                /* 1B - only HRNTI is updated in HS DSCH */
  uint8 dpch_to_scch_offset;           /* 1B - DPCH to starting HS-SCCH offset */
  uint8 dpch_to_scch_offset_for_hs_cm; /* 1B - DPCH to starting HS-SCCH offset for HS+CM logging*/
  uint16 start_global_sub_fr_num;      /* 2B - Starting global sub frame number for first HS-SCCH sub frame */
  uint16 stop_global_sub_fr_num;       /* 2B - Last HS-PDSCH global sub frame number, Used only for Disable/reslam */
  uint8 cfnx5_sub_fr_offset;           /* 1B - The offset between CFNx5 and global subframe number */

  wl1_debug_hs_dl_channel_per_carrier_info_struct_type carr_info[WL1_DEBUG_L1_HSDPA_MAX_CARRIERS]; /* 32B - Per-carrier info (2) */
} wl1_debug_hs_dl_cfg_db_info_struct_type;

/* 11 Bytes */
typedef PACKED struct PACKED_POST
{
  boolean active;                              /* 1B - indicate if HS-DPCCH is active */
  uint8 config_cfn;                            /* 1B - configuration CFN */
  uint8 action; /* 1B - HSDPA channel configuration action used by mDSP */
  uint8 dpch_to_dpcch_offset;                  /* 1B - DPCH to starting HS-DPCCH offset */
  uint16 start_global_sub_fr_num;              /* 2B - Starting global sub frame number for first HS-SCCH sub frame */
  uint16 stop_global_sub_fr_num;               /* 2B - Last HS-PDSCH global sub frame number, Used only for Disable/reslam */
  uint8 cfnx5_sub_fr_offset;                   /* 1B - The offset between CFNx5 and global subframe number */
  uint8 active_cfn;                            /* 1B - ul cfn at which response was received from FW for the HS-DPCCH cfg cmd*/
  uint8 active_tick;                           /* 1B - ul tick at which SW received the respone from FW */
} wl1_debug_hs_ul_cfg_db_info_struct_type;

/* 24 Bytes */
typedef PACKED struct PACKED_POST
{
  uint8  num_non_hs_dob_handles; /* 1B */
  uint8  num_hs_dob_handles;     /* 1B */
  uint8  num_partial;            /* 1B */
  uint8  num_complete;           /* 1B */
  uint16 hdr_rd_start;           /* 2B */
  uint16 hdr_rd_step2;           /* 2B */
  uint16 hdr_rd_end;             /* 2B */
  uint16 l2_start;               /* 2B */
  uint16 l2_end;                 /* 2B */
  uint16 p2_start;               /* 2B */
  uint16 p2_end;                 /* 2B */
  uint16 cur_tbset_proc_start;   /* 2B */
  uint16 prev_tbset_proc_start;  /* 2B */
  uint16 dmov_trig_start;        /* 2B */
} wl1_debug_dec_hdr_rd_isr_info_struct_type;

/* 11 Bytes */
typedef PACKED struct PACKED_POST
{
  uint8 cfn;           /* 1B */
  uint32 cx1;          /* 4B */
  uint8 rd_idx;        /* 1B */
  uint8 wr_idx;        /* 1B */
  uint16 start_addr;   /* 1B */
  uint8 hs_trch_type;  /* 1B */
  uint16 timetick;     /* 2B */
} wl1_debug_dec_decode_done_isr_info_struct_type;

/* 6 Bytes */
typedef PACKED struct PACKED_POST
{
  uint16 nv_value;       /* 2B - Value read from NV */
  
  uint8 btfd_amr_call_rate; /* 1B - Current AMR rate*/
  uint8 btfd_dcch_type;         /* 1B - Current BTFD DCCH type */
  
  uint8 sync_status          :1;   /* 9 bits - SW FET client status */
  uint8 hs_status            :1;
  uint8 rxd_status           :1;
  uint8 cm_status            :1;
  uint8 spread_factor_ok     :1;
  uint8 po3_ok               :1;
  uint8 gsm_tx_status        :1;
  uint8 voice_call_status    :1;
  uint8 tuned_to_multi_carr  :1;
  
  uint8 dcch_erasure_det_v2_enable :1; /* 5 bits - FET feature status */
  uint8 dcch_fr_eng_comp_enable    :1;
  uint8 fet_enable                 :1;
  uint8 msrch_enable               :1;
  uint8 dape_enable                :1;
  uint8 dcch_aware_ilpc_mode; 
} wl1_debug_fet_db_info_struct_type;

/* 4 Bytes */
typedef PACKED struct PACKED_POST
{
  uint8 filt_init                    :1; /* 1B - DSR status flags */
  uint8 power_savings_enable_state   :1;
  uint8 hs_active                    :1;
  uint8 cm_active                    :1;
  uint8 rxd_active                   :1;
  uint8 jammer_present               :1;
  uint8 start_frame_timer            :1;
  uint8 state_machine_active         :1;
  uint8 wl1_dsr_last_comit_action; /* 1B - last DSR command */
  uint16 dsr_cmd_sent_cfn;                        /* 2B - last DSR command CFN */
} wl1_debug_dsr_db_info_struct_type;

/* 8 Bytes */
typedef PACKED struct PACKED_POST
{
  struct
  {
    uint8 demodEn_0   : 1; /* 1 bit: HS SCCH/PDSCH demod enabled */
    uint8 ackNackTx_0 : 1; /* 1 bit: Ack/Nack Tx enable */
    uint8 cqiTx_0     : 1; /* 1 bit: Cqi Tx enable */
    uint8 cqiValid_0  : 1; /* 1 bit: CQI valid. Transmit DTX if not valid and cqiTx is enabled */
    
    uint8 demodEn_1   : 1; /* 1 bit: HS SCCH/PDSCH demod enabled */
    uint8 ackNackTx_1 : 1; /* 1 bit: Ack/Nack Tx enable */
    uint8 cqiTx_1     : 1; /* 1 bit: Cqi Tx enable */
    uint8 cqiValid_1  : 1; /* 1 bit: CQI valid. Transmit DTX if not valid and cqiTx is enabled */
  } ctrl[WL1_DEBUG_WFW_DEMOD_HS_CM_CTRL_TABLE_SIZE/2]; /* 16 elements packed into 8B */
} wl1_debug_hs_cm_ctrl_table_info_struct_type;

/* 206 Bytes */
typedef PACKED struct PACKED_POST
{
  /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;
  q_type dec_tsn_info_q;       /* 16B - TSNs are waiting for L2 processing */
  q_type dec_tsn_info_free_q;  /* 16B - indicates how many TSN queue elements are available */
  wl1_debug_fet_db_info_struct_type fet_db;           /* 6B - FET config DB */
  wl1_debug_dsr_db_info_struct_type dsr_db;           /* 4B - DSR config DB */

  /* Decode-done ISR info */
  wl1_debug_dec_hdr_rd_isr_info_struct_type dec_hdr_rd_isr[2];     /* 48B - timestamps and counters for last 2 header read events */
  wl1_debug_dec_decode_done_isr_info_struct_type   dec_done_isr[2];       /* 22B - timestamps and counters for last 2 decode done ISRs */
  uint8 mcalwcdma_dec_skip_decode_done_isr_cnt;         /* 1B - number of decode done ISRs skipped due to pending A2 transaction */
  uint8 mcalwcdma_dec_dmov_transfer_pending        :1;  /* 1 bit - indicates there is a pending A2 DMA transaction */
  uint8 mcalwcdma_dec_gather_and_decipher_pending  :1;  /* 1 bit - indicates there is a pending A2 P-to-C transaction */

  /* HS channel config */
  uint8 hs_channel_active             :1;               /* 1 bit - This variable indicate that HSDPA channel is active or not. */
  uint8 hs_channel_suspended          :1;               /* 1 bit - This variable indicates if HSDPA channel is internally Suspended or not */
  uint8 hs_ext_cmd_pending            :1;               /* 1 bit - This variable indicates if HS external command is pending or not */
  uint8 hs_int_cmd_pending            :1;               /* 1 bit - This variable indicates if HS internal command is pending or not */
  uint8 hs_split_recfg_as_stop_start  :1;               /* 1 bit - flag to indicate if Reconfig is split into stop and start. */
  uint8 hs_dl_cfg_pending_in_fw       :1;               /* 1 bit - DL config command pending in FW */
  uint8 hs_ul_cfg_pending_in_fw       :1;               /* 1 bit - UL config command pending in FW */
  wl1_debug_hs_dl_cfg_db_info_struct_type curr_hs_dl_cfg_db_info;  /* 53B - current DL config DB */
  wl1_debug_hs_ul_cfg_db_info_struct_type curr_hs_ul_cfg_db_info;  /* 11B - current UL config DB */
  
  /* HS logging info */
  uint8 hs_ae_log_rd_ptr, hs_ae_log_wr_ptr;                 /* 2B - Read/write pointers for A&E log packet */
  uint8 hs_decode_status_rd_ptr, hs_decode_status_wr_ptr;   /* 2B - Read/write pointers for HS decode status log packet */
 
  /* AGC command pending */
  uint8 agc_pending_a0c0 : 1;                               /* 1 bit - Pending RxAGC command */
  uint8 agc_pending_a1c0 : 1;                               /* 1 bit - Pending RxAGC command */
  uint8 agc_pending_a0c1 : 1;                               /* 1 bit - Pending RxAGC command */
  uint8 agc_pending_a1c1 : 1;                               /* 1 bit - Pending RxAGC command */
  
  /* CCTrCh table */
  uint8 dl_cctrch_id_table_index[WL1_DEBUG_L1_CCTRCH_ID_COUNT];       /* 6B - table index for each CCTrCh ID (6) */
  uint8 rrc_cctrch_id_in_use;                               /* 1B - bitmask of CCTrCh IDs in use from RRC perspective */
  
  /* HS+CM control tables */
  wl1_debug_hs_cm_ctrl_table_info_struct_type hs_cm_ctrl_table[WL1_DEBUG_WFW_DEMOD_HS_CM_CTRL_TABLE_NUM_BUF]; /* 16B - HS+CM control tables (2) */
  
} wl1_datapath_mini_debug_info_struct_type;

#ifdef FEATURE_DUAL_SIM
/*                            MULTISIM                               */
/*                                                                         */
#define WL1_DEBUG_WRM_MAX_CLIENTS 3

/*Max number of links link_info_mgr can support*/
#define WL1_DEBUG_CXM_MAX_NUM_RADIO_LINKS 10

/*   WRM Resources */
typedef enum
{
  WL1_DEBUG_WCDMA_TRM,
  WL1_DEBUG_WCDMA_TRM_SECONDARY,
  WL1_DEBUG_WCDMA_TRM_CA,
  WL1_DEBUG_WCDMA_TRM_SECONDARY_CA,
  WL1_DEBUG_WCDMA_TRM_NUM_RESOURCES
} wl1_debug_wrm_resource_enum_type;

typedef PACKED struct PACKED_POST 
{

  boolean valid;

  uint8 resource;
  /**ID of the client requesting resource**/
  uint8 client_id;

  /**Activity for which resource is requested**/
  uint8 activity;

  /**Bands for which resource is requested**/
  trm_frequency_info band_info;

  boolean wl1_device_swap_pending;

} wl1_debug_msim_wrm_cmd_debug_info_struct_type; // = 7

typedef PACKED struct PACKED_POST 
{
  uint8 cli_state;

  uint8  req_activity;

} wl1_debug_msim_wrm_client_info_struct_type; // = 2

typedef PACKED struct PACKED_POST 
{

  /**Current WRM state**/
  uint8 wrm_state; // = 1

  /**Bitmask of active clients**/
  uint8 wrm_active_client_bmsk; // =1

  /** Info about each client's state and request parameters**/
  wl1_debug_msim_wrm_client_info_struct_type wrm_client_info[WL1_DEBUG_WRM_MAX_CLIENTS];//=6

  /**RFM Device ID allocated for this resource**/
  rfm_device_enum_type device_id; // = 1

  /**Details of a WRM request and notify kept pending by WRM state machine.**/
  wl1_debug_msim_wrm_cmd_debug_info_struct_type wrm_pending_cmd; //=7

} wl1_debug_msim_wrm_resource_info_struct_type ;//=16

typedef struct 
{
  /**Number of active measurements**/
  uint8 num_active_measurements;
} wl1_debug_wrm_meas_info_struct_type;

typedef PACKED struct PACKED_POST 
{
  wl1_debug_msim_wrm_resource_info_struct_type wrm_resource_info[WL1_DEBUG_WCDMA_TRM_NUM_RESOURCES];
  wl1_debug_wrm_meas_info_struct_type meas_db;
} wl1_debug_msim_wrm_db_debug_info_struct_type; // = 65

typedef PACKED struct PACKED_POST
{
  /*If TRUE: Indication from link entity to link info mgr
  **to generate its frequency parameters from uarfcn
  **Else If FALSE: Indication from link entity to link info mgr
  **to use link_freq_info(from WL1_CXM_FREQ_MSG) as its frequency parameters*/
  boolean generate_link_freq_info;
  /*Freq channel type(DL || UL)*/
  uint8 uarfcn_type;
  /*Action mask for the link request*/
  uint8 action_mask;
  /*Freq channel of link entity*/
  uint16 uarfcn;
  /*Link entity's Frequency ID*/
  uint32 freqid;
  /*Link Entity Type*/
  uint8 link_type;
  uint8 rf_device;
}wl1_debug_msim_cxm_link_info_struct_type; //==11

typedef PACKED struct PACKED_POST
{
  /*Number of link entiy's in link_list*/
  uint8 num_links;

  /*Array of link entities and their link info*/
  wl1_debug_msim_cxm_link_info_struct_type link_list[WL1_DEBUG_CXM_MAX_NUM_RADIO_LINKS]; //=110

} wl1_debug_msim_cxm_link_list_info_struct_type;

typedef struct 
{
  /*Whether WL1 is initializing RF and mdsp*/
  boolean wl1_in_ds_prep_mode;

  /*True: UE is in dual sim mode; False: UE is in single sim mode*/
  boolean wl1_in_ds_mode;

  /* standby mode: TRUE for Dual standby, FALSE for single standby */
  uint8 wl1_standby_mode;

#ifdef FEATURE_TRIPLE_SIM
  /* Tells whether it is */
  uint8 wl1_multi_sim_mode;
#endif

  /* device mode: TRUE for dual active , FALSE for single active*/
  uint8 wl1_device_mode;

  /*WL1 procedures that are holding TRM lock*/
  uint32 wl1_ds_proc_mask;

  /*Last WL1 procedure that requested for TRM lock*/
  uint8 ds_curr_proc;

  /*Last WL1 procedure that requested for TRM lock*/
  uint8 ds_pend_proc;

  /*Last cphy_setup_cmd procedure that requested for TRM lock*/
  uint8 last_cphy_setup_proc;

  /*Indicate whether cmd is pending waiting for the sleep proc to be completed*/
  boolean trm_lock_sig_pending_on_sleep;

  /* Used to store the TRM lock signal (success/failure) in
    case TRM signal is received in the middle of paging wakeup;
    only applicable if trm_lock_sig_pending_on_sleep = TRUE */
  rex_sigs_type trm_lock_sig;

  /* Flag to indicate Ext cmd processing with no TRM lock */
  boolean process_ext_cmd_without_lock;
}wl1_debug_msim_ds_db_info_struct_type;

/*Structure to hold WL1 dual sim statistics*/
typedef struct 
{
  uint32 lock_req_and_not_cnt;

  uint32 lock_req_and_not_succ_cnt;

  uint32 lock_req_and_not_fail_cnt;

  uint32 lock_req_cnt;

  uint32 lock_req_succ_cnt;

  uint32 lock_req_fail_cnt;

  uint32 lock_res_cnt;
} wl1_debug_msim_ds_overall_stat_info_struct_type;

typedef struct
{
  #if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
  uint8  wl1_qta_state;

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
  uint8 wl1_last_granted_band;
  trm_frequency_type_t wl1_last_granted_trm_freq_info;

  /*TRM lock acquisition success/failure statistics when in Dual SIM mode*/
  wl1_debug_msim_ds_overall_stat_info_struct_type wl1_ds_overall_stat;

  #ifdef FEATURE_WCDMA_TCXOMGR30
  uint32 wl1_already_has_tcxo;
  #endif
} wl1_debug_msim_extern_db_info_struct_type;

typedef PACKED struct PACKED_POST 
{

    /* this header will have a tag and length */
  qsh_dump_tag_hdr_s hdr;
  /*Information about current WL1 Dual SIM procedure*/
  wl1_debug_msim_ds_db_info_struct_type wl1_ds_db;//=20

  wl1_debug_msim_extern_db_info_struct_type wl1_multisim_extern_db;//=64

    /*Global to indicate if WFW is in Single Sim or DSDX mode*/
  boolean wl1_multisim_cxm_is_fw_in_dsdx_mode;

  /*Global to Freeze/Un-Freeze CXM operations for QTA*/
  boolean wl1_multisim_cxm_freeze_cxm_updates_for_qta;

  wl1_debug_msim_wrm_db_debug_info_struct_type wrm_db;//=65

  /*Database to hold Frequency/Frequency Id info of all the
  **active WL1 Links which have been registered with MCS*/
  wl1_debug_msim_cxm_link_list_info_struct_type wl1_cxm_freq_db; //=111 /*Drop this is over budget*/

} wl1_dsds_mini_debug_struct_type; // = 263

#endif /* FEATURE_DUAL_SIM */


typedef enum
{
  //WL1_QSH_DUMP_TAG_WCDMA_L1
  WL1_QSH_DUMP_TAG_WCDMA_L1_COMMON,
  WL1_QSH_DUMP_TAG_WCDMA_L1_DEMOD,
  WL1_QSH_DUMP_TAG_WCDMA_L1_SEARCHER,
  WL1_QSH_DUMP_TAG_WCDMA_L1_DRX_SLEEP,
  WL1_QSH_DUMP_TAG_WCDMA_L1_CELL_RESEL,
  WL1_QSH_DUMP_TAG_WCDMA_L1_UPLINK,
  WL1_QSH_DUMP_TAG_WCDMA_L1_RXD,
  WL1_QSH_DUMP_TAG_WCDMA_L1_CME,
  WL1_QSH_DUMP_TAG_WCDMA_L1_IRAT,
  WL1_QSH_DUMP_TAG_WCDMA_L1_DATAPATH,
#ifdef FEATURE_DUAL_SIM
  WL1_QSH_DUMP_TAG_WCDMA_L1_MULTISIM,
#endif /* FEATURE_DUAL_SIM */
  WL1_QSH_DUMP_TAG_WCDMA_WFW,
  WL1_QSH_DUMP_TAG_MAX
}wl1_qsh_dump_tag_e;

typedef struct
{
  wl1_common_mini_debug_info_struct_type common;
  wl1_dl_mini_struct_type demod;
  wl1_srcher_mini_debug_info_struct_type searcher;
  wl1_drx_sleep_mini_debug_info_struct_type drx_sleep;
  wl1_cell_resel_mini_debug_info_struct_type cell_resel;
  wl1_ul_mini_debug_info_struct_type uplink;
  wl1_rxd_mini_debug_info_struct_type rxd;
  wl1_cme_mini_debug_info_struct_type cme;
  wl1_irat_mini_debug_info_struct_type irat;
  wl1_datapath_mini_debug_info_struct_type datapath;
#ifdef FEATURE_DUAL_SIM
  wl1_dsds_mini_debug_struct_type multisim;
#endif
  wfw_qsh_dump_s wfw;
} wl1_qsh_dump_tag_wcdma_l1_s;  

#endif /* FEATURE_QSH_DUMP */


#ifdef FEATURE_QSH_EVENT_METRIC
/*-------------------------------------------------------------------------*/
/*                  Debug event/metrics info Struct type declaration                      */
/*-------------------------------------------------------------------------*/

typedef enum
{
  WL1_QSH_METRIC_SERV_CELL_RF,
  WL1_QSH_METRIC_DL_DPCH_SIR_EST,
  WL1_QSH_METRIC_HSDPA_KPI,
  WL1_QSH_METRIC_UL_RACH,
  WL1_QSH_METRIC_UL_RACH_ATTEMPT_COUNT,
  WL1_QSH_METRIC_UL_TX_PWR,
  WL1_QSH_METRIC_UL_EUL_STAT,
  WL1_QSH_METRIC_SERV_CELL_RF_PRIM_DIV,
  WL1_QSH_METRIC_VOICE_BLER,
  WL1_QSH_METRIC_MAX
}wl1_qsh_metric_e;


/*METRIC_ID = WL1_QSH_METRIC_SERV_CELL_RF
**sampling_rate = 100ms
*/
typedef struct
{
/*This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;

/*No need to average since the values are filtered. Units-DB*/
  int32 wl1_serv_cell_rscp;

/*Rx Agc   - Averaged over last 5 cycles. Uints-DBM*/
  int32 wl1_serv_cell_rssi;

/*No need to average since the values are filtered. Units-DB*/
  int32 wl1_serv_cell_ecio;
}wl1_qsh_metric_serv_cell_rf_s;


/*METRIC_ID =WL1_QSH_METRIC_DL_DPCH_SIR_EST
**sampling_rate = 100ms
*/
typedef struct
{
/* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;

/*Avg DPCCH energy maintained by UE for 160ms
  (dl_dpch_sir_est)* log(2)/10 to convert into dB scale */
  int32 dl_dpch_sir_est;
}wl1_qsh_metric_dl_dpch_sir_est_s;


/*METRIC_ID = WL1_QSH_METRIC_HSDPA_KPI
**sampling_rate = 100ms
*/
typedef struct
{
/*This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;

/*Average CQI sent on HS-DPCCH*/
  uint8 hsdpa_cqi;

/*Average TB size received whether successfully decoded or not*/
  uint16 hsdpa_TBsize;

/*Number of subframes where decode was attempted*/
  uint8 num_attempted;

/*Number of subframes where decode failed*/
  uint8 num_in_err;

}wl1_qsh_metric_hsdpa_kpi_s;


/* Struct to store the PRACH statistics for event 
** logging information for a given sample period
**METRIC_ID = WL1_QSH_METRIC_UL_RACH
**sampling rate = 1s
*/
typedef struct
{
/*This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;

/* Number of PRACH Preambles attempted per Sampling time */
  uint8   num_pmbl_attempts_in_sample;

/*Number of PRACH Preambles with NO ACK per Sampling time */
  uint8   num_pmbl_no_ack_in_sample;

/* Number of PRACH Preambles with NAK per Sampling time */
  uint8   num_pmbl_nak_in_sample;

/* Average TX power used for PRACH in the sampling period in dBm */
  int8    avg_prach_tx_pwr_per_sample;
}wl1_qsh_metric_ul_rach_s;

/* Struct to store the RACH attempts statistics for event 
** logging information for a given sample period
** METRIC_ID = WL1_QSH_METRIC_UL_RACH_ATTEMPT_COUNT
** sampling rate = 1s
*/
typedef struct
{
  /*This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;

  /* Number of L2 RACH Procedures attempted in the sampling time */
  uint8   num_rach_procedures_in_sample;
}wl1_qsh_metric_ul_rach_attempts_s;


/* Struct to store the Average TX Power, UL DPCCH Pwr, UPH and 
** EUL SG and NACK Information in a given sample rate of 100ms
*/
typedef struct {
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;

/* Average TX AGC for non-RACH cases per Sampling time in dBm */
  int8    avg_tx_agc_per_sample;

/* Average UL DPCCH TX Power per Sampling time in dBm */
  int8    avg_ul_dpcch_pwr_per_sample;
}wl1_qsh_metric_ul_tx_pwr_s;   


/*METRIC_ID = WL1_QSH_METRIC_UL_EUL_STAT
**sampling_rate = 100ms
*/
typedef struct 
{
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;

/* Average Uplink Pwr Headroom in the sampling period in dBm */
  int8    avg_uph_per_sample;

/* Number of E-HICH NAKs in the Sampling Period */
  uint8   num_eul_nak_in_sample;

/* Average Serving Grant Index value per sampling time */
  uint8   average_sg_index_per_sample;
}wl1_qsh_metric_ul_eul_stat_s;   

/*METRIC_ID = WL1_QSH_METRIC_SERV_CELL_RF_PRIM_DIV
**sampling_rate = 100ms
*/
typedef struct
{
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;
  
  /*Units-DB*/
  int32 wl1_serv_cell_rscp_prim;

  /*Units-DBM*/
  int32 wl1_serv_cell_rssi_prim;

  /*Units-DB*/
  int32 wl1_serv_cell_ecio_prim;

  int32 wl1_serv_cell_rscp_div;
  int32 wl1_serv_cell_rssi_div;
  int32 wl1_serv_cell_ecio_div;
}wl1_qsh_metric_serv_cell_rf_prim_div_s;


/*METRIC_ID = WL1_QSH_METRIC_VOICE_BLER
**sampling_rate = 100ms
*/
typedef struct
{
  /* This header has the timestamp (uint16) */
  qsh_metric_hdr_s hdr;
  /*Number of blocks received*/
  uint32 num_received;
/*Number of blocks in error*/
  uint32 num_in_err;
}wl1_qsh_metric_voice_bler_s;
#endif /* FEATURE_QSH_EVENT_METRIC */

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Functions                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION wl1_qsh_cb

DESCRIPTION
  This function is called by QSH module for dump collection and metric logging

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_qsh_cb(qsh_client_cb_params_s* );
#endif /* WL1_QSH_EXT_H */
