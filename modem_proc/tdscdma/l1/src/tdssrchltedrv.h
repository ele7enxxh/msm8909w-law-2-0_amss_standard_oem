/*===========================================================================
FILE: tdssrchltedrv.h


GENERAL DESCRIPTION
  This file contains code of TDSCDMA drivers for LTE Cell Search and Measurement.

Copyright (c) 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdssrchltedrv.h#1 $ DateTime: $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/15   zli     Implement FR 25117: T2L timing maintenance and power optimization
07/08/14    zl     Extend T2L to support 8 LTE TDD frequencies
07/10/12   zl      Optimize T2L measurement scheduling
06/20/12   zl      add T2L idle mode cell reselection
08/06/10   mm      Compiler warning fix
07/14/10   mm      Fixed compiler warning for wrong parameter name in WSRCHLTEDRV_SRCH_DONE_CB_FUNC_TYPE 
                   declaration 
06/16/10   mm/yh   Added timing_adjustment_cir in wsrchlte_cell_struct_type due to interface change in LTE.
                   Added support for RF tune back to W.
                   Added RF buffer support for LTE srch.
06/02/10   stk     Added support for W2LTE measurements and reselection under 
                   FEATURE_TDSCDMA_ABSOLUTE_PRIORITY & FEATURE_TDSCDMA_TO_LTE

===========================================================================*/

#ifndef TDSSRCHLTEDRV_H
#define TDSSRCHLTEDRV_H

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
/* LTE Message header file */
#include "lte_cphy_irat_meas_msg.h"
/* Message Router header file */
#include "msgr.h"
#include "tfw_sw_intf_msg.h"
#include "lte_ml1_irat_ext_api.h"
#include "tdsl1rrcif.h"

/*-------------------------------------------------------------------------*/
/*                        MACROS                                           */
/*-------------------------------------------------------------------------*/
/* Number of LTE RX Antennas */
#define TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS LTE_CPHY_IMEAS_NUM_RX_ANTENNAS
/* Number of cells per frequency */
#define TDSSRCHLTEDRV_MAX_CELLS_PER_FREQ  16
/* Maximum number of frequencies */
#define TDSSRCHLTEDRV_MAX_FREQS            8

/*for logging purpose */
#define TDSSSRCHLTEDRV_MAX_SENT_MSG        20
#define TDSSSRCHLTEDRV_MAX_RCVD_MSG        20
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
#define TDSSRCHLTEDRV_CM_MAX_MEAS_CELLS    LTE_CPHY_IMEAS_MAX_MEAS_CELLS_TIMED //9 
#define TDSSRCHLTEDRV_CM_MAX_NEW_DETECTED_CELLS   12
#define TDSSRCHLTEDRV_MAX_NEW_DETECTED_CELLS   TDSSRCHLTEDRV_CM_MAX_NEW_DETECTED_CELLS
#define TDSSRCHLTEDRV_NUM_CARRIERS         2

#define TDSSRCHLTEDRV_STARTUP_SCRIPT_BUFF_IDX       6
#define TDSSRCHLTEDRV_CLEANUP_SCRIPT_BUFF_IDX       7
#define TDSSRCHLTEDRV_BSP_SCRIPT_BUFF_IDX           8


#endif
/*-------------------------------------------------------------------------*/
/*                        TYPE DEFINITIONS                                 */
/*-------------------------------------------------------------------------*/
/* TDSCDMA Received Search Message types*/
typedef enum
{
  TDSSRCHLTEDRV_START_SRCH_MSG,       /* 0 */
  TDSSRCHLTEDRV_ONLINE_PROC_DONE_MSG, /* 1 */ 
  TDSSRCHLTEDRV_CELL_DET_DONE_MSG,    /* 2 */ 
  TDSSRCHLTEDRV_CELL_MEAS_DONE_MSG,   /* 3 */ 
  TDSSRCHLTEDRV_CLEANUP_DONE_MSG,     /* 4 */ 
  TDSSRCHLTEDRV_INVALID_MSG 
} tdssrchltedrv_srch_msg_enum_type;

