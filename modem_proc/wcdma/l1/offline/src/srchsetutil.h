#ifndef SRCHSETUTIL_H
#define SRCHSETUTIL_H

/*=============================================================================
  FILE
    srchsetutil.h

  GENERAL DESCRIPTION
    This file provides the low-level operations related to cell set
    management. It contains only set theoric operations and does not manage 
    UMTS-level procedures.
                
  EXTERNALIZED FUNCTIONS
    srchsetutil_init()       Initialize module with heap memory
    srchsetutil_destroy()    Destroy module and return memory to heap
    srchsetutil_alloc()      Allocate a cell from the empty pool
    srchsetutil_move()       Perform a cell state transition
    srchsetutil_remove()     Return a cell to the empty pool
    srchsetutil_remove_all() Empty a set
    srchsetutil_get_set_count()      Determine the cardinality of a set
    srchsetutil_search()     Locate a cell by frequency and scrambling code
    srchsetutil_exec()       Perform a custom function on each cell in a set

  REGIONAL FUNCTIONS 
    srchsetutil_local_search() Search utility function called by externalized 
                               search functions 
  
  INITIALIZATION AND SEQUENCING REQUIREMENTS
    srchsetutil_init() should be called at WCDMA L1 stack initalization to 
    initialize the module with memory from the heap.

    srchsetutil_destroy() should be called when WCDMA L1 transitions to the 
    STOPPED state to free heap memory.

  Copyright (c) 2000-2014 by QUALCOMM.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchsetutil.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/16   rn      Dynamic cell structure memory allocation at the time of additional 
                   carrier configured.
07/01/16   rn      Searcher F3 reduction
02/24/15   ap      Initial check-in for WL1 QSH framework 
02/03/15   psr     Added rx_agc field in srch_ofreq_cell_list_struct_type
10/09/14   ag      Added API to give STTD of the serving cell
09/18/14   mk      Featurizing LTE code.
07/01/14   hk      Header file inclusion cleanup
06/17/14   ar      Get the filtered_parms structure field only for non-FACH states
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/30/14   km      New srchsetutil API to Removes cells of frequency other than
                   the specified frequency
04/07/14   sr      Setting the correct TxD status for the cell during aset update procedure
02/12/14   sad     DB-3C API check in
01/22/14   jk      Min_timer on E1D_enhancement enablling will be based on flag 
                   instead of timer being set to value other than 0.
01/13/14   jk      DC-HSUPA Interf Spec CR Changes.
01/03/14   km      Replace old srchset mutex with new srchsetutil mutex
12/16/13   as      Porting G2W TA from Triton to Dime.
11/25/13   jk      FR 15721: HSPA+: Call performance improvement when selecting the best cell.
11/18/13   rsr     Added QICE 3C Support
11/04/13   da      Neighbor set handling during DC to SC transition
10/29/13   sad     Inital check in for Secondary ASET update changes
10/17/13   bs      Preliminary 3C Support
09/12/13   jd      Searcher redesign 
07/15/13   zr      Adding size macro for pending delete composite set
06/11/13   zr      Including DC sets in ALL bmsk 
12/01/12   zr      Created file
=============================================================================*/

/*=============================================================================
  INCLUDED FILES
=============================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "srchparm.h"
#include "l1rrcif.h"
#include "l1const.h"
#include "tlm.h"

/* from interf */
#include "srchcmmeas.h"


#if defined FEATURE_QSH_DUMP || defined FEATURE_QSH_EVENT_METRIC
#include "wl1_qsh_ext.h"
#endif /* defined FEATURE_QSH_DUMP || defined FEATURE_QSH_EVENT_METRIC*/

/*=============================================================================
  PREPROCESSOR DIRECTIVES
=============================================================================*/

#define MAX_NUM_ANTENNA 2

#define SRCHSETUTIL_LOCK()    srchsetutil_get_intlock()
#define SRCHSETUTIL_UNLOCK()  srchsetutil_get_intfree()

#define SRCH_CAND_AGE_MAX 50

/* The inter-frequency sets are all the same size. */
#define SRCHSET_NUM_INTERF_CELLS 32

#define SRCHSET_NUM_GSM_CELLS    32

/* The allocated cell pool is a one-dimensional array. These macros define the
   indices of each carrier in the pool. Since intra and inter-frequency cells
   are different structures, two cell pools are defined. */

/* Carrier 0 */   
#define SRCHSET_NUM_CELLS_C0 SRCH_ESET_MAX_C0
#define SRCHSET_MIN_BASIC_CELL_IDX_C0   0
#define SRCHSET_MAX_BASIC_CELL_IDX_C0  (SRCHSET_NUM_CELLS_C0 - 1)

/* Carrier 1 */
#if (NUM_CARR > 1)
#define SRCHSET_NUM_CELLS_C1 SRCH_ESET_MAX_C1
#define SRCHSET_MIN_BASIC_CELL_IDX_C1   SRCHSET_NUM_CELLS_C0
#define SRCHSET_MAX_BASIC_CELL_IDX_C1  (SRCHSET_NUM_CELLS_C0 + \
                                        SRCHSET_NUM_CELLS_C1 - 1)
#endif

/* Carrier 2 */
#if (NUM_CARR > 2)
#define SRCHSET_NUM_CELLS_C2 SRCH_ESET_MAX_C2
#define SRCHSET_MIN_BASIC_CELL_IDX_C2  (SRCHSET_NUM_CELLS_C0 + \
                                        SRCHSET_NUM_CELLS_C1)
#define SRCHSET_MAX_BASIC_CELL_IDX_C2  (SRCHSET_NUM_CELLS_C0 + \
                                        SRCHSET_NUM_CELLS_C1 + \
                                        SRCHSET_NUM_CELLS_C2 - 1)
#endif 

/* Carrier 3 */    
#if (NUM_CARR > 3)
#define SRCHSET_NUM_CELLS_C3 SRCH_ESET_MAX_C3
#define SRCHSET_MIN_BASIC_CELL_IDX_C3  (SRCHSET_NUM_CELLS_C0 + \
                                        SRCHSET_NUM_CELLS_C1 + \
                                        SRCHSET_NUM_CELLS_C2)
#define SRCHSET_MAX_BASIC_CELL_IDX_C3  (SRCHSET_NUM_CELLS_C0 + \
                                        SRCHSET_NUM_CELLS_C1 + \
                                        SRCHSET_NUM_CELLS_C2 + \
                                        SRCHSET_NUM_CELLS_C3 - 1)
#endif 

/* Total number of intra-frequency cells across all carriers */
#if (NUM_CARR == 4)
#define SRCHSET_NUM_CELLS (SRCHSET_NUM_CELLS_C0 + \
                           SRCHSET_NUM_CELLS_C1 + \
                           SRCHSET_NUM_CELLS_C2 + \
                           SRCHSET_NUM_CELLS_C3)
#elif (NUM_CARR == 3)
#define SRCHSET_NUM_CELLS (SRCHSET_NUM_CELLS_C0 + \
                           SRCHSET_NUM_CELLS_C1 + \
                           SRCHSET_NUM_CELLS_C2)
#elif (NUM_CARR == 2)
#define SRCHSET_NUM_CELLS (SRCHSET_NUM_CELLS_C0 + \
                           SRCHSET_NUM_CELLS_C1)
#else
#define SRCHSET_NUM_CELLS (SRCHSET_NUM_CELLS_C0)
#endif

/* Bitmasks for searching all sets */
#define SRCHSET_BMSK_ALL ((1 << SET_A)        | \
                          (1 << SET_SYNC_N)   | \
                          (1 << SET_ASYNC_N)  | \
                          (1 << SET_U)        | \
                          (1 << SET_ADD_C)    | \
                          (1 << SET_DROP_C)   | \
                          (1 << SET_DCH_ONLY) | \
                          (1 << SET_A_NO_PN)  | \
                          (1 << SET_CAND_U)   | \
                          (1 << SET_SAVED)    | \
                          (1 << SET_H))

#define SRCHSET_BMSK_ALL_NO_S_H ((1 << SET_A)        | \
                          (1 << SET_SYNC_N)   | \
                          (1 << SET_ASYNC_N)  | \
                          (1 << SET_U)        | \
                          (1 << SET_ADD_C)    | \
                          (1 << SET_DROP_C)   | \
                          (1 << SET_DCH_ONLY) | \
                          (1 << SET_A_NO_PN)  | \
                          (1 << SET_CAND_U))

#define SRCHSET_COMP_SET_BMSK_ALL   0x0f
#define SRCHSET_INTERF_SET_BMSK_ALL 0x7f

/* Maximum size fields of the basic sets - Carrier 0 */
#define SRCH_ASET_MAX_C0             6  
#define SRCH_ASET_NO_PN_MAX_C0       6  
#define SRCH_ADD_CSET_MAX_C0         10 
#define SRCH_ASYNC_NSET_MAX_C0       32 
#define SRCH_CAND_USET_MAX_C0        10 
#define SRCH_DCH_ONLY_SET_MAX_C0     32 
#define SRCH_DROP_CSET_MAX_C0        20 
#define SRCH_ESET_MAX_C0             148
#define SRCH_SAVED_SET_MAX_C0        70 
#define SRCH_SYNC_NSET_MAX_C0        32 
#define SRCH_USET_MAX_C0             32 
#define SRCH_HSET_MAX_C0             15

/* Basic sets - Carrier 1 */
#if (NUM_CARR > 1)
#define SRCH_ASET_MAX_C1             4  
#define SRCH_ASET_NO_PN_MAX_C1       6  
#define SRCH_ADD_CSET_MAX_C1         10 
#define SRCH_ASYNC_NSET_MAX_C1       32 
#define SRCH_CAND_USET_MAX_C1        10 
#define SRCH_DCH_ONLY_SET_MAX_C1     32 
#define SRCH_DROP_CSET_MAX_C1        20 
#define SRCH_ESET_MAX_C1             148
#define SRCH_SAVED_SET_MAX_C1        70 
#define SRCH_SYNC_NSET_MAX_C1        32 
#define SRCH_USET_MAX_C1             32
#define SRCH_HSET_MAX_C1             15
#endif /* (NUM_CARR > 1) */

/* Basic sets - Carrier 2 */
#if (NUM_CARR > 2)
#define SRCH_ASET_MAX_C2             4  
#define SRCH_ASET_NO_PN_MAX_C2       6  
#define SRCH_ADD_CSET_MAX_C2         10 
#define SRCH_ASYNC_NSET_MAX_C2       32 
#define SRCH_CAND_USET_MAX_C2        10 
#define SRCH_DCH_ONLY_SET_MAX_C2     32 
#define SRCH_DROP_CSET_MAX_C2        20 
#define SRCH_ESET_MAX_C2             148
#define SRCH_SAVED_SET_MAX_C2        70 
#define SRCH_SYNC_NSET_MAX_C2        32 
#define SRCH_USET_MAX_C2             32
#define SRCH_HSET_MAX_C2             15
#endif /* (NUM_CARR > 2) */

/* Basic sets - Carrier 3 */
#if (NUM_CARR > 3)
#define SRCH_ASET_MAX_C3             4  
#define SRCH_ASET_NO_PN_MAX_C3       6  
#define SRCH_ADD_CSET_MAX_C3         10 
#define SRCH_ASYNC_NSET_MAX_C3       32 
#define SRCH_CAND_USET_MAX_C3        10 
#define SRCH_DCH_ONLY_SET_MAX_C3     32 
#define SRCH_DROP_CSET_MAX_C3        20 
#define SRCH_ESET_MAX_C3             148
#define SRCH_SAVED_SET_MAX_C3        70 
#define SRCH_SYNC_NSET_MAX_C3        32 
#define SRCH_USET_MAX_C3             32
#define SRCH_HSET_MAX_C3             15
#endif /* (NUM_CARR > 3) */    

