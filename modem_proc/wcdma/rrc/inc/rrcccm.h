#ifndef RRCCCM_H
#define RRCCCM_H

/*===========================================================================
  R R C    C E L L    C H A N G E    M A N A G E R    H E A D E R    F I L E
DESCRIPTION

   This file contains all prototypes used by Cell Change Manager.

EXTERNALIZED FUNCTIONS   
   

      
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcccm.h_v   1.13   14 Jun 2002 13:53:04   bumatt  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcccm.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/15   sp      Made changes to add attribute to functions to avoid comperession. Also added code to enable metrics.
02/19/14   sp      Made changes for Mav debug snapshot
11/14/14   sr      Code reduction to remove GET_ALL_SIBS handling
10/17/14   sr      CSFB call optimization changes
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
12/07/12   sn      CSG feature changes
10/29/12   gv      Mainline the changes under FEATURE_PPAC
01/12/12   gv      Added code to support FEATURE_PPAC
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/14/11   kp      Fixed compilation warning
03/28/11   rl      Merged the HS-FACH/PCH changes from mainline
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
11/03/10   rl      Fixed the compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
05/24/10   as      Made changes to index SIBDB with common PLMN ID for shared cell
05/15/09   rm      Added declaration for rrc_ccm_calculate_ul_uarfcn
05/08/09   ss      Updated Copyright Information
03/30/09   rm      CMI Featurization
05/21/08   pk      Added support for FEATURE_WCDMA_BAND_PRIORITIZATION.
03/11/08   pk      Added support to report current cell id to NAS through RRC_SERVICE_CNF,
                   RRC_SERVICE_IND and RRC_SERVICE_IND_FROM_OTA.
11/29/07   pk      Added prototype for a new function rrc_ccm_propagate_new_cell_to_waiting_proc().
10/23/07   kp      Added support for network sharing feature. Code changes to handle 
                   primary PLMN I.D in OTA message. Changes are under feature flag 
                   FEATURE_NETWORK_SHARING.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
06/13/06   segu    Added support for DSAC feature implementation
06/08/06   vm      Added prototype of new function rrc_ccm_get_curr_camped_plmn().
07/15/05   vm      Added support for handling channel config failure in CSP.
                   Added prototype for new function rrc_ccm_update_cell_camped_status().
10/18/04   sk/bu   Added prototype for rrc_ccm_cell_change_to_same_cell().
09/15/04   sk/bu   Added prototype for rrc_ccm_buid_service_ind_with_nas_cn_info().
06/14/04   bu      Renamed rrcccm_err_fatal_handler to rrcccm_force_init.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
02/12/04   bu      Added support for ERR_FATAL recovery.
10/09/03   bu      Added prototype for rrc_ccm_update_for_inter_freq_hard_handover().
07/24/03   vn      Prototype for rrc_ccm_update_with_cell_loss().
06/27/03   vn      Prototype for rrcccm_waiting_for_reselection_cnf().
04/17/03   bu      Added a formal parameter rrc_csp_acq_entry_type in 
                   rrc_ccm_update_for_gtow_handover(). 
02/27/03   vn      Added prototype for rrc_ccm_get_curr_camped_freq and
                   rrc_ccm_get_curr_camped_psc.
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to 
                   INTERRAT_HANDOVER_GTOW
02/14/03   bu      Add prototype for rrc_ccm_update_for_gtow_handover() if
                   FEATURE_INTERRAT_HANDOVER_GTOW_RRC is defined.
11/19/02   vn      Added function prototype for:
                   rrc_ccm_update_idle_mode_cell_resel_status. Needed to bar
                   going to "Connecting" during a cell reselection.
06/14/02   bu      Use mm_as_cell_access_status_T instead of 
                   rrc_cell_access_status_e_type for DUALMODE.
05/30/02   vn      Added protype for rrc_ccm_is_reselection_in_progress. 
                   Added rrc_ccm_cell_camping_status_valid_e_type
                   to cell camping info to have more info when status is invalid.
05/16/02   bu      Merged the following from cell reselection branch:
           bu      Added RRC_CCM_WAIT_CELL_RESELECTION_CNF to
                   rrc_ccm_substate_e_type.
03/07/02   kc      Added function to retreive cn info from CN_InformationInfoFull
                   struct and send it to NAS
11/06/01   bu      Changed the parameter of rrc_ccm_send_nas_cn_info().
10/22/01   vn      Added cell transition type to the cell camping info. The
                   cell transition type indicates if it is an Return to service
                   area, Reselection or a cell selection type of cell transition.
08/29/01   bu      Added prototypes of functions rrc_ccm_send_nas_cn_info
                   and rrc_ccm_send_l1_drx_req.
08/08/01   vn      Added cell access status for current and previous cells
                   in rrc_ccm_cell_camping_info_type. Added a utility -
                   rrc_ccm_get_curr_cell_access_status to get the cell access
                   status for the current camped on cell.
07/26/01   vn      A new flag is added to rrc_ccm_cell_camping_info_type 
                   to remember if RCE called and found the current camping 
                   status as "Not Camping".   rrc_ccm_get_curr_camping_status
                   now accepts Procedure ID as an argument.
03/30/01   rj      Fixed formal parameter type mismatch for 
                   rrc_ccm_get_serv_plmn_cellid function.
01/29/01   vn      Added a new state to wait for Channel Config Confirmation.                   
01/25/01   vn      Changes due to change of enum name to rrc_new_cell_status_
                   e_type.  
01/18/01   vn      Added rrc_ccm_cell_camping_info_type and prototypes of 
                   functions rrc_ccm_propagate_new_cell_to_all_procs, 
                   rrc_init_cell_change_manager and rrc_cell_change_manager.
12/22/00   vn      Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */


