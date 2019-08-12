#ifndef TDSL1CMMEAS_H
#define TDSL1CMMEAS_H
/*==========================================================================

             TDS Measurement Manager Code

DESCRIPTION
  This file contains code to process measurement control messages, manage
  compressed mode measurements required, cells to be measured, and perform
  measurement reporting when necessary.

  Copyright (c) 2002 - 2013 by QUALCOMM Technologies Incorporated.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1cmmeas.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/12   kguo    Initial Revision
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "tdscdma_variation.h"
#include "customer.h"
#include "tdsl1rrcif.h"
//#include "tdsl1m.h"
#include "tdssrchset.h"
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
#include "tdssrchltedrv.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/****************************************************************************** 
 * T2L TDD Preprocessor Directives
 *****************************************************************************/

/* The maximum number of peaks that can be returned from a Primary 
 * Synchronization Signal (PSS) or Secondary Synchronization Signal (SSS)
 * detection in a single gap.
 */
#define TDSL1CMMEAS_T2L_PSS_SSS_MAX_PEAKS 12

/* The maximum number of cells that the driver layer can report in the 
 * measurement list. The detect list can be no larger than the difference
 * of L1CMMEAS_W2L_MAX_CELLS and L1CMMEAS_W2L_MEAS_LIST_MAX_CELLS.
 */
#define TDSL1CMMEAS_T2L_MEAS_LIST_MAX_CELLS 5     //8

/* The maximum number of cells that can be contained in the union of the 
 * detection list and the measurement list. This value must be no greater
 * than L1_MAX_EUTRA_CELLS_PER_FREQUENCY.
 */
#define TDSL1CMMEAS_T2L_MAX_CELLS 9     //15 

/* The size of the cell list array. This list must be capable of 
 * simultaneously storing a complete list to be sent to the driver layer 
 * (15 cells) and one complete set of detected cells (12 cells).*/
#define TDSL1CMMEAS_T2L_CELL_LIST_SIZE \
       (TDSL1CMMEAS_T2L_MAX_CELLS + TDSL1CMMEAS_T2L_PSS_SSS_MAX_PEAKS)

/* With TDD support, the maximum number of supported EUTRA frequencies
 * is twice the number to report: 4 FDD + 4 TDD */
#define TDSL1CMMEAS_MAX_EUTRA_FREQ (2 * TDSL1_MAX_REP_EUTRA_FREQ)

#define TDSL1CMMEAS_TDD_PRUNING_MAX_MISSED_GAPS 3

/* The sliding window length used for LTE TDD cell pruning */
#define TDSL1CMMEAS_TDD_PRUNING_WINDOW_LENGTH 5

/* A byte with only the L1CMMEAS_TDD_PRUNING_WINDOW_LENGTH least significant
   bits set to be used as a mask */
#define TDSL1CMMEAS_TDD_PRUNING_HIST_BMSK (((uint8) 0xff) >> \
        (CHAR_BIT - TDSL1CMMEAS_TDD_PRUNING_WINDOW_LENGTH))

/* Minimum possible RSRP -144 dBm in x16 format */
//#define TDSSRCHLTE_MIN_RSRP -2304

/* Minimum RSRQ value -19.5 dBm in x16 format */
//#define TDSSRCHLTE_MIN_RSRQ -312

/***********************************************
For LTE filtering
**************************************************/
#define TDSL1CMMEAS_INVALID_K_VAL -1    //Invalid K value
#define TDSL1CMMEAS_LTE_MEAS_PRD_MSECS 480L    //measurement period 480ms
#define TDSL1CMMEAS_LTE_ALPHA_INDEX 0
#define TDSL1CMMEAS_LTE_BETA_INDEX 1
#define TDSL1CMMEAS_LTE_ALPHA_MULT_FACTOR_16384 16384
#define TDSL1CMMEAS_LTE_BETA_MULT_FACTOR_128 128
#define TDSL1CMMEAS_LTE_MAX_K_VALUE 19
#define TDSL1CMMEAS_LTE_VALUES_STORED_PER_K 2
#define TDSL1CMMEAS_LTE_INVALID_ALPHA -1
#define TDSL1CMMEAS_LTE_INVALID_BETA -1
#define TDSL1CMMEAS_LTE_LOG_INTERMEDIATE_MAX 62
#define TDSL1CMMEAS_LTE_SCALE_UNIT_1 1
#define TDSL1CMMEAS_LTE_LOG_FINAL_MAX 44