#define SRCH_PULLIN_SET_MAX 8

/* Maximum size of the composite sets - Carrier 0*/                 
#define SRCH_COMP_NBCH_SET_MAX_C0    2
#define SRCH_COMP_QICE_ASET_MAX_C0   8
#define SRCH_COMP_QICE_NSET_MAX_C0   5
#define SRCH_COMP_TXD_DET_SET_MAX_C0 16

/* Composite sets - Carrier 1 */
#if (NUM_CARR > 1)
#define SRCH_COMP_NBCH_SET_MAX_C1    2
#define SRCH_COMP_QICE_ASET_MAX_C1   8
#define SRCH_COMP_QICE_NSET_MAX_C1   5
#define SRCH_COMP_TXD_DET_SET_MAX_C1 16
#endif /* (NUM_CARR > 1) */ 

/* Composite sets - Carrier 2 */
#if (NUM_CARR > 2)
#define SRCH_COMP_NBCH_SET_MAX_C2    2
#define SRCH_COMP_QICE_ASET_MAX_C2   8
#define SRCH_COMP_QICE_NSET_MAX_C2   5
#define SRCH_COMP_TXD_DET_SET_MAX_C2 16
#endif /* (NUM_CARR > 2) */

/* Composite sets - Carrier 3 */
#if (NUM_CARR > 3)
#define SRCH_COMP_NBCH_SET_MAX_C3    2
#define SRCH_COMP_QICE_ASET_MAX_C3   8
#define SRCH_COMP_QICE_NSET_MAX_C3   5
#define SRCH_COMP_TXD_DET_SET_MAX_C3 16
#endif /* (NUM_CARR > 3) */

/* Inter-frequency sets - only on primary carrier */
#define SRCH_INTERF_SYNC_NSET_MAX     SRCHSET_NUM_INTERF_CELLS
#define SRCH_INTERF_ASYNC_NSET_MAX    SRCHSET_NUM_INTERF_CELLS
#define SRCH_INTERF_VA_SET_MAX        SRCHSET_NUM_INTERF_CELLS
#define SRCH_INTERF_SAVED_SET_MAX     SRCHSET_NUM_INTERF_CELLS
#define SRCH_INTERF_MONITORED_SET_MAX SRCHSET_NUM_INTERF_CELLS
#define SRCH_INTERF_NSET_MAX          SRCHSET_NUM_INTERF_CELLS
#define SRCH_INTERF_ESET_MAX          SRCHSET_NUM_INTERF_CELLS

/* GSM sets - only on primary carrier*/
#define SRCH_GSM_ESET_MAX   L1_MAX_CELL_MEAS
#define SRCH_GSM_NSET_MAX   L1_MAX_CELL_MEAS
#define SRCH_GSM_USET_MAX   L1_MAX_CELL_MEAS

/* Max number of forbidden tone positions */
#define SRCH_GSM_NUM_MAX_FORBIDDEN_OFFSET 3

#define SRCH_NO_CELL_IDX         0xff
#define SRCH_INVALID_CELL_DB_IDX 0xff

/* Number of cells (neighbors + ASET) on primary carrier to be sorted before input to QICE module  */
#define SRCH_PRI_NBRS_MAX (SRCH_ASET_MAX_C0 + SRCH_SYNC_NSET_MAX_C0 + SRCH_USET_MAX_C0)

#define SRCH_DC_NBRS_MAX (SRCH_ASET_MAX_C1 + SRCH_SYNC_NSET_MAX_C1 + SRCH_USET_MAX_C1)

#define SRCH_3C_NBRS_MAX (SRCH_ASET_MAX_C2 + SRCH_SYNC_NSET_MAX_C2 + SRCH_USET_MAX_C2)

/* Max number of neighbor cells considered for QICE on primary carrier */
#define SRCH_PRI_CARR_QICE_NBR_MAX   2
/* Max number of neighbor cells considered for QICE on secondary carrier */
#define SRCH_SEC_CARR_QICE_NBR_MAX   3

#define SRCH_TRIAGE_SET_MAX (SRCH_ASET_MAX_C0 + SRCH_ASET_MAX_C1 + SRCH_PRI_CARR_QICE_NBR_MAX + SRCH_SEC_CARR_QICE_NBR_MAX + SRCH_COMP_NBCH_SET_MAX_C0  + SRCH_COMP_NBCH_SET_MAX_C1)

#define SRCH_DC_TRIAGE_SET_MAX 5

#ifdef FEATURE_WCDMA_3C_HSDPA
#define SRCH_3C_TRIAGE_SET_MAX WFW_DEMOD_NUM_MAX_CELLS_TER_CARRIER
#endif /*FEATURE_WCDMA_3C_HSDPA*/

/* Accessor/mutator macros */ 
#define SRCH_GET_FILT_ENG(cell_ptr,filt_idx)             (cell_ptr->filtered_parms[filt_idx].filt_eng)
#define SRCH_GET_FILT_ECIO(cell_ptr,filt_idx)\
((search_state == SRCH_FACH_STATE) ? (cell_ptr->filt_ecio) : (cell_ptr->filtered_parms[filt_idx].filt_ecio))

#define SRCH_GET_FILT_RSCP(cell_ptr,filt_idx)            (cell_ptr->filtered_parms[filt_idx].filt_rscp)
#define SRCH_GET_PATHLOSS(cell_ptr,filt_idx)             (cell_ptr->filtered_parms[filt_idx].pathloss)
#define SRCH_GET_FILT_LEN(cell_ptr,filt_idx)             (cell_ptr->filtered_parms[filt_idx].filt_len)
#define SRCH_GET_FILT_ECIO_HI_RES(cell_ptr,filt_idx)     (cell_ptr->filtered_parms[filt_idx].filt_ecio_hi_res)
#define SRCH_GET_FILT_RSCP_HI_RES(cell_ptr,filt_idx)     (cell_ptr->filtered_parms[filt_idx].filt_rscp_hi_res) 
#define SRCH_GET_FILT_LEN_RSCP(cell_ptr,filt_idx)        (cell_ptr->filtered_parms[filt_idx].filt_len_rscp)

#define SRCH_SET_FILT_ENG(cell_ptr,filt_idx,val)         (cell_ptr->filtered_parms[filt_idx].filt_eng = val)
#define SRCH_SET_FILT_ECIO(cell_ptr,filt_idx,val)        (cell_ptr->filtered_parms[filt_idx].filt_ecio = val)
#define SRCH_SET_FILT_RSCP(cell_ptr,filt_idx,val)        (cell_ptr->filtered_parms[filt_idx].filt_rscp = val)
#define SRCH_SET_PATHLOSS(cell_ptr,filt_idx,val)         (cell_ptr->filtered_parms[filt_idx].pathloss = val)
#define SRCH_SET_FILT_LEN(cell_ptr,filt_idx,val)         (cell_ptr->filtered_parms[filt_idx].filt_len = val)
#define SRCH_INCREMENT_FILT_LEN(cell_ptr,filt_idx)       (cell_ptr->filtered_parms[filt_idx].filt_len++)
#define SRCH_SET_FILT_ECIO_HI_RES(cell_ptr,filt_idx,val) (cell_ptr->filtered_parms[filt_idx].filt_ecio_hi_res = val)
#define SRCH_SET_FILT_RSCP_HI_RES(cell_ptr,filt_idx,val) (cell_ptr->filtered_parms[filt_idx].filt_rscp_hi_res = val) 
#define SRCH_SET_FILT_LEN_RSCP(cell_ptr,filt_idx,val)    (cell_ptr->filtered_parms[filt_idx].filt_len_rscp = val)

/* Maximum number of event on any given WCDMA frequency */
#define SRCHDCH_MAX_EVT MAX(SRCHDCH_MAX_INTRAF_EVT, SRCHDCH_MAX_INTERF_EVT)

/*=============================================================================
  ENUMERATED TYPES
=============================================================================*/

/* The basic sets */
typedef enum
{
  /* Cell belongs to Empty Set */
  SET_E,

  /* Cell belongs to Active Set */
  SET_A, 

  /* Cell belongs to Sync Neighbor Set */
  SET_SYNC_N,
  
  /* Cell belongs to Async Neighbor Set */
  SET_ASYNC_N,
  
  /* Cell belongs to Unlisted Set. Note: during initial acquisition, 
     all cells idetified by Step_3 Search belong to Unlisted Set. */
  SET_U,
  
  /* Cell belongs to Add-Candidate Set -- a SET containing cells that are 
     originally from Sync_NSET or USET and whose energy is strong enough,
     based on network-specified event criteria (1A, 1C, 1D & 1E), 
     to be handoff candidates */
  SET_ADD_C,
  
  /* Cell belongs to Drop-Candidate Set -- a SET containing Cells that 
     are originally from ASET or Add_CSET and whose energy is NOT strong 
     enough, based on network-specified event criteria (1B & 1F), 
     to stay as handoff candidates. */
  SET_DROP_C,
  
  /* These cells are measured only in DCH state */
  SET_DCH_ONLY,

  /* Cells to be added to SET_A once a PN search finishes */
  SET_A_NO_PN,
  
  /* Cells moved to candidate set from the uset. This is also a part of the
     candidate set. However it holds only those cells from SET_U whose energy 
     is strong enough to consider as handoff candidate */
  SET_CAND_U,
  
  /* Saved set Info is used during HHO before any dch to dch hho
     We need to put all necessary set to saved set in order to retrive
     those in case HHO failure */
  SET_SAVED,
  SET_H,

  /* Number of Basic sets */
  SET_MAX
} srch_set_enum_type;


/* The composite sets */
typedef enum
{
  /* Neighbor BCH cells*/
  COMP_SET_NBCH,
  
  /* Serving cells in use by the Qualcomm Interference Cancellation 
     and Equalization service */
  COMP_SET_QICE_A,
  
  /* Intra-frequency neighbor cells in use by the Qualcomm Interference
     Cancellation and Equalization service */
  COMP_SET_QICE_N,
  
  /* Cells undergoing transmitter detection */
  COMP_SET_TXD_DET,

  /* Number of Composite sets */
  COMP_SET_MAX
} srch_comp_set_enum_type;


/* The GSM sets */
typedef enum
{
  /* Cell belongs to Empty Set */ 
  GSM_SET_E,

  /* Cell belongs to GSM Neighbor Set */
  GSM_SET_N, 

  /* Cell belongs to GSM Unmonitored Set */
  GSM_SET_U,
  
  /* Number of GSM sets */
  GSM_SET_MAX
} srch_gsm_set_enum_type;


 /*
 * Inter freq Set Management Strategy Explained...
 *
 * -E- All cells begin in the INTERF_SET_E (Empty) set.
 *     After Allocation they may be removed, at which time they
 *     will return to the empty set.
 * -SYNC_N- Cells are added to the INTERF_SET_N (Neighbor) when 
 *     position is known.
 *     When monitoring should be performed this is directed
 *     in the Cells for Measurement/meas_cell_list[]
 *     parameter of the measurement control message. 
 * -VA- Cells which are in virtual active set. This is applicable
 *      only in DCH state. Cells are used for frequency quality estimate.
 * -MONITORED- Cells that are in Cells for Measurement or Cell Info List 
 *             are in this Set. These are passed to the drivers for measurement.
 *             VA Set cells are selected from Monitored Set.
 *             Applicable in DCH only.
 * -MEAS_N- All the other frequency cells. Applicable in DCH state. 
 *          Monitored set cells are selected from MEAS_N. 
 */
