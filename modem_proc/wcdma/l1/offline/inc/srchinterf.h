#ifndef SRCHINTERF_H
#define SRCHINTERF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            S R C H I N T E R F . H                  

GENERAL DESCRIPTION
  This module handles processing of measurement requests information from RRC
  for inter-frequency neighbors.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003-2012 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/srchinterf.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
01/13/14   jk      DC-HSUPA Interf Spec CR Changes.
09/12/13   jd      Searcher redesign
08/05/13   amj     Mainlined FEATURE_MODEM_STATISTICS.
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
01/21/13   stk     Support for Femto/CSG selection and reselection
01/03/13   zr      Moving inter-frequency cell database to heap memory
10/24/12   gsk     Change of variable name
10/15/12    jd      Memory Optimization Changes
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/05/12   gsk     Idle mode FRs 1782/1705/1947/2152/1950
05/15/12   gsk     Support for feature Inter-freq List search frequency with two 
                   additional frequencies in idle 
06/07/12   vs      MBMS feature cleanup.
03/27/12   hk      Added support for FEATURE_WCDMA_HS_FACH_DRX
03/07/12   zr      Externed interf vaset structure under FEATURE_WCDMA_CM_LTE_SEARCH
01/26/12   zr      Mainlined FEATURE_T_BARRED_SUPPORT, FEATURE_FILTER_INTERF_MEASUREMENTS,
                   FEATURE_WCDMA_VASET_SUPPORT, FEATURE_WCDMA_OFFLINE_SEARCHER,
                   and FEATURE_WCDMA_HCS
11/06/11   sv      Added extern definitions for 2D delay data structure and processing functions.
09/01/11   stk     Added code to check for duplicate inter-f cells before adding them in ofreq_sync and
                   sorted_inter_f structures. This will ensure UE doesnt send duplicate cells to FW for inter-f list srch.
04/28/11    sj      upmerge from offline/main
04/19/11   sv      Added support for FEATURE_WCDMA_GET_RAT_MEAS_INFO.
11/24/10   sa      Changed prototype of interf filter function to use it for 
                   e-utra inter-rat filtering.
06/02/10   stk     Added support for W2LTE measurements and reselection under 
                   FEATURE_WCDMA_ABSOLUTE_PRIORITY & FEATURE_WCDMA_TO_LTE
05/27/10   sa      Modified the scope of variables to support feature
                   FEATURE_MODEM_STATISTICS.
06/26/09   sa      Change the SRCH_MAX_INTERF_MEAS from 7 to 8.
04/22/09   stk     Compiler warning fixes	
10/03/08   hk      Bringing in 7k Mailine fixes
08/23/06   rc      Added support for cells for measurement and virtual active
                   set under FEATURE_WCDMA_VASET_SUPPORT.
08/22/06   rc      Added declaration for srch_interf_update_cell_list_in_dch_init().
11/11/05   sk      Introduced interf saved set to hold sync information 
                   across states
08/26/05   nd      added a flag "stopped" in the meas db to indicate stopped 
                   measurements after inter freq HHO
07/22/05   vp      Changed the FACH state k-value for inter-freq filter
                   from 2 to 1. This should converge the filtered energy
                   faster to the last measured energy
06/20/06   sk      Added cell bar flag to interf cell structure
06/14/05   rc      Added support for multiple 2A events.
06/10/05   vp      Code modifications to filter inter-freq measurements
04/06/05   ub/ba   HCS based cell reselection feature (FEATURE_WCDMA_HCS)
01/10/05   src     Correction of a potential feature skew.
12/03/04   cpe     Added a couple of fields to srch_interf_cell_struct_type 
                   for supporting pathloss feature
11/01/04   vb      Extern'ed the function srch_interf_find_cell
07/28/04   rc      Added declaration for srch_interf_fach_add_cells() function
04/28/04   sh      Added field rl_set_idx to srch_interf_cell_struct_type.
04/22/04   cpe     Modified srch_inter_freq_meas_parms_struct_type to account
                   for multiple non used frequencies 
