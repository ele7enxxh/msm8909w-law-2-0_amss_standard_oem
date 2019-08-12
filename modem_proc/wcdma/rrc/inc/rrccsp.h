#ifndef RRCCSP_H
#define RRCCSP_H

/*===========================================================================
                        R R C    C S P

DESCRIPTION
    This file contains definitions needed for Cell Selection Procedure.   
   
      
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2009
Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccsp.h_v   1.8   16 May 2002 09:29:04   bumatt  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrccsp.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   sn      Changes for LTE U feature
01/12/16   sn      Changes for FR30881
08/07/14   sr      Made changes to send dummy service ind and move to idle SIB DB 
                   is not available for the current camped cell after WTOG CCO fail
06/23/14   sr      Made changes to unbar frequency info given in IE "redirection info" for 
                   connection reject and connection release OTA
09/02/13   sr      Made changes for BST optimizations
02/20/14   bc      Made changes to abort sib reading after MIB if the found VPLMN is 
                   already added to the list to be sent to NAS.
11/07/13   bc      Made changes to abort sib reading if we find that the cell is barred 
                   or la is forbidden.
09/16/13   sn      G2W TA feature changes
11/18/13   bc      Checked in REL 10 spec crs changes
11/16/13   sr      WRRC support for LW DSDS
10/07/13   rmn     Changes for Feature: RAT Search Order for FDD/TDD LTE
09/13/13   sn      Changes to include ECNO in CGI response to LTE
08/14/13   vi      Added handling for subsequent DEACT_REQ from NAS even if Lower Layers are deactivated
08/15/13   sn      reset wait for WCDMA L1 release lock flag when BPLMN search is being 
                   suspended or aborted
07/31/13   vi      Mainlining the MODEM_STATISTICS feature
06/19/13   sn      Changes to include CSG ID and RSCP in CGI response
06/06/13   sr      Made changes to abort SIB reading when MIB PLMN doesn't match with the 
                   NAS requested PLMN for XTOW redirection
04/12/13   gv      Made changes to check for the presence of primary plmn id before rejecting
                   the IDT because of PLMN mismatch
12/07/12   sn      CSG feature changes
11/30/12   ad      Made changes to remvoe the dependancy on  rfm_band_supports_dual_carrier
11/27/12   gv      Fixed compiler warnings when DSDS is enabled
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
09/10/12   gv      Mainline the changes under feature FEATURE_WCDMA_BPLMN_SUSPEND_RESUME
08/31/12   gv      Mainline the changes under feature FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL
08/17/12   gv      Fixed compilation issue
08/17/12   gv      Added changes to support suspension of intra/inter-W BPLMN search on entering DCH/FACH
                   and resumption of the same on entering PCH/IDLE
02/09/12   sn      Changes to send partial CGI info if MIB,SIB3 is read but not SIB1 
01/12/12   gv      Added code to support FEATURE_PPAC
12/12/11   mn      Made changes to process ps hard abort request received when waiting for lock
11/21/11   gv      Made changes to not include gsdi header file when FEATURE_UIM_DISABLE_GSDI_INTERFACE
                   is defined
11/18/11   mn      Made changes to disable reselection to LTE after attach reject from network
                   with cause 14.
09/28/11   gv      Moved here the macros for WCDMA frequency bands
09/12/11   gv      Renamed feature flag FEATURE_WCDMA_RRC_EMERG_CALL_SUPPORT
                   to FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL
09/12/11   gv      Added prototype for rrccsp_set_est_req_for_emerg_call
                   under feature FEATURE_WCDMA_RRC_EMERG_CALL_SUPPORT
05/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.97.50
05/05/11   vg      added channal locking feature support under rrccsp_channel_locking_nv_status
                   condition
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/25/12   as      Made changes to trigger cell update/drx req when UE recived OTA with 
                   redirection info and cell reselection in progress
04/20/11   vg      added support for mdsp halt recovery
04/08/11   sks     Featurised code for checking RF band support for DC.
04/06/11   sks     Added a function to check RF band support for dual carrier.
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/21/11   rl      Added code to support L2W CGI 
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/08/11   su      Added code to support enabling and disabling 
                   FEATURE_UEUT_IGNORE_CELL_BAR_RESERVE_STATUS using NV item.
03/04/11   sks     Fixed compilation warnings.
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/01/10   rm      Moved prototypes of rrc_register_serving_cell_change_ind_cb and
                   rrc_deregister_serving_cell_change_ind_cb to rrcmmif.h from rrccsp.h
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/22/10   sks     Added support for handling EM call during WRLF.
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/05/10   rm      Fixed compiler warnings
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/14/10   rm      Added code to support BST search on WCDMA
03/15/10   rm      Added code to support FEATURE_RAT_PRIORITY_LIST for W<->G IRAT
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/10/10   dm      Added prototype for rrc_csp_is_reselection_cleared.
11/02/09   rm      Added support for FEATURE_MODEM_STATISTICS
09/25/09   sks     Provided API rrccsp_check_initiate_cell_selection_handling()
                   to give info if CSP will handle CELL SELECTION REQ
07/03/09   rm      Modified prototype of rrccsp_is_current_band
05/08/09   ss      Updated Copyright Information
02/25/09   rm      Added code to send RRC_RESELECT_TO_GAN_REJ when reselection to GAN
                   is in progress and NAS sends STOP_WCDMA_MODE_REQ.
01/08/09   sks     Changed MAX_NUM_OF_REG_CB to 3, to allow more functions
                   that can be registerd to get cell id change notification.
12/26/08   sk      Declared rrccsp_check_gsm_bands_supported to check if GSM is supported
09/01/08   sk      Declared rrccsp_get_serving_frequency, rrccsp_get_neighbor_frequency, 
                   and rrccsp_get_curr_nas_requested_plmn
06/30/08   ns      Made the necessary changes to enable the functionality of the feature 
                   FEATURE_NETWORK_SHARING only when the GCF NV is set
06/13/08   gkg     Added Code for notifying registered entities if there's change
                   in Cell_Id through OTAs in Cell_DCH state.
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
04/10/08   pk      Teardown BCCH before sending SLEEP_IND to L1, if RRC gets 
                   DEACT_REQ/STOP_MODE_REQ/MODE_CHANGE_REQ when BPLMN search is actively
                   in progress and CSP is waiting for SIBs.
03/07/08   vr/da   Added rrc_csp_get_bplmn_nw_sel_mode() function prototype that is needed
                   for MBMS vs BPLMN prioritization. 
12/05/07   pk      Changed MAX_NUM_OF_REG_CB to 2, to allow ATCOP also to register for Cell ID
                   and PSC.
11/21/07   vm      Added support for FEATURE_UEUT_IGNORE_CELL_BAR_RESERVE_STATUS
10/23/07   pk      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE_NETWORK_SHARING.
10/12/07   vm      Added prototype of new fn rrccsp_does_mcc_belong_to_US().
10/03/07   pk      Added rrccsp_get_current_plmn_cell_id() which returns the 
                   current PLMN ID and Cell ID
06/05/07   pk      Added support for checking cell access status based on the domain.
03/29/07   segu    Added prototype of the function rrccsp_skip_sibs_for_fplmn()
                   under feature flag FEATURE SKIP SIBS FOR FPLMN
09/15/06   vm      Added support for the feature FEATURE_UI_RF_ENABLED_BANDS_SUPPORT.
                   Added prototype of a new function rrccsp_is_band_class_enabled_in_ui_rf().
08/25/06   vm      Added support for FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT.
06/08/06   vm      Updated prototype of existing function rrc_get_frequency_band()
                   and added prototype of new function rrccsp_is_sib_mcc_and_current_band_comb_valid().
05/24/06   segu    Added prototype of the function rrc_csp_get_curr_cell_access_status()
                   that returns the current cell access status of CSP 
04/24/06   vm      Added prototype for new function rrccsp_is_current_band().
03/10/06   sgk     Added prototype for rrc_csp_send_cell_param_req_for_pch_fach
01/30/06   vm      Added prototype for new function rrccsp_update_wtow_manual_bplmn_list_with_rplmn().
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
01/23/06   kp      Added support for Transaction I.D in BPLMN Abort feature 
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search.
10/03/05   da      Moved cell id change notification callback typedef to rrcmmif.h 
09/21/05   da      Modified prototype of funcs rrc_register_serving_cell_change_ind_cb()
                   and rrc_deregister_serving_cell_change_ind_cb() as they have changed
                   and return boolean value.
09/20/05   da      Added callback function type for cell id change notification and
                   defined MAX_NUM_OF_REG_CB as 1 which is the max number of functions
                   that can be registerd to get cell id change notification.
07/20/05   vr      Added prototype for rrc_csp_send_service_ind().This
                   function can be used to send service_ind from other procedures.
07/15/05   vm      Added support for not updating band information for 
                   inter-RAT handover, CCO, and reselections.
                   Function rrccsp_fill_info_for_rr() now takes only 1 argument.
06/21/05   vm      Added support for trying acquisition on the best cell in the active set on
                   DCH->Idle transition. Added prototype for new function rrc_csp_get_aset_scr_pn().
06/06/05   da      Removed Home zone related functions.  new functionality has moved to
                   rrcscmgr.
06/02/05   vm      Added support for inter-RAT abort handling:
                   added prototype for new function rrccsp_send_interrat_reselection_abort_cnf().
05/27/05   vm      The prototype of the function rrccsp_send_cphy_cell_bar_req() has changed 
                   so that T-barred timer value is now sent as uint32 instead of an enum type.
05/05/05   vm/bu   Added support for T-barred Feature. Added new enum types 
                   rrc_csp_cell_bar_info_e_type and rrc_csp_T_barred_e_type.
                   Also added prototype for new function rrccsp_send_cphy_cell_bar_req().
03/31/05   bu      Added prototype for rrccsp_is_ue_in_true_limited_service().
12/06/04   vm      Added prototype for the function rrccsp_send_bplmn_abort_cnf_to_rr.
12/02/04   da      Added prototype for rrc_register_serving_cell_change_ind_cb,
                   rrc_deregister_serving_cell_change_ind_cb and 
                   rrc_notify_cell_id_change_cb_func
11/24/04   vm      Added prototype for rrccsp_is_in_bplmn_list.
11/22/04   vm      Added prototype for rrccsp_send_bplmn_search_cnf_to_rr.
09/17/04   kc      Added prototype for rrccsp_get_supported_band_list to get the 
                   list of bands UE supports currently.
09/01/04   bu      Removed prototype for rrccsp_update_band_class_in_acq_db(). It's
                   now moved to rrccspdb.h.
08/31/04   bu      Added the prototypes for rrc_get_frequency_band() and
                   rrccsp_update_band_class_in_acq_db.
08/25/04   bu      Added prototype for rrccsp_update_acq_db().
08/18/04   bu      Included rrccspdb.h.
08/13/04   bu      Added prototypes for rrccsp_update_band_preference() and
                   rrccsp_is_supported_band().
06/14/04   bu      Added prototype for rrccsp_set_ue_in_deep_sleep().
06/01/04   bu      Added prototype for rrccsp_fill_info_for_rr().
12/16/03   bu      Added prototype for rrccsp_is_inter_frequency_cell_resel_in_progress.
10/29/03   bu      Changed the prototype for rrccsp_get_neighbor_pccpch_parms to
                   include the frequency.
04/10/04   vn      Removed function rrcmeas_save_intra_freq_meas(..) as it 
                   does not belong here. Added declaration for function
                   rrccsp_init_measurements_in_l1_on_cphy_idle.
03/28/03   vk      Declared function rrcmeas_save_intra_freq_meas(..)
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to 
                   INTERRAT_HANDOVER_GTOW
02/14/03   bu      Add prototype for rrccsp_update_plmn_info_for_gtow_handover()
                   if FEATURE_INTERRAT_HANDOVER_GTOW_RRC is defined.
10/01/02   vn      Added prototype for rrccsp_is_selected_cell_utran_directed()
                   and rrccsp_is_state_transition_triggered_by_reconfig_msg().
05/16/02   bu      Added prototype for rrccsp_get_neighbor_pccpch_parms().
06/06/01   vn      Added prototype for rrccsp_get_serving_pccpch_parms().
01/08/01   vn      Moved rrc_csp_substate_e_type to rrccsp.c.
01/19/00   vn      Added additional states SUIT_CELL_SEL_WAIT_CAMP and 
                   ANY_CELL_SEL_WAIT_CAMP while removing states related to 
                   PCH setup as per the latest RRC LLC functionality.
12/07/00   vn      Updated to work with rrc_cmd_type which is a uniform cmd type
                   for cmds coming from all sources.
12/01/00   kmp     Removed compiler error.
12/01/00   vn      Migration to REX++
10/31/00   vn      Modified for compilation errors.
10/26/00   vn      Created file.

===========================================================================*/