typedef enum
{
  /* Empty set */
  INTERF_SET_E,

  /* Timing known neighbors */
  INTERF_SET_SYNC_N, 
  
  /* Timing unknown neighbors */
  INTERF_SET_ASYNC_N,
  
  /* Virtual active set. Cells in this set have met the criteria for being
     added to the active set, but are on a different frequency. Applicable in 
     DCH only. */
  INTERF_SET_VA, 
  
  /* Saved cells. Used during inter-frequency cell reselection */
  INTERF_SET_SAVED,
 
  /* Monitored set. Applicable in DCH only. */  
  INTERF_SET_MONITORED,
  
  /* Neighbor set. Applicable in DCH only. */
  INTERF_SET_N,
  
  /* Number of inter-frequency sets */
  INTERF_SET_MAX
} srch_interf_set_enum_type;


/* TxD detection statuses */
typedef enum
{
  SRCH_TXD_STATUS_DETECTION_NEEDED,  /* Needed and can happen now    */
  SRCH_TXD_STATUS_DETECTION_IN_PROG, /* Currently ongoing            */
  SRCH_TXD_STATUS_NW_INFORMED,       /* Network has informed status  */
  SRCH_TXD_STATUS_DETECTION_ON_HOLD, /* Needed but cannot happen now */
  SRCH_TXD_STATUS_DETECTED,          /* Already detected             */
} srch_cell_txd_det_status;


/* Event timestamp structure */
typedef struct
{
  /* The sfn at which the tresel timer was started */
  uint16 ts_update_sfn;

  /* Time when the event first becomes TRUE */
  uint16 time;
  
  /*Boolean indicating min timeing enabled*/
  boolean e1d_min_time_enabled;
   
  /* event e1d enhancement timer, runs against min_timer */
  uint16 e1d_enhancement_min_time;
  /* Number of times the event has been reported for this cell */
  uint16 num_reports;

  /* Flag indicating is the event time stamp is enabled  */
  boolean enabled;

  /* variable which indicates whether the periodic event reporting
     for 1A and 1C is activated  Refer to 14.1.4 of 3gpp 25.331 */
  boolean periodic_timer_activated;
} srch_evt_ts_struct_type;

typedef struct
{
  /* Hierachical Cell Structrue Priority */
  uint16        hcs_prio;
  
  /* Quality Threshold level for HCS cell reselection */  
  uint16        q_hcs;
    
 /* Penalty_time. 0 means not used. next two not allowed for 0 */
  uint16        penalty_time_frames;
   
  /* Temporary_offset used for CPICH RSCP*/
  uint16        tmp_offset1;
    
  /* Temporary_offset used for CPICH Ec/No */
  uint16        tmp_offset2;

  /* W(x) factor for R ranking */
  uint16         Wx_R;

  /* H ranking for the cell */
  int16          H;

  /* Penalty timer for this cell (neighbor) */
  srch_evt_ts_struct_type  evt_cr_penalty_timer;

  /* boolean varibale to indicate that the cell qualifies for 
   * measurement in HCS mode
   */
  boolean cell_qualify;

  /* Counter to check for choosing all remaining cells in equal probability*/
  uint8 elapse_cntr;
} l1_cell_hcs_info_struct_type; 



/*--------------------------------------------------------------------------
        SFN-CFN OBSERVED TIME DIFFERENCE BOOK KEEPING DATA STRUCTURE
--------------------------------------------------------------------------*/
typedef struct 
{
  tlm_tfn_struct_type sfn_wrc_diff;
  boolean sfn_wrc_valid;
} sfn_wrc_diff_type;


typedef struct
{
  /* Flag indicating time difference information below is current */ 
  boolean tm_off_status;
    
  /* Defined in 5.1.9 of TS25.215 */
  uint32  tm;
    
  /* Defined in 5.1.9 of TS25.215 */
  uint16  off;

  /* reference cell physical channel combiner position when the above 
     time difference was last observed or adjusted */
  uint32  ref_pos;

  sfn_wrc_diff_type sfn_wrc_diff;
  boolean sfn_sfn_status;
  uint32 sfn_sfn_val;

#ifdef FEATURE_WCDMA_CNT_C_SFN_DIFF
  /* SFN_CNT_HIGH value */
  uint16 sfn_cnt_c_high;
#endif 
  
  /* Rescheduling of SFN decode needed ? */
  boolean reschedule_needed;

  /* Time stamp when the time differnce was initially measured */
  uint32  ts_1;
  
  /* Time stamp when the time difference was last adjusted */  
  uint16  ts_2;
} srch_sfn_cfn_tm_diff_struct_type;

typedef struct
{
  boolean enabled;
  uint16 time;
  boolean report_sent;
} srch_evt_6g_6f_struct_type;

/* Number of measurements to average RSSI during cell reselection */
#define SRCHCR_GSM_NUM_RSSI_SAMPLES_MAX 4

#define GSM_INVALID_CELL_ID 0xFF

/* Structure that contains measured values for cell during reselection  */
typedef struct
{
  /* timing information for aquisition */
  boolean timing_valid;

  /* Timing offset from the current GFN and the new cell
     in number of frames and quarter symbol offset.

     current timing
                     |<-acq start
     _____|__________|_________|_________|_________|_________|_____
                     |<--   time_offset  -->|
                     |<- frame_offset -->|
                                       ->|  |<- qs_offset

     neigbhor timing
     _________|_________|_________|_________|S|_______|_________|__
                                            SCH
  */
  uint8 frame_offset;
  uint16 qs_offset;

  /* Frequency offset */
  int16 frequency_offset;

  /* difference between this cell's multiframe and the multiframe
   * as measured from the GFN.
   */
  int32 delta_multiframe;
  
  uint32 hyper_frame_offset;

  boolean bsic_verify_reqd;

  /* BSIC for the cell found in SCH decode */
  boolean bsic_valid;
  l1_gsm_bsic_struct_type bsic;

  /* accumulated RSSI over samples */
  int32 rssi_accum_db;

  /* number of RSSI samples accumulated */
  uint32 num_rssi_samples_accum;

  /* RSSI samples for averaging */
  #if ( (SRCHCR_GSM_NUM_RSSI_SAMPLES_MAX < SRCHFACH_NUM_GSM_RSSI_PER_CARRIER) )
  int16 rssi_db_samples[SRCHFACH_NUM_GSM_RSSI_PER_CARRIER];
  #else
  int16 rssi_db_samples[SRCHCR_GSM_NUM_RSSI_SAMPLES_MAX];
  #endif

  /* averaged RSSI over SRCHGSM_NUM_RSSI_SAMPLES_MAX measurements */
  int16 filt_rssi;

  /* filter length */
  uint16 filt_len;

  /* current sample index */
  uint8 sample_idx;

  /* latest RSSI measured */
  int16 last_meas_rssi_db;

  boolean rssi_valid;

} srchcr_gsm_meas_struct_type;


#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/* The following struct is populated by the third meas request
   from the RCC (SIB 19), It is used for absolute priority */
typedef struct
{
  int8 priority;
  int16 q_rxlevmin_gsm; 
  uint8 threshx_high;  
  uint8 threshxlow;  
} srch_inter_rat_gsm_priority_info_type;
#endif


/* Union of ALL Intra-frequency and inter-frequency event Time stamps */
typedef union
{
  srch_evt_ts_struct_type  intraf_evt_ts[SRCHDCH_MAX_INTRAF_EVT];

  /* CELL_DCH Intra-frequency Event time stamps */
  srch_evt_ts_struct_type  interf_evt_ts[SRCHDCH_MAX_INTERF_EVT];

  /* CELL_DCH Intra-frequency Event time stamps */
  uint16 periodic_ts;
} srch_evt_ts_union_type;


/* Union of ALL Intra- and inter-frequency event-barred status */
typedef union
{
  boolean intraf_evt_barred[SRCHDCH_MAX_INTRAF_EVT];
} srch_evt_barred_union_type;


/* Union of ALL Intra- and inter-frequency event report status, Those flags
   are used by mobile to remember is a particular cell has been reported for 
   a particular event and therefore, to prevent network from being overwhelmed
   by event reports. The use or the necessity of those fields need to be 
   re-evaluated when multiple report and periodic report are implemented in
   the future.
 */
typedef union
{
  /* Intra-frequency event report status */
  boolean intraf_evt_rpt_status[SRCHDCH_MAX_INTRAF_EVT];
} srch_evt_rpt_union_type;


