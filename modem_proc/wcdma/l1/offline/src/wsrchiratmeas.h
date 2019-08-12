#ifndef WSRCHIRATMEAS_H
#define WSRCHIRATMEAS_H


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     --------------------------------------------------------
06/30/14    jk      Add macro 'L1CMMEAS_W2L_MEAS_LIST_MAX_CELLS_TO_DRV[5]' to gate the measured cells sent to driver layer,
                    use macro 'L1CMMEAS_W2L_MEAS_LIST_MAX_CELLS_TDD[8]' to gate before sorting. 
06/24/14    mk      Support for changing EARFCN variable type from uint16 to uint32.
01/23/14    jk      Fixed the macro that was causing zeroed out values to be sent to driver 
                    layer resulting in crash.
01/22/14    jk      Added support for CER SNR pruning for reducing spurious Event 
                    3A-C reports for LTE TDD cells. 
10/28/13    jk      Created the file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "customer.h"
#include "l1rrcif.h"
#include "wl1m.h"

#include "srchcmmeas.h"

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
#include "wsrchlte.h"
#endif


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                       			MACRO                        		   */
/*                                                                         */
/*-------------------------------------------------------------------------*/



/* Inter-Frequency Search Complete Indicator */
#define SRCHCM_INTERF_SEARCH_DONE()      ( srchcm_interf_search_done_status )
#define SRCHCM_INTERF_SEARCH_CLEAR()     { srchcm_interf_search_done_status = FALSE; }

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH

#define WSRCHIRATMEAS_GET_LTE_SRCH_RESULT_PTR() (&wsrchlte_cm_meas_rsp_info)

#define WSRCHIRATMEAS_SEND_SORTED_CELLS_TO_DRV(cell_list,num_cells)          \
                  wsrchlte_save_sorted_cell_list(cell_list,num_cells)


#ifdef FEATURE_WTOL_TDD_CM_SUPPORT
#define WSRCHIRATMEAS_EUTRA_MAX_NUM_MEAS_CELLS(earfcn) (LTE_CELL_IS_TDD(earfcn) ? \
                                      WSRCHIRATMEAS_W2L_MEAS_LIST_MAX_CELLS_TDD : \
                                      WSRCHIRATMEAS_EUTRA_MAX_NUM_MEAS_CELLS_FDD)
#else
#define WSRCHIRATMEAS_EUTRA_MAX_NUM_MEAS_CELLS(earfcn) WSRCHIRATMEAS_EUTRA_MAX_NUM_MEAS_CELLS_FDD
#endif /* FEATURE_WTOL_TDD_CM_SUPPORT */


#if defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH) && defined(FEATURE_WCDMA_TO_LTE_FACH)
#define WSRCHIRATMEAS_FACH_EUTRA_MEAS_DB() (&wsrchiratmeas_eutra_freq_db)
#endif

#ifdef FEATURE_WTOL_TDD_CM_SUPPORT

/****************************************************************************** 
 * W2L TDD Preprocessor Directives
 *****************************************************************************/

/* The maximum number of peaks that can be returned from a Primary 
 * Synchronization Signal (PSS) or Secondary Synchronization Signal (SSS)
 * detection in a single gap.
 */
#define WSRCHIRATMEAS_W2L_PSS_SSS_MAX_PEAKS 12

/* The maximum number of cells that the driver layer can report in the 
 * measurement list. The detect list can be no larger than the difference
 * of WSRCHIRATMEAS_W2L_MAX_CELLS and WSRCHIRATMEAS_W2L_MEAS_LIST_MAX_CELLS_TDD.
 */
#define WSRCHIRATMEAS_W2L_MEAS_LIST_MAX_CELLS_TDD 8

/* The maximum number of measured cells that the measurement layer can
 * report back to the driver layer to schedule the next search
 */
#define WSRCHIRATMEAS_W2L_MEAS_LIST_MAX_CELLS_TO_DRV 5

/* The maximum number of cells that can be contained in the union of the 
 * detection list and the measurement list. This value must be no greater
 * than L1_MAX_EUTRA_CELLS_PER_FREQUENCY.
 */
#define WSRCHIRATMEAS_W2L_MAX_CELLS 9
/* With TDD support, the maximum number of supported EUTRA frequencies
 * is twice the number to report: 4 FDD + 4 TDD */
#define WSRCHIRATMEAS_MAX_EUTRA_FREQ (2 * L1_MAX_REP_EUTRA_FREQ)
/* The size of the cell list array. This list must be capable of 
 * simultaneously storing a complete list to be sent to the driver layer 
 * (9 cells) and one complete set of detected cells (12 cells).*/