#include "wcdma_variation.h"
#include "comdef.h"
#include "rex.h"
#include "msg.h"

#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrcmmif.h"
#include "rrcasn1.h"
#include "rrcintif.h"
#include "rrccspdb.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* This lists all possible substates for Cell Change Manager.
 * Note that these substates are independent of the Cell Camping status
 * of the UE. For Example CCM in substate Idle could either be when
 * the UE is Camped on a cell or Not Camped on a cell 
 */
typedef enum
{
  /* CCM in Idle state */
  RRC_CCM_IDLE,
  /* Waiting for a Channel Config Confirm from LLC */
  RRC_CCM_WAIT_CHAN_CONFIG_CNF,
  /* Waiting for a Cell Reselection confirm from LLC */
  RRC_CCM_WAIT_CELL_RESELECTION_CNF,
  /* max number of valid CCM states */
  RRC_CCM_MAX_STATES
}rrc_ccm_substate_e_type;
/* Below #defines are added for logging purpose only */
#define rrc_ccm_substate_e_type_value0 RRC_CCM_IDLE,
/* Waiting for a Channel Config Confirm from LLC */
#define rrc_ccm_substate_e_type_value1 RRC_CCM_WAIT_CHAN_CONFIG_CNF,
/* Waiting for a Cell Reselection confirm from LLC */
#define rrc_ccm_substate_e_type_value2 RRC_CCM_WAIT_CELL_RESELECTION_CNF,
/* max number of valid CCM states */
#define rrc_ccm_substate_e_type_value3 RRC_CCM_MAX_STATES

/* This enumerates the possible values for Cell Camping
 * status validity. The status must be checked for validity
 * before reading the camping info.
 */
typedef enum
{
  /* Status in Invalid and Cell Camping in progress */ 
  STATUS_INVALID_CELL_TO_BE_CAMPED,
  /* Status in Invalid and Cell Reselection is in progress */ 
  STATUS_INVALID_CELL_RESELECTION,
  /* Status in Valid */ 
  STATUS_VALID
}rrc_ccm_cell_camping_status_valid_e_type;

/* Structure used to maintain the current cell camping info of UE.
 */
