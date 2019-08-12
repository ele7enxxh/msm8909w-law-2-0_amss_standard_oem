/*===========================================================================
FILE: tdssrchlte.h


GENERAL DESCRIPTION
  This file contains code responsible for LTE Cell Set Maintainence and Searches.


Copyright (c) 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdssrchlte.h#1 $ DateTime: $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/15   zli     Implement FR 25117: T2L timing maintenance and power optimization
11/10/14   zli     Handle race condition between ERUCCH and T2L
09/03/14   zli     TL1 to provide RF device info for T2L on Bolt 2.0
08/19/14   zli     Do not reset ET tracker if T2L is not triggered between TDS calls
07/14/14   zl    Use FEATURE_TDSCDMA_IRAT_FED_BASED for T2G/T2L on DPM 2.0
07/08/14    zl     Extend T2L to support 8 LTE TDD frequencies
06/12/14    zli    Disable T2L measurement based on NV
05/15/14   zl      On Bolt, temporally separate T2G measurement and T2L init to avoid MIPS issue
05/06/14    zl     Schedule T2L measurement after PCH decoding if necessary
04/21/14   zl      adjust T2L idle mode timeline for Bolt
03/13/14   zl      adjust T2L Tresel threshold based on traffic pattern
03/07/14   zl      send T2L blacklisted cell list to ML1 upon resume
11/22/13   sk      RxLM optimization phase 2 - CR56584  
10/08/13    zl     Streamline T2L idle mode procedures for Bolt. 
10/14/13   zl      Change T2L idle mode measurement duration threshold to 40ms
07/11/12    zl      Add RF cleanup support after LTE measurement
07/10/12   zl      Optimize T2L measurement scheduling
06/20/12   zl      added T2L idle mode cell reselection 
07/27/10   stk     Added cell_barred boolean to LTE cell struct type, to determine whether an LTE cell is barred or not.
                   This boolean will help in resetting tresel for that cell, when its barred.
07/19/10   mm      Added prototype for wsrchlte_meas_cleanup()  
07/14/10   mm      Fix for compiler warnings  
07/04/10   uk      Changed logic of sending LTE INIT/DEINT
07/02/10   uk      Removed redundant decleration of SRCH_DL_CFG_DECFG_SET_MAX 
07/01/10   uk      Added support for FEATURE_HACK_LTE_CELL_MEAS_AS_DETECT
06/16/10   mm/yh   Added timing_adjustment_cir in wsrchlte_cell_struct_type due to 
                   interface change in LTE.
06/02/10   stk     Initial Version. Added support for W2LTE measurements and reselection 
                   under FEATURE_TDSCDMA_ABSOLUTE_PRIORITY & FEATURE_TDSCDMA_TO_LTE

===========================================================================*/

#ifndef TDSSRCH_LTE_H
#define TDSSRCH_LTE_H

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "tdsirat.h"
#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
#include "tdsl1rrcif.h"
#include "tdsl1def.h"
#include "tdssrchltedrv.h"
//#include "tdssrchcr.h" /* for tdssrchset_evt_ts_struct_type */
#include "tdsl1m_rcinit.h"
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
//#include "tdssrchgsm.h"
#include "tdsl1cmmeas.h"
#ifdef FEATURE_TDSCDMA_DSDS_QTA
#include "tdsl1trm.h"
#endif

#endif /* defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT) */

/*-------------------------------------------------------------------------*/
/*                        MACROS                                           */
/*-------------------------------------------------------------------------*/
#define TDSSRCHLTE_NSET_MAX             32
#define TDSSRCHLTE_INVALID_EARFCN_INDEX 0xFF

/* MAX number of LTE frequencies to be searched are 8 */
#define TDSSRCHLTE_MAX_NUM_FREQ          TDSSRCHLTEDRV_MAX_FREQS

/* Scale idle mode scheduler timer using max number of LTE frequencies of 4, 
even though TL1 could have up to 8 LTE TDD frequencies. */
#define TDSSRCHLTE_MAX_NUM_FREQ_FOR_TIMER   4

#define TDSSRCHLTE_MAX_CELL_AGE          5     //For now, set the max age to 5. 

/*per TDSLTE IRAT HLD*/
#define MIN_TIME_FOR_TDD_LTE_CELL_DETECT_SRCH     52000 //52 ms in units of micro seconds 
//40ms in units of micro seconds: about 26ms for T2L meas, and 10ms for DRX wakeup preparation.   
#define MIN_TIME_FOR_TDD_LTE_CELL_MEAS_SRCH       40000  


/*LTE trigger gap buffer time from current time*/
#define TDSSRCHLTE_MEAS_REQ_BUFFER_SUBFN                  1      //one subframe
#define TDSSRCHLTE_MEAS_REQ_BUFFER_CHIPX8_OFFSET          6912   //one normal timeslot

#define TDSSRCHLTE_TFW_LTE_GAP_START_BUFF                 1

#define TDSSRCHLTE_INVALID_SUB_SFN_NUM                    0x7FFF

#define TDSSRCHLTE_INTLOCK()  REX_ISR_LOCK(&tdssrchlte_crit_sect)
#define TDSSRCHLTE_INTFREE()  REX_ISR_UNLOCK(&tdssrchlte_crit_sect)

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)

/*To check whether earfcn_idx is invalid or out of range*/
#define TDSSRCHLTE_INVALID_EARFCN_IDX(earfcn_idx) \
	(((earfcn_idx)== TDSSRCHLTE_INVALID_EARFCN_INDEX) || ((earfcn_idx)>= tdssrchlte_freq_list.num_eutra_frequencies))


#define TDSSRCHLTE_EUTRA_INVALID_FILTER_INDEX (0xFF)
/* Max number of LTE TDD/FDD frequencies maintained in the freq list, 
if FEATURE_WTOL_TDD_CM_SUPPORT is supported */

/* Invalid EARFCN */
#define TDSSRCHLTE_INVALID_EARFCN 0xFFFFFFFF
//T2L TDD will support up to 8 LTE TDD frequencies now
#define TDSSRCHLTE_CM_MAX_NUM_TDD_FREQ    8
#define TDSSRCHLTE_CM_MAX_NUM_FDD_FREQ    0

#define TDSSRCHLTE_CM_MAX_NUM_FREQ    (TDSSRCHLTE_CM_MAX_NUM_FDD_FREQ  + TDSSRCHLTE_CM_MAX_NUM_TDD_FREQ) 

/* Bitmask for cell detection window of last 5 gaps to be masked with the cell 
   detection history. */
#define TDSSRCHLTE_TDD_CM_DETECT_WINDOW_BMSK 0x1F
/* Threshold for minimum number of W2L TDD detections in the last 5 gaps for a cell 
   to be considered preferred for virtual cell selection */
#define TDSSRCHLTE_TDD_VIRTUAL_CELL_DETECT_THRESH 2

/* the maximum number of cells we can measure per LTE meas request  */
//#define TDSSRCHLTEDRV_CM_MAX_MEAS_CELLS 15

/* Max newly detected cells in CM search */
//#define TDSSRCHLTEDRV_MAX_NEW_DETECTED_CELLS   12
#define TDSSRCHLTE_MAX_NUM_FILTERS 3

/*T2L CM measuremet request window, 2 subframes (10ms) for now + 1 offset 
from the current subframe received from the gap manager*/
//#define TDSSRCHLTE_CM_MEAS_REQ_WINDOW   3 
//On bolt, we use 2 sub-frames
#ifdef FEATURE_TDSCDMA_IRAT_FED_BASED
#define TDSSRCHLTE_CM_MEAS_REQ_WINDOW   2
//On NickeL/DIME, we use 1 sub-frame
#else 
#define TDSSRCHLTE_CM_MEAS_REQ_WINDOW   1

#endif

/*LTE measurement small gap, 16 continous time slots (2 subframes) */
#define TDSSRCHLTE_CM_MEAS_SMALL_GAP_LEN   2 

/*LTE measurement medium gap, 32 continous time slots (4 subframes) */
#define TDSSRCHLTE_CM_MEAS_MEDIUM_GAP_LEN  4 

/*LTE measurement fat gap, 48 continous time slots (6 subframes) */
#define TDSSRCHLTE_CM_MEAS_FAT_GAP_LEN     6

/*The LTE CM gap start difference between TFW and LFW*/
/*LTE gap starts 1 timeslot after TFW gap start*/
#define TDSSRCHLTE_CM_GAP_START_BUFFER     1

/*The LTE CM  gap end difference btween TFW and LFW*/
#define TDSSRCHLTE_CM_GAP_END_BUFFER     0


#define TDS_HALF_SLOT_IN_Cx8 3456

/*-------Constants for LTE CM Scheduling--------------*/
//The number of consecutive cycles without detecting any cell on one freq
#define TDSSRCHLTE_CM_SCHED_SRCH_PERIOD_THRES                4
// the minimal search periodicity is 2*TbasicIdentifyEutraTDD 480ms
#define TDSSRCHLTE_CM_SCHED_MIN_SRCH_EUTRA                   2
// TbasicIdentifyEutraTDD 480ms = 96*5ms
#define TDSSRCHLTE_CM_SCHED_T_BASIC_IDENTIFY                 96

//Tdelta: 1ms: need to be subtracted from M_len
#define TDSSRCHLTE_CM_SCHED_T_DELTA                          1

//speeding factor;
#define TDSSRCHLTE_CM_SCHED_SPEEDING_FACTOR                  2

