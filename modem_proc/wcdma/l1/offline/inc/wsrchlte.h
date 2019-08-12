/*===========================================================================
FILE: wsrchlte.h


GENERAL DESCRIPTION
  This file contains code responsible for LTE Cell Set Maintainence and Searches.


Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wsrchlte.h#1 $ DateTime: $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/16   nga     FR 30575: W2L measurement support in G2L TA 
02/24/15   ap      Initial check-in for WL1 QSH framework 
05/17/16    nga     Check mode duration including lte_init_delay if LTE_INIT_REQ was not sent
09/20/15   wz/ar   Add prototypes to query if W2G/W2L CM/FMO searches are ongoing
02/25/15   psr     Increment cell_detect_earfcn_idx only after detect search is issued
11/11/14   mm      Change prototype for updating CM gap info
10/28/14   mm      0x4024 W2L log packet version upgrade
09/26/14   skk     Externed wsrchlte_get_kdetected function.
06/26/14   mm      W2L DSDA blanking support
06/24/14   mk      Support for changing EARFCN variable type from uint16 to uint32.
06/19/14   cc      IRAT DR-DSDS check in
04/28/14   ar      Add support for doing FAST lte meas in FACH state
02/12/14   ar      Change the prototype of wsrchlte_set_max_timer to take uint32 timer
04/02/14   mm      3C IRAT/CM support
03/10/14   ks      Bolt FED API checkin
01/24/14   ar      Added support for forcing LTE detect searches after a W2L Resel failure
01/24/14   ar      Add time_elapsed parameter to wsrchlte_incr_active_timers
01/23/14   ar      Added prototype for incrementing LTE search timers
01/22/14   jk      Added support for CER SNR pruning for reducing spurious Event 3A-C reports for LTE TDD cells. 
12/23/13   ar      Add prototypes to initialize wsrchlte_freq_list
12/11/13   mm      3C CM/IRAT support
11/15/13   ar      FEFACH (FR2759) absolute priority measurements in FACH - ranking and evals
11/15/13   mm      LTE search freq scheduler and timer changes for W2L in FACH
11/05/13   wz      Idle search LTE uses Timed Srch API 
10/15/13    mm      Initial FE-FACH check-in, W2L in FACH support
09/30/13   cc      DBDC IRAT support check in
07/05/13   stk/mk  FR 2677 - Initial version. Support for W2LTE CSG.
02/05/13   db      Init LTE only when W2L MCPM client is on
01/17/13   mm      Added MDSP_AGC_TUNE_STATE and prototype for agc tune done isr handler function
01/09/13   dp      Changes to allow sending w2l cm gap-end time to WFW
10/26/12   dp      Prevent fw conflict by keeping lte init separate from w2g searches
10/15/12    jd      Memory Optimization Changes
09/27/12   cc      Changed a shared macro name to prevent the confusion between idle and CM
08/15/12   cc      Deleted sss_corr from 0x4024 log packet
08/14/12   dp      Mainline FEATURE_WCDMA_DC_HSPA_W2L
08/14/12   dp      Added search support from DC WCDMA and reorganized some code
                   into a function that both idle and cm searches share
08/13/12   mm      Bandwidth information for W2L CM
08/10/12   cc      Remove mdsp agc done signal after w2l idle
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/14/12   mm      W2L TDD CM support
06/05/12   cc      Add a function prototype to solve compilation warning
06/05/12   stk     Added support for Rel9-RSRQ based W2L reselection. 
04/11/12   cc      Added Max RSSI value of antennas reported from LTE
03/23/12   dp      CM act/deact notification interface unification
03/19/12   gsk     Support for searching 4 TDD frequencies and max upto 8 (FDD+TDD) LTE frequencies in Idle mode.
02/29/11   dp      Added code for passing W2L PSHO snapshot data
02/02/12   stk     Fixed compiler issue. 
01/31/12   stk     Changes for E-UTRA detection under "FEATURE_EUTRA_DETECT"
12/21/11   stk     Added support for Rel9-RSRQ based W2L reselection. 
11/10/11   yh      Fix double allocation of RF buf and passing incorrect idx to WFW and RF
09/23/11   dp      Changed the way W2L cell reporting to measurements layer works
09/21/11   mm      Added MDSP_AGC_TUNE_STATE and prototype for agc tune done isr handler function
09/16/11   stk     Added support for faster TmeasEUTRA when Tresel has started for any LTE cells
07/06/11   dp      upmerge from offline/main
06/07/11   mr      prune out spurious LTE cells that are not detected for a certain duration of time
05/26/11   mr      skip AGC gap if previous measurement/search gap was for the same EARFCN
05/26/11   dp      Added code so W2X_IDLE_CFG gets cleanup buf idx
05/17/11   dp      Added support for W2L Search Results logging  
04/26/11   yh      Renamed feature
03/11/11   mr      Added new scheduling algorithm and interface changes for W2L connected mode
02/25/11   mr      Added interface changes for W2L connected mode
10/15/10   mm      Added support for W2L Compressed mode
09/09/10   stk     Added support to search all EARFCN's on s-fail >= nserv drx cycles.
08/23/10   uk      changed algorithm for sending LTE INIT/Deint
08/11/10   stk     Added fn wsrchlte_update_lte_freq_and_prior_info to sort and 
                   store first 4 LTE Prio and EARFCN's, received in SIB19/3rd MEAS_REQ. 
                   This also ensures highest priority EARFCN is selected for W2L measurements.  
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
                   under FEATURE_WCDMA_ABSOLUTE_PRIORITY & FEATURE_WCDMA_TO_LTE

===========================================================================*/

#ifndef WSRCH_LTE_H
#define WSRCH_LTE_H

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_WCDMA_TO_LTE
#include "l1rrcif.h"
#include "l1def.h"
#include "wsrchltedrv.h"
#include "srchset.h" /* for srch_evt_ts_struct_type */
#include "l1cmmgr.h" /* for l1_cm_active_deactive_enum_type */


/*-------------------------------------------------------------------------*/
/*                        MACROS                                           */
/*-------------------------------------------------------------------------*/
/* Max number of LTE cells */
#define WSRCHLTE_NSET_MAX             32

#define WSRCHLTE_G2WTA_MAX_DETECT_EARFCN_PER_DRX     2
#define WSRCHLTE_G2WTA_MAX_MEAS_EARFCN_PER_DRX       3

#define WSRCHLTE_NSET_CURRENT() \
  ((wsrchlte_freq_list.lte_frequency[0].num_cells)+\
   (wsrchlte_freq_list.lte_frequency[1].num_cells)+\
   (wsrchlte_freq_list.lte_frequency[2].num_cells)+\
   (wsrchlte_freq_list.lte_frequency[3].num_cells))

/* Invalid EARFCN index */
#define WSRCHLTE_INVALID_EARFCN_INDEX 0xFF

/* Invalid EARFCN */
#define WSRCHLTE_INVALID_EARFCN 0xFFFFFFFF

/* MAX number of LTE frequencies to be searched are 4, although
   SIB can contain 8, as per 25.331 */
/* Considering 4 FDD and 4 TDD LTE frequencies */
#define WSRCHLTE_MAX_NUM_FREQ          8
#define WSRCHLTE_MAX_NUM_DISCARDED_FREQ (WSRCHLTE_MAX_NUM_FREQ-1)

