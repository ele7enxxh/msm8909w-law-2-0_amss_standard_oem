#ifndef SRCH_SET_H
#define SRCH_SET_H
/*===========================================================================
   FILE: srchset.h

GENERAL DESCRIPTION:
      This file contains the typedefs, enums and defines used for the Search
      pilot set operations. 
      
      
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2000-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
      
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchset.h_v   1.36   25 Jun 2002 21:00:14   cemani  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchset.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/14   hk      Header file inclusion cleanup
06/27/14   da      Created a new api which returns bmsk for all active filters present and returns 
                   default filter if there are no active filters present
06/16/14   km      Pass carr_idx to cell_exists_in_new_aset API
03/26/14   gj      Updated comment for SRCHSET_DEMOTE_CELL_FROM_ASET.
07/01/14   amj     Clean up old ASET update APIs
12/16/13   as      Porting G2W TA from Triton to Dime.
10/29/13   sr/gsk  During cell reselection, delaying CPHY setup cnf for NBCH drop untill
                   serv cell is configured and nbr cell is deconfigured,
09/12/13   jd      Searcher redesign
08/08/13   zr      Add overflow arbitration when demoting a cell from the ASET
07/15/13   zr      Adding pending delete composite set for ASET update
06/04/13   kcm     Externed srchset_log_aset API
05/16/13   zr      Adding new fields to cell struct for new ASET update procedure
03/08/13   dm      MCVS feature enhancement
02/22/13   dm      R99 SCHIC changes with MVCS.
01/29/13   zr      Added new enum value to indicate TxD detection is needed but cannot be scheduled
01/21/13   stk     Support for Femto/CSG selection and reselection
08/03/12  stk/km   Added API to return # of SHO cells (for SVS feature)
12/07/12   ar      Merge - Modify comment for rx_agc
10/24/12   gsk     Change of variable name
10/15/12   jd      Memory Optimization Changes
09/19/12   sr      Do not trigger ASU validation check for cells already present
08/14/12   zr      Externing snapshot of PSC during HHO
08/14/12   zr      Adding function to drop the weakest cell from a specified set
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12   gsk     Idle mode FRs 1782/1705/1947/2152/1950
06/07/12   vs      MBMS feature cleanup.
03/16/12   rc      Added support for QICE in HS-FACH.
02/12/12   hk      Nikel feature cleanup
02/01/12   gnk     Nikel Feature cleanup.
01/27/12   vs      nikel feature cleanup.
01/26/12   zr      Mainlined FEATURE_WCDMA_HCS, FEATURE_WCDMA_MULTIPLE_L3_FILTERS_SUPPORTED,
                   and FEATURE_T_BARRED_SUPPORT
01/15/12   sv      Added API to determine if a cell exists in ordered ASET during 
                   an Active Set Update.
01/04/12   zr      Increased the size of saved set to accomodate USET also.
12/21/11   sv      Added data elements to search structure to capture peak position
                   filter and reference age for neighbor searches in DRX.
12/08/11   sv      Added data elements to capture primary and diversity energies.
12/02/11   stk     Added support for FEATURE_WCDMA_RXD_PCH and FEATURE_IDLEMODE_FILTER
11/12/11   sv      Externed the API to check if a cell exists in TxD set.
11/12/11   sv      Added boolean ref_pos_updated to search cell structure.
07/08/11   mk      Fixed compilation error.
07/05/11   vsr     Mainlining Genesis CME features
06/10/11   sv      Added API to delete the weakest DC sync neighbor.
05/13/11   sv      Added support for periodic TxD determination.
04/28/11    sj      upmerge from offline/main
04/04/11   sv      Added and modified some API prototypes.
01/13/11   sv      Added support for QICE ASET cell registration
10/22/10   ks      Added SRCH_DC_TRIAGE_SET_MAX
10/19/10   sv      Added changes to support full search for DC and QICE support.
07/15/10   uk      Store the value of the sfn at which tresel timer is started
                   in the timer struct to prevent it from getting reset every drx
06/30/10   sv      Changed API name.
06/02/10   stk     Added support for W2LTE measurements and reselection under 
                   FEATURE_WCDMA_ABSOLUTE_PRIORITY & FEATURE_WCDMA_TO_LTE
05/27/10   sv      Externed DC related API's.
05/14/10   nd      Made search_cell[] an array of cell pointers and related
                   changes under FEATURE_WCDMA_L1_ZI_OPT.
02/26/10   rvs     Fixes for Mustang.
02/10/10   ms      Added WL1_MAX_NUM_CARRIER_IDX
02/09/10   sv      Added definition for SRCH_CARRIER_MAX. Increased number of 
                   DC ASET cells to 2. Plus some code cleanup.
01/23/10   sv      Merging code from taxis tip to genesis  
01/21/10   ksr     compilation error (on behalf of Surjit)
11/06/09   mr      Handle MIMO S_CPICH power Imbalance from RRC
10/28/09   nd      Added few elements in the cell structure to support independent
                   filtering of RSCP.
09/15/09   sv      Externed the API that determines whether a given cell has 
                   timing info or not.
08/23/09   rksd    Changed names of enums that were clashing with 1X.
08/08/09   sq      Added FEATURE_WCDMA_DC_HSDPA
07/16/09   rgn     Enhanced existing s-cpich support   
04/17/09   rgn     Calculate candidate cell pn position to be used in cases where
                   cell to be added to aset belongs to aset_no_pn set
03/24/09   sv      Externed the srchset_reset_inter_to_intraf_cell function.
03/13/09   sv      Externed the srch_get_cell_intlock\intfree functions.
01/20/09   rgn     Added sec scr code idx to cell structure
01/13/09   sv      Externed the "srchset_get_deconfig_bmsk_for_timing_known_hho_cells" function. 
12/12/08   sv      Corrected the number of pullin set cells.
12/09/08   sv      Mainlined FEATURE_WCDMA_OFFLINE_SEARCHER, FEATURE_RUMI_BRINGUP 
                   and FEATURE_WCDMA_DL_ENHANCED.
10/03/08   hk      Bringing in 7k Mailine fixes
09/23/08   sv      Fixed the duplicate PHYCHAN_ASET_DONE_CMD sent to DL.
09/20/08   sv      Added return type to srchset_add_to_nbch_set API.
09/14/08   uk      Externed Declaration of srch_sleep_set, used to store the 
                   triage set when going to sleep
09/12/08   nd      changes for cell deconfig during state cleanup 
11/01/07   ub      MBMS: Added fn to check freq,psc of inter_as_intraf cell
10/26/07   sv      Added searcher support for FEATURE_MODEM_MBMS and FEATURE_WCDMA_DL_ENHANCED.
08/08/07   vp      Added support for reading cell id of gps bch in dch state.
06/28/07   og      Allow W to inform G about the frequency offset of the G cell
                   in W2G non-blind handover scenarios.
03/29/07   vip     Added Count_C_SFN_High value in the SFN_CFN  record strucure.
01/22/07   nd      Added event 1J support under FEATURE_WCDMA_HSUPA
01/08/07   sk      Definition of unique ARFCN in gsm cell structure.
12/07/06   bbd     Removed obsolete FEATURE_TPC_IN_SHO.
11/29/06   nd      Added multiple layer3 filter support for intra freq measurement
                   under FEATURE_WCDMA_MULTIPLE_L3_FILTERS_SUPPORTED
11/01/06   cpe     Mainlined the following features:
                    -FEATURE_OPTIMIZE_SEARCH_FOR_HHO
08/28/06   vp      Added support for new member nbr_rl_idx in srch_cell_struct
08/28/06   sk      Removed sch_gfn variable.
08/08/06   nd      modified the extern to srchset_init_uset_cell_params()
08/03/06   sk      Changes for forbidden tones and correction of delta frame offset.
07/04/06   nd      cell_from_uset flag in cell structure is moved out of detected
                   set feature flag. 
06/20/06   bd      Renamed global vars/functions to avoid conflict with 1x names
06/24/06   rc      Removed 'psc' field from srch_cell_struct_type.
04/05/06   sk      Added frequency offset to srchcr_gsm_meas_struct
03/20/06   sv      Changes to support Rx diversity searcher changes
                   under FEATURE_WCDMA_RX_DIVERSITY.
09/27/05   m       Add filt_ecio_hi_res to srch_cell_struct_type for use in
                   logarithmic filtering
09/19/05   cpe     Mainlined the feature FEATURE_WCDMA_RX_TX.
                   Added a field tot_ecio to cell structure
09/13/05   nd      added support for FEATURE_WCDMA_L1_DETECTED_CELLS_DCH
08/30/05   cpe     Added a field reschedule_needed to sfn_cfn_diff structure
                   part of the cell structure. Changed ts_1 to uint32
08/02/05   m       Added prototypes and external declaration to support
                   HHO in to SHO with no cell timing known
07/28/05   cpe     Added ecio_with_offset and rscp_with_offset to cell structure
06/09/06   sk      Added cell bar flag to cell struct and gsm cell struct
06/08/05   cpe     Added support for calculating and reporting SFN-SFN difference 
                   in FACH.
05/24/05   ub      Added penalty time backup storage status type
05/03/05   m       Fixed bugs reported in integration of srch optimization
                   feature and resume on HHO failure
05/01/05   ub      Cell reselection improvements and fixes
04/11/05   cpe     Code addition for resuming to old config upon HHO failure. 
04/13/05   m       Added support for srch optimization for HHO
04/06/05   ub/ba   HCS based cell reselection feature (FEATURE_WCDMA_HCS)
03/07/05   cpe     Added several fields to support SIB18 under FEATURE_SIB18
01/14/05   cpe     Added fields to srch_cell_struct_type to support S-CPICH
01/04/05   rc      Added booleans rssi_used_periodic and rssi_used[] array to 
                   check if rssi value is used for reporting and then start
                   cell aging.
08/30/04   rc      Added rssi_age to srch_gsm_cell_struct_type.
08/17/04   cpe     Added elements to cell structure towards implementation
                   of RxTx feature.
07/07/04   sh      Added filt_len to srchcr_gsm_meas_struct_type.
06/25/04   scm     Export srchset_find_strongest_aset_cell().
04/21/04   cpe     Exported the function srchset_reset_all_events() 
12/23/03   cpe     Added an extern 
12/17/03   yiz     Moved srchcr_gsm_meas_struct_type from srchgsmdrv.h
10/31/03   cpe     Removed FEATURE_WCDMA_MULTIPLE_INTRA_MEAS_SUPPORTED feature 
                   which is always present
10/24/03   cpe     Removed all the inter freq info to srchinterf.h file
10/13/03   scm     Defines and externs to support RESUME set.
08/05/02   cpe     Further correction to the tau-DPCH change fix (comments by
                   src).
06/20/03   cpe     Reduced the Unlisted set size to 10 (from 32)
05/23/03   cpe     Added support for multiple intra freq measurements
04/21/03   cpe     Added a new field redetect_age to the srch_cell_struct_type
                   data structure.
12/14/02   cpe/src Merged-in code changes corresponding to the TPC patch,
                   enclosing the patched code within FEATURE_TPC_IN_SHO.
12/10/02   scm     Feature def around srchset_ref_cell_changed_event().
12/04/02   djm     filter coef based rssi sort for inter-rat cells
11/25/02   ddh     Adding reselection timestamp to srch_gsm_cell_struct_type
11/25/02   ddh/cpe win_size for cell_struct changed from uint16 to uint32
11/18/02   ddh     Changed qoffset_1 and qoffset_2 from uint16 to int16
                   since the range is from -50..50 
10/31/02   djm     Added cell_reported[] array to enable limiting periodic
                   and event based reporting to only those cells for which
                   new measurements are available.
10/01/02   ddh     Added function to clear sets, added tmp nsets for use
                   during cell reselection
10/01/02   ddh     Adding info for Inter-frequency sets.
08/11/02   djm     SRCHSET_SORT_CELL_IN_GSMSET function addition.  used to
                   sort gsm nset.
08/02/02   djm     added include of customer.h, modified meas ctrl elements
                   of gsm cell struct, added extern gsm nset cnt and array
                   added srchset gsmset init cell func.
07/02/02   ddh     Adding info for GSM sets.
06/25/02   cpe     Changed max_eng size from uint16 to uint32 in srch_cell_
                    struct_type
06/21/02   yus     Added path_thresh in srch_cell_struct_type.
06/18/02   yus     Added filt_mult_factor in srch_cell_struct_type.
06/12/02   cpe     Added prototype for the fn srchset_ref_cell_changed_event
06/04/02   sh      Added rank_n to srch_cell_struct_type.
05/19/02   src     Changed references to changed_dl_seq_dpch_offset to its new
                   name - dl_rl_dpch_offset.
05/18/02   sh      Cell Reselection related changes.
                   Added srch_aset_struct_type.
                   Changed srchset_verify_aset(),srchset_aset_update() protos.
04/26/02   cpe     Added srch_evt_1c_src_psc variable to srch_cell_struct_type
                   Changed SRCH_USET_MAX to 32 and changed SRCH_OFREQ_CELL_MAX
                   to 16
03/1//02   cpe     Created a new struct srch_periodic_ts_struct_type and made
                   it part of srch_evt_ts_union_type. Also removed the field
                   generic_evt_rpt_status field from srch_evt_rpt_union_type.
02/11/02   src/gs  Merged-in fixed for D_OFF and Tm is softer handover.
01/17/02   cpe     Removed cell_id from the cell structure
                   Added the function prototype for  srchset_find_cell()
01/31/02   yus     Added start_pos in srch_cell_struct_type
11/16/01   cpe     Added cell_id to the cell structure
09/10/01   cpe     Added a new field undetected_cnt to srch_cell_struct_type
                   structure.
07/18/01   cpe     Added two more fields to srch_evt_ts_struct_type for handling
                   multiple and periodic measurement reporting.
07/04/01   wjn     Exported cell table structure "srch_cells".
                   Added "generic_evt_rpt_status" into "srch_evt_rpt_union_type"
                   to allow more efficient handling on the data structure and 
                   beefed up comment. 
06/08/01   cpe     Added cell_offset parameter to the srch_cell_structure
05/20/01   wjn     Added CPICH RSCP related fields into srch_cell_structure.
04/27/01   wjn     Added "ref_pos" (reference position of a cell for centering
                   search window) and "ref_age" (the age of "ref_pos") in 
                   "srch_cell_struct_type".
03/30/01   wjn     Modified code for new set maintenance design -- introduced
                   ADD_CSET and DROP_CSET concept and deleted original CSET 
                   and Pre-CSET.
03/23/01   wjn     Modified srch_cell_struct_type so that it is scalable to
                   inter-frequency cells.
                   Added Inter-frequency Cell list structure declaration.
                   Resized the cell table to hold both intra- and inter-
                   frequency cells.
03/17/01   wjn     Defined Pre-Candidate Set for cells which entered report
                   range but do not meet TIme-to-Trigger criteria. 
                   Expanded cell structure to included cell-specific 
                   intra-frequency event evaluation and report information 
                   in DCH state.
02/14/01   wjn     Added code to support unified aset update mechanism cross
                   all states.
01/23/01   wjn     Added stuff for cell reselection evaluation.
01/18/01   wjn     Added Energy Filtering stuff in cell structure.
12/14/2000 mdl     added rl_set_idx to srch_cell_struct_type
                   changed some comments
                   
11/26/00   wjn     Added function "srchset_get_cell_from_set(...)".
11/09/00   wjn     Updated cell struture.
08/22/00   wjn     Created file.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "srchparm.h"
#include "l1rrcif.h"
#include "l1const.h"
#include "tlm.h"
#include "srchsetutil.h"

/* used to sort maintain rssi based sorted list of GSM NSET */
extern srch_gsm_cell_struct_type * srch_gsm_rssi_nset_sort_head;

