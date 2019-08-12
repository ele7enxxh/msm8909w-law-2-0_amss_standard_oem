/*===========================================================================
         R R C    C E L L    C H A N G E    M A N A G E R    
DESCRIPTION

   This file contains Cell Change Manager (CCM). CCM is responsible for the 
   following -
    1. Carry out steps to transition to a new cell or no cell when a request
       is made by any RRC procedure.
    2. Maintain the current cell camp status. 

   Camp Status
   ======================
   It indicates if UE is at present camped on a Cell or is in Cell Loss condition. 
   When the status indicates Camping on a cell, all SIBs for that cell can be 
   presumed to be available.
   CCM maintains Camp status based on Cell Change Requests that come when a New
   Cell is selected or when a Cell Loss occurs without New Cell selection.
   CCM invalidates the Camp status during the time it processes the Cell Change 
   Request. This has the effect that till Cell Change Request is fully processed,
   UE is presumed to be Not Camping on any cell.


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

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcccm.c_v   1.37   19 Jul 2002 14:23:32   bumatt  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcccm.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   sp      Made changes to report cell-id to QSH whenever GPS cell ID request is honored.
05/09/16   sp      Made changes for Mav debug snapshot
12/30/15   sas     Made changes to update correct LAC,PLMN info values to registered clients after UMI.
02/24/15   sr      Made changes to set meas substate to idle for CSFB calls in CCM
02/09/15   sn      Changes to allow LTA even after PS RABs are released but not before
                   they are established
01/23/15   sr      Made changes to update config_ptrs with sib contents after connection reject with redirection 
01/14/15   sas     Made changes to log events for cfsb call opt feature
11/27/14   sr      NV control for FR23776
11/21/14   bc      Made changes to not change TRM priority after moving to FACH/DCH if the establishment is for CS domain.
11/14/14   sr      Code reduction to remove GET_ALL_SIBS handling
11/10/14   sp      Made changes to save procedure substates in crash debug info
10/01/14   sp      Made changes to use bit mask for fields in EVENT_WCDMA_CELL_EFACH_EPCH_ERACH_SUPPORT 
06/27/14   sn      Changes to prioritize MT call over reselection
06/23/14   vi      Made changes to skip SIB7 reading if SIB7 expiration timer is stil running
06/09/14   sn      Changes to fill active band information in service indication 
                   OTA message sent from CCM
06/02/14   sr      Made changes to handle state change indication to FACH in wait reselection_cnf substate
04/10/14   vi      Made changes to update CCM with PLMN present in CN-Informatio
04/09/14   gv      Made changes to allow QTA even though PS RAB is not established
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
04/01/14   sp      Fixed KW errors
03/26/14   gv      Made changes to now allow TA when PS RAB is not established
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
01/23/14   gv      Made changes to allow QTA even though PS RAB is not established
01/07/14   vi      Modified code to configure MAC with TVM when 
                   new cell indication is posted to meas and rrccu_substate is wait for CU confirm
07/31/13   vi      Mainling the MODEM_STATISTICS feature
06/28/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/04/13   sn      Changes not to reset network sharing flag for inter-frequency HO 
03/05/13   sn      Initialize CSG ID to invalid value for CN info received from network/HO scenarios
01/03/13   ad      Made changes to reduce the redundant F3s
12/07/12   sn      CSG feature changes
11/21/12   gv      Corrected osys migration issue to send PS domain info in SERVICE_IND_OTA to NAS
11/19/12   sn      Added new fields to WCDMA CELL ID log packet
10/29/12   gv      Mainline the changes under FEATURE_PPAC
10/26/12   db      EFACH Redesign changes
08/17/12   sa      Made the changes to delete the SIB TVM on reselections
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/07/12   sn      Update ACQ DB with PLMN info received in UMI message
04/13/12   pm      removing FEATURE_BAND_PRIORITY_ORDER_THROUGH_NV feature flag and reverting some #ifdef
                   changes
04/10/12   pm      Added code to support the band priority nv
02/09/12   gv      Added code to support inclusion of PLMN ID in IDT message with network sharing
01/12/12   gv      Added code to support FEATURE_PPAC
09/08/11   as      Made changes to display E-DRX capability event
07/08/11   sks     Made changes to update RAT priority list after successful camping during WRLF search.
04/13/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.95.50
04/08/11   as      Fixed compilation error
04/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.52
04/05/11   as      Added code to support E-FACH events
03/28/11   rl      Merged the HS-FACH/PCH changes from mainline
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   rm      Added code to support BC19 under FEATURE_WCDMA_BC19
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/15/10   su      Fixed Compilation warnings.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/06/10   su      Changes to update qchat on location after change in PLMN-id or cell-id.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/07/10   sks     Made changes to access cn_DomainInformationList only if corresponding
                   bitmask is present.
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/16/10   rm      Added code to correctly log the cell info
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/14/10   rm      Added code to support BST search on WCDMA
05/24/10   as      Made changes to index SIBDB with common PLMN ID for shared cell
05/07/10   prk      Fixed compiler warnings.
04/12/10   rm      Added code to support BC11
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
01/13/10   rm      Added code to fail the cell change request for reselection when
                   OC is set for DCH
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
11/02/09   rm      Added support for FEATURE_MODEM_STATISTICS
10/26/09   sks     Fixed klocwork errors.
07/20/09   rm      Modified the call to funtion rrcrce_force_initialize
05/15/09   rm      Made function rrc_ccm_calculate_ul_uarfcn non static
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
11/21/08   sk      Added code to to restore the previous cell info
                   if the cell reselction fails
09/29/08   kp      Added support to fail cell reselection and initiate OOS if new PLMN, 
                   cell ID are same as currently camped PLMN, cell id. 
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs in 
                   Cell_DCH state.
06/09/08   pk      Extended the window to handle RCE state RRCRCE_WAIT_FOR_REDIRECT_CNF
                   for feature FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH.
05/21/08   pk      Added support for FEATURE_WCDMA_BAND_PRIORITIZATION.
04/21/08   pk      Extended the window to handle more number of RCE states for feature 
                   FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH.
03/11/08   pk      Added support to report current cell id to NAS through RRC_SERVICE_CNF,
                   RRC_SERVICE_IND and RRC_SERVICE_IND_FROM_OTA.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
01/09/08   pk      During OOS scan for FACH->FACH redirection, send NEW_CELL_IND to CU 
                   even if the UE camps on the previously camped cell.
12/21/07   vr      Added support for MBMS. Call MBMS function to indicate new cell change
11/29/07   pk      Moved the code which sends NEW_CELL_IND to the waiting procedure during FACH->FACH
                   transition to a new function and also freed the command buffer allocated to NEW_CELL_IND
                   after its processing by the waiting procedure.
11/23/07   pk      Call RCE function to check whether NEW_CELL_IND needs to be sent to RCE or not
                   when a new cell is selected by the UE. NEW_CELL_IND will be sent to RCE, 
                   if RCE found that UE is not camped and if there is no mismatch between the LAI/RAI
                   given in the EST_REQ and the curretly camped LAI/RAI. 
                   RRC_ABORT_IND will be sent to MM if there is mismatch in LAI/RAI.
                   Changes are under the feature flag FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH.
10/23/07   kp      Added support for network sharing feature. Code changes to handle 
                   primary PLMN I.D in OTA message. Changes are under feature flag 
                   FEATURE NETWORK SHARING.
10/03/07   pk      Added support to use the variable curr_cell_access_status to fill in the cell access 
                   status instead of the variable cell_access which is stale. 
09/10/07   pk      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/06/07   vm      Added fix for CR 125332. Removed check for CSP current scan
                   before sending NEW_CELL_IND to RCE.
03/22/07   kp      Added code to send RCE NEW_CELL_IND on successful cell reselection 
                   if RCE is waiting for SIB7 to become current.
02/05/07   vm      Added a NULL pointer check before accessing the structure. Reported by Klocwork.
11/30/06   segu    Do not inform PLMN info to upper layers if N/W has not mentioned PLMN Id 
                   in any of the reconfiguration messages to UE in connected mode.
11/29/06   pk      Log the correct UL UARFCN for a given DL UARFCN
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   vm      Added support for the feature FEATURE_DCH_TO_FACH_PCH_OUT_OF_SERVICE_ENHANCEMENT.
08/21/06   tkk     Added SRB#5 support.
06/22/06   segu    Update access class info from the CSP internal database
                   i.e. curr_cell_access_status as CCM does not have the up
                   dated value 
06/13/06   sg      Added support for DSAC feature
06/08/06   vm      1. Added new function rrc_ccm_get_curr_camped_plmn() that just returns 
                      the current camped on WCDMA PLMN.
                   2. In the function rrc_ccm_update_for_gtow_handover(), CCM was incorrectly
                      not getting updated with the PLMN information. This has been corrected.
                   3. Also now the function rrc_get_frequency_band() takes plmn_id as an 
                      additional argument.
04/28/06   vm      Added support to send correct cell reserve status to QXDM for 
                   log packet 0x4127.
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
04/11/06   vr      Merged RVDS compilation changes
02/28/06   da/sm   Modified code to support migration to Dec-05 Rel-6 ASN1
01/26/06   vm      Added fix for not updating CCM state in function rrc_ccm_update_for_inter_freq_hard_handover()
                   as otherwise it leads to out of sync between CSP and CCM under certain race conditions.
                   For details please see CR 86667.
11/08/05   vm      Added support for WTOW Background PLMN Search. 
                   CCM is not updated with cell loss when function rrc_ccm_update_with_cell_loss() 
                   is called and WTOW BPLMN search is in progress.
09/22/05   vm      Added support to check for CSP substate during reselections to make
                   sure that CCM and CSP substates are in Sync.
07/15/05   vm      Added support for handling channel config failure in CSP.
                   Added new function rrc_ccm_update_cell_camped_status.
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
04/06/05   da      Added Initial support for BMC.  Calling 
                   rrcbmc_send_rrc_new_cell_ind_message() when re-selecting to 
                   a new cell.
02/24/05   bu      In rrc_ccm_update_for_inter_freq_hard_handover() use
                   rrc_get_frequency_band() to set the band_class to be written
                   into the ACQ DB for the acquisition entry. Additionally if
                   FEATURE_WCDMA_MULTIBAND is not enabled, set the band_class to
                   PCS/IMT2K correctly based on whether FEATURE_WCDMA_PCS is
                   defined or not.
11/30/04   vm/da   A new RRC/NAS interface RRC_SERVICE_IND_FROM_OTA_MSG has 
                   been added that will be used only in case the "CN info" 
11/22/04   bu      Added support for physical channel establishment failure
                   handling.
11/10/04  sm/kc/bu Removed the validation of RRC_GET_ALL_SIBS_CNF in
                   rrc_ccm_wait_sib_handler(), since we have collected all the
                   SIBS in RRC_GET_SPECIFIC_SIBS_REQ itself. RRC_GET_ALL_SIBS_REQ
                   is a NOP now.
10/28/04   bu      Set gtow_ho_active flag to FALSE in cell_selected_meas_req.
10/18/04   sk/bu   Defined rrc_ccm_cell_change_to_same_cell().
10/15/04   bu      Set the curr_cell_access to MM_AS_ACCESS_ALL_CALLS in
                   rrc_ccm_update_for_gtow_handover().
09/22/04   bu      In rrc_ccm_send_chan_config_cmd only RRC_CHANGE_CELL_SELECTION
                   cell change type should be handled in DISCONNECTED state.
09/15/04   sk/bu   Added support to handle Out of service area indication in 
                   Connected mode (CELL_FACH/CELL_PCH/URA_PCH).
09/15/04   sk/bu   Added function rrc_ccm_buid_service_ind_with_nas_cn_info 
                   to build mm command RRC_SERVICE_IND and populate CN info
                   and return the cmd_buf_ptr. This function will be used by
                   RB procedures to save CN info and send it to upper layer 
                   when the procedure is successfully completed.
08/19/04   sgk     Made the check and assigned the right enum value for cn domain id
                   to correct lint error: Type mismatch (assignment) (enum/enum)
                   in functions rrc_ccm_fill_nas_cn_specific_info and 
                   rrc_ccm_fill_nas_cn_specific_info_full  
                   type cast to uint 16 prior to assignment to fix lint Error: 
                   Loss of precision (assignment) (20 bits to 16 bits) in 
                   function rrc_ccm_update_for_inter_freq_hard_handover wrt
                   scr_code = pri_scr_code * 16;
08/13/04   bu      Added WCDMA multiband support.
08/09/04   bu      In rrc_ccm_update_with_cell_loss() set CCM substate to CCM_IDLE.
06/16/04   bu      In rrc_ccm_send_nas_cn_info() and rrc_ccm_send_nas_cn_info_full(),
                   set the gsm_map_common_info_incl flag to TRUE only if present.
06/14/04   bu      Renamed rrcccm_err_fatal_handler to rrcccm_force_init. 
05/10/04   bu      Fixed the check for cell_change_type while sending NEW_CELL_IND.
04/28/04   bu      In rrc_ccm_handle_cell_change_success(), added the case to
                   not send NEW_CELL_IND for DCH->PCH cell change types.
04/07/04   bu      Backed out the change to set cell_same_as_before to FALSE,
                   as it's not really needed and could cause potential
                   race conditions.
04/07/04   bu      Set cell_same_as_before to FALSE in rrc_ccm_handle_cell_change_success
                   if scan is not RESELECTION_SCAN.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
03/04/04   bu      Added support for CELL_PCH and URA_PCH states.
02/17/04   bu      Do an ERR_FATAL if PLMN_ID and cell ID matches during
                   cell reselection.
02/12/04   bu      Added support for ERR_FATAL recovery.
12/16/03   bu      Populate the PLMN ID from the directed message if present while
                   sending SERVICE_IND to MM.
12/08/03   bu      Set the gsm_map_common_info_incl flag to TRUE while sending
                   the SERVICE_IND to MM.
11/03/03   bu      Reset the status mask while adding the acquisition entry in
                   rrc_ccm_update_for_inter_freq_hard_handover().
10/09/03   bu      Initialized frequency and scrambling code information after
                   a successful Inter-frequency hard handover.
10/07/03   kc      Added correct SERVICE_STATUS when NAS Info is to be sent
                   in SERVICE_IND.
08/19/03   bu      Added code to do ERR_FATAL instead of returning failure
                   inside rrc_ccm_send_chan_config_cmd().
07/24/03   vn      Added a new fn rrc_ccm_update_with_cell_loss to update CCM
                   when cell is lost. This is to replace the command interface
                   so as to close windows for race conditions.
06/27/03   vn      New function rrcccm_waiting_for_reselection_cnf to return
                   if CCM is still waiting for RESELECTION_CNF.
06/24/03   bu      Removed linting errors.
04/17/03   bu      Initialized more cell camping information for GTOW handover.
02/27/03   vn      Merged following from branch:
           vn      In rrc_ccm_get_curr_camped_psc, call newly added macro 
                   SCR_CODE_TO_PRI_SCR_CODE to convert scrambling code to
                   primary scrambling code.
           vn      Added rrc_ccm_get_curr_camped_freq and 
                   rrc_ccm_get_curr_camped_psc to provide currently
                   camped on frequency and primary scrambling code resp.
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to 
                   INTERRAT_HANDOVER_GTOW
02/14/03   bu      Added rrc_ccm_update_for_gtow_handover() to update the
                   cell camping information on a GSM to WCDMA handover. The
                   support is under FEATURE_INTERRAT_HANDOVER_GTOW_RRC.
01/28/03   bu      Made the required lint changes.
12/20/02   bu      Changed the procedure name to RRC_PROCEDURE_CCM while
                   registering with the State Change Manager.
11/19/02   vn      Added code to prevent an intra frequency cell reselection
                   to co-exist with an ongoing Connection Establishment 
                   procedure or in a state other than Disconnected when
                   FEATURE_RRC_FDD_CELL_RESEL_IN_FACH is NOT defined. Added new
                   function rrc_ccm_update_idle_mode_cell_resel_status.
10/07/02   vn      On a cell selection due to DCH to FACH transition, drop
                   BCCH while setting up CCCH.
09/22/02   vn      Support for DCH->FACH transitions:Added processing to handle
                   cell change type: Transition from CELL_DCH to CELL_FACH. 
                   Send Channel Config with request to establish CCCH and to 
                   change RRC state to CELL_FACH. Do not send NEW_CELL_IND for 
                   this case.
09/20/02   bu      Added support for cell reselection in CONNECTING and 
                   CELL_FACH state. Added support to handle STATE_CHANGE_IND 
                   from any RRC state to CELL_DCH. Added checks to see if 
                   ordered config is set for CELL_DCH and abort an on-going
                   cell reselection if it is set. On a STATE_CHANGE_IND to IDLE
                   CCM will restore all the data for the last camped cell. Added
                   a new enum type rrc_cell_change_status_e_type. 
09/19/02   vn      Changed order of commands to L1 so that Measurement Req 
                   and Cell Parm Req go before a DRX Coammand, Meas and Cell Parm
                   Req are sent only after a successful cell selection/reselection.
                   Moved sending of DRX command to CSP to ensure the above order.
07/19/02   bu      Added DUALMODE support to take CCM back to not camped state
                   when a MODE_CHANGE_IND (to RRC_MODE_INACTIVE) is received.
                   The data structures associated with Cell Change Manager are
                   also re-initialized.
06/14/02   bu      Use mm_as_service_state_T instead of rrc_service_state_e_type
                   and mm_as_cell_access_status_T instead of
                   rrc_cell_access_status_e_type for DUALMODE.
06/04/02   bu      Added call to rrcsib_change_serving_cell_status() after
                   GET_ALL_SIBS_CNF is received successfully.
05/30/02   vn      Added a new function to indicate if Reselection is in 
                   progress. Changed cell camping info status to indicate
                   more info when it is invalid.
05/16/02   bu      Merged the following from cell reselection branch:
           bu      Added support to send RRC_CELL_RESELECTION_REQ to LLC and
                   receive RRC_CELL_RESELECTION_CNF from LLC. Added support
                   to send the reselection command if the cell change type is
                   RRC_CHANGE_CELL_RESELECTION in the CELL_CHANGE_REQ command.
04/15/02   bu      Removed rrc_csp_util_parm_ptr_u_type in various functions.
                   Instead use acq_entry_ptr.
03/25/02   rj      Updated function rrc_ccm_send_nas_cn_info to fix memory leak
                   while processing CN Domain Information List.
03/13/02   bu      Featurized the changes related to Activation Time in
                   rrc_ccm_send_l1_drx_req().
03/07/02   kc      Added functions to send cn info from struct CN_InformationInfoFull
                   to NAS
02/20/02   ram     Fixed incorrect assignment statement in the function
                   rrc_ccm_fill_nas_cn_specific_info.
02/01/02   kc      The length in CN common info and CN domain specific info
                   passed to MM in response to "CN Info IE" in directed 
                   messages was not being set - Fixed it. Fixed CR19359 -
                   ul_uarfcn value now set to dl_uarfcn-190*5.
01/30/02   vn      Added additional processing to ensure that RCE procedure
                   always gets NEW_CELL_IND when a cell is camped AFTER RCE
                   asked for cell status and found it to be "Not Camped". 
12/12/01   bu      Added the boolean to rrc_csp_send_sib_cmd() function call.
12/04/01   kc      Fixed CR19359.  ul_uarfcn value now set to
                   (dl_uarfcn/5-190)*5
11/06/01   bu      Changed the rrc_ccm_fill_nas_cn_specific_info() and
                   rrc_ccm_send_nas_cn_info() to send the NAS specific core
                   network information to MM.
11/01/01   vn      Changed to call new PLMN comparison functions.
10/25/01   vn      Do not send a DRX Request to L1 when in any state other than
                   Disconnected, Cell_PCH or URA_PCH.
10/22/01   vn      Cell change type is utilized to determine action on 
                   PCCH, CCCH and BCCH. Cell transition type in camped 
                   info is updated on receiving a new Cell Change Request and is
                   passed to rrc_ccm_send_chan_config_cmd. On a return to
                   service area, PCCH is reconfigured in Disconnected state and
                   CCCH is reconfigured in Connecting state. BCCH is released
                   once a PCCH is established in Disconnected state.
10/18/01   kc      Fixed a bug in cell_id logging. Corrected the 
                   log_record_length variable 
09/16/01   vn      Changed reference to the new field - drx_cycle_len due to 
                   change in l1rrcif.h DRX structure definition.
09/05/01   bu      Added check for log_alloc() returning a NULL in
                   rrc_ccm_log_cell_info().
                   Added check for get_int_cmd_buf() returning a NULL
                   while sending the measurement req to L1.
08/29/01   vk      Cell change manager now posts a internal command
                   RRC_CELL_SELECTED_MEAS_REQ to RRC measurement procedure
                   to tell L1 to start meas based on SIB11/12. The previous
                   interface of calling L1 has been removed.
08/29/01   bu      Added an interface - rrc_ccm_fill_nas_cn_specific_info to 
                   send the NAS specific core network information to MM.
                   Added a function - rrc_ccm_log_cell_info to log the
                   current cell information to QXDM.
                   Added a function - rrc_ccm_send_l1_drx_req to inform
                   L1 to use DRX mode.
08/08/01   vn      Added a utility - rrc_ccm_get_curr_cell_access_status to 
                   maintain and return cell access status for the current 
                   camped on cell.
08/06/01   vn      Removed a compilation warning.
07/26/01   vn      Changes done to remember if RCE called and found the 
                   current camping status as "Not Camping". Whenever the status
                   changes to "Camping",a New Cell Indication is sent to RCE. 
07/06/01   vn      Do not send a Cell Change Confirmation to the requesting 
                   procedure if the cell status is "Cell Loss".                   
05/17/01   ram     Added the Radio Bearer Setup Procedure to the procedures
                   that get new cell indication.
05/15/01   vn      Fixed a bug where rrc_ccm_send_chan_config_cmd was not 
                   actually enqueuing RRC internal command.
05/09/01   vn      Setup PCCH RB on a successful cell change before sending
                   a Cell Change CNF back to the requesting procedure.
04/29/01   rj      Included rrcscmgr.h since rrc_get_state function is moved
                   to RRCSCMGR module from RRCTASK module.
03/20/01   vn      Added utility to send Service Ind to MM.
03/15/01   vn      Changed to rrc_int_cell_id_type to support LED based ASN1 file.
01/26/01   vn      Updated for LLC command funneling scheme and for new cmd
                   RRC_CHAN_CONFIG_REQ going to LLC.
01/25/01   vn      Changed mesages for clarity. Changed return type from
                   rrc_ccm_get_curr_camping_status().
01/18/01   vn      Added support for processing Cell Change Requests, 
                   mainatining Cell Camping Info, sending Cell Change Cnf and 
                   New Cell Ind and for processing and propagating New Cell
                   Indications. Added CCM state machine.
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
#include "err.h"
#include "uecomdef.h"
#include "bit.h"

#include "rrcmmif.h"
#include "rrcintif.h"
#include "rrccmd_v.h"

#include "rrccspdb.h"
#include "rrccspi.h"
#include "rrcccm.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrcsibproc.h"
#include "rrcsibdb.h"

/* Needed for calling procedures when propagating New Cell Ind. */
#include "rrccsp.h"
#include "rrcrbe.h"
#include "rrcrce.h"
#include "rrcscmgr.h"
#include "rrccu.h"

