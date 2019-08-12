#ifndef TDSSRCH_SET_H
#define TDSSRCH_SET_H
/*===========================================================================
FILE: tdssrchset.h

GENERAL DESCRIPTION:
      This file contains the typedefs, enums and defines used for the Search
      set operations. 
      
      
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2000-2011 by Qualcomm Technologies Incorporated. All Rights Reserved.
      
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdssrchset.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------   
05/21/12   kguo    Removed feature FEATURE_TDSCDMA_RX_DIVERSITY


===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "tdssrchparm.h"
#include "tdsl1rrcif.h"
#include "tdsl1const.h"
#include "tdsl1def.h"
#include "tfw_sw_intf_measurement.h"
#include "tfw_sw_intf_srch.h"


#ifdef FEATURE_TDSCDMA_CELL_RESELECT_INTER_S
#include "tdssrchgsmdrv.h"
#endif

#include "custtdscdma.h"//for fast ho
#ifdef FEATURE_TDSCDMA_ANTENNA_SWITCH_DIVERSITY
#include "tdsl1_nv.h"
#include "trm.h"
#endif
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                     Intra-Frequency MACROS 
--------------------------------------------------------------------------*/
#undef SRCH_ASET_MAX
#define SRCH_ASET_MAX  TDSL1_ASET_MAX
  /* The max number of cells in Active SET */
  
  /* The max number of intra-frequency cells */
#define TDSSRCHSET_INTRA_NSET_MAX  32

#define TDSSRCHSET_NBCH_SET_MAX     2

#define TDSSRCHSET_INVALID_INDEX (0xFFFF)

#define TDSSRCHSET_MAX_CPI 128

/*--------------------------------------------------------------------------
                       Inter-RAT MACROS 
--------------------------------------------------------------------------*/

/* DJM - why not just use TDSL1_MAX_CELL_MEAS ? */
#define  TDSSRCHSET_GSM_CELL_MAX  TDSL1_MAX_CELL_MEAS
  /* The max number of cells specified for an inter-RAT GSM channel */
#define TDSSRCHSET_GSM_NSET_MAX   TDSSRCHSET_GSM_CELL_MAX
  /* The max number of GSM cells in Neighbor Set */
#define TDSSRCHSET_GSM_USET_MAX   TDSSRCHSET_GSM_CELL_MAX
  /* The max number of GSM cells in Unmonitored Set */
#define TDSSRCHSET_GSM_NUM_MAX_FORBIDDEN_OFFSET 3
  /* Max number of forbidden tone positions*/

/*--------------------------------------------------------------------------
          Cell Table Size MACROS for intra-frequency and inter-frequency
--------------------------------------------------------------------------*/
#ifdef FEATURE_JOLOKIA_MODEM
#define TDSSRCHSET_CELL_TABLE_MAX  128
#else
#define TDSSRCHSET_CELL_TABLE_MAX  96
#endif
/*--------------------------------------------------------------------------
          Joint Detection Set  MACROS
--------------------------------------------------------------------------*/
#define  TDSSRCHSET_JDCS_CELL_MAX            8
/*--------------------------------------------------------------------------
          Remaining Set Maintenance MACROS
--------------------------------------------------------------------------*/

#ifdef FEATURE_JDS_OPT 
#define TDSSRCHSET_ADD_RSET_CELL(freq_index, cpi) \
   tdssrchset_add_rset_cell(freq_index, cpi) 

#define TDSSRCHSET_REMOVE_RSET_CELL(freq_index, cpi) \
  tdssrchset_remove_rset_cell(freq_index, cpi)
#else 
#define TDSSRCHSET_ADD_RSET_CELL(freq_index, cpi) \
{ tdssrchset_freq_set[freq_index].rset_cell_lookup_bitmap_table[cpi >> 5] |= (1 << (cpi % 32)); }

#define TDSSRCHSET_REMOVE_RSET_CELL(freq_index, cpi) \
{ tdssrchset_freq_set[freq_index].rset_cell_lookup_bitmap_table[cpi >> 5] &= ~(1 << (cpi % 32)); }
#endif

/*--------------------------------------------------------------------------
          Cell Structure Pointer Check MACROS
--------------------------------------------------------------------------*/

#define TDSSRCHSET_CHECK_INVALID_CELL_STRUCT_PTR(cell_ptr) \
  ((cell_ptr < &tdssrchset_search_cells[0]) \
    ||(cell_ptr > &tdssrchset_search_cells[TDSSRCHSET_CELL_TABLE_MAX-1]))

/* To tell Driver to remove a cell pointer */
#define TDSSRCHSET_REMOVE_GSM_DRV_CELL(cell_ptr) \
           tdssrchgsm_delete_inter_rat_cell_to_set( (cell_ptr), TDSSRCHGSM_SRCH_INTER_RAT_CELL_GSM_TYPE )

/* To tell Driver to add a cell pointer */
#define TDSSRCHSET_ADD_GSM_DRV_CELL(arfcn, bsic_verify_reqd) \
              (void *) tdssrchgsm_add_gsm_cell_for_meas( \
                         arfcn, \
                         bsic_verify_reqd \
                       );


#define TDSSRCHSET_GET_SERVING_CELL_PTR()  (tdssrchset_active_cell.active_cell_ptr)
#define TDSSRCHSET_GET_NBCH_CELL_PTR()       (tdssrchset_cr_active_cell.active_cell_ptr)
#define TDSSRCHSET_GET_NBCH_FREQ()             (tdssrchset_cr_active_cell.active_cell_ptr->freq)

           
/*--------------------------------------------------------------------------
                            SEARCH SET DECLARATION
--------------------------------------------------------------------------*/
typedef enum
{
  /* Cell belongs to Active Set */
 TDSSRCHSET_A, 
  /* Cell belongs to Sync Neighbor Set */
  TDSSRCHSET_SYNC_N,
  /* Cell belongs to Async Neighbor Set */
 TDSSRCHSET_ASYNC_N,
  /* Cell belongs to Unlisted Set. Note: during initial acquisition, 
     all cells idetified by Step_3 Search belong to Unlisted Set.
   */
  TDSSRCHSET_U,
  /* Cell belongs to Add-Candidate Set -- a SET containing cells that are 
     originally from Sync_NSET or USET and whose energy is strong enough,
     based on network-specified event criteria (1A, 1C, 1D & 1E), 
     to be handoff candidates 
   */
 TDSSRCHSET_ADD_C,
  /* Cell belongs to Drop-Candidate Set -- a SET containing Cells that 
     are originally from ASET or Add_CSET and whose energy is NOT strong 
     enough, based on network-specified event criteria (1B & 1F), 
     to stay as handoff candidates.
   */
  TDSSRCHSET_DROP_C,
  /* After failed W2G, we attempt to RESUME on this set. */
  TDSSRCHSET_RESUME,
  /* These cells are measured only in DCH state */
  TDSSRCHSET_DCH_ONLY,
  TDSSRCHSET_H,
   TDSSRCHSET_A_NO_PN,
  /* cells moved to candidate set from the uset. This is also a part of the candidate
     set. However it holds only those cells from TDSSRCHSET_U whose energy is strong enough 
     to consider as handoff candidate 
  */
   TDSSRCHSET_CAND_U,
  /* Saved set Info is used during HHO before any dch to dch hho
     We need to put all necessary set to saved set in order to retrive
     those in case HHO failure 
   */
  TDSSRCHSET_SAVED_INFO,
  /* The second serving cell in DC operation */
  TDSSRCHSET_DC_A,
  /* Synchronous neighbors on second carrier in DC operation */
  TDSSRCHSET_DC_SYNC_ALL,
  /* Saved cell set on second carrier in DC operation during HHO */
  TDSSRCHSET_DC_SAVED,
  /* Cell belongs to Empty Set */
 TDSSRCHSET_E
}
tdssrchset_set_enum_type;



/*
 * GSM Set Management Strategy Explained...
 *
 * -E- All cells begin in the GSM_SET_E (Empty) set.
 *     After Allocation they may be removed, at which time they
 *     will return to the empty set.
 * -U- Cells are then Added to the GSM_SET_U (Unmonitored)
 *     through processing of the measurement control
 *     New Cell/add_cell[] parameter in the Inter-RAT Cell
 *     structure.
 * -N- Cells are then Moved to the GSM_SET_N (Neighbor)
 *     When monitoring should be performed this is directed
 *     in the Cells for Measurement/meas_cell_list[]
 *     parameter of the measurement control message.
 *
 * note: in future we may add a Removed set which can hold
 *       cells that have been allocated and then removed.
 *       we may also add hot, medium and cold priority
 *       to the neighbor set. (djm 7/17/02)
 */
typedef enum
{
  TDSSRCHSET_GSM_SET_N, 
    /* Cell belongs to GSM Neighbor Set */
  TDSSRCHSET_GSM_SET_U, 
    /* Cell belongs to GSM Unmonitored Set */
  TDSSRCHSET_GSM_SET_E
    /* Cell belongs to Empty Set */
}
tdssrchset_gsm_set_enum_type;

/*--------------------------------------------------------------------------
                      SEARCH EVENT TIME STAMP STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  boolean  enabled;
    /* Flag indicating is the event time stamp is enabled  */
  uint16 time;
    /* Time when the event first becomes TRUE */

  boolean periodic_timer_activated;
     /* variable which indicates whether the periodic event reporting
      for 1A and 1C is activated  Refer to 14.1.4 of 3gpp 25.331 */

  uint16 num_reports;
  /* Number of times the event has been reported for this cell */

#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
  /* The sfn at which the tresel timer was started, this actually is sub sfn. 5ms */
  uint16 ts_update_sfn;
#endif

 boolean fast_ho_enabled;
 uint16 reach_max_UL_power_cnt;

}
tdssrchset_evt_ts_struct_type;

typedef struct
{
    uint16 num_reports;
}
tdssrchset_periodic_ts_struct_type;

/* Maximum number of event on any given WCDMA frequency */
#define SRCHDCH_MAX_EVT  \
                MAX (TDSSRCHDCH_MAX_INTRAF_EVT, TDSSRCHDCH_MAX_INTERF_EVT)
                  
/* Union of ALL Intra-frequency and inter-frequency event Time stamps */
typedef union
{
  tdssrchset_evt_ts_struct_type  intraf_evt_ts[TDSSRCHDCH_MAX_INTRAF_EVT];
    /* CELL_DCH Intra-frequency Event time stamps */
  tdssrchset_evt_ts_struct_type  interf_evt_ts[TDSSRCHDCH_MAX_INTERF_EVT];
    /* CELL_DCH Intra-frequency Event time stamps */
  tdssrchset_periodic_ts_struct_type periodic_ts;
}
tdssrchset_evt_ts_union_type;



/* Unioin of ALL Intra- and inter-frequency event-barred status */
typedef union
{
  boolean  intraf_evt_barred[TDSSRCHDCH_MAX_INTRAF_EVT];
}
tdssrchset_evt_barred_union_type;


/* Union of ALL Intra- and inter-frequency event report status, Those flags
   are used by mobile to remember is a particular cell has been reported for 
   a particular event and therefore, to prevent network from being overwhelmed
   by event reports. The use or the necessity of those fields need to be 
   re-evaluated when multiple report and periodic report are implemented in
   the future.
 */
typedef union
{
  boolean  intraf_evt_rpt_status[TDSSRCHDCH_MAX_INTRAF_EVT];
    /* Intra-frequency event report status */
}
tdssrchset_evt_rpt_union_type;



/*--------------------------------------------------------------------------
        SFN-CFN OBSERVED TIME DIFFERENCE BOOK KEEPING DATA STRUCTURE
--------------------------------------------------------------------------*/
typedef struct {
//tdstlm_tfn_struct_type sfn_wrc_diff;

boolean sfn_wrc_valid;
} tdssrchset_sfn_wrc_diff_type;

typedef struct
{
  boolean tm_off_status;
    /* Flag indicating time difference information below is current */ 
  uint32  tm;
    /* Defined in 5.1.9 of TS25.215 */
  uint16  off;
    /* Defined in 5.1.9 of TS25.215 */
  uint32  ref_pos;
    /* reference cell physical channel combiner position when the above 
       time difference was last observed or adjusted
     */

  tdssrchset_sfn_wrc_diff_type sfn_wrc_diff;

  boolean sfn_sfn_status;
  uint32 sfn_sfn_val;


  
  /* Rescheduling of SFN decode needed ? */
  boolean reschedule_needed;

  uint32  ts_1;
    /* Time stamp when the time differnce was initially measured */
  uint16  ts_2;
    /* Time stamp when the time difference was last adjusted */
 
}
tdssrchset_sfn_cfn_tm_diff_struct_type;