/* Snapshot of reference PSC during HHO */
extern uint16 ue_saved_ref_cell_psc_in_hho;

extern uint8 hcs_intra_sync_nset_cnt ;
extern uint8 hcs_intra_async_nset_cnt;
/* HCS intra sync and async set */
extern uint8 hcs_inter_sync_nset_cnt[2];
extern uint8 hcs_inter_async_nset_cnt[2];
/* HCS inter sync and async set */
extern uint8 hcs_interrat_nset_cnt;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*=========================================================================

FUNCTION     : SRCHSET_INIT_SETS 

DESCRIPTION  : This function initializes the search database by putting
               all cells in the Enpty Set. This function also initializes
               the interRAT and inter-freq sets.
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchset_init_sets( void );

/*=========================================================================

FUNCTION     : SRCHSET_GET_EMPTY_CELL 

DESCRIPTION  : This function returns an empty slot in the sector table.
               Typically when a new sector is to be added to the set table,
               an empty slot is obtained in the sector table and is filled
               up with the appropriate sector information. This sector 
               index is then appended to the existing set tables by calling 
               srchset_add_cell() function.
             
DEPENDENCIES : None

INPUT        : None

RETURN VALUE : index   = Index to an empty slot in the sector table
               FAILURE = If all the sectors in the sector table are filled
      
SIDE EFFECTS : None

=========================================================================*/
extern srch_cell_struct_type* srchset_get_empty_cell( void );