typedef struct
{
  /* WCDMA frequency assignment for the cell */
  uint16 freq;

  /* WCDMA Cell Scrambling Code */
  uint16 scr_code;
      
  /* Position in current cell pointer array */
  uint8 idx;

  /* Current carrier index of cell */
  uint8 carr_idx;

  /* Forward power control combination index of the cell */
  uint8 tpc;

  uint8 sec_scr_code_index;

  /* TRUE if the CPICH of this cell uses STTD */
  boolean sttd;

  /* give the status of the sttd flag above whether it has been detected
     or provided from nw or it needs detection */
  srch_cell_txd_det_status txd_status;

  /* retry count for TxD */
  uint8 retry_txd_det_cnt;

  /* cell database index */
  uint8 cell_db_idx;

  /* boolean to indicate when ref_pos gets updated */
  boolean ref_pos_updated;

  /* TRUE will indicate E-DCH is active on this cell. THis info will help in
  distinguishing non E-DCH aset cells which can trigger evt 1J */
  boolean edch_active;

  /* OVSF code of the secondary CPICH if one is present.  If one
     is not this is set to NO_SECONDARY_CPICH or 0xFFFF
   */
  boolean sec_cpich_enabled;
  boolean sec_cpich_enabled_mimo_mode;
  uint16 sec_cpich_scr_code;
  uint16 sec_cpich_ovsf;
  uint16 sec_cpich_scr_code_mimo_mode;
  uint16 sec_cpich_ovsf_mimo_mode;
  uint16 sec_cpich_imb_scale;
    
  /* Which Set the Cell currently belongs to */
  srch_set_enum_type set;
  srch_set_enum_type prev_set;

  /* The total energy and Ec/Io in the best paths found in a sweep */
  uint32 tot_eng;
  uint32 tot_eng_prim;
  uint32 tot_eng_div;
  uint16 tot_ecio;
  uint16 ecio;

  /* This will be updated post PN search and will be used in RSCP filt*/
  uint16 tot_rscp;
  
  /* Filtered energy and Ec/Io and Filter length used currently */
  uint16 path_thresh;
  /* path threshold for this cell */

  
  struct
  {
   uint32 filt_eng;
   uint16 filt_ecio;
   uint16 filt_rscp;
   uint16 pathloss;
   uint16 filt_len;
   uint16 filt_len_rscp;
#if (defined(FEATURE_REL5) && defined(FEATURE_WCDMA_LOG_FILTERING))
   uint16  filt_ecio_hi_res;
   /* (Min RSCP =121) x512 = 61952. uint16 is good to hold the data*/
   uint16 filt_rscp_hi_res;
#endif   /* (defined(FEATURE_REL5) && defined(FEATURE_WCDMA_LOG_FILTERING)) */

  } filtered_parms[SRCH_WCDMA_MAX_INTRAF_FILT_SIZE];


  uint32  filt_eng;
  uint16  filt_ecio;
#if (defined(FEATURE_REL5) && defined(FEATURE_WCDMA_LOG_FILTERING))
  uint16  filt_ecio_hi_res;
#endif   /* (defined(FEATURE_REL5) && defined(FEATURE_WCDMA_LOG_FILTERING)) */
  uint16  filt_len;

  /* CPICH RSCP and filtered RSCP */
  uint16  rscp;
    /* It is RxAGC + Filtered Ec/Io. The actual CPICH RSCP is "-rscp" */
  uint16  filt_rscp;
  /*Saving RxAGC value for FR 1947 Neighbour Search Optimizations */
  int16 rx_agc;

  /*Running a 15s timer to remove the cell if the energy goes lower than threshold*/
  int32 ecio_drop_thresh_timer_value;
	
  /*Used to decrement the timer value*/
  uint32 prev_eval_drx_cycle_num ; 

  /* Flag indicating if the cell has been evaluated */
  boolean evaled;
    
  /* positions of the maximum energy paths in 1/8 chip units. */
  uint32  max_pos[ SRCH_MAX_PILOT_PEAKS ];
    
  /* Energies of the maximum energy paths */
  uint32  max_eng[ SRCH_MAX_PILOT_PEAKS ];

  /* positions of the maximum energy paths in 1/8 chip units. */
  uint32  max_pos_div[ SRCH_MAX_PILOT_PEAKS ];

  /* Energies of the maximum energy paths */
  uint32  max_eng_div[ SRCH_MAX_PILOT_PEAKS ];
 
  /* variable to tell triage if it can reliably use results from primary
   * or diversity antenna
   */
  boolean  srch_result_valid[MAX_NUM_ANTENNA];

  /* The position of the path used for centering searching window. Normally,
     it is the earliest usable (or above threshold SRCH_PATH_THRESHOLD) path.
     
       For cells being demodulated, this position is updated after each finger
       triage.
       For other cells, this position is updated after each search of the 
       cells.
       
     Valid value ranges (0..307200) in CHIPx8. 0xFFFFFFFF (or SRCH_NO_REF_POS)
     implies that ref_pos is not being set to a valid value.
   */
  uint32  ref_pos;

  /* Element to track filtered value of peak position */
  uint32  filt_win_peak;

  /* The age of the above reference position, in units of search period. 
     Search period is defined by SRCH_ASET_SRCH_PERIOD for Dmod-Set (ASET +
     optional 1 Nbr_Cell being demodulated) and CSET and 
     by SRCH_NSET_SRCH_PERIOD for NSET.
     
     At time, ref_pos is updated with a new search result or a finger dump
     result, ref_age is set to 0.
   */
  int16   ref_age;

  /* Redetect age. As soon as this exceeds 10, we'll accept the redetected  
     position and reset this variable */
  uint16 redetect_age ;
 
  /* Element to track ref age of neighbor cells in DRX state */
  uint16 ref_age_drx ;

  /* Reference Time difference to Serving cell, in chips. 
     0xFFFF indicates the field is not included. This field is 
     either obtained from a SIB message or via mobile measurement.
   */
  uint16  time_diff;
    
  /* Neighbor Cell timing accuracy relative to the serving cell.
     This will be translated to "search window size" for the neighbor.
   */
  l1_cell_time_acc_enum_type time_acc;

  /* Search window size for this cell */
  uint32  win_size;

  /* flag indicating if the cell's SFN decoding is requested for 
     measurement.
   */
  boolean read_sfn;

  /* Quality offsets to serving cell for cell reselection */
  int16  q_offset1;
  int16  q_offset2;

  /* Minimum qualities */
  int16  qual_min;
  int16  rxlev_min;
  int16  max_tx_pwr;

  /* Cell selection values evaluated by SRCH and to be reported to RRC */
  int16    s_qual;
  int16    s_rxlev;

  /**** this structure needs to be expanded to includ all other info, such
    as HCS info, etc.  **************/

  /* Cell Reselction time stamp. Enabled when the neighbor cell is
     significantly stronger than the ASET cell.
   */

  int16 cell_offset;
  /*  cell individual offset Refer to 10.3.7.2 and 14.1.5.3 
      for an explanation of this parameter. 
      Used to set invidual threshold for triggering reports 
      Range -20 to +20 in steps of 1 */


  /* Cell Reselection Timestamp for this cell (neighbor) */
  srch_evt_ts_struct_type  evt_cr_ts;

  /* Cell Reselection sorted index */
  uint16   cr_sort_inx;

  /* Cell Reselection search required flag, for weighting searches */
  boolean   cr_srch_reqd;

  /* Rank of this cell for cell reselection */
  int16 rank;

  /*this will be used to move cell out of the candidate set when it no longer */
  uint16 cand_age;

  /* HCS related info for the cell */
  l1_cell_hcs_info_struct_type cell_hcs_info;

  /* CELL_DCH Intra-frequency Event time stamps */
  srch_evt_ts_union_type   u_ts;
 
  /* CELL_DCH event rpt forbidden for the cell */
  srch_evt_barred_union_type  u_bar;

  /* CELL_DCH Intra-freqency event reporting status flag */
  srch_evt_rpt_union_type  u_rpt;

  /*  In the event of 1C, contains the cell on which 1C got triggered  */
  uint16 srch_evt_1c_src_psc;

  /* SFN-CFN time difference structure type. */ 
  srch_sfn_cfn_tm_diff_struct_type  sfn_cfn_tm_diff;

  /*  counter for applying noise floor to balance the ecio filtering  */
  uint16  undetected_cnt ;

  /* start position of search window */
  uint32  start_pos;  

  int16 cpich_tx_pwr;
  uint16 pathloss;
  uint16 pathloss_with_offset;
  int16 ecio_with_offset;
  int16 rscp_with_offset;


  uint16 earliest_rx_time;
  uint16 rx_tx_diff;
  srch_evt_6g_6f_struct_type cTRIGGERED_6F_EVENT;
  srch_evt_6g_6f_struct_type cTRIGGERED_6G_EVENT;

  boolean cell_for_measurement[SRCHDCH_MAX_INTRAF_EVT];

  /* Flag indicates whether a cell is barred or not*/
  boolean cell_barred;

  #ifdef FEATURE_DUAL_SIM
  /* The variable reflects the SFN in which aset search has been performed
   * Variable is used to perform filtering based upon the gap between two searches
   */
  uint32 srch_sfn;
  #endif

  #ifdef FEATURE_FEMTO_CSG
  uint32 csg_id;           /* for INTRA-F MACRO cells, RRC will set it to INVALID CSG ID (0xFFFFFFFF) */
  uint32 acq_sfn_extended; /* #define DL_BMC_MAX_EXTENDED_SFN 1048576 */
  int16  acq_rx_agc;       /* RxAGC value read during ASF search */
  int16  acq_ecio;         /* EcIo value read during ASF BPLMN search */
  int16  acq_rscp;         /* RSCP value read during ASF BPLMN search */
  boolean is_csg_resel_candidate;
  #endif
} srch_cell_struct_type;

/*--------------------------------------------------------------------------
                  BACKUP FDD CELL STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  /* WCDMA Cell Scrambling Code */
  uint16  scr_code;
    
  /* Penalty timer for this cell (neighbor) */
  srch_evt_ts_struct_type  evt_cr_penalty_timer;
} srch_backup_fdd_cell_struct_type;


/*--------------------------------------------------------------------------
                  BACKUP GSM CELL STRUCTURE
--------------------------------------------------------------------------*/

typedef struct 
{
/*
 * Common SIB & Measurement Control Info
 */
  l1_gsm_cell_info_type   gsm_info;     
    /* cell individ offset, bsic, band_indicator, arfcn */

  /* Penalty timer for this cell (neighbor) */
  srch_evt_ts_struct_type  evt_cr_penalty_timer;
} srch_backup_gsm_cell_struct_type;


/*--------------------------------------------------------------------------
                  BACKUP CELL STRUCTURE
--------------------------------------------------------------------------*/
/* Enum to indicate the state of backup structure */
typedef enum
{
  SRCH_BACKUP_INVALID,
  SRCH_BACKUP_UPDATED,
  SRCH_BACKUP_VALID
} srch_backup_info_state_type;


typedef struct
{
  /* Field indicating state of backup structure */
  srch_backup_info_state_type  cell_backup_state;

  /* Time of backup in seconds since power up*/
  dword    cell_backup_time;

  /* FDD frequeny info for the fdd cell info structure */
  uint16 fdd_arfcn[1 + L1_MAX_NON_USED_FREQ];

  /* End index in the fdd cell info structrue for a particular frequency */
  uint16 fdd_end_index[1 + L1_MAX_NON_USED_FREQ];

  /* FDD cell info to be backed up */
  srch_backup_fdd_cell_struct_type  fdd_cell_info [L1_MAX_CELL_MEAS + L1_MAX_CELL_MEAS];

  /* GSM cell info to be backed up */
  uint16 gsm_cell_cnt;
  srch_backup_gsm_cell_struct_type gsm_cell_info[L1_MAX_CELL_MEAS];
} srch_backup_cell_struct_type;


/*--------------------------------------------------------------------------
                         ASET UPDATE COMMAND

Used  to update Active Set.  This request is used for notifying Search of 
the initial cell selection (cell camp) result, as well as for
macro-diversity (soft handoff) in DCH state.
--------------------------------------------------------------------------*/
/* New structure to hold CPICH related info. Previously, this was part of 
 * the dl_rl_db structure */ 
typedef struct
{
  uint16 pri_scr_code; 
  /* the primary scrambling code */
  boolean tx_div_ind;
  /* transmit diversity usage */
  boolean txd_nw_informed;
  /* TxD NW informed indicator */
  boolean use_pri_cpich;
  /* whether to use the Primary CPICH for channel estimation */
  boolean sec_cpich_info_changed;
  /* Whether secondary CPICH info changed for this RL */
  boolean sec_cpich_present;  
  /* whether S-CPICH to be used */ 
  uint16 sec_cpich_ovsf;  
  /* the ovsf code for the secondary pilot if it's used */  
  uint16 sec_cpich_sec_scr_code; 
  /* the secondary CPICH secondary scrambling code */
  uint8 sec_scr_index;
  /* sec scr code index */
  uint8 tpc; 
  /* the TPC index */
  uint32 candidate_pn_pos;
  /* candidate pn pos*/
  boolean sec_cpich_enabled_mimo_mode;
  /*sec cpich enabled for mimo*/
  uint16 sec_cpich_scr_code_mimo_mode;
  /*sec cpich scr code for mimo*/
  uint16 sec_cpich_ovsf_mimo_mode;
  /*sec cpich ovsf code for mimo*/
  uint16 sec_cpich_imb_scale;
  /*sec cpich imb scale*/
} cpich_info_struct_type;


typedef struct
{
  /* Number of cells to be demodulated */
  uint16 aset_size;

 /* Boolean to indicate if measurements are enabled*/
  boolean measurements_enabled;

  /* WCDMA frequency */
  uint16 freq;

  /* WCDMA band */
  l1_freq_scan_band_enum_type band;

  /* Activation status of the carrier */
  boolean activation_status;

  cpich_info_struct_type cell[SRCH_ASET_MAX_C0];

} srch_aset_struct_type;


typedef enum
{
  ADD_CELL,
  DROP_CELL
} srch_nset_op_type;


typedef struct
{
  /* WCDMA frequency */
  uint16 freq;

  /* Number of cells to add */
  srch_nset_op_type  op;
  uint16 psc; 

} srch_nset_struct_type;