typedef struct {
  boolean enabled;
  uint16 time;
  boolean report_sent;
}  tdssrchset_evt_6g_6f_struct_type;

#ifdef FEATURE_TDSCDMA_HCS
typedef struct {
  /* Hierachical Cell Structrue Priority */
  uint16        hcs_prio;
  
  /* Quality Threshold level for HCS cell reselection */  
  uint16        q_hcs;
    
 /* Penalty_time. 0 means not used. next two not allowed for 0 */
  uint16        penalty_time_frames;
   
  /* Temporary_offset used for CPICH RSCP*/
  uint16        tmp_offset1;

  /* W(x) factor for R ranking */
  uint16         Wx_R;

  /* H ranking for the cell */
  int16          H;

  /* Penalty timer for this cell (neighbor) */
  tdssrchset_evt_ts_struct_type  evt_cr_penalty_timer;

  /* boolean varibale to indicate that the cell qualifies for 
   * measurement in HCS mode
   */
  boolean cell_qualify;

  /* Counter to check for choosing all remaining cells in equal probability*/
  uint8 elapse_cntr;
} 
tdssrchset_cell_hcs_info_struct_type; 
#endif 

typedef enum
{
  TDSSRCHSET_TXD_STATUS_DETECTION_NEEDED,
  TDSSRCHSET_TXD_STATUS_DETECTION_IN_PROG,
  TDSSRCHSET_TXD_STATUS_NW_INFORMED,
  TDSSRCHSET_TXD_STATUS_DETECTED,
}  tdssrchset_cell_txd_det_status;

/*--------------------------------------------------------------------------
                            SEARCH CELL STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  /* WCDMA Cell Scrambling Code */
  uint16  scr_code;
    
  /* WCDMA frequency assignment for the cell */
  uint16  freq;
  
  /* Forward power control combination index of the cell */
  uint8   tpc;

  //#ifdef FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
  uint8 sec_scr_code_index;
  //#endif

  /* TRUE if the CPICH of this cell uses STTD */
  boolean sttd;

  /* give the status of the sttd flag above whether it has been detected
     or provided from nw or it needs detection */
  tdssrchset_cell_txd_det_status txd_status;

  /* cell database index */
  uint8 cell_db_idx;

  /*index of the nbr rl db for SFN decode treating this cell as neighbor */
  uint8 nbr_rl_idx;

  /* TRUE if cell moved from saved set. This falg is used to identify if this 
  cell need to put back in saved set upon failure of HHO */
  boolean cell_from_saved_set;
#ifdef FEATURE_TDSCDMA_HSUPA
  /* TRUE will indicate E-DCH is active on this cell. THis info will help in
  distinguishing non E-DCH aset cells which can trigger evt 1J */
  boolean edch_active;
#endif
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
  tdssrchset_set_enum_type  set;
    

  /* The total energy and Ec/Io in the best paths found in a sweep */
  uint32  tot_eng;
  uint16 tot_ecio;
  uint16  ecio;
  #ifdef FEATURE_TDSCDMA_L1_RXD_FILTERING_SUPPORTED
  /* This will be updated post PN search and will be used in RSCP filt*/
  uint16 tot_rscp;
  #endif
  
  uint16  filt_mult_factor;
  /* Filtered energy and Ec/Io and Filter length used currently */
  uint16  path_thresh;
  /* path threshold for this cell */

#ifdef FEATURE_TDSCDMA_MULTIPLE_L3_FILTERS_SUPPORTED
  
  struct {
   uint32 filt_eng;
   uint16 filt_ecio;
   uint16 filt_rscp;
   uint16 pathloss;
   uint16 filt_len;
   #ifdef FEATURE_TDSCDMA_L1_RXD_FILTERING_SUPPORTED
   uint16 filt_len_rscp;
   #endif

  } dch_filtered_parms[TDSSRCH_TDSCDMA_MAX_FILT_SIZE];

#endif /*FEATURE_TDSCDMA_MULTIPLE_L3_FILTERS_SUPPORTED*/

  uint32  filt_eng;
  uint16  filt_ecio;
  uint16  filt_len;

  /* CPICH RSCP and filtered RSCP */
  uint16  rscp;
    /* It is RxAGC + Filtered Ec/Io. The actual CPICH RSCP is "-rscp" */
  uint16  filt_rscp;


  /* Flag indicating if the cell has been evaluated */
  boolean evaled;
    
  /* positions of the maximum energy paths in 1/8 chip units. */
  uint32  max_pos[ TDSSRCH_MAX_PILOT_PEAKS ];
    
  /* Energies of the maximum energy paths */
  uint32  max_eng[ TDSSRCH_MAX_PILOT_PEAKS ];

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
 
  /* Reference Time difference to Serving cell, in chips. 
     0xFFFF indicates the field is not included. This field is 
     either obtained from a SIB message or via mobile measurement.
   */
  uint16  time_diff;
    
  /* Neighbor Cell timing accuracy relative to the serving cell.
     This will be translated to "search window size" for the neighbor.
   */
  tdsl1_cell_time_acc_enum_type time_acc;

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
  tdssrchset_evt_ts_struct_type  evt_cr_ts;

  /* Cell Reselection sorted index */
  uint16   cr_sort_inx;

  /* Cell Reselection search required flag, for weighting searches */
  boolean   cr_srch_reqd;

  /* Rank of this cell for cell reselection */
  int16 rank;

  /* for a active set cell this flag == TRUE will tell this aset cell
     is not part of the CELL_INFO_LIST */
  boolean cell_from_uset;

  /*this will be used to move cell out of the candidate set when it no longer */
  uint16 cand_age;

#ifdef FEATURE_TDSCDMA_HCS
  /* HCS related info for the cell */
  tdssrchset_cell_hcs_info_struct_type cell_hcs_info;
#endif

  /* CELL_DCH Intra-frequency Event time stamps */
  tdssrchset_evt_ts_union_type   u_ts;
 
  /* CELL_DCH event rpt forbidden for the cell */
  tdssrchset_evt_barred_union_type  u_bar;

  /* CELL_DCH Intra-freqency event reporting status flag */
  tdssrchset_evt_rpt_union_type  u_rpt;

 /*  In the event of 1C, contains the cell on which 1C got triggered  */
 uint16 srch_evt_1c_src_psc;

  /* SFN-CFN time difference structure type. */ 
  tdssrchset_sfn_cfn_tm_diff_struct_type  sfn_cfn_tm_diff;

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
  tdssrchset_evt_6g_6f_struct_type cTRIGGERED_6F_EVENT;
  tdssrchset_evt_6g_6f_struct_type cTRIGGERED_6G_EVENT;

  boolean cell_for_measurement[TDSSRCHDCH_MAX_INTRAF_EVT];

  /* Flag indicates whether a cell is barred or not*/
  boolean cell_barred;
  
}
tdssrchset_cell_struct_type;


#define  TDSSRCHSET_MAX_TIMESLOT     6
#define  TDSSRCHSET_FREQ_MAX             12
#define  TDSSRCHSET_FREQ_MAX_ADD         3

/*--------------------------------------------------------------------------
                            SEARCH SET DECLARATION
--------------------------------------------------------------------------*/
typedef enum
{
  TDS_SET_A,
  TDS_SET_M,
  TDS_SET_D,
  TDS_SET_E
}
tdssrchset_set_enum_type_tmp;


typedef struct
{

  /* timeslot list for ISCP measurement.*/
  uint8 iscp_timeslot;

  /* iscp measurement results. It is one to one mapping 
    from iscp_timeslot_list [] to iscp_results[].
   */
  /*liner value */
  /* ISCP,range[-115,-25],corresponding to [0,91]dbm */
  int16 iscp;
  /*the actual iscp value from fw*/
  int16 iscp_x256;
  /* L3 filters, are used in FACH or PCH state */
  /* iscp filtered value.*/
  /* ISCP,range[-115,-25],corresponding to [0,91]dbm */
  int16 filt_iscp;
  int16 filt_iscp_x256;

  /* TDSCDMA multiple L3 filters, only used in DCH state */
  struct {
    /* ISCP,range[-115,-25],corresponding to [0,91]dbm */
    int16 filt_iscp;
	int16 filt_iscp_x256;
    uint16 filt_len;
  } filtered_parms[TDSSRCH_TDSCDMA_MAX_FILT_SIZE];
  /*the sfn when this timeslot's iscp is done*/
  uint16 iscp_done_stamp;

}tdssrchset_iscp_struct_type;

/*--------------------------------------------------------------------------
                            SEARCH CELL STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  /* TD-SCDMA frequency assignment for the cell */
  uint16  freq;

  /* TRUE means the inter frequency neighbour cell has a secondary frequency which is the 
      same as the current working frequency of UE. 10.3.7.13-Inter-frequency cell info list, 
      3GPP25.331(RELEASE 7) */
  boolean  intra_secondary_frequency_indicator;

  /* TD-SCDMA Cell Parameter id */
  uint16  cpi;
  
  /* which set the cell currently belongs to */
  /* {monitored set, active set and detected set, ...} */
  tdssrchset_set_enum_type_tmp set;

  /* For TD-SCDMA,if this flag == TRUE will tell this intra-cell
     is not part of the CELL_INFO_LIST */
  boolean cell_from_uset;

  /* TRUE if cell moved from saved set. This falg is used to identify if this 
  cell need to put back in saved set upon failure of HHO */
  boolean cell_from_saved_set;
 
  /* The position of the path used for centering searching window. Normally,
     it is the earliest usable (or above threshold SRCH_PATH_THRESHOLD) path.
     
       For other cells, this position is updated after each search of the 
       cells.
       
     Valid value ranges (0..51199) in CHIPx8. 0xFFFFFFFF (or SRCH_NO_REF_POS)
     implies that ref_pos is not being set to a valid value.
   */
  uint16  ref_pos;

  /* The time stamp of the above reference position, in units of sub-frame. 
     At time, ref_pos is updated with a new valid measurement result, 
	 ref_time_stamp is set to tdssrch_ts_cnt.
   */
  int16   ref_time_stamp;

  /* Search window size for this cell */
  uint32  win_size;

  /* PCCPCH RSCP,range[-5,91],corresponding to [-120,-25]dbm */
  int16  rscp;

  /*represent RSCP using 16Q8.8 */
  /*for example,if real rscp is -20.5db (-120,-25)
     this field is -20.5*2^8*/
  int16 rscp_x256;
  /* Frequency offset (unit TBD: Hz or ppm), for non-JDCS cell, 
      it's value always is 0 */
  int32 freq_offset;

  /* OTD type2*/
  int32  otd;

  /* signal-to-noise ratio */   
  uint16 snr;
  /* signal-to-interference ratio */
  uint16 sir;  
  
  /* the number of timeslots for ISCP measurement */
  uint8 iscp_timeslot_num;

  tdssrchset_iscp_struct_type iscp_info[L1_MAX_TS];

  /* Primary P-CCPCH Tx power from system information or MCM. 
     It is used to calculate pathloss.
   */
  int16 pccpch_tx_pwr;

  /* TDSCDMA multiple L3 filters, only used in DCH state */
  struct {
    /*this field is calculated by rscp_x256
     according to formula (val*(n-1)+newval)/n  
     used in DCH*/
    int16 filt_rscp_x256;
	int64 filt_rscp_q16;
    /*this field is calculated by filt_rscp_x256
     according to formula - (Filt_val_x256+2^7)>>8
     [-5,91],corresponding to [-120,-25]dbm*/
    int16 filt_rscp;
	/*calculated by filt_rscp_x256,i.e,txpower*256-filt_rscp_x256*/
	uint16 pathloss_x256;
    uint16 pathloss;
    uint16 filt_len;
  } dch_filtered_parms[TDSSRCH_TDSCDMA_MAX_FILT_SIZE];

  /*this field is calculated by rscp_x256
     according to formula (val*(n-1)+newval)/n  
     used in FACH*/
  int16 filt_rscp_x256;
  int64 filt_rscp_q16;
  /* rscp filtered value.used in FACH*/
  /*this field is calculated by filt_rscp_x256
     according to formula [- (Filt_val_x256+2^7)>>8]
     [-5,91],corresponding to [-120,-25]dbm*/
  int16 filt_rscp;
  /*calculated by filt_rscp_x256,i.e,txpower*256-filt_rscp_x256*/
  uint16 pathloss_x256;
  /* For TDD, Pathloss in dB = Primary CCPCH TX power - Primary CCPCH RSCP.*/
  uint16 pathloss;
  /* the filter length based on the filter_coefficient */
  uint16 filt_len;

  /* A separate RSCP filter for JDCS formation */
  /*range [-120,-25]*256*/
  int16 filt_rscp_for_jdcs;
  
  /*  cell individual offset Refer to 10.3.7.2 and 14.1.5.3 
      for an explanation of this parameter. 
      Used to set invidual threshold for triggering reports 
      Range -10 to +10 in steps of 0.5 */
  int16 cell_offset;

  /* subtract cell individual offset from rscp after L3 filter, get its value.*/
  int16 rscp_with_offset;

  /* subtract cell individual offset from the best iscp after L3 filter, get its value.*/
  int16 iscp_best_with_offset;

  /* subtract cell individual offset from pathloss after L3 filter, get its value.*/
  uint16 pathloss_with_offset;
 
  /* Flag indicating if the cell has been evaluated */
  boolean evaled;
    
  /* This specifies the offset between the two cells. It is used for TDD and GSM cells 
     and for FDD cells in case the quality measure for cell selection and re-selection 
	 is set to PCCPCH RSCP. ( read in system information 11/12) */
  int16  q_offset1;

  /* Minimum required RX level in the cell(dBm, read in system information 3/4)*/
  int16  rxlev_min;

  /* Maximum TX power level an UE may use when accessing the cell on RACH 
    (read in system information 3/4) (dBm).*/
  int16  max_tx_pwr;

  /* Cell selection values evaluated by SRCH and to be reported to RRC */
  int16    s_rxlev;

  /* Rank of this cell for cell reselection */
  int16 rank;

  /* Cell Reselection Timestamp for this cell (neighbor) */
  tdssrchset_evt_ts_struct_type  evt_cr_ts;

  /* Cell Reselection sorted index */
  uint16   cr_sort_inx;

  /* Cell Reselection search required flag, for weighting searches */
  boolean   cr_srch_reqd;