/*=========================================================================

FUNCTION     : SRCHSET_PUT_CELL_TO_SET 

DESCRIPTION  : This function adds a cell to the specified SET. If the 
               specified set is SET_E (Empty Set), the funtion returns
               an empty slot back to the memory pool.
             
DEPENDENCIES : None

INPUT        : None

RETURN VALUE : boolean  TRUE if successfully added, otherwise FALSE
      
SIDE EFFECTS : None

=========================================================================*/
extern boolean srchset_put_cell_to_set
( 
  srch_cell_struct_type *cell_ptr,
  srch_set_enum_type    set
);

/*=========================================================================

FUNCTION     : SRCHSET_DELETE_CELL_FROM_SET 

DESCRIPTION  : This function deletes a cell from the specified SET. 
               Immediately following this function call, there should be 
               a function call "srchset_put_cell_to_set(). Otherwise, we
               will have dangling cell around.
               
               Note: this function may not be used to delete a cell from 
               the Empty SET. In stead, srchset_get_empty_cell is used to
               get an empty cell.
             
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchset_delete_cell_from_set
( 
  srch_cell_struct_type*  cell_ptr,
  srch_set_enum_type    set
);

/*=========================================================================

FUNCTION     : SRCHSET_GET_CELL_FROM_SET

DESCRIPTION  : This function searches the specified SET for cell 
               (freq, scr_code and set). If the cell is found, the function 
               returns the pointer to the cell table entry for the matching 
               cell. Otherwise, the function returns NULL pointer.
             
DEPENDENCIES : None

INPUT        : None

RETURN VALUE : Pointer to the cell table entry.
      
SIDE EFFECTS : None

=========================================================================*/
extern srch_cell_struct_type* srchset_get_cell_from_set
(
  uint16 freq,                   /* wcdma frequency */
  uint16 scr_code,               /* cell scrambling code */
  srch_set_enum_type  set        /* SET info */
);