#include "rrcrbr.h"
#include "rrcpcreconfig.h"
#include "rrcrbreconfig.h"
#include "rrctcreconfig.h"
#include "rrcllcoc.h"
#include "rrcmeasi.h"
#include "rrcdata_v.h"

#ifdef T_WINNT
#error code not present
#endif

#include "rrclogging.h"

#include "rrcbmc.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#ifdef FEATURE_QCHAT
#include "rrc_qchatupk.h"
#endif /* FEATURE_QCHAT */

extern uint32  prev_cell_id;

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* This macro copies the contents of Cell Change Request to Current Cell data in
 * rrc_ccm_cell_camping_info.
 */
/* If FEATURE_DSAC and FEATURE_PPAC are defined get the cell access information for the CS and PS domain(s) */
#define RRC_CCM_COPY_CURR_CELL_CAMPING_INFO(cmd_ptr)    \
{\
  rrc_ccm_cell_camping_info.curr_cell.freq = cmd_ptr->cmd.cell_change_req.freq;         \
  rrc_ccm_cell_camping_info.curr_cell.scr_code = cmd_ptr->cmd.cell_change_req.scr_code; \
  rrc_ccm_cell_camping_info.curr_cell.plmn_id = cmd_ptr->cmd.cell_change_req.plmn_id;   \
  rrc_ccm_cell_camping_info.curr_cell.cell_id =  cmd_ptr->cmd.cell_change_req.cell_id;  \
  rrc_ccm_cell_camping_info.curr_cell_access = cmd_ptr->cmd.cell_change_req.new_cell_access;\
  rrc_ccm_cell_camping_info.curr_cs_cell_access = cmd_ptr->cmd.cell_change_req.new_cs_cell_access;  \
  rrc_ccm_cell_camping_info.curr_ps_cell_access = cmd_ptr->cmd.cell_change_req.new_ps_cell_access;  \
  rrc_ccm_cell_camping_info.curr_dsac_present= cmd_ptr->cmd.cell_change_req.dsac_present; \
  rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed = cmd_ptr->cmd.cell_change_req.new_cell_cs_reg_allowed;  \
  rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed = cmd_ptr->cmd.cell_change_req.new_cell_ps_reg_allowed;  \
  rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed = cmd_ptr->cmd.cell_change_req.new_cell_page_rsp_allowed;  \
  rrc_ccm_cell_camping_info.curr_ppac_present = cmd_ptr->cmd.cell_change_req.ppac_present;  \
  WRRC_MSG1_HIGH("DSAC: DSAC value at cell change req =%d {0=Not Present/1=Present}",rrc_ccm_cell_camping_info.curr_dsac_present);\
}

/*--------------------------------------------------------------------------
       Macro to convert scrambling code to primary scrambling code
--------------------------------------------------------------------------*/
#define SCR_CODE_TO_PRI_SCR_CODE(scr) ((scr) >> 4)


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* This holds the current cell camping info for the UE */ 
LOCAL  rrc_ccm_cell_camping_info_type   rrc_ccm_cell_camping_info;
/* This holds the Substate for RRC CCM */
LOCAL  rrc_ccm_substate_e_type      rrc_ccm_substate;

#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
extern rrcmeas_substate_e_type rrcmeas_current_substate;
#endif

void  
rrc_ccm_send_new_cell_ind(rrc_new_cell_status_e_type  new_cell_camp_status,
                          rrc_proc_e_type  proc_id,
                          boolean cell_same_as_before,
                          boolean inform_procedure_rce);

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_3GPP_CSFB
extern boolean rrc_csfb_call_status;
#endif

/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */


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
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/

rrc_camped_on_status_e_type  
rrc_ccm_get_curr_camping_status(rrc_proc_e_type req_proc_id)
{
  /* Cell camping status in invalid either because we are about to camp on 
   * a cell or a Reselection is in progress
   */
  if((rrc_ccm_cell_camping_info.status == STATUS_INVALID_CELL_TO_BE_CAMPED)||
     (rrc_ccm_cell_camping_info.status == STATUS_INVALID_CELL_RESELECTION))
  {
    MSG_MED("Curr Camping Status Not Valid, return %d", 
                                          RRC_NOT_CAMPED_ON, 0, 0);
    if(req_proc_id == RRC_PROCEDURE_RCE)
    {
      rrc_ccm_cell_camping_info.rce_found_status_not_camping = TRUE;
    }
    return(RRC_NOT_CAMPED_ON);
  }
  else  /* Camping status is valid */
  {
    if(rrc_ccm_cell_camping_info.cell_camp_status == RRC_NEW_CELL_SELECTED)
    {
      MSG_MED("Return Curr Camping Status %d", RRC_CAMPED_ON, 0, 0);
      if(req_proc_id == RRC_PROCEDURE_RCE)
      {
        rrc_ccm_cell_camping_info.rce_found_status_not_camping = FALSE;
      }
      return(RRC_CAMPED_ON);
    }
    else if(rrc_ccm_cell_camping_info.cell_camp_status == RRC_CELL_LOSS_NO_NEW_CELL)
    {
      MSG_MED("Return Curr Camping Status %d", RRC_NOT_CAMPED_ON, 0, 0);
      if(req_proc_id == RRC_PROCEDURE_RCE)
      {
        rrc_ccm_cell_camping_info.rce_found_status_not_camping = TRUE;
      }
      return(RRC_NOT_CAMPED_ON);
    }
    else
    {
      /* This is Invalid value - something wrong */
      WRRC_MSG2_ERROR("Invalid stored Camp Status %d, return Status %d",
                  rrc_ccm_cell_camping_info.cell_camp_status, RRC_NOT_CAMPED_ON);
      
      if(req_proc_id == RRC_PROCEDURE_RCE)
      {
        rrc_ccm_cell_camping_info.rce_found_status_not_camping = TRUE;
      }
      return(RRC_NOT_CAMPED_ON);
    }
  }  /* Camping status is valid */

} /* rrc_ccm_get_curr_camping_status */