#ifdef FEATURE_TDSCDMA_HCS
  /* HCS related info for the cell */
  tdssrchset_cell_hcs_info_struct_type cell_hcs_info;
#endif

  /* CELL_DCH Intra-frequency Event time stamps */
  tdssrchset_evt_ts_union_type   u_ts;
 
  /* CELL_DCH event rpt forbidden for the cell */
  tdssrchset_evt_barred_union_type  u_bar;

  /* CELL_DCH Intra-freqency event reporting status flag */
  tdssrchset_evt_rpt_union_type  u_rpt;

  /* Indicates whether the cell is in corresponding MCM.*/
  boolean cell_for_measurement[TDSSRCHDCH_MAX_INTRAF_EVT];

  /* Flag indicates whether a cell is barred or not*/
  boolean cell_barred;

boolean high_cost_cell;
#ifdef FEATURE_TDSCDMA_REPEAT_TO_SECOND_BEST
	boolean disable_evaluation;
	uint8 rpt_times;
#endif
  
}
tdssrchset_cell_struct_type_tmp;

typedef struct
{
  uint16 freq;
  uint8 cpi;
  /*[-5,91]*/
  int16 rscp;
  /*[-120,-25]*256*/
  int16 rscp_x256;
}
tdssrchset_rset_cell_struct_type;

typedef struct
{
  uint16 freq;
  uint8 cpi;
}tdssrchset_cell_keyword_struct_type;

extern tdssrchset_cell_keyword_struct_type  tdssrchset_old_serving_cell_before_ho;

typedef struct
{
  uint8 cpid;
  int16 ts_rsp;
}tdssrchset_cell_ts_rsp_type;


#ifdef FEATURE_JDS_OPT
#define TDSSRCHSET_FREQ_CELL_MAX  40
#define TDSSRCHSET_TS0_DEMOD_CELL_SIZE  8
#define TDSSRCHSET_NON_TS0_DEMOD_CELL_SIZE  8
#define TDSSRCHSET_RSCP_ITERATIVE_CELL_MAX   8
#define TDSSRCHSET_TS0_DEMOD_ITERATIVE_CELL_MAX   3
#define TDSSRCHSET_NON_TS0_ITERATIVE_CELL_MAX   3

#ifdef FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING
#define TDSSRCHSET_DL_TS_MAX   5
#endif /* FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING */

#else
#define TDSSRCHSET_FREQ_CELL_MAX  38
#endif

#define TDSSRCHSET_ISCP_PAIR_MAX  6

#ifdef TDSCDMA_ISCP_OPTIMIZATION_8CELLS_1SLOT
/*CR 383425 ISCP Optimixation*/
#define TDSSRCHSET_ISCP_MAX_CELL_1SLOT 4 //5  
#define TDSSRCHSET_ISCP_REMAINED_CELL_1SLOT 28//12 //15
#define TDSSRCHSET_ISCP_MAX_CELL_NONTS0  40
/*CR 383425 ISCP Optimixation*/
#endif

#define TDSSRCHSET_DSET_PER_FREQ_MAX   6
#define TDSSRCHSET_MSET_PER_FREQ_MAX  32


#define TDSSRCHSET_MEAS_REM_CELL_MAX  2
/**/
#define TDSSRCHSET_MAX_ROW_NUM 4
#define TDSSRCHSET_ROW_MAX_REM_CELLS  32

//#define FEATURE_MEASURE_SCHEDULE_TEST

#ifdef FEATURE_JDS_OPT
typedef struct
{
  boolean iterative_cell_flag;
  boolean rset_flag;
  uint16 ts_rsp_time_stamp;
  /*filtered ts-rsp,16Q8*/
  int16 ts_rsp;
  uint16 wall_time_offset_stamp;
  boolean recalculate_flag;
  /* wall_time_offset(28:16) = subframe offset 
    * wall_time_offset(15:0) = ts0StartPosition (in units of Cx8)
    */
  uint32 wall_time_offset;  
  uint16 rscp_time_stamp;
  /*L1 filtered  rscp,16Q8*/
  int16 rscp;

 #if defined(FEATURE_TDSCDMA_FILT_M) && defined(FEATURE_JDS_OPT)
  uint16 metric_M;
  
  uint16 filt_metric_M;
 #endif
}
tdssrchset_ts0_cell_info_type;

typedef struct
{
  boolean iterative_cell_flag;
  boolean rset_flag;
  #ifdef FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING
  /* The flag is used to identify whether this cell is going to be removed in
   * non-ts0 table according to non-ts0 pruning algo. 
   * TRUE - to be removed; FALSE, otherwise.
   */
  boolean pruning_flag;  
  uint16 cell_F_metric;
  #endif /* FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING */
  uint16 ts_rsp_time_stamp;
  int16 ts_rsp;
  uint8 same_cpid_nset_cells_num;
  uint16 wall_time_offset_stamp;
  /* wall_time_offset(28:16) = subframe offset 
   * wall_time_offset(15:0) = ts0StartPosition (in units of Cx8)
   */
  uint32 wall_time_offset; 
}
tdssrchset_non_ts0_cell_info_type;
#endif /* FEATURE_JDS_OPT */

#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
/* The following information is the inter frequency priority information
   that was revieved from RRC via the 3rd MEAS_REQ/SIB19 */
typedef struct
{
  int8 priority;
  uint8 threshx_high; 
  uint8 threshx_low; 
  int16 q_rxlevmin;
} tdssrchset_inter_freq_priority_info_type;
#endif
/*--------------------------------------------------------------------------
                            SEARCH FREQUENCY STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 cpi;
  int16 meas_val_x4;
}tdssrchset_cpi_and_rscp_struct_type;

typedef struct
{
  /* TD-SCDMA frequency assignment for the cell */
  uint16  freq;

  /* Absolute measured frequency (Hz), after getting measured results, its value will be updated */
  uint32  freq_HZ;

  /* UTRA carrier RSSI. Valid value ranges (0..76). 
      see subclause 9.1.1.4 of 25.123 for more detail descriptions.*/
  int8  rssi;
  /*midamble RSSI,16Q8*/
  int16  mid_rssi;

  /* RF script buffer index for inter freq tuning, if its value is 0xFF, we will ignore it.*/
  uint8 freq_rf_buf_idx;

  /* the number of all cells on the frequency */
  uint8 cell_total_cnt;

  /* cell pointer array, contains all cells except cells in the remaining set on the frequency */
  tdssrchset_cell_struct_type_tmp *cell_ptr[TDSSRCHSET_FREQ_CELL_MAX];

  /* the number of detected cells on the frequency.*/
  uint8  dset_cell_cnt;

  /* the number of cells which belongs to joint detection candidate set on the frequency.*/
  uint8  jdcs_cell_cnt;

  /* 128 bits are corresponding to 128 midamble code indexs.i.e. 128 cells on one frequency. 
      If some one bit is 1, its corresponding cell belongs to RSET.*/
  uint32 rset_cell_lookup_bitmap_table[4];

#ifdef FEATURE_MEASURE_SCHEDULE_TEST
  uint16 freq_schedule_stamp;
  uint16 total_freq_schedule_no;
  uint16 average_freq_schedule_interval;
  uint16 min_freq_schedule_interval;
  uint16 max_freq_schedule_interval;
  uint16 cell_schedule_stamp[128];
  uint16 total_cell_schedule_no[128];
  uint16 average_cell_schedule_interval[128];
  uint16 min_cell_schedule_interval[128];
  uint16 max_cell_schedule_interval[128];
#endif

#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
	boolean prior_info_present;
	/* Indicates if priority information has been provided for this frequency */

	tdssrchset_inter_freq_priority_info_type prior_info;
	/* Priority information for this frequency */
#endif
  /* if no any cells are measured on the frequency, its value is FALSE. Otherwise, its value 
      is TRUE. The field is used to judge if set management need sort cells on the frequency 
      when to build JDCS.*/
  boolean freq_measured;

  /* record the latest time which receives RSCP measurement results */
  uint16  srch_done_stamp;

  /* SW help FW to store LNA info for inter freq measurement */ 

#ifdef FEATURE_TDSCDMA_FW_NEW_LNA_INTF
  tfw_rx_lna_t lna_info;
#else
  tfw_inter_freq_lna_t inter_lna_info;  
#endif


  /*Fresh measurement results available for periodic reporting*/
  boolean periodic_fresh;

  /*Fresh measurement results available for additional reporting*/
  boolean addl_fresh;
  
#ifdef FEATURE_JDS_OPT
  /* Current TS0 iterative cells information */
  uint8 ts0_iterative_cell_cnt;
  /*used for record the number cells which is put in ts0_iterative_cpid[],
    but not used for iterative work*/
  uint8 not_ts0_iterative_cell_cnt;
  uint8 ts0_iterative_cpid[TDSSRCHSET_RSCP_ITERATIVE_CELL_MAX];

  /* TS0 Table is used to store information for iterative channel estimation set of RSCP and 
    * TS0-demodulation and TS0-ISCP. There are 128 array elements corresponding to 128 
    * cpids, i.e. TS0 Table[0] is corresponding to cpid 0, TS0 Table[1] is corresponding to cpid 1,
    * ¡­, TS0 Table[127]   is corresponding to cpid 127. Each element includes TS-RSP, RSCP, 
    * ts0_start_position and their timestamps. */
  tdssrchset_ts0_cell_info_type ts0_table[TDSSRCHSET_MAX_CPI];
  /* scan index of RSET cells in TS0 table */
  uint8 ts0_table_rset_scan_index;
  /* scan index of MSET&DSET cells in TS0 table */
  uint8 ts0_table_nset_scan_index;
  
  #ifdef FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING
  /* Time stamp to record the sub-frame nunber last received message */
  uint16 gp_n0_time_stamp;
  
  /* Noise power estimated over the gap between TS0 and DwPTS in dBm(format 16Q8) */          
  int16  gp_n0;
  #endif /* FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING */
  
#endif /* FEATURE_JDS_OPT */

  /* When to receive MCM, set management is updating the frequency set, its value is TRUE,
    * here, measurement module cannot schedule all cells' measurement on the frequency.
    * After set management finishs updating the frequency set, its value will be set as FALSE. */
  boolean locked;

}
tdssrchset_freq_list_struct_type;

extern tdssrchset_freq_list_struct_type tdssrchset_freq_set[TDSSRCHSET_FREQ_MAX+TDSSRCHSET_FREQ_MAX_ADD];
extern uint8 tdssrchset_freq_cnt;

/*--------------------------------------------------------------------------
                            SEARCH ACTIVE CELL STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  /* primary frequency */
  uint16  primary_freq;
  
  /* working frequency */
  uint16  working_freq;

  /* index of primary frequency in srch_freq_nset[] array */
  uint8  primary_freq_index;

  /* index of working frequency in srch_freq_nset[] array */
  uint8  working_freq_index;

  /* pointer to serving cell structure.*/
  tdssrchset_cell_struct_type_tmp * active_cell_ptr;
  