/*for logging*/
typedef enum
{
  TDSSRCHLTEDRV_INIT_SRCH_REQ,        //0
  TDSSRCHLTEDRV_START_SRCH_REQ,       /* 1 */
  TDSSRCHLTEDRV_MEAS_REQ,               /* 2 */ 
  TDSSRCHLTEDRV_DET_REQ,               /* 3 */ 
  TDSSRCHLTEDRV_DEINIT_REQ,            //4
   TDSSRCHLTEDRV_CLEANUP_REQ ,         //5
    TDSSRCHLTEDRV_BLACKLIST_REQ,       //6
  TDSSRCHLTEDRV_INIT_SRCH_CNF,         //7
  TDSSRCHLTEDRV_START_SRCH_CNF,       /* 8 */
  TDSSRCHLTEDRV_MEAS_CNF,              /* 9 */ 
  TDSSRCHLTEDRV_DET_CNF,               /* 10 */ 
  TDSSRCHLTEDRV_DEINIT_CNF,            //11
    TDSSRCHLTEDRV_CLEANUP_CNF,         //12
  TDSSRCHLTEDRV_INVALID_SENT_MSG 
} tdssrchltedrv_sent_msg_enum_type;

typedef struct
{
    tdssrchltedrv_sent_msg_enum_type msg_type;
    msgr_hdr_struct_type             msg_content;
} tdssrchltedrv_sent_msg_struct_type;

/*Mapping enum for bandwidth from l1rrcif to tdssrchltedrv*/
typedef enum
{
  TDSSRCHLTEDRV_MBW6,
  TDSSRCHLTEDRV_MBW15,
  TDSSRCHLTEDRV_MBW25,
  TDSSRCHLTEDRV_MBW50,
  TDSSRCHLTEDRV_MBW75,
  TDSSRCHLTEDRV_MBW100, 
  TDSSRCHLTEDRV_INVALID_BANDWIDTH 
} tdssrchltedrv_earfcn_dl_bandwidth;

/* Supported cyclix prefix modes */
typedef enum
{
  /* Normal CP */
  TDSSRCHLTEDRV_CP_MODE_NORMAL = 0,
  /* Extended CP */
  TDSSRCHLTEDRV_CP_MODE_EXTENDED,
  /* Long CP for MBSFN */
  TDSSRCHLTEDRV_CP_MODE_EXTENDED_MBSFN,
  TDSSRCHLTEDRV_MAX_NUM_CP_MODES
} tdssrchltedrv_cp_enum_type;

// TDSSRCHLTEDRV States
typedef enum
{
  TDSSRCHLTEDRV_STATE_DEINIT,
  TDSSRCHLTEDRV_STATE_INIT,
  TDSSRCHLTEDRV_STATE_CELL_DET,
  TDSSRCHLTEDRV_STATE_CELL_MEAS,
  TDSSRCHLTEDRV_STATE_CELL_CLEANUP
} tdssrchltedrv_state_type;

/*struct to snapshot TDS serving cell rx time and maintained rx time drift*/
typedef struct
{
  union
  {
    /** sub frame count and chip x8 count */
    struct
    {
      uint32  ts0StartPosition : 16;
      uint32  subFrmCnt : 13;
      uint32  : 3;
    };
    /*! \brief Offset counter including both subframe and chipx8 count */
    uint32  wallTimeOfs_snapshot;
  };
  int32  curr_rx_time_drift;
} tdssrchltedrv_serving_cell_rxtime_struct_type;