/* Max number of LTE TDD/FDD frequencies maintained in the freq list, 
if FEATURE_WTOL_TDD_CM_SUPPORT is supported */

#ifdef FEATURE_WTOL_TDD_CM_SUPPORT
#define WSRCHLTE_CM_MAX_NUM_TDD_FREQ    4
#else
#define WSRCHLTE_CM_MAX_NUM_TDD_FREQ    0
#endif
#define WSRCHLTE_CM_MAX_NUM_FDD_FREQ    4

#define WSRCHLTE_CM_MAX_NUM_FREQ       (WSRCHLTE_CM_MAX_NUM_FDD_FREQ  + WSRCHLTE_CM_MAX_NUM_TDD_FREQ)   


/* Max Number of filters for RSRP/RSRQ values (value = 3) */
#define WSRCHLTE_MAX_NUM_FILTERS  L1_MAX_FILTER_COEF_STORAGE

/* Minimum possible RSRP -144 dBm in x16 format */
#define WSRCHLTE_MIN_RSRP -2304

/* Minimum RSRQ value -19.5 dBm in x16 format */
#define WSRCHLTE_MIN_RSRQ -312

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH

/** ------------ search result logging defines ------------ */
#ifndef WSRCHLTE_W2L_SR_LOG_CODE
/* The Logging code used for W2L logging */
#define WSRCHLTE_W2L_SR_LOG_CODE (LOG_WCDMA_BASE_C + 0x24)
#else
#error "Redefinition of the log command code: WSRCHLTE_W2L_SR_LOG_CODE"  
#endif

/* used to write the search mode to the uint8 field in the log packet struct */
#define WSRCHLTE_WRITE_SR_LOG_SEARCH_MODE(field, val) \
        ( (field) = ((uint8)(field) & 0x0F) | (((uint8)(val) & 0xF)<<4) )
#define WSRCHLTE_WRITE_SR_LOG_CELL_SEARCH_TYPE(field, val) \
        ( (field) = ((uint8)(field) & 0xF0) | ((uint8)(val) & 0xF) )

/* The version of the search results log */
#define WSRCHLTE_SR_LOG_VERSION 3

/* this is the most information the log can handle */
#define W2L_SR_LOG_MAX_NUM_DET_CELLS 12
#define W2L_SR_LOG_MAX_NUM_MEAS_CELLS 28
/** ---------- end search result logging defines ---------- */


/* Timer to trigger LTE srch during CM */
#define WSRCHLTE_CM_SRCH_TIMER 144

/* TGL allowed for CM search */
#define WSRCHLTE_MAX_CM_GAP_LEN 14
#define WSRCHLTE_MIN_CM_GAP_LEN 10

/* CM gap threshold for LTE search in CHIPX 8 */
#define WSRCHLTE_CM_GAP_THRSLD_MIN (2* (CHIPX8_PER_FRAME/MSEC_PER_FRAME))
#endif  /* FEATURE_WCDMA_CM_LTE_SEARCH */

#if defined(FEATURE_WCDMA_TO_LTE_FACH) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH)
#define WSRCHLTE_ADV_LTE_MEAS_PERIOD_FRAME 0
/* Default max timer value (in W frames) for detect searches in W2L FACH */
#define WSRCHLTE_DEFAULT_MAX_TIMER_VAL               0
#endif

/*-------------------------------------------------------------------------*/
/*                      TYPE DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/
/* Search Mode types */
typedef enum
{
  /* LTE Search in Idle Mode */
  WSRCHLTE_IDLE_MODE,
  /* LTE Search in Compressed Mode */
  WSRCHLTE_COMPRESSED_MODE,
  /* LTE Search in FE-FACH */
  WSRCHLTE_FACH_MODE
} wsrchlte_srch_mode_enum_type;

/* Command Done types */
typedef enum
{
  /* Cell Detect Srch Done */
  WSRCHLTE_CELL_DETECT_DONE,
  /* Cell Meas Srch Done */
  WSRCHLTE_CELL_MEAS_DONE
} wsrchlte_cmd_enum_type;

/* LTE Search types */
typedef enum
{
  /* Connected Mode Cell AGC */
  WSRCHLTE_CELL_AGC,
  /* Connected mode Cell AGC and Meas Request for previous EARFCN in case prev freq is LTE TDD */
  WSRCHLTE_CELL_AGC_MEAS_SRCH,
  /* Idle Cell Detection Search */
  WSRCHLTE_CELL_DETECT_SRCH,
  /* Connected Mode Cell Detect & Measurement Search */
  WSRCHLTE_CELL_DETECT_AND_MEAS_SRCH,
  /* Idle & Connected Mode Cell Measurement Search */
  WSRCHLTE_CELL_MEAS_SRCH,
  /* None Search */
  WSRCHLTE_NONE_SRCH
} wsrchlte_srch_enum_type;

/* WCDMA LTE Search States */
typedef enum
{
  /* No search in progress */
  WSRCHLTE_SRCH_IDLE_STATE,
  /* IDLE Search prepare state */
  WSRCHLTE_SRCH_STARTUP_STATE,
  /* Cell Measurement sent */
  WSRCHLTE_SRCH_IDLE_SRCH_MEAS_STATE,
  /* AGC tune done ISR received from FW */
  WSRCHLTE_MDSP_AGC_TUNE_STATE,
  /* CM prepare state set when CM triggered */
  WSRCHLTE_SRCH_CM_PREPARE_STATE,
  /* CM Search sent */
  WSRCHLTE_SRCH_CM_SRCH_MEAS_STATE,
  /* CM waiting for Meas. Layer for results */
  WSRCHLTE_SRCH_CM_MEAS_EVAL_STATE,
  /* W2L FACH search/meas state */
  WSRCHLTE_FACH_SRCH_MEAS_STATE
} wsrchlte_srch_state_enum_type;

/* High Priority timer states */
typedef enum
{
  /* This is the first drx cycle in which the cell
     detect timer expired */
  WSRCHLTE_CELL_DETECT_TMR_EXPIRED_FIRST_DRX,
  /* Cell Detect timer has expired in a previous
     drx cycle then this one */
  WSRCHLTE_CELL_DETECT_TMR_EXPIRED_NEXT_DRX,
  /* Cell Detect timer has not expired */
  WSRCHLTE_CELL_DETECT_TMR_NOT_EXPIRED
} wsrchlte_high_prior_cell_detect_tmr_expiry_state_enum_type;

typedef enum
{
  /* This is the first drx cycle in which the cell
     meas timer expired */
  WSRCHLTE_G2WTA_CELL_MEAS_TMR_EXPIRED_FIRST_DRX,
  /* Cell meas timer has expired in a previous
     drx cycle then this one */
  WSRCHLTE_G2WTA_CELL_MEAS_TMR_EXPIRED_NEXT_DRX,
  /* Cell Meas timer has not expired */
  WSRCHLTE_G2WTA_CELL_MEAS_TMR_NOT_EXPIRED
} wsrchlte_g2wta_high_prior_cell_meas_tmr_expiry_state_enum_type;

