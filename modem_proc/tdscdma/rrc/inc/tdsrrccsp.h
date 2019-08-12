#ifndef TDSRRCCSP_H
#define TDSRRCCSP_H
/*===========================================================================
                        R R C    C S P

DESCRIPTION
    This file contains definitions needed for Cell Selection Procedure.   
   
      
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2010-2012
Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrccsp.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/11   zz       Merged with VU_MODEM_WCDMA_RRC_OSYS.01.93.53

===========================================================================*/

/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsrrccmd_v.h"
#include "tdsrrccspdb.h"
#include "tdsrrcueci.h"
#include "tdsrrcmm_v.h"
#include "sys.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* The following two enum types tdsrrc_csp_cell_bar_info_e_type and 
 * tdsrrc_csp_T_barred_e_type are declared for T barred support
 */

/* This is used to inform L1 if the GSM cell is barred, or TDSCDMA 
 * cell is barred, or TDSCDMA frequency is barred, or the active
 * set is barred
 */
typedef enum
{
  TDSRRC_CSP_GSM_CELL_BARRED,
  RRC_CSP_TDS_CPID_BARRED,
  RRC_CSP_TDS_FREQ_BARRED,
  RRC_CSP_TDS_ASET_BARRED,
#ifdef FEATURE_TDSCDMA_TO_LTE
  TDSRRC_CSP_LTE_CELL_BARRED,
  TDSRRC_CSP_LTE_FREQ_BARRED,
#endif
}tdsrrc_csp_cell_bar_info_e_type;


/* This is used to inform L1 of the T-barred timer value */
typedef enum
{
  TDSRRC_T_BARRED_S10,
  TDSRRC_T_BARRED_S20,
  TDSRRC_T_BARRED_S40,
  TDSRRC_T_BARRED_S80,
  TDSRRC_T_BARRED_S160,
  TDSRRC_T_BARRED_S320,
  TDSRRC_T_BARRED_S640,
  TDSRRC_T_BARRED_S1280
}tdsrrc_csp_T_barred_e_type;

extern boolean tdslac_in_ota_present;
extern uint8 tdslac_in_ota[RRC_MAX_GSMMAP_LAC_LENGTH];
#ifdef FEATURE_TDSCDMA_SRNS_MO_CALL_DEFER
extern boolean tdslac_in_ota_present_bak;
extern uint8  tdslac_in_ota_bak[RRC_MAX_GSMMAP_LAC_LENGTH]; 
#endif
#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
/*Stores cell_id for each RL given in OTA msg*/
extern uint32  tdscell_id_per_rl[TDSL1_MAX_RL];     
#endif 
/* defines the max num of callbacks that can register for 
 * cell ID change
 */
#define TDSMAX_NUM_OF_REG_CB 3



/*===========================================================================
**                  FUNCTION DECLARATIONS
**=========================================================================*/

/*===========================================================================

FUNCTION          RRC_CELL_SELECTION_PROCEDURE

DESCRIPTION       This is the entry point for Cell Selection procedure. All
                  events for CSP come to this function from RRC Dispatcher.
                  Depending on the substate of Cell Selection Procedure, it 
                  calls the corresponding substate handler. It also maintains 
                  the CSP substate and updates it with the retuned value from
                  CSP substate handler.
  
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
extern  void  tdsrrccsp_cell_selection_procedure( tdsrrc_cmd_type  *cmd_ptr );


/*===========================================================================

FUNCTION          RRC_INIT_CELL_SELECTION_PROCEDURE

DESCRIPTION       This function initializes any global data for the procedure.
                   
                  NOTE: This could be used later to get NV data from NV.
           
DEPENDENCIES
                  None

RETURN VALUE
                  None
                  NOTE: We can have a return value of Pass/fail and tie that
                  up with deciding the Offline/Online mode ofthe UE but that 
                  all is for later times.

SIDE EFFECTS
                  None

===========================================================================*/
extern  void  tdsrrccsp_init_cell_selection_procedure( void );


/*===========================================================================

FUNCTION          RRCCSP_GET_SERVING_PCCPCH_PARMS

DESCRIPTION       This function fills up scrambling code and STTD indicator
                  for serving cell PCCPCH in the passed parameters.
           
DEPENDENCIES
                  The caller has to ensure that Cell selection would have 
                  got valid values for scrambling code and STTD indicator
                  before this function call is made.

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
extern  void tdsrrccsp_get_serving_pccpch_parms(boolean *tstd_ptr);

/*===========================================================================

FUNCTION          RRCCSP_GET_CURRENT_PLMN_CELL_ID

DESCRIPTION       This function fills up PLMN & Cell ID.
           
DEPENDENCIES
                  The caller has to ensure that this function will not be called for reselection
                  and also only after CELL_TRANSITION_REQ is sent for cell selection.

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void tdsrrccsp_get_current_plmn_cell_id(rrc_plmn_identity_type *plmn_id, tdsrrc_int_cell_id_type *cell_id);


/*===========================================================================

FUNCTION          RRCCSP_GET_NEIGHBOR_PCCPCH_PARMS

DESCRIPTION       This function fills up scrambling code
                  for neighbor cell PCCPCH in the passed parameter.
           
DEPENDENCIES
                  The caller has to ensure that Cell selection would have 
                  got valid value for scrambling code
                  before this function call is made.

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void tdsrrccsp_get_neighbor_pccpch_parms(uint8 *cpid_ptr,
                                      uint16 * freq_ptr);


/*===========================================================================

FUNCTION          tdsrrccsp_is_state_transition_triggered_by_reconfig_msg

DESCRIPTION       This function returns a TRUE if the state transition was
                  trigerred by a Reconfig message and a FALSE when it was
                  done by UE on its own (e.g. due to Radio Link Failure)                  
DEPENDENCIES
                  None
RETURN VALUE
                  Boolean
SIDE EFFECTS
                  None

===========================================================================*/
boolean  tdsrrccsp_is_state_transition_triggered_by_reconfig_msg( void );


/*===========================================================================

FUNCTION          tdsrrccsp_is_selected_cell_utran_directed

DESCRIPTION       This function returns a TRUE if the selected cell is the
                  same as directed by UTRAN and a FALSE when it is
                  different or when the cell to be camped is not indicated
                  by UTRAN.
DEPENDENCIES
                  This function should be called ONLY IF a call to
                  tdsrrccsp_is_state_transition_triggered_by_reconfig_msg returns
                  a TRUE.
RETURN VALUE
                  Boolean
SIDE EFFECTS
                  None

===========================================================================*/
boolean  tdsrrccsp_is_selected_cell_utran_directed(void);


/*===========================================================================

FUNCTION          tdsrrccsp_is_inter_frequency_cell_resel_in_progress

DESCRIPTION       This function returns TRUE if Inter-frequency cell reselection
                  is in progress.

DEPENDENCIES
                  None

RETURN VALUE
                  FALSE: Inter-frequency cell reselection not in progress.
                  TRUE: Inter-frequency cell reselection is in progress.

SIDE EFFECTS
                  None

===========================================================================*/
boolean  tdsrrccsp_is_inter_frequency_cell_resel_in_progress(void);


