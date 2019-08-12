#ifndef RRC_RR_TYPES_H
#define RRC_RR_TYPES_H
/*===========================================================================
         R R C  -  R R   P R I M I T I V E / T Y P E   D E F I N I T I O N S  

DESCRIPTION

  This module is the header module that contains the type definitions for
  the primitives between RRC and RR. The module is only used for dual-mode.
  
Copyright (c) 2002-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rrc_rr_types.h#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/14   bc      Made changes to support W2G CCO Abort mechanism.
10/31/14   bc      Changes to support additional requirements in FR 20971.
07/01/14   hk      Header file inclusion cleanup
05/20/14   rmn     Made changes to support FRLTE when indicated GtoW HO Req and also indicate CSFB status in WtoG HO Req.
03/12/14   bc      Made changes to add rrc_interrat_cc_order_abort_req_type to rrc_rr_cmd_data_u_type.
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/24/14   sr      Made changes to pass PN position for GTOW reselection
11/14/13   sp      Made changes for QMI Extension for Detailed Protocol Stack Information.
11/12/13   vi      Added AS ID in CIPHER_SYNC_IND command
11/05/13   vi      Reverting CR 539702
10/03/13   vi      Made changes to define correct enum for G2W and W2G dedicated priority transfer 
08/28/13   bc      Made changes to handle GTOW CCO abort and WTOG cco abort requests
06/26/13   sr      Made changes to introduce a new macro for IHO message size
06/24/13   as      Made changes to send RRC_CIPHER_SYNC_IND to G whenever new keys are applied 
04/24/13   sn      WRRC side of changes to meet CSG ADAPT TCs
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
08/31/12   rd      Transfer dedicated priority parameters to/from WCDMA and GERAN
                   during IRAT procedure
08/17/12   gv      Added changes to support suspension of intra/inter-W BPLMN search on entering DCH/FACH
                   and resumption of the same on entering PCH/IDLE
07/29/11   ad      Made changes for CMI Phase 4
07/05/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.101.00
06/23/11   gv      Pulled required structures out of the feature FEATURE_INTERRAT_HANDOVER_GTOW
                   to fix compilation issues when the feature is not defined
06/01/11   sks     Made changes to handle LTE detected cell info in GTOW PLMN list search request.
05/30/11   rl      CMI de-featurization
02/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11   ad      Added changes to refeaturize the API files
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/21/11   ad      Made changes to fix compilation errors on 76xx target for SW decoupling
01/18/11   sks     Added a new cause for GTOW handover failure for MDSP recovery feature.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
05/24/10   sks     Made changes to support R7 spec CR related to HPLMN-VPLMN ping-pong.
03/15/10   rm      Added code to support FEATURE_RAT_PRIORITY_LIST for W<->G IRAT
11/04/09   sks     Added code to support inter-RAT redirection via connection reject
                   and both inter-RAT and inter-freq redirection via connection release.
11/03/09   sks     Added stop cause MODE_LPM to inform GSM of mode change to LPM.
08/07/09   rm      CMI change:Removed inclusion of sys_stru_v.h because NAS has moved this file into private.
06/15/09   sks     Moved commands out of GAN feature to support messaging interface
                   with RR for writing ACQ-DB to NV during power off.
05/08/09   ss      Updated Copyright Information
03/04/09   rm      CMI Change:Changed the include file name to "sys_stru_v.h" from "sys_stru.h"
12/29/08   rm      Added support for 3G<->GAN Handover
11/04/08   rm      Added RRC_MODE_RRC_INACTIVE to rrc_curr_operating_mode_e_type.
10/24/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
04/21/08   kp      Compiler warning fix which will turnup if feature 
                   FEATURE_SPEC_CR_2567_GTOW_SECURITY_KEY_HANDLING is not defined
03/13/08   vm/pk   Added support for feature FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE.
10/05/07   vm      Removed few "ifdef" checks for FEATURE_UMTS_ACQ_CHNL_INFO_SHARING.
09/20/07   vm      Added support for feature FEATURE_UMTS_ACQ_CHNL_INFO_SHARING
07/23/07   kp      Added new command RR_CIPHER_SYNC_IND in RRC - RR Command Id type
                   This is Sent from GSM to WCDMA when RR receives ciphering mode 
                   command message which will start ciphering on G for CS domain.
                   Code changes are under new feature flag
                   FEATURE_SPEC_CR_2567_GTOW_SECURITY_KEY_HANDLING.
07/02/07   bu      Allow W to inform G about the frequency offset of the G cell
                   in W2G non-blind handover scenarios.
06/15/07   vm      Added support for FEATURE_CONNECTED_MODE_UMTS_UNIFORM_OOS_HANDLING_UI_UPDATE
11/21/06   vm      Added support for the feature FEATURE UMTS BACKGROUND FULL SERVICE SEARCH.
                   Added new field "service_search" of type "sys_service_search_e_type"
                   in rrc_interrat_plmn_srch_req_type and rr_interrat_plmn_srch_req_type.
09/05/06   vm      Added support for UOOSC.
06/08/06   vm      Added another field plmn_id in the structure rrc_interrat_handover_req_type
                   under the feature flag FEATURE_MCC_AND_850_800_BAND_COMBINATION.
03/16/06   vm      Changed uint8 to uint32 type for num_plmns in structures 
                   rrc_interrat_plmn_srch_req_type and rr_interrat_plmn_srch_req_type.
01/31/06   vm      Replaced SYS_PLMN_LIST_MAX_LENGTH by SYS_PRIORITY_PLMN_LIST_MAX_LENGTH
                   for Automatic Interrat PLMN search requests.
01/20/06   vm      Added support for getting GSM arfcns during Manual/Automatic
                   PLMN search so that acq attempt on some of the W channels can
                   be skipped to reduce freq scan times in PCS/850 bands. For this,
                   structure rr_rrc_gsm_cell_list_type has been added to 
                   interrat_plmn_srch_req/cnf structures. The changes are under 
                   feature flag FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION.
11/23/05   gj/vm   Added support for feature FEATURE_PCS_850_FREQ_SCAN_NEW_OPTIMIZATION.
10/07/05   vm      Added interface support for FEATURE_WTOG_BACKGROUND_PLMN_SEARCH.
                   Added 3 new structures rr_interrat_plmn_srch_req_type, 
                   rr_interrat_plmn_srch_cnf_type, and rr_interrat_plmn_srch_abort_req_type.
                   Added 4 new commands RR_INTERRAT_PLMN_SRCH_REQ, RR_INTERRAT_PLMN_SRCH_ABORT_REQ,
                   RR_INTERRAT_PLMN_SRCH_CNF, and RR_INTERRAT_PLMN_SRCH_ABORT_CNF.
08/21/05   bu      Added the message header to rrc_interrat_cc_order_cnf_type.
08/20/05   vr      Support for GTOW Packet Cell Change Order
08/16/05   vm      Added support for feature FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION.
                   Added another enum type, RRC_RR_SERVICE_SEARCH, for RRC redirected 
                   cell search on GSM.
08/12/05   vr      Support for Network Assisted Cell Change (NACC)
07/15/05   vm      Added support for not updating band information for 
                   inter-RAT handover, CCO, and reselections.
                   Removed band_pref variable from rrc_interrat_handover_req_type,
                   rr_interrat_handover_req_type, rr_interrat_reselect_req_type,
                   rr_interrat_cc_order_req_type, and rrc_interrat_cc_order_req_type.
                   Changed W_ARFCN_T to ARFCN_T; and removed frequency_band variable
                   from rr_interrat_reselect_req_type and rr_interrat_cc_order_req_type.
06/30/05   vm      Added support for inter-RAT OOS handling in Connected Mode.
                   Added new enum type rrc_rr_cell_srch_reason_e_type and updated
                   existing structures rr_interrat_redirect_req_type and 
                   rr_interrat_redirect_rej_type with new variables.
06/30/05   vr      Support for FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
                   Added new types for rr_interrat_cc_order_cnf,
                   rr_interrat_cc_order_abort_req and rr_interrat_cc_order_abort_cnf
05/12/05   gj      Added structure definition for rrc_interrat_reselection_abort_cnf.
05/11/05   gj      Added structure definition for rr_interrat_reselection_abort_req.
05/09/05   vm      Added 4 new commands for handling reselection aborts: 
                   RR_INTERRAT_RESELECTION_ABORT_REQ, RR_INTERRAT_RESELECTION_ABORT_CNF,
                   RRC_INTERRAT_RESELECTION_ABORT_REQ, RRC_INTERRAT_RESELECTION_ABORT_CNF
04/04/05   bu      Added support for inter-RAT redirection feature.
                   . Added new RRC <-> RR commands:
                     RR_INTERRAT_REDIRECT_REQ, RR_INTERRAT_REDIRECT_ABORT_REQ,
                     RR_INTERRAT_REDIRECT_REJ, RR_INTERRAT_REDIRECT_ABORT_CNF.
                   . Added new enums and structures:
                     rr_interrat_redirect_fail_reason_type,
                     rrc_rr_gsm_cell_list_type, rr_interrat_redirect_req_type,
                     rr_interrat_redirect_rej_type, rr_interrat_redirect_abort_req_type,
                     and rr_interrat_redirect_abort_cnf_type.
03/31/05   bu      Added new RRC_WCDMA_UARFCN_BARRED to rrc_interrat_reject_reason_T.
11/11/04   bu      Added sys_detailed_plmn_list_s_type instead of an array of
                   PLMN_id_T in rrc_interrat_plmn_srch_cnf_type. Use
                   SYS_PLMN_LIST_MAX_LENGTH instead of HPLMN_PLMN_LIST_MAX_LENGTH.
                   Added sys_network_selection_mode_e_type in
                   rrc_interrat_plmn_srch_req_type. 
11/02/04   bu      Initial checkin for background 3G PLMN search while in 2G.
                   Added new primitives and structure definitions to support
                   Interrat 3G PLMN search while in 2G. New structures defined include
                   rrc_interrat_plmn_srch_req_type, rrc_interrat_plmn_srch_cnf_type,
                   rrc_interrat_plmn_srch_abort_req_type and
                   rrc_interrat_plmn_srch_abort_cnf_type. Defined a new enum
                   rr_rrc_abort_cause_e_type. Added related primitives in
                   rrc_rr_cmd_e_type.
06/01/04   bu      Added band preference and network_select_mode to all the
                   W<->G interfaces. Removed any_cell_selection_mode.
05/21/04   kc      Added RR_RRC_FREQUENCY_NOT_IMPLEMENTED to failure types for handover
                   reject cause.
04/02/04   bu      Added PLMN and forbidden LAC fields to
                   rrc_interrat_cc_order_rej_type.
03/19/03   kc      Added RRC<->RR interface types and interface commands for
                   GtoW PCCO feature.
01/29/04   bu      Added the forbidden LAI information - PLMN_ID and LAC, in
                   the GTOW reselection reject.
12/16/03   bu      Added GSM Quadband support.
           kc      Added trk_lo_adj to rrc_interrat_reselect_req_type interface.
           kc      Changes to support Non-Blind HO.  Measurement primitive
                   updated to add sttd, pn_pos and trk_lo_adj info.
05/28/03   Rao     Added PCCO ( W to G ) Support.
04/24/03   bu      Changed squalmin & srxlevmin to int16 from uint16 in
                   rrc_interrat_reselect_rej_type.
02/14/03   kc      Added types for GtoW Interrat Handover.
11/27/02   xfg     Changed name and type for RSSI in measurement data structure
11/22/02   xfg     Added rx power in measurement data structure
11/04/02   Rao     Added another InterRAT WCDMA to GSM Handover Fail Reason.
10/08/02   xfg     Added more enums for rrc_interrat_reject_reason_T.
10/01/02   xfg     Fixed a problem related to feature defines.
09/30/02   Rao     Added GSM to WCDMA Reselection Support.
09/10/02   Rao     Added Expected_RXLEV_average to interrat_reselect_rej_type. 
09/09/02   Rao     Added more WCDMA to GSM Reselect Reject reasons.
08/27/02   xfg     Added support for WCDMA to GSM inter-RAT cell reselection
08/26/02   xfg     Changed BSIC from uint16 to bsic_bcc (uint8) and bsic_ncc 
                   (uint8)
08/14/02   xfg     Added a sparate command base for commands from RRC to RR.
07/12/02   xfg     Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "sys.h"
#include "sys_stru.h"
#include "sys_plmn_selection.h"

#include "sys_type.h"
#include "lte_irat_types.h"


typedef byte SAPI_T_TEMP;

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* Maximum number of inter-RAT measurements */
#define RRC_RR_MAX_INTERRAT_MEASUREMENTS 32

