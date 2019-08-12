#ifndef SRCHMEAS_H
#define SRCHMEAS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            S R C H M E A S . H                  

GENERAL DESCRIPTION
  This module handles periodic measurement requests from RRC.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2012 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/srchmeas.h_v   1.0   11 Mar 2002 16:57:06   cemani  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/srchmeas.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/14   ar      Added a parameter to srchmeas_copy_cell_info_intra_to_inter
05/29/14   ag      R10 periodic measurement support on second carrier.
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/10/14   ks/cc   Add LTE and GSM PLT cmds
01/13/14   jk      DC-HSUPA Interf Spec CR Changes.
01/02/14   da      Fixed search measurement related KW errors
09/12/13   jd      Searcher redesign 
01/18/13   vn      Externing function srchmeas_get_meas_idx_based_on_id.
02/08/12   vs      Feature cleanup.
09/03/11   sa      Protected filter database shared between L1 and GPS task 
                   under critical section.
11/02/10   nd      Added code changes for FEATURE_WCDMA_DEFERRED_SIB11_12_READING
05/03/10   sv      Fixed compiler warnings.
09/09/09   rvs     Added function srchmeas_get_cme_filt_idx() to get filter
                   index of Event 1A for CME if available.
06/26/09   sa      Added MCM failure cause enum data type.
04/22/09   stk     Compiler warning fixes	
10/03/08   hk      Bringing in 7k Mailine fixes
03/17/07   sv      Externed the "srchmeas_print_cell_info" function. 
11/29/06   nd      Added multiple layer3 filter support for intra freq measurement
                   under FEATURE_WCDMA_MULTIPLE_L3_FILTERS_SUPPORTED
11/01/06   cpe     Mainlined the following features:
                    -FEATURE_STTD_CAPABLE
08/23/06   rc      Added support for inter-freq virtual active set under 
                   FEATURE_WCDMA_VASET_SUPPORT.
06/05/06   sv      Added prototype for update cells with meas info function.
01/10/06   ub      Added prototypes for cell info copy functions
08/05/05   ub      Added function prototypes for initializing meas filter
04/11/05   cpe     Code addition for resuming to old config upon HHO failure. 
05/26/04   cpe     Added an extern
12/09/03   yshi    Support for GSM Quadband
11/13/03   src     Added declaration for srchmeas_intra_freq_meas_performed
                   to track measurements when HHOs are performed.
11/12/03   cpe     Extern'ed a few functions.
11/04/03   cpe/src Added declaration for srchmeas_determine_sttd_behavior.
10/31/03   cpe     Removed the FEATURE_WCDMA_MULTIPLE_INTRA_MEAS_SUPPORTED 
                   feature which is always present
05/23/03   cpe     Added support for multiple intra freq measurements
03/06/03   djm     addition of srchmeas_inter_s_find_cell, a find
                   of GSM cells based on ARFCN, BSIC.
10/01/02   ddh     Added changes for InterRat processing
09/11/01   cpe     Created file.

===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "l1rrcif.h"
#include "srchset.h"
#include "srchinterf.h"
#include "wsrchi.h"

#define SRCH_FILTER_COEF_TABLE_K_VALUE              20
#define SRCH_FILTER_COEF_TABLE_PN_SRCH_INTERVAL      5

/***************************************************************************

    D A T A     S T R U C T U R E S 
    
 ***************************************************************************/
 /* This will check the cause of the failure of MCM in l1 */
 typedef enum 
 {  
   SRCHMEAS_MCM_FAILURE_CAUSE_NO_FAILURE,
   SRCHMEAS_MCM_FAILURE_CAUSE_NUM_MEAS_EXCEED
 } srchmeas_mcm_failure_cause_enum_type;
 extern srchmeas_mcm_failure_cause_enum_type srchmeas_mcm_failure_cause;

       
/*=========================================================================

    V A R I A B L E     D E C L A R A T I O N S


 ===========================================================================*/

/* This global will keep track of the situation when there are no NW configured
filter. In that case the default filter will be used. Later when NW configures a filter
this flag will be turned false */
extern boolean srchmeas_no_filt_configured_frm_nw;
       
extern rex_crit_sect_type srch_meas_filt_crit_sec;
#define SRCH_MEAS_FILT_DB_LOCK() REX_ISR_LOCK(&srch_meas_filt_crit_sec)
#define SRCH_MEAS_FILT_DB_FREE() REX_ISR_UNLOCK(&srch_meas_filt_crit_sec)