#define TDSSRCHLTE_DELTA_TIME_2MS_CHIPX8                     (10240*2) 

/*-------------------------------------------------------------------------*/
/*                        TYPE DEFINITIONS                                 */
/*-------------------------------------------------------------------------*/
/* Struct for Filtered RSRP/RSRQ values */
typedef struct
{
  int16 filt_rsrp;
  int16 filt_rsrq;
//  uint32 filt_cer_snr[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
} tdssrchlte_filt_info_struct_type;

/* cb type used with tune_done confirmations */
typedef void tdssrchlte_tune_done_cb_type(void);
#endif

typedef struct
{
  int8 priority;
  int16 q_rxlevmin_eutra; 
  uint8 threshx_high; 
  uint8 threshx_low;  
  /*add RSRQ support*/
  int16 q_qualmin_eutra;
  boolean high2_incl;
  uint8 threshx_high2; 
  boolean low2_incl;
  uint8 threshx_low2;  
} tdssrchlte_priority_info_type;
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
typedef enum
{
  TDSSRCHLTE_CELL_NOT_DETECTED,
  TDSSRCHLTE_CELL_DETECTED,
  TDSSRCHLTE_CELL_NA_PRUNING
} tdssrchlte_detected_cell_enum_type;
#endif
/* LTE cell structure */
typedef struct 
{ 
  /* The physical cell id of the detected cell */
  uint16 cell_id;
  /* The earfcn for searching LTE cells on */
  uint32 earfcn;  
  /* The CP type of the detected cell. */
  tdssrchltedrv_cp_enum_type cp_type;
  /* The frame boundary of the LTE cell based upon the LTE RTC */ 
  uint64 timing_offset[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* The timing adjustment in the cir domain */
  uint16 timing_adjustment_cir[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Iinitial frequency offset estimation in Hz*/
  int16 freq_offset_est;
  /* RSRP comb.value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrp;
  /* RSRQ value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrq;
  /* Cell selection values evaluated by SRCH and to be reported to RRC */  
  int16 s_rxlev;
  /* Cell selection values evaluated by SRCH and to be reported to RRC */  
  int16 s_qual;
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
   /* sss correlation value, used for ranking cell detection results in CM */
  uint32 sss_corr;
  /* Max RSSI value combined across Tx-Rx pairs. In linear scale. */
  int16 rssi; 
  /* Filtered RSRP/RSRQ values */
  tdssrchlte_filt_info_struct_type filters[TDSSRCHLTE_MAX_NUM_FILTERS];
  /* Instantaneous RSRP per antenna. Range -144dBm..-44dBm with resolution of 1dBm */
  int16 rsrp_ant[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Instantaneous RSRQ per antenna. Range -19.5dB ... -3dB */
  int16 rsrq_ant[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Instantaneous RSSI per antenna. Range -120 ... 0 */
  int16 rssi_ant[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* The cell detection history, represented as a bitmask. This field is used
     for virtual cell selection for TDD CM. The least significant bit holds the most
     recent value. If the bit is a 1, the cell was detected in that gap. If
     it was 0, then it wasn't. If, at any point in time, at least 2 of the 5
     least-significant bits are high, it will be given preference in virtual cell selection. */
  uint8 detect_hist;
  /* Flag for virtual cell preference. A cell is considered to be preferred for virtual cell
     selection if it has been detected at least twice in the last 5 consecutive detections */
  boolean virtual_cell_preferred;
  /* Instant. CER SNR values used for decision in reporting cell */
  //LTE FW does not support CER SNR for now. 
  //uint32 cer_snr_rx[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
#endif
  /* Flag indicates whether a cell is barred or not */
  boolean cell_barred;
  /* Cell Reselection Timestamp for this cell (neighbor) */
  tdssrchset_evt_ts_struct_type  evt_cr_ts;
  uint8   age;     //The age of this cell starting from last measurement
  boolean just_measured; 
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* Flag indicates if the cell was detected in the current gap */
  tdssrchlte_detected_cell_enum_type curr_detected_cell;
#endif
} tdssrchlte_cell_struct_type;

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)

/* Struct for T2L CM timer for search/meas
   sm_counter max 3 after which meas should be scheduled
   srch_timer expires, start new search/meas cycle */
typedef struct
{
  /* Search command idx */
  uint8 cm_small_gap_srch_cmd_idx;
  uint8 cm_medium_gap_srch_cmd_idx;
  uint8 cm_fat_gap_srch_cmd_idx;
} tdssrchlte_freq_timer_info_struct_type;

#endif

/* Struct for a LTE Frequency */
typedef struct
{
  /* LTE EARFCN */
  uint32 EARFCN;  
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* Flag for if its an LTE TDD freq */
  boolean is_tdd_freq;
  /* Flag for if BW info present */
  boolean dl_bandwith_present;
#endif
  /* Bandwidth */
  tdssrchltedrv_earfcn_dl_bandwidth dl_bandwidth;
  /* Number of blacklisted cells */
  uint8 num_blacklisted_cells;
  /* List of blacklisted cells, identified by cell_id */
  uint16 blacklisted_cells[TDSSRCHLTEDRV_MAX_CELLS_PER_FREQ];

  /* Priority information present */
  boolean prior_info_present;
  /* Priority information */
  tdssrchlte_priority_info_type prior_info;

  /* Num of cells on this frequency */
  uint8 num_cells;
  /* LTE Cell Struct */
  tdssrchlte_cell_struct_type  *cell_ptr[TDSSRCHLTEDRV_MAX_CELLS_PER_FREQ];

  #if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* CM timer info per freq */
  tdssrchlte_freq_timer_info_struct_type timer_info;
  uint16 sche_subsfn; //the subsfn when measuremnt is scheduled
  /* AGC parameters */
  tdssrchltedrv_agc_param_info_struct_type agc_info;
  
  /* Physical Cell list per EARFCN for Compressed Mode. MAX size = 16 */
  tdssrchlte_cell_struct_type meas_cell_list[TDSSRCHLTEDRV_MAX_CELLS_PER_FREQ]; 
  #endif

} tdssrchlte_frequency_info_type;

/* Struct with the total number of LTE frequencies */ 
typedef struct
{
  /* Number of active LTE frequencies */
  uint8 num_eutra_frequencies;
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* Number of FDD freqs */
  uint8 num_fdd_freq;
  /* Number of TDD freqs */
  uint8 num_tdd_freq;
#endif
  /* Frequency information */
  /* Only TDSSRCHLTE_MAX_NUM_FREQ[4] freqs supported in T2L,  TDSL1_MAX_EUTRA_FREQ[8] freqs are just used for intermediate sorting*/
  tdssrchlte_frequency_info_type lte_frequency[TDSL1_MAX_EUTRA_FREQ];
} tdssrchlte_frequency_list_type;

/* Commands during LTE NSET search */
/* Types of LTE Searches */ 
typedef enum
{
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* Connected Mode Cell AGC */
  TDSSRCHLTE_CELL_AGC,
  /* Connected mode Small gap Cell AGC and Meas Request 
  for previous EARFCN in case prev freq is LTE TDD */
  TDSSRCHLTE_CELL_AGC_MEAS_SRCH,
#endif
  /* Idle Cell Detection Search */
  TDSSRCHLTE_CELL_DETECT_SRCH,
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* Connected Mode Cell Detect & Measurement Search */
  TDSSRCHLTE_CELL_DETECT_AND_MEAS_SRCH,
#endif 
  /* Cell Measurement Search */
  TDSSRCHLTE_CELL_MEAS_SRCH,
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /*First Medium gap AGC + Search + determine viurtual cell list*/
  TDSSRCHLTE_CELL_MEDIUM_AGC_SRCH,
  /*Second Medium gap AGC + Measumrent + Search*/
  TDSSRCHLTE_CELL_MEDIUM_AGC_MEAS_SRCH,
  /*Large gap AGC + Measumrent + Search, one for all*/
  TDSSRCHLTE_CELL_LARGE_AGC_MEAS_SRCH,
#endif
  /* None Search */
  TDSSRCHLTE_NONE_SRCH,
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* In Srch */
  TDSSRCHLTE_IN_SRCH
#endif
} tdssrchlte_srch_enum_type;

/* Commands during LTE NSET search */
typedef enum
{
  /* Cell Detect Srch Done */
  TDSSRCHLTE_CELL_DETECT_DONE,
  /* Cell Meas Srch Done */
  TDSSRCHLTE_CELL_MEAS_DONE
} tdssrchlte_cmd_enum_type;

/* Local command to LTE NSET search */
typedef struct
{
  /* local command header */
  tdsl1def_l1_local_cmd_hdr_type hdr;

  /* status of command */
  boolean status;

  /* type of srch done for command */
  tdssrchlte_cmd_enum_type srch_done;

} tdssrchlte_local_cmd_type;

/* This struct is used to keep track of EARFCNs
   to search in a drx cycle. */
typedef struct
{
  /* Index into the array tdssrchlte_freq_list.lte_frequency[] */
  uint16 idx_to_earfcn;
  /* Type of srch to be done on above earfcn */
  tdssrchlte_srch_enum_type srch_type;
  /* Indicates if search is complete on this earfcn */
  boolean srch_done;
  /*indicate whether evaluation on this is done or not*/
  boolean eval_done; 
} tdssrchlte_earfcn_to_srch_struct_type;

typedef enum
{
  /* This is the first drx cycle in which the cell
     detect timer expired */
  TDSSRCHLTE_CELL_DETECT_TMR_EXPIRED_FIRST_DRX,
  /* Cell Detect timer has expired in a previous
     drx cycle then this one */
  TDSSRCHLTE_CELL_DETECT_TMR_EXPIRED_NEXT_DRX,
  /* Cell Detect timer has not expired */
  TDSSRCHLTE_CELL_DETECT_TMR_NOT_EXPIRED
}tdssrchlte_high_prior_cell_detect_tmr_expiry_state_enum_type;

/*This is for the bunch of LTE frequencies to be measured in 
  the next n consecutive frequencies. */
typedef enum
{
  /* This is the first drx cycle in which the cell
     detect timer expired */
  TDSSRCHLTE_CELL_MEAS_TMR_EXPIRED_FIRST_DRX,
  /* Cell Detect timer has expired in a previous
     drx cycle then this one */
  TDSSRCHLTE_CELL_MEAS_TMR_EXPIRED_NEXT_DRX,
  /* Cell Detect timer has not expired */
  TDSSRCHLTE_CELL_MEAS_TMR_NOT_EXPIRED
}tdssrchlte_high_prior_cell_meas_tmr_expiry_state_enum_type;

/* When UE is in High Priority Scheduling Mode, Cell Detection Search is done
   every (60 * nlayers) seconds. When this timer expires, the next 'n' consecutive
   drx cycles are used for cell detection searches with cell detection search
   on 1 earfcn per drx cycle. 'n' is the number of higher priority earfcns */

/* Struct which is the state machine for the above */
typedef struct
{
  #if 0
  /* This keeps track of the state of the Cell Detect timer */
  tdssrchlte_high_prior_cell_detect_tmr_expiry_state_enum_type cell_detect_tmr_state;

  /* Same as the number of higher prior earfcn */
  uint16 num_drx_for_cell_detect_srch;

  /*keep track of the LTE cell measurement timer*/
    tdssrchlte_high_prior_cell_meas_tmr_expiry_state_enum_type cell_meas_tmr_state;
  #endif  
  /* number of T2L measurement performed after last srch+meas. This will be tracked in all priority and high priority mode */
  uint16 num_meas_after_search;
} tdssrchlte_high_prior_state_mc_struct_type;

typedef struct
{
  /* Index into the array tdssrchlte_freq_list.lte_frequency[]. Points to the last
     EARFCN on which Cell Detection Search was done */
  uint8 cell_detect_earfcn_idx;

  /* Index into the array tdssrchlte_freq_list.lte_frequency[]. Points to the last
     EARFCN on which Cell Measurement Search was done */
  uint8 cell_meas_earfcn_idx;

  /* Number of valid entries in array earfcn_to_srch[] */
  uint8 num_earfcn_to_srch;

  /* List of EARFCN to be srched in this drx cycle */
  tdssrchlte_earfcn_to_srch_struct_type earfcn_to_srch[TDSL1_MAX_EUTRA_FREQ];

  /* Index into the array earfcn_to_srch[], this index stores the earfcn index 
     on which the current search is in progress */
  uint8 saved_earfcn_to_srch_idx;

 // uint8 scheduled_n_not_done;

  /*add a flag to indicate whether this is the first DETECTION Period*/
  uint8 is_first_detect_period;

  /* State machine for Cell Detection Searches during High Prior Scheduling */
  tdssrchlte_high_prior_state_mc_struct_type high_prior_state_mc;

  tdssrchlte_earfcn_to_srch_struct_type earfcn_srched[TDSL1_MAX_EUTRA_FREQ];
  uint8                                 num_earfcn_srched; 

} tdssrchlte_srch_schedule_tracker_struct_type;

/* TDSCDMA LTE Search States */
typedef enum
{
  TDSSRCHLTE_SRCH_IDLE_STATE,
  TDSSRCHLTE_SRCH_STARTUP_STATE,
  TDSSRCHLTE_SRCH_CELL_DET_STATE,
  TDSSRCHLTE_SRCH_CELL_MEAS_STATE,
  TDSSRCHLTE_SRCH_CLEANUP_STATE,
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* CM prepare state set when CM triggered */
  TDSSRCHLTE_SRCH_CM_PREPARE_STATE,
  /* CM Search sent */
  TDSSRCHLTE_SRCH_CM_SRCH_MEAS_STATE,
  /* CM waiting for Meas. Layer for results */
  TDSSRCHLTE_SRCH_CM_MEAS_EVAL_STATE
#endif
  ,
  TDSSRCHLTE_SRCH_IDLE_MODE_IN_PROG
} tdssrchlte_srch_state_enum_type;

/* Search Mode types */
typedef enum
{
  /* LTE Search in Idle Mode */
  TDSSRCHLTE_IDLE_MODE,
  /* LTE Search in Compressed Mode */
  TDSSRCHLTE_CONNECTED_MODE
} tdssrchlte_srch_mode_enum_type;
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/* Structure to store the rf tune time */
typedef struct
{
  /* RF tune startup time */
  uint32 startup_time_cx8;
  /* RF tune cleanup time */
  uint32 cleanup_time_cx8;
} tdssrchlte_rf_tune_time_info_struct_type;
#endif
/* Control structure for W2L search */
typedef struct
{
  /* Indicates that a search request came and it has started */
  boolean srch_in_progress;
  /* State of TDS to L search */
  tdssrchlte_srch_state_enum_type srch_state;
  /* Type of search in progress */
  tdssrchlte_srch_enum_type srch_type;
  /*current earfcn to perform search*/
  tdssrchlte_frequency_info_type *cur_searched_earfcn;
  /* whether LTE srchs in this DRX cycle are done or not. */
  boolean srch_in_this_drx_done;   
  /* whether first LTE srch in this DRX cycle is started or not*/
  boolean srch_in_this_drx_started; 
  /* Indicates the need to send DEINIT_CNF after searches are done */
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* whether current LTE measurement gap is aborted or not */
  boolean lte_gap_aborted;
  /*whether PSHO is waiting for LTE measurement to complete. */
  boolean psho_wait_for_t2l_meas;
  /*whether T2L driver is waiting for TFW gap to end*/
  boolean wait_for_tfw_gap_end;
  /*whether T2L driver needs to send de-init request*/
  boolean send_deinit_needed;  
  /* The subframe number when the CM GAP starts */
  uint16 curr_subfn;
#endif
  //below indices are also needed for idle mode. 
  /* Indicates the earfcn_idx where CM is in progress */
  uint8 earfcn_idx;
  /*Indicates the earfcn_idx which will be searched/measured next*/
  uint8 next_earfcn_idx;
  //remove unused pointers
  //void *rf_buf_ptr;
  //tdssrchlte_tune_done_cb_type *tune_done_cb;

  boolean mcpm_client_requested;
  //Whether LTE measurement is attempted in this DRX cycle or not
  boolean lte_meas_attempted_this_drx_cycle; 

  //The flag to check whether we have completed T2L scheduling in current DRX cycle or not:
  //1) next wakeup channel is PICH; or 2) cannot get TRM lock in this DRX cycle. 
  boolean lte_meas_scheduling_complete_in_curr_drx;

  #ifdef FEATURE_TDSCDMA_IRAT_FED_BASED
  boolean send_t2l_init_req_required;
  #endif
  //Needs to keep track of TRM set state output in order to fill in RF device IDs for T2L IRAT. 
  #ifdef FEATURE_TDSCDMA_TRM_NEW_API
  trm_set_client_state_output_data trm_output_state;
  #endif

} tdssrchlte_ctrl_struct_type;

extern tdssrchlte_ctrl_struct_type tdssrchlte_ctrl_info;

/*-----------------------------------------------------------*/
/* The gap information to LTE in the CM mode */
typedef struct
{
  /* gap subframe start */
  uint16 gap_subfm_start;
  /* gap subframe end */
  uint16 gap_subfm_end;
  /* gap time slot start */
  uint16 gap_ts_start;
  /* gap time slot end */
  uint16 gap_ts_end;
  /*the subframe index of the gap start*/
  uint8 gap_subfm_start_idx;
  /*the subframe index of the gap end*/
  uint8 gap_subfm_end_idx;
  /* gap time slot start index*/
  uint8 gap_ts_start_idx;
  /* gap time slot end index*/
  uint8 gap_ts_end_idx;
  /*10ms, 16 continuous time slots*/  
  boolean is_small_gap;
  /*[12, 27]ms, practically 20ms*/
  boolean is_medium_gap;
  /*>=28ms, practically 30ms*/
  boolean is_fat_gap;
  /*gap length in the numbe rof subframes*/
  uint8 gap_len;

  /*configured TFW gap subframe start*/
  uint16 tfw_gap_subfm_start;
  /*Configured TFW gap end sub-frame number*/
  uint16 tfw_gap_subfm_end;
  uint8 tfw_gap_end_ts_idx;

//For debugging purpose:
  uint16 sched_point_subfn_rx;
  uint16 sched_point_chipx8_rx;

  uint16 tfw_gap_start_subfn_rx;
  uint16 tfw_gap_start_chipx8_rx;
  uint16 tfw_gap_end_subfn_rx;
  uint16 tfw_gap_end_chipx8_rx;

  uint16 lte_gap_start_subfn_rx;
  uint16 lte_gap_start_subfn_wall;
  uint16 lte_gap_start_chipx8_rx;
  uint16 lte_gap_start_chipx8_wall;

  uint16 lte_gap_end_subfn_rx;
  uint16 lte_gap_end_subfn_wall;
  uint16 lte_gap_end_chipx8_rx;
  uint16 lte_gap_end_chipx8_wall;

} tdssrchlte_cm_meas_gap_info_struct_type;

/*For LTE CM measurement*/
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
//LTE CM Measurement Scheduler
typedef enum
{
  SRCH_PERIODICITY = 0, 
  MEAS_PERIODICITY
}tdssrchlte_scheduling_mode_enum_type;

typedef struct 
{
  uint32 EARFCN;  
  tdssrchlte_scheduling_mode_enum_type scheduling_mode;
  //The current timer_count of this frequency
  uint32 time_count;
  //Number of consecutive cycles where no cell is detected on this frequency
  uint8  num_cycles_without_detection;
} tdssrchlte_cm_scheduling_freq_info_struct_type;

typedef struct 
{
  //external information to calculate Srch periodicity and meas periodicity
  uint8 srch_min_periodicity;
  uint16 periodicity;  //unit: radio frame
  uint16 m_len;        //Unit: radio frame
  uint16 t_meas_period_eutra_tdd; //unit: ms
  uint16 t_eutra_tdd;
  uint16 gsm_present;
  uint16 speed_factor; 
  // SRCH Periodicity in unit of 5ms
  uint32 t_srch_eutra;
  // Measurement Periodicity in unit of 5ms
  uint32 t_meas_eutra;
  //current LTE frequency idx which is to be measured.
  uint8  curr_freq_idx_to_meas;
  //next LTE frequency idx which is to be measured.
  uint8  next_freq_idx_to_meas;
  //whether scheduling mode was changed for any frequency. 
  boolean sched_mode_changed;
  //The scheduling information for each EUTRA frequency
  tdssrchlte_cm_scheduling_freq_info_struct_type freq_info[TDSSRCHLTEDRV_MAX_FREQS];
}tdssrchlte_cm_scheduler_struct_type;


/* Struct for LTE Connected Mode info related to gap params and srch/meas params */
typedef struct
{
  /* Flag to check contents are updated and valid to be used in next frame's gap */
  boolean valid;
  /* Subframe number for starting the gap */
  uint16 subfn;
  /* Valid EARFCN index to search/meas on */
  uint8 earfcn_idx;
  /* freq and gap info and search-meas params to be sent to LTE */
  tdssrchltedrv_timed_srch_meas_info_struct_type timed_srch_meas_info;
} tdssrchlte_cm_srch_meas_param_info_struct_type;

/***
 ***  TTT Management
 ***/

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  tdsl1_interrat_periodic_meas_struct_type   inter_rat_meas;
}tdssrchlte_eutra_periodic_meas_cmd_type;

/*
 * Measurement Structure and Tables
 */

/* Structure to be used for adding freqs for measurement */
typedef struct
{
  /* LTE EARFCN */
  uint32 EARFCN;  
  /* Flag for if its an LTE TDD freq */
  boolean is_tdd_freq;
  /* Flag for if BW is present in MCM */
  boolean dl_bandwith_present;
  /* Bandwidth */
  tdssrchltedrv_earfcn_dl_bandwidth dl_bandwidth;
  /* Number of blacklisted cells */
  uint8 num_blacklisted_cells;
  /* List of blacklisted cells, identified by cell_id */
  uint16 blacklisted_cells[TDSSRCHLTEDRV_MAX_CELLS_PER_FREQ];
} tdssrchlte_add_freq_info_type;

/* Struct for the frequency information pending add/remove */
typedef struct
{
  /* Num FDD frequencies pending to be added */
  uint8 num_fdd_add_pending;
  /* Num FDD frequencies pending to be removed */
  uint8 num_fdd_remove_pending;

  /* Num TDD frequencies pending to be added */
  uint8 num_tdd_add_pending; 
  /* Num TDD frequencies pending to be removed */
  uint8 num_tdd_remove_pending;
  
  /* Saved freq info to be added */
  tdssrchlte_add_freq_info_type saved_add_freq_pending_list[TDSSRCHLTE_CM_MAX_NUM_FREQ];
  /* Frequency saved to be removed */
  uint16 saved_freq_remove_pending[TDSSRCHLTE_CM_MAX_NUM_FREQ];
} tdssrchlte_pending_add_remove_freq_struct_type;

/* Struct for saving and storing the meas response returned from LTE, 
   to be passed on to measurements layer */
typedef struct
{
  /* Flag to indicate the validity of the results */
  boolean valid;
  /* Frequency on which the search/meas was done */
  uint32 earfcn;  
  /* Number of cells measured */
  uint8 num_meas_cells;
  /* Number of cells that were ONLY detected (no measurements) */
  uint8 num_detected_cells;
  
  /* Cell list of cells including the old & new detected cells */
  tdssrchlte_cell_struct_type cm_meas_rsp_cell_list[TDSSRCHLTEDRV_CM_MAX_MEAS_CELLS 
	  + TDSSRCHLTEDRV_CM_MAX_NEW_DETECTED_CELLS];
} tdssrchlte_cm_meas_rsp_info_struct_type;

/* this structure is only used inside the W2L Search Results Log structure.
   The definition is separated for size calculation purposes (for the case
   with varying numbers of cells) */
typedef PACKED struct PACKED_POST
{ 
  /* physical cell ID (ranges 0-504) */
  uint16 cell_id;
  /* the CP type */
  uint8 cp_type;
  /* the estimated frequency error */
  int16 freq_offset;
  /* The detected SSS power of the cell when it was detected. */
  uint32 sss_corr;  

  /* the following fields are raw search data per antenna... */
  PACKED struct PACKED_POST
  {
    /* the timing of the cell tower (range 0-307199) */
    uint32 cell_timing;
    /* the instantaneous RSRP in units of dBx16 (range -140 to -44 dBm)*/
    int16 inst_rsrp_rx;
    /* the instantaneous RSRQ in units of dBx16 (range -30 to 0 dBm)*/
    int16 inst_rsrq_rx;
    /* the instantaneous RSSI in units of dBx4 (range -140 to -44 dBm)*/
    int16 inst_rssi_rx;
  } antenna_data[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];

  /* the following fields are FILTERED signal data */
  PACKED struct PACKED_POST
  {
    /* the filtered RSRP in units of dBx4 (range -140 to -44 dBm) */
    int16 filt_rsrp;
    /* the filtered RSRQ in units of dBx4 (range -140 to -44 dBm) */
    int16 filt_rsrq;
  } filtered_data[TDSSRCHLTE_MAX_NUM_FILTERS];   
} tdssrchlte_sr_log_cell_struct_type;

typedef enum
{
  /* This is the first command in which LTE sends the
  AGC value for the first time */
  TDSSRCHLTE_AGC_CMD,
  /* AGC cmd for current FDD/TDD and Meas Request for the previous LTE TDD freq */
  TDSSRCHLTE_AGC_CMD_MEAS_REQ,
  /* Cell detect request */
  TDSSRCHLTE_CELL_DET_REQ,
  /* Cell detect + meas for FDD, cell detect + virtual cell info for TDD */
  TDSSRCHLTE_CELL_DET_MEAS_VIRTUAL_CELL_REQ,
  /* Cell detect and measurement request */
  TDSSRCHLTE_CELL_DET_MEAS_REQ,
   /*First Medium gap AGC + Search + determine viurtual cell list*/
  TDSSRCHLTE_CELL_MEDIUM_AGC_SRCH_REQ,
  /*Second Medium gap AGC + Measumrent + Search*/
  TDSSRCHLTE_CELL_MEDIUM_AGC_MEAS_SRCH_REQ,  
  /* No. of LTE commands */
  TDSSRCHLTE_CM_LTE_NUM_CMD
} tdssrchlte_timed_srch_meas_req_enum_type;

#endif /* defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)*/

/*-----------------------------------------------------------*/
/*    Data structure for T2L Tresel Optimization                                                   */
#ifdef FEATURE_TDSCDMA_TO_LTE_EVAL_TRESEL_OPT
#define TDSSRCHLTE_MAX_DRX_EVENTS_CHECKED    10
typedef enum
{
  TDSSRCHLTE_ET_STATE_MONITORING = 0,
  TDSSRCHLTE_ET_STATE_TRIGGERED = 1
}tdssrchlte_et_tracker_state_enum_type;

typedef struct 
{
  uint8 num_consec_et_events;   
  uint8 num_consec_drx_events_with_optimized_tresel;
  boolean cur_drx_event_et_flag;
  boolean t2l_meas_is_triggered;
  tdssrchlte_et_tracker_state_enum_type et_track_state;
  uint32 cur_drx_event_duration;    //in sub-frames
  uint32 optimized_tresel_thresh;    //in sub-frames
  uint32 drx_events_durations[TDSSRCHLTE_MAX_DRX_EVENTS_CHECKED];
}tdssrchlte_t2l_eval_early_termination_tracker_type;
#endif

/*-------------------------------------------------------------------------*/
/*                        GLOBAL VARIABLES                                 */
/*-------------------------------------------------------------------------*/
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
extern tdssrchlte_cm_meas_gap_info_struct_type tdssrchlte_cm_meas_gap_info;
extern tdssrchlte_cm_srch_meas_param_info_struct_type tdssrchlte_cm_srch_meas_info;
extern boolean tdssrchlte_cm_srch_meas_round_robin;
extern boolean tdssrchlte_cm_cell_detected_in_one_cycle;
    /*LTE will return the gap state after the API call */
extern     lte_ml1_irat_gap_schedule_gap_state_s tdssrchlte_gap_state;	
    /* Gap parameters returned from LTE  */
extern     lte_ml1_irat_gap_scheduled_info_s tdssrchlte_gap_parms;
	/* The dect/meas results sent to LTE upfront at API */
extern 	lte_cphy_irat_meas_timed_srch_meas_req_s tdssrchlte_timed_srch_meas_req_msg;

	/*GAP usage Hint for LTE*/
	#ifdef FEATURE_TDSCDMA_IRAT_FED_BASED
	extern lte_ml1_irat_gap_schedule_hint_e tdssrchlte_gap_usage_hint;
  #endif
//by default, it is the dual mode with GSM.
  #ifdef FEATURE_SGLTE
extern     tdsl1_meas_mode_enum_type tdssrchlte_cm_srch_meas_mode;
  #endif
#endif


/* LTE cells, total of 32 */
extern tdssrchlte_cell_struct_type  tdssrchlte_cell_list[TDSSRCHLTE_NSET_MAX];
extern uint8 tdssrchlte_nset_cnt;

/* LTE Search information */
extern tdssrchlte_ctrl_struct_type tdssrchlte_ctrl_info;

/* Tracks globals to help schedule LTE Searchs depending on scheduling mode */
extern tdssrchlte_srch_schedule_tracker_struct_type tdssrchlte_srch_schedule_tracker;

/* LTE Neighbor List */
extern tdssrchlte_frequency_list_type tdssrchlte_freq_list;

/* Table of TmeasureEUTRA values indexed by K values. K = 0..2 is not valid */
extern uint8 tdssrchlte_t_meas_eutra_tab[10];

/* Table for scheduling LTE Cell Detection search when All Priority Scheduling
   is being used, values indexed by K values. K = 0..2 is not valid */
extern uint8 tdssrchlte_cell_detect_scheduling_tab[10];

/* Table for scheduling LTE Cell Detection search when All Priority Scheduling
   is being used, values indexed by K values. K = 0..2 is not valid */
extern uint8 tdssrchlte_cell_meas_scheduling_tab[10]; 

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/* Object to store the meas repsonse results from LTE */
extern tdssrchlte_cm_meas_rsp_info_struct_type tdssrchlte_cm_meas_rsp_info;

extern uint16 tdssrch_ts_begin_position_inc_st[8];

extern boolean tdssrchltedrv_send_fw_gap_info(void);
#endif

/*===========================================================================

FUNCTION:  TDSSRCHLTE_INIT

DESCRIPTION:
  Inits the full internal variables as well as initializing functions to
  allow this module to perform its operations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_init(void);

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/*===========================================================================

FUNCTION:  WSRCHLTE_INIT_ON_RESUME

DESCRIPTION: This function does the operations needed when a HHO fails and 
  resume to WCDMA is needed. This will be not clean up LTE frequency info
  if alreaedy there.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_init_on_resume(
  /* Search mode: Idle or Compressed Mode */
  tdssrchlte_srch_mode_enum_type srch_mode);

#endif
/*===========================================================================

FUNCTION Function_name

DESCRIPTION:
  Function Description

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_update_lte_prior_info(
  tdsl1_meas_priority_info_type *prior_info); 

/*============================================================================
FUNCTION: tdssrchlte_insert_and_sort_prior_info()

DESCRIPTION:
  Performs insertion sort on the given item and stores the value in 
  the given destination. Sorting is done in descending order by priority, so 
  the highest priority is listed first.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  Index of location where value was inserted. If the return value is greater
  than or equal to TDSSRCHLTE_MAX_NUM_FREQ, then the specified entry was not
  inserted.

SIDE EFFECTS:   
  The given destination array is updated to reflect the new insertion. 
  
EXAMPLE USAGE:
  Source SIB19 priorities: [3, 5, 2, 1, 4, 7]
  A dash (-) indicates an uninitialized value.
  
  INPUT PRIORITY  CURRENT_ENTRY_IDX   LAST_ELEM_TO_MOVE_IDX  DESTINATION ARRAY
  3               0                   0                      [3, -, -, -]
 
  5               0                   1                      [3, 3, -, -]
  5               0                   0                      [5, 3, -, -]
 
  2               0                   0                      [5, 3, -, -]
  2               1                   0                      [5, 3, -, -]
  2               2                   0                      [5, 3, 2, -]
 
  1               0                   0                      [5, 3, 2, -]
  1               1                   0                      [5, 3, 2, -]
  1               2                   0                      [5, 3, 2, -]
  1               3                   0                      [5, 3, 2, 1]
 
  4               0                   0                      [5, 3, 2, 1]
  4               1                   4                      [5, 3, 2, 1]
  4               1                   3                      [5, 3, 2, 2]
  4               1                   2                      [5, 3, 3, 2]
  4               1                   1                      [5, 4, 3, 2]
 
  7               0                   0                      [5, 4, 3, 2]
  7               0                   4                      [5, 4, 3, 2]
  7               0                   3                      [5, 4, 3, 3]
  7               0                   2                      [5, 4, 4, 3]
  7               0                   1                      [5, 5, 4, 3]
  7               0                   0                      [7, 5, 4, 3]
=============================================================================*/
extern void tdssrchlte_insert_and_sort_prior_info(
  /* The destination structure containing the list to insert into */
  tdssrchlte_frequency_list_type *destination,

  /* The entry to be inserted */
  tdsl1_inter_rat_eutra_priority_info_type *source, 
  
  /* frequency information containing bandwidth information */
  tdsl1_eutra_frequency_info_type *freq_info, 

  /* Blacklist to notify LTE */
  tdssrchltedrv_blacklist_info_struct_type *blacklist_msg,
  
  /* The number of entries in the source list */
  uint8 source_length); 

/*===========================================================================

FUNCTION Function_name

DESCRIPTION:
  Function Description

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_update_lte_nbr_info(
  tdsl1_meas_eutra_frequency_list_type *cell_list);

/*============================================================================
FUNCTION: tdssrchlte_update_lte_freq_and_prior_info()

DESCRIPTION:
  Syncs the wsrchlte frequency info structure with the data provided in SIB19 
  by RRC. 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:   
  None. 
=============================================================================*/
extern void tdssrchlte_update_lte_freq_and_prior_info(
  /* List of frequency information to be updated */
  tdsl1_meas_eutra_frequency_list_type *freq_list, 
  /* Pointer to the priority info struct received in 3rd MEAS_REQ */
  tdsl1_meas_priority_info_type *prior_info);

/*===========================================================================

FUNCTION Function_name

DESCRIPTION:
  Function Description

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
// name changed by owen for crm build pass
extern void tdssrchlte_rmv_lte_nbr_info(void);

/*===========================================================================

FUNCTION: TDSSRCHLTE_COMPUTE_LTE_MEAS_RULES

DESCRIPTION:
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchlte_compute_lte_meas_rules(boolean schedulng_mode_changed,uint16 prev_tmr_expiry_status);

/*===========================================================================

FUNCTION: TDSSRCHLTE_SRCH_DONE_HANDLER

DESCRIPTION:
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_srch_done_handler(tdssrchlte_cmd_enum_type srch_done);

/*===========================================================================

FUNCTION: TDSSRCHLTE_CHECK_NEXT_EARFCN_INDEX_FOR_SRCH

DESCRIPTION:
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
//name changed for crm build
extern uint8 tdssrchlte_check_next_earfcn_index_for_srch(tdssrchlte_srch_enum_type srch_type);

/*===========================================================================

FUNCTION: TDSSRCHLTE_UPDATE_STATUS_FLAG_FOR_EARFCN_SRCHED

DESCRIPTION:
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
//name changed by owen for crm build pass
extern void tdssrchlte_update_status_flag_for_earfcn_srched(uint8 earfcn_idx);

/*===========================================================================

FUNCTION: TDSSRCHLTE_UPDATE_LTE_FREQUENCIES

DESCRIPTION:
  This function updates the frequencies received from a SIB/MCM into internal
  database for further search procedure. This function overwrites any
  existing frequency info with the new set being received. If number of
  frequencies passed are more than 4, it will consider the first 4
  frequencies, discarding the rest.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Old frequency information will be overwitten.
===========================================================================*/
extern void tdssrchlte_update_lte_frequencies(
  /* List of frequency information to be updated */
  tdsl1_meas_eutra_frequency_list_type *freq_list);

/*===========================================================================

FUNCTION: TDSSRCHLTE_GET_LTE_CELL

DESCRIPTION: 
  This function returns an empty LTE cell entry to be used.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the new cell allocated.
  NULL if no more cell available.

SIDE EFFECTS
  If there are available cells, number of LTE cell increases by 1.
===========================================================================*/
// name changed by owen for crm build pass
extern tdssrchlte_cell_struct_type *tdssrchlte_get_lte_cell(void);

/*===========================================================================

FUNCTION: TDSSRCHLTE_RESUME_SEARCH

DESCRIPTION: 
  This function is the LTE search state machine. It controls what 
  messages to handle and what state it will move to. It will create the
  proper messages and send to LTE.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdssrchlte_resume_search(
  /* Message received that resulted in a call to resume search */
  tdssrchltedrv_srch_msg_enum_type msg_type);

/*===========================================================================

FUNCTION: tdssrchlte_start_lte_idle_meas

DESCRIPTION: 
  This function is the entry point for a LTE search/measurement from L1M module. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  State machine will start.
===========================================================================*/
extern void tdssrchlte_start_lte_idle_meas(void);

/*===========================================================================

FUNCTION: TDSSRCHLTE_DO_LTE_SEARCH

DESCRIPTION: 
  This function is the entry point for a LTE search. It will kick
  start the state machine to schedule searches into LTE. Caller should pass
  the search type and the EARFCN in which the search should be conducted.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if search started successfully.
  FALSE otherwise.

SIDE EFFECTS
  State machine will start.
===========================================================================*/
extern boolean tdssrchlte_do_lte_search(
  /* Search type requested */
  tdssrchlte_srch_enum_type srch_type,
  /* Frequency info for the search to be performed */
  tdssrchlte_frequency_info_type *earfcn_info,
  /*Whether this is the first T2L idle mode measurement in a DRX cycle*/
  boolean first_t2l_idle,
  /* CFN for which the gap will start */
  uint16 gap_start_cfn, 
  /* Search mode: Idle or Compressed Mode */
  tdssrchlte_srch_mode_enum_type srch_mode);

/*===========================================================================

FUNCTION: TDSSRCHLTE_MEAS_INIT

DESCRIPTION: 
  This function sends LTE INIT message before any measurements
  start in drx mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  LTE State machine on LTE side will be initialized
===========================================================================*/
extern void tdssrchlte_meas_init(
tdssrchlte_srch_mode_enum_type srch_mode
);

/*===========================================================================

FUNCTION: TDSSRCHLTE_MEAS_CLEANUP

DESCRIPTION:
  This function is called to cleanup LTE measurements when
  a state transition happens.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_meas_cleanup(void);


/*===========================================================================

FUNCTION: tdssrchlte_send_fw_t2x_cleanup_cmd

DESCRIPTION:
  This function is called to send FW_IRAT_T2X_CLEANUP command to FW.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_send_fw_t2x_cleanup_cmd(void);

/*===========================================================================

FUNCTION: tdssrchlte_move_to_next_low_priority_earfcn_for_srch

DESCRIPTION: 
  This function moves the tdssrchlte_srch_schedule_tracker.cell_detect_earfcn_idx
  to one earfcn before the next low priority earfcn for all priority search.
  In this way, next earfcn to be searched will be a low priority LTE frequency. 
 
  Called in case of state transition from High Priority to Low Priority. 
  
DEPENDENCIES
  None.

RETURN VALUE
  index to earfcn into the array tdssrchlte_freq_list.lte_frequency

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_move_to_next_low_priority_earfcn_for_srch(void); 

/*===========================================================================

FUNCTION: tdssrchlte_clean_pending_cell_meas_on_all_earfcn

DESCRIPTION: 
  This fn scans the tdssrchlte_srch_schedule_tracker.earfcn_to_srch to
  determine if any earfcn is remaining for cell meas search. In high priority
  scheduling mode, cell meas search can be scheduled on more then 1 earfcn, this
  fn is needed in this case to check if any earfcns are left for search once a cell
  meas on 1 earfcn completes
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any more earfcns are left.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_clean_pending_cell_meas_on_all_earfcn(void);


/*===========================================================================

FUNCTION: tdssrchlte_clean_pending_cell_detect_on_all_earfcn

DESCRIPTION: 
  This function clean all the pending LTE frequencies to be detected in the following
  DRX cycles.
 
  Called in case of state transition from High Priority mode to All Priority Mode.
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any more earfcns are left.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_clean_pending_cell_detect_on_all_earfcn(void);

/*===========================================================================

FUNCTION: tdssrchlte_update_earfcn_indices_for_detect_meas

DESCRIPTION:
  This function will send LTE search done signal to L1M. 

  lte_srch_success: TRUE: one LTE frequency is successfully searched.
                              FALSE: Otherwise. 

DEPENDENCIES
  None.

RETURN VALUE
  Return TRUE is any entries get added to tdssrchlte_srch_schedule_tracker.earfcn_to_srch[].

SIDE EFFECTS
  Below mentioned gloabl gets updated
  tdssrchlte_srch_schedule_tracker.earfcn_to_srch[]
===========================================================================*/
extern void tdssrchlte_send_srch_done_signal(boolean lte_srch_success);



/*===========================================================================

FUNCTION: tdssrchlte_is_lte_freq_present

DESCRIPTION: 
  This function checks whether any LTE frequency is present in T2L idle mode or conn mode. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: LTE frequency present 
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdssrchlte_is_lte_freq_present(void);

/*===========================================================================

FUNCTION: tdssrchlte_update_scheduler_tracker_with_appointed_earfcn

DESCRIPTION:
  This function grap all EARFCNs which need to be detected or measured, depending on their priorities 
  and the priority scheduling mode, and put their indices into a global array. 

DEPENDENCIES
  None.

RETURN VALUE
  Return TRUE is any entries get added to tdssrchlte_srch_schedule_tracker.earfcn_to_srch[].

SIDE EFFECTS
  Below mentioned gloabl gets updated
  tdssrchlte_srch_schedule_tracker.earfcn_to_srch[]
===========================================================================*/
boolean tdssrchlte_update_scheduler_tracker_with_appointed_earfcn(
uint8 earfcn_idx, 
tdssrchlte_srch_enum_type srch_type);

/*===========================================================================
FUNCTION  tdssrchlte_act_time_check

DESCRIPTION:
  This function to check the possible conflict with the act time . 

DEPENDENCIES
  None.

RETURN VALUE
TRUE:  Conflict, the act time falls in the gap
FALSE: there is no confilict.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchlte_check_act_time_conflict_with_gap(uint16 act_time_in_subfn, uint16 gap_subfm_start, uint16 gap_subfm_end);


#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
/*===========================================================================

FUNCTION: tdssrchlte_log_meas_result

DESCRIPTION: 
  This function put the measurement results of one LTE frequency into
  a log packet.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if search started successfully.
  FALSE otherwise.

SIDE EFFECTS
  State machine will start.
===========================================================================*/
void tdssrchlte_log_meas_result(uint8 earfcn_idx );

/*===========================================================================

FUNCTION: tdssrchlte_submit_log_meas_result

DESCRIPTION: 
  This function put the measurement results of one LTE frequency into
  a log packet.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if search started successfully.
  FALSE otherwise.

SIDE EFFECTS
  State machine will start.
===========================================================================*/
void tdssrchlte_submit_log_meas_result(void);

/*===========================================================================

FUNCTION: tdssrchlte_clear_log_meas_result

DESCRIPTION: 
  This function clear the measurement log packet within each DRX cycle. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if search started successfully.
  FALSE otherwise.

SIDE EFFECTS
  State machine will start.
===========================================================================*/
void tdssrchlte_clear_log_meas_result(void);
#endif /* FEATURE_TDSCDMA_L1_LOGGING_ENABLED */

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)