/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/


#include "wcdma_variation.h"
#include "comdef.h"
#include "rrccmd_v.h"
#include "rrccspdb.h"
#include "rrcueci.h"
#include "rrcmmif.h"
#include "rrc_rr_types.h"
#include "sys.h"

#ifndef FEATURE_UIM_DISABLE_GSDI_INTERFACE
#include "gsdi_exp.h"
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* The following two enum types rrc_csp_cell_bar_info_e_type and 
 * rrc_csp_T_barred_e_type are declared for T barred support
 */

/* This is used to inform L1 if the GSM cell is barred, or WCDMA 
 * cell is barred, or WCDMA frequency is barred, or the active
 * set is barred
 */
typedef enum
{
  RRC_CSP_GSM_CELL_BARRED,
  RRC_CSP_WCDMA_PSC_BARRED,
  RRC_CSP_WCDMA_FREQ_BARRED,
  RRC_CSP_WCDMA_ASET_BARRED
#ifdef FEATURE_WCDMA_TO_LTE
  ,RRC_CSP_LTE_CELL_BARRED
  ,RRC_CSP_LTE_FREQ_BARRED
#endif
  ,RRC_CSP_WCDMA_FREQ_UNBAR
}rrc_csp_cell_bar_info_e_type;


/* This is used to inform L1 of the T-barred timer value */
typedef enum
{
  RRC_T_BARRED_S10,
  RRC_T_BARRED_S20,
  RRC_T_BARRED_S40,
  RRC_T_BARRED_S80,
  RRC_T_BARRED_S160,
  RRC_T_BARRED_S320,
  RRC_T_BARRED_S640,
  RRC_T_BARRED_S1280
}rrc_csp_T_barred_e_type;

extern boolean lac_in_ota_present;
extern uint8 lac_in_ota[RRC_MAX_GSMMAP_LAC_LENGTH];

/* defines the max num of callbacks that can register for 
 * cell ID change
 */
#define MAX_NUM_OF_REG_CB 3