04/06/04   cpe     Exported a couple of functions for usage in other files
12/10/03   cpe     Added a prototype for srch_interf_delete_cell_from_set
12/05/03   cpe     Added a prototype for srch_inter_f_meas_sib_update
12/01/03   cpe     Added a prototype for srch_interf_remove_all_interf_cells()
11/13/02   src     Removed extra hash-endif to fix compilation problem.
11/12/03   sh      Added some fields for inter-f cell structure for cell
                   reselection.
11/12/03   cpe     Extern'ed a few functions
11/05/03   cpe/src Changed a hash-ifdef to consider both inter-frequency cell
                   re-selection as well as inter-frequency compressed-mode
                   measurements.
10/24/03   cpe     Expanded the support for inter frequency measurements,
                   set maintenance structures, function prototypes etc.
01/21/03   ddh     Created file.

===========================================================================*/
#include "wcdma_variation.h"
#include "srchcmmeas.h"
#include "wsrch.h"             

/*=========================================================================

    V A R I A B L E     D E C L A R A T I O N S                                                                             

 ===========================================================================*/
extern srch_inter_freq_meas_parms_struct_type 
                                srch_inter_freq_meas_db[SRCH_MAX_INTERF_MEAS];

extern srchinterf_2d_info_struct_type evt_2d[SRCH_MAX_INTERF_MEAS];

extern uint16 srch_interf_num_non_used_freq;

extern uint16 srch_interf_non_used_freq_array[L1_MAX_NON_USED_FREQ];

extern boolean srch_interf_periodic_rpts_requested;
extern boolean srch_interf_evt_trig_rpts_requested;
extern uint16 srch_num_interf_meas_in_progress;

extern boolean srch_inter_freq_meas_performed;

extern srch_ofreq_cell_list_struct_type 
         srch_ofreq_sync_nset[SRCH_OFREQ_FREQ_MAX];
  /* indexes and associated info for cells in the inter-freq sync NSET */
extern srch_ofreq_cell_list_struct_type 
         srch_ofreq_async_nset[SRCH_OFREQ_FREQ_MAX];
  /* indexes and associated info for cells in the inter-freq async NSET */
extern srch_ofreq_uset_struct_type 
         srch_ofreq_uset[SRCH_OFREQ_FREQ_MAX];
  /* indexes and associated info for cells in the inter-freq USET */

extern srch_ofreq_cell_list_struct_type 
         srch_ofreq_saved_set[SRCH_OFREQ_FREQ_MAX];
  /* indexes and associated info for cells in the inter-freq SAVED SET */

extern uint16 srch_interf_sync_nset_cnt, srch_interf_async_nset_cnt,srch_interf_saved_set_cnt;

/* Inter freq sets and set count variables */
extern uint16 srch_interf_vaset_cnt;
/* Inter Freq Sync Nbr cells*/
extern srch_interf_cell_struct_type *srch_interf_vaset_ptr[SRCH_OFREQ_CELL_MAX];

/* monitored and neighbor set count variables */
extern uint16 srch_interf_monitored_set_cnt;
/* Inter Freq Async Nbr cells*/
extern srch_interf_cell_struct_type *srch_interf_monitored_set_ptr[SRCH_OFREQ_CELL_MAX];

/* Inter Freq Sync Nbr cells*/
extern srch_interf_cell_struct_type *srch_interf_sync_nset_ptr[SRCH_OFREQ_CELL_MAX];
/* Inter Freq Async Nbr cells*/
extern srch_interf_cell_struct_type *srch_interf_async_nset_ptr[SRCH_OFREQ_CELL_MAX];
/* Inter Freq Saved set cells*/
extern srch_interf_cell_struct_type *srch_interf_saved_set_ptr[SRCH_OFREQ_CELL_MAX];

extern boolean srch_interf_reset_interf_meas_during_hho;

extern uint16 srch_interf_num_non_used_freq;
extern uint16 srch_interf_non_used_freq_array[L1_MAX_NON_USED_FREQ];

#ifdef FEATURE_WCDMA_DC_HSUPA
extern boolean srch_interf_sec_cell_info_list_configured;
#endif

/*=========================================================================

    F U N C T I O N     D E C L A R A T I O N S                                                                             
  
 ===========================================================================*/