/*===========================================================================
FUNCTION  TDSSRCHLTE_CLEANUP_SEND_LTE_DEINIT

DESCRIPTION:
  This function cleans up the T2L measurements and sends LTE Deinit req message
  in case of transition from CELL DCH state.

DEPENDENCIES
  None.

RETURN VALUE 
  None

SIDE EFFECTS
  None
===========================================================================*/

void tdssrchlte_cleanup_send_lte_deinit(void);

/*===========================================================================

FUNCTION  TDSSRCHLTE_CM_RESULTS_PROC

DESCRIPTION:
  This function processes the measurement results returned from LTE in Connected
  Mode. It copies the results over to a global temp array and passes it over to the
  measurement layer for calculating the filtered values and sorting based on those
  values.

DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchlte_cm_results_proc(void); 


/*=====================================================================================

FUNCTION  TDSSRCHLTE_SAVE_SORTED_MEAS_CELL_LIST

DESCRIPTION:
  This function is to save the current meas cell list in the dect/srch state. 

DEPENDENCIES
  None.

RETURN VALUE 
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchlte_save_sorted_meas_cell_list(  
  /*pointer to the current meas list*/
  tdssrchlte_frequency_info_type* cur_meas_freq,
  /* Number of cells in the sorted list */
  uint8 num_cells);