/* 2100 MHz band (Band I) frequency ranges for UARFCN */
#define RRC_UL_BAND_A_UARFCN_MIN 9612
#define RRC_UL_BAND_A_UARFCN_MAX 9888
#define RRC_DL_BAND_A_UARFCN_MIN 10562
#define RRC_DL_BAND_A_UARFCN_MAX 10838

/* 1900 Band (Band II) frequency ranges for UARFCN */
#define RRC_UL_BAND_B_UARFCN_MIN 9262
#define RRC_UL_BAND_B_UARFCN_MAX 9538
#define RRC_DL_BAND_B_UARFCN_MIN 9662
#define RRC_DL_BAND_B_UARFCN_MAX 9938

/* 850 MHz Band (Band V) Band frequency ranges for UARFCN */
#define RRC_UL_BAND_C_UARFCN_MIN 4132
#define RRC_UL_BAND_C_UARFCN_MAX 4233
#define RRC_DL_BAND_C_UARFCN_MIN 4357
#define RRC_DL_BAND_C_UARFCN_MAX 4458

/* 800 MHz Band (VI) frequency ranges for UARFCN */
#define RRC_UL_BAND_D_UARFCN_MIN 4162
#define RRC_UL_BAND_D_UARFCN_MAX 4188
#define RRC_DL_BAND_D_UARFCN_MIN 4387
#define RRC_DL_BAND_D_UARFCN_MAX 4413

/* 1800 MHz Band (III) frequency ranges for UARFCN */

#define RRC_UL_BAND_E_UARFCN_MIN 937
#define RRC_UL_BAND_E_UARFCN_MAX 1288
#define RRC_DL_BAND_E_UARFCN_MIN 1162
#define RRC_DL_BAND_E_UARFCN_MAX 1513


/* 900 MHz Band (VIII) frequency ranges for UARFCN */

#define RRC_UL_BAND_F_UARFCN_MIN 2712
#define RRC_UL_BAND_F_UARFCN_MAX 2863
#define RRC_DL_BAND_F_UARFCN_MIN 2937
#define RRC_DL_BAND_F_UARFCN_MAX 3088


/* 1700 MHz Band (IX) frequency ranges for UARFCN */

#define RRC_UL_BAND_G_UARFCN_MIN 8762
#define RRC_UL_BAND_G_UARFCN_MAX 8912
#define RRC_DL_BAND_G_UARFCN_MIN 9237
#define RRC_DL_BAND_G_UARFCN_MAX 9387


/* 1700_2100 AWS Band (IV) frequency ranges for UARFCN */

#define RRC_UL_BAND_H_UARFCN_MIN 1312
#define RRC_UL_BAND_H_UARFCN_MAX 1513
#define RRC_DL_BAND_H_UARFCN_MIN 1537
#define RRC_DL_BAND_H_UARFCN_MAX 1738


/* 1500 Band (XI) frequency ranges for UARFCN */
#define RRC_UL_BAND_I_UARFCN_MIN 3487
#define RRC_UL_BAND_I_UARFCN_MAX 3562
#define RRC_DL_BAND_I_UARFCN_MIN 3712
#define RRC_DL_BAND_I_UARFCN_MAX 3787

/* Band (XIX) frequency ranges for UARFCN */
#define RRC_UL_BAND_J_UARFCN_MIN 312
#define RRC_UL_BAND_J_UARFCN_MAX 363
#define RRC_DL_BAND_J_UARFCN_MIN 712
#define RRC_DL_BAND_J_UARFCN_MAX 763

/* Valid additional channels for 1900 MHz band */
/* NOTE: These arrays must be zero-terminated. */
extern rrc_UARFCN rrc_ul_band_b_addl_channels[];
extern rrc_UARFCN rrc_dl_band_b_addl_channels[];

/* Valid additional channels for 850 MHz band */
/* NOTE: These arrays must be zero-terminated. */
extern rrc_UARFCN rrc_ul_band_c_addl_channels[];
extern rrc_UARFCN rrc_dl_band_c_addl_channels[];

/* Valid additional channels for 800 MHz band */
/* NOTE: These arrays must be zero-terminated. */
extern rrc_UARFCN rrc_ul_band_d_addl_channels[];
extern rrc_UARFCN rrc_dl_band_d_addl_channels[];

/* Valid additional channels for 1700_2100 AWS MHz band (band IV) */
/* NOTE: These arrays must be zero-terminated. */
/*lint -esym(714,rrc_ul_band_h_addl_channels) */
/*lint -esym(714,rrc_dl_band_h_addl_channels) */
extern rrc_UARFCN rrc_ul_band_h_addl_channels[];
extern rrc_UARFCN rrc_dl_band_h_addl_channels[];

extern rrc_UARFCN rrc_ul_band_j_addl_channels[];
extern rrc_UARFCN rrc_dl_band_j_addl_channels[];


/*Definitions of FDL offset in 100KHz, used for deriving frequency from ARFCN*/
#define RRC_DEFAULT_FDL_OFFSET 0
#define RRC_FDL_OFFSET_BAND2_ADDL 18501
#define RRC_FDL_OFFSET_BAND3 15750
#define RRC_FDL_OFFSET_BAND4 18050
#define RRC_FDL_OFFSET_BAND4_ADDL 17351
#define RRC_FDL_OFFSET_BAND5_BAND6_ADDL 6701
#define RRC_FDL_OFFSET_BAND8 3400
#define RRC_FDL_OFFSET_BAND11 7360
#define RRC_FDL_OFFSET_BAND19 7350
#define RRC_FDL_OFFSET_BAND19_ADDL 7201
#define RRC_DEFAULT_FUL_OFFSET 0
#define RRC_FUL_OFFSET_BAND2_ADDL 18501
#define RRC_FUL_OFFSET_BAND3 15250
#define RRC_FUL_OFFSET_BAND4 14500
#define RRC_FUL_OFFSET_BAND4_ADDL 13801
#define RRC_FUL_OFFSET_BAND5_BAND6_ADDL 6701
#define RRC_FUL_OFFSET_BAND8 3400
#define RRC_FUL_OFFSET_BAND11 7330
#define RRC_FUL_OFFSET_BAND19 7700
#define RRC_FUL_OFFSET_BAND19_ADDL 7551


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
extern  void  rrc_cell_selection_procedure( rrc_cmd_type  *cmd_ptr );


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
extern  void  rrc_init_cell_selection_procedure( void );


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
extern  void  rrccsp_get_serving_pccpch_parms(uint16 *scr_code_ptr, boolean *sttd_ptr);

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
void rrccsp_get_current_plmn_cell_id(rrc_plmn_identity_type *plmn_id, rrc_int_cell_id_type *cell_id);


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
void rrccsp_get_neighbor_pccpch_parms(uint16 *scr_code_ptr,
                                      uint16 * freq_ptr);


/*===========================================================================

FUNCTION          rrccsp_is_state_transition_triggered_by_reconfig_msg

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
boolean  rrccsp_is_state_transition_triggered_by_reconfig_msg( void );


/*===========================================================================

FUNCTION          rrccsp_is_selected_cell_utran_directed

DESCRIPTION       This function returns a TRUE if the selected cell is the
                  same as directed by UTRAN and a FALSE when it is
                  different or when the cell to be camped is not indicated
                  by UTRAN.
DEPENDENCIES
                  This function should be called ONLY IF a call to
                  rrccsp_is_state_transition_triggered_by_reconfig_msg returns
                  a TRUE.
RETURN VALUE
                  Boolean
SIDE EFFECTS
                  None

===========================================================================*/
boolean  rrccsp_is_selected_cell_utran_directed(void);