/*======================================================================
FUNCTION SRCH_INTERF_PROCESS_MEAS_CTRL_MSG

DESCRIPTION
    Currently we use this in DCH state to obtain a new inter_f 
    measurement infor.
     

RETURN VALUE
    boolean

=======================================================================*/
extern boolean srch_interf_process_meas_ctrl_msg (l1_meas_cmd_type *cmd_ptr);

/*======================================================================
FUNCTION SRCH_INTERF_RESET_MEAS_DBASE

DESCRIPTION
    Resets the existing interf freq measurement dbase
    In particular the following data structures are impacted
        
        srch_inter_freq_meas_db
     
RETURN VALUE
    void

=======================================================================*/
extern void srch_interf_reset_meas_dbase(void);

/*======================================================================
FUNCTION SRCH_INTERF_UPDATE_CELL_LIST_IN_DCH_INIT

DESCRIPTION
    This function moves cells to VASet/Monitored set from Sync and Async
    sets during a DCH->FACH->DCH transition.
         
RETURN VALUE
    void

=======================================================================*/
extern void srch_interf_update_cell_list_in_dch_init(void);

/*======================================================================
FUNCTION SRCH_INTERF_RESET_VASET_EVENT_DBASE

DESCRIPTION
    Resets the existing interf freq vaset event dbase
    In particular the following data structures are impacted
        
        srch_interf_vaset_intra_freq_evt_db
     
RETURN VALUE
    void

=======================================================================*/
extern void srch_interf_reset_vaset_event_dbase(void);

/*======================================================================
FUNCTION SRCH_INTERF_REMOVE_INTERF_CELLS

DESCRIPTION
    Removes all the cells (from driver as well) upon entering DCH state
         
RETURN VALUE
    void

=======================================================================*/
extern void srch_interf_remove_all_interf_cells(void);

/*======================================================================
FUNCTION SRCH_INTERF_DIVIDE_CELLS_BY_FREQ

DESCRIPTION
    Used to update the inter-frequency cell list during idle mode.

RETURN VALUE
    void

=======================================================================*/
extern void srch_interf_divide_cells_by_freq( void );

/*======================================================================
FUNCTION SRCH_INTERF_GET_CELL_INFO

DESCRIPTION
    Used to update the inter-frequency cell list.

RETURN VALUE
    boolean

=======================================================================*/
extern void srch_interf_get_cell_info(l1_inter_freq_cell_list_struct_type *cell_list,
                                      l1_meas_ctrl_enum_type               parm_type);

/*======================================================================
FUNCTION SRCH_MEAS_INTER_F_SIB_UPDATE

DESCRIPTION
    Used to update the inter-frequency SIB information received in 
    a CPHY_MEASUREMENT_REQ command message.

RETURN VALUE
    boolean

=======================================================================*/
extern void srch_inter_f_meas_sib_update(l1_meas_cmd_type *cmd_ptr);

/*======================================================================
FUNCTION SRCHMEAS_REMOVE_INTERFREQ_MEASUREMENTS

DESCRIPTION
    Currently we use this in DCH state to release inter freq measurement
     

RETURN VALUE
    boolean

=======================================================================*/ 
extern boolean srch_interf_remove_interfreq_measurements(uint16 meas_id);
 
/*======================================================================
FUNCTION SRCH_INTERF_GET_MEAS_IDX_BASED_ON_ID

DESCRIPTION
    Returns the meas index into the measurements DB table based on
    the measurement id
RETURN VALUE
    uint16

=======================================================================*/
extern uint16 srch_interf_get_meas_idx_based_on_id(uint16 meas_id);
/*======================================================================
FUNCTION SRCH_INTERF_UPDATE_INTERF_RPT_TRANS_MODE_INFO

DESCRIPTION
    Used to update the report trans mode
     

RETURN VALUE
    void

=======================================================================*/
extern void srch_interf_update_interf_rpt_trans_mode_info(uint16 meas_id,
                 l1_meas_report_trans_mode_enum_type rpt_trans_mode);