//per ML1, there is no MIN. value definition, they could even report -182, -162 etc, so we define as -200
#define TDSL1CMMEAS_INVALID_FILTERED_RSRP -3200 //-200dbm in x16 format
#define TDSL1CMMEAS_INVALID_FILTERED_RSRQ -480  //-30dbm in x16 format

#define TDSL1CMMEAS_LTE_FILTER_CALC_NUME(time_since_last_meas) \
            (time_since_last_meas*(int64)TDSL1CMMEAS_LTE_BETA_MULT_FACTOR_128)
#define TDSL1CMMEAS_LTE_FILTER_CALC_DENO(filt_len_param, time_since_last_meas) \
            ((time_since_last_meas*(int64)TDSL1CMMEAS_LTE_BETA_MULT_FACTOR_128)+\
            (uint32)tdsl1cmmeas_lte_filter_coef_tab[filt_len_param][TDSL1CMMEAS_LTE_BETA_INDEX]*\
            (int64)TDSL1CMMEAS_LTE_MEAS_PRD_MSECS)




/* Status indicator after attempting to insert a EUTRA frequency to the 
   measurement layer database */
typedef enum
{
  /* The frequency was successfully inserted */
  TDSL1CMMEAS_EUTRA_FREQ_ADD_STATUS_SUCCESS,

  /* The frequency list is full */  
  TDSL1CMMEAS_EUTRA_FREQ_ADD_STATUS_LIST_FULL,

  /* The bandwidth information for the cell is invalid */
  TDSL1CMMEAS_EUTRA_FREQ_ADD_STATUS_INVALID_BW,

  /* The blacklist is full so the new frequency's blacklist could not be processed */
  TDSL1CMMEAS_EUTRA_FREQ_ADD_STATUS_BLACKLIST_FULL
} tdsl1cmmeas_eutra_freq_add_status_type;

typedef struct
{
 /* spec range : -140 dBm to -44 dBm with 1 dB resolution*/
 int16 filt_rsrp;
 /* spec range : -19.5 dB to -3 with 0.5 dB resolution */
 int16 filt_rsrq;
  //LTE side does not provide such information for now. 
 //uint32 filt_cer_snr[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
}tdsl1cmmeas_filt_param_struct_type;


/* Struct for saving and storing the meas response returned from LTE, 
to be passed on to measurements layer */