/*===========================================================================

FUNCTION          rrccsp_is_inter_frequency_cell_resel_in_progress

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
boolean  rrccsp_is_inter_frequency_cell_resel_in_progress(void);


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
void rrccsp_update_plmn_info_for_gtow_handover
(
  rrc_activation_rsp_type *rrc_activation
);


/*===========================================================================

FUNCTION          rrccsp_init_measurements_in_l1_on_cphy_idle

DESCRIPTION       This function returns a value that indicates if Measurements
                  need to be initialized in L1 on a CPHY_IDLE_REQ.

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
boolean  rrccsp_init_measurements_in_l1_on_cphy_idle(void);


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
void rrccsp_fill_info_for_rr(sys_network_selection_mode_e_type *network_select_mode);


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
void rrccsp_set_ue_in_deep_sleep(void);


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
void rrccsp_update_band_preference(void);


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
boolean rrccsp_is_supported_band(rrc_csp_band_class_type band_class);

/*===========================================================================

FUNCTION          RRCCSP_IS_CURRENT_BAND

DESCRIPTION       This function checks whether the band passed to the function is 
                  the same as the current band, i.e., the band on which the UE is 
                  camped on or searching for service. This function is currently 
                  called by only SIB procedure for serving cell to determine if the 
                  frequency band indicator in SIB5/SIB5bis/SIB6 is the same as the 
                  band on which UE is camped on or searching for service.
                  
DEPENDENCIES
                  None.
                  
RETURN VALUE
                  TRUE: band_class passed is the same as the current band.
                  FALSE: otherwise.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
boolean rrccsp_is_current_band(rrc_csp_band_class_type band_class,rrc_plmn_identity_type plmn_id);


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
uecomdef_status_e_type  rrccsp_update_acq_db(rrc_freq_type freq);


/*===========================================================================

FUNCTION        RRC_GET_FREQUENCY_BAND

DESCRIPTION     This function looks at the passed in frequency and PLMN and
                returns the band the frequency belongs to. The PLMN information is used
                to determine the band if the frequency is present in the 
                overlapping region of 850 and 800 bands. 
  
RETURN VALUE    Returns FAILURE if freq is invalid.
 
SIDE EFFECTS    None

===========================================================================*/
rrc_csp_band_class_type rrc_get_frequency_band
(    
  rrc_freq_type freq,
  rrc_plmn_identity_type plmn_id
);


/*===========================================================================

FUNCTION          rrccsp_get_supported_band_list

DESCRIPTION       This function checks which bands are supported and sends the
                  populated list to UECI.

DEPENDENCIES
                  
RETURN VALUE
                  None.

SIDE EFFECTS
                  None.
                  
===========================================================================*/
void rrccsp_get_supported_band_list(rrcueci_band_storage_type * ueci_band);


/*===========================================================================

FUNCTION          RRCCSP_SEND_BPLMN_SEARCH_CNF_TO_RR

DESCRIPTION       This function is used by CSP to send RRC_INTERRAT_PLMN_SRCH_CNF
                  to RR when BPLMN search timer expires, or highest priority PLMN
                  is found, or all frequencies are exhausted.

DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void rrccsp_send_bplmn_search_cnf_to_rr( void );


/*===========================================================================

FUNCTION          RRCCSP_SEND_BPLMN_SEARCH_ABORT_CNF_TO_RR

DESCRIPTION       This function is used by CSP to send 
                  RRC_INTERRAT_PLMN_SRCH_ABORT_CNF
                  to RR when CSP receives RRC_INTERRAT_PLMN_SRCH_ABORT_REQ
                  from RR (via MCM)

DEPENDENCIES      NONE

RETURN VALUE
                  
SIDE EFFECTS

===========================================================================*/
void
rrccsp_send_bplmn_abort_cnf_to_rr( void );


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
boolean rrccsp_is_in_bplmn_list (rrc_plmn_identity_type plmn_id
                                 , rrc_MasterInformationBlock *mib_ptr
                                 ); 


/*===========================================================================

FUNCTION          rrccsp_is_ue_in_true_limited_service

DESCRIPTION       This function checks the network select mode and returns TRUE/FALSE
                  to the calling procedure.
                             
DEPENDENCIES
                  None
RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
boolean rrccsp_is_ue_in_true_limited_service(void);


/*===========================================================================

FUNCTION RRCCSP_SEND_CPHY_CELL_BAR_REQ

DESCRIPTION

  This function is called by CSP, SIB, RCE, and CHO procedures to send CPHY_CELL_BAR_REQ
  to L1 so that the corresponding cell or frequency or active set can be barred.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrccsp_send_cphy_cell_bar_req(rrc_csp_cell_bar_info_e_type, uint32);


/*===========================================================================

FUNCTION          RRCCSP_SEND_INTERRAT_RESELECTION_ABORT_CNF

DESCRIPTION       This function is responsible for sending
                  RRC_INTERRAT_RESELECTION_ABORT_CNF to RR.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_interrat_reselection_abort_cnf( void );


/*===========================================================================

FUNCTION          RRCCSP_SEND_INTERRAT_RESELECTION_REJECT

DESCRIPTION       This function is responsible for sending
                  RR_INTERRAT_RESELECTION_REJ to RR.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_interrat_reselection_reject( void );


/*===========================================================================

FUNCTION RRC_CSP_GET_ASET_SCR_PN

DESCRIPTION
  This function gets the scrambling code and pn position of the best cell
  in the active set.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrc_csp_get_aset_scr_pn(void);


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
void rrc_csp_send_service_ind (void);

/*===========================================================================

FUNCTION          RRC_CSP_SEND_SERVICE_IND_DUMMY

DESCRIPTION       This function sends RRC_SERVICE_IND to MM
                  with the last sent service indication parameters.Currently 
                  used by CHO after a W->G Cell change order failure

DEPENDENCIES
                  None.

RETURN VALUE
                  None.
                  
SIDE EFFECTS
                  None.
===========================================================================*/
void rrc_csp_send_service_ind_dummy(void);


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
extern void rrc_notify_cell_id_change
(
  void
); 