#ifdef FEATURE_JDS_OPT
  /* the cell number of neighbor set (nonTS0dem-NS)  */
  uint8 non_ts0_nset_cell_cnt;
  
  /* Current non-TS0 iterative cells information */
  uint8 non_ts0_iterative_cell_cnt;
  uint8 non_ts0_iterative_cpid[TDSSRCHSET_NON_TS0_ITERATIVE_CELL_MAX];

  /* non-TS0 Table is used to store information for iterative channel estimation set of  
    * non-TS0 demodulation and non-TS0 ISCP. There are 128 array elements corresponding 
    * to 128 cpids, i.e. non-TS0 Table[0] is corresponding to cpid 0, non-TS0 Table[1] is 
    * corresponding to cpid 1,¡­, non-TS0 Table[127]   is corresponding to cpid 127. Each 
    * element includes TS-RSP and its timestamps. */
  tdssrchset_non_ts0_cell_info_type non_ts0_table[TDSSRCHSET_MAX_CPI];
  /* scan index of RSET cells in non-TS0 table */
  uint8 non_ts0_table_rset_scan_index;
  /* scan index of MSET&DSET cells in non-TS0 table */
  uint8 non_ts0_table_nset_scan_index;

 #ifdef FEATURE_TDSCDMA_LOWER_T2G_RESEL_CHANCE
  /*rscp for reselection to GSM in DRX(16Q8)*/
  /*comments:_x256*/
  int16 l1_filt_rscp_for_gsm_resel;
  /*new ranking value for reselection to GSM*/
  int16 rank_for_gsm_resel;
 #endif

  #ifdef FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING
  /* Time stamp to record the sub-frame nunber last received message */
  uint16 n0_rssi_time_stamp;

  /* Noise power at the CIR Post Proc out put which combine all the active slot 
   * together in dBm(format 16Q8) 
   */       
  int16 n0;
  
  /* Midamble RSSI which combine all the active slot together in dBm(format 16Q8)
   */             
  int16 rssi;
  #endif /* FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING */
  
  int16 cir_snr;
  
#endif  /* FEATURE_JDS_OPT */
}
tdssrchset_active_cell_info_struct_type;

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
typedef struct
{
#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
  /* The sfn at which the tresel timer wa started */
  uint16 ts_update_sfn;
#endif
  uint16 time;
    /* Time when the event first becomes TRUE */
  uint16 num_reports;
  /* Number of times the event has been reported for this cell */

  boolean  enabled;
    /* Flag indicating is the event time stamp is enabled  */

  boolean periodic_timer_activated;
     /* variable which indicates whether the periodic event reporting
      for 1A and 1C is activated  Refer to 14.1.4 of 3gpp 25.331 */
}
tdssrch_evt_ts_struct_type;
#endif 

#ifdef TDSCDMA_FEATURE_SERVING_CELL_RSCP_ANT1

/*Serving cell RSCP RXD*/
typedef struct
{
  /* PCCPCH RSCP,range[-5,91],corresponding to [-120,-25]dbm */
  int16  rscp_ant1;


  /*represent RSCP using 16Q8.8 */
  /*for example,if real rscp is -20.5db (-120,-25)
     this field is -20.5*2^8*/
  int16 rscp_x256_ant1;

    struct {
    /*this field is calculated by rscp_x256
     according to formula (val*(n-1)+newval)/n  
     used in DCH*/
    int16 l3_filt_rscp_x256_ant1;
	int64 l3_filt_rscp_q16_ant1;

    /*this field is calculated by filt_rscp_x256
     according to formula - (Filt_val_x256+2^7)>>8
     [-5,91],corresponding to [-120,-25]dbm*/
    int16 l3_filt_rscp_ant1;

  } dch_filtered_parms[TDSSRCH_TDSCDMA_MAX_FILT_SIZE];

   /*this field is calculated by rscp_x256
     according to formula (val*(n-1)+newval)/n  
     used in DCH*/
  int16 l1_filt_rscp_x256_ant1;
  int16 l3_filt_rscp_x256_ant1;
  int64 l3_filt_rscp_q16_ant1;

  /*this field is calculated by filt_rscp_x256
     according to formula - (Filt_val_x256+2^7)>>8
     [-5,91],corresponding to [-120,-25]dbm*/
  int16 l1_filt_rscp_ant1;
  int16 l3_filt_rscp_ant1;

  int16 jdcs_rscp_ant1;

  uint16 rscp_time_stamp;

}
tdssrchset_active_cell_rscp_ant1_info_struct_type;
extern tdssrchset_active_cell_rscp_ant1_info_struct_type tdssrchset_active_cell_rscp_ant1_info;
extern boolean bserving_cell_rscp_ant1_flag;
#endif

extern tdssrchset_active_cell_info_struct_type tdssrchset_active_cell;
/* Active cell information during handover */
extern tdssrchset_active_cell_info_struct_type tdssrchset_ho_active_cell;

extern tdssrchset_active_cell_info_struct_type tdssrchset_cr_active_cell;

#ifdef FEATURE_TDSCDMA_ANTENNA_SWITCH_DIVERSITY

#define TDSSRCH_ASDIV_ANTENNA_ARRAY_SIZE 80
#define TDSSRCH_ASDIV_MIN_T1_LENGTH 50
#define TDSSRCH_ASDIV_MAX_PROBE_CYCLE 10
#define TDSSRCH_ASDIV_T2_COOLDOWN 2000
#define TDSSRCH_ASDIV_MIN_TEST_MODE_IDLE_SWITCH_PERIOD 2

typedef enum
{
	TDSSRCHSET_IDLE_PHASE,
	TDSSRCHSET_PROBING_PHASE,
	TDSSRCHSET_WAITING_PHASE
} tdssrchset_ante_switch_phase_enum_type;

typedef enum
{
	STEP0_FIRST_ANT,
	STEP0_SECOND_ANT,
	STEP1_FIRST_ANT,
	STEP1_SECOND_ANT
} tdssrchset_asdiv_acq_state;

typedef struct
{
	// ASDiv algorithm used, either type 1 or type 2
	uint8 algorithm_type;
	// Current state
	tdssrchset_ante_switch_phase_enum_type ante_switch_phase;
	// Current state under ACQ
	tdssrchset_asdiv_acq_state acq_state;
	// Current antenna
	uint8 ant_no;
	// T1 probe time (ms)
	uint16 T1_probe_time;
	// Number of probe cycles
	uint8 num_probe_cycles;
	
	// Average RSCP for ant0, Q8, updated every T1 period
	int32 ant0_rscp;
	// Number of received RSCP values in current T1 period
	int16 ant0_rscp_num_in_T1;
	// Average RSCP history for ant0
	int16 ant0_rscp_history[TDSSRCH_ASDIV_ANTENNA_ARRAY_SIZE];
	// Average Tx power for ant0, updated every T1 period
	int32 ant0_txpwr;
	// Number of received Tx power values in current T1 period
	int16 ant0_txpwr_num_in_T1;
	// Average Tx power history for ant0
	int16 ant0_txpwr_history[TDSSRCH_ASDIV_ANTENNA_ARRAY_SIZE];
	// The index for ant0 Tx power history array
	int8 ant0_history_index;

	// Average RSCP for ant1, Q8, updated every T1 period
	int32 ant1_rscp;
	// Number of received RSCP values in current T1 period
	int16 ant1_rscp_num_in_T1;
	// Average RSCP history for ant1
	int16 ant1_rscp_history[TDSSRCH_ASDIV_ANTENNA_ARRAY_SIZE];
	// Average Tx power for ant1, updated every T1 period
	int32 ant1_txpwr;
	// Number of received Tx power values in current T1 period
	int16 ant1_txpwr_num_in_T1;
	// Average Tx power history for ant1
	int16 ant1_txpwr_history[TDSSRCH_ASDIV_ANTENNA_ARRAY_SIZE];
	// The index for ant1 Tx power history array
	int8 ant1_history_index;

	// Timer for individual T1 period
	uint16 t1_timer;
	// Timer for the probing phase
	uint16 probing_timer;
	// Timer for the waiting phase
	uint16 waiting_timer;
	// Number of subframes stayed in current antenna
	uint16 num_sf_stayed_in_current_ant;
	// Average power delta
	int16 avgPwrDelta;

	// Total number of Tx timeslots every T1 period
	int16 ant0_total_tx_slots[TDSSRCH_ASDIV_MAX_PROBE_CYCLE];
	// Number of timeslots which Tx exceeds MTPL every T1 period
	int16 ant0_exceed_mtpl[TDSSRCH_ASDIV_MAX_PROBE_CYCLE];
	//The index for ant0 MTPL array
	int8 ant0_mtpl_index;
	// Total number of Tx timeslots every T1 period
	int16 ant1_total_tx_slots[TDSSRCH_ASDIV_MAX_PROBE_CYCLE];
	// Number of timeslots which Tx exceeds MTPL every T1 period
	int16 ant1_exceed_mtpl[TDSSRCH_ASDIV_MAX_PROBE_CYCLE];
	//The index for ant1 MTPL array
	int8 ant1_mtpl_index;
	// Total uplink timeslots number in T1 period
	uint16 txpwr_total_num;
	// Total uplink timeslots number whose txpower reach mtpl in T1 period
	uint16 txpwr_reach_mtpl_num;
	
	// Save the switch complete parameters
	trm_switch_complete_input_type switch_complete_input;
	// if TRUE, means RF/FW is doing antenna switch
	boolean wait_switch_complete_flag;

	// if FALSE, we still need to initialize the ASDiv config during boot-up
	boolean config_initialized_when_asdiv_is_disabled;
	// The config granted from the callback of trm_asd_set_mode_and_config
	trm_asd_set_mode_and_config_type set_mode_and_config_result;
	// In DSDS mode, L1 will use tdsrf_enter_mode instead of tdsrf_wakeup.
	// So we need this to indicate that TDS is in DRX sleep but not the really doing enter mode. 
	boolean is_drx_sleep;
	// if TRUE, we have RSCP measurement during this DRX cycle
	boolean has_rscp_measurement_during_wakeup;
	// Traffic radio counts how many times its switch request was continuously denied by TRM. 
	// When the counter reach M (M is NV configurable for each tech), notify TRM by setting a “crisis flag” in the switch request
	uint8 crisis_mode_count;
	// Each idle radio shall count how many times it has changed its preferred antenna config continuously (back-to-back). 
	// When the number reaches N (N is NV configurable for each tech), stop changing preferred config for L (NV configurable) paging cycles. 
	uint8 idle_pingpong_count;
	uint8 idle_suspension_count;
	// How many subframes to force a switch in test mode
	uint64 test_mode_switch_period_connected;
	// How many sleep cycles to force a switch in test mode
	uint64 test_mode_switch_period_idle;
	// Current sleep cycle number
	uint64 test_mode_switch_count_idle;
} tdssrchset_active_cell_ante_switch_struct_type;

extern tdssrchset_active_cell_ante_switch_struct_type tdssrchset_scell_antenna_switch_info;
extern tdsl1_nv_antenna_switch_struct_type tdsschset_scell_antenna_switch_timer_threshold_nv;

#define TDSSRCH_ASDIV_TYPE1_ALGO_PROBING_PERIOD 100 //calsulate avgRSCP in every probing period 500ms, 100sfn
#define TDSSRCH_ASDIV_TYPE1_ALGO_RXD_PERIOD 50 //turn on DRx chain for Tp period of every 250ms, 50sfn
#define TDSSRCH_ASDIV_TYPE1_ALGO_LOOKBACK_TIME 600 //lookback time is 3s, 600sfn
#define TDSSRCH_ASDIV_TYPE1_ALGO_ARRAY_SIZE 20

typedef enum
{
	TDSSRCH_IDLE_PHASE,
	TDSSRCH_PROBING_FOR_SWITCH_PHASE,
	TDSSRCH_PROBING_FOR_SWITCH_BACK_PHASE,
	TDSSRCH_HOLDING_OFF_PHASE
} tdssrch_ante_switch_phase_type1_algo_enum_type;