#if defined(FEATURE_INTERRAT_HANDOVER_G2TD) || defined(FEATURE_TDSCDMA_L2TD_PS_HANDOVER)
/*===========================================================================

FUNCTION          RRCCSP_UPDATE_PLMN_INFO_FOR_GTOW_HANDOVER

DESCRIPTION       This function updates the CSP internal data with the PLMN Id
                  and PLMN type information. It further updates CCM camping
          information.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void tdsrrccsp_update_plmn_info_for_g2td_handover
(
  rrc_activation_rsp_type *rrc_activation
);
#endif


/*===========================================================================

FUNCTION          tdsrrccsp_init_measurements_in_l1_on_cphy_idle

DESCRIPTION       This function returns a value that indicates if Measurements
                  need to be initialized in L1 on a TDSL1_CPHY_IDLE_REQ.

DEPENDENCIES
                  None

RETURN VALUE
                  FALSE: Intra-freq measurements and Inter-freq meas need to
                  be saved.
                  TRUE: Intra and Inter frequency measurements do not need 
                  to be initialized.

SIDE EFFECTS
                  None

===========================================================================*/
boolean  tdsrrccsp_init_measurements_in_l1_on_cphy_idle(void);


/*===========================================================================

FUNCTION          RRCCSP_FILL_INFO_FOR_RR

DESCRIPTION       This function is responsible for filling the band preference
                  and the network select mode to be sent to RR.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
void tdsrrccsp_fill_info_for_rr(sys_network_selection_mode_e_type *network_select_mode);


/*===========================================================================

FUNCTION          TDSRRCCSP_FILL_FPR_INFO_FOR_RR

DESCRIPTION       This function is responsible for filling the pseudo fast 
                  return info, including TD frequencies and RSCP threshold,
                  to be sent to RR.

DEPENDENCIES
                  
RETURN VALUE
                  TRUE if Pseudo FR is enable; FALSE otherwise

SIDE EFFECTS
                  None.
                  
===========================================================================*/
boolean tdsrrccsp_fill_pfr_info_for_rr(tdsrr_ho_pseudo_fast_return_type *pfr_info_ptr);


/*===========================================================================

FUNCTION          RRCCSP_SET_UE_IN_DEEP_SLEEP

DESCRIPTION       This function is responsible for setting the deep sleep variable
                  when UE is going to deep sleep.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
void tdsrrccsp_set_ue_in_deep_sleep(void);


/*===========================================================================

FUNCTION          RRCCSP_UPDATE_BAND_PREFERENCE

DESCRIPTION       This function updates the information related to supported
                  frequency bands based on the band_pref mask.

DEPENDENCIES

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

===========================================================================*/
void tdsrrccsp_update_band_preference(void);


/*===========================================================================

FUNCTION          RRCCSP_IS_SUPPORTED_BAND

DESCRIPTION       This function checks whether the passed in band is supported
                  by the UE or not.

DEPENDENCIES

RETURN VALUE
                  None.

SIDE EFFECTS
                  None.

===========================================================================*/
boolean tdsrrccsp_is_supported_band(tdsrrc_csp_band_class_type band_class);


/*===========================================================================

FUNCTION          RRCCSP_IS_PREFERENCE_BAND

DESCRIPTION       This function checks whether the passed in band is in preference
                  band list from NAS.

DEPENDENCIES
                  
RETURN VALUE
                  BOOLEAN.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
boolean tdsrrccsp_is_preference_band(tdsrrc_csp_band_class_type band_class);

/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_ACQ_DB

DESCRIPTION       This function adds an acquisition entry based on the network
                  direction (e.g. RRC connection release message) to the 
                  acquisition list. While doing so, it also updates the 
                  acquisition database pointers so that the 1st pointer points
                  to this most recent acquisition entry.
                   
           
DEPENDENCIES
                  None

RETURN VALUE
                  Success/Failure. Failure will be an extra-ordinary event and
                  could be due to irreversible causes like corruption of pointers.

SIDE EFFECTS
                  None.

===========================================================================*/
uecomdef_status_e_type  tdsrrccsp_update_acq_db(tdsrrc_freq_type freq);


/*===========================================================================

FUNCTION        RRC_GET_FREQUENCY_BAND

DESCRIPTION     This function looks at the passed in frequency and
                returns the band the frequency belongs to.
  
RETURN VALUE    Returns TDSRRC_CSP_BAND_MAX if UARFCN is invalid.
 
SIDE EFFECTS    None

===========================================================================*/
tdsrrc_csp_band_class_type tdsrrc_get_frequency_band
(    
  tdsrrc_freq_type freq
);


/*===========================================================================

FUNCTION          tdsrrccsp_get_supported_band_list

DESCRIPTION       This function checks which bands are supported and sends the
                  populated list to UECI.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
void tdsrrccsp_get_supported_band_list(tdsrrcueci_band_storage_type * ueci_band);


/*===========================================================================

FUNCTION          RRCCSP_SEND_BPLMN_SEARCH_CNF_TO_RR

DESCRIPTION       This function is used by CSP to send TDSRRC_INTERRAT_PLMN_SRCH_CNF
                  to RR when BPLMN search timer expires, or highest priority PLMN
                  is found, or all frequencies are exhausted.

DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void tdsrrccsp_send_bplmn_search_cnf_to_rr( void );


/*===========================================================================

FUNCTION          RRCCSP_SEND_BPLMN_SEARCH_ABORT_CNF_TO_RR

DESCRIPTION       This function is used by CSP to send 
                  TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF
                  to RR when CSP receives TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ
                  from RR (via MCM)

DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void
tdsrrccsp_send_bplmn_abort_cnf_to_rr( void );


/*===========================================================================

FUNCTION RRCCSP_IS_IN_BPLMN_LIST

DESCRIPTION

  This function is called by SIB procedure once it reads the MIB to check if it
  should proceed further and read SIB1 and SIB3 for that cell. If any of the requested 
  PLMNs are in the bcch shared plmn list then SIB should proceed with reading SIBS.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If PLMN is in the BPLMN list
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrccsp_is_in_bplmn_list (rrc_plmn_identity_type plmn_id
                                 , tdsrrc_MasterInformationBlock *mib_ptr
                                 ); 


/*===========================================================================

FUNCTION          tdsrrccsp_is_ue_in_true_limited_service

DESCRIPTION       This function checks the network select mode and returns TRUE/FALSE
                  to the calling procedure.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
boolean tdsrrccsp_is_ue_in_true_limited_service(void);


/*===========================================================================

FUNCTION RRCCSP_SEND_CPHY_CELL_BAR_REQ

DESCRIPTION

  This function is called by CSP, SIB, RCE, and CHO procedures to send TDSL1_CPHY_CELL_BAR_REQ
  to L1 so that the corresponding cell or frequency or active set can be barred.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrccsp_send_cphy_cell_bar_req(tdsrrc_csp_cell_bar_info_e_type, uint32);


/*===========================================================================

FUNCTION          RRCCSP_SEND_INTERRAT_RESELECTION_ABORT_CNF

DESCRIPTION       This function is responsible for sending
                  TDSRRC_INTERRAT_RESELECTION_ABORT_CNF to RR.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_interrat_reselection_abort_cnf( void );


/*===========================================================================

FUNCTION          RRCCSP_SEND_INTERRAT_RESELECTION_REJECT

DESCRIPTION       This function is responsible for sending
                  TDSRR_INTERRAT_RESELECTION_REJ to RR.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_interrat_reselection_reject( void );


/*===========================================================================

FUNCTION RRC_CSP_GET_ASET_CPID_PN

DESCRIPTION
  This function gets the cell parameters ID and pn position of the best cell
  in the active set.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsrrccsp_get_aset_cpid_pn(void);


/*===========================================================================

FUNCTION          RRC_CSP_SEND_SERVICE_IND

DESCRIPTION       This function sends the RRC_SERVICE_IND to MM
                  with the current cell's parameters.Currently used by 
                  CHO after a W->G Cell change order failure

DEPENDENCIES
                  None.
                  
RETURN VALUE
                  None.
                  
SIDE EFFECTS
                  None.
===========================================================================*/
void tdsrrccsp_send_service_ind (void);