/*=========================================================================

FUNCTION     : SRCHSET_FIND_CELL

DESCRIPTION  : This function searches all the sets for cell 
               (freq, scr_code). If the cell is found, the function returns
               the pointer to the cell table entry for the matching cell.
               Otherwise, the function returns NULL pointer.
             
DEPENDENCIES : None

INPUT        : None

RETURN VALUE : Pointer to the cell table entry.
      
SIDE EFFECTS : None

=========================================================================*/
extern srch_cell_struct_type* srchset_find_cell
(
  uint16 freq,       /* find cell with this cdma frequency */
  uint16 scr_code    /* and  this scrambling code */            
) ;

#if !defined(FEATURE_DIAG_NO_EVENTS)
/*=========================================================================

FUNCTION     : SRCHSET_REF_CELL_CHANGED_EVENT

DESCRIPTION  : This function logs the ref cell changed event.
             
DEPENDENCIES : None

RETURN VALUE : None.
      
SIDE EFFECTS :  

=========================================================================*/
extern void srchset_ref_cell_changed_event(void);
#endif

/*===========================================================================

FUNCTION SRCHSET_CLEAR_SET

DESCRIPTION
  This function is used to clear all the entries out of the specified set,
  and to set the associated set count to 0.

DEPENDENCIES
  This function should only be called AFTER the cells in a set have been 
  moved somewhere else.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void srchset_clear_set(srch_set_enum_type    set);

/*=========================================================================

FUNCTION     : SRCHSET_RESET_ALL_EVENTS

DESCRIPTION  : This function resets the MR database.
             
DEPENDENCIES : None

RETURN VALUE : void
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchset_reset_all_events(uint8);

/*=========================================================================

FUNCTION     : SRCHSET_PROCESS_ASET_UPDATE

DESCRIPTION  : This function performs set maintenance for ASET update.
             
DEPENDENCIES : None

RETURN VALUE : TRUE, if any cell in the new ASET is not already in ASET or CSET.
               FALSE, Otherwise.
      
SIDE EFFECTS : The function may change flag srch_dmod_set_changed.

=========================================================================*/
void srchset_process_aset_update(void);