typedef struct
{
  /* Indicates if the status is valid. During processing of a Cell Change Req
   * the status is set to STATUS_INVALID_CELL_TO_BE_CAMPED or
   * STATUS_INVALID_CELL_RESELECTION. It is also initialized to 
   * STATUS_INVALID_CELL_TO_BE_CAMPED in the 
   * CCM inititialization.
   */
  rrc_ccm_cell_camping_status_valid_e_type   status;
  /* Indicates the status of cell - it is set based on the Cell Change Req. 
   * It is initialized to RRC_CELL_LOSS_NO_NEW_CELL in CCM initialization.
   */
  rrc_new_cell_status_e_type  cell_camp_status;

  /* Indicates the Procedure who last requested a Cell Change */
  rrc_proc_e_type   req_proc_id;

  /* Indicates requested cell change type */
  rrc_cell_change_e_type  cell_change_type;

  /* Cell data for Current Cell is valid only if camped status is 
   * RRCCCM_CAMPED and the converse is also true. 
   */
  /* Cell data for current cell */
  rrc_csp_acq_entry_type  curr_cell;

  mm_as_cell_access_status_T    curr_cell_access;
   
  /*  Current CS and PS cell access details are captured
   *  into this structure and curr_dsac_present variable 
   *  tells whether DSAC specifc cell access information
   *  is present (or) not
   */
  boolean curr_dsac_present;

  /* Cell Access information for CS */
  mm_as_cell_access_status_T curr_cs_cell_access;

  /* Cell Access information for PS */
  mm_as_cell_access_status_T curr_ps_cell_access;

  /* TRUE  if PPAC is present
  FALSE if PPAC is not present */
  boolean curr_ppac_present;
  
  /* Stores the new cell CS reg status */
  boolean                    curr_cell_cs_reg_allowed;
  
  /* Storest the new cell PS reg status */
  boolean                    curr_cell_ps_reg_allowed;

  /* Stores the new cell paging response status */
  mm_as_page_rsp_e_type      curr_cell_page_rsp_allowed;

  /* Cell data for previous cell */
  rrc_csp_acq_entry_type  prev_cell;

  mm_as_cell_access_status_T    prev_cell_access; 

 /*  Previous CS and PS cell access details are captured
  *  into this structure and curr_dsac_present variable 
  *  tells whether DSAC specifc cell access information
  *  is present (or) not
  */
  boolean prev_dsac_present;

  /* Cell Access information for CS */
  mm_as_cell_access_status_T prev_cs_cell_access;

  /* Cell Access information for PS */
  mm_as_cell_access_status_T prev_ps_cell_access;

  /* TRUE  if PPAC is present
  FALSE if PPAC is not present */
  boolean prev_ppac_present;
  
  /* Stores the new cell CS reg status */
  boolean                          prev_cell_cs_reg_allowed;
  
  /* Storest the new cell PS reg status */
  boolean                          prev_cell_ps_reg_allowed;

  /* Stores the new cell paging response status */
  mm_as_page_rsp_e_type            prev_cell_page_rsp_allowed;

  /* Indicates if RRC Connection Establishment procedure asked for camping
   * status when we were not camped.
   */
  boolean  rce_found_status_not_camping;

  boolean       is_curr_cell_id_valid;

}rrc_ccm_cell_camping_info_type;


/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/*===========================================================================

FUNCTION          RRC_CCM_UPDATE_WITH_CELL_LOSS 

DESCRIPTION       This function updates the Camping status in CCM while 
                  maintaining the CCM state.
  
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
void rrc_ccm_update_with_cell_loss(void);


/*===========================================================================
FUNCTION          RRCCCM_WAITING_FOR_RESELECTION_CNF

DESCRIPTION       This function returns a Boolean indicating if CCM is still
                  waiting for Cell Reselection Confirmation.
                  A State Change Indication could have put CCM to Idle substate
                  after it had sent the last Reselection Req. The function 
                  provides info to detect such an incidence.

DEPENDENCIES      None

RETURN VALUE      TRUE/FALSE

SIDE EFFECTS      None.
===========================================================================*/
boolean  rrcccm_waiting_for_reselection_cnf(void);

