/*===========================================================================
FILE: wsrchltedrv.h


GENERAL DESCRIPTION
  This file contains code of WCDMA drivers for LTE Cell Search and Measurement.

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wsrchltedrv.h#1 $ DateTime: $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/16   nga     Changing the EARFCN declaration from uint16 to uint32
11/19/14   gm      To implement changes to inform TRM if div is needed for measurement.
08/23/14   gv      Featurization to support W compiled out
06/26/14   mm      W2L DSDA blanking support
06/24/14   mk      Support for changing EARFCN variable type from uint16 to uint32.
04/02/14   mm      3C IRAT/CM support
01/22/14   jk      Added support for CER SNR pruning for reducing spurious Event 3A-C reports for LTE TDD cells. 
12/11/13   mm      3C CM/IRAT support
12/09/13   ar      Remove the assignment to WSRCHLTEDRV_INVALID_BANDWIDTH in enum type
11/15/13   mm      LTE search freq scheduler and timer changes for W2L in FACH
11/05/13   wz      Idle search LTE uses Timed Srch API
10/30/13   ks      Fix KW issue and have #define's match up with ML1 
10/15/13   mm      Initial FE-FACH check-in, W2L in FACH support
09/30/13   cc      DBDC IRAT support check in
09/12/13   jd      Searcher redesign
08/29/13   mm      Callback for sending LTE DEINIT REQ if LTE init was sent
01/17/13   mm      Added MDSP_AGC_DONE msg type
01/09/13   dp      Changes to allow sending w2l cm gap-end time to WFW
11/27/12   dp      Fix KW issues
09/27/12   cc      Changed a shared macro name to prevent the confusion between idle and CM
09/19/12   mm      Interface for sending blacklist msg to LTE after CM re-activation
09/12/12   cc      Increase the array size ensuring that we could meas upto 16 cells, but will only loop 12 times for detected cells 
08/13/12   mm      Bandwidth information interface for W2L CM
08/10/12   cc      Remove mdsp agc done signal after w2l idle
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/03/12   mm      W2L TDD CM support
06/21/12   cc      Added a global array to store the header id for W2L message router
05/18/12   cc      Added a local struct to save LTE timing snapshot info
04/11/12   cc      Delted 2 unused fields, RSRQ and RSSI
03/30/12   dp      Pass cell's sss correlation back to LTE SW
03/19/12   gsk     Support for searching 4 TDD frequencies and max upto 8 (FDD+TDD) LTE frequencies in Idle mode.
02/29/11   dp      Added code for passing W2L PSHO snapshot data
11/10/11   yh      Fix double allocation of RF buf and passing incorrect idx to WFW and RF
11/09/11   dp      W2L idle searches adjust timing based on W cell idle timing slew
09/23/11   dp      Changed the way W2L cell reporting to measurements layer works
09/21/11   mm      Added MDSP_AGC_DONE msg type
05/26/11   dp      Added code so W2X_IDLE_CFG gets cleanup buf idx
03/11/11   mr      Added new scheduling algorithm and interface changes for W2L connected mode
02/25/11   mr      Added interface changes for W2L connected mode
12/01/10   dp      Added RF "Script" tuning functionality for idle w2l
10/15/10   mm/mr   Added support for W2L Compressed mode
08/23/10   uk      changed algorithm for sending LTE INIT/Deint
08/19/10   stk     Added default value for LTE Bandwidth 
08/06/10   mm      Compiler warning fix
07/14/10   mm      Fixed compiler warning for wrong parameter name in WSRCHLTEDRV_SRCH_DONE_CB_FUNC_TYPE 
                   declaration 
06/16/10   mm/yh   Added timing_adjustment_cir in wsrchlte_cell_struct_type due to interface change in LTE.
                   Added support for RF tune back to W.
                   Added RF buffer support for LTE srch.
06/02/10   stk     Added support for W2LTE measurements and reselection under 
                   FEATURE_WCDMA_ABSOLUTE_PRIORITY & FEATURE_WCDMA_TO_LTE

===========================================================================*/

#ifndef WSRCHLTEDRV_H
#define WSRCHLTEDRV_H

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_WCDMA_TO_LTE
#include "lte_ml1_irat_ext_api.h"
/* LTE Message header file */
#include "lte_cphy_irat_meas_msg.h"
/* Message Router header file */
#include "msgr.h"
#include "mcalwcdma_cm.h"
#include "lte_irat_types.h"
#include "tlm.h"
#include "wl1rfmif.h"