/*===========================================================================

FUNCTION SRCH_FIND_HHO_CELL_TIMING

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void srch_find_cell_timing
(
  srch_aset_struct_type *new_aset
);

extern void srchset_move_cells_to_saved_set_for_hho( void);

/*=========================================================================

FUNCTION     : SRCHSET_SORT_CELL_IN_GSMSET

DESCRIPTION  : This function creates a linked list of sorted cells based
               on filtered rssi specified by fc_index.  Highest to lowest.
               Currently this function is only valid for GSM_SET_N.
             
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
void srchset_sort_cell_in_gsmset
(
  srch_gsm_set_enum_type  set,
  uint32  fc_index
);

/*=========================================================================

FUNCTION     : SRCHSET_INTER_RAT_INIT_CELL 
               
DESCRIPTION  : This function initializes the Inter-Rat cell structure.
               The cell is placed in the empty set and additional parameters
               are set appropriately.
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
void srchset_inter_rat_init_empty_cell( uint16 cell_index, srch_gsm_set_enum_type  set );

/*===========================================================================

FUNCTION     SRCHSET_INIT_USET_CELL_PARAMS

DESCRIPTION
  

DEPENDENCIES
  None.

PARAMETERS
 parameter cell_ptr is the pointer to the cell which is being added/deleted by the ASU.
 
RETURN VALUE
  none
SIDE EFFECTS
   scr_code, freq are not set in this function
===========================================================================*/
extern void srchset_init_uset_cell_params(srch_cell_struct_type  *cell_ptr,boolean new_detected);