/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPED_FREQ

DESCRIPTION       This function is responsible for returning the current 
                  Camped on frequency of the UE.

DEPENDENCIES      Calling procedure needs to call 
                  rrc_ccm_get_curr_camping_status before calling this function.
                  The returned value from this function is valid only if 
                  rrc_ccm_get_curr_camping_status returns RRC_CAMPED_ON

RETURN VALUE
                  Currently camped on frequency

SIDE EFFECTS      None.
===========================================================================*/
rrc_freq_type rrc_ccm_get_curr_camped_freq(void);

/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPED_PSC

DESCRIPTION       This function is responsible for returning the current 
                  Camped on Primary Scrambling Code of the UE.

DEPENDENCIES      Calling procedure needs to call 
                  rrc_ccm_get_curr_camping_status before calling this function.
                  The returned value from this function is valid only if 
                  rrc_ccm_get_curr_camping_status returns RRC_CAMPED_ON

RETURN VALUE
                  Currently camped on Primary Scrambling Code

SIDE EFFECTS      None.
===========================================================================*/
rrc_scr_code_type rrc_ccm_get_curr_camped_psc(void);

/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPED_PLMN

DESCRIPTION       This function is responsible for returning the current 
                  Camped on PLMN of the UE.

DEPENDENCIES      NONE

RETURN VALUE
                  Currently camped on PLMN

SIDE EFFECTS      None.
===========================================================================*/
rrc_plmn_identity_type rrc_ccm_get_curr_camped_plmn(void);

/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPED_CELL_ID

DESCRIPTION       This function is responsible for returning the current 
                  Camped on cell id of the UE.

DEPENDENCIES      NONE

RETURN VALUE
                  Currently camped on cell

SIDE EFFECTS      None.
===========================================================================*/
rrc_int_cell_id_type rrc_ccm_get_curr_camped_cell_id(void);

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPED_CSG_ID

DESCRIPTION       This function returns current camped CSG id of the UE.

DEPENDENCIES      NONE

RETURN VALUE
                  CSG ID if camped on CSG cell else invalid value

SIDE EFFECTS      None.
===========================================================================*/
rrc_int_csg_id_type rrc_ccm_get_curr_camped_csg_id(void);
#endif

/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPING_STATUS

DESCRIPTION       This function is responsible for returning the current 
                  Camping Status of the UE.

DEPENDENCIES
                  None.

RETURN VALUE
                  The Current Camping Status as maintained by Cell Change Mgr.

SIDE EFFECTS      None.
===========================================================================*/
rrc_camped_on_status_e_type  
rrc_ccm_get_curr_camping_status(rrc_proc_e_type req_proc_id);

/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CELL_ACCESS_STATUS

DESCRIPTION       This function is responsible for returning the 
                  cell access status for the currently camped on cell.
                  If not camped on a cell, a cell access status of 
                  RRC_NO_CELL_ACCESS is returned.

DEPENDENCIES
                  None.

RETURN VALUE
              
SIDE EFFECTS      None.
===========================================================================*/
mm_as_cell_access_status_T rrc_ccm_get_curr_cell_access_status(void);


/*===========================================================================

FUNCTION          RRC_CCM_GET_SERV_PLMN_CELLID

DESCRIPTION       This function is responsible for returning the PLMN and 
                  Cell ID of the serving cell.

DEPENDENCIES
                  None

RETURN VALUE
                  A status that indicates whether there is a serving cell 
                  or not..
SIDE EFFECTS
                  The returned status MUST be checked by the caller before
                  reading the PLMN or Cell ID. If returned status indicates
                  No Serving Cell, PLMN and Cell ID will not be initialized.

===========================================================================*/
rrc_new_cell_status_e_type
rrc_ccm_get_serv_plmn_cellid(rrc_int_cell_id_type *cell_id, rrc_plmn_identity_type *plmn_id);