/*=====================================================================================

FUNCTION  TDSSRCHLTE_COPY_SORTED_MEAS_CELL_LIST

DESCRIPTION:
  This function is to copy the meas cell list back to the current list in the dect/srch state. 

DEPENDENCIES
  None.

RETURN VALUE 
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchlte_copy_sorted_meas_cell_list(
   /* Pointer to the sorted 16-cell list from measurement layer */
  tdssrchlte_cell_struct_type * sorted_cell_list,
    /*pointer to the current meas list*/
  tdssrchlte_cell_struct_type * meas_cell_list,
  /* Number of cells in the sorted list */
  uint8 num_cells, 
  uint8 cell_idx);


/*==========================================================================

FUNCTION  WSRCHLTE_SAVE_SORTED_CELL_LIST

DESCRIPTION:
  This function is called by measurements layer after calculating the filtered values 
  and sorting the measured cell list sent by W2L drivers. It saves the top 16 cells 
  to be sent next time that freq is searched/measured. 

DEPENDENCIES
  None.

RETURN VALUE 
  void

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_save_sorted_cell_list(
  /* Pointer to the sorted 16-cell list from measurement layer */
  tdssrchlte_cell_struct_type *sorted_cell_list,
  /* Number of cells in the sorted list */
  uint8 num_cells);