/*===========================================================================
FUNCTION         SRCHSET_CHECK_INTERF_CELL_AS_INTRAF

DESCRIPTION      
    Check if the freq,psc stored in "srch_inter_to_intraf_cell" struct
    type same as that of the passed in inter-f cell. 

DEPENDENCIES

RETURN VALUE     
    Boolean - TRUE if freq,psc match, FALSE otherwise
    
SIDE EFFECTS
    None
===========================================================================*/
extern boolean srchset_check_interf_cell_as_intraf(uint16 freq, uint16 scr_code);

/*===========================================================================
FUNCTION         SRCHSET_RESET_INTERF_TO_INTRAF_CELL

DESCRIPTION      
    This function resets elements in the "srch_inter_to_intraf_cell" struct.

DEPENDENCIES

RETURN VALUE     
    NONE              
SIDE EFFECTS
    None
===========================================================================*/
extern void srchset_reset_inter_to_intraf_cell( void );

/*===========================================================================

FUNCTION     srch_get_sfn_wrc_diff

DESCRIPTION
  

DEPENDENCIES
  None.

PARAMETERS
 None 
 
RETURN VALUE
  none
SIDE EFFECTS
 None 
===========================================================================*/
extern void srch_get_sfn_wrc_diff (uint16 psc, uint16 freq, sfn_wrc_diff_type *out);

/*===========================================================================

FUNCTION     srch_put_sfn_wrc_diff

DESCRIPTION
  

DEPENDENCIES
  None.

PARAMETERS
 None 
RETURN VALUE
  none
SIDE EFFECTS
   None
===========================================================================*/
extern boolean srch_put_sfn_wrc_diff(uint16 psc, uint16 freq, sfn_wrc_diff_type *in);

extern void srchset_process_nset_update(srch_nset_struct_type *new_nset);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION         srchset_readd_to_sbch_set

DESCRIPTION      

DEPENDENCIES

RETURN VALUE     
    NONE              
SIDE EFFECTS
    
===========================================================================*/

extern boolean srchset_readd_to_sbch_set(srch_cell_struct_type *cell_ptr);
#endif

extern void srchset_remove_from_nbch_set(uint16 freq, uint16 psc);
extern void srchset_remove_from_qice_nset(uint16 freq, uint16 psc);

extern boolean srchset_add_to_nbch_set(srch_cell_struct_type *cell_ptr);
extern boolean srchset_can_cell_be_deleted(srch_cell_struct_type *cell_ptr);


extern void srch_handle_cleanup_after_cell_trans(boolean cell_trans_in_prog);
extern boolean srch_handle_cleanup_after_cell_trans_fail(void);

/*=========================================================================
 
 FUNCTION     : srchset_get_bmsk_of_cells_deconfigd_during_aset_upd
 
 DESCRIPTION  : This function give the deconfig cell bitmask during a aset update procedure
 to DL module. THis will be called even before DL post the SRCH_ASET_UPD local cmd to 
 searcher 
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
       
 SIDE EFFECTS : 
 
 =========================================================================*/
extern uint8 srchset_get_bmsk_of_cells_deconfigd_during_aset_upd
( 
  srch_aset_struct_type *new_aset
);

/*=========================================================================

 FUNCTION     : srchset_get_deconfig_bmsk_for_timing_known_hho_cells
 
 DESCRIPTION  : This function gives the deconfig cell bitmask during a timing 
                known HHO scenario where L1 does NOT go through DCH->SUSPENDED->IDLE-ACQ->DCH. 
                In such cases, all previously timing known cells are moved to 
                SAVED_SET, but remain in TRIAGE_SET. Here, we can get the cell
                bitmask for all cells that need to be deconfigured. 
              
 DEPENDENCIES : 
 
 RETURN VALUE : Bitmask of all cells to deconfig. 
       
 SIDE EFFECTS : 
==========================================================================*/
uint16 srchset_get_deconfig_bmsk_for_timing_known_hho_cells(void);