#define WSRCHIRATMEAS_W2L_CELL_LIST_SIZE \
       (WSRCHIRATMEAS_W2L_MAX_CELLS + WSRCHIRATMEAS_W2L_PSS_SSS_MAX_PEAKS)
#else
#define WSRCHIRATMEAS_W2L_CELL_LIST_SIZE 9
#define WSRCHIRATMEAS_MAX_EUTRA_FREQ     L1_MAX_REP_EUTRA_FREQ
#endif /* FEATURE_WTOL_TDD_CM_SUPPORT */

#endif /*FEATURE_WCDMA_CM_LTE_SEARCH*/

/* RATs that can be searched */
/*
typedef enum
{
  WL1_RAT_WCDMA,
  WL1_RAT_GSM,
  WL1_RAT_LTE,
  WL1_RAT_MAX
} wsrchirat_rat_enum_type;

*/
/* IRAT search done callback functions. One callback for each RAT, for each L1 state */
//LOCAL wsrchirat_srch_evt_struct_type wsrch_irat_srch_done_handler[WSRCH_IRAT_RAT_MAX][L1M_NUM_STATES];


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                           Data Declarations                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/


#ifdef FEATURE_WCDMA_CM_LTE_SEARCH

/* Lte cell structure */
/* Merging wsrchlte_cell_struct_type and this one will increase the 
    memory by ~ 10kB (assuming 8 freq's and 16 cells per freq). So, we
    we will maintain seperate structures for CM and Idle*/
typedef struct
{ 

#ifdef FEATURE_WTOL_TDD_CM_SUPPORT


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
  uint64 timing_offset[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];

  /* Secondary synchronization signal correlation, used during LTE TDD cell detection*/
  uint32 sss_corr;

  /* The CP type of the detected cell. */
  wsrchltedrv_cp_enum_type cp_type;

  /* The timing adjustment in the cir domain */
  uint16 timing_adjustment_cir[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];  

  /* Iinitial frequency offset estimation in Hz */
  int16 freq_offset_est;

  /* Instantaneous RSSI per antenna. Range -120 ... 0 */
  int16 rssi_ant[WSRCHLTEDRV_NUM_LTE_RX_ANTENNAS];
  
  /* True if the cell was detected in the current gap, false otherwise */
  boolean curr_detected_cell;
  

#endif /* FEATURE_WTOL_TDD_CM_SUPPORT */

  /* Frequency on which the search/meas was done */
  uint32 earfcn;
  
  /*time stamp when cell was last searched */
  uint16 ts_last_srch;

  /* physical layer cell indentity of a cell */
  uint16 phy_cell_id; 
  
  /* cell age, this will keep on incrementing if cell did not show up in detection stage search */
  uint16 cell_age;
  
  /* filtered rsrp/rsrq values */
  wsrchlte_filt_info_struct_type filt_params[L1_MAX_FILTER_COEF_STORAGE];

  /* event database for cell */
  srch_evt_ts_struct_type eutra_event_ts[L1_MAX_EUTRA_MEAS_EVENT];  
  
  /* Flag to indicate the validity of the results */
  boolean valid; 
  
}wsrchiratmeas_lte_cell_struct_type;

typedef struct
{

  /*LTE cells array */
  wsrchiratmeas_lte_cell_struct_type lte_cell[L1_MAX_EUTRA_CELLS_PER_FREQUENCY];

  /* EARFCN of LTE frequency */
  uint32 earfcn;

  /* Downlink bandwidth */
  l1_eutra_meas_bandwidth_type dl_bandwidth;

  /* blacklisted cells physical layer id */
  uint16 bl_cell_id[L1_MAX_EUTRA_CELLS_PER_FREQUENCY];

  /* Total number of cells for this frequency. In TDD, 
    (num_cells - num_meas_cells = num_detect_cells) */
  uint8 num_cells;

#ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Number of measured cells for this frequency
    (num_cells - num_meas_cells = num_detect_cells) */
  uint8 num_meas_cells;
#endif /* FEATURE_WTOL_TDD_CM_SUPPORT */

  /* number of blacklisted cells*/
  uint8 num_bl_cells;

}wsrchiratmeas_freq_db_struct_type;

typedef struct 
{
  /* each frequecy database */
  wsrchiratmeas_freq_db_struct_type freq_info[WSRCHIRATMEAS_MAX_EUTRA_FREQ];
  /* Number of LTE FDD frequencies to be searched */
  uint8 num_fdd_freq;

#ifdef FEATURE_WTOL_TDD_CM_SUPPORT
  /* Number of LTE TDD frequencies to be searched */
  uint8 num_tdd_freq;
#endif /* FEATURE_WTOL_TDD_CM_SUPPORT */

}wsrchiratmeas_freq_list_struct_type;