typedef enum
{
  /* This is the first command in which LTE sends the
  AGC value for the first time */
  WSRCHLTE_AGC_CMD,
  /* AGC cmd for current FDD/TDD and Meas Request for the previous LTE TDD freq */
  WSRCHLTE_AGC_CMD_MEAS_REQ,
  /* Cell detect request */
  WSRCHLTE_CELL_DET_REQ,
  /* Cell detect + meas for FDD, cell detect + virtual cell info for TDD */
  WSRCHLTE_CELL_DET_MEAS_VIRTUAL_CELL_REQ,
  /* Cell detect and measurement request */
  WSRCHLTE_CELL_DET_MEAS_REQ,
  /* No. of LTE commands */
  WSRCHLTE_CM_LTE_NUM_CMD
} wsrchlte_timed_srch_meas_req_enum_type;


#ifdef FEATURE_WTOL_TDD_CM_SUPPORT
typedef enum
{
  WSRCHLTE_CELL_NOT_DETECTED,
  WSRCHLTE_CELL_DETECTED,
  WSRCHLTE_CELL_NA_PRUNING
} wsrchlte_detected_cell_enum_type;
#endif


typedef struct
{
  int16 q_rxlevmin_eutra; 
  int16 q_qualmin_eutra; 
  int8 priority;
  uint8 threshx_high; 
  uint8 threshx_low;  
  boolean high2_incl;
  uint8 threshx_high2; 
  boolean low2_incl;
  uint8 threshx_low2;  
} wsrchlte_priority_info_type;