/*=========================================================================

 FUNCTION     : srchset_get_deconfig_bmsk_for_cells_in_curr_decfg
 
 DESCRIPTION  : This function gives the deconfig cell bitmask during a SHO.
                Here, we get the cell bitmask for all cells that are in the 
                ordered list in ASU and are currently in deconfig. 
              
 DEPENDENCIES : 
 
 RETURN VALUE : Bitmask of all cells to deconfig. 
       
 SIDE EFFECTS : 
==========================================================================*/
uint16 srchset_get_deconfig_bmsk_for_cells_in_curr_decfg(void);

#ifdef FEATURE_WCDMA_DC_HSDPA
/*=========================================================================

FUNCTION     : srchset_dc_save_sets_for_hho

DESCRIPTION  : This function saves SET_DC_A and SET_DC_SYNC_ALL to SET_DC_SAVED. 
The ASET cell is saved as the 1st element in SET_DC_SAVED.
             
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchset_dc_save_sets_for_hho( void);

/*=========================================================================

FUNCTION     : srchset_dc_restore_sets

DESCRIPTION  : This function restores SET_DC_A and SET_DC_SYNC_ALL from 
SET_DC_SAVED. The ASET cell is saved as the 1st element in SET_DC_SAVED.
             
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchset_dc_restore_sets( void);

#endif /* FEATURE_WCDMA_DC_HSDPA */

#ifdef FEATURE_WCDMA_L1_ZI_OPT

/*=========================================================================

 FUNCTION     :SRCHSET_INIT_PARAMS_CELLS_ALLOCATED
 
 DESCRIPTION  : This function will initialise cell parameters of the cell structures. 
 These will be done when cells are allocated on WCDMA start and also during every
 state transition through IDLE.
              
 DEPENDENCIES : 
 
 RETURN VALUE : . 
       
 SIDE EFFECTS : 
==========================================================================*/
void srchset_init_params_cells_allocated(srch_cell_struct_type *cell_ptr);
#endif /* FEATURE_WCDMA_L1_ZI_OPT */
       
boolean srchset_cell_has_service(uint16 freq, uint16 scr_code);
boolean srchset_is_cell_on_secondary_carrier(srch_cell_struct_type *cell_ptr);

/*===========================================================================
FUNCTION srchset_find_cell_for_cell_db_idx

DESCRIPTION      
  This function returnscell ptr for the given cell db idx

DEPENDENCIES
  None

RETURN VALUE     
  PSC
                   
SIDE EFFECTS
  NONE
===========================================================================*/
srch_cell_struct_type* srchset_find_cell_for_cell_db_idx(uint8 cell_db_idx);

boolean srchset_is_cell_timing_known(srch_cell_struct_type *cell_ptr);
boolean srchset_remove_from_new_aset(uint8 cell_idx);
boolean srchset_cell_exists_in_new_aset(uint16 freq, uint16 scr_code, uint8 carr_idx);
boolean srchset_cell_exists_in_pn_set(uint16 freq, uint16 scr_code);
boolean srchset_remove_from_pn_set(srch_cell_struct_type* cell_ptr);
srch_cell_struct_type* srchset_cell_exists_in_triage_set(uint8 carr_idx, uint16 freq, uint16 scr_code);

/*=========================================================================

 FUNCTION     :srchset_get_txd_status
 
 DESCRIPTION  : This function returns TRUE if TxD is active on any cell.
              
 DEPENDENCIES : 
 
 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
boolean srchset_get_txd_status(void);
 
/*===========================================================================
FUNCTION         SRCHSET_IS_CELL_UNIQUE
       
DESCRIPTION      
    This function accepts -
    1) a cell pointer, 
    2) the destination address to an array of cell pointers and 
    3) the number of cell pointers in the destination array. 

    The function loops through the destination array and checks if the source cell 
    pointer is present in it .
 
DEPENDENCIES
              
RETURN VALUE     
    TRUE: if given cell pointer does not exist in destination array.
    FALSE: otherwise.
 
SIDE EFFECTS
       
===========================================================================*/
boolean srchset_is_cell_unique
(
  srch_cell_struct_type *cell_ptr,
  srch_cell_struct_type **start_address,
  uint8 cell_cnt
);