#endif /*FEATURE_WCDMA_CM_LTE_SEARCH*/


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Variable Declarations                           */
/*                             (Externized)                                  */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_WCDMA_CM_LTE_SEARCH
extern l1_common_filter_coef_enum_type  wsrchiratmeas_eutra_interrat_filter_coef[L1_MAX_FILTER_COEF_STORAGE];

extern wsrchiratmeas_freq_list_struct_type  wsrchiratmeas_eutra_freq_db;  

extern wsrchiratmeas_lte_cell_struct_type wsrchiratmeas_sorted_lte_cell[WSRCHIRATMEAS_W2L_CELL_LIST_SIZE];
#endif /*FEATURE_WCDMA_CM_LTE_SEARCH*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                       Function Declarations                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION     WSRCHIRATMEAS_PROCESS_SRCH_DONE_SIGNAL?

DESCRIPTION
  Handle searh done signal
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wsrchiratmeas_process_srch_done_signal(void);


/*===========================================================================
FUNCTION     WSRCHIRATMEAS_PROCESS_EUTRA_SRCH_DONE_SIGNAL

DESCRIPTION
  Handle LTE searh done signal and process event evaluation.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wsrchiratmeas_process_eutra_srch_done_signal(void);

/*===========================================================================
  FUNCTION 
    WSRCHIRATMEAS_GET_ACTIVE_FILT_IDX
  
  DESCRIPTION
    This function determines the first active filter index. The index can be
    used to access the elements of wsrchiratmeas_eutra_interrat_filter_coef. If
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
extern uint8 wsrchiratmeas_get_active_filt_idx(void);

/*===========================================================================
FUNCTION     L1CMSRCH_FIND_EUTRA_CELL

DESCRIPTION
  This function will find the cell in a LTE frequency .

DEPENDENCIES
  None

RETURN VALUE
  index of cell in the frequency database.

SIDE EFFECTS
  None
===========================================================================*/
uint8 wsrchiratmeas_find_eutra_cell(uint16 freq_idx, uint16 phy_cell_id);

/*===========================================================================
FUNCTION     WSRCHIRATMEAS_INIT_CELL_PARAMS

DESCRIPTION
  Initialize filter paramters for a LTE cell.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wsrchiratmeas_init_cell_params(  
  uint16 cell_idx 
);

/*===========================================================================
FUNCTION     WSRCHIRATMEAS_FILTER_MEAS_QUANT

DESCRIPTION
  Filtering for LTE measurement quantities
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wsrchiratmeas_filter_meas_quant(   
   uint16 cell_idx,
   uint16 ts_last_srch);

/*===========================================================================
FUNCTION     L1CMSRCH_FIND_EUTRA_FREQ_INFO

DESCRIPTION
  This function will remove eutra freuency from the frequency info list

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
uint16 wsrchiratmeas_find_eutra_freq_info(uint32 earfcn);

/*===========================================================================
FUNCTION     L1CMMSRCH_DELETE_EUTRA_CELL

DESCRIPTION
  This function will remove eutra freuency from the frequency info list

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
void wsrchiratmeas_delete_eutra_cell(uint16 freq_idx, uint16 cell_idx);


LOCAL int8 wsrchiratmeas_meas_quant_sort_comp_fn
(
  const void *p1,
  const void *p2
);


#ifdef FEATURE_WTOL_TDD_CM_SUPPORT
LOCAL void wsrchiratmeas_copy_cell_info_in
(
  wsrchiratmeas_lte_cell_struct_type* dest, 
  const wsrchlte_cell_struct_type* src
);

LOCAL void wsrchiratmeas_copy_cell_info_out
(
  wsrchlte_cell_struct_type* dest, 
  const wsrchiratmeas_lte_cell_struct_type* src
);


LOCAL void wsrchiratmeas_lte_tdd_rank_cells(uint32 earfcn);

/*===========================================================================
  FUNCTION 
    WSRCHIRATMEAS_LTE_TDD_HANDLE_CELL_DETECTION
  
  DESCRIPTION
    This function handles the provided list from the driver layer and 
    imports the detected cells into the measurement layer database.
   
  DEPENDENCIES
    None
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    wsrchiratmeas_sorted_lte_cell may be updated.
 ===========================================================================*/
void wsrchiratmeas_lte_tdd_handle_cell_detection
(
  /* List of cell detection and measurement results from driver layer */
  const wsrchlte_cm_meas_rsp_info_struct_type* result
);