/*===========================================================================
FUNCTION          RRC_CCM_IS_RESELECTION_IN_PROGRESS

DESCRIPTION       This function is responsible for returning the current 
                  status of Reselection in progress.
DEPENDENCIES
                  None

RETURN VALUE
                  A status that indicates if a Reselection is in progress
                  or not.
                  
SIDE EFFECTS      None
===========================================================================*/
boolean  rrc_ccm_is_reselection_in_progress(void);

/*===========================================================================

FUNCTION          RRC_CCM_PROPAGATE_NEW_CELL_TO_ALL_PROCS

DESCRIPTION       This function is responsible for propagating the received 
                  RRC_NEW_CELL_IND to all procedures that deserve to be 
                  informed of it. 
                  One of the procedures that would otherwise be informed can 
                  be over-ruled by putting its ID in the cmd. If a procedure's
                  ID appears as "No Ind Procedure ID" in the cmd, it is NOT
                  informed with RRC_NEW_CELL_IND.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  None
SIDE EFFECTS
                  A possible side effect could come from the order in which
                  procedures are informed. The Cell Selection Procedure must
                  always be informed (if not indicated otherwise in cmd)before
                  any other procedures.
===========================================================================*/
void  rrc_ccm_propagate_new_cell_to_all_procs( rrc_cmd_type  *cmd_ptr );


/*===========================================================================

FUNCTION          RRC_INIT_CELL_CHANGE_MANAGER

DESCRIPTION       This function is responsible for initializing data 
                  structures associated with Cell Change Manager.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

NOTE
                  This function SHALL NOT send any cmd to any other procedure.
===========================================================================*/
void rrc_init_cell_change_manager( void );


/*===========================================================================

FUNCTION          RRC_CCM_SEND_NAS_CN_INFO

DESCRIPTION       This function is responsible for sending the NAS specific
                  CN information to MM.

DEPENDENCIES
                  

RETURN VALUE
                  SUCCESS: Able to send RRC_SERVICE_IND successfully.
                  FAILURE: Not able to send due to any of the multiple reasons.

SIDE EFFECTS      None.
===========================================================================*/
uecomdef_status_e_type
rrc_ccm_send_nas_cn_info
(
  rrc_CN_InformationInfo* cn_information_info_ptr
   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  ,boolean primary_plmn_Id_present
   /*pointer to primary plmn i.d if present in OTA msg or NULL otherwise*/
  ,rrc_PLMN_Identity *primary_plmn_Identity
);
/*===========================================================================

FUNCTION          RRC_CCM_BUILD_SERVICE_IND_WITH_NAS_CN_INFO

DESCRIPTION       This function is responsible for building service_ind with
                  the NAS specific CN information.

DEPENDENCIES
                  

RETURN VALUE
                  mm_cmd_type * cmd: mm command

SIDE EFFECTS      None.
===========================================================================*/

mm_cmd_type *
rrc_ccm_buid_service_ind_with_nas_cn_info
(
  rrc_CN_InformationInfo* cn_information_info_ptr
   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  ,boolean primary_plmn_Id_present
   /*pointer to primary plmn i.d if present in OTA msg or NULL otherwise*/
  ,rrc_PLMN_Identity *primary_plmn_Identity
);


/*===========================================================================

FUNCTION          rrc_ccm_buid_service_ind_with_nas_cn_info_r6

DESCRIPTION       This function is responsible for building service_ind with
                  the NAS specific CN information.

DEPENDENCIES
                  

RETURN VALUE
                  mm_cmd_type * cmd: mm command

SIDE EFFECTS      None.
===========================================================================*/
mm_cmd_type *
rrc_ccm_buid_service_ind_with_nas_cn_info_r6
(
  rrc_CN_InformationInfo_r6* cn_information_info_ptr
);