typedef struct
{
	// Current state
	tdssrch_ante_switch_phase_type1_algo_enum_type ante_switch_phase;
	// Indicate PRx chain is on which antenna 
	// 0 means PRx chain on ant0(switch config is straight) 
	// 1 means PRx chain on ant1(switch config is straight)
	// But as "active_cell_ptr->rscp_x256" always means rscp from PRx chain, 
	// "rscp_x256_ant1" always means rscp from DRx chain, need to use this flag??
	uint8 prx_chain_no;
  
	// calcuate avgRSCP0(on PRx chain) in each probe period (500ms), unit is sfn
	uint16 probe_period;
	// turn on-off RxD chain in each rxd period (250ms), unit is sfn
	uint8 rxd_period;
	// calcuate avgRSCP1(on DRx chain) in each Tp period (75ms), unit is sfn
	uint8 tp_period;
	// holding off type1 algorithm period (3s), unit is sfn
	uint16 holding_off_period;
	
	// total RSCP for PRx chain in every IDLE/PROBING period, Q8, updated every TS0 rsp
	int32 prx_rscp;
	// Number of received RSCP values for PRx chain in current IDLE/PROBING period
	int16 prx_rscp_num_in_probing;
	// Average RSCP history for PRx chain, lookback time is 3s, probing period is 500ms, 
	// so we most need to save 6 history values for lookback
	int16 prx_rscp_history[TDSSRCH_ASDIV_TYPE1_ALGO_ARRAY_SIZE];
	// The index for PRx chain RSCP history array
	int8 prx_history_index;

	// Average RSCP for DRx chain in current PROBING period, Q8, updated every probing period
	// finally averaged at the end of PROBING period
	int32 drx_rscp;
	// RSCP for DRx chain is valid or not.
	boolean drx_rscp_is_valid;
	// Number of received RSCP values for DRx chain in current in every IDLE/PROBING period
	int16 drx_rscp_num_in_probing;
	
	// Average Tx power in current PROBING period, updated every subframe, Q4	
	// finally averaged at the end of PROBING period
	int32 txpwr;
	// Number of received Tx power values in current PROBING period
	int16 txpwr_num_in_probing;

	// PowerMetric in the probing period before switch, Q8
	int16 powerMetric_preSwitch;
	// PowerMetric in the probing period after switch, Q8
	int16 powerMetric_postSwitch;

	// Timer for the idle and probing phase
	uint8 probing_timer;
	// Timer for turn on-off RxD chain
	uint8 rxd_timer;
	// Timer for the holding off phase
	uint16 holding_off_timer;

	// Save the switch complete parameters
	trm_switch_complete_input_type switch_complete_input;
	// if TRUE, means RF/FW is doing antenna switch
	boolean wait_switch_complete_flag;
	// Traffic radio counts how many times its switch request was continuously denied by TRM. 
	// When the counter reach M (M is NV configurable for each tech), notify TRM by setting a “crisis flag” in the switch request
	uint8 crisis_mode_count;

	// The config granted from the callback of trm_asd_set_mode_and_config
	//trm_asd_set_mode_and_config_type set_mode_and_config_result;
} tdssrch_ante_switch_type1_algo_struct_type;

extern tdssrch_ante_switch_type1_algo_struct_type tdssrch_asdiv_type1_algo_params;
extern tdsl1_nv_asdiv_type1_algo_struct_type tdsl1_antenna_switch_type1_algo_params;
#endif

#define TDSSRCHSET_OLD_INTRA_CELL_HO_OR_RESELECTION 1
#define TDSSRCHSET_OLD_INTER_CELL_HO_OR_RESELECTION 2

extern uint8 tdssrchset_old_cells_bitmap_after_ho_or_reselection;


extern boolean tdssrchset_aset_from_intra_mcm ;
extern boolean tdssrchset_aset_from_inter_mcm ;



/*TBD,the values maybe changed later*/
#define TDSSRCHSET_Q_DETECT_OUT        (-105*256)
#define TDSSRCHSET_Q_DETECT_IN            (-102*256)
#define TDSSRCHSET_Q_JDCS_ABSOLUTE   (-102*256)
#define TDSSRCHSET_Q_JDCS_RELATIVE   \
     ((tdssrchset_active_cell.active_cell_ptr->filt_rscp_for_jdcs) -20*256 )
#define TDSSRCHSET_Q_JDCS                     \
      MAX(TDSSRCHSET_Q_JDCS_ABSOLUTE, TDSSRCHSET_Q_JDCS_RELATIVE)
      
#ifdef FEATURE_JDS_OPT
/* gives the threshold above which SW trusts TS-RSP values enough to be considered for JDS. Below that value SW essentially treats 
  * the TS-RSP value as a bogus value and the relevant cell won't go in JDS.*/
#define TDSSRCHSET_TS_RSP_THRESHOLD_ROUND_ROBIN      -110*256 
/* gives the initialization value for the TS-RSP table for the cells detected by searcher. All we need here is a value somehow higher 
  * then TSRSP_Threshold_RoundRobin_dBm to make sure that the interfering cells detected by searcher are measured without waiting 
  * for round robin to schedule them. */
#define TDSSRCHSET_INIT_TS_RSP_FROM_ACQ  (TDSSRCHSET_TS_RSP_THRESHOLD_ROUND_ROBIN + 10*256)
#endif

#ifdef FEATURE_TDSCDMA_MULTIPLE_L3_FILTERS_SUPPORTED
/* for multiple filter in DCH state */
#define TDSSRCHSET_GET_FILT_ISCP_MULTI_FILTER(cell_ptr,filt_idx,ts) (cell_ptr->iscp_info[ts].filtered_parms[filt_idx].filt_iscp)
#define TDSSRCHSET_GET_FILT_ENG(cell_ptr,filt_idx) (cell_ptr->dch_filtered_parms[filt_idx].filt_eng)
#define TDSSRCHSET_GET_FILT_RSCP_MULTI_FILTER(cell_ptr,filt_idx) (cell_ptr->dch_filtered_parms[filt_idx].filt_rscp)
#define TDSSRCHSET_GET_FILT_RSCP_MULTI_FILTER_x256(cell_ptr,filt_idx) (cell_ptr->dch_filtered_parms[filt_idx].filt_rscp_x256)
#define TDSSRCHSET_GET_FILT_RSCP_MULTI_FILTER_q16(cell_ptr,filt_idx) (cell_ptr->dch_filtered_parms[filt_idx].filt_rscp_q16)

#define TDSSRCHSET_GET_PATHLOSS_MULTI_FILTER(cell_ptr,filt_idx) (cell_ptr->dch_filtered_parms[filt_idx].pathloss)
#define TDSSRCHSET_GET_PATHLOSS_MULTI_FILTER_x256(cell_ptr,filt_idx) (cell_ptr->dch_filtered_parms[filt_idx].pathloss_x256)

#define TDSSRCHSET_GET_FILT_LEN(cell_ptr,filt_idx) (cell_ptr->dch_filtered_parms[filt_idx].filt_len)

#define TDSSRCHSET_SET_FILT_ISCP_MULTI_FILTER(cell_ptr,filt_idx,ts,val) (cell_ptr->iscp_info[ts].filtered_parms[filt_idx].filt_iscp = val)
#define TDSSRCHSET_SET_FILT_RSCP_MULTI_FILTER(cell_ptr,filt_idx,val) (cell_ptr->dch_filtered_parms[filt_idx].filt_rscp = val)
#define TDSSRCHSET_SET_FILT_RSCP_MULTI_FILTER_x256(cell_ptr,filt_idx,val) (cell_ptr->dch_filtered_parms[filt_idx].filt_rscp_x256 = val)
#define TDSSRCHSET_SET_FILT_RSCP_MULTI_FILTER_q16(cell_ptr,filt_idx,val) (cell_ptr->dch_filtered_parms[filt_idx].filt_rscp_q16 = val)

#define TDSSRCHSET_SET_PATHLOSS_MULTI_FILTER(cell_ptr,filt_idx,val) (cell_ptr->dch_filtered_parms[filt_idx].pathloss = val)
#define TDSSRCHSET_SET_PATHLOSS_MULTI_FILTER_x256(cell_ptr,filt_idx,val) (cell_ptr->dch_filtered_parms[filt_idx].pathloss_x256 = val)

#define TDSSRCHSET_SET_FILT_LEN(cell_ptr,filt_idx,val) (cell_ptr->dch_filtered_parms[filt_idx].filt_len = val)
#define TDSSRCHSET_INCREMENT_FILT_LEN(cell_ptr,filt_idx) (cell_ptr->dch_filtered_parms[filt_idx].filt_len ++)
#endif /* FEATURE_TDSCDMA_MULTIPLE_L3_FILTERS_SUPPORTED */

#define TDSSRCHSET_GET_FILT_ISCP(cell_ptr,ts) (cell_ptr->iscp_info[ts].filt_iscp)
#define TDSSRCHSET_GET_FILT_RSCP(cell_ptr) (cell_ptr->filt_rscp)
#define TDSSRCHSET_GET_FILT_RSCP_x256(cell_ptr) (cell_ptr->filt_rscp_x256)
#define TDSSRCHSET_GET_PATHLOSS(cell_ptr) (cell_ptr->pathloss)
#define TDSSRCHSET_GET_PATHLOSS_x256(cell_ptr) (cell_ptr->pathloss_x256)
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
#define TDSSRCHSET_GET_FILT_ECIO(cell_ptr,filt_idx) (cell_ptr->dch_filtered_parms[filt_idx].filt_ecio)
#endif
#define TDSSRCHSET_SET_PATHLOSS(cell_ptr,val) (cell_ptr->pathloss = val)
#define TDSSRCHSET_SET_PATHLOSS_x256(cell_ptr,val) (cell_ptr->pathloss_x256 = val)


/*--------------------------------------------------------------------------
                        INTER-RAT CELL STRUCTURE
--------------------------------------------------------------------------*/
#if defined(FEATURE_TDSCDMA_CELL_RESELECT_INTER_S)  

#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
/* The following struct is populated by the third meas request
   from the RCC (SIB 19), It is used for absolute priority */
typedef struct
{
  int8 priority;
  int16 q_rxlevmin_gsm; 
  uint8 threshx_high;  
  uint8 threshxlow;  
} tdssrchset_inter_rat_gsm_priority_info_type;
#endif

typedef struct tdssrchset_gsm_cell_struct
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
  boolean                 rssi_used[TDSL1_MAX_INTERRAT_EVENTS];
    
  /* Check to see if rssi value is used atleast once for periodic reporting
     before aging starts */
  boolean                 rssi_used_periodic;

  tdssrchset_gsm_set_enum_type  set;  
    /* Which Set the Cell currently belongs to */  

#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
  /* Indicates if priority info present for this ARFCN */
  boolean prior_info_present;

  boolean cell_qualify_for_prio;

  /* Priority information for this ARFCN */
  tdssrchset_inter_rat_gsm_priority_info_type prior_info;
#endif

  tdsl1_gsm_cell_info_type   gsm_info;     
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

  void * drv_cell_ptr;
    /* driver Cell Pointer, valid if measure_cell == TRUE after addition */
  boolean rssi_valid;
  /*unit:1/16 dbm*/
  int16   last_rssi;
  /* last measured rssi */
  /*unit:1/16 dbm*/
  int32 filtered_rssi[TDSL1_MAX_FILTER_COEF_STORAGE];
    /* if filter_coef=INVALID, value set to last_rssi, 
     otherwise Fn as in 8.6.7.2 
   */
  /*unit:1/16 dbm*/
  int16 filt_rssi; /*++++++ this is a new field ++++++*/
  /* filtered rssi in FACH mode */

  boolean cell_reported[TDSL1_MAX_MEAS_EVENT];
  struct  tdssrchset_gsm_cell_struct * sort_next;

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
  tdssrchset_evt_ts_struct_type  evt_cr_ts;

#ifdef FEATURE_TDSCDMA_HCS
  /* HCS related info for the cell */
  tdssrchset_cell_hcs_info_struct_type cell_hcs_info;
#endif

  /* Flag indicates whether a cell is barred or not*/
  boolean cell_barred;

} 
tdssrchset_gsm_cell_struct_type;

/* used to sort maintain rssi based sorted list of GSM NSET */
extern tdssrchset_gsm_cell_struct_type * tdssrchset_gsm_rssi_nset_sort_head;

#endif

#ifdef FEATURE_TDSCDMA_HCS
/*--------------------------------------------------------------------------
                  BACKUP FDD CELL STRUCTURE
--------------------------------------------------------------------------*/
typedef struct
{
  /* WCDMA Cell Scrambling Code */
  uint16  scr_code;
    
  /* Penalty timer for this cell (neighbor) */
  tdssrchset_evt_ts_struct_type  evt_cr_penalty_timer;
}
tdssrchset_backup_tdd_cell_struct_type;

/*--------------------------------------------------------------------------
                  BACKUP GSM CELL STRUCTURE
--------------------------------------------------------------------------*/
#if defined(FEATURE_TDSCDMA_CELL_RESELECT_INTER_S)
typedef struct 
{
/*
 * Common SIB & Measurement Control Info
 */
  tdsl1_gsm_cell_info_type   gsm_info;     
    /* cell individ offset, bsic, band_indicator, arfcn */

  /* Penalty timer for this cell (neighbor) */
  tdssrchset_evt_ts_struct_type  evt_cr_penalty_timer;
} 
tdssrchset_backup_gsm_cell_struct_type;
#endif