/*-------------------------------------------------------------------------*/
/*                        MACROS                                           */
/*-------------------------------------------------------------------------*/
/* Number of LTE RX Antennas */
#define WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS LTE_CPHY_IMEAS_NUM_RX_ANTENNAS
/* the maximum number of cells we can measure per idle mode LTE meas request */
#define WSRCHLTEDRV_IDLE_MAX_MEAS_CELLS LTE_CPHY_IMEAS_MAX_MEAS_CELLS
/* the maximum number of cells we can measure per CM mode LTE meas request  */
#define WSRCHLTEDRV_CM_MAX_MEAS_CELLS LTE_CPHY_IMEAS_MAX_MEAS_CELLS_TIMED
/* Maximum number of cells we can store per frequency.  
   Should be >= IDLE_MAX_MEAS_CELLS and CM_MAX_MEAS_CELLS */
#define WSRCHLTEDRV_MAX_CELLS_PER_FREQ  MAX(WSRCHLTEDRV_IDLE_MAX_MEAS_CELLS, WSRCHLTEDRV_CM_MAX_MEAS_CELLS)
/* Maximum number of frequencies */
#define WSRCHLTEDRV_MAX_FREQS           LTE_CPHY_IMEAS_MAX_BLACKLIST_FREQS
/* Max newly detected cells in CM search */
#define WSRCHLTEDRV_MAX_NEW_DETECTED_CELLS   LTE_CPHY_IMEAS_MAX_DETECTED_CELLS
/* Number of Carriers for LTE -- Single/Dual Carrier/3 Carrier */
#define WSRCHLTEDRV_NUM_CARRIERS 3
/* Maximum number of the MSGR logs */
#define WSRCHLTEDRV_MSG_LOG_MAX 20

/* LTE can not open requested gap */
#define WSRCHLTEDRV_LTE_GAP_SCHEDULE_FAIL 0xffffffff
/*-------------------------------------------------------------------------*/
/*                        TYPE DEFINITIONS                                 */
/*-------------------------------------------------------------------------*/
/* WCDMA Received Search Message types */
typedef enum
{
  WSRCHLTEDRV_START_SRCH_MSG,        
  WSRCHLTEDRV_CELL_SRCH_MEAS_DONE_MSG,        
  WSRCHLTEDRV_MDSP_AGC_DONE_MSG,
  WSRCHLTEDRV_INVALID_MSG 
} wsrchltedrv_srch_msg_enum_type;

/* Mapping enum for bandwidth from l1rrcif to wsrchltedrv */
typedef enum
{
  WSRCHLTEDRV_MBW6,
  WSRCHLTEDRV_MBW15,
  WSRCHLTEDRV_MBW25,
  WSRCHLTEDRV_MBW50,
  WSRCHLTEDRV_MBW75,
  WSRCHLTEDRV_MBW100,
  WSRCHLTEDRV_INVALID_BANDWIDTH
} wsrchltedrv_earfcn_dl_bandwidth;

/* Supported cyclix prefix modes */ 
typedef enum
{
  /* Normal CP */
  WSRCHLTEDRV_CP_MODE_NORMAL = 0,
  /* Extended CP */ 
  WSRCHLTEDRV_CP_MODE_EXTENDED,
  /* Long CP for MBSFN */
  WSRCHLTEDRV_CP_MODE_EXTENDED_MBSFN,
  WSRCHLTEDRV_MAX_NUM_CP_MODES
} wsrchltedrv_cp_enum_type;

/* Pointer to RF buffer allocated for LTE search */
typedef struct
{
  mcalwcdma_cm_irat_rf_buf_ctrl_struct_type* rf_buf;
} wsrchltedrv_startup_cmd_param_struct_type;
 