/*=========================================================================

FUNCTION     : SRCHSET_SELECT_REF_CELL

DESCRIPTION  : This function check if the reference cell change is
               necessary. If neeeded, it sets the reference cell using 
               searcher filtered energy as the criteria of selection.
             
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchset_select_ref_cell(void);

/*=========================================================================
 FUNCTION     : SRCHSET_CELL_PRESENT_IN_NEW_ASET
 
 DESCRIPTION  : Determines if a cell is present in the new aset

 DEPENDENCIES : None
 
 RETURN VALUE : TRUE if the given cell is present in the new aset, FALSE 
                otherwise

 SIDE EFFECTS : None
=========================================================================*/
extern boolean srchset_cell_present_in_new_aset(srch_cell_struct_type* cell_ptr,
                                                srch_aset_struct_type* new_aset_ptr);

/*=========================================================================

FUNCTION     : SRCHSET_LOG_ASET

DESCRIPTION  : This function send the event EVENT_WCDMA_ASET to diag. its called once the aset update 
                         procedure is completed
             
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srchset_log_aset(void);

/*=============================================================================
 FUNCTION     : SRCHSET_PUT_CELL_TO_PEND_DELETE_SET
 
 DESCRIPTION  : This function attempts to add a cell to the pending delete
                set, which is used during DC to SC transitions when DC cells
                must be deconfigured before they can be safely moved to the
                empty set.

 DEPENDENCIES : None
 
 RETURN VALUE : TRUE if the given cell was successfully added to the
                pending delete set or if the cell was already present,
                FALSE otherwise.

 SIDE EFFECTS : None
=============================================================================*/
extern boolean srchset_put_cell_to_pend_delete_set(srch_cell_struct_type* cell_ptr);

/*=============================================================================
 FUNCTION     : SRCHSET_FLUSH_PEND_DELETE_SET
 
 DESCRIPTION  : This function moves all cells in the pending delete set to the
                empty set.

 DEPENDENCIES : None
 
 RETURN VALUE : None

 SIDE EFFECTS : Up to 3 cells will be moved to the empty set.
=============================================================================*/
extern void srchset_flush_pend_delete_set(void);

/*=============================================================================
 FUNCTION     : SRCHSET_CELL_EXISTS_IN_PEND_DELETE_SET
 
 DESCRIPTION  : This function checks to see if a cell is present in the
                pending delete set. It checks by cell pointer, not frequency
                and scr_code.

 DEPENDENCIES : None
 
 RETURN VALUE : TRUE if the cell is present, FALSE otherwise.

 SIDE EFFECTS : None
=============================================================================*/
extern boolean srchset_cell_exists_in_pend_delete_set(srch_cell_struct_type* cell_ptr);

/*=============================================================================
 FUNCTION     : SRCHSET_DEMOTE_CELL_FROM_ASET
 
 DESCRIPTION  : This function attempts to drop the specified ASET cell to the
                target set(SET_U and SET_SYNC_N ONLY) during an ASET update, but allows for overflow 
                handling if the initial transition fails.

 DEPENDENCIES : Assumes the given cell has already been removed from its
                original set
 
 RETURN VALUE : None

 SIDE EFFECTS : Crashes if the cell cannot be demoted or invalid arguments
                are given
=============================================================================*/
extern void srchset_demote_cell_from_aset(srch_cell_struct_type* cell_ptr, 
                                          srch_set_enum_type target_set);

/*===========================================================================

FUNCTION     srchcr_decfg_cell_for_interf_resel_cb

DESCRIPTION
  This is a CB function which triggers a serving cell configure/deconfigure soon 
  after configuring/deconfiguring of the neighbor cell

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
extern void srchcr_decfg_cell_for_interf_resel_cb(void);

/*===========================================================================

FUNCTION     SRCH_GET_ACTIVE_FILT_BMSK

DESCRIPTION
  This function will set the bmsk for all active filters and if there is no
  active filter present (in IDLE mode scenarios) then the default filter`s
  bmsk will be returned

DEPENDENCIES
  None.

RETURN VALUE
  unit8 bmsk

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 srch_get_active_filt_bmsk(void);

/*=========================================================================

FUNCTION     : SRCHSET_IS_CELL_INVALIDATED

DESCRIPTION  : This function checks if the cell issued in PN search is removed when search is in progress.

DEPENDENCIES : None

RETURN VALUE : boolean, TRUE if removed

SIDE EFFECTS : None

=========================================================================*/
extern boolean srchset_is_cell_invalidated(srch_cell_struct_type* cell_ptr , uint16 index);

#endif /* SRCH_SET_H */