/* Maximum length of the GSM Command */
#define RRC_RR_MAX_GSM_MESSAGE_LENGTH 256

/* Maximum length of Sngle GSM message */
#define RRC_RR_MAX_SINGLE_GSM_MESSAGE_LENGTH 64

/* Maximum number of the GSM commands */
#define RRC_RR_MAX_GSM_COMMANDS 4 /* define as 4 for now, it should only be one */

/* RRC and RR command identifier base */
#define RRC_RR_CMD_ID_BASE 0x07000000 /* NOTE: This value should not be changed.
                                         The same value is also used in rrccmd.h */ 

/* Max. SI block length in GERAN System Information for Network Assisted Cell Change(NACC) */
#define RRC_RR_MAX_SI_BLOCK_LENGTH 23

/* Max. number of SI blocks in GERAN System Information for Network Assisted Cell Change(NACC) */
#define RRC_RR_MAX_SI_BLOCKS 8

/* Max. # of GSM channels reported to RRC */
#define MAX_RR_RRC_GSM_CELL_LIST_SIZE 140
/* Max. # of WCDMA channels reported to GSM-RR */
#define RRC_RR_MAX_NUM_FREQ 20

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* RRC - RR Command Id type */
typedef enum
{
  /* RRC to RR command base. Note: starting from 0 */
  RRC_TO_RR_BASE = 0,
  /* Add RRC to RR command below */

  /* WCDMA to GSM inter-RAT handover request from RRC to RR */
  RR_INTERRAT_HANDOVER_REQ,

  /* WCDMA to GSM inter-RAT handover abort request from RRC to RR */
  RR_INTERRAT_HANDOVER_ABORT_REQ,

  /* NAS Signaling data indication from RRC to RR */
  RR_NAS_DATA_IND,

  /* WCDMA to GSM inter-RAT Reselection Request from RRC to RR */
  RR_INTERRAT_RESELECTION_REQ,

  /* GSM to WCDMA inter-RAT Reselection Reject from RRC to RR */
  RRC_INTERRAT_RESELECTION_REJ,

  /* WCDMA to GSM inter-RAT CC Order Request from RRC to RR */
  RR_INTERRAT_CC_ORDER_REQ,

  /*WCDMA to GSM Handover confirmation status*/
  RRC_INTERRAT_HANDOVER_CNF,

  /*WCDMA to GSM Handover Abort confirmation status*/
  RRC_INTERRAT_HANDOVER_ABORT_CNF,

  /* GSM to WCDMA inter-RAT CC Order Reject from RR to RRC */
  RRC_INTERRAT_CC_ORDER_REJ,

  /* Background 3G PLMN Search Cnf from RRC to RR */
  RRC_INTERRAT_PLMN_SRCH_CNF,
  
  /* Background 3G PLMN Search Abort Cnf from RRC to RR */
  RRC_INTERRAT_PLMN_SRCH_ABORT_CNF,

  /* WCDMA to GSM service redirection request from RRC to RR */
  RR_INTERRAT_REDIRECT_REQ,

  /* WCDMA to GSM service redirection abort request from RRC to RR */
  RR_INTERRAT_REDIRECT_ABORT_REQ,

  /* This command is sent by RRC to RR to abort an on-going WTOG reselection */
  RR_INTERRAT_RESELECTION_ABORT_REQ,

  /* This command is sent by RRC to RR after the abort request has been 
   * successfully processed. 
   * Note: The GTOW reselection (Idle) abort cannot fail.
   */
  RRC_INTERRAT_RESELECTION_ABORT_CNF,

  /* WCDMA to GSM inter-RAT CC Order Abort Request from RRC to RR */
  RR_INTERRAT_CC_ORDER_ABORT_REQ,

  /* GSM to WCDMA inter-RAT CC Order Cnf from RRC to RR*/
  RRC_INTERRAT_CC_ORDER_CNF,

  /* Background 2G PLMN Search request from RRC to RR */
  RR_INTERRAT_PLMN_SRCH_REQ,

  /* Background 2G PLMN Search Abort Request from RRC to RR */
  RR_INTERRAT_PLMN_SRCH_ABORT_REQ,
    
  /* Background 2G PLMN Search Suspend Indication from RRC to RR */
  RR_INTERRAT_PLMN_SRCH_SUSPEND_IND,

  /* RRC->RR indication that RRC has received a service request. */
  RR_GAN_ACT_IND,

  /* RRC->RR autonomous indication of RRC state. */
  RR_GAN_MODE_IND,

  /* RRC->RR response to RRC_RESELECT_TO_GAN_REQ indicating successful
  completion of reselection to GAN. */
  RRC_RESELECT_TO_GAN_CNF,

  /* RRC->RR response to RRC_RESELECT_TO_GAN_REQ indicating temporary
  failure of reselection to GAN, which should be retried by RR at some
  point in the future. */
  RRC_RESELECT_TO_GAN_REJ,

  /* RRC->RR response to RRC_GAN_CELL_INFO_REQ. */
  RRC_GAN_CELL_INFO_CNF,
  /*RRC->RR response to RRC_RESLECT_FROM_GAN_REQ, indicating failure
    of reselection from GAN request  
  */
  RRC_RESELECT_FROM_GAN_REJ,
  /*RRC->RR Response to RRC_GAN_ACT_REQ, indicating failure of
    RRC_GAN_ACT_REQ
  */
  RRC_GAN_ACT_REJ,
  RRC_INTERRAT_REDIRECT_REJ,

  RRC_INTERRAT_REDIRECT_ABORT_CNF,
  /*RRC->RR Response to RRC_RR_STOP_WCDMA_REQ
  */
  RRC_RR_STOP_WCDMA_CNF,
  /*RRC->RR Request from RRC to Stop GSM
  */
  RR_RRC_STOP_GSM_REQ,

  /* RRC->RR Request for dedicated priorities. */
  RR_DEDICATED_PRIORITIES_REQ,

  /* RRC->RR Response to request for dedicated priorities. */
  RRC_DEDICATED_PRIORITIES_RSP,

  /* RRC->RR Indicate that RR should clear it's dedicated priorities. */
  RR_CLEAR_DEDICATED_PRIORITIES_IND,

  /* RRC->RR Indicate that new keys are activated at W*/
  RRC_CIPHER_SYNC_IND,

  /* RRC->RR Indicates that WL1 is stopped.*/
  RR_INTERRAT_WL1_STOP_IND,

  /**********************************************************************/
  /* RR to RRC command base. Note: starting value is RRC_RR_CMD_ID_BASE */
  RR_TO_RRC_BASE = RRC_RR_CMD_ID_BASE,
  /* Add new RR to RRC commands below */
  /**********************************************************************/

  /* WCDMA to GSM inter-RAT handover confirmation from RR to RRC */
  RR_INTERRAT_HANDOVER_CNF,

  /* WCDMA to GSM inter-RAT handover abort confirmation from RR to RRC */
  RR_INTERRAT_HANDOVER_ABORT_CNF,

  /* WCDMA to GSM inter-RAT Reselection Reject from RR to RRC */
  RR_INTERRAT_RESELECTION_REJ,

  /* WCDMA to GSM inter-RAT CC Order Reject from RR to RRC */
  RR_INTERRAT_CC_ORDER_REJ,

  /* GSM to WCDMA inter-RAT Reselection Request from RR to RRC */
  RRC_INTERRAT_RESELECTION_REQ,

  /* GSM to WCDMA inter-RAT CC Order Request from RR to RRC */
  RRC_INTERRAT_CC_ORDER_REQ,

  /* GSM to WCDMA inter-RAT CC Order Abort Request from RR to RRC */
  RRC_INTERRAT_CC_ORDER_ABORT_REQ,

  /* GSM to WCDMA inter-RAT Reselection Request from RR to RRC */
  RRC_INTERRAT_HANDOVER_REQ,
  
  /* GSM to WCDMA inter-RAT Reselection Abort Request from RR to RRC */
  RRC_INTERRAT_HANDOVER_ABORT_REQ,

  /* GSM to WCDMA NAS Transfer indication */
  RRC_NAS_DATA_IND,

  /* Background 3G PLMN Search request from RR to RRC */
  RRC_INTERRAT_PLMN_SRCH_REQ,

  /* Background 3G PLMN Search Abort Request from RR to RRC */
  RRC_INTERRAT_PLMN_SRCH_ABORT_REQ,

  /* WCDMA to GSM Inter-RAT Redirection Reject from RR to RRC */
  RR_INTERRAT_REDIRECT_REJ,

  /* WCDMA to GSM Inter-RAT Redirection abort confirm from RR to RRC */
  RR_INTERRAT_REDIRECT_ABORT_CNF,

  /* This command is sent by RR to RRC to abort an on-going GTOW reselection. */
  RRC_INTERRAT_RESELECTION_ABORT_REQ,

  /* This command is sent by RR to RRC after the abort request has been 
   * successfully processed. 
   * Note: The WTOG reselection (Idle) abort cannot fail.
   */
  RR_INTERRAT_RESELECTION_ABORT_CNF,

  /* WCDMA to GSM inter-RAT CC Order Cnf from RR to RRC*/
  RR_INTERRAT_CC_ORDER_CNF,

  /* WCDMA to GSM inter-RAT CC Order Abort Cnf from RR to RRC*/
  RR_INTERRAT_CC_ORDER_ABORT_CNF,

  /* Background 2G PLMN Search Cnf from RR to RRC */
  RR_INTERRAT_PLMN_SRCH_CNF,

  /* Background 2G PLMN Search Abort Cnf from RR to RRC */
  RR_INTERRAT_PLMN_SRCH_ABORT_CNF

  /* Sent from GSM to WCDMA when RR receives ciphering mode 
   * command message which will start ciphering on G for CS domain
   */
  ,RR_CIPHER_SYNC_IND

  /* RR->RRC request to initiate a reselection to GAN procedure. */
  , RRC_RESELECT_TO_GAN_REQ

  /* RR->RRC request for RRC serving cell system information. */
  , RRC_GAN_CELL_INFO_REQ
  /* RR->RRC request to initiate a reselection from GAN procedure.
  */
  , RRC_RESELECT_FROM_GAN_REQ
  /*RR->RRC request to go to IDLE_DETACHED_MODE
  */
  ,RRC_GO_TO_IDLE_DETACHED_REQ
  /*RR->RRC request to wake up WCDMA
  */
  ,RRC_GAN_WAKE_UP_REQ
  /*RR->RRC request to camp on specific PLMN
  */
  ,RRC_GAN_ACT_REQ
  ,RRC_IMR_FROM_GAN_REQ
  ,RRC_CANCEL_HANDOVER_TO_GAN_REQ 

  , RRC_INTERRAT_REDIRECT_REQ

  , RRC_INTERRAT_REDIRECT_ABORT_REQ
  /*RR->RRC request to stop WCDMA
  */
  , RRC_RR_STOP_WCDMA_REQ
  /*RR->RRC response to RR_RRC_STOP_GSM_REQ
  */
  ,RR_RRC_STOP_GSM_CNF

  /* RR->RRC Request for dedicated priorities. */
  ,RRC_DEDICATED_PRIORITIES_REQ

  /* RR->RRC Response to request for dedicated priorities. */
  ,RR_DEDICATED_PRIORITIES_RSP

  /* RR->RRC Indicate that RR should clear it's dedicated priorities. */
  ,RRC_CLEAR_DEDICATED_PRIORITIES_IND

  /* RR->RRC Indicates that GSM has received Stop after sending CCO failure ind to RRC */
  ,RRC_GRR_INTERRAT_PROC_ABORT_IND

} rrc_rr_cmd_e_type;
/* Below #defines are added for logging purpose only */  
#define rrc_rr_cmd_e_type_value0x1 RR_INTERRAT_HANDOVER_REQ,
#define rrc_rr_cmd_e_type_value0x2 RR_INTERRAT_HANDOVER_ABORT_REQ,
#define rrc_rr_cmd_e_type_value0x3 RR_NAS_DATA_IND,
#define rrc_rr_cmd_e_type_value0x4 RR_INTERRAT_RESELECTION_REQ,
#define rrc_rr_cmd_e_type_value0x5 RRC_INTERRAT_RESELECTION_REJ,
#define rrc_rr_cmd_e_type_value0x6 RR_INTERRAT_CC_ORDER_REQ,
#define rrc_rr_cmd_e_type_value0x7 RRC_INTERRAT_HANDOVER_CNF,
#define rrc_rr_cmd_e_type_value0x8 RRC_INTERRAT_HANDOVER_ABORT_CNF,
#define rrc_rr_cmd_e_type_value0x9 RRC_INTERRAT_CC_ORDER_REJ,
#define rrc_rr_cmd_e_type_value0xa RRC_INTERRAT_PLMN_SRCH_CNF,
#define rrc_rr_cmd_e_type_value0xb RRC_INTERRAT_PLMN_SRCH_ABORT_CNF,
#define rrc_rr_cmd_e_type_value0xc RR_INTERRAT_REDIRECT_REQ,
#define rrc_rr_cmd_e_type_value0xd RR_INTERRAT_REDIRECT_ABORT_REQ,
#define rrc_rr_cmd_e_type_value0xe RR_INTERRAT_RESELECTION_ABORT_REQ,
#define rrc_rr_cmd_e_type_value0xf RRC_INTERRAT_RESELECTION_ABORT_CNF,
#define rrc_rr_cmd_e_type_value0x10 RR_INTERRAT_CC_ORDER_ABORT_REQ,
#define rrc_rr_cmd_e_type_value0x11 RRC_INTERRAT_CC_ORDER_CNF,
#define rrc_rr_cmd_e_type_value0x12 RR_INTERRAT_PLMN_SRCH_REQ,
#define rrc_rr_cmd_e_type_value0x13 RR_INTERRAT_PLMN_SRCH_ABORT_REQ,
#define rrc_rr_cmd_e_type_value0x14 RR_INTERRAT_PLMN_SRCH_SUSPEND_IND,
#define rrc_rr_cmd_e_type_value0x15 RR_GAN_ACT_IND,
#define rrc_rr_cmd_e_type_value0x16 RR_GAN_MODE_IND,
#define rrc_rr_cmd_e_type_value0x17 RRC_RESELECT_TO_GAN_CNF,
#define rrc_rr_cmd_e_type_value0x18 RRC_RESELECT_TO_GAN_REJ,
#define rrc_rr_cmd_e_type_value0x19 RRC_GAN_CELL_INFO_CNF,
#define rrc_rr_cmd_e_type_value0x1a RRC_RESELECT_FROM_GAN_REJ,
#define rrc_rr_cmd_e_type_value0x1b RRC_GAN_ACT_REJ,
#define rrc_rr_cmd_e_type_value0x1c RRC_INTERRAT_REDIRECT_REJ,
#define rrc_rr_cmd_e_type_value0x1d RRC_INTERRAT_REDIRECT_ABORT_CNF,
#define rrc_rr_cmd_e_type_value0x1e RRC_RR_STOP_WCDMA_CNF,
#define rrc_rr_cmd_e_type_value0x1f RR_RRC_STOP_GSM_REQ,
#define rrc_rr_cmd_e_type_value0x20 RR_DEDICATED_PRIORITIES_REQ,
#define rrc_rr_cmd_e_type_value0x21 RRC_DEDICATED_PRIORITIES_RSP,
#define rrc_rr_cmd_e_type_value0x22 RR_CLEAR_DEDICATED_PRIORITIES_IND,
#define rrc_rr_cmd_e_type_value0x23 RRC_CIPHER_SYNC_IND,