/* Struct for Filtered RSRP/RSRQ values */
typedef struct
{
  /* spec range : -140 dBm to -44 dBm with 1 dB resolution*/
  int16 filt_rsrp;
  /* spec range : -19.5 dB to -3 with 0.5 dB resolution */
  int16 filt_rsrq;
  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  uint32 filt_cer_snr[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  #endif
} wsrchlte_filt_info_struct_type;

/* LTE cell structure */
typedef struct 
{ 
  /* The frame boundary of the LTE cell based upon the LTE RTC */
  uint64 timing_offset[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];

  uint32 sss_corr;
  /* Max RSRP comb.value combined across Tx-Rx pairs. In linear scale. */
  /* Filtered RSRP/RSRQ values */
  wsrchlte_filt_info_struct_type filt_params[WSRCHLTE_MAX_NUM_FILTERS];
  /* Cell Reselection Timestamp for this cell (neighbor) */
  srch_evt_ts_struct_type  evt_cr_ts;
  /* The physical cell id of the detected cell */
  uint16 cell_id;
  /* The earfcn for searching LTE cells on */
  uint32 earfcn;
  /* The CP type of the detected cell. */
  wsrchltedrv_cp_enum_type cp_type;
  /* The timing adjustment in the cir domain */
  uint16 timing_adjustment_cir[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];  
  /* Iinitial frequency offset estimation in Hz */
  int16 freq_offset_est;
  /* sss correlation value, used for ranking cell detection results in CM */
  int16 rsrp;
  /* Max RSRQ value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrq;
  /* Max RSSI value combined across Tx-Rx pairs. In linear scale. */
  int16 rssi;
  /* Instantaneous RSRP per antenna. Range -144dBm..-44dBm with resolution of 1dBm */
  int16 rsrp_ant[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Instantaneous RSRQ per antenna. Range -19.5dB ... -3dB */
  int16 rsrq_ant[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Instantaneous RSSI per antenna. Range -120 ... 0 */
  int16 rssi_ant[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Instant. CER SNR values used for decision in reporting cell */
  uint32 cer_snr_rx[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  #endif
  /* Cell selection values evaluated by SRCH and to be reported to RRC */  
  int16 s_rxlev;
  /* Cell selection values evaluated by SRCH and to be reported to RRC */  
  int16 s_qual;
  /* Flag indicates whether a cell is barred or not */
  boolean cell_barred;
  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Flag indicates if the cell was detected in the current gap */
  wsrchlte_detected_cell_enum_type curr_detected_cell;
  #else
  /* Flag TRUE indicates if the cell was detected in the current gap */
  boolean curr_detected_cell;
  #endif
  /* true if the samples for measuring this cell were corrupted in the last search */
  boolean last_samps_corrupted;
} wsrchlte_cell_struct_type;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/* Structure to be used for adding freqs for measurement */
typedef struct
{
  /* LTE EARFCN */
  uint32 EARFCN;
  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Flag for if its an LTE TDD freq */
  boolean is_tdd_freq;
  #endif

  /* Flag for if BW is present in MCM */
  boolean dl_bandwith_present;
  /* Bandwidth */
  wsrchltedrv_earfcn_dl_bandwidth dl_bandwidth;
  /* Number of blacklisted cells */
  uint8 num_blacklisted_cells;
  /* List of blacklisted cells, identified by cell_id */
  uint16 blacklisted_cells[WSRCHLTEDRV_MAX_CELLS_PER_FREQ];
} wsrchlte_add_freq_info_type;

/* Struct for W2L CM timer for search/meas
   sm_counter max 3 after which meas should be scheduled
   srch_timer expires, start new search/meas cycle */
typedef struct
{
  /* Search command idx */
  uint8 cm_srch_cmd_idx;
} wsrchlte_freq_timer_info_struct_type;

/* Struct for the frequency information pending add/remove */
typedef struct
{
  /* Num FDD frequencies pending to be added */
  uint8 num_fdd_add_pending;
  /* Num FDD frequencies pending to be removed */
  uint8 num_fdd_remove_pending;

  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Num TDD frequencies pending to be added */
  uint8 num_tdd_add_pending; 
  /* Num TDD frequencies pending to be removed */
  uint8 num_tdd_remove_pending;
  #endif

  /* Saved freq info to be added */
  wsrchlte_add_freq_info_type saved_add_freq_pending_list[WSRCHLTE_CM_MAX_NUM_FREQ];
  /* Frequency saved to be removed */
  uint32 saved_freq_remove_pending[WSRCHLTE_CM_MAX_NUM_FREQ];
} wsrchlte_pending_add_remove_freq_struct_type;
#endif

/* Structure to store the rf tune time */
typedef struct
{
  /* RF tune startup time */
  uint32 startup_time_cx8;
  /* RF tune cleanup time */
  uint32 cleanup_time_cx8;
} wsrchlte_rf_tune_time_info_struct_type;

/* Struct for a LTE Frequency */
typedef struct
{
  /* List of blacklisted cells, identified by cell_id */
  uint16 blacklisted_cells[WSRCHLTEDRV_MAX_CELLS_PER_FREQ];

  /* LTE EARFCN */
  uint32 EARFCN;
  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Flag for if its an LTE TDD freq */
  boolean is_tdd_freq;
  #endif
  /* Flag for if BW info present */
  boolean dl_bandwith_present;
  /* Bandwidth */
  wsrchltedrv_earfcn_dl_bandwidth dl_bandwidth;
  /* Priority information */
  wsrchlte_priority_info_type prior_info;

  /* Number of blacklisted cells */
  uint8 num_blacklisted_cells;

  /* Priority information present */
  boolean prior_info_present;

  /* Num of cells on this frequency */
  uint8 num_cells;

  #if defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH) && defined(FEATURE_WCDMA_TO_LTE_FACH)
  /* Number of cells from FACH W2L measurement*/
  uint8 num_fach_meas_cells;

  /* Timer fields for EDRX */
  /* Flag for search allowed on freq based on measurement rules and scheduling mode */
  boolean srch_allowed;

  /* Current running timer updated every FMO gap */
  uint32 active_timer;

  /* Max timer updated when FMOs are initialized or when cells detected */
  uint32 max_timer;
  #endif

  #ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  /* CM timer info per freq */
  wsrchlte_freq_timer_info_struct_type timer_info;
  /* AGC parameters */
  wsrchltedrv_agc_param_info_struct_type agc_info;
  #endif

  #ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  /* Physical Cell list per EARFCN for Compressed Mode. MAX size = 16 */
  wsrchlte_cell_struct_type meas_cell_list[WSRCHLTEDRV_MAX_CELLS_PER_FREQ]; 
  #else
  /* LTE Cell Struct */
  wsrchlte_cell_struct_type  *cell_ptr[WSRCHLTEDRV_MAX_CELLS_PER_FREQ];
  #endif

  /*Cell list to store FACH measurement results*/
#if defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH) && defined(FEATURE_WCDMA_TO_LTE_FACH)
  wsrchlte_cell_struct_type *lte_cell_ptr[WSRCHLTEDRV_MAX_CELLS_PER_FREQ];
#endif

} wsrchlte_frequency_info_type;

/* Struct with the total number of LTE frequencies */ 
typedef struct
{
  /* Number of active LTE frequencies */
  uint8 num_eutra_frequencies;
  /* Number of discarded LTE frequencies */
  uint8 num_eutra_discarded_freq;
  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Number of FDD freqs */
  uint8 num_fdd_freq;
  /* Number of TDD freqs */
  uint8 num_tdd_freq;
  #endif
  /* Frequency information */
  wsrchlte_frequency_info_type lte_frequency[WSRCHLTE_MAX_NUM_FREQ];
  wsrchlte_frequency_info_type lte_discarded_frequency[WSRCHLTE_MAX_NUM_DISCARDED_FREQ]; 
} wsrchlte_frequency_list_type;

/* Commands during LTE NSET search */
/* Local command to LTE NSET search */
typedef struct
{
  /* local command header */
  l1_local_cmd_hdr_type hdr;
  /* status of command */
  boolean status;
  /* type of srch done for command */
  wsrchlte_cmd_enum_type srch_done;
} wsrchlte_local_cmd_type;

/* Local command to LTE Driver to searcher for INIT cnf */
typedef struct
{
  /* local command header */
  l1_local_cmd_hdr_type hdr;
  /* status of command */
  boolean status;
} wsrchlte_init_cnf_done_cmd_type;


/* This struct is used to keep track of EARFCNs
   to search in a drx cycle. */
typedef struct
{
  /* Index into the array wsrchlte_freq_list.lte_frequency[] */
  uint16 idx_to_earfcn;
  /* Type of srch to be done on above earfcn */
  wsrchlte_srch_enum_type srch_type;
  /* Indicates if search is complete on this earfcn */
  boolean srch_done;
} wsrchlte_earfcn_to_srch_struct_type;

/* When UE is in High Priority Scheduling Mode, Cell Detection Search is done
   every (60 * nlayers) seconds. When this timer expires, the next 'n' consecutive
   drx cycles are used for cell detection searches with cell detection search
   on 1 earfcn per drx cycle. 'n' is the number of higher priority earfcns */

/* Struct which is the state machine for the above */
typedef struct
{
  /* This keeps track of the state of the Cell Detect timer */
  wsrchlte_high_prior_cell_detect_tmr_expiry_state_enum_type cell_detect_tmr_state;
  /* Same as the number of higher prior earfcn */
  uint16 num_drx_for_cell_detect_srch;
} wsrchlte_high_prior_state_mc_struct_type;

typedef struct
{
  wsrchlte_g2wta_high_prior_cell_meas_tmr_expiry_state_enum_type g2wta_cell_meas_tmr_state;
  uint8 g2wta_high_prio_earfcn_meas_mask;
} wsrchlte_g2wta_high_prior_state_mc_struct_type;

/* Tracks globals to help schedule LTE Searchs depending on scheduling mode */
typedef struct
{
  /* Index into the array wsrchlte_freq_list.lte_frequency[]. Points to the NEXT
     EARFCN on which Cell Detection Search will be done */
  uint8 cell_detect_earfcn_idx;
  /* Index into the array wsrchlte_freq_list.lte_frequency[]. Points to the LAST
     EARFCN on which Cell Measurement Search was done */
  uint8 cell_meas_earfcn_idx;
  /* Number of valid entries in array earfcn_to_srch[] */
  uint8 num_earfcn_to_srch;
  /* List of EARFCN to be srched in this drx cycle */
  wsrchlte_earfcn_to_srch_struct_type earfcn_to_srch[L1_MAX_EUTRA_FREQ];
  /* Index into the array earfcn_to_srch[], this index stores the earfcn index 
     on which the current search is in progress */
  uint8 saved_earfcn_to_srch_idx;
  /* State machine for Cell Detection Searches during High Prior Scheduling */
  wsrchlte_high_prior_state_mc_struct_type high_prior_state_mc;
  wsrchlte_g2wta_high_prior_state_mc_struct_type g2wta_high_prior_cell_meas_state_mc;
  uint8 pended_detect_earfcn_idx;
} wsrchlte_srch_schedule_tracker_struct_type;

/* Control structure for W2L search */
typedef struct
{
  /* Indicates that a search request came and it has started */
  boolean srch_in_progress;
  #ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  /* The CFN when the CM GAP starts */
  uint16 curr_cfn;
  /* Indicates the earfcn_idx where CM is in progress */
  uint8 earfcn_idx;
  #endif
  /* Indicates the need to send DEINIT_CNF after searches are done */
  boolean send_deinit_needed;
  /* Flag to check if AGC tune done ISR is received from MDSP */
  boolean agc_tune_done_isr_rcvd;
  /* State of W to L search */
  wsrchlte_srch_state_enum_type srch_state;
  /* srch mode */
  wsrchlte_srch_mode_enum_type srch_mode;
  /* Type of search in progress */
  wsrchlte_srch_enum_type srch_type;
  /* current earfcn to perform search */
  wsrchlte_frequency_info_type *cur_searched_earfcn;
  /* pointer to RF buffer to be used for cleanup after search */
  void *rf_buf_ptr;
} wsrchlte_ctrl_struct_type;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/* Struct for LTE Connected Mode info related to gap params and srch/meas params */
typedef struct
{
  /* Flag to check contents are updated and valid to be used in next frame's gap */
  boolean valid;
  /* Frame number for starting the gap */
  uint8 cfn;
  /* Valid EARFCN index to search/meas on */
  uint8 earfcn_idx;
  /* freq and gap info and search-meas params to be sent to LTE */
  wsrchltedrv_timed_srch_meas_info_struct_type timed_srch_meas_info;
} wsrchlte_srch_meas_param_info_struct_type;

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
  wsrchlte_cell_struct_type cm_meas_rsp_cell_list[WSRCHLTEDRV_CM_MAX_MEAS_CELLS 
	  + WSRCHLTEDRV_MAX_NEW_DETECTED_CELLS];
} wsrchlte_cm_meas_rsp_info_struct_type;

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
  } antenna_data[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];

  /* the following fields are FILTERED signal data */
  PACKED struct PACKED_POST
  {
    /* the filtered RSRP in units of dBx4 (range -140 to -44 dBm) */
    int16 filt_rsrp;
    /* the filtered RSRQ in units of dBx4 (range -140 to -44 dBm) */
    int16 filt_rsrq;
  } filtered_data;   
} wsrchlte_sr_log_cell_struct_type;

typedef PACKED struct PACKED_POST
{
  /* the call frame number when the search gap started */
  uint8 cfn;

  /* the cell frame number when the search gap started */
  uint16 start_frc;
  uint32 start_cx8;

  /* the cell frame number when the search gap ended */
  uint16 end_frc;
  uint32 end_cx8;

  /* the cell frame number when the clock turned off */
  uint16 clock_off_frc;
  uint32 clock_off_cx8;
} wsrchlte_sr_log_cm_struct_type;

/* The following structure is specialy defined for logging search results.
 * Use "WSRCHLTE_W2L_SR_LOG_CODE_type" to refer to the structure type*/
LOG_RECORD_DEFINE(WSRCHLTE_W2L_SR_LOG_CODE)

  /* the version of the log packet system */
  uint8 log_pkt_version;

  /* the search mode field containing W2L Search Mode info as well as
   * cell search type info (each gets 4 bits) and the encodings follow the 
   * enumerations:  wsrchlte_srch_mode_enum_type, wsrchlte_srch_enum_type
   *  most significant 4 bits:   0 - W2L in idle mode 
   *                             1 - W2L in compressed mode
   *  least significant 4 bits:  0 - CELL_AGC
   *                             1 - CELL_DETECT
   *  			         2 - CELL_DETECT_AND_MEAS
   *                             3 - CELL_MEAS
   */ 
  uint8 srch_mode_info;

  /* the frequency measured */
  uint32 earfcn;
  /* FDD = 0, TDD = 1 */
  uint8 is_tdd;

  /* the AGC value (valid only during agc searches) */
  int32 agc_value;

  /* number of cells that were detected in the search */
  uint8 num_detected_cells;
  /* number of cells that were measured in the search */
  uint8 num_measured_cells;

  /* number of cells being remembered on this frequency for future searches
   * (also determines the number of stored cells in the next section)
   */
  uint8 num_stored_cells;

  /* this space is used to transfer 
     1) Timing info of the gap if in CM
     2) the detected cell ID list, (2 bytes per detected cell (each is uint16))
     3) the measured cell ID list, (2 bytes per measured cell (each is uint16))
     4) the stored cell info  (1 wsrchlte_sr_log_cell_struct_type per stored cell)
     (in that order) */
  uint8 misc_storage[
        sizeof(wsrchlte_sr_log_cm_struct_type)
        + WSRCHLTEDRV_MAX_CELLS_PER_FREQ*sizeof(wsrchlte_sr_log_cell_struct_type)
        + 2*W2L_SR_LOG_MAX_NUM_DET_CELLS + 2*W2L_SR_LOG_MAX_NUM_MEAS_CELLS];

LOG_RECORD_END

#endif /* FEATURE_WCDMA_CM_LTE_SEARCH */

#ifdef FEATURE_EUTRA_DETECT
/* Support for EUTRA Detection */
typedef enum
{
  WSRCHLTE_CELL_DETECT_NOT_STARTED,
  WSRCHLTE_CELLS_DETECTED,
  WSRCHLTE_CELLS_NOT_DETECTED
}wsrchlte_det_state_enum_type;
#endif

/*-------------------------------------------------------------------------*/
/*                        GLOBAL VARIABLES                                 */
/*-------------------------------------------------------------------------*/

/* LTE cells, total of 32 */
extern wsrchlte_cell_struct_type  wsrchlte_cell_list[WSRCHLTE_NSET_MAX];
extern uint8 wsrchlte_nset_cnt;

/* LTE Search information */
extern wsrchlte_ctrl_struct_type wsrchlte_ctrl_info;

/* Tracks globals to help schedule LTE Searchs depending on scheduling mode */
extern wsrchlte_srch_schedule_tracker_struct_type wsrchlte_srch_schedule_tracker;

/* LTE Neighbor List */
extern wsrchlte_frequency_list_type wsrchlte_freq_list;

/* Table of TmeasureEUTRA values indexed by K values. K = 0..2 is not valid */
extern uint8 wsrchlte_t_meas_eutra_tab[10];

/* Table of faster TmeasureEUTRA values (when Tresel is running)  indexed by K values. K = 0..2 is not valid */
extern uint8 wsrchlte_fast_t_meas_eutra_tab[10];

/* Table for scheduling LTE Cell Detection search when All Priority Scheduling
   is being used, values indexed by K values. K = 0..2 is not valid */
extern uint8 wsrchlte_cell_detect_scheduling_tab[10];

/* Table for scheduling LTE Cell Measurement search when All Priority Scheduling
   is being used, values indexed by K values. K = 0..2 is not valid */
extern uint8 wsrchlte_cell_meas_scheduling_tab[10]; 

/* Table for faster scheduling of LTE Cell Measurement search (when Tresel is running) when All Priority Scheduling
   is being used, values indexed by K values. K = 0..2 is not valid */
extern uint8 wsrchlte_fast_cell_meas_scheduling_tab[10]; 

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/* Object to store the meas repsonse results from LTE */
extern wsrchlte_cm_meas_rsp_info_struct_type wsrchlte_cm_meas_rsp_info;
#endif

#if defined(FEATURE_WCDMA_TO_LTE_FACH) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH)
/* Object for sending LTE FE-FACH search/meas */
extern wsrchlte_srch_meas_param_info_struct_type wsrchlte_fach_srch_meas_info;

/* Gets a reference to the wsrchlte_frequency_info_type from eutra freq list*/
#define WSRCHLTE_GET_EUTRA_FREQ_PTR_BY_IDX(idx) \
  ((idx >= wsrchlte_freq_list.num_eutra_frequencies)?NULL:(&wsrchlte_freq_list.lte_frequency[idx]))

#define WSRCHLTE_GET_NUM_EUTRA_FACH_CELLS(idx) \
  ((idx >= wsrchlte_freq_list.num_eutra_frequencies) ? INT8_MIN : (wsrchlte_freq_list.lte_frequency[idx].num_fach_meas_cells))
  

#endif

/*-------------------------------------------------------------------------*/
/*                            FUNCTIONS                                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION:  WSRCHLTE_INIT

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
extern void wsrchlte_init(void);

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
extern void wsrchlte_init_on_resume(
  /* Search mode: Idle or Compressed Mode */
  wsrchlte_srch_mode_enum_type srch_mode);

/*===========================================================================

FUNCTION: WSRCHLTE_LTE_INIT_SENT

DESCRIPTION:
  This function returns a boolean indicating if LTE INIT message has been sent or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If INIT message has been sent
  FALSE - If INIT message has not been semt.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wsrchlte_lte_init_sent(void);

/*===========================================================================

FUNCTION: WSRCHLTE_GET_LTE_CELL

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
extern wsrchlte_cell_struct_type *wsrchlte_get_lte_cell(void);

/*===========================================================================
FUNCTION: WSRCHLTE_MEAS_INIT

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
extern void wsrchlte_meas_init(
  /* Search mode: Idle or Compressed Mode */
  wsrchlte_srch_mode_enum_type srch_mode);