/*===========================================================================
  FUNCTION 
    WSRCHIRATMEAS_LTE_TDD_HANDLE_CELL_MEASUREMENT
  
  DESCRIPTION
    This function handles the cell measurement algorithm. Cell detection
    occurs across 3 consecutive gaps, and 12 cells can be detected at each
    gap. As a result, a total of 36 cells can be detected in a single cycle.
    However, only 8 of these detected cells can be promoted into the
    measurement list. If a cell was both detected and measured in the same
    gap, it is considered to be a measured cell and is handled in this
    function. The cell promotion algorithm follows: 
    
    for each cell in the measurement list from the driver layer:
      (1) Attempt to find the cell's index in the detected list. Search by
          EARFCN and cell ID.
      (2) If the cell was not present in the detected list, then it must
          already be in the measurement list. In this case, update the RSRP
          of the existing entry and proceed to (1) for the next cell.
      (3) If the cell was present in the detected list in the measurement
          layer database, swap the cell at the index found in (1) with the
          cell at the index equal to the current size of the measurement list.
          This operation effectively places the newly-measured cell at the
          end of the measurement list.
      (4) Increment the size of the measurement list.
      (5) Decrement the size of the detected list.
 
      === EXAMPLE ===
      Detect List (meas layer db): [A B C D E F]
      Meas List   (driver layer) : [D A E]
 
      STEP #     MEAS DB         MEAS_ELEM     MEAS_SIZE  DETECT_SIZE
      1          [A B C D E F]   D             0          6
      2          [D B C A E F]                 1          5     
      3          [D B C A E F]   A             1          5
      4          [D A C B E F]                 2          4
      5          [D A C B E F]   E             2          4
      6          [D A E B C F]                 3          3
 
      After completion, the measurement layer database will contain all the
      measured cells followed by the detected cells. In this case: 
 
      Database:    [D A E B C F] 
      Meas list:   [D A E]
      Detect list: [B C F]
 
   
  DEPENDENCIES
    None
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    The measurement list may be updated.
 ===========================================================================*/
void wsrchiratmeas_lte_tdd_handle_cell_measurement
(
  const wsrchlte_cm_meas_rsp_info_struct_type* result
);

/*===========================================================================
  FUNCTION 
    wsrchiratmeas_prepare_eutra_cell_list_for_drv
  
  DESCRIPTION
    This function populates the given cell listing to be sent to the driver
    layer. It does not actually send the list to the driver; it simply
    populates the outbound buffer with data from the measurement layer.
   
  DEPENDENCIES
    None
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None.
 ===========================================================================*/
void wsrchiratmeas_prepare_eutra_cell_list_for_drv(wsrchlte_cm_meas_rsp_info_struct_type* result);


/*===========================================================================
  FUNCTION     wsrchiratmeas_sort_drv_layer_lte_cells
  
  DESCRIPTION
    Sort LTE cells in the driver layer structure based on measurement
    quantities.
    
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
 ===========================================================================*/
extern void wsrchiratmeas_sort_drv_layer_lte_cells
(
  uint8 start_idx, 
  uint8 max_cnt, 
  l1_sort_cmp_fn_type cmp_fn
);

/*===========================================================================
  FUNCTION
    wsrchiratmeas_sort_meas_layer_lte_cells
  
  DESCRIPTION
    Sort LTE cells in the measurement layer structure based on measurement
    quantities.
    
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
 ===========================================================================*/
extern void wsrchiratmeas_sort_meas_layer_lte_cells
(
  uint16 freq_idx,
  uint8 start_idx, 
  uint8 stop_idx, 
  l1_sort_cmp_fn_type cmp_fn
);


#endif /*FEATURE_WTOL_TDD_CM_SUPPORT*/


#if defined(FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH) && defined(FEATURE_WCDMA_TO_LTE_FACH)
/*===========================================================================
FUNCTION WSRCHLTE_FACH_ADD_CELLS

DESCRIPTION
 This function adds the LTE frequencies sent in MEAS_REQ to the CM module db. measurement layer 
 uses that db to scheduler LTE measurements in FACH
 
RETURN VALUE
   None.
SIDE EFFECTS
   None
=============================================================================*/
void wsrchiratmeas_fach_add_cells(void);

void wsrchiratmeas_add_eutra_cells_for_eval(uint16 freq_idx);

#endif

/*===========================================================================
FUNCTION        WSRCHIRATMEAS_CONN_STATE_CLEANUP

FILE NAME

DESCRIPTION     This is the connected (FACH/DCH) state cleanup routine for SRCH non-Intra modules

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern boolean wsrchiratmeas_conn_state_cleanup(l1_module_enum_type irat_module);

LOCAL void wsrchiratmeas_gsm_cleanup_done(void);

LOCAL void wsrchiratmeas_interf_cleanup_done(void);

#if defined(FEATURE_WCDMA_TO_LTE) && defined(FEATURE_WCDMA_CM_LTE_SEARCH)
LOCAL void wsrchiratmeas_lte_cleanup_done(void);
#endif

#endif /* WSRCHIRATMEAS_H*/