typedef struct srch_gsm_cell_struct
{
/*
 * Common SIB & Measurement Control Info
 */
  uint16                  cell_id;     
    /* Assigned Inter-RAT cellid */
  
  uint16                  rssi_age;
    /* Indicates the time when the last rssi measurements were taken */

  
  /* Check to see if rssi value is used atleast once for event based reporting
     before aging starts */
  boolean                 rssi_used[L1_MAX_INTERRAT_EVENTS];
    
  /* Check to see if rssi value is used atleast once for periodic reporting
     before aging starts */
  boolean                 rssi_used_periodic;

  uint8 idx;

  srch_gsm_set_enum_type  set;  
  srch_gsm_set_enum_type  prev_set;   

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
  /* Indicates if priority info present for this ARFCN */
  boolean prior_info_present;

  boolean cell_qualify_for_prio;

  /* Priority information for this ARFCN */
  srch_inter_rat_gsm_priority_info_type prior_info;
#endif

  l1_gsm_cell_info_type   gsm_info;     
    /* cell individ offset, bsic, band_indicator, arfcn */

  /* Indication if the bsic has been verified: 
   *   TRUE if it has, 
   *   FALSE otherwise */  
  boolean                 bsic_verified;

  boolean  bsic_verify_reqd;
    /* BSIC Verification Required, or Not */

/*
 * Measurement Control CELL_DCH, Compressed Mode Specific Information 
 */
  uint16 measid;
    /* measurement control measid which 'added' this cell */
    /* DJM - don't think this is needed, but RRC seems to claim that release of */
    /*       measid implies removal of cells created by that measid... CELL_DCH */

  void * cm_drv_cell_ptr;
    /* Compressed Mode driver Cell Pointer, valid if measure_cell == TRUE after addition */

  uint8  fn_offset;
  uint16 qbit_offset;
  uint32 hyper_frame_offset;
    /* GSM Cell Timing Information, available when bsic_verified==TRUE */

  boolean rssi_valid;
  int16   last_rssi;
    /* last measured rssi */
  int32 filtered_rssi[L1_MAX_FILTER_COEF_STORAGE];
    /* if filter_coef=INVALID, value set to last_rssi, 
     otherwise Fn as in 8.6.7.2 
   */

  boolean cell_reported[L1_MAX_MEAS_EVENT];
  struct  srch_gsm_cell_struct * sort_next;

/*
 * SIB Information
 */
  boolean  gsm_sib_info_signaled;
    /* this variable determines whether GSM SIB Information was 
       signalled by UTRAN 
     */

  /* The following fields consist of Cell Selection Re-Selection  */
  /* Info   --- Corresponding to 10.3.7.4 of TS25.331 */  

  int16        qoffset_s_n;    
    /* Offset used for RSCP. Integer (-50..50), Default 0 */  
  int16        max_tx_pwr;    
    /* Maximum allowed ULTX power. UE_TXPWR_MAX_RACH dBm */  
  int16        rxlev_min;    
    /* Minimum required RX level in the cell (dBm) */  

  /* Cell selection values evaluated by SRCH and to be reported to RRC */  
  int16    s_rxlev;  

  /* Rank of this cell as neighbor for interrat cell reselection */  
  int16 rank;
  
  /* Cell Reselection Timestamp for this cell (neighbor) */
  srch_evt_ts_struct_type  evt_cr_ts;

  /* HCS related info for the cell */
  l1_cell_hcs_info_struct_type cell_hcs_info;

  /* Measurements made during cell reselection */
  srchcr_gsm_meas_struct_type cell_resel_meas;

  /* Flag indicates whether a cell is barred or not*/
  boolean cell_barred;

  /* this information is for a forbidden QS offset for this cell
     It is reset at entry in to BSIC Id state and also reset
     when coming back from BSIC Id aborted */
  uint8  num_forbidden_offset;
  uint8  forbidden_frame_offset[SRCH_GSM_NUM_MAX_FORBIDDEN_OFFSET];
  uint16 forbidden_qs_offset[SRCH_GSM_NUM_MAX_FORBIDDEN_OFFSET];

  /* If this cell is to be included for rssi measurements. Only one 
     arfcn measurement suffice. So we take cell for RSSI measurements 
     only when this flag is TRUE*/
  boolean unique_arfcn;
  
  /* freq offset */ 
  int16  frequency_offset;
} srch_gsm_cell_struct_type;

/***************************************************************************

    D A T A     S T R U C T U R E S 
    
 ***************************************************************************/
/* Make the number of interf measurement from 7 to 8 because it was required
by some some n/w configurations */
#define SRCH_MAX_INTERF_MEAS 8
#define SRCH_NO_INTERF_MEAS_ID (0xFF)
/*--------------------------------------------------------------------------
                       Inter-Frequency MACROS 
--------------------------------------------------------------------------*/

#define INTERF_DEFAULT_K_DCH 0 /* No filtering*/
#define INTERF_DEFAULT_K_FACH 1
#define INTERF_INVALID_K_VAL -1
#define SRCHCM_INTERF_MEAS_PRD_MSECS 480L 
#define INTERF_MAX_K_VALUE 19
#define INTERF_VALUES_STORED_PER_K 2
#define INTERF_INVALID_ALPHA -1
#define INTERF_INVALID_BETA -1
#define INTERF_ALPHA_INDEX 0
#define INTERF_BETA_INDEX 1
#define INTERF_ALPHA_MULT_FACTOR_16384 16384
#define INTERF_BETA_MULT_FACTOR_128 128
#define INTERF_LOG_INTERMEDIATE_MAX 62
#define INTERF_SCALE_UNIT_1 1
#define INTERF_LOG_FINAL_MAX 44

#define INTERF_FILTER_COEFF_VALIDATE(k) \
            ((k>INTERF_MAX_K_VALUE)||(srch_interf_filter_coef_tab[k][INTERF_ALPHA_INDEX]==\
            INTERF_INVALID_ALPHA)) ? FALSE:TRUE
#define INTERF_FILTER_CALC_NUME(time_since_last_meas) \
            (time_since_last_meas*(int64)INTERF_BETA_MULT_FACTOR_128)
#define INTERF_FILTER_CALC_DENO(filt_len_param, time_since_last_meas) \
            ((time_since_last_meas*(int64)INTERF_BETA_MULT_FACTOR_128)+\
            (uint32)srch_interf_filter_coef_tab[filt_len_param][INTERF_BETA_INDEX]*\
            (int64)SRCHCM_INTERF_MEAS_PRD_MSECS)
            

#define  SRCH_OFREQ_FREQ_MAX  2
  /* The max number of WCDMA inter-freuency channels the mobile searches */

#define  SRCH_OFREQ_CELL_MAX  32
  /* The max number of cells specified for an inter-frequency WCDMA channel */
  
   

/* The max number of intra-freq events that are evaluated to update VAset */
#define SRCH_MAX_INTRA_FREQ_EVENTS_ON_VASET 3

/* enumeration of all the events that are evaluated to update VAset */
typedef enum
{
  VASET_EVT_1A,
  VASET_EVT_1B,
  VASET_EVT_1C
}
srch_interf_vaset_evt_enum_type;


/* VAset event structure for 1A, 1B, 1C */
typedef struct
{
  /* Flag indicating if the event time stamp is enabled  */
  boolean  enabled;
  /* Time when the event first becomes TRUE */  
  uint16 time;    
}
srch_interf_vaset_evt_struct_type;


/* The number of cells to be included in the measured results */
typedef struct 
{
    uint16 num_vaset_cells;
    uint16 num_monitored_set_cells;
} srch_interf_cell_meas_cnt_struct_type;


/*--------------------------------------------------------------------------
                        INTER FREQ SEARCH CELL STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  /* WCDMA Cell Scrambling Code */
  uint16  scr_code;
    
  /* WCDMA frequency assignment for the cell */
  uint16  freq;
  
  /* Current carrier index of cell */
  uint8 carr_idx;

  /* TRUE if the CPICH of this cell uses STTD */
  boolean sttd;
          
  /* Which Set the Cell currently belongs to */
  srch_interf_set_enum_type  set;
  srch_interf_set_enum_type  prev_set;
    
  uint16  path_thresh;
  /* path threshold for this cell */

  /* The total energy and Ec/Io in the best paths found in a sweep */
  uint32  tot_eng;
  uint16  ecio;

  /* Filtered energy and Ec/Io and Filter length used currently */
  uint8 idx;

  uint32  filt_eng;
  uint16  filt_ecio;
  uint16  filt_len;

  /* filtered values using intra-freq filter coefficient. 
     Used to evaluate VAset events 1A, 1B, 1C */
  uint32 intraf_coeff_filt_eng;
  uint16 intraf_coeff_filt_ecio;
  uint16 intraf_coeff_filt_rscp;

#ifdef FEATURE_WCDMA_DC_HSUPA
  uint16 filt_len_rscp;
#if (defined(FEATURE_REL5) && defined(FEATURE_WCDMA_LOG_FILTERING))
  uint16  filt_ecio_hi_res;
  uint16 filt_rscp_hi_res;