/*===========================================================================

FUNCTION          tdsrrccsp_send_backup_service_ind

DESCRIPTION       This function sends RRC_SERVICE_IND to MM with backup service ind

DEPENDENCIES
                  None.

RETURN VALUE
                  None.
                  
SIDE EFFECTS
                  None.
===========================================================================*/
void tdsrrccsp_send_backup_service_ind(void);

/*===========================================================================

FUNCTION RRC_DEREGISTER_SERCING_CELL_CHANGE_IND_CB

DESCRIPTION

This function enables upper layer to deregister the callback function that was
registered for notification of serving cell id.


DEPENDENCIES

  None.
  
RETURN VALUE
  
  BOOLEAN:  TRUE signifies that deregistration has succeeded.  FALSE signifies
            that deregistration failed.

SIDE EFFECTS

  None.

===========================================================================*/
extern boolean tdsrrc_deregister_serving_cell_change_ind_cb 
(
  TDSRRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE *cb_ptr
);


/*===========================================================================

FUNCTION RRC_NOTIFY_CELL_ID_CHANGE_CB_FUNC

DESCRIPTION

This function is called to notify the registered callback of the serving
cell change id.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
extern void tdsrrccsp_notify_cell_id_change
(
  void
); 

/*===========================================================================
FUNCTION tdsrrccsp_get_lac_id_in_dch

DESCRIPTION
  This function is called to retrieve LAC from CN Info.

DEPENDENCIES
  None.
  
RETURN VALUE
  SUCCESS - LAC was extracted from CN Info.
  FAILURE - LAC couldn't be ectracted from CN Info.

SIDE EFFECTS
  None.
===========================================================================*/
uecomdef_status_e_type tdsrrccsp_get_lac_id_in_dch
(
  rrc_gsm_map_nas_sys_info_type  * nas_common_info
);

#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
/*===========================================================================
FUNCTION tdsrrccsp_get_rac_id_in_dch

DESCRIPTION
  This function is called to retrieve RAC from CN Info

DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void tdsrrccsp_get_rac_id_in_dch
(
  tdsrrc_CN_DomainInformationList *cn_domain_info_list_ptr
);
#endif
/*===========================================================================
FUNCTION tdsrrc_notify_cell_id_in_dch

DESCRIPTION
  This function is called to notify the registered callback of the serving
  cell_id in Cell_DCH state.

DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tdsrrc_notify_cell_id_in_dch
(
  void
);

/*===========================================================================

FUNCTION   RRC_CSP_SEND_BPLMN_SUSPEND_REQ

DESCRIPTION

  This function sends TDSRRC_BPLMN_SUSPEND_REQ to CSP
  to suspend the ongoing Manual or Automatic Background PLMN Search.
  If RCE sends the suspend request then the ongoing BPLMN search is aborted
  while for other procedures such as SIB and CU, BPLMN search is only
  suspended
  
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.
 
SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrccsp_send_bplmn_suspend_req(tdsrrc_proc_e_type proc, tdsrrc_bplmn_suspend_cause_e_type cause);


/*===========================================================================

FUNCTION          RRCCSP_UPDATE_WTOW_BACKGROUND_PLMN_LIST

DESCRIPTION       This function reads the SIBs for a cell and checks if it is
                  barred or in the forbidden lai list. If not, it will try to
                  add it to the WTOW background PLMN list to be returned to MM in the
                  RRC_PLMN_LIST_CNF.
                  
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void tdsrrccsp_update_td2td_background_plmn_list(
  rrc_plmn_identity_type plmn_id, tdsrrc_int_cell_id_type cell_id);


/*===========================================================================

FUNCTION          RRCCSP_UPDATE_WTOW_BACKGROUND_PLMN_LIST_FOR_SIB_INCOMPLETE

DESCRIPTION       This function is called when SIB incomplete but shall 
                  check and update BPLMN result. It reads the SIBs for a
				  cell and checks if it is barred or in the forbidden lai list. 
				  If SIB1/SIB3 not exist, ignore the corresponding check.
				  It will try to add it to the WTOW background PLMN list 
				  to be returned to MM in the RRC_PLMN_LIST_CNF.
                  
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void tdsrrccsp_update_td2td_background_plmn_list_for_sib_incomplete(
  rrc_plmn_identity_type plmn_id, tdsrrc_int_cell_id_type cell_id);

/*===========================================================================

FUNCTION          RRCCSP_UPDATE_WTOW_MANUAL_BPLMN_LIST_WITH_RPLMN

DESCRIPTION       This function updates the WTOW Available PLMN list with the 
                  PLMN on which UE is camped on. This function is called only
                  when MANUAL PLMN_LIST_REQ is received from NAS and UE is 
                  in service.
                                    
DEPENDENCIES
                  None
RETURN VALUE
                  None
SIDE EFFECTS
                  None

===========================================================================*/
void tdsrrccsp_update_td2td_manual_bplmn_list_with_rplmn(
  rrc_plmn_identity_type plmn_id, tdsrrc_int_cell_id_type cell_id);