/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

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
void rrc_ccm_update_with_cell_loss(void)
{
 if((rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)
     && (rrc_csp_int_data.curr_scan != RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
  {
    MSG_MED("Updating CCM with cell loss. Scan type is %d", rrc_csp_int_data.curr_scan, 0, 0);

    /* First copy current cell to previous cell since we are ready to accept
    * next cell change request.
    */
    rrc_ccm_cell_camping_info.prev_cell = 
      rrc_ccm_cell_camping_info.curr_cell;

    rrc_ccm_cell_camping_info.prev_cell_access = 
        rrc_ccm_cell_camping_info.curr_cell_access;


  rrc_ccm_cell_camping_info.prev_dsac_present = 
    rrc_ccm_cell_camping_info.curr_dsac_present;

  rrc_ccm_cell_camping_info.prev_cs_cell_access = 
    rrc_ccm_cell_camping_info.curr_cs_cell_access;

  rrc_ccm_cell_camping_info.prev_ps_cell_access = 
    rrc_ccm_cell_camping_info.curr_ps_cell_access;   
  WRRC_MSG0_HIGH("DSAC/PPAC: Back up of DSAC/PPAC info during  ccm update with cell loss is done !");

  rrc_ccm_cell_camping_info.prev_ppac_present = 
    rrc_ccm_cell_camping_info.curr_ppac_present;

  rrc_ccm_cell_camping_info.prev_cell_cs_reg_allowed = 
    rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed;

  rrc_ccm_cell_camping_info.prev_cell_ps_reg_allowed = 
    rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed;

  rrc_ccm_cell_camping_info.prev_cell_page_rsp_allowed = 
    rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed;

  rrc_ccm_cell_camping_info.req_proc_id = RRC_PROCEDURE_CSP;

  rrc_ccm_cell_camping_info.cell_camp_status = RRC_CELL_LOSS_NO_NEW_CELL;

  rrc_ccm_substate = RRC_CCM_IDLE;
  }
  else
  {
    MSG_MED("Not updating CCM with cell loss. Scan type is %d", rrc_csp_int_data.curr_scan, 0, 0);
  }  
    
} /* rrc_ccm_update_with_cell_loss */


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
rrc_freq_type rrc_ccm_calculate_ul_uarfcn(rrc_freq_type uarfcn_dl,rrc_plmn_identity_type plmn_id)
{
  rrc_csp_band_class_type band_class;
  rrc_freq_type uarfcn_ul = uarfcn_dl;

  /* Find out the band to which the downlink UARFCN belongs to */
  band_class = rrc_get_frequency_band(uarfcn_dl,plmn_id);

  /* Calculate the uplink UARFCN corresponding to the passed in 
     downlink UARFCN 
  */
  if(band_class == RRC_CSP_IMT2K_BAND)
    uarfcn_ul = uarfcn_dl - 950;
  else if(band_class == RRC_CSP_PCS_BAND)
    uarfcn_ul = uarfcn_dl - 400;

  else if(band_class == RRC_CSP_WCDMA_1700_BAND)
    uarfcn_ul = uarfcn_dl - 475;

  else if((band_class ==  RRC_CSP_WCDMA_850_BAND)|| (band_class == RRC_CSP_WCDMA_800_BAND)
  || (band_class == RRC_CSP_WCDMA_1800_BAND)
  || (band_class == RRC_CSP_WCDMA_900_BAND)
  ||  (band_class == RRC_CSP_WCDMA_1700_2100_BAND)
  || (band_class == RRC_CSP_WCDMA_1500_BAND)
  )
    uarfcn_ul = uarfcn_dl - 225;
  else if(band_class == RRC_CSP_WCDMA_BC19_BAND)
    uarfcn_ul = uarfcn_dl - 400;
  else
  {
    WRRC_MSG1_ERROR("Received invalid DL UARFCN %d", uarfcn_dl);
  }

  return uarfcn_ul; /* returns UL UARFCN same as DL UARFCN if DL UARCN is 
                       invalid */
} /* rrc_ccm_calculate_ul_uarfcn */
/*===========================================================================

FUNCTION          RRC_CCM_LOG_CELL_INFO 

DESCRIPTION       This function will log the current cell information. This function
                  should only be called when UE is camped, i.e, this function
                  should not be called immediately after reading SIBs, etc. 

  
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None

===========================================================================*/
static void rrc_ccm_log_cell_info( void)
{  

#if (!defined T_WINNT)

/* Do QXDM logging here */
  uint32 log_record_length;                 /* Local variable to store Log 
                                               Record Length */

  WCDMA_CELL_ID_type *log_record_ptr;       /* Local variable to store Log
                                               Record */
  rrc_int_ura_id_type ura_id = 0;

  mm_as_cell_access_status_T  cell_access;
  uint32 idx=0;
  rrc_gsm_map_nas_sys_info_type nas_common_info;
  rrc_CN_DomainSysInfoList * cn_domain_sys_info_list_ptr;
  rrc_cn_domain_sys_info_type  rrc_cn_domain_sys_info;
  rrc_SysInfoType1 *sib1_ptr;
  rrc_SysInfoType2 *sib2_ptr;
  rrc_SysInfoType3 *sib3_ptr;
  rrc_SysInfoType4 *sib4_ptr;
  rrc_CellAccessRestriction *sib3_4_access_restriction_ptr;

  rrc_state_e_type rrc_state = rrc_get_state();

  rrc_plmn_identity_type plmn_id;
  if(rrc_ccm_cell_camping_info.curr_cell.is_valid_nw_sharing)
  {
    plmn_id = rrc_ccm_cell_camping_info.curr_cell.shared_list_of_plmns.common_plmn_id;
  }
  else
  {
    plmn_id = rrc_ccm_cell_camping_info.curr_cell.plmn_id;
  }

  sib1_ptr = (rrc_SysInfoType1 *) rrc_sibdb_return_sib_for_cell(
                            rrc_SIB1, 
                            plmn_id, 
                            rrc_ccm_cell_camping_info.curr_cell.cell_id);

  sib2_ptr = (rrc_SysInfoType2 *) rrc_sibdb_return_sib_for_cell(
                            rrc_SIB2, 
                            plmn_id, 
                            rrc_ccm_cell_camping_info.curr_cell.cell_id);

  sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_cell(
                            rrc_SIB3, 
                            plmn_id, 
                            rrc_ccm_cell_camping_info.curr_cell.cell_id);

  /*Calculate the record length 
    4 is added to include the length of rac which is uint32*/
  /*lint -e413*/
  log_record_length = FPOS(WCDMA_CELL_ID_type, rac) + 4;
  /*lint +e413*/
  
  /*Allocate memory for Log Record. */
  log_record_ptr = (WCDMA_CELL_ID_type *)log_alloc( WCDMA_CELL_ID, log_record_length);
      
  if( sib3_ptr == NULL )
  {
    /* SIB3 for this PLMN, CEll ID combination not present */
    ERR("SIB3 for MCC %d, MNC %d, Cell ID %d not present",
          RRC_CSP_GET_INT_MCC(plmn_id), 
          RRC_CSP_GET_INT_MNC(plmn_id), 
          rrc_ccm_cell_camping_info.curr_cell.cell_id);

    if(log_record_ptr != NULL)
    {
      log_record_ptr->dl_uarfcn = log_record_ptr->ul_uarfcn = log_record_ptr->cell_id = 0;
      log_record_ptr->psc = 0;
      log_record_ptr->ura_id = 0;
      log_record_ptr->cell_access_rest = 0xF;

      /*commit the log record */
      log_commit(log_record_ptr);
    }
    else
    {
      MSG_MED("diag is not enabled", 0, 0, 0);
    } 

    return;
  } /* SIB3 not present */

  if((rrc_state != RRC_STATE_DISCONNECTED) && (rrc_state != RRC_STATE_CONNECTING))
  {
    if(sib3_ptr->sib4indicator == TRUE)
    {
      /* Read SIB4. If it's not there mark and error and continue with SIB3 */
      sib4_ptr = (rrc_SysInfoType4 *) rrc_sibdb_return_sib_for_cell(
                                rrc_SIB4, 
                                plmn_id, 
                                rrc_ccm_cell_camping_info.curr_cell.cell_id);
      if( sib4_ptr == NULL )
      {
        /* SIB4 not present */
        WRRC_MSG0_HIGH("SIB4 indicated but not present, using SIB3");
        sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
      }
      else
      {
        sib3_4_access_restriction_ptr = &(sib4_ptr->cellAccessRestriction);
      }
    }
    else
    {
      sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
    }
  }
  else
  {
    sib3_4_access_restriction_ptr = &(sib3_ptr->cellAccessRestriction);
  }

  /* Make sure memory allocation is successful before to proceed with 
  logging */
  if(log_record_ptr != NULL)
  {
    /* Update dl_uarfcn */
    log_record_ptr->dl_uarfcn = rrc_ccm_cell_camping_info.curr_cell.freq;
    log_record_ptr->psc = rrc_ccm_cell_camping_info.curr_cell.scr_code;
    log_record_ptr->mcc[0] = plmn_id.mcc[0];
    log_record_ptr->mcc[1] = plmn_id.mcc[1];
    log_record_ptr->mcc[2] = plmn_id.mcc[2];
    log_record_ptr->mnc[0] = plmn_id.mnc[0];
    log_record_ptr->mnc[1] = plmn_id.mnc[1];
    log_record_ptr->mnc[2] = plmn_id.mnc[2];
    if (plmn_id.num_mnc_digits == 2) 
    {
      log_record_ptr->mnc[2] = 0xF;
    }
    log_record_ptr->lac = 0;
    log_record_ptr->rac = 0;
    if (sib1_ptr != NULL)
    {
      (void)rrccsp_fill_nas_common_info(sib1_ptr, &nas_common_info);
      log_record_ptr->lac = b_unpackw(nas_common_info.value, 0, 16);

      cn_domain_sys_info_list_ptr =  &sib1_ptr->cn_DomainSysInfoList; 
      do
      {            
        /* First check if Core network is GSM MAP. */
        if((cn_domain_sys_info_list_ptr->elem[idx].cn_Type.t == T_rrc_CN_DomainSysInfo_cn_Type_gsm_MAP)&&
           (cn_domain_sys_info_list_ptr->elem[idx].cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain))
        {
          if(rrccsp_fill_nas_cn_specific_info(&cn_domain_sys_info_list_ptr->elem[idx], 
                                              &rrc_cn_domain_sys_info) != FAILURE)
          {
            log_record_ptr->rac = 
              rrc_cn_domain_sys_info.cn_domain_specific_info.gsm_map_info.value[0];
          }
        }
        idx++;

      }while(cn_domain_sys_info_list_ptr->n > idx); 

    }

    log_record_ptr->ul_uarfcn = rrc_ccm_calculate_ul_uarfcn(log_record_ptr->dl_uarfcn,plmn_id);
    WRRC_MSG2_HIGH("UL UARFCN : %d, DL UARFCN: %d",log_record_ptr->ul_uarfcn,log_record_ptr->dl_uarfcn);

    /* Update cell_id */
    log_record_ptr->cell_id = rrc_ccm_cell_camping_info.curr_cell.cell_id;

    /* Update ura_id */
    if(sib2_ptr != NULL)
    {
      rrc_translate_ura_id(sib2_ptr->ura_IdentityList.elem, &ura_id);
      log_record_ptr->ura_id = (uint16) ura_id;
    }
    else
    {
      log_record_ptr->ura_id = 0;
    } 

    /* Update cell_access_rest */
    log_record_ptr->cell_access_rest = 0;
    if(sib3_4_access_restriction_ptr->cellBarred.t == T_rrc_CellBarred_barred)
    {
      log_record_ptr->cell_access_rest |= RRCLOG_CELL_BARRED;
    }
    else
    {
      log_record_ptr->cell_access_rest |= RRCLOG_CELL_NOT_BARRED;
    }

    if(sib3_4_access_restriction_ptr->cellReservedForOperatorUse == rrc_ReservedIndicator_reserved)
    {
      log_record_ptr->cell_access_rest |= RRCLOG_CELL_RESERVED;
    }
    else
    {
      log_record_ptr->cell_access_rest |= RRCLOG_CELL_NOT_RESERVED;
    }

    if(sib3_4_access_restriction_ptr->cellReservationExtension == rrc_ReservedIndicator_reserved)
    {
      log_record_ptr->cell_access_rest |= RRCLOG_CELL_RESERVED_SOLSA;
    }
    else
    {
      log_record_ptr->cell_access_rest |= RRCLOG_CELL_NOT_RESERVED_SOLSA;
    }

    if (rrc_ccm_cell_camping_info.cell_camp_status == RRC_NEW_CELL_SELECTED)
    {
      log_record_ptr->cell_access_rest |= RRCLOG_UE_CAMPED;
    }
    else
    {
      log_record_ptr->cell_access_rest |= RRCLOG_UE_NOT_CAMPED;
    }

    /* temporary structure to hold current cell access */
    cell_access =rrc_ccm_cell_camping_info.curr_cell_access;

    if(rrc_ccm_cell_camping_info.curr_dsac_present == TRUE)
    {
      if((rrc_ccm_cell_camping_info.curr_cs_cell_access == MM_AS_ACCESS_ALL_CALLS)
         ||(rrc_ccm_cell_camping_info.curr_ps_cell_access == MM_AS_ACCESS_ALL_CALLS))
      {
         cell_access = MM_AS_ACCESS_ALL_CALLS;
         log_record_ptr->call_access = RRCLOG_ALL_CALLS;
      }
      else
      {
        cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
        log_record_ptr->call_access = RRCLOG_EMERGENCY_CALLS;
      }
      WRRC_MSG1_HIGH("DSAC: CCM cell access for cell camping info",cell_access);
    }
    /* Update call_access */
    if ((rrc_ccm_cell_camping_info.curr_cell_access == MM_AS_ACCESS_ALL_CALLS) ||
        (rrc_ccm_cell_camping_info.curr_cell_access == MM_AS_ACCESS_NORMAL_ONLY))
    {
      log_record_ptr->call_access = RRCLOG_ALL_CALLS;
    }
    else
    {
      log_record_ptr->call_access = RRCLOG_EMERGENCY_CALLS;
    } 

    /*commit the log record */
    log_commit(log_record_ptr);
  }
  else
  {
    MSG_MED("diag is not enabled", 0, 0, 0);
  }

#else
#error code not present
#endif /*QXDM_LOGGING */

} /* rrc_ccm_log_cell_info */

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
boolean  rrcccm_waiting_for_reselection_cnf(void)
{
  if(rrc_ccm_substate == RRC_CCM_WAIT_CELL_RESELECTION_CNF)
  {
    /* Still waiting for cell reselection */
    return(TRUE);
  }
  else
  {
    /* CCM could have been taken to Idle due to a State Change */
    return(FALSE);
  }
} /* rrcccm_waiting_for_reselection_cnf */

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
rrc_freq_type rrc_ccm_get_curr_camped_freq(void)
{
  return(rrc_ccm_cell_camping_info.curr_cell.freq);
}

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
rrc_scr_code_type rrc_ccm_get_curr_camped_psc(void)
{
  return(SCR_CODE_TO_PRI_SCR_CODE(rrc_ccm_cell_camping_info.curr_cell.scr_code));
}

/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPED_PLMN

DESCRIPTION       This function is responsible for returning the current 
                  Camped on PLMN of the UE.

DEPENDENCIES      NONE

RETURN VALUE
                  Currently camped on PLMN

SIDE EFFECTS      None.
===========================================================================*/
rrc_plmn_identity_type rrc_ccm_get_curr_camped_plmn(void)
{
  return(rrc_ccm_cell_camping_info.curr_cell.plmn_id);
}

/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPED_CELL_ID

DESCRIPTION       This function is responsible for returning the current 
                  Camped on cell id of the UE.

DEPENDENCIES      NONE

RETURN VALUE
                  Currently camped on cell

SIDE EFFECTS      None.
===========================================================================*/
rrc_int_cell_id_type rrc_ccm_get_curr_camped_cell_id(void)
{
  return(rrc_ccm_cell_camping_info.curr_cell.cell_id);
}

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION          RRC_CCM_GET_CURR_CAMPED_CSG_ID

DESCRIPTION       This function returns current camped CSG id of the UE.

DEPENDENCIES      NONE

RETURN VALUE
                  CSG ID if camped on CSG cell else invalid value

SIDE EFFECTS      None.
===========================================================================*/
rrc_int_csg_id_type rrc_ccm_get_curr_camped_csg_id(void)
{
  return(rrc_ccm_cell_camping_info.curr_cell.csg_id);
}
#endif
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

mm_as_cell_access_status_T
rrc_ccm_get_curr_cell_access_status(void)
{
  if((rrc_ccm_cell_camping_info.status != STATUS_VALID) ||
     (rrc_ccm_cell_camping_info.cell_camp_status == RRC_CELL_LOSS_NO_NEW_CELL)
     )
  {
    /* This is the case when we are not camped on any cell */
    return(MM_AS_ACCESS_NO_CALLS);
  }
  else
  {
    /* camped on a cell - return the current cell access status */
    return(rrc_ccm_cell_camping_info.curr_cell_access);
  }
} /* rrc_ccm_get_curr_cell_access_status */


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
rrc_ccm_get_serv_plmn_cellid(rrc_int_cell_id_type *cell_id, rrc_plmn_identity_type *plmn_id)
{
  if((rrc_ccm_cell_camping_info.cell_camp_status != RRC_NEW_CELL_SELECTED) ||
     (rrc_ccm_cell_camping_info.status != STATUS_VALID))
  {
    return(RRC_CELL_LOSS_NO_NEW_CELL);
  }
  else
  {
    *cell_id = rrc_ccm_cell_camping_info.curr_cell.cell_id;
    *plmn_id = rrc_ccm_cell_camping_info.curr_cell.plmn_id;
    return(RRC_NEW_CELL_SELECTED);
  }
} /* rrc_ccm_get_serv_plmn_cellid */

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
rrc_csp_acq_entry_type rrc_ccm_get_curr_cell_info(void)
{
  return(rrc_ccm_cell_camping_info.curr_cell);
}
#endif

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
boolean  rrc_ccm_is_reselection_in_progress(void)
{
  if(rrc_ccm_cell_camping_info.status == STATUS_INVALID_CELL_RESELECTION)
  {
    /* We are in cell reselection */
    return(TRUE);
  }
  else
  {
    /* Not in Cell Reselection at present */
    return(FALSE);
  }
}

/*===========================================================================

FUNCTION          RRC_CCM_PROPAGATE_NEW_CELL_TO_ALL_PROCS

DESCRIPTION       This function is responsible for propagating the received 
                  RRC_NEW_CELL_IND to all procedures that deserve to be 
                  informed of it. 
                  If a procedure's ID appears as "No Ind Procedure ID" in the
                  cmd, it is NOT informed of RRC_NEW_CELL_IND.
                  
DEPENDENCIES
                  None

RETURN VALUE
                  None
SIDE EFFECTS
                  A possible side effect could come from the order in which
                  procedures are informed. The Cell Selection Procedure must
                  always be informed (if not indicated otherwise in cmd) before
                  any other procedures.
===========================================================================*/
void  rrc_ccm_propagate_new_cell_to_all_procs( rrc_cmd_type  *cmd_ptr )
{
  /* First look at the New Cell status. Different set of procedures need be
   * informed depending on if a New Cell has been selected or if there has
   * been a Cell Loss without new cell selection.
   */
  rrc_state_e_type rrc_state;
  rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrc_ccm_propagate_new_cell_to_all_procs is called in rrc_state = rrc_state_e_type_value%d and new_cell_ind.new_cell_status = %d", rrc_state, cmd_ptr->cmd.new_cell_ind.new_cell_status);

  if(cmd_ptr->cmd.new_cell_ind.new_cell_status == RRC_NEW_CELL_SELECTED)
  {
    /* New Cell selected */
    /* Send New Cell Ind to the relevant procedures. The procedure corresponding
     * to "No Ind Procedure ID" in the cmd must not get the Ind.
     */

    /* CELL SELECTION Procedure - should always be the first one to be in synch
     * with any changes related to Cell.
     */
    if((cmd_ptr->cmd.new_cell_ind.no_ind_proc_id != RRC_PROCEDURE_CSP) &&
       (cmd_ptr->cmd.new_cell_ind.cell_same_as_before == FALSE))
    {
      WRRC_MSG1_HIGH("New Cell Ind (New Cell) sent to Proc rrc_proc_e_type_value%d", RRC_PROCEDURE_CSP);
      rrc_cell_selection_procedure(cmd_ptr);
    }

    /* RRC CONNECTION ESTABLISHMENT Procedure should be sent the New Cell Ind
    * only if it last checked the camping status and found it to be "Not Camping"
    */

    if(rrc_state == RRC_STATE_DISCONNECTED)
    {
      if((cmd_ptr->cmd.new_cell_ind.no_ind_proc_id != RRC_PROCEDURE_RCE) &&
         (cmd_ptr->cmd.new_cell_ind.inform_procedure_rce))
      {
        WRRC_MSG1_HIGH("New Cell Ind (New Cell) sent to Proc rrc_proc_e_type_value%d", RRC_PROCEDURE_RCE);
        /* Call RRC Connection Establishment procedure here */
        rrcrce_procedure_event_handler(cmd_ptr);
      }
      else if(rrcrce_get_rce_sub_state()  ==  RRCRCE_WAIT_FOR_SIB7_CURRENT)
      {
        /*RCE procedure needs New Cell Ind  in its sub state wait for sib7 to get current.
          Since cell reselection should be considered as implicate SIB7 update */
        WRRC_MSG1_HIGH("New Cell Ind (New Cell) sent to Proc rrc_proc_e_type_value%d", RRC_PROCEDURE_RCE);
        /* Call RRC Connection Establishment procedure here */
        rrcrce_procedure_event_handler(cmd_ptr);
      }
    }
    
    /* Send the NEW_CELL_IND to Cell Update Procedure only if the current RRC state is
     * not DISCONNECTED
     */
    if((rrc_state == RRC_STATE_CELL_FACH) || (rrc_state == RRC_STATE_CELL_PCH) ||
	   (rrc_state == RRC_STATE_URA_PCH))
    {
      if(cmd_ptr->cmd.new_cell_ind.no_ind_proc_id != RRC_PROCEDURE_CU) 
      {
        WRRC_MSG1_HIGH("New Cell Ind (New Cell) sent to Proc rrc_proc_e_type_value%d", RRC_PROCEDURE_CU);
        rrccu_procedure_event_handler(cmd_ptr);
      }
    }
    else if(rrc_state == RRC_STATE_CONNECTING)
    {
      if(cmd_ptr->cmd.new_cell_ind.no_ind_proc_id != RRC_PROCEDURE_RCE)
      {
        WRRC_MSG1_HIGH("New Cell Ind (New Cell) sent to Proc rrc_proc_e_type_value%d", RRC_PROCEDURE_RCE);
        rrcrce_procedure_event_handler(cmd_ptr);
      }
    }

    /* Lower Layer Controller Procedure */
    if((rrc_state == RRC_STATE_CELL_FACH) || (rrc_state == RRC_STATE_CELL_PCH) ||
	    (rrc_state == RRC_STATE_URA_PCH) || (rrc_state == RRC_STATE_CONNECTING) || 
        (rrc_state == RRC_STATE_DISCONNECTED))
    {
      if((cmd_ptr->cmd.new_cell_ind.no_ind_proc_id != RRC_PROCEDURE_LLC) &&
         (cmd_ptr->cmd.new_cell_ind.cell_same_as_before == FALSE))
      {
        WRRC_MSG1_HIGH("New Cell Ind (New Cell) sent to Proc rrc_proc_e_type_value%d", RRC_PROCEDURE_LLC);
        rrcllc_process_cmd(cmd_ptr);
      }
    }
    
    /* Add new procedures - determine their relative priority and put them in 
     * the corect order.
     */

  } /* New cell selected */

  else if(cmd_ptr->cmd.new_cell_ind.new_cell_status == RRC_CELL_LOSS_NO_NEW_CELL)
  {
    /* Cell loss with no new cell */
    /* CELL SELECTION Procedure - should always be the first one to be in synch
     * with any changes related to Cell.
     */
    if(cmd_ptr->cmd.new_cell_ind.no_ind_proc_id != RRC_PROCEDURE_CSP)
    {
      WRRC_MSG1_HIGH("New Cell Ind (Cell Loss) sent to Proc rrc_proc_e_type_value%d", RRC_PROCEDURE_CSP);
      rrc_cell_selection_procedure(cmd_ptr);
    }

  } /* Cell loss with no new cell */

  else /* Invalid Cell Status type */
  {
    WRRC_MSG1_HIGH("Invalid Cell Status %din New Cell Ind",cmd_ptr->cmd.new_cell_ind.new_cell_status);
  }

} /* rrc_ccm_propagate_new_cell_to_all_procs  */



/*===========================================================================

FUNCTION          RRC_CCM_SEND_CHAN_CONFIG_CMD

DESCRIPTION       This function is responsible for sending 
                  RRC_CHANNEL_CONFIG_REQ. This function decides
                  on the action needed on RB0 (CCCH) and State Change
                  based on the passed current RRC state.
                  It also sends an Establishment request for PCCH. 
                  NOTE: Cell Change Manager only limits to RB0 handling in
                  specific scenarios. 
                  TBD: If there is a need to reconfig PCCH on a cell change, 
                  how is that handled and who handles it ?
                  RB Setups/Releases for RB0 and other RBs are handled 
                  by other procedures.

DEPENDENCIES
                  None.

RETURN VALUE
                  None.

SIDE EFFECTS      None.
===========================================================================*/
static void  rrc_ccm_send_chan_config_cmd
(
 rrc_state_e_type  current_rrc_state,
 rrc_cell_change_e_type  cell_change_type
)
{

  rrc_cmd_type   *csp_out_cmd_ptr;
  uint32 count=0;
  uint32 urb_count=0;
  uint32 rab_count;
  uint32 rb_count;
  rlc_lc_id_type ul_rlc_lc_id = RRCLCM_RLC_LC_ID_NOT_FOUND;
  rrc_RB_Identity rb_id;

  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    csp_out_cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
    csp_out_cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_CCM;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
    csp_out_cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;
    csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = RRCLLC_CHAN_CFG_REASON_NONE;

    switch(current_rrc_state)
    {
      case RRC_STATE_CONNECTING:

        if(cell_change_type == RRC_CHANGE_RETURN_TO_SERVICE_AREA)
        {
          /* Setup CCCH, drop BCCH */
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 2;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = CCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_config = RELEASE_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
            RRCLLC_CHAN_CFG_REASON_RE_ENTERING_SERVICE;
        }
        else
        {
          ERR_FATAL("Cell chg type %d invalid in RRC st %d", 
                        cell_change_type, current_rrc_state, 0); 
        }
        break;

      case RRC_STATE_CELL_FACH:
        if(cell_change_type == RRC_CHANGE_RETURN_TO_SERVICE_AREA)
        {
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = CCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RELEASE_RB;

          /*add dcch+dtch RBs in to chan config. Read this info from
          established_rabs variable */
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_UM_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_AM_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_DT_HIGH_PRI_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          /* check whether RB4 was present or not */
          ul_rlc_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH, 
                                       DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                       UE_MODE_ACKNOWLEDGED_DATA);

          if(ul_rlc_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
          {
            WRRC_MSG0_HIGH("No RB4 in CELL_FACH");
          }
          else 
          {
            csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_DT_LOW_PRI_RADIO_BEARER_ID;
            csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          }
          /*setup the user plane RBs also at this time. Do not have to worry about the
          ul transmission as it will not happen until we get a valid C-RNTI in the 
          cell update confirm message */
          for (rab_count=0; rab_count<MAX_RAB_TO_SETUP; rab_count++)
          {
            if (
                 ((rrc_est_rabs.rabs[rab_count].cn_domain == RRC_CS_DOMAIN_CN_ID)|| 
                   (rrc_est_rabs.rabs[rab_count].cn_domain == RRC_PS_DOMAIN_CN_ID))
                  && 
                  (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0)
                )
            {
              /*it is a valid established rab, now compare each rb-id in this established rab with 
              the rb-id received in the message */
              for (rb_count=0; ((rb_count<rrc_est_rabs.rabs[rab_count].num_rbs_for_rab) && (rb_count<MAX_RB_PER_RAB) && (count<MAX_RB)); rb_count++)
              {
                csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = 
                  rrc_est_rabs.rabs[rab_count].rb_for_rab[rb_count].rb_id;
                csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
                urb_count++;
              }
            } /*end of if*/
          }/* rab_count loop */

          if((rrclcm_get_rb_id_if_dl_lc_type_exists(UE_LOGCHAN_TM_DCCH_DL, &rb_id) == RRCLCM_SUCCESS) && (count<MAX_RB))
          {
            csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = rb_id;
            csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          }
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = (uint16)count;

          csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;
          WRRC_MSG2_HIGH("RRC_CHANNEL_CONFIG_REQ to reconfig %dRBs,uplane RBs:%d",
                   count,urb_count);
          csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
            RRCLLC_CHAN_CFG_REASON_RE_ENTERING_SERVICE;
        }
        else if(cell_change_type == RRC_CHANGE_CELL_FACH_TO_CELL_FACH)
        {
          /* Add PCCH, drop BCCH */
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_CELL_FACH;
          
        

          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = CCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RELEASE_RB;

           /*add dcch+dtch RBs in to chan config. Read this info from
          established_rabs variable */
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_UM_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_AM_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_DT_HIGH_PRI_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          /* check whether RB4 was present or not */
          ul_rlc_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH, 
                                       DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                       UE_MODE_ACKNOWLEDGED_DATA);

          if(ul_rlc_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
          {
            WRRC_MSG0_HIGH("No RB4 in CELL_FACH");
          }
          else 
          {
            csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_DT_LOW_PRI_RADIO_BEARER_ID;
            csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          }
          /*setup the user plane RBs also at this time. Do not have to worry about the
          ul transmission as it will not happen until we get a valid C-RNTI in the 
          cell update confirm message */
          for (rab_count=0; rab_count<MAX_RAB_TO_SETUP; rab_count++)
          {
            if (
                 ((rrc_est_rabs.rabs[rab_count].cn_domain == RRC_CS_DOMAIN_CN_ID)|| 
                   (rrc_est_rabs.rabs[rab_count].cn_domain == RRC_PS_DOMAIN_CN_ID))
                  &&
                  (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0)
                )
            {
              /*it is a valid established rab, now compare each rb-id in this established rab with 
              the rb-id received in the message */
              for (rb_count=0; ((rb_count<rrc_est_rabs.rabs[rab_count].num_rbs_for_rab) && (rb_count<MAX_RB_PER_RAB)&& (count<MAX_RB)); rb_count++)
              {
                csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = 
                  rrc_est_rabs.rabs[rab_count].rb_for_rab[rb_count].rb_id;
                csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
                urb_count++;
              }
            } /*end of if*/
          }/* rab_count loop */

          if((rrclcm_get_rb_id_if_dl_lc_type_exists(UE_LOGCHAN_TM_DCCH_DL, &rb_id) == RRCLCM_SUCCESS)&& (count<MAX_RB))
          {
            csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = rb_id;
            csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RECONFIG_RB;
          }
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = count;
          csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;
          if((rrcllc_get_toc_usage() == TOC_FOR_OOS_WITHOUT_DCH_INFO) || 
             (rrcllc_get_toc_usage() == TOC_FOR_OOS_WITH_DCH_INFO))
          {
            csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
                                                                      RRCLLC_CHAN_CFG_REASON_RE_ENTERING_SERVICE;
          }
          else
          {
            csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
                                                        RRCLLC_CHAN_CFG_REASON_WITHIN_FACH;
          }
        }
        else if(cell_change_type == RRC_CHANGE_CELL_FACH_TO_CELL_PCH)
        {
          /* Add PCCH, drop BCCH */
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_CELL_PCH;
          
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 2;

          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = PCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_config = RELEASE_RB;

          csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
                                                                       RRCLLC_CHAN_CFG_REASON_FACH_TO_PCH;
          csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;
        }
        else if(cell_change_type == RRC_CHANGE_CELL_FACH_TO_URA_PCH)
        {
          /* Add PCCH, drop BCCH */
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_URA_PCH;
          
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 2;

          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = PCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_config = RELEASE_RB;

          csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
                                                                       RRCLLC_CHAN_CFG_REASON_FACH_TO_PCH;
          csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;
        }
        else
        {
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 1;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = CCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = RECONFIG_RB;
        }
        break;

      case RRC_STATE_CELL_PCH:
        if(cell_change_type == RRC_CHANGE_RETURN_TO_SERVICE_AREA)
        {
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_CELL_PCH;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = PCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = RELEASE_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = (uint16)count;

          csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
            RRCLLC_CHAN_CFG_REASON_RE_ENTERING_SERVICE;

        }
        else
        {
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_CELL_FACH;
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 1;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = CCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
        }
        break;

      case RRC_STATE_URA_PCH:
        if(cell_change_type == RRC_CHANGE_RETURN_TO_SERVICE_AREA)
        {
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_URA_PCH;
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 2;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = PCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_config = RELEASE_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.chan_config_reason = 
            RRCLLC_CHAN_CFG_REASON_RE_ENTERING_SERVICE;
        }
        else
        {
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_CELL_FACH;
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 1;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = CCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
        }
        break;

      case RRC_STATE_DISCONNECTED:
        if(cell_change_type == RRC_CHANGE_CELL_SELECTION)
        {
          /* Add PCCH, drop BCCH */
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 2;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = PCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_config = RELEASE_RB;
        }
        else
        {
          ERR_FATAL("Cell Chg type %d invalid in RRC st %d", cell_change_type, current_rrc_state, 0); 
        }
        break;

      case RRC_STATE_CELL_DCH:
        if(cell_change_type == RRC_CHANGE_CELL_DCH_TO_CELL_FACH)
        {
          /* Add PCCH, drop BCCH */
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_CELL_FACH;
          
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 2;

          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = CCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_config = RELEASE_RB;

          csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;
        }
        else if(cell_change_type == RRC_CHANGE_CELL_DCH_TO_CELL_PCH)
        {
          /* Add PCCH, drop BCCH */
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_CELL_PCH;
          
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 2;

          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = PCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_config = RELEASE_RB;

          csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;
        }
        else if(cell_change_type == RRC_CHANGE_CELL_DCH_TO_URA_PCH)
        {
          /* Add PCCH, drop BCCH */
          csp_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
          csp_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_URA_PCH;
          
          csp_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 2;

          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_id = PCCH_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[0].rb_config = ESTABLISH_RB;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_id = BCCH_S_RADIO_BEARER_ID;
          csp_out_cmd_ptr->cmd.chan_config_req.rb[1].rb_config = RELEASE_RB;

          csp_out_cmd_ptr->cmd.chan_config_req.release_phychls = FALSE;
          csp_out_cmd_ptr->cmd.chan_config_req.release_trchls = FALSE;
        }
        else
        {
          ERR_FATAL("Cell Chg type %d rxed in CELL_DCH", cell_change_type, 0, 0);
        }
        break;

      default:
        ERR_FATAL("No Channel Config to LLC in RRC State %d", current_rrc_state, 0, 0);
    }

    /* Send the Internal RRC Command - LLC should directly handle it */
    rrc_put_int_cmd( csp_out_cmd_ptr );
  }


} /* rrc_ccm_send_chan_config_cmd */


/*===========================================================================

FUNCTION          RRC_CSP_SEND_MM_SERVICE_IND

DESCRIPTION       This function is responsible for sending RRC_SERVICE_IND
                  to MM.

DEPENDENCIES
                  SIB1 should be available.

RETURN VALUE
                  SUCCESS: Able to send RRC_SERVICE_IND successfully.
                  FAILURE: Not able to send due to any of the multiple reasons.

SIDE EFFECTS      None.
===========================================================================*/
static uecomdef_status_e_type rrc_csp_send_mm_service_ind(void)
{
  /* TBD: Send the NAS system info from SIB1 to MM */

  return(SUCCESS);
}

/*===========================================================================

FUNCTION          RRC_CCM_FILL_NAS_CN_SPECIFIC_INFO

DESCRIPTION       This function is responsible for filling the Core Network 
                  information to MM.

DEPENDENCIES
                  

RETURN VALUE
                  SUCCESS: Able to fill the CN info successfully.
                  FAILURE: Not able to fill the CN info.

SIDE EFFECTS      None.
===========================================================================*/
static uecomdef_status_e_type  
rrc_ccm_fill_nas_cn_specific_info
(
  rrc_CN_DomainInformation rrc_cn_domain_information,
  rrc_cn_domain_sys_info_type  *rrc_cn_domain_sys_info_ptr
)
{
  unsigned char  *data_ptr;
  uint32          i;

  if(rrc_cn_domain_information.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
  {
    rrc_cn_domain_sys_info_ptr->cn_domain_id = RRC_CS_DOMAIN_CN_ID; 
  }
  else
  {
    rrc_cn_domain_sys_info_ptr->cn_domain_id = RRC_PS_DOMAIN_CN_ID;
  }
    

  /* Check if the RRC MM interface structure array can hold this info */
  if( RRC_GET_OCTET_STRING_LEN(rrc_cn_domain_information.cn_DomainSpecificNAS_Info) >
     RRC_MAX_GSM_INFO_OCTET_COUNT)
  {
    WRRC_MSG1_ERROR("Not enough space for NAS Sys Info (%d bytes)",
         RRC_GET_OCTET_STRING_LEN(rrc_cn_domain_information.cn_DomainSpecificNAS_Info));
    return(FAILURE);
  }
  
  rrc_cn_domain_sys_info_ptr->cn_domain_specific_info.gsm_map_info.length 
  = RRC_GET_OCTET_STRING_LEN(rrc_cn_domain_information.cn_DomainSpecificNAS_Info);

  data_ptr = RRC_GET_OCTET_STRING_PTR(rrc_cn_domain_information.cn_DomainSpecificNAS_Info);

  for( i = 0; i < RRC_GET_OCTET_STRING_LEN(rrc_cn_domain_information.cn_DomainSpecificNAS_Info); i ++)
  {
    rrc_cn_domain_sys_info_ptr->cn_domain_specific_info.gsm_map_info.value[i] =
     *data_ptr;

    data_ptr ++;
  }

  return(SUCCESS);
} /* rrc_ccm_fill_nas_cn_specific_info */
/*===========================================================================

FUNCTION          RRC_CCM_FILL_NAS_CN_SPECIFIC_INFO_FULL

DESCRIPTION       This function is responsible for filling the Core Network 
                  information to MM.

DEPENDENCIES
                  

RETURN VALUE
                  SUCCESS: Able to fill the CN info successfully.
                  FAILURE: Not able to fill the CN info.

SIDE EFFECTS      None.
===========================================================================*/
static uecomdef_status_e_type  
rrc_ccm_fill_nas_cn_specific_info_full
(
  rrc_CN_DomainInformationFull rrc_cn_domain_information_full,
  rrc_cn_domain_sys_info_type  *rrc_cn_domain_sys_info_ptr
)
{
  unsigned char  *data_ptr;
  uint32          i;

  if(rrc_cn_domain_information_full.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
  {
    rrc_cn_domain_sys_info_ptr->cn_domain_id = RRC_CS_DOMAIN_CN_ID; 
  }
  else
  {
    rrc_cn_domain_sys_info_ptr->cn_domain_id = RRC_PS_DOMAIN_CN_ID;
  }
  /* Check if the RRC MM interface structure array can hold this info */
  if((RRC_GET_OCTET_STRING_LEN(rrc_cn_domain_information_full.cn_DomainSpecificNAS_Info) >
     RRC_MAX_GSM_INFO_OCTET_COUNT))
  {
    WRRC_MSG1_ERROR("Not enough space for NAS Sys Info (%d bytes)",
         RRC_GET_OCTET_STRING_LEN(rrc_cn_domain_information_full.cn_DomainSpecificNAS_Info));
    return(FAILURE);
  }
  
  rrc_cn_domain_sys_info_ptr->cn_domain_specific_info.gsm_map_info.length 
  = RRC_GET_OCTET_STRING_LEN(rrc_cn_domain_information_full.cn_DomainSpecificNAS_Info);

  data_ptr = RRC_GET_OCTET_STRING_PTR(rrc_cn_domain_information_full.cn_DomainSpecificNAS_Info);

  for( i = 0; i < RRC_GET_OCTET_STRING_LEN(rrc_cn_domain_information_full.cn_DomainSpecificNAS_Info); i ++)
  {
    rrc_cn_domain_sys_info_ptr->cn_domain_specific_info.gsm_map_info.value[i] =
     *data_ptr;

    data_ptr ++;
  }

  return(SUCCESS);
} /* rrc_ccm_fill_nas_cn_specific_info_full */

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
)
{
  /* Send the CN info to MM */

  mm_cmd_type *mm_buf_ptr = NULL;
  uint32 num_cn_domains = 0;
  unsigned char  *data_ptr;
  uint32          i;
  rrc_int_cell_id_type cell_id = 0;
  rrc_plmn_identity_type plmn_id;
 
 

  if(cn_information_info_ptr == NULL)
  {
    WRRC_MSG0_ERROR("NULL Pointer for CN Information");
    return(mm_buf_ptr);
  }

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_SERVICE_IND_FROM_OTA_MSG);

#ifdef FEATURE_FEMTO_CSG
  mm_buf_ptr->cmd.rrc_service_ind.service_info.csg_info.csg_id = SYS_CSG_ID_INVALID;
#endif

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_service_ind.as_id = rrc_get_as_id();
#endif

#ifdef FEATURE_QCHAT
  WRRC_MSG0_HIGH("Notify cell-plmn id change to QChat UPK");
  qchatupk_update_location(SYS_SYS_MODE_WCDMA);
#endif /* FEATURE_QCHAT */

  /* Fill in the service status */
  if ((rrc_ccm_get_serv_plmn_cellid(&cell_id, &plmn_id) == RRC_CELL_LOSS_NO_NEW_CELL))
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_NO_SERVICE;
  }
  else
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_SERVICE_AVAILABLE;
  }

  mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = FALSE;

  if(mm_buf_ptr->cmd.rrc_service_ind.service_status == MM_AS_SERVICE_AVAILABLE)
  {

#ifdef FEATURE_WCDMA_MULTIBAND
    rrc_csp_band_class_type band_class = rrc_get_frequency_band(rrc_ccm_get_curr_camped_freq(), rrc_ccm_get_curr_camped_plmn());
    sys_band_mask_e_type sys_band = rrc_convert_band_to_sys_band(band_class);

    mm_buf_ptr->cmd.rrc_service_ind.active_band = sys_band;
#endif

    if (rrc_ccm_cell_camping_info.is_curr_cell_id_valid)
    {
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.cell_id = cell_id;
      WRRC_MSG1_HIGH("Reporting Cell_ID :%d to NAS",cell_id);
    }

    /* Fill the cell access */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access =
      rrc_csp_int_data.curr_cell_access_status;
    if(rrc_csp_int_data.is_dsac_valid == TRUE)
    {
       mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present  = 
                                                 rrc_csp_int_data.is_dsac_valid;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access = 
                                                 rrc_csp_int_data.curr_cs_cell_access;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access = 
                                                 rrc_csp_int_data.curr_ps_cell_access;
    }
    else
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present = rrc_csp_int_data.is_dsac_valid;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access = rrc_csp_int_data.curr_cell_access_status;
    }

    if(rrc_csp_int_data.is_dsac_valid == TRUE)
    {
      MSG_MED("DSAC: DSAC info is valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
      rrc_ccm_cell_camping_info.curr_cs_cell_access =   
                                            mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access;
      rrc_ccm_cell_camping_info.curr_ps_cell_access =   
                                 mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access;
    }
    else
    {
       MSG_MED("DSAC: DSAC info is not valid for CCM",0,0,0);
       rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.ppac_present = rrc_csp_int_data.is_ppac_valid;

    if(rrc_csp_int_data.is_ppac_valid == TRUE)
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_cs_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_ps_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed = 
                                                rrc_csp_int_data.curr_cell_page_rsp_allowed;
    }

    rrc_ccm_cell_camping_info.curr_ppac_present = rrc_csp_int_data.is_ppac_valid;

    if(rrc_csp_int_data.is_ppac_valid == TRUE)
    {
      MSG_MED("PPAC: PPAC info is valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed = 
                                            mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed = 
                                            mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed = 
                                            mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed;
    }

    MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"DSAC/PPAC: DSAC, CS and PS cell access values are %d, %d, %d cell Access Info is %d PPAC valid, CS reg, PS reg, paging status values are %d, %d, %d, %d",
                        rrc_csp_int_data.is_dsac_valid,
                        rrc_csp_int_data.curr_cs_cell_access,
                        rrc_csp_int_data.curr_ps_cell_access,
                        rrc_csp_int_data.curr_cell_access_status,
                        rrc_csp_int_data.is_ppac_valid,
                        rrc_csp_int_data.curr_cell_cs_reg_allowed,
                        rrc_csp_int_data.curr_cell_ps_reg_allowed,
                        rrc_csp_int_data.curr_cell_page_rsp_allowed);

    /* Fill the PLMN information */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

    mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = FALSE;

    /*lint -e737*/
    /*lint -e650*/

    if(/*Check to see if Primary PLMN I.D is present */
       (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
       rrc_CN_InformationInfo_r6,primary_plmn_Identity))
       ||/*If not then check to see if common PLMN I.D is present */
       (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
       rrc_CN_InformationInfo_r6,plmn_Identity))
      )
    {
      /*lint +e737*/
      /*lint +e650*/
      /*If primary PLMN I.D is present then this should be considered 
       *  over common PLMN I.D.Since common PLMN I.D is optional field
       *  so it may not be included. In theses cases if primary PLMN I.D present
       *  honor it & send to MM
       */
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
	  	rrc_CN_InformationInfo_r6,primary_plmn_Identity))
      {
        WRRC_MSG0_HIGH("NWS:Primary PLMN I.D present in OTA message");
        rrc_translate_gsm_map_plmn_id 
         (&cn_information_info_ptr->primary_plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );
        rrc_ccm_cell_camping_info.curr_cell.plmn_id = 
         mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id;
      }
      else
      {
        rrc_translate_gsm_map_plmn_id 
         (&cn_information_info_ptr->plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );
      }
      /* Update the current requested PLMN with the new PLMN */
      rrc_csp_int_data.curr_req_plmn.plmn_id  = 
        mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id;

      rrc_csp_int_data.primary_plmn_Id_present = cn_information_info_ptr->m.primary_plmn_IdentityPresent;

      PRINT_PLMN_ID(rrc_csp_int_data.curr_req_plmn.plmn_id)
    }
    else
    {
            mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0]= 0xFF;
            mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1]= 0xFF;
            mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]= 0xFF;
            MSG_HIGH("DSAC: Set mcc[0]=0x%x, mcc[1]=0x%x, mcc[2]=0x%x in RRC_SERVICE_IND_FROM_OTA_MSG ",
                                                 mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0],
                                                 mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1],
                                                 mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]);
    }

    /* Fill in the CN common GSM-MAP NAS system information now */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
             rrc_CN_InformationInfo_r6,cn_CommonGSM_MAP_NAS_SysInfo))
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.length =
      RRC_GET_OCTET_STRING_LEN(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      data_ptr = RRC_GET_OCTET_STRING_PTR(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      for( i = 0; i < RRC_GET_OCTET_STRING_LEN(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo); i ++)
      {
        mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.value[i] =
         *data_ptr;

        data_ptr ++;
      }
      /*Store the LAC from the OTA in global variable lac_in_ota*/	  
      if(rrc_get_lac_id_in_dch(&(mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info)) == FAILURE)
      {
        WRRC_MSG0_ERROR("Not able to get LAC from CN Info.");
      }	  
    }

    /* Fill in the CN domain specific NAS system info now */

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
		rrc_CN_InformationInfo_r6,cn_DomainInformationList))
    {
      uint32 idx =0;
      /* Save the head pointer of the linked list */
     

      do
      {
        if(rrc_ccm_fill_nas_cn_specific_info(cn_information_info_ptr->cn_DomainInformationList.elem[idx],
            &mm_buf_ptr->cmd.rrc_service_ind.service_info.
             cn_domain_info[num_cn_domains]) == FAILURE)
        {
          mm_free_cmd_buf(mm_buf_ptr);
          return((mm_cmd_type *)NULL);
        }
        num_cn_domains ++;
        idx++;

      }while((cn_information_info_ptr->cn_DomainInformationList.n >idx) &&
            (num_cn_domains < RRC_MAX_CN_DOMAINS));


      rrc_get_rac_id_in_dch(&(cn_information_info_ptr->cn_DomainInformationList));

     
    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.num_cn_domains =
      num_cn_domains;

  } /* Service is available */

  /* Put the command on MM queue */
  WRRC_MSG1_HIGH("Built MM cmd 0x%x", RRC_SERVICE_IND_FROM_OTA_MSG);

  return(mm_buf_ptr);
} /* rrc_ccm_buid_service_ind_with_nas_cn_info_r6 */

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
)
{
  /* Send the CN info to MM */

  mm_cmd_type *mm_buf_ptr;
  uint32 num_cn_domains = 0;
  unsigned char  *data_ptr;
  uint32          i;
  rrc_int_cell_id_type cell_id = 0;
  rrc_plmn_identity_type plmn_id;
 
 

  if(cn_information_info_ptr == NULL)
  {
    WRRC_MSG0_ERROR("NULL Pointer for CN Information");
    return(FAILURE);
  }

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_SERVICE_IND_FROM_OTA_MSG);