/*--------------------------------------------------------------------------
                  BACKUP CELL STRUCTURE
--------------------------------------------------------------------------*/
/* Enum to indicate the state of backup structure */
typedef enum
{
  TDSSRCHSET_BACKUP_INVALID,
  TDSSRCHSET_BACKUP_UPDATED,
  TDSSRCHSET_BACKUP_VALID
}
tdssrchset_backup_info_state_type;

typedef struct
{
  /* Field indicating state of backup structure */
  tdssrchset_backup_info_state_type  cell_backup_state;

  /* Time of backup in seconds since power up*/
  dword    cell_backup_time;

  /* TDD frequeny info for the tdd cell info structure */
  uint16 tdd_arfcn[1
#ifdef FEATURE_TDSCDMA_CELL_RESELECT_INTER_F
           + TDSSRCHSET_FREQ_MAX - 1
#endif
           ];

  /* End index in the fdd cell info structrue for a particular frequency */
  uint16 tdd_end_index[1
#ifdef FEATURE_TDSCDMA_CELL_RESELECT_INTER_F
           + TDSSRCHSET_FREQ_MAX - 1
#endif
           ];

  /* TDD cell info to be backed up */
  tdssrchset_backup_tdd_cell_struct_type  tdd_cell_info [TDSL1_MAX_CELL_MEAS
#ifdef FEATURE_TDSCDMA_CELL_RESELECT_INTER_F
           + TDSL1_MAX_CELL_MEAS
#endif
           ];

  /* GSM cell info to be backed up */
#ifdef FEATURE_TDSCDMA_CELL_RESELECT_INTER_S
  uint16 gsm_cell_cnt;
  tdssrchset_backup_gsm_cell_struct_type gsm_cell_info[TDSL1_MAX_CELL_MEAS];
#endif
}
tdssrchset_backup_cell_struct_type;

#endif  /* FEATURE_TDSCDMA_HCS */

/*--------------------------------------------------------------------------
           Exported variables and functions from tdssrchset.c 
--------------------------------------------------------------------------*/        
  /* Table which contains all the intra- and inter-frequency cells that 
     need to be searched 
   */
extern tdssrchset_cell_struct_type_tmp  tdssrchset_search_cells[TDSSRCHSET_CELL_TABLE_MAX];

/* Number of empty cells in the cell table */
extern uint8 tdssrchset_emptyset_cnt;

extern tdssrchset_cell_struct_type  *tdssrchset_aset_ptr[SRCH_ASET_MAX];   
  /* ASET index table */
extern uint8 tdssrchaset_cnt;          
  /* Number of elements in ASET */

#ifdef  FEATURE_TDSCDMA_HCS 
extern uint8 tdshcs_intra_nset_cnt;
/* HCS intra sync and async set */

extern uint8 tdshcs_interrat_nset_cnt;
/* HCS inter RAT set */
#endif 

#if defined(FEATURE_TDSCDMA_CELL_RESELECT_INTER_S)  
extern tdssrchset_gsm_cell_struct_type  tdssrchset_gsm_cells[TDSSRCHSET_GSM_CELL_MAX];
  /* Table which contains all the interRAT GSM cells to be searched */

extern tdssrchset_gsm_cell_struct_type  *tdssrchset_gsm_nset_ptr[TDSSRCHSET_GSM_CELL_MAX];
  /* GSM_NSET index table */
extern uint8 tdssrchset_gsm_nset_cnt;          
  /* Number of GSM cells in GSM_NSET */

extern tdssrchset_gsm_cell_struct_type  *tdssrchset_gsm_uset_ptr[TDSSRCHSET_GSM_CELL_MAX];
  /* GSM_USET index table */
#endif


/* When serving cell is in inter freq, and ISCP configured, set to TRUE*/
extern uint8 tdssrchset_aset_iscp_timeslot_num;
extern tdssrchset_iscp_struct_type tdssrchset_aset_iscp_info[L1_MAX_TS];

extern uint8 tdssrchset_aset_ts_num_configured_in_intra_different_from_primary;
extern uint8 tdssrch_set_aset_non_ts0_timeslot[L1_MAX_TS];

extern boolean tdssrchset_aset_iscp_from_intra_mcm;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*=========================================================================

FUNCTION     : TDSSRCHSET_GET_CELL_INTLOCK 
               
DESCRIPTION  : This function tries to acquire the mutex tdssrchset_get_cell_crit_sect.
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : The function can block until the mutex is available for locking

=========================================================================*/
extern void tdssrchset_get_cell_intlock( void );

/*=========================================================================
FUNCTION     : TDSSRCHSET_GET_CELL_INTFREE 
               
DESCRIPTION  : This function releases the mutex tdssrchset_get_cell_crit_sect.
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void tdssrchset_get_cell_intfree( void );

/*=========================================================================

FUNCTION     : TDSSRCHSET_RESET_ALL_EVENTS

DESCRIPTION  : This function resets the MR database.
             
DEPENDENCIES : None

RETURN VALUE : void
      
SIDE EFFECTS : None

=========================================================================*/
extern void tdssrchset_reset_all_events(void);


/*=========================================================================

FUNCTION     : tdssrchset_resume_cells_to_saved_set

DESCRIPTION  :.
             
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void tdssrchset_resume_cells_to_saved_set(void);

#if defined(FEATURE_TDSCDMA_CELL_RESELECT_INTER_S) 
/*=========================================================================

FUNCTION     : TDSSRCHSET_INTER_S_SORT_CELL_IN_GSMSET

DESCRIPTION  : This function creates a linked list of sorted cells based
               on filtered rssi specified by fc_index.  Highest to lowest.
               Currently this function is only valid for GSM_SET_N.
             
DEPENDENCIES : None

INPUT PARAMETERS 
  tdssrchset_gsm_set_enum_type  set: indicates which set  need be sorted.
  uint32  fc_index: the index in filtered_rssi[]
  
RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void tdssrchset_inter_s_sort_cell_in_gsmset
(
  tdssrchset_gsm_set_enum_type  set,
  uint32  fc_index
);

/*=========================================================================

FUNCTION     : TDSSRCHSET_INTER_S_PUT_CELL_IN_GSMSET

DESCRIPTION  : This function puts the specified cell in the specified GSM SET
             
DEPENDENCIES : None

INPUT PARAMETERS 
  tdssrchset_gsm_cell_struct_type*   cell_ptr: the specified cell's pointer
  tdssrchset_gsm_set_enum_type       set: the specified GSM SET
  
RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void tdssrchset_inter_s_put_cell_in_gsmset
(
  tdssrchset_gsm_cell_struct_type*   cell_ptr,
  tdssrchset_gsm_set_enum_type       set
);

/*=========================================================================

FUNCTION     : TDSSRCHSET_DELETE_CELL_FROM_GSMSET 

DESCRIPTION  : This function deletes a gsm cell from GSM SET which it belongs to. 
               
DEPENDENCIES : None

INPUT PARAMETERS :
  tdssrchset_gsm_cell_struct_type*  cell_ptr: the gsm cell's pointer
  
RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void tdssrchset_inter_s_delete_cell_from_gsmset
( 
  tdssrchset_gsm_cell_struct_type*  cell_ptr
);

/*=========================================================================

FUNCTION     : TDSSRCHSET_RESET_USET_IN_GSMSET

DESCRIPTION  : This function clear GSM USET.
             
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void tdssrchset_inter_s_reset_gsm_uset(void);

/*===========================================================================
FUNCTION     TDSSRCHSET_INTER_S_INIT_ARFCN_ARRAY

DESCRIPTION
  clear the unique_arfcn_array[] and the unique ARFCN count.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdssrchset_inter_s_init_arfcn_array( void );

/*=========================================================================

FUNCTION     : TDSSRCHSET_INTER_S_INIT_CELL 
               
DESCRIPTION  : This function initializes the Inter-Rat cell structure.
               The cell is placed in the empty set and additional parameters
               are set appropriately.
  
DEPENDENCIES : None

INPUT PARAMETERS :
  uint16 cell_index: the cell index in tdssrchset_gsm_cells[].

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void tdssrchset_inter_s_init_cell( uint16 cell_index );

/*======================================================================
FUNCTION TDSSRCHSET_INTER_S_RMV_CELL

DESCRIPTION
    Used to remove one cell from the inter_rat cell list

DEPENDENCIES : None

INPUT PARAMETERS: 
   uint16 cellid: the index of tdssrchset_gsm_cells[]

RETURN VALUE: None
      
SIDE EFFECTS : None

=======================================================================*/
extern void tdssrchset_inter_s_rmv_cell( uint16 cellid );

/*======================================================================
FUNCTION TDSSRCHSET_INTER_S_RMV_ALL_CELLS

DESCRIPTION
    Used to remove *all* cells from the Inter-RAT database

DEPENDENCIES : None

INPUT PARAMETERS: None

RETURN VALUE: None
      
SIDE EFFECTS : None

=======================================================================*/
extern void tdssrchset_inter_s_rmv_all_cells ( void );

/*======================================================================
FUNCTION TDSSRCHSET_INTER_S_RMV_CELLS

DESCRIPTION
    Used to remove cells from the Inter-RAT Neighbor set and srch list 
    using the information received in a TDSL1_CPHY_MEASUREMENT_REQ message.

DEPENDENCIES : None

INPUT PARAMETERS: 
  tdsl1_inter_sys_cell_list_struct_type *cell_list: cell list from MCM.
  
RETURN VALUE: None
      
SIDE EFFECTS : None

=======================================================================*/
extern void tdssrchset_inter_s_rmv_cells
(
  tdsl1_inter_sys_cell_list_struct_type *cell_list,
  tdsl1_meas_ctrl_enum_type              parm_type
);

/*======================================================================
FUNCTION TDSSRCHSET_INTER_S_GET_CELLPTR

DESCRIPTION
    Used to get an  Inter-RAT cell pointer.

DEPENDENCIES : None

INPUT PARAMETERS: 
   uint16 cellid: the index of tdssrchset_gsm_cells[]

RETURN VALUE: None
      
SIDE EFFECTS : None

=======================================================================*/
extern tdssrchset_gsm_cell_struct_type * tdssrchset_inter_s_get_cellptr
(
  uint16 cell_id
);

/*===========================================================================
FUNCTION     TDSSRCHSET_INTER_S_FIND_CELL

DESCRIPTION
  This function will search all neighbor set cells to find one that
  has an ARFCN and BSIC that match those provided.

DEPENDENCIES
  None

INPUT PARAMETERS 
  W_ARFCN_T arfcn: band and frequency information.
  uint8 bsic_ncc: network color code
  uint8 bsic_bcc: base transceive station color code

RETURN VALUE
  tdssrchset_gsm_cell_struct_type * - if not NULL, a pointer to the cell structure
                                for which ARFCN,BSIC match.  NULL if no match
                                was found.
SIDE EFFECTS
  None
===========================================================================*/
extern tdssrchset_gsm_cell_struct_type * tdssrchset_inter_s_find_cell
( W_ARFCN_T arfcn, uint8 bsic_ncc, uint8 bsic_bcc );

/*======================================================================
FUNCTION TDSSRCHSET_INTER_S_ADD_CELLS

DESCRIPTION
    Used to add cells to the Inter-RAT Neighbor set and srch list using
    the information received in a TDSL1_CPHY_MEASUREMENT_REQ command message.

DEPENDENCIES
  None

INPUT PARAMETERS 
  tdsl1_inter_sys_cell_list_struct_type *cell_list: inter-RAT cell list from MCM.
  tdsl1_meas_ctrl_enum_type  parm_type: Identifies whether SIB, or Measurement Control Parameters
  are contained within the measurement command.
  boolean bsic_verify_reqd: the flag indicates if bsic verify is required.
  uint32  meas_id: if the inter-RAT cell list is from SIB, the input parameter is TDSL1_MEAS_ID_INVALID;
    othersize, its value is measurement control message id.
  
RETURN VALUE
    boolean
    
SIDE EFFECTS
  None
=======================================================================*/
extern boolean tdssrchset_inter_s_add_cells
(
  tdsl1_inter_sys_cell_list_struct_type *cell_list,
  tdsl1_meas_ctrl_enum_type              parm_type,
  boolean bsic_verify_reqd,
  uint32  meas_id
);

#endif /* (FEATURE_TDSCDMA_CELL_RESELECT_INTER_S)   */