/*===========================================================================
FUNCTION rrc_get_lac_id_in_dch

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
uecomdef_status_e_type rrc_get_lac_id_in_dch
(
  rrc_gsm_map_nas_sys_info_type  * nas_common_info
);

/*===========================================================================
FUNCTION rrc_get_rac_id_in_dch

DESCRIPTION
  This function is called to retrieve RAC from CN Info

DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void rrc_get_rac_id_in_dch
(
  rrc_CN_DomainInformationList *cn_domain_info_list_ptr
);

/*===========================================================================
FUNCTION rrc_notify_cell_id_in_dch

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
void rrc_notify_cell_id_in_dch
(
  void
);

/*===========================================================================

FUNCTION   RRC_CSP_SEND_BPLMN_SUSPEND_REQ

DESCRIPTION

  This function sends RRC_BPLMN_SUSPEND_REQ to CSP
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
void rrc_csp_send_bplmn_suspend_req(rrc_proc_e_type proc, rrc_bplmn_suspend_cause_e_type cause);


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
void rrccsp_update_wtow_background_plmn_list(
  rrc_plmn_identity_type plmn_id, rrc_int_cell_id_type cell_id);


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
void rrccsp_update_wtow_manual_bplmn_list_with_rplmn(
  rrc_plmn_identity_type plmn_id, rrc_int_cell_id_type cell_id);


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
void rrccsp_prioritize_pending_commands(rrc_cmd_type *cmd_ptr);


/*===========================================================================

FUNCTION          RRC_CSP_SEND_MM_PLMN_LIST_CNF_AS_ABORTED

DESCRIPTION       This function is responsible for filling up data structures
                  for PLMN List Confirm command to MM sent by CSP.
                  This function is called when PLMN_LIST_REQ from NAS needs to
                  be aborted immediately without affecting the ongoing BPLMN
                  search (if it is in progress). The n/w select mode in 
                  PLMN_LIST_CNF is always set to MANUAL in this function when
                  feature FEATURE_WTOW_BACKGROUND_PLMN_SEARCH is enabled.

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_send_mm_plmn_list_cnf_as_aborted( uint8);


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
void rrccsp_save_req(rrc_cmd_type *cmd_ptr);


/*===========================================================================

FUNCTION          rrccsp_send_gsm_interrat_bplmn_abort_req

DESCRIPTION       This function is responsible for sending
                  RR_INTERRAT_PLMN_SRCH_ABORT_REQ to RR for aborting
                  WTOG BPLMN search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_gsm_interrat_bplmn_abort_req( void );

/*===========================================================================

FUNCTION          rrccsp_send_gsm_interrat_bplmn_suspend_ind

DESCRIPTION       This function is responsible for sending
                  RR_INTERRAT_PLMN_SRCH_SUSPEND_IND to RR for suspending
                  WTOG BPLMN search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_gsm_interrat_bplmn_suspend_ind( void );

/*===========================================================================

FUNCTION          rrc_csp_send_cell_param_req_for_pch_fach

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
void rrc_csp_send_cell_param_req_for_pch_fach(void);

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
extern mm_as_cell_access_status_T rrc_csp_get_curr_cell_access_status
(
  rrc_cn_domain_identity_e_type  cn_domain_id
);

#ifdef FEATURE_PPAC
/*===========================================================================

FUNCTION RRC_CSP_GET_LOC_REG_ALLOWED_STATUS

DESCRIPTION

This function returns  location/registration allowed status

DEPENDENCIES

  None.
  
RETURN VALUE

  TRUE - If location/registration are allowed 
  FALSE - Otherwise


SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_csp_get_loc_reg_allowed_status
(
  rrc_cn_domain_identity_e_type  cn_domain_id
);
/*===========================================================================

FUNCTION RRC_CSP_IS_PPAC_VALID

DESCRIPTION

This function returns  PPAC validity status

DEPENDENCIES

  None.
  
RETURN VALUE

  TRUE - If PPAC is valid
  FALSE - Otherwise


SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_csp_is_ppac_valid(void);

/*===========================================================================

FUNCTION RRC_CSP_GET_PAGE_RSP_ALLOWED_STATUS

DESCRIPTION

This function returns  paging response allowed status

DEPENDENCIES

  None.
  
RETURN VALUE

  TRUE - If paging response is allowed 
  FALSE - Otherwise


SIDE EFFECTS

  None.

===========================================================================*/
boolean rrc_csp_get_page_rsp_allowed_status
(
  rrc_cn_domain_identity_e_type  cn_domain_id
);

#endif

/*===========================================================================

FUNCTION     RRCCSP_IS_SIB_MCC_AND_850_800_CURRENT_BAND_COMB_VALID

DESCRIPTION

  This function is called by SIB procedure during cell selection once it reads the 
  MIB, to check if it should proceed further and read remaining SIBs for that cell.
  E.g., if the MCC in the MIB is Japan and the frequency is in the 800 band then the current
  band should be 800 otherwise the combination is invalid and UE should try the next
  cell/frequency. Similarly, if the MCC in the MIB is NOT Japan and the frequency is in the 
  800 band then the current band should be 850 otherwise the combination is invalid 
  and UE should try the next cell/frequency; etc.
   
DEPENDENCIES

  This function should only be called when the 850 or 800 band is supported, and the 
  UARFCN is in the overlapping region of 850 and 800 bands. 

RETURN VALUE

  TRUE - MCC, Frequency and Current Band combination is valid 
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrccsp_is_sib_mcc_and_850_800_current_band_comb_valid(rrc_plmn_identity_type plmn_id);


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
uecomdef_status_e_type rrccsp_fill_nas_common_info(
  rrc_SysInfoType1  *sib1_ptr,
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
uecomdef_status_e_type rrccsp_fill_nas_cn_specific_info(
  rrc_CN_DomainSysInfo *     asn1_cn_domain_sys_info_ptr,
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
boolean  rrc_check_if_dsac_is_present(  rrc_SysInfoType3   *sib3_ptr);

#ifdef FEATURE_PPAC
/*====================================================================================

FUNCTION          RRC_CHECK_IF_PPAC_IS_PRESENT

DESCRIPTION     This function is to check if PPAC info is present in SIB3 (or) not

DEPENDENCIES

RETURN VALUE
                Returns value TRUE   if PPAC information is present in SIB3
                Returns value FALSE value if PPAC information is not present in SIB3
SIDE EFFECTS
                  None
======================================================================================*/

boolean  rrc_check_if_ppac_is_present(  rrc_SysInfoType3   *sib3_ptr);
#endif


/*====================================================================
FUNCTION: RRCCSP_SEND_CONN_MODE_OOS_DS_REQ

DESCRIPTION:
  This function constructs the RRC_CONN_MODE_DEEP_SLEEP_REQ primitive and sends it
  to LLC. This function is called when UE needs to be put in deep sleep in
  RRC Connected Mode.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
====================================================================*/
void rrccsp_send_conn_mode_oos_ds_req (void);

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
uecomdef_status_e_type rrccsp_send_conn_mode_oos_ds_wakeup_req (void);


/*===========================================================================

FUNCTION          RRCCSP_IS_BAND_CLASS_ENABLED_IN_UI_RF

DESCRIPTION       This function is used to check if the band_class is enabled in
                  both the UI and RF.
                  The band_class passed to the function is added to the wcdma_band_mask
                  (CSP uses this band_mask to determine what all bands it should search
                  for when UE goes OOS), if the second argument passed to this function
                  is set to TRUE. E.g., when this function is called from inside the 
                  function validate_fdd_frequency()/validate_dl_arfcn() then the value 
                  of this argument is TRUE. This is because the function 
                  validate_fdd_frequency() is called when the frequency is received from 
                  UTRAN in SIBs or redirection or reconfig messages, etc. and for all these 
                  cases the design is that the frequency will be added to the wcdma_band_mask. 
                  Now, when this function is called from inside the function 
                  rrccsp_get_supported_band_list() then the value of the argument is always 
                  FALSE as the function rrccsp_get_supported_band_list() is called by UECI 
                  procedure only to check which bands are supported by the UE - UI && RF settings.
                  
DDEPENDENCIES
                  
RETURN VALUE
                  TRUE: band_class is enabled in UI and RF.
                  FALSE: Otherwise. 

SIDE EFFECTS
                  None.
                  
===========================================================================*/
boolean rrccsp_is_band_class_enabled_in_ui_rf(
  rrc_csp_band_class_type band_class,
  boolean update_wcdma_band_mask);

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
boolean rrccsp_skip_sibs_for_fplmn
(
  rrc_plmn_identity_type plmn_id
  , rrc_MasterInformationBlock *mib_ptr
);