/*===========================================================================

FUNCTION: WSRCHLTE_ADD_LTE_FREQ_FOR_MEAS

DESCRIPTION: 
  This function adds a new freq received from NW into database to be searched. It 
  also stores the blakclist info and sends it for all the frequencies sent to it in 
  in the MCM when the last freq to be added is received. If a search is in progress, 
  it saves the frequency and blacklist info to be added later. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -- successfully added/ saved to be added 
  FALSE -- otherwise
 
SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdssrchlte_add_lte_freq_for_meas(
  /* Frequency information to be added */
  tdssrchlte_add_freq_info_type* lte_earfcn_to_add, 
  /* Flag for the last freq */
  boolean last_freq);

/*===========================================================================

FUNCTION  TDSSRCHLTE_RESET_CM_LTE_SRCH_CMD_IDX

DESCRIPTION:
  This function resets the CM LTE srch cmd idx across all the
  eutra frequency.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchlte_reset_cm_lte_srch_cmd_idx(void);


/*==========================================================================

FUNCTION  TDSSRCHLTE_CM_MEAS_POST_PROC

DESCRIPTION:
  This function is called to set srch_in_progress to FALSE and saved freq are
  added or removed and sm_counter is updated. 

DEPENDENCIES
  None.

RETURN VALUE 
  void

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_cm_meas_post_proc(void);

/*===========================================================================

FUNCTION: TDSSRCHLTE_ACTIVATION_TIME_FOR_L1M

DESCRIPTION: 
  This function accept the activation time from L1M or proposes a new one if LTE CM meas is ongoing 

DEPENDENCIES
  None.

RETURN VALUE
 activation time:
 case 1: If no LTE meas ongoing, or LTE meas ongoing, but the new act time from L1M is not in the current LTE meas gap,
            we accept the act time from L1M. The return value is the same act time from L1M.
            T2L scheduler will mark this time and void the act time falls into a LTE gap.
 case 2: If LTE meas ongoing, and the act time from L1M falls in the current gap, we suggest a new act time.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 tdssrchlte_activation_time_for_l1m(
  /* the activation time from and to L1M to set up a new act time*/
  uint16 act_time_cfn);