#define rrc_rr_cmd_e_type_value0x7000001 RR_INTERRAT_HANDOVER_CNF,
#define rrc_rr_cmd_e_type_value0x7000002 RR_INTERRAT_HANDOVER_ABORT_CNF,
#define rrc_rr_cmd_e_type_value0x7000003 RR_INTERRAT_RESELECTION_REJ,
#define rrc_rr_cmd_e_type_value0x7000004 RR_INTERRAT_CC_ORDER_REJ,
#define rrc_rr_cmd_e_type_value0x7000005 RRC_INTERRAT_RESELECTION_REQ,
#define rrc_rr_cmd_e_type_value0x7000006 RRC_INTERRAT_CC_ORDER_REQ,
#define rrc_rr_cmd_e_type_value0x7000007 RRC_INTERRAT_CC_ORDER_ABORT_REQ,
#define rrc_rr_cmd_e_type_value0x7000008 RRC_INTERRAT_HANDOVER_REQ,
#define rrc_rr_cmd_e_type_value0x7000009 RRC_INTERRAT_HANDOVER_ABORT_REQ,
#define rrc_rr_cmd_e_type_value0x700000a RRC_NAS_DATA_IND,
#define rrc_rr_cmd_e_type_value0x700000b RRC_INTERRAT_PLMN_SRCH_REQ,
#define rrc_rr_cmd_e_type_value0x700000c RRC_INTERRAT_PLMN_SRCH_ABORT_REQ,
#define rrc_rr_cmd_e_type_value0x700000d RR_INTERRAT_REDIRECT_REJ,
#define rrc_rr_cmd_e_type_value0x700000e RR_INTERRAT_REDIRECT_ABORT_CNF,
#define rrc_rr_cmd_e_type_value0x700000f RRC_INTERRAT_RESELECTION_ABORT_REQ,
#define rrc_rr_cmd_e_type_value0x7000010 RR_INTERRAT_RESELECTION_ABORT_CNF,
#define rrc_rr_cmd_e_type_value0x7000011 RR_INTERRAT_CC_ORDER_CNF,
#define rrc_rr_cmd_e_type_value0x7000012 RR_INTERRAT_CC_ORDER_ABORT_CNF,
#define rrc_rr_cmd_e_type_value0x7000013 RR_INTERRAT_PLMN_SRCH_CNF,
#define rrc_rr_cmd_e_type_value0x7000014 RR_INTERRAT_PLMN_SRCH_ABORT_CNF
#define rrc_rr_cmd_e_type_value0x7000015 RR_CIPHER_SYNC_IND
#define rrc_rr_cmd_e_type_value0x7000016 RRC_RESELECT_TO_GAN_REQ
#define rrc_rr_cmd_e_type_value0x7000017 RRC_GAN_CELL_INFO_REQ
#define rrc_rr_cmd_e_type_value0x7000018 RRC_RESELECT_FROM_GAN_REQ
#define rrc_rr_cmd_e_type_value0x7000019 RRC_GO_TO_IDLE_DETACHED_REQ
#define rrc_rr_cmd_e_type_value0x700001a RRC_GAN_WAKE_UP_REQ
#define rrc_rr_cmd_e_type_value0x700001b RRC_GAN_ACT_REQ
#define rrc_rr_cmd_e_type_value0x700001c RRC_IMR_FROM_GAN_REQ
#define rrc_rr_cmd_e_type_value0x700001d RRC_CANCEL_HANDOVER_TO_GAN_REQ
#define rrc_rr_cmd_e_type_value0x700001e RRC_INTERRAT_REDIRECT_REQ
#define rrc_rr_cmd_e_type_value0x700001f RRC_INTERRAT_REDIRECT_ABORT_REQ
#define rrc_rr_cmd_e_type_value0x7000020 RRC_RR_STOP_WCDMA_REQ
#define rrc_rr_cmd_e_type_value0x7000021 RR_RRC_STOP_GSM_CNF
#define rrc_rr_cmd_e_type_value0x7000022 RRC_DEDICATED_PRIORITIES_REQ
#define rrc_rr_cmd_e_type_value0x7000023 RR_DEDICATED_PRIORITIES_RSP
#define rrc_rr_cmd_e_type_value0x7000024 RRC_CLEAR_DEDICATED_PRIORITIES_IND

  
typedef struct
{
  sys_modem_as_id_e_type     as_id;
}rrc_rr_as_id_type;