#ifdef FEATURE_FEMTO_CSG
  mm_buf_ptr->cmd.rrc_service_ind.service_info.csg_info.csg_id = SYS_CSG_ID_INVALID;
#endif

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_service_ind.as_id = rrc_get_as_id();
#endif

#ifdef FEATURE_QCHAT
  WRRC_MSG0_HIGH("Notify cell-plmn id change to QChat UPK");
  qchatupk_update_location(SYS_SYS_MODE_WCDMA);
#endif /* FEATURE_QCHAT */

  /* Fill in the service status */
  if ((rrc_ccm_get_serv_plmn_cellid(&cell_id, &plmn_id) == RRC_CELL_LOSS_NO_NEW_CELL))
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_NO_SERVICE;
  }
  else
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_SERVICE_AVAILABLE;
  }

  mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = FALSE;

  if(mm_buf_ptr->cmd.rrc_service_ind.service_status == MM_AS_SERVICE_AVAILABLE)
  {

#ifdef FEATURE_WCDMA_MULTIBAND
    rrc_csp_band_class_type band_class = rrc_get_frequency_band(rrc_ccm_get_curr_camped_freq(), rrc_ccm_get_curr_camped_plmn());
    sys_band_mask_e_type sys_band = rrc_convert_band_to_sys_band(band_class);

    mm_buf_ptr->cmd.rrc_service_ind.active_band = sys_band;
#endif

    if (rrc_ccm_cell_camping_info.is_curr_cell_id_valid)
    {
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.cell_id = cell_id;
      WRRC_MSG1_HIGH("Reporting Cell_ID :%d to NAS",cell_id);
    }

    /* Fill the cell access */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access =
      rrc_csp_int_data.curr_cell_access_status;
    if(rrc_csp_int_data.is_dsac_valid == TRUE)
    {
       MSG_MED("DSAC: DSAC info is present",0,0,0);
       mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present  = 
                                             rrc_csp_int_data.is_dsac_valid;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access = 
                                             rrc_csp_int_data.curr_cs_cell_access;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access = 
                                             rrc_csp_int_data.curr_ps_cell_access;
    }
    else
    {
      MSG_MED("DSAC: DSAC info is not present",0,0,0);
      mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present = rrc_csp_int_data.is_dsac_valid;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access = rrc_csp_int_data.curr_cell_access_status;
    }

    if(rrc_csp_int_data.is_dsac_valid == TRUE) 
    {
      MSG_MED("DSAC: DSAC info is valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_dsac_present = 
                           rrc_csp_int_data.is_dsac_valid;
      rrc_ccm_cell_camping_info.curr_cs_cell_access =   
                           mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access;
      rrc_ccm_cell_camping_info.curr_ps_cell_access =   
                            mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access;
    }
    else
    {
      MSG_MED("DSAC: DSAC info is not valid for CCM",0,0,0);	  
      rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.ppac_present = rrc_csp_int_data.is_ppac_valid;

    if(rrc_csp_int_data.is_ppac_valid == TRUE)
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_cs_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_ps_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed = 
                                                rrc_csp_int_data.curr_cell_page_rsp_allowed;
    }

    rrc_ccm_cell_camping_info.curr_ppac_present = rrc_csp_int_data.is_ppac_valid;

    if(rrc_csp_int_data.is_ppac_valid == TRUE)
    {
      MSG_MED("PPAC: PPAC info is valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed =
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed =
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed = 
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed;
    }

    MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"DSAC/PPAC: DSAC, CS and PS cell access values are %d, %d, %d cell Access Info is %d PPAC valid, CS reg, PS reg, paging status values are %d, %d, %d, %d",
                        rrc_csp_int_data.is_dsac_valid,
                        rrc_csp_int_data.curr_cs_cell_access,
                        rrc_csp_int_data.curr_ps_cell_access,
                        rrc_csp_int_data.curr_cell_access_status,
                        rrc_csp_int_data.is_ppac_valid,
                        rrc_csp_int_data.curr_cell_cs_reg_allowed,
                        rrc_csp_int_data.curr_cell_ps_reg_allowed,
                        rrc_csp_int_data.curr_cell_page_rsp_allowed);

    /* Fill the PLMN information */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

    mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = FALSE;

    /*lint -e737*/
    /*lint -e650*/
    if (/*Check to see if Primary PLMN I.D is present */
       (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
       rrc_CN_InformationInfo_r6,primary_plmn_Identity))
       ||/*If not then check to see if common PLMN I.D is present */
       (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
       rrc_CN_InformationInfo_r6,plmn_Identity))
      )
    {
      /*lint +e737*/
      /*lint +e650*/
      /*If primary PLMN I.D is present then this should be considered 
       *  over common PLMN I.D.Since common PLMN I.D is optional field
       *  so it may not be included. In theses cases if primary PLMN I.D present
       *  honor it & send to MM
       */
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
       rrc_CN_InformationInfo_r6,primary_plmn_Identity))
      {
        WRRC_MSG0_HIGH("NWS:Primary PLMN I.D present in OTA message");
        rrc_translate_gsm_map_plmn_id 
         (&cn_information_info_ptr->primary_plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );
      }
      else 
      {
        rrc_translate_gsm_map_plmn_id 
         (&cn_information_info_ptr->plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );
      }
      /* Update the current requested PLMN with the new PLMN */
      rrc_csp_int_data.curr_req_plmn.plmn_id  = 
        mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id;

      rrc_ccm_cell_camping_info.curr_cell.plmn_id =  mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id;

      rrc_csp_int_data.primary_plmn_Id_present = cn_information_info_ptr->m.primary_plmn_IdentityPresent;

      PRINT_PLMN_ID(rrc_csp_int_data.curr_req_plmn.plmn_id)
    }
    else
    {
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0]= 0xFF;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1]= 0xFF;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]= 0xFF;
      MSG_HIGH("DSAC: Set mcc[0]=0x%x, mcc[1]=0x%x, mcc[2]=0x%x in RRC_SERVICE_IND_FROM_OTA_MSG ",
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0],
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1],
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]);
    }

    /* Fill in the CN common GSM-MAP NAS system information now */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
           rrc_CN_InformationInfo,cn_CommonGSM_MAP_NAS_SysInfo))
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.length =
      RRC_GET_OCTET_STRING_LEN(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      data_ptr = RRC_GET_OCTET_STRING_PTR(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      for( i = 0; i < RRC_GET_OCTET_STRING_LEN(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo); i ++)
      {
        mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.value[i] =
         *data_ptr;

        data_ptr ++;
      }
      /*Store the LAC from the OTA in global variable lac_in_ota*/	  	  
      if(rrc_get_lac_id_in_dch(&(mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info)) == FAILURE)
      {
        WRRC_MSG0_ERROR("Not able to get LAC from CN Info.");
      }	  
    }

    /* Fill in the CN domain specific NAS system info now */

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
		rrc_CN_InformationInfo_r6,cn_DomainInformationList))
    {
     uint32 idx =0;

      do
      {
        if(rrc_ccm_fill_nas_cn_specific_info(cn_information_info_ptr->cn_DomainInformationList.elem[idx],
            &mm_buf_ptr->cmd.rrc_service_ind.service_info.
             cn_domain_info[num_cn_domains]) == FAILURE)
        {
          mm_free_cmd_buf(mm_buf_ptr);
          return(FAILURE);
        }
        num_cn_domains ++;
        idx++;

      }while((cn_information_info_ptr->cn_DomainInformationList.n > idx) &&
            (num_cn_domains < RRC_MAX_CN_DOMAINS));
      /* Restore the head pointer of the linked list */

      rrc_get_rac_id_in_dch(&(cn_information_info_ptr->cn_DomainInformationList));

    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.num_cn_domains =
      num_cn_domains;

  } /* Service is available */

  /* Put the command on MM queue */
  WRRC_MSG1_HIGH("Dispatching out MM cmd RRC_SERVICE_IND_FROM_OTA_MSG / Sending MM cmd 0x%x", RRC_SERVICE_IND_FROM_OTA_MSG);
  rrc_send_rrc_message_to_mm(mm_buf_ptr);

  return(SUCCESS);
} /* rrc_ccm_send_nas_cn_info_r6 */


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
)
{
  /* Send the CN info to MM */

  mm_cmd_type *mm_buf_ptr = NULL;
  uint32 num_cn_domains = 0;
  unsigned char  *data_ptr;
  uint32          i;
  rrc_int_cell_id_type cell_id = 0;
  rrc_plmn_identity_type plmn_id;
 
 
                          /* Local pointer to CN Domain Information Info */

  if(cn_information_info_ptr == NULL)
  {
    WRRC_MSG0_ERROR("NULL Pointer for CN Information");
    return(mm_buf_ptr);
  }

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_SERVICE_IND_FROM_OTA_MSG);

#ifdef FEATURE_FEMTO_CSG
  mm_buf_ptr->cmd.rrc_service_ind.service_info.csg_info.csg_id = SYS_CSG_ID_INVALID;
#endif

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_service_ind.as_id = rrc_get_as_id();
#endif

#ifdef FEATURE_QCHAT
  WRRC_MSG0_HIGH("Notify cell-plmn id change to QChat UPK");
  qchatupk_update_location(SYS_SYS_MODE_WCDMA);