/*===========================================================================

FUNCTION: TDSSRCHLTE_VALIDATE_EARFCN_FOR_CM_SRCH_TABLE_BASED

DESCRIPTION: 
  This function validates the next LTE EARFCN to be searched for CM based on the 
  AGC, Cell Detect and Meas Counter 

DEPENDENCIES
  None.

RETURN VALUE
  Search Type, based on EARFCN validity (NONE/Detect/Detect+Meas/Meas)

SIDE EFFECTS
  None.
===========================================================================*/
tdssrchlte_srch_enum_type tdssrchlte_validate_earfcn_for_cm_srch_table_based(
  /* Reference to return EARFCN index on which CM search is to be scheduled */
  uint8 *earfcn_idx);

/*===========================================================================

FUNCTION  TDSSRCHLTE_CM_GAP_TIMING_CONVERTER

DESCRIPTION:
  This function converts the Rx time to the wall time for a valid LTE measurement gap in the CM mode

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tdssrchlte_cm_gap_timing_converter(
uint8 gap_subfm_start, 
uint8 gap_subfm_end, 
uint8 gap_ts_start, 
uint8 gap_ts_end);

/*===========================================================================

FUNCTION  tdssrchlte_cm_is_ts_in_LTE_gap

DESCRIPTION:
  This function judges whether one timeslot in one sub_fn is within a LTE measurement gap or not. 

DEPENDENCIES
  None.

RETURN VALUE
                TRUE:    is within the gap
                 FALSE:  Otherwise. 

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchlte_cm_is_ts_in_LTE_gap
(
uint16 sub_fn,
uint8  ts_idx
);


/*===========================================================================

FUNCTION  TDSSRCHLTE_CM_GAP_INFO_FOR_UL

DESCRIPTION:
  This function gives UL the gap information for T2L CM measumrent  

DEPENDENCIES
  None.

RETURN VALUE
                  TRUE:   One LTE measurement is going on. The valid gap information is returned.
                 FALSE:  No LTE measurement is going on. The gap information should be ignored. 

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchlte_cm_gap_info_for_UL(
/*subframe number of the gap start*/
uint16* gap_sfm_start, 
/*subframe number of the gap end which is included in the gap
  so, gap_sfm_end+1 will has no LTE measurement*/