/*===========================================================================

FUNCTION: WSRCHLTE_MEAS_CLEANUP

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
extern void wsrchlte_meas_cleanup(void);

/*===========================================================================

FUNCTION  WSRCHLTE_SECONDARY_ANTENNA_LOCKED

DESCRIPTION:
  This function  sets the wsrchlte_sec_antenna_state flag to TRUE when 
  secondary antenna is under WCDMA control before proceeding CM srch

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_secondary_antenna_locked(void);

/*===========================================================================

FUNCTION  WSRCHLTE_SECONDARY_ANTENNA_UNLOCKED

DESCRIPTION:
  This function sets the wsrchlte_sec_antenna_state flag to FALSE when 
  secondary antenna is NOT under WCDMA control

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_secondary_antenna_unlocked(void);

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================

FUNCTION  WSRCHLTE_RESET_CM_LTE_SRCH_CMD_IDX

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
void wsrchlte_reset_cm_lte_srch_cmd_idx(void);
#endif

/*===========================================================================

FUNCTION WSRCHLTE_RMV_LTE_NBR_INFO

DESCRIPTION:
   This fn is called to clear the LTE neighbor lists. This is called on a cell
   transition complete, since if not done we may be left with a stale neighbor list,
   if we don't get a new CPHY_MEASUREMENT_REQ from RRC after we transition to new cell.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_rmv_lte_nbr_info(void);

/*============================================================================
FUNCTION: wsrchlte_update_lte_freq_and_prior_info()

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
extern void wsrchlte_update_lte_freq_and_prior_info(
  /* List of frequency information to be updated */
  l1_meas_eutra_frequency_list_type *freq_list, 
  /* Pointer to the priority info struct received in 3rd MEAS_REQ */
  l1_meas_priority_info_type *prior_info);

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
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
extern boolean wsrchlte_add_lte_freq_for_meas(
  /* Frequency information to be added */
  wsrchlte_add_freq_info_type* lte_earfcn_to_add, 
  /* Flag for the last freq */
  boolean last_freq);