/*====================================================================
FUNCTION: RRCCSP_SKIP_SIBS_FOR_VPLMN

DESCRIPTION:
         This function checks if the PLMN is a visitor PLMN and if it is already 
         present in the list to be sent to NAS.
DEPENDENCIES:
         None

RETURN VALUE:
         TRUE: skip reading SIBS if the passed PLMN is VPLMN and if the PLMN is 
               already added in the list to be sent to NAS.
         FALSE: otherwise

SIDE EFFECTS:
         None.
====================================================================*/
boolean rrccsp_skip_sibs_for_vplmn
(
  rrc_plmn_identity_type plmn_id
  , rrc_MasterInformationBlock *mib_ptr
);
/*===========================================================================

FUNCTION          RRCCSP_IS_PRIMARY_PLMN_ID_PRESENT

DESCRIPTION       Used to find out if the primary plmn id is received in RRC dedicated OTA
                  
DEPENDENCIES      None

RETURN VALUE      TRUE: If primary plmn id is presenr
                  FALSE: Otherwise

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_is_primary_plmn_id_present
(
  void
);

/*===========================================================================

FUNCTION RRCCSP_CHECK_PLMN_MATCH

DESCRIPTION

  This function checks the PLMN ID given as an arguement is 
  matching with the NAS requested PLMN or equivalent PLMN.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  boolean

SIDE EFFECTS
===========================================================================*/
boolean rrccsp_check_plmn_match( rrc_plmn_identity_type plmn_id
  , rrc_MasterInformationBlock *mib_ptr, boolean update_acq_db);


/*===========================================================================

FUNCTION          RRCCSP_RESET_NUM_TRIES_DEST_FREQ
DESCRIPTION       Resets the num tries to be done om destination frequency
                  which is received in the case of redirection
                  
DEPENDENCIES      None

RETURN VALUE      void
                 

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_reset_num_tries_dest_freq(void);

/*===========================================================================

FUNCTION          RRCCSP_IS_CAMPED_CELL_NETWORK_SHARED

DESCRIPTION       Used to find out whether the current camped cell is a part of shared network
                  
DEPENDENCIES      None

RETURN VALUE      TRUE: Camped cell is a part of shared network
                          FALSE: Camped cell belongs to a dedicated network

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_is_camped_cell_network_shared
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
boolean rrccsp_is_mib_plmn_valid(rrc_MasterInformationBlock *mib_ptr);

/*===========================================================================

FUNCTION          RRCCSP_CONSTRUCT_MULTIPLE_PLMN_LIST_FROM_MIB

DESCRIPTION       Constructs the shared PLMN list given the Mib
                  
DEPENDENCIES      None

RETURN VALUE      number of shared PLMNs

SIDE EFFECTS      None
===========================================================================*/
uint8 rrccsp_construct_multiple_plmn_list_from_mib(
                                                   rrc_MasterInformationBlock *mib_ptr,
                                                   rrc_plmn_identity_type* shared_PLMN_list);

/*===========================================================================

FUNCTION          RRCCSP_PRIORITIZE_THE_LIST_OF_SHARED_PLMNS

DESCRIPTION       Used to prioritize a PLMN among the available PLMNs based on the current scan type so that
                  this PLMN is considered first for PLMN selection if it is part of the list of shared PLMNs.
                  
DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_prioritize_the_list_of_shared_plmns(void);

/*===========================================================================

FUNCTION          RRCCSP_FIND_SHARED_PLMNS

DESCRIPTION       Finds out the list of shared PLMNs and fills in the nw_db_entry_ptr.
                  
DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
void rrccsp_find_shared_plmns(
                              rrc_MasterInformationBlock* mib_ptr,
                              rrc_plmn_identity_type plmn_id,
                              rrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
                              );


/*===========================================================================

FUNCTION          RRCCSP_GET_CELL_STATUS

DESCRIPTION       Finds out the suitability of the cell for camping based on the cell bar status, requesed PLMN etc.
                  
DEPENDENCIES      None

RETURN VALUE      
                  RRC_CSP_CELL_BETTER_THAN_EXPECTED: Cell exceeds expectations.
                  RRC_CSP_CELL_AS_EXPECTED: Cell meets expectations.
                  RRC_CSP_CELL_BELOW_EXPECTED: Cell is below expectations.

SIDE EFFECTS      None
===========================================================================*/
rrc_csp_cell_actual_vs_expected_e_type rrccsp_get_cell_status(
                                                              rrc_csp_plmn_status_e_type* cell_plmn_status,
                                                              rrc_csp_acq_entry_type* acq_entry_ptr
                                                              ,rrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
                                                              );

/*===========================================================================

FUNCTION          RRCCSP_GET_CELL_STATUS_DURING_BPLMN_SEARCH

DESCRIPTION       Finds out the suitability of the cell during bplmn search based on the 
                  cell bar status, list of requesed PLMNs etc.
                  
DEPENDENCIES      None

RETURN VALUE      
                  RRC_CSP_CELL_BETTER_THAN_EXPECTED: Cell exceeds expectations.
                  RRC_CSP_CELL_AS_EXPECTED: Cell meets expectations.
                  RRC_CSP_CELL_BELOW_EXPECTED: Cell is below expectations.

SIDE EFFECTS      None
===========================================================================*/
rrc_csp_cell_actual_vs_expected_e_type rrccsp_get_cell_status_during_bplmn_search(
                                                                                  rrc_csp_acq_entry_type* acq_entry_ptr
                                                                                  ,rrc_csp_nw_sharing_entry_type* nw_db_entry_ptr
                                                                                  );

/*===========================================================================

FUNCTION          RRCCSP_DOES_MCC_BELONG_TO_US

DESCRIPTION       This function returns TRUE if the passed PLMN belongs to US
                  otherwise it returns FALSE.
                                    
DEPENDENCIES      None

RETURN VALUE      TRUE/FALSE
                  
SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_does_mcc_belong_to_US(rrc_plmn_identity_type plmn_id);

/*===========================================================================
FUNCTION: RRCCSP_CHECK_AND_TEARDOWN_SBCCH_DURING_BPLMN

DESCRIPTION: This function is called when RRC receives DEACT_REQ / MODE_CHANGE_REQ 
             / STOP_WCDMA_MODE_REQ . If BPLMN is actively in progress and CSP is waiting for SIBs,
             it sends SBCCH release req.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rrccsp_check_and_teardown_sbcch_during_bplmn(void);

#ifdef FEATURE_GAN
#error code not present
#endif

/*===========================================================================

FUNCTION          RRCCSP_SEND_GTOW_REDIRECTION_ABORT_CNF

DESCRIPTION       This function is responsible for sending
                  RRC_INTERRAT_REDIRECT_ABORT_CNF to RR after getting 
                  RRC_INTERRAT_REDIRECT_ABORT_REQ from RR to abort GtoW redirection.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_gtow_redirection_abort_cnf(void);

/*===========================================================================

FUNCTION          RRCCSP_SEND_GTOW_REDIRECTION_REJECT

DESCRIPTION       This function is responsible for sending
                  RRC_INTERRAT_REDIRECT_REJ to RR when no WCDMA cell is found for 
                  GtoW redirection search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_gtow_redirection_reject(void);


/*===========================================================================

FUNCTION          RRCCSP_GET_SERVING_FREQUENCY

DESCRIPTION       This function gets the frequency of the serving cell 

DEPENDENCIES
                  None.

RETURN VALUE
                  Serving cell's frequency.

SIDE EFFECTS      None.
===========================================================================*/
rrc_freq_type rrccsp_get_serving_frequency(void);

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
rrc_freq_type rrccsp_get_neighbor_frequency(void);

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
rrc_plmn_identity_type rrccsp_get_curr_nas_requested_plmn(void);

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
boolean rrccsp_check_gsm_bands_supported(void);
#endif