/* Minimum cell struct */
typedef struct
{
  /* Flag for cell found in cell detection*/
  boolean cell_found;
  /* The physical cell id of the detected cell */
  uint16 cell_id;
  /* The CP type of the detected cell. */
  tdssrchltedrv_cp_enum_type cp_type;  
  /* The frame boundary of the LTE cell based upon the LTE RTC */ 
  uint64 timing_offset[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* The timing adjustment in the cir domain */
  uint16 timing_adjustment_cir[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Initial PSS based frequency offset estimation in Hz */
  int16 freq_offset_est;
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  uint32 sss_corr;
#endif
} tdssrchltedrv_lte_cell_struct_type;

/* Complete cell info */
typedef struct
{
  /*whether LTE srch is performed or not in LTE side*/
  boolean search_performed;
  /* Number of cells */
  uint8 num_detected_cells;
  /* Cell list */
  tdssrchltedrv_lte_cell_struct_type cell_info[TDSSRCHLTEDRV_MAX_CELLS_PER_FREQ];
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* Measurement frequency information, to be filled for every meas request
     Mainly to distinguish between meas freq and AGC freq in AGC + meas gap */
  uint32 meas_freq; 
  /* Virtual cell information if vcell_info_present is TRUE */
  tdssrchltedrv_lte_cell_struct_type vcell_info;
#endif
} tdssrchltedrv_lte_cell_info_db_struct_type; 

/* Measurement response info */
typedef struct 
{ 
  /* Flag for cell found in cell measurement */
  boolean cell_found;
  /* The physical cell id of the detected cell */
  uint16 cell_id;
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  tdssrchltedrv_cp_enum_type cp_type; 
#endif
  /* The frame boundary of the LTE cell based upon the LTE RTC */ 
  uint64 timing_offset[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* The timing adjustment in the cir domain */
  uint16 timing_adjustment_cir[LTE_CPHY_IMEAS_NUM_RX_ANTENNAS];
  /* Initial PSS based frequency offset estimation in Hz*/
  int16 freq_offset_est;
  /* RSRP comb.value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrp;
  /* Instant. RSRQ value combined across Tx-Rx pairs. In linear scale. */
  int16 rsrq;
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* Instant. RSRP values per antenna */
  int16 rsrp_ant[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Instant. RSRQ values per antenna */
  int16 rsrq_ant[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* Instant. RSSI values per antenna */
  int16 rssi_ant[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  /* The detected SSS power of the cell when it was detected. */
  uint32 sss_corr;
  /* Instant. CER SNR values per antenna */
  /*LTE FW does not support such values at present. */
//  uint32 cer_snr_rx[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
#endif
  
} tdssrchltedrv_lte_meas_struct_type;

/* Complete measurement info */
typedef struct
{
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* Frequency on which cells are measured. Will be different from freq in 
  wsrchltedrv_lte_results_db_struct_type for AGC + Meas gap. */
  uint32 meas_freq;
#endif
  /* Number of cells */
  uint8 num_cells;
  /* Cell list */
  tdssrchltedrv_lte_meas_struct_type cells[TDSSRCHLTEDRV_MAX_CELLS_PER_FREQ];
} tdssrchltedrv_lte_meas_info_db_struct_type; 

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
  /* uint16 blacklist_cells[TDSSRCHLTEDRV_MAX_CELLS_PER_FREQ]; */
} tdssrchltedrv_blacklist_struct_type;

/* Blacklist for all freqs */
typedef struct
{
  /* Number of freqs */
  uint8 num_freqs;
  /* Blacklist info */
   /* Only TDSSRCHLTE_MAX_NUM_FREQ[4] freqs supported in T2L,  TDSL1_MAX_EUTRA_FREQ[8] freqs are just used for intermediate sorting*/
  tdssrchltedrv_blacklist_struct_type blacklist_freq[TDSL1_MAX_EUTRA_FREQ];
} tdssrchltedrv_blacklist_info_struct_type;

/* Recent timing snapshot info for T and L system timer, 
   Used to recover timer for LTE when it is deinit and its timer is not running */
typedef struct
{
  /* To determine whether we have snapshot info 
     that indicates if we are capable to maintain timer */  
  boolean snapshot_present;
  /* Timing snapshot info, containing W2L stmr information */  
  lte_l1_lte_stmr_snapshot_s snapshot;
} tdssrchltedrv_snapshot_info_type;

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
#if 0
typedef struct
{
  mcaltds_cm_irat_rf_buf_ctrl_struct_type* rf_buf;
  /* Source parameters to be passed to LTE */
  tdssrchltedrv_rf_source_params_struct_type source_parameters;
} tdssrchltedrv_rf_param_struct_type;
#endif


/* Struct for RF source parameters */
typedef struct
{
  /* Source band info; RFCOM enum type */
  uint32 source_band;
  /* Channel Number for both single and dual carriers */
  uint16 channel[TDSSRCHLTEDRV_NUM_CARRIERS];
  /* Number of carriers: 0 - Single carrier 1- Dual Carrier */
  uint8 carrier_type;
} tdssrchltedrv_rf_source_params_struct_type;


/* AGC parameter info struct type */
typedef struct
{
  /* The AGC value for the frequency */
  uint32 agc_value;
} tdssrchltedrv_agc_param_info_struct_type;
#endif

/* Database struct for both cell detection and cell measurement results   
  for both Idle and CM */
typedef struct
{
  /* LTE EARFCN on which srch/meas is done. This corresponds to AGC freq for AGC + meas gap.
     Measured freq field is in wsrchltedrv_lte_meas_info_db_struct_type */
  uint32 freq;  
  #if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  /* AGC parameter info for frequency */
  tdssrchltedrv_agc_param_info_struct_type agc_info;
  boolean gap_aborted;
  #endif
  /* Cell detection results database */
  tdssrchltedrv_lte_cell_info_db_struct_type cell_detect_info;
  /* Cell measurement results database */
  tdssrchltedrv_lte_meas_info_db_struct_type cell_meas_info; 
} tdssrchltedrv_lte_results_db_struct_type;


#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/* CM gap param struct */
typedef struct
{
  /* TDSCDMA Frame number */
  uint32 frame_number;
  /* Offset in chip x8 */
  uint32 offset;
} tdssrchltedrv_cm_gap_frame_offset_struct_type;

/* CM search param struct */
typedef struct
{
  /* Flag to indicate search & meas or only meas. */
  boolean perform_search;
  /* Flag to measure new cells; TRUE only if perform_search is TRUE */
  boolean measure_newly_detected_cells;
} tdssrchltedrv_timed_srch_param_struct_type;

/* CM timed search-meas info struct. Contains all the info to be sent to LTE for search and meas */
typedef struct
{
  /* The frequency on which to tune to. range 0...65535 */
  uint32 freq; 
  /* The gap start frame number and offset for LTE search/Meas. Ta = tg_sn + UL_DL_DPCH_CHIPX8_OFFSET */
  tdssrchltedrv_cm_gap_frame_offset_struct_type gap_start_frame_offset;

  /* The gap end frame number and offset for LTE search/Meas as seen by LTE.
	 Tb = Ta + TGL1 - 2*UL_DL_DPCH_CHIPX8_OFFSET - rf_cleanup_time */
  tdssrchltedrv_cm_gap_frame_offset_struct_type gap_end_frame_offset;

  /* The frame number and offset for LTE to switch off its clocks.
  Happens after the RF tune back on source RAT 
  Tc = Tb + rf_cleanup_time + UL_DL_DPCH_CHIPX8_OFFSET */ 
  tdssrchltedrv_cm_gap_frame_offset_struct_type clock_off_frame_offset;

  /* RF parameters to be sent to LTE */
  //tdssrchltedrv_rf_param_struct_type rf_params;
  lte_cphy_irat_meas_startup_rf_params_s rf_params;


  /* TRUE if the agc info is populated and to be used */
  boolean freq_state_included;

//  #ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Virtual LTE cell is the cell with the best SSS_CORR in current frequency.
  TRUE if the vcell info is present and the next gap is used for measurement */
  boolean vcell_info_present;
//  #endif


  /* Bandwidth information */
  boolean dl_bandwith_present;

  tdssrchltedrv_earfcn_dl_bandwidth dl_bandwidth;

  /* AGC parameters */
  tdssrchltedrv_agc_param_info_struct_type agc_info;

  /* Measurement parameters */
  tdssrchltedrv_lte_cell_info_db_struct_type meas_params;

  /* Search parameters */
  tdssrchltedrv_timed_srch_param_struct_type search_params;

} tdssrchltedrv_timed_srch_meas_info_struct_type;

#endif /*end if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)*/

/* Callback function type */
typedef void (*TDSSRCHLTEDRV_SRCH_DONE_CB_FUNC_TYPE)(tdssrchltedrv_srch_msg_enum_type);

/* Callback function for DEINIT_CNF done */
typedef void (*TDSSRCHLTEDRV_DEINIT_DONE_CB_FUNC_TYPE)(void);

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/* Callback function type for LTE CM measurement */
typedef void (*TDSSRCHLTEDRV_TIMED_SRCH_MEAS_DONE_CB_FUNC_TYPE)(void);
#endif

/*-------------------------------------------------------------------------*/
/*                         Functions                                       */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_INIT

DESCRIPTION: 
  This function initializes the global variable for the TDSCDMA to LTE drivers

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchltedrv_init(void);

/*for logging*/
extern void tdssrchltedrv_save_sent_msg(tdssrchltedrv_sent_msg_enum_type, 
                                        msgr_hdr_struct_type*);

extern void tdssrchltedrv_save_rcvd_msg(tdssrchltedrv_sent_msg_enum_type, 
                                        msgr_hdr_struct_type*);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SET_SRCH_DONE_CB

DESCRIPTION
  This function sets the callback function to the srch done handler
  function.
   
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchltedrv_set_srch_done_cb(
  /* Callback function */
  TDSSRCHLTEDRV_SRCH_DONE_CB_FUNC_TYPE tdssrchlte_srch_done_handler);


#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_TIMED_SRCH_MEAS_REQ_MSG

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
extern void tdssrchltedrv_send_timed_srch_meas_req_msg(
  /* Pointer to CM search/meas info object */
  tdssrchltedrv_timed_srch_meas_info_struct_type* cm_srch_meas_info);


/*===========================================================================
FUNCTION  tdssrchltedrv_save_timed_meas_req

DESCRIPTION:
  This function stores the timed_meas_req msg to LTE.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchltedrv_save_timed_meas_req(lte_cphy_irat_meas_timed_srch_meas_req_s *p_msg);

/*===========================================================================
FUNCTION  tdssrchltedrv_save_timed_meas_rcnf

DESCRIPTION:
 This function stores the timed_meas_cnf msg fromo LTE

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void tdssrchltedrv_save_timed_meas_cnf(lte_cphy_irat_meas_timed_srch_meas_cnf_s *p_msg);


/*===========================================================================
FUNCTION  TDSSRCHLTEDRV_FILLUP_TIMED_SRCH_MEAS_REQ_MSG

DESCRIPTION:
  This function fills up the timed_meas_req structure before calling the LTE API.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchltedrv_fillup_timed_srch_meas_req_msg(
  /* Pointer to CM search/meas info object */
  tdssrchltedrv_timed_srch_meas_info_struct_type* cm_srch_meas_info);

/*===========================================================================
FUNCTION  TDSSRCHLTEDRV_SEND_RETURNED_TIMED_SRCH_MEAS_REQ_MSG

DESCRIPTION:
  This function sends the search and meas request message for Connected Mode 
  after we call the LTE API function with the returned timed_srch_meas_req structure.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchltedrv_send_returned_timed_srch_meas_req_msg(
  /* Pointer to CM search/meas info object */
  tdssrchltedrv_timed_srch_meas_info_struct_type* cm_srch_meas_info);
#endif

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_BLACKLIST_MSG

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
extern void tdssrchltedrv_send_blacklist_msg(
  /* Blacklist params */
  tdssrchltedrv_blacklist_info_struct_type *blacklist_info);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_INIT_REQ_MSG

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
extern void tdssrchltedrv_send_init_req_msg(
boolean maintain_timing
);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_DEINIT_REQ_MSG

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
extern void tdssrchltedrv_send_deinit_req_msg(void);

/*===========================================================================

FUNCTION  tdssrchltedrv_set_deinit_done_cb

DESCRIPTION
  This function sets the callback function for the DEINIT_CNF.
   
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchltedrv_set_deinit_done_cb(
  /* Callback function */
  TDSSRCHLTEDRV_DEINIT_DONE_CB_FUNC_TYPE deinit_done_cb);

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/*===========================================================================
FUNCTION  TDSRCHLTEDRV_GET_INIT_CNF_STATE

DESCRIPTION: 
  This function returns the state of tdssrchltedrv_init_cnf_done flag.

DEPENDENCIES
  None.

RETURN VALUE
  tdssrchltedrv_init_cnf_done

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchltedrv_get_init_cnf_state(void);
#endif
/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_STARTUP_CMD_MSG

DESCRIPTION
  This function sends the STARTUP_CMD message with the appropriate header
  and parameters to the LTE MSGR interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchltedrv_send_startup_cmd_msg(
  /* EARFCN to search */
  uint32 earfcn, 
  /* Bandwidth of the freq */
  tdssrchltedrv_earfcn_dl_bandwidth bandwidth);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_SEARCH_REQ_MSG

DESCRIPTION:
  This function sends the SEARCH_REQ message with the appropriate header
  to the LTE MSGR interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchltedrv_send_search_req_msg(void);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_MEAS_REQ_MSG

DESCRIPTION:
  This function sends the MEAS_REQ message with the appropriate header
  and parameters to the LTE MSGR interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchltedrv_send_meas_req_msg(
  /* Message parameters */
  tdssrchltedrv_lte_cell_info_db_struct_type* cells_list);



#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_GET_CM_RESULTS_PTR

DESCRIPTION:
  This function returns the pointer to the LTE CM cell measurement results.

DEPENDENCIES
  None.

RETURN VALUE
  tdssrchltedrv_lte_results_db_struct_type*

SIDE EFFECTS
  None
===========================================================================*/
extern tdssrchltedrv_lte_results_db_struct_type* tdssrchltedrv_get_cm_results_ptr(void);
#endif

/*===========================================================================
FUNCTION  TDSSRCHLTEDRV_SEND_FW_STATE_CFG

DESCRIPTION:
  This function sends state config command to FW 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchltedrv_send_fw_state_cfg(tfw_state_t fw_state);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_FW_T2X_CLEANUP_CMD

DESCRIPTION:
  This function sends cleanup command to FW to clean up after LTE search is done. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchltedrv_send_fw_t2x_cleanup_cmd(void);


/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SEND_CLEANUP_REQ_MSG

DESCRIPTION:
  This function sends the CLEANUP_REQ message with the appropriate header
  to the LTE MSGR interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchltedrv_send_cleanup_req_msg(void);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_TUNE_BACK

DESCRIPTION:
  This function tunes back the RF to TDSCDMA after receiving the
  LTE_CPHY_IRAT_MEAS_TDS2L_ONLINE_DONE_IND message from the LTE MSGR

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchltedrv_tune_back(uint16 src_freq, uint32 tgt_freq);  

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_GET_CELL_DET_RESULT_PTR

DESCRIPTION:
  This function returns the pointer to the cell detection results.

DEPENDENCIES
  None.

RETURN VALUE
  tdssrchltedrv_lte_cell_info_db_struct_type*  

SIDE EFFECTS
  None
===========================================================================*/
extern tdssrchltedrv_lte_cell_info_db_struct_type* tdssrchltedrv_get_cell_det_result_ptr(void);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_GET_CELL_MEAS_RESULT_PTR

DESCRIPTION:
  This function returns the pointer to the cell measurement results.

DEPENDENCIES
  None.

RETURN VALUE
  tdssrchltedrv_lte_cell_meas_db_struct_type*  

SIDE EFFECTS
  None
===========================================================================*/
extern tdssrchltedrv_lte_meas_info_db_struct_type* tdssrchltedrv_get_cell_meas_result_ptr(void);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_LTE_MSG_HANDLER

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
extern void tdssrchltedrv_lte_msg_handler(
  /* Message received */
  msgr_hdr_struct_type* hdr);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_STATE_TRANSITION

DESCRIPTION:
  This function handles all the state transitions in the LTE Srch and Meas
  State machine.
  The State transitions are:
  DE_INIT <--> INIT --> STARTUP --> CELL_DET
                 |        |            |  |
              CLEANUP     |            |  |
                 |<--- CELL_MEAS <------  |
                 |________________________|

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchltedrv_state_transition( tdssrchltedrv_state_type state);

/*===========================================================================

FUNCTION  tdssrchltedrv_restore_tfw_state_n_timing

DESCRIPTION:
  This function moves TFW from IRAT to Tracking. Afterward, it configures RF,
  and issue a REACQ command to TFW. 

DEPENDENCIES
  When this function is called, TFW should be in IRAT state and RF clean-up has
  been completed after LTE measurements. 

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchltedrv_restore_tfw_state_n_timing( void );


/*===========================================================================

FUNCTION  tdssrchltedrv_get_init_status

DESCRIPTION:
  This function returns the LTE driver init conformation received flag. 

DEPENDENCIES
  When this function is called, TFW should be in IRAT state and RF clean-up has
  been completed after LTE measurements. 

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdssrchltedrv_get_init_status( void );

#ifdef FEATURE_TDSCDMA_LTE_MEAS_TIMING_IMPROVEMENT
/*===========================================================================

FUNCTION  tdssrchltedrv_update_rx_time_drift

DESCRIPTION: 
  This function updates the rx time drift based on walltime offset snapshots. It will also update the snapshots. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchltedrv_update_rx_time_drift(uint32 serving_walltime_offset);
#endif

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_QUERY_LTE_TIMING_MAINTAINED

DESCRIPTION:
  This function will return snapshot status

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdssrchltedrv_query_lte_timing_maintained(void);


/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_GIVE_EUTRA_SNAPSHOT_DATA

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
extern boolean tdssrchltedrv_give_eutra_snapshot_data(lte_l1_lte_stmr_snapshot_s *snpsht_ptr);

/*===========================================================================

FUNCTION  TDSSRCHLTEDRV_SAVE_CM_MEAS_RESULTS

DESCRIPTION
  This function saves the results returned by LTE for cell measurement in Compressed
  Mode after receiving a CNF message from LTE. 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern void tdssrchltedrv_save_cm_meas_results(
  /* Message containing CM meas results */
  void* hdr);