/*===========================================================================
FUNCTION     TDSSRCHSET_CLEAR_NBCH_CELL

DESCRIPTION
  This function clears the NBCH cell. 

DEPENDENCIES
  None.

PARAMETERS
 None.
 
RETURN VALUE
  none

SIDE EFFECTS
   None
===========================================================================*/
extern void tdssrchset_clear_nbch_cell( void );

/*=========================================================================

FUNCTION     : TDSSRCHSET_LOCK _FREQ_SET
               
DESCRIPTION  : This function will lock the frequency set. 

DEPENDENCIES : None

INPUT PARAMETERS :
    uint16 freq: the frequency information.
    
RETURN VALUE : None
      
SIDE EFFECTS : measurement module cannot visit all cells' information on the frequency.
=========================================================================*/
LOCAL void tdssrchset_lock_freq_set( uint16 freq );

/*=========================================================================

FUNCTION     : TDSSRCHSET_UNLOCK _FREQ_SET
               
DESCRIPTION  : This function will unlock the frequency set, then measurement module can visit all 
                         cells' information on the frequency.
  
DEPENDENCIES : None

INPUT PARAMETERS :
    uint16 freq: the frequency information.

RETURN VALUE : None
      
SIDE EFFECTS : 

=========================================================================*/
LOCAL void tdssrchset_unlock_freq_set( uint16 freq );

/*=========================================================================

FUNCTION     : TDSSRCHSET_UNLOCK _ALL_FREQ_SETS
               
DESCRIPTION  : This function will unlock all frequency sets.
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : 

=========================================================================*/
LOCAL void tdssrchset_unlock_all_freq_sets( void );

/*=========================================================================

FUNCTION     : TDSSRCHSET_FREQ _SET_ACCESS_RIGHTS_FOR_MEAS
               
DESCRIPTION  : When measurement module need visit the frequency set, it need call the function to make sure 
                         if the frequency set is allowed to visit firstly. 
  
DEPENDENCIES : None

INPUT PARAMETERS :
    uint8 freq_index : the frequency index in freq sets
    
RETURN VALUE : 
    boolean: if its value is TRUE, the frequency set is allowed to visit. Otherwise, it cannot be visited.
    
SIDE EFFECTS : 

=========================================================================*/
extern boolean tdssrchset_freq_set_access_rights_for_meas( uint8 freq_index );

/*=========================================================================

FUNCTION     : TDSSRCHSET_FREQ _SET_ACCESS_RIGHTS_FOR_MEAS
               
DESCRIPTION  : The function is provided for measurement module to check if the frequency set is changed. 
  
DEPENDENCIES : None

INPUT PARAMETERS :
    uint16 freq: the frequency information.
    uint8 freq_index : the frequency index in freq sets
    
RETURN VALUE : 
    boolean: if its value is FALSE, There are uncoordinated freq relations between measurement and set 
    management. i.e. frequency set is changed.
    
SIDE EFFECTS : 

=========================================================================*/
extern boolean tdssrchset_check_freq_index_for_meas( uint16 freq, uint8 freq_index );

/*=========================================================================

 FUNCTION     :TDSSRCHSET_GET_EMPTY_CELL
 
 DESCRIPTION  : This function returns an empty slot in the cell table. Typically when a new cell is to be added to 
 the set table, an empty slot is obtained in the cell table and is filled up with the appropriate cell information. 
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
 cell_ptr   = Pointer to an empty slot in the cell table
 NULL      = If no empty slot available in the cell table
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL tdssrchset_cell_struct_type_tmp* tdssrchset_get_empty_cell( void );

/*===========================================================================

FUNCTION     TDSSRCHSET_INIT_DSET_CELL_PARAMS

DESCRIPTION
  After one unlisted cell is detected, use some default values to init its cell structure because
  NW didn't tell UE about their values.

DEPENDENCIES
  None.

INPUT PARAMETERS
 parameter cell_ptr is the pointer to the cell which is detected.
 
RETURN VALUE
  none
  
SIDE EFFECTS
   
===========================================================================*/
LOCAL void tdssrchset_init_dset_cell_params(tdssrchset_cell_struct_type_tmp  *cell_ptr);

/*===========================================================================

FUNCTION     TDSSRCHSET_UPDATE_DSET_PARAMS

DESCRIPTION
  Update all DSET cells' parameters. Normally, if serving cell's parameters are updated, we need call the function
  to update all DSET cells' parameters. Because, some DSET cell information is from serving cell.

DEPENDENCIES
  None.

RETURN VALUE
  none
  
SIDE EFFECTS
   
===========================================================================*/
extern void tdssrchset_update_dset_params(void);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_INIT_FREQ_SET
 
 DESCRIPTION  : initialize the appointed frequency set. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   freq_index: frequency index in frequency sets
   
 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL void tdssrchset_init_freq_set( uint8 freq_index );

/*=========================================================================

 FUNCTION     :TDSSRCHSET_INIT_SETS
 
 DESCRIPTION  : This function initializes the search database by putting all cells in the Empty Set and initializes 
                          cell parameters of the cell structures in searcher table, and initializes the active cell structure 
                          and all sets structure.
              
 DEPENDENCIES : 
   
 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_init_sets( void );

/*=========================================================================

 FUNCTION     :TDSSRCHSET_INIT_CELL_PARAMS
 
 DESCRIPTION  : This function will initialise cell parameters of the cell structures. 
 These will be done when cells are allocated on TDSCDMA start and also during every
 state transition through IDLE.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdssrchset_cell_struct_type *cell_ptr: the pointer points to a cell structure to be initialised.
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_init_cell_params(tdssrchset_cell_struct_type_tmp *cell_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_GET_CELLS_FOR_SELECTION
 
 DESCRIPTION  : The function is used to get candidate cells for selection. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint16 freq: the appointed frequency.
   boolean single_cell: if the parameter is TRUE, "cpi" parameter is valid.
   uint16 cpi: cell parameter id.
   uint8* cell_num : a pointer in which the function need put the number of candidate cells for selection.
   uint8* cpid_list : a pointer to cpi array in which the function need put all cpids of candidate cells for selection.
 
 RETURN VALUE : boolean
   TRUE: L1 SW doesn't need measure the Freq. 
   FALSE: L1 SW need measure the Freq.
   
 SIDE EFFECTS : None
==========================================================================*/
boolean tdssrchset_get_cells_for_selection
( 
  uint16 freq,       
  boolean single_cell,
  uint8 cpi,    
  uint8 *cell_num,  
  uint8 *cpid_list
);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_GET_TS0_JDS
 
 DESCRIPTION  : The function is used to update TS0-JDS. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   * cell_num : a pointer in which the function need put the number of TS0 JDS.
   * cpid_list : a pointer to cpi array in which the function need put all detected cpids of TS0 JDS cells.
 
 RETURN VALUE : None
   
 SIDE EFFECTS : None