uint16* gap_sfm_end 
);

/*===========================================================================

FUNCTION  TDSSRCHLTE_UPDATE_CM_GAP_INFO_FOR_LTE_SRCH

DESCRIPTION:
  This function computes and updates the CM gap params for LTE  

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchlte_update_cm_gap_info_for_lte_srch(void);

/*===========================================================================

FUNCTION  TDSSRCHLTE_SELECT_VIRTUAL_CELL

DESCRIPTION:
  This function selects the virtual cell to be used in the next measurement gap 
  in W2L TDD CM. The virtual cell selection is determined by the highest sss correlation 
  among the preferred cells. Preferred cells are those which have been detected at least 
  twice in the last 5 detections. If there are no preferred cells, then the cell with 
  the max sss correlation is selected as the virtual cell. 

DEPENDENCIES
  None.

RETURN VALUE
  Cell index of the virtual cell

SIDE EFFECTS
  None
===========================================================================*/
//static uint8 tdssrchlte_select_virtual_cell(
  /* LTE frequency list */
//  tdssrchlte_frequency_info_type* lte_frequency);

/*===========================================================================

FUNCTION  TDSSRCHLTE_UPDATE_CM_LTE_MEAS_INFO

DESCRIPTION:
  This function updates the LTE measurement parameters during CM search. 
  This function is called when CM search takes place.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchlte_update_cm_lte_meas_info(
  /* LTE frequency list */
  tdssrchlte_frequency_info_type* lte_frequency, 
  /* measurement params */
  tdssrchltedrv_lte_cell_info_db_struct_type* meas_params,
  /* CM search type */
  tdssrchlte_srch_enum_type cm_trigger_state);

/*===========================================================================

FUNCTION  TDSSRCHLTE_TRIGGER_LTE_MEAS

DESCRIPTION:
  This function triggers an LTE search and measurement if the LTE EARFCN is
  valid, LTE init req has been sent and  gap length is sufficient to do the search 
  in LTE connected mode.

DEPENDENCIES
  None.

RETURN VALUE
  Search Type (NONE/Detect/Detect+Meas/Meas)

SIDE EFFECTS
  None
===========================================================================*/
tdssrchlte_srch_enum_type  tdssrchlte_trigger_lte_meas(void);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_FW_GAP_INFO

DESCRIPTION:
  This function will send the FW LTE gap info to FW

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchltedrv_send_fw_gap_info(void);

/*===========================================================================

FUNCTION  TDSSRCHLTE_CLEANUP_LTE_CM_MEAS

DESCRIPTION:
  This function checks if LTE measurement is completely cleaned up. If yes,
  it returns TRUE.
  Else, if searches are in progress, it marks the need to send LTE_DEINIT_REQ
  and saves the callback to be called when LTE_DEINIT_CNF is received in drivers.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: LTE measurements are completely cleaned up
  FALSE: Still waiting for DEINIT_CNF to come.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchlte_cleanup_lte_cm_meas(
  /* Post search done call back function */
  TDSSRCHLTEDRV_DEINIT_DONE_CB_FUNC_TYPE lte_cleanup_cb_func);


/*===========================================================================
FUNCTION tdssrchlte_cm_deinit_done

DESCRIPTION
  cb function for when lte finishes deinitializing after CM seacrhes finish

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_cm_deinit_done_cb(void);

/*===========================================================================

FUNCTION  TDSSRCHLTE_GIVE_EUTRA_SNAPSHOT_DATA

DESCRIPTION:
  This function will memcpy the w/l timing snapshot data to whatever location 
  is passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchlte_give_eutra_snapshot_data(lte_l1_lte_stmr_snapshot_s *snpsht_ptr);

/*===========================================================================
FUNCTION  TDSSRCHLTE_CM_GAP_INFO_UPDATE

DESCRIPTION:
  This function converts the returend gap size from LTE to the T Rx time and update the structure

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchlte_cm_gap_info_update(uint32 returend_gap_len);

/*===========================================================================
FUNCTION  TDSSRCHLTE_CM_GAP_TIME_SLOT_USAGE_UPDATE

DESCRIPTION:
  This function computes and updates the time slot usage in the CM gap for LTE  

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
//extern void tdssrchlte_cm_gap_time_slot_usage_update(
   /* update tdsgapmgr_ts_natural_dmo_idleinterval_subframes*/
  // tdssrchmeas_ts_usage_state_enum_type ts_usage, 
   /* uppdate tdsgapmgr_idle_ts_source_subframes*/
  // tdsgapmgr_idle_ts_source_enum ts_source_bitmask);


/*===========================================================================

FUNCTION  TDSSRCHLTE_CM_LTE_MEAS_INFO_UPDATE_API

DESCRIPTION:
  This function updates the LTE measurement parameters during CM search. 
  This function is called when CM search takes place.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchlte_cm_lte_meas_info_update(
  /* LTE frequency list */
  tdssrchlte_frequency_info_type* lte_frequency, 
  /* measurement params */
  tdssrchltedrv_lte_cell_info_db_struct_type* meas_params);