extern uint16 srchmeas_get_meas_idx_based_on_id(uint8, uint16);
extern boolean srchmeas_inter_s_sib_update(l1_meas_cmd_type *);
/*===========================================================================
FUNCTION        SRCHMEAS_RESET_NEIGHBOR_CELL_LISTS
FILE NAME       

DESCRIPTION     This function resets the neighbor cell lists

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void srchmeas_reset_neighbor_cell_lists(uint8);       
       
/*===========================================================================
FUNCTION        SRCHMEAS_SEND_PERIODIC_MEAS_REPORT
FILE NAME       

DESCRIPTION     This function prepares and sends the measurement report
                to RRC
DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void srchmeas_send_periodic_meas_report(uint8 carr_idx,uint16 meas_id);

/*===========================================================================
FUNCTION     SRCHMEAS_INTER_S_FIND_CELL

DESCRIPTION
  This function will search all neighbor set cells to find one that
  has an ARFCN and BSIC that match those provided.

DEPENDENCIES
  None

RETURN VALUE
  srch_gsm_cell_struct_type * - a pointer to the GSM cell for which
                                <ARFCN,BSIC> match, NULL if not found.

SIDE EFFECTS
  None
===========================================================================*/
extern srch_gsm_cell_struct_type * srchmeas_inter_s_find_cell(
                                              W_ARFCN_T arfcn,
                                              uint8 bsic_ncc,
                                              uint8 bsic_bcc
                                            );

/*======================================================================
FUNCTION SRCHMEAS_INTER_S_RMV_CELL

DESCRIPTION
    Used to remove cell one cell from the inter_rat cell list

RETURN VALUE
    boolean
=======================================================================*/
void srchmeas_inter_s_rmv_cell( uint16 cellidx, srch_gsm_set_enum_type  set );

/*======================================================================
FUNCTION SRCHMEAS_INTER_S_RMV_CELLS

DESCRIPTION
    Used to remove cells from the Inter-RAT Neighbor set and srch list 
    using the information received in a CPHY_MEASUREMENT_REQ message.

RETURN VALUE
    boolean
=======================================================================*/
void srchmeas_inter_s_rmv_cells
(
  l1_inter_sys_cell_list_struct_type *cell_list
);

/*======================================================================
FUNCTION SRCHMEAS_INTER_S_RMV_ALL_CELLS

DESCRIPTION
    Used to remove *all* cells from the Inter-RAT database

RETURN VALUE
    None
=======================================================================*/
extern void srchmeas_inter_s_rmv_all_cells ( void );

/*======================================================================
FUNCTION SRCHMEAS_INTER_S_GET_CELLPTR

DESCRIPTION
    Used to add cells to the Inter-RAT Neighbor set and srch list using
    the information received in a CPHY_MEASUREMENT_REQ command message.

RETURN VALUE
    boolean

=======================================================================*/
srch_gsm_cell_struct_type * srchmeas_inter_s_get_cellptr
(
  uint16 cell_id
);

/*======================================================================
FUNCTION SRCHMEAS_INTER_S_ADD_CELLS

DESCRIPTION
    Used to add cells to the Inter-RAT Neighbor set and srch list using
    the information received in a CPHY_MEASUREMENT_REQ command message.

RETURN VALUE
    boolean

=======================================================================*/
boolean srchmeas_inter_s_add_cells
(
  l1_inter_sys_cell_list_struct_type *cell_list,
  l1_meas_ctrl_enum_type               parm_type,
  boolean bsic_verify_reqd,
  uint32  meas_id
);

/*======================================================================
FUNCTION SRCHMEAS_DETERMINE_STTD_BEHAVIOR

DESCRIPTION
    This function is used to determine STTD behavior     

RETURN VALUE
    void

=======================================================================*/
extern void srchmeas_determine_sttd_behavior(void);

/*===========================================================================
FUNCTION        SRCHMEAS_SUSPEND_MEASUREMENTS_FOR_HHO

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  void.

SIDE EFFECTS    
===========================================================================*/
extern void srchmeas_suspend_measurements_for_hho(void);