/*===========================================================================

FUNCTION          RRCCSP_PRIORITIZE_PENDING_COMMANDS

DESCRIPTION       This function is called when WTOW BPLMN search is 
                  in progress and multiple commands are received by CSP when 
                  it is waiting for confirmation from L1 - acq_cnf, freq_scan_cnf 
                  etc. This function allows CSP to prioritize the commands 
                  received in its pending substates and send appropriate confirmation.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void tdsrrccsp_prioritize_pending_commands(tdsrrc_cmd_type *cmd_ptr);


/*===========================================================================

FUNCTION          RRC_CSP_SEND_MM_PLMN_LIST_CNF_AS_ABORTED

DESCRIPTION       This function is responsible for filling up data structures
                  for PLMN List Confirm command to MM sent by CSP.
                  This function is called when PLMN_LIST_REQ from NAS needs to
                  be aborted immediately without affecting the ongoing BPLMN
                  search (if it is in progress). The n/w select mode in 
                  PLMN_LIST_CNF is always set to MANUAL in this function .

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void tdsrrccsp_send_mm_plmn_list_cnf_as_aborted( uint8);


/*===========================================================================

FUNCTION          RRCCSP_SAVE_REQ

DESCRIPTION       This function saves the new request for later. This request
                  will be processed once the appropriate confirmation on which
                  CSP was waiting on is received.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void tdsrrccsp_save_req(tdsrrc_cmd_type *cmd_ptr);


/*===========================================================================

FUNCTION          tdsrrccsp_send_gsm_interrat_bplmn_abort_req

DESCRIPTION       This function is responsible for sending
                  TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ to RR for aborting
                  WTOG BPLMN search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_gsm_interrat_bplmn_abort_req( void );

/*===========================================================================

FUNCTION          tdsrrccsp_send_gsm_interrat_bplmn_suspend_ind

DESCRIPTION       This function is responsible for sending
                  TDSRR_INTERRAT_PLMN_SRCH_SUSPEND_IND to RR for suspending
                  TD2G BPLMN search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_gsm_interrat_bplmn_suspend_ind( void );

/*===========================================================================

FUNCTION          tdsrrccsp_send_cell_param_req_for_pch_fach

DESCRIPTION       This function sends the cell param request to Layer1 if on  
                  transition from FACH to PCH or PCH to FACH, SIb4 is present
                  in the db and has reselection extensions t_reselection or
                  q_hyst.
                                               
DEPENDENCIES
                  None
RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void tdsrrccsp_send_cell_param_req_for_pch_fach(void);

/*===========================================================================

FUNCTION RRC_CSP_GET_CURR_CELL_ACCESS_STATUS

DESCRIPTION

This function returns the current cell access status of CSP

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
extern mm_as_cell_access_status_T tdsrrccsp_get_curr_cell_access_status
(
  rrc_cn_domain_identity_e_type  cn_domain_id
);

/*====================================================================
FUNCTION: RRCCSP_FILL_NAS_COMMON_INFO

DESCRIPTION:
  This function constructs fills NAS common info which is then sent to
  NAS in service_cnf and service_ind commands. 
  
DEPENDENCIES:
  None

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type tdsrrccsp_fill_nas_common_info(
  tdsrrc_SysInfoType1  *sib1_ptr,
  rrc_gsm_map_nas_sys_info_type  *nas_common_info_ptr
); 


/*====================================================================
FUNCTION: RRCCSP_FILL_NAS_CN_SPECIFIC_INFO

DESCRIPTION:
  This function fills NAS CN common info which is then sent to
  NAS in service_cnf and service_ind commands. 

DEPENDENCIES:
  None

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type tdsrrccsp_fill_nas_cn_specific_info(
  tdsrrc_CN_DomainSysInfo *     asn1_cn_domain_sys_info_ptr,
  rrc_cn_domain_sys_info_type  *rrc_cn_domain_sys_info_ptr
); 


/*====================================================================================

FUNCTION          RRC_CHECK_IF_DSAC_IS_PRESENT

DESCRIPTION     This function is to check if DSAC info is present in SIB3 (or) not

DEPENDENCIES

RETURN VALUE
                Returns value TRUE   if DSAC information is present in SIB3
                Returns value FALSE value if DSAC information is not present in SIB3
SIDE EFFECTS
                  None
======================================================================================*/
boolean  tdsrrccsp_check_if_dsac_is_present(  tdsrrc_SysInfoType3   *sib3_ptr);



/*====================================================================
FUNCTION: RRCCSP_SEND_CONN_MODE_OOS_DS_REQ

DESCRIPTION:
  This function constructs the TDSRRC_CONN_MODE_DEEP_SLEEP_REQ primitive and sends it
  to LLC. This function is called when UE needs to be put in deep sleep in
  RRC Connected Mode.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
====================================================================*/
void tdsrrccsp_send_conn_mode_oos_ds_req (void);

/*====================================================================
FUNCTION: RRCCSP_SEND_CONN_MODE_OOS_DS_WAKEUP_REQ

DESCRIPTION:
  This function constructs the RRCCSP_SEND_CONN_MODE_OOS_DS_WAKEUP_REQ primitive and sends it
  to LLC. This function is called when UE needs to be woken up from deep sleep in
  RRC Connected Mode.

DEPENDENCIES:
  None

RETURN VALUE:
  uecomdef_status_e_type.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type tdsrrccsp_send_conn_mode_oos_ds_wakeup_req (void);



/*====================================================================
FUNCTION: RRCCSP_SKIP_SIBS_FOR_FPLMN

DESCRIPTION:
         This function checks whether SIB procedure needs to read SIBs or not after reading MIB.
         This function will be used only by SIB module to avoid extra time consumed during SIB
         reading if (all shared) PLMN(s) (are) is forbidden.
         CSP will check for forbidden status of the PLMN only when n/w sel mode is 
         automatic and acq_type is SPECIFIC and Cell SELECTION is in progress.

DEPENDENCIES:
         None

RETURN VALUE:
         TRUE: skip reading SIBS if the passed PLMN is FPLMN
         FALSE: otherwise

SIDE EFFECTS:
         None.
====================================================================*/
boolean tdsrrccsp_skip_sibs_for_fplmn
(
  rrc_plmn_identity_type plmn_id
  , tdsrrc_MasterInformationBlock *mib_ptr
);

/*===========================================================================

FUNCTION          RRCCSP_IS_CAMPED_CELL_NETWORK_SHARED

DESCRIPTION       Used to find out whether the current camped cell is a part of shared network
                  
DEPENDENCIES      None

RETURN VALUE      TRUE: Camped cell is a part of shared network
                          FALSE: Camped cell belongs to a dedicated network

SIDE EFFECTS      None
===========================================================================*/
boolean tdsrrccsp_is_camped_cell_network_shared
(
  void
);

/*===========================================================================

FUNCTION          RRCCSP_IS_MIB_PLMN_VALID

DESCRIPTION       If network is shared finds out the validity of the common PLMN given in the mib.
                  
DEPENDENCIES      None

RETURN VALUE      TRUE: common PLMN is valid.
                          FALSE: common PLMN is not valid and therefore can not be considered for PLMN selection

SIDE EFFECTS      None
===========================================================================*/
boolean tdsrrccsp_is_mib_plmn_valid(tdsrrc_MasterInformationBlock *mib_ptr);

/*===========================================================================

FUNCTION          RRCCSP_CONSTRUCT_MULTIPLE_PLMN_LIST_FROM_MIB

DESCRIPTION       Constructs the shared PLMN list given the Mib
                  
DEPENDENCIES      None

RETURN VALUE      number of shared PLMNs

SIDE EFFECTS      None
===========================================================================*/
uint8 tdsrrccsp_construct_multiple_plmn_list_from_mib(
                                                   tdsrrc_MasterInformationBlock *mib_ptr,
                                                   rrc_plmn_identity_type* shared_PLMN_list);