/*===========================================================================

FUNCTION          RRC_CSP_IS_RESELECTION_CLEARED

DESCRIPTION       This function is called on reception of Chan config req for N-BCCH 
               to check if DL WEAK IND was received for this cell and reselection data has been cleared.

DEPENDENCIES      None

RETURN VALUE      TRUE : if reselection is cleared.
                  FLASE: otherwise

SIDE EFFECTS      None
===========================================================================*/
boolean rrc_csp_is_reselection_cleared(void);


#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
/*===========================================================================

FUNCTION          RRCCSP_CHECK_IF_CAMPED_NORMALLY

DESCRIPTION       This function checks if UE is camped normally, 

DEPENDENCIES      None

RETURN VALUE      TRUE if camped normally
                          FALSE if camped on any cell

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_check_if_camped_normally
(
  void
);
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST
/*===========================================================================

FUNCTION          RRC_EXTRACT_GW_BAND_PREF_FROM_RAT_PRI_LIST

DESCRIPTION       This function extracts the GW band pref from RAT PRI List

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
sys_band_mask_type rrc_extract_gw_band_pref_from_rat_pri_list
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
sys_band_mask_type rrc_extract_gw_band_pref_from_service_req
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
void rrc_csp_update_mode_band_pref_after_succ_camping
(
  void
);

/*===========================================================================

FUNCTION          RRC_CSP_UPDATE_RAT_PRI_LIST_INFO

DESCRIPTION       This function updates rat_pri_list_info in CSP INT DATA

DEPENDENCIES      None

RETURN VALUE      GW band pref

SIDE EFFECTS      None
===========================================================================*/
void rrc_csp_update_rat_pri_list_info
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
void rrc_csp_get_rat_pri_list_info
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info
);
#endif

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
/*===========================================================================

FUNCTION          RRC_EXTRACT_LTE_BAND_PREF_FROM_RAT_PRI_LIST

DESCRIPTION       This function extracts the LTE band pref from RAT PRI List

DEPENDENCIES      None

RETURN VALUE      LTE band pref

SIDE EFFECTS      None
===========================================================================*/
#ifdef LTE_BAND_NUM
sys_lte_band_mask_e_type rrc_extract_lte_band_pref_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list
);
#else
sys_band_mask_type rrc_extract_lte_band_pref_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list
);
#endif
#ifdef FEATURE_WCDMA_TO_LTE

/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_BPLMN_ABORT_REQ

DESCRIPTION       This function is responsible for sending
                  LTE_RRC_WCDMA_ABORT_PLMN_SRCH_REQ to LTE RRC for aborting
                  WTOG BPLMN search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_lte_interrat_bplmn_abort_req
(
  void
);

/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_BPLMN_SUSPEND_IND

DESCRIPTION       This function is responsible for sending
                  LTE_RRC_WCDMA_SUSPEND_PLMN_SRCH_IND to LTE RRC for suspending
                  WTOL BPLMN search

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_lte_interrat_bplmn_suspend_ind
(
  void
);

#ifdef LTE_BAND_NUM
/*===========================================================================

FUNCTION          rrccsp_check_if_lte_band_pref_is_set

DESCRIPTION       This function checks if LTE bands pref is present or not

DEPENDENCIES
                  None.

RETURN VALUE
                  TRUE if present else FALSE

SIDE EFFECTS      None.
===========================================================================*/
boolean rrccsp_check_if_lte_band_pref_is_set(sys_lte_band_mask_e_type lte_band_mask);
#endif
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
sys_lte_band_mask_e_type rrccsp_get_lte_band_pref
(
  void
);
#else
sys_band_mask_type rrccsp_get_lte_band_pref
(
  void
);
#endif
/*===========================================================================

FUNCTION          rrccsp_get_wg_band_pref

DESCRIPTION       This function populates WCDMA & GSM band preference

DEPENDENCIES
                  None.

RETURN VALUE
                  None

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_get_wg_band_pref
(
  sys_band_mask_type *wcdma_band_pref,
  sys_band_mask_type *gsm_band_pref
);
#endif

#ifdef FEATURE_LTE_TO_WCDMA
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
void rrccsp_send_lte_interrat_reselection_abort_rsp
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
void rrccsp_send_lte_interrat_plmn_srch_abort_rsp
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
void rrccsp_send_lte_interrat_redirection_abort_rsp
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
void rrccsp_send_lte_interrat_cgi_abort_rsp
(
  void
);

/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_RESELECTION_FAILED_RSP

DESCRIPTION       This function is responsible for sending
                  WCDMA_RRC_LTE_RESEL_FAILED_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_lte_interrat_reselection_failed_rsp
(
  void
);

/*===========================================================================

FUNCTION          RRCCSP_SEND_LTE_INTERRAT_REDIRECTION_FAILED_RSP

DESCRIPTION       This function is responsible for sending
                  WCDMA_RRC_LTE_REDIR_FAILED_RSP to LTE RRC.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_lte_interrat_redirection_failed_rsp
(
  void
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
uint16 rrccsp_derive_freq_from_uarfcn
(
  rrc_freq_type freq,
  rrc_csp_band_class_type band_class
);

/*===========================================================================

FUNCTION          RRCCSP_DERIVE_FREQ_FROM_UARFCN_UL

DESCRIPTION       This function derivces frequency from UL UARFCN 

DEPENDENCIES      None

RETURN VALUE      Frequency in multiple of 100KHZ

SIDE EFFECTS      None
===========================================================================*/
uint16 rrccsp_derive_freq_from_uarfcn_ul
(
  rrc_freq_type freq,
  rrc_csp_band_class_type band_class
);


/*===========================================================================

FUNCTION          RRCCSP_SEND_IRAT_BPLMN_ABORT_REQ

DESCRIPTION       This function is responsible for sending ABORT_REQ to the RAT by checking the BPLMN RAT TYPE

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_irat_bplmn_abort_req
(
  void
);

/*===========================================================================

FUNCTION          RRCCSP_SEND_IRAT_BPLMN_SUSPEND_IND

DESCRIPTION       This function is responsible for sending SUSPEND_IND to the RAT by checking the BPLMN RAT TYPE

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
void rrccsp_send_irat_bplmn_suspend_ind
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
void rrc_csp_check_n_send_suitable_srch_end_ind
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
void rrc_csp_send_disable_bplmn_ind(boolean status);

/*===========================================================================
FUNCTION: RRCCSP_CHECK_AND_TEARDOWN_SBCCH_DURING_WRLF

DESCRIPTION: This function is called when RRC receives DEACT_REQ / MODE_CHANGE_REQ 
             / STOP_WCDMA_MODE_REQ . If WRLF in progress and CSP is waiting for SIBs,
             it sends SBCCH release req.

DEPENDENCIES: NONE

RETURN VALUE: NONE

SIDE EFFECTS: NONE
=============================================================================*/
void rrccsp_check_and_teardown_sbcch_during_wrlf
(
  void
);
#endif


#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
/*===========================================================================

FUNCTION rrccsp_handle_activation_rsp_for_mdsp_halt

DESCRIPTION 
    This function handles activation response from NAS
    when mDSP halt recovery is pending at CSP.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
void rrccsp_handle_activation_rsp_for_mdsp_halt(void);

/*===========================================================================

FUNCTION rrccsp_is_rrc_waiting_for_act_rsp_to_handle_mdsp_halt

DESCRIPTION 
    This function returns the status whether CSP is waiting for
    activation responce from NAS, to start mDSP halt recovery.

DEPENDENCIES
    None.

RETURN VALUE
    boolean.

===========================================================================*/
boolean rrccsp_is_rrc_waiting_for_act_rsp_to_handle_mdsp_halt(void);
#endif