/*===========================================================================
FUNCTION  tdssrchlte_validate_lte_freq

DESCRIPTION:
  This function validates the LTE frequency list and decide whether there is
  any LTE frequency available for measurement or not

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean  tdssrchlte_cm_validate_lte_freq(void);

/*===========================================================================
FUNCTION  tdssrchlte_cm_get_next_earfcn_idx

DESCRIPTION:
  This function returns the next frequency index to the LTE API call

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
uint32 tdssrchlte_cm_get_next_earfcn(void); 

/*===========================================================================

FUNCTION  TDSSRCHLTE_TRIGGER_LTE_MEAS_API

DESCRIPTION:
  This function triggers an LTE search and measurement if the LTE EARFCN is
  valid, LTE init req has been sent and  gap length is sufficient to do the search 
  in LTE connected mode.

DEPENDENCIES
  None.

RETURN VALUE
  Search Type (NONE/IN_SRCH)

SIDE EFFECTS
  None
===========================================================================*/
tdssrchlte_srch_enum_type  tdssrchlte_cm_scheduler_trigger_lte_meas(void);


/*===========================================================================

FUNCTION  Tdssrchlte_cm_scheduler_update_timer

DESCRIPTION:
  This function will update the timer of each LTE frequency, in unit of 5ms.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_cm_scheduler_update_timer(uint16 delta);

/*===========================================================================

FUNCTION  Tdssrchlte_cm_scheduler_update_timer

DESCRIPTION:
  This function will update the scheduler information based on the updated LTE frequencies.
  1) Reset the scheduler database;
  2) calculate new srch periodicity and measurement periodicity;
  3) Stretch out the timing counter of each LTE frequency. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: database is correctly updated.
  FALSE: still keep the original database. 

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchlte_cm_scheduler_update_scheduling_on_freq_change(void);


/*===========================================================================

FUNCTION  Tdssrchlte_cm_scheduler_update_timer

DESCRIPTION:
  This function will update the scheduler information based on the updated LTE frequencies.
  1) calculate new srch periodicity and measurement periodicity;
 
  This function will not reset the Scheduling Database. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: database is correctly updated.
  FALSE: still keep the original database. 

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchlte_cm_scheduler_update_scheduling_on_gapconfig_change(void);

/*===========================================================================

FUNCTION  tdssrchlte_cm_scheduler_time_to_expiration

DESCRIPTION:
  This function return the current timing to timer expiration, the scheduler mode and 
  the frequency index. 

DEPENDENCIES
  It's the caller's responsibility to make sure that no LTE frequency is in the
  middle of a detect/meas cycle when this function is called.

RETURN VALUE
  TRUE: database is correctly updated.
  FALSE: still keep the original database. 

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_cm_scheduler_time_status(
uint8 * p_freq_idx, 
uint32 * p_time, 
tdssrchlte_scheduling_mode_enum_type *p_sched_mode
);


/*===========================================================================

FUNCTION  Tdssrchlte_cm_scheduler_get_freq_to_meas

DESCRIPTION:
  This function will get the current frequency to be measured based on timer counter.
  Besides, it will also give the next frequency to be measured.
  If no LTE timer is expiring, then both frequency indices will be INVALID.  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: database is correctly updated.
  FALSE: still keep the original database. 

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchlte_cm_scheduler_get_freq_to_meas(uint8 * freq_idx, uint8 * next_freq_idx);

/*===========================================================================

FUNCTION  Tdssrchlte_cm_scheduler_meas_cycle_done

DESCRIPTION:
  This function will update the Scheduling mode of the frequency indicated
  by freq_idx, based on whether any cell is detected on this frequency or not. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: database is correctly updated.
  FALSE: still keep the original database. 

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchlte_cm_scheduler_meas_cycle_done(uint8 freq_idx, boolean cell_detected);

/*===========================================================================

FUNCTION  tdssrchlte_cm_scheduler_timer_expiring

DESCRIPTION:
  This function will check whether the SRCH timer or MEAS timer of one frequency
  is expiring or not. 

DEPENDENCIES
  None.

RETURN VALUE
  FALSE: peridocities not updated.
  TRUE: periodicities properly updated. 

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchlte_cm_scheduler_timer_expiring(uint8 freq_idx);

/*===========================================================================

FUNCTION  tdssrchlte_cm_scheduler_cal_periodicities

DESCRIPTION:
  This function will calculate the new periodicities for LTE measurement and
  search. 

DEPENDENCIES
  None.

RETURN VALUE
  FALSE: peridocities not updated.
  TRUE: periodicities properly updated. 

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchlte_cm_scheduler_cal_periodicities(void);

/*===========================================================================

FUNCTION  tdssrchlte_cm_scheduler_find_best_freq_idx

DESCRIPTION:
  This function get the currently best frequency to be measured, based on:
  1) the timer status;
  2) priorities in the future.
 
  While selecting frequency index, the input index needs to be excluded. 

DEPENDENCIES
  

RETURN VALUE
  Frequency index.  

SIDE EFFECTS
  None
===========================================================================*/
uint8 tdssrchlte_cm_scheduler_find_best_freq_idx(uint8 freq_idx_to_exclude);

#endif 

/*===========================================================================

FUNCTION: tdssrchlte_sched_idle_meas

DESCRIPTION: 
  This function schedules idle mode LTE measurement.  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if T2L idle mode measurement started successfully.
  FALSE otherwise.

SIDE EFFECTS
  State machine will start.
===========================================================================*/
extern boolean tdssrchlte_sched_idle_meas(boolean called_from_isr);

uint16 tdssrchlte_round_chipx8_to_timeslot_idx_plus(uint16 offset_chipx8);

void tdssrchlte_convert_rx_to_wall(
	uint16 *sfn_wall, 
	uint16 *chipx8_wall, 
	uint16 sfn_rx, 
	uint16 chipx8_rx);

boolean tdssrchlte_idle_check_n_prepare(void);

boolean tdssrchlte_prepare_timed_meas_info(uint16 earfcn_idx);

/*===========================================================================

FUNCTION  tdssrchlte_send_blacklist_on_resume

DESCRIPTION:
  This function sends cell blacklist to ML1 side if necessary. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchlte_send_blacklist_on_resume(void);

#ifdef FEATURE_TDSCDMA_DR_DSDS
/*===========================================================================
FUNCTION     tdssrchlte_check_secondary_client_release()

DESCRIPTION
  This function is to reply the secondary chain release request from TDRM for both T2L and DS fallback

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE

===========================================================================*/
void  tdssrchlte_check_secondary_client_release(uint64 unlock_by_ms);
#endif

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
/*===========================================================================
FUNCTION  tdssrchlte_fillup_set_state_input

DESCRIPTION:
  

DEPENDENCIES
  

RETURN VALUE
  Frequency index.  

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchlte_fillup_set_state_input(trm_set_client_state_input_type *input_t2l_state, boolean irat_start, uint8 earfcn_idx);
#endif

#ifdef FEATURE_TDSCDMA_TO_LTE_EVAL_TRESEL_OPT
/*-----------------------------------------------------------*/
/*    Function Definitions for T2L Tresel Optimization                                                   */
/*===========================================================================
FUNCTION  tdssrchlte_early_termination_tracker_init

DESCRIPTION:

This function initialize the T2L evaluation early termination tracker. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_early_termination_tracker_init(void); 

/*===========================================================================
FUNCTION  tdssrchlte_early_termination_tracker_reset

DESCRIPTION:

This function resets the T2L evaluation early termination tracker. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchlte_early_termination_tracker_reset(void);

/*===========================================================================
FUNCTION  tdssrchlte_early_termination_tracker_enter_pch

DESCRIPTION:

This function sets the tracking states/variables of the T2L evaluation early termination tracker, 
when TL1 moves into PCH state. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchlte_early_termination_tracker_enter_pch(void);
/*===========================================================================
FUNCTION  tdssrchlte_early_termination_tracker_exit_pch

DESCRIPTION:

This function calculates the tracking states/variables of the T2L evaluation early termination tracker, 
when TL1 moves out of PCH state. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchlte_early_termination_tracker_exit_pch(void);

/*===========================================================================
FUNCTION  tdssrchlte_early_termination_tracker_update_drx_dur

DESCRIPTION:

This function updates the DRX event duration with a DRX cycle length, in unit of sub-frame. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchlte_early_termination_tracker_update_drx_dur(uint16 inst_duration);

/*===========================================================================
FUNCTION  tdssrchlte_early_termination_tracker_update_et_flag

DESCRIPTION:

This function updates the early termination flag in the current DRX event. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdssrchlte_early_termination_tracker_update_et_flag(boolean et_flag);
/*===========================================================================
FUNCTION  tdssrchlte_early_termination_tracker_triggerd

DESCRIPTION:

This function returns whether T2L evaluation early termination condition is triggered or not. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: triggered
  FALSE: not triggered

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdssrchlte_early_termination_tracker_triggerd(void);

/*===========================================================================
FUNCTION  tdssrchlte_early_termination_tracker_get_treselopt

DESCRIPTION:
This function returns the optimized Tresel threshold. Please note that this function will only return calculated tresel threshold 
value in case of early termination condition is triggered. Otherwise, value 0 will be returned. 

DEPENDENCIES
  None.

RETURN VALUE
  uint32: treselection threshold in unit of sub-frame

SIDE EFFECTS
  None
===========================================================================*/

extern uint32 tdssrchlte_early_termination_tracker_get_treselopt(void);
#endif

/*===========================================================================
FUNCTION tdssrchgsm_t2l_ctrl_nv_init

DESCRIPTION
  This function initiates the global variables to enable/disable T2L IRAT acitvities in DCH and IDLE states, 
  using NV item.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Depending on NV itemss, T2L idle mode measurement or conn mode measurement will be skipped.
===========================================================================*/
extern void tdssrchlte_t2l_ctrl_nv_init(void);


#endif /* defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT) */
#endif /* defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY) */