/*===========================================================================

FUNCTION          RRCCSP_PRIORITIZE_THE_LIST_OF_SHARED_PLMNS

DESCRIPTION       Used to prioritize a PLMN among the available PLMNs based on the current scan type so that
                  this PLMN is considered first for PLMN selection if it is part of the list of shared PLMNs.
                  
DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void tdsrrccsp_prioritize_the_list_of_shared_plmns(void);

/*===========================================================================

FUNCTION          RRCCSP_FIND_SHARED_PLMNS

DESCRIPTION       Finds out the list of shared PLMNs and fills in the nw_db_entry_ptr.
                  
DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void tdsrrccsp_find_shared_plmns(
                              tdsrrc_MasterInformationBlock* mib_ptr,
                              rrc_plmn_identity_type plmn_id,
                              tdsrrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
                              );

/*===========================================================================

FUNCTION TDSRRCCSP_CHECK_PLMN_MATCH

DESCRIPTION

  This function checks the PLMN ID given as an arguement is 
  matching with the NAS requested PLMN or equivalent PLMN.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  boolean

SIDE EFFECTS
===========================================================================*/
boolean tdsrrccsp_check_plmn_match( 
                            rrc_plmn_identity_type plmn_id,
                            tdsrrc_MasterInformationBlock *mib_ptr
                            );

/*===========================================================================

FUNCTION          RRCCSP_GET_CELL_STATUS

DESCRIPTION       Finds out the suitability of the cell for camping based on the cell bar status, requesed PLMN etc.
                  
DEPENDENCIES      None

RETURN VALUE      
                  TDSRRC_CSP_CELL_BETTER_THAN_EXPECTED: Cell exceeds expectations.
                  TDSRRC_CSP_CELL_AS_EXPECTED: Cell meets expectations.
                  TDSRRC_CSP_CELL_BELOW_EXPECTED: Cell is below expectations.

SIDE EFFECTS      None
===========================================================================*/
tdsrrc_csp_cell_actual_vs_expected_e_type tdsrrccsp_get_cell_status(
                                                              tdsrrc_csp_plmn_status_e_type* cell_plmn_status,
                                                              tdsrrc_csp_acq_entry_type* acq_entry_ptr
                                                              ,tdsrrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
                                                              );

/*===========================================================================

FUNCTION          RRCCSP_GET_CELL_STATUS_DURING_BPLMN_SEARCH

DESCRIPTION       Finds out the suitability of the cell during bplmn search based on the 
                  cell bar status, list of requesed PLMNs etc.
                  
DEPENDENCIES      None

RETURN VALUE      
                  TDSRRC_CSP_CELL_BETTER_THAN_EXPECTED: Cell exceeds expectations.
                  TDSRRC_CSP_CELL_AS_EXPECTED: Cell meets expectations.
                  TDSRRC_CSP_CELL_BELOW_EXPECTED: Cell is below expectations.

SIDE EFFECTS      None
===========================================================================*/
tdsrrc_csp_cell_actual_vs_expected_e_type tdsrrccsp_get_cell_status_during_bplmn_search(
                                                                                  tdsrrc_csp_acq_entry_type* acq_entry_ptr
                                                                                  ,tdsrrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
                                                                                  );


/*===========================================================================

FUNCTION          RRCCSP_IS_PLMN_MATCH_FOR_AUTO_BPLMN_SEARCH

DESCRIPTION       check if the sib incomplete cell's PLMN fullfill with the 
                  PLMN that MM request for auto BPLMN search
                  
DEPENDENCIES      None

RETURN VALUE      
                  TDSRRC_CSP_CELL_BETTER_THAN_EXPECTED: Cell exceeds expectations.
                  TDSRRC_CSP_CELL_AS_EXPECTED: Cell meets expectations.
                  TDSRRC_CSP_CELL_BELOW_EXPECTED: Cell is below expectations.

SIDE EFFECTS      None
===========================================================================*/
tdsrrc_csp_cell_actual_vs_expected_e_type tdsrrccsp_is_plmn_match_for_auto_bplmn_search(
                                                                                  tdsrrc_csp_acq_entry_type* acq_entry_ptr
                                                                                  ,tdsrrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
																				  );
												

												
/*===========================================================================

FUNCTION          RRCCSP_DOES_MCC_BELONG_TO_US

DESCRIPTION       This function returns TRUE if the passed PLMN belongs to US
                  otherwise it returns FALSE.
                                    
DEPENDENCIES      None

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      None
===========================================================================*/
boolean tdsrrccsp_does_mcc_belong_to_US(rrc_plmn_identity_type plmn_id);


/*===========================================================================
FUNCTION: RRCCSP_CHECK_AND_TEARDOWN_SBCCH_DURING_BPLMN

DESCRIPTION: This function is called when RRC receives DEACT_REQ / MODE_CHANGE_REQ 
             / STOP_WCDMA_MODE_REQ . If BPLMN is actively in progress and CSP is waiting for SIBs,
             it sends SBCCH release req.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void tdsrrccsp_check_and_teardown_sbcch_during_bplmn(void);


/*===========================================================================

FUNCTION          RRCCSP_SEND_GTOW_REDIRECTION_ABORT_CNF

DESCRIPTION       This function is responsible for sending
                  TDSRRC_INTERRAT_REDIRECT_ABORT_CNF to RR after getting 
                  TDSRRC_INTERRAT_REDIRECT_ABORT_REQ from RR to abort GtoW redirection.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_g2td_redirection_abort_cnf(void);

/*===========================================================================

FUNCTION          RRCCSP_SEND_GTOW_REDIRECTION_REJECT

DESCRIPTION       This function is responsible for sending
                  TDSRRC_INTERRAT_REDIRECT_REJ to RR when no TDS cell is found for 
                  GtoW redirection search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_g2td_redirection_reject(void);


/*===========================================================================

FUNCTION          RRCCSP_GET_SERVING_FREQUENCY

DESCRIPTION       This function gets the frequency of the serving cell 

DEPENDENCIES
                  None.

RETURN VALUE
                  Serving cell's frequency.

SIDE EFFECTS      None.
===========================================================================*/
tdsrrc_freq_type tdsrrccsp_get_serving_frequency(void);

/*===========================================================================

FUNCTION          RRCCSP_GET_NEIGHBOR_FREQUENCY

DESCRIPTION       This function gets the frequency of the neighbor cell UE is
                  reselecting to
DEPENDENCIES
                  None.

RETURN VALUE
                  Neighbor frequency.

SIDE EFFECTS      None.
===========================================================================*/
tdsrrc_freq_type tdsrrccsp_get_neighbor_frequency(void);

/*===========================================================================

FUNCTION          RRCCSP_GET_CURR_NAS_REQUESTED_PLMN

DESCRIPTION       This function returns the specific PLMN on which NAS has 
                  requested RRC to camp.
DEPENDENCIES
                  None.

RETURN VALUE
                  NAS requested PLMN.

SIDE EFFECTS      None.
===========================================================================*/
rrc_plmn_identity_type tdsrrccsp_get_curr_nas_requested_plmn(void);