#endif  
#endif

  /* CPICH RSCP and filtered RSCP */
  uint16  rscp;

  /*Running a 15s timer to remove the cell if the energy goes lower than threshold*/
  int32 ecio_drop_thresh_timer_value;
	
  /*Used to decrement the timer value*/
  uint32 prev_eval_drx_cycle_num ; 

  /*Saving RxAGC value for FR 1947 Neighbour Search Optimizations */
  int16 rx_agc;

    /* It is RxAGC + Filtered Ec/Io. The actual CPICH RSCP is "-rscp" */
  uint16  filt_rscp;
  int16   rssi_dbm;
  /* Flag indicating if the cell has been evaluated */
  boolean evaled;

  /* the index of the radio link set that this cell belongs to */
  uint8 rl_set_idx;

  /* positions of the maximum energy paths in 1/8 chip units. */
  uint32  max_pos[ SRCH_MAX_PILOT_PEAKS ];
    
  /* Energies of the maximum energy paths */
  uint32  max_eng[ SRCH_MAX_PILOT_PEAKS ];

  /* positions of the maximum energy paths in 1/8 chip units. */
  uint32  max_pos_div[ SRCH_MAX_PILOT_PEAKS ];

  /* Energies of the maximum energy paths */
  uint32  max_eng_div[ SRCH_MAX_PILOT_PEAKS ];
 
  
  /* The position of the path used for centering searching window. Normally,
     it is the earliest usable (or above threshold SRCH_PATH_THRESHOLD) path.
     
     Valid value ranges (0..307200) in CHIPx8. 0xFFFFFFFF (or SRCH_NO_REF_POS)
     implies that ref_pos is not being set to a valid value.
   */
  uint32  ref_pos;

  /* variable to tell  if it can reliably use results from primary
   * or diversity antenna
   */
  boolean  srch_result_valid[MAX_NUM_ANTENNA];

  /* The age of the above reference position, in units of search period. 
     
     At time ref_pos is updated with a new search result, ref_age is set to 0.
   */
  int16   ref_age; 
 
  /* Reference Time difference to Serving cell, in chips. 
     0xFFFF indicates the field is not included. This field is 
     either obtained from a SIB message or via mobile measurement.
   */
  uint16  time_diff;
    
  /* Neighbor Cell timing accuracy relative to the serving cell.
     This will be translated to "search window size" for the neighbor.
   */
  l1_cell_time_acc_enum_type time_acc;

  /* flag indicating if the cell's SFN decoding is requested for 
     measurement.
   */
  boolean read_sfn;

    /* Cell Reselection search required flag, for weighting searches */
  boolean   cr_srch_reqd;

  /* Quality offsets to serving cell for cell reselection */
  int16  q_offset1;
  int16  q_offset2;

  /* Minimum qualities */
  int16  qual_min;
  int16  rxlev_min;
  int16  max_tx_pwr;

  /* Cell selection values evaluated by SRCH and to be reported to RRC */
  int16    s_qual;
  int16    s_rxlev;

  int16 cell_offset;
  /*  cell individual offset Refer to 10.3.7.2 and 14.1.5.3 
      for an explanation of this parameter. 
      Used to set invidual threshold for triggering reports 
      Range -20 to +20 in steps of 1 */

  int16 cpich_tx_pwr;
  uint16 pathloss;
    /*  Primary CPICH Tx power. Needed if calculating patchloss
        Units: Int  -10... +50 dBm */

  /* CELL_DCH Intra-frequency Event time stamps */
  srch_evt_ts_union_type   u_ts;
 

  /* VAset event maintainance */
  srch_interf_vaset_evt_struct_type vaset_evt[SRCH_MAX_INTRA_FREQ_EVENTS_ON_VASET];


    /* CELL_DCH event rpt forbidden for the cell */
  srch_evt_barred_union_type  u_bar;

  /* CELL_DCH Intra-freqency event reporting status flag */
  srch_evt_rpt_union_type  u_rpt;


  /* Flag indicating whether the cell is barred*/
  boolean cell_barred;

  /* SFN-CFN time difference structure type. */ 
  srch_sfn_cfn_tm_diff_struct_type  sfn_cfn_tm_diff;

  /* Pointer to CM cell */
  srchcm_interf_cell_struct_type *srch_cm_cell_ptr ;

  /* Search window size for this cell */
  uint32  win_size;

  /* Cell Reselection sorted index */
  uint16   cr_sort_inx;



  /* Cell Reselection time stamp. Enabled when the neighbor cell is
     significantly stronger than the ASET cell.
   */
  srch_evt_ts_struct_type  evt_cr_ts;

  /* Rank of this cell for cell reselection */
  int16 rank;

  /* HCS related info for the cell */
  l1_cell_hcs_info_struct_type  cell_hcs_info;

  #ifdef FEATURE_FEMTO_CSG
  uint32 csg_id;           /* for INTRA-F MACRO cells, RRC will set it to INVALID CSG ID (0xFFFFFFFF) */
  uint32 acq_sfn_extended; /* #define DL_BMC_MAX_EXTENDED_SFN 1048576 */
  int16  acq_rx_agc;       /* RxAGC value read during ASF search */
  int16  acq_ecio;         /* EcIo value read during ASF BPLMN search */
  int16  acq_rscp;         /* RSCP value read during ASF BPLMN search */
  boolean is_csg_resel_candidate; 
  #endif

}srch_interf_cell_struct_type;


#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/* The following information is the inter frequency priority information
   that was revieved from RRC via the 3rd MEAS_REQ/SIB19 */
typedef struct
{
  int8 priority;
  uint8 threshx_high; 
  uint8 threshx_low; 
  int16 q_qualmin_fdd; 
  int16 q_rxlevmin_fdd;
} srch_inter_freq_priority_info_type;
#endif

typedef struct
{
  /*RxAGC is needed for evals done after offline NASTT in case of G2W TA. The AGC from RF 
  is saved before issuing NASTT since SS MGR will put RF to sleep and release the lock for G-tech*/ 
  int16   rx_agc;
  uint16  freq;
    /* Frequency specification */
  uint8   cell_cnt;

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
  boolean prior_info_present;
  /* Indicates if priority information has been provided for this frequency */

  srch_inter_freq_priority_info_type prior_info;
  /* Priority information for this frequency */
#endif
    /* Number of cells listed on this frequency */
  srch_interf_cell_struct_type *cell_ptr[SRCH_OFREQ_CELL_MAX];
    /* Pointers to cell table entries */
}
srch_ofreq_cell_list_struct_type;


typedef enum
{
    eIF_TTT_DISABLED,
    eIF_TTT_ENABLED,
    eIF_TTT_REPORTED
} srch_interf_evt_ttt_enum_type;

    
/* For event 2A, we need to maintain best freq */
typedef struct {
      /* Frequency */
      uint16 freq;
      /* Measured value */
      int16 meas_val;

} srch_interf_evt_2a_type;
    

typedef struct {
        /* Event id */
        l1_inter_freq_event_enum_type evt_id ;

        /* Threshold used for event 2B, 2D or 2F, in dB */
        int16  used_thresh;
 
        /* Hysteresis. 0..15 (0..7.5dB) by step of 0.5 dB */
        uint16 hyst;

        /* Weight for even 2A,2B,2D and 2F. 0..20 by step of 0.1. 
           set to 0xFFFF for all other events. */
        uint16 w;

        /* Time-to-trigger in 10ms units */
        uint16 trig_timer;

        /* Non-used freq parameters */

        /* Threshold non-used */
        int16 non_used_thresh;

        /* W non-used in steps of *10 */
        uint16 w_non_thresh;

} srch_interf_evt_trig_struct_type ; 

typedef struct {
    /* measurement id */
    uint16 meas_id;
    /* reporting mode */
    l1_inter_freq_meas_report_mode_enum_type rpt_mode;

    /* layer 3 filter_coeff  */
    uint16 layer3_filter_coeff;

    /* filter_idx- Can be 0,1 or 2 */
    uint16 filt_idx;

    boolean meas_quan_incl;

    /* Quantity Measured. Note that, for WCDMA FDD cells, we currently support
         Ec/Io or RSCP Measurement Quantity for DCH Measurement report. */
    l1_meas_quan_enum_type   intraf_quan_type;

    l1_meas_quan_enum_type   interf_quan_type;


    /*  Following variable indicates the current Best Frequency among the
   serving frequency and (upto) two non-used frequencies.
   Upon entering the DCH state, this value is initialized with the
   current serving cell */
    srch_interf_evt_2a_type freqBEST_FREQUENCY_2A_EVENT;

    struct {
      /* Number of reports currently sent */
      uint16 periodic_rpt_cnt;

      /* timestamp in units of 10 ms. Used for determining TTT */
      uint16 ts_cnt;
      /* Number of reports requested */
      uint16 num_reports_req;
      /* Reporting interval, in multiples of 10 ms */
      uint16 reporting_interval;
    } periodic_s;

    /* Even triggered structure */
    srch_interf_evt_trig_struct_type  evt_trig_s;

    /* Tracks the current state of the measurement */
    srch_interf_evt_ttt_enum_type evt_ttt[SRCH_OFREQ_FREQ_MAX];
    
    /* For event triggered reporting */
    boolean meas_reported[SRCH_OFREQ_FREQ_MAX] ;

    uint16 ts_cnt[SRCH_OFREQ_FREQ_MAX];
    /* common to both evt trig and periodic meas reporting */
    boolean rpt_cell_status_incl;
    l1_meas_rpt_cell_status_struct_type  rpt_cell_status;

    boolean rpt_quan_incl;

    /* Reporting quantities */
    l1_inter_freq_rpt_quan_struct_type    rpt_quan;
    
    /* Measuremnt Report transfer criteria to UTRAN */
    l1_meas_report_trans_mode_enum_type  rpt_trans_mode;

    /* For event triggered, maintain a TRIGGERED_EVENT sub structure.
       This is required so that we can report all the cells in here
       every time a cell is reported.
        */
    uint16 num_freq_triggered;
    /* scr code */
    uint16 freq_triggered[SRCH_OFREQ_FREQ_MAX];
    boolean stopped;

    /* This flag indicates if VAset is configured in the current MCM */
    boolean vaset_configured;

} srch_inter_freq_meas_parms_struct_type;

/*--------------------------------------------------------------------------
                      INTER-FREQUENCY CELL LIST STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  uint16  freq;
    /* Frequency specification */
  uint8   cell_cnt;
    /* Number of cells listed on this frequency */
  srch_cell_struct_type *cell_ptr[SRCH_USET_MAX_C0];
    /* Pointers to cell table entries */
} srch_ofreq_uset_struct_type;

/* Representation of a basic set */
typedef struct
{
  /* Set being represented by this structure */
  srch_set_enum_type set;

  /* Current size of the set */
  uint8 cnt;
  
  /* Maximum size of the set */
  uint8 max;
  
  /* Cell array */
  srch_cell_struct_type** cell;
} srch_set_struct_type;

/* Representation of a composite set */
typedef struct
{
  /* Set being represented by this structure */
  srch_comp_set_enum_type set;

  /* Current size of the set */
  uint8 cnt;
  
  /* Maximum size of the set */
  uint8 max;
  
  /* Cell array */
  srch_cell_struct_type** cell;
} srch_comp_set_struct_type;

/* Representation of an inter-frequency set */
typedef struct
{
  /* Set being represented by this structure */
  srch_interf_set_enum_type set;

  /* Current size of the set */
  uint8 cnt;
  
  /* Maximum size of the set */
  uint8 max;
  
  /* Cell array */
  srch_interf_cell_struct_type** cell;
} srch_interf_set_struct_type;


/* Representation of a GSM set */
typedef struct
{
  /* Set being represented by this structure */
  srch_gsm_set_enum_type set;

  /* Current size of the set */
  uint8 cnt;
  
  /* Maximum size of the set */
  uint8 max;
  
  /* Cell array */
  srch_gsm_cell_struct_type** cell;
} srch_gsm_set_struct_type;


/* Representation of a group of sets on a specific carrier */
typedef struct
{  
  /* Basic sets */
  srch_set_struct_type* set[SET_MAX];
  
  /* Composite sets */
  srch_comp_set_struct_type* comp[COMP_SET_MAX];
} srch_carr_struct_type;


/* Representation of the entire search set state */
typedef struct
{
  /* Basic/composite cell pool */
  srch_cell_struct_type* pool[SRCHSET_NUM_CELLS];
 
  /* Inter-frequency cell pool */
  srch_interf_cell_struct_type* interf_pool[SRCHMEAS_MAX_NUM_CARR][SRCHSET_NUM_INTERF_CELLS];  
   
  /* Inter-frequency sets */                           
  srch_interf_set_struct_type* interf[SRCHMEAS_MAX_NUM_CARR][INTERF_SET_MAX];    

  /* GSM cell pool */
  srch_gsm_cell_struct_type* gsm_pool[SRCHSET_NUM_GSM_CELLS];    
  
  /* GSM sets - only one carrier's worth of cells */   
  srch_gsm_set_struct_type* gsm[GSM_SET_MAX];

  /* Groups of sets arranged by their carrier */
  srch_carr_struct_type carr[NUM_CARR];
} srchset_state_struct_type;

typedef struct{
  uint16  dl_freq;
  boolean initial_vaset_setup;  
}srch_interf_vaset_freq_array_struct_type;

/* Max number of pending delete cells for ASU: 1 DC ASET + 2 QICE NSET */
#define SRCH_PEND_DELETE_SET_MAX 10

typedef enum
{
  SRCHSETUTIL_BASIC_SET_SANITY,
  SRCHSETUTIL_COMP_SET_SANITY,
  SRCHSETUTIL_INTERF_SET_SANITY,
  SRCHSETUTIL_SANITY_MAX
} srchsetutil_sanity__enum_type;
/*=============================================================================
  EXTERNALIZED VARIABLES
=============================================================================*/