/*===========================================================================

FUNCTION rrccsp_channel_locking_nv_status

DESCRIPTION 
    This function returns true if nv is set

DEPENDENCIES
    None.

RETURN VALUE
    boolean.

===========================================================================*/

boolean rrccsp_channel_locking_nv_status(void);

/*===========================================================================

FUNCTION rrccsp_channel_locking_nv_status

DESCRIPTION 
    This function returns nv value

DEPENDENCIES
    None.

RETURN VALUE
    boolean.

===========================================================================*/

uint16 rrccsp_get_locking_nv_value(void);

/*===========================================================================

FUNCTION rrccsp_set_est_req_for_emerg_call

DESCRIPTION 
    This function sets establishment request flag for emergency call

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/

void rrccsp_set_est_req_for_emerg_call(void);

/*===========================================================================

FUNCTION rrc_csp_check_and_save_barred_freq

DESCRIPTION 
    This function saves barred frequency and adds it to the barred list
DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/

void rrc_csp_save_barred_freq(void);

/*===========================================================================

FUNCTION          rrccsp_get_lte_resel_allow_status

DESCRIPTION       This function will return lte_resel_allow_status

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_get_lte_resel_allow_status(void);

#ifdef FEATURE_LTE_TO_WCDMA
/*===========================================================================

FUNCTION        rrc_csp_fill_cgi_bplmn_tmr_exp

DESCRIPTION     This function will fill CGI info from MIB and SIB3 if available 
                upon BPLMN CGI timer expiry

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrc_csp_fill_cgi_bplmn_tmr_exp(void);
#endif

/*===========================================================================
  FUNCTION        rr_send_rrc_message_to_rr_internal

  DESCRIPTION     This function will add AS ID to the messages sent from RRC to RR
                  and calls the function which actually sends the message

  DEPENDENCIES
                None

  RETURN VALUE
                None

  SIDE EFFECTS
                None
===========================================================================*/
void rr_send_rrc_message_to_rr_internal(
  void *            message_ptr,
  rrc_rr_cmd_e_type rrc_cmd_id,
  word              length
);

/*===========================================================================

FUNCTION        RR_RRC_GET_ARFCN_BAND_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
sys_band_T rr_rrc_get_arfcn_band_internal(
  uint16                       arfcn,
  rrc_rr_frequence_band_e_type band_ind
  );

/*===========================================================================

FUNCTION        RR_RRC_SET_BAND_PREF_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rr_rrc_set_band_pref_internal(sys_band_mask_type     band_pref);

/*===========================================================================

FUNCTION        RR_RRC_SET_BST_BAND_PREF_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rr_rrc_set_bst_band_pref_internal(sys_band_mask_type     band_pref);


/*===========================================================================

FUNCTION        RR_RRC_CLEAR_ACQ_CHNL_INFO_INTERNAL

DESCRIPTION     This function will call appropriate RR function based on DSDS flag

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rr_rrc_clear_acq_chnl_info_internal(void);


/*===========================================================================

FUNCTION          rrccsp_check_if_em_plmn

DESCRIPTION       This function will return true if UE is currently camped on EM NW
                  This is needed as HSPA+ (Category 14) should be indicated in case 
                  of EM network.
DEPENDENCIES      None

RETURN VALUE      Boolean

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_check_if_em_plmn(void);


#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION          rrccsp_asf_actively_in_prog

DESCRIPTION       This function will return TRUE if ASF is actively 
                  in progress

DEPENDENCIES      None

RETURN VALUE      bool

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_asf_actively_in_prog(void);

/*===========================================================================

FUNCTION          rrccsp_csg_in_whitelist

DESCRIPTION       This function will return CSG whitelist query based on 
                  current scan 

DEPENDENCIES      None

RETURN VALUE      bool

SIDE EFFECTS      None
===========================================================================*/

boolean rrccsp_csg_in_whitelist(uint32 csg_id, rrc_plmn_identity_type plmn_id);

/*===========================================================================

FUNCTION          RRCCSP_GET_CURR_NAS_REQUESTED_CSG_ID

DESCRIPTION       This function returns the specific CSG ID on which NAS has 
                  requested RRC to camp.
DEPENDENCIES
                  None.

RETURN VALUE
                  NAS requested PLMN.

SIDE EFFECTS      None.
===========================================================================*/
rrc_int_csg_id_type rrccsp_get_curr_nas_requested_csg_id(void);

/*===========================================================================

FUNCTION          RRCCSP_CSG_SET_FP_INFO

DESCRIPTION       This function updates the FP vars with current camped macro
                  upon W manual CSG search trigger
DEPENDENCIES
                  None.

RETURN VALUE
                  None

SIDE EFFECTS      None.
===========================================================================*/

void rrccsp_csg_set_fp_info(void);
#endif

/*===========================================================================

FUNCTION          rrccsp_initial_suitability_check

DESCRIPTION       This function will check initial suitability of the cell

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

void rrccsp_initial_suitability_check
(
  rrc_MasterInformationBlock *mib_ptr, 
  rrc_SysInfoType3 *sib3_ptr,
  rrc_plmn_identity_type mib_plmn_id, 
#ifdef FEATURE_FEMTO_CSG
  uint32 csg_id, 
#endif
  rrc_sib_status_e_type *status
);


/*===========================================================================

FUNCTION          RRCCSP_CONVERT_RCSP_MEAS_TO_REPORT

DESCRIPTION       Converts measured RSCP value to reported value. 
                  Spec 25.133 section 9.1.1.3 CPICH RSCP 
                  measurement report mapping

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

int16 rrccsp_convert_rcsp_meas_to_report(int16 rscp);
/*===========================================================================

FUNCTION        rrccsp_force_send_deact_cnf

DESCRIPTION     This function handles the subsequent DEACT_REQ came from NAS
                even .when Lower Layers are already deactivated.

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrccsp_force_send_deact_cnf(void);
#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION          rrccsp_reset_wait_l1_rel_lock

DESCRIPTION       To reset csp_waiting_for_wl1_to_release_lock flag

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

void rrccsp_reset_wait_l1_rel_lock(void);

/*===========================================================================

FUNCTION        RRC_CSP_BPLMN_UPDATE_SELECT_DATA_FOR_REATTEMPT

DESCRIPTION     This function will update the indices of the ACQ DB/rrc_csp_curr_select_data so that search is attempted on the currently
                populated entry

DEPENDENCIES
                None

RETURN VALUE
                None

SIDE EFFECTS
                None

===========================================================================*/
void rrc_csp_bplmn_update_select_data_for_reattempt(void);
#endif
/*===========================================================================

FUNCTION          RRCCSP_CONVERT_ECIO_MEAS_TO_REPORT

DESCRIPTION       Converts measured ECIO value to reported value. 
                  Spec 25.133 section 9.1.2.3 CPICH ECIO 
                  measurement report mapping

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

uint8 rrccsp_convert_ecio_meas_to_report(int16 ecio);

/*===========================================================================

FUNCTION          RRC_CSP_SEND_MM_PLMN_LIST_CNF_AS_SUCCESS

DESCRIPTION       

DEPENDENCIES
                  None
                  
RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_csp_send_mm_plmn_list_cnf_as_success(uint8 trans_id);
#endif /* RRCCSP_H */