#ifdef FEATURE_DISCONNECT_ON_UNSUPPORTED_IRAT_REDIRECTION

/*===========================================================================

FUNCTION          RRCCSP_CHECK_GSM_BANDS_SUPPORTED

DESCRIPTION       This function check if UE can support GSM bands or not.

DEPENDENCIES
                  
RETURN VALUE
                  TRUE: If UE can support GSM.
                  FALSE: Otherwise. 

SIDE EFFECTS
                  None.
                  
===========================================================================*/
boolean tdsrrccsp_check_gsm_bands_supported(void);
#endif
/*===========================================================================

FUNCTION          RRCCSP_CHECK_INITIATE_CELL_SELECTION_HANDLING

DESCRIPTION       This function is called to check if CSP is in proper substate to handle 
                         TDSRRC_INITIATE_CELL_SELECTION_IND to transition to DCH or FACH.

DEPENDENCIES      None

RETURN VALUE      TRUE/FALSE

SIDE EFFECTS      None
===========================================================================*/
boolean tdsrrccsp_check_initiate_cell_selection_handling(void);
/*===========================================================================

FUNCTION          RRC_CSP_IS_RESELECTION_CLEARED

DESCRIPTION       This function is called on reception of Chan config req for N-BCCH 
               to check if DL WEAK IND was received for this cell and reselection data has been cleared.

DEPENDENCIES      None

RETURN VALUE      TRUE : if reselection is cleared.
                  FLASE: otherwise

SIDE EFFECTS      None
===========================================================================*/
boolean tdsrrccsp_is_reselection_cleared(void);



#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
/*===========================================================================

FUNCTION          RRCCSP_CHECK_IF_CAMPED_NORMALLY

DESCRIPTION       This function checks if UE is camped normally, 

DEPENDENCIES      None

RETURN VALUE      TRUE if camped normally
                          FALSE if camped on any cell

SIDE EFFECTS      None
===========================================================================*/
boolean tdsrrccsp_check_if_camped_normally
(
  void
);
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST
/*===========================================================================

FUNCTION          RRC_EXTRACT_TDS_BAND_PREF_FROM_RAT_PRI_LIST

DESCRIPTION       This function extracts the TDS band pref from RAT PRI List

DEPENDENCIES      None

RETURN VALUE      TDS band pref

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_type tdsrrccsp_extract_tds_band_pref_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list
);

/*===========================================================================

FUNCTION          RRC_EXTRACT_GW_BAND_PREF_FROM_RAT_PRI_LIST

DESCRIPTION       This function extracts the GW band pref from RAT PRI List

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_type tdsrrccsp_extract_gw_band_pref_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list
);

/*===========================================================================

FUNCTION          RRC_EXTRACT_TDS_BAND_PREF_FROM_SERVICE_REQ

DESCRIPTION       This function extracts the TDS band pref from RAT PRI List of service_req

DEPENDENCIES      None

RETURN VALUE      TDS band pref

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_type tdsrrccsp_extract_tds_band_pref_from_service_req
(
  sys_rat_pri_list_info_s_type *rat_pri_list
);

/*===========================================================================

FUNCTION          RRC_EXTRACT_GW_BAND_PREF_FROM_SERVICE_REQ

DESCRIPTION       This function extracts the GW band pref from RAT PRI List of service_req

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_type tdsrrccsp_extract_gw_band_pref_from_service_req
(
  sys_rat_pri_list_info_s_type *rat_pri_list
);

/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_GW_BAND_PREF_AFTER_SUCC_CAMPING

DESCRIPTION       This function updates mode and band preference after successful camping

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void tdsrrccsp_update_mode_band_pref_after_succ_camping
(
  void
);

/*===========================================================================

FUNCTION          RRC_CSP_STORE_RAT_PRI_LIST_TO_INT_DATA

DESCRIPTION       This function stores given rat_pri_list in CSP INT DATA struct  tdsrrc_csp_int_data.rat_pri_list_info.
                           Depending on featurization, it may remove LTE FDD bands if provided.

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void tdsrrccsp_store_rat_pri_list_to_int_data
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info
);

/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_RAT_PRI_LIST_INFO

DESCRIPTION       This function updates rat_pri_list_info in CSP INT DATA

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
void tdsrrccsp_update_rat_pri_list_info
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info
);

/*===========================================================================

FUNCTION          RRC_CSP_GET_RAT_PRI_LIST_INFO

DESCRIPTION       This function fills the arguments with rat_pri_list_info in CSP INT DATA

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
void tdsrrccsp_get_rat_pri_list_info
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info
);
#endif

#if defined (FEATURE_TDSCDMA_TO_LTE) || defined (FEATURE_LTE_TO_TDSCDMA)
/*===========================================================================

FUNCTION          RRC_EXTRACT_LTE_BAND_PREF_FROM_RAT_PRI_LIST

DESCRIPTION       This function extracts the LTE band pref from RAT PRI List

DEPENDENCIES      None

RETURN VALUE      LTE band pref

SIDE EFFECTS      None
===========================================================================*/
#ifdef LTE_BAND_NUM//xlong 0323
sys_lte_band_mask_e_type tdsrrc_extract_lte_band_pref_from_rat_pri_list
#else
sys_band_mask_type tdsrrc_extract_lte_band_pref_from_rat_pri_list
#endif
(
  sys_rat_pri_list_info_s_type *rat_pri_list,
  boolean                       tdd_band_only
);

#ifdef FEATURE_TDSCDMA_TO_LTE

/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_BPLMN_ABORT_REQ

DESCRIPTION       This function is responsible for sending
                  LTE_RRC_TDSCDMA_ABORT_PLMN_SRCH_REQ to LTE RRC for aborting
                  TD2G BPLMN search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_bplmn_abort_req
(
  void
);