/* Minimum cell struct */
typedef struct
{
  /* Flag for cell found in cell detection */
  boolean cell_found;
  /* The physical cell id of the detected cell */
  uint16 cell_id;
  /* The CP type of the detected cell. */
  wsrchltedrv_cp_enum_type cp_type;  
  /* The frame boundary of the LTE cell based upon the LTE RTC */ 
  uint64 timing_offset[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* The timing adjustment in the cir domain */
  uint16 timing_adjustment_cir[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Initial PSS based frequency offset estimation in Hz */
  int16 freq_offset_est;
  /* sss correlation value */
  uint32 sss_corr;
} wsrchltedrv_lte_cell_struct_type;

/* Complete cell info */
typedef struct
{
  /* Flag for if search done in gap */
  boolean search_performed;
  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Measurement frequency information, to be filled for every meas request
     Mainly to distinguish between meas freq and AGC freq in AGC + meas gap */
  uint32 meas_freq;
  /* Virtual cell information if vcell_info_present is TRUE */
  wsrchltedrv_lte_cell_struct_type vcell_info;
  #endif
  /* Number of cells */
  uint8 num_detected_cells;
  /* Cell list */
  wsrchltedrv_lte_cell_struct_type cell_info[WSRCHLTEDRV_MAX_CELLS_PER_FREQ];
} wsrchltedrv_lte_cell_info_db_struct_type; 

/* Measurement response info */
typedef struct 
{ 
  /* TRUE if the samples were interfered with (could be due to DSDA) */
  boolean samples_corrupted;
  /* Flag for cell found in cell measurement */
  boolean cell_found;
  /* The physical cell id of the detected cell */ 
  uint16 cell_id;
  /* The CP type of the detected cell. */
  wsrchltedrv_cp_enum_type cp_type; 
  /* The frame boundary of the LTE cell based upon the LTE RTC  */
  uint64 timing_offset[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* The timing adjustment in the cir domain */
  uint16 timing_adjustment_cir[LTE_CPHY_IMEAS_NUM_RX_ANTENNAS];
  /* Initial PSS based frequency offset estimation in Hz */
  int16 freq_offset_est;
  /* RSRP comb.value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrp;
  /* RSRQ value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrq;
  /* Instant. RSRP values per antenna */
  int16 rsrp_ant[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Instant. RSRQ values per antenna */
  int16 rsrq_ant[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Instant. RSSI values per antenna */
  int16 rssi_ant[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Instant. CER SNR values per antenna */
  uint32 cer_snr_rx[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  #endif
  /* The detected SSS power of the cell when it was detected. */
  uint32 sss_corr;
} wsrchltedrv_lte_meas_struct_type;

/* Complete measurement info */
typedef struct
{
  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Frequency on which cells are measured. Will be different from freq in 
  wsrchltedrv_lte_results_db_struct_type for AGC + Meas gap. */
  uint32 meas_freq;
  #endif
  /* Number of cells */
  uint8 num_cells;
  /* Cell list */
  wsrchltedrv_lte_meas_struct_type cells[WSRCHLTEDRV_MAX_CELLS_PER_FREQ];
} wsrchltedrv_lte_meas_info_db_struct_type; 

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/* AGC parameter info struct type */
typedef struct
{
  /* The AGC value for the frequency */
  int32 agc_value;
} wsrchltedrv_agc_param_info_struct_type;
#endif

/* Database struct for both cell detection and cell measurement results   
  for both Idle and CM */
typedef struct
{
  /* LTE EARFCN on which srch/meas is done. This corresponds to AGC freq for AGC + meas gap.
     Measured freq field is in wsrchltedrv_lte_meas_info_db_struct_type */
  uint32 freq;
  #ifdef FEATURE_WCDMA_CM_LTE_SEARCH
  /* AGC parameter info for frequency */
  wsrchltedrv_agc_param_info_struct_type agc_info;
  #endif
  /* Cell detection results database */
  wsrchltedrv_lte_cell_info_db_struct_type cell_detect_info;
  /* Cell measurement results database */
  wsrchltedrv_lte_meas_info_db_struct_type cell_meas_info; 
  /* Boolean to indicate if the gap is aborted */
  boolean gap_aborted;
} wsrchltedrv_lte_results_db_struct_type;

/* Blacklist per freq */
typedef struct
{
  /* EARFCN */
  uint32 earfcn;
  /* Large bitmask identifying which cells should be barred by l1
   	 See lte_cphy_irat_meas_blacklist_cells_s for more details
  */
  uint8 blacklist_cell_array_mask[CELL_BLACKLIST_ARRAY_MASK_SIZE ];
  /* Number of cells */
  /*  uint8 num_cells;*/
  /* Cell IDs in blacklist */
  /* uint16 blacklist_cells[WSRCHLTEDRV_MAX_CELLS_PER_FREQ]; */
} wsrchltedrv_blacklist_struct_type;

/* Blacklist for all freqs */
typedef struct
{
  /* Number of freqs */
  uint8 num_freqs;
  /* Blacklist info */
  wsrchltedrv_blacklist_struct_type blacklist_freq[WSRCHLTEDRV_MAX_FREQS];
} wsrchltedrv_blacklist_info_struct_type;


/* RF params for W2L idle and CM search/meas */
typedef struct
{
  uint8 num_source_item;
  mcalwcdma_cm_irat_rf_buf_ctrl_struct_type* rf_buf;
  #ifdef FEATURE_WCDMA
  /* Source parameters to be passed to LTE */
  wl1_rf_source_params_struct_type source_parameters[WL1_RF_HEADER_DB_NUM_SOURCE];
  #endif
} wsrchltedrv_rf_param_struct_type;

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/* CM search param struct */
typedef struct
{
  /* Flag to indicate search & meas or only meas. */
  boolean perform_search;
  /* Flag to measure new cells; TRUE only if perform_search is TRUE */
  boolean measure_newly_detected_cells;
} wsrchltedrv_timed_srch_param_struct_type;

/* CM timed search-meas info struct. Contains all the info to be sent to LTE for search and meas */
typedef struct
{
  /* The frequency on which to tune to. range 0...65535 */
  uint32 freq;

  #if defined(FEATURE_WCDMA_TO_LTE_FACH) && defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH)
  /* Next frequency information for gap scheduling */
  uint16 next_freq;
  #endif

  /* The gap start frame number and offset for LTE search/Meas. (in FRC time) */
  tlm_tfn_struct_type gap_start_frame_offset;

  /* The gap end frame number and offset for LTE search/Meas as seen by LTE (in FRC time) */
  tlm_tfn_struct_type gap_end_frame_offset;

  /* The frame number and offset for LTE to switch off its clocks.
  Happens after the RF tune back on source RAT (in FRC time) */ 
  tlm_tfn_struct_type clock_off_frame_offset;

  /* RF parameters to be sent to LTE */
  wsrchltedrv_rf_param_struct_type rf_params;

  /* TRUE if the agc info is populated and to be used */
  boolean freq_state_included;

  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Virtual LTE cell is the cell with the best SSS_CORR in current frequency.
  TRUE if the vcell info is present and the next gap is used for measurement */
  boolean vcell_info_present;
  #endif
  
  /* Bandwidth information */
  boolean dl_bandwith_present;

  wsrchltedrv_earfcn_dl_bandwidth dl_bandwidth;

  /* AGC parameters */
  wsrchltedrv_agc_param_info_struct_type agc_info;

  /* Measurement parameters */
  wsrchltedrv_lte_cell_info_db_struct_type meas_params;

  /* Search parameters */
  wsrchltedrv_timed_srch_param_struct_type search_params;

  /* The accumulated timing drift from the source RAT stmr, between
     the last STARTUP_REQ and this STARTUP_REQ.
     Units are in LTE RTC/OSTMR ticks, 30720 ticks/ ms which
     is equivalent to WCDMA cx8.
     used by idle srch only. */
  int32 timing_adjustment;

  /* Gap will start as soon as possible. gap_start field will
     be ignored but gap_end should remain valid.
     used by idle srch only */
  boolean asap_gap_start;

  /* if use default schedule hint AUTO when calling LTE API
     to schedule gap. used by idle srch only */
  boolean use_auto_schedule_hint;
} wsrchltedrv_timed_srch_meas_info_struct_type;
#endif

/* Recent timing snapshot info for W and L system timer, 
   Used to recover timer for LTE when it is deinit and its timer is not running */
typedef struct
{
  /* To determine whether we have snapshot info 
     that indicates if we are capable to maintain timer */  
  boolean snapshot_present;
  /* Timing snapshot info, containing W2L stmr information */  
  lte_l1_lte_stmr_snapshot_s snapshot;
} wsrchltedrv_snapshot_info_type;

/* Callback function for signalling srch scheduler to begin srch */
typedef void (*WSRCHLTEDRV_SIGNAL_SRCH_SCHED_CB_FUNC_TYPE)(void);

/* Callback function for DEINIT_CNF done */
typedef void (*WSRCHLTEDRV_DEINIT_DONE_CB_FUNC_TYPE)(void);

/* Callback function for sending a blacklist message after an INIT_CNF */
typedef void (*WSRCHLTEDRV_INIT_CNF_CB_FUNC_TYPE) (void);

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/* Callback function type for LTE CM measurement */
typedef void (*WSRCHLTEDRV_TIMED_SRCH_MEAS_DONE_CB_FUNC_TYPE)(void);
#endif

/* Callback for checking if DEINIT REQ came after INIT was sent */
typedef void (*WSRCHLTEDRV_CHECK_FOR_DEINIT_CB_FUNC_TYPE)(void);

/* WSRCHLTEDRV msg log structure type */
typedef struct 
{
  msgr_umid_type id;
  uint8 cfn;
} wsrchltedrv_msg_struct_type;

/* Message array and its index */
typedef struct 
{
  uint8 idx;
  wsrchltedrv_msg_struct_type array[WSRCHLTEDRV_MSG_LOG_MAX]; 
} wsrchltedrv_msg_log_struct_type;
/*-------------------------------------------------------------------------*/
/*                         Externed Globals                                */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                         Functions                                       */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION  WSRCHLTEDRV_INIT

DESCRIPTION: 
  This function initializes the global variable for the WCDMA to LTE drivers

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_init(void);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_GET_INIT_CNF_STATE

DESCRIPTION: 
  This function returns the state of wsrchltedrv_init_cnf_done flag.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if wsrchltedrv_init_cnf_done is TRUE
  FALSE, otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wrsrchltedrv_get_init_cnf_state(void);

/*===========================================================================
FUNCTION  WSRCHLTEDRV_SET_TIMED_SRCH_MEAS_CB

DESCRIPTION
  This function sets the callback function for the timed_srch_meas_cnf message
  from LTE received in Compressed Mode. It sets the callback function to
  wsrchlte_cm_results_proc for processing the CM results.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_set_timed_srch_meas_cb(
  /* Callback function to handle the CM results */
  WSRCHLTEDRV_TIMED_SRCH_MEAS_DONE_CB_FUNC_TYPE wsrchlte_cm_results_handler);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_SET_INIT_CNF_CB

DESCRIPTION
  This function sets the callback function for Blacklist message to be sent to
  LTE in Compressed Mode after an INIT_CNF is received. It sets the callback function to
  wsrchlte_send_blacklist_msg_after_init_cnf. It is used to send the blacklist in case of 
  LTE DEINIT followed by INIT without new MCM information. 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_set_init_cnf_cb(
  /* Callback function installed for sending blacklist message */
  WSRCHLTEDRV_INIT_CNF_CB_FUNC_TYPE wsrchlte_init_cnf_handler);

/*===========================================================================
FUNCTION  wsrchltedrv_set_signal_srch_sched_cb

DESCRIPTION
  This function sets the callback function to signal the srch scheduler to start LTE
  search when LTE init cnf is received.
   
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_set_signal_srch_sched_cb(
  /* Callback function */
  WSRCHLTEDRV_SIGNAL_SRCH_SCHED_CB_FUNC_TYPE wsrchlte_signal_srch_sched);

/*===========================================================================

FUNCTION  wsrchltedrv_set_deinit_done_cb

DESCRIPTION
  This function sets the callback function for the DEINIT_CNF.
   
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_set_deinit_done_cb(
  /* Callback function */
  WSRCHLTEDRV_DEINIT_DONE_CB_FUNC_TYPE deinit_done_cb);

/*===========================================================================

FUNCTION  wsrchltedrv_set_send_deinit_cb

DESCRIPTION
  This function sets the callback function to send DEINIT_REQ.
   
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_set_send_deinit_cb(
  /* Callback function */
  WSRCHLTEDRV_CHECK_FOR_DEINIT_CB_FUNC_TYPE send_deinit_cb);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_SEND_BLACKLIST_MSG

DESCRIPTION 
  This function sends the BLACKLIST_REQ message with the appropriate header
  and parameters to the LTE MSGR interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_send_blacklist_msg(
  /* Blacklist params */
  wsrchltedrv_blacklist_info_struct_type *blacklist_info);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_SEND_INIT_REQ_MSG

DESCRIPTION:
  This function sends the INIT_REQ message with the appropriate header
  and parameters to the LTE MSGR interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_send_init_req_msg( 
  /* maintain timing field for init_req msg*/
  boolean maintain_timing);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_SEND_DEINIT_REQ_MSG

DESCRIPTION:
  This function sends the DEINT message with the appropriate header
  and parameters to the LTE MSGR interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_send_deinit_req_msg(void);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_SEND_TIMED_SRCH_MEAS_REQ_MSG

DESCRIPTION:
  This function sends the search and meas request message for Connected Mode 
  with the appropriate header to the LTE MSGR interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_send_timed_srch_meas_req_msg(
  /* Pointer to CM search/meas info object */
  wsrchltedrv_timed_srch_meas_info_struct_type* cm_srch_meas_info,
  /* sending req for FACH, extensible for CM later */
  boolean is_connected_meas);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_GET_CELL_DET_RESULT_PTR

DESCRIPTION:
  This function returns the pointer to the cell detection results.

DEPENDENCIES
  None.

RETURN VALUE
  wsrchltedrv_lte_cell_info_db_struct_type*  

SIDE EFFECTS
  None
===========================================================================*/
extern wsrchltedrv_lte_cell_info_db_struct_type* wsrchltedrv_get_cell_det_result_ptr(void);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_GET_CELL_MEAS_RESULT_PTR

DESCRIPTION:
  This function returns the pointer to the cell measurement results.

DEPENDENCIES
  None.

RETURN VALUE
  wsrchltedrv_lte_cell_meas_db_struct_type*  

SIDE EFFECTS
  None
===========================================================================*/
extern wsrchltedrv_lte_meas_info_db_struct_type* wsrchltedrv_get_cell_meas_result_ptr(void);

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
/*===========================================================================

FUNCTION  WSRCHLTEDRV_GET_CM_RESULTS_PTR

DESCRIPTION:
  This function returns the pointer to the LTE CM cell measurement results.

DEPENDENCIES
  None.

RETURN VALUE
  wsrchltedrv_lte_results_db_struct_type*

SIDE EFFECTS
  None
===========================================================================*/
extern wsrchltedrv_lte_results_db_struct_type* wsrchltedrv_get_cm_results_ptr(void);
#endif

/*===========================================================================

FUNCTION  WSRCHLTEDRV_LTE_MSG_HANDLER

DESCRIPTION:
  This function handles all the messages received from LTE. It will
  extract the results if needed and call the callback function,
  informing that a message was received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_lte_msg_handler(
  /* Message received */
  msgr_hdr_struct_type* hdr);

#ifdef FEATURE_WTOL_PS_HANDOVER
/*===========================================================================

FUNCTION  WSRCHLTEDRV_GIVE_EUTRA_SNAPSHOT_DATA

DESCRIPTION:
  This function will memcpy the w/l timing snapshot data to whatever location 
  is passed in.

DEPENDENCIES
  Caller must lock the wsrchlte mutex so this function can safely read globals


RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wsrchltedrv_give_eutra_snapshot_data(lte_l1_lte_stmr_snapshot_s *snpsht_ptr);

#endif /* FEATURE_WTOL_PS_HANDOVER */
/*===========================================================================

FUNCTION  WSRCHLTEDRV_QUERY_LTE_TIMING_MAINTAINED

DESCRIPTION:
  This function will return snapshot status

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None
===========================================================================*/
boolean wsrchltedrv_query_lte_timing_maintained(void);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_LTE_GAP_INIT

DESCRIPTION:
  This function initializes the gap for LTE FACH searches.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wsrchltedrv_lte_gap_init(void);

/*===========================================================================

FUNCTION  WSRCHLTEDRV_SCHEDULE_LTE_MEAS_GAP

DESCRIPTION:
  This function schedules the gap for LTE FACH searches. It populates the gap scheduling
  info and the last gap configured info including the time stamp to be sent back
  to LTE in the next gap. It also takes next freq as one of the args for AGC + Meas
  gap.

DEPENDENCIES
  None.

RETURN VALUE
  WSRCHLTEDRV_LTE_GAP_SCHEDULE_FAIL, if gap configure fails
  otherwise, gap successfully configured

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 wsrchltedrv_schedule_lte_meas_gap(
  /* Pointer to CM search/meas info object */
  wsrchltedrv_timed_srch_meas_info_struct_type* timed_srch_meas_info,
  /* Next freq to be searched. It can be used for configuring and making decisions 
     on gap type AGC + Meas. If there is single freq, next freq is
     to be set equal to the freq mentioned in the "search_meas_req" above. */
  uint32 next_freq,
  /* sending req for FACH, extensible for CM later */
  boolean is_connected_meas);
  
/*===========================================================================

FUNCTION  WSRCHLTEDRV_QUERY_LTE_SINGLE_PRX_MEAS_CAP

DESCRIPTION:
  This function will return the single Primary measurement capability from LTE.
  Calling this function when diversity chain is not usable for W2L search.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wsrchltedrv_query_lte_single_prx_meas_cap(void);

#endif /* FEATURE_WCDMA_TO_LTE */
#endif /* WSRCHLTEDRV_H */