/*===========================================================================

FUNCTION          RRC_CCM_SEND_NAS_CN_INFO_R6

DESCRIPTION       This function is responsible for sending the NAS specific
                  CN information to MM.

DEPENDENCIES
                  

RETURN VALUE
                  SUCCESS: Able to send RRC_SERVICE_IND_FROM_OTA_MSG successfully.
                  FAILURE: Not able to send due to any of the multiple reasons.

SIDE EFFECTS      None.
===========================================================================*/
uecomdef_status_e_type rrc_ccm_send_nas_cn_info_r6
(
  rrc_CN_InformationInfo_r6* cn_information_info_ptr
);

/*===========================================================================

FUNCTION          RRC_CCM_SEND_NAS_CN_INFO_FULL

DESCRIPTION       This function is responsible for sending the NAS specific
                  CN FULL information to MM.

DEPENDENCIES
                  

RETURN VALUE
                  SUCCESS: Able to send RRC_SERVICE_IND successfully.
                  FAILURE: Not able to send due to any of the multiple reasons.

SIDE EFFECTS      None.
===========================================================================*/
uecomdef_status_e_type
rrc_ccm_send_nas_cn_info_full
(
  rrc_CN_InformationInfoFull* cn_information_info_full_ptr
   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  ,boolean primary_plmn_Id_present
   /*pointer to primary plmn i.d if present in OTA msg or NULL otherwise*/
  ,rrc_PLMN_Identity *primary_plmn_Identity
);


/*===========================================================================

FUNCTION          RRC_CCM_SEND_L1_DRX_REQ

DESCRIPTION       This function is responsible for sending CPHY_DRX_REQ
                  to L1.

DEPENDENCIES
                  

RETURN VALUE
                  SUCCESS: Able to send CPHY_DRX_REQ successfully.
                  FAILURE: Not able to send due to any of the multiple reasons.

SIDE EFFECTS      None.
===========================================================================*/
uecomdef_status_e_type rrc_ccm_send_l1_drx_req(void);

/*===========================================================================
FUNCTION          rrc_ccm_update_cell_camped_status

DESCRIPTION       This function is responsible for putting the 
                  "Cell camp status" to RRC_NEW_CELL_SELECTED
                  when that status type is passed as a parameter. 
                  Also, new cell indication is sent to RCE if it was waiting
                  for UE to camp on

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void rrc_ccm_update_cell_camped_status(rrc_new_cell_status_e_type status);

/*===========================================================================
FUNCTION          rrc_ccm_update_for_gtow_handover

DESCRIPTION       This function will update the CCM camping information.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void
rrc_ccm_update_for_gtow_handover(rrc_csp_acq_entry_type * acq_entry_ptr);


/*===========================================================================
FUNCTION          rrc_ccm_update_for_inter_freq_hard_handover

DESCRIPTION       This function will update the CCM camping information with
                  the new frequency and scrambling code after a successful
                  inter-frequency hard handover.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void
rrc_ccm_update_for_inter_freq_hard_handover (rrc_freq_type freq,
                                             rrc_scr_code_type pri_scr_code);



/*===========================================================================

FUNCTION          RRCCCM_FORCE_INIT

DESCRIPTION       This function is responsible for initializing data 
                  structures associated with Cell Change Manager.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

NOTE
                  This function SHALL NOT send any cmd to any other procedure.
===========================================================================*/
void rrcccm_force_init( void );


/*===========================================================================

FUNCTION          RRC_CELL_CHANGE_MANAGER

DESCRIPTION       This function is responsible for handling all commands 
                  destined for Cell Change Manager.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

===========================================================================*/
void  rrc_cell_change_manager( rrc_cmd_type  *cmd_ptr );

/*===========================================================================

FUNCTION          RRC_CCM_CELL_CHANGE_TO_SAME_CELL

DESCRIPTION       This function returns a boolean that tells whether UE
                  camped on the same cell as before.
DEPENDENCIES
                  None

RETURN VALUE
                  boolean true : cell is same as before
                         false : cell is different

SIDE EFFECTS
                  None.

===========================================================================*/
boolean rrc_ccm_cell_change_to_same_cell(void);