/* Handover status */
typedef enum
{
  RRC_RR_SUCCESS,
  RRC_RR_FAILURE,
  RRC_RR_MAX_STATUS
} rrc_rr_status_e_type;

/* Handover failure causes */
typedef enum
{
  RRC_RR_INVALID_STATE,
  RRC_RR_GSM_PROTOCOL_ERROR,
  RRC_RR_INVALID_CONFIGURATION,
  RRC_RR_PHYSICAL_CHANNEL_FAILURE,
  RRC_RR_MAX_FAILURE_CAUSE,
  RRC_RR_CIPHER_KEY_NOT_AVAILABLE,
  /* Cause to indicate MDSP halt */
  RRC_RR_UNEXPECTED_FAILURE
} rrc_rr_failure_cause_e_type;


/* Inter-RAT (RR to RRC) abort causes */
typedef enum
{
  RR_RRC_PLMN_LIST_REQ_RCVD,
  RR_RRC_PLMN_SELECT_REQ_RCVD,
  RR_RRC_EST_REQ_RCVD,
  RR_RRC_GTOW_RESELECT_REQ_RCVD,
  RR_RRC_ABORT_MAX_STATUS
} rr_rrc_abort_cause_e_type;

/**
 * Used by RR to indicate a list of GSM cells to RRC.  Included in 
 * RR_INTERRAT_REDIRECT_REJ for SERVICE_SEARCH to indicate which ARFCNs
 * are valid GSM cells.
 * And also included in RRC_INTERRAT_PLMN_SRCH_REQ and RR_INTERRAT_PLMN_SRCH_CNF
 * to indicate the ARFCNs on which PLMNs were found.
 */
typedef struct
{
  byte size;
  ARFCN_T cells[MAX_RR_RRC_GSM_CELL_LIST_SIZE];
} rr_rrc_gsm_cell_list_type;



#define PN_OFFSET_UNKNOWN 65535

#define RR_RRC_MAX_INTERRAT_MEASUREMENTS 96

typedef enum 
{ 
  DIVERSITY_OFF, 
  DIVERSITY_ON, 
  DIVERSITY_UNKNOWN=255 
} diversity_T;

typedef struct
{
  word   uarfcn;
  word   primary_scrambling_code; /* 0-511 */
  diversity_T   diversity; 
  uint16 PN_Offset; /* 0-38399 (valid), 65535 (not-known) */
} wcdma_cell_meas_type;

typedef struct 
{
  byte number_of_cells;
  wcdma_cell_meas_type wcdma_cell_measurements[RR_RRC_MAX_INTERRAT_MEASUREMENTS];
} rr_rrc_interrat_measurement_data_type;



/* Handover status */
typedef enum
{
  RR_RRC_SUCCESS,
  RR_RRC_FAILURE,
  RR_RRC_MAX_STATUS
} rr_rrc_status_e_type;

/* RRC Handover failure causes */
typedef enum
{
  RR_RRC_INVALID_STATE,
  RR_RRC_WCDMA_PROTOCOL_ERROR,
  RR_RRC_INVALID_CONFIGURATION,
  RR_RRC_PHYSICAL_CHANNEL_FAILURE,
  RR_RRC_INVALID_CIPHER_ALGORITHM,
  RR_RRC_FREQUENCY_NOT_IMPLEMENTED,
  /*Cause when reselections, OOS search in progress etc*/
  RR_RRC_ACTIVITY_IN_IDLE_DETACHED,
  RR_RRC_MAX_FAILURE_CAUSE
} rr_rrc_failure_cause_e_type;

/*----------------------------------------------------------------------------
                         RRC_INTERRAT_HANDOVER_REQ 
This primitive is sent from RR to RRC to initiate a GSM to WCDMA inter-RAT 
handover. 
----------------------------------------------------------------------------*/

typedef struct
{
  sys_modem_as_id_e_type       as_id;

  /* Cipher Enable Indicator */
  boolean cipher_enabled;

  /* Inter-RAT measurement data */
  rr_rrc_interrat_measurement_data_type interrat_measurement_data;

  /* Ptr to byte stream containing handOverToUTRANCommand*/
  byte ho_cmd[MAX_OCTETS_L3_MESSAGE];

  /* TRK_LO_ADJ */
  uint16 trk_lo_adj;

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  /* PLMNId of the PLMN UE is camped on the GSM side */
  PLMN_id_T  plmn_id;

  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  lte_irat_dedicated_priority_info_s dedicated_priority_info;

  boolean csfb_call_status;
} rrc_interrat_handover_req_type;

/*----------------------------------------------------------------------------
                         RRC_INTERRAT_HANDOVER_CNF
This primitive is sent from RRC to RR to respond the RRC_INTERRAT_HANDOVER_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message header */
  IMH_T message_header;

  sys_modem_as_id_e_type      as_id;

  /* Handover status */
  rr_rrc_status_e_type handover_status;

  /* Handover failure cause: this field is only valid if handover status is
     failure */
  rr_rrc_failure_cause_e_type failure_cause;

} rrc_interrat_handover_cnf_type;

/*----------------------------------------------------------------------------
                         RRC_INTERRAT_HANDOVER_ABORT_REQ 
This primitive is sent from RR to RRC to abort WCDMA to GSM inter-RAT 
handover. 
----------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type      as_id;

  /* Handvoer Abort status */
  rr_rrc_status_e_type abort_status;
} rrc_interrat_handover_abort_req_type;

/*----------------------------------------------------------------------------
                         RRC_INTERRAT_HANDOVER_ABORT_CNF
This primitive is sent from RRC to RR in response to
RRC_INTERRAT_HANDOVER_ABORT_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message header */
  IMH_T message_header;

  sys_modem_as_id_e_type      as_id;
} rrc_interrat_handover_abort_cnf_type;

/*----------------------------------------------------------------------------
                         RRC_CIPHER_SYNC_IND
 This primitive is sent from RRC to RR to indicated
   that the new keys are activated
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message header */
  IMH_T message_header;
  sys_modem_as_id_e_type as_id;
} rrc_cipher_sync_ind_type;

/*----------------------------------------------------------------------------
                         RRC_NAS_DATA_IND
This primitive is sent from RR to RRC to forward bufferred NAS signaling 
message. If there is not message left to send, RR will send the primitive to 
RR with the message length field set to 0. 
----------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type       as_id;

  /* NAS signalling data */
  SAPI_T_TEMP       SAPI;
  byte              l3_message_length;
  byte              l3_message[MAX_OCTETS_L3_MESSAGE];
} rrc_nas_data_ind_type;


/* Frequence band indicator type */
typedef enum
{
  RRC_RR_DCS_1800,
  RRC_RR_PCS_1900,
  RRC_RR_MAX_BANDS
} rrc_rr_frequence_band_e_type;

/* Inter RAT WCDMA to GSM Type definition */
typedef enum
{
  RR_GSM_OTHER_REASON,
  RR_GSM_CELL_BARRED,
  RR_GSM_CELL_FORBIDDEN,
  RR_GSM_PLMN_MISMATCH,
  RR_GSM_CELL_LOW_C1,
  RR_GSM_BAND_NOT_SUPPORTED,
  RR_GSM_BSIC_NOT_FOUND,
  RR_GSM_BCCH_NOT_FOUND,
  RR_GSM_BCCH_DECODE_FAIL,
  RR_GSM_INVALID_STATE
} rr_interrat_reselect_fail_reason_type;


/* Inter RAT WCDMA to GSM Type definition */
typedef enum
{
  RR_GSM_CCO_OTHER_REASON,
  RR_GSM_CCO_CELL_BARRED,
  RR_GSM_CCO_CELL_FORBIDDEN,
  RR_GSM_CCO_CELL_LOW_C1,
  RR_GSM_CCO_BAND_NOT_SUPPORTED,
  RR_GSM_CCO_BSIC_NOT_FOUND,
  RR_GSM_CCO_BCCH_NOT_FOUND,
  RR_GSM_CCO_BCCH_DECODE_FAIL,
  RR_GSM_CCO_T309_EXPIRED,
  RR_GSM_CCO_INVALID_STATE
} rr_interrat_cc_order_fail_reason_type;


/*  NC Mode types */
typedef enum
{
  NC_MODE_0,
  NC_MODE_1,
  NC_MODE_2
} nc_mode_e_type;

typedef enum
{
  RRC_WCDMA_NONE,
  RRC_WCDMA_INAVALID_STATE,
  RRC_WCDMA_CELL_BARRED,
  RRC_WCDMA_UARFCN_BARRED,
  RRC_WCDMA_PLMN_MISMATCH,
  RRC_WCDMA_LOW_S_VALUE,
  RRC_WCDMA_ACQUISITION_FAIL,
  RRC_WCDMA_SIBS_FAILED_LOWER_LAYERS,
  RRC_WCDMA_INVALID_SIBS,
  RRC_WCDMA_SIBS_TIMEOUT,
  RRC_WCDMA_SIBS_FAILED_OTHER,
  RRC_WCDMA_FORBIDDEN_LA,
  RRC_WCDMA_CELL_QUALITY_FAILURE,
  RRC_WCDMA_CELL_NOT_SUITABLE,
  RRC_WCDMA_CELL_NOT_EVALUATED,
  RRC_WCDMA_CELL_CHANGE_FAILURE,
  RRC_WCDMA_T3174_EXPIRED,
  RRC_WCDMA_MAX_RACH_ATTEMPTS,
  RRC_WCDMA_CONNECTION_REJECT,
  RRC_WCDMA_CSG_FORBIDDEN
} rrc_interrat_reject_reason_T;

/* Inter-RAT measurement data type */
typedef struct 
{  
  /* GSM ARFCN */
  ARFCN_T bcch_arfcn;

  /* Frame offset */
  uint32 fn_offset;
  
  /* qbit offset */
  uint16 qbit_offset;

  /* GSM BSIC */
  uint8 bsic_ncc;
  uint8 bsic_bcc;

  /* RSSI */
  int16  rssi_dBm_x16;

  /* frequency offset */
  int16  frequency_offset;

  /*Srxlev of the cell */
  int16 srxlev;

  /* Rank of the cell */
  int16 rank;

} rrc_rr_interrat_measurement_type;