==========================================================================*/
extern void tdssrchset_get_ts0_jds( uint8 *cell_num,  uint8 * cpid_list, tdssrchset_cell_struct_type_tmp* cell_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_START_SET_MANAGEMENT
 
 DESCRIPTION  : After acquisition step 1 done, the function will be called to start set management, i.e. establish 
 active cell, JDCS and TS0 JDS in set management. The input parameters are detected cells' information in step 1.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tfw_srch_syncdl_midamble_detect_rsp_t *syncdl_midamble_detect_rsp : detected cells' information in 
   acquisition step 1
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_start_set_management(tfw_srch_syncdl_midamble_detect_rsp_t  *syncdl_midamble_detect_rsp);

/*======================================================================
 FUNCTION     :TDSSRCHSET_PRINT_CELL_COUNT
 
 DESCRIPTION  : print frequency count, and MSET/DSET/JDCS cell counts.
              
 DEPENDENCIES : 
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
=======================================================================*/
LOCAL void tdssrchset_print_cell_count(void);

/*======================================================================
 FUNCTION     :TDSSRCHSET_PRINT_CELL_COUNT
 
 DESCRIPTION  : check total cell number for every freq.
              
 DEPENDENCIES : 
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
=======================================================================*/
extern void tdssrchset_check_cell_count(void);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_FIND_CELL
 
 DESCRIPTION  : Given a frequency and cell parameter id, this function loops through tdssrchset_freq_set[] arrays 
 and returns the matching cell pointer.
              
 DEPENDENCIES : 

 INPUT PARAMETERS :
   uint16 freq : the TDS frequency of the target cell.
   uint16 cpi : the cell parameter id of the target cell

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern tdssrchset_cell_struct_type_tmp* tdssrchset_find_cell(uint16 freq, uint16 cpi);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_FIND_JDCS_CELL
 
 DESCRIPTION  : Given a cell parameter id, this function loops through tdssrchset_freq_set[] arrays 
 and returns the matching cell JDCS cell pointer.
              
 DEPENDENCIES : 

 INPUT PARAMETERS :
   uint16 cpi : the cell parameter id of the target JDCS cell

 RETURN VALUE : the matching cell JDCS cell pointer
       
 SIDE EFFECTS : 
==========================================================================*/
extern tdssrchset_cell_struct_type_tmp* tdssrchset_find_jdcs_cell(uint16 cpi);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_FIND_CELL_LOCATION
 
 DESCRIPTION  : Given a frequency and cell parameter id, this function loops through tdssrchset_freq_set[] arrays 
 and returns the matching cell pointer, and frequency index and cell index.
              
 DEPENDENCIES : 

 INPUT PARAMETERS:
   uint16 freq : the TDS frequency of the target cell.
   uint16 cpi : the cell parameter id of the target cell
   uint8 * freq_index_ptr : a address is used to put frequency index in frequency sets
   uint8 * cell_index_ptr : a address is used to put cell index in frequency sets
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL tdssrchset_cell_struct_type_tmp * tdssrchset_find_cell_location(uint16 freq, uint16 cpi, uint8 * freq_index_ptr, uint8 * cell_index_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_FIND_FREQ_IDX
 
 DESCRIPTION  : Given a frequency, this function loops through tdssrchset_freq_set[] arrays 
 and returns the frequency index.
              
 DEPENDENCIES : 

 INPUT PARAMETERS :
   uint16 freq : the TDS frequency of the target cell.

 RETURN VALUE : if found, return frequency index. Otherwise, return 0xFF.
       
 SIDE EFFECTS : 
==========================================================================*/
extern uint8 tdssrchset_find_freq_idx(uint16 freq);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_FREE_CELL
 
 DESCRIPTION  : free a cell structure to the search table, and reset its related fields.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
    tdssrchset_cell_struct_type *cell_ptr: the pointer points to a cell structure to be freed.

 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL void tdssrchset_free_cell(tdssrchset_cell_struct_type_tmp* cell_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_FREE_FREQ
 
 DESCRIPTION  : Free all cell structures on the frequency, and reset related fields about the frequency.
              
 DEPENDENCIES : 
 
 RETURN VALUE : None.
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL void tdssrchset_free_freq(uint8 freq_index);

/*======================================================================
 FUNCTION     :TDSSRCHSET_GET_WEAKEST_DSET_IDX
 
 DESCRIPTION  : Returns the weakest cell placed in DSET.
              
 DEPENDENCIES : 

 INPUT PARAMETERS :
    uint8 freq_index: the frequency index in frequency sets.
    boolean can_be_deleted: if its value is TRUE, we find the weakest DSET cell, but its cell structure is being 
                              used, we can not delete the cell, and we need find the second weakest DSET cell. The 
                              rest can be done in the same manner.
 
 RETURN VALUE : value of the index. 0xff, if none is available
       
 SIDE EFFECTS : 
=======================================================================*/
LOCAL uint8 tdssrchset_get_weakest_dset_idx (uint8 freq_index, boolean can_be_deleted);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_UPDATE_ASET_FREQ_INDEX
 
 DESCRIPTION  : Update primary frequency index and working frequency index in frequency sets.
              
 DEPENDENCIES : 
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL void tdssrchset_update_aset_freq_index(void);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_ADD_CELL_TO_MSET
 
 DESCRIPTION  : This function is used to add a cell to MSET.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdssrchset_cell_struct_type* cell_ptr : the cell structure pointer of the target cell 
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL void tdssrchset_add_cell_to_mset( tdssrchset_cell_struct_type_tmp*  cell_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_REMOVE_CELL_FROM_MSET
 
 DESCRIPTION  : This function is used to remove a cell from MSET.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdssrchset_cell_struct_type* cell_ptr : the cell structure pointer of the target cell 

 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL void tdssrchset_remove_cell_from_mset(tdssrchset_cell_struct_type_tmp *cell_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_ADD_CELL_TO_DSET
 
 DESCRIPTION  : After a cell is detected, measurement module will call the function to add the remaining cell to 
 detected set. If its return value is NULL, it indicates that DSET is full, and either the RSET cell's RSCP value is below 
 all DSET cells' or the RSET cell has replaced one DSET cell. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
 
 RETURN VALUE : If its return value is a DSET cell structure pointer, it indicates that the RSET cell has been added 
 into DSET.
       
 SIDE EFFECTS : 
==========================================================================*/
extern tdssrchset_cell_struct_type_tmp* tdssrchset_add_cell_to_dset(tdssrchset_rset_cell_struct_type* cell_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_DROP_CELL_FROM_DSET
 
 DESCRIPTION  : If a detected cell's filtered RSCP is below Qdetect_out, measurement module need call the  
   function to move the cell from detected set to remaining set.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdssrchset_cell_struct_type* cell_ptr : the cell structure pointer of the target cell 
   
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_drop_cell_from_dset(tdssrchset_cell_struct_type_tmp* cell_ptr);
/*=========================================================================

 FUNCTION     :TDSSRCHSET_UPDATE_JDCS
 
 DESCRIPTION  : Sort all cell's RSCP and get the strongest 8 cells, subject to meeting an RSCP threshold. After a 
 frequency is measured, measurement module will call the function to update JDCS information.
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL void tdssrchset_update_jdcs(void);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_RSCP_SORT_FREQ_SET
 
 DESCRIPTION  : After a frequency is measured and all cells are filtered on the frequency, measurement module need 
 call the function to sort all cells on the frequency in the RSCP descending order. Because the number of cells on each 
 frequency is not big and measurement results are ordered by and large most of the time, we adopt insert-sorting.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint8 freq_index: frequency index in frequency sets
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_rscp_sort_freq_set(uint8 freq_index);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_ASET_UPDATE
 
 DESCRIPTION  : The function will be used for updating serving cell information and JDCS. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint16 primary_freq: new serving cell's primary frequency
   uint16 working_freq: new serving cell's working frequency
   uint8 cpi: new serving cell's cell parameter id
   
 RETURN VALUE : 
   boolean: if serving cell will change, return TRUE; Otherwise, return FALSE.
   
 SIDE EFFECTS : 
==========================================================================*/
extern boolean tdssrchset_aset_update(uint16 primary_freq, uint16 working_freq, uint8 cpi);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_SETUP_HO_CELL
 
 DESCRIPTION  : The function will be used to setup handover cell data structure. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint16 primary_freq: new serving cell's primary frequency
   uint16 working_freq: new serving cell's working frequency
   uint8 cpi: new serving cell's cell parameter id
   
 RETURN VALUE : 
   
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_setup_ho_cell(uint16 primary_freq, uint16 working_freq, uint8 cpi);

extern uint8 tdssrchset_init_interf_for_measure_all( uint16 freq);
/*=========================================================================

 FUNCTION     :TDSSRCHSET_CLEAN_FREQ_SETS
 
 DESCRIPTION  : After some inter-frequencies' cells are removed, the function is used to remove inter-frequencies 
                          without any cells.
              
 DEPENDENCIES : 
 
 RETURN VALUE : Return TRUE if there is any frequency which is removed, else it will return FALSE.
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL boolean tdssrchset_clean_freq_sets(void);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_ADD_INTERF_CELLS
 
 DESCRIPTION  : Used to add cells to the inter-frequency NSETs using the information
    received in a CPHY_MEASUREMENT_REQ command message.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdsl1_inter_freq_cell_list_struct_type *cell_list : Inter-frequency cell list information
 
 RETURN VALUE : 
 
 SIDE EFFECTS : 
==========================================================================*/
LOCAL void tdssrchset_add_interf_cells( tdsl1_inter_freq_cell_list_struct_type *cell_list);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_REMOVE_INTERF_CELLS
 
 DESCRIPTION  :     Used to remove cells from the inter-frequency NSETs using the 
    information received in a CPHY_MEASUREMENT_REQ command message.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdsl1_inter_freq_cell_list_struct_type *cell_list : Inter-frequency cell list information
 
 RETURN VALUE : None
       
 SIDE EFFECTS : 
==========================================================================*/
LOCAL void tdssrchset_remove_interf_cells( tdsl1_inter_freq_cell_list_struct_type *cell_list);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_UPDATE_INTERF_CELL_INFO
 
 DESCRIPTION  : This function is used to obtain a new inter-frequency cell list.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdsl1_inter_freq_cell_list_struct_type *cell_list_ptr : point to intra-cell list  from SIB/MCM
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_update_interf_cell_info( 
    tdsl1_inter_freq_cell_list_struct_type *cell_list);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_UPDATE_INTERF_CELL_INFO
 
 DESCRIPTION  : This function is used to obtain a new intra-cell list.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdsl1_intra_freq_cell_list_struct_type *cell_list_ptr: Intra-frequency cell list information
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_update_intraf_cell_info( 
    tdsl1_intra_freq_cell_list_struct_type *cell_list_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_RESET_INTERF_CELL_LIST
 
 DESCRIPTION  : This function is used to .
              
 DEPENDENCIES : 
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_reset_interf_cell_list(void);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_RESET_INTRAF_CELL_LIST
 
 DESCRIPTION  : This function is used to reset intra-frequency cell list..
              
 DEPENDENCIES : 
 
 RETURN VALUE : None
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_reset_intraf_cell_list(void);

/*===========================================================================
FUNCTION        tdssrchset_ho_cell_become_serving_cell

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  void.

SIDE EFFECTS    
===========================================================================*/
extern void tdssrchset_ho_cell_become_serving_cell(void);

/*===========================================================================
FUNCTION         TDSSRCHSET_ADD_TO_NBCH_CELL

DESCRIPTION      
    This function is called by L1M everytime a neighbor PCCPCH setup is needed.
    The "tdssrchset_cr_active_cell" holds one cell being used for NBCH decode at any time.

DEPENDENCIES

RETURN VALUE     
    NONE              
SIDE EFFECTS
    
===========================================================================*/
extern void tdssrchset_add_to_nbch_cell(uint16 freq, uint8 cpi);

/*=========================================================================

FUNCTION     : TDSSRCHSET_IS_SERVING_CELL
               
DESCRIPTION  : check if it is serving cell according to frequency and cpid.
  
DEPENDENCIES : None

INPUT PARAMETERS :
    uint16 freq: the frequency information.
    uint8 cpi: the cell parameter id of the cell

RETURN VALUE : if the cell is serving cell, return TURE; Othersize, return FALSE.
      
SIDE EFFECTS : 

=========================================================================*/
extern boolean tdssrchset_is_serving_cell( uint16 freq, uint8 cpi );


/*=========================================================================

FUNCTION     : TDSSRCHSET_IS_NBCH_CELL
               
DESCRIPTION  : check if it is nbch cell according to frequency and cpid.
  
DEPENDENCIES : None

INPUT PARAMETERS :
    uint16 freq: the frequency information.
    uint8 cpi: the cell parameter id of the cell

RETURN VALUE : if the cell belongs to NBCH cell, return TURE; Othersize, return FALSE.
      
SIDE EFFECTS : 

=========================================================================*/
extern boolean tdssrchset_is_nbch_cell( uint16 freq, uint8 cpi );

/*=========================================================================

 FUNCTION     :TDSSRCHSET_GET_CURRENT_ACTIVE_CELL_POINTER
 
 DESCRIPTION  : This function return current active_cell_info_struct pointer.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
 
 RETURN VALUE : . 
       tdssrchset_active_cell_info_struct_type * :
       
 SIDE EFFECTS : 
==========================================================================*/
extern tdssrchset_active_cell_info_struct_type * tdssrchset_get_current_active_cell_pointer(void);

#ifdef FEATURE_JDS_OPT
/*=========================================================================

 FUNCTION     :TDSSRCHSET_RESET_NON_TS0_TABLE
 
 DESCRIPTION  : This function will initialise all related fields of appointed non-ts0 table.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdssrchset_active_cell_info_struct_type *active_cell_ptr:
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_reset_non_ts0_table(tdssrchset_active_cell_info_struct_type *active_cell_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_ESTABLISH_NON_TS0_TABLE
 
 DESCRIPTION  : If handover or reselection happens, the function is used to establish non-ts0 table of the target cell.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdssrchset_active_cell_info_struct_type *active_cell_ptr:
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_establish_non_ts0_table(tdssrchset_active_cell_info_struct_type *active_cell_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_REMOVE_RSET_CELL
 
 DESCRIPTION  : A cell is removed from RSET, it must be moved into ASET/MSET/DSET. So now it belongs to NSET.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint8 freq_index, uint8 cpi:
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_remove_rset_cell(uint8 freq_index, uint8 cpi);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_ADD_RSET_CELL
 
 DESCRIPTION  : A cell is added to RSET, the function will set related fields in ts0 table and non-ts0 table.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint8 freq_index, uint8 cpi:
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_add_rset_cell(uint8 freq_index, uint8 cpi);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_UPDATE_NON_TS0_TABLE
 
 DESCRIPTION  : the function use TS-RSP measurement results to update non-ts0 table.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
  tdssrchset_active_cell_info_struct_type *active_cell_ptr,
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_update_non_ts0_table( tdssrchset_active_cell_info_struct_type *active_cell_ptr);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_UPDATE_TS0_TABLE
 
 DESCRIPTION  : the function use TS-RSP measurement results to update ts0 table.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
  uint8 freq_idx:
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_update_ts0_table(uint8 freq_idx);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_ASET_UPDATE_SCHEDULING
 
 DESCRIPTION  : This function schedules aset update.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_aset_update_scheduling(void);
#endif /*FEATURE_JDS_OPT */

#ifdef FEATURE_TDSCDMA_FW_NEW_LNA_INTF  
/*=========================================================================

 FUNCTION     :TDSSRCHSET_GET_LNA_INFO
 
 DESCRIPTION  : Given a frequency, this function returns the LNA info for 
                that frequency
              
 DEPENDENCIES : 

 INPUT PARAMETERS :
   uint16 freq : the TDS frequency of the target cell.
   tfw_rx_lna_t *lna_ptr: a pointer to the LNA info data structure

 RETURN VALUE : if found, return the LNA info in the provided reference, 
                and return TRUE, else return FALSE.
       
 SIDE EFFECTS : 
==========================================================================*/
extern boolean tdssrchset_get_lna_info(uint16 freq, tfw_rx_lna_t *lna_ptr);
#endif

/*=========================================================================

 FUNCTION     :TDSSRCHSET_SORT_FREQS
 
 DESCRIPTION  :The function is used to get all mset freqs in the RSCP descending order. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tdsl1_mset_list_type *tdsl1_mset_list
 
 RETURN VALUE : void
   
 SIDE EFFECTS : None
==========================================================================*/
extern void tdssrchset_sort_freqs(tdsl1_mset_list_type *tdsl1_mset_list);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_FIND_STRONG_CELLS_ACROSS_FREQS
 
 DESCRIPTION  : The function is used to get strong cells from appointed freq list. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint8 freq_num: the number of frequency.
   uint16 freq_list[]: frequency list.
   uint8 *cell_num: it is used to return the number of strong cells.
   tdssrchset_cell_struct_type_tmp * cpid_list : a pointer to cpi array in 
     which the function need put all candidate cells' pointer.
 
 RETURN VALUE : None
   
 SIDE EFFECTS : None
==========================================================================*/
extern void tdssrchset_find_strong_cells_across_freqs
( 
  uint8 freq_num, 
  uint16 freq_list[], 
  uint8 *cell_num,  
  tdssrchset_cell_struct_type_tmp *cpid_list[]
);

/*=========================================================================

 FUNCTION     :TDSSRCHSET_START_SET_MANAGEMENT_REQACQ
 
 DESCRIPTION  : After acquisition step 1 done, the function will be called to start set management, i.e. establish 
 active cell, JDCS and TS0 JDS in set management. The input parameters are detected cells' information in step 1.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   tfw_srch_acq_timing_para_t *acq_timing_para : detected cells' information from RRC during the L2T
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 

 
==========================================================================*/
extern void tdssrchset_start_set_management_reacq(tdsl1_acq_timing_type *acq_timing_para);
/*=========================================================================

 FUNCTION     :TDSSRCHSET_INIT_ASET_INTRA_FREQ_SET
 
 DESCRIPTION  : This function initializes the search database by putting all cells in the Empty Set and initializes 
                          cell parameters of the cell structures in searcher table, and initializes the active cell structure 
                          and all sets structure.
              
 DEPENDENCIES : 
   
 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void tdssrchset_init_aset_intra_freq_set( void );

#endif /* TDSSRCH_SET_H */