#endif /* FEATURE_QCHAT */

  /* Fill in the service status */
  if ((rrc_ccm_get_serv_plmn_cellid(&cell_id, &plmn_id) == RRC_CELL_LOSS_NO_NEW_CELL))
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_NO_SERVICE;
  }
  else
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_SERVICE_AVAILABLE;
  }

  mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = FALSE;

  if(mm_buf_ptr->cmd.rrc_service_ind.service_status == MM_AS_SERVICE_AVAILABLE)
  {    

#ifdef FEATURE_WCDMA_MULTIBAND
    rrc_csp_band_class_type band_class = rrc_get_frequency_band(rrc_ccm_get_curr_camped_freq(), rrc_ccm_get_curr_camped_plmn());
    sys_band_mask_e_type sys_band = rrc_convert_band_to_sys_band(band_class);

    mm_buf_ptr->cmd.rrc_service_ind.active_band = sys_band;
#endif

    if (rrc_ccm_cell_camping_info.is_curr_cell_id_valid)
    {
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.cell_id = cell_id;
      WRRC_MSG1_HIGH("Reporting Cell_ID :%d to NAS",cell_id);
    }

    /* Fill the cell access */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access =
                                                         rrc_csp_int_data.curr_cell_access_status;
    if(rrc_csp_int_data.is_dsac_valid == TRUE)
    {
       mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present  = 
                                                 rrc_csp_int_data.is_dsac_valid;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access = 
                                                 rrc_csp_int_data.curr_cs_cell_access;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access = 
                                                 rrc_csp_int_data.curr_ps_cell_access;
    }
    else
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present = rrc_csp_int_data.is_dsac_valid;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access = rrc_csp_int_data.curr_cell_access_status;
    }

    if(rrc_csp_int_data.is_dsac_valid == TRUE)
    {
      MSG_MED("DSAC: DSAC info is valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
      rrc_ccm_cell_camping_info.curr_cs_cell_access =   
                                            mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access;
      rrc_ccm_cell_camping_info.curr_ps_cell_access =   
                                 mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access;
    }
    else
    {
       MSG_MED("DSAC: DSAC info is not valid for CCM",0,0,0);
       rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.ppac_present = rrc_csp_int_data.is_ppac_valid;

    rrc_ccm_cell_camping_info.curr_ppac_present = rrc_csp_int_data.is_ppac_valid;

    if(rrc_csp_int_data.is_ppac_valid == TRUE)
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_cs_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_ps_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed = 
                                                rrc_csp_int_data.curr_cell_page_rsp_allowed;

      MSG_MED("PPAC: PPAC info is valid for CCM",0,0,0);

      rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed =
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed =
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed = 
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed;
    }

    MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"DSAC/PPAC: DSAC, CS and PS cell access values are %d, %d, %d cell Access Info is %d PPAC valid, CS reg, PS reg, paging status values are %d, %d, %d, %d",
                        rrc_csp_int_data.is_dsac_valid,
                        rrc_csp_int_data.curr_cs_cell_access,
                        rrc_csp_int_data.curr_ps_cell_access,
                        rrc_csp_int_data.curr_cell_access_status,
                        rrc_csp_int_data.is_ppac_valid,
                        rrc_csp_int_data.curr_cell_cs_reg_allowed,
                        rrc_csp_int_data.curr_cell_ps_reg_allowed,
                        rrc_csp_int_data.curr_cell_page_rsp_allowed);
    /* Fill the PLMN information */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

    mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = FALSE;

    /*lint -e737*/
    /*lint -e650*/
    
    if(/*Check to see if Primary PLMN I.D is present */
       ((primary_plmn_Id_present  == TRUE)&&
        (primary_plmn_Identity != NULL)
       )
       ||/*If not then check to see if common PLMN I.D is present */
       (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
       rrc_CN_InformationInfo,plmn_Identity))
      )
    {
      /*lint +e737*/
      /*lint +e650*/
      /*If primary PLMN I.D is present then this should be considered 
       *  over common PLMN I.D.Since common PLMN I.D is optional field
       *  so it may not be included. In theses cases if primary PLMN I.D present
       *  honor it & send to MM
       */
      if(primary_plmn_Id_present  == TRUE)
      {
        WRRC_MSG0_HIGH("NWS:Primary PLMN I.D present in OTA message");
        rrc_translate_gsm_map_plmn_id 
         (primary_plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );
      }
      else
      {
        rrc_translate_gsm_map_plmn_id 
         (&cn_information_info_ptr->plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );
      }
      /* Update the current requested PLMN with the new PLMN */
      rrc_csp_int_data.curr_req_plmn.plmn_id  = 
        mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id;

      rrc_csp_int_data.primary_plmn_Id_present = primary_plmn_Id_present;

      PRINT_PLMN_ID(rrc_csp_int_data.curr_req_plmn.plmn_id)

    }
    else
    {
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0]= 0xFF;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1]= 0xFF;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]= 0xFF;
      MSG_HIGH("DSAC: Set mcc[0]=0x%x, mcc[1]=0x%x, mcc[2]=0x%x in RRC_SERVICE_IND_FROM_OTA_MSG ",
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0],
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1],
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]);
    }

    /* Fill in the CN common GSM-MAP NAS system information now */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
             rrc_CN_InformationInfo,cn_CommonGSM_MAP_NAS_SysInfo))
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.length =
      RRC_GET_OCTET_STRING_LEN(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      data_ptr = RRC_GET_OCTET_STRING_PTR(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      for( i = 0; i < RRC_GET_OCTET_STRING_LEN(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo); i ++)
      {
        mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.value[i] =
         *data_ptr;

        data_ptr ++;
      }
      /*Store the LAC from the OTA in global variable lac_in_ota*/	 	  
      if(rrc_get_lac_id_in_dch(&(mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info)) == FAILURE)
      {
        WRRC_MSG0_ERROR("Not able to get LAC from CN Info.");
      }	  
    }

    /* Fill in the CN domain specific NAS system info now */

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,rrc_CN_InformationInfo,
		cn_DomainInformationList))
    {
      uint32 idx =0;
     

      do
      {
        if(rrc_ccm_fill_nas_cn_specific_info(cn_information_info_ptr->cn_DomainInformationList.elem[idx],
            &mm_buf_ptr->cmd.rrc_service_ind.service_info.
             cn_domain_info[num_cn_domains]) == FAILURE)
        {
          mm_free_cmd_buf(mm_buf_ptr);
          return((mm_cmd_type *)NULL);
        }
        num_cn_domains ++;
        idx++;

      }while((cn_information_info_ptr->cn_DomainInformationList.n > idx) &&
            (num_cn_domains < RRC_MAX_CN_DOMAINS));
      /* Restore the head pointer of the linked list */

      rrc_get_rac_id_in_dch(&(cn_information_info_ptr->cn_DomainInformationList));

    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.num_cn_domains =
      num_cn_domains;

  } /* Service is available */

  /* Put the command on MM queue */
  WRRC_MSG1_HIGH("Built MM cmd 0x%x", RRC_SERVICE_IND_FROM_OTA_MSG);

  return(mm_buf_ptr);
} /* rrc_ccm_buid_service_ind_with_nas_cn_info */

/*===========================================================================

FUNCTION          RRC_CCM_SEND_NAS_CN_INFO

DESCRIPTION       This function is responsible for sending the NAS specific
                  CN information to MM.

DEPENDENCIES
                  

RETURN VALUE
                  SUCCESS: Able to send RRC_SERVICE_IND_FROM_OTA_MSG successfully.
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
)
{
  /* Send the CN info to MM */

  mm_cmd_type *mm_buf_ptr;
  uint32 num_cn_domains = 0;
  unsigned char  *data_ptr;
  uint32          i;
  rrc_int_cell_id_type cell_id = 0;
  rrc_plmn_identity_type plmn_id;
 
 

  if(cn_information_info_ptr == NULL)
  {
    WRRC_MSG0_ERROR("NULL Pointer for CN Information");
    return(FAILURE);
  }

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_SERVICE_IND_FROM_OTA_MSG);

#ifdef FEATURE_FEMTO_CSG
  mm_buf_ptr->cmd.rrc_service_ind.service_info.csg_info.csg_id = SYS_CSG_ID_INVALID;
#endif

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_service_ind.as_id = rrc_get_as_id();
#endif

#ifdef FEATURE_QCHAT
  WRRC_MSG0_HIGH("Notify cell-plmn id change to QChat UPK");
  qchatupk_update_location(SYS_SYS_MODE_WCDMA);
#endif /* FEATURE_QCHAT */

  /* Fill in the service status */
  if ((rrc_ccm_get_serv_plmn_cellid(&cell_id, &plmn_id) == RRC_CELL_LOSS_NO_NEW_CELL))
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_NO_SERVICE;
  }
  else
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_SERVICE_AVAILABLE;
  }

  mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = FALSE;

  if(mm_buf_ptr->cmd.rrc_service_ind.service_status == MM_AS_SERVICE_AVAILABLE)
  {

#ifdef FEATURE_WCDMA_MULTIBAND
    rrc_csp_band_class_type band_class = rrc_get_frequency_band(rrc_ccm_get_curr_camped_freq(), rrc_ccm_get_curr_camped_plmn());
    sys_band_mask_e_type sys_band = rrc_convert_band_to_sys_band(band_class);

    mm_buf_ptr->cmd.rrc_service_ind.active_band = sys_band;
#endif

    if (rrc_ccm_cell_camping_info.is_curr_cell_id_valid)
    {
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.cell_id = cell_id;
      WRRC_MSG1_HIGH("Reporting Cell_ID :%d to NAS",cell_id);
    }

    /* Fill the cell access */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access =
    rrc_csp_int_data.curr_cell_access_status;

    if(rrc_csp_int_data.is_dsac_valid == TRUE)
    {
       MSG_MED("DSAC: DSAC info is present",0,0,0);
       mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present  = 
                                             rrc_csp_int_data.is_dsac_valid;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access = 
                                             rrc_csp_int_data.curr_cs_cell_access;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access = 
                                             rrc_csp_int_data.curr_ps_cell_access;
    }
    else
    {
      MSG_MED("DSAC: DSAC info is not present",0,0,0);
      mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present = rrc_csp_int_data.is_dsac_valid;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access = rrc_csp_int_data.curr_cell_access_status;
    }

    if(rrc_csp_int_data.is_dsac_valid == TRUE) 
    {
      MSG_MED("DSAC: DSAC info is valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_dsac_present = 
                           rrc_csp_int_data.is_dsac_valid;
      rrc_ccm_cell_camping_info.curr_cs_cell_access =   
                           mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access;
      rrc_ccm_cell_camping_info.curr_ps_cell_access =   
                            mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access;
    }
    else
    {
      MSG_MED("DSAC: DSAC info is not valid for CCM",0,0,0);	  
      rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.ppac_present = rrc_csp_int_data.is_ppac_valid;

    rrc_ccm_cell_camping_info.curr_ppac_present = rrc_csp_int_data.is_ppac_valid;

    if(rrc_csp_int_data.is_ppac_valid == TRUE)
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_cs_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_ps_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed = 
                                                rrc_csp_int_data.curr_cell_page_rsp_allowed;

      MSG_MED("PPAC: PPAC info is valid for CCM",0,0,0);

      rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed =
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed =
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed = 
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed;
    }

    MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"DSAC/PPAC: DSAC, CS and PS cell access values are %d, %d, %d cell Access Info is %d PPAC valid, CS reg, PS reg, paging status values are %d, %d, %d, %d",
                        rrc_csp_int_data.is_dsac_valid,
                        rrc_csp_int_data.curr_cs_cell_access,
                        rrc_csp_int_data.curr_ps_cell_access,
                        rrc_csp_int_data.curr_cell_access_status,
                        rrc_csp_int_data.is_ppac_valid,
                        rrc_csp_int_data.curr_cell_cs_reg_allowed,
                        rrc_csp_int_data.curr_cell_ps_reg_allowed,
                        rrc_csp_int_data.curr_cell_page_rsp_allowed);
    /* Fill the PLMN information */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

    mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = FALSE;

    /*lint -e737*/
    /*lint -e650*/
    if(/*Check to see if Primary PLMN I.D is present */
       ((primary_plmn_Id_present  == TRUE)&&
        (primary_plmn_Identity != NULL)
       )
       ||/*If not then check to see if common PLMN I.D is present */
       (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
       rrc_CN_InformationInfo,plmn_Identity))
      )
    {
      /*lint +e737*/
      /*lint +e650*/
      /*If primary PLMN I.D is present then this should be considered 
       *  over common PLMN I.D.Since common PLMN I.D is optional field
       *  so it may not be included. In theses cases if primary PLMN I.D present
       *  honor it & send to MM
       */
      if(primary_plmn_Id_present  == TRUE)
      {
        WRRC_MSG0_HIGH("NWS:Primary PLMN I.D present in OTA message");
        rrc_translate_gsm_map_plmn_id 
         (primary_plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );
      }
      else
      {
        rrc_translate_gsm_map_plmn_id 
         (&cn_information_info_ptr->plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );
      }
      /* Update the current requested PLMN with the new PLMN */
      rrc_csp_int_data.curr_req_plmn.plmn_id  = 
        mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id;

      rrc_ccm_cell_camping_info.curr_cell.plmn_id =  mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id;

      rrc_csp_int_data.primary_plmn_Id_present = primary_plmn_Id_present;

      PRINT_PLMN_ID(rrc_csp_int_data.curr_req_plmn.plmn_id)
  
    }
    else
    {
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0]= 0xFF;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1]= 0xFF;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]= 0xFF;
      MSG_HIGH("DSAC: Set mcc[0]=0x%x, mcc[1]=0x%x, mcc[2]=0x%x in RRC_SERVICE_IND_FROM_OTA_MSG ",
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0],
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1],
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]);
    }

    /* Fill in the CN common GSM-MAP NAS system information now */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
             rrc_CN_InformationInfo,cn_CommonGSM_MAP_NAS_SysInfo))
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.length =
      RRC_GET_OCTET_STRING_LEN(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      data_ptr = RRC_GET_OCTET_STRING_PTR(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      for( i = 0; i < RRC_GET_OCTET_STRING_LEN(cn_information_info_ptr->cn_CommonGSM_MAP_NAS_SysInfo); i ++)
      {
        mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.value[i] =
         *data_ptr;

        data_ptr ++;
      }
      /*Store the LAC from the OTA in global variable lac_in_ota*/		  
      if(rrc_get_lac_id_in_dch(&(mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info)) == FAILURE)
      {
        WRRC_MSG0_ERROR("Not able to get LAC from CN Info.");
      }
    }

    /* Fill in the CN domain specific NAS system info now */

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_ptr,
          rrc_CN_InformationInfo,cn_DomainInformationList))
    {
      uint32 idx =0;

      do
      {
        if(rrc_ccm_fill_nas_cn_specific_info(cn_information_info_ptr->cn_DomainInformationList.elem[idx],
            &mm_buf_ptr->cmd.rrc_service_ind.service_info.
             cn_domain_info[num_cn_domains]) == FAILURE)
        {
          mm_free_cmd_buf(mm_buf_ptr);
          return(FAILURE);
        }
        num_cn_domains ++;
        idx++;

      }while((cn_information_info_ptr->cn_DomainInformationList.n > idx) &&
            (num_cn_domains < RRC_MAX_CN_DOMAINS));
      /* Restore the head pointer of the linked list */

      rrc_get_rac_id_in_dch(&(cn_information_info_ptr->cn_DomainInformationList));

    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.num_cn_domains =
      num_cn_domains;

  } /* Service is available */

  /* Put the command on MM queue */
  WRRC_MSG1_HIGH("Sending MM cmd 0x%x / Dispatching out MM cmd RRC_SERVICE_IND_FROM_OTA_MSG", RRC_SERVICE_IND_FROM_OTA_MSG);
  rrc_send_rrc_message_to_mm(mm_buf_ptr);

  return(SUCCESS);
} /* rrc_ccm_send_nas_cn_info */
/*===========================================================================

FUNCTION          RRC_CCM_SEND_NAS_CN_INFO_FULL

DESCRIPTION       This function is responsible for sending the NAS specific
                  CN_FULL information to MM.

DEPENDENCIES
                  

RETURN VALUE
                  SUCCESS: Able to send RRC_SERVICE_IND_FROM_OTA_MSG successfully.
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
)
{
  /* Send the CN info to MM */

  mm_cmd_type *mm_buf_ptr;
  uint32 num_cn_domains = 0;
  unsigned char  *data_ptr;
  uint32          i;
  rrc_int_cell_id_type cell_id = 0;
  rrc_plmn_identity_type plmn_id;

  

  if(cn_information_info_full_ptr == NULL)
  {
    WRRC_MSG0_ERROR("NULL Pointer for CN Information Full");
    return(FAILURE);
  }

  /* Get a buffer from MM task */
  mm_buf_ptr = mm_rrc_get_cmd_buf(RRC_SERVICE_IND_FROM_OTA_MSG);

#ifdef FEATURE_FEMTO_CSG
  mm_buf_ptr->cmd.rrc_service_ind.service_info.csg_info.csg_id = SYS_CSG_ID_INVALID;
#endif

#ifdef FEATURE_DUAL_SIM
  mm_buf_ptr->cmd.rrc_service_ind.as_id = rrc_get_as_id();
#endif

#ifdef FEATURE_QCHAT
  WRRC_MSG0_HIGH("Notify cell-plmn id change to QChat UPK");
  qchatupk_update_location(SYS_SYS_MODE_WCDMA);
#endif /* FEATURE_QCHAT */

  /* Fill in the service status */
  if ((rrc_ccm_get_serv_plmn_cellid(&cell_id, &plmn_id) == RRC_CELL_LOSS_NO_NEW_CELL))
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_NO_SERVICE;
  }
  else
  {
    mm_buf_ptr->cmd.rrc_service_ind.service_status = MM_AS_SERVICE_AVAILABLE;
  }

  mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = FALSE;

  if(mm_buf_ptr->cmd.rrc_service_ind.service_status == MM_AS_SERVICE_AVAILABLE)
  {    

#ifdef FEATURE_WCDMA_MULTIBAND
    rrc_csp_band_class_type band_class = rrc_get_frequency_band(rrc_ccm_get_curr_camped_freq(), rrc_ccm_get_curr_camped_plmn());
    sys_band_mask_e_type sys_band = rrc_convert_band_to_sys_band(band_class);

    mm_buf_ptr->cmd.rrc_service_ind.active_band = sys_band;
#endif

    if (rrc_ccm_cell_camping_info.is_curr_cell_id_valid)
    {
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.is_cell_id_valid = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.cell_id = cell_id;
      WRRC_MSG1_HIGH("Reporting Cell_ID :%d to NAS",cell_id);
    }

    /* Fill the cell access */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access =
                                                            rrc_csp_int_data.curr_cell_access_status;
    if(rrc_csp_int_data.is_dsac_valid == TRUE)
    {
       MSG_MED("DSAC: DSAC info is present",0,0,0);
       mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present  =  
                                                  rrc_csp_int_data.is_dsac_valid;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access = 
                                                  rrc_csp_int_data.curr_cs_cell_access;
       mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access = 
                                                  rrc_csp_int_data.curr_ps_cell_access;
    }
    else
    {
      MSG_MED("DSAC: DSAC info is not valid",0,0,0);
      mm_buf_ptr->cmd.rrc_service_ind.service_info.dsac_present = 
                                                rrc_csp_int_data.is_dsac_valid;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cell_access = 
                                               rrc_csp_int_data.curr_cell_access_status;
    }

    if(rrc_csp_int_data.is_dsac_valid == TRUE) 
    {
       MSG_MED("DSAC: DSAC info is valid for CCM",0,0,0);
       rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
       rrc_ccm_cell_camping_info.curr_cs_cell_access =   mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_cell_access;
       rrc_ccm_cell_camping_info.curr_ps_cell_access =   mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_cell_access;
    }
    else
    {
      MSG_MED("DSAC: DSAC info is not valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.ppac_present = rrc_csp_int_data.is_ppac_valid;

    rrc_ccm_cell_camping_info.curr_ppac_present = rrc_csp_int_data.is_ppac_valid;

    if(rrc_csp_int_data.is_ppac_valid == TRUE)
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_cs_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_ps_reg_allowed;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed = 
                                                rrc_csp_int_data.curr_cell_page_rsp_allowed;

      MSG_MED("PPAC: PPAC info is valid for CCM",0,0,0);

      rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed =
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed =
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed = 
                                     mm_buf_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed;
    }

    MSG_8(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"DSAC/PPAC: DSAC, CS and PS cell access values are %d, %d, %d cell Access Info is %d PPAC valid, CS reg, PS reg, paging status values are %d, %d, %d, %d",
                        rrc_csp_int_data.is_dsac_valid,
                        rrc_csp_int_data.curr_cs_cell_access,
                        rrc_csp_int_data.curr_ps_cell_access,
                        rrc_csp_int_data.curr_cell_access_status,
                        rrc_csp_int_data.is_ppac_valid,
                        rrc_csp_int_data.curr_cell_cs_reg_allowed,
                        rrc_csp_int_data.curr_cell_ps_reg_allowed,
                        rrc_csp_int_data.curr_cell_page_rsp_allowed);
    /* Fill the PLMN information */
    mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_type = RRC_GSM_MAP_PLMN_SEL;

    mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = FALSE;

    /*lint -e737*/
    if(/*Check to see if Primary PLMN I.D is present */
       ((primary_plmn_Id_present  == TRUE)&&
        (primary_plmn_Identity != NULL)
       )
       ||/*If not then check to see if common PLMN I.D is present */
       (RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_full_ptr,
       rrc_CN_InformationInfoFull,plmn_Identity))
     )
    {
      /*lint +e737*/
      /*If primary PLMN I.D is present then this should be considered 
       *  over common PLMN I.D.Since common PLMN I.D is optional field
       *  so it may not be included. In theses cases if primary PLMN I.D present
       *  honor it & send to MM
       */
      if(primary_plmn_Id_present  == TRUE)
      {
        WRRC_MSG0_HIGH("NWS:Primary PLMN I.D present in OTA message");
        rrc_translate_gsm_map_plmn_id 
         (primary_plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );
      }
      else
      {
        rrc_translate_gsm_map_plmn_id 
         (&cn_information_info_full_ptr->plmn_Identity,
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id
         );  
      }
      /* Update the current requested PLMN with the new PLMN */
      rrc_csp_int_data.curr_req_plmn.plmn_id  = 
        mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id;

      rrc_ccm_cell_camping_info.curr_cell.plmn_id =  mm_buf_ptr->cmd.rrc_service_ind.service_info.selected_plmn.plmn_id;

      rrc_csp_int_data.curr_acq_entry.plmn_id = rrc_csp_int_data.curr_req_plmn.plmn_id;

#ifdef FEATURE_FEMTO_CSG
      rrc_csp_int_data.curr_acq_entry.csg_id = SYS_CSG_ID_INVALID;
      rrc_csp_int_data.curr_req_plmn.csg_id = SYS_CSG_ID_INVALID;
#endif

      /* Update the ACQ DB entry with the PLMN info received in UMI message*/
      (void)rrc_csp_add_acq_entry(&rrc_csp_int_data.curr_acq_entry);

      rrc_csp_int_data.primary_plmn_Id_present = primary_plmn_Id_present;

      PRINT_PLMN_ID(rrc_csp_int_data.curr_req_plmn.plmn_id)
   
    }
    else
    {
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0]= 0xFF;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1]= 0xFF;
      mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]= 0xFF;
      MSG_HIGH("DSAC: Set mcc[0]=0x%x, mcc[1]=0x%x, mcc[2]=0x%x in RRC_SERVICE_IND_FROM_OTA_MSG ",
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[0],
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[1],
                                               mm_buf_ptr->cmd.rrc_service_ind_from_ota_msg.service_info.selected_plmn.plmn_id.mcc[2]);
    }

    /* Fill in the CN common GSM-MAP NAS system information now */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(cn_information_info_full_ptr,
       rrc_CN_InformationInfoFull,cn_CommonGSM_MAP_NAS_SysInfo))
    {
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info_incl = TRUE;
      mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.length =
       RRC_GET_OCTET_STRING_LEN(cn_information_info_full_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      data_ptr = RRC_GET_OCTET_STRING_PTR(cn_information_info_full_ptr->cn_CommonGSM_MAP_NAS_SysInfo);

      for( i = 0; i < RRC_GET_OCTET_STRING_LEN(cn_information_info_full_ptr->cn_CommonGSM_MAP_NAS_SysInfo); i ++)
      {
        mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info.value[i] =
         *data_ptr;

        data_ptr ++;
      }
      /*Store the LAC from the OTA in global variable lac_in_ota*/
      if(rrc_get_lac_id_in_dch(&(mm_buf_ptr->cmd.rrc_service_ind.service_info.gsm_map_common_info)) == FAILURE)
      {
        WRRC_MSG0_ERROR("Not able to get LAC from CN Info.");
      }
      else  //Success
      {
        /*Invalidate cell_id so that CB functions in rrc_notify_cell_id_in_dch() will be updated with cell_info 
          as we have already updated the same with incomplete info during HO to W*/
        prev_cell_id = WCDMA_INVALID_CELL_ID; 
      }
    }


    /* Fill in the CN domain specific NAS system info now */
    if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(cn_information_info_full_ptr,
		cn_DomainInformationListFull))
    {
      uint32 idx =0;
      do
      {
        if(rrc_ccm_fill_nas_cn_specific_info_full(
          cn_information_info_full_ptr->cn_DomainInformationListFull.elem[idx],
          &mm_buf_ptr->cmd.rrc_service_ind.service_info.
          cn_domain_info[num_cn_domains]) == FAILURE)
        {
          mm_free_cmd_buf(mm_buf_ptr);
          return(FAILURE);
        }
        num_cn_domains ++;
        idx++;

      }while((cn_information_info_full_ptr->cn_DomainInformationListFull.n > idx) &&
            (num_cn_domains < RRC_MAX_CN_DOMAINS));
    }

    mm_buf_ptr->cmd.rrc_service_ind.service_info.num_cn_domains =
      num_cn_domains;

  } /* Service is available */

  /* Put the command on MM queue */
  WRRC_MSG1_HIGH("Sending MM cmd 0x%x / Dispatching out MM cmd RRC_SERVICE_IND_FROM_OTA_MSG", RRC_SERVICE_IND_FROM_OTA_MSG);
  rrc_send_rrc_message_to_mm(mm_buf_ptr);

  return(SUCCESS);
} /* rrc_ccm_send_nas_cn_info_full */