/*===========================================================================

FUNCTION: WSRCHLTE_REMOVE_LTE_FREQ_FOR_MEAS

DESCRIPTION: 
  This function removed the frequency from the search LTE frequency list if the 
  frequency has been removed by the MCM. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -- successfully removed/ saved to be removed 
  FALSE -- otherwise  

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wsrchlte_remove_lte_freq_for_meas(
  /* Frequency to remove */
  uint32 freq);
#endif

/*===========================================================================

FUNCTION: WSRCHLTE_CHECK_NEXT_EARFCN_INDEX_FOR_SRCH

DESCRIPTION: Scans the wsrchlte_srch_schedule_tracker.earfcn_to_srch array to get the next
  entry whose status flag indicates no search has been done on the earfcn
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 wsrchlte_check_next_earfcn_index_for_srch(
  /* LTE search type */
  wsrchlte_srch_enum_type srch_type);

/*===========================================================================

FUNCTION: WSRCHLTE_UPDATE_STATUS_FLAG_FOR_EARFCN_SRCHED

DESCRIPTION: Sets the status flag for an entry in wsrchlte_srch_schedule_tracker.earfcn_to_srch
  once a search completes on a earfcn entry

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_update_status_flag_for_earfcn_srched(
  /* Index for EARFCN */
  uint8 earfcn_idx);

/*===========================================================================
FUNCTION: WSRCHLTE_COMPUTE_LTE_MEAS_RULES_ON_S_FAIL

DESCRIPTION:
  This function is called from srchcr_update_prior_meas_rules when S fails for serv cell.
  This function adds all LTE EARFCN's to the schedule tracker, and sets the srch_type to CELL_DETECT, 
  so that it gets searched in this DRX.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If LTE is srch's this DRX.
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wsrchlte_compute_lte_meas_rules_on_s_fail(void);

/*===========================================================================

FUNCTION: WSRCHLTE_COMPUTE_LTE_MEAS_RULES

DESCRIPTION: This fn computes the measurement rules for LTE nbrs to be used in this drx
  cycle depending on scheduling mode and timer expiry status
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wsrchlte_compute_lte_meas_rules(
  /* indicates whether scheduling mode has been changed from HIGH to ALL or viceversa */
  boolean schedulng_mode_changed,
  /* Indicates expiry status of IDLE timers */
  uint16 prev_tmr_expiry_status);

void wsrchlte_get_band_info(sys_band_class_e_type *bandInfo, uint32 idle_earfcn);

extern boolean wsrchlte_send_wrm_start_meas (void);

extern boolean wsrchlte_allocate_rf_buf_and_send_wrm_start_meas (uint32 idle_earfcn, uint32 idle_dirat);

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================

FUNCTION  WSRCHLTE_VALIDATE_GAP_LENGTH_FOR_CM_SRCH

DESCRIPTION:
  This function checks if the gap length is sufficient and valid for 
  Connected Mode search and measurement

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if CM Gap length is successfully validated
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wsrchlte_validate_gap_length_for_cm_srch(
  /* Number of slots in the compressed mode GAP*/
  uint8 num_tg_slots);

/*===========================================================================

FUNCTION: WSRCHLTE_VALIDATE_EARFCN_FOR_CM_SRCH_TABLE_BASED

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
wsrchlte_srch_enum_type wsrchlte_validate_earfcn_for_cm_srch_table_based(
  /* Reference to return EARFCN index on which CM search is to be scheduled */
  uint8 *earfcn_idx);

/*===========================================================================

FUNCTION  WSRCHLTE_UPDATE_CM_GAP_INFO_FOR_LTE_SRCH

DESCRIPTION:
  This function computes and updates the CM gap params for LTE  
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wsrchlte_update_cm_gap_info_for_lte_srch( 
  /* Number of slots in the compressed mode GAP*/
  uint8 num_tg_slots,
  /* TGSN */
  uint8 num_tg_sn,
  /* CFN for which CM GAP starts */
  uint8 tg_cfn);

/*===========================================================================

FUNCTION  WSRCHLTE_UPDATE_CM_LTE_MEAS_INFO

DESCRIPTION:
  This function updates the LTE measurement parameters during CM search. 
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_update_cm_lte_meas_info(
  /* LTE frequency list */
  wsrchlte_frequency_info_type* lte_frequency, 
  /* measurement params */
  wsrchltedrv_lte_cell_info_db_struct_type* meas_params,
  /* CM search type */
  wsrchlte_srch_enum_type cm_trigger_state);
#endif 

/*===========================================================================

FUNCTION: WSRCHLTE_RESUME_SEARCH

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
extern void wsrchlte_resume_search(
  /* Message received that resulted in a call to resume search */
  wsrchltedrv_srch_msg_enum_type msg_type);

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================

FUNCTION  WSRCHLTE_TRIGGER_LTE_MEAS

DESCRIPTION:
  This function triggers an LTE search and measurement if the LTE EARFCN is
  valid, LTE init req has been sent and  gap length is sufficient to do
  the search in LTE connected mode.

DEPENDENCIES
  None.

RETURN VALUE
  Search Type (NONE/Detect/Detect+Meas/Meas)

SIDE EFFECTS
  None
===========================================================================*/
extern wsrchlte_srch_enum_type wsrchlte_trigger_lte_meas(
  /* Number of slots in the compressed mode GAP */
  uint8 num_tg_slots,
  /* slot where cm gap starts  */
  uint8 tgsn,
  /* CFN for which CM GAP starts */
  uint8 tg_cfn,
  /* earfcn_idx to search */
  uint8 *freq_idx,
  /* this function writes to this location the point in time that L gap 
     activity finishes (in terms of FRC counter) */
  tlm_tfn_struct_type *gap_cleanup_point_ptr);
#endif