/*===========================================================================

FUNCTION          RRC_CCM_UPDATE_SERVICE_INFO

DESCRIPTION       This function updates service info OTA sent to MM

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

===========================================================================*/
void rrc_ccm_update_service_info(mm_cmd_type *service_info_ota_ptr);

/*===========================================================================

FUNCTION          RRC_CCM_PROPAGATE_NEW_CELL_TO_WAITING_PROC

DESCRIPTION       This function is responsible for sending the NEW_CELL_IND to the
                  procedure waiting for it.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  None
SIDE EFFECTS
                  None
===========================================================================*/
void rrc_ccm_propagate_new_cell_to_waiting_proc
(
  rrc_proc_e_type proc_id,
  boolean cell_same_as_before,
  boolean inform_procedure_rce
);

/*==================================================================================

FUNCTION          rrc_ccm_calculate_ul_uarfcn

DESCRIPTION       This function calculates the uplink frequency given the down 
                  link frequency and PLMN id. PLMN information is used only when 
                  the downlink frequency falls in the overlapping region of bands 
                  RRC_CSP_WCDMA_850_BAND & RRC_CSP_WCDMA_800_BAND.

DEPENDENCIES

RETURN VALUE
                 Returns UL UARFCN for a DL UARFCN passed in the function argument. 
                 If the DL UARFCN is invalid DL UARFCN is returned as UL UARFCN

SIDE EFFECTS
                  None.

=====================================================================================*/
rrc_freq_type rrc_ccm_calculate_ul_uarfcn
(
  rrc_freq_type uarfcn_dl,
  rrc_plmn_identity_type plmn_id
);
/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPED_COMMON_PLMN

DESCRIPTION       This function is responsible for updating plmn_id with common PLMN_ID if curr cell is shared

DEPENDENCIES      NONE

RETURN VALUE
                  TRUE      If Curr cell is shared
                  FALSE     Otherwise

SIDE EFFECTS      None.
===========================================================================*/
boolean rrc_ccm_get_curr_camped_common_plmn
(
  rrc_plmn_identity_type *plmn_id
);

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================

FUNCTION          rrc_ccm_get_previous_camped_cell_info

DESCRIPTION       This function is responsible for providing previous cell i.d/plmn i.d

DEPENDENCIES      NONE

RETURN VALUE
                  Void

SIDE EFFECTS      None.
===========================================================================*/
void rrc_ccm_get_previous_camped_cell_info
(
  rrc_plmn_identity_type *prev_plmn_id,
  /* Cell ID */
  rrc_int_cell_id_type *cell_id
);
#endif

#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CELL_INFO

DESCRIPTION       This function is responsible for returning the current 
                  Camped cell information structure

DEPENDENCIES      None

RETURN VALUE
                  Currently camped cell info structure

SIDE EFFECTS      None.
===========================================================================*/
rrc_csp_acq_entry_type rrc_ccm_get_curr_cell_info(void);
#endif
/*===========================================================================
FUNCTION rrcccm_get_curr_cell_camped_status

DESCRIPTION
  This function returns curr cell camp status

DEPENDENCIES
  None


RETURN VALUE
rrc_new_cell_status_e_type


SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
rrc_new_cell_status_e_type rrcccm_get_curr_cell_camped_status(void);

/*===========================================================================
FUNCTION rrcccm_get_camping_status

DESCRIPTION
  This function returns camping status

DEPENDENCIES
  None


RETURN VALUE
rrc_new_cell_status_e_type


SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrc_ccm_cell_camping_status_valid_e_type rrcccm_get_camping_status(void);

/*===========================================================================
FUNCTION rrcccm_return_substate

DESCRIPTION
  This function returns substate

DEPENDENCIES
  None


RETURN VALUE
rrc_ccm_substate_e_type


SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrc_ccm_substate_e_type rrcccm_return_substate(void);
#endif