/*===========================================================================
FUNCTION        srch_interf_evaluate_events_2d_2f
    

DESCRIPTION     This function evaluates the event triggered inter freq
                measurements 2D and 2F.
                
DEPENDENCIES    

RETURN VALUE    
            None
        
SIDE EFFECTS    
===========================================================================*/
extern void srch_interf_evaluate_events_2d_2f(void);

/*===========================================================================
FUNCTION        SRCH_INTERF_SEND_PERIODIC_MEAS_REPORT
    

DESCRIPTION     This function prepares and sends the periodic measurement 
                report for inter freq measurements to RRC
                
DEPENDENCIES    

RETURN VALUE    
            None
        
SIDE EFFECTS    
===========================================================================*/
extern void srch_interf_send_periodic_meas_report(uint16 meas_id);
/*======================================================================
FUNCTION SRCH_INTERF_DCH_INIT

DESCRIPTION
    Resets all the DS upon entering DCH state
         
RETURN VALUE
    void

=======================================================================*/
extern void srch_interf_dch_init(void);

/*======================================================================
FUNCTION SRCH_INTERF_PROCESS_SRCH_DONE_SIGNAL

DESCRIPTION
    This function processes the CM search done signal
     

RETURN VALUE
    void

=======================================================================*/
extern void srch_interf_process_srch_done_signal (void) ;

/*=========================================================================

FUNCTION     : SRCH_INTERF_FACH_ADD_CELLS 

DESCRIPTION  : This function add FACH InterFreq cells
              
DEPENDENCIES : Must be called at the end of srch_interf_add_cells()

RETURN VALUE : None
      
SIDE EFFECTS : As said
=========================================================================*/

extern void srch_interf_fach_add_cells(void);
/*=========================================================================

FUNCTION     : SRCH_INTERF_FILTER_ENG 

DESCRIPTION  : This function filters the interf freq measurements. The formula 
for calculating alpha_prime is:
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
extern void srch_interf_filter_eng(uint32 *filt_eng, uint32 last_meas_eng, uint32 last_meas_gap, int8 filt_len,uint32 srch_period);


/*=========================================================================

FUNCTION     : srch_interf_handle_sync_state_for_evt2d_delay 
               
DESCRIPTION  : This function will be called every frame from srch hb evt handler. This
                      function checks the sync state and make decision whether the 2D TTT
                      should be extended or not and by what margin if extended 
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
extern void srch_interf_handle_sync_state_for_evt2d_delay(void);

/*=========================================================================

FUNCTION     : srch_interf_add_evt2d_delay_info 
               
DESCRIPTION  : This function will enable the evt_2d database indexed by meas_idx 
  
DEPENDENCIES : None

RETURN VALUE : None
      
SIDE EFFECTS : None

=========================================================================*/
LOCAL void srch_interf_add_evt2d_delay_info(uint16 meas_idx);

/*===========================================================================
FUNCTION         srchset_is_interf_cell_unique

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

extern boolean srchset_is_interf_cell_unique
(
  srch_interf_cell_struct_type  *cell_ptr,
  srch_interf_cell_struct_type  **start_address,
  uint8 cell_cnt
);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION     SRCH_INTERF_GET_CARR_FREQ

DESCRIPTION
      This Function will be used specifically for the purpose of getting downlink freq. 
      of multi-carrier(2C/3C etc.) which is eligible for inter-f measurement without CM.
      If the return value is NO_FREQUENCY for the input carr,it means that either freq. is not 
      configured or if configured, not eligible for measurement without compressed mode.

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  Downlink UARFCN of the carrier specified.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 srch_interf_get_carr_freq(uint8 carr_idx);

/*===========================================================================

FUNCTION     SRCH_INTERF_GET_CARR_INTERF_IDX

DESCRIPTION
      This Function will give the index of the inter frequency which is same as that of the carrier
      provided as input.

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  Inter frequency index of the carrier specified 

SIDE EFFECTS
  None.
===========================================================================*/
uint8 srch_interf_get_carr_interf_idx(uint8 carr_idx);