/*===========================================================================
FUNCTION  TDSSRCHLTEDRV_SET_TIMED_SRCH_MEAS_CB

DESCRIPTION
  This function sets the callback function for the timed_srch_meas_cnf message
  from LTE received in Compressed Mode. It sets the callback function to
  tdssrchlte_cm_results_proc for processing the CM results.
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchltedrv_set_timed_srch_meas_cb(
  /* Callback function */
  TDSSRCHLTEDRV_TIMED_SRCH_MEAS_DONE_CB_FUNC_TYPE tdssrchlte_cm_results_handler);

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED

/*===========================================================================

FUNCTION  tdssrchltedrv_log_t2l_tfw_gap_config_update

DESCRIPTION:
  This function will log the TFW GAP configure

DEPENDENCIES
  GAP triggered

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void tdssrchltedrv_log_t2l_tfw_gap_config_update(void);

/*===========================================================================

FUNCTION  tdssrchltedrv_log_t2l_eutra_measurment_result_update

DESCRIPTION:
  This function will update measurement result per each measurement tick

DEPENDENCIES
  measurement triggered
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void tdssrchltedrv_log_t2l_eutra_measurment_result_update(lte_cphy_irat_meas_timed_srch_meas_cnf_s* cm_meas_results_ptr);

/*===========================================================================

FUNCTION  tdssrchltedrv_log_t2l_time_meas_srch_req_update

DESCRIPTION:
  This function will update t2l timed measurement srch REQ

DEPENDENCIES
  measurement triggered
  
RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void tdssrchltedrv_log_t2l_time_meas_srch_req_update(lte_cphy_irat_meas_timed_srch_meas_req_s* timed_srch_meas_req_msg_ptr);


#endif


#endif

#endif /* TDSSRCHLTEDRV_H */
#endif /* FEATURE_TDSCDMA_TO_LTE */