/* Lte cell structure */
typedef struct
{ 
  /* The cell detection history, represented as a bitmask. This field is used
     for 2-out-of-5 cell pruning. The least significant bit holds the most
     recent value. If the bit is a 1, the cell was detected in that gap. If
     it is 0, then it wasn't. The cell will not be pruned as long as at least 
     2 of the last 5 bits are high. */
  uint8 detect_hist;

  /* The number of times the cell has been measured since first detected. This
     value is used to determine the CER_SNR threshold when pruning spurious
     cells. */
  uint8 num_meas;

  /* The frame boundary of the LTE cell based upon the LTE RTC */
  uint64 timing_offset[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];

  /* Secondary synchronization signal correlation, used during LTE TDD cell detection*/
  uint32 sss_corr;

  /* The CP type of the detected cell. */
  tdssrchltedrv_cp_enum_type cp_type;

  /* The timing adjustment in the cir domain */
  uint16 timing_adjustment_cir[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];  

  /* Iinitial frequency offset estimation in Hz */
  int16 freq_offset_est;

  /* Instantaneous RSSI per antenna. Range -120 ... 0 */
  int16 rssi_ant[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  
  /* True if the cell was detected in the current gap, false otherwise */
  boolean curr_detected_cell;

  /* Frequency on which the search/meas was done */
  uint32 earfcn;  
  
  /*time stamp when cell was last searched */
  uint16 ts_last_srch;

  /* physical layer cell indentity of a cell */
  uint16 phy_cell_id; 
  
  /* cell age, this will keep on incrementing if cell did not show up in detection stage search */
  uint16 cell_age;
  
  /* filtered rsrp/rsrq values */
  tdsl1cmmeas_filt_param_struct_type filt_params[TDSL1_MAX_FILTER_COEF_STORAGE];

  /* event database for cell */
  tdssrchset_evt_ts_struct_type eutra_event_ts[TDSL1_MAX_EUTRA_MEAS_EVENT];  
  
  /* Flag to indicate the validity of the results */
  boolean valid; 
  
}tdsl1cmsrch_lte_cell_struct_type;

typedef struct
{

  /*LTE cells array */
  tdsl1cmsrch_lte_cell_struct_type lte_cell[TDSL1_MAX_EUTRA_CELLS_PER_FREQUENCY];

  /* EARFCN of LTE frequency */
  uint32 earfcn;  
  /* blacklisted cells physical layer id */
  uint16 bl_cell_id[TDSL1_MAX_EUTRA_CELLS_PER_FREQUENCY];

  /* Total number of cells for this frequency. In TDD, 
    (num_cells - num_meas_cells = num_detect_cells) */
  uint8 num_cells;

  /* Number of measured cells for this frequency
    (num_cells - num_meas_cells = num_detect_cells) */
  uint8 num_meas_cells;

  /* number of blacklisted cells*/
  uint8 num_bl_cells;

}tdsl1cmsrch_freq_db_struct_type;

typedef struct 
{
  /* each frequecy database */
  tdsl1cmsrch_freq_db_struct_type freq_info[TDSL1CMMEAS_MAX_EUTRA_FREQ];
  /* Number of LTE FDD frequencies to be searched */
  uint8 num_fdd_freq;
  /* Number of LTE TDD frequencies to be searched */
  uint8 num_tdd_freq;
}tdsl1cmsrch_freq_list_struct_type;

typedef struct
{
  uint32 evt_idx;
} tdsl1_interrat_periodic_meas_struct_type;

extern tdsl1cmsrch_freq_list_struct_type  tdsl1cmmeas_eutra_freq_db;  
extern boolean tdssrchcm_eutra_inter_rat_search_done_status;
extern uint8 tdsl1cmmeas_num_eutra_meas_in_prog;

#define TDS_L1CMMEAS_GET_FILT_RSRP(cell_ptr,fc_index)   (cell_ptr->filt_params[fc_index].filt_rsrp)
#define TDS_L1CMMEAS_GET_FILT_RSRQ(cell_ptr,fc_index)   (cell_ptr->filt_params[fc_index].filt_rsrq)

#define TDS_L1CMMEAS_LTE_CELLS_MEM_SIZE   (sizeof(tdsl1cmsrch_lte_cell_struct_type)*TDSL1_MAX_EUTRA_CELLS_PER_FREQUENCY)     

typedef struct
{
   uint8 num_cells;
   uint16 phy_cell_id[TDSL1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY];
}tdssrchlte_eutra_meas_rpt_cells;

typedef struct
{
 /* spec range : -140 dBm to -44 dBm with 1 dB resolution*/
 int16 filt_rsrp;
 /* spec range : -19.5 dB to -3 with 0.5 dB resolution */
 int16 filt_rsrq;

//#ifdef FEATURE_WTOL_TDD_CM_SUPPORT
//  uint32 filt_cer_snr[TDSSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
//#endif /* FEATURE_WTOL_TDD_CM_SUPPORT */
}tdssrchlte_filt_param_struct_type;

typedef enum
{
  TDS_EUTRA_TTT_NOT_SET,
  TDS_EUTRA_TTT_SET,
  TDS_EUTRA_TTT_WAITING_TO_REPORT,
  TDS_EUTRA_TTT_REPORTED
} tdssrchlte_cell_ttt_state_enum_type; 

/* Enum type for evaluating 3a event criteria on TDS frequency */
typedef enum
{
  TDS_EVENT_3A_RESET_ON_USED_FREQ,
  TDS_EVENT_3A_SET_ON_USED_FREQ,  
  TDS_EVENT_3A_NO_CHANGE
}tdssrchdch_event3a_crit_enum_type;

/*
 * Measurement Structure and Tables
 */
  
typedef struct
{
  uint16                         meas_id;
    /* measurement identity */

  uint16  periodic_rpt_cnt;
    /* Periodic Timer Counter, for when termination expected */
  rex_timer_type * periodic_timer;
    /* Periodic Timer, for timer registration */

  /* indicates transition into condition where UMTS has fallen below threshold */
  boolean event3a_tds_qual_below_thresh; 

  tdsl1_meas_report_mode_enum_type meas_evt_type;
    /* reporting criteria, periodic or event and params */
  tdsl1_per_rpt_crit_struct_type          periodic_crit;
  tdsl1_inter_sys_event_crit_struct_type  event_crit;

  tdsl1_meas_report_trans_mode_enum_type  rpt_trans_mode;

  /*
   * Measurement Quantity Information
   */
  /* filter_coefficient parameters */
  uint32 fc_index;
    /* filter coefficient storage index */
  boolean bsic_verify_reqd;
    /* just track whether this event configured bsic_verification_required */
  tdsl1_intra_freq_meas_quan_struct_type intraf_meas_quan;
    /* intra-frequency measurement quantity for use in event 3A UTRAN meas */
    /* filter_coef is unused/unsupported  */

  struct
  {
    tdssrchlte_cell_ttt_state_enum_type state;
    uint16  time;
  } ttt[TDSL1_MAX_CELL_MEAS];
    /* time to trigger for each cell, set for this event */

  /*
   * Event 3d identifies a change in the best cell
   *  This variable is the cellid of the current best cell
   */
  tdssrchset_gsm_cell_struct_type * event3d_best_cell_ptr;  //replace srch_gsm_cell_struct_type

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined (FEATURE_TDSCDMA_DMO_SUPPORT)
   //tdssrchlte_cm_lte_cell_struct_type  *event3d_eutra_best_cell_ptr;
   // The current best_cell_id. 
   uint16 event3d_eutra_best_cell_id; 
   //The index of the current LTE frequency which holds the current best cell. 
   uint8  event3d_eutra_best_freq_idx;
   //The best cell pointer. 
   tdsl1cmsrch_lte_cell_struct_type *event3d_eutra_best_cell_ptr;
   tdsl1_eutra_meas_rpt_quantity_enum_type eutra_meas_quan;
   tdssrchlte_eutra_meas_rpt_cells    rpt_cell_db;
#endif
  /*
   * Msg Report Management Information
   */
  uint16 max_num_rpt_cells;
    /* if provided will indicate 1..12 max number of cells to report */

  boolean reporting_cell_status_included;
  /* If reporting cell status not included do not report measured cells */ 

  tdsl1_inter_sys_dch_rpt_quan_struct_type        rpt_quantity;
    /* reporting quantity, include time diff & GSM RSSI? */

  boolean stopped;                                                                                                           
  /* old MCM should be stopped after HO to new cell*/                                                                        
} tdssrchlte_inter_rat_event_info_struct_type;  

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
extern tdssrchlte_inter_rat_event_info_struct_type  tdsl1_eutra_inter_rat_event_array[TDSL1_MAX_EUTRA_MEAS_EVENT];
#endif

/******************************************************************************
*******************************************************************************
              INIT Functions
*******************************************************************************
******************************************************************************/

/*=========================================================================

FUNCTION     : TDSL1CMMEAS_INIT

DESCRIPTION  : This function will Initialize the L1CMMEAS subsystem

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/
void tdsl1cmmeas_init ( void );


/******************************************************************************
*******************************************************************************
              Compressed Mode Gap Configuration
*******************************************************************************
******************************************************************************/



/******************************************************************************
*******************************************************************************
              L1M Entry Points To Compressed Mode Measurement Control
*******************************************************************************
******************************************************************************/
/*===========================================================================
FUNCTION     TDSL1CMSRCH_MSMT_CTRL_MSG_PROCESSING

DESCRIPTION
  Handle Measurement Control Message Processing

DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the
  message

SIDE EFFECTS
  None
===========================================================================*/
//boolean tdsl1cmsrch_msmt_ctrl_msg_processing( tdsl1_meas_cmd_type *meas_cmd );


/******************************************************************************
*******************************************************************************
        Compressed Mode TGPS Registration & Run-Time Error Reporting
*******************************************************************************
******************************************************************************/

/*===========================================================================
FUNCTION     L1CMSRCH_EUTRA_PROCESS_PERIODIC

DESCRIPTION
  This function performs periodic event processing at the time the
  periodic timer expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 void tdsl1cmsrch_process_eutra_periodic_cb( uint32  event_idx );

/*===========================================================================
FUNCTION     TDSL1CMMEAS_CHECK_IF_FILT_NOT_USED
DESCRIPTION
  This function will check if filter index is using by some other measurement or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1cmmeas_check_if_filt_not_used(
 uint8 filt_idx,
 uint8 meas_id
 );

/*===========================================================================
FUNCTION     L1CMMEAS_RESET_EUTRA_FREQ_DB

DESCRIPTION
  This function will reset the frequency database at measurement side.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tdsl1cmmeas_reset_eutra_freq_db(void);

/*=============================================================================
   FUNCTION 
     TDSL1CMMEAS_CALC_RSRP_THRESH
   
   DESCRIPTION
     Find the RSRP threshold, based on the EARFCN of the cell being evaluated
     and the threshold specified by the network. The network threshold is
     expected to be in x16 dB units, since the measurement layer stores
     RSRP values internally in those units.

   DEPENDENCIES
     None
   
   RETURN VALUE
     The RSRP threshold to use if the given EARFCN was present in the database
     and had at least one cell, noise floor otherwise.
   
   SIDE EFFECTS
     None
 ============================================================================*/
int16 tdsl1cmmeas_calc_rsrp_thresh(uint32 earfcn, int16 nw_thresh); 
/*===========================================================================
FUNCTION     TDSL1CMSRCH_REMOVE_INTER_RAT_MEASUREMENTS

DESCRIPTION
  This function will remove the measurement specified, from the
  inter-rat measurement list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrchlte_remove_interrat_measurements( uint32 meas_id, tdssrchlte_inter_rat_event_info_struct_type  *l1_inter_rat_events_db);

/*===========================================================================
FUNCTION     tdsl1cmsrchlte_remove_idle_interval_information_from_measurement_release

DESCRIPTION
  This function will remove the measurement specified, from the
  inter-rat measurement list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tdsl1cmsrchlte_remove_idle_interval_information_from_measurement_release(uint32 meas_id);


/*===========================================================================
FUNCTION     tdsl1cmsrchlte_remove_all_idle_interval_information_from_voice_call_setup

DESCRIPTION
  This function will remove all the idle interval when a cs call setup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrchlte_remove_all_idle_interval_information_from_voice_call_setup(void);


/*===========================================================================
FUNCTION     TDSL1CMSRCH_REMOVE_INTERRAT_MEASUREMENT_EVENT

DESCRIPTION
  This function will (re-)initialize the event specified, from the
  inter-rat measurement list, thereby performing a remove operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrch_remove_interrat_measurement_event( uint32 event_idx ,                  
                  tdssrchlte_inter_rat_event_info_struct_type *l1_inter_rat_events_db);
/*===========================================================================
FUNCTION     TDSL1CMMEAS_RESET_CELL_DB
DESCRIPTION
  This function will reset the cell database in a frequency.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmmeas_reset_cell_db(uint8 freq_idx);

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/*===========================================================================
FUNCTION  tdsl1cmmeas_valid_eutra_meas_evt

DESCRIPTION
  This function is to see of we still have valid eutra evt in the database
  
DEPENDENCIES
  None.

RETURN VALUE
  -True: still valid event, T2L meas should continue
  -False:No meas event anymore.

SIDE EFFECTS
  None.
===========================================================================*/
boolean tdsl1cmmeas_valid_eutra_meas_evt(void);

/*===========================================================================
FUNCTION     TDSL1CMSRCH_MSMT_CTRL_MSG_EUTRA_INTER_RAT_PROCESSING

DESCRIPTION
  Handle Measurement Control Message EUTRA Inter-RAT Related Processing
  as specified in section 8.4.1.3 of 25.331

DEPENDENCIES
  None

RETURN VALUE
  FALSE if measurement control message processing identified errors in the message

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1cmsrch_msmt_ctrl_msg_eutra_inter_rat_processing( tdsl1_meas_cmd_type *meas_cmd );

/*===========================================================================
FUNCTION     tdssrchlte_set_ttt_array_state

DESCRIPTION
  This function will run through the TTT ARRAY and set [].state element as
  requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchlte_set_ttt_array_state( uint32 event_idx, tdssrchlte_cell_ttt_state_enum_type ttt_state );

/*===========================================================================
  FUNCTION 
    TDSL1CMMEAS_GET_ACTIVE_FILT_IDX
  
  DESCRIPTION
    This function determines the first active filter index. The index can be
    used to access the elements of l1cmsrch_eutra_interrat_filter_coef. If
    provided, filt_cnt will be set to the number of inactive filters that
    were encountered before finding the active filter.
   
  DEPENDENCIES
    None
  
  RETURN VALUE
    The index of the first active filter, or 0 (default) if no filters are
    active.
  
  SIDE EFFECTS
    None
 ===========================================================================*/
uint8 tdsl1cmmeas_get_active_filt_idx(void);



/*===========================================================================
FUNCTION     TDSL1CMSRCH_EUTRA_RESET_TTT_ARRAY_STATE

DESCRIPTION
  This function will reset cell measurement data for event_idx

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrch_eutra_reset_ttt_array_state( uint8 event_idx );

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
/*======================================================================
FUNCTION tdssrchmeas_log_inter_sys_eutra_meas_config_update

DESCRIPTION
    This function will log the eutra measurement control information configured by network. 
     
RETURN VALUE
    void

=======================================================================*/
extern void	tdsl1cmmeas_log_inter_sys_eutra_meas_config_update(tdsl1_meas_ctrl_parm_struct_type *meas_ctrl_ptr);

/*======================================================================
FUNCTION tdsl1cmmeas_log_t2l_eutra_cell_accumulate_meas_result_update

DESCRIPTION
    This function will log the eutra cell accumulate meas result. 
     
RETURN VALUE
    void

=======================================================================*/
extern void tdsl1cmmeas_log_t2l_eutra_measurement_accumulate_result_pkt_type_update(void);

/*======================================================================
FUNCTION tdsl1cmmeas_log_t2l_eutra_cell_accumulate_meas_result_update

DESCRIPTION
    This function will log the eutra cell accumulate meas result. 
     
RETURN VALUE
    void

=======================================================================*/
extern void tdsl1cmmeas_log_t2l_eutra_measurement_accumulate_result_pkt_type_update(void);

#endif


#endif /* FEATURE_TDSCDMA_DMO_SUPPORT */
/*===========================================================================
FUNCTION     L1CMSRCH_REMOVE_MEAS

DESCRIPTION
  This function will remove measurement database for a system.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrch_remove_meas(uint32 meas_id,tdsl1_rat_type_enum_type rat);

/*===========================================================================
FUNCTION     TDSL1CMSRCH_ADD_EUTRA_FREQ_INFO

DESCRIPTION
  This function will add eutra freuency in frequency info list and will 
  update the added frequency in the CM drivers to do search on that freq.

DEPENDENCIES
  None

RETURN VALUE
  Status of the insertion attempt

SIDE EFFECTS
  None
===========================================================================*/
tdsl1cmmeas_eutra_freq_add_status_type tdsl1cmsrch_add_eutra_freq_info(tdsl1_meas_eutra_frequency_list_type*
                                                                 add_freq_list);

/*===========================================================================
FUNCTION     TDSL1CMSRCH_PROCESS_EUTRA_EVENT3B

DESCRIPTION
  This function will perform event 3b processing when measurements
  have been completed.

  Event 3b: The estimated quality of the other system is below a certain threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrch_process_eutra_event3b( uint32 event_idx ,uint8 freq_idx);


/*===========================================================================
FUNCTION     TDSL1CMSRCH_PROCESS_EUTRA_EVENT3C

DESCRIPTION
  This function will perform event 3c processing when measurements
  have been completed.

  Event 3c: The estimated quality of the other system is above a certain threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrch_process_eutra_event3c( uint32 event_idx,uint8 freq_idx );

/*===========================================================================
FUNCTION     TDSL1CMSRCH_PROCESS_EUTRA_EVENT3A

DESCRIPTION
  This function will perform event 3a processing when measurements
  have been completed.

  Event 3a: The estimated quality of the currently used UTRAN frequency is
            below a certain threshold *and* the estimated quality of the
            other system is above a certain threshold

DEPENDENCIES
  l1 intra-freq interface for determining estimated quality of currently used
  utran

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrch_process_eutra_event3a( uint32 event_idx ,uint8 freq_idx);


/*===========================================================================
FUNCTION     TDSL1CMSRCH_PROCESS_EUTRA_EVENT3D

DESCRIPTION
  This function will perform event 3d processing when measurements
  have been completed.

  Event 3d: Change of best cell in other system

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrch_process_eutra_event3d( uint32 event_idx,uint8 freq_idx );

/*===========================================================================
FUNCTION     TDSL1CMMEAS_EUTRA_EVENT_REPORT_EVALUATION

DESCRIPTION
  This function performs processing of all active events to determine
  whether reporting is required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 void tdsl1cmmeas_eutra_event_report_evaluation( uint8 freq_idx );


/*===========================================================================
FUNCTION     TDSL1CMMEAS_INIT_CELL_PARAMS

DESCRIPTION
  Initialize filter paramters for a LTE cell.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmmeas_init_cell_params(  
  uint16 cell_idx 
);
/*===========================================================================
FUNCTION     TDSL1CMMEAS_PROCESS_EUTRA_SRCH_DONE_SIGNAL

DESCRIPTION
  Handle LTE searh done signal and process event evaluation.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmmeas_process_eutra_srch_done_signal(void);

/*===========================================================================
FUNCTION     TDSL1CMMEAS_SHOULD_SEND_LTE_MEAS_RPT

DESCRIPTION
  Determines if the LTE measurement with the EARFCN specified by the given
  freq_indx should be reported to RRC.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if the measurement should be reported, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1cmmeas_should_send_lte_meas_rpt(uint16 freq_indx);

/*===========================================================================
FUNCTION     TDSL1CMMEAS_FILTER_MEAS_QUANT

DESCRIPTION
  Filtering for LTE measurement quantities
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmmeas_filter_meas_quant(   
   uint16 cell_idx,
   uint16 ts_last_srch);

/*===========================================================================
FUNCTION     TDSL1CMSRCH_EUTRA_PROCESS_PERIODIC

DESCRIPTION
  This function performs periodic event processing at the time the
  periodic timer expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1cmsrch_send_eutra_periodic_msmt_rpt_msg( uint32 event_idx, 
                                                         boolean last_periodic_rpt );

/*===========================================================================
FUNCTION     TDSL1CMSRCH_EUTRA_PROCESS_PERIODIC_RPTS

DESCRIPTION
  This function performs periodic event processing at the time the
  periodic timer expires.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
 void tdsl1cmsrch_eutra_process_periodic_rpts( 
                                          uint32 event_idx);


/******************************************************************************
*******************************************************************************
**                  TL1 - RRC Interface Function(s)
*******************************************************************************
******************************************************************************/

/*=============================================================================
  FUNCTION     : TDSL1_GET_EUTRA_MEASUREMENT_DATA
 
  DESCRIPTION  : This function will accept an LTE IRAT measurement data
                 structure pointer.  It will fill in the structure with
                 all relevant cell measurement information.

                 This function is used by RRC to retrieve timing information
                 for known LTE cells when performing Hard Handover to LTE.
                 This timing information is given to RRC for use in LTE
                 acquisition.

  DEPENDENCIES : None

  RETURN VALUE : None

  SIDE EFFECTS : None
=============================================================================*/
extern void tdsl1_get_eutra_measurement_data(lte_irat_timing_tx_info_s* p_msmt_data);

/*=========================================================================
FUNCTION     : tdsl1cmmeas_filter_eng 

DESCRIPTION  : This function filters the LTE measurement results. It is modified from W function 
srch_interf_filter_eng();

The formula for calculating alpha_prime is:
FOR THE CASE:last_meas_gap <= SRCHCM_INTERF_MEAS_PRD_MSECS
alpha_prime=1/(1+(beta*(SRCHCM_INTERF_MEAS_PRD_MSECS/last_meas_gap)
FOR THE CASE:last_meas_gap > SRCHCM_INTERF_MEAS_PRD_MSECS
alpha_prime=1/2 - 
(1/2)*max(0, numerator_prime/denominator_prime)+
 (1/2)*(1/(1+(beta*(SRCHCM_INTERF_MEAS_PRD_MSECS/last_meas_gap))
where 
numerator_prime=1-((floor(last_meas_gap, SRCHCM_INTERF_MEAS_PRD_MSECS))*alpha)
denominator_prime=1/(1+(beta*((last_meas_gap%SRCHCM_INTERF_MEAS_PRD_MSECS)/last_meas_gap)))

After calculating alpha_prime, it is applied in the following formula:
filtered_energy=((1-alpha_prime)*previous_filtered_energy) + (alpha_prime*current_measured_energy)

DEPENDENCIES : The function expects last_meas_gap in milliseconds. Spec 25.331
describes the possible values k can take in the range 0 to 19. This fucntion
does not check for the validity of k (the caller has to check). For invalid k-values,
the function may act weird because the alpha-beta table holds invalid values for
invalid cases.

RETURN VALUE : None
      
SIDE EFFECTS : None
=========================================================================*/
void tdsl1cmmeas_filter_eng(uint32 *filt_eng, uint32 last_meas_eng, 
	uint32 last_meas_gap, int8 filt_len, uint32 srch_period);


#endif //T2L PSHO. 
#endif /* TDSL1CMMEAS_H */