/*===========================================================================
FUNCTION        SRCHMEAS_RESTORE_MEASUREMENTS_UPON_RESUMPTION_ON_ORIG_FREQ

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  void.

SIDE EFFECTS    
===========================================================================*/
extern void srchmeas_restore_measurements_upon_resumption_on_orig_freq(void);

/*===========================================================================
FUNCTION        SRCHMEAS_RESTORE_MEASUREMENTS_UPON_SUCCESSFUL_HHO

FILE NAME       

DESCRIPTION     

DEPENDENCIES    

RETURN VALUE    
  void.

SIDE EFFECTS    
===========================================================================*/
extern void srchmeas_restore_measurements_upon_successful_hho(void);

/*===========================================================================
FUNCTION        SRCHMEAS_INIT_ASET_ENG_AND_FILT_LEN

FILE NAME       

DESCRIPTION     
  Used to initialize the filt_eng and filt_len of serving cell

DEPENDENCIES    

RETURN VALUE    
  void.

SIDE EFFECTS    
  Filt energy is initialized to 0 and filter is restarted
===========================================================================*/
extern void srchmeas_init_aset_eng_and_filt_len(void);

/*===========================================================================
FUNCTION        SRCHMEAS_INIT_INTRA_ENG_AND_FILT_LEN

FILE NAME       

DESCRIPTION     
  Used to initialize the filt_eng, filt_len, tot_eng and max_eng[0] of
  all intra freq neighbor cells and intra freq detected cells

DEPENDENCIES    

RETURN VALUE    
  void.

SIDE EFFECTS    
  Total and filt energy are initialized to 0 and filter is restarted
===========================================================================*/
extern void srchmeas_init_intra_eng_and_filt_len(void);

/*===========================================================================

FUNCTION     SRCHMEAS_COPY_CELL_INFO_INTER_TO_INTRA

DESCRIPTION
  Copy cell info from inter freq cell structure to intra freq cell structure

DEPENDENCIES
  None.

PARAMETERS
  pointer to inter freq cell structure and intra freq cell structure
 
RETURN VALUE
  none

SIDE EFFECTS
  inter freq cell info copied to intra freq cell info
===========================================================================*/
extern void srchmeas_copy_cell_info_inter_to_intra(
              srch_interf_cell_struct_type *inter_f_cell_ptr,
              srch_cell_struct_type *intra_f_cell_ptr);

/*===========================================================================

FUNCTION     SRCHMEAS_COPY_CELL_INFO_INTRA_TO_INTER

DESCRIPTION
  Copy cell info from intra freq cell structure to inter freq cell structure

DEPENDENCIES
  None.

PARAMETERS
  pointer to intra freq cell structure and inter freq cell structure
 
RETURN VALUE
  none

SIDE EFFECTS
  inter freq cell info copied to intra freq cell info
===========================================================================*/
extern void srchmeas_copy_cell_info_intra_to_inter(
              srch_cell_struct_type *intra_f_cell_ptr,
              srch_interf_cell_struct_type *inter_f_cell_ptr, boolean reset_filt);

extern void srchmeas_update_cells_with_cells_for_meas_info(uint8 carr_idx);

extern boolean srchmeas_get_cell_for_meas_info(srch_cell_struct_type *cell_ptr, uint16 meas_idx);

extern uint16 srchmeas_get_filt_idx(void);

extern uint16 srchmeas_get_cme_filt_idx(void);

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
/*===========================================================================

FUNCTION     SRCHMEAS_DEFERRED_SIB_MEAS_REQ_UPDATE

DESCRIPTION
    This function handle the deferred SIB meas request from RRC and update the parameters.

DEPENDENCIES
  None.

PARAMETERS

 
RETURN VALUE
  boolean - TRUE - if it successful, FALSE otherwise

SIDE EFFECTS
  
===========================================================================*/
boolean srchmeas_deferred_sib_meas_req_update(l1_meas_cmd_type *cmd_ptr);
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================

FUNCTION     SRCHMEAS_DC_UL_FREQ_DECONFIG

DESCRIPTION
  This function will do the step of procedure in searchers on deconfiguration of DC in UL.

DEPENDENCIES
  None.

PARAMETERS
  None
 
RETURN VALUE
  none

SIDE EFFECTS
  Reset the Measurement and SFN related parameters on DC carrier 
===========================================================================*/
extern void srchmeas_dc_ul_freq_deconfig(void);
#endif

#endif /* SRCHMEAS_H */