/*===========================================================================

FUNCTION          tdsrrccsp_send_lte_interrat_bplmn_suspend_ind

DESCRIPTION       This function is responsible for sending
                  LTE_RRC_TDSCDMA_SUSPEND_PLMN_SRCH_IND to LTE RRC for suspending
                  TD2L BPLMN search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_bplmn_suspend_ind
(
  void
);

/*===========================================================================

FUNCTION          rrccsp_get_lte_band_pref

DESCRIPTION       This function returns LTE band preference

DEPENDENCIES
                  None.

RETURN VALUE
                  LTE band preference

SIDE EFFECTS      None.
===========================================================================*/
#ifdef LTE_BAND_NUM
sys_lte_band_mask_e_type tdsrrccsp_get_lte_band_pref //xlong 0323
(
  void
);
#else
sys_band_mask_type tdsrrccsp_get_lte_band_pref
(
  void
);
#endif
/*===========================================================================

FUNCTION          rrccsp_get_wg_band_pref

DESCRIPTION       This function populates TDSCDMA & GSM band preference

DEPENDENCIES
                  None.

RETURN VALUE
                  None

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_get_tg_band_pref
(
  sys_band_mask_type *tds_band_pref,
  sys_band_mask_type *gsm_band_pref
);
#endif

#ifdef FEATURE_LTE_TO_TDSCDMA
/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_RESELECTION_ABORT_RSP

DESCRIPTION       This function is responsible for sending
                  WCDMA_RRC_LTE_ABORT_RESEL_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_reselection_abort_rsp
(
  void
);

/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_PLMN_SRCH_ABORT_RSP

DESCRIPTION       This function is responsible for sending
                  WCDMA_RRC_LTE_ABORT_PLMN_SRCH_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_plmn_srch_abort_rsp
(
  void
);

/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_REDIRECTION_ABORT_RSP

DESCRIPTION       This function is responsible for sending
                  WCDMA_RRC_LTE_ABORT_REDIR_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_redirection_abort_rsp
(
  void
);

/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_CGI_ABORT_RSP

DESCRIPTION       This function is responsible for sending
                  WCDMA_RRC_LTE_ABORT_CGI_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_cgi_abort_rsp
(
  void
);

/*===========================================================================

FUNCTION          TDSRRCCSP_SEND_LTE_INTERRAT_CGI_RSP

DESCRIPTION       This function is responsible for sending
                  TDSCDMA_RRC_LTE_CGI_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_cgi_rsp
(
  void
);

/*===========================================================================

FUNCTION          TDSRRCCSP_SEND_LTE_INTERRAT_RESELECTION_FAILED_RSP

DESCRIPTION       This function is responsible for sending
                  TDSCDMA_RRC_LTE_RESEL_FAILED_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_reselection_failed_rsp
(
  void
);

/*===========================================================================

FUNCTION          TDSRRCCSP_SEND_LTE_INTERRAT_REDIRECTION_FAILED_RSP

DESCRIPTION       This function is responsible for sending
                  TDSCDMA_RRC_LTE_REDIR_FAILED_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_redirection_failed_rsp
(
  void
);

/*===========================================================================

FUNCTION          tdsrrccsp_send_lte_interrat_info_ind

DESCRIPTION       This function is responsible for sending
                  TDSCDMA_RRC_LTE_IRAT_INFO_IND to LTE RRC after successful
                  L2T handover, redirection or reselection.

DEPENDENCIES
                  This function is called to send the message after 
                  TDSRRC_ACTIVATION_RSP is received.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_interrat_info_ind
(
  void
);


#ifdef FEATURE_TDSCDMA_SUPPORT_LTE_RLF_REPORT_ENHANCEMENT
/*===========================================================================

FUNCTION          TDSRRCCSP_SEND_LTE_UTRA_CELL_INFO_RSP

DESCRIPTION       This function is responsible for sending
                  TDSCDMA_RRC_LTE_UTRA_CELL_INFO_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_lte_utra_cell_info_rsp
(
  boolean success
);
#endif
#endif

/*===========================================================================

FUNCTION          RRCCSP_DERIVE_FREQ_FROM_UARFCN

DESCRIPTION       This function derivces 

DEPENDENCIES      None

RETURN VALUE      Frequency in multiple of 100KHZ

SIDE EFFECTS      None
===========================================================================*/
uint16 tdsrrccsp_derive_freq_from_uarfcn
(
  tdsrrc_freq_type freq,
  tdsrrc_csp_band_class_type band_class
);
#endif

/*===========================================================================

FUNCTION          RRCCSP_SEND_IRAT_BPLMN_ABORT_REQ

DESCRIPTION       This function is responsible for sending ABORT_REQ to the RAT by checking the BPLMN RAT TYPE

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_irat_bplmn_abort_req
(
  void
);

/*===========================================================================

FUNCTION          tdsrrccsp_send_irat_bplmn_suspend_ind

DESCRIPTION       This function is responsible for sending SUSPEND_IND to the 
                  RAT by checking the BPLMN RAT TYPE

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void tdsrrccsp_send_irat_bplmn_suspend_ind
(
  void
);

#ifdef FEATURE_WRLF_SYSTEM_SEL
/*===========================================================================

FUNCTION          RRC_CSP_CHECK_N_SEND_SUITABLE_SRCH_END_IND

DESCRIPTION       This function checks and sends suitable_srch_end_ind
                  to NAS.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void tdsrrccsp_check_n_send_suitable_srch_end_ind
(
  void
);

/*===========================================================================

FUNCTION          RRC_CSP_SEND_DISABLE_BPLMN_IND

DESCRIPTION       This function sends disable_bplmn_ind to NAS.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void tdsrrccsp_send_disable_bplmn_ind(boolean status);

/*===========================================================================
FUNCTION: RRCCSP_CHECK_AND_TEARDOWN_SBCCH_DURING_WRLF

DESCRIPTION: This function is called when RRC receives DEACT_REQ / MODE_CHANGE_REQ 
             / STOP_WCDMA_MODE_REQ . If WRLF in progress and CSP is waiting for SIBs,
             it sends SBCCH release req.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void tdsrrccsp_check_and_teardown_sbcch_during_wrlf
(
  void
);
#endif

/*===========================================================================

FUNCTION          tdsrrccsp_set_est_req_for_emerg_call

DESCRIPTION       This function sets establishment request flag for emergency call

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

void tdsrrccsp_set_est_req_for_emerg_call(void);

/*===========================================================================

FUNCTION          tdsrrccsp_get_lte_resel_allow_status

DESCRIPTION       This function will return lte_resel_allow_status

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

boolean tdsrrccsp_get_lte_resel_allow_status(void);

/*===========================================================================

FUNCTION          tdsrrccsp_read_nv_cmcc_ehplmn_list

DESCRIPTION       This function will read CMCC EHPLMNs from NV/EFS. It is needed to check if a PLMN ID is supported by
                           the TD-SCDMA carrier

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

void tdsrrccsp_read_nv_cmcc_ehplmn_list(void);

#ifdef FEATURE_LTE_TO_TDSCDMA
/*===========================================================================

FUNCTION        tdsrrccsp_fill_cgi_bplmn_tmr_exp

DESCRIPTION     This function will fill CGI info from MIB and SIB3 if available 
                upon BPLMN CGI timer expiry

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrccsp_fill_cgi_bplmn_tmr_exp(void);

/*===========================================================================
FUNCTION          RRCCSP_CHECK_CGI_SCAN

DESCRIPTION       This function checks whether cgi scan is enabled

DEPENDENCIES      None

RETURN VALUE      Next CSP Substate

SIDE EFFECTS      None
===========================================================================*/
boolean tdsrrccsp_check_cgi_scan(void);
#endif