/*===========================================================================

FUNCTION: WSRCHLTE_DO_LTE_SEARCH

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
extern boolean wsrchlte_do_lte_search(
  /* Search type requested */
  wsrchlte_srch_enum_type srch_type,
  /* Frequency info for the search to be performed */
  wsrchlte_frequency_info_type *earfcn_info,
  /* CFN for which the gap will start */
  uint8 gap_start_cfn,
  /* Search mode: Idle or Compressed Mode */
  wsrchlte_srch_mode_enum_type srch_mode);

/*===========================================================================

FUNCTION: WSRCHLTE_SRCH_DONE_HANDLER

DESCRIPTION: This is the search done handler called from srchcr. Depending on the type
  of search, it calls the corresponding function to process the results 
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_srch_done_handler(
  /* Srch done command: cell detect done or cell meas done */
  wsrchlte_cmd_enum_type srch_done);

/*===========================================================================

FUNCTION  WSRCHLTE_SIGNAL_SRCH_SCHEDULER

DESCRIPTION:
  This function posts a local command when a LTE INIT CNF is rcvd to signal the
  scheduler so that LTE search can be scheduled.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_signal_srch_scheduler(void);

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================

FUNCTION  WSRCHLTE_CLEANUP_SEND_LTE_DEINIT

DESCRIPTION:
  This function cleans up the W2L measurements and sends LTE Deinit req message
  in case of transition from CELL DCH state.

DEPENDENCIES
  None.

RETURN VALUE 
  None

SIDE EFFECTS
  None
===========================================================================*/

void wsrchlte_cleanup_send_lte_deinit(void);

/*===========================================================================

FUNCTION  WSRCHLTE_CM_RESULTS_PROC

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
extern void wsrchlte_cm_results_proc(void); 

/*==========================================================================

FUNCTION  WSRCHLTE_CM_MEAS_POST_PROC

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
extern void wsrchlte_cm_meas_post_proc(void);

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
extern void wsrchlte_save_sorted_cell_list(
  /* Pointer to the sorted 16-cell list from measurement layer */
  wsrchlte_cell_struct_type *sorted_cell_list,
  /* Number of cells in the sorted list */
  uint8 num_cells);
#endif

/*===========================================================================

FUNCTION  WSRCHLTE_CLEANUP_LTE_MEAS

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
extern boolean wsrchlte_cleanup_lte_meas(
  /* Post search done call back function */
  WSRCHLTEDRV_DEINIT_DONE_CB_FUNC_TYPE lte_cleanup_cb_func);

/*===========================================================================

FUNCTION  WSRCHLTE_ALLOCATE_W2L_RF_BUFS

DESCRIPTION:
  This function allocates IRAT rf buffers for the upcoming LTE search.  It will \
  also give wsrchltedrv a pointer to the buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_allocate_w2l_rf_bufs(void);

/*===========================================================================

FUNCTION  WSRCHLTE_MDSP_AGC_TUNE_DONE

DESCRIPTION:
  This function is the callback function registered for the event for WFW AGC TUNE
  DONE ISR. The cb function is de-registered from the event and search state machine
  is resumed with AGC tune done message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_mdsp_agc_tune_done(void);



#ifdef FEATURE_EUTRA_DETECT
/*===========================================================================
FUNCTION  WSRCHLTE_GET_EUTRA_DETECT_STATUS

DESCRIPTION:
  This function is called from srchmeas.c to perform the necessary functionality needed by RRC. 
  If the IE "E-UTRA detection" is included in a received message and set to TRUE and the UE is in CELL_PCH,
  URA_PCH state or idle mode, the UE may detect the presence of a E-UTRA cell on a frequency with a priority lower
  than the current UTRA cell and report the information to the NAS.

DEPENDENCIES
  None.

RETURN VALUE
  WSRCHLTE_CELL_DETECT_NOT_STARTED
  WSRCHLTE_CELLS_DETECTED
  WSRCHLTE_CELLS_NOT_DETECTED

SIDE EFFECTS
  None
===========================================================================*/
extern wsrchlte_det_state_enum_type wsrchlte_get_eutra_detect_status(void);
#endif /* FEATURE_EUTRA_DETECT */

/*===========================================================================
FUNCTION wsrchlte_act_on_cm_status_change

DESCRIPTION
  This function takes care of sending lte initializations/de-initializations 
  when lte patterns go active or deactive (and exclusive clients like CME are 
  off).

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_act_on_cm_status_change(
  /* whether CM is going active or deactive */
  l1_cm_active_deactive_enum_type cm_active);
/*===========================================================================
FUNCTION wsrchlte_lte_deinit_done

DESCRIPTION
  returns the lte client status

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
extern boolean wsrchlte_lte_client_status_cb(void);

#ifdef FEATURE_WTOL_PS_HANDOVER
/*===========================================================================

FUNCTION  WSRCHLTE_GIVE_EUTRA_SNAPSHOT_DATA

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
extern boolean wsrchlte_give_eutra_snapshot_data(lte_l1_lte_stmr_snapshot_s *snpsht_ptr);

#endif /* FEATURE_WTOL_PS_HANDOVER */


/*===========================================================================
FUNCTION wsrchlte_read_dc_w2l_nv_setting

DESCRIPTION
  This function determines the UE's ability to do DC + W2L measurements by 
  reading the relevant NV item.  If that NV does not exist, it creates it and
  initializes the value to FALSE, assuming that the UE can't do DC + W2L.
 
RETURN VALUE
   None.
SIDE EFFECTS
  Sets global flag wsrchlte_dc_w2l_enabled
=============================================================================*/
void wsrchlte_read_dc_w2l_nv_setting(void);

#if defined(FEATURE_WCDMA_TO_LTE_FACH) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH)

/*===========================================================================

FUNCTION  WSRCHLTE_UPDATE_FACH_GAP_INFO

DESCRIPTION:
  This function computes and updates the FMO gap params for LTE. It takes SCCPCH
  offset and number of FMO gap slots as input.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_update_fach_gap_info(
   /* Offset between PCCPCH and SCCPCH frame boundary */
   uint16 sccpch_offset, 
   /* Length of FMO gap in slots */
   uint16 num_gap_slots);
/*===========================================================================

FUNCTION  WSRCHLTE_UPDATE_FACH_LTE_MEAS_INFO

DESCRIPTION:
  This function updates the search and meas parameters to be sent to LTE for W2L
  searches in FMO. It also sends the virtual cell info for LTE TDD freq if exists.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_update_fach_lte_meas_info(
  /* Frequency index for measurement */
  uint8 earfcn_index,
  /* Next frequency that could be measured */
  uint8 next_freq_index
  );