/* Inter-RAT measurement data list type */
typedef struct
{
  /* number of cells */
  uint8 number_of_cells;

  /* Inter-RAT measurement data */
  rrc_rr_interrat_measurement_type interrat_measurements[RRC_RR_MAX_INTERRAT_MEASUREMENTS];
} rrc_rr_interrat_measurement_data_type;

/* GSM command type */
typedef struct
{
  /* Length of the command */
  uint8 message_length;
 
  /* Message payload */
  uint8 payload[RRC_RR_MAX_GSM_MESSAGE_LENGTH];
} rrc_rr_gsm_command_type;

/* GSM command list type */
typedef struct
{
  /* Number of the GSM Commands */
  uint8 number_of_commands;
 
  /* GSM command list */
  rrc_rr_gsm_command_type gsm_commands[RRC_RR_MAX_GSM_COMMANDS];
} rrc_rr_gsm_command_list_type;         

typedef enum
{
  RR_RRC_REDIRECT_FAIL_OTHER_REASON,
  RR_RRC_REDIRECT_FAIL_WAIT_TIMER_EXPIRY,
  RR_RRC_REDIRECT_FAIL_NO_SUITABLE_CELL_FOUND,
  RR_RRC_REDIRECT_FAIL_MAX
} rr_interrat_redirect_fail_reason_type;

/* GERAN SysInfo type identifier */
typedef enum 
{
  GERAN_INFO_TYPE_NONE,
  GERAN_INFO_TYPE_SI,
  GERAN_INFO_TYPE_PSI
} geran_sysinfo_choice_e_type;

/* ===========================================================================
**                       RRC - RR Primitive Type Definitions 
** ======================================================================== */
/*----------------------------------------------------------------------------
                         RR_INTERRAT_HANDOVER_REQ 
This primitive is sent from RRC to RR to initiate a WCDMA to GSM inter-RAT 
handover. 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message header */
  IMH_T message_header;

  sys_modem_as_id_e_type       as_id;
  /* Frequence band indicator */
  rrc_rr_frequence_band_e_type frequency_band;

  /* Cipher Enable Indicator */
  boolean cipher_enabled;

  /* Inter-RAT measurement data */
  rrc_rr_interrat_measurement_data_type interrat_measurement_data;

  /* GSM Commands: it should only include one command: GSM handover command */
  rrc_rr_gsm_command_list_type gsm_command_list;

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  lte_irat_dedicated_priority_info_s dedicated_priority_info;

  boolean csfb_call_status;
} rr_interrat_handover_req_type;

/*----------------------------------------------------------------------------
                         RR_INTERRAT_HANDOVER_CNF
This primitive is sent from RR to RRC to respond the RR_INTERRAT_HANDOVER_REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type      as_id;

  /* Handover status */
  rrc_rr_status_e_type handover_status;

  /* Handover failure cause: this field is only valid if handover status is
     failure */
  rrc_rr_failure_cause_e_type failure_cause;

  /* GSM commands: this field is only filled if handover is failure */
  rrc_rr_gsm_command_list_type gsm_command_list;
} rr_interrat_handover_cnf_type;

/*----------------------------------------------------------------------------
                         RR_INTERRAT_HANDOVER_ABORT_REQ 
This primitive is sent from RRC to RR to abort WCDMA to GSM inter-RAT 
handover. 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message header */
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rr_interrat_handover_abort_req_type;

/*----------------------------------------------------------------------------
                         RR_INTERRAT_HANDOVER_ABORT_CNF
This primitive is sent from RR to RRC in response to
RR_INTERRAT_HANDOVER_ABORT_ REQ 
----------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type     as_id;

  /* Handvoer Abort status */
  rrc_rr_status_e_type abort_status;
} rr_interrat_handover_abort_cnf_type;

/*----------------------------------------------------------------------------
                         RR_NAS_DATA_IND
This primitive is sent from RRC to RR to forward bufferred NAS signaling 
message. If there is not message left to send, RRC will send the primitive to 
RR with the message length field set to 0. 
----------------------------------------------------------------------------*/
typedef struct
{
  /* Message Header */
  IMH_T message_header;

  sys_modem_as_id_e_type       as_id;

  /* NAS signalling data */
  rrc_rr_gsm_command_type nas_data;
} rr_nas_data_ind_type;

/**********************************************************/
/* RR_INTERRAT_RESELECT_REQ primitive :                   */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Request the InterRAT Reselection to GSM cell           */
/**********************************************************/
typedef struct
{
   IMH_T                          message_header;

   sys_modem_as_id_e_type     as_id;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type network_select_mode;

   /* indicates the Target GSM Cell BCCH ARFCN */
   ARFCN_T                      BCCH_ARFCN;

   /* indicates the Target GSM Cell BSIC */
   uint8                          BSIC_NCC;
   uint8                          BSIC_BCC;

   /* RXLev Average of the GSM Target Cell */
   byte                           RXLEV_average;

   /* PLMNId of the requested PLMN */
   PLMN_id_T                      requested_PLMN;

   sys_rat_pri_list_info_s_type   rat_pri_list_info;

   lte_irat_dedicated_priority_info_s dedicated_priority_info;

} rr_interrat_reselect_req_type;


/**********************************************************/
/* RR_INTERRAT_RESELECTION_ABORT_REQ primitive:           */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Abort an ongoing W2G reselection.                      */
/**********************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rr_interrat_reselection_abort_req_type;

/********************************************************************/
/* RR_INTERRAT_RESELECTION_ABORT_CNF primitive :                    */
/* FROM :  RR                                                       */
/* TO   :  RRC                                                      */
/* DESCRIPTION :                                                    */
/* Confirm from RR to RRC that on-going WTOG reselection is aborted */
/********************************************************************/
typedef rrc_rr_as_id_type rr_interrat_reselection_abort_cnf_type;

/**********************************************************/
/* RR_INTERRAT_RESELECT_REJ primitive :                   */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Response from RR regarding InterRAT Reselection (Fail) */
/**********************************************************/
typedef struct
{
   sys_modem_as_id_e_type     as_id;

   /* Cause of InterRAT Reselection Failure */
   rr_interrat_reselect_fail_reason_type failure_reason;

   /* Time ( in Sec ) when the same cell should be considered */
   word                        time_to_reselect;

   /* Expected RXLev Average of the GSM Target Cell */
   byte                        Expected_RXLEV_average;
} rr_interrat_reselect_rej_type;


/* Geran SI block structure */
typedef struct
{
  // Length could be anywhere between 1 and 23 bytes
  uint8 msg_length;                       

  // The actual SI block message
  byte  msg_string[RRC_RR_MAX_SI_BLOCK_LENGTH];    

} geran_SI_block_type; 

/**********************************************************/
/* RR_INTERRAT_CC_ORDER_REQ primitive :                   */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Request the InterRAT CC Order to GSM cell              */
/**********************************************************/
typedef struct
{
   IMH_T                          message_header;

   sys_modem_as_id_e_type     as_id;

   /* indicates NC Mode is present */
   boolean                        NC_Mode_present;

   /*  NC Mode */

   nc_mode_e_type                 NC_Mode;

   /* indicates the Target GSM Cell BCCH ARFCN */
   ARFCN_T                          BCCH_ARFCN;

   /* indicates the Target GSM Cell BSIC */
   uint8                          BSIC_NCC;
   uint8                          BSIC_BCC;

   /* RXLev Average of the GSM Target Cell */
   byte                           RXLEV_average;

   /* Timer T309 Value */
   uint32                         T309_value;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type network_select_mode;

   /* Type of Geran SysInfo */
   geran_sysinfo_choice_e_type geran_sysinfo_choice;

   /* Number of SI blocks in Geran SysInfo*/
   uint8  num_blocks; 

   /* Geran SysInfo blocks */
   geran_SI_block_type geran_SI[RRC_RR_MAX_SI_BLOCKS];

   sys_rat_pri_list_info_s_type   rat_pri_list_info;

   lte_irat_dedicated_priority_info_s dedicated_priority_info;

} rr_interrat_cc_order_req_type;


/**********************************************************/
/* RR_INTERRAT_CC_ORDER_REJ primitive :                   */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/* Response from RR regarding InterRAT Reselection (Fail) */
/**********************************************************/
typedef struct
{
   sys_modem_as_id_e_type     as_id;

   /* Cause of InterRAT CC Order Failure */
   rr_interrat_cc_order_fail_reason_type failure_reason;

} rr_interrat_cc_order_rej_type;

/**********************************************************/
/* RRC_INTERRAT_RESELECT_REQ primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/* Request the InterRAT Reselection to UTRAN cell         */
/**********************************************************/
typedef struct
{

   sys_modem_as_id_e_type     as_id;

   /* The Frequency of the Target UTRAN Cell */
   uint32                               UARFCN;

   /* The Scrambling code of the target UTRAN Cell */
   uint32                               scrambling_code;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type network_select_mode;

   /* PLMNId of the requested PLMN */
   PLMN_id_T                            requested_PLMN;

   /* TRK_LO_ADJ */
   uint16 trk_lo_adj;
   /*boolean indicating if UE is in GAN mode*/
   boolean ue_in_gan_mode;

   sys_rat_pri_list_info_s_type   rat_pri_list_info;

  lte_irat_dedicated_priority_info_s dedicated_priority_info;

  uint16 PN_Offset; /* 0-38399 (valid), 65535 (not-known) */

} rrc_interrat_reselect_req_type;


/****************************************************************/
/* RRC_INTERRAT_RESELECTION_ABORT_REQ primitive :               */
/* FROM :  RR                                                   */
/* TO   :  RRC                                                  */
/* DESCRIPTION :                                                */
/* Request from RR to RRC to abort an on-going GTOW reselection */
/****************************************************************/
typedef rrc_rr_as_id_type rrc_interrat_reselection_abort_req_type;

/**********************************************************/
/* RRC_INTERRAT_RESELECT_REJ primitive :                   */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Response from RR regarding InterRAT Reselection (Fail) */
/**********************************************************/
typedef struct
{

   IMH_T                          message_header;

   sys_modem_as_id_e_type     as_id;

   /* Cause of InterRAT Reselection Failure */
   rrc_interrat_reject_reason_T   reject_reason;

   /* Time ( in Sec ) when the same cell should be considered */
   uint32                         time_to_reselect;

   /* Thresholds to be applied to Ec/No measurements */
   int16                         squalmin;

   int16                         srxlevmin;

   /* PLMN Id of the WCDMA cell */
   PLMN_id_T                     PLMN;

   /* LAC */
   uint8                         forbidden_lac[2];

} rrc_interrat_reselect_rej_type;