/*===========================================================================

FUNCTION          RRC_CCM_SEND_NEW_CELL_IND

DESCRIPTION       This function is responsible for sending the New Cell Ind.
                  to Internal RRC queue.

DEPENDENCIES
                  None

RETURN VALUE
                  A status that indicates if the New Cell Ind. could be
                  sent Successfully or not.

SIDE EFFECTS      None.

===========================================================================*/
void  
rrc_ccm_send_new_cell_ind(rrc_new_cell_status_e_type  new_cell_camp_status,
                          rrc_proc_e_type  proc_id,
                          boolean cell_same_as_before,
                          boolean inform_procedure_rce)
{
  rrc_cmd_type   *csp_out_cmd_ptr;
  /* Send New Cell Ind */
  if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {  
    csp_out_cmd_ptr->cmd_hdr.cmd_id = RRC_NEW_CELL_IND;
    csp_out_cmd_ptr->cmd.new_cell_ind.new_cell_status = new_cell_camp_status;
    csp_out_cmd_ptr->cmd.new_cell_ind.no_ind_proc_id = proc_id;
    csp_out_cmd_ptr->cmd.new_cell_ind.cell_same_as_before = cell_same_as_before;
    csp_out_cmd_ptr->cmd.new_cell_ind.inform_procedure_rce = inform_procedure_rce;

    /* Send the Internal cmd to RRC */
    MSG_HIGH("Post NEW_CELL_IND %d, %d, %d", 
             new_cell_camp_status, 
             cell_same_as_before, 
             inform_procedure_rce);
    rrc_put_int_cmd( csp_out_cmd_ptr );
  }
} /* rrc_ccm_send_new_cell_ind */