/* Set protector mutex */
extern rex_crit_sect_type srchsetutil_crit_sect;

/* Tracks the psc of the current and previous reference cells */
extern uint16 ue_ref_cell_psc;
extern uint16 ue_prev_ref_cell_psc;

/* Flag to indicate whether DPCH offsets have been adjusted */
extern boolean dpch_off_not_adjusted;

/* boolean to determine if we need to choose a new ref cell  */
extern boolean choose_ref_cell;

/* Offset of original and the current reference cell in 256 chip unit */
extern uint8  delta_ref_cell_from_orig_cell;

/* Snapshot of reference PSC during HHO */
extern uint16 ue_saved_ref_cell_psc_in_hho;

#ifdef FEATURE_WCDMA_DC_HSDPA
/* Data structure to store the sorted R99 neighbors for QICE */
extern uint8                     srch_pri_carr_qice_sorted_cnt;
extern srch_cell_struct_type     *srch_pri_carr_qice_sorted_ptr[SRCH_SYNC_NSET_MAX_C0];

#ifdef FEATURE_WCDMA_HS_FACH
/* Data structure to store the sorted R99 neighbors for QICE in FACH */
extern srch_cell_struct_type  *srch_prim_nbr_ptr[SRCH_PRI_NBRS_MAX];
extern uint8                  srch_prim_nbr_cnt;
#endif

/* Data structure to store the sorted DC neighbors for QICE */
extern uint8                     srch_sec_carr_qice_sorted_cnt[WL1_MAX_VALID_CARRIER_IDX];
extern srch_cell_struct_type     *srch_sec_carr_qice_sorted_ptr[WL1_MAX_VALID_CARRIER_IDX][SRCH_SYNC_NSET_MAX_C1];

#endif /* FEATURE_WCDMA_DC_HSDPA */

/* Cell pointers to a cell table entries for the new ASET */
extern srch_cell_struct_type* new_aset_ptr[SRCH_ASET_MAX_C0];
extern uint8                  new_aset_cnt;

/* Cell Table pointers for those cells sent for PN srch */
extern srch_cell_struct_type  *srch_pn_srch_cell_ptr[SRCH_NORMAL_MODE_MAX_PN_TASKS];   

/* Number of tasks for PN srch command */
extern uint16 srch_pn_srch_cell_cnt;    

extern uint8 srch_triage_cnt[NUM_CARR];
extern srch_cell_struct_type* srch_triage_ptr[NUM_CARR][SRCH_TRIAGE_SET_MAX];

extern uint8 srch_sleep_set_cnt;
extern srch_cell_struct_type *srch_sleep_set[SRCH_TRIAGE_SET_MAX];

extern srch_cell_struct_type* srch_cr_cleanup_cell_ptr;

extern srch_cell_struct_type* srch_pullin_set_ptr[SRCH_PULLIN_SET_MAX];

/* indexes to cells for pull-in operation */
extern uint8 srch_pullin_set_cnt;

/*=============================================================================
  FUNCTION DECLARATIONS
=============================================================================*/

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_INIT
  
  DESCRIPTION
    This function allocates heap memory for the cell pool and initializies
    the cell pointers to their default values.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_init(uint8);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_DESTROY
  
  DESCRIPTION
    This function frees the heap memory and invalidats all the cell
    pointers in the pool.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_destroy(uint8);

/*=========================================================================
  FUNCTION
    SRCHSETUTIL_GET_INTLOCK
  
  DESCRIPTION
    This function tries to acquire the mutex srch_get_cell_crit_sect
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    The function can block until the mutex is available for locking  
=========================================================================*/
void srchsetutil_get_intlock(void);