/**********************************************************/
/* RRC_INTERRAT_RESELECTION_ABORT_CNF primitive:          */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* RRC confirms that the IRAT reselection has been aborted*/
/**********************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rrc_interrat_reselection_abort_cnf_type;


/**********************************************************/
/* RRC_INTERRAT_CC_ORDER_REQ primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/* Request the InterRAT PCCO  to UTRAN cell               */
/**********************************************************/
typedef struct
{

   sys_modem_as_id_e_type     as_id;

   /* The Frequency of the Target UTRAN Cell */
   uint16                               UARFCN;

   /* The Scrambling code of the target UTRAN Cell */
   uint16                               scrambling_code;

   /* Indicates how a PLMN is to be selected. */
   sys_network_selection_mode_e_type network_select_mode;

   /* PLMNId of the requested PLMN */
   PLMN_id_T                            requested_PLMN;

   /* Timer T3174 */
   uint32                               t3174;

   sys_rat_pri_list_info_s_type   rat_pri_list_info;

   lte_irat_dedicated_priority_info_s dedicated_priority_info;

} rrc_interrat_cc_order_req_type;

/**********************************************************/
/* RRC_INTERRAT_CC_ORDER_REJ primitive :                  */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Response from RR regarding InterRAT CC Order (Fail)    */
/**********************************************************/
typedef struct
{

   IMH_T                          message_header;

   sys_modem_as_id_e_type     as_id;

   /* Cause of InterRAT Reselection Failure */
   rrc_interrat_reject_reason_T   reject_reason;

   /* Time ( in Sec ) when the same cell should be considered */
   uint32                         time_to_reselect;

   /* Thresholds to be applied to Ec/No measurements */
   int16                         squalmin;

   int16                         srxlevmin;

   /* PLMN Id of the WCDMA cell */
   PLMN_id_T                     PLMN;

   /* LAC */
   uint8                         forbidden_lac[2];
} rrc_interrat_cc_order_rej_type;

/**********************************************************/
/* RRC_INTERRAT_PLMN_SRCH_REQ primitive :                 */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/* Request WCDMA to do PLMN Search                        */
/**********************************************************/
typedef struct
{
  sys_modem_as_id_e_type     as_id;

  uint32       num_plmns;
  /* PLMN Ids to be searched on 3G with 0th element being
   * the highest priority PLMN */
  PLMN_id_T  plmn[SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

   /* Time allowed by GL1 for WCDMA to be active */
  uint32 timer_val;

  /* Indicates the list of ARFCNs that are valid GSM cells */
  rr_rrc_gsm_cell_list_type gsm_cell_list;
  /* Used by RRC to determine if the plmn search request
   * is for "full service search" or "plmn list search"
   */
  sys_service_search_e_type service_search;

  boolean camped_suitably;
  detected_frequency_list_type detected_frequency_list;
  
  /* Sent by RR so that WRRC can determine if the plmn search request
   * is for "full band search" or "ACQ_DB Search"
   *
   * RR originally receives the enum value from NAS
   */  
  sys_scan_scope_e_type   scan_scope;

} rrc_interrat_plmn_srch_req_type;


/**********************************************************/
/* RRC_INTERRAT_PLMN_SRCH_CNF primitive :                 */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Confirm sent by RRC to RR in response to PLMN Search   */
/* request                                                */
/**********************************************************/

typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;

  /* List of higher priority PLMNs that were found on 3G */
  sys_detailed_plmn_list_s_type available_plmn_list;
  /* Indicates if the BPLMN search is done or not */
  boolean srch_done;
  /* Indicates if the highest priority PLMN was found or not */
  boolean hplmn_found;
} rrc_interrat_plmn_srch_cnf_type;


/**********************************************************/
/* RRC_INTERRAT_PLMN_SRCH_ABORT_REQ primitive :                 */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/* Request from RR to RRC to abort the PLMN Search        */
/**********************************************************/
typedef struct
{
  sys_modem_as_id_e_type     as_id;

  /* Indicates why the search is being aborted */
  rr_rrc_abort_cause_e_type abort_cause;
} rrc_interrat_plmn_srch_abort_req_type;

/**********************************************************/
/* RRC_INTERRAT_CC_ORDER_ABORT_REQ primitive :                 */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/* Request from RR to RRC to abort the CCO        */
/**********************************************************/
typedef struct
{

  sys_modem_as_id_e_type    as_id;

}rrc_interrat_cc_order_abort_req_type;


/**********************************************************/
/* RRC_INTERRAT_PLMN_SRCH_ABORT_CNF primitive :           */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Confirm sent by RRC to RR in response to PLMN Search   */
/* abort request                                          */
/**********************************************************/

typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rrc_interrat_plmn_srch_abort_cnf_type;

/**********************************************************/
/* RR_INTERRAT_PLMN_SRCH_REQ primitive :                  */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Request GSM to do PLMN Search                          */
/**********************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;

  uint32     num_plmns;
  /* PLMN Ids to be searched on 2G with 0th element being
   * the highest priority PLMN */
  PLMN_id_T  plmn[SYS_PRIORITY_PLMN_LIST_MAX_LENGTH];

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type network_select_mode;

   /* Time allowed by GL1 for WCDMA to be active */
  uint32 timer_val;

  /* Sent to RR so that RR can determine if the plmn search request
   * is for "full service search" or "plmn list search"
   *
   * RRC originally receives the enum value from NAS
   * in the PLMN_LIST_REQ
   */
  sys_service_search_e_type service_search;
  
  /* Sent to RR so that RR can determine if the plmn search request
   * is for "full band search" or "ACQ_DB Search"
   *
   * RRC originally receives the enum value from NAS
   * in the PLMN_LIST_REQ
   */
  sys_scan_scope_e_type   scan_scope;
} rr_interrat_plmn_srch_req_type;


/**********************************************************/
/* RR_INTERRAT_PLMN_SRCH_CNF primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/* Confirm sent by RR to RRC in response to PLMN Search   */
/* request                                                */
/**********************************************************/

typedef struct
{
  sys_modem_as_id_e_type     as_id;

  /* Indicates if the BPLMN search is done or not */
  boolean srch_done;

  /* Indicates if the highest priority PLMN was found or not */
  boolean hplmn_found;

  /* List of higher priority PLMNs that were found on 3G */
  sys_detailed_plmn_list_s_type available_plmn_list;

  /* Indicates the list of ARFCNs that are valid GSM cells */
  rr_rrc_gsm_cell_list_type gsm_cell_list;

} rr_interrat_plmn_srch_cnf_type;

/***************************************************************/
/* RR_INTERRAT_PLMN_SRCH_ABORT_CNF primitive :                 */
/* FROM :  RR                                                  */
/* TO   :  RRC                                                 */
/* DESCRIPTION :                                               */
/* Confirm sent by RR to RRC in response to PLMN Search Abort  */
/* request                                                     */
/***************************************************************/

typedef rrc_rr_as_id_type rr_interrat_plmn_srch_abort_cnf_type;

/**********************************************************/
/* RR_INTERRAT_PLMN_SRCH_ABORT_REQ primitive :            */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Request from RRC to RR to abort the PLMN Search        */
/**********************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rr_interrat_plmn_srch_abort_req_type;

/**********************************************************/
/* RR_INTERRAT_PLMN_SRCH_SUSPEND_IND primitive :          */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Request from RRC to RR to suspend the PLMN Search      */
/**********************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rr_interrat_plmn_srch_suspend_ind_type;

#define MAX_RRC_RR_GSM_CELL_LIST_SIZE 32

typedef struct
{
  uint8                          BSIC_NCC;
  uint8                          BSIC_BCC;
} BSIC_type;

typedef struct
{
  uint32 size;
  ARFCN_T                     arfcns[MAX_RRC_RR_GSM_CELL_LIST_SIZE];

  BSIC_type                   BSIC[MAX_RRC_RR_GSM_CELL_LIST_SIZE];
} rrc_rr_gsm_cell_list_type;


/* Used to inform RR if cell search on GSM is being 
 * done due to inter-RAT redirection via 
 * RRC connection reject (RRC_RR_REDIRECTION_SEARCH),
 * or due to the fact that UE is Out of Service in 
 * Connected Mode (RRC_RR_OUT_OF_SERVICE_SEARCH)
 */
typedef enum
{
  RRC_RR_REDIRECTION_SEARCH,
  RRC_RR_OUT_OF_SERVICE_SEARCH,
  RRC_RR_SERVICE_SEARCH,
  RRC_RR_REDIRECTION_AFTER_CONN_RELEASE
} rrc_rr_cell_srch_reason_e_type;


/*********************************************************************
 * RR_INTERRAT_REDIRECT_REQ (RRC->RR)
 *
 * Sent from RRC to RR to request service redirection.
 *********************************************************************/

typedef struct
{
  IMH_T                             message_header;

  sys_modem_as_id_e_type     as_id;

  /* PLMN of the WCDMA cell */
  PLMN_id_T                         PLMN;  

  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type network_select_mode;

  /* Time for which GSM can look for service */
  uint32                            wait_timer;

  /* Establishment cause - Emergency call or not */
  boolean                           is_emergency_call;

  /* GSM neighbor list that will be searched first before cell selection
   * is triggered */
  rrc_rr_gsm_cell_list_type         gsm_ncell_list;

  rrc_rr_cell_srch_reason_e_type    cell_srch_reason;


  /* Used by RR to determine if service search should be done according to the uniform
   * out of service operation or according to the default behaviour.
   * TRUE: Uniform OOS operation needs to be followed. 
   * FALSE: Otherwise
   */
  boolean                           uoos_gsm_use_timer;  

  /* Used by RR to determine if service search needs to be started from the beginning
   * or it should be continued. If it is set to TRUE then no stored information related to 
   * power scanning should be used by RR; otherwise stored information should be used. 
   * Default value is TRUE.
   */
  boolean                           uoos_gsm_new_scan;

  /* Timer in milliseconds that determines how long RR should search for 
   * service on G
   */
  uint32                            uoos_gsm_scan_time; 

  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  lte_irat_dedicated_priority_info_s dedicated_priority_info;

  boolean is_ehplmn_to_be_camped;

} rr_interrat_redirect_req_type;

/*********************************************************************
 * RR_INTERRAT_REDIRECT_REJ (RR->RRC)
 *
 * Sent to RRC by RR to indicate that service redirection has failed.
 *********************************************************************/