/*===========================================================================

FUNCTION          RRC_CCM_SEND_CELL_CHANGE_CNF

DESCRIPTION       This function is responsible for sending the Cell Change
                  Confirm to the passed RRC Procedure.

DEPENDENCIES
                  None

RETURN VALUE
                  A status that indicates if the Cell Change Cnf could be
                  sent Successfully or not.
SIDE EFFECTS
===========================================================================*/
static uecomdef_status_e_type 
rrc_ccm_send_cell_change_cnf(rrc_cell_change_status_e_type  cell_change_status, 
                             rrc_new_cell_status_e_type  new_cell_camp_status,
                             rrc_proc_e_type   proc_id,
                             rrc_csp_acq_entry_type  *acq_entry_ptr)
{

  rrc_cmd_type   *csp_out_cmd_ptr;

  /* Check validity of Acq Entry pointer */
  if(acq_entry_ptr == NULL)
  {
    WRRC_MSG0_ERROR("Passed Acq Entry pointer is NULL");
    return(FAILURE);
  }

  /* Proceed further only if procedure ID is valid */
  if(proc_id < MAX_NUM_OF_PROCEDURES)
  {
    /* Send Cell Change Confirm back to requesting procedure */
    if((csp_out_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
    {    
      csp_out_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_CHANGE_CNF;
      csp_out_cmd_ptr->cmd.cell_change_cnf.req_proc_id = proc_id;
      csp_out_cmd_ptr->cmd.cell_change_cnf.cell_change_status = cell_change_status;
      csp_out_cmd_ptr->cmd.cell_change_cnf.new_cell_status = new_cell_camp_status;
      csp_out_cmd_ptr->cmd.cell_change_cnf.freq = acq_entry_ptr->freq;
      csp_out_cmd_ptr->cmd.cell_change_cnf.scr_code = acq_entry_ptr->scr_code;
      csp_out_cmd_ptr->cmd.cell_change_cnf.plmn_id = acq_entry_ptr->plmn_id;
      csp_out_cmd_ptr->cmd.cell_change_cnf.cell_id = acq_entry_ptr->cell_id;
      /* Send the Internal cmd to RRC */
      rrc_put_int_cmd( csp_out_cmd_ptr );
      return(SUCCESS);
    }
    else
    {
      return(FAILURE);
    }
  } /* Procedure ID is valid */

  else  /* Procedure ID is invalid */
  {
    WRRC_MSG0_HIGH("Proc ID for req procedure is invalid");
    return(FAILURE);
  }

} /* rrc_ccm_send_cell_change_cnf */

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
boolean rrc_ccm_cell_change_to_same_cell(void)
{
  rrc_plmn_identity_type prev_plmn_id, current_plmn_id;
  if(rrc_ccm_cell_camping_info.prev_cell.is_valid_nw_sharing)
  {
    prev_plmn_id = rrc_ccm_cell_camping_info.prev_cell.shared_list_of_plmns.common_plmn_id;
  }
  else
  {
    prev_plmn_id = rrc_ccm_cell_camping_info.prev_cell.plmn_id;
  }

  if(rrc_ccm_cell_camping_info.curr_cell.is_valid_nw_sharing)
  {
    current_plmn_id = rrc_ccm_cell_camping_info.curr_cell.shared_list_of_plmns.common_plmn_id;
  }
  else
  {
    current_plmn_id = rrc_ccm_cell_camping_info.curr_cell.plmn_id;
  }
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Cell info: curr cell id: %d curr freq: %d cur_psc:%d prev_psc:%d",
             rrc_ccm_cell_camping_info.curr_cell.cell_id,
             rrc_ccm_cell_camping_info.curr_cell.freq,
             rrc_ccm_cell_camping_info.curr_cell.scr_code,
             rrc_ccm_cell_camping_info.prev_cell.scr_code
             );
  if( (rrc_bcch_plmns_match(current_plmn_id,prev_plmn_id) == FALSE) ||
      (rrc_ccm_cell_camping_info.curr_cell.cell_id != rrc_ccm_cell_camping_info.prev_cell.cell_id) )
  {
    return(FALSE);
  }
  else
  {
    return(TRUE);
  }
} /* rrc_ccm_cell_change_to_same_cell */


/*===========================================================================

FUNCTION          RRC_CCM_HANDLE_CELL_CHANGE_SUCCESS

DESCRIPTION       This function is responsible for all actions needed when
                  a Cell Change Request succeeds.
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

===========================================================================*/
static void rrc_ccm_handle_cell_change_success(void)
{
  boolean  cell_same_as_before = TRUE;
  boolean  inform_procedure_rce = FALSE;

  rrc_proc_e_type                 proc_id;

  rrcrce_substate_e_type  rrcrce_substate = rrcrce_get_rce_sub_state();

  rrc_SysInfoType1 *sib1_ptr = NULL;
  
  rrc_plmn_identity_type prev_plmn_id, current_plmn_id;
#ifdef FEATURE_WCDMA_HS_FACH
  rrclog_efach_epch_erachtype rrc_efach_epch_event;
#endif

  rrc_ccm_cell_camping_info.is_curr_cell_id_valid = TRUE;

  /* Before sending new cell related cmds to other modules, change 
   * camping status to RRC_NEW_CELL_SELECTED.
   */
  rrc_ccm_cell_camping_info.cell_camp_status = RRC_NEW_CELL_SELECTED;
  rrc_ccm_cell_camping_info.status = STATUS_VALID;

  /* Now send a Service Ind to NAS here. If the cell has been changed
   * due to a request from Cell Selection Procedure, it is left 
   * to the cell selection to decide if a SERVICE_CNF or a 
   * SERVICE_IND needs to be sent.
   */
  if(rrc_ccm_cell_camping_info.req_proc_id != RRC_PROCEDURE_CSP)
  {
    if(rrc_csp_send_mm_service_ind() == FAILURE)
    {
      WRRC_MSG0_ERROR("Service ind not sent");
    }
  }

  /* Send a Cell Change Cnf with Success to the calling procedure */
  if(rrc_ccm_send_cell_change_cnf(RRC_CELL_CHANGE_SUCCESS,
                                  rrc_ccm_cell_camping_info.cell_camp_status,
                                  rrc_ccm_cell_camping_info.req_proc_id,
                                  &rrc_ccm_cell_camping_info.curr_cell)
     == FAILURE)
  {
    ERR_FATAL("Failure to send Cell Change Cnf", 0, 0, 0);
  }

  if(rrc_ccm_cell_camping_info.prev_cell.is_valid_nw_sharing)
  {
    prev_plmn_id = rrc_ccm_cell_camping_info.prev_cell.shared_list_of_plmns.common_plmn_id;
  }
  else
  {
    prev_plmn_id = rrc_ccm_cell_camping_info.prev_cell.plmn_id;
  }

  if(rrc_ccm_cell_camping_info.curr_cell.is_valid_nw_sharing)
  {
    current_plmn_id = rrc_ccm_cell_camping_info.curr_cell.shared_list_of_plmns.common_plmn_id;
  }
  else
  {
    current_plmn_id = rrc_ccm_cell_camping_info.curr_cell.plmn_id;
  }

  if( (rrc_bcch_plmns_match(current_plmn_id, 
                            prev_plmn_id) == FALSE) ||
      (rrc_ccm_cell_camping_info.curr_cell.cell_id != rrc_ccm_cell_camping_info.prev_cell.cell_id) )
  {
    cell_same_as_before = FALSE;
  }
  else
  {
    if(rrc_csp_int_data.curr_scan == RRC_CSP_CELL_RESELECTION_SCAN)
    {
      ERR_FATAL("Same cell/PLMN id during cell reselection",0,0,0);
    }
  }

  if(rrc_ccm_cell_camping_info.rce_found_status_not_camping)
  {
    inform_procedure_rce = TRUE;
    rrc_ccm_cell_camping_info.rce_found_status_not_camping = FALSE;
  }

  if(inform_procedure_rce || 
     ( (rrcrce_substate == RRCRCE_WAIT_FOR_SIB7_CURRENT) ||
       (rrcrce_substate == RRCRCE_WAIT_FOR_RRC_CONN_SETUP) ||
       (rrcrce_substate == RRCRCE_WAIT_FOR_RESELCT_COMPL) ||
       (rrcrce_substate == RRCRCE_WAIT_FOR_REDIRECT_CNF) )
    )
  {
    if(rrcrce_get_rrc_est_cause() != RRC_EST_EMERGENCY_CALL)
    {
      sib1_ptr = (rrc_SysInfoType1 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB1);
      // Compare the currently camped RAI vs the one given in EST_REQ
      if(rrcrce_check_for_rai_mismatch(rrc_ccm_cell_camping_info.curr_cell.plmn_id,sib1_ptr))
      {
        WRRC_MSG0_HIGH("LA/RA mismatch, Initialize RCE");
        inform_procedure_rce = FALSE;
        rrcrce_force_initialize(TRUE);
      }
    }
  }

  /* Send an Internal New Cell Ind only if Cell Change is not due to
   * transition from CELL_DCH to CELL_FACH / CELL_FACH to PCH .
   * For CELL_FACH to CELL_FACH, send New Cell Ind to CU if OOS 
   * is triggered.
   */
  if((rrc_ccm_cell_camping_info.cell_change_type != RRC_CHANGE_CELL_DCH_TO_CELL_FACH) &&
     (rrc_ccm_cell_camping_info.cell_change_type != RRC_CHANGE_CELL_DCH_TO_CELL_PCH) &&
     (rrc_ccm_cell_camping_info.cell_change_type != RRC_CHANGE_CELL_DCH_TO_URA_PCH)
     && (rrc_ccm_cell_camping_info.cell_change_type != RRC_CHANGE_CELL_FACH_TO_CELL_PCH)
     && (rrc_ccm_cell_camping_info.cell_change_type != RRC_CHANGE_CELL_FACH_TO_URA_PCH)
     && (!((rrc_ccm_cell_camping_info.cell_change_type == RRC_CHANGE_CELL_FACH_TO_CELL_FACH) &&
           (rrcrb_check_if_proc_waiting_for_new_cell_ind(&proc_id) == TRUE )))
    )
  {
    if((cell_same_as_before == FALSE) || 
       (inform_procedure_rce == TRUE) ||
       (rrc_ccm_cell_camping_info.cell_change_type == RRC_CHANGE_RETURN_TO_SERVICE_AREA) 
       || ((rrc_ccm_cell_camping_info.cell_change_type == RRC_CHANGE_CELL_FACH_TO_CELL_FACH)
           && ((rrcllc_get_toc_usage() == TOC_FOR_OOS_WITH_DCH_INFO )||
               (rrcllc_get_toc_usage() == TOC_FOR_OOS_WITHOUT_DCH_INFO )))
       )
    {
      /* Send an Indication ONLY when either the new cell is different
       * than the last camped on cell OR if RCE wants an Indication.
       */
      /* We don't want the procedure initiating this change to get an IND 
       * It will get a CNF and an extra IND might be confusing.
       */
      rrc_ccm_send_new_cell_ind(RRC_NEW_CELL_SELECTED, 
                                rrc_ccm_cell_camping_info.req_proc_id,
                                cell_same_as_before,
                                inform_procedure_rce);
    }
    WRRC_MSG0_HIGH("calling BMC func");
    rrcbmc_send_rrc_new_cell_ind_message();
  } /* If Cell Change is not due to CELL_DCH to CELL_FACH transition */

  /* For CELL_FACH to CELL_FACH send New Cell Ind to the procedure which initiated
   * the cell selection due to redirection, as there will not be any State Change Ind
   * to take care of.
   */
  if((rrc_ccm_cell_camping_info.cell_change_type == RRC_CHANGE_CELL_FACH_TO_CELL_FACH)
     && (rrcrb_check_if_proc_waiting_for_new_cell_ind(&proc_id) == TRUE ))
  {
    rrc_ccm_propagate_new_cell_to_waiting_proc(proc_id, cell_same_as_before, inform_procedure_rce);
  }

#ifdef FEATURE_RAT_PRIORITY_LIST
  if((rrc_ccm_cell_camping_info.cell_change_type == RRC_CHANGE_CELL_SELECTION)
#ifdef FEATURE_WRLF_SYSTEM_SEL
      || (RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
#endif
    )
  {
    rrc_csp_update_mode_band_pref_after_succ_camping();
  }
#endif
#ifdef FEATURE_WCDMA_HS_FACH
  rrc_efach_epch_event.rrc_efach_hsrach_event = 0;
  rrc_efach_epch_event.rrc_efach_hsrach_event |= rrcenhstate_camped_cell_supports_e_fach();
#ifdef FEATURE_WCDMA_HS_FACH_DRX
  /* Set the 3rd bit from LSB if cell supports efach drx */
  rrc_efach_epch_event.rrc_efach_hsrach_event |= ((rrcenhstate_camped_cell_supports_e_fach_drx())<<2);
#endif
#ifdef FEATURE_WCDMA_HS_RACH
  /* Set the 4th bit from LSB if cell supports hsrach */
  rrc_efach_epch_event.rrc_efach_hsrach_event |= ((rrchsrach_camped_cell_supports_hsrach())<<3);
#endif

  //MSG_HIGH("efach =%d epch =%d efach_drx = %d",rrc_efach_epch_event.efach,rrc_efach_epch_event.epch,rrc_efach_epch_event.efach_drx);

  event_report_payload(EVENT_WCDMA_CELL_EFACH_EPCH_ERACH_SUPPORT,
                     sizeof(rrc_efach_epch_event),
                     (void *)&rrc_efach_epch_event);
#endif
} /* rrc_ccm_handle_cell_change_success */


/*===========================================================================

FUNCTION          RRC_CCM_HANDLE_CELL_CHANGE_FAILURE

DESCRIPTION       This function is responsible for all actions needed when
                  a Cell Change Request fails due to any of the multiple 
                  reasons.
DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

===========================================================================*/
static void rrc_ccm_handle_cell_change_failure(rrc_proc_e_type  proc_id, 
                                        rrc_csp_acq_entry_type  *acq_entry_ptr,
                                        rrc_cell_change_status_e_type cell_change_status)
{
  /* Set the camping Status to RRC_CELL_LOSS_NO_NEW_CELL */
  rrc_ccm_cell_camping_info.cell_camp_status = RRC_CELL_LOSS_NO_NEW_CELL;
  rrc_ccm_cell_camping_info.status = STATUS_VALID;
  
  /* Send a Cell Change CNF with Failure to the passed Procedure ID */
  if(rrc_ccm_send_cell_change_cnf(cell_change_status,
                                  rrc_ccm_cell_camping_info.cell_camp_status,
                                  proc_id, acq_entry_ptr)
     == FAILURE)
  {
    ERR_FATAL("Failure to send Cell Change Cnf", 0, 0, 0);
  }

  /* Let us send a New Cell Ind so that those procedures that want to be
   * informed of a loss of service will get to know.
   */
  rrc_ccm_send_new_cell_ind(RRC_CELL_LOSS_NO_NEW_CELL, proc_id, FALSE, FALSE);

} /* rrc_ccm_handle_cell_change_failure */


/*===========================================================================

FUNCTION        RRC_CCM_UPDATE_FOR_STATE_CHANGE

DESCRIPTION
                This function processes State Change Indication.
  
DEPENDENCIES
                None

RETURN VALUE
                None.

SIDE EFFECTS
                None

===========================================================================*/
static rrc_ccm_substate_e_type
rrc_ccm_update_state_change( rrc_cmd_type *cmd_ptr )
{
  rrc_ccm_substate_e_type next_substate = RRC_CCM_IDLE;
  if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_DCH)
  {
    /* Copy previous cell to current cell since we are ready to accept
     * next cell change request.
     */
    rrc_ccm_cell_camping_info.status = STATUS_VALID;
    rrc_ccm_cell_camping_info.curr_cell = rrc_ccm_cell_camping_info.prev_cell;
    rrc_ccm_cell_camping_info.curr_cell_access = 
                   rrc_ccm_cell_camping_info.prev_cell_access;
    rrc_ccm_cell_camping_info.curr_dsac_present= 
                         rrc_ccm_cell_camping_info.prev_dsac_present;
    rrc_ccm_cell_camping_info.curr_cs_cell_access = 
                         rrc_ccm_cell_camping_info.prev_cs_cell_access;
    rrc_ccm_cell_camping_info.curr_ps_cell_access = 
                         rrc_ccm_cell_camping_info.prev_ps_cell_access;
    MSG_MED("DSAC: Back up of DSAC info during  <ccm> update state change !",0,0,0);

    rrc_ccm_cell_camping_info.curr_ppac_present = 
                         rrc_ccm_cell_camping_info.prev_ppac_present;
    rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed = 
                         rrc_ccm_cell_camping_info.prev_cell_cs_reg_allowed;
    rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed = 
                         rrc_ccm_cell_camping_info.prev_cell_ps_reg_allowed;
    rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed = 
                         rrc_ccm_cell_camping_info.prev_cell_page_rsp_allowed;
    WRRC_MSG0_HIGH("State change to DCH rxed , reset resel vars, PPAC: Back of PPAC info during <ccm> update state change !");
  }
  else if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
  {
    WRRC_MSG0_HIGH("State change to Idle, going back to CCM_IDLE");
    /* The status valid is set to FALSE till a Cell Change Req is processed.
     */
    rrc_ccm_cell_camping_info.status = STATUS_INVALID_CELL_TO_BE_CAMPED;
    rrc_ccm_cell_camping_info.cell_camp_status = RRC_CELL_LOSS_NO_NEW_CELL;
  
    /* Set to FALSE, the boolean that indicates that RCE found status as "Not Camping"*/
    rrc_ccm_cell_camping_info.rce_found_status_not_camping = FALSE;
    /* Set the Cell change requesting procedure ID to Invalid value */
    rrc_ccm_cell_camping_info.req_proc_id = MAX_NUM_OF_PROCEDURES;
  
    /* Initialize the state of RRC CCM to IDLE */
    next_substate = RRC_CCM_IDLE;
  }
  else if((cmd_ptr->cmd.state_change_ind.previous_state == RRC_STATE_CONNECTING) &&
          (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_FACH))
  {
    /*  Do not modify the CCM substate for connecting to FACH state change
     *  In a corner scenario connection setup message is received when reselections are 
     *  in progress..on receiving state change indication CCM is substate is changed to CCM_IDLE 
     *  resulting in LLC not sending RESELECTION_CNF 
     */
    next_substate = rrc_ccm_substate;
  }
  else
  {
    WRRC_MSG1_ERROR("Unexpected state change to %d",cmd_ptr->cmd.state_change_ind.new_state);
  }

  return next_substate;

} /* rrc_ccm_update_for_state_change */


/*===========================================================================

FUNCTION          RRC_CCM_WAIT_CELL_RESELECTION_HANDLER

DESCRIPTION       This function is responsible for handling all commands 
                  in RRC_CCM_WAIT_CELL_RESELECTION_CNF substate.
DEPENDENCIES
                  None

RETURN VALUE
                  Next CCM substate.

SIDE EFFECTS
                  None.
===========================================================================*/
static rrc_ccm_substate_e_type
rrc_ccm_wait_cell_reselection_handler( rrc_cmd_type  *cmd_ptr)
{
  rrc_ccm_substate_e_type next_substate = RRC_CCM_WAIT_CELL_RESELECTION_CNF;

  /* To post a command RRC_CELL_SELECTED_MEAS_REQ to measurement procedure */
  rrc_cmd_type* rrcmeas_cmd_ptr;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_STATE_CHANGE_IND:
      next_substate = rrc_ccm_update_state_change(cmd_ptr);
      break;

    case RRC_CELL_RESELECTION_CNF:
      /* Reselection confirm received */
      if(cmd_ptr->cmd.cell_resel_cnf.status == FAILURE)
      {
        /* Cell Reselection failed - send CELL_CHANGE_CNF with failure to CSP */
        WRRC_MSG0_HIGH("Quit Cell Change (Not Camped)- Cell Resel failed");
        /*  Stop SIb 7 timer*/
        rrctmr_stop_timer(RRCTMR_SIB7_EXPIRATION_TIMER);
        rrc_ccm_handle_cell_change_failure(rrc_ccm_cell_camping_info.req_proc_id,
                                           &rrc_ccm_cell_camping_info.curr_cell,
                                           RRC_CELL_CHANGE_FAILURE_OTHER);

        rrc_ccm_cell_camping_info.curr_cell =  
                        rrc_ccm_cell_camping_info.prev_cell;
                  
        rrc_ccm_cell_camping_info.curr_cell_access = 
                          rrc_ccm_cell_camping_info.prev_cell_access;
                  
                  
        rrc_ccm_cell_camping_info.curr_dsac_present = 
                      rrc_ccm_cell_camping_info.prev_dsac_present;
                  
        rrc_ccm_cell_camping_info.curr_cs_cell_access = 
                      rrc_ccm_cell_camping_info.prev_cs_cell_access;
                  
        rrc_ccm_cell_camping_info.curr_ps_cell_access = 
                      rrc_ccm_cell_camping_info.prev_ps_cell_access;   

          rrc_ccm_cell_camping_info.curr_ppac_present = 
                               rrc_ccm_cell_camping_info.prev_ppac_present;
          rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed = 
                               rrc_ccm_cell_camping_info.prev_cell_cs_reg_allowed;
          rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed = 
                               rrc_ccm_cell_camping_info.prev_cell_ps_reg_allowed;
          rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed = 
                               rrc_ccm_cell_camping_info.prev_cell_page_rsp_allowed;

      } /* Cell reselection failure */
      else
      {
        MSG_MED("Cell change in RRC state %d", rrc_get_state(), 0, 0);

        /* Send a Cell parameter Req to L1 */
        rrc_csp_send_l1_cmd(RRC_PROCEDURE_CCM, CPHY_CELL_PARM_REQ, &rrc_ccm_cell_camping_info.curr_cell);
      
        /* Send a Cell measurement Req to L1 */
        if((rrc_get_state() == RRC_STATE_CELL_FACH)
           &&(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_CELL_UPDATE_CONFIRM)
           &&(rrc_CellUpdateCause_uplinkDataTransmission == rrccu_get_cu_cause())
          )
        {
           /* Fixing this for the case where cu is waiting for cu confirm and cause is UL data transmission and reselection happens
              We need to configure MAC with measurement before cu gets triggered with cause UL data transmisson*/
           rrcmeas_handle_cell_sel_meas_req_in_fach();
        }
        else if((rrcmeas_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
        {
          rrcmeas_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_SELECTED_MEAS_REQ;
          rrcmeas_cmd_ptr->cmd.cell_selected_meas_req.gtow_ho_active = FALSE;
          rrc_put_int_cmd(rrcmeas_cmd_ptr);
        }

        rrc_ccm_handle_cell_change_success();

#if (!(defined(T_WINNT)))
        /* Log the state change with QXDM */
        rrc_ccm_log_cell_info();
#endif /* (!(defined(T_WINNT))) */

      } /* Successful cell reselection */

      next_substate = RRC_CCM_IDLE;

    break;

    default:
      WRRC_MSG2_HIGH("Unexpected Cmd 0x%x in RRC CCM substate %d", 
                              cmd_ptr->cmd_hdr.cmd_id, rrc_ccm_substate);
      break;

  }

  return(next_substate);

} /* rrc_ccm_wait_cell_reselection_handler */


/*===========================================================================

FUNCTION          RRC_CCM_WAIT_CHAN_CONFIG_HANDLER

DESCRIPTION       This function is responsible for handling all commands 
                  in RRC_CCM_WAIT_CHAN_CONFIG_CNF substate.
DEPENDENCIES
                  None

RETURN VALUE
                  Next CCM substate.

SIDE EFFECTS
                  None.
===========================================================================*/
static rrc_ccm_substate_e_type
rrc_ccm_wait_chan_config_handler( rrc_cmd_type  *cmd_ptr )
{
  rrc_cmd_type* rrcmeas_cmd_ptr;  
  rrc_ccm_substate_e_type next_substate = RRC_CCM_WAIT_CHAN_CONFIG_CNF;
  rrc_cell_change_status_e_type cell_change_status;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_STATE_CHANGE_IND:
      next_substate = rrc_ccm_update_state_change(cmd_ptr);
      break;

    case RRC_CHANNEL_CONFIG_CNF:
      /* First check the validity of Confirmation */
      if(cmd_ptr->cmd.chan_config_cnf.procedure != RRC_PROCEDURE_CCM)
      {
        WRRC_MSG1_ERROR("Unexpected Channel Config CNF with Proc ID rrc_proc_e_type_value%d",
             cmd_ptr->cmd.chan_config_cnf.procedure);
        return (next_substate);
      }
    
      if(cmd_ptr->cmd.chan_config_cnf.chan_cfg_status != RRCLLC_CHAN_CFG_SUCCESS)
      {
        /* Channel Config failed - go to Idle substate and send Cell Loss to other procs */
        WRRC_MSG0_HIGH("Quit Cell Change (Not Camped)- Channel Config failed");
        switch(cmd_ptr->cmd.chan_config_cnf.chan_cfg_status)
        {
          case RRCLLC_CHAN_CFG_FAILURE_PHY_CHL:
          case RRCLLC_CHAN_CFG_FAILURE_LOG_CHL: 
            cell_change_status = RRC_CELL_CHANGE_FAILURE_LOWER_LAYERS;
            break;
        
          case RRCLLC_CHAN_CFG_FAILURE_INCOMPATIBLE_STATE:
            cell_change_status = RRC_CELL_CHANGE_FAILURE_INCOMPATIBLE_STATE;
            break;
        
          default:
            cell_change_status = RRC_CELL_CHANGE_FAILURE_OTHER;
            break;
        }
    
        rrc_ccm_handle_cell_change_failure(rrc_ccm_cell_camping_info.req_proc_id,
                                           &rrc_ccm_cell_camping_info.curr_cell,
                                           cell_change_status);
        next_substate = RRC_CCM_IDLE;
      }
      else /* Channel Config succedded */
      {
        /* Send a Cell parameter Req to L1 */
        rrc_csp_send_l1_cmd(RRC_PROCEDURE_CCM, CPHY_CELL_PARM_REQ, &rrc_ccm_cell_camping_info.curr_cell);
    
        if((rrcmeas_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
        {
          rrcmeas_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_SELECTED_MEAS_REQ;
          rrcmeas_cmd_ptr->cmd.cell_selected_meas_req.gtow_ho_active = FALSE;
          rrc_put_int_cmd(rrcmeas_cmd_ptr);
        }

        rrc_ccm_handle_cell_change_success();
    
        /* Set the CCM substate to Idle so as to accept the next Cell Change Req.*/
        next_substate = RRC_CCM_IDLE;
    
    #if (!(defined(T_WINNT)))
        /* Log the state change with QXDM */
        rrc_ccm_log_cell_info();
    #endif /* (!(defined(T_WINNT))) */
    
      } /* Channel Config succedded */

      break;

    default:
      WRRC_MSG2_HIGH("Unexpected Cmd 0x%x in RRC CCM substate %d", 
                              cmd_ptr->cmd_hdr.cmd_id, rrc_ccm_substate);
      break;

  }

  return(next_substate);

} /* rrc_ccm_wait_chan_config_handler */


/*===========================================================================

FUNCTION          RRC_CCM_IDLE_HANDLER

DESCRIPTION       This function is responsible for handling all commands 
                  in RRC_CCM_IDLE substate.
DEPENDENCIES
                  None

RETURN VALUE
                  Next CCM substate.

SIDE EFFECTS
                  None.

===========================================================================*/
static rrc_ccm_substate_e_type rrc_ccm_idle_handler( rrc_cmd_type  *cmd_ptr )
{
  rrc_ccm_substate_e_type  next_substate = RRC_CCM_IDLE;
  rrc_csp_sib_mask_type    sib_mask = 0;
  rrc_cmd_type             *cell_reselection_cmd_ptr;
  rrc_state_e_type         rrc_state;
  rrc_csp_substate_e_type  csp_sstate;

  rrc_plmn_identity_type prev_plmnid, curr_plmnid;

  /* Cell selection status to be provided to SIB procedure */
  boolean cell_selection_status = TRUE;
  /* Set the following to TRUE if we can go back to the existing serving
   * cell. This is only used if cell_selection_status (above)
   * is set to FALSE. Set to FALSE if the existing cell is
   * no longer valid and the new cell selection failed. */
  boolean existing_serving_cell_valid = TRUE;
  
#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
  rrclog_wcdma_csfb_call_opt_event_type  event_payload;
#endif

  MSG_LOW("Cmd 0x%x received in CCM substate %d", 
                          cmd_ptr->cmd_hdr.cmd_id, rrc_ccm_substate, 0);

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_CELL_CHANGE_REQ:
      if(cmd_ptr->cmd.cell_change_req.req_proc_id >= MAX_NUM_OF_PROCEDURES)
      {
        /* We don't entertain procedures that mask their identity */
        WRRC_MSG1_ERROR("Cell Change Req recvd from Invalid Proc rrc_proc_e_type_value%d",
                            cmd_ptr->cmd.cell_change_req.req_proc_id);
      }
      else /* Request came from a valid Procedure ID */
      {
        rrc_state = rrc_get_state();
        
        if(
            (((rrcllc_get_ordered_config_status() == OC_SET_FOR_CELL_DCH) || 
           ((rrcllc_get_ordered_config_status() != OC_SET_FOR_DCH_FACH_TRANS) && 
            (rrcllc_get_ordered_config_status() != OC_SET_FOR_DCH_CELL_PCH_TRANS) && 
            (rrcllc_get_ordered_config_status() != OC_SET_FOR_DCH_URA_PCH_TRANS)
            && ((rrcllc_get_toc_usage() != TOC_FOR_OOS))
            ))&& (rrc_state == RRC_STATE_CELL_DCH)) 
            ||
            /*
              If OC is set for DCH and reselection is triggered, then drop the cell change request
              >If going to DCH fails , OOS search will anyhow be initiated
              >If going to DCH succeeds, L1 will clear the reselection
            */
            ((rrcllc_get_ordered_config_status() == OC_SET_FOR_CELL_DCH) &&
              (cmd_ptr->cmd.cell_change_req.new_cell_camp_status == RRC_NEW_CELL_SELECTED) &&
              (cmd_ptr->cmd.cell_change_req.cell_change_type == RRC_CHANGE_CELL_RESELECTION)
            )
          )
        {
          WRRC_MSG0_HIGH("Incompatible state, drop cell change");
          /* Send a Cell Change CNF with Failure (incompatible state) to CSP */
          if(rrc_ccm_send_cell_change_cnf(RRC_CELL_CHANGE_FAILURE_INCOMPATIBLE_STATE,
                                          rrc_ccm_cell_camping_info.cell_camp_status,
                                          cmd_ptr->cmd.cell_change_req.req_proc_id,
                                          &rrc_ccm_cell_camping_info.curr_cell)
             == FAILURE)
          {
            ERR_FATAL("Failure to send Cell Change Cnf", 0, 0, 0);
          }
        }
        else
        {
          /* First copy current cell to previous cell since we are ready to accept
           * next cell change request.
           */
          rrc_ccm_cell_camping_info.prev_cell = rrc_ccm_cell_camping_info.curr_cell;
          rrc_ccm_cell_camping_info.prev_cell_access =  rrc_ccm_cell_camping_info.curr_cell_access;
          MSG_MED("DSAC: Back up of DSAC info during  <ccm_idle_handler> !",0,0,0);
          rrc_ccm_cell_camping_info.prev_dsac_present =  
                                  rrc_ccm_cell_camping_info.curr_dsac_present;

          rrc_ccm_cell_camping_info.prev_cs_cell_access = 
                                  rrc_ccm_cell_camping_info.curr_cs_cell_access;

          rrc_ccm_cell_camping_info.prev_ps_cell_access = 
                                  rrc_ccm_cell_camping_info.curr_ps_cell_access;

          MSG_MED("PPAC: Back up of PPAC info during <ccm_idle_handler> !",0,0,0);
          rrc_ccm_cell_camping_info.prev_ppac_present = 
                                  rrc_ccm_cell_camping_info.curr_ppac_present;
          rrc_ccm_cell_camping_info.prev_cell_cs_reg_allowed = 
                                  rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed;
          rrc_ccm_cell_camping_info.prev_cell_ps_reg_allowed = 
                                  rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed;
          rrc_ccm_cell_camping_info.prev_cell_page_rsp_allowed = 
                                  rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed;
          /* TBD: Introduce a check on the state in which Cell Change command comes */
          if(cmd_ptr->cmd.cell_change_req.new_cell_camp_status == RRC_CELL_LOSS_NO_NEW_CELL)
          {
            /* Request is for a Cell Loss */
            /* Check the existing status */
            if(rrc_ccm_cell_camping_info.cell_camp_status == RRC_NEW_CELL_SELECTED) 
            {
              /* Existing status is RRC_NEW_CELL_SELECTED */
              rrc_ccm_cell_camping_info.req_proc_id = 
                                        cmd_ptr->cmd.cell_change_req.req_proc_id;
              rrc_ccm_cell_camping_info.cell_camp_status = 
                                        cmd_ptr->cmd.cell_change_req.new_cell_camp_status;
  
              /* TBD: Do we need to indicate to LLC about loss of service - so as to 
               * bring down channels ? Thought for later.
               */     
  
              /* Let us send a New Cell Ind so that those procedures who want to be
               * informed of a loss of cell will get to know.
               */
              rrc_ccm_send_new_cell_ind(RRC_CELL_LOSS_NO_NEW_CELL, 
                                        rrc_ccm_cell_camping_info.req_proc_id,
                                        FALSE, FALSE);
  
            } /* Existing status is RRC_NEW_CELL_SELECTED */
  
            else /* Existing Status is RRC_CELL_LOSS_NO_NEW_CELL */
            {
              /* This is a request indicating loss of cell. Simply update cell 
               * camping info.
               */
              rrc_ccm_cell_camping_info.req_proc_id = 
                                        cmd_ptr->cmd.cell_change_req.req_proc_id;
              rrc_ccm_cell_camping_info.cell_camp_status = 
                                        cmd_ptr->cmd.cell_change_req.new_cell_camp_status;
  
              /* No need to inform LLC in this case since we were not camped 
               * before also.
               */
  
            } /* Existing Status is RRC_CELL_LOSS_NO_NEW_CELL */
  
          } /* Requested Cell Status == RRC_CELL_LOSS_NO_NEW_CELL */
  
          else if(cmd_ptr->cmd.cell_change_req.new_cell_camp_status == RRC_NEW_CELL_SELECTED)
          {
            csp_sstate = rrc_get_csp_substate();

            /* TBD: Set the camping status to Not Camped so as to deter activity by
             * any procedure that cares about the Cell camping status. This status
             * will remain as Not Camped till all things that are needed for cell
             * change are accomplished successfully.
             */
            rrc_ccm_cell_camping_info.req_proc_id = 
                                        cmd_ptr->cmd.cell_change_req.req_proc_id;
            rrc_ccm_cell_camping_info.cell_camp_status = 
                                        cmd_ptr->cmd.cell_change_req.new_cell_camp_status;

            rrc_ccm_cell_camping_info.cell_change_type = 
                                        cmd_ptr->cmd.cell_change_req.cell_change_type;

            /* Initialize the Cell Camping Info with the cmd data */
            RRC_CCM_COPY_CURR_CELL_CAMPING_INFO(cmd_ptr);       
#ifdef FEATURE_FEMTO_CSG
            rrc_ccm_cell_camping_info.curr_cell.csg_id =  cmd_ptr->cmd.cell_change_req.csg_id;  
#endif    

            rrc_ccm_cell_camping_info.curr_cell.is_valid_nw_sharing = cmd_ptr->cmd.cell_change_req.is_valid_nw_sharing;
            rrc_ccm_cell_camping_info.curr_cell.shared_list_of_plmns.common_plmn_id = cmd_ptr->cmd.cell_change_req.common_plmn_id;
            if(rrc_ccm_cell_camping_info.curr_cell.is_valid_nw_sharing)
            {
              WRRC_MSG2_HIGH("NWS: Valid NWS common PLMN MCC:%d MNC:%d",
                  RRC_CSP_GET_INT_MCC(rrc_ccm_cell_camping_info.curr_cell.shared_list_of_plmns.common_plmn_id),
                  RRC_CSP_GET_INT_MNC(rrc_ccm_cell_camping_info.curr_cell.shared_list_of_plmns.common_plmn_id));
            }
            if(rrc_ccm_cell_camping_info.prev_cell.is_valid_nw_sharing)
            {
              prev_plmnid = rrc_ccm_cell_camping_info.prev_cell.shared_list_of_plmns.common_plmn_id;
            }
            else
            {
              prev_plmnid = rrc_ccm_cell_camping_info.prev_cell.plmn_id;
            }
          
            if(rrc_ccm_cell_camping_info.curr_cell.is_valid_nw_sharing)
            {
              curr_plmnid = rrc_ccm_cell_camping_info.curr_cell.shared_list_of_plmns.common_plmn_id;
            }
            else
            {
              curr_plmnid = rrc_ccm_cell_camping_info.curr_cell.plmn_id;
            }

            if( (rrc_csp_int_data.curr_scan == RRC_CSP_CELL_RESELECTION_SCAN) && 
                (rrc_bcch_plmns_match(curr_plmnid, prev_plmnid) == TRUE) &&
                (rrc_ccm_cell_camping_info.curr_cell.cell_id == rrc_ccm_cell_camping_info.prev_cell.cell_id) )
            {
              /* Cell Reselection to the same cell - Fail it */
              WRRC_MSG0_ERROR("Cell Reselection to the same cell- failing reselection");

              rrcsib_print_cellid_psc_plmnid_from_sibdb();

              rrc_ccm_handle_cell_change_failure(rrc_ccm_cell_camping_info.req_proc_id,
                                                 &rrc_ccm_cell_camping_info.curr_cell,
                                                 RRC_CELL_CHANGE_FAILURE_OTHER);
              break;
            }

            if(cmd_ptr->cmd.cell_change_req.cell_change_type == RRC_CHANGE_CELL_RESELECTION)
            {
              /* This check is done to make sure that CCM and CSP substates are in Sync 
               * during reselection
               */
              if(   (csp_sstate == RRCCSP_SUIT_CELL_RESELECT_WT_CAMP)
                 || (csp_sstate == RRCCSP_ACCEPT_CELL_RESELECT_WT_CAMP)
                 || (csp_sstate == RRCCSP_SUIT_CELL_RESELECT_WT_CAMP_PENDING)
                 || (csp_sstate == RRCCSP_ACCEPT_CELL_RESELECT_WT_CAMP_PENDING))
              {
                rrc_ccm_cell_camping_info.status = STATUS_INVALID_CELL_RESELECTION;
                         
               /*Delete the old cells sib based TVM before MAC is configured with new cells 
                         configuration. This is needed because MAC can incorrectly trigger TVM based on 
                         old sib configuration as soon as RACH is setup*/
                if((rrc_state == RRC_STATE_CONNECTING) || (rrc_state == RRC_STATE_CELL_FACH))
                {
                  rrctvm_delete_sib_tvm_config();
                }
                if((cell_reselection_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
                {
                  /* Fill the RRC_CELL_RESELECTION_REQ to be sent to LLC asking it to
                  configure the lower layers */
                  cell_reselection_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_RESELECTION_REQ;
                  cell_reselection_cmd_ptr->cmd.cell_resel_req.freq = cmd_ptr->cmd.cell_change_req.freq;
                  cell_reselection_cmd_ptr->cmd.cell_resel_req.scr_code = cmd_ptr->cmd.cell_change_req.scr_code;
                  if(cmd_ptr->cmd.cell_change_req.is_valid_nw_sharing)
                  {
                    cell_reselection_cmd_ptr->cmd.cell_resel_req.plmn_id = cmd_ptr->cmd.cell_change_req.common_plmn_id;
                  }
                  else
                  {
                    cell_reselection_cmd_ptr->cmd.cell_resel_req.plmn_id = cmd_ptr->cmd.cell_change_req.plmn_id;
                  }
                  cell_reselection_cmd_ptr->cmd.cell_resel_req.cell_id = cmd_ptr->cmd.cell_change_req.cell_id;
                }
  
                /* Send the Internal RRC Command - LLC should directly handle it */
                rrc_put_int_cmd( cell_reselection_cmd_ptr );
  
                /* Wait for RRC_CELL_RESELECTION_CNF from LLC */
                next_substate = RRC_CCM_WAIT_CELL_RESELECTION_CNF; 
              }
              else
              {
                WRRC_MSG1_ERROR("Ignoring RRC_CELL_CHANGE_REQ (for resel) in CCM when CSP st %d", csp_sstate); 
                rrc_ccm_update_with_cell_loss();
              }
            }
            else
            {
              rrc_ccm_cell_camping_info.status = STATUS_INVALID_CELL_TO_BE_CAMPED;

              /* Indicate the cell selection status to SIB procedure */
                if(FAILURE == rrcsib_change_serving_cell_status(cell_selection_status, existing_serving_cell_valid, 
                  curr_plmnid,rrc_ccm_cell_camping_info.curr_cell.cell_id))
                {
                  WRRC_MSG0_ERROR("Could not find PLMN/CellId");
                }

#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
              /* Post CELL CHANGE SUCCESS if CSFB call is in prorgress and SIB container is present */
              if(
                  (WCDMA_RRC_IDLE_FEATURES(RRC_CSFB_CALL_TIMELINE_OPT)) && 
                  rrc_csfb_call_status && (rrc_state == RRC_STATE_DISCONNECTED)
                )
              {
                event_payload.status = RRC_PCH_SETUP_SKIPPED;
                event_report_payload(EVENT_WCDMA_CSFB_CALL_OPT, sizeof(event_payload),(void *)&event_payload);

                rrc_ccm_handle_cell_change_success();

                /* Setting meas substate to idle since it is set to initial now */
                /* Since CELL_SELECTED_MEAS_REQ won't be sent */
                rrcmeas_current_substate = RRCMEAS_IDLE;

                if ( rrcmeas_read_sib11() == FAILURE )
                {
                  WRRC_MSG0_ERROR("SIB11 read failed");
                }

                /* Set the CCM substate to Idle so as to accept the next Cell Change Req.*/
                next_substate = RRC_CCM_IDLE;

#if (!(defined(T_WINNT)))
                /* Log the state change with QXDM */
                rrc_ccm_log_cell_info();
#endif /* (!(defined(T_WINNT))) */
              }  
              else
#endif
              {
                /* Send the ASET Update Command to L1 */
#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
                /* Skip sending Cell_transition req if we are in connecting state */
                if(
                    (WCDMA_RRC_IDLE_FEATURES(RRC_CSFB_CALL_TIMELINE_OPT)) && 
                    rrc_csfb_call_status && (rrc_state == RRC_STATE_CONNECTING)
                  )
                {
                   MSG_HIGH("CSFB_OPT:Skip sending CPHY_CELL_TRANSITION_REQ",0,0,0);
                }
                else
#endif
                {
                  rrc_csp_send_l1_cmd(RRC_PROCEDURE_CCM, CPHY_CELL_TRANSITION_REQ, 
                                  &rrc_ccm_cell_camping_info.curr_cell);

                  /* Send LLC a command indicating the new state and a RB reconfig if that
                   * applies and wait for LLC to respond back with a CNF.
                   */
                  /* Delete the old cells sib based TVM before MAC is configured with new cells 
                     configuration. This is needed because MAC can incorrectly trigger TVM based on 
                     old sib configuration as soon as RACH is setup*/

                  if((rrc_state == RRC_STATE_CONNECTING) || (rrc_state == RRC_STATE_CELL_FACH))
                  {
                    rrctvm_delete_sib_tvm_config();
                  }
                }
                rrc_ccm_send_chan_config_cmd(rrc_state,rrc_ccm_cell_camping_info.cell_change_type);
                /* Channel Config Command sent successfully */
                next_substate = RRC_CCM_WAIT_CHAN_CONFIG_CNF;
              }
            }   
          } /* Requested Cell Status == RRC_NEW_CELL_SELECTED */
          else /* Unexpected cell status */
          {
            WRRC_MSG1_ERROR("Invalid cell status %d in Cell Change Req - Ignored",
                                 cmd_ptr->cmd.cell_change_req.new_cell_camp_status);
            /* Do not send a CNF back - Ignore this Cell Change Req. */
          }
        }
      }/* Request came from valid Procedure ID */

        break;

    case RRC_STATE_CHANGE_IND:
      WRRC_MSG0_HIGH("State change in CCM_IDLE, ignore it");
      break;

    default:
      WRRC_MSG2_HIGH("Unexpected Cmd 0x%x in RRC CCM substate %d", 
                              cmd_ptr->cmd_hdr.cmd_id, rrc_ccm_substate);
      break;
  }

  return(next_substate);

} /* rrc_ccm_idle_handler */

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
void rrc_ccm_update_cell_camped_status(rrc_new_cell_status_e_type status)
{
  if(rrc_ccm_substate == RRC_CCM_IDLE)
  {  
    if(status == RRC_NEW_CELL_SELECTED)
    {
      if(rrc_ccm_cell_camping_info.cell_camp_status == RRC_CELL_LOSS_NO_NEW_CELL)
      {
        rrc_ccm_cell_camping_info.req_proc_id = RRC_PROCEDURE_CSP;
        rrc_ccm_cell_camping_info.cell_camp_status = status;

        /* No need to inform LLC in this case */
        if(rrc_ccm_cell_camping_info.rce_found_status_not_camping)
        {
          /* Send an Indication ONLY when either the new cell is different
           * than the last camped on cell OR if RCE wants an Indication.
           * In this case, the cell remains the same, so the check is only
           * on RCE wanting an Indication.
           */
          /* We don't want the procedure initiating this change to get an IND 
           * It will get a CNF and an extra IND might be confusing.
           */
          WRRC_MSG0_HIGH("Send New Cell Ind to RCE");
          rrc_ccm_send_new_cell_ind(RRC_NEW_CELL_SELECTED, 
                                    RRC_PROCEDURE_CSP,
                                    TRUE,
                                    rrc_ccm_cell_camping_info.rce_found_status_not_camping);

          rrc_ccm_cell_camping_info.rce_found_status_not_camping = FALSE;

        }  /* If inform_rce_procedure */
      } /* If camping status is "Cell loss Idle resel in prog" */
      else
      {
        WRRC_MSG1_HIGH("NEW_CELL_SELECTED, curr camp %d ", rrc_ccm_cell_camping_info.cell_camp_status);
      }
    } /* New Cell Selected */
    else
    {
      WRRC_MSG1_ERROR("ERR Status type %d not recognized", status);
    }
  } /* CCM is in Idle substate */
  else
  {
    WRRC_MSG1_ERROR("ERR update cell status func called when CCM is not in idle %d", rrc_ccm_substate);
  }
} /* rrc_ccm_update_cell_camped_status */


/*===========================================================================
FUNCTION          rrc_ccm_update_for_gtow_handover

DESCRIPTION       This function will update the CCM camping information for
                  GSM to WCDMA handover.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
void
rrc_ccm_update_for_gtow_handover(rrc_csp_acq_entry_type * acq_entry_ptr)
{
  rrc_ccm_cell_camping_info.status = STATUS_VALID;
  rrc_ccm_cell_camping_info.cell_camp_status = RRC_NEW_CELL_SELECTED;
  rrc_ccm_cell_camping_info.cell_change_type = RRC_CHANGE_CELL_SELECTION;
  rrc_ccm_cell_camping_info.curr_cell.scr_code = acq_entry_ptr->scr_code;
  rrc_ccm_cell_camping_info.curr_cell.freq = acq_entry_ptr->freq;
  rrc_ccm_cell_camping_info.curr_cell.plmn_id = acq_entry_ptr->plmn_id;
  rrc_ccm_cell_camping_info.is_curr_cell_id_valid = FALSE;
  rrc_ccm_cell_camping_info.curr_cell.cell_id = WCDMA_INVALID_CELL_ID;
#ifdef FEATURE_FEMTO_CSG
  rrc_ccm_cell_camping_info.curr_cell.csg_id = SYS_CSG_ID_INVALID;
#endif
  rrc_ccm_cell_camping_info.rce_found_status_not_camping = FALSE;
  rrc_ccm_cell_camping_info.curr_cell_access = MM_AS_ACCESS_ALL_CALLS;
  rrc_ccm_cell_camping_info.curr_cs_cell_access = MM_AS_ACCESS_ALL_CALLS;
  rrc_ccm_cell_camping_info.curr_ps_cell_access = MM_AS_ACCESS_ALL_CALLS;
  rrc_ccm_cell_camping_info.curr_dsac_present = TRUE;
  rrc_ccm_substate = RRC_CCM_IDLE;

} /* rrc_ccm_update_for_gtow_handover */




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
rrc_ccm_update_for_inter_freq_hard_handover(rrc_freq_type freq, rrc_scr_code_type pri_scr_code)
{
  rrc_scr_code_type scr_code;

  /* Since primary scrambling code is passed into this function,
   * multiply it by 16 here
   */
  scr_code = (uint16)(pri_scr_code * 16);

  rrc_ccm_cell_camping_info.curr_cell.scr_code = scr_code;
  rrc_ccm_cell_camping_info.curr_cell.freq = freq;

#ifdef FEATURE_FEMTO_CSG
  rrc_ccm_cell_camping_info.curr_cell.csg_id = SYS_CSG_ID_INVALID;
  rrc_csp_int_data.curr_acq_entry.csg_id = SYS_CSG_ID_INVALID;
#endif

  rrc_csp_int_data.curr_acq_entry.freq     = freq;
  rrc_csp_int_data.curr_acq_entry.scr_code = scr_code;
  rrc_csp_int_data.curr_acq_entry.status_mask = 0;

#ifdef FEATURE_WCDMA_MULTIBAND
  rrc_csp_int_data.curr_acq_entry.band_class = rrc_get_frequency_band
                               (freq, rrc_csp_int_data.curr_acq_entry.plmn_id);
#else
   rrc_csp_int_data.curr_acq_entry.band_class = RRC_CSP_PCS_BAND;
#endif

   WRRC_MSG2_HIGH("Setting band_class %d for freq %d",
              rrc_csp_int_data.curr_acq_entry.band_class,
              freq);

  /* Store the Current Acq. entry in Acq. database. */
  (void)rrc_csp_add_acq_entry(&rrc_csp_int_data.curr_acq_entry);

} /* rrc_ccm_update_for_inter_freq_hard_handover */



/*===========================================================================

FUNCTION          RRC_CELL_CHANGE_MANAGER

DESCRIPTION       This function is responsible for handling all commands 
                  relevant to Cell Change Manager.

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

===========================================================================*/
void  rrc_cell_change_manager( rrc_cmd_type  *cmd_ptr )
{
  
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_CCM,rrc_ccm_substate,cmd_ptr->cmd_hdr.cmd_id);

#ifdef FEATURE_DUAL_SIM
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_STATE_CHANGE_IND)
  {
    if(((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_DCH) ||
        (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_FACH)) &&
       (cmd_ptr->cmd.state_change_ind.previous_state == RRC_STATE_CONNECTING)
      )
    {
      if((rrc_rce_get_est_cn_domain_id() != RRC_CS_DOMAIN_CN_ID)
#ifdef FEATURE_3GPP_CSFB
      && (!rrc_csfb_call_status)
#endif
        )
      {
        /* Do not change the Lock priority if this is a CSFB call or if the est req is for CS domain*/
        rrc_check_send_change_priority();
        /*This is needed to not allow LTA before PS RABs are established*/
        rrc_allow_lta = FALSE;
      }
    }
  }
#endif

  WRRC_MSG1_HIGH_OPT("function rrc_cell_change_manager is called in rrc_ccm_substate = rrc_ccm_substate_e_type_value%d",rrc_ccm_substate);

  if(cmd_ptr->cmd_hdr.cmd_id == RRC_MODE_CHANGE_IND)
  {
    if(cmd_ptr->cmd.mode_change_ind.to_mode == RRC_MODE_INACTIVE )
    {
      /* CCM goes back to the initial sub-state and the CCM database is cleared.
       * The status valid is set to FALSE till a Cell Change Req is processed.
       */
      rrc_ccm_cell_camping_info.status = STATUS_INVALID_CELL_TO_BE_CAMPED;
      rrc_ccm_cell_camping_info.cell_camp_status = RRC_CELL_LOSS_NO_NEW_CELL;
    
      /* Set to FALSE, the boolean that indicates that RCE found status as "Not Camping"*/
      rrc_ccm_cell_camping_info.rce_found_status_not_camping = FALSE;
      /* Set the Cell change requesting procedure ID to Invalid value */
      rrc_ccm_cell_camping_info.req_proc_id = MAX_NUM_OF_PROCEDURES;
    
      /* Initialize the state of RRC CCM here */
      rrc_ccm_substate = RRC_CCM_IDLE;
    }
    else
    {
      /* TBD - Processing for other mode changes */
      WRRC_MSG1_HIGH("Mode change to %d not supported", cmd_ptr->cmd.mode_change_ind.to_mode);
    }
  }
  else
  {
    switch(rrc_ccm_substate)
    {
      case RRC_CCM_IDLE:
        rrc_ccm_substate = rrc_ccm_idle_handler(cmd_ptr);
        break;
      case RRC_CCM_WAIT_CHAN_CONFIG_CNF:
        rrc_ccm_substate = rrc_ccm_wait_chan_config_handler(cmd_ptr);
        break;
      case RRC_CCM_WAIT_CELL_RESELECTION_CNF:
        rrc_ccm_substate = rrc_ccm_wait_cell_reselection_handler(cmd_ptr);
        break;
      default:
        WRRC_MSG1_ERROR("Invalid RRC CCM substate %d", rrc_ccm_substate);
        break;
    }
  }

} /* rrc_cell_change_manager */


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
void rrcccm_force_init( void )
{
  /* The status valid is set to FALSE till a Cell Change Req is processed.
   */
  rrc_ccm_cell_camping_info.status = STATUS_INVALID_CELL_TO_BE_CAMPED;
  rrc_ccm_cell_camping_info.cell_camp_status = RRC_CELL_LOSS_NO_NEW_CELL;

  /* Set to FALSE, the boolean that indicates that RCE found status as "Not Camping"*/
  rrc_ccm_cell_camping_info.rce_found_status_not_camping = FALSE;
  /* Set the Cell change requesting procedure ID to Invalid value */
  rrc_ccm_cell_camping_info.req_proc_id = MAX_NUM_OF_PROCEDURES;

  /* Initialize the state of RRC CCM here */
  rrc_ccm_substate = RRC_CCM_IDLE;

} /* rrcccm_force_init */

/*===========================================================================

FUNCTION          rrccm_update_service_info

DESCRIPTION       This function is responsible for updatintng service access
                            information OTA message sent to MM 

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None.

NOTE
                  This function SHALL NOT send any cmd to any other procedure.
===========================================================================*/
void rrc_ccm_update_service_info(mm_cmd_type *service_info_ota_ptr)
{
  rrc_plmn_identity_type plmn_id;
  rrc_int_cell_id_type cell_id;

  /* Fill in the service status */
  if ((rrc_ccm_get_serv_plmn_cellid(&cell_id, &plmn_id) == RRC_CELL_LOSS_NO_NEW_CELL))
  {
    service_info_ota_ptr->cmd.rrc_service_ind.service_status = MM_AS_NO_SERVICE;
  }
  else
  {
    service_info_ota_ptr->cmd.rrc_service_ind.service_status = MM_AS_SERVICE_AVAILABLE;
  }

#ifdef FEATURE_FEMTO_CSG
  rrc_ccm_cell_camping_info.curr_cell.csg_id = service_info_ota_ptr->cmd.rrc_service_ind.service_info.csg_info.csg_id;
#endif

  if(service_info_ota_ptr->cmd.rrc_service_ind.service_status== MM_AS_SERVICE_AVAILABLE)
  {
    /* Fill the cell access */
    service_info_ota_ptr->cmd.rrc_service_ind.service_info.cell_access =
                                                            rrc_csp_int_data.curr_cell_access_status;
    WRRC_MSG2_HIGH("Curr Cell Access Info= %d  Prev Cell Access Info= %d", service_info_ota_ptr->cmd.rrc_service_ind.service_info.cell_access,
                                                         rrc_ccm_cell_camping_info.prev_cell_access);
    if(rrc_csp_int_data.is_dsac_valid == TRUE)
    {
      MSG_MED("DSAC: DSAC  is present",0,0,0);
      service_info_ota_ptr->cmd.rrc_service_ind.service_info.dsac_present  =  
                                                rrc_csp_int_data.is_dsac_valid;
      service_info_ota_ptr->cmd.rrc_service_ind.service_info.cs_cell_access = 
                                                rrc_csp_int_data.curr_cs_cell_access;
      service_info_ota_ptr->cmd.rrc_service_ind.service_info.ps_cell_access = 
                                                 rrc_csp_int_data.curr_ps_cell_access;
      MSG_HIGH("DSAC: DSAC, CS and PS cell access values:  %d,  %d, %d", 
                          service_info_ota_ptr->cmd.rrc_service_ind.service_info.dsac_present,
                          service_info_ota_ptr->cmd.rrc_service_ind.service_info.cs_cell_access,
                          service_info_ota_ptr->cmd.rrc_service_ind.service_info.ps_cell_access);
    }
    else
    {
       MSG_MED("DSAC: DSAC info is not present",0,0,0);
       service_info_ota_ptr->cmd.rrc_service_ind.service_info.dsac_present = rrc_csp_int_data.is_dsac_valid;
       service_info_ota_ptr->cmd.rrc_service_ind.service_info.cell_access = rrc_csp_int_data.curr_cell_access_status;
    }
    if(rrc_csp_int_data.is_dsac_valid == TRUE) 
    {
      MSG_MED("DSAC: DSAC info is valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
      rrc_ccm_cell_camping_info.curr_cs_cell_access =   
                                  service_info_ota_ptr->cmd.rrc_service_ind.service_info.cs_cell_access;
      rrc_ccm_cell_camping_info.curr_ps_cell_access =   
                                 service_info_ota_ptr->cmd.rrc_service_ind.service_info.ps_cell_access;
      MSG_HIGH("DSAC: DSAC, CS and PS A/C values in ccm cell camp info %d,  %d, %d", 
                               rrc_ccm_cell_camping_info.curr_dsac_present,
                               rrc_ccm_cell_camping_info.curr_cs_cell_access,
                               rrc_ccm_cell_camping_info.curr_ps_cell_access);
    }
    else
    {
      MSG_MED("DSAC: DSAC info is not valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_dsac_present = rrc_csp_int_data.is_dsac_valid;
    }

    service_info_ota_ptr->cmd.rrc_service_ind.service_info.ppac_present = rrc_csp_int_data.is_ppac_valid;
    if(rrc_csp_int_data.is_ppac_valid == TRUE)
    {
      MSG_MED("PPAC: PPAC  is present",0,0,0);
      service_info_ota_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed = 
                                                rrc_csp_int_data.curr_cell_cs_reg_allowed;
      service_info_ota_ptr->cmd.rrc_service_ind.service_info.ps_cell_access = 
                                                rrc_csp_int_data.curr_cell_ps_reg_allowed;
      service_info_ota_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed = 
                                                rrc_csp_int_data.curr_cell_page_rsp_allowed;
      MSG_HIGH("PPAC: CS reg, PS reg and paging status values are %d, %d, %d", 
                          service_info_ota_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed,
                          service_info_ota_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed,
                          service_info_ota_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed);
    }

    if(rrc_csp_int_data.is_ppac_valid == TRUE) 
    {
      MSG_MED("PPAC: PPAC info is valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_ppac_present = rrc_csp_int_data.is_ppac_valid;
      rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed = 
                                  service_info_ota_ptr->cmd.rrc_service_ind.service_info.cs_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed = 
                                  service_info_ota_ptr->cmd.rrc_service_ind.service_info.ps_reg_allowed;
      rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed = 
                                  service_info_ota_ptr->cmd.rrc_service_ind.service_info.page_rsp_allowed;
      MSG_HIGH("PPAC: CS reg, PS reg and paging status values in ccm cell camp info %d,	%d, %d", 
                               rrc_ccm_cell_camping_info.curr_cell_cs_reg_allowed,
                               rrc_ccm_cell_camping_info.curr_cell_ps_reg_allowed,
                               rrc_ccm_cell_camping_info.curr_cell_page_rsp_allowed);
    }
    else
    {
      MSG_MED("PPAC: PPAC info is not valid for CCM",0,0,0);
      rrc_ccm_cell_camping_info.curr_ppac_present = rrc_csp_int_data.is_ppac_valid;
    }
  }
}
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
void rrc_init_cell_change_manager( void )
{
  /* The status valid is set to FALSE till a Cell Change Req is processed.
   */
  rrc_ccm_cell_camping_info.status = STATUS_INVALID_CELL_TO_BE_CAMPED;
  rrc_ccm_cell_camping_info.cell_camp_status = RRC_CELL_LOSS_NO_NEW_CELL;

  /* Set to FALSE, the boolean that indicates that RCE found status as "Not Camping"*/
  rrc_ccm_cell_camping_info.rce_found_status_not_camping = FALSE;
  /* Set the Cell change requesting procedure ID to Invalid value */
  rrc_ccm_cell_camping_info.req_proc_id = MAX_NUM_OF_PROCEDURES;

  /* Initialize the state of RRC CCM here */
  rrc_ccm_substate = RRC_CCM_IDLE;

  rrc_ccm_cell_camping_info.is_curr_cell_id_valid = FALSE;

  /* Register with the State Change Manager for all state transitions 
   * from any state to Idle Disconnected and DCH states.
   */
  rrcscmgr_register_for_scn
  (
    RRC_PROCEDURE_CCM,   
    RRC_STATE_WILDCARD,  
    RRC_STATE_DISCONNECTED
  );

  rrcscmgr_register_for_scn
  (
    RRC_PROCEDURE_CCM,   
    RRC_STATE_WILDCARD,  
    RRC_STATE_CELL_DCH
  );

  rrcscmgr_register_for_scn
  (
    RRC_PROCEDURE_CCM,
    RRC_STATE_CONNECTING,
    RRC_STATE_CELL_FACH
  );
} /* rrc_init_cell_change_manager */

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
)
{
  rrc_cmd_type   *cmd_ptr;
  /* Send New Cell Ind */
  if((cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
  {
    cmd_ptr->cmd_hdr.cmd_id = RRC_NEW_CELL_IND;
    cmd_ptr->cmd.new_cell_ind.new_cell_status = RRC_NEW_CELL_SELECTED;
    cmd_ptr->cmd.new_cell_ind.no_ind_proc_id = proc_id;
    cmd_ptr->cmd.new_cell_ind.cell_same_as_before = cell_same_as_before;
    cmd_ptr->cmd.new_cell_ind.inform_procedure_rce = inform_procedure_rce;

    /* Send the Internal cmd to RRC */
    MSG_HIGH("Post NEW_CELL_IND %d, %d, %d", RRC_NEW_CELL_IND,
                                            cell_same_as_before, 
                                            inform_procedure_rce
                                            );

    switch (proc_id)
    {
      case RRC_PROCEDURE_RBE:
        rrcrbe_procedure_event_handler( cmd_ptr );
        break;

      case RRC_PROCEDURE_RBRC:
        rrcrbrc_procedure_event_handler( cmd_ptr );
        break;

      case RRC_PROCEDURE_TCR:
        rrctcrc_procedure_event_handler( cmd_ptr );
        break;

      case RRC_PROCEDURE_PCR:
        rrcpcrc_procedure_event_handler( cmd_ptr );
        break;

      case RRC_PROCEDURE_RBR:
        rrcrbr_procedure_event_handler( cmd_ptr );
        break;

      case RRC_PROCEDURE_CU:
        rrccu_procedure_event_handler( cmd_ptr );
        break;  

      default:
        WRRC_MSG1_ERROR("This proc is not expected : rrc_proc_e_type_value%d",
                                              proc_id);
        break;
    }
  }
  rrc_free_cmd_buf( cmd_ptr );
}

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
)
{
  if(rrc_ccm_cell_camping_info.curr_cell.is_valid_nw_sharing)
  {
    *plmn_id = rrc_ccm_cell_camping_info.curr_cell.shared_list_of_plmns.common_plmn_id;
  }
  return rrc_ccm_cell_camping_info.curr_cell.is_valid_nw_sharing;
}
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
)
{
 
  if(rrc_ccm_cell_camping_info.prev_cell.is_valid_nw_sharing)
  {
    *prev_plmn_id = rrc_ccm_cell_camping_info.prev_cell.shared_list_of_plmns.common_plmn_id;
  }
  else
  {
    *prev_plmn_id = rrc_ccm_cell_camping_info.prev_cell.plmn_id;
  }
  *cell_id = rrc_ccm_cell_camping_info.prev_cell.cell_id;
}

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
rrc_new_cell_status_e_type rrcccm_get_curr_cell_camped_status(void)
{
  return rrc_ccm_cell_camping_info.cell_camp_status;
}
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
rrc_ccm_cell_camping_status_valid_e_type rrcccm_get_camping_status(void)
{
  return rrc_ccm_cell_camping_info.status;
}
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
rrc_ccm_substate_e_type rrcccm_return_substate(void)
{
  return rrc_ccm_substate;
}