/*========================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_INTFREE
  
  DESCRIPTION
    This function releases the mutex srch_get_cell_crit_sect.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None 
=========================================================================*/
void srchsetutil_get_intfree(void);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_ALLOC
  
  DESCRIPTION
    This function initializes a cell pointer from the empty set and
    attempts to move it into the specified basic set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    A pointer to the allocated cell on success, NULL on failure.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_cell_struct_type* srchsetutil_alloc(uint8 carr_idx, 
                                         srch_set_enum_type set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_ALLOC_INTERF
  
  DESCRIPTION
    This function initializes a cell pointer from the empty set and
    attempts to move it into the specified inter-frequency set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    A pointer to the allocated cell on success, NULL on failure.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_interf_cell_struct_type* srchsetutil_alloc_interf(uint8 carr_idx,srch_interf_set_enum_type set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_ALLOC_GSM
  
  DESCRIPTION
    This function initializes a cell pointer from the empty set and
    attempts to move it into the specified GSM set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    A pointer to the allocated cell on success, NULL on failure.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_gsm_cell_struct_type* srchsetutil_alloc_gsm(srch_gsm_set_enum_type set);  
  
/*=============================================================================
  FUNCTION
    SRCHSETUTIL_MOVE
  
  DESCRIPTION
    This function attempts to transition a cell from its current basic set
    to the specified basic set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the transition was successful, FALSE otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_move(srch_cell_struct_type* cell_ptr, 
                         srch_set_enum_type set); 

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_MOVE_COMP
  
  DESCRIPTION
    This function attempts to add a cell to the specified composite set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the addition was successful, FALSE otherwise. If the specified 
    cell already belongs to the specified composite set, this function will 
    return FALSE.
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_move_comp(srch_cell_struct_type* cell_ptr,
                              srch_comp_set_enum_type set);
                              
/*=============================================================================
  FUNCTION
    SRCHSETUTIL_MOVE_INTERF
  
  DESCRIPTION
    This function attempts to add a cell to the specified inter-frequency
    set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the addition was successful, FALSE otherwise. If the specified 
    cell already belongs to the specified inter-frequency set, this function 
    will return FALSE.
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_move_interf(srch_interf_cell_struct_type* cell_ptr,
                                srch_interf_set_enum_type set);                              

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_MOVE_GSM
  
  DESCRIPTION
    This function attempts to add a cell to the specified GSM set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the addition was successful, FALSE otherwise. If the specified 
    cell already belongs to the specified inter-frequency set, this function 
    will return FALSE.
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_move_gsm(srch_gsm_cell_struct_type* cell_ptr,
                             srch_gsm_set_enum_type set);  


/*=============================================================================
  FUNCTION
    SRCHSETUTIL_MOVE_ALL
  
  DESCRIPTION
    This function attempts to move every cell in the from_set to the to_set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the addition was successful, FALSE otherwise.
 
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_move_all(uint8 carr_idx, 
                             srch_set_enum_type from_set, 
                             srch_set_enum_type to_set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_MOVE_ALL_INTERF
  
  DESCRIPTION
    This function attempts to move every cell in the from_set to the to_set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the addition was successful, FALSE otherwise.
 
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_move_all_interf(uint8 carr_idx,srch_interf_set_enum_type from_set, 
                                    srch_interf_set_enum_type to_set);
/*=============================================================================
  FUNCTION
    SRCHSETUTIL_REMOVE
  
  DESCRIPTION
    This function attempts to transition the specified cell from its basic
    set to the empty set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the removal was successful, FALSE otherwise. If the specified 
    cell already belongs to the empty set, this function will return FALSE.
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_remove(srch_cell_struct_type* cell_ptr);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_REMOVE_COMP
  
  DESCRIPTION
    This function attempts to remove the specified cell from the specified
    composite set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the removal was successful, FALSE otherwise. If the specified 
    cell does not belong to the specified set, this function will return 
    FALSE.
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_remove_comp(srch_cell_struct_type* cell_ptr,
                                srch_comp_set_enum_type set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_REMOVE_INTERF
  
  DESCRIPTION
    This function attempts to remove the specified cell from the specified
    inter-frequency set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the removal was successful, FALSE otherwise. If the specified 
    cell does not belong to the specified set, this function will return 
    FALSE.
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_remove_interf(srch_interf_cell_struct_type* cell_ptr);
                                
/*=============================================================================
  FUNCTION
    SRCHSETUTIL_REMOVE_GSM
  
  DESCRIPTION
    This function attempts to remove the specified cell from the specified
    GSM set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the removal was successful, FALSE otherwise. If the specified 
    cell does not belong to the specified set, this function will return 
    FALSE.
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_remove_gsm(srch_gsm_cell_struct_type* cell_ptr);                                
                                
/*=============================================================================
  FUNCTION
    SRCHSETUTIL_REMOVE_ALL
  
  DESCRIPTION
    This function attempts to move every element of the specified basic set 
    to the empty set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the removal was successful for every cell, FALSE otherwise. If
    any cell in the specified set belongs to a composite set, then this 
    function will return FALSE and perform no operation.
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_remove_all(uint8 carr_idx, srch_set_enum_type set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_REMOVE_ALL_OTHER_FREQ

  DESCRIPTION
    Removes cells of frequency other than the specified frequency

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_remove_all_other_freq(uint8 carr_idx, uint16 freq, uint16 bmsk);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_REMOVE_ALL_COMP
  
  DESCRIPTION
    This function drops every cell from the specified composite set. It has 
    no effect on the cells' basic set association.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_remove_all_comp(uint8 carr_idx, srch_comp_set_enum_type set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_REMOVE_ALL_INTERF
  
  DESCRIPTION
    This function drops every cell from the specified inter-frequency set. 
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_remove_all_interf(uint8 carr_idx, srch_interf_set_enum_type set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_REMOVE_ALL_GSM
  
  DESCRIPTION
    This function drops every cell from the specified GSM set. 
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_remove_all_gsm(srch_gsm_set_enum_type set);

/*=============================================================================
  FUNCTION
    srchsetutil_get_set_count
  
  DESCRIPTION
    This function retuns the cardinality of the specified basic set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    The number of cells in the specified basic set. If the specified set is
    invalid, this function will return 0.
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_set_count(uint8 carr_idx, srch_set_enum_type set);

/*=============================================================================
  FUNCTION
    srchsetutil_get_composite_count
  
  DESCRIPTION
    This function retuns the cardinality of the specified composite set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    The number of cells in the specified composite set. If the specified
    set is invalid, this function will return 0.
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_composite_count(uint8 carr_idx, srch_comp_set_enum_type set);

/*=============================================================================
  FUNCTION
    srchsetutil_get_interf_count 
  
  DESCRIPTION
    This function retuns the cardinality of the specified inter-frequency set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    The number of cells in the specified inter-frequency set. If the specified
    set is invalid, this function will return 0.
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_interf_count(uint8 carr_idx, srch_interf_set_enum_type set);

/*=============================================================================
  FUNCTION
    srchsetutil_get_gsm_count
  
  DESCRIPTION
    This function retuns the cardinality of the specified GSM set.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    The number of cells in the specified GSM set. If the specified set is
    invalid, this function will return 0.
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_gsm_count(srch_gsm_set_enum_type set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_SEARCH
  
  DESCRIPTION
    This cell searches the basic sets for a cell matching the specified
    frequency and scrambling code. The bitmask parameter defines the sets 
    to search through. The sets are specified by 1 shifted left by the 
    enumerated value of srch_set_enum_type.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    A pointer to the matched cell if it was found, NULL otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_cell_struct_type* srchsetutil_search(uint8  carr_idx,
                                          uint16 freq, 
                                          uint16 scr_code,
                                          uint16 bmsk);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_SEARCH_COMP
  
  DESCRIPTION
    This cell searches the composite sets for a cell matching the specified
    frequency and scrambling code. The bitmask parameter defines the sets to 
    search through. The sets are specified by 1 shifted left by the enumerated 
    value of srch_comp_set_enum_type.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    A pointer to the matched cell if it was found, NULL otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_cell_struct_type* srchsetutil_search_comp(uint8  carr_idx,
                                               uint16 freq, 
                                               uint16 scr_code,
                                               uint8  bmsk,
                                               srch_comp_set_enum_type* set);


/*=============================================================================
  FUNCTION
    SRCHSETUTIL_SEARCH_INTERF
  
  DESCRIPTION
    This cell searches the inter-frequency sets for a cell matching the 
    specified frequency and scrambling code. The bitmask parameter defines 
    the sets to search through. The sets are specified by 1 shifted left by
    the enumerated value of srch_set_enum_type.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    A pointer to the matched cell if it was found, NULL otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_interf_cell_struct_type* srchsetutil_search_interf(uint8 carr_idx,
                                                        uint16 freq, 
                                                        uint16 scr_code,
                                                        uint16 bmsk);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_SEARCH_GSM
  
  DESCRIPTION
    This cell searches a the GSM sets for a cell matching the specified
    frequency and scrambling code. 
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    A pointer to the matched cell if it was found, NULL otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_gsm_cell_struct_type* srchsetutil_search_gsm(uint16 cell_id);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_GET_SERVING_CELL
  
  DESCRIPTION
    This functions returns the serving cell on the specified carrier, if
    present.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    A pointer to the serving cell if present, NULL otherwise
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_cell_struct_type* srchsetutil_get_serving_cell(uint8 carr_idx);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_PRINT
  
  DESCRIPTION
    This debug function prints the cells in the specified basic set in
    MSG_HIGHs.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_print(uint8 carr_idx, srch_set_enum_type set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_PRINT_COMP
  
  DESCRIPTION
    This debug function prints the cells in the specified composite set in
    MSG_HIGHs.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_print_comp(uint8 carr_idx, srch_comp_set_enum_type set);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_PRINT_INTERF
  
  DESCRIPTION
    This debug function prints the cells in the specified inter-frequency set
    in MSG_HIGHs.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_print_interf(srch_interf_set_enum_type set);

/*=============================================================================
  FUNCTION
    srchsetutil_print_gsm
  
  DESCRIPTION
    This debug function prints the cells in the specified GSM set in
    MSG_HIGHs.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_print_gsm(srch_gsm_set_enum_type set);



/****************************** UTILITY FUNCTIONS ****************************/

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_DROP_WEAKEST_CELL
  
  DESCRIPTION
    This function returns the cell with the maximum ref age in the
    specified set having no service to SET_E. 
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if a cell was dropped, FALSE otherwise.
  
  SIDE EFFECTS
    Moves the weakest cell to SET_E if possible
=============================================================================*/
boolean srchsetutil_drop_weakest_cell(uint8 carr_idx, 
                                      srch_set_enum_type from_set, 
                                      srch_set_enum_type to_set);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_BASE_PTR
  
  DESCRIPTION
    This function returns a pointer to the underlying set array.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    Pointer to the specified set of cells if the given set exists on the
    specified carrier, NULL otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_cell_struct_type** srchsetutil_get_base_ptr(uint8 carr_idx, 
                                                 srch_set_enum_type set);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_COMP_BASE_PTR
  
  DESCRIPTION
    This function returns a pointer to the underlying composite set array.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    Pointer to the specified set of cells if the given set exists on the
    specified carrier, NULL otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_cell_struct_type** srchsetutil_get_comp_base_ptr(uint8 carr_idx, 
                                                      srch_comp_set_enum_type set);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_INTERF_BASE_PTR
  
  DESCRIPTION
    This function returns a pointer to the underlying inter-frequency set
    array.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    Pointer to the specified set of cells if the given set exists on the
    specified carrier, NULL otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_interf_cell_struct_type** srchsetutil_get_interf_base_ptr(uint8 carr_idx,srch_interf_set_enum_type set);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_GSM_CELL_FROM_SET
  
  DESCRIPTION
    This function returns a pointer to the underlying GSM set array.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    Pointer to the specified set of cells if the given set exists on the
    specified carrier, NULL otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_gsm_cell_struct_type * srchsetutil_get_gsm_cell_from_set(uint16 idx, srch_gsm_set_enum_type  set);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_GSM_BASE_PTR
  
  DESCRIPTION
    This function returns a pointer to the underlying GSM set array.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    Pointer to the specified set of cells if the given set exists on the
    specified carrier, NULL otherwise.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_gsm_cell_struct_type** srchsetutil_get_gsm_base_ptr(srch_gsm_set_enum_type set);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_SET_MAX
  
  DESCRIPTION
    This function returns the maximum number of cells supported on the given
    set.
 
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Maximum number of cells supported on the specified set
  
  SIDE EFFECTS
    None
=============================================================================*/
uint16 srchsetutil_get_set_max(uint8 carr_idx, srch_set_enum_type set);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_COMP_SET_MAX
  
  DESCRIPTION
    This function returns the maximum number of cells supported on the given
    composite set.
 
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Maximum number of cells supported on the specified composite set
  
  SIDE EFFECTS
    None
=============================================================================*/
uint16 srchsetutil_get_comp_set_max(uint8 carr_idx,srch_comp_set_enum_type set);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_ASET_COUNT_C0
  
  DESCRIPTION
    This function returns the number of cells currently in the ASET on the 
    primary carrier.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Number of cells in the specified set
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_aset_count_c0(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_SYNC_NSET_COUNT_C0
  
  DESCRIPTION
    This function returns the number of cells currently in the SYNC_NSET on 
    the primary carrier.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Number of cells in the specified set
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_sync_nset_count_c0(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_USET_COUNT_C0
  
  DESCRIPTION
    This function returns the number of cells currently in the USET on the
    primary carrier.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Number of cells in the specified set
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_uset_count_c0(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_BASE_ASET_PTR_C0
  
  DESCRIPTION
    This function returns a pointer to the underlying ASET array on the 
    primary carrier.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Pointer to the specified set of cells
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_cell_struct_type** srchsetutil_get_base_aset_ptr_c0(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_BASE_SYNC_NSET_PTR_C0
  
  DESCRIPTION
    This function returns a pointer to the underlying SYNC_NSET array on the 
    primary carrier.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Pointer to the specified set of cells
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_cell_struct_type** srchsetutil_get_base_sync_nset_ptr_c0(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_BASE_USET_PTR_C0
  
  DESCRIPTION
    This function returns a pointer to the underlying USET array on the 
    primary carrier.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Pointer to the specified set of cells
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_cell_struct_type** srchsetutil_get_base_uset_ptr_c0(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_GSM_NSET_COUNT
  
  DESCRIPTION
    This function returns the number of cells currently in the GSN NSET.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Number of cells in the specified set
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_gsm_nset_count(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_INTERF_SYNC_NSET_COUNT
  
  DESCRIPTION
    This function returns the number of cells currently in the inter-f
    SYNC_NSET.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Number of cells in the specified set
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_interf_sync_nset_count(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_INTERF_ASYNC_NSET_COUNT
  
  DESCRIPTION
    This function returns the number of cells currently in the inter-f
    ASYNC_NSET.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Number of cells in the specified set
  
  SIDE EFFECTS
    None
=============================================================================*/
uint8 srchsetutil_get_interf_async_nset_count(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_BASE_INTERF_SYNC_NSET_PTR
  
  DESCRIPTION
    This function returns a pointer to the underlying inter-f SYNC_NSET array
    on the primary carrier.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Pointer to the specified set of cells
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_interf_cell_struct_type** srchsetutil_get_base_interf_sync_nset_ptr(void);

/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_BASE_INTERF_ASYNC_NSET_PTR
  
  DESCRIPTION
    This function returns a pointer to the underlying inter-f ASYNC_NSET array
    on the primary carrier.
  
  DEPENDENCIES
    Assumes set database has already been initialized via srchsetutil_init
  
  RETURN VALUE
    Pointer to the specified set of cells
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_interf_cell_struct_type** srchsetutil_get_base_interf_async_nset_ptr(void);

void srchsetutil_reset_interf_db(void);


/*============================================================================= 
  FUNCTION
    SRCHSETUTIL_GET_GSM_POOL_PTR
  
  DESCRIPTION
    This function returns a pointer to the underlying GSM pool array.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    This function returns the starting address of the gsm pool.
  
  SIDE EFFECTS
    None
=============================================================================*/
srch_gsm_cell_struct_type** srchsetutil_get_gsm_pool_ptr(void);


/*=========================================================================
FUNCTION     : SRCHSETUTIL_FIND_GSM_CELL_BY_CELL_ID
               
DESCRIPTION  : This function searches the cell id from GSM_SET_N
  
DEPENDENCIES : None

RETURN VALUE : Pointer to a GSM Cell
      
SIDE EFFECTS : None

=========================================================================*/
srch_gsm_cell_struct_type*  srchsetutil_find_gsm_cell_by_cell_id(uint16 cell_id );

#ifdef FEATURE_WCDMA_REACQLESS_PICH   
/*=============================================================================
  FUNCTION
    SRCHSETUTIL_GET_SERVING_CELL_STTD
  
  DESCRIPTION
    This function returns the sttd of serving cell based on the carrier specified.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    STTD of the serving cell i.e TRUE/FALSE
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_get_serving_cell_sttd(uint8 carr_idx);
#endif

#ifdef FEATURE_QSH_DUMP
/*=============================================================================
  FUNCTION
    SRCHSETUTIL_GET_TOTAL_CELLS_IN_CARR
  
  DESCRIPTION
    This function returns the total number of cells in a carrier.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/

uint8 srchsetutil_get_total_cells_in_carr(uint8 carr_idx);

/*=============================================================================
  FUNCTION
	SRCHSETUTIL_FIND_NEXT_BEST_SET_IDX
  
  DESCRIPTION
    This function returns find next best set index.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/

uint8 srchsetutil_find_next_best_set_idx(uint8 set_idx);

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_QSH_DUMP_CB
  
  DESCRIPTION
    This function returns the sttd of serving cell based on the carrier specified.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
void srchsetutil_qsh_dump_cb(wl1_srcher_mini_debug_info_struct_type *addr_ptr_t);
#endif /* FEATURE_QSH_DUMP */

/*=============================================================================
  FUNCTION
    SRCHSETUTIL_BASIC_SANITY_CHECK
  
  DESCRIPTION
    This function checks the sanity of carr_idx and set passed to it
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
=============================================================================*/
boolean srchsetutil_basic_sanity_check(uint8 carr_idx, srch_set_enum_type basic_set,
                                      srch_comp_set_enum_type comp_set, 
                                      srch_interf_set_enum_type interf_set,                                       
                                      uint8 bmsk);
/*=========================================================================
FUNCTION     : SRCHSETUTIL_MANAGE_CELL_ALLOCATION
               
DESCRIPTION  : This function allocates the cell for multi carrier if not already allocated
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchsetutil_manage_cell_allocation( void );

/*=========================================================================
FUNCTION     : SRCHSETUTIL_MANAGE_CELL_DE-ALLOCATION
               
DESCRIPTION  : This function allocates the cell for multi carrier if not already allocated
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchsetutil_manage_cell_deallocation( void );
/*=========================================================================
FUNCTION     : SRCHSETUTIL_CRASH_DEBUG

DESCRIPTION  : This function crashes when more carrier cell structures are allocated 
               than the carriers configured at the time of W_DE_INIT.
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchsetutil_crash_debug( void );

#endif /* !SRCHSETUTIL_H */