typedef struct
{
  sys_modem_as_id_e_type     as_id;

  byte       num_avail_plmns;

  /* PLMN Ids of GSM cells found */
  PLMN_id_T  plmn[SYS_PLMN_LIST_MAX_LENGTH];

  /* Cause of InterRAT Redirection Failure */
  rr_interrat_redirect_fail_reason_type failure_reason;

  /* Signal quality */
  sys_signal_quality_e_type  signal_quality[SYS_PLMN_LIST_MAX_LENGTH];

  /* Signaling strength of the strongest cell in PLMN */
  sys_signal_strength_type  signal_strength[SYS_PLMN_LIST_MAX_LENGTH];

  byte          num_available_fplmns;
  /* Indicates the list of ARFCNs that are valid GSM cells */
  rr_rrc_gsm_cell_list_type gsm_cell_list;
  /* This variable is used by RR to inform RRC whether GSM
   * search was completed or not in the alloted time.
   * TRUE  => Search Done.
   * FALSE => Otherwise.
   * The value of this variable is valid only when uoos_gsm_use_timer
   * in RR_INTERRAT_REDIRECT_REQ is set to TRUE 
   */
  boolean uoos_gsm_search_done;

} rr_interrat_redirect_rej_type;



/*********************************************************************
 * RR_INTERRAT_REDIRECT_ABORT_REQ (RRC->RR)
 *
 * This command is sent by RRC to RR asking it to abort the Inter-RAT 
 * redirection procedure,
 *********************************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rr_interrat_redirect_abort_req_type;



/*********************************************************************
 * RR_INTERRAT_REDIRECT_ABORT_CNF (RR->RRC)
 *
 * This command is sent by RR to RRC. It will inform RRC if GSM side 
 * was aborted successfully or not.
 *********************************************************************/
typedef struct
{
  sys_modem_as_id_e_type     as_id;

  rrc_rr_status_e_type abort_status;

  /* Timer in milliseconds that stores the remaining GSM search time.
   * This time is used to compute the total remaining awake time, i.e., W + G,  
   * that determines how long UE should search for service in
   * Idle mode after Releasing the RRC Connection.
   */                        
  uint32 uoos_rem_gsm_awake_time;

} rr_interrat_redirect_abort_cnf_type;

/**********************************************************/
/* RR_INTERRAT_CC_ORDER_CNF primitive :                   */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/* Response from RR regarding InterRAT CC Order           */
/**********************************************************/
typedef enum
{
  CCO_SUCCESS,
  CCO_FAILURE_BEFORE_ACT_IND,
  CCO_FAILURE_AFTER_ACT_IND,
  CCO_ABORTED_DUE_TO_HIGH_PRI_REQ
} rr_interrat_cc_order_status_e_type;

typedef struct
{
  sys_modem_as_id_e_type     as_id;

  /* WTG CCO Status */
  rr_interrat_cc_order_status_e_type status;

  /* Failure cause if the status is not Success */
  rr_interrat_cc_order_fail_reason_type failure_reason;

} rr_interrat_cc_order_cnf_type;
typedef enum
{
  RR_WL1_STOP_IND_NO_CAUSE,
  RR_WL1_STOP_IND_W2G_CCO  
}rr_wl1_stopped_cause_type;

typedef struct
{
  IMH_T    message_header;
  sys_modem_as_id_e_type     as_id;  
  rr_wl1_stopped_cause_type cause;
}rr_interrat_wl1_stop_ind_type;


/**********************************************************/
/* RR_INTERRAT_CC_ORDER_ABORT_REQ primitive :             */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Request to Abort the W->G  CC Order                    */
/**********************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rr_interrat_cc_order_abort_req_type;

/**********************************************************/
/* RR_INTERRAT_CC_ORDER_ABORT_CNF primitive :             */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/* Response from RR regarding InterRAT CC Order Abort     */
/**********************************************************/
typedef struct
{
  sys_modem_as_id_e_type     as_id;

  /* Status of the Abort request */
   rrc_rr_status_e_type abort_status;
} rr_interrat_cc_order_abort_cnf_type;

/**********************************************************/
/* RRC_INTERRAT_CC_ORDER_CNF primitive :                  */
/* FROM :  RRC                                            */
/* TO   :  RR                                             */
/* DESCRIPTION :                                          */
/* Response from RRC regarding InterRAT CC Order Req      */
/**********************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;

  /* GTW PCCO Status */
  rr_interrat_cc_order_status_e_type status;

  /* Failure cause if the status is not Success */
  rrc_interrat_reject_reason_T failure_reason;

} rrc_interrat_cc_order_cnf_type;



/* WCDMA freq type that is passed to GSM-RR */
typedef uint16 rrc_rr_freq_type;

/* WCDMA channel info that is sent to GSM-RR */
typedef struct
{
  rrc_rr_freq_type      freq; /* Acquired Frequency */
  sys_band_mask_e_type  band; /* Band corresponding to the frequency */
} rrc_chnl_info_type;

/* Structure in which WCDMA acq channel info is sent to GSM-RR */
typedef struct
{
  uint32                num_chnls; /* # of WCDMA channels */
  rrc_chnl_info_type    chnl_info[RRC_RR_MAX_NUM_FREQ]; /* contains freq and band info */
} rrc_rr_wcdma_acq_chnl_info_type;

typedef struct
{
  /* A new variable is added that stores lai and PLMN information
   * together */
  inter_task_lai_T    curr_lai;

  uint32    cell_id;

}rrc_gan_serving_cell_info_type;


typedef enum
{
  RRC_MODE_RRC_NO_SERVICE,
  RRC_MODE_RRC_EMI,
  RRC_MODE_RRC_IDLE,
  RRC_MODE_RRC_CELL_DCH_CS,
  RRC_MODE_RRC_CELL_DCH_PS,
  RRC_MODE_RRC_CELL_DCH_CS_PS,
  RRC_MODE_RRC_CELL_FACH_CS,
  RRC_MODE_RRC_CELL_FACH_PS,
  RRC_MODE_RRC_CELL_PCH,
  RRC_MODE_RRC_URA_PCH ,
  RRC_MODE_RRC_INACTIVE 
} rrc_curr_operating_mode_e_type;


typedef enum
{
  BUFFER_RESEL_REQ = 0,
  FAIL_RESEL_REQ
} rrc_resel_to_gan_reject_cause_e_type;

/**********************************************************/
/* RR_GAN_ACT_IND primitive :                  */
/* FROM :  RRC                                             */
/* TO   :  RR                                            */
/* DESCRIPTION :                                          */
/* Report the NAS requested PLMN id and nw select mode         */
/**********************************************************/
typedef struct
{
  IMH_T    message_header;

  /* Indicates how a PLMN is to be selected. */
  sys_network_selection_mode_e_type    network_select_mode;

  /* PLMNId of the requested PLMN */
  PLMN_id_T    requested_PLMN;

} rr_gan_activation_ind_type;


/**********************************************************/
/* RR_GAN_MODE_IND primitive :                  */
/* FROM :  RRC                                             */
/* TO   :  RR                                            */
/* DESCRIPTION :                                          */
/* Report the current service status         */
/**********************************************************/
typedef struct
{
  IMH_T    message_header;

  rrc_curr_operating_mode_e_type      rrc_curr_mode;

} rr_gan_mode_ind_type;


/**********************************************************/
/* RRC_GAN_CELL_INFO_CNF primitive :                  */
/* FROM :  RRC                                             */
/* TO   :  RR                                            */
/* DESCRIPTION :                                          */
/* Report the current WCDMA service status. 
 * Sent in response to RRC_GAN_CELL_INFO_REQ */
/**********************************************************/
typedef struct
{
  IMH_T    message_header;

  rrc_curr_operating_mode_e_type      rrc_curr_mode;

  /* Below field is valid if rrc_curr_mode is not RRC_MODE_RRC_NO_SERVICE */
  rrc_gan_serving_cell_info_type    serving_cell_info;

} rrc_gan_cell_info_cnf_type;


/**********************************************************/
/* RRC_RESELECT_TO_GAN_CNF primitive :                  */
/* FROM :  RRC                                             */
/* TO   :  RR                                            */
/* DESCRIPTION :                                          */
/* Indicate willingness to accept InterRAT Reselection to GAN cell         */
/**********************************************************/
typedef struct
{
  IMH_T    message_header;
} rrc_reselect_to_gan_cnf_type;


/**********************************************************/
/* RRC_RESELECT_TO_GAN_REJ primitive :                  */
/* FROM :  RRC                                             */
/* TO   :  RR                                            */
/* DESCRIPTION :                                          */
/* Reject InterRAT Reselection to GAN cell, RR buffers the request         */
/**********************************************************/
typedef struct
{
  IMH_T    message_header;

  rrc_resel_to_gan_reject_cause_e_type    reject_cause;
} rrc_reselect_to_gan_rej_type;

/**********************************************************/
/* RRC_RESELECT_FROM_GAN_REQ primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/*  Reselect from GAN request        */
/**********************************************************/
typedef struct
{
  /* PLMN of the WCDMA cell */
  PLMN_id_T    PLMN;  
  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type    network_select_mode;
} rrc_reselect_from_gan_req_type;

typedef enum
{
  FAIL_INVALID_ROVE_OUT_REQ=0,
  FAIL_ROVE_OUT_REQ,
  BUFFER_ROVE_OUT_REQ
} rrc_resel_from_gan_reject_cause_e_type;

/**********************************************************/
/* RRC_RESELECT_FROM_GAN_REJ primitive :                  */
/* FROM :  RRC                                             */
/* TO   :  RR                                            */
/* DESCRIPTION :                                          */
/*  Response for RRC_RESELECT_FROM_GAN_REQ        */
/**********************************************************/
typedef struct
{
  IMH_T    message_header;

  rrc_resel_from_gan_reject_cause_e_type    reject_cause;
} rrc_reselect_from_gan_rej_type;

/**********************************************************/
/* RRC_GO_TO_IDLE_DETACHED_REQ primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/*  Request for taking WCDMA to idle detached mode        */
/**********************************************************/
typedef struct
{
  /* PLMN of the WCDMA cell */
  PLMN_id_T    PLMN;  
  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type    network_select_mode;
} rrc_go_to_idle_detached_req_type;


/**********************************************************/
/* RRC_GAN_WAKE_UP_REQ primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/*  Request for waking up WCDMA         */
/**********************************************************/
typedef struct
{
  /* PLMN of the WCDMA cell */
  PLMN_id_T    PLMN;  
  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type    network_select_mode;
} rrc_gan_wake_up_req_type;

/**********************************************************/
/* RRC_GAN_ACT_REQ primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/*  Request for selecting a specific PLMN         */
/**********************************************************/
typedef struct
{
  /* PLMN of the WCDMA cell */
  PLMN_id_T    PLMN;  
  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type    network_select_mode;
} rrc_gan_act_req_type;