/*===========================================================================

FUNCTION     srch_interf_is_carr_present_in_interf_db

DESCRIPTION
      This Function will give the freq idx of inter frequency which coincide with DC freq
      while DC HSUPA is configured

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  Inter frequency index which co-incides with downlink DC freq else default value 0xff

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean srch_interf_is_carr_present_in_interf_db(uint8 carr_idx);

/*===========================================================================
FUNCTION     SRCH_INTERF_DETERMINE_DC_CARR_IDX

DESCRIPTION
   This Function will check if we have to use Inter-f cells on index 0 or 1.

DEPENDENCIES
  None.

PARAMETERS
  None
 
RETURN VALUE
  Void

SIDE EFFECTS
  None 
===========================================================================*/
extern uint8 srch_interf_determine_dc_carr_idx(void);

/*===========================================================================
FUNCTION     SRCH_INTERF_UPDATE_CELLS_FROM_INTRAF_CELL_INFO_LIST

DESCRIPTION
   This Function will update the interf cell info list through intraf cell info list configured on secondary 
   carrier while DC-HUSPA is configured. These cells will be used for inter-f measurement evluation on 
   secondary DL frequency, if configured as one of the inter-frequency on primary carrier. 

DEPENDENCIES
  None.

PARAMETERS
  Intra-f cell info list
 
RETURN VALUE
  Void

SIDE EFFECTS
  Inter-f cell list will be update (Add/Remove)
===========================================================================*/
extern void srch_interf_update_interf_cells_from_intraf_cell_info_list(l1_intra_freq_cell_list_struct_type *);

/*===========================================================================

FUNCTION     SRCH_INTERF_GET_SET_COUNT_ON_FREQ_IDX

DESCRIPTION
   This Function will count and give no. of VASET,VMSET,NSET cells on the frequency index specified 

DEPENDENCIES
  None.

PARAMETERS
  pointerf carrying the value of Vaset and Vmset
 
RETURN VALUE
  Return is void, but argument values will be filled with VASET,VMSET,NSET on freq. idx specified.

SIDE EFFECTS
  None.
===========================================================================*/
//LOCAL void srch_interf_get_set_count_on_freq_idx(uint8 freq_idx,uint16 *vaset_cnt, uint16 *vmset_cnt,uint16 *nset_cnt);

/*===========================================================================

FUNCTION     SRCH_INTERF_EVALUATE_INTERF_MEAS

DESCRIPTION
  This Function will evaluate the interf measurement on Multi carrier once Intraf PN search is done.

DEPENDENCIES
  None.

PARAMETERS
  Carrier Index
 
RETURN VALUE
  Void

SIDE EFFECTS
  This may cause change in interf cell parameters, set management and Measurement reporting
===========================================================================*/
extern void srch_interf_evaluate_interf_events_in_intraf_mode(uint8 carr_idx);

/*===========================================================================

FUNCTION     SRCH_INTERF_UPDATE_PARAMS_FROM_INTRA_CELL

DESCRIPTION
      This Function will be update the parameters of interf cell from intraf cell.
      
DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  None

SIDE EFFECTS
  Interf cell pointer params will get updated.
===========================================================================*/
extern void srch_interf_update_params_from_intraf_cell(boolean first_time ,srch_interf_cell_struct_type *, 
                                                               srch_cell_struct_type *);

/*===========================================================================

FUNCTION     SRCH_INTERF_MULTI_CARR_CONFIG

DESCRIPTION
      This Function will be called with the configuration of multi carrier(other than primary) in DL/UL and
      set the flag and other related parameters used to carry interf measurement through intraf mode, if
      already being done by Compressed Mode.

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  None

SIDE EFFECTS
  Set movement and parameter change in interf cells
===========================================================================*/
extern void srch_interf_multi_carr_config(uint8 carrier_idx);

/*===========================================================================

FUNCTION   SRCH_INTERF_MULTI_CARR_DECONFIG

DESCRIPTION
      This Function will be called while a additional carrier is deconfigured in UL and set the flag and other related parameters 
      used to carry the interf measurement through CM mode, if it is already ongoing through DC mode.

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void srch_interf_multi_carr_deconfig(uint8 carrier_idx);
#endif

#endif /* SRCHINTERF_H */