/*===========================================================================

FUNCTION  WSRCHLTE_PREPARE_FACH_LTE_MEAS

DESCRIPTION:
  This function prepares for W2L searches in FMO by sending LTE init req cmd
  if not sent before and allocating RF buffers for the search.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE, if LTE INIT REQ not sent
  TRUE, otherwise

SIDE EFFECTS
  RF buffers will be allocated for search
===========================================================================*/
extern boolean wsrchlte_prepare_fach_lte_meas(void);
/*===========================================================================

FUNCTION  WSRCHLTE_FACH_SCHEDULE_LTE_MEAS

DESCRIPTION:
  This function schedules the LTE EARFCN for next FMO gap. It takes into account
  if the active timer has expired for the frequency and if search is allowed on the
  frequency based on the measurement rules.
  
DEPENDENCIES
  None.

RETURN VALUE
  EARFCN index for current scheduled freq, next freq index

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 wsrchlte_fach_schedule_lte_freq(uint8* next_freq_index);

/*===========================================================================

FUNCTION  WSRCHLTE_SET_MAX_TIMER

DESCRIPTION:
  This function updates the max timer for an LTE frequency.It will be updated to a shorter
  periodicity if a frequency has detected cells. If no cells are detected, then the timer
  will be set to 60*Nlayers periodicity.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_set_max_timer(uint32 timer_val, boolean reset_all);

#if defined(FEATURE_WCDMA_TO_LTE_FACH) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH)
/*===========================================================================

FUNCTION  WSRCHLTE_INCR_ACTIVE_TIMER

DESCRIPTION:
  This function updates the active timer on the LTE frequency if the max timer has a non-zero value.
  It is used to schedule the LTE frequencies for detection and measurements

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchlte_incr_active_timer(uint16 time_elapsed);
#endif

/*===========================================================================
FUNCTION WSRCHLTE_ALLOCATE_FACH_MEAS_CELLS

DESCRIPTION
  This function allocates the LTE Cell list used in FACH W2L measurements
 
RETURN VALUE
NONE
   
SIDE EFFECTS
NONE
=============================================================================*/

extern void wsrchlte_allocate_fach_meas_cells(void);

/*===========================================================================
FUNCTION WSRCHLTE_DEALLOCATE_FACH_MEAS_CELLS

DESCRIPTION
  This function deallocates the LTE Cell list used in FACH W2L measurements
 
RETURN VALUE
NONE
   
SIDE EFFECTS
NONE
=============================================================================*/
extern void wsrchlte_deallocate_fach_meas_cells(void);

/*===========================================================================
FUNCTION WSRCHLTE_LTE_DUE_FOR_MEAS

DESCRIPTION
  This function runs the LTE scheduling algorithm to find if any EARFCN's are due for measurements
 
RETURN VALUE
  TRUE - If there is an LTE EARFCN due for measurements
  FALSE - Otherwise

   
SIDE EFFECTS
NONE
=============================================================================*/
extern boolean wsrchlte_lte_due_for_meas(boolean scheduling_mode_change);

#endif

/*===========================================================================
FUNCTION WSRCHLTE_FIND_EUTRA_FREQ

DESCRIPTION
  This function does a lookup of EARFCN in the wsrchlte_freq_list
 
RETURN VALUE
   NULL - if the frequency is not found
   pointer to the freq_info_type - if the frequency is found
   
SIDE EFFECTS
NONE
=============================================================================*/
extern wsrchlte_frequency_info_type* wsrchlte_get_eutra_freq_info(uint32 earfcn);

/*===========================================================================
FUNCTION WSRCHLTE_GET_EUTRA_CELL

DESCRIPTION
  This function does a lookup of (EARFCN,PhyCellID) in the wsrchlte_freq_list
 
RETURN VALUE
   NULL - if not found
   pointer to the LTE cell - if found
   
SIDE EFFECTS
NONE
=============================================================================*/
extern wsrchlte_cell_struct_type* wsrchlte_get_eutra_cell(uint32 earfcn,uint16 phy_cell_id);

/*===========================================================================
FUNCTION WSRCHLTE_GET_PRIORITY_BY_IDX

DESCRIPTION
  This function returns the priority value of the LTE Freq at Idx
 
RETURN VALUE
  Invalid_Priority (-1) - if freq_idx is out of range
  priority value of Freq idx - otherwise
   
SIDE EFFECTS
NONE
=============================================================================*/
extern int8 wsrchlte_get_priority_by_idx(uint8 freq_idx);

/*===========================================================================
FUNCTION WSRCHLTE_INIT_FREQ_LIST

DESCRIPTION
  This function initializes the LTE frequency list
 
RETURN VALUE
NONE
   
SIDE EFFECTS
NONE
=============================================================================*/
extern void wsrchlte_init_freq_list(wsrchlte_frequency_list_type* freq_list_ptr);

/*===========================================================================
FUNCTION WSRCHLTE_INIT_CELL_PARMS

DESCRIPTION
  This function initializes the LTE cell parameters
 
RETURN VALUE
NONE
   
SIDE EFFECTS
NONE
=============================================================================*/
extern void wsrchlte_init_cell_parms(wsrchlte_cell_struct_type* cell_ptr);

/*===========================================================================
FUNCTION WSRCHLTE_GET_EARFCN_IDX

DESCRIPTION
  Find the idx of the earfcn provided
 
RETURN VALUE
   Index of the earfcn in the wsrchlte_freq_list
   
SIDE EFFECTS
=============================================================================*/
extern uint8 wsrchlte_get_earfcn_idx(uint32 lte_freq);

/*===========================================================================
FUNCTION WSRCHLTE_CHECK_LTE_CELL_BARRED

DESCRIPTION
  Check if a LTE cell is barred
 
RETURN VALUE
   True - If cell is barred
   FALSE - Otherwise
   
SIDE EFFECTS
=============================================================================*/
extern boolean wsrchlte_check_lte_cell_barred(uint32 freq, uint16 cell_id);


/*===========================================================================
FUNCTION WSRCHLTE_SAVE_RESEL_EARFCN_IDX

DESCRIPTION
  Store the earfcn idx of the last reselected LTE cell. This will be used to pend the lte search if
  the reselection failed due to non-interference scenarios. 
 
RETURN VALUE
   
SIDE EFFECTS
=============================================================================*/

extern void wsrchlte_save_resel_earfcn_idx(uint32 freq,uint16 cell_id);

/*============================================================================
FUNCTION: WSRCHLTE_FAST_MEAS_ON_ACTIVE_TRESEL

DESCRIPTION: This function checks if Tresel is active on any LTE frequency and updates the 
meas timer to meas timer/2 for FAST LTE meas.
      
DEPENDENCIES:
None.

RETURN VALUE:
NONE

SIDE EFFECTS:
=============================================================================*/
extern void wsrchlte_fast_meas_on_active_tresel(void);

/*===========================================================================
FUNCTION WSRCHLTE_GET_KDETECTED

DESCRIPTION
  Function to find Kdetected (number frequencies which have detected cells)
 
RETURN VALUE
  
SIDE EFFECTS
=============================================================================*/
extern uint8 wsrchlte_get_kdetected(void);
/*===========================================================================
FUNCTION wsrchlte_calc_lte_init_dealy
DESCRIPTION
  This function returns 1 if LTE_INIT_REQ was already sent otherwise 0. 
 
RETURN VALUE
  NONE
  
SIDE EFFECTS
=============================================================================*/
extern uint8 wsrchlte_calc_lte_init_delay(void);
/*===========================================================================
FUNCTION wsrchlte_query_w2l_meas_ongoing

DESCRIPTION
  This function is to determine if W2L meas is in the progress.
 
RETURN VALUE
  NONE
  
SIDE EFFECTS
=============================================================================*/
extern boolean wsrchlte_query_w2l_meas_ongoing(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wsrchlte_get_w2l_meas_info

DESCRIPTION
  This function is to collect the w2l meas info for debugging.
 
RETURN VALUE
  NONE
  
SIDE EFFECTS
=============================================================================*/
extern void wsrchlte_get_w2l_meas_info(void *debug_buf, boolean fach_meas);
#endif /* FEATURE_QSH_DUMP */
#endif /* FEATURE_WCDMA_TO_LTE */

#endif 