/**********************************************************/
/* RRC_GAN_ACT_REJ primitive :                  */
/* FROM :  RRC                                             */
/* TO   :  RR                                            */
/* DESCRIPTION :                                          */
/*  Response to RRC_GAN_ACT_REQ        */
/**********************************************************/
typedef struct
{
  IMH_T    message_header;
} rrc_gan_act_rej_type;

/**********************************************************/
/* RRC_IMR_FROM_GAN_REQ primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/*  Request for sending mesurement report for GAN cell         */
/**********************************************************/
typedef struct
{
  /* indicates the GAN Cell BCCH ARFCN */
  uint16    BCCH_ARFCN;

  /* indicates the GAN Cell BSIC */
  uint8    BSIC_NCC;
  uint8    BSIC_BCC;
  /*Indicates if GU is preferred mode or not*/
  boolean is_gu_preferred_mode;
} rrc_imr_from_gan_req_type;

/* Note: MAX_UTRAN_FDD_CELLS  and MAX_UTRAN_FDD_FREQUNECIES are arbitrary values 
 * chosen because the MS must be able to monitor this many FDD neighbor cells. 
 * This is also because the CSN decoder imposes the same constraints. 
 * There is no limit (except message length) imposed on how many FDD neighour
 * cells might appear in the message.
*/

#define MAX_UTRAN_FDD_CELLS    32
#define MAX_UTRAN_FDD_FREQUNECIES    8

typedef struct
{
  uint32    scrambling_code;
  boolean    diversity;
}rr_rrc_fdd_cell_type;

typedef struct
{
  uint32    uarfcn;
  uint8    num_of_cells;
  rr_rrc_fdd_cell_type    cell_list [MAX_UTRAN_FDD_CELLS];
} rr_rrc_fdd_cell_list_type;

typedef struct
{
  uint8    num_of_frequencies;
  rr_rrc_fdd_cell_list_type    fdd_cell_list[MAX_UTRAN_FDD_FREQUNECIES];
} rr_rrc_fdd_target_info_type;

/****************************************************/
/* RRC_INTERRAT_REDIRECT_REQ primitive :            */
/* FROM :  RR                                       */
/* TO   :  RRC                                      */
/* DESCRIPTION :                                    */
/*  Redirection request from GSM to WCDMA           */
/****************************************************/
typedef struct
{
  sys_modem_as_id_e_type     as_id;
  /* PLMN of the WCDMA cell */
  PLMN_id_T    PLMN;  
  /* Indicates how a PLMN is to be selected */
  sys_network_selection_mode_e_type    network_select_mode;

  /* list of WCDMA cells or frequencies */
 rr_rrc_fdd_target_info_type    fdd_target_info;

  /* Indicates the list of ARFCNs that are valid GSM cells */
  rr_rrc_gsm_cell_list_type    gsm_cell_list;
  sys_rat_pri_list_info_s_type   rat_pri_list_info;

  lte_irat_dedicated_priority_info_s dedicated_priority_info;

} rrc_interrat_redirect_req_type;

/****************************************************/
/* RRC_INTERRAT_REDIRECT_ABORT_REQ primitive :      */
/* FROM :  RR                                       */
/* TO   :  RRC                                      */
/* DESCRIPTION :                                    */
/*  Redirection abort request from GSM to WCDMA     */
/****************************************************/
typedef rrc_rr_as_id_type rrc_interrat_redirect_abort_req_type;

/****************************************************/
/* RRC_INTERRAT_REDIRECT_REJ  primitive :           */
/* FROM :  RRC                                      */
/* TO   :  RR                                       */
/* DESCRIPTION :                                    */
/*  Redirection reject from GSM to WCDMA            */
/****************************************************/
typedef struct
{
  IMH_T                      message_header;

  sys_modem_as_id_e_type     as_id;
} rrc_interrat_redirect_rej_type;

/****************************************************/
/* RRC_INTERRAT_REDIRECT_ABORT_CNF primitive :      */
/* FROM :  RRC                                      */
/* TO   :  RR                                       */
/* DESCRIPTION :                                    */
/* Confirm Redirect abort request from GSM to WCDMA */
/****************************************************/

typedef struct
{
  IMH_T                      message_header;

  sys_modem_as_id_e_type     as_id;
} rrc_interrat_redirect_abort_cnf_type;

typedef enum
{
  GO_TO_INACTIVE=0,
  POWER_OFF,
  MODE_LPM,

  HIBERNATE_FROM_GAN
} rrc_rr_stop_wcdma_cause_e_type;

/**********************************************************/
/* RRC_RR_STOP_WCDMA_REQ primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/*  Request for stopping WCDMA        */
/**********************************************************/
typedef struct
{
  sys_modem_as_id_e_type     as_id;

  /* Indicates the reason for stopping WCDMA */
  rrc_rr_stop_wcdma_cause_e_type    stop_req_cause;
} rrc_rr_stop_wcdma_req_type;

/**********************************************************/
/* RRC_RR_STOP_WCDMA_CNF primitive :                  */
/* FROM :  RRC                                             */
/* TO   :  RR                                            */
/* DESCRIPTION :                                          */
/*  Response for RRC_RR_STOP_WCDMA_REQ        */
/**********************************************************/
typedef struct
{
  IMH_T    message_header;

  sys_modem_as_id_e_type     as_id;
  /* Indicates the reason for stopping WCDMA */
  rrc_rr_stop_wcdma_cause_e_type    stop_cnf_cause;
} rrc_rr_stop_wcdma_cnf_type;
/**********************************************************/
/* RR_RRC_STOP_GSM_CNF primitive :                  */
/* FROM :  RR                                             */
/* TO   :  RRC                                            */
/* DESCRIPTION :                                          */
/*  Response for RR_RRC_STOP_GSM_REQ        */
/**********************************************************/
typedef struct
{

  sys_modem_as_id_e_type     as_id;
  /* Indicates the reason for stopping WCDMA */
  rrc_rr_stop_wcdma_cause_e_type    stop_cnf_cause;
} rr_rrc_stop_gsm_cnf_type;

/**********************************************************/
/* RR_RRC_STOP_GSM_REQ primitive :                  */
/* FROM :  RRC                                             */
/* TO   :  RR                                            */
/* DESCRIPTION :                                          */
/*  Request for Stopping GSM        */
/**********************************************************/
typedef struct
{
  IMH_T    message_header;

  sys_modem_as_id_e_type     as_id;
  /* Indicates the reason for stopping WCDMA */
  rrc_rr_stop_wcdma_cause_e_type    stop_req_cause;
} rr_rrc_stop_gsm_req_type;


/*****************************************************************************/
/* RR_DEDICATED_PRIORITIES_REQ                                               */
/* FROM: WCDMA RRC                                                           */
/* TO:   GERAN RR                                                            */
/* DESCRIPTION:                                                              */
/*   Request for a copy of RR's dedicated priority information and           */
/*   associated PLMN.                                                        */
/*****************************************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rr_dedicated_priorities_req_type;


/*****************************************************************************/
/* RR_DEDICATED_PRIORITIES_RSP                                               */
/* FROM: GERAN RR                                                            */
/* TO:   WCDMA RRC                                                           */
/* DESCRIPTION:                                                              */
/*   This response contains a copy of RR's dedicated priority information    */
/*   and associated PLMN.                                                    */
/*****************************************************************************/
typedef struct
{
  sys_modem_as_id_e_type     as_id;
  /* Boolean indicating if dedicated priorities are valid or not */
  boolean dedicated_pri_valid;

  /* PLMN ID in which the dedicated priorities are received */
  sys_plmn_id_s_type    plmn;

  /* Dedicated priority info */
  lte_irat_dedicated_priority_info_s dedicated_pri_info;

} rr_dedicated_priorities_rsp_type;


/*****************************************************************************/
/* RR_CLEAR_DEDICATED_PRIORITIES_IND                                         */
/* FROM: WCDMA RRC                                                           */
/* TO:   GERAN RR                                                            */
/* DESCRIPTION:                                                              */
/*   Indicates that RR should clear it's dedicated priority information.     */
/*   Used when RRC becomes active, if RR has ownership of dedicated          */
/*   priorities as indicated by lte_rrc_dedicated_pri_get_owner().           */
/*****************************************************************************/
typedef struct
{
  IMH_T message_header;

  sys_modem_as_id_e_type     as_id;
} rr_clear_dedicated_priorities_ind_type;


/*****************************************************************************/
/* RRC_DEDICATED_PRIORITIES_REQ                                              */
/* FROM: GERAN RR                                                            */
/* TO:   WCDMA RRC                                                           */
/* DESCRIPTION:                                                              */
/*   Request for a copy of RRC's dedicated priority information and          */
/*   associated PLMN.                                                        */
/*****************************************************************************/
/* This signal carries no payload, so no struct is required. */

typedef rrc_rr_as_id_type rrc_dedicated_priorities_req_type;

/*****************************************************************************/
/* RRC_DEDICATED_PRIORITIES_RSP                                              */
/* FROM: WCDMA RRC                                                           */
/* TO:   GERAN RR                                                            */
/* DESCRIPTION:                                                              */
/*   This response contains a copy of RRC's dedicated priority information   */
/*   and associated PLMN.                                                    */
/*****************************************************************************/
typedef struct
{
  IMH_T message_header;
  sys_modem_as_id_e_type     as_id;

  /* Boolean indicating if dedicated priorities are valid or not */
  boolean dedicated_pri_valid;

  /* PLMN ID in which the dedicated priorities are received */
  sys_plmn_id_s_type    plmn;

  /* Dedicated priority info */
  lte_irat_dedicated_priority_info_s dedicated_pri_info;

} rrc_dedicated_priorities_rsp_type;


/*****************************************************************************/
/* RRC_CLEAR_DEDICATED_PRIORITIES_IND                                        */
/* FROM: GERAN RR                                                            */
/* TO:   WCDMA RRC                                                           */
/* DESCRIPTION:                                                              */
/*   Indicates that RRC should clear it's dedicated priority information.    */
/*   Used when RR becomes active, if RRC has ownership of dedicated          */
/*   priorities as indicated by lte_rrc_dedicated_pri_get_owner().           */
/*****************************************************************************/
/* This signal carries no payload, so no struct is required. */

typedef rrc_rr_as_id_type rrc_clear_dedicated_priorities_ind_type;

/*****************************************************************************/
/* RR_CIPHER_SYNC_IND                                                        */
/* FROM: GERAN RR                                                            */
/* TO:   WCDMA RRC                                                           */
/* DESCRIPTION:                                                              */
/*   This is Sent from GSM to WCDMA when RR receives ciphering mode          */
/*   command message which will start ciphering on G for CS domain.          */
/*****************************************************************************/
typedef rrc_rr_as_id_type rr_cipher_sync_ind_type;


#endif /* RRC_RR_TYPES_H */