/*===========================================================================

FUNCTION          tdsrrccsp_initial_suitability_check

DESCRIPTION       This function will check initial suitability of the cell

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

void tdsrrccsp_initial_suitability_check
(
  tdsrrc_MasterInformationBlock *mib_ptr, 
  tdsrrc_SysInfoType3 *sib3_ptr,
  rrc_plmn_identity_type mib_plmn_id, 
  tdsrrc_sib_status_e_type *status
);

/*===========================================================================

FUNCTION          TDSRRCCSP_GET_BAND_E_LOWER_BOUND

DESCRIPTION       This function get the floor limit of band E considering
                  NV setting.
                  Three NV bits to control the range of band E to search:
                  1. If TDSCHECK_OPTIONAL_FEATURE_SUBBAND_E1_SET is set,
                     then the lower bound of Band E is 2320MHz;
                  2. else if TDSCHECK_OPTIONAL_FEATURE_SUBBAND_E2_SET is set,
                     then the lower bound of Band E is 2330MHz;
                  3. else if TDSCHECK_OPTIONAL_FEATURE_SUBBAND_E3_SET is set,
                     then the lower bound of Band E is 2350MHz;
                  4. else search whole band, so start from 2300MHz;

RETURN VALUE
                  floor limit frequency
SIDE EFFECTS
                  None
===========================================================================*/
uint16 tdsrrccsp_get_band_e_lower_bound(void);

/*===========================================================================

FUNCTION          TDSRRCCSP_GET_BAND_E_UPPER_BOUND

DESCRIPTION       This function get the upper limit of band E considering
                  NV setting.
                  Three NV bits to control the range of band E to search:
                  1. If TDSCHECK_OPTIONAL_FEATURE_SUBBAND_E3_SET is set,
                     then the upper bound of Band E is 2370MHz;
                  2. else if TDSCHECK_OPTIONAL_FEATURE_SUBBAND_E2_SET is set,
                     then the upper bound of Band E is 2350MHz;
                  3. else if TDSCHECK_OPTIONAL_FEATURE_SUBBAND_E1_SET is set,
                     then the upper bound of Band E is 2330MHz;
                  4. else scan whole band, so end at 2400MHz;

RETURN VALUE
                  upper limit frequency
SIDE EFFECTS
                  None
===========================================================================*/
uint16 tdsrrccsp_get_band_e_upper_bound(void);

#if defined (FEATURE_TDSCDMA_TO_LTE) || defined (FEATURE_LTE_TO_TDSCDMA)
/*===========================================================================

FUNCTION          RRCCSP_UPDATE_IRAT_DETECTED_FREQUENCIES

DESCRIPTION       This function updates the detected frequencies in 
                  tdsrrc_csp_int_data with detected frequency list
                  received from other RAT

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void tdsrrccsp_update_irat_detected_frequencies
(
  detected_frequency_list_type* detected_frequency_list
);
#endif

/*===========================================================================

FUNCTION          tdsrrccsp_set_band_f_search_pattern

DESCRIPTION       This function is used to set band F search pattern according
                  to NV setting. And if it's not specified in NV, then use
                  default search pattern (lower band shallow+deep and upper
                  band shallow) instead.

DEPENDENCIES      It's to invoke one time after read NV66024.

RETURN VALUE      NONE
                  
SIDE EFFECTS      NONE

===========================================================================*/
void tdsrrccsp_set_band_f_search_pattern(void);

/*===========================================================================

FUNCTION          tdsrrccsp_set_band_a_search_pattern

DESCRIPTION       This function is used to set band A search pattern according
                  to NV setting. And if it's not specified in NV, then use
                  default search pattern (shallow+deep) instead.

DEPENDENCIES      It's to invoke one time after read NV66024.

RETURN VALUE      NONE
                  
SIDE EFFECTS      NONE

===========================================================================*/
void tdsrrccsp_set_band_a_search_pattern(void);

/*===========================================================================

FUNCTION          tdsrrccsp_set_band_e_search_pattern

DESCRIPTION       This function is used to set band E search pattern according
                  to NV setting. And if it's not specified in NV, then use
                  default search pattern (shallow) instead.

DEPENDENCIES      It's to invoke one time after read NV66024.

RETURN VALUE      NONE
                  
SIDE EFFECTS      NONE

===========================================================================*/
void tdsrrccsp_set_band_e_search_pattern(void);


#ifdef FEATURE_TDSCDMA_DSDS
/*===========================================================================

FUNCTION          tdsrrccsp_reset_wait_l1_rel_lock

DESCRIPTION       To reset csp_waiting_for_wl1_to_release_lock flag

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

void tdsrrccsp_reset_wait_l1_rel_lock(void);
#endif

/*===========================================================================

FUNCTION        RRC_CSP_BPLMN_UPDATE_SELECT_DATA_FOR_REATTEMPT

DESCRIPTION     This function will update the indices of the ACQ DB/rrc_csp_curr_
                select_data so that search is attempted on the currently
                populated entry

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void tdsrrccsp_bplmn_update_select_data_for_reattempt(void);

#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
/*===========================================================================

FUNCTION          tdsrrccsp_is_drx_expiry_ind_pending

DESCRIPTION       This function returns a value that indicates if 
                  TDSL1CPHY_DRX_TIMER_EXPIRY_IND send to tl1 or not

DEPENDENCIES
                  None

RETURN VALUE
                  FALSE: not used or Send to l1 already by csp
                  TRUE:  Wait LLC to send acq_req or freq_scan to l1 

SIDE EFFECTS
                  None

===========================================================================*/
boolean  tdsrrccsp_is_drx_expiry_ind_pending(void);
/*===========================================================================

FUNCTION          tdsrrccsp_reset_drx_expiry_ind_pending_flag

DESCRIPTION       This function reset bplmn spltd flag

DEPENDENCIES
                  None

RETURN VALUE

SIDE EFFECTS
                  None

===========================================================================*/
void  tdsrrccsp_reset_drx_expiry_ind_pending_flag(void);
#endif

/*===========================================================================
  FUNCTION        tdsrr_send_rrc_message_to_rr_internal

  DESCRIPTION     This function will add AS ID to the messages sent from RRC to RR
                  and calls the function which actually sends the message

  DEPENDENCIES
                None

  RETURN VALUE
                None

  SIDE EFFECTS
                None
===========================================================================*/
void tdsrr_send_rrc_message_to_rr_internal(
  void *            message_ptr,
  tdsrr_rrc_cmd_e_type rrc_cmd_id,
  word              length
);

/*===========================================================================

FUNCTION        TDSRR_GET_ARFCN_BAND_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
sys_band_T tdsrr_get_arfcn_band_internal(
  uint16                          arfcn,
  tdsrr_frequence_band_e_type band_ind
  );

/*===========================================================================

FUNCTION        TDSRR_SET_BAND_PREF_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void tdsrr_set_band_pref_internal(sys_band_mask_type     band_pref);

/*===========================================================================

FUNCTION        TDSRR_SET_BST_BAND_PREF_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void tdsrr_set_bst_band_pref_internal(sys_band_mask_type band_pref);

/*===========================================================================

FUNCTION        TDSRR_GET_RR_CURR_CAMPED_PLMN_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
PLMN_id_T  tdsrr_get_rr_curr_camped_plmn_internal(void);

/*===========================================================================

FUNCTION        TDSRR_ACQ_DB_SAVE_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void tdsrr_acq_db_save_internal(void);
#endif /* TDSRRCCSP_H */
